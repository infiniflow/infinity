//! Minimal JSON value model + parser + compact serializer.
//!
//! This is the storage-layer JSON representation (`Value` of
//! `LogicalType::kJson` in the reference). The expression crate keeps its own
//! path-extraction helpers on top of the same model shape.

/// A parsed JSON value (object key order is preserved).
#[derive(Debug, Clone, PartialEq, Default)]
pub enum Json {
    #[default]
    Null,
    Bool(bool),
    Int(i64),
    Float(f64),
    Str(String),
    Arr(Vec<Json>),
    Obj(Vec<(String, Json)>),
}

impl Json {
    /// Compact serialization (no spaces); object keys are rendered sorted, as
    /// the reference's JSON writer does.
    pub fn compact(&self) -> String {
        match self {
            Json::Null => "null".to_string(),
            Json::Bool(b) => b.to_string(),
            Json::Int(i) => i.to_string(),
            Json::Float(f) => fmt_json_float(*f),
            Json::Str(s) => format!("\"{}\"", escape(s)),
            Json::Arr(items) => {
                let inner: Vec<String> = items.iter().map(|v| v.compact()).collect();
                format!("[{}]", inner.join(","))
            }
            Json::Obj(entries) => {
                let mut sorted: Vec<&(String, Json)> = entries.iter().collect();
                sorted.sort_by(|a, b| a.0.cmp(&b.0));
                let inner: Vec<String> = sorted
                    .iter()
                    .map(|(k, v)| format!("\"{}\":{}", escape(k), v.compact()))
                    .collect();
                format!("{{{}}}", inner.join(","))
            }
        }
    }
}

fn fmt_json_float(f: f64) -> String {
    if f.fract() == 0.0 && f.abs() < 1e15 {
        format!("{f:.1}")
    } else {
        format!("{f}")
    }
}

fn escape(s: &str) -> String {
    let mut out = String::new();
    for c in s.chars() {
        match c {
            '"' => out.push_str("\\\""),
            '\\' => out.push_str("\\\\"),
            '\n' => out.push_str("\\n"),
            '\t' => out.push_str("\\t"),
            '\r' => out.push_str("\\r"),
            other => out.push(other),
        }
    }
    out
}

/// Parses a JSON document, returning `None` on malformed input.
pub fn parse(text: &str) -> Option<Json> {
    let mut p = Parser {
        chars: text.chars().collect(),
        pos: 0,
    };
    p.skip_ws();
    let v = p.value()?;
    p.skip_ws();
    Some(v)
}

struct Parser {
    chars: Vec<char>,
    pos: usize,
}

impl Parser {
    fn peek(&self) -> Option<char> {
        self.chars.get(self.pos).copied()
    }
    fn bump(&mut self) -> Option<char> {
        let c = self.peek();
        if c.is_some() {
            self.pos += 1;
        }
        c
    }
    fn skip_ws(&mut self) {
        while matches!(self.peek(), Some(c) if c.is_whitespace()) {
            self.pos += 1;
        }
    }
    fn value(&mut self) -> Option<Json> {
        self.skip_ws();
        match self.peek()? {
            '{' => self.object(),
            '[' => self.array(),
            '"' => self.string().map(Json::Str),
            't' | 'f' => self.boolean(),
            'n' => self.null(),
            _ => self.number(),
        }
    }
    fn object(&mut self) -> Option<Json> {
        self.bump(); // {
        let mut entries = Vec::new();
        self.skip_ws();
        if self.peek() == Some('}') {
            self.bump();
            return Some(Json::Obj(entries));
        }
        loop {
            self.skip_ws();
            let key = self.string()?;
            self.skip_ws();
            if self.bump() != Some(':') {
                return None;
            }
            let val = self.value()?;
            entries.push((key, val));
            self.skip_ws();
            match self.bump() {
                Some(',') => continue,
                Some('}') => break,
                _ => return None,
            }
        }
        Some(Json::Obj(entries))
    }
    fn array(&mut self) -> Option<Json> {
        self.bump(); // [
        let mut items = Vec::new();
        self.skip_ws();
        if self.peek() == Some(']') {
            self.bump();
            return Some(Json::Arr(items));
        }
        loop {
            items.push(self.value()?);
            self.skip_ws();
            match self.bump() {
                Some(',') => continue,
                Some(']') => break,
                _ => return None,
            }
        }
        Some(Json::Arr(items))
    }
    fn string(&mut self) -> Option<String> {
        if self.bump() != Some('"') {
            return None;
        }
        let mut s = String::new();
        loop {
            let c = self.bump()?;
            match c {
                '"' => break,
                '\\' => match self.bump()? {
                    '"' => s.push('"'),
                    '\\' => s.push('\\'),
                    '/' => s.push('/'),
                    'n' => s.push('\n'),
                    't' => s.push('\t'),
                    'r' => s.push('\r'),
                    'b' => s.push('\u{8}'),
                    'f' => s.push('\u{c}'),
                    'u' => {
                        let hex: String = (0..4).filter_map(|_| self.bump()).collect();
                        if let Ok(cp) = u32::from_str_radix(&hex, 16) {
                            if let Some(ch) = char::from_u32(cp) {
                                s.push(ch);
                            }
                        }
                    }
                    other => s.push(other),
                },
                other => s.push(other),
            }
        }
        Some(s)
    }
    fn boolean(&mut self) -> Option<Json> {
        if self.chars[self.pos..].starts_with(&['t', 'r', 'u', 'e']) {
            self.pos += 4;
            Some(Json::Bool(true))
        } else if self.chars[self.pos..].starts_with(&['f', 'a', 'l', 's', 'e']) {
            self.pos += 5;
            Some(Json::Bool(false))
        } else {
            None
        }
    }
    fn null(&mut self) -> Option<Json> {
        if self.chars[self.pos..].starts_with(&['n', 'u', 'l', 'l']) {
            self.pos += 4;
            Some(Json::Null)
        } else {
            None
        }
    }
    fn number(&mut self) -> Option<Json> {
        let start = self.pos;
        while matches!(self.peek(), Some(c) if c.is_ascii_digit() || matches!(c, '-' | '+' | '.' | 'e' | 'E'))
        {
            self.pos += 1;
        }
        let text: String = self.chars[start..self.pos].iter().collect();
        if text.is_empty() {
            return None;
        }
        if !text.contains(['.', 'e', 'E']) {
            if let Ok(i) = text.parse::<i64>() {
                return Some(Json::Int(i));
            }
        }
        text.parse::<f64>().ok().map(Json::Float)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn parses_and_compacts_objects() {
        let text = "{\"key\": \"value\", \"number\": 42}";
        let json = parse(text).unwrap();
        assert_eq!(
            json,
            Json::Obj(vec![
                ("key".to_string(), Json::Str("value".to_string())),
                ("number".to_string(), Json::Int(42)),
            ])
        );
        assert_eq!(json.compact(), "{\"key\":\"value\",\"number\":42}");
        assert_eq!(parse("not json"), None);
    }
}
