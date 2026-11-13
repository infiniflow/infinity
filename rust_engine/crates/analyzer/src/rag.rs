use super::*;

/// C++: `DartsTrie` (`darts_trie_impl.cpp`) over a Darts-clone unit array.
///
/// The file is the raw little-endian `u32` array (`darts.h::save`): byte 0 of
/// each unit is the label, bit 8 marks a leaf, bits 10.. hold the offset.
pub struct DartsTrie {
    pub(crate) units: Vec<u32>,
}

impl DartsTrie {
    pub fn load(path: &Path) -> Result<Self, String> {
        let bytes = std::fs::read(path)
            .map_err(|err| format!("failed to read {}: {err}", path.display()))?;
        Self::from_units(bytes)
    }

    /// The same checks on a unit array that did not come from a file (the one
    /// the builder produces).
    pub(crate) fn from_units(bytes: Vec<u8>) -> Result<Self, String> {
        if bytes.len() % 4 != 0 {
            return Err(format!("invalid trie length: {}", bytes.len()));
        }
        let units: Vec<u32> = bytes
            .as_chunks::<4>()
            .0
            .iter()
            .map(|chunk| u32::from_le_bytes(*chunk))
            .collect();
        if units.len() < 256 || !units.len().is_multiple_of(256) {
            return Err(format!("invalid trie unit count: {}", units.len()));
        }
        Ok(Self { units })
    }

    pub(crate) fn label(unit: u32) -> u32 {
        unit & 0x8000_00FF
    }

    pub(crate) fn has_leaf(unit: u32) -> bool {
        (unit >> 8) & 1 == 1
    }

    pub(crate) fn offset(unit: u32) -> u32 {
        (unit >> 10) << ((unit & (1 << 9)) >> 6)
    }

    pub(crate) fn value(unit: u32) -> i32 {
        (unit & 0x7FFF_FFFF) as i32
    }

    /// C++: `DartsTrie::Get` / `DoubleArray::exactMatchSearch`.
    pub(crate) fn exact_match(&self, key: &[u8]) -> i32 {
        let mut node = 0usize;
        let mut unit = self.units[node];
        for &byte in key {
            node ^= (Self::offset(unit) ^ u32::from(byte)) as usize;
            unit = self.units[node];
            if Self::label(unit) != u32::from(byte) {
                return -1;
            }
        }
        if !Self::has_leaf(unit) {
            return -1;
        }
        let leaf = self.units[node ^ Self::offset(unit) as usize];
        Self::value(leaf)
    }

    /// C++: `DartsTrie::HasKeysWithPrefix` — the traversal only fails when a
    /// label does not match.
    pub(crate) fn has_prefix(&self, key: &[u8]) -> bool {
        let mut node = 0usize;
        let mut unit = self.units[node];
        for &byte in key {
            node ^= (Self::offset(unit) ^ u32::from(byte)) as usize;
            unit = self.units[node];
            if Self::label(unit) != u32::from(byte) {
                return false;
            }
        }
        true
    }
}

/// C++: `RAGAnalyzer` (`rag_analyzer.cppm`).
///
/// The WordNet lemmatizer is loaded for English; OpenCC traditional→simplified
/// conversion is the identity here because the shipped inputs are already
/// simplified (the reference conversion is identity for them as well).
#[derive(Clone)]
pub struct RAGAnalyzer {
    pub(crate) trie: Arc<DartsTrie>,
    /// C++: RAGAnalyzer::opencc_ - traditional to simplified, applied after
    /// StrQ2B and before the language split.
    pub(crate) opencc: Arc<OpenCcConverter>,
    pub(crate) fine_grained: bool,
    pub(crate) enable_position: bool,
    pub(crate) fold_diacritics: bool,
    pub(crate) stem_algorithm: Option<rust_stemmers::Algorithm>,
    pub(crate) lemmatizer: Option<Arc<WordNetLemmatizer>>,
    pub(crate) use_lemmatizer: bool,
    /// C++: `RAGAnalyzer::pos_table_` — the POS tags of `rag/pos-id.def`, in
    /// the index order the trie values encode.
    pub(crate) pos_table: Arc<Vec<String>>,
}

impl RAGAnalyzer {
    /// C++: `RAGAnalyzer::Load` — require the analyzer files and open the trie.
    pub fn load(resource_dir: impl AsRef<Path>) -> Result<Self, String> {
        let root = resource_dir.as_ref();
        for required in ["rag/huqie.txt", "rag/pos-id.def", "wordnet", "opencc"] {
            let path = root.join(required);
            if !path.exists() {
                return Err(format!("missing analyzer file: {}", path.display()));
            }
        }
        // C++: RAGAnalyzer::Load builds the trie from the dictionary the first
        // time and writes it next to it (rag_analyzer_impl.cpp:752-782). A
        // failed save is ignored there and ignored here: the analyzer works
        // from the in-memory array either way.
        let trie_path = root.join("rag/huqie.trie");
        let trie = if trie_path.is_file() {
            DartsTrie::load(&trie_path)?
        } else {
            let units = build_trie(root)?;
            let _ = std::fs::write(&trie_path, &units);
            DartsTrie::from_units(units)?
        };
        Ok(Self {
            trie: Arc::new(trie),
            opencc: Arc::new(OpenCcConverter::load(root)?),
            fine_grained: false,
            enable_position: false,
            fold_diacritics: false,
            stem_algorithm: Some(rust_stemmers::Algorithm::English),
            lemmatizer: WordNetLemmatizer::load(&root.join("wordnet"))
                .ok()
                .map(Arc::new),
            use_lemmatizer: true,
            pos_table: Arc::new(load_pos_table(&root.join("rag/pos-id.def"))?),
        })
    }

    /// C++: `RAGAnalyzer::SetLanguage`. Slovak and Czech have no Snowball
    /// stemmer: their diacritics are folded to ASCII and stemming is disabled.
    pub fn set_language(&mut self, language: &str) {
        let key = language.trim().to_ascii_lowercase();
        if key == "slovak" || key == "czech" {
            self.fold_diacritics = true;
            self.stem_algorithm = None;
            self.use_lemmatizer = false;
            return;
        }
        if let Some(algorithm) = snowball_algorithm(&key) {
            self.fold_diacritics = false;
            self.stem_algorithm = Some(algorithm);
            // WordNet only supports English.
            self.use_lemmatizer = key == "english";
        }
        // Unknown languages keep the current defaults (C++ logs and returns).
    }

    /// C++: `RAGAnalyzer::SetFineGrained`.
    pub fn set_fine_grained(&mut self, fine_grained: bool) {
        self.fine_grained = fine_grained;
    }

    /// C++: `RAGAnalyzer::fine_grained_`.
    pub fn fine_grained(&self) -> bool {
        self.fine_grained
    }

    /// C++: `RAGAnalyzer::enable_position_`.
    pub fn enable_position(&self) -> bool {
        self.enable_position
    }

