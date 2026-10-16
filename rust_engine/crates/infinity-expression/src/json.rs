//! Minimal JSON value model + parser + compact serializer + path extraction,
//! used by the `json_extract*` scalar functions.

use crate::Value;

/// A parsed JSON value (object key order is preserved).
#[derive(Debug, Clone, PartialEq)]
pub enum Json {
    Null,
    Bool(bool),
    Int(i64),
    Float(f64),
    Str(String),
    Arr(Vec<Json>),
    Obj(Vec<(String, Json)>),
}

impl Json {
    /// Compact serialization (no spaces), preserving key order.
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
                // Infinity renders object keys in sorted order.
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

/// Applies a `$.a.b[i]`-style path to a JSON value.
pub fn extract<'a>(root: &'a Json, path: &str) -> Option<&'a Json> {
    let mut cur = root;
    let bytes: Vec<char> = path.chars().collect();
    let mut i = 0;
    if bytes.first() == Some(&'$') {
        i = 1;
    }
    while i < bytes.len() {
        match bytes[i] {
            '.' => {
                i += 1;
                let start = i;
                while i < bytes.len() && bytes[i] != '.' && bytes[i] != '[' {
                    i += 1;
                }
                let key: String = bytes[start..i].iter().collect();
                cur = match cur {
                    Json::Obj(entries) => &entries.iter().find(|(k, _)| *k == key)?.1,
                    _ => return None,
                };
            }
            '[' => {
                i += 1;
                let start = i;
                while i < bytes.len() && bytes[i] != ']' {
                    i += 1;
                }
                let idx_text: String = bytes[start..i].iter().collect();
                if i < bytes.len() {
                    i += 1; // ]
                }
                let idx: usize = idx_text.trim().parse().ok()?;
                cur = match cur {
                    Json::Arr(items) => items.get(idx)?,
                    _ => return None,
                };
            }
            _ => return None,
        }
    }
    Some(cur)
}

/// `json_contains(doc, path, value)`: true if the path value equals `value`, or
/// (when the path value is an array) any element equals it.
pub fn contains(doc: &str, path: &str, target: &Value) -> Value {
    let Some(root) = parse(doc) else {
        return Value::Null;
    };
    let Some(found) = extract(&root, path) else {
        return Value::Bool(false);
    };
    let hit = match found {
        Json::Arr(items) => items.iter().any(|v| json_matches(v, target)),
        other => json_matches(other, target),
    };
    Value::Bool(hit)
}

/// `json_contains(doc, value)`: true when `doc` is an array whose top-level
/// elements include `value`. A non-array document (object/scalar) never
/// contains anything.
pub fn contains_value(doc: &str, target: &Value) -> Value {
    let Some(root) = parse(doc) else {
        return Value::Null;
    };
    match root {
        Json::Arr(items) => Value::Bool(items.iter().any(|v| json_matches(v, target))),
        _ => Value::Bool(false),
    }
}

fn json_matches(a: &Json, target: &Value) -> bool {
    match target {
        Value::Int(i) => {
            matches!(a, Json::Int(j) if j == i) || matches!(a, Json::Float(f) if *f == *i as f64)
        }
        Value::Float(f) => match a {
            Json::Float(j) => j == f,
            Json::Int(j) => *j as f64 == *f,
            _ => false,
        },
        Value::Bool(b) => matches!(a, Json::Bool(j) if j == b),
        Value::Str(s) => {
            // The target is JSON when it parses as such (`"1"` is the string
            // "1", not the number), and a bare string otherwise (`b`).
            match parse(s) {
                Some(t) => a == &t,
                None => match a {
                    Json::Str(v) => v == s,
                    Json::Null => s == "null",
                    other => other.compact() == *s,
                },
            }
        }
        Value::Null => matches!(a, Json::Null),
    }
}

pub fn extract_typed(text: &str, path: &str, kind: &str) -> Value {
    let Some(root) = parse(text) else {
        return Value::Null;
    };
    let found = extract(&root, path);
    match kind {
        "json_extract" => match found {
            None => Value::Null,
            Some(Json::Null) => Value::Null,
            Some(Json::Int(i)) => Value::Int(*i),
            Some(Json::Float(f)) => Value::Float(*f),
            Some(Json::Bool(b)) => Value::Bool(*b),
            Some(Json::Str(s)) => Value::Str(format!("\"{s}\"")),
            Some(other) => Value::Str(other.compact()),
        },
        "json_extract_string" => match found {
            Some(Json::Int(i)) => Value::Str(i.to_string()),
            Some(Json::Float(f)) => Value::Str(fmt_json_float(*f)),
            Some(Json::Str(s)) => Value::Str(s.clone()),
            Some(Json::Bool(b)) => Value::Str(b.to_string()),
            Some(Json::Arr(_)) | Some(Json::Obj(_)) => Value::Str(found.unwrap().compact()),
            _ => Value::Null,
        },
        "json_extract_int" => match found {
            Some(Json::Int(i)) => Value::Int(*i),
            _ => Value::Null,
        },
        "json_extract_double" | "json_extract_float" => match found {
            Some(Json::Int(i)) => Value::Float(*i as f64),
            Some(Json::Float(f)) => Value::Float(*f),
            _ => Value::Null,
        },
        "json_extract_bool" => match found {
            Some(Json::Bool(b)) => Value::Bool(*b),
            _ => Value::Null,
        },
        "json_extract_isnull" => match found {
            None => Value::Null,
            Some(Json::Null) => Value::Bool(true),
            _ => Value::Bool(false),
        },
        "json_exists_path" => Value::Bool(found.is_some()),
        _ => Value::Null,
    }
}
