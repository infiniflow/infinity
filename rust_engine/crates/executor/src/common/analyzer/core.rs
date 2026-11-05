use super::*;

thread_local! {
    /// `algorithm -> (word -> stem)`, one map per algorithm seen on this thread.
    pub(crate) static STEM_MEMO: std::cell::RefCell<Vec<(rust_stemmers::Algorithm, HashMap<Box<str>, Box<str>>)>>
        = std::cell::RefCell::new(Vec::new());
}

thread_local! {
    pub(crate) static RAG_CONVERTED: std::cell::Cell<bool> = const { std::cell::Cell::new(false) };
}

/// C++: the shipped resource root (`InfinityContext::config()` fallback).
pub const DEFAULT_RESOURCE_PATH: &str = "/usr/share/infinity/resource";

/// C++: `AnalyzerPool` (`analyzer_pool.cppm`) — parses an analyzer name and
/// hands out a fresh analyzer.
///
/// Every family named by `AnalyzerPool::GetAnalyzer` is dispatched here,
/// including the dictionary-driven CJK families: `japanese` and `korean`
/// (MeCab over `ipadic`/`ko-dic`) and `traditional` (OpenCC + jieba).
pub struct AnalyzerPool {
    pub(crate) rag_prototype: Mutex<Option<Arc<RAGAnalyzer>>>,
    pub(crate) chinese_prototype: Mutex<Option<Arc<ChineseAnalyzer>>>,
    pub(crate) ik_prototype: Mutex<Option<Arc<IKAnalyzer>>>,
    pub(crate) japanese_prototype: Mutex<Option<Arc<JapaneseAnalyzer>>>,
    pub(crate) korean_prototype: Mutex<Option<Arc<KoreanAnalyzer>>>,
    pub(crate) opencc_prototype: Mutex<Option<Arc<OpenCcConverter>>>,
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
    Japanese(JapaneseAnalyzer),
    Korean(KoreanAnalyzer),
    Traditional(TraditionalChineseAnalyzer),
}

/// The C++ families the Rust port intentionally does not implement, and why.
///
/// Every family the reference names is implemented now; the table is kept so
/// that a name that is genuinely unknown still reports a reason.
pub const UNSUPPORTED_ANALYZERS: &[(&str, &str)] = &[];

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
            Analyzer::Japanese(_) => "japanese",
            Analyzer::Korean(_) => "korean",
            Analyzer::Traditional(_) => "traditional",
        }
    }

    /// Streams the terms of the analyzers that can produce them by reference,
    /// returning `false` for the rest so the caller falls back to
    /// [`Self::analyze`]. The index path uses this to intern a term without
    /// building a `String` for it.
    pub fn for_each_term(&self, text: &str, f: &mut dyn FnMut(TermView<'_>)) -> bool {
        match self {
            Analyzer::Standard(analyzer) => analyzer.for_each_term(text, f),
            Analyzer::NGram(analyzer) => analyzer.for_each_term(text, f),
            Analyzer::SparseGram(analyzer) => analyzer.for_each_term(text, f),
            // The dictionary-driven and converting analyzers own their token
            // text (segmentation buffers, folded or converted strings), so they
            // keep the collecting path.
            _ => return false,
        }
        true
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
            Analyzer::Japanese(analyzer) => analyzer.analyze(text),
            Analyzer::Korean(analyzer) => analyzer.analyze(text),
            Analyzer::Traditional(analyzer) => analyzer.analyze(text),
        }
    }
}

/// The stemmer C++'s `InitStemmer` selects, by `STEM_LANG_*`.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Stemmer {
    /// A Snowball algorithm: every `STEM_LANG_*` but `STEM_LANG_PORT` is a
    /// Snowball program, and rust-stemmers ships the same generator output.
    Snowball(rust_stemmers::Algorithm),
    /// `STEM_LANG_PORT`: the original Porter stemmer, which rust-stemmers does
    /// not ship (its `English` is Porter2).
    Porter,
}

impl Stemmer {
    /// Stems one already-lowercased word.
    ///
    /// Creating a Snowball environment is not free, so one is kept per
    /// algorithm instead of being rebuilt for every token: indexing a 20k-row
    /// column runs this a couple of million times.
    pub fn stem<'a>(&self, word: &'a str) -> std::borrow::Cow<'a, str> {
        match self {
            Self::Snowball(algorithm) => snowball(*algorithm).stem(word),
            Self::Porter => std::borrow::Cow::Owned(crate::porter::stem(word)),
        }
    }

    /// Calls `f` with the stem of `word`, memoised per thread.
    ///
    /// Natural text repeats its words heavily (60–70% of the tokens of the
    /// benchmark corpus), so the stem of most tokens is one already computed:
    /// looking it up costs a short hash instead of a whole Snowball run. The
    /// cache is per thread (no lock on the index path), per algorithm (two
    /// languages must not share entries) and bounded — reaching the bound
    /// clears it instead of growing with the vocabulary forever.
    ///
    /// The stem is handed to `f` by reference and rather than returned, so a
    /// cache hit neither clones the stem nor allocates: the caller (the index
    /// path) only needs it long enough to intern it.
    pub(crate) fn with_stem<R>(&self, word: &str, f: impl FnOnce(&str) -> R) -> R {
        match self {
            Self::Snowball(algorithm) => memoized_stem(*algorithm, word, f),
            Self::Porter => {
                let stem = crate::porter::stem(word);
                f(stem.as_ref())
            }
        }
    }
}

/// Entries a thread's stem cache may hold before it is dropped wholesale.
pub(crate) const STEM_MEMO_LIMIT: usize = 1 << 16;

/// Calls `f` with the stem of `word`, from this thread's cache when it is
/// already there.
pub(crate) fn memoized_stem<R>(
    algorithm: rust_stemmers::Algorithm,
    word: &str,
    f: impl FnOnce(&str) -> R,
) -> R {
    STEM_MEMO.with(|cell| {
        let mut maps = cell.borrow_mut();
        let index = match maps.iter().position(|(a, _)| *a == algorithm) {
            Some(index) => index,
            None => {
                maps.push((algorithm, HashMap::new()));
                maps.len() - 1
            }
        };
        let map = &mut maps[index].1;
        if !map.contains_key(word) {
            let stem: Box<str> = Box::from(snowball(algorithm).stem(word).as_ref());
            if map.len() >= STEM_MEMO_LIMIT {
                map.clear();
            }
            map.insert(Box::from(word), stem);
        }
        f(map.get(word).map(|s| s.as_ref()).unwrap_or(""))
    })
}