    /// C++: `RAGAnalyzer::SetEnablePosition`.
    pub fn set_enable_position(&mut self, enable_position: bool) {
        self.enable_position = enable_position;
    }

    /// C++: `RAGAnalyzer::IsStopword`.
    pub fn is_stopword(term: &str) -> bool {
        is_stopword(term)
    }

    /// C++: `RAGAnalyzer::FoldDiacritics`.
    pub fn fold_diacritics(input: &str) -> String {
        fold_diacritics(input)
    }

    /// C++: `RAGAnalyzer::Tokenize` — returns the space-joined token stream.
    pub fn tokenize(&self, line: &str) -> String {
        let folded = self.fold_diacritics.then(|| fold_diacritics(line));
        let input = folded.as_deref().unwrap_or(line);

        // Python-style simple tokenization: re.sub(r"\W+", " ", line).
        let processed = strip_non_word(input);
        let half_width = str_q2b(&processed);
        // C++: opencc_->convert(str1, strline) - traditional to simplified,
        // before the language split.
        let strline = self.opencc.convert(&half_width);
        #[cfg(test)]
        RAG_CONVERTED.with(|c| c.set(true));

        let mut res: Vec<String> = Vec::new();
        for (segment, is_zh) in split_by_lang(&strline) {
            if !is_zh {
                // NLTK word tokenizer: punctuation was already replaced, so
                // the remaining tokens are whitespace-delimited runs (with
                // `_` split out as its own token).
                for word in nltk_word_tokenize(&segment) {
                    res.push(self.normalize_token(&word));
                }
                continue;
            }
            let length = char_len(&segment);
            if length < 2 || ends_with_pattern2(&segment) || ends_with_pattern3(&segment) {
                res.push(segment);
                continue;
            }
            res.extend(self.tokenize_inner(&segment));
        }
        self.merge(&res.join(" "))
    }

    /// C++: `RAGAnalyzer::AnalyzeImpl` + `Analyze`.
    pub fn analyze(&self, text: &str) -> Vec<Term> {
        let mut terms = Vec::new();
        if self.enable_position {
            let (mut tokens, mut positions) = self.tokenize_with_position(text);
            if self.fine_grained {
                let (fine_tokens, fine_positions) =
                    self.fine_grained_tokenize_with_position(&tokens.join(" "), &positions);
                tokens = fine_tokens;
                positions = fine_positions;
            }
            for (token, (start, end)) in tokens.into_iter().zip(positions) {
                if token.is_empty() || is_stopword(&token) {
                    continue;
                }
                terms.push(Term {
                    text: token,
                    word_offset: start,
                    char_offset: 0,
                    end_offset: end,
                    is_index: false,
                    payload: 0,
                });
            }
            return terms;
        }

        let result = self.tokenize(text);
        let tokens: Vec<String> = if self.fine_grained {
            self.fine_grained_tokenize(&result)
        } else {
            result
                .split(' ')
                .filter(|token| !token.is_empty())
                .map(str::to_string)
                .collect()
        };
        let mut offset = 0u32;
        for token in tokens {
            if token.is_empty() || is_stopword(&token) {
                continue;
            }
            terms.push(Term {
                text: token,
                word_offset: offset,
                char_offset: 0,
                end_offset: 0,
                is_index: false,
                payload: 0,
            });
            offset += 1;
        }
        terms
    }

    /// C++: `RAGAnalyzer::TokenizeWithPosition` — the coarse token stream plus
    /// each token's byte span in the original input.
    ///
    /// Positions survive diacritics folding, `\W+` replacement, `StrQ2B` and
    /// OpenCC (identity here) through the same mapping chain as the reference.
    pub fn tokenize_with_position(&self, line: &str) -> (Vec<String>, Vec<(u32, u32)>) {
        let folded;
        let mut fold_map: Option<Vec<u32>> = None;
        let input = if self.fold_diacritics {
            folded = fold_diacritics(line);
            fold_map = Some(build_position_mapping(line, &folded));
            folded.as_str()
        } else {
            line
        };

        let (processed, pcre2_map) = strip_non_word_with_position(input);
        let (str1, q2b_map) = str_q2b_with_position(&processed);
        // C++: opencc_->convert(str1, strline), then BuildPositionMapping for
        // the stage, so the offsets survive the conversion.
        let strline = self.opencc.convert(&str1);
        let opencc_map = build_position_mapping(&str1, &strline);

        // strline byte -> original input byte, through OpenCC -> StrQ2B ->
        // the \W+ replacement -> diacritics folding, the reference's chain.
        let mut final_map = Vec::with_capacity(strline.len() + 1);
        for i in 0..strline.len() {
            let str1_pos = opencc_map.get(i).copied().unwrap_or(str1.len() as u32) as usize;
            let processed_pos = q2b_map
                .get(str1_pos)
                .copied()
                .unwrap_or(processed.len() as u32) as usize;
            let folded_pos = pcre2_map
                .get(processed_pos)
                .copied()
                .unwrap_or(input.len() as u32);
            final_map.push(folded_pos);
        }
        final_map.push(input.len() as u32);
        if let Some(fold_map) = &fold_map {
            for pos in &mut final_map {
                *pos = fold_map
                    .get(*pos as usize)
                    .copied()
                    .unwrap_or(line.len() as u32);
            }
        }
        let map_pos =
            |pos: usize| -> u32 { final_map.get(pos).copied().unwrap_or(line.len() as u32) };

        let mut tokens = Vec::new();
        let mut positions = Vec::new();
        for (segment, is_zh, segment_start) in split_by_lang_spans(&strline) {
            if segment.is_empty() {
                continue;
            }
            if !is_zh {
                let mut sentence_start_pos = segment_start;
                for sentence in sentence_split(&segment) {
                    let terms = nltk_word_tokenize(&sentence);
                    let mut search_pos = 0usize;
                    for term in terms {
                        let Some(rel) = sentence[search_pos..].find(&term) else {
                            continue;
                        };
                        let pos_in_sentence = search_pos + rel;
                        let start_pos = sentence_start_pos + pos_in_sentence;
                        let end_pos = start_pos + term.len();
                        tokens.push(self.normalize_token(&term));
                        positions.push((map_pos(start_pos), map_pos(end_pos)));
                        search_pos = pos_in_sentence + term.len();
                    }
                    sentence_start_pos += sentence.len();
                }
                continue;
            }

            let length = char_len(&segment);
            if length < 2 || ends_with_pattern2(&segment) || ends_with_pattern3(&segment) {
                let start_pos = segment_start;
                let end_pos = segment_start + segment.len();
                tokens.push(segment);
                positions.push((map_pos(start_pos), map_pos(end_pos)));
                continue;
            }
            self.tokenize_inner_with_position(
                &segment,
                segment_start,
                &final_map,
                &mut tokens,
                &mut positions,
            );
        }

        merge_with_position(self, &tokens, &positions)
    }

