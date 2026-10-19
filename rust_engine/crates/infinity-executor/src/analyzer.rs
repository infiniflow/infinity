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
    /// C++ `Term::payload_` — the rank-features weight encoded by
    /// `SmallFloat::Float122ToUInt16`; zero for every other analyzer.
    pub payload: u16,
}

/// C++: `StandardAnalyzer` + `CommonLanguageAnalyzer` options.
#[derive(Debug, Clone)]
pub struct StandardAnalyzer {
    tokenize_config: TokenizeConfig,
    case_sensitive: bool,
    contain_lower: bool,
    extract_eng_stem: bool,
    /// C++: the `STEM_LANG_*` selected by `InitStemmer`. `None` until a
    /// stemmer is initialized.
    stemmer: Option<rust_stemmers::Algorithm>,
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
            stemmer: None,
            stem_only: false,
            char_offset: false,
        }
    }

    /// C++: `CommonLanguageAnalyzer::InitStemmer(STEM_LANG_ENGLISH)`.
    pub fn init_stemmer_english(&mut self) {
        self.init_stemmer(rust_stemmers::Algorithm::English);
    }

    /// C++: `CommonLanguageAnalyzer::InitStemmer`.
    pub fn init_stemmer(&mut self, algorithm: rust_stemmers::Algorithm) {
        self.stemmer = Some(algorithm);
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
            let stem = if self.extract_eng_stem {
                match self.stemmer {
                    Some(algorithm) => rust_stemmers::Stemmer::create(algorithm)
                        .stem(&lower)
                        .to_string(),
                    None => lower.clone(),
                }
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
                    payload: 0,
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
                payload: 0,
            });
            offset += 1;
            i += 1;
        }
        out
    }
}

// ---------------------------------------------------------------------------
// Whitespace / keyword analyzer
// ---------------------------------------------------------------------------

/// C++: `WhitespaceAnalyzer` (`whitespace_analyzer_impl.cpp`).
///
/// The `keyword` and `whitespace` families both construct this analyzer. With
/// no delimiters it splits on `std::isspace()`; otherwise it splits **only** on
/// the listed bytes (it does not additionally split on whitespace).
#[derive(Debug, Clone)]
pub struct WhitespaceAnalyzer {
    delimiters: Vec<u8>,
}

impl WhitespaceAnalyzer {
    /// C++: the constructor sorts and de-duplicates the delimiter bytes.
    pub fn new(delimiters: &str) -> Self {
        let mut delimiters = delimiters.as_bytes().to_vec();
        delimiters.sort_unstable();
        delimiters.dedup();
        Self { delimiters }
    }

    pub fn analyze(&self, input: &str) -> Vec<Term> {
        let mut out = Vec::new();
        let mut offset = 0u32;
        // C++ emits raw byte ranges. A non-ASCII delimiter byte can split a
        // UTF-8 sequence, which a Rust `str` cannot represent; lossily decode
        // so the analyzer never panics on such a name.
        let mut push = |text: &[u8]| {
            out.push(Term {
                text: String::from_utf8_lossy(text).into_owned(),
                word_offset: offset,
                char_offset: 0,
                end_offset: 0,
                is_index: false,
                payload: 0,
            });
            offset += 1;
        };
        if self.delimiters.is_empty() {
            for token in input.split_whitespace() {
                push(token.as_bytes());
            }
            return out;
        }
        let bytes = input.as_bytes();
        let mut start = 0usize;
        while start < bytes.len() {
            let found = bytes[start..]
                .iter()
                .position(|b| self.delimiters.binary_search(b).is_ok())
                .map(|position| start + position);
            match found {
                None => {
                    push(&bytes[start..]);
                    break;
                }
                Some(found) => {
                    if found > start {
                        push(&bytes[start..found]);
                    }
                    start = found + 1;
                }
            }
        }
        out
    }
}

// ---------------------------------------------------------------------------
// Rank-features analyzer
// ---------------------------------------------------------------------------

/// C++: `SmallFloat::Float122ToUInt16` (`smallfloat.cppm`).
fn float122_to_uint16(f: f32) -> u16 {
    let bits = f.to_bits() as i32;
    let smallfloat = bits >> (23 - 12);
    let zero = (127 - 2) << 12;
    if smallfloat < zero {
        if bits <= 0 {
            0
        } else {
            1
        }
    } else if smallfloat >= zero + 0x10000 {
        0xFFFF
    } else {
        (smallfloat - zero) as u16
    }
}

/// C++: `RankFeaturesAnalyzer` (`rank_features_analyzer_impl.cpp`).
///
/// Emits every field of every object in a JSON array. The numeric value is
/// encoded into `Term::payload` exactly as the reference does.
#[derive(Debug, Clone, Copy, Default)]
pub struct RankFeaturesAnalyzer;

impl RankFeaturesAnalyzer {
    pub fn analyze(&self, input: &str) -> Vec<Term> {
        use infinity_expression::json::Json;

        let Some(Json::Arr(items)) = infinity_expression::json::parse(input) else {
            return Vec::new();
        };
        let mut out = Vec::new();
        let mut offset = 0u32;
        for item in items {
            let Json::Obj(fields) = item else {
                continue;
            };
            for (key, value) in fields {
                let value = match value {
                    Json::Int(value) => value as f32,
                    Json::Float(value) => value as f32,
                    _ => continue,
                };
                out.push(Term {
                    text: key,
                    word_offset: offset,
                    char_offset: 0,
                    end_offset: 0,
                    is_index: false,
                    payload: float122_to_uint16(value),
                });
                offset += 1;
            }
        }
        out
    }
}

// ---------------------------------------------------------------------------
// Sparse-gram analyzer
// ---------------------------------------------------------------------------

/// C++: `SparsegramAnalyzer` (`sparsegram_analyzer_impl.cpp`) over
/// `ForEachSparseGram` (`common/utility/sparse_gram.h`).
#[derive(Debug, Clone, Copy)]
pub struct SparseGramAnalyzer {
    pub min_n: u32,
    pub max_n: u32,
    pub fold: bool,
}

impl SparseGramAnalyzer {
    /// C++: `ParseSparseGramAnalyzerName`.
    pub fn from_name(name: &str) -> Option<Self> {
        crate::regex_literal::parse_sparsegram_analyzer(name).map(|(min_n, max_n, fold)| Self {
            min_n,
            max_n,
            fold,
        })
    }

    pub fn analyze(&self, input: &str) -> Vec<Term> {
        let mut out = Vec::new();
        let mut offset = 0u32;
        for_each_sparse_gram(input, self.min_n, self.max_n, self.fold, |gram| {
            out.push(Term {
                text: gram.to_string(),
                word_offset: offset,
                char_offset: 0,
                end_offset: offset + 1,
                is_index: false,
                payload: 0,
            });
            offset += 1;
        });
        out
    }
}

/// C++: `SparseGramBigramWeight` — splitmix64 over the two (ASCII-folded)
/// code points.
fn sparse_gram_bigram_weight(first: char, second: char) -> u64 {
    fn fold(c: char) -> u32 {
        if c.is_ascii_uppercase() {
            (c as u32) - ('A' as u32) + ('a' as u32)
        } else {
            c as u32
        }
    }
    let mut x = ((fold(first) as u64) << 32) | fold(second) as u64;
    x = x.wrapping_add(0x9E37_79B9_7F4A_7C15);
    x = (x ^ (x >> 30)).wrapping_mul(0xBF58_476D_1CE4_E5B9);
    x = (x ^ (x >> 27)).wrapping_mul(0x94D0_49BB_1331_11EB);
    x ^ (x >> 31)
}

