//! Text analysis: the standard tokenizer/analyzer.
//!
//! C++: `TokenizeConfig` / `Tokenizer` (`src/common/analyzer/tokenizer.cppm`),
//! `CommonLanguageAnalyzer` (`common_analyzer.cppm`) and `StandardAnalyzer`
//! (`standard_analyzer.cppm`). The full-text index path already used the same
//! tokenization rules through `lib.rs`; this module makes the classes explicit
//! and exposes the configurable options the unit tests exercise.

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
}
