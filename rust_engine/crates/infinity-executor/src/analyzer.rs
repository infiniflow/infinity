//! Text analysis: the standard tokenizer/analyzer.
//!
//! C++: `TokenizeConfig` / `Tokenizer` (`src/common/analyzer/tokenizer.cppm`),
//! `CommonLanguageAnalyzer` (`common_analyzer.cppm`) and `StandardAnalyzer`
//! (`standard_analyzer.cppm`). The full-text index path already used the same
//! tokenization rules through `lib.rs`; this module makes the classes explicit
//! and exposes the configurable options the unit tests exercise.
//!
//! It also carries the `AnalyzerPool` name dispatcher and the `RAGAnalyzer`
//! subset it needs (`rag[-<lang>][-fine]`), including the Darts-clone trie
//! reader used for dictionary-driven Chinese segmentation.

use std::path::Path;
use std::sync::{Arc, Mutex, OnceLock};

/// Token classes, mirroring the reference's per-byte table.
const ALLOW: u8 = 0;
const DELIM: u8 = 1;
const SPACE: u8 = 2;
const UNITE: u8 = 3;

/// C++: `TokenizeConfig` (`tokenizer.cppm`). Defaults: ASCII alphanumerics and
/// every byte above 127 are allowed, ASCII whitespace separates, `/` unites,
/// everything else divides.
#[derive(Debug, Clone)]
pub struct TokenizeConfig {
    kind: [u8; 256],
}

impl Default for TokenizeConfig {
    fn default() -> Self {
        let mut kind = [DELIM; 256];
        for (b, slot) in kind.iter_mut().enumerate() {
            *slot = if b > 127 || (b as u8).is_ascii_alphanumeric() {
                ALLOW
            } else if (b as u8).is_ascii_whitespace() {
                SPACE
            } else if b == b'/' as usize {
                UNITE
            } else {
                DELIM
            };
        }
        Self { kind }
    }
}

impl TokenizeConfig {
    pub fn new() -> Self {
        Self::default()
    }

    /// C++: `TokenizeConfig::AddAllows`.
    pub fn add_allows(&mut self, chars: &str) {
        for b in chars.bytes() {
            self.kind[b as usize] = ALLOW;
        }
    }

    /// C++: `TokenizeConfig::AddDivides`.
    pub fn add_divides(&mut self, chars: &str) {
        for b in chars.bytes() {
            self.kind[b as usize] = DELIM;
        }
    }

    /// C++: `TokenizeConfig::AddUnites`.
    pub fn add_unites(&mut self, chars: &str) {
        for b in chars.bytes() {
            self.kind[b as usize] = UNITE;
        }
    }

    fn class(&self, b: u8) -> u8 {
        self.kind[b as usize]
    }
}

/// C++: `Tokenizer::Tokenize` — returns each raw token with its byte span.
///
/// A delimiter is skipped on its own; a unite byte is dropped while the token
/// continues, but a *leading* unite byte starts the token (the reference's
/// scanner behavior, verified on the engine: `x/y` -> `xy`, `/foo` -> `/foo`).
pub fn tokenize(s: &str, config: &TokenizeConfig) -> Vec<(String, usize, usize)> {
    let bytes = s.as_bytes();
    let mut out = Vec::new();
    let mut i = 0usize;
    while i < bytes.len() {
        while i < bytes.len() && config.class(bytes[i]) == SPACE {
            i += 1;
        }
        if i == bytes.len() {
            break;
        }
        if config.class(bytes[i]) == DELIM {
            i += 1;
            continue;
        }
        let start = i;
        let mut tok = vec![bytes[i]];
        i += 1;
        while i < bytes.len() {
            match config.class(bytes[i]) {
                SPACE | DELIM => break,
                ALLOW => {
                    tok.push(bytes[i]);
                    i += 1;
                }
                _ => i += 1, // UNITE: dropped, the token continues
            }
        }
        let text = String::from_utf8_lossy(&tok).into_owned();
        if !text.is_empty() {
            out.push((text, start, i));
        }
    }
    out
}

/// C++: `Term`.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Term {
    pub text: String,
    pub word_offset: u32,
    pub char_offset: u32,
    pub end_offset: u32,
    pub is_index: bool,
}

/// C++: `StandardAnalyzer` + `CommonLanguageAnalyzer` options.
#[derive(Debug, Clone)]
pub struct StandardAnalyzer {
    tokenize_config: TokenizeConfig,
    case_sensitive: bool,
    contain_lower: bool,
    extract_eng_stem: bool,
    stemmer_initialized: bool,
    stem_only: bool,
    char_offset: bool,
}

impl Default for StandardAnalyzer {
    fn default() -> Self {
        Self::new()
    }
}

impl StandardAnalyzer {
    pub fn new() -> Self {
        Self {
            tokenize_config: TokenizeConfig::default(),
            case_sensitive: false,
            contain_lower: false,
            extract_eng_stem: true,
            stemmer_initialized: false,
            stem_only: false,
            char_offset: false,
        }
    }

    /// C++: `CommonLanguageAnalyzer::InitStemmer(STEM_LANG_ENGLISH)` — the port
    /// ships the English stemmer only.
    pub fn init_stemmer_english(&mut self) {
        self.stemmer_initialized = true;
    }

