//! Minimal HTTP/1.1 JSON front-end seed (plan §2.5 / M4). Hand-rolled (no
//! framework dependency) so it builds offline; a production front-end will use a
//! real HTTP stack (axum/hyper) per the plan's dependency table.
//!
//! Routes:
//!   `GET  /health`  -> `ok`
//!   `POST /query`   body `{"sql": "..."}` -> `{"columns":[...],"rows":[[...]]}` or `{"error":"..."}`

use std::io::{Read, Write};
use std::net::{TcpListener, TcpStream};
use std::sync::Arc;

use infinity_executor::{Engine, QueryResult};

use crate::{Error, Result};

/// A tiny HTTP server that runs SQL against an [`Engine`].
pub struct HttpServer {
    listener: TcpListener,
    engine: Arc<Engine>,
}

impl HttpServer {
    pub fn bind(addr: &str, engine: Arc<Engine>) -> Result<Self> {
        let listener = TcpListener::bind(addr).map_err(|e| Error::io(addr, e))?;
        Ok(Self { listener, engine })
    }

    pub fn local_addr(&self) -> Result<String> {
        Ok(self
            .listener
            .local_addr()
            .map_err(|e| Error::io("<addr>", e))?
            .to_string())
    }

    pub fn run(self) -> ! {
        for stream in self.listener.incoming() {
            match stream {
                Ok(s) => {
                    let engine = self.engine.clone();
                    std::thread::spawn(move || {
                        let _ = handle(s, &engine);
                    });
                }
                Err(_) => continue,
            }
        }
        unreachable!()
    }
}

fn json_escape(s: &str) -> String {
    let mut out = String::with_capacity(s.len());
    for c in s.chars() {
        match c {
            '"' => out.push_str("\\\""),
            '\\' => out.push_str("\\\\"),
            '\n' => out.push_str("\\n"),
            '\r' => out.push_str("\\r"),
            '\t' => out.push_str("\\t"),
            _ => out.push(c),
        }
    }
    out
}

fn rows_json(columns: &[String], rows: &[Vec<String>]) -> String {
    let cols = columns
        .iter()
        .map(|c| format!("\"{}\"", json_escape(c)))
        .collect::<Vec<_>>()
        .join(",");
    let rs = rows
        .iter()
        .map(|r| {
            let cells = r
                .iter()
                .map(|v| format!("\"{}\"", json_escape(v)))
                .collect::<Vec<_>>()
                .join(",");
            format!("[{cells}]")
        })
        .collect::<Vec<_>>()
        .join(",");
    format!("{{\"columns\":[{cols}],\"rows\":[{rs}]}}")
}

/// Extracts the `sql` field from a tiny JSON object like `{"sql":"..."}`.
fn extract_sql(body: &str) -> Option<String> {
    let key = body.find("\"sql\"")? + 5;
    let rest = &body[key..];
    let colon = rest.find(':')? + 1;
    let mut chars = rest[colon..].chars().peekable();
    while matches!(chars.peek(), Some(c) if c.is_whitespace()) {
        chars.next();
    }
    if chars.next() != Some('"') {
        return None;
    }
    let mut out = String::new();
    let mut escaped = false;
    for c in chars {
        if escaped {
            out.push(c);
            escaped = false;
        } else if c == '\\' {
            escaped = true;
        } else if c == '"' {
            return Some(out);
        } else {
            out.push(c);
        }
    }
    None
}

// ---------------------------------------------------------------------------
// JSON (a small recursive-descent reader: the REST front-end only sees plain
// JSON documents, so a full serde dependency is unnecessary)
// ---------------------------------------------------------------------------

#[derive(Debug, Clone, PartialEq)]
pub enum Json {
    Null,
    Bool(bool),
    Num(f64),
    Str(String),
    Arr(Vec<Json>),
    Obj(Vec<(String, Json)>),
}

impl Json {
    fn get(&self, key: &str) -> Option<&Json> {
        match self {
            Json::Obj(m) => m.iter().find(|(k, _)| k == key).map(|(_, v)| v),
            _ => None,
        }
    }
    fn as_str(&self) -> Option<&str> {
        match self {
            Json::Str(s) => Some(s),
            _ => None,
        }
    }
    fn as_f64(&self) -> Option<f64> {
        match self {
            Json::Num(n) => Some(*n),
            _ => None,
        }
    }
    fn as_arr(&self) -> Option<&[Json]> {
        match self {
            Json::Arr(a) => Some(a),
            _ => None,
        }
    }
    fn as_obj(&self) -> Option<&[(String, Json)]> {
        match self {
            Json::Obj(o) => Some(o),
            _ => None,
        }
    }
}

fn parse_json(text: &str) -> Option<Json> {
    fn skip(s: &[u8], i: &mut usize) {
        while *i < s.len() && (s[*i] as char).is_whitespace() {
            *i += 1;
        }
    }
    fn value(s: &[u8], i: &mut usize) -> Option<Json> {
        skip(s, i);
        if *i >= s.len() {
            return None;
        }
        match s[*i] {
            b'{' => {
                *i += 1;
                let mut out = Vec::new();
                skip(s, i);
                if *i < s.len() && s[*i] == b'}' {
                    *i += 1;
                    return Some(Json::Obj(out));
                }
                loop {
                    skip(s, i);
                    let key = string(s, i)?;
                    skip(s, i);
                    if *i >= s.len() || s[*i] != b':' {
                        return None;
                    }
                    *i += 1;
                    let v = value(s, i)?;
                    out.push((key, v));
                    skip(s, i);
                    if *i < s.len() && s[*i] == b',' {
                        *i += 1;
                        continue;
                    }
                    break;
                }
                skip(s, i);
                if *i >= s.len() || s[*i] != b'}' {
                    return None;
                }
                *i += 1;
                Some(Json::Obj(out))
            }
            b'[' => {
                *i += 1;
                let mut out = Vec::new();
                skip(s, i);
                if *i < s.len() && s[*i] == b']' {
                    *i += 1;
                    return Some(Json::Arr(out));
                }
                loop {
                    let v = value(s, i)?;
                    out.push(v);
                    skip(s, i);
                    if *i < s.len() && s[*i] == b',' {
                        *i += 1;
                        continue;
                    }
                    break;
                }
                skip(s, i);
                if *i >= s.len() || s[*i] != b']' {
                    return None;
                }
                *i += 1;
                Some(Json::Arr(out))
            }
            b'"' => Some(Json::Str(string(s, i)?)),
            b't' if s.len() >= *i + 4 && &s[*i..*i + 4] == b"true" => {
                *i += 4;
                Some(Json::Bool(true))
            }
            b'f' if s.len() >= *i + 5 && &s[*i..*i + 5] == b"false" => {
                *i += 5;
                Some(Json::Bool(false))
            }
            b'n' if s.len() >= *i + 4 && &s[*i..*i + 4] == b"null" => {
                *i += 4;
                Some(Json::Null)
            }
            _ => {
                let start = *i;
                while *i < s.len()
                    && matches!(s[*i], b'-' | b'+' | b'.' | b'e' | b'E' | b'0'..=b'9')
                {
                    *i += 1;
                }
                std::str::from_utf8(&s[start..*i])
                    .ok()?
                    .parse::<f64>()
                    .ok()
                    .map(Json::Num)
            }
        }
    }
    fn string(s: &[u8], i: &mut usize) -> Option<String> {
        if *i >= s.len() || s[*i] != b'"' {
            return None;
        }
        *i += 1;
        let mut out: Vec<u8> = Vec::new();
        while *i < s.len() {
            let c = s[*i];
            *i += 1;
            match c {
                b'"' => return String::from_utf8(out).ok(),
                b'\\' => {
                    if *i >= s.len() {
                        return None;
                    }
                    let e = s[*i];
                    *i += 1;
                    match e {
                        b'n' => out.push(b'\n'),
                        b't' => out.push(b'\t'),
                        b'r' => out.push(b'\r'),
                        b'b' => out.push(8),
                        b'f' => out.push(12),
                        b'u' => {
                            let hex = std::str::from_utf8(s.get(*i..*i + 4)?).ok()?;
                            let cp = u32::from_str_radix(hex, 16).ok()?;
                            *i += 4;
                            let ch = char::from_u32(cp).unwrap_or('\u{fffd}');
                            let mut buf = [0u8; 4];
                            out.extend_from_slice(ch.encode_utf8(&mut buf).as_bytes());
                        }
                        other => out.push(other),
                    }
                }
                _ => out.push(c),
            }
        }
        None
    }
    let bytes = text.as_bytes();
    let mut i = 0usize;
    let v = value(bytes, &mut i)?;
    Some(v)
}

