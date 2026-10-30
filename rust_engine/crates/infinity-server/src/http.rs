//! Minimal HTTP/1.1 JSON front-end seed (plan §2.5 / M4). Hand-rolled (no
//! framework dependency) so it builds offline; a production front-end will use a
//! real HTTP stack (axum/hyper) per the plan's dependency table.
//!
//! Routes mirror `src/network/http_server_impl.cpp` (§4249-4357): the
//! database/table/column/index/docs families, `SHOW SEGMENTS`/`SHOW BLOCKS`,
//! `/configs`, `/snapshots/*`, `/variables/*`, `/instance/*` and `/admin/*`.
//! A route whose data has no engine accessor yet answers with the reference's
//! error envelope and error code 3032 (`kNotSupported`) instead of a
//! fabricated value.
//!
//!   `GET  /health`  -> `ok`
//!   `POST /query`   body `{"sql": "..."}` -> `{"columns":[...],"rows":[[...]]}` or `{"error":"..."}`

use std::io::{Read, Write};
use std::net::{TcpListener, TcpStream};
use std::sync::Arc;

use infinity_executor::{Engine, QueryResult};

use crate::peer::{NodeRole, PeerManager};
use crate::{
    show_blocks_rows, show_segments_rows, Error, Result, SHOW_BLOCKS_COLUMNS, SHOW_SEGMENTS_COLUMNS,
};

/// A tiny HTTP server that runs SQL against an [`Engine`].
pub struct HttpServer {
    listener: TcpListener,
    engine: Arc<Engine>,
    /// Cluster state behind `/admin/node*` (C++ `AdminShowCurrentNodeHandler`
    /// &co). A standalone HTTP front end is the leader of an empty cluster,
    /// the same node `Server::bind` registers for `SHOW CURRENT NODE`.
    peer: Arc<PeerManager>,
}

impl HttpServer {
    pub fn bind(addr: &str, engine: Arc<Engine>) -> Result<Self> {
        let listener = TcpListener::bind(addr).map_err(|e| Error::io(addr, e))?;
        let bound = listener.local_addr().map_err(|e| Error::io(addr, e))?;
        let peer = Arc::new(PeerManager::new(
            "leader",
            bound.ip().to_string(),
            i64::from(bound.port()),
        ));
        Ok(Self {
            listener,
            engine,
            peer,
        })
    }