    /// C++: `CommonLanguageAnalyzer::SetCaseSensitive`.
    pub fn set_case_sensitive(&mut self, case_sensitive: bool, contain_lower: bool) {
        self.case_sensitive = case_sensitive;
        self.contain_lower = contain_lower;
    }

    /// C++: `CommonLanguageAnalyzer::SetExtractEngStem`.
    pub fn set_extract_eng_stem(&mut self, extract: bool) {
        self.extract_eng_stem = extract;
    }

    /// C++: `CommonLanguageAnalyzer::SetStemOnly`.
    pub fn set_stem_only(&mut self, stem_only: bool) {
        self.stem_only = stem_only;
    }

    /// C++: `SetCharOffset` — emit byte offsets instead of token ordinals.
    pub fn set_char_offset(&mut self, char_offset: bool) {
        self.char_offset = char_offset;
    }

    /// C++: `StandardAnalyzer::SetTokenizerConfig`.
    pub fn set_tokenizer_config(&mut self, config: TokenizeConfig) {
        self.tokenize_config = config;
    }

    /// C++: `CommonLanguageAnalyzer::AnalyzeImpl` + `StandardAnalyzer::NextToken`.
    pub fn analyze(&self, input: &str) -> Vec<Term> {
        let mut out = Vec::new();
        for (word_index, (token, start, end)) in tokenize(input, &self.tokenize_config)
            .into_iter()
            .enumerate()
        {
            let word_offset = word_index as u32;
            let offset = if self.char_offset {
                start as u32
            } else {
                word_offset
            };
            let end_offset = if self.char_offset {
                end as u32
            } else {
                word_offset
            };
            let lower = token.to_ascii_lowercase();
            let stem = if self.extract_eng_stem && self.stemmer_initialized {
                crate::stem_english(&lower)
            } else {
                lower.clone()
            };
            let stem_differs = stem != lower;
            let lower_differs = token != lower;

            let mut emit = |text: &str| {
                out.push(Term {
                    text: text.to_string(),
                    word_offset,
                    char_offset: offset,
                    end_offset,
                    is_index: true,
                });
            };

            if stem_differs && self.stem_only {
                emit(&stem);
            } else if stem_differs || (self.case_sensitive && self.contain_lower && lower_differs) {
                if self.case_sensitive {
                    emit(&token);
                } else {
                    emit(&lower);
                }
                if stem_differs {
                    emit(&stem);
                }
                if self.case_sensitive && self.contain_lower && lower_differs {
                    emit(&lower);
                }
            } else if self.case_sensitive {
                emit(&token);
            } else {
                emit(&lower);
            }
        }
        out
    }
}

/// C++: `NGramAnalyzer` (`ngram_analyzer.cppm`, `ngram_analyzer_impl.cpp`).
///
/// Slides a window of `n` code points over each whitespace-delimited run; a run
/// shorter than `n` at the end of the input produces nothing.
#[derive(Debug, Clone)]
pub struct NGramAnalyzer {
    ngram: usize,
}

impl NGramAnalyzer {
    pub fn new(ngram: usize) -> Self {
        Self { ngram }
    }

    pub fn analyze(&self, input: &str) -> Vec<Term> {
        let chars: Vec<(usize, char)> = input.char_indices().collect();
        let mut out = Vec::new();
        let mut offset = 0u32;
        let mut i = 0usize;
        while i < chars.len() {
            if chars[i].1.is_whitespace() {
                i += 1;
                continue;
            }
            let mut end = i;
            let mut count = 0usize;
            let mut hit_space = false;
            while end < chars.len() && count < self.ngram {
                if chars[end].1.is_whitespace() {
                    hit_space = true;
                    break;
                }
                end += 1;
                count += 1;
            }
            if count < self.ngram {
                if hit_space {
                    // A space inside the window ends this run without emitting;
                    // continue with the next word.
                    i = end;
                    while i < chars.len() && chars[i].1.is_whitespace() {
                        i += 1;
                    }
                    continue;
                }
                // Fewer than `n` code points remain: the reference stops.
                break;
            }
            let start_byte = chars[i].0;
            let end_byte = chars.get(end).map(|(b, _)| *b).unwrap_or(input.len());
            out.push(Term {
                text: input[start_byte..end_byte].to_string(),
                word_offset: offset,
                char_offset: start_byte as u32,
                end_offset: end_byte as u32,
                is_index: false,
            });
            offset += 1;
            i += 1;
        }
        out
    }
}

// ---------------------------------------------------------------------------
// RAG analyzer
// ---------------------------------------------------------------------------

/// C++: the shipped resource root (`InfinityContext::config()` fallback).
pub const DEFAULT_RESOURCE_PATH: &str = "/usr/share/infinity/resource";

/// C++: `AnalyzerPool` (`analyzer_pool.cppm`) — parses an analyzer name and
/// hands out a fresh analyzer.
///
/// Only the `rag[-<lang>][-fine]`, `standard` and `ngram-<n>` families are
/// ported; the CJK/IK/whitespace families live in C++ only.
pub struct AnalyzerPool {
    rag_prototype: Mutex<Option<Arc<RAGAnalyzer>>>,
}