fn quote_sql(s: &str) -> String {
    format!("'{}'", s.replace('\'', "''"))
}

/// Restricts a list of identifiers to the ones we accept as names.
fn safe_ident(name: &str) -> Option<&str> {
    // Identifiers start with a letter and contain letters, digits and `_`.
    let mut chars = name.chars();
    let first_ok = chars
        .next()
        .map(|c| c.is_ascii_alphabetic())
        .unwrap_or(false);
    let rest_ok = chars.all(|c| c.is_ascii_alphanumeric() || c == '_' || c == '.');
    if first_ok && rest_ok {
        Some(name)
    } else {
        None
    }
}

/// `vector,4,float` (the REST spelling) -> `Embedding(float,4)`.
fn type_to_sql(ty: &str) -> String {
    let parts: Vec<&str> = ty.split(',').map(|s| s.trim()).collect();
    let head = parts.first().copied().unwrap_or("").to_ascii_lowercase();
    match head.as_str() {
        "integer" => "INTEGER".into(),
        "int" => "INTEGER".into(),
        "tinyint" => "TINYINT".into(),
        "smallint" => "SMALLINT".into(),
        "bigint" => "BIGINT".into(),
        "float" => "FLOAT".into(),
        "double" => "DOUBLE".into(),
        "varchar" => "VARCHAR".into(),
        "boolean" => "BOOLEAN".into(),
        "vector" => format!(
            "Embedding({},{})",
            parts.get(2).copied().unwrap_or("float"),
            parts.get(1).copied().unwrap_or("0")
        ),
        "tensor" => format!(
            "Tensor({},{})",
            parts.get(2).copied().unwrap_or("float"),
            parts.get(1).copied().unwrap_or("0")
        ),
        "tensorarray" => format!(
            "TensorArray({},{})",
            parts.get(2).copied().unwrap_or("float"),
            parts.get(1).copied().unwrap_or("0")
        ),
        "multivector" => format!(
            "MultiVector({},{})",
            parts.get(2).copied().unwrap_or("float"),
            parts.get(1).copied().unwrap_or("0")
        ),
        "sparse" => format!(
            "Sparse({},{},{})",
            parts.get(2).copied().unwrap_or("float"),
            parts.get(3).copied().unwrap_or("int8"),
            parts.get(1).copied().unwrap_or("0")
        ),
        "array" => {
            let inner = parts[1..].join(",");
            format!("Array({})", type_to_sql(&inner))
        }
        "json" => "JSON".into(),
        _ => ty.to_ascii_uppercase(),
    }
}

/// Renders a JSON value as a SQL literal; `ty` is the target column type.
fn sql_value(v: &Json, ty: &str) -> String {
    let up = ty.to_ascii_uppercase();
    match v {
        Json::Null => "NULL".into(),
        Json::Bool(b) => (if *b { "true" } else { "false" }).into(),
        Json::Num(n) => {
            if n.fract() == 0.0 && n.abs() < 9e15 {
                format!("{}", *n as i64)
            } else {
                format!("{n}")
            }
        }
        Json::Str(s) => quote_sql(s),
        Json::Arr(items) => {
            if up.starts_with("JSON") {
                quote_sql(&render_json(v))
            } else {
                let inner = items
                    .iter()
                    .map(|x| sql_value(x, &up))
                    .collect::<Vec<_>>()
                    .join(",");
                format!("[{inner}]")
            }
        }
        Json::Obj(map) => {
            if up.starts_with("SPARSE") {
                let inner = map
                    .iter()
                    .map(|(k, v)| match v {
                        Json::Num(n) => format!("{k}:{n}"),
                        Json::Str(s) => format!("{k}:{s}"),
                        _ => format!("{k}:0"),
                    })
                    .collect::<Vec<_>>()
                    .join(",");
                format!("[{inner}]")
            } else {
                quote_sql(&render_json(v))
            }
        }
    }
}

fn render_json(v: &Json) -> String {
    match v {
        Json::Null => "null".into(),
        Json::Bool(b) => b.to_string(),
        Json::Num(n) => format!("{n}"),
        Json::Str(s) => format!("\"{}\"", json_escape(s)),
        Json::Arr(a) => format!(
            "[{}]",
            a.iter().map(render_json).collect::<Vec<_>>().join(",")
        ),
        Json::Obj(o) => format!(
            "{{{}}}",
            o.iter()
                .map(|(k, v)| format!("\"{}\":{}", json_escape(k), render_json(v)))
                .collect::<Vec<_>>()
                .join(",")
        ),
    }
}

fn err_response(code: i64, msg: &str) -> (u16, String) {
    (
        500,
        format!(
            "{{\"error_code\":{code},\"error_msg\":\"{}\"}}",
            json_escape(msg)
        ),
    )
}

fn engine_err(e: &Error) -> (u16, String) {
    err_response(e.status_code(), &e.to_string())
}