    /// C++: `RAGAnalyzer::FineGrainedTokenize`.
    pub fn fine_grained_tokenize(&self, tokens: &str) -> Vec<String> {
        let tks: Vec<&str> = tokens
            .split(' ')
            .filter(|token| !token.is_empty())
            .collect();
        let zh_num = tks
            .iter()
            .flat_map(|token| token.chars())
            .filter(|c| is_chinese(*c))
            .count();
        let mut res: Vec<String> = Vec::new();
        if (zh_num as f64) < (tks.len() as f64) * 0.2 {
            for token in &tks {
                res.extend(token.split('/').map(str::to_string));
            }
            return res;
        }

        for token in &tks {
            let token_len = char_len(token);
            if token_len < 3 || ends_with_pattern4(token) {
                res.push((*token).to_string());
                continue;
            }
            let token_list = if token_len > 10 {
                vec![vec![((*token).to_string(), encode(-1, 0))]]
            } else {
                let mut dfs = Dfs::new(self, token, true);
                dfs.run(0, &mut Vec::new(), 0);
                dfs.token_list
            };
            if token_list.len() < 2 {
                res.push((*token).to_string());
                continue;
            }
            let mut sorted: Vec<(f64, Vec<String>)> = token_list
                .iter()
                .map(|candidate| {
                    let (tokens, score) = score_tokens(candidate);
                    (score, tokens)
                })
                .collect();
            sorted.sort_by(|a, b| b.0.partial_cmp(&a.0).unwrap_or(std::cmp::Ordering::Equal));
            let stk = &sorted[1].1;
            if stk.len() == token_len {
                res.push((*token).to_string());
            } else if token
                .chars()
                .any(|c| c.is_ascii_alphabetic() || c == '.' || c == '-')
            {
                if stk.iter().any(|part| char_len(part) < 3) {
                    res.push((*token).to_string());
                } else {
                    res.extend(stk.iter().cloned());
                }
            } else {
                res.extend(stk.iter().cloned());
            }
        }
        self.english_normalize(&res)
    }

    /// C++: `RAGAnalyzer::FineGrainedTokenizeWithPosition`.
    pub fn fine_grained_tokenize_with_position(
        &self,
        tokens_str: &str,
        positions: &[(u32, u32)],
    ) -> (Vec<String>, Vec<(u32, u32)>) {
        let tks: Vec<&str> = tokens_str
            .split(' ')
            .filter(|token| !token.is_empty())
            .collect();
        let zh_num = tks
            .iter()
            .flat_map(|token| token.chars())
            .filter(|c| is_chinese(*c))
            .count();
        let mut fine_tokens = Vec::new();
        let mut fine_positions = Vec::new();

        if (zh_num as f64) < (tks.len() as f64) * 0.2 {
            for (i, token) in tks.iter().enumerate() {
                let (start_pos, _) = positions[i];
                let mut sub_start = start_pos;
                for sub_token in token.split('/').filter(|part| !part.is_empty()) {
                    let sub_end = sub_start + sub_token.len() as u32;
                    fine_tokens.push(sub_token.to_string());
                    fine_positions.push((sub_start, sub_end));
                    sub_start = sub_end + 1;
                }
            }
            return (fine_tokens, fine_positions);
        }

        for (i, token) in tks.iter().enumerate() {
            let (start_pos, end_pos) = positions[i];
            let token_len = char_len(token);
            if token_len < 3 || ends_with_pattern4(token) {
                fine_tokens.push((*token).to_string());
                fine_positions.push((start_pos, end_pos));
                continue;
            }
            let token_list = if token_len > 10 {
                vec![vec![((*token).to_string(), encode(-1, 0))]]
            } else {
                let mut dfs = Dfs::new(self, token, true);
                dfs.run(0, &mut Vec::new(), 0);
                dfs.token_list
            };
            if token_list.len() < 2 {
                fine_tokens.push((*token).to_string());
                fine_positions.push((start_pos, end_pos));
                continue;
            }
            let mut sorted: Vec<(f64, Vec<String>)> = token_list
                .iter()
                .map(|candidate| {
                    let (tokens, score) = score_tokens(candidate);
                    (score, tokens)
                })
                .collect();
            sorted.sort_by(|a, b| b.0.partial_cmp(&a.0).unwrap_or(std::cmp::Ordering::Equal));
            let stk = &sorted[1].1;
            if stk.len() == token_len {
                fine_tokens.push((*token).to_string());
                fine_positions.push((start_pos, end_pos));
            } else if token
                .chars()
                .any(|c| c.is_ascii_alphabetic() || c == '.' || c == '-')
            {
                if stk.iter().any(|part| char_len(part) < 3) {
                    fine_tokens.push((*token).to_string());
                    fine_positions.push((start_pos, end_pos));
                } else {
                    let mut sub_pos = start_pos;
                    for part in stk {
                        let sub_end = sub_pos + char_len(part) as u32;
                        fine_tokens.push(part.clone());
                        fine_positions.push((sub_pos, sub_end));
                        sub_pos = sub_end;
                    }
                }
            } else {
                let mut sub_pos = start_pos;
                for part in stk {
                    let sub_end = sub_pos + part.len() as u32;
                    fine_tokens.push(part.clone());
                    fine_positions.push((sub_pos, sub_end));
                    sub_pos = sub_end;
                }
            }
        }
        (fine_tokens, fine_positions)
    }

    /// C++: `RAGAnalyzer::EnglishNormalize`.
    pub(crate) fn english_normalize(&self, tokens: &[String]) -> Vec<String> {
        tokens
            .iter()
            .map(|token| {
                let is_word = token
                    .chars()
                    .last()
                    .is_some_and(|c| c.is_ascii_alphabetic() || c == '_' || c == '-');
                if is_word {
                    self.normalize_token(token)
                } else {
                    token.clone()
                }
            })
            .collect()
    }

    /// The reference's per-token pipeline: lower-case, optionally lemmatize
    /// (English only), then stem with the selected Snowball algorithm.
    pub(crate) fn normalize_token(&self, token: &str) -> String {
        let lower = token.to_ascii_lowercase();
        let is_word = lower
            .chars()
            .last()
            .is_some_and(|c| c.is_ascii_alphabetic() || c == '_' || c == '-');
        if !is_word {
            return lower;
        }
        let lemma = match (&self.lemmatizer, self.use_lemmatizer) {
            (Some(lemmatizer), true) => lemmatizer.lemmatize(&lower),
            _ => lower,
        };
        self.stem_word(&lemma)
    }

    pub(crate) fn stem_word(&self, token: &str) -> String {
        match self.stem_algorithm {
            Some(algorithm) => snowball(algorithm).stem(token).into_owned(),
            None => token.to_string(),
        }
    }

