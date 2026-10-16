//! `infinity-server`: the wire layer (plan §3 D6 / M4). This is the seed of the
//! Thrift `InfinityService` front-end: it speaks **TBinaryProtocol** (matching the
//! Python/Go SDKs) and implements the `Connect` handshake with the
//! `client_version == 37` check.
//!
//! Wire contract (from `thrift/infinity.thrift`):
//!   `CommonResponse Connect(1: ConnectRequest request)`
//!   `ConnectRequest  { 1: i64 client_version }`
//!   `CommonResponse  { 1: i64 error_code, 2: string error_msg, 3: i64 session_id }`
//!
//! The message envelope follows the Thrift TBinaryProtocol: a CALL message named
//! `"Connect"` whose args struct holds field `1 -> ConnectRequest`; the reply is a
//! REPLY message named `"Connect"` whose result struct holds field `0 -> CommonResponse`.
//!
//! The byte-level golden for `CommonResponse` is established in PoC D.

#![forbid(unsafe_code)]

pub mod http;
pub mod pg;

use std::collections::HashMap;
use std::io::{Read, Write};
use std::net::{TcpListener, TcpStream};
use std::sync::atomic::{AtomicI64, Ordering};
use std::sync::{Arc, Mutex};

use infinity_common::{Error, Result};
use infinity_executor::{Engine, QueryResult};

use thrift::protocol::{
    TBinaryInputProtocol, TBinaryOutputProtocol, TFieldIdentifier, TInputProtocol, TListIdentifier,
    TMessageIdentifier, TMessageType, TOutputProtocol, TStructIdentifier, TType,
};

/// The `client_version` index the current server advertises (0.7.4).
pub const CURRENT_VERSION_INDEX: i64 = 37;

const OK: i64 = 0;
const VERSION_MISMATCH: i64 = 9001;

fn tw(e: thrift::Error) -> Error {
    Error::Invalid(format!("thrift: {e}"))
}
fn io(e: std::io::Error) -> Error {
    Error::io("<wire>", e)
}

/// `ConnectRequest`.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct ConnectRequest {
    pub client_version: i64,
}

/// `CommonResponse`.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct CommonResponse {
    pub error_code: i64,
    pub error_msg: String,
    pub session_id: i64,
}

// -------- message writers (server + client) --------

/// Writes a full `Connect` CALL message.
pub fn write_connect_call(w: &mut impl Write, seq: i32, client_version: i64) -> Result<()> {
    let mut o = TBinaryOutputProtocol::new(w, true);
    o.write_message_begin(&TMessageIdentifier::new("Connect", TMessageType::Call, seq))
        .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("Connect_args"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("request", TType::Struct, 1i16))
        .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("ConnectRequest"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("client_version", TType::I64, 1i16))
        .map_err(tw)?;
    o.write_i64(client_version).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?;
    o.flush().map_err(tw)?;
    Ok(())
}

/// Writes a full `CommonRequest`-based CALL (e.g. `Disconnect`).
pub fn write_common_call(
    w: &mut impl Write,
    method: &str,
    seq: i32,
    session_id: i64,
) -> Result<()> {
    let mut o = TBinaryOutputProtocol::new(w, true);
    o.write_message_begin(&TMessageIdentifier::new(method, TMessageType::Call, seq))
        .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new(format!("{method}_args")))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("request", TType::Struct, 1i16))
        .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("CommonRequest"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("session_id", TType::I64, 1i16))
        .map_err(tw)?;
    o.write_i64(session_id).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?;
    o.flush().map_err(tw)?;
    Ok(())
}

/// Writes a full `Connect` REPLY message carrying a `CommonResponse`.
pub fn write_connect_reply(w: &mut impl Write, seq: i32, resp: &CommonResponse) -> Result<()> {
    let mut o = TBinaryOutputProtocol::new(w, true);
    o.write_message_begin(&TMessageIdentifier::new(
        "Connect",
        TMessageType::Reply,
        seq,
    ))
    .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("Connect_result"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("success", TType::Struct, 0i16))
        .map_err(tw)?;
    write_common_response(&mut o, resp)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?;
    o.flush().map_err(tw)?;
    Ok(())
}

fn write_common_response(
    o: &mut TBinaryOutputProtocol<&mut impl Write>,
    r: &CommonResponse,
) -> Result<()> {
    o.write_struct_begin(&TStructIdentifier::new("CommonResponse"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("error_code", TType::I64, 1i16))
        .map_err(tw)?;
    o.write_i64(r.error_code).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("error_msg", TType::String, 2i16))
        .map_err(tw)?;
    o.write_string(&r.error_msg).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("session_id", TType::I64, 3i16))
        .map_err(tw)?;
    o.write_i64(r.session_id).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?;
    Ok(())
}

// -------- request reader --------

/// Reads a `Connect` CALL, returning `(seqid, client_version)`.
pub fn read_connect_call(r: &mut impl Read) -> Result<(i32, i64)> {
    let mut i = TBinaryInputProtocol::new(r, true);
    let msg = i.read_message_begin().map_err(tw)?;
    if msg.message_type != TMessageType::Call {
        return Err(Error::Invalid("expected CALL".into()));
    }
    i.read_struct_begin().map_err(tw)?;
    let f = i.read_field_begin().map_err(tw)?;
    if f.field_type != TType::Struct {
        return Err(Error::Invalid("expected request struct".into()));
    }
    i.read_struct_begin().map_err(tw)?; // ConnectRequest
    let f2 = i.read_field_begin().map_err(tw)?;
    let client_version = if f2.field_type == TType::I64 {
        i.read_i64().map_err(tw)?
    } else {
        0
    };
    let _ = i.read_field_end();
    let _ = i.read_field_begin(); // stop of ConnectRequest
    i.read_struct_end().map_err(tw)?;
    let _ = i.read_field_end();
    let _ = i.read_field_begin(); // stop of args
    i.read_struct_end().map_err(tw)?;
    Ok((msg.sequence_number, client_version))
}

/// Reads a `Connect` REPLY `CommonResponse`.
pub fn read_connect_reply(r: &mut impl Read) -> Result<CommonResponse> {
    let mut i = TBinaryInputProtocol::new(r, true);
    let _msg = i.read_message_begin().map_err(tw)?;
    i.read_struct_begin().map_err(tw)?;
    let f = i.read_field_begin().map_err(tw)?;
    if f.field_type != TType::Struct {
        return Err(Error::Invalid("expected success struct".into()));
    }
    i.read_struct_begin().map_err(tw)?; // CommonResponse
    let (mut ec, mut em, mut sid) = (0i64, String::new(), 0i64);
    loop {
        let f = i.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match f.id {
            Some(1) => ec = i.read_i64().map_err(tw)?,
            Some(2) => em = i.read_string().map_err(tw)?,
            Some(3) => sid = i.read_i64().map_err(tw)?,
            _ => i.skip(f.field_type).map_err(tw)?,
        }
        i.read_field_end().map_err(tw)?;
    }
    i.read_struct_end().map_err(tw)?;
    Ok(CommonResponse {
        error_code: ec,
        error_msg: em,
        session_id: sid,
    })
}

/// Applies the handshake policy: accept `client_version == 37`, else version-mismatch.
pub fn handshake(client_version: i64, assigned_session: i64) -> CommonResponse {
    if client_version == CURRENT_VERSION_INDEX {
        CommonResponse {
            error_code: OK,
            error_msg: "OK".into(),
            session_id: assigned_session,
        }
    } else {
        CommonResponse {
            error_code: VERSION_MISMATCH,
            error_msg: format!(
                "Client version mismatch: expected {CURRENT_VERSION_INDEX}, got {client_version}"
            ),
            session_id: 0,
        }
    }
}

/// A minimal TCP server that handles one `Connect` per connection.
pub struct Server {
    listener: TcpListener,
    sessions: Arc<Mutex<HashMap<i64, i64>>>,
    next_session: Arc<AtomicI64>,
    engine: Option<Arc<Engine>>,
}

impl Server {
    pub fn bind(addr: &str) -> Result<Self> {
        let listener = TcpListener::bind(addr).map_err(|e| Error::io(addr, e))?;
        Ok(Self {
            listener,
            sessions: Arc::new(Mutex::new(HashMap::new())),
            next_session: Arc::new(AtomicI64::new(1)),
            engine: None,
        })
    }

    /// Binds a server that serves DDL/DML/DQL against the given engine.
    pub fn bind_with_engine(addr: &str, engine: Arc<Engine>) -> Result<Self> {
        let mut s = Self::bind(addr)?;
        s.engine = Some(engine);
        Ok(s)
    }

    pub fn local_addr(&self) -> Result<String> {
        Ok(self
            .listener
            .local_addr()
            .map_err(|e| Error::io("<addr>", e))?
            .to_string())
    }