/// The declared types of a table's columns (`SHOW TABLE … COLUMNS`).
fn column_types(engine: &Engine, table: &str) -> Vec<(String, String)> {
    let sql = format!("SHOW TABLE {table} COLUMNS;");
    let mut out = Vec::new();
    if let Ok(results) = engine.execute(&sql) {
        for r in results {
            if let QueryResult::Rows { rows, .. } = r {
                for row in rows {
                    if row.len() >= 2 {
                        out.push((row[0].clone(), row[1].clone()));
                    }
                }
            }
        }
    }
    out
}

/// A JSON cell for a result value, typed from the declared column type.
fn cell_json(value: &str, ty: &str) -> String {
    let up = ty.to_ascii_uppercase();
    if value.eq_ignore_ascii_case("null") || value.is_empty() {
        return "null".to_string();
    }
    let numeric = up.is_empty()
        || up.starts_with("INT")
        || up.starts_with("UINT")
        || up.starts_with("TINYINT")
        || up.starts_with("SMALLINT")
        || up.starts_with("BIGINT")
        || up.starts_with("FLOAT")
        || up.starts_with("DOUBLE")
        || up.starts_with("DECIMAL");
    if numeric {
        if let Ok(n) = value.parse::<i64>() {
            return n.to_string();
        }
        if let Ok(f) = value.parse::<f64>() {
            return format!("{f}");
        }
    }
    format!("\"{}\"", json_escape(value))
}

/// The REST spelling of a declared type (`Integer` -> `integer`).
#[allow(dead_code)]
fn rest_type_name(decl: &str) -> String {
    let up = decl.to_ascii_uppercase();
    for (prefix, name) in [
        ("TINYINT", "tinyint"),
        ("SMALLINT", "smallint"),
        ("BIGINT", "bigint"),
        ("INTEGER", "integer"),
        ("INT", "int"),
        ("FLOAT16", "float16"),
        ("BFLOAT16", "bfloat16"),
        ("FLOAT", "float"),
        ("DOUBLE", "double"),
        ("BOOLEAN", "boolean"),
        ("VARCHAR", "varchar"),
        ("JSON", "json"),
    ] {
        if up.starts_with(prefix) {
            return name.to_string();
        }
    }
    decl.to_string()
}

/// `[[{"col": v, "col_bitmap": true}, …], …]`
fn output_json(columns: &[String], types: &[String], rows: &[Vec<String>]) -> String {
    let rows_json = rows
        .iter()
        .map(|r| {
            let cells = r
                .iter()
                .enumerate()
                .map(|(i, v)| {
                    let name = columns.get(i).cloned().unwrap_or_default();
                    let ty = types.get(i).cloned().unwrap_or_default();
                    format!(
                        "{{\"{}\":{},\"{}_bitmap\":true}}",
                        json_escape(&name),
                        cell_json(v, &ty),
                        json_escape(&name)
                    )
                })
                .collect::<Vec<_>>()
                .join(",");
            format!("[{cells}]")
        })
        .collect::<Vec<_>>()
        .join(",");
    format!("[{rows_json}]")
}

fn create_option_sql(body: &Json) -> std::result::Result<(bool, bool), (u16, String)> {
    match body.get("create_option") {
        None => Ok((false, false)),
        Some(v) => match v.as_str().unwrap_or("") {
            "error" => Ok((false, false)),
            "ignore_if_exists" => Ok((true, false)),
            "replace_if_exists" => Ok((false, true)),
            other => Err(err_response(
                3074,
                &format!("invalid create option: {other}"),
            )),
        },
    }
}

fn drop_option_sql(body: &Json) -> std::result::Result<bool, (u16, String)> {
    match body.get("drop_option") {
        None => Ok(false),
        Some(v) => match v.as_str().unwrap_or("") {
            "error" => Ok(false),
            "ignore_if_not_exists" => Ok(true),
            other => Err(err_response(3074, &format!("invalid drop option: {other}"))),
        },
    }
}