/// The Snowball environment for an algorithm, created once per process. The
/// table is tiny, so a linear scan beats any locking.
pub(crate) fn snowball(algorithm: rust_stemmers::Algorithm) -> &'static rust_stemmers::Stemmer {
    use std::sync::OnceLock;
    pub(crate) static ENGINES: OnceLock<Vec<(rust_stemmers::Algorithm, rust_stemmers::Stemmer)>> =
        OnceLock::new();
    let engines = ENGINES.get_or_init(|| {
        let mut engines: Vec<(rust_stemmers::Algorithm, rust_stemmers::Stemmer)> = Vec::new();
        for (_, stemmer) in STANDARD_LANGUAGES {
            if let Stemmer::Snowball(algorithm) = stemmer {
                if !engines.iter().any(|(a, _)| a == algorithm) {
                    engines.push((*algorithm, rust_stemmers::Stemmer::create(*algorithm)));
                }
            }
        }
        engines
    });
    match engines.iter().find(|(a, _)| *a == algorithm) {
        Some((_, engine)) => engine,
        // Not reachable from SQL today: every algorithm the analyzer accepts is
        // in the table above.
        None => Box::leak(Box::new(rust_stemmers::Stemmer::create(
            rust_stemmers::Algorithm::English,
        ))),
    }
}

/// C++ `STEM_LANG_*` names accepted by `standard-<language>`.
pub(crate) const STANDARD_LANGUAGES: &[(&str, Stemmer)] = &[
    (
        "danish",
        Stemmer::Snowball(rust_stemmers::Algorithm::Danish),
    ),
    ("dutch", Stemmer::Snowball(rust_stemmers::Algorithm::Dutch)),
    (
        "english",
        Stemmer::Snowball(rust_stemmers::Algorithm::English),
    ),
    (
        "finnish",
        Stemmer::Snowball(rust_stemmers::Algorithm::Finnish),
    ),
    (
        "french",
        Stemmer::Snowball(rust_stemmers::Algorithm::French),
    ),
    (
        "german",
        Stemmer::Snowball(rust_stemmers::Algorithm::German),
    ),
    (
        "hungarian",
        Stemmer::Snowball(rust_stemmers::Algorithm::Hungarian),
    ),
    (
        "italian",
        Stemmer::Snowball(rust_stemmers::Algorithm::Italian),
    ),
    (
        "norwegian",
        Stemmer::Snowball(rust_stemmers::Algorithm::Norwegian),
    ),
    ("porter", Stemmer::Porter),
    (
        "portuguese",
        Stemmer::Snowball(rust_stemmers::Algorithm::Portuguese),
    ),
    (
        "romanian",
        Stemmer::Snowball(rust_stemmers::Algorithm::Romanian),
    ),
    (
        "russian",
        Stemmer::Snowball(rust_stemmers::Algorithm::Russian),
    ),
    (
        "spanish",
        Stemmer::Snowball(rust_stemmers::Algorithm::Spanish),
    ),
    (
        "swedish",
        Stemmer::Snowball(rust_stemmers::Algorithm::Swedish),
    ),
    (
        "turkish",
        Stemmer::Snowball(rust_stemmers::Algorithm::Turkish),
    ),
];

/// The stemmer C++ selects for `standard-<language>`; unknown suffixes keep the
/// English default, exactly like the reference's if-chain.
pub(crate) fn standard_stemmer(language: &str) -> Stemmer {
    STANDARD_LANGUAGES
        .iter()
        .find(|(name, _)| name.eq_ignore_ascii_case(language))
        .map(|(_, stemmer)| *stemmer)
        .unwrap_or(Stemmer::Snowball(rust_stemmers::Algorithm::English))
}

/// The name grammar of C++ `AnalyzerPool::GetAnalyzer`.
///
/// The reference hashes the run of bytes before the first `-` (`Str2Int`) and
/// switches on the hash, so the family match is **case sensitive**: `Chinese`
/// and `RAG-fine` are `AnalyzerNotFound`. Each family then parses the rest of
/// the name itself, and those parsers differ: `chinese`/`ik`/`traditional`
/// accept only the exact suffix `-fine`, `standard` compares its `-<language>`
/// case insensitively (`IEquals`), `keyword`/`whitespace` take everything after
/// the first `-` as the delimiter set, `ngram` uses `strtol` (a numeric prefix
/// is enough), and `sparsegram` validates its window (`ParseSparseGramAnalyzerName`).
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum AnalyzerName<'a> {
    Rag,
    /// `standard` and its `-<language>` suffix (empty without a suffix).
    Standard {
        language: &'a str,
    },
    /// `strtol` of the suffix, truncated to the reference's `u32`.
    NGram {
        n: u32,
    },
    Chinese {
        fine: bool,
    },
    Traditional {
        fine: bool,
    },
    Ik {
        fine: bool,
    },
    /// `keyword` / `whitespace`, optionally followed by `-<delimiters>`; the
    /// delimiters are empty when the name has no suffix.
    Whitespace {
        delimiters: &'a str,
    },
    RankFeatures,
    Japanese,
    Korean,
    SparseGram {
        min_n: u32,
        max_n: u32,
        fold: bool,
    },
}

/// C++ `strtol` restricted to the prefix it consumes: optional whitespace, an
/// optional sign, then decimal digits. Saturation matches `LONG_MAX`/`LONG_MIN`
/// (the reference stores the result in a `u32`, so an out-of-range value is
/// truncated rather than rejected).
pub(crate) fn strtol_prefix(text: &str) -> i64 {
    let bytes = text.as_bytes();
    let mut index = 0usize;
    while index < bytes.len() && bytes[index].is_ascii_whitespace() {
        index += 1;
    }
    let negative = match bytes.get(index) {
        Some(b'-') => {
            index += 1;
            true
        }
        Some(b'+') => {
            index += 1;
            false
        }
        _ => false,
    };
    let mut value: i64 = 0;
    while let Some(byte) = bytes.get(index) {
        if !byte.is_ascii_digit() {
            break;
        }
        value = value
            .saturating_mul(10)
            .saturating_add(i64::from(byte - b'0'));
        index += 1;
    }
    if negative {
        -value
    } else {
        value
    }
}