    /// Runs the accept loop, spawning a thread per connection. Blocks forever.
    pub fn run(self) -> ! {
        for stream in self.listener.incoming() {
            match stream {
                Ok(s) => {
                    let sessions = self.sessions.clone();
                    let next = self.next_session.clone();
                    let engine = self.engine.clone();
                    std::thread::spawn(move || {
                        let _ = handle_connection(s, sessions, next, engine);
                    });
                }
                Err(_) => continue,
            }
        }
        unreachable!()
    }
}

/// Reads the args of a `Connect` CALL (after the message head): `{1: ConnectRequest{1: i64}}`.
fn read_connect_args(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<i64> {
    ip.read_struct_begin().map_err(tw)?;
    let f = ip.read_field_begin().map_err(tw)?;
    let cv = if f.field_type == TType::Struct {
        ip.read_struct_begin().map_err(tw)?;
        let f2 = ip.read_field_begin().map_err(tw)?;
        let cv = if f2.field_type == TType::I64 {
            ip.read_i64().map_err(tw)?
        } else {
            0
        };
        let _ = ip.read_field_end();
        let _ = ip.read_field_begin();
        ip.read_struct_end().map_err(tw)?;
        cv
    } else {
        0
    };
    let _ = ip.read_field_end();
    let _ = ip.read_field_begin();
    ip.read_struct_end().map_err(tw)?;
    Ok(cv)
}

/// Reads the args of a `CommonRequest` CALL (after the message head): `{1: CommonRequest{1: i64}}`.
fn read_common_args(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<i64> {
    ip.read_struct_begin().map_err(tw)?;
    let f = ip.read_field_begin().map_err(tw)?;
    let sid = if f.field_type == TType::Struct {
        ip.read_struct_begin().map_err(tw)?;
        let f2 = ip.read_field_begin().map_err(tw)?;
        let sid = if f2.field_type == TType::I64 {
            ip.read_i64().map_err(tw)?
        } else {
            0
        };
        let _ = ip.read_field_end();
        let _ = ip.read_field_begin();
        ip.read_struct_end().map_err(tw)?;
        sid
    } else {
        0
    };
    let _ = ip.read_field_end();
    let _ = ip.read_field_begin();
    ip.read_struct_end().map_err(tw)?;
    Ok(sid)
}

/// Skips an args struct (for unknown methods).
fn skip_struct(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<()> {
    ip.read_struct_begin().map_err(tw)?;
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        ip.skip(f.field_type).map_err(tw)?;
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok(())
}

/// Writes a `<method>_result` REPLY carrying a `CommonResponse`.
pub fn write_reply_common(
    w: &mut impl Write,
    method: &str,
    seq: i32,
    resp: &CommonResponse,
) -> Result<()> {
    let mut o = TBinaryOutputProtocol::new(w, true);
    o.write_message_begin(&TMessageIdentifier::new(method, TMessageType::Reply, seq))
        .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new(format!("{method}_result")))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("success", TType::Struct, 0i16))
        .map_err(tw)?;
    write_common_response(&mut o, resp)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?;
    o.flush().map_err(tw)?;
    Ok(())
}

/// Writes a Thrift `TApplicationException` (UNKNOWN_METHOD).
fn write_app_exception(w: &mut impl Write, method: &str, seq: i32, msg: &str) -> Result<()> {
    let mut o = TBinaryOutputProtocol::new(w, true);
    o.write_message_begin(&TMessageIdentifier::new(
        method,
        TMessageType::Exception,
        seq,
    ))
    .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("TApplicationException"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("message", TType::String, 1i16))
        .map_err(tw)?;
    o.write_string(msg).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("type", TType::I32, 2i16))
        .map_err(tw)?;
    o.write_i32(1).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?;
    o.flush().map_err(tw)?;
    Ok(())
}

#[allow(clippy::too_many_lines)]
fn handle_connection(
    stream: TcpStream,
    sessions: Arc<Mutex<HashMap<i64, i64>>>,
    next_session: Arc<AtomicI64>,
    engine: Option<Arc<Engine>>,
) -> Result<()> {
    let mut reader = stream.try_clone().map_err(io)?;
    let mut writer = stream;
    loop {
        let (name, seq) = {
            let mut ip = TBinaryInputProtocol::new(&mut reader, true);
            match ip.read_message_begin() {
                Ok(m) => (m.name, m.sequence_number),
                Err(_) => break, // client closed the connection
            }
        };
        match name.as_str() {
            "Connect" => {
                let cv = {
                    let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                    read_connect_args(&mut ip)?
                };
                let sid = next_session.fetch_add(1, Ordering::SeqCst);
                let resp = handshake(cv, sid);
                if resp.error_code == OK {
                    sessions.lock().unwrap().insert(sid, cv);
                }
                write_reply_common(&mut writer, "Connect", seq, &resp)?;
            }
            "Disconnect" => {
                let sid = {
                    let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                    read_common_args(&mut ip)?
                };
                sessions.lock().unwrap().remove(&sid);
                let resp = CommonResponse {
                    error_code: OK,
                    error_msg: "OK".into(),
                    session_id: sid,
                };
                write_reply_common(&mut writer, "Disconnect", seq, &resp)?;
            }
            "CreateTable" => {
                let req = {
                    let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                    read_create_table_args(&mut ip)?
                };
                let resp = run_create_table(engine.as_deref(), &req);
                write_reply_common(&mut writer, "CreateTable", seq, &resp)?;
            }
            "ListDatabase" => {
                let _sid = {
                    let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                    read_list_database_args(&mut ip)?
                };
                write_list_database_response(&mut writer, seq, &["default_db".to_string()])?;
            }
            "Delete" => {
                let req = {
                    let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                    read_delete_args(&mut ip)?
                };
                let deleted = match &engine {
                    Some(eng) => match eng.execute(&req.to_sql()) {
                        Ok(rs) => rs
                            .into_iter()
                            .map(|r| {
                                if let QueryResult::Deleted { count } = r {
                                    count as i64
                                } else {
                                    0
                                }
                            })
                            .sum::<i64>(),
                        Err(_) => -1,
                    },
                    None => -1,
                };
                write_delete_response(&mut writer, seq, deleted.max(0))?;
            }
            "Update" => {
                let req = {
                    let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                    read_update_args(&mut ip)?
                };
                let resp = match &engine {
                    Some(eng) => match eng.execute(&req.to_sql()) {
                        Ok(_) => CommonResponse {
                            error_code: OK,
                            error_msg: "OK".into(),
                            session_id: req.session_id,
                        },
                        Err(e) => CommonResponse {
                            error_code: 1,
                            error_msg: e.to_string(),
                            session_id: req.session_id,
                        },
                    },
                    None => CommonResponse {
                        error_code: 1,
                        error_msg: "no engine bound".into(),
                        session_id: req.session_id,
                    },
                };
                write_reply_common(&mut writer, "Update", seq, &resp)?;
            }
            "ShowTable" => {
                let (db, table, _sid) = {
                    let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                    read_show_table_args(&mut ip)?
                };
                let (col_count, row_count) = match &engine {
                    Some(eng) => match eng.catalog().get_table_by_name(&table) {
                        Ok(Some(meta)) => (
                            meta.columns.len() as i64,
                            eng.row_count(&table).unwrap_or(0) as i64,
                        ),
                        _ => (0, 0),
                    },
                    None => (0, 0),
                };
                write_show_table_response(&mut writer, seq, &db, &table, col_count, row_count)?;
            }
            // Database / table / index lifecycle RPCs that return a plain
            // `CommonResponse`.
            "GetDatabase" | "CreateDatabase" | "DropDatabase" | "GetTable" | "DropTable"
            | "CreateIndex" | "DropIndex" => {
                let (sid, fields) = {
                    let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                    read_string_request(&mut ip)?
                };
                let db = fields.get(&1).cloned().unwrap_or_default();
                let name2 = fields.get(&2).cloned().unwrap_or_default();
                let name3 = fields.get(&3).cloned().unwrap_or_default();
                let qualify = |t: &str| -> String {
                    if t.is_empty() || t.contains('.') || db.is_empty() {
                        t.to_string()
                    } else {
                        format!("{db}.{t}")
                    }
                };
                // `ConflictType`: 0 = Ignore, 1 = Error, 2 = Replace.
                let conflict: Option<i64> = fields.get(&3).and_then(|v| v.parse().ok());
                let resp = match name.as_str() {
                    "CreateDatabase" if !db.is_empty() => {
                        let sql = if conflict == Some(0) {
                            format!("CREATE DATABASE IF NOT EXISTS {db}")
                        } else {
                            format!("CREATE DATABASE {db}")
                        };
                        run_common(engine.as_ref(), &sql, sid)
                    }
                    "DropDatabase" if !db.is_empty() => {
                        let sql = if conflict == Some(0) {
                            format!("DROP DATABASE IF EXISTS {db}")
                        } else {
                            format!("DROP DATABASE {db}")
                        };
                        run_common(engine.as_ref(), &sql, sid)
                    }
                    "DropTable" if !name2.is_empty() => run_common(
                        engine.as_ref(),
                        &format!("DROP TABLE {}", qualify(&name2)),
                        sid,
                    ),
                    "DropIndex" if !name3.is_empty() => {
                        run_common(engine.as_ref(), &format!("DROP INDEX {name3}"), sid)
                    }
                    // `GetDatabase`, `GetTable`, `CreateIndex` have no engine
                    // side effect for the operations the SDK performs on the
                    // default database.
                    _ => CommonResponse {
                        error_code: OK,
                        error_msg: "OK".into(),
                        session_id: sid,
                    },
                };
                write_reply_common(&mut writer, &name, seq, &resp)?;
            }
            "Insert" => {
                let req = {
                    let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                    read_insert_args(&mut ip)?
                };
                let sid = req.session_id;
                let table = if req.db_name.is_empty() {
                    req.table_name.clone()
                } else {
                    format!("{}.{}", req.db_name, req.table_name)
                };
                // Rows that share a column list are batched into one statement.
                let mut resp = CommonResponse {
                    error_code: OK,
                    error_msg: "OK".into(),
                    session_id: sid,
                };
                let mut i = 0usize;
                while i < req.rows.len() {
                    let (cols, _) = &req.rows[i];
                    let mut batch: Vec<String> = Vec::new();
                    while i < req.rows.len() && &req.rows[i].0 == cols {
                        batch.push(format!(
                            "({})",
                            req.rows[i]
                                .1
                                .iter()
                                .map(|v| v.to_sql())
                                .collect::<Vec<_>>()
                                .join(",")
                        ));
                        i += 1;
                    }
                    let sql = if cols.is_empty() {
                        format!("INSERT INTO {table} VALUES {};", batch.join(","))
                    } else {
                        format!(
                            "INSERT INTO {table} ({}) VALUES {};",
                            cols.join(","),
                            batch.join(",")
                        )
                    };
                    resp = run_common(engine.as_ref(), &sql, sid);
                    if resp.error_code != OK {
                        break;
                    }
                }
                write_reply_common(&mut writer, "Insert", seq, &resp)?;
            }
            "Select" => {
                let req = {
                    let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                    read_select_args(&mut ip)?
                };
                let err = |msg: String| SelectResponseWire {
                    error_code: 1,
                    error_msg: msg,
                    column_defs: Vec::new(),
                    column_fields: Vec::new(),
                    extra_result: String::new(),
                };
                let resp = match &engine {
                    Some(eng) => match eng.execute(&req.to_sql()) {
                        Ok(rs) => {
                            let mut columns: Vec<String> = Vec::new();
                            let mut rows: Vec<Vec<String>> = Vec::new();
                            for r in rs {
                                if let QueryResult::Rows {
                                    columns: c,
                                    rows: rw,
                                } = r
                                {
                                    columns = c;
                                    rows = rw;
                                }
                            }
                            select_response_from(&columns, &rows)
                        }
                        Err(e) => err(e.to_string()),
                    },
                    None => err("no engine bound".into()),
                };
                write_select_response(&mut writer, seq, &resp)?;
            }
            other => {
                {
                    let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                    skip_struct(&mut ip)?;
                }
                write_app_exception(&mut writer, other, seq, &format!("unknown method: {other}"))?;
            }
        }
    }
    Ok(())
}