/// Builds `SELECT …` for a `GET …/docs` request (the HTTP query DSL).
fn docs_sql(body: &Json, db: &str, table: &str) -> String {
    let output: Vec<String> = body
        .get("output")
        .and_then(Json::as_arr)
        .map(|a| {
            a.iter()
                .filter_map(Json::as_str)
                .map(|s| s.to_string())
                .collect()
        })
        .unwrap_or_else(|| vec!["*".to_string()]);
    let mut sql = format!("SELECT {} FROM {db}.{table}", output.join(", "));

    if let Some(h) = body.get("highlight").and_then(Json::as_arr) {
        let cols: Vec<&str> = h.iter().filter_map(Json::as_str).collect();
        if !cols.is_empty() {
            // HIGHLIGHT follows the select list
            sql = format!(
                "SELECT {} HIGHLIGHT {} FROM {db}.{table}",
                output.join(", "),
                cols.join(", ")
            );
        }
    }

    if let Some(search) = body.get("search").and_then(Json::as_arr) {
        let mut parts: Vec<String> = Vec::new();
        for e in search {
            let method = e.get("match_method").and_then(Json::as_str).unwrap_or("");
            let topn = e.get("topn").and_then(Json::as_f64).unwrap_or(10.0) as i64;
            let params = e
                .get("params")
                .and_then(Json::as_obj)
                .map(|o| {
                    o.iter()
                        .map(|(k, v)| {
                            format!(
                                "{k}={}",
                                v.as_str()
                                    .map(|s| s.to_string())
                                    .unwrap_or_else(|| render_json(v))
                            )
                        })
                        .collect::<Vec<_>>()
                        .join(";")
                })
                .unwrap_or_default();
            match method {
                "dense" => {
                    let field = e.get("fields").and_then(Json::as_str).unwrap_or("");
                    let vec = e
                        .get("query_vector")
                        .map(sql_value_arr)
                        .unwrap_or("[]".into());
                    let elem = e
                        .get("element_type")
                        .and_then(Json::as_str)
                        .unwrap_or("float");
                    let metric = e.get("metric_type").and_then(Json::as_str).unwrap_or("l2");
                    let mut opts = format!("topn={topn}");
                    if !params.is_empty() {
                        opts = format!("{opts};{params}");
                    }
                    parts.push(format!(
                        "MATCH VECTOR ({field}, {vec}, '{elem}', '{metric}', {topn}, '{}')",
                        opts.replace('\'', "''")
                    ));
                }
                "text" => {
                    let field = e.get("fields").and_then(Json::as_str).unwrap_or("");
                    let text = e.get("matching_text").and_then(Json::as_str).unwrap_or("");
                    let mut opts = format!("topn={topn}");
                    if !params.is_empty() {
                        opts = format!("{opts};{params}");
                    }
                    parts.push(format!(
                        "MATCH TEXT ('{field}', '{}', '{}')",
                        text.replace('\'', "''"),
                        opts.replace('\'', "''")
                    ));
                }
                "sparse" => {
                    let field = e.get("fields").and_then(Json::as_str).unwrap_or("");
                    let qv = e
                        .get("query_vector")
                        .map(|v| sql_value(v, "SPARSE"))
                        .unwrap_or("[]".into());
                    let metric = e.get("metric_type").and_then(Json::as_str).unwrap_or("ip");
                    let mut opts = format!("topn={topn}");
                    if !params.is_empty() {
                        opts = format!("{opts};{params}");
                    }
                    parts.push(format!(
                        "MATCH SPARSE ({field}, {qv}, '{metric}', {topn}, '{}')",
                        opts.replace('\'', "''")
                    ));
                }
                "tensor" => {
                    let field = e.get("field").and_then(Json::as_str).unwrap_or("");
                    let vec = e
                        .get("query_tensor")
                        .map(sql_value_arr)
                        .unwrap_or("[]".into());
                    let elem = e
                        .get("element_type")
                        .and_then(Json::as_str)
                        .unwrap_or("float");
                    let mut opts = format!("topn={topn}");
                    if !params.is_empty() {
                        opts = format!("{opts};{params}");
                    }
                    parts.push(format!(
                        "MATCH TENSOR ({field}, {vec}, '{elem}', 'maxsim', '{}')",
                        opts.replace('\'', "''")
                    ));
                }
                _ => {}
            }
        }
        if let Some(f) = body.get("fusion").and_then(Json::as_arr) {
            for e in f {
                let m = e
                    .get("fusion_method")
                    .and_then(Json::as_str)
                    .unwrap_or("rrf");
                parts.push(format!("FUSION('{m}')"));
            }
        }
        if !parts.is_empty() {
            sql.push_str(&format!(" SEARCH {}", parts.join(", ")));
        }
    }

    if let Some(f) = body.get("filter").and_then(Json::as_str) {
        if !f.is_empty() {
            sql.push_str(&format!(" WHERE {f}"));
        }
    }
    if let Some(g) = body.get("group_by").and_then(Json::as_arr) {
        let cols: Vec<&str> = g.iter().filter_map(Json::as_str).collect();
        if !cols.is_empty() {
            sql.push_str(&format!(" GROUP BY {}", cols.join(", ")));
        }
    }
    if let Some(h) = body.get("having").and_then(Json::as_arr) {
        let preds: Vec<&str> = h.iter().filter_map(Json::as_str).collect();
        if !preds.is_empty() {
            sql.push_str(&format!(" HAVING {}", preds.join(" AND ")));
        }
    }
    if let Some(s) = body.get("sort").and_then(Json::as_arr) {
        let keys: Vec<String> = s
            .iter()
            .filter_map(|x| {
                // `[col, "asc"]` (thrift-like) or `{col: "asc"}` (the REST SDK)
                if let Some(a) = x.as_arr() {
                    let col = a.first()?.as_str()?;
                    let dir = a.get(1).and_then(Json::as_str).unwrap_or("asc");
                    return Some(format!(
                        "{col} {}",
                        if dir.eq_ignore_ascii_case("desc") {
                            "DESC"
                        } else {
                            "ASC"
                        }
                    ));
                }
                let obj = x.as_obj()?;
                let (col, dir) = obj.first()?;
                let dir = dir.as_str().unwrap_or("asc");
                Some(format!(
                    "{col} {}",
                    if dir.eq_ignore_ascii_case("desc") {
                        "DESC"
                    } else {
                        "ASC"
                    }
                ))
            })
            .collect();
        if !keys.is_empty() {
            sql.push_str(&format!(" ORDER BY {}", keys.join(", ")));
        }
    }
    if let Some(l) = body.get("limit") {
        let n = l
            .as_f64()
            .map(|n| n as i64)
            .or_else(|| l.as_str().and_then(|s| s.parse().ok()));
        if let Some(n) = n {
            sql.push_str(&format!(" LIMIT {n}"));
        }
    }
    if let Some(o) = body.get("offset") {
        let n = o
            .as_f64()
            .map(|n| n as i64)
            .or_else(|| o.as_str().and_then(|s| s.parse().ok()));
        if let Some(n) = n {
            sql.push_str(&format!(" OFFSET {n}"));
        }
    }
    sql.push(';');
    sql
}

fn sql_value_arr(v: &Json) -> String {
    match v {
        Json::Arr(items) => format!(
            "[{}]",
            items
                .iter()
                .map(|x| match x {
                    Json::Num(n) => format!("{n}"),
                    Json::Str(s) => format!("'{}'", s.replace('\'', "''")),
                    _ => "0".to_string(),
                })
                .collect::<Vec<_>>()
                .join(",")
        ),
        _ => "[]".to_string(),
    }
}

fn rows_of(results: Vec<QueryResult>) -> (Vec<String>, Vec<Vec<String>>) {
    let mut cols = Vec::new();
    let mut rows = Vec::new();
    for r in results {
        if let QueryResult::Rows { columns, rows: rs } = r {
            cols = columns;
            rows = rs;
        }
    }
    (cols, rows)
}

fn count_of(results: Vec<QueryResult>) -> i64 {
    let mut n = 0i64;
    for r in results {
        match r {
            QueryResult::Deleted { count } | QueryResult::Inserted { count } => n += count as i64,
            _ => {}
        }
    }
    n
}