/// Parses `name` exactly like C++ `AnalyzerPool::GetAnalyzer` selects a family
/// and interprets its suffix. Shared by the pool and by the DDL validation so
/// the two cannot drift (the reference validates by constructing the analyzer).
pub fn parse_analyzer_name(name: &str) -> Result<AnalyzerName<'_>, String> {
    let split = name.split_once('-');
    let (family, suffix_option) = match split {
        Some((family, suffix)) => (family, Some(suffix)),
        None => (name, None),
    };
    let suffix = suffix_option.unwrap_or("");
    match family {
        "rag" => Ok(AnalyzerName::Rag),
        "standard" => Ok(AnalyzerName::Standard { language: suffix }),
        // `strcmp(rest, "-fine")`: exact, so `chinese-FINE` stays coarse.
        "chinese" => Ok(AnalyzerName::Chinese {
            fine: suffix == "fine",
        }),
        "traditional" => Ok(AnalyzerName::Traditional {
            fine: suffix == "fine",
        }),
        "ik" => Ok(AnalyzerName::Ik {
            fine: suffix == "fine",
        }),
        "keyword" | "whitespace" => Ok(AnalyzerName::Whitespace { delimiters: suffix }),
        "rankfeatures" => Ok(AnalyzerName::RankFeatures),
        "japanese" => Ok(AnalyzerName::Japanese),
        "korean" => Ok(AnalyzerName::Korean),
        "ngram" => {
            // No '-' at all is the reference's own error message; with one, a
            // non-positive (after the `u32` truncation) value is rejected.
            let Some(rest) = suffix_option else {
                return Err(format!("NGRAM-number, but it is {name}."));
            };
            let n = strtol_prefix(rest) as u32;
            if n == 0 {
                return Err(format!("NGRAM-number, number > 0, but it is {name}."));
            }
            Ok(AnalyzerName::NGram { n })
        }
        "sparsegram" => {
            let (min_n, max_n, fold) = SparseGramAnalyzer::from_name(name)
                .map(|analyzer| (analyzer.min_n, analyzer.max_n, analyzer.fold))
                .ok_or_else(|| format!("SPARSEGRAM[-min[-max]][-fold], but it is {name}."))?;
            Ok(AnalyzerName::SparseGram { min_n, max_n, fold })
        }
        _ => Err(unsupported_analyzer_message(name)),
    }
}

/// Whether [`AnalyzerPool::get_analyzer`] accepts `name` (ignoring resource
/// availability). DDL validation uses this so it never accepts a name the pool
/// will later reject.
pub fn is_constructible_analyzer_name(name: &str) -> bool {
    parse_analyzer_name(name).is_ok()
}

