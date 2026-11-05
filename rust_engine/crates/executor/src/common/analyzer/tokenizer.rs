use super::*;

/// Token classes, mirroring the reference's per-byte table.
pub(crate) const ALLOW: u8 = 0;

pub(crate) const DELIM: u8 = 1;

pub(crate) const SPACE: u8 = 2;

pub(crate) const UNITE: u8 = 3;

/// C++: `TokenizeConfig` (`tokenizer.cppm`). Defaults: ASCII alphanumerics and
/// every byte above 127 are allowed, ASCII whitespace separates, `/` unites,
/// everything else divides.
#[derive(Debug, Clone)]
pub struct TokenizeConfig {
    pub(crate) kind: [u8; 256],
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

    pub(crate) fn class(&self, b: u8) -> u8 {
        self.kind[b as usize]
    }
}

/// C++: `Tokenizer::Tokenize` — returns each raw token with its byte span.
///
/// A delimiter is skipped on its own; a unite byte is dropped while the token
/// continues, but a *leading* unite byte starts the token (the reference's
/// scanner behavior, verified on the engine: `x/y` -> `xy`, `/foo` -> `/foo`).
/// Feeds every token of the input to the callback as (text, start, end).
///
/// The text is a slice of the input unless a UNITE byte was dropped inside the
/// token (an apostrophe inside a word, say); those come from a scratch buffer
/// reused for every token. The reference's tokenizer produces the same tokens,
/// but the port used to allocate a byte Vec and a String per token - three
/// million of each for a 20k-row column.
pub fn for_each_token(s: &str, config: &TokenizeConfig, mut f: impl FnMut(&str, usize, usize)) {
    let bytes = s.as_bytes();
    let mut buf: Vec<u8> = Vec::new();
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
        let mut dropped = false;
        buf.clear();
        buf.push(bytes[i]);
        i += 1;
        while i < bytes.len() {
            match config.class(bytes[i]) {
                SPACE | DELIM => break,
                ALLOW => {
                    buf.push(bytes[i]);
                    i += 1;
                }
                _ => {
                    dropped = true;
                    i += 1; // UNITE: dropped, the token continues
                }
            }
        }
        let text: &str = if dropped {
            match std::str::from_utf8(&buf) {
                Ok(text) => text,
                Err(_) => continue,
            }
        } else {
            &s[start..i]
        };
        if !text.is_empty() {
            f(text, start, i);
        }
    }
}

/// for_each_token, collecting owned tokens.
pub fn tokenize(s: &str, config: &TokenizeConfig) -> Vec<(String, usize, usize)> {
    let mut out = Vec::new();
    for_each_token(s, config, |text, start, end| {
        out.push((text.to_string(), start, end))
    });
    out
}

/// One term the analyzer emits, borrowed from the input (or from the stem
/// cache) — the shape [`StandardAnalyzer::for_each_term`] hands out so the
/// index path can intern a term without building a `String` for it.
pub struct TermView<'a> {
    pub text: &'a str,
    pub word_offset: u32,
    pub char_offset: u32,
    pub end_offset: u32,
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