/// Routes a request to `(status, body)`.
pub fn route(engine: &Engine, method: &str, path: &str, body: &str) -> (u16, String) {
    if method == "GET" && path == "/health" {
        return (200, "ok".to_string());
    }
    if method == "POST" && path == "/query" {
        return match extract_sql(body) {
            None => (400, "{\"error\":\"missing sql\"}".to_string()),
            Some(sql) => match engine.execute(&sql) {
                Ok(results) => {
                    let (cols, rows) = rows_of(results);
                    (200, rows_json(&cols, &rows))
                }
                Err(e) => err_response(e.status_code(), &e.to_string()),
            },
        };
    }

    if std::env::var_os("INF_DEBUG_SQL").is_some() {
        eprintln!("[HTTP] {method} {path} {body}");
    }
    let doc = if body.trim().is_empty() {
        Json::Obj(Vec::new())
    } else {
        match parse_json(body) {
            Some(j) => j,
            None => return (400, "{\"error\":\"bad json\"}".to_string()),
        }
    };
    let path_only = path.split('?').next().unwrap_or(path);
    let parts: Vec<&str> = path_only
        .trim_matches('/')
        .split('/')
        .filter(|p| !p.is_empty())
        .collect();

    let run = |sql: &str| -> std::result::Result<Vec<QueryResult>, (u16, String)> {
        engine.execute(sql).map_err(|e| engine_err(&e))
    };

    match (method, parts.as_slice()) {
        // ---- databases ----
        ("GET", ["databases"]) => {
            let names: Vec<String> = engine.catalog().list_databases().unwrap_or_default();
            let list = names
                .iter()
                .map(|n| format!("\"{}\"", json_escape(n)))
                .collect::<Vec<_>>()
                .join(",");
            (
                200,
                format!("{{\"error_code\":0,\"error_msg\":\"OK\",\"databases\":[{list}]}}"),
            )
        }
        ("POST", ["databases", db]) => {
            let Some(db) = safe_ident(db) else {
                return (404, String::new());
            };
            let (ignore, replace) = match create_option_sql(&doc) {
                Ok(v) => v,
                Err(e) => return e,
            };
            if replace {
                let _ = engine.execute(&format!("DROP DATABASE IF EXISTS {db};"));
            }
            let sql = if ignore {
                format!("CREATE DATABASE IF NOT EXISTS {db};")
            } else {
                format!("CREATE DATABASE {db};")
            };
            match run(&sql) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("DELETE", ["databases", db]) => {
            let Some(db) = safe_ident(db) else {
                return (404, String::new());
            };
            let drop_ignore = match drop_option_sql(&doc) {
                Ok(v) => v,
                Err(e) => return e,
            };
            let sql = if drop_ignore {
                format!("DROP DATABASE IF EXISTS {db};")
            } else {
                format!("DROP DATABASE {db};")
            };
            match run(&sql) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("GET", ["databases", db]) => {
            let Some(db) = safe_ident(db) else {
                return (404, String::new());
            };
            let exists = engine
                .catalog()
                .list_databases()
                .unwrap_or_default()
                .iter()
                .any(|d| d.eq_ignore_ascii_case(db));
            if !exists {
                return err_response(3021, &format!("database '{db}' does not exist"));
            }
            (
                200,
                format!(
                    "{{\"error_code\":0,\"error_msg\":\"OK\",\"database_name\":\"{}\"}}",
                    json_escape(db)
                ),
            )
        }
        // ---- tables ----
        ("POST", ["databases", db, "tables", table]) => {
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return (404, String::new());
            };
            let fields = doc.get("fields").and_then(Json::as_arr).unwrap_or(&[]);
            if fields.is_empty() {
                return err_response(3013, "syntax: a table needs at least one column");
            }
            let mut cols = Vec::new();
            for f in fields {
                let name = f.get("name").and_then(Json::as_str).unwrap_or("");
                let ty = f.get("type").and_then(Json::as_str).unwrap_or("varchar");
                if safe_ident(name).is_none() {
                    return err_response(3013, "syntax: invalid column name");
                }
                let mut col = format!("{name} {}", type_to_sql(ty));
                if let Some(default) = f.get("default") {
                    let d = match default {
                        Json::Str(s) => quote_sql(s),
                        other => render_json(other),
                    };
                    col.push_str(&format!(" DEFAULT {d}"));
                }
                if let Some(cs) = f.get("constraints").and_then(Json::as_arr) {
                    for c in cs.iter().filter_map(Json::as_str) {
                        if c.eq_ignore_ascii_case("not null") {
                            col.push_str(" NOT NULL");
                        } else if c.eq_ignore_ascii_case("primary key") {
                            col.push_str(" PRIMARY KEY");
                        } else if c.eq_ignore_ascii_case("unique") {
                            col.push_str(" UNIQUE");
                        }
                    }
                }
                cols.push(col);
            }
            let (ignore, replace) = match create_option_sql(&doc) {
                Ok(v) => v,
                Err(e) => return e,
            };
            let full = format!("{db}.{table}");
            if replace {
                let _ = engine.execute(&format!("DROP TABLE IF EXISTS {full};"));
            } else if ignore
                && engine
                    .catalog()
                    .get_table_by_name(&full)
                    .ok()
                    .flatten()
                    .is_some()
            {
                return (200, ok_json());
            }
            let sql = format!("CREATE TABLE {full} ({});", cols.join(", "));
            match run(&sql) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("DELETE", ["databases", db, "tables", table]) => {
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return (404, String::new());
            };
            let drop_ignore = match drop_option_sql(&doc) {
                Ok(v) => v,
                Err(e) => return e,
            };
            let sql = format!(
                "DROP TABLE {}{db}.{table};",
                if drop_ignore { "IF EXISTS " } else { "" }
            );
            match run(&sql) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("GET", ["databases", db, "tables"]) => {
            let Some(db) = safe_ident(db) else {
                return (404, String::new());
            };
            let prefix = format!("{db}.");
            let tables: Vec<String> = engine
                .catalog()
                .list_tables()
                .unwrap_or_default()
                .into_iter()
                .filter(|t| t.name.starts_with(&prefix))
                .map(|t| t.name.trim_start_matches(&prefix).to_string())
                .collect();
            let names = tables
                .iter()
                .map(|t| format!("\"{}\"", json_escape(t)))
                .collect::<Vec<_>>()
                .join(",");
            let objs = tables
                .iter()
                .map(|t| {
                    format!(
                        "{{\"name\":\"{}\",\"table_name\":\"{}\"}}",
                        json_escape(t),
                        json_escape(t)
                    )
                })
                .collect::<Vec<_>>()
                .join(",");
            (
                200,
                format!(
                    "{{\"error_code\":0,\"error_msg\":\"OK\",\"tables\":[{objs}],\"table_names\":[{names}]}}"
                ),
            )
        }
        ("GET", ["databases", db, "tables", table]) => {
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return (404, String::new());
            };
            let full = format!("{db}.{table}");
            match engine.catalog().get_table_by_name(&full) {
                Ok(Some(meta)) => {
                    let rows = engine.row_count(&full).unwrap_or(0);
                    (
                        200,
                        format!(
                            "{{\"error_code\":0,\"error_msg\":\"OK\",\"database_name\":\"{}\",\"table_name\":\"{}\",\"storage_directory\":\"\",\"column_count\":{},\"segment_count\":1,\"row_count\":{rows}}}",
                            json_escape(db),
                            json_escape(table),
                            meta.columns.len()
                        ),
                    )
                }
                _ => err_response(3022, &format!("table '{table}' does not exist")),
            }
        }
        ("POST", ["databases", db, "tables", table, "columns"]) => {
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return (404, String::new());
            };
            let fields = match doc.get("fields").and_then(Json::as_arr) {
                Some(f) if !f.is_empty() => f,
                _ => return err_response(3013, "syntax: no column definition"),
            };
            let mut cols = Vec::new();
            for f in fields {
                let name = f.get("name").and_then(Json::as_str).unwrap_or("");
                let ty = f.get("type").and_then(Json::as_str).unwrap_or("varchar");
                if safe_ident(name).is_none() {
                    return err_response(3013, "syntax: invalid column name");
                }
                let mut def = format!("{name} {}", type_to_sql(ty));
                // `constraints: ["not null"]` is part of the column definition.
                let not_null = f
                    .get("constraints")
                    .and_then(Json::as_arr)
                    .map(|cs| {
                        cs.iter().any(|c| {
                            c.as_str()
                                .map(|s| s.eq_ignore_ascii_case("not null") || s == "not_null")
                                .unwrap_or(false)
                        })
                    })
                    .unwrap_or(false);
                if not_null {
                    def.push_str(" NOT NULL");
                }
                // A declared default must be applied to the rows already stored.
                if let Some(d) = f.get("default").map(|v| v.as_str().unwrap_or("")) {
                    def.push_str(&format!(" DEFAULT {}", crate::render_default(d)));
                }
                cols.push(def);
            }
            match run(&format!(
                "ALTER TABLE {db}.{table} ADD COLUMN ({});",
                cols.join(", ")
            )) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("DELETE", ["databases", db, "tables", table, "columns"]) => {
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return (404, String::new());
            };
            let names: Vec<&str> = doc
                .get("column_names")
                .and_then(Json::as_arr)
                .map(|a| a.iter().filter_map(Json::as_str).collect())
                .unwrap_or_default();
            let mut resp = ok_json();
            for n in names {
                if safe_ident(n).is_none() {
                    return err_response(3003, "invalid column name");
                }
                match run(&format!("ALTER TABLE {db}.{table} DROP COLUMN {n};")) {
                    Ok(_) => {}
                    Err(e) => return e,
                }
            }
            resp.push_str("");
            (200, resp)
        }
        ("GET", ["databases", db, "tables", table, "columns"]) => {
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return (404, String::new());
            };
            let sql = format!("SHOW TABLE {db}.{table} COLUMNS;");
            match run(&sql) {
                Ok(results) => {
                    let (_, rows) = rows_of(results);
                    let cols = rows
                        .iter()
                        .map(|r| {
                            format!(
                                "{{\"name\":\"{}\",\"type\":\"{}\",\"column_name\":\"{}\",\"column_type\":\"{}\",\"default\":{},\"comment\":{},\"constraints\":[]}}",
                                json_escape(r.first().map(String::as_str).unwrap_or("")),
                                json_escape(r.get(1).map(String::as_str).unwrap_or("")),
                                json_escape(r.first().map(String::as_str).unwrap_or("")),
                                json_escape(r.get(1).map(String::as_str).unwrap_or("")),
                                match r.get(2).map(String::as_str).unwrap_or("") {
                                    "" | "null" => "null".to_string(),
                                    d => format!("\"{}\"", json_escape(d)),
                                },
                                match r.get(3).map(String::as_str).unwrap_or("") {
                                    "" => "null".to_string(),
                                    c => format!("\"{}\"", json_escape(c)),
                                }
                            )
                        })
                        .collect::<Vec<_>>()
                        .join(",");
                    (
                        200,
                        format!("{{\"error_code\":0,\"error_msg\":\"OK\",\"columns\":[{cols}]}}"),
                    )
                }
                Err(e) => e,
            }
        }
        // ---- index ----
        ("POST", ["databases", db, "tables", table, "indexes", index]) => {
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return (404, String::new());
            };
            // The REST body is `{fields: [target], index: {type, …params}}`.
            let column = doc
                .get("fields")
                .and_then(Json::as_arr)
                .and_then(|a| a.first())
                .and_then(Json::as_str)
                .or_else(|| doc.get("column_name").and_then(Json::as_str))
                .unwrap_or("");
            let info = doc.get("index").cloned().unwrap_or(Json::Null);
            let kind = info.get("type").and_then(Json::as_str).unwrap_or("");
            let mut sql = format!(
                "CREATE INDEX {} {index} ON {db}.{table} ({column})",
                if create_option_sql(&doc).map(|v| v.0).unwrap_or(false) {
                    "IF NOT EXISTS"
                } else {
                    ""
                }
            );
            if !kind.is_empty() {
                sql.push_str(&format!(" USING {kind}"));
            }
            let mut opts: Vec<String> = Vec::new();
            if let Some(map) = info.as_obj() {
                for (k, v) in map {
                    if k.eq_ignore_ascii_case("type") {
                        continue;
                    }
                    let value = v
                        .as_str()
                        .map(|s| s.to_string())
                        .unwrap_or_else(|| render_json(v));
                    opts.push(format!("{k}={value}"));
                }
            }
            if !opts.is_empty() {
                sql.push_str(&format!(" WITH ({})", opts.join(",")));
            }
            sql.push(';');
            match run(&sql) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("DELETE", ["databases", db, "tables", table, "indexes", index]) => {
            let (Some(table), Some(index)) = (safe_ident(table), safe_ident(index)) else {
                return (404, String::new());
            };
            let _ = (db, table);
            let drop_ignore = drop_option_sql(&doc).unwrap_or(false);
            let sql = format!(
                "DROP INDEX {}{index};",
                if drop_ignore { "IF EXISTS " } else { "" }
            );
            match run(&sql) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("GET", ["databases", db, "tables", table, "indexes"]) => {
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return (404, String::new());
            };
            let full = format!("{db}.{table}");
            let names: Vec<String> = engine
                .catalog()
                .list_indexes()
                .unwrap_or_default()
                .into_iter()
                .filter(|i| i.table == full)
                .map(|i| i.name)
                .collect();
            let list = names
                .iter()
                .map(|n| format!("\"{}\"", json_escape(n)))
                .collect::<Vec<_>>()
                .join(",");
            (
                200,
                format!("{{\"error_code\":0,\"error_msg\":\"OK\",\"index_names\":[{list}]}}"),
            )
        }
        ("GET", ["databases", db, "tables", table, "indexes", index]) => {
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return (404, String::new());
            };
            let full = format!("{db}.{table}");
            let found = engine
                .catalog()
                .list_indexes()
                .unwrap_or_default()
                .into_iter()
                .find(|i| i.table == full && i.name.eq_ignore_ascii_case(index));
            match found {
                Some(i) => (
                    200,
                    format!(
                        "{{\"error_code\":0,\"error_msg\":\"OK\",\"database_name\":\"{}\",\"table_name\":\"{}\",\"index_name\":\"{}\",\"index_comment\":\"\",\"index_type\":\"{}\",\"index_column_names\":\"{}\",\"index_column_ids\":\"\",\"index_function_info\":\"\",\"other_parameters\":\"\",\"store_dir\":\"\",\"segment_index_count\":\"\"}}",
                        json_escape(db),
                        json_escape(table),
                        json_escape(&i.name),
                        json_escape(&i.index_type),
                        json_escape(&i.column)
                    ),
                ),
                None => err_response(3023, &format!("index '{index}' does not exist")),
            }
        }
        ("GET", ["databases", db, "tables", table, "meta"]) => {
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return (404, String::new());
            };
            let explain = match doc.get("explain_type").and_then(Json::as_str) {
                Some(t) => t.to_ascii_uppercase(),
                None => "PHYSICAL".into(),
            };
            let output: Vec<String> = doc
                .get("output")
                .and_then(Json::as_arr)
                .map(|a| {
                    a.iter()
                        .filter_map(Json::as_str)
                        .map(String::from)
                        .collect()
                })
                .unwrap_or_else(|| vec!["*".into()]);
            let filter = doc.get("filter").and_then(Json::as_str).unwrap_or("");
            let mut inner = format!("SELECT {} FROM {db}.{table}", output.join(", "));
            if !filter.is_empty() {
                inner.push_str(&format!(" WHERE {filter}"));
            }
            let kind = match explain.as_str() {
                "LOGICAL" => "LOGICAL",
                "OPT" | "UNOPT" => "LOGICAL",
                "PIPELINE" | "FRAGMENT" => "PHYSICAL",
                _ => "PHYSICAL",
            };
            let sql = format!("EXPLAIN {kind} {inner};");
            match run(&sql) {
                Ok(results) => {
                    let (cols, rows) = rows_of(results);
                    // the SDK renders every value under its column name
                    let mut out = String::new();
                    if let Some(first) = rows.first() {
                        for (i, v) in first.iter().enumerate() {
                            let name = cols.get(i).cloned().unwrap_or_else(|| "explain".into());
                            if i > 0 {
                                out.push(',');
                            }
                            out.push_str(&format!(
                                "{{\"{}\":\"{}\"}}",
                                json_escape(&name),
                                json_escape(v)
                            ));
                        }
                    }
                    (
                        200,
                        format!("{{\"error_code\":0,\"error_msg\":\"OK\",\"output\":[{out}]}}"),
                    )
                }
                Err(e) => e,
            }
        }
        // ---- import / export / rename / compact / optimize ----
        ("PUT", ["databases", db, "tables", table]) => {
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return (404, String::new());
            };
            let path = quote_sql(doc.get("file_path").and_then(Json::as_str).unwrap_or(""));
            let format = doc
                .get("file_type")
                .and_then(Json::as_str)
                .unwrap_or("csv")
                .to_ascii_uppercase();
            let delim = doc.get("delimiter").and_then(Json::as_str).unwrap_or(",");
            let header = doc
                .get("header")
                .and_then(|v| match v {
                    Json::Bool(b) => Some(*b),
                    _ => None,
                })
                .unwrap_or(false);
            let sql = format!(
                "COPY {db}.{table} FROM {path} WITH (DELIMITER '{}', FORMAT {format}, HEADER {header});",
                delim.replace('\'', "''")
            );
            match run(&sql) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("GET", ["databases", db, "table", table]) => {
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return (404, String::new());
            };
            let path = quote_sql(doc.get("file_path").and_then(Json::as_str).unwrap_or(""));
            let format = doc
                .get("file_type")
                .and_then(Json::as_str)
                .unwrap_or("csv")
                .to_ascii_uppercase();
            let delim = doc.get("delimiter").and_then(Json::as_str).unwrap_or(",");
            let header = doc
                .get("header")
                .and_then(|v| match v {
                    Json::Bool(b) => Some(*b),
                    _ => None,
                })
                .unwrap_or(false);
            let sql = format!(
                "COPY {db}.{table} TO {path} WITH (DELIMITER '{}', FORMAT {format}, HEADER {header});",
                delim.replace('\'', "''")
            );
            match run(&sql) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("POST", ["databases", db, "tables", table, "rename"]) => {
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return (404, String::new());
            };
            let new_name = doc
                .get("new_table_name")
                .and_then(Json::as_str)
                .unwrap_or("");
            let Some(new_name) = safe_ident(new_name) else {
                return err_response(3003, "invalid table name");
            };
            match run(&format!("ALTER TABLE {db}.{table} RENAME TO {new_name};")) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("PUT", ["databases", db, "tables", table, "compact"]) => {
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return (404, String::new());
            };
            match run(&format!("COMPACT TABLE {db}.{table};")) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("PUT", ["databases", db, "tables", table, "optimize"]) => {
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return (404, String::new());
            };
            match run(&format!("COMPACT TABLE {db}.{table};")) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("GET", ["databases", db, "tables", table, "segments"]) => {
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return (404, String::new());
            };
            // one segment per table in this engine; report its blocks
            let rows = engine.row_count(&format!("{db}.{table}")).unwrap_or(0);
            (
                200,
                format!(
                    "{{\"error_code\":0,\"error_msg\":\"OK\",\"segments\":[{{\"segment_id\":0,\"row_count\":{rows},\"block_count\":1,\"status\":\"sealed\"}}]}}"
                ),
            )
        }
        ("GET", ["databases", db, "tables", table, "segments", _seg, "blocks"]) => {
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return (404, String::new());
            };
            let rows = engine.row_count(&format!("{db}.{table}")).unwrap_or(0);
            (
                200,
                format!(
                    "{{\"error_code\":0,\"error_msg\":\"OK\",\"blocks\":[{{\"block_id\":0,\"row_count\":{rows}}}]}}"
                ),
            )
        }
        // ---- docs: insert / delete / update / search ----
        ("POST", ["databases", db, "tables", table, "docs"]) => {
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return (404, String::new());
            };
            let full = format!("{db}.{table}");
            let types = column_types(engine, &full);
            let ty_of = |name: &str| -> String {
                types
                    .iter()
                    .find(|(c, _)| c.eq_ignore_ascii_case(name))
                    .map(|(_, t)| t.clone())
                    .unwrap_or_default()
            };
            let rows = match &doc {
                Json::Arr(rs) => rs.clone(),
                Json::Obj(_) => vec![doc.clone()],
                _ => Vec::new(),
            };
            if rows.is_empty() {
                return err_response(3013, "insert without values");
            }
            let mut n = 0i64;
            for row in rows {
                let Some(fields) = row.as_obj() else {
                    return err_response(3013, "insert row must be an object");
                };
                if fields.is_empty() {
                    continue;
                }
                let cols: Vec<String> = fields.iter().map(|(k, _)| k.clone()).collect();
                let vals: Vec<String> = fields
                    .iter()
                    .map(|(k, v)| sql_value(v, &ty_of(k)))
                    .collect();
                let sql = format!(
                    "INSERT INTO {full} ({}) VALUES ({});",
                    cols.join(","),
                    vals.join(",")
                );
                match run(&sql) {
                    Ok(_) => n += 1,
                    Err(e) => return e,
                }
            }
            (
                200,
                format!("{{\"error_code\":0,\"error_msg\":\"OK\",\"inserted_rows\":{n}}}"),
            )
        }
        ("DELETE", ["databases", db, "tables", table, "docs"]) => {
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return (404, String::new());
            };
            let filter = doc.get("filter").and_then(Json::as_str).unwrap_or("");
            let sql = format!(
                "DELETE FROM {db}.{table}{};",
                if filter.is_empty() {
                    String::new()
                } else {
                    format!(" WHERE {filter}")
                }
            );
            match run(&sql) {
                Ok(results) => (
                    200,
                    format!(
                        "{{\"error_code\":0,\"error_msg\":\"OK\",\"deleted_rows\":{}}}",
                        count_of(results)
                    ),
                ),
                Err(e) => e,
            }
        }
        ("PUT", ["databases", db, "tables", table, "docs"]) => {
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return (404, String::new());
            };
            let filter = doc.get("filter").and_then(Json::as_str).unwrap_or("");
            let Some(update) = doc.get("update").and_then(Json::as_obj) else {
                return err_response(3013, "update needs values");
            };
            let full = format!("{db}.{table}");
            let types = column_types(engine, &full);
            let sets = update
                .iter()
                .map(|(k, v)| {
                    let ty = types
                        .iter()
                        .find(|(c, _)| c.eq_ignore_ascii_case(k))
                        .map(|(_, t)| t.clone())
                        .unwrap_or_default();
                    format!("{k} = {}", sql_value(v, &ty))
                })
                .collect::<Vec<_>>()
                .join(", ");
            let sql = format!(
                "UPDATE {full} SET {sets}{};",
                if filter.is_empty() {
                    String::new()
                } else {
                    format!(" WHERE {filter}")
                }
            );
            match run(&sql) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("GET", ["databases", db, "tables", table, "docs"]) => {
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return (404, String::new());
            };
            let full = format!("{db}.{table}");
            let types = column_types(engine, &full);
            let sql = docs_sql(&doc, db, table);
            match run(&sql) {
                Ok(results) => {
                    let (cols, rows) = rows_of(results);
                    let out_types: Vec<String> = cols
                        .iter()
                        .map(|c| {
                            types
                                .iter()
                                .find(|(n, _)| n.eq_ignore_ascii_case(c))
                                .map(|(_, t)| t.clone())
                                .unwrap_or_default()
                        })
                        .collect();
                    let mut extra = String::new();
                    if let Some(opt) = doc.get("option") {
                        if opt
                            .get("total_hits_count")
                            .and_then(|v| match v {
                                Json::Bool(b) => Some(*b),
                                _ => None,
                            })
                            .unwrap_or(false)
                        {
                            // count the same query without LIMIT/OFFSET
                            let mut count_sql = sql.clone();
                            if let Some(pos) = count_sql.rfind(" LIMIT ") {
                                count_sql.truncate(pos);
                            }
                            count_sql = count_sql.replace("SELECT ", "SELECT count(*) AS n ");
                            let total = match engine.execute(&count_sql) {
                                Ok(rs) => {
                                    let (_, rows) = rows_of(rs);
                                    rows.first()
                                        .and_then(|r| r.first())
                                        .and_then(|v| v.parse::<i64>().ok())
                                        .unwrap_or(0)
                                }
                                Err(_) => 0,
                            };
                            extra = format!(",\"total_hits_count\":{total}");
                        }
                    }
                    (
                        200,
                        format!(
                            "{{\"error_code\":0,\"error_msg\":\"OK\",\"output\":{}{extra}}}",
                            output_json(&cols, &out_types, &rows)
                        ),
                    )
                }
                Err(e) => e,
            }
        }
        _ => (404, "{\"error\":\"not found\"}".to_string()),
    }
}