    /// The peer/cluster registry the admin routes report.
    pub fn peer(&self) -> Arc<PeerManager> {
        Arc::clone(&self.peer)
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
                    let _ = s.set_nodelay(true);
                    let engine = self.engine.clone();
                    let peer = self.peer.clone();
                    std::thread::spawn(move || {
                        let _ = handle(s, &engine, &peer);
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

/// Validates an identifier and normalises it to lower case (the catalog keeps
/// canonical lower-case names).
fn ident(name: &str) -> Option<String> {
    safe_ident(name).map(|n| n.to_ascii_lowercase())
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

/// 3032 `kNotSupported` (`src/common/status.cppm:83`).
const NOT_SUPPORTED: i64 = 3032;

/// The reference's error envelope for a route this build cannot serve: the
/// value would have to be fabricated, so the route says what data it needs
/// instead. `needs` names the engine accessor that would make it real.
fn not_supported(route: &str, needs: &str) -> (u16, String) {
    (
        501,
        format!(
            "{{\"error_code\":{NOT_SUPPORTED},\"error_msg\":\"{} is not supported in this build: needs {}\"}}",
            json_escape(route),
            json_escape(needs)
        ),
    )
}

/// The configuration knobs `SHOW CONFIGS` reports before anything is set
/// (`Engine::config_get`'s defaults), in the reference's alphabetical order.
const CONFIG_KEYS: [&str; 5] = [
    "cache_result_capacity",
    "checkpoint_interval",
    "cleanup_interval",
    "log_level",
    "result_cache",
];

/// `ShowConfigsHandler`: `{"<config>": "<value>", …, "error_code": 0}`.
fn configs_json(engine: &Engine) -> String {
    let mut pairs: std::collections::BTreeMap<String, String> = std::collections::BTreeMap::new();
    for key in CONFIG_KEYS {
        if let Some(v) = engine.config_get(key) {
            pairs.insert(key.to_string(), v);
        }
    }
    for (k, v) in engine.config_all() {
        pairs.insert(k, v);
    }
    let body = pairs
        .iter()
        .map(|(k, v)| format!("\"{}\":\"{}\"", json_escape(k), json_escape(v)))
        .collect::<Vec<_>>()
        .join(",");
    if body.is_empty() {
        "{\"error_code\":0}".to_string()
    } else {
        format!("{{{body},\"error_code\":0}}")
    }
}

/// The single-field body of `POST /configs` (`SetConfigHandler`): the name and
/// its JSON literal, or the reference's 3076 "No config will be set".
fn single_field(doc: &Json) -> Option<(String, String)> {
    let obj = doc.as_obj()?;
    if obj.len() != 1 {
        return None;
    }
    let (name, value) = &obj[0];
    let literal = match value {
        Json::Str(s) => quote_sql(s),
        Json::Num(n) if n.fract() == 0.0 => format!("{}", *n as i64),
        Json::Num(n) => format!("{n}"),
        Json::Bool(b) => b.to_string(),
        _ => return None,
    };
    Some((name.clone(), literal))
}

/// Appends the rows of a `SHOW …` result as `(column, value)` pairs — the
/// `/variables/<scope>` body is one flat object, not a list.
/// Quotes a snapshot name for SQL. `CREATE SNAPSHOT "snap-1" …` is the form
/// the parser accepts for names that are not bare identifiers.
fn snapshot_sql(name: &str) -> String {
    if safe_ident(name).is_some() {
        name.to_string()
    } else {
        format!("\"{}\"", name.replace('"', "\"\""))
    }
}

/// `NodeInfo` as the reference's node objects (`AdminListAllNodesHandler`).
fn node_json(node: &crate::peer::NodeInfo) -> String {
    format!(
        "{{\"name\":\"{}\",\"role\":\"{}\",\"status\":\"{}\",\"address\":\"{}:{}\",\"last_update\":\"{}\",\"heartbeat\":{}}}",
        json_escape(node.node_name()),
        json_escape(&node.node_role().to_string()),
        json_escape(&node.node_status().to_string()),
        json_escape(node.node_ip()),
        node.node_port(),
        node.update_ts(),
        node.heartbeat_count()
    )
}

/// Maps the `role` field of `POST /admin/node/current` onto a [`NodeRole`].
fn parse_role(text: &str) -> Option<NodeRole> {
    match text.to_ascii_lowercase().as_str() {
        "admin" => Some(NodeRole::Admin),
        "standalone" => Some(NodeRole::Standalone),
        "leader" => Some(NodeRole::Leader),
        "follower" => Some(NodeRole::Follower),
        "learner" => Some(NodeRole::Learner),
        _ => None,
    }
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
                        out.push((row[0].to_slt_string(), row[1].to_slt_string()));
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
    if value.eq_ignore_ascii_case("null") {
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
/// The result type of a projected expression the engine names after itself
/// (`trunc(c1, 14)` yields text). Unknown expressions stay untyped.
fn expr_result_type(name: &str) -> String {
    let n = name.trim().to_ascii_lowercase();
    let f = n.split('(').next().unwrap_or("");
    match f {
        "trunc"
        | "truncate"
        | "upper"
        | "lower"
        | "substr"
        | "substring"
        | "trim"
        | "ltrim"
        | "rtrim"
        | "reverse"
        | "md5"
        | "concat"
        | "json_extract"
        | "json_extract_string"
        | "json_keys" => "Varchar".to_string(),
        _ => String::new(),
    }
}

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
                        "{{\"{}\":{},\"{}_bitmap\":{}}}",
                        json_escape(&name),
                        cell_json(v, &ty),
                        json_escape(&name),
                        !v.eq_ignore_ascii_case("null")
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
            // The reference does not implement replace yet.
            "replace_if_exists" => Err(err_response(3032, "not supported: replace_if_exists")),
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
            other => Err(err_response(3075, &format!("invalid drop option: {other}"))),
        },
    }
}

/// Accepts either a bare string (`"c1"`, `"c1,c2"`) or an array of strings.
fn str_list(v: &Json) -> Vec<String> {
    match v {
        Json::Str(s) => s
            .split(',')
            .map(str::trim)
            .filter(|p| !p.is_empty())
            .map(str::to_string)
            .collect(),
        Json::Arr(a) => a
            .iter()
            .filter_map(Json::as_str)
            .map(str::to_string)
            .collect(),
        _ => Vec::new(),
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
    let named: Vec<String> = output.iter().map(|o| named_output(o)).collect();
    let mut sql = format!("SELECT {} FROM {db}.{table}", named.join(", "));

    if let Some(h) = body.get("highlight").and_then(Json::as_arr) {
        let cols: Vec<&str> = h.iter().filter_map(Json::as_str).collect();
        if !cols.is_empty() {
            // HIGHLIGHT follows the select list
            sql = format!(
                "SELECT {} HIGHLIGHT {} FROM {db}.{table}",
                named.join(", "),
                cols.join(", ")
            );
        }
    }

    if let Some(search) = body.get("search").and_then(Json::as_arr) {
        let mut parts: Vec<String> = Vec::new();
        for e in search {
            let method = e.get("match_method").and_then(Json::as_str).unwrap_or("");
            // An integer topn ≥ 1; anything else becomes -1 so the engine
            // reports the reference's invalid-topn error.
            let topn: i64 = match e.get("topn") {
                None => 10,
                Some(Json::Num(n)) if n.fract() == 0.0 && *n >= 1.0 => *n as i64,
                Some(_) => -1,
            };
            // `params` is a dict in the SDK, but tests also send the raw
            // `topn=…` string (or a scalar); anything that is not a dict or a
            // string must be rejected by the engine, so it becomes an unknown
            // option.
            let params = match e.get("params") {
                None | Some(Json::Null) => String::new(),
                Some(Json::Str(s)) => s.clone(),
                Some(Json::Obj(o)) => o
                    .iter()
                    .map(|(k, v)| {
                        format!(
                            "{k}={}",
                            v.as_str()
                                .map(|s| s.to_string())
                                .unwrap_or_else(|| render_json(v))
                        )
                    })
                    .collect::<Vec<_>>()
                    .join(";"),
                Some(_) => "__invalid_param__".to_string(),
            };
            match method {
                "dense" => {
                    let field = e.get("fields").and_then(Json::as_str).unwrap_or("");
                    let vec = e
                        .get("query_vector")
                        .map(sql_value_arr)
                        .unwrap_or("[]".into());
                    let elem = match e.get("element_type") {
                        None => "float".to_string(),
                        Some(Json::Str(s)) => s.clone(),
                        Some(_) => "__invalid__".to_string(),
                    };
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
                    let field = match e.get("fields") {
                        None => "",
                        Some(Json::Str(s)) => s,
                        // A non-string field list is invalid; the engine will
                        // report the unknown column.
                        Some(_) => "__invalid__",
                    };
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
                    let elem = match e.get("element_type") {
                        None => "float".to_string(),
                        Some(Json::Str(s)) => s.clone(),
                        // A non-string type is invalid; let the engine reject it.
                        Some(_) => "__invalid__".to_string(),
                    };
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
            sql.push_str(&format!(" WHERE {}", rewrite_pseudo(f)));
        }
    }
    if let Some(g) = body.get("group_by") {
        let cols = str_list(g);
        if !cols.is_empty() {
            let cols: Vec<String> = cols.iter().map(|c| rewrite_pseudo(c)).collect();
            sql.push_str(&format!(" GROUP BY {}", cols.join(", ")));
        }
    }
    if let Some(h) = body.get("having") {
        let preds = str_list(h);
        if !preds.is_empty() {
            let preds: Vec<String> = preds.iter().map(|p| rewrite_pseudo(p)).collect();
            sql.push_str(&format!(" HAVING {}", preds.join(" AND ")));
        }
    }
    if let Some(s) = body.get("sort").and_then(Json::as_arr) {
        let keys: Vec<String> = s
            .iter()
            .filter_map(|x| {
                // `[col, "asc"]` (thrift-like) or `{col: "asc"}` (the REST SDK)
                if let Some(a) = x.as_arr() {
                    let col = rewrite_pseudo(a.first()?.as_str()?);
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
                let col = rewrite_pseudo(col);
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

/// The REST DSL's pseudo-columns and the functions they stand for.
const PSEUDO_COLUMNS: [(&str, &str); 3] = [
    // `_row_id` is a real (hidden) column the planner resolves; only the
    // search pseudo-columns need a function spelling.
    ("_score", "score()"),
    ("_distance", "distance()"),
    ("_similarity", "similarity()"),
];

/// Replaces `_row_id`/`_score`/… inside a filter/sort/group expression.
fn rewrite_pseudo(text: &str) -> String {
    let mut out = text.to_string();
    for (alias, expr) in PSEUDO_COLUMNS {
        let mut from = 0usize;
        while let Some(pos) = out[from..].find(alias) {
            let start = from + pos;
            let end = start + alias.len();
            let before_ok = start == 0
                || !out.as_bytes()[start - 1].is_ascii_alphanumeric()
                    && out.as_bytes()[start - 1] != b'_';
            let after_ok = end >= out.len()
                || !out.as_bytes()[end].is_ascii_alphanumeric() && out.as_bytes()[end] != b'_';
            if before_ok && after_ok {
                out.replace_range(start..end, expr);
                from = start + expr.len();
            } else {
                from = end;
            }
        }
    }
    out
}

/// Wraps a bare arithmetic expression so the engine names it `(a + b)`, as
/// the reference does (`(c1 + c2)`).
fn named_output(o: &str) -> String {
    let t = o.trim();
    // The REST DSL names the pseudo-columns with a leading underscore.
    for (alias, expr) in PSEUDO_COLUMNS {
        if t.eq_ignore_ascii_case(alias) {
            return expr.to_string();
        }
    }
    if t.starts_with('(') || t == "*" || t == "count(*)" {
        return t.to_string();
    }
    let has_op = [" + ", " - ", " * ", " / ", " % "]
        .iter()
        .any(|op| t.contains(op));
    if has_op {
        format!("({t})")
    } else {
        t.to_string()
    }
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
                    // A tensor query is a list of sub-vectors; recurse so the
                    // nesting survives (a flat `[0,0]` scores every row 0).
                    Json::Arr(_) => sql_value_arr(x),
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
            rows = rs
                .iter()
                .map(|r| r.iter().map(|v| v.to_slt_string()).collect())
                .collect();
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
pub fn route(
    engine: &Engine,
    peer: &PeerManager,
    method: &str,
    path: &str,
    body: &str,
) -> (u16, String) {
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
            let Some(db) = ident(db) else {
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
            let Some(db) = ident(db) else {
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
            let Some(db) = ident(db) else {
                return (404, String::new());
            };
            let exists = engine
                .catalog()
                .list_databases()
                .unwrap_or_default()
                .iter()
                .any(|d| d.eq_ignore_ascii_case(&db));
            if !exists {
                return err_response(3021, &format!("database '{db}' does not exist"));
            }
            (
                200,
                format!(
                    "{{\"error_code\":0,\"error_msg\":\"OK\",\"database_name\":\"{}\"}}",
                    json_escape(&db)
                ),
            )
        }
        // ---- tables ----
        ("POST", ["databases", db, "tables", table]) => {
            let Some(db) = ident(db) else {
                return err_response(3003, "invalid database name");
            };
            let Some(table) = ident(table) else {
                return err_response(3004, "invalid table name");
            };
            let fields = doc.get("fields").and_then(Json::as_arr).unwrap_or(&[]);
            if fields.is_empty() {
                return err_response(3048, "no column defined");
            }
            let mut cols = Vec::new();
            for f in fields {
                let name = f.get("name").and_then(Json::as_str).unwrap_or("");
                let ty = f.get("type").and_then(Json::as_str).unwrap_or("varchar");
                if name.is_empty() {
                    return err_response(3042, "empty column name");
                }
                if safe_ident(name).is_none() {
                    return err_response(3005, &format!("invalid column name '{name}'"));
                }
                let name = name.to_ascii_lowercase();
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
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
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
            let Some(db) = ident(db) else {
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
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
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
                            json_escape(&db),
                            json_escape(&table),
                            meta.columns.len()
                        ),
                    )
                }
                _ => err_response(3022, &format!("table '{table}' does not exist")),
            }
        }
        ("POST", ["databases", db, "tables", table, "columns"]) => {
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
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
                if let Some(d) = f.get("default") {
                    let lit = match d {
                        Json::Str(s) => quote_sql(s),
                        other => render_json(other),
                    };
                    def.push_str(&format!(" DEFAULT {lit}"));
                }
                cols.push(def);
            }
            let stmt = format!("ALTER TABLE {db}.{table} ADD COLUMN ({});", cols.join(", "));
            match run(&stmt) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("DELETE", ["databases", db, "tables", table, "columns"]) => {
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
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
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
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
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
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
                    opts.push(format!("{k}='{}'", value.replace('\'', "''")));
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
        // `ALTER INDEX … SET (topk=…)` only tunes a runtime hint; accept it
        // (after checking the index exists) without changing the mirror.
        ("PUT", ["databases", db, "tables", table, "indexes", index]) => {
            let (Some(_db), Some(table), Some(index)) = (ident(db), ident(table), ident(index))
            else {
                return (404, String::new());
            };
            let _ = table;
            let exists = engine
                .catalog()
                .list_indexes()
                .map(|is| is.iter().any(|i| i.name == index))
                .unwrap_or(false);
            if !exists {
                return err_response(3023, &format!("index '{index}' does not exist"));
            }
            (200, ok_json())
        }
        ("DELETE", ["databases", db, "tables", table, "indexes", index]) => {
            let (Some(table), Some(index)) = (ident(table), ident(index)) else {
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
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
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
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
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
                        json_escape(&db),
                        json_escape(&table),
                        json_escape(&i.name),
                        json_escape(&i.index_type),
                        json_escape(&i.column)
                    ),
                ),
                None => err_response(3023, &format!("index '{index}' does not exist")),
            }
        }
        ("GET", ["databases", db, "tables", table, "meta"]) => {
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
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
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
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
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
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
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
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
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
                return (404, String::new());
            };
            match run(&format!("COMPACT TABLE {db}.{table};")) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("PUT", ["databases", db, "tables", table, "optimize"]) => {
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
                return (404, String::new());
            };
            match run(&format!("COMPACT TABLE {db}.{table};")) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("GET", ["databases", db, "tables", table, "segments"]) => {
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
                return (404, String::new());
            };
            // `ShowSegmentsListHandler`: the `SHOW SEGMENTS` rows, plus the
            // table name.
            let rows = show_segments_rows(engine, &format!("{db}.{table}"));
            let items = rows
                .iter()
                .map(|row| {
                    let fields: Vec<String> = row
                        .iter()
                        .enumerate()
                        .map(|(i, v)| {
                            format!(
                                "\"{}\":\"{}\"",
                                json_escape(SHOW_SEGMENTS_COLUMNS[i]),
                                json_escape(v)
                            )
                        })
                        .collect();
                    format!("{{{}}}", fields.join(","))
                })
                .collect::<Vec<_>>()
                .join(",");
            (
                200,
                format!(
                    "{{\"error_code\":0,\"table_name\":\"{}\",\"segments\":[{}]}}",
                    json_escape(&table),
                    items
                ),
            )
        }
        ("GET", ["databases", db, "tables", table, "segments", seg, "blocks"]) => {
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
                return (404, String::new());
            };
            let Ok(segment) = seg.parse::<u32>() else {
                return err_response(3037, "invalid segment id");
            };
            // `ShowBlocksListHandler`.
            let rows = show_blocks_rows(engine, &format!("{db}.{table}"), segment);
            let items = rows
                .iter()
                .map(|row| {
                    let fields: Vec<String> = row
                        .iter()
                        .enumerate()
                        .map(|(i, v)| {
                            format!(
                                "\"{}\":\"{}\"",
                                json_escape(SHOW_BLOCKS_COLUMNS[i]),
                                json_escape(v)
                            )
                        })
                        .collect();
                    format!("{{{}}}", fields.join(","))
                })
                .collect::<Vec<_>>()
                .join(",");
            (
                200,
                format!(
                    "{{\"error_code\":0,\"segment_id\":{segment},\"blocks\":[{}]}}",
                    items
                ),
            )
        }
        // ---- docs: insert / delete / update / search ----
        ("POST", ["databases", db, "tables", table, "docs"]) => {
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
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
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
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
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
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
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
                return (404, String::new());
            };
            let full = format!("{db}.{table}");
            let types = column_types(engine, &full);
            let sql = docs_sql(&doc, &db, &table);
            if std::env::var_os("INF_DEBUG_SQL").is_some() {
                eprintln!("[HTTP-SQL] {sql}");
            }
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
                                .unwrap_or_else(|| expr_result_type(c))
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
        // ---- configs (`ShowConfigsHandler` / `ShowConfigHandler` /
        //      `SetConfigHandler`, http_server_impl.cpp:2469-2900) ----
        ("GET", ["configs"]) => (200, configs_json(engine)),
        ("GET", ["configs", name]) => match engine.config_get(name) {
            Some(value) => (
                200,
                format!(
                    "{{\"{}\":\"{}\",\"error_code\":0}}",
                    json_escape(name),
                    json_escape(&value)
                ),
            ),
            None => err_response(1008, &format!("Option: {name} doesn't exist.")),
        },
        ("POST", ["configs"]) => {
            let Some((name, literal)) = single_field(&doc) else {
                return err_response(3076, "No config will be set");
            };
            let sql = format!("SET CONFIG {} {literal};", name.to_ascii_lowercase());
            match run(&sql) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        // ---- variables (`ShowGlobalVariablesHandler`,
        //      `ShowGlobalVariableHandler`, `SetGlobalVariableHandler`, the
        //      session pair, http_server_impl.cpp:2540-2830) ----
        // The reference flattens the SHOW VARIABLES rows into the response
        // object (`json_response[config_name] = config_value`), so the body is
        // `{"error_code":0,"<name>":"<value>",…}` — not a list.
        ("GET", ["variables", scope]) if variables_scope(scope).is_some() => {
            let scope = variables_scope(scope).expect("checked above");
            let vars = engine.variables(scope);
            let body = vars
                .iter()
                .map(|(name, value)| {
                    format!("\"{}\":\"{}\"", json_escape(name), json_escape(value))
                })
                .collect::<Vec<_>>()
                .join(",");
            (200, format!("{{\"error_code\":0,{body}}}"))
        }
        ("GET", ["variables", scope, name]) if variables_scope(scope).is_some() => {
            let scope = variables_scope(scope).expect("checked above");
            match engine.show_variable(name, scope) {
                Ok(result) => {
                    let value = match result {
                        QueryResult::Rows { rows, .. } => rows
                            .first()
                            .and_then(|r| r.first())
                            .map(|v| v.to_slt_string())
                            .unwrap_or_default(),
                        _ => String::new(),
                    };
                    (
                        200,
                        format!(
                            "{{\"error_code\":0,\"{}\":\"{}\"}}",
                            json_escape(name),
                            json_escape(&value)
                        ),
                    )
                }
                Err(e) => engine_err(&e),
            }
        }
        // `SetVariableOrConfig` with a one-field body; the reference answers
        // 3076 "No variable will be set" for any other arity.
        ("SET", ["variables", scope]) if variables_scope(scope).is_some() => {
            let scope = variables_scope(scope).expect("checked above");
            let Some((name, value)) = single_variable(&doc) else {
                return err_response(3076, "No variable will be set");
            };
            match engine.set_variables(scope, &[(name, value)]) {
                Ok(()) => (200, ok_json()),
                Err(e) => engine_err(&e),
            }
        }
        // ---- snapshots (`ListSnapshotsHandler`, `ShowSnapshotHandler`,
        //      `Create*SnapshotHandler`, http_server_impl.cpp:3491-3860) ----
        ("GET", ["snapshots"]) => match engine.snapshot_names() {
            Ok(names) => {
                let items = names
                    .iter()
                    .map(|n| format!("{{\"name\":\"{}\"}}", json_escape(n)))
                    .collect::<Vec<_>>()
                    .join(",");
                (
                    200,
                    format!("{{\"error_code\":0,\"error_msg\":\"\",\"snapshots\":[{items}]}}"),
                )
            }
            Err(e) => engine_err(&e),
        },
        ("GET", ["snapshots", name]) => match engine.snapshot_names() {
            Ok(names) if names.iter().any(|n| n == name) => (
                200,
                format!(
                    "{{\"error_code\":0,\"error_msg\":\"\",\"snapshot\":{{\"name\":\"{}\"}}}}",
                    json_escape(name)
                ),
            ),
            Ok(_) => err_response(3037, &format!("snapshot '{name}' does not exist")),
            Err(e) => engine_err(&e),
        },
        ("POST", ["snapshots"]) => {
            let db = doc.get("db_name").and_then(Json::as_str).unwrap_or("default_db");
            let table = doc
                .get("table_name")
                .and_then(Json::as_str)
                .unwrap_or_default();
            let Some(snapshot) = doc.get("snapshot_name").and_then(Json::as_str) else {
                return err_response(3037, "snapshot_name is required");
            };
            let (Some(db), Some(table)) = (safe_ident(db), safe_ident(table)) else {
                return err_response(3037, "invalid table name");
            };
            let sql = format!(
                "CREATE SNAPSHOT {} ON TABLE {db}.{table};",
                snapshot_sql(snapshot)
            );
            match run(&sql) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("POST", ["snapshots", "restore"]) => {
            let Some(snapshot) = doc.get("snapshot_name").and_then(Json::as_str) else {
                return err_response(3037, "snapshot_name is required");
            };
            match run(&format!(
                "RESTORE TABLE SNAPSHOT {};",
                snapshot_sql(snapshot)
            )) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("POST", ["snapshots", "system"]) => {
            let Some(snapshot) = doc.get("snapshot_name").and_then(Json::as_str) else {
                return err_response(3037, "snapshot_name is required");
            };
            match run(&format!(
                "CREATE SNAPSHOT {} ON SYSTEM;",
                snapshot_sql(snapshot)
            )) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("POST", ["snapshots", "system", "restore"]) => {
            let Some(snapshot) = doc.get("snapshot_name").and_then(Json::as_str) else {
                return err_response(3037, "snapshot_name is required");
            };
            match run(&format!(
                "RESTORE SYSTEM SNAPSHOT {};",
                snapshot_sql(snapshot)
            )) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("POST", ["snapshots", "database"]) => {
            let db = doc.get("db_name").and_then(Json::as_str).unwrap_or("default_db");
            let Some(snapshot) = doc.get("snapshot_name").and_then(Json::as_str) else {
                return err_response(3037, "snapshot_name is required");
            };
            let Some(db) = safe_ident(db) else {
                return err_response(3003, "invalid database name");
            };
            match run(&format!(
                "CREATE SNAPSHOT {} ON DATABASE {db};",
                snapshot_sql(snapshot)
            )) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("POST", ["snapshots", "database", "restore"]) => {
            let Some(snapshot) = doc.get("snapshot_name").and_then(Json::as_str) else {
                return err_response(3037, "snapshot_name is required");
            };
            match run(&format!(
                "RESTORE DATABASE SNAPSHOT {};",
                snapshot_sql(snapshot)
            )) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("DELETE", ["snapshots", name]) => {
            match run(&format!("DROP SNAPSHOT {};", snapshot_sql(name))) {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        // ---- instance (`*Handler` in http_server_impl.cpp:2917-3465) ----
        // Each row shape below is the `SHOW …` statement the C++ handler runs:
        // `SHOW BUFFER` (path, status, size, buffered_type, type), `SHOW
        // PROFILES`, `SHOW MEMINDEX`, `SHOW QUERIES`, `SHOW QUERY <id>`, `SHOW
        // TRANSACTIONS`, `SHOW TRANSACTION <id>`, `SHOW OBJECTS`, `SHOW FILES`,
        // `SHOW MEMORY` (`physical_show_impl.cpp:160-240`).
        ("POST", ["instance", "flush"]) => match engine.flush() {
            Ok(()) => (200, ok_json()),
            Err(e) => engine_err(&e),
        },
        ("GET", ["instance", "buffer"]) => (
            200,
            format!("{{\"error_code\":0,\"buffer\":[{}]}}", buffer_rows(engine)),
        ),
        ("GET", ["instance", "profiles"]) => (
            200,
            format!(
                "{{\"error_code\":0,\"profiles\":[{}]}}",
                profile_rows(engine)
            ),
        ),
        ("GET", ["instance", "memindex"]) => (
            200,
            format!("{{\"error_code\":0,\"index\":[{}]}}", memory_index_rows(engine)),
        ),
        ("GET", ["instance", "queries"]) => (
            200,
            format!(
                "{{\"error_code\":0,\"queries\":[{}]}}",
                query_rows(engine)
            ),
        ),
        ("GET", ["instance", "queries", id]) => match id.parse::<u64>() {
            Ok(id) => match engine.query_profile(id) {
                Some(row) => (
                    200,
                    format!(
                        "{{\"error_code\":0,\"query\":{}}}",
                        query_row_json(&row)
                    ),
                ),
                None => err_response(3088, &format!("query {id} does not exist")),
            },
            Err(_) => err_response(3014, &format!("invalid query id '{id}'")),
        },
        ("GET", ["instance", "transactions"]) => (
            200,
            format!(
                "{{\"error_code\":0,\"transactions\":[{}]}}",
                transaction_rows(engine)
            ),
        ),
        ("GET", ["instance", "transactions", id]) => match id.parse::<u64>() {
            Ok(id) => match engine.transaction_detail(id) {
                Some(info) => (
                    200,
                    format!(
                        "{{\"error_code\":0,\"transaction\":{{\"transaction_id\":{},\"transaction_text\":\"{}\"}}}}",
                        info.txn_id,
                        json_escape(&info.txn_text)
                    ),
                ),
                None => err_response(3088, &format!("transaction {id} does not exist")),
            },
            Err(_) => err_response(3014, &format!("invalid transaction id '{id}'")),
        },
        // `ShowObjectsHandler` reads the persistence manager's object table in
        // the reference; this engine has no persistence manager, and the objects
        // it actually keeps resident are the buffered blocks, so the same rows as
        // `/instance/buffer` are served here.
        ("GET", ["instance", "objects"]) => (
            200,
            format!("{{\"error_code\":0,\"objects\":[{}]}}", buffer_rows(engine)),
        ),
        // The C++ route takes `{object_name}` as one path segment; a buffered
        // block's path has several, so the remaining segments are joined back
        // together and matched as a path suffix (`data/db_1/tbl_0/seg_0/blk_0`)
        // when they are not the absolute path.
        ("GET", ["instance", "objects", tail @ ..]) if !tail.is_empty() => {
            let name = tail.join("/");
            let found = engine.buffer_object(&name).or_else(|| {
                engine
                    .buffer_objects()
                    .into_iter()
                    .find(|row| row.object_path.to_string_lossy().ends_with(&name))
            });
            match found {
                Some(row) => (
                    200,
                    format!(
                        "{{\"error_code\":0,\"object\":{}}}",
                        buffer_row_json(
                            &row.object_path.to_string_lossy(),
                            row.buffered_status.as_str(),
                            row.object_size,
                            row.buffered_type.as_str(),
                            row.file_type.as_str(),
                        )
                    ),
                ),
                None => err_response(3088, &format!("object '{name}' does not exist")),
            }
        }
        ("GET", ["instance", "files"]) => (
            200,
            format!("{{\"error_code\":0,\"files\":[{}]}}", file_rows(engine)),
        ),
        // `SHOW LOGS` is the **WAL** dump in the reference (transaction_id,
        // commit_ts, command_type, command_text), not a log-file listing.
        ("GET", ["instance", "logs"]) => not_supported(
            "GET /instance/logs",
            "a textual rendering of `WalCmd` (the typed WAL stores the command \
             payload, but `wal_entry.rs` has no `ToString`, so the `text` column \
             of SHOW LOGS cannot be filled)",
        ),
        // `SHOW MEMORY`: `GlobalResourceUsage`'s two counters. The reference
        // prints "Not activate" when its `INFINITY_STATS` build flag is off; the
        // port always keeps the counters, so the real numbers are reported.
        ("GET", ["instance", "memory"]) => {
            let usage = infinity_executor::global_resource_usage::GlobalResourceUsage::instance();
            (
                200,
                format!(
                    "{{\"error_code\":0,\"memory_objects\":\"{}\",\"memory_allocation\":\"{}\"}}",
                    usage.get_object_count(),
                    usage.get_raw_memory_count()
                ),
            )
        }
        // `ShowMemoryObjectsHandler` / `ShowMemoryAllocationsHandler` enumerate
        // the per-object allocator (C++ `NewCatalog`'s memory tracker). This
        // port has no allocator hook: `GlobalResourceUsage` keeps two totals
        // and no per-object map, so there are no rows to serve.
        ("GET", ["instance", "memory", "objects"]) => not_supported(
            "GET /instance/memory/objects",
            "a per-object allocator tracker (the port only keeps \
             GlobalResourceUsage's two totals, with no per-object map)",
        ),
        ("GET", ["instance", "memory", "allocations"]) => not_supported(
            "GET /instance/memory/allocations",
            "a per-allocation tracker (the port only keeps \
             GlobalResourceUsage's two totals, with no allocation records)",
        ),
        // ---- admin (`Admin*Handler`, http_server_impl.cpp:3861-4240) ----
        ("GET", ["admin", "nodes"]) => {
            let nodes = peer
                .list_nodes()
                .iter()
                .map(node_json)
                .collect::<Vec<_>>()
                .join(",");
            (200, format!("{{\"error_code\":0,\"nodes\":[{nodes}]}}"))
        }
        ("GET", ["admin", "node", "current"]) => match peer.this_node() {
            Some(node) => (200, format!("{{\"error_code\":0,\"node\":{}}}", node_json(&node))),
            None => err_response(3037, "node is not initialized"),
        },
        ("POST", ["admin", "node", "current"]) => {
            let Some(role) = doc.get("role").and_then(Json::as_str) else {
                return err_response(3037, "Field 'role' is required");
            };
            let Some(role) = parse_role(role) else {
                return err_response(3037, &format!("Invalid node role {role}"));
            };
            match peer.change_role(role) {
                Ok(_) => (200, ok_json()),
                Err(e) => err_response(e.code(), &e.to_string()),
            }
        }
        ("GET", ["admin", "node", name]) => {
            let found = peer
                .list_nodes()
                .into_iter()
                .find(|n| n.node_name() == *name);
            match found {
                Some(node) => (200, format!("{{\"error_code\":0,\"node\":{}}}", node_json(&node))),
                None => err_response(3037, &format!("node '{name}' does not exist")),
            }
        }
        ("DELETE", ["admin", "node", name]) => match peer.terminate_peer(name) {
            Ok(_) => (
                200,
                format!(
                    "{{\"error_code\":0,\"message\":\"Node {} removed successfully.\"}}",
                    json_escape(name)
                ),
            ),
            Err(e) => err_response(e.code(), &e.to_string()),
        },
        ("GET", ["admin", "configs"]) => (200, configs_json(engine)),
        // `AdminShowNodeVariablesHandler` / `AdminShowNodeVariableHandler` read
        // the same variable table as the `/variables/*` routes.
        ("GET", ["admin", "variables"]) => {
            let vars = engine.variables(infinity_executor::Scope::Global);
            let body = vars
                .iter()
                .map(|(name, value)| {
                    format!("\"{}\":\"{}\"", json_escape(name), json_escape(value))
                })
                .collect::<Vec<_>>()
                .join(",");
            (200, format!("{{\"error_code\":0,{body}}}"))
        }
        ("GET", ["admin", "variables", name]) => {
            match engine.show_variable(name, infinity_executor::Scope::Global) {
                Ok(QueryResult::Rows { rows, .. }) => {
                    let value = rows
                        .first()
                        .and_then(|r| r.first())
                        .map(|v| v.to_slt_string())
                        .unwrap_or_default();
                    (
                        200,
                        format!(
                            "{{\"error_code\":0,\"{}\":\"{}\"}}",
                            json_escape(name),
                            json_escape(&value)
                        ),
                    )
                }
                Ok(_) => err_response(3088, &format!("variable '{name}' does not exist")),
                Err(e) => engine_err(&e),
            }
        }
        // `AdminShowLogsHandler` lists the node's log **files** (index,
        // filename, type) from the log directory. This engine logs to stderr
        // through `tracing` and keeps no log-file directory or index, so there
        // is no row to serve.
        ("GET", ["admin", "logs"]) => not_supported(
            "GET /admin/logs",
            "a log-file directory/index (the engine logs to stderr; C++ \
             AdminShowLogs lists the files under the node's log dir)",
        ),
        _ => (404, "{\"error\":\"not found\"}".to_string()),
    }
}

/// The `SetScope` behind a `/variables/<scope>` path segment.
fn variables_scope(scope: &str) -> Option<infinity_executor::Scope> {
    match scope {
        "global" => Some(infinity_executor::Scope::Global),
        "session" => Some(infinity_executor::Scope::Session),
        _ => None,
    }
}

/// The one field of a `SET /variables/<scope>` body, with its value spelled the
/// way the reference reads it (`simdjson` string / number / bool).
fn single_variable(doc: &Json) -> Option<(String, String)> {
    let obj = doc.as_obj()?;
    if obj.len() != 1 {
        return None;
    }
    let (name, value) = &obj[0];
    let value = match value {
        Json::Str(s) => s.clone(),
        Json::Num(n) if n.fract() == 0.0 => format!("{}", *n as i64),
        Json::Num(n) => format!("{n}"),
        Json::Bool(b) => b.to_string(),
        _ => return None,
    };
    Some((name.clone(), value))
}

/// One `SHOW BUFFER` row (`physical_show_buffer_impl.cpp`).
fn buffer_row_json(
    path: &str,
    status: &str,
    size: usize,
    buffered_type: &str,
    file_type: &str,
) -> String {
    format!(
        "{{\"path\":\"{}\",\"status\":\"{}\",\"size\":{},\"buffered_type\":\"{}\",\"type\":\"{}\"}}",
        json_escape(path),
        json_escape(status),
        size,
        json_escape(buffered_type),
        json_escape(file_type)
    )
}

fn buffer_rows(engine: &Engine) -> String {
    engine
        .buffer_objects()
        .iter()
        .map(|row| {
            buffer_row_json(
                &row.object_path.to_string_lossy(),
                row.buffered_status.as_str(),
                row.object_size,
                row.buffered_type.as_str(),
                row.file_type.as_str(),
            )
        })
        .collect::<Vec<_>>()
        .join(",")
}

/// One `SHOW QUERIES` row (`physical_show_queries_impl.cpp`).
fn query_row_json(row: &infinity_executor::diagnostics::QueryRow) -> String {
    format!(
        "{{\"session_id\":{},\"query_id\":{},\"query_kind\":\"{}\",\"start_time\":\"{}\",\"time_consumption\":\"{}\"}}",
        row.session_id,
        row.query_id,
        json_escape(&row.query_kind),
        json_escape(&row.start_time),
        json_escape(&row.time_consumption)
    )
}

fn query_rows(engine: &Engine) -> String {
    engine
        .running_queries()
        .iter()
        .map(query_row_json)
        .collect::<Vec<_>>()
        .join(",")
}

/// `SHOW PROFILES`. The reference emits one column per query phase; the port
/// records one wall-clock duration per finished statement, so a row carries the
/// statement and that duration.
fn profile_rows(engine: &Engine) -> String {
    engine
        .profiles()
        .iter()
        .enumerate()
        .map(|(i, row)| {
            format!(
                "{{\"record_no\":\"{i}\",\"query_id\":{},\"query_kind\":\"{}\",\"query_text\":\"{}\",\"time_consumption\":\"{}\"}}",
                row.query_id,
                json_escape(&row.query_kind),
                json_escape(&row.query_text),
                json_escape(&infinity_executor::diagnostics::elapsed_to_string(
                    std::time::Duration::from_nanos(row.duration_ns.max(0) as u64)
                ))
            )
        })
        .collect::<Vec<_>>()
        .join(",")
}

/// `SHOW MEMINDEX` rows: `index_name`, `table_name`, `db_name`, `size`,
/// `row_count` (`physical_show_impl.cpp:172`).
fn memory_index_rows(engine: &Engine) -> String {
    engine
        .memory_indexes()
        .iter()
        .map(|row| {
            let (db, table) = match row.table.split_once('.') {
                Some((db, table)) => (db, table),
                None => ("", row.table.as_str()),
            };
            format!(
                "{{\"index_name\":\"{}\",\"table_name\":\"{}\",\"db_name\":\"{}\",\"size\":{},\"row_count\":{}}}",
                json_escape(&row.index),
                json_escape(table),
                json_escape(db),
                row.mem_used,
                row.row_count
            )
        })
        .collect::<Vec<_>>()
        .join(",")
}

/// `SHOW TRANSACTIONS` rows: `transaction_id`, `transaction_text`.
fn transaction_rows(engine: &Engine) -> String {
    engine
        .active_transactions()
        .iter()
        .map(|info| {
            format!(
                "{{\"transaction_id\":{},\"transaction_text\":\"{}\"}}",
                info.txn_id,
                json_escape(&info.txn_text)
            )
        })
        .collect::<Vec<_>>()
        .join(",")
}

/// `SHOW FILES` rows: `file_name`, `object_name`, `offset`, `size`. Without a
/// persistence manager a file is its own object at offset 0.
fn file_rows(engine: &Engine) -> String {
    engine
        .all_files()
        .iter()
        .map(|(path, size)| {
            let name = path.to_string_lossy();
            format!(
                "{{\"file_name\":\"{}\",\"object_name\":\"{}\",\"offset\":0,\"size\":{}}}",
                json_escape(&name),
                json_escape(&name),
                size
            )
        })
        .collect::<Vec<_>>()
        .join(",")
}

fn ok_json() -> String {
    "{\"error_code\":0,\"error_msg\":\"OK\"}".to_string()
}

fn handle(mut stream: TcpStream, engine: &Engine, peer: &PeerManager) -> Result<()> {
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

    let (status, resp_body) = route(engine, peer, method, path, &body);
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

    /// A standalone node, the same registry `Server::bind` installs.
    fn leader() -> PeerManager {
        PeerManager::new("leader", "127.0.0.1", 23820)
    }

    #[test]
    fn route_query_and_health() {
        let dir = tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        let peer = leader();
        engine
            .execute("CREATE TABLE t (id INT, name VARCHAR);")
            .unwrap();
        engine.execute("INSERT INTO t VALUES (1, 'a');").unwrap();

        let (s, b) = route(
            &engine,
            &peer,
            "POST",
            "/query",
            "{\"sql\":\"SELECT * FROM t;\"}",
        );
        assert_eq!(s, 200);
        assert!(b.contains("\"columns\":[\"id\",\"name\"]"), "{b}");
        assert!(b.contains("[\"1\",\"a\"]"), "{b}");

        assert_eq!(
            route(&engine, &peer, "GET", "/health", ""),
            (200, "ok".to_string())
        );
        assert_eq!(route(&engine, &peer, "POST", "/query", "{}").0, 400);
        assert_eq!(route(&engine, &peer, "GET", "/nope", "").0, 404);
    }

    /// `/configs` (`ShowConfigsHandler`, `ShowConfigHandler`,
    /// `SetConfigHandler` in `src/network/http_server_impl.cpp`).
    #[test]
    fn http_config_routes() {
        let dir = tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        let peer = leader();

        let (s, b) = route(&engine, &peer, "GET", "/configs", "");
        assert_eq!(s, 200, "{b}");
        assert!(b.starts_with("{\"cache_result_capacity\":"), "{b}");
        assert!(b.contains("\"log_level\":\"info\""), "{b}");
        assert!(b.contains("\"error_code\":0"), "{b}");

        let (s, b) = route(&engine, &peer, "GET", "/configs/log_level", "");
        assert_eq!(s, 200, "{b}");
        assert_eq!(b, "{\"log_level\":\"info\",\"error_code\":0}");

        let (s, b) = route(
            &engine,
            &peer,
            "POST",
            "/configs",
            "{\"log_level\":\"debug\"}",
        );
        assert_eq!(s, 200, "{b}");
        let (_, b) = route(&engine, &peer, "GET", "/configs/log_level", "");
        assert!(b.contains("\"debug\""), "{b}");

        // Unknown knob: the reference's InvalidConfig (1008).
        let (s, b) = route(&engine, &peer, "GET", "/configs/nope", "");
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("\"error_code\":1008"), "{b}");
        // Two fields is "No config will be set" (3076).
        let (s, b) = route(&engine, &peer, "POST", "/configs", "{\"a\":1,\"b\":2}");
        assert!(s != 200, "{b}");
        assert!(b.contains("\"error_code\":3076"), "{b}");
    }

    /// `/snapshots*` (`ListSnapshotsHandler`, `ShowSnapshotHandler`,
    /// `CreateTableSnapshotHandler`, `RestoreTableSnapshotHandler`,
    /// `DropSnapshotHandler`).
    #[test]
    fn http_snapshot_routes() {
        let dir = tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        let peer = leader();
        engine.execute("CREATE TABLE t (id INT);").unwrap();
        engine.execute("INSERT INTO t VALUES (1);").unwrap();

        let (s, b) = route(&engine, &peer, "GET", "/snapshots", "");
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"snapshots\":[]"), "{b}");

        let (s, b) = route(
            &engine,
            &peer,
            "POST",
            "/snapshots",
            "{\"db_name\":\"default_db\",\"table_name\":\"t\",\"snapshot_name\":\"snap0\"}",
        );
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"error_code\":0"), "{b}");

        let (s, b) = route(&engine, &peer, "GET", "/snapshots", "");
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("{\"name\":\"snap0\"}"), "{b}");

        let (s, b) = route(&engine, &peer, "GET", "/snapshots/snap0", "");
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"snapshot\":{\"name\":\"snap0\"}"), "{b}");

        // Restoring into an existing table fails (DROP TABLE first), so the
        // restore path is exercised after dropping it.
        engine.execute("DROP TABLE t;").unwrap();
        let (s, b) = route(
            &engine,
            &peer,
            "POST",
            "/snapshots/restore",
            "{\"snapshot_name\":\"snap0\"}",
        );
        assert_eq!(s, 200, "{b}");
        assert_eq!(
            engine.execute("SELECT * FROM t;").unwrap().len(),
            1,
            "the restored table serves its rows"
        );

        // Database and system snapshots share the engine's snapshot store.
        let (s, b) = route(
            &engine,
            &peer,
            "POST",
            "/snapshots/database",
            "{\"db_name\":\"default_db\",\"snapshot_name\":\"dbsnap\"}",
        );
        assert_eq!(s, 200, "{b}");
        let (s, b) = route(
            &engine,
            &peer,
            "POST",
            "/snapshots/system",
            "{\"snapshot_name\":\"sysnap\"}",
        );
        assert_eq!(s, 200, "{b}");

        let (s, b) = route(&engine, &peer, "DELETE", "/snapshots/snap0", "");
        assert_eq!(s, 200, "{b}");
        let (s, b) = route(&engine, &peer, "GET", "/snapshots/snap0", "");
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("does not exist"), "{b}");
    }

    /// `/instance/*`: every route the engine can answer serves real rows.
    #[test]
    fn http_instance_routes() {
        let dir = tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        let peer = leader();
        engine.execute("CREATE TABLE t (id INT);").unwrap();
        engine.execute("INSERT INTO t VALUES (1);").unwrap();

        let (s, b) = route(&engine, &peer, "POST", "/instance/flush", "");
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"error_code\":0"), "{b}");

        // The checkpoint wrote the block mirror; reading it back buffers the
        // block, so the buffer/objects routes must list the object the test
        // itself touched.
        engine.read_cpp_blocks("t").unwrap();
        let (s, b) = route(&engine, &peer, "GET", "/instance/buffer", "");
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"error_code\":0"), "{b}");
        assert!(b.contains("\"buffer\":[{\"path\":\""), "{b}");
        assert!(b.contains("/seg_0/blk_0\""), "{b}");
        assert!(b.contains("\"status\":\"Loaded\""), "{b}");
        assert!(b.contains("\"buffered_type\":\"Persistent\""), "{b}");
        assert!(b.contains("\"type\":\"data\""), "{b}");
        assert!(!b.contains("\"size\":0,"), "{b}");

        let (s, b) = route(&engine, &peer, "GET", "/instance/objects", "");
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"objects\":[{\"path\":\""), "{b}");
        assert!(b.contains("/seg_0/blk_0\""), "{b}");

        // The object is addressable by its path suffix (the C++ route takes one
        // path segment, a block path has several).
        let (s, b) = route(
            &engine,
            &peer,
            "GET",
            "/instance/objects/data/db_1/tbl_0/seg_0/blk_0",
            "",
        );
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"object\":{\"path\":\""), "{b}");
        assert!(b.contains("\"status\":\"Loaded\""), "{b}");
        let (s, b) = route(&engine, &peer, "GET", "/instance/objects/data/nope", "");
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("\"error_code\":3088"), "{b}");