    /// C++: `RAGAnalyzer::TokenizeInner` — forward/backward maximum matching
    /// plus the DFS re-scoring of the divergent middle.
    pub(crate) fn tokenize_inner(&self, line: &str) -> Vec<String> {
        let tks = self.max_forward(line);
        let tks1 = self.max_backward(line);
        let mut res: Vec<String> = Vec::new();

        let (mut i, mut j, mut same) = (0usize, 0usize, 0usize);
        while i + same < tks1.len() && j + same < tks.len() && tks1[i + same].0 == tks[j + same].0 {
            same += 1;
        }
        if same > 0 {
            res.push(join_tokens(&tks, j, j + same));
        }
        let mut i_anchor = i + same;
        let mut j_anchor = j + same;
        j = j_anchor + 1;
        i = i_anchor + 1;

        while i < tks1.len() && j < tks.len() {
            let tk1 = concat_tokens(&tks1, i_anchor, i);
            let tk = concat_tokens(&tks, j_anchor, j);
            if tk1 != tk {
                if tk1.len() > tk.len() {
                    j += 1;
                } else {
                    i += 1;
                }
                continue;
            }
            if tks1[i].0 != tks[j].0 {
                i += 1;
                j += 1;
                continue;
            }
            let mut dfs = Dfs::new(self, &concat_tokens(&tks, j_anchor, j), false);
            dfs.run(0, &mut Vec::new(), 0);
            res.push(dfs.best_tokens.join(" "));

            same = 1;
            while i + same < tks1.len()
                && j + same < tks.len()
                && tks1[i + same].0 == tks[j + same].0
            {
                same += 1;
            }
            res.push(join_tokens(&tks, j, j + same));
            i_anchor = i + same;
            j_anchor = j + same;
            j = j_anchor + 1;
            i = i_anchor + 1;
        }

        if i_anchor < tks1.len() {
            let mut dfs = Dfs::new(self, &concat_tokens(&tks, j_anchor, tks.len()), false);
            dfs.run(0, &mut Vec::new(), 0);
            res.push(dfs.best_tokens.join(" "));
        }
        res
    }

    /// C++: `RAGAnalyzer::TokenizeInnerWithPosition`.
    #[allow(clippy::too_many_arguments)]
    pub(crate) fn tokenize_inner_with_position(
        &self,
        line: &str,
        base_pos: usize,
        pos_mapping: &[u32],
        tokens: &mut Vec<String>,
        positions: &mut Vec<(u32, u32)>,
    ) {
        let tks = self.max_forward(line);
        let tks1 = self.max_backward(line);

        let slice_len = |start: usize, end: usize| -> usize {
            tks[start..end].iter().map(|(token, _)| token.len()).sum()
        };
        let mut emit = |token: &str, start: usize, end: usize| {
            emit_with_positions(tokens, positions, token, start, end, pos_mapping);
        };

        let (mut i, mut j, mut same) = (0usize, 0usize, 0usize);
        while i + same < tks1.len() && j + same < tks.len() && tks1[i + same].0 == tks[j + same].0 {
            same += 1;
        }
        if same > 0 {
            let token_str = join_tokens(&tks, j, j + same);
            let start_pos = base_pos + slice_len(0, j);
            emit(&token_str, start_pos, start_pos + token_str.len());
        }
        let mut i_anchor = i + same;
        let mut j_anchor = j + same;
        j = j_anchor + 1;
        i = i_anchor + 1;

        while i < tks1.len() && j < tks.len() {
            let tk1 = concat_tokens(&tks1, i_anchor, i);
            let tk = concat_tokens(&tks, j_anchor, j);
            if tk1 != tk {
                if tk1.len() > tk.len() {
                    j += 1;
                } else {
                    i += 1;
                }
                continue;
            }
            if tks1[i].0 != tks[j].0 {
                i += 1;
                j += 1;
                continue;
            }

            let mut dfs = Dfs::new(self, &concat_tokens(&tks, j_anchor, j), false);
            dfs.run(0, &mut Vec::new(), 0);
            let best_token_str = dfs.best_tokens.join(" ");
            let start_pos = base_pos + slice_len(0, j_anchor);
            let original_token_str = concat_tokens(&tks, j_anchor, j);
            let end_pos = start_pos + original_token_str.len();
            emit(&best_token_str, start_pos, end_pos);

            same = 1;
            while i + same < tks1.len()
                && j + same < tks.len()
                && tks1[i + same].0 == tks[j + same].0
            {
                same += 1;
            }
            let token_str = join_tokens(&tks, j, j + same);
            let start_pos = base_pos + slice_len(0, j);
            emit(&token_str, start_pos, start_pos + token_str.len());

            i_anchor = i + same;
            j_anchor = j + same;
            j = j_anchor + 1;
            i = i_anchor + 1;
        }

        if i_anchor < tks1.len() {
            let mut dfs = Dfs::new(self, &concat_tokens(&tks, j_anchor, tks.len()), false);
            dfs.run(0, &mut Vec::new(), 0);
            let best_token_str = dfs.best_tokens.join(" ");
            let start_pos = base_pos + slice_len(0, j_anchor);
            let original_token_str = concat_tokens(&tks, j_anchor, tks.len());
            let end_pos = start_pos + original_token_str.len();
            emit(&best_token_str, start_pos, end_pos);
        }
    }

    /// C++: `RAGAnalyzer::MaxForward`.
    pub(crate) fn max_forward(&self, line: &str) -> Vec<(String, i32)> {
        let chars: Vec<char> = line.chars().collect();
        let len = chars.len();
        let mut res = Vec::new();
        let mut s = 0usize;
        while s < len {
            let mut e = s + 1;
            let mut token = chars[s..e].iter().collect::<String>();
            while e < len && self.trie.has_prefix(key(&token).as_bytes()) {
                e += 1;
                token = chars[s..e].iter().collect();
            }
            while e - 1 > s && self.trie.exact_match(key(&token).as_bytes()) == -1 {
                e -= 1;
                token = chars[s..e].iter().collect();
            }
            let value = self.trie.exact_match(key(&token).as_bytes());
            res.push((token, if value != -1 { value } else { 0 }));
            s = e;
        }
        res
    }

    /// C++: `RAGAnalyzer::MaxBackward`.
    pub(crate) fn max_backward(&self, line: &str) -> Vec<(String, i32)> {
        let chars: Vec<char> = line.chars().collect();
        let mut res = Vec::new();
        let mut s = chars.len() as i64 - 1;
        while s >= 0 {
            let e = s + 1;
            let mut token = chars[s as usize..e as usize].iter().collect::<String>();
            while s > 0 && self.trie.has_prefix(rkey(&token).as_bytes()) {
                s -= 1;
                token = chars[s as usize..e as usize].iter().collect();
            }
            while s + 1 < e && self.trie.exact_match(key(&token).as_bytes()) == -1 {
                s += 1;
                token = chars[s as usize..e as usize].iter().collect();
            }
            let value = self.trie.exact_match(key(&token).as_bytes());
            res.push((token, if value != -1 { value } else { 0 }));
            s -= 1;
        }
        res.reverse();
        res
    }

