use super::*;

/// C++: `cppjieba::PreFilter` — maximal non-separator runs.
///
/// The reference skips leading separators and stops a range *before* the next
/// separator, so punctuation never reaches the segmenter.
pub(crate) fn jieba_ranges(chars: &[char]) -> Vec<std::ops::Range<usize>> {
    let mut ranges = Vec::new();
    let mut cursor = 0usize;
    while cursor < chars.len() {
        while cursor < chars.len() && is_jieba_separator(chars[cursor]) {
            cursor += 1;
        }
        if cursor == chars.len() {
            break;
        }
        let begin = cursor;
        while cursor < chars.len() && !is_jieba_separator(chars[cursor]) {
            cursor += 1;
        }
        ranges.push(begin..cursor);
    }
    ranges
}

/// C++: `cppjieba::DictTrie` — the word weights and user-dictionary markers.
pub(crate) struct JiebaDict {
    pub(crate) words: HashMap<String, DictUnit>,
    pub(crate) min_weight: f64,
    /// `user_dict_single_chinese_word_`: user words that are one code point.
    pub(crate) user_single: HashSet<char>,
}

impl JiebaDict {
    pub fn load(dict_path: &Path, user_dict_path: &Path) -> Result<Self, String> {
        let text = std::fs::read_to_string(dict_path)
            .map_err(|err| format!("failed to read {}: {err}", dict_path.display()))?;
        let mut entries = Vec::new();
        let mut freq_sum = 0.0f64;
        for line in text.lines() {
            // limonp `Split(line, buf, " ")` only splits on ASCII spaces; the
            // dictionary contains words made of U+00A0, which Unicode
            // whitespace splitting would discard.
            let parts: Vec<&str> = line.split(' ').filter(|part| !part.is_empty()).collect();
            if parts.len() != 3 {
                return Err(format!("split result illegal, line: {line}"));
            }
            let freq: f64 = parts[1]
                .parse()
                .map_err(|err| format!("invalid frequency in {line}: {err}"))?;
            freq_sum += freq;
            entries.push((parts[0].to_string(), freq, parts[2].to_string()));
        }
        if entries.is_empty() || freq_sum <= 0.0 {
            return Err(format!("empty jieba dictionary: {}", dict_path.display()));
        }

        let mut words = HashMap::with_capacity(entries.len());
        let mut weights = Vec::with_capacity(entries.len());
        for (word, freq, tag) in entries {
            let weight = (freq / freq_sum).ln();
            weights.push(weight);
            let len = word.chars().count();
            words.insert(word, DictUnit { weight, tag, len });
        }
        weights.sort_by(|a, b| a.total_cmp(b));
        let min_weight = weights[0];
        let median_weight = weights[weights.len() / 2];

        let user_text = std::fs::read_to_string(user_dict_path)
            .map_err(|err| format!("failed to read {}: {err}", user_dict_path.display()))?;
        let mut user_single = HashSet::new();
        for line in user_text.lines() {
            let parts: Vec<&str> = line.split(' ').filter(|part| !part.is_empty()).collect();
            if parts.is_empty() {
                continue;
            }
            let word = parts[0];
            let (weight, tag) = match parts.len() {
                1 => (median_weight, ""),
                2 => (median_weight, parts[1]),
                _ => {
                    let freq: f64 = parts[1]
                        .parse()
                        .map_err(|err| format!("invalid user frequency in {line}: {err}"))?;
                    ((freq / freq_sum).ln(), parts[2])
                }
            };
            let len = word.chars().count();
            words.insert(
                word.to_string(),
                DictUnit {
                    weight,
                    tag: tag.to_string(),
                    len,
                },
            );
            if let Some(single) = word.chars().next().filter(|_| len == 1) {
                user_single.insert(single);
            }
        }

        Ok(Self {
            words,
            min_weight,
            user_single,
        })
    }
}

/// C++: `cppjieba::Jieba` — the mix (MP + HMM) and query segmenters.
pub(crate) struct Jieba {
    pub(crate) dict: JiebaDict,
    pub(crate) hmm: HmmModel,
}