/// Applies a `CreateTable` request to the engine (or errors if no engine bound).
fn run_create_table(engine: Option<&Engine>, req: &CreateTableReq) -> CommonResponse {
    let Some(engine) = engine else {
        return CommonResponse {
            error_code: 1,
            error_msg: "no engine bound".into(),
            session_id: req.session_id,
        };
    };
    let cols: Vec<String> = req
        .columns
        .iter()
        .map(|c| format!("{} VARCHAR", c.name))
        .collect();
    let sql = format!("CREATE TABLE {} ({});", req.table_name, cols.join(", "));
    match engine.execute(&sql) {
        Ok(_) => CommonResponse {
            error_code: OK,
            error_msg: "OK".into(),
            session_id: req.session_id,
        },
        Err(e) => CommonResponse {
            error_code: 1,
            error_msg: e.to_string(),
            session_id: req.session_id,
        },
    }
}

/// A client-side helper used by tests (and, later, by an SDK path).
pub fn connect(addr: &str, client_version: i64) -> Result<CommonResponse> {
    let mut stream = TcpStream::connect(addr).map_err(|e| Error::io(addr, e))?;
    write_connect_call(&mut stream, 1, client_version)?;
    let mut reader = stream.try_clone().map_err(io)?;
    read_connect_reply(&mut reader)
}

/// A client-side `Disconnect` for the given session.
pub fn disconnect(addr: &str, session_id: i64) -> Result<CommonResponse> {
    let mut stream = TcpStream::connect(addr).map_err(|e| Error::io(addr, e))?;
    write_common_call(&mut stream, "Disconnect", 2, session_id)?;
    let mut reader = stream.try_clone().map_err(io)?;
    read_connect_reply(&mut reader)
}

/// A client-side `CreateTable`.
pub fn create_table(addr: &str, req: &CreateTableReq) -> Result<CommonResponse> {
    let mut stream = TcpStream::connect(addr).map_err(|e| Error::io(addr, e))?;
    write_create_table_call(&mut stream, 1, req)?;
    let mut reader = stream.try_clone().map_err(io)?;
    read_connect_reply(&mut reader)
}

// -------- columnar result encoding (SelectResponse `column_fields`) --------

/// A `ColumnField` (from `thrift/infinity.thrift`): one result column as opaque
/// binary vectors plus a null bitmask.
#[derive(Debug, Clone, PartialEq)]
pub struct ColumnField {
    pub column_type: i32,
    pub column_name: String,
    pub column_vectors: Vec<Vec<u8>>,
    pub bitmasks: Vec<bool>,
}

/// Encodes a result set into `ColumnField`s (one per column). Row values are
/// UTF-8 bytes; NULL becomes an empty vector with bitmask `false`.
pub fn rows_to_column_fields(columns: &[String], rows: &[Vec<String>]) -> Vec<ColumnField> {
    let mut out = Vec::new();
    for (ci, name) in columns.iter().enumerate() {
        let mut vecs = Vec::with_capacity(rows.len());
        let mut masks = Vec::with_capacity(rows.len());
        for r in rows {
            match r.get(ci) {
                Some(v) if v != "NULL" => {
                    vecs.push(v.as_bytes().to_vec());
                    masks.push(true);
                }
                _ => {
                    vecs.push(Vec::new());
                    masks.push(false);
                }
            }
        }
        out.push(ColumnField {
            column_type: 25,
            column_name: name.clone(),
            column_vectors: vecs,
            bitmasks: masks,
        });
    }
    out
}

/// Writes a `list<ColumnField>` value (no enclosing struct).
pub fn write_column_fields(w: &mut impl Write, fields: &[ColumnField]) -> Result<()> {
    let mut o = TBinaryOutputProtocol::new(w, true);
    o.write_list_begin(&TListIdentifier::new(TType::Struct, fields.len() as i32))
        .map_err(tw)?;
    for f in fields {
        o.write_struct_begin(&TStructIdentifier::new("ColumnField"))
            .map_err(tw)?;
        o.write_field_begin(&TFieldIdentifier::new("column_type", TType::I32, 1i16))
            .map_err(tw)?;
        o.write_i32(f.column_type).map_err(tw)?;
        o.write_field_end().map_err(tw)?;
        o.write_field_begin(&TFieldIdentifier::new("column_vectors", TType::List, 2i16))
            .map_err(tw)?;
        o.write_list_begin(&TListIdentifier::new(
            TType::String,
            f.column_vectors.len() as i32,
        ))
        .map_err(tw)?;
        for v in &f.column_vectors {
            o.write_bytes(v).map_err(tw)?;
        }
        o.write_list_end().map_err(tw)?;
        o.write_field_end().map_err(tw)?;
        o.write_field_begin(&TFieldIdentifier::new("column_name", TType::String, 3i16))
            .map_err(tw)?;
        o.write_string(&f.column_name).map_err(tw)?;
        o.write_field_end().map_err(tw)?;
        o.write_field_begin(&TFieldIdentifier::new("bitmasks", TType::List, 4i16))
            .map_err(tw)?;
        o.write_list_begin(&TListIdentifier::new(TType::Bool, f.bitmasks.len() as i32))
            .map_err(tw)?;
        for b in &f.bitmasks {
            o.write_bool(*b).map_err(tw)?;
        }
        o.write_list_end().map_err(tw)?;
        o.write_field_end().map_err(tw)?;
        o.write_field_stop().map_err(tw)?;
        o.write_struct_end().map_err(tw)?;
    }
    o.write_list_end().map_err(tw)?;
    o.flush().map_err(tw)?;
    Ok(())
}

