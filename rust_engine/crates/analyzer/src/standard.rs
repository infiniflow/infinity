use super::*;

/// C++: `StandardAnalyzer` + `CommonLanguageAnalyzer` options.
#[derive(Debug, Clone)]
pub struct StandardAnalyzer {
    pub(crate) tokenize_config: TokenizeConfig,
    pub(crate) case_sensitive: bool,
    pub(crate) contain_lower: bool,
    pub(crate) extract_eng_stem: bool,
    /// C++: the `STEM_LANG_*` selected by `InitStemmer`. `None` until a
    /// stemmer is initialized.
    pub(crate) stemmer: Option<Stemmer>,
    pub(crate) stem_only: bool,
    pub(crate) char_offset: bool,
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
        self.init_stemmer(Stemmer::Snowball(rust_stemmers::Algorithm::English));
    }

    /// C++: `CommonLanguageAnalyzer::InitStemmer`.
    pub fn init_stemmer(&mut self, algorithm: Stemmer) {
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

    /// Feeds every term the analyzer emits to `f`, borrowing the text.
    ///
    /// The index path needs a term only long enough to intern it, so it walks
    /// the terms here instead of calling [`Self::analyze`]: a 20k-row column is
    /// ~3.7M terms, and a `String` per term was measurable (see
    /// `docs/FULLTEXT_BENCH.md` §8.1).
    pub fn for_each_term(&self, input: &str, mut f: impl FnMut(TermView<'_>)) {
        let mut word_index = 0u32;
        for_each_token(input, &self.tokenize_config, |token, start, end| {
            let word_offset = word_index;
            word_index += 1;
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
            // Lower-casing only costs an allocation when the token has an
            // uppercase byte; most words of a corpus are already lowercase.
            let lower: std::borrow::Cow<str> = if token.bytes().any(|b| b.is_ascii_uppercase()) {
                std::borrow::Cow::Owned(token.to_ascii_lowercase())
            } else {
                std::borrow::Cow::Borrowed(token)
            };
            let lower_differs = token != lower.as_ref();
            let mut emit = |text: &str| {
                f(TermView {
                    text,
                    word_offset,
                    char_offset: offset,
                    end_offset,
                    payload: 0,
                })
            };
            let emit = &mut emit;
            let stem_emitted = match self.stemmer {
                Some(stemmer) if self.extract_eng_stem => stemmer.with_stem(&lower, |stem| {
                    // Only a stem that differs from the word becomes a term.
                    if stem == lower.as_ref() {
                        return false;
                    }
                    if self.stem_only {
                        emit(stem);
                    } else if self.case_sensitive {
                        emit(token);
                        emit(stem);
                        if self.contain_lower && lower_differs {
                            emit(&lower);
                        }
                    } else {
                        emit(&lower);
                        emit(stem);
                    }
                    true
                }),
                _ => false,
            };
            if !stem_emitted {
                if self.case_sensitive {
                    if self.contain_lower && lower_differs {
                        emit(token);
                        emit(&lower);
                    } else {
                        emit(token);
                    }
                } else {
                    emit(&lower);
                }
            }
        });
    }

    /// C++: CommonLanguageAnalyzer::AnalyzeImpl + StandardAnalyzer::NextToken.
    pub fn analyze(&self, input: &str) -> Vec<Term> {
        let mut out = Vec::new();
        self.for_each_term(input, |term| {
            out.push(Term {
                text: term.text.to_string(),
                word_offset: term.word_offset,
                char_offset: term.char_offset,
                end_offset: term.end_offset,
                is_index: true,
                payload: 0,
            })
        });
        out
    }
}