impl Jieba {
    pub fn load(resource_dir: &Path) -> Result<Self, String> {
        let dict_dir = resource_dir.join("jieba/dict");
        Ok(Self {
            dict: JiebaDict::load(
                &dict_dir.join("jieba.dict.utf8"),
                &dict_dir.join("user.dict.utf8"),
            )?,
            hmm: HmmModel::load(&dict_dir.join("hmm_model.utf8"))?,
        })
    }

    /// C++: `MPSegment::Cut` — maximum-probability segmentation.
    pub(crate) fn mp_cut_range(&self, chars: &[char]) -> Vec<String> {
        let len = chars.len();
        // `dags[i].nexts`: (end index, dictionary entry) pairs. The single
        // character fallback is present even when it is not a dictionary word,
        // in which case its entry is `None`.
        let mut nexts: Vec<Vec<(usize, Option<&DictUnit>)>> = Vec::with_capacity(len);
        for start in 0..len {
            let mut entries = Vec::new();
            let mut word = String::new();
            let stop = (start + MAX_WORD_LENGTH).min(len);
            for (offset, &c) in chars[start..stop].iter().enumerate() {
                word.push(c);
                if let Some(unit) = self.dict.words.get(&word) {
                    entries.push((start + offset, Some(unit)));
                }
            }
            if entries
                .first()
                .is_none_or(|(end, unit)| *end != start || unit.is_none())
            {
                entries.insert(0, (start, None));
            }
            nexts.push(entries);
        }

        // C++: `MPSegment::CalcDP` — right-to-left maximum score.
        let mut weight = vec![MIN_DOUBLE; len];
        let mut p_info: Vec<Option<&DictUnit>> = vec![None; len];
        for start in (0..len).rev() {
            let mut best = MIN_DOUBLE;
            let mut best_unit = None;
            for (end, unit) in &nexts[start] {
                let mut value = if end + 1 < len { weight[end + 1] } else { 0.0 };
                value += unit.map_or(self.dict.min_weight, |unit| unit.weight);
                if value > best {
                    best = value;
                    best_unit = *unit;
                }
            }
            weight[start] = best;
            p_info[start] = best_unit;
        }

        // C++: `MPSegment::CutByDag`.
        let mut words = Vec::new();
        let mut start = 0usize;
        while start < len {
            if let Some(unit) = p_info[start] {
                let end = start + unit.len;
                words.push(chars[start..end].iter().collect());
                start = end;
            } else {
                words.push(chars[start].to_string());
                start += 1;
            }
        }
        words
    }

    /// C++: `MixSegment::Cut` with `hmm = true` — MP words are kept, and runs
    /// of single-character non-user words are re-cut by the HMM.
    pub(crate) fn mix_cut_range(&self, chars: &[char]) -> Vec<String> {
        let words = self.mp_cut_range(chars);
        let mut out = Vec::new();
        let mut i = 0usize;
        while i < words.len() {
            let word = &words[i];
            let length = word.chars().count();
            let is_user_single = length == 1
                && word
                    .chars()
                    .next()
                    .is_some_and(|c| self.dict.user_single.contains(&c));
            if length > 1 || is_user_single {
                out.push(word.clone());
                i += 1;
                continue;
            }

            let mut j = i;
            while j < words.len() {
                let candidate = &words[j];
                let single = candidate.chars().next().expect("non-empty word");
                if candidate.chars().count() == 1 && !self.dict.user_single.contains(&single) {
                    j += 1;
                } else {
                    break;
                }
            }
            let run: Vec<char> = words[i..j].iter().flat_map(|word| word.chars()).collect();
            out.extend(self.hmm_cut(&run));
            i = j;
        }
        out
    }

    /// C++: `HMMSegment::Cut`.
    pub(crate) fn hmm_cut(&self, chars: &[char]) -> Vec<String> {
        let mut out = Vec::new();
        for range in jieba_ranges(chars) {
            self.hmm_cut_range(&chars[range], &mut out);
        }
        out
    }