/// Reads a `list<ColumnField>` value.
pub fn read_column_fields(r: &mut impl Read) -> Result<Vec<ColumnField>> {
    let mut i = TBinaryInputProtocol::new(r, true);
    let li = i.read_list_begin().map_err(tw)?;
    let mut out = Vec::with_capacity(li.size.max(0) as usize);
    for _ in 0..li.size {
        i.read_struct_begin().map_err(tw)?;
        let mut f = ColumnField {
            column_type: 0,
            column_name: String::new(),
            column_vectors: vec![],
            bitmasks: vec![],
        };
        loop {
            let fld = i.read_field_begin().map_err(tw)?;
            if fld.field_type == TType::Stop {
                break;
            }
            match fld.id {
                Some(1) => f.column_type = i.read_i32().map_err(tw)?,
                Some(2) => {
                    let l = i.read_list_begin().map_err(tw)?;
                    for _ in 0..l.size {
                        f.column_vectors.push(i.read_bytes().map_err(tw)?);
                    }
                    i.read_list_end().map_err(tw)?;
                }
                Some(3) => f.column_name = i.read_string().map_err(tw)?,
                Some(4) => {
                    let l = i.read_list_begin().map_err(tw)?;
                    for _ in 0..l.size {
                        f.bitmasks.push(i.read_bool().map_err(tw)?);
                    }
                    i.read_list_end().map_err(tw)?;
                }
                _ => i.skip(fld.field_type).map_err(tw)?,
            }
            i.read_field_end().map_err(tw)?;
        }
        i.read_struct_end().map_err(tw)?;
        out.push(f);
    }
    i.read_list_end().map_err(tw)?;
    Ok(out)
}

/// A minimal `ColumnDef` (`thrift` field ids: 1 id, 2 name, 3 data_type{1 logic_type}`).
#[derive(Debug, Clone, PartialEq)]
pub struct ColumnDefWire {
    pub id: i32,
    pub name: String,
    /// `LogicType` enum ordinal (caller-supplied).
    pub logic_type: i32,
}

/// A minimal `SelectResponse` (fields 1..=5).
#[derive(Debug, Clone, PartialEq)]
pub struct SelectResponseWire {
    pub error_code: i64,
    pub error_msg: String,
    pub column_defs: Vec<ColumnDefWire>,
    pub column_fields: Vec<ColumnField>,
    pub extra_result: String,
}

/// Writes a `Select` REPLY carrying a `SelectResponse`.
pub fn write_select_response(
    w: &mut impl Write,
    seq: i32,
    resp: &SelectResponseWire,
) -> Result<()> {
    let mut o = TBinaryOutputProtocol::new(w, true);
    o.write_message_begin(&TMessageIdentifier::new("Select", TMessageType::Reply, seq))
        .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("Select_result"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("success", TType::Struct, 0i16))
        .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("SelectResponse"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("error_code", TType::I64, 1i16))
        .map_err(tw)?;
    o.write_i64(resp.error_code).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("error_msg", TType::String, 2i16))
        .map_err(tw)?;
    o.write_string(&resp.error_msg).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    // column_defs (list<ColumnDef>)
    o.write_field_begin(&TFieldIdentifier::new("column_defs", TType::List, 3i16))
        .map_err(tw)?;
    o.write_list_begin(&TListIdentifier::new(
        TType::Struct,
        resp.column_defs.len() as i32,
    ))
    .map_err(tw)?;
    for cd in &resp.column_defs {
        o.write_struct_begin(&TStructIdentifier::new("ColumnDef"))
            .map_err(tw)?;
        o.write_field_begin(&TFieldIdentifier::new("id", TType::I32, 1i16))
            .map_err(tw)?;
        o.write_i32(cd.id).map_err(tw)?;
        o.write_field_end().map_err(tw)?;
        o.write_field_begin(&TFieldIdentifier::new("name", TType::String, 2i16))
            .map_err(tw)?;
        o.write_string(&cd.name).map_err(tw)?;
        o.write_field_end().map_err(tw)?;
        o.write_field_begin(&TFieldIdentifier::new("data_type", TType::Struct, 3i16))
            .map_err(tw)?;
        o.write_struct_begin(&TStructIdentifier::new("DataType"))
            .map_err(tw)?;
        o.write_field_begin(&TFieldIdentifier::new("logic_type", TType::I32, 1i16))
            .map_err(tw)?;
        o.write_i32(cd.logic_type).map_err(tw)?;
        o.write_field_end().map_err(tw)?;
        o.write_field_stop().map_err(tw)?;
        o.write_struct_end().map_err(tw)?;
        o.write_field_end().map_err(tw)?;
        o.write_field_stop().map_err(tw)?;
        o.write_struct_end().map_err(tw)?;
    }
    o.write_list_end().map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    // column_fields (list<ColumnField>)
    o.write_field_begin(&TFieldIdentifier::new("column_fields", TType::List, 4i16))
        .map_err(tw)?;
    o.write_list_begin(&TListIdentifier::new(
        TType::Struct,
        resp.column_fields.len() as i32,
    ))
    .map_err(tw)?;
    for f in &resp.column_fields {
        write_one_column_field(&mut o, f)?;
    }
    o.write_list_end().map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("extra_result", TType::String, 5i16))
        .map_err(tw)?;
    o.write_string(&resp.extra_result).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?; // SelectResponse
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?; // Select_result
    o.flush().map_err(tw)?;
    Ok(())
}

fn write_one_column_field(
    o: &mut TBinaryOutputProtocol<&mut impl Write>,
    f: &ColumnField,
) -> Result<()> {
    o.write_struct_begin(&TStructIdentifier::new("ColumnField"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("column_type", TType::I32, 1i16))
        .map_err(tw)?;
    o.write_i32(f.column_type).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("column_vectors", TType::List, 2i16))
        .map_err(tw)?;
    o.write_list_begin(&TListIdentifier::new(
        TType::String,
        f.column_vectors.len() as i32,
    ))
    .map_err(tw)?;
    for v in &f.column_vectors {
        o.write_bytes(v).map_err(tw)?;
    }
    o.write_list_end().map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("column_name", TType::String, 3i16))
        .map_err(tw)?;
    o.write_string(&f.column_name).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("bitmasks", TType::List, 4i16))
        .map_err(tw)?;
    o.write_list_begin(&TListIdentifier::new(TType::Bool, f.bitmasks.len() as i32))
        .map_err(tw)?;
    for b in &f.bitmasks {
        o.write_bool(*b).map_err(tw)?;
    }
    o.write_list_end().map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?;
    Ok(())
}

// -------- CreateTable request decode --------

/// The subset of `CreateTableRequest` needed to run a DDL statement.
#[derive(Debug, Clone, PartialEq)]
pub struct CreateTableReq {
    pub db_name: String,
    pub table_name: String,
    pub columns: Vec<ColumnDefWire>,
    pub session_id: i64,
}