fn ok_json() -> String {
    "{\"error_code\":0,\"error_msg\":\"OK\"}".to_string()
}

fn handle(mut stream: TcpStream, engine: &Engine) -> Result<()> {
    let mut buf = Vec::new();
    let mut tmp = [0u8; 1024];
    // read until end of headers
    let header_end = loop {
        let n = stream.read(&mut tmp).map_err(|e| Error::io("<http>", e))?;
        if n == 0 {
            return Ok(());
        }
        buf.extend_from_slice(&tmp[..n]);
        if let Some(pos) = find_subslice(&buf, b"\r\n\r\n") {
            break pos + 4;
        }
    };
    let head = String::from_utf8_lossy(&buf[..header_end]).to_string();
    let mut lines = head.lines();
    let request_line = lines.next().unwrap_or("");
    let mut parts = request_line.split_whitespace();
    let method = parts.next().unwrap_or("");
    let path = parts.next().unwrap_or("");

    let content_length = head
        .lines()
        .find_map(|l| {
            let l = l.to_ascii_lowercase();
            l.strip_prefix("content-length:")
                .map(|v| v.trim().parse::<usize>().unwrap_or(0))
        })
        .unwrap_or(0);
    while buf.len() < header_end + content_length {
        let n = stream.read(&mut tmp).map_err(|e| Error::io("<http>", e))?;
        if n == 0 {
            break;
        }
        buf.extend_from_slice(&tmp[..n]);
    }
    let body = String::from_utf8_lossy(&buf[header_end..]).to_string();

    let (status, resp_body) = route(engine, method, path, &body);
    let reason = if status == 200 { "OK" } else { "Error" };
    let response = format!(
        "HTTP/1.1 {status} {reason}\r\nContent-Type: application/json\r\nContent-Length: {}\r\nConnection: close\r\n\r\n{}",
        resp_body.len(),
        resp_body
    );
    stream
        .write_all(response.as_bytes())
        .map_err(|e| Error::io("<http>", e))?;
    stream.flush().map_err(|e| Error::io("<http>", e))?;
    Ok(())
}