    pub(crate) fn hmm_cut_range(&self, chars: &[char], out: &mut Vec<String>) {
        let mut left = 0usize;
        let mut right = 0usize;
        while right < chars.len() {
            if (chars[right] as u32) < 0x80 {
                if left != right {
                    self.internal_cut(&chars[left..right], out);
                }
                left = right;
                let mut end = sequential_letter_end(chars, left);
                if end == left {
                    end = numbers_end(chars, left);
                    if end == left {
                        end = left + 1;
                    }
                }
                out.push(chars[left..end].iter().collect());
                left = end;
                right = end;
            } else {
                right += 1;
            }
        }
        if left != right {
            self.internal_cut(&chars[left..right], out);
        }
    }

    /// C++: `HMMSegment::InternalCut` — cut after every E/S state.
    pub(crate) fn internal_cut(&self, chars: &[char], out: &mut Vec<String>) {
        let status = self.viterbi(chars);
        let mut left = 0usize;
        for (index, state) in status.iter().enumerate() {
            if state % 2 == 1 {
                out.push(chars[left..=index].iter().collect());
                left = index + 1;
            }
        }
    }

    /// C++: `HMMSegment::Viterbi` over the four B/E/M/S states.
    pub(crate) fn viterbi(&self, chars: &[char]) -> Vec<usize> {
        let len = chars.len();
        let mut path = vec![0usize; len * 4];
        let mut weight = vec![MIN_DOUBLE; len * 4];

        for state in 0..4 {
            weight[state * len] = self.hmm.start[state] + self.hmm.emit_prob(state, chars[0]);
            path[state * len] = usize::MAX;
        }
        for (position, &c) in chars.iter().enumerate().skip(1) {
            for state in 0..4 {
                let now = position + state * len;
                weight[now] = MIN_DOUBLE;
                path[now] = 1; // HMMModel::E, the reference's fallback
                let emit = self.hmm.emit_prob(state, c);
                for previous in 0..4 {
                    let old = position - 1 + previous * len;
                    let candidate = weight[old] + self.hmm.trans[previous][state] + emit;
                    if candidate > weight[now] {
                        weight[now] = candidate;
                        path[now] = previous;
                    }
                }
            }
        }

        let end_e = weight[len - 1 + len]; // E == 1
        let end_s = weight[len - 1 + 3 * len]; // S == 3
        let mut state = if end_e >= end_s { 1 } else { 3 };
        let mut status = vec![0usize; len];
        for position in (0..len).rev() {
            status[position] = state;
            state = path[position + state * len];
        }
        status
    }

    /// C++: `QuerySegment::Cut` — the fine-grained (search) mode.
    pub(crate) fn query_cut(&self, chars: &[char]) -> Vec<String> {
        let mut out = Vec::new();
        for range in jieba_ranges(chars) {
            out.extend(self.query_cut_range(&chars[range]));
        }
        out
    }

    pub(crate) fn query_cut_range(&self, chars: &[char]) -> Vec<String> {
        let mut out = Vec::new();
        for word in self.mix_cut_range(chars) {
            let runes: Vec<char> = word.chars().collect();
            let length = runes.len();
            if length > 2 {
                if word.bytes().all(|byte| byte.is_ascii_alphanumeric()) {
                    out.push(word);
                    continue;
                }
                for start in 0..length - 1 {
                    let bigram: String = runes[start..start + 2].iter().collect();
                    if self.dict.words.contains_key(&bigram) {
                        out.push(bigram);
                    }
                }
            } else {
                out.push(word);
            }
            if length > 3 {
                for start in 0..length - 2 {
                    let trigram: String = runes[start..start + 3].iter().collect();
                    if self.dict.words.contains_key(&trigram) {
                        out.push(trigram);
                    }
                }
            }
        }
        out
    }