/// A concrete analyzer handed out by [`AnalyzerPool`].
pub enum Analyzer {
    Rag(RAGAnalyzer),
    Standard(Box<StandardAnalyzer>),
    NGram(NGramAnalyzer),
}

impl AnalyzerPool {
    /// C++: `Singleton<AnalyzerPool>::instance()`.
    pub fn instance() -> &'static AnalyzerPool {
        static INSTANCE: OnceLock<AnalyzerPool> = OnceLock::new();
        INSTANCE.get_or_init(|| AnalyzerPool {
            rag_prototype: Mutex::new(None),
        })
    }

    /// C++: `AnalyzerPool::GetAnalyzer`.
    pub fn get_analyzer(&self, name: &str) -> Result<Analyzer, String> {
        match name.split('-').next().unwrap_or("") {
            "rag" => self.get_rag(name),
            "standard" => {
                // C++: divide `@#$`, unite `/`, then InitStemmer(English).
                let mut analyzer = StandardAnalyzer::new();
                let mut config = TokenizeConfig::default();
                config.add_divides("@#$");
                config.add_unites("/");
                analyzer.set_tokenizer_config(config);
                analyzer.init_stemmer_english();
                Ok(Analyzer::Standard(Box::new(analyzer)))
            }
            "ngram" => {
                let n = name
                    .strip_prefix("ngram-")
                    .and_then(|suffix| suffix.parse::<usize>().ok())
                    .filter(|n| *n > 0)
                    .ok_or_else(|| format!("NGRAM-number, number > 0, but it is {name}."))?;
                Ok(Analyzer::NGram(NGramAnalyzer::new(n)))
            }
            _ => Err(format!("analyzer not found: {name}")),
        }
    }

    /// C++: the `case Str2Int(RAG)` branch — `rag` optionally followed by
    /// `-`-separated segments that are either `fine` or a language name, in
    /// any order. Unrecognized segments are ignored and keep the defaults.
    fn get_rag(&self, name: &str) -> Result<Analyzer, String> {
        let mut prototype = self
            .rag_prototype
            .lock()
            .map_err(|_| "analyzer pool was poisoned".to_string())?;
        if prototype.is_none() {
            *prototype = Some(Arc::new(RAGAnalyzer::load(DEFAULT_RESOURCE_PATH)?));
        }
        // Every call copies the cached prototype, so per-name settings never
        // leak into the next analyzer handed out.
        let mut analyzer = prototype
            .as_ref()
            .expect("prototype was just initialized")
            .as_ref()
            .clone();

        let mut fine_grained = false;
        let mut rest = &name["rag".len()..];
        while let Some(stripped) = rest.strip_prefix('-') {
            rest = stripped;
            let end = rest.find('-').unwrap_or(rest.len());
            let segment = &rest[..end];
            rest = &rest[end..];
            if segment == "fine" {
                fine_grained = true;
            } else if !segment.is_empty() {
                analyzer.set_language(segment);
            }
        }
        analyzer.set_fine_grained(fine_grained);
        Ok(Analyzer::Rag(analyzer))
    }
}

/// C++: `DartsTrie` (`darts_trie_impl.cpp`) over a Darts-clone unit array.
///
/// The file is the raw little-endian `u32` array (`darts.h::save`): byte 0 of
/// each unit is the label, bit 8 marks a leaf, bits 10.. hold the offset.
pub struct DartsTrie {
    units: Vec<u32>,
}

impl DartsTrie {
    fn load(path: &Path) -> Result<Self, String> {
        let bytes = std::fs::read(path)
            .map_err(|err| format!("failed to read {}: {err}", path.display()))?;
        if bytes.len() % 4 != 0 {
            return Err(format!("invalid trie length: {}", bytes.len()));
        }
        let units: Vec<u32> = bytes
            .chunks_exact(4)
            .map(|chunk| u32::from_le_bytes([chunk[0], chunk[1], chunk[2], chunk[3]]))
            .collect();
        if units.len() < 256 || !units.len().is_multiple_of(256) {
            return Err(format!("invalid trie unit count: {}", units.len()));
        }
        Ok(Self { units })
    }

    fn label(unit: u32) -> u32 {
        unit & 0x8000_00FF
    }

    fn has_leaf(unit: u32) -> bool {
        (unit >> 8) & 1 == 1
    }

    fn offset(unit: u32) -> u32 {
        (unit >> 10) << ((unit & (1 << 9)) >> 6)
    }

    fn value(unit: u32) -> i32 {
        (unit & 0x7FFF_FFFF) as i32
    }