    /// C++: `RAGAnalyzer::Merge` — greedily re-joins adjacent tokens when the
    /// concatenation is a dictionary word.
    pub(crate) fn merge(&self, tks_str: &str) -> String {
        let mut collapsed = String::with_capacity(tks_str.len());
        let mut prev_space = false;
        for c in tks_str.chars() {
            if c == ' ' {
                if !prev_space {
                    collapsed.push(' ');
                    prev_space = true;
                }
            } else {
                collapsed.push(c);
                prev_space = false;
            }
        }
        let tokens: Vec<&str> = collapsed.split(' ').filter(|t| !t.is_empty()).collect();
        let mut res: Vec<String> = Vec::new();
        let mut s = 0usize;
        while s < tokens.len() {
            let mut end = s + 1;
            let stop = (tokens.len() + 1).min(s + 6);
            for e in (s + 2)..stop {
                let joined = tokens[s..e].concat();
                if regex_split_partial(&joined) && self.freq(&joined) > 0 {
                    end = e;
                }
            }
            res.push(tokens[s..end].concat());
            s = end;
        }
        res.join(" ")
    }

    /// C++: `RAGAnalyzer::Freq`.
    pub fn freq(&self, key: &str) -> i32 {
        let value = self.trie.exact_match(key.as_bytes());
        let freq = decode_freq(value);
        (freq as f64).exp().mul_add(DENOMINATOR, 0.5) as i32
    }

    /// C++: `RAGAnalyzer::Tag` — the POS tag the dictionary recorded for
    /// `key` (ASCII-lowercased first, like `Key()`), or `None` when the word is
    /// not in the trie or carries no tag.
    pub fn tag(&self, key: &str) -> Option<String> {
        let lower = key.to_ascii_lowercase();
        let value = self.trie.exact_match(lower.as_bytes());
        if value == -1 {
            return None;
        }
        let pos_idx = ((value as u32) >> 24) & 0xFF;
        if pos_idx == 0 {
            return None;
        }
        self.pos_table.get(pos_idx as usize).cloned()
    }
}

/// C++: `WordNetLemmatizer` (`wordnet_lemmatizer_impl.cpp`).
///
/// Only the noun part of speech is needed: the reference calls
/// `Lemmatize(form)` with an empty POS, which selects `NOUN` to match Python
/// NLTK's default.
pub(crate) struct WordNetLemmatizer {
    pub(crate) noun_lemmas: HashSet<String>,
    pub(crate) noun_exceptions: HashMap<String, Vec<String>>,
}

impl WordNetLemmatizer {
    pub fn load(root: &Path) -> Result<Self, String> {
        let index = std::fs::read_to_string(root.join("index.noun")).map_err(|err| {
            format!(
                "failed to read {}: {err}",
                root.join("index.noun").display()
            )
        })?;
        let mut noun_lemmas = HashSet::new();
        for line in index.lines() {
            if line.is_empty() || line.starts_with(' ') {
                continue;
            }
            if let Some(lemma) = line.split_whitespace().next() {
                noun_lemmas.insert(lemma.to_string());
            }
        }

        let mut noun_exceptions = HashMap::new();
        if let Ok(text) = std::fs::read_to_string(root.join("noun.exc")) {
            for line in text.lines() {
                let mut parts = line.split_whitespace();
                let Some(inflected) = parts.next() else {
                    continue;
                };
                let bases = parts.map(str::to_string).collect();
                noun_exceptions.insert(inflected.to_string(), bases);
            }
        }
        Ok(Self {
            noun_lemmas,
            noun_exceptions,
        })
    }

    /// C++: `WordNetLemmatizer::Lemmatize` with the default noun POS.
    pub(crate) fn lemmatize(&self, form: &str) -> String {
        let mut forms = match self.noun_exceptions.get(form) {
            Some(bases) => bases.clone(),
            None => noun_substitutions(form),
        };
        forms.push(form.to_string());

        let analyses: Vec<&String> = forms
            .iter()
            .filter(|candidate| self.noun_lemmas.contains(*candidate))
            .collect();
        if analyses.is_empty() {
            return form.to_string();
        }
        // Python NLTK returns the shortest lemma, preferring the original form
        // on a tie.
        let mut shortest = analyses[0];
        for analysis in &analyses[1..] {
            if analysis.len() < shortest.len() {
                shortest = analysis;
            }
        }
        analyses
            .iter()
            .find(|analysis| analysis.as_str() == form && analysis.len() == shortest.len())
            .unwrap_or(&shortest)
            .to_string()
    }
}

/// C++: `MORPHOLOGICAL_SUBSTITUTIONS[NOUN]`.
pub(crate) fn noun_substitutions(form: &str) -> Vec<String> {
    const SUBSTITUTIONS: &[(&str, &str)] = &[
        ("s", ""),
        ("ses", "s"),
        ("ves", "f"),
        ("xes", "x"),
        ("zes", "z"),
        ("ches", "ch"),
        ("shes", "sh"),
        ("men", "man"),
        ("ies", "y"),
    ];
    let mut forms = Vec::new();
    for (old_suffix, new_suffix) in SUBSTITUTIONS {
        if let Some(stripped) = form.strip_suffix(old_suffix) {
            forms.push(format!("{stripped}{new_suffix}"));
        }
    }
    forms
}

/// C++: `MAX_DFS_DEPTH`.
pub(crate) const MAX_DFS_DEPTH: usize = 10;

/// C++: `RAGAnalyzer::Load`'s trie build (`rag_analyzer_impl.cpp:752-782`).
///
/// Reads `rag/huqie.txt` (word, raw frequency, POS tag), encodes each entry the
/// way the reference does - log-scaled frequency in the low 23 bits, POS index
/// in bits 24-31 - and adds the reversed key ("DD" + reversed word, lowered)
/// so the backward pass can probe it. The unit array it returns is what
/// `DartsTrie::load` reads, so a directory without `rag/huqie.trie` ends up
/// with exactly the file the reference would have written.
pub(crate) fn build_trie(root: &Path) -> Result<Vec<u8>, String> {
    let pos_tags = load_pos_table(&root.join("rag/pos-id.def"))?;
    let dict_path = root.join("rag/huqie.txt");
    let dict = std::fs::read_to_string(&dict_path)
        .map_err(|err| format!("failed to read {}: {err}", dict_path.display()))?;
    let mut entries: Vec<(Vec<u8>, i32)> = Vec::with_capacity(1 << 21);
    for line in dict.lines() {
        let line = line.split('\r').next().unwrap_or("");
        if line.is_empty() {
            continue;
        }
        let fields: Vec<&str> = line
            .split([' ', '\t'])
            .filter(|field| !field.is_empty())
            .collect();
        if fields.len() != 3 {
            return Err("Invalid dictionary format".to_string());
        }
        let raw: u32 = fields[1]
            .parse()
            .map_err(|_| format!("Invalid dictionary frequency: {}", fields[1]))?;
        // C++: i32(std::log(float(freq) / DENOMINATOR) + 0.5). The log is taken
        // in single precision and the result is *truncated* toward zero, not
        // rounded: log(3/1e6) + 0.5 is -12.2 and int() gives -12.
        let freq = (f64::from((raw as f32 / DENOMINATOR as f32).ln()) + 0.5) as i32;
        // An unknown tag keeps the reference's -1: POSTable::GetPOSIndex misses
        // and Encode shifts that into the top byte.
        let pos = pos_tags
            .iter()
            .position(|tag| tag == fields[2])
            .map_or(-1, |i| i as i32);
        entries.push((fields[0].as_bytes().to_vec(), encode(freq, pos)));
        entries.push((rkey(fields[0]).into_bytes(), encode(1, 0)));
    }
    Ok(crate::darts_build::build_dictionary(entries))
}