    /// C++: `PosTagger::LookupTag`.
    pub(crate) fn lookup_tag(&self, word: &str) -> String {
        if let Some(unit) = self.dict.words.get(word) {
            if !unit.tag.is_empty() {
                return unit.tag.clone();
            }
        }
        let runes: Vec<char> = word.chars().collect();
        let mut digits = 0usize;
        let mut ascii = 0usize;
        let mut index = 0usize;
        while index < runes.len() && ascii < runes.len() / 2 {
            if (runes[index] as u32) < 0x80 {
                ascii += 1;
                if runes[index].is_ascii_digit() {
                    digits += 1;
                }
            }
            index += 1;
        }
        if ascii == 0 {
            "x".to_string()
        } else if digits == ascii {
            "m".to_string()
        } else {
            "eng".to_string()
        }
    }

    /// C++: `Jieba::IsAlpha` — English or numeric tokens are lower-cased by
    /// `CommonLanguageAnalyzer::AnalyzeImpl`.
    pub(crate) fn is_alpha(&self, word: &str) -> bool {
        matches!(self.lookup_tag(word).as_str(), "eng" | "m")
    }
}

/// C++: `ChineseAnalyzer` (`chinese_analyzer.cppm`) over the shipped jieba
/// dictionary. The C++ copy constructor shares the dictionary; the clone here
/// shares the same `Arc`.
#[derive(Clone)]
pub struct ChineseAnalyzer {
    pub(crate) jieba: Arc<Jieba>,
    pub(crate) stopwords: Arc<HashSet<String>>,
    pub(crate) cut_grain: CutGrain,
}

impl ChineseAnalyzer {
    /// C++: `ChineseAnalyzer::Load` — all five jieba files must exist.
    pub fn load(resource_dir: impl AsRef<Path>) -> Result<Self, String> {
        let root = resource_dir.as_ref();
        let dict_dir = root.join("jieba/dict");
        for name in [
            "jieba.dict.utf8",
            "hmm_model.utf8",
            "user.dict.utf8",
            "idf.utf8",
            "stop_words.utf8",
        ] {
            let path = dict_dir.join(name);
            if !path.is_file() {
                return Err(format!("missing analyzer file: {}", path.display()));
            }
        }
        let stopword_path = dict_dir.join("stop_words.utf8");
        let stopwords: HashSet<String> = std::fs::read_to_string(&stopword_path)
            .map_err(|err| format!("failed to read {}: {err}", stopword_path.display()))?
            .lines()
            .map(str::to_string)
            .collect();
        Ok(Self {
            jieba: Arc::new(Jieba::load(root)?),
            stopwords: Arc::new(stopwords),
            cut_grain: CutGrain::Coarse,
        })
    }

    /// C++: `ChineseAnalyzer::SetCutGrain`.
    pub fn set_cut_grain(&mut self, cut_grain: CutGrain) {
        self.cut_grain = cut_grain;
    }

    /// C++: `CommonLanguageAnalyzer::AnalyzeImpl` over `ChineseAnalyzer`'s
    /// `NextToken`.
    pub fn analyze(&self, input: &str) -> Vec<Term> {
        let chars: Vec<char> = input.chars().collect();
        let words = match self.cut_grain {
            CutGrain::Coarse => {
                let mut words = Vec::new();
                for range in jieba_ranges(&chars) {
                    words.extend(self.jieba.mix_cut_range(&chars[range]));
                }
                words
            }
            CutGrain::Fine => self.jieba.query_cut(&chars),
        };

        let mut terms = Vec::new();
        let mut offset = 0u32;
        for word in words {
            if word.is_empty() || word.len() >= MAX_TUPLE_LENGTH {
                continue;
            }
            if self.stopwords.contains(&word) {
                continue;
            }
            let text = if self.jieba.is_alpha(&word) {
                word.to_ascii_lowercase()
            } else {
                word
            };
            terms.push(Term {
                text,
                word_offset: offset,
                char_offset: 0,
                end_offset: 0,
                is_index: true,
                payload: 0,
            });
            offset += 1;
        }
        terms
    }
}
