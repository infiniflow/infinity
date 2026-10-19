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

use std::collections::{HashMap, HashSet};
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

/// C++: `RAGAnalyzer` (`rag_analyzer.cppm`).
///
/// The WordNet lemmatizer is loaded for English; OpenCC traditional→simplified
/// conversion is the identity here because the shipped inputs are already
/// simplified (the reference conversion is identity for them as well).
#[derive(Clone)]
pub struct RAGAnalyzer {
    trie: Arc<DartsTrie>,
    fine_grained: bool,
    enable_position: bool,
    fold_diacritics: bool,
    stem_algorithm: Option<rust_stemmers::Algorithm>,
    lemmatizer: Option<Arc<WordNetLemmatizer>>,
    use_lemmatizer: bool,
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
            enable_position: false,
            fold_diacritics: false,
            stem_algorithm: Some(rust_stemmers::Algorithm::English),
            lemmatizer: WordNetLemmatizer::load(&root.join("wordnet"))
                .ok()
                .map(Arc::new),
            use_lemmatizer: true,
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
        // OpenCC conversion is not ported; the inputs here are already
        // simplified Chinese, for which the reference conversion is identity.

        let mut res: Vec<String> = Vec::new();
        for (segment, is_zh) in split_by_lang(&half_width) {
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
        // OpenCC conversion is identity for the shipped inputs.
        let strline = str1.as_str();

        // strline byte -> original input byte.
        let mut final_map = Vec::with_capacity(strline.len() + 1);
        for i in 0..strline.len() {
            let processed_pos = q2b_map.get(i).copied().unwrap_or(processed.len() as u32) as usize;
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
        for (segment, is_zh, segment_start) in split_by_lang_spans(strline) {
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
    fn fine_grained_tokenize_with_position(
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
    fn english_normalize(&self, tokens: &[String]) -> Vec<String> {
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
    fn normalize_token(&self, token: &str) -> String {
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
    fn tokenize_inner_with_position(
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

/// C++: `WordNetLemmatizer` (`wordnet_lemmatizer_impl.cpp`).
///
/// Only the noun part of speech is needed: the reference calls
/// `Lemmatize(form)` with an empty POS, which selects `NOUN` to match Python
/// NLTK's default.
struct WordNetLemmatizer {
    noun_lemmas: HashSet<String>,
    noun_exceptions: HashMap<String, Vec<String>>,
}

impl WordNetLemmatizer {
    fn load(root: &Path) -> Result<Self, String> {
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
    fn lemmatize(&self, form: &str) -> String {
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
fn noun_substitutions(form: &str) -> Vec<String> {
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

/// C++: `PCRE2GlobalReplaceWithPosition(input, r"\W+", " ")` — the processed
/// string plus, for every output byte, its byte offset in `input`. The single
/// replacement byte maps to the start of the replaced run, like the reference.
fn strip_non_word_with_position(input: &str) -> (String, Vec<u32>) {
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
fn build_position_mapping(original: &str, converted: &str) -> Vec<u32> {
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

fn utf8_len(lead: u8) -> usize {
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
fn str_q2b_with_position(input: &str) -> (String, Vec<u32>) {
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
fn nltk_word_tokenize(text: &str) -> Vec<String> {
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
fn sentence_split(text: &str) -> Vec<String> {
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

/// C++: the token emission shared by `TokenizeInnerWithPosition` and
/// `FineGrainedTokenizeWithPosition`. A token containing spaces is split and
/// each part gets a consecutive span; otherwise the given span is mapped back
/// through `mapping`.
fn emit_with_positions(
    tokens: &mut Vec<String>,
    positions: &mut Vec<(u32, u32)>,
    token: &str,
    start: usize,
    end: usize,
    mapping: &[u32],
) {
    let map = |pos: usize| -> u32 { mapping.get(pos).copied().unwrap_or(0) };
    if token.contains(' ') {
        let mut space_start = start;
        for part in token.split(' ').filter(|part| !part.is_empty()) {
            let space_end = space_start + part.len();
            tokens.push(part.to_string());
            positions.push((map(space_start), map(space_end)));
            space_start = space_end;
        }
    } else {
        tokens.push(token.to_string());
        positions.push((map(start), map(end)));
    }
}

/// C++: `RAGAnalyzer::MergeWithPosition`.
fn merge_with_position(
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
/// punctuation class, of `[a-zA-Z0-9,.-]+`, and of everything else. Each chunk
/// carries its byte start so position-aware tokenization can map it back.
fn split_keep_delimiters_spans(line: &str) -> Vec<(String, usize)> {
    let chars: Vec<(usize, char)> = line.char_indices().collect();
    let mut chunks = Vec::new();
    let mut i = 0usize;
    while i < chars.len() {
        let start = i;
        if is_split_delimiter(chars[i].1) {
            while i < chars.len() && is_split_delimiter(chars[i].1) {
                i += 1;
            }
        } else if chars[i].1.is_ascii_alphanumeric() {
            i += 1;
            while i < chars.len()
                && (chars[i].1.is_ascii_alphanumeric() || matches!(chars[i].1, ',' | '.' | '-'))
            {
                i += 1;
            }
        } else {
            while i < chars.len()
                && !is_split_delimiter(chars[i].1)
                && !chars[i].1.is_ascii_alphanumeric()
            {
                i += 1;
            }
        }
        let start_byte = chars[start].0;
        let end_byte = chars.get(i).map(|(byte, _)| *byte).unwrap_or(line.len());
        chunks.push((line[start_byte..end_byte].to_string(), start_byte));
    }
    chunks
}

/// C++: `RAGAnalyzer::SplitByLang` — split each chunk into maximal Chinese /
/// non-Chinese runs, keeping each run's byte start.
fn split_by_lang_spans(line: &str) -> Vec<(String, bool, usize)> {
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
fn split_by_lang(line: &str) -> Vec<(String, bool)> {
    split_by_lang_spans(line)
        .into_iter()
        .map(|(segment, is_zh, _)| (segment, is_zh))
        .collect()
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

/// C++: `Join(tokens, start, end, "")` — the DFS input must be the raw
/// concatenation of the chosen span, not a space-joined string.
fn concat_tokens(tokens: &[(String, i32)], start: usize, end: usize) -> String {
    tokens[start..end]
        .iter()
        .map(|(token, _)| token.as_str())
        .collect()
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

/// `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` — the 13
/// `RAGAnalyzerTest` cases. The Python oracle is the same
/// `python/infinity_sdk/infinity/rag_tokenizer.py` script the reference
/// shells out to.
#[cfg(test)]
mod rag_analyzer_tests {
    use super::*;
    use std::path::{Path, PathBuf};
    use std::process::Command;

    /// The reference fixture: a missing resource directory is a hard failure.
    const RESOURCE_DIR: &str = "/usr/share/infinity/resource";

    fn load_analyzer() -> RAGAnalyzer {
        RAGAnalyzer::load(RESOURCE_DIR).expect("RAGAnalyzer load")
    }

    fn repo_root() -> PathBuf {
        Path::new(env!("CARGO_MANIFEST_DIR")).join("../../..")
    }

    fn tokenizer_script() -> PathBuf {
        repo_root().join("python/infinity_sdk/infinity/rag_tokenizer.py")
    }

    fn input_file() -> PathBuf {
        repo_root().join("python/infinity_sdk/infinity/tokenizer_input.txt")
    }

    /// C++: `std::getline` over `tokenizer_input.txt`, skipping empty lines.
    fn input_lines() -> Vec<String> {
        std::fs::read_to_string(input_file())
            .expect("read tokenizer_input.txt")
            .lines()
            .filter(|line| !line.is_empty())
            .map(str::to_string)
            .collect()
    }

    /// C++: `system("uv run rag_tokenizer.py -f <input> -o <output> ...")`,
    /// followed by reading one result line per input line.
    ///
    /// `uv` runs from the temp directory: the script is a PEP 723 script, and
    /// starting outside the checkout keeps `uv` from rewriting `uv.lock`.
    fn python_tokenizer_lines(extra: &[&str], tag: &str) -> Vec<String> {
        let work_dir = std::env::temp_dir();
        let output_path = work_dir.join(format!(
            "infinity_rag_tokenizer_{}_{}.txt",
            std::process::id(),
            tag
        ));
        let output = Command::new("uv")
            .current_dir(&work_dir)
            .arg("run")
            .arg(tokenizer_script())
            .arg("-f")
            .arg(input_file())
            .arg("-o")
            .arg(&output_path)
            .args(extra)
            .output()
            .expect("run uv for the Python tokenizer");
        assert!(
            output.status.success(),
            "Call Python tokenizer failed: {}",
            String::from_utf8_lossy(&output.stderr)
        );
        let text = std::fs::read_to_string(&output_path).expect("read Python tokenizer output");
        let _ = std::fs::remove_file(&output_path);
        text.lines().map(str::to_string).collect()
    }

    /// C++: `popen("uv run rag_tokenizer.py ... <text>")` — the single-text
    /// mode prints exactly one line to stdout.
    fn python_tokenize_text(args: &[&str]) -> String {
        let output = Command::new("uv")
            .current_dir(std::env::temp_dir())
            .arg("run")
            .arg(tokenizer_script())
            .args(args)
            .output()
            .expect("run uv for the Python tokenizer");
        assert!(
            output.status.success(),
            "Call Python tokenizer failed: {}",
            String::from_utf8_lossy(&output.stderr)
        );
        String::from_utf8_lossy(&output.stdout)
            .trim_end()
            .to_string()
    }

    fn assert_tokens_equal(actual: &[String], expected: &[String], context: &str) {
        assert_eq!(
            actual.len(),
            expected.len(),
            "token count mismatch {context}: Rust {actual:?}, Python {expected:?}"
        );
        for (index, (left, right)) in actual.iter().zip(expected).enumerate() {
            assert_eq!(
                left, right,
                "MISMATCH at {index} {context}: Rust='{left}' Python='{right}'"
            );
        }
    }

    /// C++: `test_analyze_enable_position` — the reference only prints the
    /// stream; the assertion is that the analyzer loaded and analysis runs.
    #[test]
    fn test_analyze_enable_position() {
        let mut analyzer = load_analyzer();
        analyzer.set_enable_position(true);
        analyzer.set_fine_grained(false);
        for line in input_lines() {
            let terms = analyzer.analyze(&line);
            assert!(
                terms.iter().all(|term| !term.text.is_empty()),
                "empty term for input: {line}"
            );
        }
    }

    /// C++: `test_analyze_enable_position_fine_grained`.
    #[test]
    fn test_analyze_enable_position_fine_grained() {
        let mut analyzer = load_analyzer();
        analyzer.set_enable_position(true);
        analyzer.set_fine_grained(true);
        for line in input_lines() {
            let terms = analyzer.analyze(&line);
            assert!(
                terms.iter().all(|term| !term.text.is_empty()),
                "empty term for input: {line}"
            );
        }
    }

    /// C++: `test_tokenize_consistency_with_posiiton`.
    #[test]
    fn test_tokenize_consistency_with_posiiton() {
        let analyzer = load_analyzer();
        for line in input_lines() {
            let tokens_str = analyzer.tokenize(&line);
            let coarse: Vec<&str> = tokens_str.split_whitespace().collect();
            let (with_position, _positions) = analyzer.tokenize_with_position(&line);
            assert_eq!(
                coarse, with_position,
                "Tokenize and TokenizeWithPosition disagree for input: {line}"
            );
        }
    }

    /// C++: `test_tokenize_consistency_with_python`.
    #[test]
    fn test_tokenize_consistency_with_python() {
        let analyzer = load_analyzer();
        let python_lines = python_tokenizer_lines(&[], "basic");
        let lines = input_lines();
        assert_eq!(python_lines.len(), lines.len(), "Python output line count");
        for (line, python_tokens) in lines.iter().zip(&python_lines) {
            let tokens = analyzer.tokenize(line);
            let actual: Vec<String> = tokens.split_whitespace().map(str::to_string).collect();
            let expected: Vec<String> = python_tokens
                .split_whitespace()
                .map(str::to_string)
                .collect();
            assert_tokens_equal(&actual, &expected, &format!("for input: {line}"));
        }
    }

    /// C++: `test_fine_grained_tokenize_consistency_with_python`.
    #[test]
    fn test_fine_grained_tokenize_consistency_with_python() {
        let mut analyzer = load_analyzer();
        let python_lines = python_tokenizer_lines(&["--fine-grained"], "fine");
        analyzer.set_enable_position(false);
        analyzer.set_fine_grained(true);

        let lines = input_lines();
        assert_eq!(python_lines.len(), lines.len(), "Python output line count");
        for (line, python_tokens) in lines.iter().zip(&python_lines) {
            let coarse = analyzer.tokenize(line);
            let fine = analyzer.fine_grained_tokenize(&coarse);
            let expected: Vec<String> = python_tokens
                .split_whitespace()
                .map(str::to_string)
                .collect();
            assert_tokens_equal(&fine, &expected, &format!("for input: {line}"));
        }
    }

    /// C++: `test_set_language_dutch`.
    #[test]
    fn test_set_language_dutch() {
        let mut analyzer = load_analyzer();
        let python_result = python_tokenize_text(&["-l", "dutch", "huizen"]);
        analyzer.set_language("dutch");
        let cxx_result = analyzer.tokenize("huizen");
        assert!(
            cxx_result.contains("huiz"),
            "Dutch 'huizen' must stem to 'huiz', got '{cxx_result}'"
        );
        assert_eq!(cxx_result, python_result);
    }

    /// C++: `test_set_language_slovak`.
    #[test]
    fn test_set_language_slovak() {
        let mut analyzer = load_analyzer();
        let input = "škola daňové priznanie";
        let python_result = python_tokenize_text(&["-l", "slovak", input]);
        analyzer.set_language("slovak");
        let cxx_result = analyzer.tokenize(input);
        assert!(cxx_result.contains("skola"), "got '{cxx_result}'");
        assert!(cxx_result.contains("danove"), "got '{cxx_result}'");
        assert_eq!(cxx_result, python_result);
    }

    /// C++: `test_set_language_czech`.
    #[test]
    fn test_set_language_czech() {
        let mut analyzer = load_analyzer();
        let input = "příliš žluťoučký kůň";
        let python_result = python_tokenize_text(&["-l", "czech", input]);
        analyzer.set_language("czech");
        let cxx_result = analyzer.tokenize(input);
        assert!(cxx_result.contains("prilis"), "got '{cxx_result}'");
        assert!(cxx_result.contains("zlutoucky"), "got '{cxx_result}'");
        assert!(cxx_result.contains("kun"), "got '{cxx_result}'");
        assert_eq!(cxx_result, python_result);
    }

    /// C++: `test_slovak_no_stemming`.
    #[test]
    fn test_slovak_no_stemming() {
        let mut analyzer = load_analyzer();
        analyzer.set_language("slovak");
        assert_eq!(analyzer.tokenize("skoly running"), "skoly running");

        analyzer.set_language("english");
        assert_eq!(analyzer.tokenize("running"), "run");
    }

    /// C++: `test_diacritics_folding_table`.
    #[test]
    fn test_diacritics_folding_table() {
        assert_eq!(
            RAGAnalyzer::fold_diacritics("áäčďéíĺľňóôŕšťúýž"),
            "aacdeillnoorstuyz"
        );
        assert_eq!(
            RAGAnalyzer::fold_diacritics("ÁÄČĎÉÍĹĽŇÓÔŔŠŤÚÝŽ"),
            "AACDEILLNOORSTUYZ"
        );
        assert_eq!(RAGAnalyzer::fold_diacritics("ěřůý ĚŘŮÝ"), "eruy ERUY");
        assert_eq!(RAGAnalyzer::fold_diacritics("Škola Daňové"), "Skola Danove");
        assert_eq!(RAGAnalyzer::fold_diacritics("æœßøłđÆŒØŁĐ"), "æœßøłđÆŒØŁĐ");
        assert_eq!(RAGAnalyzer::fold_diacritics("ḍṣṭḥ ḌỊ"), "ḍṣṭḥ ḌỊ");
        assert_eq!(RAGAnalyzer::fold_diacritics("한국어"), "한국어");
        assert_eq!(
            RAGAnalyzer::fold_diacritics("s\u{030C}kola"),
            "s\u{030C}kola"
        );
        assert_eq!(
            RAGAnalyzer::fold_diacritics("hello world 123"),
            "hello world 123"
        );
        assert_eq!(RAGAnalyzer::fold_diacritics("中文测试"), "中文测试");
    }

    /// C++: `test_out_of_range_diacritics_parity_with_python`.
    #[test]
    fn test_out_of_range_diacritics_parity_with_python() {
        let input = "škola ḍamage 한국어 ṣṭḥ";
        let python_result = python_tokenize_text(&["-l", "slovak", input]);
        let mut analyzer = load_analyzer();
        analyzer.set_language("slovak");
        let cxx_result = analyzer.tokenize(input);

        assert!(cxx_result.contains("skola"), "got '{cxx_result}'");
        assert!(cxx_result.contains("한국어"), "got '{cxx_result}'");
        assert!(cxx_result.contains("ṣṭḥ"), "got '{cxx_result}'");
        assert_eq!(cxx_result, python_result);
    }

    /// C++: `test_slovak_tokenize_with_position` — positions are byte offsets
    /// in the original input even though folding shrinks the letters.
    #[test]
    fn test_slovak_tokenize_with_position() {
        let mut analyzer = load_analyzer();
        analyzer.set_language("slovak");
        let (tokens, positions) = analyzer.tokenize_with_position("škola daňové");
        assert_eq!(tokens, vec!["skola".to_string(), "danove".to_string()]);
        assert_eq!(positions.len(), 2);
        assert_eq!(positions[0], (0, 6));
        assert_eq!(positions[1], (7, 15));
    }

    /// C++: `test_chinese_stopword_filter`.
    #[test]
    fn test_chinese_stopword_filter() {
        assert!(RAGAnalyzer::is_stopword("的"));
        assert!(RAGAnalyzer::is_stopword("是"));
        assert!(RAGAnalyzer::is_stopword("了"));
        assert!(RAGAnalyzer::is_stopword("什么"));
        assert!(RAGAnalyzer::is_stopword("怎么"));
        assert!(!RAGAnalyzer::is_stopword("贡献"));
        assert!(!RAGAnalyzer::is_stopword("最大"));
        assert!(!RAGAnalyzer::is_stopword("谁"));

        let query = "谁的贡献最大";

        let mut analyzer = load_analyzer();
        analyzer.set_enable_position(false);
        analyzer.set_fine_grained(false);
        let terms = analyzer.analyze(query);
        assert!(
            terms.iter().all(|term| term.text != "的"),
            "Analyzer must drop 的 from token stream: {:?}",
            terms
        );

        analyzer.set_enable_position(true);
        let terms = analyzer.analyze(query);
        assert!(
            terms.iter().all(|term| term.text != "的"),
            "Position-enabled path must also drop 的: {:?}",
            terms
        );

        analyzer.set_enable_position(false);
        let raw = analyzer.tokenize(query);
        assert!(
            raw.contains("的"),
            "Raw Tokenize() must still emit 的 (filter is only at AnalyzeImpl boundary), got '{raw}'"
        );
    }
}