/// C++: `POSTable::Load` - the POS tags in the order their index encodes.
///
/// The reference collects the names into a `std::map` and then walks it, so
/// the table is sorted and de-duplicated rather than in file order.
pub(crate) fn load_pos_table(path: &Path) -> Result<Vec<String>, String> {
    let text = std::fs::read_to_string(path)
        .map_err(|err| format!("failed to read {}: {err}", path.display()))?;
    let tags: std::collections::BTreeSet<String> = text
        .lines()
        .map(|line| line.split('\r').next().unwrap_or("").to_string())
        .filter(|line| !line.is_empty())
        .collect();
    Ok(tags.into_iter().collect())
}

/// C++: `RAGAnalyzer::DENOMINATOR`.
pub(crate) const DENOMINATOR: f64 = 1_000_000.0;

/// C++: `RAGAnalyzer::Score`'s `B`.
pub(crate) const SCORE_B: f64 = 30.0;

/// C++: `RAGAnalyzer::DFS` — enumerates the dictionary paths through `chars`.
pub(crate) struct Dfs<'a> {
    pub(crate) analyzer: &'a RAGAnalyzer,
    pub(crate) chars: Vec<char>,
    pub(crate) memo_all: bool,
    pub(crate) token_list: Vec<Vec<(String, i32)>>,
    pub(crate) best_tokens: Vec<String>,
    pub(crate) max_score: f64,
}

impl<'a> Dfs<'a> {
    pub fn new(analyzer: &'a RAGAnalyzer, text: &str, memo_all: bool) -> Self {
        Self {
            analyzer,
            chars: text.chars().collect(),
            memo_all,
            token_list: Vec::new(),
            best_tokens: Vec::new(),
            max_score: f64::MIN,
        }
    }

    pub(crate) fn record(&mut self, pre_tokens: &[(String, i32)]) {
        if self.memo_all {
            self.token_list.push(pre_tokens.to_vec());
        } else {
            let (tokens, score) = score_tokens(pre_tokens);
            if score > self.max_score {
                self.best_tokens = tokens;
                self.max_score = score;
            }
        }
    }

    pub fn run(&mut self, s: usize, pre_tokens: &mut Vec<(String, i32)>, depth: usize) -> usize {
        let len = self.chars.len();
        let mut res = s;

        if depth > MAX_DFS_DEPTH {
            if s < len {
                let mut pretks = pre_tokens.clone();
                pretks.push((self.chars[s..].iter().collect(), encode(-12, 0)));
                self.record(&pretks);
            }
            return len;
        }
        if s >= len {
            self.record(pre_tokens);
            return res;
        }

        let mut start = s + 1;
        if s + 2 <= len {
            let t1: String = self.chars[s..s + 1].iter().collect();
            let t2: String = self.chars[s..s + 2].iter().collect();
            if self.analyzer.trie.has_prefix(key(&t1).as_bytes())
                && !self.analyzer.trie.has_prefix(key(&t2).as_bytes())
            {
                start = s + 2;
            }
        }
        if pre_tokens.len() > 2 {
            let n = pre_tokens.len();
            if char_len(&pre_tokens[n - 1].0) == 1
                && char_len(&pre_tokens[n - 2].0) == 1
                && char_len(&pre_tokens[n - 3].0) == 1
            {
                let t1 = format!("{}{}", pre_tokens[n - 1].0, self.chars[s]);
                if self.analyzer.trie.has_prefix(key(&t1).as_bytes()) {
                    start = s + 2;
                }
            }
        }

        for e in start..=len {
            let token: String = self.chars[s..e].iter().collect();
            let k = key(&token);
            if e > s + 1 && !self.analyzer.trie.has_prefix(k.as_bytes()) {
                break;
            }
            let value = self.analyzer.trie.exact_match(k.as_bytes());
            if value != -1 {
                // C++ copies the prefix list per branch, so paths cannot
                // contaminate each other.
                let mut pretks = pre_tokens.clone();
                pretks.push((token, value));
                res = res.max(self.run(e, &mut pretks, depth + 1));
            }
        }
        if res > s {
            return res;
        }

        let token: String = self.chars[s..s + 1].iter().collect();
        let value = self.analyzer.trie.exact_match(key(&token).as_bytes());
        pre_tokens.push((token, if value != -1 { value } else { encode(-12, 0) }));
        self.run(s + 1, pre_tokens, depth + 1)
    }
}

/// C++: `RAGAnalyzer::Score`.
pub(crate) fn score_tokens(token_freqs: &[(String, i32)]) -> (Vec<String>, f64) {
    let mut f = 0i64;
    let mut l = 0i64;
    let mut tokens = Vec::with_capacity(token_freqs.len());
    for (token, freq_tag) in token_freqs {
        f += i64::from(decode_freq(*freq_tag));
        l += if char_len(token) < 2 { 0 } else { 1 };
        tokens.push(token.clone());
    }
    let n = tokens.len() as f64;
    (tokens, SCORE_B / n + l as f64 / n + f as f64)
}

/// C++: `Encode`.
pub(crate) fn encode(freq: i32, idx: i32) -> i32 {
    let mut encoded: u32 = if freq < 0 {
        ((-freq) as u32) | (1 << 23)
    } else {
        (freq as u32) & 0x7F_FFFF
    };
    encoded |= (idx as u32) << 24;
    encoded as i32
}

/// C++: `DecodeFreq`.
pub(crate) fn decode_freq(value: i32) -> i32 {
    let mut v1 = (value as u32) & 0xFF_FFFF;
    if v1 & (1 << 23) != 0 {
        v1 &= 0x7F_FFFF;
        -(v1 as i32)
    } else {
        v1 as i32
    }
}

pub(crate) fn char_len(text: &str) -> usize {
    text.chars().count()
}

/// C++: `RAGAnalyzer::Key` — ASCII lowering only.
pub(crate) fn key(text: &str) -> String {
    text.to_ascii_lowercase()
}

/// C++: `RAGAnalyzer::RKey` — `"DD"` followed by the reversed code points,
/// lowered.
pub(crate) fn rkey(text: &str) -> String {
    let reversed: String = text.chars().rev().collect();
    format!("DD{}", reversed.to_ascii_lowercase())
}