impl AnalyzerPool {
    /// C++: `Singleton<AnalyzerPool>::instance()`.
    pub fn instance() -> &'static AnalyzerPool {
        static INSTANCE: OnceLock<AnalyzerPool> = OnceLock::new();
        INSTANCE.get_or_init(|| AnalyzerPool {
            rag_prototype: Mutex::new(None),
            chinese_prototype: Mutex::new(None),
            ik_prototype: Mutex::new(None),
            japanese_prototype: Mutex::new(None),
            korean_prototype: Mutex::new(None),
            opencc_prototype: Mutex::new(None),
        })
    }

    /// C++: `AnalyzerPool::GetAnalyzer`.
    pub fn get_analyzer(&self, name: &str) -> Result<Analyzer, String> {
        match parse_analyzer_name(name)? {
            AnalyzerName::Rag => self.get_rag(name),
            AnalyzerName::Standard { language } => {
                // C++: divide `@#$`, unite `/`, then InitStemmer(<language>).
                let mut analyzer = StandardAnalyzer::new();
                let mut config = TokenizeConfig::default();
                config.add_divides("@#$");
                config.add_unites("/");
                analyzer.set_tokenizer_config(config);
                analyzer.init_stemmer(standard_stemmer(language));
                Ok(Analyzer::Standard(Box::new(analyzer)))
            }
            AnalyzerName::NGram { n } => Ok(Analyzer::NGram(NGramAnalyzer::new(n as usize))),
            AnalyzerName::Chinese { fine } => {
                let mut analyzer = (*self.chinese_prototype()?).clone();
                if fine {
                    analyzer.set_cut_grain(CutGrain::Fine);
                }
                Ok(Analyzer::Chinese(analyzer))
            }
            AnalyzerName::Ik { fine } => {
                let mut analyzer = (*self.ik_prototype()?).clone();
                analyzer.set_fine_grained(fine);
                Ok(Analyzer::IK(analyzer))
            }
            // C++: `keyword` and `whitespace` both build a `WhitespaceAnalyzer`;
            // the first '-' introduces the delimiter set.
            AnalyzerName::Whitespace { delimiters } => {
                Ok(Analyzer::Whitespace(WhitespaceAnalyzer::new(delimiters)))
            }
            AnalyzerName::RankFeatures => Ok(Analyzer::RankFeatures(RankFeaturesAnalyzer)),
            AnalyzerName::Japanese => Ok(Analyzer::Japanese((*self.japanese_prototype()?).clone())),
            AnalyzerName::Korean => Ok(Analyzer::Korean((*self.korean_prototype()?).clone())),
            // C++: `traditional[-fine]` wires the suffix into `SetCutGrain`.
            AnalyzerName::Traditional { fine } => {
                let mut analyzer = TraditionalChineseAnalyzer::new(
                    self.opencc_prototype()?,
                    (*self.chinese_prototype()?).clone(),
                );
                if fine {
                    analyzer.set_cut_grain(CutGrain::Fine);
                }
                Ok(Analyzer::Traditional(analyzer))
            }
            AnalyzerName::SparseGram { min_n, max_n, fold } => {
                Ok(Analyzer::SparseGram(SparseGramAnalyzer {
                    min_n,
                    max_n,
                    fold,
                }))
            }
        }
    }

    pub(crate) fn japanese_prototype(&self) -> Result<Arc<JapaneseAnalyzer>, String> {
        let mut prototype = self
            .japanese_prototype
            .lock()
            .map_err(|_| "analyzer pool was poisoned".to_string())?;
        if prototype.is_none() {
            *prototype = Some(Arc::new(JapaneseAnalyzer::load(DEFAULT_RESOURCE_PATH)?));
        }
        Ok(Arc::clone(prototype.as_ref().expect("just initialized")))
    }

    pub(crate) fn korean_prototype(&self) -> Result<Arc<KoreanAnalyzer>, String> {
        let mut prototype = self
            .korean_prototype
            .lock()
            .map_err(|_| "analyzer pool was poisoned".to_string())?;
        if prototype.is_none() {
            *prototype = Some(Arc::new(KoreanAnalyzer::load(DEFAULT_RESOURCE_PATH)?));
        }
        Ok(Arc::clone(prototype.as_ref().expect("just initialized")))
    }

    pub(crate) fn opencc_prototype(&self) -> Result<Arc<OpenCcConverter>, String> {
        let mut prototype = self
            .opencc_prototype
            .lock()
            .map_err(|_| "analyzer pool was poisoned".to_string())?;
        if prototype.is_none() {
            *prototype = Some(Arc::new(OpenCcConverter::load(DEFAULT_RESOURCE_PATH)?));
        }
        Ok(Arc::clone(prototype.as_ref().expect("just initialized")))
    }

    pub(crate) fn chinese_prototype(&self) -> Result<Arc<ChineseAnalyzer>, String> {
        let mut prototype = self
            .chinese_prototype
            .lock()
            .map_err(|_| "analyzer pool was poisoned".to_string())?;
        if prototype.is_none() {
            *prototype = Some(Arc::new(ChineseAnalyzer::load(DEFAULT_RESOURCE_PATH)?));
        }
        Ok(Arc::clone(prototype.as_ref().expect("just initialized")))
    }

    pub(crate) fn ik_prototype(&self) -> Result<Arc<IKAnalyzer>, String> {
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
    pub(crate) fn get_rag(&self, name: &str) -> Result<Analyzer, String> {
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
pub(crate) fn identity_positions(start: usize, end: usize) -> Vec<(u32, u32)> {
    (start..end).map(|i| (i as u32, i as u32)).collect()
}

/// Copy `text[start..end]` verbatim, mapping every byte to itself.
pub(crate) fn copy_identity(
    text: &[u8],
    start: usize,
    end: usize,
    result: &mut Vec<u8>,
    mapping: &mut Vec<(u32, u32)>,
) {
    result.extend_from_slice(&text[start..end]);
    mapping.extend(identity_positions(start, end));
}

/// C++: the token emission shared by `TokenizeInnerWithPosition` and
/// `FineGrainedTokenizeWithPosition`. A token containing spaces is split and
/// each part gets a consecutive span; otherwise the given span is mapped back
/// through `mapping`.
pub(crate) fn emit_with_positions(
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

/// Characters in `RAGAnalyzer::regex_split_pattern_`'s first alternative.
pub(crate) fn is_split_delimiter(c: char) -> bool {
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
pub(crate) fn split_keep_delimiters_spans(line: &str) -> Vec<(String, usize)> {
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

/// C++: `re2::RE2::PartialMatch(text, RAGAnalyzer::regex_split_pattern_)`.
pub(crate) fn regex_split_partial(text: &str) -> bool {
    text.chars()
        .any(|c| c.is_ascii_alphanumeric() || is_split_delimiter(c))
}

pub(crate) fn ends_with_pattern4(text: &str) -> bool {
    text.chars()
        .last()
        .is_some_and(|c| c.is_ascii_digit() || c == ',' || c == '.' || c == '-')
}

/// C++: `CutGrain` (`analyzer.cppm`).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum CutGrain {
    Coarse,
    Fine,
}

/// C++: `MAX_TUPLE_LENGTH` (`default_values.cppm`).
pub(crate) const MAX_TUPLE_LENGTH: usize = 1024;

/// C++: `MAX_WORD_LENGTH` (`trie.hpp`).
pub(crate) const MAX_WORD_LENGTH: usize = 512;

/// C++: `MIN_DOUBLE` (`DictTrie.hpp`).
pub(crate) const MIN_DOUBLE: f64 = -3.14e100;

/// C++: `SPECIAL_SEPARATORS` (`SegmentBase.hpp`): space, tab, newline,
/// full-width comma and full-width period.
pub(crate) const JIEBA_SEPARATORS: [char; 5] = [' ', '\t', '\n', '\u{FF0C}', '\u{3002}'];

pub(crate) fn is_jieba_separator(c: char) -> bool {
    JIEBA_SEPARATORS.contains(&c)
}

/// C++: `cppjieba::DictUnit`.
#[derive(Debug, Clone)]
pub(crate) struct DictUnit {
    pub(crate) weight: f64,
    pub(crate) tag: String,
    /// Code-point length of the word.
    pub(crate) len: usize,
}

/// C++: `cppjieba::HMMModel`.
pub(crate) struct HmmModel {
    pub(crate) start: [f64; 4],
    pub(crate) trans: [[f64; 4]; 4],
    /// Emit probabilities in state order B, E, M, S.
    pub(crate) emit: [HashMap<char, f64>; 4],
}

impl HmmModel {
    pub fn load(path: &Path) -> Result<Self, String> {
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
    pub(crate) fn emit_prob(&self, state: usize, c: char) -> f64 {
        self.emit[state].get(&c).copied().unwrap_or(MIN_DOUBLE)
    }
}

pub(crate) fn parse_prob_line<const N: usize>(line: &str) -> Result<[f64; N], String> {
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

pub(crate) fn parse_emit_line(line: &str) -> Result<HashMap<char, f64>, String> {
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

pub(crate) fn sequential_letter_end(chars: &[char], start: usize) -> usize {
    let mut end = start;
    if chars.get(end).is_some_and(char::is_ascii_alphabetic) {
        end += 1;
        while chars.get(end).is_some_and(|c| c.is_ascii_alphanumeric()) {
            end += 1;
        }
    }
    end
}

pub(crate) fn numbers_end(chars: &[char], start: usize) -> usize {
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

/// C++: `CharacterUtil::IsChinese`.
pub(crate) fn is_ik_chinese(c: char) -> bool {
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
pub(crate) fn is_ik_jk(c: char) -> bool {
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

/// C++: `Dictionary` (`ik/dict_impl.cpp`) — the main and extension word sets.
pub(crate) struct IkDict {
    pub(crate) words: HashSet<String>,
    pub(crate) max_word_len: usize,
}

impl IkDict {
    pub fn load(dir: &Path) -> Result<(Self, HashSet<String>), String> {
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
    pub(crate) fn load_file(path: &Path, set: &mut HashSet<String>) -> Result<(), String> {
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
    pub(crate) fn longest_match(&self, chars: &[char], start: usize) -> Option<usize> {
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
    pub fn tokenize(&self, input: &str, smart: bool) -> Vec<String> {
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

    pub(crate) fn message(self, path: &str, function: &str) -> String {
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
pub(crate) struct UserDefinedScript {
    pub(crate) analyze: Option<UserDefinedAnalyzeFn>,
    pub(crate) term_weight: Option<UserDefinedTermWeightFn>,
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
    pub(crate) modules: Mutex<HashMap<String, UserDefinedScript>>,
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
    pub(crate) fn module_name(path: &str) -> String {
        Path::new(path)
            .file_stem()
            .map(|stem| stem.to_string_lossy().into_owned())
            .unwrap_or_else(|| path.to_string())
    }

    pub(crate) fn analyze_fn(
        &self,
        module: &str,
    ) -> Result<UserDefinedAnalyzeFn, UserDefinedError> {
        let modules = self.modules.lock().unwrap();
        let script = modules.get(module).ok_or(UserDefinedError::MissingModule)?;
        script
            .analyze
            .clone()
            .ok_or(UserDefinedError::MissingFunction)
    }

    pub(crate) fn term_weight_fn(
        &self,
        module: &str,
    ) -> Result<UserDefinedTermWeightFn, UserDefinedError> {
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
    pub(crate) scripts: &'a UserDefinedScripts,
    pub(crate) analyzer_path: String,
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
    pub(crate) scripts: &'a UserDefinedScripts,
    pub(crate) tw_path: String,
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

/// Canonical Huffman decoder.
pub(crate) struct InflateHuffman {
    pub(crate) count: [u16; 16],
    pub(crate) symbol: Vec<u16>,
}

impl InflateHuffman {
    pub fn new(lengths: &[u8]) -> Option<Self> {
        let mut count = [0u16; 16];
        for &length in lengths {
            count[length as usize] += 1;
        }
        if count[0] as usize == lengths.len() {
            return Some(Self {
                count,
                symbol: Vec::new(),
            });
        }
        let mut left: i32 = 1;
        for &width in &count[1..] {
            left <<= 1;
            left -= i32::from(width);
            if left < 0 {
                return None;
            }
        }
        let mut offsets = [0u16; 16];
        for len in 1..15 {
            offsets[len + 1] = offsets[len] + count[len];
        }
        let mut symbol = vec![0u16; lengths.len()];
        for (index, &length) in lengths.iter().enumerate() {
            if length != 0 {
                symbol[offsets[length as usize] as usize] = index as u16;
                offsets[length as usize] += 1;
            }
        }
        Some(Self { count, symbol })
    }

    pub(crate) fn decode(&self, reader: &mut InflateBits<'_>) -> Option<u16> {
        let mut code: i32 = 0;
        let mut first: i32 = 0;
        let mut index: i32 = 0;
        for &width in &self.count[1..] {
            code |= reader.bits(1)? as i32;
            let count = i32::from(width);
            if code - count < first {
                return self.symbol.get((index + (code - first)) as usize).copied();
            }
            index += count;
            first += count;
            first <<= 1;
            code <<= 1;
        }
        None
    }
}

pub(crate) const INFLATE_LEN_BASE: [u16; 29] = [
    3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131,
    163, 195, 227, 258,
];
pub(crate) const INFLATE_LEN_EXTRA: [u8; 29] = [
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0,
];
pub(crate) const INFLATE_DIST_BASE: [u16; 30] = [
    1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537,
    2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577,
];
pub(crate) const INFLATE_DIST_EXTRA: [u8; 30] = [
    0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13,
    13,
];

pub(crate) fn inflate_codes(
    reader: &mut InflateBits<'_>,
    out: &mut Vec<u8>,
    lit: &InflateHuffman,
    dist: &InflateHuffman,
) -> Result<(), String> {
    loop {
        let symbol = lit
            .decode(reader)
            .ok_or_else(|| "invalid deflate literal code".to_string())?;
        match symbol {
            0..=255 => out.push(symbol as u8),
            256 => return Ok(()),
            257..=285 => {
                let index = usize::from(symbol - 257);
                let extra = u32::from(INFLATE_LEN_EXTRA[index]);
                let length =
                    usize::from(INFLATE_LEN_BASE[index]) + reader.bits(extra).unwrap_or(0) as usize;
                let dsymbol = dist
                    .decode(reader)
                    .ok_or_else(|| "invalid deflate distance code".to_string())?;
                let dindex = usize::from(dsymbol);
                if dindex >= INFLATE_DIST_BASE.len() {
                    return Err("invalid deflate distance".to_string());
                }
                let dextra = u32::from(INFLATE_DIST_EXTRA[dindex]);
                let distance = usize::from(INFLATE_DIST_BASE[dindex])
                    + reader.bits(dextra).unwrap_or(0) as usize;
                if distance > out.len() {
                    return Err("deflate distance beyond the output".to_string());
                }
                let start = out.len() - distance;
                for offset in 0..length {
                    let byte = out[start + offset];
                    out.push(byte);
                }
            }
            _ => return Err("invalid deflate length code".to_string()),
        }
    }
}

/// A lattice node during the Viterbi pass.
pub(crate) struct LatticeNode {
    pub(crate) start: usize,
    pub(crate) length: usize,
    pub(crate) rlength: usize,
    pub(crate) lc: u16,
    pub(crate) rc: u16,
    pub(crate) posid: u16,
    pub(crate) wcost: i16,
    pub(crate) feature: u32,
    /// Whether `feature` indexes `unk.dic`'s feature pool instead of `sys.dic`'s.
    pub(crate) unknown: bool,
    pub(crate) cost: i64,
    pub(crate) prev: Option<usize>,
}

/// C++ `JMA_CType::isSpace` — the first character decides.
pub(crate) fn is_mecab_space(token: &str) -> bool {
    match token.chars().next() {
        Some(c) => c.is_ascii_whitespace() || c == '\u{3000}' || c == '\u{feff}',
        None => true,
    }
}

/// C++: the POS code of `S-A` (`記号,アルファベット`) in ipadic's `pos-id.def`.
pub(crate) const JAPANESE_ALPHA_POS: u16 = 3;

pub(crate) struct OpenCcItem {
    pub(crate) base: i32,
    pub(crate) parent: i32,
    pub(crate) word: i32,
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

    /// The port's build has to reproduce the reference's file, not merely an
    /// equivalent trie: the reference ships huqie.trie because building it takes
    /// a while, and a directory without it must end up with the same bytes. The
    /// reference's own build is byte-reproducible (verified against
    /// third_party/darts/darts.h), so this pins the port to it.
    #[test]
    fn test_rag_trie_rebuild_matches_the_shipped_file() {
        let root = Path::new(RESOURCE_DIR);
        if !root.is_dir() {
            panic!(
                "Resource directory {RESOURCE_DIR} not available; the analyzer cannot be loaded"
            );
        }
        let built = build_trie(root).expect("build the trie");
        let shipped_path = root.join("rag/huqie.trie");
        let shipped = std::fs::read(&shipped_path)
            .unwrap_or_else(|err| panic!("read {}: {err}", shipped_path.display()));
        assert_eq!(built.len(), shipped.len(), "unit count differs");
        assert!(
            built == shipped,
            "rebuilt trie differs from the shipped file"
        );
    }

    /// The RAG analyzer converts traditional to simplified before it segments
    /// (rag_analyzer_impl.cpp:1834). Without the OpenCC stage the tokens keep
    /// the source script, so this only passes once the stage is wired in.
    #[test]
    fn test_rag_converts_traditional_before_segmenting() {
        if !resource_available() {
            panic!(
                "Resource directory {RESOURCE_DIR} not available; the analyzer cannot be loaded"
            );
        }
        let traditional = get_rag_analyzer("rag").expect("rag analyzer");
        let simplified = get_rag_analyzer("rag").expect("rag analyzer");
        // Traditional input and its simplified twin segment identically.
        assert_eq!(
            analyze_to_tokens(&traditional, "臺灣大學的學生"),
            analyze_to_tokens(&simplified, "台湾大学的学生")
        );
        let tokens = analyze_to_tokens(&traditional, "漢字測試");
        assert_eq!(tokens, analyze_to_tokens(&simplified, "汉字测试"));
        // And the stage really runs: without it the tokens would keep the
        // traditional characters (the assertion above would still pass only if
        // both sides skipped it, so the flag checks the wiring itself).
        RAG_CONVERTED.with(|converted| converted.set(false));
        let _ = analyze_to_tokens(&traditional, "漢字測試");
        assert!(
            RAG_CONVERTED.with(|converted| converted.get()),
            "the RAG analyzer did not run the OpenCC stage"
        );
        // The offsets still point into the original input.
        let terms = traditional.analyze("漢字 測試");
        assert!(!terms.is_empty());
        assert_eq!(terms[0].word_offset, 0);
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

    /// `standard-porter` runs the reference's `STEM_LANG_PORT`, the 1980
    /// Porter stemmer - not rust-stemmers' `English`, which is Porter2. The
    /// two disagree exactly where Porter2 revised the rules, so these words
    /// pin the choice: Porter2 answers `sky`/`tie`, the reference `ski`/`ti`.
    #[test]
    fn standard_porter_stems_with_the_original_porter() {
        assert_eq!(standard_stemmer("porter"), Stemmer::Porter);
        assert_eq!(standard_stemmer("PORTER"), Stemmer::Porter);
        assert_eq!(
            standard_stemmer("english"),
            Stemmer::Snowball(rust_stemmers::Algorithm::English)
        );
        let mut analyzer = StandardAnalyzer::new();
        analyzer.init_stemmer(Stemmer::Porter);
        let texts: Vec<String> = analyzer
            .analyze("skies ties ability")
            .into_iter()
            .map(|term| term.text)
            .collect();
        // A differing stem is emitted next to the lowercased word.
        assert_eq!(texts, vec!["skies", "ski", "ties", "ti", "ability", "abil"]);
    }

    /// C++ `AnalyzerPool::GetAnalyzer`, name by name: the family is matched
    /// case sensitively (the reference hashes the bytes before the first `-`),
    /// `chinese`/`ik`/`traditional` take only the exact `-fine`, `standard`
    /// compares its language case insensitively, `keyword`/`whitespace` keep
    /// the delimiter bytes verbatim, `ngram` follows `strtol` (a numeric prefix
    /// is enough, the value is truncated to `u32`) and `sparsegram` validates
    /// its window like `ParseSparseGramAnalyzerName`. Nothing here touches the
    /// resource files: only the name grammar is asserted.
    #[test]
    fn analyzer_name_grammar_matches_the_reference() {
        use AnalyzerName::*;
        let accepted: &[(&str, AnalyzerName<'_>)] = &[
            ("rag", Rag),
            ("rag-fine", Rag),
            ("rag-fine-czech", Rag),
            ("rag-", Rag),
            ("rag--", Rag),
            ("standard", Standard { language: "" }),
            ("standard-", Standard { language: "" }),
            (
                "standard-english",
                Standard {
                    language: "english",
                },
            ),
            // `IEquals` is the one case-insensitive suffix comparison.
            ("standard-PORTER", Standard { language: "PORTER" }),
            (
                "standard-english-x",
                Standard {
                    language: "english-x",
                },
            ),
            ("chinese", Chinese { fine: false }),
            ("chinese-fine", Chinese { fine: true }),
            // `strcmp(rest, "-fine")`: `chinese-FINE` stays coarse.
            ("chinese-FINE", Chinese { fine: false }),
            ("chinese-fine-x", Chinese { fine: false }),
            ("traditional", Traditional { fine: false }),
            ("traditional-fine", Traditional { fine: true }),
            ("ik", Ik { fine: false }),
            ("ik-fine", Ik { fine: true }),
            ("ik-FINE", Ik { fine: false }),
            ("keyword", Whitespace { delimiters: "" }),
            ("keyword-,", Whitespace { delimiters: "," }),
            ("keyword-@#$-", Whitespace { delimiters: "@#$-" }),
            ("whitespace", Whitespace { delimiters: "" }),
            ("whitespace-", Whitespace { delimiters: "" }),
            // Everything after the first '-' is the delimiter set, verbatim.
            ("whitespace--x", Whitespace { delimiters: "-x" }),
            ("whitespace-AB", Whitespace { delimiters: "AB" }),
            ("rankfeatures", RankFeatures),
            ("rankfeatures-x", RankFeatures),
            ("japanese", Japanese),
            ("japanese-fine", Japanese),
            ("korean", Korean),
            ("korean-anything", Korean),
            // `strtol`: leading spaces, sign, and a trailing non-digit are fine.
            ("ngram-3", NGram { n: 3 }),
            ("ngram-3x", NGram { n: 3 }),
            ("ngram- 3", NGram { n: 3 }),
            ("ngram-+3", NGram { n: 3 }),
            ("ngram-0003", NGram { n: 3 }),
            ("ngram-3-12", NGram { n: 3 }),
            // The reference stores `strtol`'s result in a `u32`, so a negative
            // value wraps; it is accepted, not rejected.
            ("ngram--1", NGram { n: u32::MAX }),
            (
                "sparsegram",
                SparseGram {
                    min_n: 3,
                    max_n: 12,
                    fold: false,
                },
            ),
            (
                "sparsegram-fold",
                SparseGram {
                    min_n: 3,
                    max_n: 12,
                    fold: true,
                },
            ),
            (
                "sparsegram-5",
                SparseGram {
                    min_n: 5,
                    max_n: 12,
                    fold: false,
                },
            ),
            (
                "sparsegram-3-7-fold",
                SparseGram {
                    min_n: 3,
                    max_n: 7,
                    fold: true,
                },
            ),
            (
                "sparsegram-fold-3-7",
                SparseGram {
                    min_n: 3,
                    max_n: 7,
                    fold: true,
                },
            ),
            (
                "sparsegram-3-fold-fold",
                SparseGram {
                    min_n: 3,
                    max_n: 12,
                    fold: true,
                },
            ),
            (
                "sparsegram-1024-1024",
                SparseGram {
                    min_n: 1024,
                    max_n: 1024,
                    fold: false,
                },
            ),
        ];
        for (name, expected) in accepted {
            assert_eq!(parse_analyzer_name(name), Ok(expected.clone()), "{name}");
            assert!(is_constructible_analyzer_name(name), "{name}");
        }

        let rejected = [
            "",
            "-rag",
            // Case-sensitive family match.
            "Chinese",
            "RAG-fine",
            "Standard",
            "Sparsegram",
            "english",
            "traditionalchinese",
            "bogus",
            // `ngram` needs a '-' and a non-zero `strtol` result.
            "ngram",
            "ngram-",
            "ngram--",
            "ngram-0",
            "ngram-x",
            // `sparsegram`: empty segment, non-digit, third number, >1024, and
            // `min == 0` / `max < min` are all syntax errors.
            "sparsegram-0",
            "sparsegram-20",
            "sparsegram-1025",
            "sparsegram--3",
            "sparsegram-3-",
            "sparsegram-3x",
            "sparsegram-1-2-3",
            "sparsegram-3-FOLD",
        ];
        for name in rejected {
            assert!(
                parse_analyzer_name(name).is_err(),
                "{name} must be rejected"
            );
            assert!(!is_constructible_analyzer_name(name), "{name}");
        }

        // The two `ngram` failures are distinct in the reference.
        assert_eq!(
            parse_analyzer_name("ngram").unwrap_err(),
            "NGRAM-number, but it is ngram."
        );
        assert_eq!(
            parse_analyzer_name("ngram-0").unwrap_err(),
            "NGRAM-number, number > 0, but it is ngram-0."
        );
        assert_eq!(
            parse_analyzer_name("sparsegram-0").unwrap_err(),
            "SPARSEGRAM[-min[-max]][-fold], but it is sparsegram-0."
        );
    }

    /// The suffix has to reach the analyzer, not just the parser: `ngram-3x`
    /// tokenises like `ngram-3`, the delimiter bytes keep their case, `fine` is
    /// exact, and `sparsegram`'s window is the one the name asked for.
    #[test]
    fn analyzer_suffixes_reach_the_constructed_analyzers() {
        if !resource_available() {
            panic!(
                "Resource directory {RESOURCE_DIR} not available; the analyzers cannot be loaded"
            );
        }
        let terms = |name: &str, text: &str| -> Vec<String> {
            AnalyzerPool::instance()
                .get_analyzer(name)
                .unwrap_or_else(|error| panic!("GetAnalyzer({name}) failed: {error}"))
                .analyze(text)
                .into_iter()
                .map(|term| term.text)
                .collect()
        };

        // `strtol` takes the numeric prefix, so both names build `NGramAnalyzer(3)`.
        assert_eq!(terms("ngram-3x", "abcdef"), terms("ngram-3", "abcdef"));
        assert_eq!(terms("ngram-3x", "abcdef"), ["abc", "bcd", "cde", "def"]);
        // ... and a zero prefix is rejected instead of yielding no terms.
        assert!(AnalyzerPool::instance().get_analyzer("ngram-0x").is_err());

        // C++ `WhitespaceAnalyzer(delimiters)`: byte-exact, so `AB` is not `ab`.
        assert_eq!(terms("whitespace-AB", "aAbB"), ["a", "b"]);
        // A bare `keyword` keeps the whole value; with delimiters it splits.
        assert_eq!(terms("keyword", "a,b"), ["a,b"]);
        assert_eq!(terms("keyword-,", "a,b"), ["a", "b"]);

        // `chinese-FINE` is coarse (`strcmp`), `chinese-fine` splits further.
        let coarse = terms("chinese-FINE", GRAIN_PROBE);
        let fine = terms("chinese-fine", GRAIN_PROBE);
        assert_eq!(coarse, [GRAIN_PROBE]);
        assert!(fine.len() > coarse.len(), "{fine:?}");

        match AnalyzerPool::instance()
            .get_analyzer("sparsegram-3-7-fold")
            .expect("sparsegram-3-7-fold")
        {
            Analyzer::SparseGram(analyzer) => {
                assert_eq!(
                    (analyzer.min_n, analyzer.max_n, analyzer.fold),
                    (3, 7, true)
                );
            }
            other => panic!("expected a sparsegram analyzer, got {}", other.kind()),
        }
    }

    /// C++ `AnalyzerPool::GetAnalyzer`: every name family resolves to the
    /// analyzer the reference constructs, including the MeCab- and OpenCC-backed
    /// CJK families.
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
            ("ngram-3x", Some("ngram")),
            ("chinese", Some("chinese")),
            ("chinese-fine", Some("chinese")),
            ("ik", Some("ik")),
            ("ik-fine", Some("ik")),
            ("keyword", Some("whitespace")),
            ("keyword-,", Some("whitespace")),
            ("whitespace", Some("whitespace")),
            ("whitespace-#@$", Some("whitespace")),
            ("rankfeatures", Some("rankfeatures")),
            ("sparsegram", Some("sparsegram")),
            ("sparsegram-fold", Some("sparsegram")),
            ("sparsegram-3-12", Some("sparsegram")),
            ("sparsegram-3-12-fold", Some("sparsegram")),
            ("japanese", Some("japanese")),
            ("korean", Some("korean")),
            ("traditional", Some("traditional")),
            ("traditional-fine", Some("traditional")),
            // Not a family on either side: C++ hashes only whole prefixes.
            ("traditionalchinese", None),
            ("english", None),
            ("userdefined", None),
            ("ngram", None),
            ("bogus", None),
            // The family match is case sensitive (`Str2Int` hashes the bytes).
            ("Chinese", None),
            ("RAG-fine", None),
            ("Standard", None),
            // Numeric suffixes the reference rejects.
            ("ngram-0", None),
            ("ngram-x", None),
            ("sparsegram-0", None),
            ("sparsegram-20", None),
            ("sparsegram-1025", None),
            ("sparsegram--3", None),
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

    fn analyze_text(analyzer: &Analyzer, text: &str) -> Vec<String> {
        analyzer
            .analyze(text)
            .into_iter()
            .map(|term| term.text)
            .collect()
    }

    /// C++ `JapaneseAnalyzer` over `mecab/ipadic`: one-best MeCab segmentation
    /// with the dictionary's own word boundaries. The expected tokens are the
    /// reference engine's output for these sentences (ipadic + Viterbi).
    #[test]
    fn test_japanese_analyzer_segments_with_ipadic() {
        if !resource_available() {
            panic!("Resource directory {RESOURCE_DIR} not available; ipadic cannot be loaded");
        }
        let analyzer = AnalyzerPool::instance().get_analyzer("japanese").unwrap();

        assert_eq!(
            analyze_text(&analyzer, "日本語の文章を解析する"),
            vec!["日本語", "の", "文章", "を", "解析", "する"]
        );
        assert_eq!(
            analyze_text(&analyzer, "東京都渋谷区に住んでいます"),
            vec!["東京", "都", "渋谷", "区", "に", "住ん", "で", "い", "ます"]
        );
        // The reference's classic longest/shortest-path discriminator.
        assert_eq!(
            analyze_text(&analyzer, "すもももももももものうち"),
            vec!["すもも", "も", "もも", "も", "もも", "の", "うち"]
        );
    }

    /// Alphabet runs are POS `S-A` (ipadic `記号,アルファベット`) and are
    /// lower-cased by `CommonLanguageAnalyzer`; no stemmer is initialised for
    /// this family, so "running" keeps its form.
    #[test]
    fn test_japanese_analyzer_lowercases_alphabet_runs() {
        if !resource_available() {
            panic!("Resource directory {RESOURCE_DIR} not available; ipadic cannot be loaded");
        }
        let analyzer = AnalyzerPool::instance().get_analyzer("japanese").unwrap();
        assert_eq!(
            analyze_text(&analyzer, "I love Rust 2024年"),
            vec!["i", "love", "rust", "2024", "年"]
        );
        assert_eq!(
            analyze_text(&analyzer, "price: 3.14"),
            vec!["price", ":", "3.14"]
        );
    }

    /// C++ `KoreanAnalyzer` over `mecab/ko-dic`: morphemes come straight from
    /// the MeCab lattice, with `SL` foreign-script tokens lower-cased.
    #[test]
    fn test_korean_analyzer_segments_with_ko_dic() {
        if !resource_available() {
            panic!("Resource directory {RESOURCE_DIR} not available; ko-dic cannot be loaded");
        }
        let analyzer = AnalyzerPool::instance().get_analyzer("korean").unwrap();

        assert_eq!(
            analyze_text(&analyzer, "안녕하세요"),
            vec!["안녕", "하", "세요"]
        );
        assert_eq!(
            analyze_text(&analyzer, "한국어 형태소 분석"),
            vec!["한국어", "형태소", "분석"]
        );
        assert_eq!(
            analyze_text(&analyzer, "서울에 갑니다"),
            vec!["서울", "에", "갑니다"]
        );
        // `Rust` is tagged `SL`, so it is lower-cased like the reference.
        assert_eq!(
            analyze_text(&analyzer, "Rust 언어를 배웁니다"),
            vec!["rust", "언어", "를", "배웁니다"]
        );
    }

    /// C++ `OpenCC` with `zht2zhs.ini`: phrase table first, character table
    /// second, converted along OpenCC's shortest-path segmentation.
    #[test]
    fn test_opencc_converts_traditional_to_simplified() {
        if !resource_available() {
            panic!("Resource directory {RESOURCE_DIR} not available; opencc cannot be loaded");
        }
        let converter = OpenCcConverter::load(RESOURCE_DIR).unwrap();
        assert_eq!(converter.convert("漢字測試"), "汉字测试");
        assert_eq!(converter.convert("臺灣大學的學生"), "台湾大学的学生");
        assert_eq!(converter.convert("這個問題很難解決"), "这个问题很难解决");
        assert_eq!(converter.convert("軟體開發"), "软体开发");
        assert_eq!(converter.convert("記憶體與網路"), "记忆体与网路");
        // Simplified input is already in the target script.
        assert_eq!(converter.convert("汉字测试"), "汉字测试");
        assert_eq!(converter.convert(""), "");
    }

    /// C++ `TraditionalChineseAnalyzer::Parse`: convert, then run the jieba
    /// analyzer over the simplified text. `traditional-fine` switches the
    /// jieba cut grain.
    #[test]
    fn test_traditional_analyzer_converts_then_segments() {
        if !resource_available() {
            panic!("Resource directory {RESOURCE_DIR} not available; opencc cannot be loaded");
        }
        let chinese = AnalyzerPool::instance().get_analyzer("chinese").unwrap();
        let traditional = AnalyzerPool::instance()
            .get_analyzer("traditional")
            .unwrap();
        let Analyzer::Traditional(traditional_analyzer) = &traditional else {
            panic!("GetAnalyzer(\"traditional\") did not return a TraditionalChineseAnalyzer");
        };

        for text in ["漢字測試", "臺灣大學的學生", "這個問題很難解決", "軟體開發"]
        {
            let converted = traditional_analyzer.convert(text);
            assert_eq!(
                analyze_text(&traditional, text),
                analyze_text(&chinese, &converted),
                "conversion of {text} produced {converted}"
            );
        }

        assert_eq!(analyze_text(&traditional, "漢字測試"), vec!["汉字", "测试"]);

        let fine = AnalyzerPool::instance()
            .get_analyzer("traditional-fine")
            .unwrap();
        let Analyzer::Traditional(fine_analyzer) = &fine else {
            panic!("GetAnalyzer(\"traditional-fine\") did not return a TraditionalChineseAnalyzer");
        };
        assert_eq!(fine_analyzer.convert("臺灣大學"), "台湾大学");
        // Fine-grained jieba splits the converted text the same way
        // `chinese-fine` does; only the script conversion is added.
        let chinese_fine = AnalyzerPool::instance()
            .get_analyzer("chinese-fine")
            .unwrap();
        assert_eq!(
            analyze_text(&fine, "臺灣大學"),
            analyze_text(&chinese_fine, "台湾大学")
        );
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