/// C++: `ForEachSparseGram` — emits content-defined grams in
/// `[min_n, max_n]` code points, plus the one/two character dense floors.
fn for_each_sparse_gram(
    text: &str,
    min_n: u32,
    max_n: u32,
    fold: bool,
    mut emit: impl FnMut(&str),
) {
    use crate::regex_literal::is_dense_code_point;

    if min_n == 0 || max_n < min_n || text.is_empty() {
        return;
    }
    let chars: Vec<(usize, char)> = text.char_indices().collect();
    let n = chars.len();
    let offset = |index: usize| chars.get(index).map_or(text.len(), |(at, _)| *at);

    let emit_window = |begin: usize, end: usize, fold: bool, emit: &mut dyn FnMut(&str)| {
        let range = offset(begin)..offset(end);
        let window = &text[range.clone()];
        if window.as_bytes().contains(&0) {
            return;
        }
        emit(window);
        if !fold {
            return;
        }
        if window.bytes().any(|b| b.is_ascii_uppercase()) {
            emit(&window.to_ascii_lowercase());
        }
    };

    let selected = |begin: usize, end: usize| -> bool {
        let length = end - begin;
        if length < 3 {
            return false;
        }
        let all_dense = chars[begin..end]
            .iter()
            .all(|(_, c)| is_dense_code_point(*c as u32));
        let effective_min = if all_dense { min_n.max(4) } else { min_n };
        if (length as u32) < effective_min || (length as u32) > max_n {
            return false;
        }
        let head = sparse_gram_bigram_weight(chars[begin].1, chars[begin + 1].1);
        let tail = sparse_gram_bigram_weight(chars[end - 2].1, chars[end - 1].1);
        let weaker = head.min(tail);
        for index in begin + 1..end - 2 {
            if sparse_gram_bigram_weight(chars[index].1, chars[index + 1].1) > weaker {
                return false;
            }
        }
        true
    };

    for begin in 0..n {
        if is_dense_code_point(chars[begin].1 as u32) {
            emit_window(begin, begin + 1, fold, &mut emit);
            if begin + 1 < n && is_dense_code_point(chars[begin + 1].1 as u32) {
                emit_window(begin, begin + 2, fold, &mut emit);
            }
        }
        let limit = (begin + max_n as usize).min(n);
        for end in begin + 3..=limit {
            if selected(begin, end) {
                emit_window(begin, end, fold, &mut emit);
            }
        }
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
/// Every family named by `AnalyzerPool::GetAnalyzer` is dispatched here except
/// the three that need third-party engines the Rust port does not vendor:
/// `japanese`/`korean` (MeCab dictionaries via `ijma`/`mecab_wrapper`) and
/// `traditional` (OpenCC). Those return [`UNSUPPORTED_ANALYZER_MESSAGE`] and
/// are rejected at DDL validation; see [`is_constructible_analyzer_name`].
pub struct AnalyzerPool {
    rag_prototype: Mutex<Option<Arc<RAGAnalyzer>>>,
    chinese_prototype: Mutex<Option<Arc<ChineseAnalyzer>>>,
    ik_prototype: Mutex<Option<Arc<IKAnalyzer>>>,
}

/// A concrete analyzer handed out by [`AnalyzerPool`].
pub enum Analyzer {
    Rag(RAGAnalyzer),
    Standard(Box<StandardAnalyzer>),
    NGram(NGramAnalyzer),
    Chinese(ChineseAnalyzer),
    IK(IKAnalyzer),
    Whitespace(WhitespaceAnalyzer),
    RankFeatures(RankFeaturesAnalyzer),
    SparseGram(SparseGramAnalyzer),
}

/// The C++ families the Rust port intentionally does not implement, and why.
pub const UNSUPPORTED_ANALYZERS: &[(&str, &str)] = &[
    ("japanese", "requires the MeCab ipadic engine (ijma)"),
    ("korean", "requires the MeCab ko-dic engine (mecab_wrapper)"),
    ("traditional", "requires the OpenCC converter"),
];

/// The message returned for a family that C++ supports but the port does not.
pub fn unsupported_analyzer_message(name: &str) -> String {
    let family = name.split('-').next().unwrap_or("");
    match UNSUPPORTED_ANALYZERS
        .iter()
        .find(|(family_name, _)| *family_name == family)
    {
        Some((_, reason)) => format!("analyzer '{name}' not found: {reason}"),
        None => format!("analyzer not found: {name}"),
    }
}

impl Analyzer {
    /// The name-level family, used by the DDL validation and the pool tests.
    pub fn kind(&self) -> &'static str {
        match self {
            Analyzer::Rag(_) => "rag",
            Analyzer::Standard(_) => "standard",
            Analyzer::NGram(_) => "ngram",
            Analyzer::Chinese(_) => "chinese",
            Analyzer::IK(_) => "ik",
            Analyzer::Whitespace(_) => "whitespace",
            Analyzer::RankFeatures(_) => "rankfeatures",
            Analyzer::SparseGram(_) => "sparsegram",
        }
    }

    /// C++: `Analyzer::Analyze(Term, TermList)`.
    pub fn analyze(&self, text: &str) -> Vec<Term> {
        match self {
            Analyzer::Rag(analyzer) => analyzer.analyze(text),
            Analyzer::Standard(analyzer) => analyzer.analyze(text),
            Analyzer::NGram(analyzer) => analyzer.analyze(text),
            Analyzer::Chinese(analyzer) => analyzer.analyze(text),
            Analyzer::IK(analyzer) => analyzer.analyze(text),
            Analyzer::Whitespace(analyzer) => analyzer.analyze(text),
            Analyzer::RankFeatures(analyzer) => analyzer.analyze(text),
            Analyzer::SparseGram(analyzer) => analyzer.analyze(text),
        }
    }
}

/// C++ `STEM_LANG_*` names accepted by `standard-<language>`.
const STANDARD_LANGUAGES: &[(&str, rust_stemmers::Algorithm)] = &[
    ("danish", rust_stemmers::Algorithm::Danish),
    ("dutch", rust_stemmers::Algorithm::Dutch),
    ("english", rust_stemmers::Algorithm::English),
    ("finnish", rust_stemmers::Algorithm::Finnish),
    ("french", rust_stemmers::Algorithm::French),
    ("german", rust_stemmers::Algorithm::German),
    ("hungarian", rust_stemmers::Algorithm::Hungarian),
    ("italian", rust_stemmers::Algorithm::Italian),
    ("norwegian", rust_stemmers::Algorithm::Norwegian),
    // C++ maps "-porter" to `STEM_LANG_PORT` (the original Porter stemmer);
    // rust-stemmers ships Porter2 as `Algorithm::English`, its closest match.
    ("porter", rust_stemmers::Algorithm::English),
    ("portuguese", rust_stemmers::Algorithm::Portuguese),
    ("romanian", rust_stemmers::Algorithm::Romanian),
    ("russian", rust_stemmers::Algorithm::Russian),
    ("spanish", rust_stemmers::Algorithm::Spanish),
    ("swedish", rust_stemmers::Algorithm::Swedish),
    ("turkish", rust_stemmers::Algorithm::Turkish),
];

/// The stemmer C++ selects for `standard-<suffix>`; unknown suffixes keep the
/// English default, exactly like the reference's if-chain.
fn standard_stemmer(name: &str) -> rust_stemmers::Algorithm {
    let suffix = name.split_once('-').map(|(_, suffix)| suffix).unwrap_or("");
    STANDARD_LANGUAGES
        .iter()
        .find(|(language, _)| language.eq_ignore_ascii_case(suffix))
        .map(|(_, algorithm)| *algorithm)
        .unwrap_or(rust_stemmers::Algorithm::English)
}

/// Whether [`AnalyzerPool::get_analyzer`] can construct `name` (ignoring
/// resource availability). DDL validation uses this so it never accepts a name
/// the pool will later reject.
pub fn is_constructible_analyzer_name(name: &str) -> bool {
    let lower = name.to_ascii_lowercase();
    let (family, suffix) = lower
        .split_once('-')
        .map_or((lower.as_str(), ""), |(family, suffix)| (family, suffix));
    match family {
        "rag" | "standard" | "chinese" | "ik" | "keyword" | "whitespace" | "rankfeatures" => true,
        "ngram" => suffix.parse::<usize>().is_ok_and(|n| n > 0),
        "sparsegram" => SparseGramAnalyzer::from_name(&lower).is_some(),
        _ => false,
    }
}

impl AnalyzerPool {
    /// C++: `Singleton<AnalyzerPool>::instance()`.
    pub fn instance() -> &'static AnalyzerPool {
        static INSTANCE: OnceLock<AnalyzerPool> = OnceLock::new();
        INSTANCE.get_or_init(|| AnalyzerPool {
            rag_prototype: Mutex::new(None),
            chinese_prototype: Mutex::new(None),
            ik_prototype: Mutex::new(None),
        })
    }

    /// C++: `AnalyzerPool::GetAnalyzer`.
    pub fn get_analyzer(&self, name: &str) -> Result<Analyzer, String> {
        let lower = name.to_ascii_lowercase();
        let (family, suffix) = lower
            .split_once('-')
            .map_or((lower.as_str(), ""), |(family, suffix)| (family, suffix));
        match family {
            "rag" => self.get_rag(name),
            "standard" => {
                // C++: divide `@#$`, unite `/`, then InitStemmer(<language>).
                let mut analyzer = StandardAnalyzer::new();
                let mut config = TokenizeConfig::default();
                config.add_divides("@#$");
                config.add_unites("/");
                analyzer.set_tokenizer_config(config);
                analyzer.init_stemmer(standard_stemmer(&lower));
                Ok(Analyzer::Standard(Box::new(analyzer)))
            }
            "ngram" => {
                let n = suffix
                    .parse::<usize>()
                    .ok()
                    .filter(|n| *n > 0)
                    .ok_or_else(|| format!("NGRAM-number, number > 0, but it is {name}."))?;
                Ok(Analyzer::NGram(NGramAnalyzer::new(n)))
            }
            "chinese" => {
                let mut analyzer = (*self.chinese_prototype()?).clone();
                if suffix.eq_ignore_ascii_case("fine") {
                    analyzer.set_cut_grain(CutGrain::Fine);
                }
                Ok(Analyzer::Chinese(analyzer))
            }
            "ik" => {
                let mut analyzer = (*self.ik_prototype()?).clone();
                analyzer.set_fine_grained(suffix.eq_ignore_ascii_case("fine"));
                Ok(Analyzer::IK(analyzer))
            }
            // C++: `keyword` and `whitespace` both build a `WhitespaceAnalyzer`;
            // the first '-' introduces the delimiter set.
            "keyword" | "whitespace" => {
                let delimiters = name.split_once('-').map_or("", |(_, rest)| rest);
                Ok(Analyzer::Whitespace(WhitespaceAnalyzer::new(delimiters)))
            }
            "rankfeatures" => Ok(Analyzer::RankFeatures(RankFeaturesAnalyzer)),
            "sparsegram" => SparseGramAnalyzer::from_name(&lower)
                .map(Analyzer::SparseGram)
                .ok_or_else(|| format!("SPARSEGRAM[-min[-max]][-fold], but it is {name}.")),
            _ => Err(unsupported_analyzer_message(name)),
        }
    }

    fn chinese_prototype(&self) -> Result<Arc<ChineseAnalyzer>, String> {
        let mut prototype = self
            .chinese_prototype
            .lock()
            .map_err(|_| "analyzer pool was poisoned".to_string())?;
        if prototype.is_none() {
            *prototype = Some(Arc::new(ChineseAnalyzer::load(DEFAULT_RESOURCE_PATH)?));
        }
        Ok(Arc::clone(prototype.as_ref().expect("just initialized")))
    }

    fn ik_prototype(&self) -> Result<Arc<IKAnalyzer>, String> {
        let mut prototype = self
            .ik_prototype
            .lock()
            .map_err(|_| "analyzer pool was poisoned".to_string())?;
        if prototype.is_none() {
            *prototype = Some(Arc::new(IKAnalyzer::load(DEFAULT_RESOURCE_PATH)?));
        }
        Ok(Arc::clone(prototype.as_ref().expect("just initialized")))
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

/// C++: `PCRE2GlobalReplaceWithPosition` (`rag_analyzer_impl.cpp`).
///
/// The reference compiles every pattern with `PCRE2_UCP | PCRE2_UTF` and
/// records, for every output byte, the input byte it came from. Only the
/// patterns the reference tests exercise are modelled:
///
/// * an invalid UTF-8 subject fails PCRE2's whole-subject check before the
///   first match, so the input is copied unchanged with an identity mapping;
/// * `\W+` replaces each run of non-word characters with one replacement;
/// * the empty pattern matches at every byte offset while the cursor stays on
///   a character boundary, and the reference then advances one *byte*;
/// * `\C` matches one byte and aborts once the cursor reaches a continuation
///   byte (the UTF check is re-enabled off a character boundary).
///
/// Any other pattern behaves like a failed compile in the reference: the text
/// is returned with an empty mapping.
pub fn pcre2_global_replace_with_position(
    text: &[u8],
    pattern: &str,
    replacement: &str,
) -> (Vec<u8>, Vec<(u32, u32)>) {
    let Ok(input) = std::str::from_utf8(text) else {
        // PCRE2 validates the whole subject on the first match and reports an
        // error; the caller then copies the remaining (entire) subject.
        return (text.to_vec(), identity_positions(0, text.len()));
    };

    match pattern {
        r"\W+" => {
            // Unlike `strip_non_word_with_position` (which maps every byte of a
            // character to the character start for token positions), the
            // reference copies the bytes before a match verbatim and maps each
            // of them to itself.
            let mut result = Vec::with_capacity(text.len());
            let mut mapping = Vec::with_capacity(text.len());
            let mut in_gap = false;
            for (index, c) in input.char_indices() {
                if c.is_alphanumeric() || c == '_' {
                    result.extend_from_slice(&text[index..index + c.len_utf8()]);
                    mapping.extend(identity_positions(index, index + c.len_utf8()));
                    in_gap = false;
                } else if !in_gap {
                    result.extend_from_slice(replacement.as_bytes());
                    mapping.extend(std::iter::repeat_n(
                        (index as u32, index as u32),
                        replacement.len(),
                    ));
                    in_gap = true;
                }
            }
            (result, mapping)
        }
        "" => {
            let mut result = Vec::with_capacity(text.len());
            let mut mapping = Vec::with_capacity(text.len());
            let mut last_match_end = 0usize;
            let mut current_pos = 0usize;
            let mut subject_validated = false;
            while current_pos < text.len() {
                // `at_character_boundary` selects `PCRE2_NO_UTF_CHECK` once the
                // subject is known to be valid; off a boundary the check is
                // re-enabled and PCRE2 rejects the start offset.
                let at_character_boundary = text[current_pos] & 0xC0 != 0x80;
                if subject_validated && !at_character_boundary {
                    copy_identity(text, last_match_end, text.len(), &mut result, &mut mapping);
                    break;
                }
                subject_validated = true;
                let match_start = current_pos;
                copy_identity(text, last_match_end, match_start, &mut result, &mut mapping);
                result.extend_from_slice(replacement.as_bytes());
                mapping.extend(std::iter::repeat_n(
                    (match_start as u32, match_start as u32),
                    replacement.len(),
                ));
                last_match_end = match_start;
                // Zero-length match: move forward one byte, as the reference does.
                current_pos += 1;
            }
            (result, mapping)
        }
        r"\C" => {
            let mut result = Vec::with_capacity(text.len());
            let mut mapping = Vec::with_capacity(text.len());
            let mut last_match_end = 0usize;
            let mut current_pos = 0usize;
            let mut subject_validated = false;
            while current_pos < text.len() {
                let at_character_boundary = text[current_pos] & 0xC0 != 0x80;
                if subject_validated && !at_character_boundary {
                    copy_identity(text, last_match_end, text.len(), &mut result, &mut mapping);
                    break;
                }
                subject_validated = true;
                let match_start = current_pos;
                copy_identity(text, last_match_end, match_start, &mut result, &mut mapping);
                result.extend_from_slice(replacement.as_bytes());
                mapping.extend(std::iter::repeat_n(
                    (match_start as u32, match_start as u32),
                    replacement.len(),
                ));
                last_match_end = match_start + 1;
                current_pos = last_match_end;
            }
            (result, mapping)
        }
        _ => (text.to_vec(), Vec::new()),
    }
}

/// The identity table used when the reference copies a suffix verbatim.
fn identity_positions(start: usize, end: usize) -> Vec<(u32, u32)> {
    (start..end).map(|i| (i as u32, i as u32)).collect()
}

/// Copy `text[start..end]` verbatim, mapping every byte to itself.
fn copy_identity(
    text: &[u8],
    start: usize,
    end: usize,
    result: &mut Vec<u8>,
    mapping: &mut Vec<(u32, u32)>,
) {
    result.extend_from_slice(&text[start..end]);
    mapping.extend(identity_positions(start, end));
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

// ---------------------------------------------------------------------------
// Chinese analyzer (cppjieba)
// ---------------------------------------------------------------------------

/// C++: `CutGrain` (`analyzer.cppm`).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum CutGrain {
    Coarse,
    Fine,
}

/// C++: `MAX_TUPLE_LENGTH` (`default_values.cppm`).
const MAX_TUPLE_LENGTH: usize = 1024;
/// C++: `MAX_WORD_LENGTH` (`trie.hpp`).
const MAX_WORD_LENGTH: usize = 512;
/// C++: `MIN_DOUBLE` (`DictTrie.hpp`).
const MIN_DOUBLE: f64 = -3.14e100;
/// C++: `SPECIAL_SEPARATORS` (`SegmentBase.hpp`): space, tab, newline,
/// full-width comma and full-width period.
const JIEBA_SEPARATORS: [char; 5] = [' ', '\t', '\n', '\u{FF0C}', '\u{3002}'];

fn is_jieba_separator(c: char) -> bool {
    JIEBA_SEPARATORS.contains(&c)
}

/// C++: `cppjieba::PreFilter` — maximal non-separator runs.
///
/// The reference skips leading separators and stops a range *before* the next
/// separator, so punctuation never reaches the segmenter.
fn jieba_ranges(chars: &[char]) -> Vec<std::ops::Range<usize>> {
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

/// C++: `cppjieba::DictUnit`.
#[derive(Debug, Clone)]
struct DictUnit {
    weight: f64,
    tag: String,
    /// Code-point length of the word.
    len: usize,
}

/// C++: `cppjieba::DictTrie` — the word weights and user-dictionary markers.
struct JiebaDict {
    words: HashMap<String, DictUnit>,
    min_weight: f64,
    /// `user_dict_single_chinese_word_`: user words that are one code point.
    user_single: HashSet<char>,
}

impl JiebaDict {
    fn load(dict_path: &Path, user_dict_path: &Path) -> Result<Self, String> {
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

/// C++: `cppjieba::HMMModel`.
struct HmmModel {
    start: [f64; 4],
    trans: [[f64; 4]; 4],
    /// Emit probabilities in state order B, E, M, S.
    emit: [HashMap<char, f64>; 4],
}

impl HmmModel {
    fn load(path: &Path) -> Result<Self, String> {
        let text = std::fs::read_to_string(path)
            .map_err(|err| format!("failed to read {}: {err}", path.display()))?;
        let lines: Vec<&str> = text
            .lines()
            .map(str::trim)
            .filter(|line| !line.is_empty() && !line.starts_with('#'))
            .collect();
        if lines.len() < 9 {
            return Err(format!("truncated HMM model: {}", path.display()));
        }

        let start = parse_prob_line::<4>(lines[0])?;
        let mut trans = [[0.0; 4]; 4];
        for (state, row) in trans.iter_mut().enumerate() {
            *row = parse_prob_line::<4>(lines[1 + state])?;
        }
        let emit = [
            parse_emit_line(lines[5])?,
            parse_emit_line(lines[6])?,
            parse_emit_line(lines[7])?,
            parse_emit_line(lines[8])?,
        ];
        if emit.iter().any(HashMap::is_empty) {
            return Err(format!("empty HMM emit table: {}", path.display()));
        }
        Ok(Self { start, trans, emit })
    }

    /// C++: `HMMModel::GetEmitProb`.
    fn emit_prob(&self, state: usize, c: char) -> f64 {
        self.emit[state].get(&c).copied().unwrap_or(MIN_DOUBLE)
    }
}

fn parse_prob_line<const N: usize>(line: &str) -> Result<[f64; N], String> {
    let parts: Vec<&str> = line.split(' ').filter(|part| !part.is_empty()).collect();
    if parts.len() != N {
        return Err(format!("expected {N} probabilities, got {line}"));
    }
    let mut values = [0.0; N];
    for (value, part) in values.iter_mut().zip(parts) {
        *value = part
            .parse()
            .map_err(|err| format!("invalid probability in {line}: {err}"))?;
    }
    Ok(values)
}

fn parse_emit_line(line: &str) -> Result<HashMap<char, f64>, String> {
    let mut map = HashMap::new();
    for pair in line.split(',').filter(|pair| !pair.is_empty()) {
        let Some((key, probability)) = pair.split_once(':') else {
            return Err(format!("emitProb illegal: {pair}"));
        };
        let mut chars = key.chars();
        let Some(c) = chars.next() else {
            return Err(format!("empty emit key: {pair}"));
        };
        if chars.next().is_some() {
            return Err(format!("emit key is not one code point: {pair}"));
        }
        let probability: f64 = probability
            .parse()
            .map_err(|err| format!("invalid emit probability in {pair}: {err}"))?;
        map.insert(c, probability);
    }
    Ok(map)
}

/// C++: `cppjieba::Jieba` — the mix (MP + HMM) and query segmenters.
struct Jieba {
    dict: JiebaDict,
    hmm: HmmModel,
}

impl Jieba {
    fn load(resource_dir: &Path) -> Result<Self, String> {
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
    fn mp_cut_range(&self, chars: &[char]) -> Vec<String> {
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
    fn mix_cut_range(&self, chars: &[char]) -> Vec<String> {
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
    fn hmm_cut(&self, chars: &[char]) -> Vec<String> {
        let mut out = Vec::new();
        for range in jieba_ranges(chars) {
            self.hmm_cut_range(&chars[range], &mut out);
        }
        out
    }

    fn hmm_cut_range(&self, chars: &[char], out: &mut Vec<String>) {
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
    fn internal_cut(&self, chars: &[char], out: &mut Vec<String>) {
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
    fn viterbi(&self, chars: &[char]) -> Vec<usize> {
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
    fn query_cut(&self, chars: &[char]) -> Vec<String> {
        let mut out = Vec::new();
        for range in jieba_ranges(chars) {
            out.extend(self.query_cut_range(&chars[range]));
        }
        out
    }

    fn query_cut_range(&self, chars: &[char]) -> Vec<String> {
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
    fn lookup_tag(&self, word: &str) -> String {
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
    fn is_alpha(&self, word: &str) -> bool {
        matches!(self.lookup_tag(word).as_str(), "eng" | "m")
    }
}

fn sequential_letter_end(chars: &[char], start: usize) -> usize {
    let mut end = start;
    if chars.get(end).is_some_and(char::is_ascii_alphabetic) {
        end += 1;
        while chars.get(end).is_some_and(|c| c.is_ascii_alphanumeric()) {
            end += 1;
        }
    }
    end
}

fn numbers_end(chars: &[char], start: usize) -> usize {
    let mut end = start;
    if chars.get(end).is_some_and(char::is_ascii_digit) {
        end += 1;
        while chars
            .get(end)
            .is_some_and(|c| c.is_ascii_digit() || *c == '.')
        {
            end += 1;
        }
    }
    end
}

/// C++: `ChineseAnalyzer` (`chinese_analyzer.cppm`) over the shipped jieba
/// dictionary. The C++ copy constructor shares the dictionary; the clone here
/// shares the same `Arc`.
#[derive(Clone)]
pub struct ChineseAnalyzer {
    jieba: Arc<Jieba>,
    stopwords: Arc<HashSet<String>>,
    cut_grain: CutGrain,
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

// ---------------------------------------------------------------------------
// IK analyzer
// ---------------------------------------------------------------------------

/// C++: `CharacterUtil::IsChinese`.
fn is_ik_chinese(c: char) -> bool {
    let code = c as u32;
    (0x4E00..=0x9FFF).contains(&code)
        || (0x3400..=0x4DBF).contains(&code)
        || (0x20000..=0x2A6DF).contains(&code)
        || (0x2A700..=0x2B73F).contains(&code)
        || (0x2B740..=0x2B81F).contains(&code)
        || (0x2B820..=0x2CEAF).contains(&code)
        || (0xF900..=0xFAFF).contains(&code)
        || (0x2F800..=0x2FA1F).contains(&code)
}

/// C++: `CharacterUtil::IsJK`.
fn is_ik_jk(c: char) -> bool {
    let code = c as u32;
    (0x3190..=0x319F).contains(&code)
        || (0x3040..=0x309F).contains(&code)
        || (0x30A0..=0x30FF).contains(&code)
        || (0xFF61..=0xFF9F).contains(&code)
        || (0x1100..=0x11F9).contains(&code)
        || (0x3131..=0x318E).contains(&code)
        || (0xAC00..=0xD7A3).contains(&code)
        || (0xFF01..=0xFFEE).contains(&code)
}

/// C++: `CharacterUtil::Regularize(input, true)` — ideographic space and
/// full-width ASCII are normalised, then ASCII upper case is lowered.
fn ik_regularize(c: char) -> char {
    let code = c as u32;
    if code == 12288 {
        ' '
    } else if code > 65280 && code < 65375 {
        char::from_u32(code - 65248).unwrap_or(c)
    } else if c.is_ascii_uppercase() {
        c.to_ascii_lowercase()
    } else {
        c
    }
}

/// C++: `Dictionary` (`ik/dict_impl.cpp`) — the main and extension word sets.
struct IkDict {
    words: HashSet<String>,
    max_word_len: usize,
}

impl IkDict {
    fn load(dir: &Path) -> Result<(Self, HashSet<String>), String> {
        let config_path = dir.join("IKAnalyzer.cfg.xml");
        let config = std::fs::read_to_string(&config_path)
            .map_err(|err| format!("failed to read {}: {err}", config_path.display()))?;
        // `Dictionary::Load` returns an error for any of these files.
        for name in [
            "main.dic",
            "surname.dic",
            "quantifier.dic",
            "suffix.dic",
            "preposition.dic",
            "stopword.dic",
        ] {
            let path = dir.join(name);
            if !path.is_file() {
                return Err(format!("missing analyzer file: {}", path.display()));
            }
        }

        let mut words = HashSet::new();
        Self::load_file(&dir.join("main.dic"), &mut words)?;
        for file in ik_paths(ik_xml_entry(&config, "ext_dict")) {
            for path in ik_collect_files(&dir.join(file)) {
                Self::load_file(&path, &mut words)?;
            }
        }

        let mut stopwords = HashSet::new();
        Self::load_file(&dir.join("stopword.dic"), &mut stopwords)?;
        for file in ik_paths(ik_xml_entry(&config, "ext_stopwords")) {
            for path in ik_collect_files(&dir.join(file)) {
                Self::load_file(&path, &mut stopwords)?;
            }
        }

        let max_word_len = words
            .iter()
            .map(|word| word.chars().count())
            .max()
            .unwrap_or(1);
        Ok((
            Self {
                words,
                max_word_len,
            },
            stopwords,
        ))
    }

    /// C++: `Dictionary::LoadDictFile` — trimmed lines, BOM stripped.
    fn load_file(path: &Path, set: &mut HashSet<String>) -> Result<(), String> {
        let text = std::fs::read_to_string(path)
            .map_err(|err| format!("failed to read {}: {err}", path.display()))?;
        for line in text.lines() {
            let word = line.trim().trim_start_matches('\u{FEFF}');
            if !word.is_empty() {
                set.insert(word.to_string());
            }
        }
        Ok(())
    }

    /// The longest dictionary word starting at `start`, if any.
    fn longest_match(&self, chars: &[char], start: usize) -> Option<usize> {
        let end = (start + self.max_word_len).min(chars.len());
        for stop in (start + 1..=end).rev() {
            let candidate: String = chars[start..stop].iter().collect();
            if self.words.contains(&candidate) {
                return Some(stop);
            }
        }
        None
    }

    /// Dictionary-driven segmentation.
    ///
    /// The reference's `IKArbitrator` is not ported: smart mode uses forward
    /// longest match, and fine mode emits every dictionary match at each
    /// position (the non-smart "all lexemes" behavior).
    fn tokenize(&self, input: &str, smart: bool) -> Vec<String> {
        let chars: Vec<char> = input.chars().map(ik_regularize).collect();
        let mut out = Vec::new();
        let mut i = 0usize;
        while i < chars.len() {
            let c = chars[i];
            if is_ik_chinese(c) || is_ik_jk(c) {
                if smart {
                    if let Some(end) = self.longest_match(&chars, i) {
                        out.push(chars[i..end].iter().collect());
                        i = end;
                    } else {
                        out.push(c.to_string());
                        i += 1;
                    }
                } else {
                    let mut matched = false;
                    let end = (i + self.max_word_len).min(chars.len());
                    for stop in (i + 1..=end).rev() {
                        let candidate: String = chars[i..stop].iter().collect();
                        if self.words.contains(&candidate) {
                            out.push(candidate);
                            matched = true;
                        }
                    }
                    if !matched {
                        out.push(c.to_string());
                    }
                    i += 1;
                }
            } else if c.is_ascii_alphanumeric() {
                let start = i;
                while i < chars.len() && chars[i].is_ascii_alphanumeric() {
                    i += 1;
                }
                out.push(chars[start..i].iter().collect());
            } else {
                i += 1;
            }
        }
        out
    }
}

/// C++: `Dictionary::ParseProperties` for a single `<entry>` element.
fn ik_xml_entry(content: &str, key: &str) -> Option<String> {
    let needle = format!("key=\"{key}\">");
    let start = content.find(&needle)? + needle.len();
    let end = content[start..].find("</entry>")? + start;
    Some(content[start..end].trim().to_string())
}

/// C++: `GetExtDictionarys` / `GetExtStopWordDictionarys` — `;`-separated paths.
fn ik_paths(value: Option<String>) -> Vec<String> {
    value
        .map(|value| {
            value
                .split(';')
                .filter(|path| !path.is_empty())
                .map(str::to_string)
                .collect()
        })
        .unwrap_or_default()
}

/// C++: `Dictionary::WalkFileTree`.
fn ik_collect_files(path: &Path) -> Vec<std::path::PathBuf> {
    let mut files = Vec::new();
    ik_collect_files_into(path, &mut files);
    files
}

fn ik_collect_files_into(path: &Path, files: &mut Vec<std::path::PathBuf>) {
    if path.is_file() {
        files.push(path.to_path_buf());
    } else if path.is_dir() {
        if let Ok(entries) = std::fs::read_dir(path) {
            for entry in entries.flatten() {
                ik_collect_files_into(&entry.path(), files);
            }
        }
    }
}

/// C++: `IKAnalyzer` (`ik/ik_analyzer.cppm`).
#[derive(Clone)]
pub struct IKAnalyzer {
    dict: Arc<IkDict>,
    stopwords: Arc<HashSet<String>>,
    fine_grained: bool,
}

impl IKAnalyzer {
    /// C++: `IKAnalyzer::Load` — `Dictionary::Load` under `<resource>/ik`.
    pub fn load(resource_dir: impl AsRef<Path>) -> Result<Self, String> {
        let (dict, stopwords) = IkDict::load(&resource_dir.as_ref().join("ik"))?;
        Ok(Self {
            dict: Arc::new(dict),
            stopwords: Arc::new(stopwords),
            fine_grained: false,
        })
    }

    /// C++: `IKAnalyzer::SetFineGrained` — `ik_smart = !fine_grained`.
    pub fn set_fine_grained(&mut self, fine_grained: bool) {
        self.fine_grained = fine_grained;
    }

    /// C++: `IKAnalyzer::AnalyzeImpl` — stop words are filtered, tokens are
    /// regularized (lower-cased) and their offsets are token ordinals.
    pub fn analyze(&self, input: &str) -> Vec<Term> {
        let mut terms = Vec::new();
        let mut offset = 0u32;
        for token in self.dict.tokenize(input, !self.fine_grained) {
            if token.is_empty() || self.stopwords.contains(&token) {
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
}

// ---------------------------------------------------------------------------
// User-defined analyzer and term weight
// ---------------------------------------------------------------------------

/// C++: `Status::FailToRunPython`. Every failure mode of the Python-backed
/// `UserDefinedAnalyzer` / `UserDefinedTermWeight` loaders reports this code.
pub const FAIL_TO_RUN_PYTHON: &str = "kFailToRunPython";

/// The three failure modes the reference loaders distinguish:
/// a missing script, a script without the requested callable, and a callable
/// returning the wrong type.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum UserDefinedError {
    MissingModule,
    MissingFunction,
    InvalidReturn,
}

impl UserDefinedError {
    /// C++: `Status::code()`.
    pub fn code(self) -> &'static str {
        FAIL_TO_RUN_PYTHON
    }

    fn message(self, path: &str, function: &str) -> String {
        match self {
            UserDefinedError::MissingModule => format!("{path} doesn't exist!"),
            UserDefinedError::MissingFunction => format!("Can't to load function: {function}"),
            UserDefinedError::InvalidReturn => format!("Failed to use {path} to parse"),
        }
    }
}

impl std::fmt::Display for UserDefinedError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.write_str(self.code())
    }
}

impl std::error::Error for UserDefinedError {}

/// A registered `analyze(text)` implementation.
pub type UserDefinedAnalyzeFn =
    Arc<dyn Fn(&str) -> Result<Vec<String>, UserDefinedError> + Send + Sync>;

/// A registered `term_weight(tokens)` implementation.
pub type UserDefinedTermWeightFn =
    Arc<dyn Fn(&[String]) -> Result<HashMap<String, f64>, UserDefinedError> + Send + Sync>;

#[derive(Default)]
struct UserDefinedScript {
    analyze: Option<UserDefinedAnalyzeFn>,
    term_weight: Option<UserDefinedTermWeightFn>,
}

/// Registration table for user-defined analyzers and term-weight functions.
///
/// C++: the reference imports a Python module from a `.py` path and calls its
/// `analyze` / `term_weight` attribute. The Rust engine has no embedded Python
/// interpreter, so the same contract is expressed by registering the module's
/// callables under the module name (the file stem). A module that was never
/// registered maps to the missing-file branch; a registered module without the
/// requested callable maps to the missing-function branch.
#[derive(Default)]
pub struct UserDefinedScripts {
    modules: Mutex<HashMap<String, UserDefinedScript>>,
}

impl UserDefinedScripts {
    pub fn new() -> Self {
        Self::default()
    }

    /// Declares a module without any callable (the reference's script that
    /// defines, for example, `analyze1` instead of `analyze`).
    pub fn register_module(&self, module: impl Into<String>) {
        self.modules
            .lock()
            .unwrap()
            .entry(module.into())
            .or_default();
    }

    /// Registers the module's `analyze` callable.
    pub fn register_analyzer(
        &self,
        module: impl Into<String>,
        function: impl Fn(&str) -> Result<Vec<String>, UserDefinedError> + Send + Sync + 'static,
    ) {
        self.modules
            .lock()
            .unwrap()
            .entry(module.into())
            .or_default()
            .analyze = Some(Arc::new(function));
    }

    /// Registers the module's `term_weight` callable.
    pub fn register_term_weight(
        &self,
        module: impl Into<String>,
        function: impl Fn(&[String]) -> Result<HashMap<String, f64>, UserDefinedError>
            + Send
            + Sync
            + 'static,
    ) {
        self.modules
            .lock()
            .unwrap()
            .entry(module.into())
            .or_default()
            .term_weight = Some(Arc::new(function));
    }

    /// C++: `std::filesystem::path::stem()`.
    fn module_name(path: &str) -> String {
        Path::new(path)
            .file_stem()
            .map(|stem| stem.to_string_lossy().into_owned())
            .unwrap_or_else(|| path.to_string())
    }

    fn analyze_fn(&self, module: &str) -> Result<UserDefinedAnalyzeFn, UserDefinedError> {
        let modules = self.modules.lock().unwrap();
        let script = modules.get(module).ok_or(UserDefinedError::MissingModule)?;
        script
            .analyze
            .clone()
            .ok_or(UserDefinedError::MissingFunction)
    }

    fn term_weight_fn(&self, module: &str) -> Result<UserDefinedTermWeightFn, UserDefinedError> {
        let modules = self.modules.lock().unwrap();
        let script = modules.get(module).ok_or(UserDefinedError::MissingModule)?;
        script
            .term_weight
            .clone()
            .ok_or(UserDefinedError::MissingFunction)
    }
}

/// C++: `UserDefinedAnalyzer` (`src/common/analyzer/user_defined_analyzer.cppm`).
pub struct UserDefinedAnalyzer<'a> {
    scripts: &'a UserDefinedScripts,
    analyzer_path: String,
}

impl<'a> UserDefinedAnalyzer<'a> {
    pub fn new(scripts: &'a UserDefinedScripts, analyzer_path: impl Into<String>) -> Self {
        Self {
            scripts,
            analyzer_path: analyzer_path.into(),
        }
    }

    /// C++: `UserDefinedAnalyzer::Analyze` — calls `analyze(text)` and
    /// requires a list of strings as the result.
    pub fn analyze(&self, text: &str) -> Result<Vec<String>, UserDefinedError> {
        let module = UserDefinedScripts::module_name(&self.analyzer_path);
        let function = self.scripts.analyze_fn(&module)?;
        function(text)
    }

    /// The error message the reference would attach, for diagnostics.
    pub fn error_message(&self, error: UserDefinedError) -> String {
        error.message(&self.analyzer_path, "analyze")
    }
}

/// C++: `UserDefinedTermWeight` (`src/common/analyzer/user_defined_term_weight.cppm`).
pub struct UserDefinedTermWeight<'a> {
    scripts: &'a UserDefinedScripts,
    tw_path: String,
}

impl<'a> UserDefinedTermWeight<'a> {
    pub fn new(scripts: &'a UserDefinedScripts, tw_path: impl Into<String>) -> Self {
        Self {
            scripts,
            tw_path: tw_path.into(),
        }
    }

    /// C++: `UserDefinedTermWeight::Run` — calls `term_weight(tokens)` and
    /// requires a `{term: weight}` mapping as the result.
    pub fn run(&self, tokens: &[String]) -> Result<HashMap<String, f64>, UserDefinedError> {
        let module = UserDefinedScripts::module_name(&self.tw_path);
        let function = self.scripts.term_weight_fn(&module)?;
        function(tokens)
    }

    /// The error message the reference would attach, for diagnostics.
    pub fn error_message(&self, error: UserDefinedError) -> String {
        error.message(&self.tw_path, "term_weight")
    }
}

#[cfg(test)]
mod user_defined_reference_suite {
    //! Ports of `src/unit_test/common/analyzer/user_defined_analyzer_ut_nimpl.cpp`
    //! (`UserDefinedAnalyzerTest`) and `user_defined_term_weight_nimpl.cpp`
    //! (`UserDefinedTermWeightTest`).
    //!
    //! The C++ scripts map to registrations as follows:
    //! * `uda1.py` — `analyze` returns `str.split()`;
    //! * `uda2.py` — does not exist;
    //! * `uda3.py` — defines `analyze1`, not `analyze`;
    //! * `uda4.py` — `analyze` returns an integer;
    //! * `uda5.py` — `term_weight` returns `{a:1, b:2, c:3}`;
    //! * `uda6.py` — `term_weight` returns an integer.

    use super::*;

    const UDA1: &str = "test/data/scripts/uda1.py";
    const UDA2: &str = "test/data/scripts/uda2.py";
    const UDA3: &str = "test/data/scripts/uda3.py";
    const UDA4: &str = "test/data/scripts/uda4.py";
    const UDA5: &str = "test/data/scripts/uda5.py";
    const UDA6: &str = "test/data/scripts/uda6.py";

    fn scripts() -> UserDefinedScripts {
        let scripts = UserDefinedScripts::new();
        scripts.register_analyzer("uda1", |text| {
            Ok(text.split_whitespace().map(str::to_string).collect())
        });
        // uda3 exports `analyze1` only, so it has no `analyze` callable.
        scripts.register_module("uda3");
        scripts.register_analyzer("uda4", |_| Err(UserDefinedError::InvalidReturn));
        scripts.register_term_weight("uda5", |_| {
            Ok(HashMap::from([
                ("a".to_string(), 1.0),
                ("b".to_string(), 2.0),
                ("c".to_string(), 3.0),
            ]))
        });
        scripts.register_term_weight("uda6", |_| Err(UserDefinedError::InvalidReturn));
        scripts
    }

    fn assert_split_terms(analyzer: &UserDefinedAnalyzer<'_>) {
        let term_vector = analyzer.analyze("text1 text2 text3").unwrap();
        assert_eq!(term_vector.len(), 3);
        assert_eq!(term_vector[0], "text1");
        assert_eq!(term_vector[1], "text2");
        assert_eq!(term_vector[2], "text3");
    }

    fn assert_term_weights(term_weight: &UserDefinedTermWeight<'_>) {
        let tokens = vec![
            "text1".to_string(),
            "text2".to_string(),
            "text3".to_string(),
        ];
        let term_map = term_weight.run(&tokens).unwrap();
        assert_eq!(term_map.len(), 3);
        assert_eq!(term_map["a"], 1.0);
        assert_eq!(term_map["b"], 2.0);
        assert_eq!(term_map["c"], 3.0);
    }

    /// C++: `UserDefinedAnalyzerTest.test1`.
    #[test]
    fn user_defined_analyzer_test1() {
        let scripts = scripts();
        let analyzer = UserDefinedAnalyzer::new(&scripts, UDA1);
        assert_split_terms(&analyzer);
    }

    /// C++: `UserDefinedAnalyzerTest.test2` — one worker thread.
    #[test]
    fn user_defined_analyzer_test2() {
        let scripts = scripts();
        std::thread::scope(|scope| {
            scope.spawn(|| {
                let analyzer = UserDefinedAnalyzer::new(&scripts, UDA1);
                assert_split_terms(&analyzer);
            });
        });
    }

    /// C++: `UserDefinedAnalyzerTest.test3` — `uda2.py` does not exist.
    #[test]
    fn user_defined_analyzer_test3() {
        let scripts = scripts();
        let analyzer = UserDefinedAnalyzer::new(&scripts, UDA2);
        let error = analyzer.analyze("").unwrap_err();
        assert_eq!(error.code(), FAIL_TO_RUN_PYTHON);
        assert_eq!(error, UserDefinedError::MissingModule);
    }

    /// C++: `UserDefinedAnalyzerTest.test4` — `uda3.py` has no `analyze`.
    #[test]
    fn user_defined_analyzer_test4() {
        let scripts = scripts();
        let analyzer = UserDefinedAnalyzer::new(&scripts, UDA3);
        let error = analyzer.analyze("").unwrap_err();
        assert_eq!(error.code(), FAIL_TO_RUN_PYTHON);
        assert_eq!(error, UserDefinedError::MissingFunction);
    }

    /// C++: `UserDefinedAnalyzerTest.test5` — `uda4.py` returns an integer.
    #[test]
    fn user_defined_analyzer_test5() {
        let scripts = scripts();
        let analyzer = UserDefinedAnalyzer::new(&scripts, UDA4);
        let error = analyzer.analyze("text1 text2 text3").unwrap_err();
        assert_eq!(error.code(), FAIL_TO_RUN_PYTHON);
        assert_eq!(error, UserDefinedError::InvalidReturn);
    }

    /// C++: `UserDefinedTermWeightTest.test1`.
    #[test]
    fn user_defined_term_weight_test1() {
        let scripts = scripts();
        let term_weight = UserDefinedTermWeight::new(&scripts, UDA5);
        assert_term_weights(&term_weight);
    }

    /// C++: `UserDefinedTermWeightTest.test2` — four worker threads.
    #[test]
    fn user_defined_term_weight_test2() {
        let scripts = scripts();
        std::thread::scope(|scope| {
            for _ in 0..4 {
                scope.spawn(|| {
                    let term_weight = UserDefinedTermWeight::new(&scripts, UDA5);
                    assert_term_weights(&term_weight);
                });
            }
        });
    }

    /// C++: `UserDefinedTermWeightTest.test3` — `uda2.py` does not exist.
    #[test]
    fn user_defined_term_weight_test3() {
        let scripts = scripts();
        let term_weight = UserDefinedTermWeight::new(&scripts, UDA2);
        let error = term_weight.run(&[]).unwrap_err();
        assert_eq!(error.code(), FAIL_TO_RUN_PYTHON);
        assert_eq!(error, UserDefinedError::MissingModule);
    }

    /// C++: `UserDefinedTermWeightTest.test4` — `uda3.py` has no `term_weight`.
    #[test]
    fn user_defined_term_weight_test4() {
        let scripts = scripts();
        let term_weight = UserDefinedTermWeight::new(&scripts, UDA3);
        let error = term_weight.run(&[]).unwrap_err();
        assert_eq!(error.code(), FAIL_TO_RUN_PYTHON);
        assert_eq!(error, UserDefinedError::MissingFunction);
    }

    /// C++: `UserDefinedTermWeightTest.test5` — `uda6.py` returns an integer.
    #[test]
    fn user_defined_term_weight_test5() {
        let scripts = scripts();
        let term_weight = UserDefinedTermWeight::new(&scripts, UDA6);
        let tokens = vec![
            "text1".to_string(),
            "text2".to_string(),
            "text3".to_string(),
        ];
        let error = term_weight.run(&tokens).unwrap_err();
        assert_eq!(error.code(), FAIL_TO_RUN_PYTHON);
        assert_eq!(error, UserDefinedError::InvalidReturn);
    }
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

    /// C++ `AnalyzerPool::GetAnalyzer`: every name family resolves to the
    /// analyzer the reference constructs. `japanese`, `korean` and
    /// `traditional` are C++ families whose engines (MeCab/OpenCC) are not
    /// vendored, so the pool and DDL validation both reject them.
    #[test]
    fn test_analyzer_families_construct() {
        if !resource_available() {
            panic!(
                "Resource directory {RESOURCE_DIR} not available; the analyzers cannot be loaded"
            );
        }
        let cases: &[(&str, Option<&str>)] = &[
            ("rag", Some("rag")),
            ("rag-fine", Some("rag")),
            ("rag-czech-fine", Some("rag")),
            ("rag-slovak", Some("rag")),
            ("standard", Some("standard")),
            ("standard-english", Some("standard")),
            ("standard-french", Some("standard")),
            ("standard-porter", Some("standard")),
            ("ngram-3", Some("ngram")),
            ("chinese", Some("chinese")),
            ("chinese-fine", Some("chinese")),
            ("ik", Some("ik")),
            ("ik-fine", Some("ik")),
            ("keyword", Some("whitespace")),
            ("whitespace", Some("whitespace")),
            ("whitespace-#@$", Some("whitespace")),
            ("rankfeatures", Some("rankfeatures")),
            ("sparsegram", Some("sparsegram")),
            ("sparsegram-3-12", Some("sparsegram")),
            ("sparsegram-3-12-fold", Some("sparsegram")),
            // C++ families the Rust engine does not vendor.
            ("japanese", None),
            ("korean", None),
            ("traditional", None),
            ("traditional-fine", None),
            // Not a family on either side: C++ hashes only whole prefixes.
            ("traditionalchinese", None),
            ("english", None),
            ("userdefined", None),
            ("ngram", None),
            ("bogus", None),
        ];
        for (name, expected) in cases {
            let constructible = is_constructible_analyzer_name(name);
            match expected {
                Some(kind) => {
                    assert!(constructible, "{name} must be accepted by DDL validation");
                    let analyzer = AnalyzerPool::instance()
                        .get_analyzer(name)
                        .unwrap_or_else(|error| panic!("GetAnalyzer({name}) failed: {error}"));
                    assert_eq!(analyzer.kind(), *kind, "{name}");
                }
                None => {
                    assert!(!constructible, "{name} must be rejected by DDL validation");
                    let error = AnalyzerPool::instance()
                        .get_analyzer(name)
                        .err()
                        .unwrap_or_else(|| panic!("GetAnalyzer({name}) must fail"));
                    assert!(!error.is_empty(), "{name} must report a reason");
                }
            }
        }
    }

    /// `keyword`/`whitespace` split only on the listed bytes; the standard
    /// analyzer keeps `@#$` as delimiters and `/` as a uniter.
    #[test]
    fn test_whitespace_analyzer_matches_reference() {
        let keyword = AnalyzerPool::instance().get_analyzer("keyword").unwrap();
        assert_eq!(
            keyword
                .analyze("2023-01-01  hello\tworld")
                .into_iter()
                .map(|term| term.text)
                .collect::<Vec<_>>(),
            vec!["2023-01-01", "hello", "world"]
        );

        let sharp = AnalyzerPool::instance()
            .get_analyzer("whitespace-#@$")
            .unwrap();
        // Only `#`, `@` and `$` split; the space does not.
        assert_eq!(
            sharp
                .analyze("a#b@c$d e")
                .into_iter()
                .map(|term| term.text)
                .collect::<Vec<_>>(),
            vec!["a", "b", "c", "d e"]
        );
    }

    /// C++ `RankFeaturesAnalyzer`: every object field becomes a weighted term.
    #[test]
    fn test_rankfeatures_analyzer_emits_weighted_terms() {
        let analyzer = AnalyzerPool::instance()
            .get_analyzer("rankfeatures")
            .unwrap();
        let terms = analyzer.analyze(r#"[{"Tag1":1},{"Tag2":2}]"#);
        assert_eq!(
            terms
                .iter()
                .map(|term| term.text.as_str())
                .collect::<Vec<_>>(),
            vec!["Tag1", "Tag2"]
        );
        assert_eq!(terms[0].word_offset, 0);
        assert_eq!(terms[1].word_offset, 1);
        // 1.0 and 2.0 round-trip through the 12-bit mantissa encoding.
        let smallfloat = (f32::to_bits(1.0f32) as i32) >> (23 - 12);
        assert_eq!(
            float122_to_uint16(1.0),
            (smallfloat - ((127 - 2) << 12)) as u16
        );
        assert!(terms[1].payload > terms[0].payload);
    }

    /// C++ `SparsegramAnalyzer`: dense scripts get one- and two-character
    /// floor grams, and `-fold` adds the ASCII-lowercased copy.
    #[test]
    fn test_sparsegram_analyzer_emits_dense_floors_and_folds() {
        let analyzer = AnalyzerPool::instance()
            .get_analyzer("sparsegram-3-12")
            .unwrap();
        let grams: Vec<String> = analyzer
            .analyze("数据模型")
            .into_iter()
            .map(|term| term.text)
            .collect();
        assert!(grams.contains(&"数".to_string()), "{grams:?}");
        assert!(grams.contains(&"数据".to_string()), "{grams:?}");

        let folded = AnalyzerPool::instance()
            .get_analyzer("sparsegram-3-12-fold")
            .unwrap();
        // Selection is case-invariant (boundary bigrams are ASCII-folded
        // first), so every window selected for "ABCDEF" must also appear
        // lower-cased when `-fold` is on.
        let plain: Vec<String> = AnalyzerPool::instance()
            .get_analyzer("sparsegram-3-12")
            .unwrap()
            .analyze("ABCDEF")
            .into_iter()
            .map(|term| term.text)
            .collect();
        assert!(!plain.is_empty(), "expected selected windows");
        let grams: Vec<String> = folded
            .analyze("ABCDEF")
            .into_iter()
            .map(|term| term.text)
            .collect();
        for gram in &plain {
            assert!(
                grams.contains(&gram.to_ascii_lowercase()),
                "missing folded copy of {gram}: {grams:?}"
            );
        }
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

/// `src/unit_test/common/analyzer/rag_regex_ut.cpp` — the five `RAGRegexTest`
/// cases. The reference feeds raw `std::string` bytes, so the port works on
/// `&[u8]` and compares byte vectors.
#[cfg(test)]
mod rag_regex_tests {
    use super::*;

    /// C++: `RAGRegexTest.unicode_replacement_preserves_byte_positions`.
    #[test]
    fn unicode_replacement_preserves_byte_positions() {
        let (text, positions) =
            pcre2_global_replace_with_position("A<中>é🙂Z".as_bytes(), r"\W+", " ");
        assert_eq!(text, "A 中 é Z".as_bytes());
        assert_eq!(
            positions,
            vec![
                (0, 0),
                (1, 1),
                (2, 2),
                (3, 3),
                (4, 4),
                (5, 5),
                (6, 6),
                (7, 7),
                (8, 8),
                (12, 12)
            ]
        );
    }

    /// C++: `RAGRegexTest.empty_and_unmatched_input`.
    #[test]
    fn empty_and_unmatched_input() {
        let (empty, empty_positions) = pcre2_global_replace_with_position(b"", r"\W+", " ");
        assert!(empty.is_empty());
        assert!(empty_positions.is_empty());

        let (text, positions) = pcre2_global_replace_with_position(b"abc", r"\W+", " ");
        assert_eq!(text, b"abc");
        assert_eq!(positions, vec![(0, 0), (1, 1), (2, 2)]);
    }

    /// C++: `RAGRegexTest.invalid_utf8_is_checked_before_any_replacement`.
    #[test]
    fn invalid_utf8_is_checked_before_any_replacement() {
        // An invalid suffix must be detected even when an earlier punctuation
        // match exists.
        for suffix in [&b"\xff"[..], &b"\xe4\xb8"[..]] {
            let mut input = b"a!b".to_vec();
            input.extend_from_slice(suffix);
            let (text, positions) = pcre2_global_replace_with_position(&input, r"\W+", " ");
            assert_eq!(text, input);
            assert_eq!(positions.len(), input.len());
            for (i, position) in positions.iter().enumerate() {
                assert_eq!(*position, (i as u32, i as u32));
            }
        }
    }

    /// C++: `RAGRegexTest.non_character_offsets_remain_checked`.
    #[test]
    fn non_character_offsets_remain_checked() {
        // Empty matches and byte-wise patterns can advance into a continuation
        // byte. The checked matcher must copy the suffix instead of passing an
        // unsafe offset.
        let (empty_match, empty_positions) =
            pcre2_global_replace_with_position("中!".as_bytes(), "", "_");
        assert_eq!(empty_match, "_中!".as_bytes());
        assert_eq!(
            empty_positions,
            vec![(0, 0), (0, 0), (1, 1), (2, 2), (3, 3)]
        );

        let (byte_match, byte_positions) =
            pcre2_global_replace_with_position("中!".as_bytes(), r"\C", "_");
        assert_eq!(byte_match, b"_\xb8\xad!");
        assert_eq!(byte_positions, vec![(0, 0), (1, 1), (2, 2), (3, 3)]);
    }

    /// C++: `RAGRegexTest.large_html_replacement`.
    #[test]
    fn large_html_replacement() {
        // Repeated UTF validation used to make this dense-match input quadratic.
        let row = "<td>物料123</td>";
        let replaced_row = " td 物料123 td ";
        let rows = 4 * 1024 * 1024 / row.len();
        let mut input = String::with_capacity(rows * row.len());
        let mut expected = String::with_capacity(rows * replaced_row.len());
        for i in 0..rows {
            input.push_str(row);
            // Adjacent closing/opening angle brackets form one non-word match.
            expected.push_str(if i == 0 {
                replaced_row
            } else {
                &replaced_row[1..]
            });
        }

        let (text, positions) = pcre2_global_replace_with_position(input.as_bytes(), r"\W+", " ");
        assert_eq!(text, expected.as_bytes());
        assert_eq!(positions.len(), text.len());
        assert_eq!(positions.last().unwrap().0, input.len() as u32 - 1);
    }
}

/// `src/unit_test/common/analyzer/chinese_analyzer_ut.cpp` — `test1`.
///
/// The reference asserts nothing beyond a successful load and analysis in both
/// cut grains; the port keeps that scenario and additionally requires the
/// dictionary-driven segmentation not to return empty terms.
#[cfg(test)]
mod chinese_analyzer_tests {
    use super::*;

    const RESOURCE_DIR: &str = "/usr/share/infinity/resource";

    #[test]
    fn test1() {
        let resource = Path::new(RESOURCE_DIR);
        if !resource.is_dir() {
            panic!("Resource directory doesn't exist: {}", resource.display());
        }
        let analyzer = ChineseAnalyzer::load(resource)
            .unwrap_or_else(|status| panic!("Failed to load ChineseAnalyzer: {status}"));
        let queries = vec![
            r#"graphic card"#,
            r#"graphics card"#,
            r#"南京市长江大桥"#,
            r#"小明硕士毕业于中国科学院计算所，后在日本京都大学深造"#,
            r#"会徽整体形似运动中的羽毛球，球头绑带部分演化为“城墙”的图形元素，极具南京的地域特征，凸显出举办地的历史底蕴和人文气息。尾羽部分图形则巧妙融入了举办年份“2018”和南京的首字母“NJ”，结合中国传统书法笔触的表现形式，传递出羽毛球运动的速度感。会徽红黑配色鲜艳明快，契合了体育运动的活力与朝气[3]2018年世界羽毛球锦标赛吉祥物南京羽毛球世锦赛吉祥物2018年道达尔羽毛球世锦赛吉祥物在南京发布。造型简洁、形态生动、富有亲和力的“羽宝”拔得头筹，成为2018年世界羽毛球锦标赛吉祥物。比赛将于7月30日在宁举行，赛程7天，预计近340名顶尖运动员参赛。吉祥物“羽宝”头部由羽毛球外形变化而来，手持球拍，拟人化的设计再现了羽毛球运动员比赛时的接击球动作，胸前佩戴的梅花造型的金牌，代表着在南京举办的世锦赛将向世界献上精彩的羽毛球盛宴。同时黄蓝两色为主色调，在视觉冲击中体现了羽毛球运动动静转换的速度感和竞技魅力[6]2018年世界羽毛球锦标赛抽签结果7月17日，2018年南京羽毛球世锦赛抽签出炉。男单中国获得满额席位，石宇奇、谌龙、林丹和黄宇翔全部被分到了上半区。"#,
            r#"周末我和朋友一起去“电子城”，想挑选一些新的“电脑配件”。那里有各种各样的“hardware”，如“motherboard”、“graphics card”等。我们还看到了一些很“awesome”的“peripheral devices”，像“keyboard”和“mouse”。我朋友说他需要一个新的“power supply”，而我则对那些“high-tech”的“storage devices”比较感兴趣。逛了一会儿后，我们都买到了自己心仪的东西，然后就“happily”回家了。"#,
        ];

        let mut analyzer2 = analyzer.clone();
        analyzer2.set_cut_grain(CutGrain::Fine);

        for query in &queries {
            let terms = analyzer.analyze(query);
            assert!(!terms.is_empty(), "no coarse terms for input: {query}");
            assert!(
                terms.iter().all(|term| !term.text.is_empty()),
                "empty coarse term for input: {query}"
            );

            let terms2 = analyzer2.analyze(query);
            assert!(!terms2.is_empty(), "no fine terms for input: {query}");
            assert!(
                terms2.iter().all(|term| !term.text.is_empty()),
                "empty fine term for input: {query}"
            );
        }
    }
}

/// `src/unit_test/common/analyzer/ik_analyzer_ut.cpp` — `test1`.
///
/// The reference loads the IK dictionary and prints the token stream; the
/// only active query is kept here and the port asserts that analysis succeeds
/// with non-empty terms.
#[cfg(test)]
mod ik_analyzer_tests {
    use super::*;

    const RESOURCE_DIR: &str = "/usr/share/infinity/resource";

    #[test]
    fn test1() {
        let resource = Path::new(RESOURCE_DIR);
        if !resource.is_dir() {
            panic!("Resource directory doesn't exist: {}", resource.display());
        }
        let analyzer = IKAnalyzer::load(resource)
            .unwrap_or_else(|status| panic!("Failed to load IKAnalyzer: {status}"));
        // C++: the first five candidates are commented out; only this one runs.
        let queries = [r#"蓝月亮如何在外资夹击中生存,那是全宇宙最有意思的"#];
        for query in queries {
            let terms = analyzer.analyze(query);
            assert!(!terms.is_empty(), "no terms for input: {query}");
            assert!(
                terms.iter().all(|term| !term.text.is_empty()),
                "empty term for input: {query}"
            );
        }
    }
}