/// C++: `Config::ResourcePath()` fallback table for Snowball stemmers.
pub(crate) fn snowball_algorithm(language: &str) -> Option<rust_stemmers::Algorithm> {
    use rust_stemmers::Algorithm;
    Some(match language {
        "english" => Algorithm::English,
        "dutch" => Algorithm::Dutch,
        "german" => Algorithm::German,
        "french" => Algorithm::French,
        "spanish" => Algorithm::Spanish,
        "italian" => Algorithm::Italian,
        "portuguese" | "portuguese br" => Algorithm::Portuguese,
        "russian" => Algorithm::Russian,
        "danish" => Algorithm::Danish,
        "finnish" => Algorithm::Finnish,
        "hungarian" => Algorithm::Hungarian,
        "norwegian" => Algorithm::Norwegian,
        "romanian" => Algorithm::Romanian,
        "swedish" => Algorithm::Swedish,
        "turkish" => Algorithm::Turkish,
        _ => return None,
    })
}

/// C++: `RAGAnalyzer::IsStopword` — the RAGFlow Chinese stopword set.
pub(crate) fn is_stopword(term: &str) -> bool {
    const STOPWORDS: &[&str] = &[
        "请问", "您", "你", "我", "他", "是", "的", "就", "有", "于", "及", "即", "在", "为", "最",
        "从", "以", "了", "将", "与", "吗", "吧", "中", "#", "什么", "怎么", "哪个", "哪些", "啥",
        "相关",
    ];
    STOPWORDS.contains(&term)
}

/// C++: `RAGAnalyzer::FoldDiacritics` — ASCII base letter for U+00C0–U+017F,
/// or 0 for code points without a single-ASCII-letter NFD decomposition.
pub(crate) const DIACRITICS_FOLD_TABLE: [u8; 0xC0] = [
    b'A', b'A', b'A', b'A', b'A', b'A', 0, b'C', b'E', b'E', b'E', b'E', b'I', b'I', b'I', b'I', 0,
    b'N', b'O', b'O', b'O', b'O', b'O', 0, 0, b'U', b'U', b'U', b'U', b'Y', 0, 0, b'a', b'a', b'a',
    b'a', b'a', b'a', 0, b'c', b'e', b'e', b'e', b'e', b'i', b'i', b'i', b'i', 0, b'n', b'o', b'o',
    b'o', b'o', b'o', 0, 0, b'u', b'u', b'u', b'u', b'y', 0, b'y', b'A', b'a', b'A', b'a', b'A',
    b'a', b'C', b'c', b'C', b'c', b'C', b'c', b'C', b'c', b'D', b'd', 0, 0, b'E', b'e', b'E', b'e',
    b'E', b'e', b'E', b'e', b'E', b'e', b'G', b'g', b'G', b'g', b'G', b'g', b'G', b'g', b'H', b'h',
    0, 0, b'I', b'i', b'I', b'i', b'I', b'i', b'I', b'i', b'I', 0, 0, 0, b'J', b'j', b'K', b'k', 0,
    b'L', b'l', b'L', b'l', b'L', b'l', 0, 0, 0, 0, b'N', b'n', b'N', b'n', b'N', b'n', 0, 0, 0,
    b'O', b'o', b'O', b'o', b'O', b'o', 0, 0, b'R', b'r', b'R', b'r', b'R', b'r', b'S', b's', b'S',
    b's', b'S', b's', b'S', b's', b'T', b't', b'T', b't', 0, 0, b'U', b'u', b'U', b'u', b'U', b'u',
    b'U', b'u', b'U', b'u', b'U', b'u', b'W', b'w', b'Y', b'y', b'Y', b'Z', b'z', b'Z', b'z', b'Z',
    b'z', 0,
];

pub(crate) fn fold_diacritics(input: &str) -> String {
    let mut output = String::with_capacity(input.len());
    for c in input.chars() {
        let code = c as u32;
        if (0xC0..0x180).contains(&code) {
            let folded = DIACRITICS_FOLD_TABLE[(code - 0xC0) as usize];
            if folded != 0 {
                output.push(char::from(folded));
                continue;
            }
        }
        output.push(c);
    }
    output
}

/// C++: `RAGAnalyzer::StrQ2B` — full-width ASCII and ideographic space.
pub(crate) fn str_q2b(input: &str) -> String {
    input
        .chars()
        .map(|c| match c {
            '\u{FF01}'..='\u{FF5E}' => char::from_u32(c as u32 - 0xFEE0).unwrap_or(c),
            '\u{3000}' => ' ',
            _ => c,
        })
        .collect()
}

/// C++: `PCRE2GlobalReplace(input, r"\W+", " ")` with UCP: every run of
/// non-word code points becomes one space.
pub(crate) fn strip_non_word(input: &str) -> String {
    let mut output = String::with_capacity(input.len());
    let mut in_gap = false;
    for c in input.chars() {
        if c.is_alphanumeric() || c == '_' {
            output.push(c);
            in_gap = false;
        } else if !in_gap {
            output.push(' ');
            in_gap = true;
        }
    }
    output
}

/// C++: `PCRE2GlobalReplaceWithPosition(input, r"\W+", " ")` — the processed
/// string plus, for every output byte, its byte offset in `input`. The single
/// replacement byte maps to the start of the replaced run, like the reference.
pub(crate) fn strip_non_word_with_position(input: &str) -> (String, Vec<u32>) {
    let mut output = String::with_capacity(input.len());
    let mut mapping = Vec::with_capacity(input.len());
    let mut in_gap = false;
    for (index, c) in input.char_indices() {
        if c.is_alphanumeric() || c == '_' {
            output.push(c);
            for _ in 0..c.len_utf8() {
                mapping.push(index as u32);
            }
            in_gap = false;
        } else if !in_gap {
            output.push(' ');
            mapping.push(index as u32);
            in_gap = true;
        }
    }
    (output, mapping)
}

/// C++: `RAGAnalyzer::BuildPositionMapping` — every byte of a converted
/// character maps to the first byte of the matching original character;
/// trailing entries map to the end of the original string.
pub(crate) fn build_position_mapping(original: &str, converted: &str) -> Vec<u32> {
    let mut mapping = vec![0u32; converted.len() + 1];
    let original_bytes = original.as_bytes();
    let converted_bytes = converted.as_bytes();
    let mut original_pos = 0usize;
    let mut converted_pos = 0usize;
    while original_pos < original.len() && converted_pos < converted.len() {
        let original_len = utf8_len(original_bytes[original_pos]);
        let converted_len = utf8_len(converted_bytes[converted_pos]);
        for offset in 0..converted_len {
            if converted_pos + offset < mapping.len() {
                mapping[converted_pos + offset] = original_pos as u32;
            }
        }
        original_pos += original_len;
        converted_pos += converted_len;
    }
    for entry in mapping.iter_mut().skip(converted_pos) {
        *entry = original.len() as u32;
    }
    mapping
}

pub(crate) fn utf8_len(lead: u8) -> usize {
    if lead < 0x80 {
        1
    } else if lead & 0xE0 == 0xC0 {
        2
    } else if lead & 0xF0 == 0xE0 {
        3
    } else if lead & 0xF8 == 0xF0 {
        4
    } else {
        1
    }
}