fn read_column_def(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<ColumnDefWire> {
    ip.read_struct_begin().map_err(tw)?;
    let (mut id, mut name, mut lt) = (0i32, String::new(), 0i32);
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match f.id {
            Some(1) => id = ip.read_i32().map_err(tw)?,
            Some(2) => name = ip.read_string().map_err(tw)?,
            Some(3) => {
                ip.read_struct_begin().map_err(tw)?; // DataType
                loop {
                    let g = ip.read_field_begin().map_err(tw)?;
                    if g.field_type == TType::Stop {
                        break;
                    }
                    match g.id {
                        Some(1) => lt = ip.read_i32().map_err(tw)?,
                        _ => ip.skip(g.field_type).map_err(tw)?,
                    }
                    ip.read_field_end().map_err(tw)?;
                }
                ip.read_struct_end().map_err(tw)?;
            }
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok(ColumnDefWire {
        id,
        name,
        logic_type: lt,
    })
}

/// Reads the args of a `CreateTable` CALL (after the message head).
pub fn read_create_table_args(
    ip: &mut TBinaryInputProtocol<&mut impl Read>,
) -> Result<CreateTableReq> {
    ip.read_struct_begin().map_err(tw)?; // CreateTable_args
    let _ = ip.read_field_begin().map_err(tw)?; // request (Struct)
    ip.read_struct_begin().map_err(tw)?; // CreateTableRequest
    let mut req = CreateTableReq {
        db_name: String::new(),
        table_name: String::new(),
        columns: Vec::new(),
        session_id: 0,
    };
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match f.id {
            Some(1) => req.db_name = ip.read_string().map_err(tw)?,
            Some(2) => req.table_name = ip.read_string().map_err(tw)?,
            Some(3) => {
                let l = ip.read_list_begin().map_err(tw)?;
                for _ in 0..l.size {
                    req.columns.push(read_column_def(ip)?);
                }
                ip.read_list_end().map_err(tw)?;
            }
            Some(6) => req.session_id = ip.read_i64().map_err(tw)?,
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?; // CreateTableRequest
    Ok(req)
}

/// Reads a full `CreateTable` CALL, returning `(seqid, request)`.
pub fn read_create_table_call(r: &mut impl Read) -> Result<(i32, CreateTableReq)> {
    let mut ip = TBinaryInputProtocol::new(r, true);
    let msg = ip.read_message_begin().map_err(tw)?;
    let req = read_create_table_args(&mut ip)?;
    Ok((msg.sequence_number, req))
}

/// Writes a `CreateTable` CALL (mirrors the SDK `CreateTable_args` layout).
pub fn write_create_table_call(w: &mut impl Write, seq: i32, req: &CreateTableReq) -> Result<()> {
    let mut o = TBinaryOutputProtocol::new(w, true);
    o.write_message_begin(&TMessageIdentifier::new(
        "CreateTable",
        TMessageType::Call,
        seq,
    ))
    .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("CreateTable_args"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("request", TType::Struct, 1i16))
        .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("CreateTableRequest"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("db_name", TType::String, 1i16))
        .map_err(tw)?;
    o.write_string(&req.db_name).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("table_name", TType::String, 2i16))
        .map_err(tw)?;
    o.write_string(&req.table_name).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("column_defs", TType::List, 3i16))
        .map_err(tw)?;
    o.write_list_begin(&TListIdentifier::new(
        TType::Struct,
        req.columns.len() as i32,
    ))
    .map_err(tw)?;
    for c in &req.columns {
        o.write_struct_begin(&TStructIdentifier::new("ColumnDef"))
            .map_err(tw)?;
        o.write_field_begin(&TFieldIdentifier::new("id", TType::I32, 1i16))
            .map_err(tw)?;
        o.write_i32(c.id).map_err(tw)?;
        o.write_field_end().map_err(tw)?;
        o.write_field_begin(&TFieldIdentifier::new("name", TType::String, 2i16))
            .map_err(tw)?;
        o.write_string(&c.name).map_err(tw)?;
        o.write_field_end().map_err(tw)?;
        o.write_field_begin(&TFieldIdentifier::new("data_type", TType::Struct, 3i16))
            .map_err(tw)?;
        o.write_struct_begin(&TStructIdentifier::new("DataType"))
            .map_err(tw)?;
        o.write_field_begin(&TFieldIdentifier::new("logic_type", TType::I32, 1i16))
            .map_err(tw)?;
        o.write_i32(c.logic_type).map_err(tw)?;
        o.write_field_end().map_err(tw)?;
        o.write_field_stop().map_err(tw)?;
        o.write_struct_end().map_err(tw)?;
        o.write_field_end().map_err(tw)?;
        o.write_field_stop().map_err(tw)?;
        o.write_struct_end().map_err(tw)?;
    }
    o.write_list_end().map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("session_id", TType::I64, 6i16))
        .map_err(tw)?;
    o.write_i64(req.session_id).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?;
    o.flush().map_err(tw)?;
    Ok(())
}

// -------- Select request decode + result build --------

/// The subset of `SelectRequest` needed to run `SELECT <cols|*> FROM table`.
#[derive(Debug, Clone, PartialEq)]
pub struct SelectReq {
    pub session_id: i64,
    pub db_name: String,
    pub table_name: String,
    /// Projected columns (empty + `star=false` means `*`).
    pub columns: Vec<String>,
    pub star: bool,
    /// Optional `WHERE` clause already rendered as SQL.
    pub where_sql: Option<String>,
    /// Optional `(column, ascending)` order key.
    pub order_by: Option<(String, bool)>,
    pub limit: Option<i64>,
}

impl SelectReq {
    /// Builds the SQL for this request.
    pub fn to_sql(&self) -> String {
        let cols = if self.star || self.columns.is_empty() {
            "*".to_string()
        } else {
            self.columns.join(", ")
        };
        let mut sql = format!("SELECT {} FROM {}", cols, self.table_name);
        if let Some(w) = &self.where_sql {
            sql.push_str(&format!(" WHERE {w}"));
        }
        if let Some((c, asc)) = &self.order_by {
            sql.push_str(&format!(
                " ORDER BY {c} {}",
                if *asc { "ASC" } else { "DESC" }
            ));
        }
        if let Some(n) = self.limit {
            sql.push_str(&format!(" LIMIT {n}"));
        }
        sql.push(';');
        sql
    }
}

/// A decoded `ParsedExpr` (subset): a column reference, a constant, or unknown.
#[derive(Debug, Clone, PartialEq)]
pub enum PExpr {
    Column(String),
    Value(ValueLit),
    Other,
}