    /// C++: `DartsTrie::Get` / `DoubleArray::exactMatchSearch`.
    fn exact_match(&self, key: &[u8]) -> i32 {
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
    fn has_prefix(&self, key: &[u8]) -> bool {
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

/// C++: `RAGAnalyzer` (`rag_analyzer.cppm`) — the subset `AnalyzerPool` needs.
///
/// Not ported because the analyzer tests do not exercise them: OpenCC
/// traditional→simplified conversion (identity here), the WordNet lemmatizer,
/// position-enabled tokenization, and the trie build path (the shipped
/// `rag/huqie.trie` is required).
#[derive(Clone)]
pub struct RAGAnalyzer {
    trie: Arc<DartsTrie>,
    fine_grained: bool,
    fold_diacritics: bool,
    stem_algorithm: Option<rust_stemmers::Algorithm>,
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
        let trie_path = root.join("rag/huqie.trie");
        if !trie_path.is_file() {
            return Err(format!(
                "missing analyzer file: {} (building the trie is not ported)",
                trie_path.display()
            ));
        }
        Ok(Self {
            trie: Arc::new(DartsTrie::load(&trie_path)?),
            fine_grained: false,
            fold_diacritics: false,
            stem_algorithm: Some(rust_stemmers::Algorithm::English),
        })
    }

    /// C++: `RAGAnalyzer::SetLanguage`. Slovak and Czech have no Snowball
    /// stemmer: their diacritics are folded to ASCII and stemming is disabled.
    pub fn set_language(&mut self, language: &str) {
        let key = language.trim().to_ascii_lowercase();
        if key == "slovak" || key == "czech" {
            self.fold_diacritics = true;
            self.stem_algorithm = None;
            return;
        }
        if let Some(algorithm) = snowball_algorithm(&key) {
            self.fold_diacritics = false;
            self.stem_algorithm = Some(algorithm);
        }
        // Unknown languages keep the current defaults (C++ logs and returns).
    }

    /// C++: `RAGAnalyzer::SetFineGrained`.
    pub fn set_fine_grained(&mut self, fine_grained: bool) {
        self.fine_grained = fine_grained;
    }

    /// C++: `RAGAnalyzer::Tokenize` — returns the space-joined token stream.
    pub fn tokenize(&self, line: &str) -> String {
        let folded = self.fold_diacritics.then(|| fold_diacritics(line));
        let input = folded.as_deref().unwrap_or(line);

        // Python-style simple tokenization: re.sub(r"\W+", " ", line).
        let processed = strip_non_word(input);
        let half_width = str_q2b(&processed);
        // OpenCC conversion is not ported; the inputs here are already
        // simplified Chinese, for which the reference conversion is identity.

        let mut res: Vec<String> = Vec::new();
        for (segment, is_zh) in split_by_lang(&half_width) {
            if !is_zh {
                // NLTK word tokenizer: punctuation was already replaced, so
                // word-character runs are the tokens.
                for word in segment
                    .split(|c: char| !(c.is_alphanumeric() || c == '_'))
                    .filter(|word| !word.is_empty())
                {
                    let lower = word.to_ascii_lowercase();
                    res.push(self.stem_word(&lower));
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

    /// C++: `RAGAnalyzer::AnalyzeImpl` (position-disabled branch) + `Analyze`.
    pub fn analyze(&self, text: &str) -> Vec<Term> {
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
        let mut terms = Vec::new();
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
            });
            offset += 1;
        }
        terms
    }

    /// C++: `RAGAnalyzer::FineGrainedTokenize`.
    fn fine_grained_tokenize(&self, tokens: &str) -> Vec<String> {
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

    /// C++: `RAGAnalyzer::EnglishNormalize` (the WordNet lemmatizer is not
    /// ported; the Snowball stemmer is applied to the lower-cased token).
    fn english_normalize(&self, tokens: &[String]) -> Vec<String> {
        tokens
            .iter()
            .map(|token| {
                let is_word = token
                    .chars()
                    .last()
                    .is_some_and(|c| c.is_ascii_alphabetic() || c == '_' || c == '-');
                if is_word {
                    self.stem_word(&token.to_ascii_lowercase())
                } else {
                    token.clone()
                }
            })
            .collect()
    }

    fn stem_word(&self, token: &str) -> String {
        match self.stem_algorithm {
            Some(rust_stemmers::Algorithm::English) => crate::stem_english(token),
            Some(algorithm) => rust_stemmers::Stemmer::create(algorithm)
                .stem(token)
                .into_owned(),
            None => token.to_string(),
        }
    }

    /// C++: `RAGAnalyzer::TokenizeInner` — forward/backward maximum matching
    /// plus the DFS re-scoring of the divergent middle.
    fn tokenize_inner(&self, line: &str) -> Vec<String> {
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
            let tk1 = join_tokens(&tks1, i_anchor, i);
            let tk = join_tokens(&tks, j_anchor, j);
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
            let mut dfs = Dfs::new(self, &join_tokens(&tks, j_anchor, j), false);
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
            let mut dfs = Dfs::new(self, &join_tokens(&tks, j_anchor, tks.len()), false);
            dfs.run(0, &mut Vec::new(), 0);
            res.push(dfs.best_tokens.join(" "));
        }
        res
    }

    /// C++: `RAGAnalyzer::MaxForward`.
    fn max_forward(&self, line: &str) -> Vec<(String, i32)> {
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
    fn max_backward(&self, line: &str) -> Vec<(String, i32)> {
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
    fn merge(&self, tks_str: &str) -> String {
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
    fn freq(&self, key: &str) -> i32 {
        let value = self.trie.exact_match(key.as_bytes());
        let freq = decode_freq(value);
        (freq as f64).exp().mul_add(DENOMINATOR, 0.5) as i32
    }
}

/// C++: `MAX_DFS_DEPTH`.
const MAX_DFS_DEPTH: usize = 10;
/// C++: `RAGAnalyzer::DENOMINATOR`.
const DENOMINATOR: f64 = 1_000_000.0;
/// C++: `RAGAnalyzer::Score`'s `B`.
const SCORE_B: f64 = 30.0;

/// C++: `RAGAnalyzer::DFS` — enumerates the dictionary paths through `chars`.
struct Dfs<'a> {
    analyzer: &'a RAGAnalyzer,
    chars: Vec<char>,
    memo_all: bool,
    token_list: Vec<Vec<(String, i32)>>,
    best_tokens: Vec<String>,
    max_score: f64,
}

impl<'a> Dfs<'a> {
    fn new(analyzer: &'a RAGAnalyzer, text: &str, memo_all: bool) -> Self {
        Self {
            analyzer,
            chars: text.chars().collect(),
            memo_all,
            token_list: Vec::new(),
            best_tokens: Vec::new(),
            max_score: f64::MIN,
        }
    }

    fn record(&mut self, pre_tokens: &[(String, i32)]) {
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

    fn run(&mut self, s: usize, pre_tokens: &mut Vec<(String, i32)>, depth: usize) -> usize {
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
fn score_tokens(token_freqs: &[(String, i32)]) -> (Vec<String>, f64) {
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
fn encode(freq: i32, idx: i32) -> i32 {
    let mut encoded: u32 = if freq < 0 {
        ((-freq) as u32) | (1 << 23)
    } else {
        (freq as u32) & 0x7F_FFFF
    };
    encoded |= (idx as u32) << 24;
    encoded as i32
}

/// C++: `DecodeFreq`.
fn decode_freq(value: i32) -> i32 {
    let mut v1 = (value as u32) & 0xFF_FFFF;
    if v1 & (1 << 23) != 0 {
        v1 &= 0x7F_FFFF;
        -(v1 as i32)
    } else {
        v1 as i32
    }
}

fn char_len(text: &str) -> usize {
    text.chars().count()
}

/// C++: `RAGAnalyzer::Key` — ASCII lowering only.
fn key(text: &str) -> String {
    text.to_ascii_lowercase()
}

/// C++: `RAGAnalyzer::RKey` — `"DD"` followed by the reversed code points,
/// lowered.
fn rkey(text: &str) -> String {
    let reversed: String = text.chars().rev().collect();
    format!("DD{}", reversed.to_ascii_lowercase())
}

/// C++: `Config::ResourcePath()` fallback table for Snowball stemmers.
fn snowball_algorithm(language: &str) -> Option<rust_stemmers::Algorithm> {
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
fn is_stopword(term: &str) -> bool {
    const STOPWORDS: &[&str] = &[
        "请问", "您", "你", "我", "他", "是", "的", "就", "有", "于", "及", "即", "在", "为", "最",
        "从", "以", "了", "将", "与", "吗", "吧", "中", "#", "什么", "怎么", "哪个", "哪些", "啥",
        "相关",
    ];
    STOPWORDS.contains(&term)
}

/// C++: `RAGAnalyzer::FoldDiacritics` — ASCII base letter for U+00C0–U+017F,
/// or 0 for code points without a single-ASCII-letter NFD decomposition.
const DIACRITICS_FOLD_TABLE: [u8; 0xC0] = [
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

fn fold_diacritics(input: &str) -> String {
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
fn str_q2b(input: &str) -> String {
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
fn strip_non_word(input: &str) -> String {
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

/// C++: `IsChinese` — a 3-byte sequence with a 0xE4..=0xE9 lead byte, i.e.
/// U+4000..=U+9FFF.
fn is_chinese(c: char) -> bool {
    ('\u{4000}'..='\u{9FFF}').contains(&c)
}

/// Characters in `RAGAnalyzer::regex_split_pattern_`'s first alternative.
fn is_split_delimiter(c: char) -> bool {
    matches!(
        c,
        ' ' | ','
            | '.'
            | '<'
            | '>'
            | '/'
            | '?'
            | ';'
            | ':'
            | '\''
            | '['
            | ']'
            | '\\'
            | '`'
            | '!'
            | '@'
            | '#'
            | '$'
            | '%'
            | '^'
            | '&'
            | '*'
            | '('
            | ')'
            | '{'
            | '}'
            | '|'
            | '_'
            | '+'
            | '='
            | '《'
            | '》'
            | '，'
            | '。'
            | '？'
            | '、'
            | '；'
            | '‘'
            | '’'
            | '：'
            | '“'
            | '”'
            | '【'
            | '】'
            | '~'
            | '！'
            | '￥'
            | '…'
            | '（'
            | '）'
            | '—'
            | '-'
    )
}

/// C++: `Split(line, regex_split_pattern_, arr, true)` — maximal runs of the
/// punctuation class, of `[a-zA-Z0-9,.-]+`, and of everything else.
fn split_keep_delimiters(line: &str) -> Vec<String> {
    let chars: Vec<char> = line.chars().collect();
    let mut chunks = Vec::new();
    let mut i = 0usize;
    while i < chars.len() {
        let start = i;
        if is_split_delimiter(chars[i]) {
            while i < chars.len() && is_split_delimiter(chars[i]) {
                i += 1;
            }
        } else if chars[i].is_ascii_alphanumeric() {
            i += 1;
            while i < chars.len()
                && (chars[i].is_ascii_alphanumeric() || matches!(chars[i], ',' | '.' | '-'))
            {
                i += 1;
            }
        } else {
            while i < chars.len()
                && !is_split_delimiter(chars[i])
                && !chars[i].is_ascii_alphanumeric()
            {
                i += 1;
            }
        }
        chunks.push(chars[start..i].iter().collect());
    }
    chunks
}

/// C++: `RAGAnalyzer::SplitByLang` — split each chunk into maximal Chinese /
/// non-Chinese runs.
fn split_by_lang(line: &str) -> Vec<(String, bool)> {
    let mut out = Vec::new();
    for chunk in split_keep_delimiters(line) {
        let mut run = String::new();
        let mut run_is_zh: Option<bool> = None;
        for c in chunk.chars() {
            let is_zh = is_chinese(c);
            if run_is_zh == Some(is_zh) {
                run.push(c);
                continue;
            }
            if let Some(previous) = run_is_zh {
                if !run.is_empty() {
                    out.push((std::mem::take(&mut run), previous));
                }
            }
            run.push(c);
            run_is_zh = Some(is_zh);
        }
        if let Some(is_zh) = run_is_zh {
            if !run.is_empty() {
                out.push((run, is_zh));
            }
        }
    }
    out
}

/// C++: `re2::RE2::PartialMatch(text, RAGAnalyzer::regex_split_pattern_)`.
fn regex_split_partial(text: &str) -> bool {
    text.chars()
        .any(|c| c.is_ascii_alphanumeric() || is_split_delimiter(c))
}

fn ends_with_pattern2(text: &str) -> bool {
    text.chars()
        .last()
        .is_some_and(|c| c.is_ascii_alphabetic() || c == '.' || c == '-')
}

fn ends_with_pattern3(text: &str) -> bool {
    text.chars()
        .last()
        .is_some_and(|c| c.is_ascii_digit() || c == '.' || c == '-')
}

fn ends_with_pattern4(text: &str) -> bool {
    text.chars()
        .last()
        .is_some_and(|c| c.is_ascii_digit() || c == ',' || c == '.' || c == '-')
}

fn join_tokens(tokens: &[(String, i32)], start: usize, end: usize) -> String {
    tokens[start..end]
        .iter()
        .map(|(token, _)| token.as_str())
        .collect::<Vec<_>>()
        .join(" ")
}

#[cfg(test)]
mod tests {
    use super::*;

    fn texts(terms: &[Term]) -> Vec<(&str, u32)> {
        terms
            .iter()
            .map(|t| (t.text.as_str(), t.word_offset))
            .collect()
    }

    /// `src/unit_test/common/analyzer/standard_analyzer_ut.cpp::test1`.
    #[test]
    fn standard_analyzer_lowercases_without_stemming() {
        let mut analyzer = StandardAnalyzer::new();
        analyzer.init_stemmer_english();
        analyzer.set_extract_eng_stem(false);
        let terms = analyzer.analyze("Boost unit tests.");
        assert_eq!(texts(&terms), vec![("boost", 0), ("unit", 1), ("tests", 2)]);
    }

    /// `test2`: case-sensitive, no lower-case companion.
    #[test]
    fn standard_analyzer_keeps_the_source_case() {
        let mut analyzer = StandardAnalyzer::new();
        analyzer.set_extract_eng_stem(false);
        analyzer.set_case_sensitive(true, false);
        let terms = analyzer.analyze("Boost unit tests.");
        assert_eq!(texts(&terms), vec![("Boost", 0), ("unit", 1), ("tests", 2)]);
    }

    /// `test3`: the word and its stem share one position.
    #[test]
    fn standard_analyzer_emits_word_and_stem() {
        let mut analyzer = StandardAnalyzer::new();
        analyzer.init_stemmer_english();
        let terms = analyzer.analyze("Boost unit tests.");
        assert_eq!(
            texts(&terms),
            vec![("boost", 0), ("unit", 1), ("tests", 2), ("test", 2)]
        );
    }

    /// `test4`: case-sensitive with the lower-case companion.
    #[test]
    fn standard_analyzer_emits_case_and_lower_forms() {
        let mut analyzer = StandardAnalyzer::new();
        analyzer.set_extract_eng_stem(false);
        analyzer.set_case_sensitive(true, true);
        let terms = analyzer.analyze("Boost unit tests.");
        assert_eq!(
            texts(&terms),
            vec![("Boost", 0), ("boost", 0), ("unit", 1), ("tests", 2)]
        );
    }

    /// `test5`: a token whose stem equals its lower-case form is emitted once.
    #[test]
    fn standard_analyzer_deduplicates_the_stem() {
        let mut analyzer = StandardAnalyzer::new();
        analyzer.set_extract_eng_stem(false);
        let terms = analyzer.analyze("BoostBoostboostBoostboost unit tests.");
        assert_eq!(
            texts(&terms),
            vec![("boostboostboostboostboost", 0), ("unit", 1), ("tests", 2)]
        );
    }

    /// `test6`: a custom tokenizer config allows `-` inside a date.
    #[test]
    fn standard_analyzer_honours_the_tokenizer_config() {
        let mut analyzer = StandardAnalyzer::new();
        let mut config = TokenizeConfig::default();
        config.add_allows("-");
        config.add_divides("@#$");
        config.add_unites("/");
        analyzer.set_tokenizer_config(config);
        let terms = analyzer.analyze("2012-01-02 unit tests.");
        assert_eq!(
            texts(&terms),
            vec![("2012-01-02", 0), ("unit", 1), ("tests", 2)]
        );
    }

    /// `test7`: a large wiki extract is analyzed without panicking (the
    /// reference asserts nothing there either).
    #[test]
    fn standard_analyzer_handles_a_large_extract() {
        let mut analyzer = StandardAnalyzer::new();
        analyzer.init_stemmer_english();
        analyzer.set_extract_eng_stem(true);
        analyzer.set_char_offset(true);
        let mut config = TokenizeConfig::default();
        config.add_allows("-");
        config.add_divides("@#$");
        config.add_unites("/");
        analyzer.set_tokenizer_config(config);
        let input = "{{Redirect|Anarchist|the fictional character|Anarchist (comics)}} \
             '''Anarchism''' is generally defined as the [[political philosophy]] which holds \
             the [[state (polity)|state]] to be undesirable, unnecessary, and harmful, \
             http://www.marxists.org/archive/malatesta/1930s/xx/toanarchy.htm \
             Anarchism as a Theory of Organization 2008-04-14";
        assert!(!analyzer.analyze(input).is_empty());
    }
    /// `src/unit_test/common/analyzer/ngram_analyzer_ut.cpp::test1`.
    #[test]
    fn ngram_two_slides_over_words() {
        let analyzer = NGramAnalyzer::new(2);
        let terms = analyzer.analyze("hello world  123");
        assert_eq!(
            texts(&terms),
            vec![
                ("he", 0),
                ("el", 1),
                ("ll", 2),
                ("lo", 3),
                ("wo", 4),
                ("or", 5),
                ("rl", 6),
                ("ld", 7),
                ("12", 8),
                ("23", 9)
            ]
        );
    }

    /// `ngram_analyzer_ut.cpp::test2` — single-character grams.
    #[test]
    fn ngram_one_emits_each_character() {
        let analyzer = NGramAnalyzer::new(1);
        let terms = analyzer.analyze("abc de  fg");
        assert_eq!(
            texts(&terms),
            vec![
                ("a", 0),
                ("b", 1),
                ("c", 2),
                ("d", 3),
                ("e", 4),
                ("f", 5),
                ("g", 6)
            ]
        );
    }
}

/// `src/unit_test/common/analyzer/analyzer_pool_ut.cpp`.
///
/// Covers `AnalyzerPool::GetAnalyzer()` name parsing for the RAG analyzer:
/// `"rag"` optionally followed by `-`-separated segments that are either
/// `"fine"` or a language name, in any order.
#[cfg(test)]
mod analyzer_pool_tests {
    use super::*;
    use std::path::Path;

    /// A missing resource directory is a hard failure rather than a skip,
    /// matching the reference: CI always mounts it, so skipping would turn a
    /// broken resource setup into a green run.
    const RESOURCE_DIR: &str = "/usr/share/infinity/resource";

    /// Coarse tokenization keeps this as one token, fine-grained tokenization
    /// splits it into "中华" + "人民共和国".
    const GRAIN_PROBE: &str = "中华人民共和国";

    fn resource_available() -> bool {
        Path::new(RESOURCE_DIR).is_dir()
    }

    /// C++: `AnalyzerPoolTest::GetRAGAnalyzer`.
    fn get_rag_analyzer(name: &str) -> Option<RAGAnalyzer> {
        match AnalyzerPool::instance().get_analyzer(name) {
            Ok(Analyzer::Rag(analyzer)) => Some(analyzer),
            Ok(_) => panic!("GetAnalyzer(\"{name}\") did not return a RAGAnalyzer"),
            Err(status) => panic!("GetAnalyzer(\"{name}\") failed: {status}"),
        }
    }

    /// C++: `AnalyzerPoolTest::AnalyzeToTokens` — `Analyze()` is the only place
    /// fine-grained tokenization is observable; `Tokenize()` is always coarse.
    fn analyze_to_tokens(analyzer: &RAGAnalyzer, text: &str) -> Vec<String> {
        analyzer
            .analyze(text)
            .into_iter()
            .map(|term| term.text)
            .collect()
    }

    #[test]
    fn test_rag_default_is_english_and_coarse() {
        if !resource_available() {
            panic!(
                "Resource directory {RESOURCE_DIR} not available; the analyzer cannot be loaded"
            );
        }
        let analyzer = get_rag_analyzer("rag").expect("rag analyzer");

        // English stemming is on by default.
        assert_eq!(analyzer.tokenize("running"), "run");
        // Diacritics are not folded for the default language.
        assert!(analyzer.tokenize("škola").contains("kola"));
        assert!(!analyzer.tokenize("škola").contains("skola"));
        // Coarse grain.
        assert_eq!(
            analyze_to_tokens(&analyzer, GRAIN_PROBE),
            vec!["中华人民共和国".to_string()]
        );
    }

    #[test]
    fn test_rag_fine_keeps_english_stemming() {
        if !resource_available() {
            panic!(
                "Resource directory {RESOURCE_DIR} not available; the analyzer cannot be loaded"
            );
        }
        let analyzer = get_rag_analyzer("rag-fine").expect("rag-fine analyzer");

        assert_eq!(analyzer.tokenize("running"), "run");
        assert_eq!(
            analyze_to_tokens(&analyzer, GRAIN_PROBE),
            vec!["中华".to_string(), "人民共和国".to_string()]
        );
    }

    #[test]
    fn test_rag_slovak_folds_and_disables_stemming() {
        if !resource_available() {
            panic!(
                "Resource directory {RESOURCE_DIR} not available; the analyzer cannot be loaded"
            );
        }
        let analyzer = get_rag_analyzer("rag-slovak").expect("rag-slovak analyzer");

        assert_eq!(analyzer.tokenize("škola"), "skola");
        assert_eq!(analyzer.tokenize("daňové priznanie"), "danove priznanie");
        // No stemming for Slovak.
        assert_eq!(analyzer.tokenize("running"), "running");
        // Still coarse without a "fine" segment.
        assert_eq!(
            analyze_to_tokens(&analyzer, GRAIN_PROBE),
            vec!["中华人民共和国".to_string()]
        );
    }

    #[test]
    fn test_rag_czech_folds_and_disables_stemming() {
        if !resource_available() {
            panic!(
                "Resource directory {RESOURCE_DIR} not available; the analyzer cannot be loaded"
            );
        }
        let analyzer = get_rag_analyzer("rag-czech").expect("rag-czech analyzer");

        assert_eq!(analyzer.tokenize("škola"), "skola");
        assert_eq!(
            analyzer.tokenize("příliš žluťoučký kůň"),
            "prilis zlutoucky kun"
        );
        assert_eq!(analyzer.tokenize("running"), "running");
        assert_eq!(
            analyze_to_tokens(&analyzer, GRAIN_PROBE),
            vec!["中华人民共和国".to_string()]
        );
    }

    #[test]
    fn test_rag_language_and_fine_in_both_orders() {
        if !resource_available() {
            panic!(
                "Resource directory {RESOURCE_DIR} not available; the analyzer cannot be loaded"
            );
        }
        let expected = vec![
            "skola".to_string(),
            "中华".to_string(),
            "人民共和国".to_string(),
        ];
        for name in [
            "rag-czech-fine",
            "rag-fine-czech",
            "rag-slovak-fine",
            "rag-fine-slovak",
        ] {
            let analyzer = get_rag_analyzer(name).expect(name);
            assert_eq!(analyzer.tokenize("škola"), "skola", "{name}");
            assert_eq!(analyzer.tokenize("running"), "running", "{name}");
            assert_eq!(
                analyze_to_tokens(&analyzer, &format!("škola {GRAIN_PROBE}")),
                expected,
                "{name}"
            );
        }
    }

    #[test]
    fn test_rag_unknown_segment_falls_back_to_legacy() {
        if !resource_available() {
            panic!(
                "Resource directory {RESOURCE_DIR} not available; the analyzer cannot be loaded"
            );
        }
        // Unknown languages, an empty segment and a stray trailing '-' must not
        // crash and must leave the legacy English/coarse behaviour in place.
        for name in [
            "rag-",
            "rag--",
            "rag-slovakk",
            "rag-klingon",
            "rag-nosuchlang",
        ] {
            let analyzer = get_rag_analyzer(name).expect(name);
            assert_eq!(analyzer.tokenize("running"), "run", "{name}");
            assert!(!analyzer.tokenize("škola").contains("skola"), "{name}");
            assert_eq!(
                analyze_to_tokens(&analyzer, GRAIN_PROBE),
                vec!["中华人民共和国".to_string()],
                "{name}"
            );
        }

        // An unknown segment next to "fine" still selects fine-grained.
        let analyzer = get_rag_analyzer("rag-klingon-fine").expect("rag-klingon-fine");
        assert_eq!(analyzer.tokenize("running"), "run");
        assert_eq!(
            analyze_to_tokens(&analyzer, GRAIN_PROBE),
            vec!["中华".to_string(), "人民共和国".to_string()]
        );
    }

    #[test]
    fn test_rag_analyzers_are_independent() {
        if !resource_available() {
            panic!(
                "Resource directory {RESOURCE_DIR} not available; the analyzer cannot be loaded"
            );
        }
        // Every GetAnalyzer() call copies the cached prototype; a
        // language-specific instance must not leak its settings into the next
        // one handed out.
        let slovak = get_rag_analyzer("rag-slovak").expect("rag-slovak analyzer");
        assert_eq!(slovak.tokenize("škola"), "skola");

        let english = get_rag_analyzer("rag").expect("rag analyzer");
        assert_eq!(english.tokenize("running"), "run");
        assert!(!english.tokenize("škola").contains("skola"));

        // The Slovak instance is unaffected by the later English one.
        assert_eq!(slovak.tokenize("daňové"), "danove");
    }
}
