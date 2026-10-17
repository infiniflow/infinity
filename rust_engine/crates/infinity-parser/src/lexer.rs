//! Hand-written lexer. Keywords are lexed as [`Token::Ident`]; the parser matches
//! them case-insensitively.

use infinity_common::{Error, Result};

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
    let chars: Vec<char> = sql.chars().collect();
    let mut i = 0;
    let mut out = Vec::new();
    while i < chars.len() {
        let c = chars[i];
        match c {
            c if c.is_whitespace() => {
                i += 1;
            }
            ',' => {
                out.push(Token::Comma);
                i += 1;
            }
            '(' => {
                out.push(Token::LParen);
                i += 1;
            }
            ')' => {
                out.push(Token::RParen);
                i += 1;
            }
            '[' => {
                out.push(Token::LBracket);
                i += 1;
            }
            ']' => {
                out.push(Token::RBracket);
                i += 1;
            }
            '{' => {
                out.push(Token::LBrace);
                i += 1;
            }
            '}' => {
                out.push(Token::RBrace);
                i += 1;
            }
            '"' => {
                i += 1;
                let mut s = String::new();
                while i < chars.len() && chars[i] != '"' {
                    s.push(chars[i]);
                    i += 1;
                }
                if i >= chars.len() {
                    return Err(Error::Invalid("unterminated quoted identifier".into()));
                }
                i += 1;
                out.push(Token::Str(s));
            }
            ':' => {
                out.push(Token::Colon);
                i += 1;
            }
            '*' => {
                out.push(Token::Star);
                i += 1;
            }
            ';' => {
                out.push(Token::Semicolon);
                i += 1;
            }
            // `==` is accepted as an alias for `=`
            '=' => {
                if i + 1 < chars.len() && chars[i + 1] == '=' {
                    i += 2;
                } else {
                    i += 1;
                }
                out.push(Token::Op("=".into()));
            }
            '.' => {
                out.push(Token::Op(".".into()));
                i += 1;
            }
            '!' if i + 1 < chars.len() && chars[i + 1] == '=' => {
                out.push(Token::Op("!=".into()));
                i += 2;
            }
            '<' => {
                if i + 1 < chars.len() && chars[i + 1] == '=' {
                    out.push(Token::Op("<=".into()));
                    i += 2;
                } else if i + 1 < chars.len() && chars[i + 1] == '>' {
                    out.push(Token::Op("<>".into()));
                    i += 2;
                } else {
                    out.push(Token::Op("<".into()));
                    i += 1;
                }
            }
            '>' => {
                if i + 1 < chars.len() && chars[i + 1] == '=' {
                    out.push(Token::Op(">=".into()));
                    i += 2;
                } else {
                    out.push(Token::Op(">".into()));
                    i += 1;
                }
            }
            '\'' => {
                i += 1;
                let mut s = String::new();
                loop {
                    if i >= chars.len() {
                        return Err(Error::Invalid("unterminated string literal".into()));
                    }
                    if chars[i] == '\'' {
                        // `''` inside a literal is an escaped quote.
                        if i + 1 < chars.len() && chars[i + 1] == '\'' {
                            s.push('\'');
                            i += 2;
                            continue;
                        }
                        i += 1; // closing quote
                        break;
                    }
                    s.push(chars[i]);
                    i += 1;
                }
                out.push(Token::Str(s));
            }
            c if c.is_ascii_digit()
                || (c == '-'
                    && i + 1 < chars.len()
                    && chars[i + 1].is_ascii_digit()
                    && !prev_is_value(&out)) =>
            {
                let start = i;
                if c == '-' {
                    i += 1;
                }
                let mut is_float = false;
                while i < chars.len() && (chars[i].is_ascii_digit() || chars[i] == '.') {
                    if chars[i] == '.' {
                        is_float = true;
                    }
                    i += 1;
                }
                let text: String = chars[start..i].iter().collect();
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
            c if c.is_alphabetic() || c == '_' => {
                let start = i;
                while i < chars.len() && (chars[i].is_alphanumeric() || chars[i] == '_') {
                    i += 1;
                }
                out.push(Token::Ident(chars[start..i].iter().collect()));
            }
            '+' | '-' | '/' | '%' => {
                out.push(Token::Op(c.to_string()));
                i += 1;
            }
            other => return Err(Error::Invalid(format!("unexpected character '{other}'"))),
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
}