        // Files: the block mirror and the catalog are on disk, with sizes.
        let (s, b) = route(&engine, &peer, "GET", "/instance/files", "");
        assert_eq!(s, 200, "{b}");
        assert!(b.starts_with("{\"error_code\":0,\"files\":["), "{b}");
        assert!(b.contains("/blk_0/"), "{b}");
        assert!(b.contains("\"object_name\":\""), "{b}");

        // No index / query / transaction exists yet: an empty list, not an error.
        for (path, key) in [
            ("/instance/memindex", "index"),
            ("/instance/queries", "queries"),
            ("/instance/transactions", "transactions"),
            ("/instance/profiles", "profiles"),
        ] {
            let (s, b) = route(&engine, &peer, "GET", path, "");
            assert_eq!(s, 200, "{path}: {b}");
            assert!(b.contains(&format!("\"{key}\":[")), "{path}: {b}");
        }

        // With profiling on, a finished statement is in the history and is
        // addressable through `SHOW QUERY <id>`.
        let (s, b) = route(
            &engine,
            &peer,
            "SET",
            "/variables/global",
            "{\"profile\":true}",
        );
        assert_eq!(s, 200, "{b}");
        engine.execute("SELECT 1;").unwrap();
        let (s, b) = route(&engine, &peer, "GET", "/instance/profiles", "");
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"query_kind\":\"SELECT\""), "{b}");
        assert!(b.contains("\"query_text\":\"SELECT 1;\""), "{b}");
        let select_id = engine
            .profiles()
            .iter()
            .find(|p| p.query_kind == "SELECT")
            .expect("the SELECT is in the profile history")
            .query_id;
        let (s, b) = route(
            &engine,
            &peer,
            "GET",
            &format!("/instance/queries/{select_id}"),
            "",
        );
        assert_eq!(s, 200, "{b}");
        assert!(b.contains(&format!("\"query_id\":{select_id}")), "{b}");
        assert!(b.contains("\"query_kind\":\"SELECT\""), "{b}");
        let (s, b) = route(&engine, &peer, "GET", "/instance/queries/999", "");
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("\"error_code\":3088"), "{b}");
        let (s, b) = route(&engine, &peer, "GET", "/instance/transactions/1", "");
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("\"error_code\":3088"), "{b}");

        // Memory: the port's two real counters.
        let (s, b) = route(&engine, &peer, "GET", "/instance/memory", "");
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"memory_objects\":\""), "{b}");
        assert!(b.contains("\"memory_allocation\":\""), "{b}");

        // The three routes with no data source keep 501 / 3032, and say what is
        // missing.
        for path in [
            "/instance/logs",
            "/instance/memory/objects",
            "/instance/memory/allocations",
        ] {
            let (s, b) = route(&engine, &peer, "GET", path, "");
            assert_eq!(s, 501, "{path}: {b}");
            assert!(b.contains("\"error_code\":3032"), "{path}: {b}");
            assert!(b.contains("not supported in this build"), "{path}: {b}");
            assert!(b.contains("needs "), "{path}: {b}");
        }
    }

    /// `/variables/*`: the listing is the reference's flat object, one name is
    /// its own `{"<name>":"<value>"}` body, and SET follows the engine's rules.
    #[test]
    fn http_variable_routes_serve_the_engine_table() {
        let dir = tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        let peer = leader();

        let (s, b) = route(&engine, &peer, "GET", "/variables/global", "");
        assert_eq!(s, 200, "{b}");
        assert!(
            b.starts_with("{\"error_code\":0,\"active_txn_count\":\"0\""),
            "{b}"
        );
        assert!(b.contains("\"schedule_policy\":\"round robin\""), "{b}");
        // A live counter, not a constant: the engine has an RSS and open files.
        assert!(b.contains("\"system_memory_usage\":\""), "{b}");
        assert!(!b.contains("\"system_memory_usage\":\"0\""), "{b}");
        assert!(!b.contains("\"open_file_count\":\"0\""), "{b}");

        let (s, b) = route(&engine, &peer, "GET", "/variables/global/query_count", "");
        assert_eq!(s, 200, "{b}");
        assert_eq!(b.matches("query_count").count(), 1, "{b}");
        assert!(b.starts_with("{\"error_code\":0,\"query_count\":\""), "{b}");

        let (s, b) = route(&engine, &peer, "GET", "/variables/session", "");
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"connected_timestamp\":\""), "{b}");

        let (s, b) = route(&engine, &peer, "GET", "/variables/session/query_count", "");
        assert_eq!(s, 200, "{b}");
        assert!(b.starts_with("{\"error_code\":0,\"query_count\":\""), "{b}");

        // An unknown name is the reference's "No system variable" error.
        let (s, b) = route(&engine, &peer, "GET", "/variables/global/nope", "");
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("No system variable: nope"), "{b}");

        // SET profile works, SET a read-only variable is 3076.
        let (s, b) = route(
            &engine,
            &peer,
            "SET",
            "/variables/global",
            "{\"profile\":true}",
        );
        assert_eq!(s, 200, "{b}");
        let (_, b) = route(&engine, &peer, "GET", "/variables/global/profile", "");
        assert!(b.contains("\"profile\":\"true\""), "{b}");
        let (s, b) = route(
            &engine,
            &peer,
            "SET",
            "/variables/global",
            "{\"query_count\":9}",
        );
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("\"error_code\":3076"), "{b}");
        // Two fields is "No variable will be set".
        let (s, b) = route(
            &engine,
            &peer,
            "SET",
            "/variables/global",
            "{\"a\":1,\"b\":2}",
        );
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("No variable will be set"), "{b}");
        // Session variables are read-only.
        let (s, b) = route(
            &engine,
            &peer,
            "SET",
            "/variables/session",
            "{\"query_count\":1}",
        );
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("\"error_code\":3076"), "{b}");
    }

    /// `/admin/*` (`AdminShowCurrentNodeHandler`, `AdminListAllNodesHandler`,
    /// `AdminShowNodeByNameHandler`, `AdminSetNodeRoleHandler`,
    /// `AdminRemoveNodeHandler`, `AdminShowNodeConfigsHandler`).
    #[test]
    fn http_admin_routes() {
        let dir = tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        let peer = leader();

        let (s, b) = route(&engine, &peer, "GET", "/admin/nodes", "");
        assert_eq!(s, 200, "{b}");
        assert!(b.starts_with("{\"error_code\":0,\"nodes\":[{"), "{b}");
        assert!(b.contains("\"name\":\"leader\""), "{b}");
        assert!(b.contains("\"role\":\"leader\""), "{b}");
        assert!(b.contains("\"status\":\"alive\""), "{b}");
        assert!(b.contains("\"address\":\"127.0.0.1:23820\""), "{b}");

        let (s, b) = route(&engine, &peer, "GET", "/admin/node/current", "");
        assert_eq!(s, 200, "{b}");
        assert!(b.starts_with("{\"error_code\":0,\"node\":{"), "{b}");
        assert!(b.contains("\"role\":\"leader\""), "{b}");

        let (s, b) = route(&engine, &peer, "GET", "/admin/node/leader", "");
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"name\":\"leader\""), "{b}");
        let (s, b) = route(&engine, &peer, "GET", "/admin/node/ghost", "");
        assert_eq!(s, 500, "{b}");

        let (s, b) = route(
            &engine,
            &peer,
            "POST",
            "/admin/node/current",
            "{\"role\":\"admin\"}",
        );
        assert_eq!(s, 200, "{b}");
        let (_, b) = route(&engine, &peer, "GET", "/admin/node/current", "");
        assert!(b.contains("\"role\":\"admin\""), "{b}");
        let (s, b) = route(
            &engine,
            &peer,
            "POST",
            "/admin/node/current",
            "{\"role\":\"nope\"}",
        );
        assert_eq!(s, 500, "{b}");
        let (s, b) = route(&engine, &peer, "POST", "/admin/node/current", "{}");
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("Field 'role' is required"), "{b}");

        let (s, b) = route(&engine, &peer, "GET", "/admin/configs", "");
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"log_level\""), "{b}");

        let (s, b) = route(&engine, &peer, "DELETE", "/admin/node/leader", "");
        assert!(s == 200 || s == 500, "{b}");
        assert!(b.contains("\"error_code\":"), "{b}");

        // The node's variables are the engine's global table.
        let (s, b) = route(&engine, &peer, "GET", "/admin/variables", "");
        assert_eq!(s, 200, "{b}");
        assert!(
            b.starts_with("{\"error_code\":0,\"active_txn_count\":\"0\""),
            "{b}"
        );
        assert!(b.contains("\"schedule_policy\":\"round robin\""), "{b}");

        let (s, b) = route(&engine, &peer, "GET", "/admin/variables/query_count", "");
        assert_eq!(s, 200, "{b}");
        assert!(b.starts_with("{\"error_code\":0,\"query_count\":\""), "{b}");
        let (s, b) = route(&engine, &peer, "GET", "/admin/variables/nope", "");
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("No system variable: nope"), "{b}");

        // No log-file directory exists, so the log listing keeps 3032.
        let (s, b) = route(&engine, &peer, "GET", "/admin/logs", "");
        assert_eq!(s, 501, "{b}");
        assert!(b.contains("\"error_code\":3032"), "{b}");
        assert!(b.contains("needs "), "{b}");
    }

    /// `GET /segments` / `GET /segments/{id}/blocks` reuse the real
    /// `SHOW SEGMENTS` / `SHOW BLOCKS` metadata (`ShowSegmentsListHandler`,
    /// `ShowBlocksListHandler`).
    #[test]
    fn http_segment_and_block_routes_report_real_metadata() {
        let dir = tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        let peer = leader();
        engine.execute("CREATE TABLE t (id INT);").unwrap();
        engine.execute("INSERT INTO t VALUES (1);").unwrap();
        // A `COPY` seals its segment, so the table ends up with two segments.
        let csv = dir.path().join("one_row.csv");
        std::fs::write(&csv, "2\n").unwrap();
        engine
            .execute(&format!(
                "COPY t FROM '{}' WITH (DELIMITER ',', FORMAT CSV);",
                csv.display()
            ))
            .unwrap();
        engine.flush().unwrap();

        let (s, b) = route(
            &engine,
            &peer,
            "GET",
            "/databases/default_db/tables/t/segments",
            "",
        );
        assert_eq!(s, 200, "{b}");
        assert!(
            b.starts_with("{\"error_code\":0,\"table_name\":\"t\",\"segments\":[{"),
            "{b}"
        );
        // Two real segments, each holding its own row.
        assert_eq!(b.matches("\"row_count\":\"1\"").count(), 2, "{b}");
        assert!(!b.contains("\"segment_id\":0"), "no synthetic segment: {b}");

        // The mirror writes each segment into its own directory, so the two
        // one-row segments have one block each.
        let (s, b) = route(
            &engine,
            &peer,
            "GET",
            "/databases/default_db/tables/t/segments/1/blocks",
            "",
        );
        assert_eq!(s, 200, "{b}");
        assert!(
            b.starts_with("{\"error_code\":0,\"segment_id\":1,\"blocks\":[{"),
            "{b}"
        );
        assert!(b.contains("\"row_count\":\"1\""), "{b}");
        assert!(b.contains("\"size\":\""), "{b}");
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