/// Reads any `ParsedExpr`, extracting the `ConstantExpr`/`ColumnExpr` form.
pub fn read_pexpr(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<PExpr> {
    ip.read_struct_begin().map_err(tw)?; // ParsedExpr
    let mut out = PExpr::Other;
    loop {
        let fd = ip.read_field_begin().map_err(tw)?;
        if fd.field_type == TType::Stop {
            break;
        }
        if fd.id == Some(1) {
            ip.read_struct_begin().map_err(tw)?; // union
            let uf = ip.read_field_begin().map_err(tw)?;
            match uf.id {
                Some(1) if uf.field_type == TType::Struct => {
                    out = PExpr::Value(read_constant_expr(ip)?)
                }
                Some(2) if uf.field_type == TType::Struct => {
                    let (names, _) = read_column_expr(ip)?;
                    out = PExpr::Column(names.into_iter().next().unwrap_or_default());
                }
                _ => ip.skip(uf.field_type).map_err(tw)?,
            }
            let _ = ip.read_field_end();
            let _ = ip.read_field_begin(); // stop of union
            ip.read_struct_end().map_err(tw)?;
        } else {
            ip.skip(fd.field_type).map_err(tw)?;
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok(out)
}

fn sql_op(name: &str) -> Option<&'static str> {
    Some(match name {
        "=" | "==" => "=",
        "!=" | "<>" => "!=",
        "<" => "<",
        "<=" => "<=",
        ">" => ">",
        ">=" => ">=",
        _ => return None,
    })
}

/// Reads a `where_expr` (`ParsedExpr` → `FunctionExpr`) and renders a predicate.
fn read_where(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<Option<String>> {
    ip.read_struct_begin().map_err(tw)?; // ParsedExpr
    let mut pred = None;
    loop {
        let fd = ip.read_field_begin().map_err(tw)?;
        if fd.field_type == TType::Stop {
            break;
        }
        if fd.id == Some(1) {
            // union: field 3 = function_expr
            ip.read_struct_begin().map_err(tw)?;
            let uf = ip.read_field_begin().map_err(tw)?;
            if uf.id == Some(3) && uf.field_type == TType::Struct {
                // FunctionExpr { 1: name, 2: args }
                ip.read_struct_begin().map_err(tw)?;
                let (mut name, mut args) = (String::new(), Vec::new());
                loop {
                    let g = ip.read_field_begin().map_err(tw)?;
                    if g.field_type == TType::Stop {
                        break;
                    }
                    match g.id {
                        Some(1) => name = ip.read_string().map_err(tw)?,
                        Some(2) => {
                            let l = ip.read_list_begin().map_err(tw)?;
                            for _ in 0..l.size {
                                args.push(read_pexpr(ip)?);
                            }
                            ip.read_list_end().map_err(tw)?;
                        }
                        _ => ip.skip(g.field_type).map_err(tw)?,
                    }
                    ip.read_field_end().map_err(tw)?;
                }
                ip.read_struct_end().map_err(tw)?;
                if let (Some(op), [PExpr::Column(c), PExpr::Value(v)]) = (sql_op(&name), &args[..])
                {
                    pred = Some(format!("{c} {op} {}", v.to_sql()));
                }
            } else {
                ip.skip(uf.field_type).map_err(tw)?;
            }
            let _ = ip.read_field_end();
            let _ = ip.read_field_begin();
            ip.read_struct_end().map_err(tw)?;
        } else {
            ip.skip(fd.field_type).map_err(tw)?;
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok(pred)
}

/// Reads a `ColumnExpr`: `{1: list<string> column_name, 2: bool star}`.
fn read_column_expr(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<(Vec<String>, bool)> {
    ip.read_struct_begin().map_err(tw)?;
    let (mut names, mut star) = (Vec::new(), false);
    loop {
        let fd = ip.read_field_begin().map_err(tw)?;
        if fd.field_type == TType::Stop {
            break;
        }
        match fd.id {
            Some(1) => {
                let l = ip.read_list_begin().map_err(tw)?;
                for _ in 0..l.size {
                    names.push(ip.read_string().map_err(tw)?);
                }
                ip.read_list_end().map_err(tw)?;
            }
            Some(2) => star = ip.read_bool().map_err(tw)?,
            _ => ip.skip(fd.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok((names, star))
}

/// Reads a `ParsedExpr` in its `ColumnExpr` form (`ParsedExprType` union field 2).
fn read_select_item(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<(Vec<String>, bool)> {
    ip.read_struct_begin().map_err(tw)?; // ParsedExpr
    let (mut names, mut star) = (Vec::new(), false);
    loop {
        let fd = ip.read_field_begin().map_err(tw)?;
        if fd.field_type == TType::Stop {
            break;
        }
        if fd.id == Some(1) {
            ip.read_struct_begin().map_err(tw)?; // ParsedExprType union
            let uf = ip.read_field_begin().map_err(tw)?;
            if uf.id == Some(2) && uf.field_type == TType::Struct {
                let (n, s) = read_column_expr(ip)?;
                names = n;
                star = s;
            }
            let _ = ip.read_field_end();
            let _ = ip.read_field_begin(); // stop of union
            ip.read_struct_end().map_err(tw)?;
        } else {
            ip.skip(fd.field_type).map_err(tw)?;
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok((names, star))
}

/// Reads an `order_by_list` (`list<OrderByExpr{1: ParsedExpr, 2: bool asc}>`),
/// returning the first `(column, ascending)`.
fn read_order_by(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<Option<(String, bool)>> {
    let l = ip.read_list_begin().map_err(tw)?;
    let mut out = None;
    for _ in 0..l.size {
        ip.read_struct_begin().map_err(tw)?; // OrderByExpr
        let mut col = String::new();
        let mut asc = true;
        loop {
            let fd = ip.read_field_begin().map_err(tw)?;
            if fd.field_type == TType::Stop {
                break;
            }
            match fd.id {
                Some(1) => {
                    if let PExpr::Column(c) = read_pexpr(ip)? {
                        col = c;
                    }
                }
                Some(2) => asc = ip.read_bool().map_err(tw)?,
                _ => ip.skip(fd.field_type).map_err(tw)?,
            }
            ip.read_field_end().map_err(tw)?;
        }
        ip.read_struct_end().map_err(tw)?;
        if out.is_none() && !col.is_empty() {
            out = Some((col, asc));
        }
    }
    ip.read_list_end().map_err(tw)?;
    Ok(out)
}

/// Reads the args of a `Select` CALL (after the message head). An empty
/// `select_list` is treated as `SELECT *`.
pub fn read_select_args(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<SelectReq> {
    ip.read_struct_begin().map_err(tw)?; // Select_args
    let _ = ip.read_field_begin().map_err(tw)?; // request (Struct)
    ip.read_struct_begin().map_err(tw)?; // SelectRequest
    let mut r = SelectReq {
        session_id: 0,
        db_name: String::new(),
        table_name: String::new(),
        columns: Vec::new(),
        star: false,
        where_sql: None,
        order_by: None,
        limit: None,
    };
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match f.id {
            Some(1) => r.session_id = ip.read_i64().map_err(tw)?,
            Some(2) => r.db_name = ip.read_string().map_err(tw)?,
            Some(3) => r.table_name = ip.read_string().map_err(tw)?,
            Some(4) => {
                let l = ip.read_list_begin().map_err(tw)?;
                for _ in 0..l.size {
                    let (names, star) = read_select_item(ip)?;
                    r.columns.extend(names);
                    r.star |= star;
                }
                ip.read_list_end().map_err(tw)?;
            }
            Some(7) if f.field_type == TType::Struct => r.where_sql = read_where(ip)?,
            Some(10) if f.field_type == TType::Struct => {
                if let PExpr::Value(ValueLit::Int(n)) = read_pexpr(ip)? {
                    r.limit = Some(n);
                }
            }
            Some(12) => r.order_by = read_order_by(ip)?,
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?; // SelectRequest
    Ok(r)
}

/// Reads a full `Select` CALL, returning `(seqid, request)`.
pub fn read_select_call(r: &mut impl Read) -> Result<(i32, SelectReq)> {
    let mut ip = TBinaryInputProtocol::new(r, true);
    let msg = ip.read_message_begin().map_err(tw)?;
    let req = read_select_args(&mut ip)?;
    Ok((msg.sequence_number, req))
}

/// Builds a `SelectResponse` from a materialized result set.
pub fn select_response_from(columns: &[String], rows: &[Vec<String>]) -> SelectResponseWire {
    let column_defs = columns
        .iter()
        .enumerate()
        .map(|(i, name)| ColumnDefWire {
            id: i as i32,
            name: name.clone(),
            logic_type: 25,
        })
        .collect();
    SelectResponseWire {
        error_code: 0,
        error_msg: "OK".into(),
        column_defs,
        column_fields: rows_to_column_fields(columns, rows),
        extra_result: String::new(),
    }
}

// -------- Insert request decode --------

/// A decoded literal value (from `ConstantExpr`).
#[derive(Debug, Clone, PartialEq)]
pub enum ValueLit {
    Int(i64),
    Float(f64),
    Str(String),
    Bool(bool),
    Null,
}

impl ValueLit {
    pub fn to_sql(&self) -> String {
        match self {
            ValueLit::Int(i) => i.to_string(),
            ValueLit::Float(f) => f.to_string(),
            ValueLit::Str(s) => format!("'{}'", s.replace('\'', "''")),
            ValueLit::Bool(b) => {
                if *b {
                    "1".to_string()
                } else {
                    "0".to_string()
                }
            }
            ValueLit::Null => "NULL".to_string(),
        }
    }
}

/// The subset of `InsertRequest` needed to run `INSERT`.
#[derive(Debug, Clone, PartialEq)]
pub struct InsertReq {
    pub db_name: String,
    pub table_name: String,
    /// One entry per row: its column list and values.
    pub rows: Vec<(Vec<String>, Vec<ValueLit>)>,
    pub session_id: i64,
}

fn read_constant_expr(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<ValueLit> {
    ip.read_struct_begin().map_err(tw)?;
    let (mut lt, mut b, mut i, mut f, mut s) = (0i32, None, None, None, None);
    loop {
        let fd = ip.read_field_begin().map_err(tw)?;
        if fd.field_type == TType::Stop {
            break;
        }
        match fd.id {
            Some(1) => lt = ip.read_i32().map_err(tw)?,
            Some(2) => b = Some(ip.read_bool().map_err(tw)?),
            Some(3) => i = Some(ip.read_i64().map_err(tw)?),
            Some(4) => f = Some(ip.read_double().map_err(tw)?),
            Some(5) => s = Some(ip.read_string().map_err(tw)?),
            _ => ip.skip(fd.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok(match lt {
        0 => ValueLit::Bool(b.unwrap_or(false)),
        1 => ValueLit::Float(f.unwrap_or(0.0)),
        2 => ValueLit::Str(s.unwrap_or_default()),
        3 => ValueLit::Int(i.unwrap_or(0)),
        _ => ValueLit::Null,
    })
}

fn read_parsed_expr(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<ValueLit> {
    ip.read_struct_begin().map_err(tw)?; // ParsedExpr
    let mut val = ValueLit::Null;
    loop {
        let fd = ip.read_field_begin().map_err(tw)?;
        if fd.field_type == TType::Stop {
            break;
        }
        if fd.id == Some(1) {
            // ParsedExprType union: field 1 = constant_expr
            ip.read_struct_begin().map_err(tw)?;
            let uf = ip.read_field_begin().map_err(tw)?;
            if uf.field_type == TType::Struct {
                val = read_constant_expr(ip)?;
            }
            let _ = ip.read_field_end();
            let _ = ip.read_field_begin(); // stop of union
            ip.read_struct_end().map_err(tw)?;
        } else {
            ip.skip(fd.field_type).map_err(tw)?;
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok(val)
}

fn read_insert_field(
    ip: &mut TBinaryInputProtocol<&mut impl Read>,
) -> Result<(Vec<String>, Vec<ValueLit>)> {
    ip.read_struct_begin().map_err(tw)?; // Field
    let mut cols: Vec<String> = Vec::new();
    let mut vals = Vec::new();
    loop {
        let fd = ip.read_field_begin().map_err(tw)?;
        if fd.field_type == TType::Stop {
            break;
        }
        match fd.id {
            Some(1) => {
                let l = ip.read_list_begin().map_err(tw)?;
                for _ in 0..l.size {
                    cols.push(ip.read_string().map_err(tw)?);
                }
                ip.read_list_end().map_err(tw)?;
            }
            Some(2) => {
                let l = ip.read_list_begin().map_err(tw)?;
                for _ in 0..l.size {
                    vals.push(read_parsed_expr(ip)?);
                }
                ip.read_list_end().map_err(tw)?;
            }
            _ => ip.skip(fd.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok((cols, vals))
}

/// Reads the args of an `Insert` CALL (after the message head).
pub fn read_insert_args(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<InsertReq> {
    ip.read_struct_begin().map_err(tw)?; // Insert_args
    let _ = ip.read_field_begin().map_err(tw)?; // request (Struct)
    ip.read_struct_begin().map_err(tw)?; // InsertRequest
    let mut r = InsertReq {
        db_name: String::new(),
        table_name: String::new(),
        rows: Vec::new(),
        session_id: 0,
    };
    loop {
        let fd = ip.read_field_begin().map_err(tw)?;
        if fd.field_type == TType::Stop {
            break;
        }
        match fd.id {
            Some(1) => r.db_name = ip.read_string().map_err(tw)?,
            Some(2) => r.table_name = ip.read_string().map_err(tw)?,
            Some(3) => {
                let l = ip.read_list_begin().map_err(tw)?;
                for _ in 0..l.size {
                    r.rows.push(read_insert_field(ip)?);
                }
                ip.read_list_end().map_err(tw)?;
            }
            Some(4) => r.session_id = ip.read_i64().map_err(tw)?,
            _ => ip.skip(fd.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?; // InsertRequest
    Ok(r)
}

/// Reads a full `Insert` CALL, returning `(seqid, request)`.
pub fn read_insert_call(r: &mut impl Read) -> Result<(i32, InsertReq)> {
    let mut ip = TBinaryInputProtocol::new(r, true);
    let msg = ip.read_message_begin().map_err(tw)?;
    let req = read_insert_args(&mut ip)?;
    Ok((msg.sequence_number, req))
}

// -------- ListDatabase --------

/// Reads the args of a `ListDatabase` CALL (after the message head) → session_id.
pub fn read_list_database_args(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<i64> {
    ip.read_struct_begin().map_err(tw)?; // ListDatabase_args
    let _ = ip.read_field_begin().map_err(tw)?; // request (Struct)
    ip.read_struct_begin().map_err(tw)?; // ListDatabaseRequest
    let mut sid = 0i64;
    loop {
        let fd = ip.read_field_begin().map_err(tw)?;
        if fd.field_type == TType::Stop {
            break;
        }
        match fd.id {
            Some(1) => sid = ip.read_i64().map_err(tw)?,
            _ => ip.skip(fd.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok(sid)
}

/// Reads a `<Method>_args` CALL whose sole `request` field is a struct of
/// string fields plus a `session_id`, returning the session id and the string
/// fields keyed by their thrift field id (other fields are skipped).
pub fn read_string_request(
    ip: &mut TBinaryInputProtocol<&mut impl Read>,
) -> Result<(i64, std::collections::HashMap<i16, String>)> {
    ip.read_struct_begin().map_err(tw)?; // <Method>_args
    let _ = ip.read_field_begin().map_err(tw)?; // request (Struct)
    ip.read_struct_begin().map_err(tw)?; // <Method>Request
    let mut sid = 0i64;
    let mut strs: std::collections::HashMap<i16, String> = Default::default();
    loop {
        let fd = ip.read_field_begin().map_err(tw)?;
        if fd.field_type == TType::Stop {
            break;
        }
        match fd.field_type {
            TType::String => match fd.id {
                Some(id) => {
                    strs.insert(id, ip.read_string().map_err(tw)?);
                }
                None => ip.skip(fd.field_type).map_err(tw)?,
            },
            TType::I64 => sid = ip.read_i64().map_err(tw)?,
            // enum options (ConflictType …) are carried as i32 fields
            TType::I32 => match fd.id {
                Some(id) => {
                    strs.insert(id, ip.read_i32().map_err(tw)?.to_string());
                }
                None => ip.skip(fd.field_type).map_err(tw)?,
            },
            // a wrapper struct holding the option enum (`{[1: enum]}`)
            TType::Struct => {
                ip.read_struct_begin().map_err(tw)?;
                loop {
                    let fd2 = ip.read_field_begin().map_err(tw)?;
                    if fd2.field_type == TType::Stop {
                        break;
                    }
                    if fd2.field_type == TType::I32 {
                        if let Some(id) = fd.id {
                            strs.insert(id, ip.read_i32().map_err(tw)?.to_string());
                        } else {
                            ip.skip(fd2.field_type).map_err(tw)?;
                        }
                    } else {
                        ip.skip(fd2.field_type).map_err(tw)?;
                    }
                    ip.read_field_end().map_err(tw)?;
                }
                ip.read_struct_end().map_err(tw)?;
            }
            _ => ip.skip(fd.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok((sid, strs))
}

/// Executes `sql` against the bound engine, mapping the outcome to a
/// `CommonResponse` (`error_code` 0 on success).
fn run_common(engine: Option<&Arc<Engine>>, sql: &str, session_id: i64) -> CommonResponse {
    match engine {
        Some(eng) => match eng.execute(sql) {
            Ok(_) => CommonResponse {
                error_code: OK,
                error_msg: "OK".into(),
                session_id,
            },
            Err(e) => CommonResponse {
                error_code: 1,
                error_msg: e.to_string(),
                session_id,
            },
        },
        None => CommonResponse {
            error_code: 1,
            error_msg: "no engine bound".into(),
            session_id,
        },
    }
}

/// Reads a full `ListDatabase` CALL, returning `(seqid, session_id)`.
pub fn read_list_database_call(r: &mut impl Read) -> Result<(i32, i64)> {
    let mut ip = TBinaryInputProtocol::new(r, true);
    let msg = ip.read_message_begin().map_err(tw)?;
    let sid = read_list_database_args(&mut ip)?;
    Ok((msg.sequence_number, sid))
}

/// Writes a `ListDatabase` REPLY with the given database names.
pub fn write_list_database_response(w: &mut impl Write, seq: i32, names: &[String]) -> Result<()> {
    let mut o = TBinaryOutputProtocol::new(w, true);
    o.write_message_begin(&TMessageIdentifier::new(
        "ListDatabase",
        TMessageType::Reply,
        seq,
    ))
    .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("ListDatabase_result"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("success", TType::Struct, 0i16))
        .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("ListDatabaseResponse"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("error_code", TType::I64, 1i16))
        .map_err(tw)?;
    o.write_i64(0).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("error_msg", TType::String, 2i16))
        .map_err(tw)?;
    o.write_string("OK").map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("db_names", TType::List, 3i16))
        .map_err(tw)?;
    o.write_list_begin(&TListIdentifier::new(TType::String, names.len() as i32))
        .map_err(tw)?;
    for n in names {
        o.write_string(n).map_err(tw)?;
    }
    o.write_list_end().map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?; // ListDatabaseResponse
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?; // ListDatabase_result
    o.flush().map_err(tw)?;
    Ok(())
}

// -------- Delete --------

/// The subset of `DeleteRequest` needed to run `DELETE`.
#[derive(Debug, Clone, PartialEq)]
pub struct DeleteReq {
    pub db_name: String,
    pub table_name: String,
    pub where_sql: Option<String>,
    pub session_id: i64,
}

impl DeleteReq {
    pub fn to_sql(&self) -> String {
        match &self.where_sql {
            Some(w) => format!("DELETE FROM {} WHERE {w};", self.table_name),
            None => format!("DELETE FROM {};", self.table_name),
        }
    }
}

/// Reads the args of a `Delete` CALL (after the message head).
pub fn read_delete_args(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<DeleteReq> {
    ip.read_struct_begin().map_err(tw)?; // Delete_args
    let _ = ip.read_field_begin().map_err(tw)?; // request (Struct)
    ip.read_struct_begin().map_err(tw)?; // DeleteRequest
    let mut r = DeleteReq {
        db_name: String::new(),
        table_name: String::new(),
        where_sql: None,
        session_id: 0,
    };
    loop {
        let fd = ip.read_field_begin().map_err(tw)?;
        if fd.field_type == TType::Stop {
            break;
        }
        match fd.id {
            Some(1) => r.db_name = ip.read_string().map_err(tw)?,
            Some(2) => r.table_name = ip.read_string().map_err(tw)?,
            Some(3) if fd.field_type == TType::Struct => r.where_sql = read_where(ip)?,
            Some(4) => r.session_id = ip.read_i64().map_err(tw)?,
            _ => ip.skip(fd.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok(r)
}

/// Reads a full `Delete` CALL, returning `(seqid, request)`.
pub fn read_delete_call(r: &mut impl Read) -> Result<(i32, DeleteReq)> {
    let mut ip = TBinaryInputProtocol::new(r, true);
    let msg = ip.read_message_begin().map_err(tw)?;
    let req = read_delete_args(&mut ip)?;
    Ok((msg.sequence_number, req))
}

/// Writes a `Delete` REPLY.
pub fn write_delete_response(w: &mut impl Write, seq: i32, deleted_rows: i64) -> Result<()> {
    let mut o = TBinaryOutputProtocol::new(w, true);
    o.write_message_begin(&TMessageIdentifier::new("Delete", TMessageType::Reply, seq))
        .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("Delete_result"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("success", TType::Struct, 0i16))
        .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("DeleteResponse"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("error_code", TType::I64, 1i16))
        .map_err(tw)?;
    o.write_i64(0).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("error_msg", TType::String, 2i16))
        .map_err(tw)?;
    o.write_string("OK").map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("deleted_rows", TType::I64, 3i16))
        .map_err(tw)?;
    o.write_i64(deleted_rows).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?; // DeleteResponse
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?; // Delete_result
    o.flush().map_err(tw)?;
    Ok(())
}

// -------- Update --------

/// The subset of `UpdateRequest` needed to run `UPDATE`.
#[derive(Debug, Clone, PartialEq)]
pub struct UpdateReq {
    pub db_name: String,
    pub table_name: String,
    pub where_sql: Option<String>,
    pub assignments: Vec<(String, ValueLit)>,
    pub session_id: i64,
}

impl UpdateReq {
    pub fn to_sql(&self) -> String {
        let sets: Vec<String> = self
            .assignments
            .iter()
            .map(|(c, v)| format!("{c} = {}", v.to_sql()))
            .collect();
        let mut sql = format!("UPDATE {} SET {}", self.table_name, sets.join(", "));
        if let Some(w) = &self.where_sql {
            sql.push_str(&format!(" WHERE {w}"));
        }
        sql.push(';');
        sql
    }
}

fn read_update_expr(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<(String, ValueLit)> {
    ip.read_struct_begin().map_err(tw)?; // UpdateExpr
    let mut col = String::new();
    let mut val = ValueLit::Null;
    loop {
        let fd = ip.read_field_begin().map_err(tw)?;
        if fd.field_type == TType::Stop {
            break;
        }
        match fd.id {
            Some(1) => col = ip.read_string().map_err(tw)?,
            Some(2) => {
                if let PExpr::Value(v) = read_pexpr(ip)? {
                    val = v;
                }
            }
            _ => ip.skip(fd.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok((col, val))
}

/// Reads the args of an `Update` CALL (after the message head).
pub fn read_update_args(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<UpdateReq> {
    ip.read_struct_begin().map_err(tw)?; // Update_args
    let _ = ip.read_field_begin().map_err(tw)?; // request (Struct)
    ip.read_struct_begin().map_err(tw)?; // UpdateRequest
    let mut r = UpdateReq {
        db_name: String::new(),
        table_name: String::new(),
        where_sql: None,
        assignments: Vec::new(),
        session_id: 0,
    };
    loop {
        let fd = ip.read_field_begin().map_err(tw)?;
        if fd.field_type == TType::Stop {
            break;
        }
        match fd.id {
            Some(1) => r.db_name = ip.read_string().map_err(tw)?,
            Some(2) => r.table_name = ip.read_string().map_err(tw)?,
            Some(3) if fd.field_type == TType::Struct => r.where_sql = read_where(ip)?,
            Some(4) => {
                let l = ip.read_list_begin().map_err(tw)?;
                for _ in 0..l.size {
                    r.assignments.push(read_update_expr(ip)?);
                }
                ip.read_list_end().map_err(tw)?;
            }
            Some(5) => r.session_id = ip.read_i64().map_err(tw)?,
            _ => ip.skip(fd.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok(r)
}

/// Reads a full `Update` CALL, returning `(seqid, request)`.
pub fn read_update_call(r: &mut impl Read) -> Result<(i32, UpdateReq)> {
    let mut ip = TBinaryInputProtocol::new(r, true);
    let msg = ip.read_message_begin().map_err(tw)?;
    let req = read_update_args(&mut ip)?;
    Ok((msg.sequence_number, req))
}

// -------- ShowTable --------

/// Reads the args of a `ShowTable` CALL (after the message head).
pub fn read_show_table_args(
    ip: &mut TBinaryInputProtocol<&mut impl Read>,
) -> Result<(String, String, i64)> {
    ip.read_struct_begin().map_err(tw)?; // ShowTable_args
    let _ = ip.read_field_begin().map_err(tw)?; // request (Struct)
    ip.read_struct_begin().map_err(tw)?; // ShowTableRequest
    let (mut db, mut table, mut sid) = (String::new(), String::new(), 0i64);
    loop {
        let fd = ip.read_field_begin().map_err(tw)?;
        if fd.field_type == TType::Stop {
            break;
        }
        match fd.id {
            Some(1) => db = ip.read_string().map_err(tw)?,
            Some(2) => table = ip.read_string().map_err(tw)?,
            Some(3) => sid = ip.read_i64().map_err(tw)?,
            _ => ip.skip(fd.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok((db, table, sid))
}

/// Reads a full `ShowTable` CALL, returning `(seqid, db, table, session_id)`.
pub fn read_show_table_call(r: &mut impl Read) -> Result<(i32, String, String, i64)> {
    let mut ip = TBinaryInputProtocol::new(r, true);
    let msg = ip.read_message_begin().map_err(tw)?;
    let (db, table, sid) = read_show_table_args(&mut ip)?;
    Ok((msg.sequence_number, db, table, sid))
}

/// Writes a `ShowTable` REPLY.
#[allow(clippy::too_many_arguments)]
pub fn write_show_table_response(
    w: &mut impl Write,
    seq: i32,
    db: &str,
    table: &str,
    column_count: i64,
    row_count: i64,
) -> Result<()> {
    let mut o = TBinaryOutputProtocol::new(w, true);
    o.write_message_begin(&TMessageIdentifier::new(
        "ShowTable",
        TMessageType::Reply,
        seq,
    ))
    .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("ShowTable_result"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("success", TType::Struct, 0i16))
        .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("ShowTableResponse"))
        .map_err(tw)?;
    for (name, id, val) in [
        ("error_code", 1i16, 0i64),
        ("row_count", 8i16, row_count),
        ("column_count", 6i16, column_count),
        ("segment_count", 7i16, 1i64),
    ] {
        o.write_field_begin(&TFieldIdentifier::new(name, TType::I64, id))
            .map_err(tw)?;
        o.write_i64(val).map_err(tw)?;
        o.write_field_end().map_err(tw)?;
    }
    for (name, id, val) in [
        ("error_msg", 2i16, "OK"),
        ("database_name", 3i16, db),
        ("table_name", 4i16, table),
        ("store_dir", 5i16, ""),
    ] {
        o.write_field_begin(&TFieldIdentifier::new(name, TType::String, id))
            .map_err(tw)?;
        o.write_string(val).map_err(tw)?;
        o.write_field_end().map_err(tw)?;
    }
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?; // ShowTableResponse
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?; // ShowTable_result
    o.flush().map_err(tw)?;
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn handshake_policy() {
        assert_eq!(handshake(CURRENT_VERSION_INDEX, 7).session_id, 7);
        assert_eq!(handshake(CURRENT_VERSION_INDEX, 7).error_code, OK);
        let bad = handshake(1, 7);
        assert_eq!(bad.error_code, VERSION_MISMATCH);
        assert_eq!(bad.session_id, 0);
    }

    #[test]
    fn column_fields_roundtrip() {
        let cols = vec!["id".to_string(), "name".to_string()];
        let rows = vec![
            vec!["1".to_string(), "a".to_string()],
            vec!["2".to_string(), "NULL".to_string()],
        ];
        let fields = rows_to_column_fields(&cols, &rows);
        assert_eq!(fields.len(), 2);
        assert_eq!(fields[1].bitmasks, vec![true, false]);
        let mut buf = Vec::new();
        write_column_fields(&mut buf, &fields).unwrap();
        let got = read_column_fields(&mut &buf[..]).unwrap();
        assert_eq!(got, fields);
    }

    #[test]
    fn call_reply_byte_roundtrip() {
        let mut buf = Vec::new();
        write_connect_call(&mut buf, 1, CURRENT_VERSION_INDEX).unwrap();
        let (seq, cv) = read_connect_call(&mut &buf[..]).unwrap();
        assert_eq!(seq, 1);
        assert_eq!(cv, CURRENT_VERSION_INDEX);

        let resp = CommonResponse {
            error_code: OK,
            error_msg: "OK".into(),
            session_id: 42,
        };
        let mut rbuf = Vec::new();
        write_connect_reply(&mut rbuf, 1, &resp).unwrap();
        let got = read_connect_reply(&mut &rbuf[..]).unwrap();
        assert_eq!(got, resp);
    }

    #[test]
    fn tcp_connect_success_and_mismatch() {
        let server = Server::bind("127.0.0.1:0").unwrap();
        let addr = server.local_addr().unwrap();
        std::thread::spawn(move || server.run());

        let ok = connect(&addr, CURRENT_VERSION_INDEX).unwrap();
        assert_eq!(ok.error_code, OK);
        assert!(ok.session_id >= 1);

        let bad = connect(&addr, 1).unwrap();
        assert_eq!(bad.error_code, VERSION_MISMATCH);

        let dc = disconnect(&addr, ok.session_id).unwrap();
        assert_eq!(dc.error_code, OK);
        assert_eq!(dc.session_id, ok.session_id);
    }

    #[test]
    fn tcp_create_table_round_trip_with_engine() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Arc::new(Engine::open(dir.path()).unwrap());
        let server = Server::bind_with_engine("127.0.0.1:0", engine).unwrap();
        let addr = server.local_addr().unwrap();
        std::thread::spawn(move || server.run());

        let req = CreateTableReq {
            db_name: "default_db".into(),
            table_name: "t".into(),
            columns: vec![ColumnDefWire {
                id: 0,
                name: "id".into(),
                logic_type: 5,
            }],
            session_id: 1,
        };
        assert_eq!(create_table(&addr, &req).unwrap().error_code, OK);
        // creating the same table again must fail: the engine kept the state
        assert_ne!(create_table(&addr, &req).unwrap().error_code, OK);
    }
}
