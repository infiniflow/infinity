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

use std::net::TcpListener;
use std::sync::Arc;

use common::LogicalType;
use executor::{Engine, Literal, QueryResult};
use parser::{
    BoolOp, ExprAst, MatchClause, OrderKey, Predicate, SearchClause, SelectItem, Statement,
};

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
        // Same trap as the Thrift port: macOS lets the wildcard bind coexist
        // with another process's `127.0.0.1` listener and sends every local REST
        // client there (see `reject_taken_loopback`).
        crate::reject_taken_loopback(addr)?;
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

    /// Serves HTTP on the **caller's** runtime.
    ///
    /// One task per connection; the request body is decoded on the reactor and
    /// the SQL runs on the engine's scheduler, so a slow query never blocks the
    /// accept loop. The front end no longer owns a runtime: it is a task on the
    /// one the caller spawned it on.
    pub async fn serve(self) {
        self.listener
            .set_nonblocking(true)
            .expect("set the http listener non-blocking");
        let listener =
            tokio::net::TcpListener::from_std(self.listener).expect("adopt the http listener");
        loop {
            let Ok((stream, _)) = listener.accept().await else {
                continue;
            };
            let _ = stream.set_nodelay(true);
            let engine = Arc::clone(&self.engine);
            let peer = Arc::clone(&self.peer);
            tokio::spawn(async move {
                let _ = handle_async(stream, engine, peer).await;
            });
        }
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
    /// A JSON number, kept as its source text: a 64-bit integer has to keep
    /// every digit (the reference reads `raw_json()`), which `f64` cannot.
    Num(String),
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
            Json::Num(n) => n.parse().ok(),
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
                let text = std::str::from_utf8(&s[start..*i]).ok()?;
                text.parse::<f64>().ok()?;
                Some(Json::Num(text.to_string()))
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

/// The reference's `ParseColumnType` (`http_server_impl.cpp`) maps the REST
/// spelling onto a `DataType` and rejects what it cannot represent; the SQL
/// grammar is laxer (it accepts `EMBEDDING(bool,3)`), so the check has to run
/// here, on the API boundary, exactly where the reference has it.
fn column_type_error(ty: &str) -> Option<(i64, String)> {
    // `ParseColumnDefs` lower-cases the type and splits it on commas, with no
    // bracket handling: a type that carries a stray `(` — `SPARSE(`, `vector(` —
    // lands in the head token, matches no branch below, and would otherwise be
    // stored verbatim, only to be re-parsed (and indexed into) by the executor.
    let parts: Vec<&str> = ty.split(',').map(|s| s.trim()).collect();
    let head = parts.first().copied().unwrap_or("").to_ascii_lowercase();
    if head.contains(['(', ')']) {
        return Some((3013, format!("{} isn't supported.", parts[0])));
    }
    match head.as_str() {
        "vector" | "multivector" | "tensor" | "tensorarray" => {
            if parts.len() != 3 {
                return Some((
                    3013,
                    "vector / multivector / tensor / tensorarray type syntax error".into(),
                ));
            }
            let etype = parts[2].to_ascii_lowercase();
            let ok = matches!(
                etype.as_str(),
                "int"
                    | "integer"
                    | "int32"
                    | "uint8"
                    | "int8"
                    | "tinyint"
                    | "float"
                    | "float32"
                    | "double"
                    | "float64"
                    | "float16"
                    | "bfloat16"
                    | "bit"
            );
            if !ok {
                return Some((3057, format!("Invalid embedding data type: {}.", parts[2])));
            }
        }
        "sparse" => {
            if parts.len() != 4 {
                return Some((3013, "sparse type syntax error".into()));
            }
            let dtype = parts[2].to_ascii_lowercase();
            let d_ok = matches!(
                dtype.as_str(),
                "integer" | "int" | "int32" | "float" | "float32" | "double" | "float64"
            );
            let itype = parts[3].to_ascii_lowercase();
            let i_ok = matches!(
                itype.as_str(),
                "tinyint"
                    | "int8"
                    | "smallint"
                    | "int16"
                    | "integer"
                    | "int"
                    | "int32"
                    | "bigint"
                    | "int64"
            );
            if !d_ok {
                return Some((3057, format!("Invalid embedding data type: {}.", parts[2])));
            }
            if !i_ok {
                return Some((3057, format!("Invalid embedding data type: {}.", parts[3])));
            }
        }
        "array" => return column_type_error(&parts[1..].join(",")),
        _ => {}
    }
    None
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

/// A declared type whose cells are floating point.
fn is_float_type(upper_type: &str) -> bool {
    upper_type.starts_with("FLOAT") || upper_type.starts_with("DOUBLE")
}

/// Converts one JSON cell into the literal the executor takes, the job the
/// reference's `InsertHandler` does when it builds a `ConstantExpr`
/// (`http_server_impl.cpp:955-1350`). No SQL text is produced: the literal
/// vector goes straight to `Engine::insert_literals_inner`.
///
/// `ty` is the target column's declared type, which only decides how an
/// object/array is read (a `JSON` column keeps the JSON text, a `SPARSE` one
/// takes `{index: value}` pairs).
/// `Ok(None)` when the JSON number cannot be represented in the target column.
///
/// The old text path let the lexer apply this check: it read the cell text and
/// failed with `bad int` on a value beyond `i64`. Building the literal
/// directly means the check has to happen here, or the value is silently
/// wrapped.
fn insert_literal(v: &Json, ty: &str) -> std::result::Result<Literal, (u16, String)> {
    let up = ty.to_ascii_uppercase();
    Ok(match v {
        Json::Null => Literal::Null,
        // The reference records `kBoolean`; the literal form the insert path
        // and the reference's stored text agree on is `true`/`false`.
        Json::Bool(b) => Literal::Str(if *b { "true" } else { "false" }.into()),
        Json::Num(n) => match n.parse::<i64>() {
            Ok(i) => Literal::Int(i),
            // Not an `i64`: a fraction/exponent (a float literal), or a u64 the
            // wire allowed. A u64 above `i64::MAX` must not be cast to `i64` —
            // `18446744073709551615` would land as `-1`. For a floating-point
            // column the value survives as a float; anywhere else it cannot be
            // represented, which is what the old text path reported (`bad int`).
            Err(_) => match n.parse::<u64>() {
                Ok(u) if is_float_type(&up) => Literal::Float(u as f64),
                Ok(_) => {
                    return Err(err_response(
                        3013,
                        &format!("parse error: bad int {n} for a column of type '{ty}'"),
                    ))
                }
                Err(_) => Literal::Float(n.parse::<f64>().unwrap_or_default()),
            },
        },
        Json::Str(s) => Literal::Str(s.clone()),
        // A `JSON` column stores the document's own text; any other array
        // column keeps its elements as source text, which is what the
        // encoder's `[a,b,c]` reader consumes for a dense vector or tensor.
        Json::Arr(_) if up.starts_with("JSON") => Literal::Str(render_json(v)),
        Json::Arr(items) => Literal::Array(items.iter().map(array_item_text).collect()),
        Json::Obj(map) if up.starts_with("SPARSE") => Literal::Array(
            map.iter()
                .map(|(k, v)| match v {
                    Json::Num(n) => format!("{k}:{n}"),
                    Json::Str(s) => format!("{k}:{s}"),
                    _ => format!("{k}:0"),
                })
                .collect(),
        ),
        Json::Obj(_) => Literal::Str(render_json(v)),
    })
}

/// One element of a [`Literal::Array`], as the text the executor's array and
/// tensor encoders read back. A nested array keeps its brackets; a string
/// element is bare text, because this literal is built directly rather than
/// re-read from SQL (the parser would have stripped the quotes).
fn array_item_text(v: &Json) -> String {
    match v {
        // The text the SQL round-trip used to produce: `NULL` arrived as a bare
        // identifier, so it kept its upper-case spelling.
        Json::Null => "NULL".into(),
        Json::Bool(b) => b.to_string(),
        Json::Num(n) => n.clone(),
        Json::Str(s) => s.clone(),
        Json::Arr(items) => format!(
            "[{}]",
            items
                .iter()
                .map(array_item_text)
                .collect::<Vec<_>>()
                .join(",")
        ),
        Json::Obj(_) => render_json(v),
    }
}

fn render_json(v: &Json) -> String {
    match v {
        Json::Null => "null".into(),
        Json::Bool(b) => b.to_string(),
        Json::Num(n) => n.clone(),
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
/// The reference's `IntegerOption`s are rendered as JSON numbers (the SDK
/// hands `config_value` to the caller as an `int`), everything else as strings.
pub(crate) fn config_is_numeric(name: &str) -> bool {
    matches!(
        name.to_ascii_lowercase().as_str(),
        "cleanup_interval"
            | "checkpoint_interval"
            | "cache_result_capacity"
            | "mem_index_capacity"
            | "buffer_manager_size"
            | "memindex_memory_quota"
            | "lru_num"
            | "wal_compact_threshold"
            | "log_file_max_size"
            | "log_file_rotate_count"
    )
}

fn config_value_json(name: &str, value: &str) -> String {
    if config_is_numeric(name) {
        value.trim().to_string()
    } else {
        format!("\"{}\"", json_escape(value))
    }
}

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
        .map(|(k, v)| format!("\"{}\":{}", json_escape(k), config_value_json(k, v)))
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
/// The one `(name, value)` pair of a request body, with the value as text.
///
/// The value is **not** SQL-quoted: the caller hands it to
/// [`Statement::SetConfig`], which takes the setting's own spelling.
fn single_field(doc: &Json) -> Option<(String, String)> {
    let obj = doc.as_obj()?;
    if obj.len() != 1 {
        return None;
    }
    let (name, value) = &obj[0];
    let value = match value {
        Json::Str(s) => s.clone(),
        Json::Num(n) => n.clone(),
        Json::Bool(b) => b.to_string(),
        _ => return None,
    };
    Some((name.clone(), value))
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

/// The declared types of a table's columns.
///
/// Reads the catalog record the `SHOW TABLE … COLUMNS` statement would have
/// reported, instead of running that statement: the caller already holds the
/// name, so the statement would only round-trip through the parser.
fn column_types(engine: &Engine, table: &str) -> Vec<(String, String)> {
    match engine.catalog().get_table_by_name(table) {
        Ok(Some(meta)) => meta
            .columns
            .iter()
            .enumerate()
            .map(|(i, c)| (c.clone(), meta.types.get(i).cloned().unwrap_or_default()))
            .collect(),
        _ => Vec::new(),
    }
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

/// Builds a [`parser::ColumnDef`] from one REST `fields` entry.
///
/// The executor reads `type_text` (and `comment`/`default`/`not_null`); `ty`
/// mirrors what the SQL parser would have produced so the AST is the same
/// shape either way.
fn column_def(
    name: &str,
    ty: &str,
    comment: Option<&str>,
    default: Option<&Json>,
    not_null: bool,
) -> std::result::Result<parser::ColumnDef, (u16, String)> {
    let text = type_to_sql(ty);
    let head = text
        .split(['(', ','])
        .next()
        .unwrap_or("")
        .trim()
        .to_ascii_uppercase();
    let logical = match head.as_str() {
        "BOOLEAN" | "BOOL" => LogicalType::Boolean,
        "TINYINT" => LogicalType::TinyInt,
        "SMALLINT" => LogicalType::SmallInt,
        "INTEGER" | "INT" => LogicalType::Integer,
        "BIGINT" => LogicalType::BigInt,
        "FLOAT" => LogicalType::Float,
        "DOUBLE" => LogicalType::Double,
        "VARCHAR" | "TEXT" => LogicalType::Varchar,
        _ => LogicalType::Invalid,
    };
    let default = match default {
        Some(Json::Str(s)) => Some(Literal::Str(s.clone())),
        Some(other) => Some(insert_literal(other, &text)?),
        None => None,
    };
    Ok(parser::ColumnDef {
        name: name.to_ascii_lowercase(),
        ty: logical,
        type_text: text.to_ascii_uppercase(),
        default,
        not_null,
        comment: comment.map(str::to_string),
        sparse_store: None,
    })
}

/// Parses a REST `filter` string as **one** predicate.
///
/// The fragment is client text, so it is parsed here and never concatenated
/// into a statement: `parse_predicate` rejects anything after the expression,
/// which is what stops `c1 = 1; DROP TABLE t` from being two statements.
///
/// Only an **empty** string means "no filter", which is what the text path did:
/// a whitespace-only one used to reach `parse` and fail, and treating it as
/// "match everything" turns a malformed DELETE/UPDATE into a whole-table one.
///
/// The text is not rewritten here. `rewrite_pseudo` (`_score` → `score()`)
/// substitutes on the raw text and would also hit those spellings inside a
/// string literal, so `label = '_score'` would match the wrong rows. The query
/// path applies it where it was always applied — see `docs_select`.
fn parse_filter(text: &str) -> std::result::Result<Option<Predicate>, (u16, String)> {
    if text.is_empty() {
        return Ok(None);
    }
    parser::parse_predicate(text)
        .map(Some)
        .map_err(|e| err_response(3063, &format!("Invalid filter expression: {text} ({e})")))
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

/// Builds the `SELECT` a `GET …/docs` request describes.
///
/// The request fields are assembled into a [`Statement::Select`] directly. The
/// previous version rendered them to SQL text, which meant a `filter` string
/// the client sent went through the parser as text: `c1 = 1; DROP TABLE t`
/// filtered nothing and dropped `t`. Parsing the fragment on its own (with
/// [`parser::parse_predicate`]) and handing the AST to the engine keeps the
/// fragment a fragment.
fn docs_select(
    body: &Json,
    db: &str,
    table: &str,
) -> std::result::Result<Statement, (u16, String)> {
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
    // `SELECT … AS c2_sub … WHERE c2_sub = …`: the Thrift path substitutes the
    // projection aliases into WHERE/HAVING (`SelectReq::to_sql`), so mirror it
    // here — the engine itself resolves WHERE against table columns only.
    let aliases: Vec<(String, String)> = named.iter().filter_map(|n| split_alias(n)).collect();

    let err = |e: common::Error| err_response(3013, &e.to_string());
    let mut items: Vec<SelectItem> = Vec::with_capacity(named.len());
    let mut out_aliases: Vec<Option<String>> = Vec::with_capacity(named.len());
    for n in &named {
        // The REST DSL puts the alias in the projection text (`expr AS name`),
        // so it is split off and re-attached after the item is parsed.
        let (expr, alias) = match split_alias(n) {
            Some((alias, expr)) => (expr, Some(alias)),
            None => (n.clone(), None),
        };
        let (item, parsed_alias) = parser::parse_select_item(&expr).map_err(err)?;
        items.push(item);
        out_aliases.push(alias.or(parsed_alias));
    }

    let mut highlight = Vec::new();
    if let Some(h) = body.get("highlight").and_then(Json::as_arr) {
        highlight = h
            .iter()
            .filter_map(Json::as_str)
            .map(str::to_string)
            .collect();
    }

    let mut search = None;
    if let Some(list) = body.get("search").and_then(Json::as_arr) {
        let mut clause = SearchClause::default();
        for e in list {
            let method = e.get("match_method").and_then(Json::as_str).unwrap_or("");
            // An integer topn ≥ 1; anything else becomes -1 so the engine
            // reports the reference's invalid-topn error.
            let topn: i64 = match e.get("topn") {
                None => 10,
                Some(Json::Num(n)) => match n.parse::<f64>() {
                    Ok(v) if v.fract() == 0.0 && v >= 1.0 => v as i64,
                    _ => -1,
                },
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
            let mut opts = format!("topn={topn}");
            if !params.is_empty() {
                opts = format!("{opts};{params}");
            }
            // Options are a single-quoted literal in the reference's request,
            // so the text keeps its own quotes inside the string value.
            let text_lit = |s: &str| ExprAst::Literal(Literal::Str(s.to_string()));
            match method {
                "dense" => {
                    let field = e.get("fields").and_then(Json::as_str).unwrap_or("");
                    let vec = e
                        .get("query_vector")
                        .map(|v| ExprAst::Literal(Literal::Array(json_array_items(v))))
                        .unwrap_or_else(|| ExprAst::Literal(Literal::Array(Vec::new())));
                    let elem = match e.get("element_type") {
                        None => "float".to_string(),
                        Some(Json::Str(s)) => s.clone(),
                        Some(_) => "__invalid__".to_string(),
                    };
                    let metric = e.get("metric_type").and_then(Json::as_str).unwrap_or("l2");
                    clause.matches.push(MatchClause {
                        kind: "VECTOR".into(),
                        column: field.to_string(),
                        args: vec![
                            vec,
                            text_lit(&elem),
                            text_lit(metric),
                            ExprAst::Literal(Literal::Int(topn)),
                            text_lit(&opts),
                        ],
                        filter: None,
                    });
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
                    clause.matches.push(MatchClause {
                        kind: "TEXT".into(),
                        column: field.to_string(),
                        args: vec![text_lit(text), text_lit(&opts)],
                        filter: None,
                    });
                }
                "sparse" => {
                    let field = e.get("fields").and_then(Json::as_str).unwrap_or("");
                    let qv = e
                        .get("query_vector")
                        .map(|v| ExprAst::Literal(Literal::Array(sparse_query_items(v))))
                        .unwrap_or_else(|| ExprAst::Literal(Literal::Array(Vec::new())));
                    let metric = e.get("metric_type").and_then(Json::as_str).unwrap_or("ip");
                    clause.matches.push(MatchClause {
                        kind: "SPARSE".into(),
                        column: field.to_string(),
                        args: vec![
                            qv,
                            text_lit(metric),
                            ExprAst::Literal(Literal::Int(topn)),
                            text_lit(&opts),
                        ],
                        filter: None,
                    });
                }
                "tensor" => {
                    let field = e.get("field").and_then(Json::as_str).unwrap_or("");
                    let vec = e
                        .get("query_tensor")
                        .map(|v| ExprAst::Literal(Literal::Array(json_array_items(v))))
                        .unwrap_or_else(|| ExprAst::Literal(Literal::Array(Vec::new())));
                    let elem = match e.get("element_type") {
                        None => "float".to_string(),
                        Some(Json::Str(s)) => s.clone(),
                        Some(_) => "__invalid__".to_string(),
                    };
                    // `MATCH TENSOR (col, vec, 'elem', 'options')`: the row count
                    // and the options share the trailing parameter string, so
                    // `topn` is carried in the bag (`args[3]`).
                    clause.matches.push(MatchClause {
                        kind: "TENSOR".into(),
                        column: field.to_string(),
                        args: vec![vec, text_lit(&elem), text_lit("maxsim"), text_lit(&opts)],
                        filter: None,
                    });
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
                clause.fusions.push((m.to_string(), String::new()));
            }
            if let Some((m, _)) = clause.fusions.last() {
                clause.fusion = Some(m.clone());
            }
        }
        if !clause.matches.is_empty() {
            search = Some(Box::new(clause));
        }
    }

    let where_pred = match body.get("filter").and_then(Json::as_str) {
        Some(f) if !f.is_empty() => {
            let f = crate::substitute_aliases(&rewrite_pseudo(f), &aliases);
            Some(parser::parse_predicate(&f).map_err(err)?)
        }
        _ => None,
    };

    let mut group_by = Vec::new();
    if let Some(g) = body.get("group_by") {
        for c in str_list(g) {
            group_by.push(parser::parse_expr(&rewrite_pseudo(&c)).map_err(err)?);
        }
    }

    let having = match body.get("having") {
        Some(h) => {
            let preds = str_list(h);
            if preds.is_empty() {
                None
            } else {
                let mut acc: Option<Predicate> = None;
                for p in preds {
                    let p = crate::substitute_aliases(&rewrite_pseudo(&p), &aliases);
                    let pred = parser::parse_predicate(&p).map_err(err)?;
                    acc = Some(match acc {
                        Some(a) => Predicate::Bool(Box::new(a), BoolOp::And, Box::new(pred)),
                        None => pred,
                    });
                }
                acc.map(Box::new)
            }
        }
        None => None,
    };

    let mut order_by = Vec::new();
    if let Some(s) = body.get("sort").and_then(Json::as_arr) {
        for x in s {
            // `[col, "asc"]` (thrift-like) or `{col: "asc"}` (the REST SDK)
            let (col, dir) = if let Some(a) = x.as_arr() {
                let col = a.first().and_then(Json::as_str).unwrap_or("");
                let dir = a.get(1).and_then(Json::as_str).unwrap_or("asc");
                (col, dir)
            } else {
                match x.as_obj().and_then(|o| o.first()) {
                    Some((col, dir)) => (col.as_str(), dir.as_str().unwrap_or("asc")),
                    None => continue,
                }
            };
            let col = rewrite_pseudo(col);
            let expr = parser::parse_expr(&col).map_err(err)?;
            order_by.push(OrderKey {
                column: col,
                asc: !dir.eq_ignore_ascii_case("desc"),
                expr: Some(expr),
            });
        }
    }

    let limit = body.get("limit").and_then(|l| {
        l.as_f64()
            .map(|n| n as i64)
            .or_else(|| l.as_str().and_then(|s| s.parse().ok()))
    });
    let offset = body.get("offset").and_then(|o| {
        o.as_f64()
            .map(|n| n as i64)
            .or_else(|| o.as_str().and_then(|s| s.parse().ok()))
    });

    // An offset without a limit means "every row after n": the REST request
    // carries the two as independent fields, and the text path spelled out an
    // unbounded LIMIT so the SQL grammar (which insists on one) accepted it.
    // The tree has no such restriction, so the unbounded limit is written here.
    let limit = limit.or_else(|| offset.map(|_| i64::MAX));
    // `SEARCH` is the one clause the parser refuses to combine with `ORDER BY`
    // (`parser.y:1546`); building the tree skips that reduction, so it is
    // checked here.
    if search.is_some() && !order_by.is_empty() {
        return Err(err_response(
            3013,
            "parse error: SEARCH cannot be combined with ORDER BY",
        ));
    }

    Ok(Statement::Select {
        items,
        from: Some(format!("{db}.{table}")),
        join: None,
        where_pred,
        group_by,
        aliases: out_aliases,
        having,
        order_by,
        limit: limit.map(|n| n.max(0) as u64),
        offset: offset.map(|n| n.max(0) as u64),
        search,
        distinct: false,
        highlight,
        ctes: Vec::new(),
        cross_tables: Vec::new(),
        limit_expr: None,
        offset_expr: None,
    })
}

/// A JSON array as the `Literal::Array` items the executor's vector readers
/// take. A tensor query is a list of sub-vectors, so nesting is preserved.
fn json_array_items(v: &Json) -> Vec<String> {
    match v {
        Json::Arr(items) => items.iter().map(array_item_text).collect(),
        _ => Vec::new(),
    }
}

/// A sparse query `{index: value}` as `index:value` items.
fn sparse_query_items(v: &Json) -> Vec<String> {
    match v {
        Json::Obj(map) => map
            .iter()
            .map(|(k, v)| match v {
                Json::Num(n) => format!("{k}:{n}"),
                Json::Str(s) => format!("{k}:{s}"),
                _ => format!("{k}:0"),
            })
            .collect(),
        Json::Arr(_) => json_array_items(v)
            .into_iter()
            .map(|i| format!("{i}:1"))
            .collect(),
        _ => Vec::new(),
    }
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

/// `expr AS alias` -> `(expr, alias)`; `None` when the projection has no
/// trailing alias.
fn split_alias(projection: &str) -> Option<(String, String)> {
    let lower = projection.to_ascii_lowercase();
    let at = lower.rfind(" as ")?;
    let expr = projection[..at].trim();
    let alias = projection[at + 4..].trim();
    if expr.is_empty() || alias.is_empty() {
        return None;
    }
    if !alias.chars().all(|c| c.is_ascii_alphanumeric() || c == '_') {
        return None;
    }
    Some((alias.to_string(), expr.to_string()))
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
pub async fn route(
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
            Some(sql) => match engine.execute(&sql).await {
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

    // The router is a task, so a statement must be awaited: `execute_blocking`
    // parks the calling thread and a statement's completion can need the very
    // worker it parked (verified by its own assertion, which the HTTP suite hit
    // as `DROP TABLE` killed the connection).
    //
    /// Runs statements the handler built itself.
    ///
    /// Every route that used to `format!` a statement out of request fields
    /// goes through here instead. The formatted text was re-tokenised by the
    /// parser, so a request field the caller controls (a REST `filter`, a
    /// config name) could carry `;` and run a second statement the route never
    /// authored. Building the AST directly removes the text layer entirely.
    async fn run_ast(
        engine: &Engine,
        stmt: Statement,
        text: &str,
    ) -> std::result::Result<Vec<QueryResult>, (u16, String)> {
        engine
            .execute_parsed(vec![stmt], text)
            .await
            .map_err(|e| engine_err(&e))
    }

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
                let _ = engine
                    .execute_parsed(
                        vec![Statement::DropDatabase {
                            name: db.clone(),
                            if_exists: true,
                        }],
                        "",
                    )
                    .await;
            }
            let stmt = Statement::CreateDatabase {
                name: db.clone(),
                if_not_exists: ignore,
            };
            match run_ast(engine, stmt, "").await {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("DELETE", ["databases", db]) => {
            let Some(db) = ident(db) else {
                return (404, String::new());
            };
            let if_exists = match drop_option_sql(&doc) {
                Ok(v) => v,
                Err(e) => return e,
            };
            match run_ast(
                engine,
                Statement::DropDatabase {
                    name: db,
                    if_exists,
                },
                "",
            )
            .await
            {
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
            let mut cols: Vec<parser::ColumnDef> = Vec::new();
            for f in fields {
                let name = f.get("name").and_then(Json::as_str).unwrap_or("");
                let ty = f.get("type").and_then(Json::as_str).unwrap_or("varchar");
                if name.is_empty() {
                    return err_response(3042, "empty column name");
                }
                if safe_ident(name).is_none() {
                    return err_response(3005, &format!("invalid column name '{name}'"));
                }
                if let Some((code, msg)) = column_type_error(ty) {
                    return err_response(code, &msg);
                }
                let mut not_null = false;
                if let Some(cs) = f.get("constraints").and_then(Json::as_arr) {
                    for c in cs.iter().filter_map(Json::as_str) {
                        if c.eq_ignore_ascii_case("not null") {
                            not_null = true;
                        }
                    }
                }
                cols.push(match column_def(
                    name,
                    ty,
                    f.get("comment").and_then(Json::as_str),
                    f.get("default"),
                    not_null,
                ) {
                    Ok(c) => c,
                    Err(e) => return e,
                });
            }
            let (ignore, replace) = match create_option_sql(&doc) {
                Ok(v) => v,
                Err(e) => return e,
            };
            let full = format!("{db}.{table}");
            if replace {
                let _ = run_ast(
                    engine,
                    Statement::DropTable {
                        name: full.clone(),
                        if_exists: true,
                    },
                    "",
                )
                .await;
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
            let stmt = Statement::CreateTable {
                name: full,
                columns: cols,
                opts: Vec::new(),
            };
            match run_ast(engine, stmt, "").await {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("DELETE", ["databases", db, "tables", table]) => {
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
                return (404, String::new());
            };
            let if_exists = match drop_option_sql(&doc) {
                Ok(v) => v,
                Err(e) => return e,
            };
            let stmt = Statement::DropTable {
                name: format!("{db}.{table}"),
                if_exists,
            };
            match run_ast(engine, stmt, "").await {
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
                    // A table that has not been written to has no segment yet
                    // (the reference reports 0 here too).
                    let segments = crate::show_segments_rows(engine, &full).len();
                    (
                        200,
                        format!(
                            "{{\"error_code\":0,\"error_msg\":\"OK\",\"database_name\":\"{}\",\"table_name\":\"{}\",\"storage_directory\":\"\",\"column_count\":{},\"segment_count\":{segments},\"row_count\":{rows}}}",
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
                if let Some((code, msg)) = column_type_error(ty) {
                    return err_response(code, &msg);
                }
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
                cols.push(match column_def(
                    name,
                    ty,
                    None,
                    // A declared default must be applied to the rows already
                    // stored.
                    f.get("default"),
                    not_null,
                ) {
                    Ok(c) => c,
                    Err(e) => return e,
                });
            }
            let stmt = Statement::AlterTableAdd {
                table: format!("{db}.{table}"),
                columns: cols,
            };
            match run_ast(engine, stmt, "").await {
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
            let resp = ok_json();
            for n in names {
                if safe_ident(n).is_none() {
                    return err_response(3003, "invalid column name");
                }
                let stmt = Statement::AlterTableDropColumn {
                    table: format!("{db}.{table}"),
                    column: n.to_ascii_lowercase(),
                };
                match run_ast(engine, stmt, "").await {
                    Ok(_) => {}
                    Err(e) => return e,
                }
            }
            (200, resp)
        }
        ("GET", ["databases", db, "tables", table, "columns"]) => {
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
                return (404, String::new());
            };
            let stmt = Statement::ShowTableColumns {
                table: format!("{db}.{table}"),
            };
            match run_ast(engine, stmt, "").await {
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
                                    "" | "null" | "(empty)" => "null".to_string(),
                                    d => format!("\"{}\"", json_escape(d)),
                                },
                                // SHOW renders an absent comment as "(empty)";
                                // the SDK expects an empty string.
                                match r.get(3).map(String::as_str).unwrap_or("") {
                                    "" | "(empty)" => "\"\"".to_string(),
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
            // Identifiers are case-insensitive; the catalog keeps the
            // canonical lower-case spelling (as the Thrift `CreateIndex` does).
            let index = index.to_ascii_lowercase();
            let full = format!("{db}.{table}");
            let (ignore, _) = match create_option_sql(&doc) {
                Ok(v) => v,
                Err(e) => return e,
            };
            let comment = doc
                .get("comment")
                .and_then(Json::as_str)
                .unwrap_or("")
                .to_string();
            // The REST body is `{fields: [target], index: {type, …params}}`.
            let column = doc
                .get("fields")
                .and_then(Json::as_arr)
                .and_then(|a| a.first())
                .and_then(Json::as_str)
                .or_else(|| doc.get("column_name").and_then(Json::as_str))
                .unwrap_or("");
            let info = doc.get("index").cloned().unwrap_or(Json::Null);
            // A functional secondary index shares the SECONDARY kind (the Thrift
            // `IndexType` enum maps 5 onto 4 as well); the expression in
            // `fields` is what makes it functional.
            let kind = match info.get("type").and_then(Json::as_str).unwrap_or("") {
                k if k.eq_ignore_ascii_case("SECONDARYFUNCTIONAL") => "SECONDARY",
                k => k,
            };
            let mut raw: Vec<(String, String)> = Vec::new();
            if let Some(map) = info.as_obj() {
                for (k, v) in map {
                    if k.eq_ignore_ascii_case("type") {
                        continue;
                    }
                    // The reference lower-cases parameter names *and* values at
                    // the API boundary (`Infinity::CreateIndex`), so over REST
                    // `ANALYZER='STANDARD'` means `analyzer='standard'` too.
                    let value = v
                        .as_str()
                        .map(|s| s.to_string())
                        .unwrap_or_else(|| render_json(v))
                        .to_lowercase();
                    raw.push((k.to_ascii_uppercase(), value));
                }
            }
            // A tree, not a statement string: `index`, `column` and every
            // option come from the request body. The target still gets the same
            // validation the SQL parser applied when it read `(column)`.
            let column = match crate::index_target(column) {
                Ok(c) => c,
                Err(e) => return err_response(3013, &e),
            };
            let stmt = Statement::CreateIndex {
                name: index.clone(),
                table: full.clone(),
                column,
                index_type: kind.to_string(),
                opts: crate::index_params(&raw),
                if_not_exists: ignore,
            };
            match run_ast(engine, stmt, "").await {
                Ok(_) => {
                    // `SHOW INDEX` reports this back, as in the reference.
                    engine.set_index_comment(&full, &index, &comment);
                    (200, ok_json())
                }
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
            let full = format!("{db}.{table}");
            let exists = engine
                .catalog()
                .index_by_name(&full, &index)
                .map(|i| i.is_some())
                .unwrap_or(false);
            if !exists {
                return err_response(3023, &format!("index '{index}' does not exist"));
            }
            (200, ok_json())
        }
        ("DELETE", ["databases", db, "tables", table, "indexes", index]) => {
            let (Some(db), Some(table), Some(index)) = (ident(db), ident(table), ident(index))
            else {
                return (404, String::new());
            };
            let drop_ignore = match drop_option_sql(&doc) {
                Ok(v) => v,
                Err(e) => return e,
            };
            let _ = (db, table);
            let stmt = Statement::DropIndex {
                name: index.clone(),
                if_exists: drop_ignore,
            };
            match run_ast(engine, stmt, "").await {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        // `DumpIndex` flushes the memory index into on-disk chunks; the Thrift
        // request of the same name calls the very same entry point.
        ("PUT", ["databases", db, "tables", table, "indexes", index, "dump"]) => {
            let (Some(db), Some(table), Some(index)) = (ident(db), ident(table), ident(index))
            else {
                return (404, String::new());
            };
            let full = format!("{db}.{table}");
            match engine.dump_index_transaction(&full, &index).await {
                Ok(()) => (200, ok_json()),
                Err(e) => engine_err(&e),
            }
        }
        ("GET", ["databases", db, "tables", table, "indexes"]) => {
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
                return (404, String::new());
            };
            let full = format!("{db}.{table}");
            let names: Vec<String> = engine
                .catalog()
                .indexes_of_table(&full)
                .unwrap_or_default()
                .into_iter()
                .map(|i| i.name)
                .collect();
            let list = names
                .iter()
                .map(|n| format!("\"{}\"", json_escape(n)))
                .collect::<Vec<_>>()
                .join(",");
            // The reference's `ListTableIndexesHandler` answers
            // `{"indexes": [{"index_name": …}]}`; `index_names` is the flat
            // form the older clients read.
            let objs = names
                .iter()
                .map(|n| format!("{{\"index_name\":\"{}\"}}", json_escape(n)))
                .collect::<Vec<_>>()
                .join(",");
            (
                200,
                format!(
                    "{{\"error_code\":0,\"error_msg\":\"OK\",\"indexes\":[{objs}],\"index_names\":[{list}]}}"
                ),
            )
        }
        ("GET", ["databases", db, "tables", table, "indexes", index]) => {
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
                return (404, String::new());
            };
            let full = format!("{db}.{table}");
            let found = engine
                .catalog()
                .indexes_of_table(&full)
                .unwrap_or_default()
                .into_iter()
                .find(|i| i.name.eq_ignore_ascii_case(index));
            match found {
                Some(i) => (
                    200,
                    format!(
                        "{{\"error_code\":0,\"error_msg\":\"OK\",\"database_name\":\"{}\",\"table_name\":\"{}\",\"index_name\":\"{}\",\"index_comment\":\"{}\",\"index_type\":\"{}\",\"index_column_names\":\"{}\",\"index_column_ids\":\"\",\"index_function_info\":\"\",\"other_parameters\":\"\",\"store_dir\":\"\",\"segment_index_count\":\"\"}}",
                        json_escape(&db),
                        json_escape(&table),
                        json_escape(&i.name),
                        json_escape(&engine.index_comment(&full, &i.name).unwrap_or_default()),
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
            // The inner SELECT is built as an AST and then wrapped, so a
            // `filter` the client sent is parsed once as a predicate.
            let inner = match docs_select(
                &Json::Obj(vec![
                    (
                        "output".into(),
                        Json::Arr(output.into_iter().map(Json::Str).collect()),
                    ),
                    ("filter".into(), Json::Str(filter.to_string())),
                ]),
                &db,
                &table,
            ) {
                Ok(s) => s,
                Err(e) => return e,
            };
            let kind = match explain.as_str() {
                "LOGICAL" | "OPT" | "UNOPT" => parser::ExplainKind::Logical,
                _ => parser::ExplainKind::Physical,
            };
            let stmt = Statement::Explain {
                kind,
                inner: Box::new(inner),
            };
            match run_ast(engine, stmt, "").await {
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
            let path_text = doc
                .get("file_path")
                .and_then(Json::as_str)
                .unwrap_or("")
                .to_string();
            // `ImportHandler` lower-cases the type and accepts only the four it
            // knows about; anything else is `kNotSupported` before the engine is
            // even reached (the Thrift adapter rejects the same set client-side).
            let file_type = doc
                .get("file_type")
                .and_then(Json::as_str)
                .unwrap_or("csv")
                .to_ascii_lowercase();
            let format = match file_type.as_str() {
                "csv" | "json" | "jsonl" | "fvecs" => file_type.to_ascii_uppercase(),
                _ => return err_response(3032, &format!("Not supported file type {file_type}")),
            };
            let delim = doc.get("delimiter").and_then(Json::as_str).unwrap_or(",");
            if format == "CSV" && delim.chars().count() != 1 {
                return err_response(3032, &format!("Not supported delimiter: {delim}"));
            }
            let header = doc
                .get("header")
                .and_then(|v| match v {
                    Json::Bool(b) => Some(*b),
                    _ => None,
                })
                .unwrap_or(false);
            let stmt = Statement::Copy {
                table: format!("{db}.{table}"),
                from: true,
                path: path_text,
                opts: vec![
                    ("DELIMITER".into(), delim.to_string()),
                    ("FORMAT".into(), format.clone()),
                    ("HEADER".into(), header.to_string()),
                ],
            };
            match run_ast(engine, stmt, "").await {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("GET", ["databases", db, "table", table]) => {
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
                return (404, String::new());
            };
            let file = doc
                .get("file_path")
                .and_then(Json::as_str)
                .unwrap_or("")
                .to_string();
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
            let num = |key: &str| -> i64 { doc.get(key).and_then(Json::as_f64).unwrap_or(0.0) as i64 };
            let offset = num("offset");
            // `limit` is the total number of rows, `row_limit` the size of one
            // part: the reference writes the first `row_limit` rows to the file
            // and the rest to `<file>.part1`, `.part2`, …
            let total = (num("limit") > 0).then(|| num("limit"));
            let part = (num("row_limit") > 0).then(|| num("row_limit"));
            let mut written = 0i64;
            let mut index = 0i64;
            loop {
                let remaining = total.map(|t| t - written);
                if remaining.is_some_and(|r| r <= 0) {
                    break;
                }
                let take = match (part, remaining) {
                    (Some(p), Some(r)) => p.min(r),
                    (Some(p), None) => p,
                    (None, Some(r)) => r,
                    (None, None) => 0,
                };
                let path = if index == 0 {
                    file.clone()
                } else {
                    format!("{file}.part{index}")
                };
                let mut opts = vec![
                    ("DELIMITER".into(), delim.to_string()),
                    ("FORMAT".into(), format.clone()),
                    ("HEADER".into(), header.to_string()),
                    ("OFFSET".into(), (offset + written).to_string()),
                ];
                if take > 0 {
                    opts.push(("LIMIT".into(), take.to_string()));
                }
                let stmt = Statement::Copy {
                    table: format!("{db}.{table}"),
                    from: false,
                    path,
                    opts,
                };
                let n = match run_ast(engine, stmt, "").await {
                    Ok(results) => results
                        .into_iter()
                        .map(|r| match r {
                            QueryResult::Inserted { count } => count as i64,
                            _ => 0,
                        })
                        .max()
                        .unwrap_or(0),
                    Err(e) => return e,
                };
                written += n;
                if n == 0 || part.is_none() {
                    break;
                }
                index += 1;
            }
            (200, ok_json())
        }
        ("POST", ["databases", db, "tables", table, "rename"]) => {
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
                return (404, String::new());
            };
            let new_name = doc
                .get("new_table_name")
                .and_then(Json::as_str)
                .unwrap_or("");
            // Same check the SQL path applies to a rename target: `123` is a
            // number, not a name.
            let new_name = match crate::rename_target(new_name) {
                Ok(n) => n,
                Err(e) => return err_response(3013, &e),
            };
            let stmt = Statement::RenameTable {
                table: format!("{db}.{table}"),
                new_name,
            };
            match run_ast(engine, stmt, "").await {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("PUT", ["databases", db, "tables", table, "compact"]) => {
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
                return (404, String::new());
            };
            // `COMPACT TABLE …` parses to the no-op `Statement::Utility`; the
            // Thrift `Compact` request calls the engine entry point instead, and
            // the REST front end has to do the same.
            match engine.compact_table_async(&format!("{db}.{table}")).await {
                Ok(()) => (200, ok_json()),
                Err(e) => engine_err(&e),
            }
        }
        ("PUT", ["databases", db, "tables", table, "optimize"]) => {
            let (Some(db), Some(table)) = (ident(db), ident(table)) else {
                return (404, String::new());
            };
            let _ = (db, table);
            match engine.optimize_indexes_async().await {
                Ok(()) => (200, ok_json()),
                Err(e) => engine_err(&e),
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
            let rows = match &doc {
                Json::Arr(rs) => rs.clone(),
                Json::Obj(_) => vec![doc.clone()],
                _ => Vec::new(),
            };
            if rows.is_empty() {
                return err_response(3013, "insert without values");
            }
            // The reference rejects one INSERT batch above a block's worth of
            // rows (the Thrift handler reports the same text).
            if rows.len() > 8192 {
                return err_response(3014, "Insert batch row limit shouldn't more than 8193");
            }
            let meta = match engine.catalog().get_table_by_name(&full) {
                Ok(Some(meta)) => meta,
                Ok(None) => {
                    return err_response(3022, &format!("table '{full}' does not exist"));
                }
                Err(e) => return engine_err(&e),
            };
            let ty_of = |name: &str| -> String {
                meta.columns
                    .iter()
                    .position(|c| c.eq_ignore_ascii_case(name))
                    .and_then(|i| meta.types.get(i))
                    .cloned()
                    .unwrap_or_default()
            };
            let mut prepared: Vec<(Vec<String>, Vec<Literal>)> = Vec::with_capacity(rows.len());
            for row in &rows {
                let Some(fields) = row.as_obj() else {
                    return err_response(3013, "insert row must be an object");
                };
                // An empty row object is a syntax error in the reference, not a
                // no-op: the SDK's `insert([{}])` must raise (3013).
                if fields.is_empty() {
                    return err_response(3013, "insert row must not be empty");
                }
                // Column names are case-insensitive. The SQL path got the
                // canonical spelling from the lexer/lowercasing the parser
                // applied; here the key is the client's, so lower-case it —
                // `InsertHandler` does the same (`http_server_impl.cpp:950`) and
                // reports a repeat that only appears after folding
                // (`{"c1":1,"C1":2}`) as a duplicate column.
                let mut seen = std::collections::HashSet::with_capacity(fields.len());
                let mut cols: Vec<String> = Vec::with_capacity(fields.len());
                let mut vals: Vec<Literal> = Vec::with_capacity(fields.len());
                for (k, v) in fields {
                    let key = k.to_ascii_lowercase();
                    if !seen.insert(key.clone()) {
                        return err_response(3068, &format!("Duplicated column name: {key}"));
                    }
                    vals.push(match insert_literal(v, &ty_of(&key)) {
                        Ok(l) => l,
                        Err(e) => return e,
                    });
                    cols.push(key);
                }
                prepared.push((cols, vals));
            }
            // One statement for the whole request (`InsertHandler` runs one
            // `QueryStatement`), so the batch spends one transaction id.
            engine.begin_statement();
            // Rows that share a column list go in one call: the executor takes
            // one column list per batch, and the Thrift `Insert` RPC groups the
            // same way.
            let mut n = 0i64;
            let mut i = 0usize;
            let mut failed = None;
            while i < prepared.len() && failed.is_none() {
                let cols = prepared[i].0.clone();
                let mut batch: Vec<Vec<Literal>> = Vec::new();
                while i < prepared.len() && prepared[i].0 == cols {
                    batch.push(std::mem::take(&mut prepared[i].1));
                    i += 1;
                }
                match engine
                    .insert_literals_retrying(&full, Some(&cols), batch)
                    .await
                {
                    Ok(QueryResult::Inserted { count }) => n += count as i64,
                    Ok(_) => {}
                    Err(e) => failed = Some(engine_err(&e)),
                }
            }
            engine.end_statement();
            if let Some(e) = failed {
                return e;
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
            let where_pred = match parse_filter(filter) {
                Ok(p) => p,
                Err(e) => return e,
            };
            let stmt = Statement::Delete {
                table: format!("{db}.{table}"),
                where_pred,
            };
            match run_ast(engine, stmt, "").await {
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
            // An empty `SET` list was a syntax error in the text path, and the
            // executor's Update arm would otherwise delete and re-insert every
            // matching row, giving them new row ids for no change.
            if update.is_empty() {
                return err_response(3013, "update needs values");
            }
            let full = format!("{db}.{table}");
            let types = column_types(engine, &full);
            let mut assignments = Vec::with_capacity(update.len());
            for (k, v) in update {
                let ty = types
                    .iter()
                    .find(|(c, _)| c.eq_ignore_ascii_case(k))
                    .map(|(_, t)| t.clone())
                    .unwrap_or_default();
                let lit = match insert_literal(v, &ty) {
                    Ok(l) => l,
                    Err(e) => return e,
                };
                assignments.push((k.to_ascii_lowercase(), ExprAst::Literal(lit)));
            }
            let where_pred = match parse_filter(filter) {
                Ok(p) => p,
                Err(e) => return e,
            };
            let stmt = Statement::Update {
                table: full,
                assignments,
                where_pred,
            };
            match run_ast(engine, stmt, "").await {
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
            let stmt = match docs_select(&doc, &db, &table) {
                Ok(s) => s,
                Err(e) => return e,
            };
            let selected = stmt.clone();
            match run_ast(engine, stmt, "").await {
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
                            // Count the rows the query would return without
                            // LIMIT/OFFSET/ORDER BY: the same statement with the
                            // projection replaced by `count(*)` and the tail
                            // dropped. It is rebuilt from the AST rather than by
                            // editing SQL text, so nothing the request carried
                            // reaches the parser twice.
                            let mut counting = selected.clone();
                            if let Statement::Select {
                                items,
                                aliases,
                                order_by,
                                limit,
                                offset,
                                limit_expr,
                                offset_expr,
                                ..
                            } = &mut counting
                            {
                                // `count(*)` parses to an aggregate with **no**
                                // arguments (the star is a flag, not an arg).
                                *items = vec![SelectItem::Aggregate(parser::SelectAgg {
                                    func: parser::AggFunc::Count,
                                    args: Vec::new(),
                                    arg_exprs: Vec::new(),
                                    distinct: false,
                                })];
                                *aliases = vec![Some("n".into())];
                                order_by.clear();
                                *limit = None;
                                *offset = None;
                                *limit_expr = None;
                                *offset_expr = None;
                            }
                            let total = match run_ast(engine, counting, "").await {
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
                    "{{\"{}\":{},\"error_code\":0}}",
                    json_escape(name),
                    config_value_json(name, &value)
                ),
            ),
            None => err_response(1008, &format!("Option: {name} doesn't exist.")),
        },
        ("POST", ["configs"]) => {
            let Some((name, literal)) = single_field(&doc) else {
                return err_response(3076, "No config will be set");
            };
            let lower = name.to_ascii_lowercase();
            // The Thrift handler's checks, which the reference applies to both
            // front ends: a read-only knob rejects any assignment, and
            // `log_level` takes a string only.
            if lower == "mem_index_capacity" {
                return err_response(
                    3076,
                    "Invalid command: Config mem_index_capacity is read-only",
                );
            }
            if lower == "log_level" && !matches!(doc.get(&name), Some(Json::Str(_))) {
                return err_response(3010, "data type mismatch for config 'log_level'");
            }
            let stmt = Statement::SetConfig {
                key: lower,
                value: literal,
            };
            match run_ast(engine, stmt, "").await {
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
                Err(e) => {
                    // The engine reports an unknown variable as a plain
                    // invalid-argument error; the reference's code is
                    // `NO_SUCH_SYSTEM_VAR` (3027).
                    let msg = e.to_string();
                    if msg.contains("No system variable") {
                        err_response(3027, msg.trim_start_matches("invalid argument: "))
                    } else {
                        engine_err(&e)
                    }
                }
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
            let stmt = Statement::CreateSnapshot {
                name: snapshot.to_string(),
                table: format!("{db}.{table}"),
            };
            match run_ast(engine, stmt, "").await {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("POST", ["snapshots", "restore"]) => {
            let Some(snapshot) = doc.get("snapshot_name").and_then(Json::as_str) else {
                return err_response(3037, "snapshot_name is required");
            };
            match run_ast(
                engine,
                Statement::RestoreSnapshot {
                    name: snapshot.to_string(),
                },
                "",
            )
            .await
            {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("POST", ["snapshots", "system"]) => {
            let Some(snapshot) = doc.get("snapshot_name").and_then(Json::as_str) else {
                return err_response(3037, "snapshot_name is required");
            };
            match run_ast(
                engine,
                Statement::CreateSystemSnapshot {
                    name: snapshot.to_string(),
                },
                "",
            )
            .await
            {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("POST", ["snapshots", "system", "restore"]) => {
            let Some(snapshot) = doc.get("snapshot_name").and_then(Json::as_str) else {
                return err_response(3037, "snapshot_name is required");
            };
            match run_ast(
                engine,
                Statement::RestoreSystemSnapshot {
                    name: snapshot.to_string(),
                },
                "",
            )
            .await
            {
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
            let stmt = Statement::CreateDatabaseSnapshot {
                name: snapshot.to_string(),
                database: db.to_string(),
            };
            match run_ast(engine, stmt, "").await {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("POST", ["snapshots", "database", "restore"]) => {
            let Some(snapshot) = doc.get("snapshot_name").and_then(Json::as_str) else {
                return err_response(3037, "snapshot_name is required");
            };
            match run_ast(
                engine,
                Statement::RestoreDatabaseSnapshot {
                    name: snapshot.to_string(),
                },
                "",
            )
            .await
            {
                Ok(_) => (200, ok_json()),
                Err(e) => e,
            }
        }
        ("DELETE", ["snapshots", name]) => {
            let stmt = Statement::DropSnapshot {
                name: name.to_string(),
            };
            match run_ast(engine, stmt, "").await {
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
        ("POST", ["instance", "flush"]) => match engine.flush().await {
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
            let usage = executor::global_resource_usage::GlobalResourceUsage::instance();
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
            let vars = engine.variables(executor::Scope::Global);
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
            match engine.show_variable(name, executor::Scope::Global) {
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
fn variables_scope(scope: &str) -> Option<executor::Scope> {
    match scope {
        "global" => Some(executor::Scope::Global),
        "session" => Some(executor::Scope::Session),
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
        Json::Num(n) => n.clone(),
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
fn query_row_json(row: &executor::diagnostics::QueryRow) -> String {
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
                json_escape(&executor::diagnostics::elapsed_to_string(
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

/// The async connection loop: read the request on the reactor, run the route
/// on the engine's pool, write the reply on the reactor (plan §6.3/§6.5).
async fn handle_async(
    mut stream: tokio::net::TcpStream,
    engine: Arc<Engine>,
    peer: Arc<PeerManager>,
) -> Result<()> {
    use tokio::io::{AsyncReadExt, AsyncWriteExt};
    let mut buf = Vec::new();
    let mut tmp = [0u8; 1024];
    // Same bound as the synchronous reader (plan §6.2.1): a client that never
    // sends the terminator cannot grow this buffer without limit.
    const MAX_HEADER_BYTES: usize = 64 * 1024;
    let header_end = loop {
        let n = stream
            .read(&mut tmp)
            .await
            .map_err(|e| Error::io("<http>", e))?;
        if n == 0 {
            return Ok(());
        }
        buf.extend_from_slice(&tmp[..n]);
        if let Some(pos) = find_subslice(&buf, b"\r\n\r\n") {
            break pos + 4;
        }
        if buf.len() > MAX_HEADER_BYTES {
            let _ = stream
                .write_all(
                    b"HTTP/1.1 431 Request Header Fields Too Large\r\nContent-Length: 0\r\nConnection: close\r\n\r\n",
                )
                .await;
            return Ok(());
        }
    };
    let head = String::from_utf8_lossy(&buf[..header_end]).to_string();
    let request_line = head.lines().next().unwrap_or("");
    let mut parts = request_line.split_whitespace();
    let method = parts.next().unwrap_or("").to_string();
    let path = parts.next().unwrap_or("").to_string();
    let content_length = head
        .lines()
        .find_map(|l| {
            let l = l.to_ascii_lowercase();
            l.strip_prefix("content-length:")
                .map(|v| v.trim().parse::<usize>().unwrap_or(0))
        })
        .unwrap_or(0);
    while buf.len() < header_end + content_length {
        let n = stream
            .read(&mut tmp)
            .await
            .map_err(|e| Error::io("<http>", e))?;
        if n == 0 {
            break;
        }
        buf.extend_from_slice(&tmp[..n]);
    }
    let body = String::from_utf8_lossy(&buf[header_end..]).to_string();

    let (status, resp_body) = route(&engine, &peer, &method, &path, &body).await;
    let reason = if status == 200 { "OK" } else { "Error" };
    let response = format!(
        "HTTP/1.1 {status} {reason}\r\nContent-Type: application/json\r\nContent-Length: {}\r\nConnection: close\r\n\r\n{}",
        resp_body.len(),
        resp_body
    );
    stream
        .write_all(response.as_bytes())
        .await
        .map_err(|e| Error::io("<http>", e))?;
    stream.flush().await.map_err(|e| Error::io("<http>", e))?;
    Ok(())
}

fn find_subslice(hay: &[u8], needle: &[u8]) -> Option<usize> {
    hay.windows(needle.len()).position(|w| w == needle)
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::io::{Read, Write};
    use std::net::TcpStream;
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
            .execute_blocking("CREATE TABLE t (id INT, name VARCHAR);")
            .unwrap();
        engine
            .execute_blocking("INSERT INTO t VALUES (1, 'a');")
            .unwrap();

        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "POST",
            "/query",
            "{\"sql\":\"SELECT * FROM t;\"}",
        ));
        assert_eq!(s, 200);
        assert!(b.contains("\"columns\":[\"id\",\"name\"]"), "{b}");
        assert!(b.contains("[\"1\",\"a\"]"), "{b}");

        assert_eq!(
            executor::block_on(route(&engine, &peer, "GET", "/health", "")),
            (200, "ok".to_string())
        );
        assert_eq!(
            executor::block_on(route(&engine, &peer, "POST", "/query", "{}")).0,
            400
        );
        assert_eq!(
            executor::block_on(route(&engine, &peer, "GET", "/nope", "")).0,
            404
        );
    }

    /// `/configs` (`ShowConfigsHandler`, `ShowConfigHandler`,
    /// `SetConfigHandler` in `src/network/http_server_impl.cpp`).
    #[test]
    fn http_config_routes() {
        let dir = tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        let peer = leader();

        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/configs", ""));
        assert_eq!(s, 200, "{b}");
        assert!(b.starts_with("{\"cache_result_capacity\":"), "{b}");
        assert!(b.contains("\"log_level\":\"info\""), "{b}");
        assert!(b.contains("\"error_code\":0"), "{b}");

        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/configs/log_level", ""));
        assert_eq!(s, 200, "{b}");
        assert_eq!(b, "{\"log_level\":\"info\",\"error_code\":0}");

        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "POST",
            "/configs",
            "{\"log_level\":\"debug\"}",
        ));
        assert_eq!(s, 200, "{b}");
        let (_, b) = executor::block_on(route(&engine, &peer, "GET", "/configs/log_level", ""));
        assert!(b.contains("\"debug\""), "{b}");

        // Unknown knob: the reference's InvalidConfig (1008).
        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/configs/nope", ""));
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("\"error_code\":1008"), "{b}");
        // Two fields is "No config will be set" (3076).
        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "POST",
            "/configs",
            "{\"a\":1,\"b\":2}",
        ));
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
        engine.execute_blocking("CREATE TABLE t (id INT);").unwrap();
        engine
            .execute_blocking("INSERT INTO t VALUES (1);")
            .unwrap();

        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/snapshots", ""));
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"snapshots\":[]"), "{b}");

        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "POST",
            "/snapshots",
            "{\"db_name\":\"default_db\",\"table_name\":\"t\",\"snapshot_name\":\"snap0\"}",
        ));
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"error_code\":0"), "{b}");

        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/snapshots", ""));
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("{\"name\":\"snap0\"}"), "{b}");

        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/snapshots/snap0", ""));
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"snapshot\":{\"name\":\"snap0\"}"), "{b}");

        // Restoring into an existing table fails (DROP TABLE first), so the
        // restore path is exercised after dropping it.
        engine.execute_blocking("DROP TABLE t;").unwrap();
        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "POST",
            "/snapshots/restore",
            "{\"snapshot_name\":\"snap0\"}",
        ));
        assert_eq!(s, 200, "{b}");
        assert_eq!(
            engine.execute_blocking("SELECT * FROM t;").unwrap().len(),
            1,
            "the restored table serves its rows"
        );

        // Database and system snapshots share the engine's snapshot store.
        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "POST",
            "/snapshots/database",
            "{\"db_name\":\"default_db\",\"snapshot_name\":\"dbsnap\"}",
        ));
        assert_eq!(s, 200, "{b}");
        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "POST",
            "/snapshots/system",
            "{\"snapshot_name\":\"sysnap\"}",
        ));
        assert_eq!(s, 200, "{b}");

        let (s, b) = executor::block_on(route(&engine, &peer, "DELETE", "/snapshots/snap0", ""));
        assert_eq!(s, 200, "{b}");
        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/snapshots/snap0", ""));
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("does not exist"), "{b}");
    }

    /// `/instance/*`: every route the engine can answer serves real rows.
    #[test]
    fn http_instance_routes() {
        let dir = tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        let peer = leader();
        engine.execute_blocking("CREATE TABLE t (id INT);").unwrap();
        engine
            .execute_blocking("INSERT INTO t VALUES (1);")
            .unwrap();

        let (s, b) = executor::block_on(route(&engine, &peer, "POST", "/instance/flush", ""));
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"error_code\":0"), "{b}");

        // The checkpoint wrote the block mirror; reading it back maps the
        // block, so the buffer/objects routes must list the mapping the test
        // itself touched.
        engine.read_blocks("t").unwrap();
        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/instance/buffer", ""));
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"error_code\":0"), "{b}");
        assert!(b.contains("\"buffer\":[{\"path\":\""), "{b}");
        assert!(b.contains("/seg_0/blk_0\""), "{b}");
        assert!(b.contains("\"status\":\"Loaded\""), "{b}");
        assert!(b.contains("\"buffered_type\":\"Mmap\""), "{b}");
        assert!(b.contains("\"type\":\"data\""), "{b}");
        assert!(!b.contains("\"size\":0,"), "{b}");

        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/instance/objects", ""));
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"objects\":[{\"path\":\""), "{b}");
        assert!(b.contains("/seg_0/blk_0\""), "{b}");

        // The object is addressable by its path suffix (the C++ route takes one
        // path segment, a block path has several).
        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "GET",
            "/instance/objects/data/db_1/tbl_0/seg_0/blk_0",
            "",
        ));
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"object\":{\"path\":\""), "{b}");
        assert!(b.contains("\"status\":\"Loaded\""), "{b}");
        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "GET",
            "/instance/objects/data/nope",
            "",
        ));
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("\"error_code\":3088"), "{b}");

        // Files: the block mirror and the catalog are on disk, with sizes.
        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/instance/files", ""));
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
            let (s, b) = executor::block_on(route(&engine, &peer, "GET", path, ""));
            assert_eq!(s, 200, "{path}: {b}");
            assert!(b.contains(&format!("\"{key}\":[")), "{path}: {b}");
        }

        // With profiling on, a finished statement is in the history and is
        // addressable through `SHOW QUERY <id>`.
        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "SET",
            "/variables/global",
            "{\"profile\":true}",
        ));
        assert_eq!(s, 200, "{b}");
        engine.execute_blocking("SELECT 1;").unwrap();
        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/instance/profiles", ""));
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"query_kind\":\"SELECT\""), "{b}");
        assert!(b.contains("\"query_text\":\"SELECT 1;\""), "{b}");
        let select_id = engine
            .profiles()
            .iter()
            .find(|p| p.query_kind == "SELECT")
            .expect("the SELECT is in the profile history")
            .query_id;
        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "GET",
            &format!("/instance/queries/{select_id}"),
            "",
        ));
        assert_eq!(s, 200, "{b}");
        assert!(b.contains(&format!("\"query_id\":{select_id}")), "{b}");
        assert!(b.contains("\"query_kind\":\"SELECT\""), "{b}");
        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/instance/queries/999", ""));
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("\"error_code\":3088"), "{b}");
        let (s, b) =
            executor::block_on(route(&engine, &peer, "GET", "/instance/transactions/1", ""));
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("\"error_code\":3088"), "{b}");

        // Memory: the port's two real counters.
        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/instance/memory", ""));
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
            let (s, b) = executor::block_on(route(&engine, &peer, "GET", path, ""));
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

        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/variables/global", ""));
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

        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "GET",
            "/variables/global/query_count",
            "",
        ));
        assert_eq!(s, 200, "{b}");
        assert_eq!(b.matches("query_count").count(), 1, "{b}");
        assert!(b.starts_with("{\"error_code\":0,\"query_count\":\""), "{b}");

        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/variables/session", ""));
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"connected_timestamp\":\""), "{b}");

        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "GET",
            "/variables/session/query_count",
            "",
        ));
        assert_eq!(s, 200, "{b}");
        assert!(b.starts_with("{\"error_code\":0,\"query_count\":\""), "{b}");

        // An unknown name is the reference's "No system variable" error.
        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/variables/global/nope", ""));
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("No system variable: nope"), "{b}");

        // SET profile works, SET a read-only variable is 3076.
        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "SET",
            "/variables/global",
            "{\"profile\":true}",
        ));
        assert_eq!(s, 200, "{b}");
        let (_, b) = executor::block_on(route(
            &engine,
            &peer,
            "GET",
            "/variables/global/profile",
            "",
        ));
        assert!(b.contains("\"profile\":\"true\""), "{b}");
        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "SET",
            "/variables/global",
            "{\"query_count\":9}",
        ));
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("\"error_code\":3076"), "{b}");
        // Two fields is "No variable will be set".
        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "SET",
            "/variables/global",
            "{\"a\":1,\"b\":2}",
        ));
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("No variable will be set"), "{b}");
        // Session variables are read-only.
        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "SET",
            "/variables/session",
            "{\"query_count\":1}",
        ));
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

        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/admin/nodes", ""));
        assert_eq!(s, 200, "{b}");
        assert!(b.starts_with("{\"error_code\":0,\"nodes\":[{"), "{b}");
        assert!(b.contains("\"name\":\"leader\""), "{b}");
        assert!(b.contains("\"role\":\"leader\""), "{b}");
        assert!(b.contains("\"status\":\"alive\""), "{b}");
        assert!(b.contains("\"address\":\"127.0.0.1:23820\""), "{b}");

        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/admin/node/current", ""));
        assert_eq!(s, 200, "{b}");
        assert!(b.starts_with("{\"error_code\":0,\"node\":{"), "{b}");
        assert!(b.contains("\"role\":\"leader\""), "{b}");

        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/admin/node/leader", ""));
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"name\":\"leader\""), "{b}");
        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/admin/node/ghost", ""));
        assert_eq!(s, 500, "{b}");

        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "POST",
            "/admin/node/current",
            "{\"role\":\"admin\"}",
        ));
        assert_eq!(s, 200, "{b}");
        let (_, b) = executor::block_on(route(&engine, &peer, "GET", "/admin/node/current", ""));
        assert!(b.contains("\"role\":\"admin\""), "{b}");
        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "POST",
            "/admin/node/current",
            "{\"role\":\"nope\"}",
        ));
        assert_eq!(s, 500, "{b}");
        let (s, b) = executor::block_on(route(&engine, &peer, "POST", "/admin/node/current", "{}"));
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("Field 'role' is required"), "{b}");

        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/admin/configs", ""));
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"log_level\""), "{b}");

        let (s, b) = executor::block_on(route(&engine, &peer, "DELETE", "/admin/node/leader", ""));
        assert!(s == 200 || s == 500, "{b}");
        assert!(b.contains("\"error_code\":"), "{b}");

        // The node's variables are the engine's global table.
        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/admin/variables", ""));
        assert_eq!(s, 200, "{b}");
        assert!(
            b.starts_with("{\"error_code\":0,\"active_txn_count\":\"0\""),
            "{b}"
        );
        assert!(b.contains("\"schedule_policy\":\"round robin\""), "{b}");

        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "GET",
            "/admin/variables/query_count",
            "",
        ));
        assert_eq!(s, 200, "{b}");
        assert!(b.starts_with("{\"error_code\":0,\"query_count\":\""), "{b}");
        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/admin/variables/nope", ""));
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("No system variable: nope"), "{b}");

        // No log-file directory exists, so the log listing keeps 3032.
        let (s, b) = executor::block_on(route(&engine, &peer, "GET", "/admin/logs", ""));
        assert_eq!(s, 501, "{b}");
        assert!(b.contains("\"error_code\":3032"), "{b}");
        assert!(b.contains("needs "), "{b}");
    }

    /// `GET /segments` / `GET /segments/{id}/blocks` reuse the real
    /// `SHOW SEGMENTS` / `SHOW BLOCKS` metadata (`ShowSegmentsListHandler`,
    /// `ShowBlocksListHandler`).
    /// A request field that reaches the engine as a **fragment** (a `filter`,
    /// a projection, a sort key) must not be able to carry a second statement.
    ///
    /// These routes used to render the request to SQL text and hand it to
    /// `Engine::execute`, which parses `;`-separated statements: `filter` was
    /// `c1 = 1; DROP TABLE victim;` and the drop ran.
    #[test]
    fn a_request_fragment_cannot_run_a_second_statement() {
        let dir = tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        let peer = leader();
        for t in ["host", "victim"] {
            engine
                .execute_blocking(&format!("CREATE TABLE {t} (c1 INT, c2 VARCHAR);"))
                .unwrap();
        }
        engine
            .execute_blocking("INSERT INTO host VALUES (1, 'a'), (2, 'b');")
            .unwrap();

        let victim_alive = |engine: &Engine| {
            engine
                .catalog()
                .get_table_by_name("default_db.victim")
                .unwrap()
                .is_some()
        };

        // Each of the three `filter` routes, plus the projection/group/sort
        // fragments of the query DSL.
        for (method, path, body) in [
            (
                "GET",
                "/databases/default_db/tables/host/docs",
                "{\"output\":[\"c1\"],\"filter\":\"c1 = 1; DROP TABLE victim;\"}",
            ),
            (
                "DELETE",
                "/databases/default_db/tables/host/docs",
                "{\"filter\":\"c1 = 1; DROP TABLE victim;\"}",
            ),
            (
                "PUT",
                "/databases/default_db/tables/host/docs",
                "{\"update\":{\"c1\":2},\"filter\":\"c1 = 1; DROP TABLE victim;\"}",
            ),
            (
                "GET",
                "/databases/default_db/tables/host/docs",
                "{\"output\":[\"c1\"],\"group_by\":\"c1; DROP TABLE victim;\"}",
            ),
            (
                "GET",
                "/databases/default_db/tables/host/docs",
                "{\"output\":[\"c1\"],\"sort\":[[\"c1; DROP TABLE victim;\",\"asc\"]]}",
            ),
            (
                "GET",
                "/databases/default_db/tables/host/meta",
                "{\"output\":[\"c1\"],\"filter\":\"c1 = 1; DROP TABLE victim;\"}",
            ),
        ] {
            let (status, body) = executor::block_on(route(&engine, &peer, method, path, body));
            assert_ne!(
                status, 200,
                "{method} {path} accepted the injection: {body}"
            );
            assert!(
                victim_alive(&engine),
                "{method} {path} dropped the other table: {body}"
            );
        }

        // A plain filter still works.
        let (status, body) = executor::block_on(route(
            &engine,
            &peer,
            "GET",
            "/databases/default_db/tables/host/docs",
            "{\"output\":[\"c1\"],\"filter\":\"c1 = 2\"}",
        ));
        assert_eq!(status, 200, "{body}");
        assert!(body.contains("\"c1\":2"), "{body}");

        // A table name carrying a `;` is a name, not statement text.
        let (status, _) = executor::block_on(route(
            &engine,
            &peer,
            "POST",
            "/databases/default_db/tables/host%3B%20DROP%20TABLE%20victim/docs",
            "[{\"c1\":1}]",
        ));
        assert_ne!(status, 200);
        assert!(victim_alive(&engine), "an insert dropped the other table");

        // Index names, import/export paths and delimiters are request fields
        // too; each used to be interpolated into statement text. A name that
        // carries `;` is a name now, so the request may legitimately succeed —
        // what must never happen is the second statement running.
        // An export writes a file: keep it inside the test's tempdir so the
        // probe does not leave a fixture in the source tree.
        let export_body = format!(
            "{{\"file_path\":\"{}\",\"file_type\":\"csv\"}}",
            dir.path().join("a.csv; DROP TABLE victim;").display()
        );
        for (method, path, body) in [
            (
                "POST",
                "/databases/default_db/tables/host/indexes/idx%3B%20DROP%20TABLE%20victim",
                "{\"fields\":[\"c1\"],\"index\":{\"type\":\"secondary\"}}",
            ),
            (
                "POST",
                "/databases/default_db/tables/host/indexes/idx",
                "{\"fields\":[\"c1; DROP TABLE victim;\"],\"index\":{\"type\":\"secondary\"}}",
            ),
            (
                "POST",
                "/databases/default_db/tables/host/indexes/idx",
                "{\"fields\":[\"c1\"],\"index\":{\"type\":\"secondary\",\"x\":\"1; DROP TABLE victim;\"}}",
            ),
            (
                "DELETE",
                "/databases/default_db/tables/host/indexes/idx%3B%20DROP%20TABLE%20victim",
                "",
            ),
            (
                "PUT",
                "/databases/default_db/tables/host",
                "{\"file_path\":\"a.csv; DROP TABLE victim;\",\"file_type\":\"csv\"}",
            ),
            (
                "PUT",
                "/databases/default_db/tables/host",
                "{\"file_path\":\"a.csv\",\"file_type\":\"csv; DROP TABLE victim;\"}",
            ),
            (
                "GET",
                "/databases/default_db/table/host",
                export_body.as_str(),
            ),
        ] {
            let (status, body) = executor::block_on(route(&engine, &peer, method, path, body));
            assert!(
                victim_alive(&engine),
                "{method} {path} dropped the other table ({status}): {body}"
            );
        }
    }

    /// The REST mutation and query DSL must keep the semantics the statement
    /// text gave it: an empty filter is "all rows" but a blank one is an error,
    /// a literal that looks like a pseudo-column stays a literal, and an offset
    /// without a limit still returns the rows after it.
    #[test]
    fn rest_dsl_keeps_the_text_path_semantics() {
        let dir = tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        let peer = leader();
        let route_of = |method: &str, path: &str, body: &str| {
            executor::block_on(route(&engine, &peer, method, path, body))
        };
        engine
            .execute_blocking(
                "CREATE TABLE dsl (c1 INT, label VARCHAR); \
                 INSERT INTO dsl VALUES (1, '_score'), (2, 'score()'), (3, 'plain');",
            )
            .unwrap();
        let rows = |sql: &str| -> Vec<Vec<String>> {
            engine
                .execute_blocking(sql)
                .unwrap()
                .into_iter()
                .find_map(|r| match r {
                    QueryResult::Rows { .. } => Some(r.rows_text()),
                    _ => None,
                })
                .unwrap_or_default()
        };

        // A whitespace-only filter is not "match everything".
        let (s, b) = route_of(
            "DELETE",
            "/databases/default_db/tables/dsl/docs",
            "{\"filter\":\"   \"}",
        );
        assert_ne!(s, 200, "a blank filter must not delete every row: {b}");
        assert_eq!(rows("SELECT c1 FROM dsl;").len(), 3, "rows were deleted");

        // A string literal that spells a pseudo-column keeps its value:
        // rewriting the text before parsing would turn `'_score'` into
        // `'score()'` and delete the wrong row.
        let (s, b) = route_of(
            "DELETE",
            "/databases/default_db/tables/dsl/docs",
            "{\"filter\":\"label = '_score'\"}",
        );
        assert_eq!(s, 200, "{b}");
        assert_eq!(
            rows("SELECT c1 FROM dsl;"),
            vec![vec!["2".to_string()], vec!["3".to_string()]],
            "the wrong row was deleted"
        );

        // An offset without a limit returns every row after it. Two rows are
        // left at this point, so `offset: 1` yields exactly the second one.
        let (s, b) = route_of(
            "GET",
            "/databases/default_db/tables/dsl/docs",
            "{\"output\":[\"c1\"],\"offset\":1}",
        );
        assert_eq!(s, 200, "{b}");
        assert!(b.contains("\"c1\":3"), "the row after the offset: {b}");
        assert!(!b.contains("\"c1\":2"), "the skipped row came back: {b}");
    }

    /// The request path builds the insert literal itself, so it has to keep the
    /// checks the SQL lexer used to apply: column names are case-insensitive,
    /// a u64 above `i64::MAX` is not silently wrapped to `-1`, and a malformed
    /// column type is rejected instead of reaching a `parts[1]` index.
    #[test]
    fn rest_insert_and_ddl_keep_the_parser_checks() {
        let dir = tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        let peer = leader();
        let insert = |table: &str, body: &str| {
            executor::block_on(route(
                &engine,
                &peer,
                "POST",
                &format!("/databases/default_db/tables/{table}/docs"),
                body,
            ))
        };

        // 1. An upper-cased column names the same column (the reference
        //    lower-cases every key before it builds the row).
        engine
            .execute_blocking("CREATE TABLE up (c1 INT);")
            .unwrap();
        let (s, b) = insert("up", "[{\"C1\":1}]");
        assert_eq!(s, 200, "{b}");
        let rows = engine
            .execute_blocking("SELECT c1 FROM up;")
            .unwrap()
            .into_iter()
            .find_map(|r| match r {
                QueryResult::Rows { rows, .. } => Some(rows),
                _ => None,
            })
            .unwrap();
        assert_eq!(rows.len(), 1, "the row was stored under the canonical name");

        // A repeat that only appears after folding is a duplicate column.
        let (s, b) = insert("up", "[{\"c1\":1,\"C1\":2}]");
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("Duplicated column name"), "{b}");

        // 2. A u64 beyond `i64::MAX` keeps its value on a DOUBLE column and is
        //    refused on an integer one (the text path reported `bad int`).
        engine
            .execute_blocking("CREATE TABLE big (d DOUBLE, i BIGINT);")
            .unwrap();
        let (s, b) = insert("big", "[{\"d\":18446744073709551615}]");
        assert_eq!(s, 200, "{b}");
        engine
            .execute_blocking("CREATE TABLE bigi (i BIGINT);")
            .unwrap();
        let (s, b) = insert("bigi", "[{\"i\":18446744073709551615}]");
        assert_ne!(s, 200, "an out-of-range integer must not be stored: {b}");

        // 3. A malformed type is a request error, never a panic.
        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "POST",
            "/databases/default_db/tables/sp",
            "{\"fields\":[{\"name\":\"c1\",\"type\":\"SPARSE(\"}]}",
        ));
        assert_eq!(s, 500, "{b}");
        assert!(b.contains("isn't supported"), "{b}");
    }

    #[test]
    fn http_segment_and_block_routes_report_real_metadata() {
        let dir = tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        let peer = leader();
        engine.execute_blocking("CREATE TABLE t (id INT);").unwrap();
        engine
            .execute_blocking("INSERT INTO t VALUES (1);")
            .unwrap();
        // A `COPY` seals its segment, so the table ends up with two segments.
        let csv = dir.path().join("one_row.csv");
        std::fs::write(&csv, "2\n").unwrap();
        engine
            .execute_blocking(&format!(
                "COPY t FROM '{}' WITH (DELIMITER ',', FORMAT CSV);",
                csv.display()
            ))
            .unwrap();
        executor::block_on(engine.flush()).unwrap();

        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "GET",
            "/databases/default_db/tables/t/segments",
            "",
        ));
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
        let (s, b) = executor::block_on(route(
            &engine,
            &peer,
            "GET",
            "/databases/default_db/tables/t/segments/1/blocks",
            "",
        ));
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
        engine.execute_blocking("CREATE TABLE t (id INT);").unwrap();
        engine
            .execute_blocking("INSERT INTO t VALUES (42);")
            .unwrap();
        let server = HttpServer::bind("127.0.0.1:0", engine).unwrap();
        let addr = server.local_addr().unwrap();
        std::thread::spawn(move || {
            // Each fixture gets its own runtime: the front end is a task on
            // whichever runtime spawns it, and a test that shared the engine's
            // would shut it down when the thread returned.
            tokio::runtime::Builder::new_multi_thread()
                .enable_all()
                .thread_stack_size(8 * 1024 * 1024)
                .build()
                .expect("build the test runtime")
                .block_on(server.serve());
        });

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
