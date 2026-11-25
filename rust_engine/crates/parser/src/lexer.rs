// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//! Hand-written lexer. Keywords are lexed as [`Token::Ident`]; the parser matches
//! them case-insensitively.

use common::{Error, Result};

#[derive(Debug, Clone, PartialEq)]
pub enum Token {
    Ident(String),
    Int(i64),
    Float(f64),
    Str(String),
    Op(String),
    Comma,
    LParen,
    RParen,
    LBracket,
    RBracket,
    LBrace,
    RBrace,
    Colon,
    Star,
    Semicolon,
    Eof,
}

/// True if the previous token ends a value (so `-`/`+` is a binary operator
/// rather than a sign).
fn prev_is_value(out: &[Token]) -> bool {
    matches!(
        out.last(),
        Some(
            Token::Ident(_)
                | Token::Int(_)
                | Token::Float(_)
                | Token::Str(_)
                | Token::RParen
                | Token::RBracket
        )
    )
}

pub fn tokenize(sql: &str) -> Result<Vec<Token>> {
    let bytes = sql.as_bytes();
    let len = bytes.len();
    let mut i = 0;
    let mut out = Vec::new();
    while i < len {
        let b = bytes[i];
        match b {
            b if b.is_ascii_whitespace() || b == b'\x0b' => {
                i += 1;
            }
            b',' => {
                out.push(Token::Comma);
                i += 1;
            }
            b'(' => {
                out.push(Token::LParen);
                i += 1;
            }
            b')' => {
                out.push(Token::RParen);
                i += 1;
            }
            b'[' => {
                out.push(Token::LBracket);
                i += 1;
            }
            b']' => {
                out.push(Token::RBracket);
                i += 1;
            }
            b'{' => {
                out.push(Token::LBrace);
                i += 1;
            }
            b'}' => {
                out.push(Token::RBrace);
                i += 1;
            }
            b'"' => {
                i += 1;
                let start = i;
                while i < len && bytes[i] != b'"' {
                    i += 1;
                }
                if i >= len {
                    return Err(Error::Invalid("unterminated quoted identifier".into()));
                }
                out.push(Token::Str(sql[start..i].to_owned()));
                i += 1;
            }
            b':' => {
                out.push(Token::Colon);
                i += 1;
            }
            b'*' => {
                out.push(Token::Star);
                i += 1;
            }
            b';' => {
                out.push(Token::Semicolon);
                i += 1;
            }
            // `==` is accepted as an alias for `=`
            b'=' => {
                if i + 1 < len && bytes[i + 1] == b'=' {
                    i += 2;
                } else {
                    i += 1;
                }
                out.push(Token::Op("=".into()));
            }
            b'.' => {
                out.push(Token::Op(".".into()));
                i += 1;
            }
            b'!' if i + 1 < len && bytes[i + 1] == b'=' => {
                out.push(Token::Op("!=".into()));
                i += 2;
            }
            b'<' => {
                if i + 1 < len && bytes[i + 1] == b'=' {
                    out.push(Token::Op("<=".into()));
                    i += 2;
                } else if i + 1 < len && bytes[i + 1] == b'>' {
                    out.push(Token::Op("<>".into()));
                    i += 2;
                } else {
                    out.push(Token::Op("<".into()));
                    i += 1;
                }
            }
            b'>' => {
                if i + 1 < len && bytes[i + 1] == b'=' {
                    out.push(Token::Op(">=".into()));
                    i += 2;
                } else {
                    out.push(Token::Op(">".into()));
                    i += 1;
                }
            }
            b'\'' => {
                i += 1;
                let start = i;
                let mut escaped: Option<String> = None;
                let mut seg_start = i;
                loop {
                    if i >= len {
                        return Err(Error::Invalid("unterminated string literal".into()));
                    }
                    if bytes[i] == b'\'' {
                        // `''` inside a literal is an escaped quote.
                        if i + 1 < len && bytes[i + 1] == b'\'' {
                            let buf = escaped.get_or_insert_with(String::new);
                            buf.push_str(&sql[seg_start..i]);
                            buf.push('\'');
                            i += 2;
                            seg_start = i;
                            continue;
                        }
                        let s = match escaped {
                            Some(mut buf) => {
                                buf.push_str(&sql[seg_start..i]);
                                buf
                            }
                            None => sql[start..i].to_owned(),
                        };
                        i += 1; // closing quote
                        out.push(Token::Str(s));
                        break;
                    }
                    i += 1;
                }
            }
            b if b.is_ascii_digit()
                || (b == b'-'
                    && i + 1 < len
                    && bytes[i + 1].is_ascii_digit()
                    && !prev_is_value(&out)) =>
            {
                let start = i;
                if b == b'-' {
                    i += 1;
                }
                let mut is_float = false;
                while i < len && (bytes[i].is_ascii_digit() || bytes[i] == b'.') {
                    if bytes[i] == b'.' {
                        is_float = true;
                    }
                    i += 1;
                }
                // An exponent belongs to the literal: `1e-5`, `2.5E+3`. Without
                // this the array reader saw `1`, `e`, `-`, `5`.
                if i < len && (bytes[i] == b'e' || bytes[i] == b'E') {
                    let mut j = i + 1;
                    if j < len && (bytes[j] == b'+' || bytes[j] == b'-') {
                        j += 1;
                    }
                    if j < len && bytes[j].is_ascii_digit() {
                        while j < len && bytes[j].is_ascii_digit() {
                            j += 1;
                        }
                        i = j;
                        is_float = true;
                    }
                }
                let text = &sql[start..i];
                if is_float {
                    out.push(Token::Float(
                        text.parse()
                            .map_err(|_| Error::Invalid(format!("bad float {text}")))?,
                    ));
                } else {
                    out.push(Token::Int(
                        text.parse()
                            .map_err(|_| Error::Invalid(format!("bad int {text}")))?,
                    ));
                }
            }
            b if b.is_ascii_alphabetic() || b == b'_' => {
                let start = i;
                i += 1;
                while i < len {
                    if bytes[i] < 0x80 {
                        if bytes[i].is_ascii_alphanumeric() || bytes[i] == b'_' {
                            i += 1;
                        } else {
                            break;
                        }
                    } else {
                        let c = sql[i..].chars().next().unwrap();
                        if c.is_alphanumeric() || c == '_' {
                            i += c.len_utf8();
                        } else {
                            break;
                        }
                    }
                }
                out.push(Token::Ident(sql[start..i].to_owned()));
            }
            b'+' | b'-' | b'/' | b'%' => {
                out.push(Token::Op(sql[i..i + 1].to_owned()));
                i += 1;
            }
            b if b >= 0x80 => {
                let c = sql[i..].chars().next().unwrap();
                if c.is_whitespace() {
                    i += c.len_utf8();
                } else if c.is_alphabetic() {
                    let start = i;
                    i += c.len_utf8();
                    while i < len {
                        if bytes[i] < 0x80 {
                            if bytes[i].is_ascii_alphanumeric() || bytes[i] == b'_' {
                                i += 1;
                            } else {
                                break;
                            }
                        } else {
                            let c2 = sql[i..].chars().next().unwrap();
                            if c2.is_alphanumeric() || c2 == '_' {
                                i += c2.len_utf8();
                            } else {
                                break;
                            }
                        }
                    }
                    out.push(Token::Ident(sql[start..i].to_owned()));
                } else {
                    return Err(Error::Invalid(format!("unexpected character '{c}'")));
                }
            }
            other => {
                return Err(Error::Invalid(format!(
                    "unexpected character '{}'",
                    other as char
                )))
            }
        }
    }
    out.push(Token::Eof);
    Ok(out)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn lexes_basic() {
        let t = tokenize("CREATE TABLE t (a INT, b);").unwrap();
        assert_eq!(
            t,
            vec![
                Token::Ident("CREATE".into()),
                Token::Ident("TABLE".into()),
                Token::Ident("t".into()),
                Token::LParen,
                Token::Ident("a".into()),
                Token::Ident("INT".into()),
                Token::Comma,
                Token::Ident("b".into()),
                Token::RParen,
                Token::Semicolon,
                Token::Eof,
            ]
        );
    }

    #[test]
    fn lexes_numbers_and_strings() {
        let t = tokenize("1, -2, 3.5, 'hi'").unwrap();
        assert_eq!(
            t,
            vec![
                Token::Int(1),
                Token::Comma,
                Token::Int(-2),
                Token::Comma,
                Token::Float(3.5),
                Token::Comma,
                Token::Str("hi".into()),
                Token::Eof
            ]
        );
        // after a value, `-` is a binary operator (not a sign)
        let t = tokenize("a-2").unwrap();
        assert_eq!(
            t,
            vec![
                Token::Ident("a".into()),
                Token::Op("-".into()),
                Token::Int(2),
                Token::Eof
            ]
        );
    }

    #[test]
    fn lexes_vertical_tab_whitespace() {
        let t = tokenize("SELECT\u{000B}1").unwrap();
        assert_eq!(
            t,
            vec![Token::Ident("SELECT".into()), Token::Int(1), Token::Eof]
        );
    }
}