fn find_subslice(hay: &[u8], needle: &[u8]) -> Option<usize> {
    hay.windows(needle.len()).position(|w| w == needle)
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::tempdir;

    #[test]
    fn route_query_and_health() {
        let dir = tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute("CREATE TABLE t (id INT, name VARCHAR);")
            .unwrap();
        engine.execute("INSERT INTO t VALUES (1, 'a');").unwrap();

        let (s, b) = route(&engine, "POST", "/query", "{\"sql\":\"SELECT * FROM t;\"}");
        assert_eq!(s, 200);
        assert!(b.contains("\"columns\":[\"id\",\"name\"]"), "{b}");
        assert!(b.contains("[\"1\",\"a\"]"), "{b}");

        assert_eq!(
            route(&engine, "GET", "/health", ""),
            (200, "ok".to_string())
        );
        assert_eq!(route(&engine, "POST", "/query", "{}").0, 400);
        assert_eq!(route(&engine, "GET", "/nope", "").0, 404);
    }

    #[test]
    fn http_over_socket() {
        let dir = tempdir().unwrap();
        let engine = Arc::new(Engine::open(dir.path()).unwrap());
        engine.execute("CREATE TABLE t (id INT);").unwrap();
        engine.execute("INSERT INTO t VALUES (42);").unwrap();
        let server = HttpServer::bind("127.0.0.1:0", engine).unwrap();
        let addr = server.local_addr().unwrap();
        std::thread::spawn(move || server.run());

        let mut stream = TcpStream::connect(&addr).unwrap();
        let body = "{\"sql\":\"SELECT * FROM t;\"}";
        let req = format!(
            "POST /query HTTP/1.1\r\nHost: x\r\nContent-Length: {}\r\n\r\n{}",
            body.len(),
            body
        );
        stream.write_all(req.as_bytes()).unwrap();
        let mut resp = String::new();
        stream.read_to_string(&mut resp).unwrap();
        assert!(resp.starts_with("HTTP/1.1 200"), "{resp}");
        assert!(resp.contains("[\"42\"]"), "{resp}");
    }
}