/// C++: `StrQ2B` plus the `BuildPositionMapping` table for its output.
pub(crate) fn str_q2b_with_position(input: &str) -> (String, Vec<u32>) {
    let mut output = String::with_capacity(input.len());
    let mut mapping = Vec::with_capacity(input.len() + 1);
    for (index, c) in input.char_indices() {
        let converted = match c {
            '\u{FF01}'..='\u{FF5E}' => char::from_u32(c as u32 - 0xFEE0).unwrap_or(c),
            '\u{3000}' => ' ',
            _ => c,
        };
        output.push(converted);
        for _ in 0..converted.len_utf8() {
            mapping.push(index as u32);
        }
    }
    mapping.push(input.len() as u32);
    (output, mapping)
}

/// C++: `NLTKWordTokenizer::Tokenize` after the `\W+` replacement removed all
/// punctuation: whitespace-delimited runs, with `_` split into its own token.
pub(crate) fn nltk_word_tokenize(text: &str) -> Vec<String> {
    let mut tokens = Vec::new();
    for token in text.split_whitespace() {
        if token == "_" {
            tokens.push("_".to_string());
        } else if token.contains('_') {
            for (index, part) in token.split('_').enumerate() {
                if index > 0 {
                    tokens.push("_".to_string());
                }
                if !part.is_empty() {
                    tokens.push(part.to_string());
                }
            }
            if token.ends_with('_') {
                tokens.push("_".to_string());
            }
        } else {
            tokens.push(token.to_string());
        }
    }
    tokens
}

/// C++: `SentenceSplitter` — split after a `.`/`?`/`!` plus closing quotes and
/// spaces. The tokenization pipeline has already replaced every such character
/// with a space, so this normally yields the whole segment; the port keeps the
/// reference's behavior for direct callers.
pub(crate) fn sentence_split(text: &str) -> Vec<String> {
    let bytes = text.as_bytes();
    let mut result = Vec::new();
    let mut start = 0usize;
    while start < bytes.len() {
        let mut found = None;
        for position in start..bytes.len() {
            let mut end = position;
            while end < bytes.len() && bytes[end] == b' ' {
                end += 1;
            }
            if end < bytes.len() && matches!(bytes[end], b'.' | b'?' | b'!') {
                end += 1;
                while end < bytes.len() && matches!(bytes[end], b'\'' | b'"' | b')' | b']') {
                    end += 1;
                }
                while end < bytes.len() && bytes[end] == b' ' {
                    end += 1;
                }
                found = Some((position, end));
                break;
            }
        }
        match found {
            Some((match_start, match_end)) => {
                if match_start > start {
                    result.push(text[start..match_end].to_string());
                }
                start = match_end;
            }
            None => {
                result.push(text[start..].to_string());
                break;
            }
        }
    }
    result
}

/// C++: `RAGAnalyzer::MergeWithPosition`.
pub(crate) fn merge_with_position(
    analyzer: &RAGAnalyzer,
    tokens: &[String],
    positions: &[(u32, u32)],
) -> (Vec<String>, Vec<(u32, u32)>) {
    let mut filtered_tokens = Vec::new();
    let mut filtered_positions = Vec::new();
    for (token, position) in tokens.iter().zip(positions) {
        if !token.is_empty() && token != " " {
            filtered_tokens.push(token.as_str());
            filtered_positions.push(*position);
        }
    }

    let mut merged_tokens = Vec::new();
    let mut merged_positions = Vec::new();
    let mut start = 0usize;
    while start < filtered_tokens.len() {
        let mut end = start + 1;
        let stop = (filtered_tokens.len() + 1).min(start + 6);
        for candidate_end in (start + 2)..stop {
            let joined = filtered_tokens[start..candidate_end].concat();
            if regex_split_partial(&joined) && analyzer.freq(&joined) > 0 {
                end = candidate_end;
            }
        }
        merged_tokens.push(filtered_tokens[start..end].concat());
        merged_positions.push((filtered_positions[start].0, filtered_positions[end - 1].1));
        start = end;
    }
    (merged_tokens, merged_positions)
}

/// C++: `IsChinese` — a 3-byte sequence with a 0xE4..=0xE9 lead byte, i.e.
/// U+4000..=U+9FFF.
pub(crate) fn is_chinese(c: char) -> bool {
    ('\u{4000}'..='\u{9FFF}').contains(&c)
}

/// C++: `RAGAnalyzer::SplitByLang` — split each chunk into maximal Chinese /
/// non-Chinese runs, keeping each run's byte start.
pub(crate) fn split_by_lang_spans(line: &str) -> Vec<(String, bool, usize)> {
    let mut out = Vec::new();
    for (chunk, chunk_start) in split_keep_delimiters_spans(line) {
        let mut run = String::new();
        let mut run_is_zh: Option<bool> = None;
        let mut run_start = chunk_start;
        for (offset, c) in chunk.char_indices() {
            let is_zh = is_chinese(c);
            match run_is_zh {
                Some(previous) if previous == is_zh => run.push(c),
                Some(previous) => {
                    if !run.is_empty() {
                        out.push((std::mem::take(&mut run), previous, run_start));
                    }
                    run_start = chunk_start + offset;
                    run.push(c);
                    run_is_zh = Some(is_zh);
                }
                None => {
                    run_start = chunk_start + offset;
                    run.push(c);
                    run_is_zh = Some(is_zh);
                }
            }
        }
        if let Some(is_zh) = run_is_zh {
            if !run.is_empty() {
                out.push((run, is_zh, run_start));
            }
        }
    }
    out
}

/// C++: `RAGAnalyzer::SplitByLang` without byte spans.
pub(crate) fn split_by_lang(line: &str) -> Vec<(String, bool)> {
    split_by_lang_spans(line)
        .into_iter()
        .map(|(segment, is_zh, _)| (segment, is_zh))
        .collect()
}

pub(crate) fn ends_with_pattern2(text: &str) -> bool {
    text.chars()
        .last()
        .is_some_and(|c| c.is_ascii_alphabetic() || c == '.' || c == '-')
}

pub(crate) fn ends_with_pattern3(text: &str) -> bool {
    text.chars()
        .last()
        .is_some_and(|c| c.is_ascii_digit() || c == '.' || c == '-')
}

pub(crate) fn join_tokens(tokens: &[(String, i32)], start: usize, end: usize) -> String {
    tokens[start..end]
        .iter()
        .map(|(token, _)| token.as_str())
        .collect::<Vec<_>>()
        .join(" ")
}

/// C++: `Join(tokens, start, end, "")` — the DFS input must be the raw
/// concatenation of the chosen span, not a space-joined string.
pub(crate) fn concat_tokens(tokens: &[(String, i32)], start: usize, end: usize) -> String {
    tokens[start..end]
        .iter()
        .map(|(token, _)| token.as_str())
        .collect()
}
