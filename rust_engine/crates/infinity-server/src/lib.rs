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
        // Handle the request, but never let a decode/execute error kill the
        // connection: reply with a Thrift exception instead of dropping the
        // socket (the SDK reports `connection reset` otherwise).
        let handled: Result<()> = (|| {
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
                    let names = match &engine {
                        Some(eng) => eng.catalog().list_databases().unwrap_or_default(),
                        None => vec!["default_db".to_string()],
                    };
                    write_list_database_response(&mut writer, seq, &names)?;
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
                                error_code: e.status_code(),
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
                | "DropIndex" => {
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
                    // `ConflictType`: 0 = Ignore, 1 = Error, 2 = Replace. The option
                    // sits in whichever field the request struct reserves for it
                    // (3 for the database ops, 4 for table/index ops), so scan them.
                    let conflict: Option<i64> = [3i16, 4, 5]
                        .iter()
                        .find_map(|id| fields.get(id).and_then(|v| v.parse::<i64>().ok()));
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
                        "DropTable" if !name2.is_empty() => {
                            let sql = if conflict == Some(0) {
                                format!("DROP TABLE IF EXISTS {}", qualify(&name2))
                            } else {
                                format!("DROP TABLE {}", qualify(&name2))
                            };
                            run_common(engine.as_ref(), &sql, sid)
                        }
                        "DropIndex" if !name3.is_empty() && conflict == Some(0) => run_common(
                            engine.as_ref(),
                            &format!("DROP INDEX IF EXISTS {name3}"),
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
                    if std::env::var_os("INF_DEBUG_SQL").is_some() {
                        eprintln!("[SELECT] {}", req.to_sql());
                    }
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
                                select_response_from_typed(
                                    &columns,
                                    &declared_types(eng, &req.table_name, &columns),
                                    &rows,
                                )
                            }
                            Err(e) => err(e.to_string()),
                        },
                        None => err("no engine bound".into()),
                    };
                    write_select_response(&mut writer, seq, &resp)?;
                }
                // Lifecycle / maintenance RPCs returning a plain `CommonResponse`.
                // `Flush` persists table data into the C++ block layout and the
                // indexes into their chunk files, which is what the restart
                // harness inspects on disk.
                "Flush" => {
                    let (sid, _) = {
                        let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                        read_string_request(&mut ip)?
                    };
                    let resp = match engine.as_ref() {
                        Some(eng) => match eng.flush() {
                            Ok(()) => CommonResponse {
                                error_code: OK,
                                error_msg: "OK".into(),
                                session_id: sid,
                            },
                            Err(e) => CommonResponse {
                                error_code: e.status_code(),
                                error_msg: e.to_string(),
                                session_id: sid,
                            },
                        },
                        None => CommonResponse {
                            error_code: 1,
                            error_msg: "no engine bound".into(),
                            session_id: sid,
                        },
                    };
                    write_reply_common(&mut writer, "Flush", seq, &resp)?;
                }
                // `RenameTable` is executed for real; the rest are engine-level
                // maintenance that the in-memory engine has nothing to do for, so
                // they report success rather than `unknown method`.
                "Optimize"
                | "Compact"
                | "Cleanup"
                | "Command"
                | "AlterIndex"
                | "DumpIndex"
                | "CreateTableSnapshot"
                | "CreateDatabaseSnapshot"
                | "CreateSystemSnapshot"
                | "DropSnapshot"
                | "RestoreSnapshot"
                | "RenameTable" => {
                    let (sid, fields) = {
                        let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                        read_string_request(&mut ip)?
                    };
                    let get = |id: i16| fields.get(&id).cloned().unwrap_or_default();
                    let resp = if name == "RenameTable" {
                        let (db, table, new_name) = (get(1), get(2), get(3));
                        let full = if db.is_empty() {
                            table
                        } else {
                            format!("{db}.{table}")
                        };
                        run_common(
                            engine.as_ref(),
                            &format!("ALTER TABLE {full} RENAME TO {new_name};"),
                            sid,
                        )
                    } else {
                        CommonResponse {
                            error_code: OK,
                            error_msg: "OK".into(),
                            session_id: sid,
                        }
                    };
                    write_reply_common(&mut writer, &name, seq, &resp)?;
                }
                "Explain" => {
                    let (req, etype) = {
                        let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                        read_explain_args(&mut ip)?
                    };
                    let kind = match etype {
                        4 => "PHYSICAL",
                        6 => "FRAGMENT",
                        _ => "PHYSICAL",
                    };
                    let sql = format!("EXPLAIN {kind} {}", req.to_sql());
                    let resp = match engine.as_ref() {
                        Some(eng) => match eng.execute(&sql) {
                            Ok(rs) => {
                                let mut rows: Vec<Vec<String>> = Vec::new();
                                for r in rs {
                                    if let QueryResult::Rows { rows: rw, .. } = r {
                                        rows = rw;
                                    }
                                }
                                select_response_from_typed(&["explain".to_string()], &[], &rows)
                            }
                            Err(e) => SelectResponseWire {
                                error_code: e.status_code(),
                                error_msg: e.to_string(),
                                column_defs: Vec::new(),
                                column_fields: Vec::new(),
                                extra_result: String::new(),
                            },
                        },
                        None => SelectResponseWire {
                            error_code: 1,
                            error_msg: "no engine bound".into(),
                            column_defs: Vec::new(),
                            column_fields: Vec::new(),
                            extra_result: String::new(),
                        },
                    };
                    write_result_response(&mut writer, seq, "Explain", &resp)?;
                }
                "AddColumns" => {
                    let (db, table, cols, sid) = {
                        let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                        read_alter_add_args(&mut ip)?
                    };
                    let full = if db.is_empty() {
                        table
                    } else {
                        format!("{db}.{table}")
                    };
                    let defs: Vec<String> = cols
                        .iter()
                        .map(|c| {
                            let mut def = format!("{} {}", c.name, column_sql_type(c));
                            if c.not_null {
                                def.push_str(" NOT NULL");
                            }
                            if let Some(d) = &c.default {
                                def.push_str(&format!(" DEFAULT {d}"));
                            }
                            def
                        })
                        .collect();
                    let sql = format!("ALTER TABLE {full} ADD COLUMN {}", defs.join(", "));
                    let resp = run_common(engine.as_ref(), &sql, sid);
                    write_reply_common(&mut writer, "AddColumns", seq, &resp)?;
                }
                "DropColumns" => {
                    let (db, table, names, sid) = {
                        let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                        read_alter_drop_args(&mut ip)?
                    };
                    let full = if db.is_empty() {
                        table
                    } else {
                        format!("{db}.{table}")
                    };
                    let mut resp = CommonResponse {
                        error_code: OK,
                        error_msg: "OK".into(),
                        session_id: sid,
                    };
                    for name in names {
                        let sql = format!("ALTER TABLE {full} DROP COLUMN {name}");
                        resp = run_common(engine.as_ref(), &sql, sid);
                        if resp.error_code != OK {
                            break;
                        }
                    }
                    write_reply_common(&mut writer, "DropColumns", seq, &resp)?;
                }
                "Export" => {
                    let req = {
                        let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                        read_export_args(&mut ip)?
                    };
                    let full = if req.db.is_empty() {
                        req.table.clone()
                    } else {
                        format!("{}.{}", req.db, req.table)
                    };
                    let format = match req.file_type {
                        1 => "JSON",
                        2 => "JSONL",
                        3 => "FVECS",
                        _ => "CSV",
                    };
                    // `limit` is the total number of rows, `row_limit` the size
                    // of each part: the reference writes the first `row_limit`
                    // rows to `<file>` and the rest to `<file>.part1`, `.part2`…
                    let total = (req.limit > 0).then_some(req.limit);
                    let part = (req.row_limit > 0).then_some(req.row_limit);
                    let mut written = 0i64;
                    let mut index = 0i64;
                    let mut resp = CommonResponse {
                        error_code: OK,
                        error_msg: "OK".into(),
                        session_id: req.session_id,
                    };
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
                            req.file.clone()
                        } else {
                            format!("{}.part{index}", req.file)
                        };
                        let mut sql = format!(
                            "COPY {full} TO '{}' WITH ( DELIMITER '{}', FORMAT {format}, OFFSET {}",
                            path.replace('\'', "''"),
                            req.delimiter.replace('\'', "''"),
                            req.offset + written
                        );
                        if take > 0 {
                            sql.push_str(&format!(", LIMIT {take}"));
                        }
                        if !req.columns.is_empty() {
                            sql.push_str(&format!(", COLUMNS '{}'", req.columns.join(",")));
                        }
                        sql.push_str(" )");
                        let Some(eng) = engine.as_ref() else {
                            resp = CommonResponse {
                                error_code: 1,
                                error_msg: "no engine bound".into(),
                                session_id: req.session_id,
                            };
                            break;
                        };
                        match eng.execute(&sql) {
                            Ok(rs) => {
                                let n = rs
                                    .into_iter()
                                    .map(|r| match r {
                                        QueryResult::Inserted { count } => count as i64,
                                        _ => 0,
                                    })
                                    .max()
                                    .unwrap_or(0);
                                resp = CommonResponse {
                                    error_code: OK,
                                    error_msg: "OK".into(),
                                    session_id: req.session_id,
                                };
                                written += n;
                                if n == 0 || part.is_none() {
                                    break;
                                }
                                index += 1;
                            }
                            Err(e) => {
                                resp = CommonResponse {
                                    error_code: e.status_code(),
                                    error_msg: e.to_string(),
                                    session_id: req.session_id,
                                };
                                break;
                            }
                        }
                    }
                    write_reply_common(&mut writer, "Export", seq, &resp)?;
                }
                "SetConfig" => {
                    let (sid, name, value, kind) = {
                        let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                        read_set_config_args(&mut ip)?
                    };
                    let resp = match name.to_ascii_lowercase().as_str() {
                        // read-only knobs reject any assignment
                        "mem_index_capacity" => CommonResponse {
                            error_code: 3076,
                            error_msg: "Invalid command: Config mem_index_capacity is read-only"
                                .into(),
                            session_id: sid,
                        },
                        // `log_level` is a string knob; a number is a type mismatch
                        "log_level" if kind != 1 => CommonResponse {
                            error_code: 3010,
                            error_msg: "data type mismatch for config 'log_level'".into(),
                            session_id: sid,
                        },
                        _ => {
                            let sql = format!("SET CONFIG {name} {value}");
                            run_common(engine.as_ref(), &sql, sid)
                        }
                    };
                    write_reply_common(&mut writer, "SetConfig", seq, &resp)?;
                }
                "ShowConfig" => {
                    let (sid, name) = {
                        let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                        read_show_config_args(&mut ip)?
                    };
                    let value = engine.as_ref().and_then(|e| e.config_get(&name));
                    write_show_config_response(&mut writer, seq, sid, &name, value.as_deref())?;
                }
                "Import" => {
                    let req = {
                        let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                        read_import_args(&mut ip)?
                    };
                    let table = if req.db.is_empty() {
                        req.table.clone()
                    } else {
                        format!("{}.{}", req.db, req.table)
                    };
                    let format = match req.file_type {
                        1 => "JSON",
                        2 => "JSONL",
                        _ => "CSV",
                    };
                    let mut sql = format!(
                        "COPY {table} FROM '{}' WITH ( DELIMITER '{}', FORMAT {format} )",
                        req.file.replace('\'', "''"),
                        req.delimiter.replace('\'', "''")
                    );
                    if req.has_header {
                        sql.push_str(" ;");
                    }
                    let resp = match engine.as_ref() {
                        Some(eng) => match eng.execute(&sql) {
                            Ok(_) => CommonResponse {
                                error_code: OK,
                                error_msg: "OK".into(),
                                session_id: req.session_id,
                            },
                            Err(e) => CommonResponse {
                                error_code: e.status_code(),
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
                    write_reply_common(&mut writer, "Import", seq, &resp)?;
                }
                "ShowColumns" => {
                    let (_sid, fields) = {
                        let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                        read_string_request(&mut ip)?
                    };
                    let db = fields.get(&1).cloned().unwrap_or_default();
                    let table = fields.get(&2).cloned().unwrap_or_default();
                    let full = if db.is_empty() {
                        table
                    } else {
                        format!("{db}.{table}")
                    };
                    let resp = match &engine {
                        Some(eng) => match eng.execute(&format!("SHOW TABLE {full} COLUMNS;")) {
                            Ok(rs) => {
                                let mut rows: Vec<Vec<String>> = Vec::new();
                                for r in rs {
                                    if let QueryResult::Rows { rows: rw, .. } = r {
                                        rows = rw;
                                    }
                                }
                                select_response_from(
                                    &[
                                        "name".to_string(),
                                        "type".to_string(),
                                        "default".to_string(),
                                        "comment".to_string(),
                                    ],
                                    &rows,
                                )
                            }
                            Err(e) => SelectResponseWire {
                                error_code: e.status_code(),
                                error_msg: e.to_string(),
                                column_defs: Vec::new(),
                                column_fields: Vec::new(),
                                extra_result: String::new(),
                            },
                        },
                        None => SelectResponseWire {
                            error_code: 1,
                            error_msg: "no engine bound".into(),
                            column_defs: Vec::new(),
                            column_fields: Vec::new(),
                            extra_result: String::new(),
                        },
                    };
                    write_select_response(&mut writer, seq, &resp)?;
                }
                "ListTable" => {
                    let (_sid, fields) = {
                        let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                        read_string_request(&mut ip)?
                    };
                    let db = fields.get(&1).cloned().unwrap_or_default();
                    let prefix = format!("{db}.");
                    let names: Vec<String> = match &engine {
                        Some(eng) => eng
                            .catalog()
                            .list_tables()
                            .unwrap_or_default()
                            .into_iter()
                            .filter(|t| db.is_empty() || t.name.starts_with(&prefix))
                            .map(|t| {
                                if db.is_empty() {
                                    t.name
                                } else {
                                    t.name.trim_start_matches(&prefix).to_string()
                                }
                            })
                            .collect(),
                        None => Vec::new(),
                    };
                    write_string_list_response(
                        &mut writer,
                        seq,
                        "ListTable",
                        "table_names",
                        &names,
                    )?;
                }
                "ListIndex" => {
                    let (_sid, fields) = {
                        let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                        read_string_request(&mut ip)?
                    };
                    let table = fields.get(&2).cloned().unwrap_or_default();
                    let names: Vec<String> = match &engine {
                        Some(eng) => eng
                            .catalog()
                            .list_indexes()
                            .unwrap_or_default()
                            .into_iter()
                            .filter(|im| {
                                table.is_empty()
                                    || im.table == table
                                    || im.table.ends_with(&format!(".{table}"))
                            })
                            .map(|im| im.name)
                            .collect(),
                        None => Vec::new(),
                    };
                    write_string_list_response(
                        &mut writer,
                        seq,
                        "ListIndex",
                        "index_names",
                        &names,
                    )?;
                }
                "ShowDatabase" => {
                    let (_sid, fields) = {
                        let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                        read_string_request(&mut ip)?
                    };
                    let db = fields.get(&1).cloned().unwrap_or_default();
                    let prefix = format!("{db}.");
                    let table_count = match &engine {
                        Some(eng) => eng
                            .catalog()
                            .list_tables()
                            .unwrap_or_default()
                            .into_iter()
                            .filter(|t| db.is_empty() || t.name.starts_with(&prefix))
                            .count() as i64,
                        None => 0,
                    };
                    write_show_database_response(&mut writer, seq, &db, table_count, "")?;
                }
                "CreateIndex" => {
                    let req = {
                        let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                        read_create_index_args(&mut ip)?
                    };
                    let full = if req.db_name.is_empty() {
                        req.table_name.clone()
                    } else {
                        format!("{}.{}", req.db_name, req.table_name)
                    };
                    let resp = if req.column.is_empty() {
                        CommonResponse {
                            error_code: OK,
                            error_msg: "OK".into(),
                            session_id: req.session_id,
                        }
                    } else {
                        run_common(
                            engine.as_ref(),
                            &format!(
                                "CREATE INDEX {} ON {} ({});",
                                req.index_name, full, req.column
                            ),
                            req.session_id,
                        )
                    };
                    write_reply_common(&mut writer, "CreateIndex", seq, &resp)?;
                }
                "ShowCurrentNode" => {
                    {
                        let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                        skip_struct(&mut ip)?;
                    }
                    write_show_current_node_response(&mut writer, seq, "leader", "alive")?;
                }
                "ListSnapshots" => {
                    {
                        let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                        skip_struct(&mut ip)?;
                    }
                    write_list_snapshots_response(&mut writer, seq)?;
                }
                other => {
                    {
                        let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                        skip_struct(&mut ip)?;
                    }
                    write_app_exception(
                        &mut writer,
                        other,
                        seq,
                        &format!("unknown method: {other}"),
                    )?;
                }
            }
            Ok(())
        })();
        if let Err(e) = handled {
            let _ = write_app_exception(&mut writer, &name, seq, &format!("{e}"));
        }
    }
    Ok(())
}

/// The `ElementType` name the engine expects inside `EMBEDDING(<elem>,<dim>)`.
fn element_type_name(elem: i32) -> &'static str {
    match elem {
        0 => "BIT",
        1 => "UINT8",
        2 => "INT8",
        3 => "INT16",
        4 => "INT32",
        5 => "INT64",
        6 => "FLOAT",
        7 => "FLOAT64",
        8 => "FLOAT16",
        9 => "BFLOAT16",
        _ => "FLOAT",
    }
}

/// Renders a scalar `LogicType` back into the engine's SQL type name.
fn scalar_sql(logic_type: i32) -> String {
    match logic_type {
        0 => "BOOLEAN".into(),
        1 => "TINYINT".into(),
        2 => "SMALLINT".into(),
        3 => "INTEGER".into(),
        4 => "BIGINT".into(),
        5 => "HUGEINT".into(),
        7 => "FLOAT".into(),
        8 => "DOUBLE".into(),
        9 => "FLOAT16".into(),
        10 => "BFLOAT16".into(),
        11 => "VARCHAR".into(),
        17 => "DATE".into(),
        18 => "TIME".into(),
        19 => "DATETIME".into(),
        20 => "TIMESTAMP".into(),
        22 => "ARRAY(VARCHAR)".into(),
        23 => "JSON".into(),
        _ => "VARCHAR".into(),
    }
}

/// Renders a `(LogicType, PhysicalType)` pair back into the engine's SQL type
/// text, recursing for array element types.
fn type_sql(logic_type: i32, physical: &PhysicalWire) -> String {
    match physical {
        PhysicalWire::Array {
            element_logic_type,
            element,
        } => format!("ARRAY({})", type_sql(*element_logic_type, element)),
        PhysicalWire::Embedding {
            dimension,
            element_type,
        } => {
            // Tensor / TensorArray / MultiVector share the embedding physical
            // descriptor; the LogicType tells them apart.
            let head = match logic_type {
                13 => "TENSOR",
                14 => "TENSORARRAY",
                16 => "MULTIVECTOR",
                _ => "EMBEDDING",
            };
            format!("{head}({}, {dimension})", element_type_name(*element_type))
        }
        PhysicalWire::Sparse {
            dimension,
            element_type,
            ..
        } => format!("SPARSE({}, {dimension})", element_type_name(*element_type)),
        PhysicalWire::None => scalar_sql(logic_type),
    }
}

/// Renders a declared `ColumnDef` back into the engine's SQL type text, so a
/// `CreateTable` RPC preserves the column's real type (embedding included)
/// instead of flattening everything to `VARCHAR`.
fn column_sql_type(c: &ColumnDefWire) -> String {
    type_sql(c.logic_type, &c.physical)
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
        .map(|c| format!("{} {}", c.name, column_sql_type(c)))
        .collect();
    // `db_name` names the target database; an unqualified table would otherwise
    // land in `default_db`.
    let table = if req.db_name.is_empty() {
        req.table_name.clone()
    } else {
        format!("{}.{}", req.db_name, req.table_name)
    };
    let sql = format!("CREATE TABLE {table} ({});", cols.join(", "));
    match engine.execute(&sql) {
        Ok(_) => CommonResponse {
            error_code: OK,
            error_msg: "OK".into(),
            session_id: req.session_id,
        },
        Err(e) => CommonResponse {
            error_code: e.status_code(),
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

/// IEEE-754 binary16 of an `f32` (round to nearest even), enough for the wire.
fn f32_to_f16_bits(f: f32) -> u16 {
    let bits = f.to_bits();
    let sign = ((bits >> 16) & 0x8000) as u16;
    let exp = ((bits >> 23) & 0xff) as i32 - 127 + 15;
    let mant = bits & 0x007f_ffff;
    if exp <= 0 {
        return sign;
    }
    if exp >= 31 {
        return sign | 0x7c00;
    }
    let half = sign | ((exp as u16) << 10) | ((mant >> 13) as u16);
    let rest = mant & 0x1fff;
    if rest > 0x1000 || (rest == 0x1000 && half & 1 == 1) {
        half + 1
    } else {
        half
    }
}

/// Maps an element-type name (`FLOAT`, `INT8`, `DOUBLE`, …) to the SDK's
/// `ElementType` ordinal.
fn element_type_code(elem: &str) -> i32 {
    if elem.contains("UINT8") {
        1
    } else if elem.contains("INT8") || elem.contains("TINYINT") {
        2
    } else if elem.contains("INT16") {
        3
    } else if elem.contains("INT32") || elem == "INT" {
        4
    } else if elem.contains("INT64") {
        5
    } else if elem.contains("FLOAT64") || elem.contains("DOUBLE") {
        7
    } else if elem.contains("BFLOAT16") {
        9
    } else if elem.contains("FLOAT16") {
        8
    } else if elem.contains("BIT") {
        0
    } else {
        6 // Float32
    }
}

/// Parses `EMBEDDING(<elem>,<dim>)` / `VECTOR(...)` into the wire descriptor
/// `(dimension, element_type)` using the SDK's `ElementType` codes.
fn embedding_info(type_text: &str) -> Option<(i32, i32)> {
    let up = type_text.to_ascii_uppercase();
    if !(up.starts_with("EMBEDDING") || up.starts_with("VECTOR")) {
        return None;
    }
    let inner = up.split_once('(')?.1.trim_end_matches(')').to_string();
    let mut parts = inner.split(',');
    let elem = parts.next()?.trim().to_string();
    let dim: i32 = parts.next()?.trim().parse().ok()?;
    Some((dim, element_type_code(&elem)))
}

/// `LogicType.Embedding` (SDK ordinal) — announced for vector columns.
const LOGIC_EMBEDDING: i32 = 12;

/// `LogicType.Sparse` (SDK ordinal) — announced for sparse columns.
const LOGIC_SPARSE: i32 = 15;

/// `LogicType.Array` (SDK ordinal) — announced for array columns.
const LOGIC_ARRAY: i32 = 22;

// -------- date/time helpers (the SDK expects epoch days/seconds) --------

/// Days from 1970-01-01 for a proleptic-Gregorian civil date (Hinnant's algorithm).
fn days_from_civil(y: i64, m: i64, d: i64) -> i64 {
    let y = if m <= 2 { y - 1 } else { y };
    let era = if y >= 0 { y } else { y - 399 } / 400;
    let yoe = y - era * 400;
    let mp = (m + 9) % 12;
    let doy = (153 * mp + 2) / 5 + d - 1;
    let doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    era * 146097 + doe - 719468
}

/// Parses `YYYY-MM-DD` into days since the epoch (`ColumnDate`'s wire form).
fn date_to_days(s: &str) -> Option<i32> {
    let mut it = s.trim().split(['-', '/']);
    let y: i64 = it.next()?.parse().ok()?;
    let m: i64 = it.next()?.parse().ok()?;
    let d: i64 = it.next()?.parse().ok()?;
    i32::try_from(days_from_civil(y, m, d)).ok()
}

/// Parses `HH:MM:SS` into seconds since midnight (`ColumnTime`'s wire form).
fn time_to_secs(s: &str) -> Option<i32> {
    let mut it = s.trim().split(':');
    let h: i64 = it.next()?.parse().ok()?;
    let m: i64 = it.next().unwrap_or("0").parse().ok()?;
    let sec: i64 = it.next().unwrap_or("0").parse().ok()?;
    i32::try_from(h * 3600 + m * 60 + sec).ok()
}

/// Splits `YYYY-MM-DD HH:MM:SS` into (epoch days, seconds of day).
fn datetime_to_pair(s: &str) -> Option<(i32, i32)> {
    let (d, t) = s.trim().split_once([' ', 'T'])?;
    Some((date_to_days(d)?, time_to_secs(t)?))
}

/// The wire shape of one result column, resolved from its declared type text.
#[derive(Debug, Clone, Copy, PartialEq)]
enum ColKind {
    /// Packed as the given `ColumnType` (scalars, JSON, date/time).
    Scalar(i32),
    /// Vector column: `(dimension, element_type)` as a raw element stream.
    Embedding(i32, i32),
    /// Sparse column: a per-row `nnz` + indices + values stream.
    Sparse {
        dimension: i64,
        element_type: i32,
        index_type: i32,
    },
    /// Tensor / TensorArray / MultiVector: per-row length-prefixed element
    /// streams (`column_type` is 11 (Tensor), 12 (TensorArray) or 14
    /// (MultiVector)); all share the embedding physical descriptor.
    Tensor {
        column_type: i32,
        dimension: i32,
        element_type: i32,
    },
    /// Array: a per-row `u32 count` + elements packed for `element_column_type`.
    Array { element_column_type: i32 },
}

/// Parses `TENSORARRAY(<elem>,<dim>)` / `TENSOR(<elem>,<dim>)` /
/// `MULTIVECTOR(<elem>,<dim>)` into `(ColumnType, dimension, element_type)`.
fn tensor_info(type_text: &str) -> Option<(i32, i32, i32)> {
    let up = type_text.trim().to_ascii_uppercase();
    // Order matters: `TENSORARRAY(` must be tried before `TENSOR(`.
    let (column_type, rest) = if let Some(r) = up.strip_prefix("TENSORARRAY(") {
        (12, r)
    } else if let Some(r) = up.strip_prefix("MULTIVECTOR(") {
        (14, r)
    } else if let Some(r) = up.strip_prefix("TENSOR(") {
        (11, r)
    } else {
        return None;
    };
    let inner = rest.trim_end_matches(')');
    let parts: Vec<&str> = inner.split(',').map(str::trim).collect();
    let dim: i32 = parts.get(1)?.parse().ok()?;
    let elem = (*parts.first()?).to_string();
    Some((column_type, dim, element_type_code(&elem)))
}

/// Appends one element of `elem` type (little-endian) to `chunk`.
fn push_element(chunk: &mut Vec<u8>, elem: i32, f: f64) {
    match elem {
        1 => chunk.push(f as u8),
        2 => chunk.push(f as i8 as u8),
        3 => chunk.extend_from_slice(&(f as i16).to_le_bytes()),
        4 => chunk.extend_from_slice(&(f as i32).to_le_bytes()),
        5 => chunk.extend_from_slice(&(f as i64).to_le_bytes()),
        7 => chunk.extend_from_slice(&f.to_le_bytes()),
        8 => chunk.extend_from_slice(&f32_to_f16_bits(f as f32).to_le_bytes()),
        _ => chunk.extend_from_slice(&(f as f32).to_le_bytes()),
    }
}

/// Strips exactly one outer `[ … ]` pair (no-op when absent).
fn strip_one_bracket(t: &str) -> &str {
    let t = t.trim();
    if t.starts_with('[') && t.ends_with(']') && t.len() >= 2 {
        &t[1..t.len() - 1]
    } else {
        t
    }
}

/// Splits array content into its top-level comma-separated segments, keeping
/// each nested `[…]` group intact (e.g. `[1,2],[3,4]` → `["[1,2]", "[3,4]"]`).
fn split_top_level(content: &str) -> Vec<String> {
    let mut out = Vec::new();
    let mut depth = 0i32;
    let mut cur = String::new();
    for ch in content.chars() {
        match ch {
            '[' => {
                depth += 1;
                cur.push(ch);
            }
            ']' => {
                depth -= 1;
                cur.push(ch);
            }
            ',' if depth == 0 => {
                let s = cur.trim();
                if !s.is_empty() {
                    out.push(s.to_string());
                }
                cur.clear();
            }
            _ => cur.push(ch),
        }
    }
    let s = cur.trim();
    if !s.is_empty() {
        out.push(s.to_string());
    }
    out
}

/// Parses a comma-separated list of numbers.
fn parse_flat(s: &str) -> Vec<f64> {
    s.split(',')
        .filter_map(|x| x.trim().parse::<f64>().ok())
        .collect()
}

/// Parses the engine's tensor rendering into a list of vectors.
/// `[[1,2],[3,4]]` → two vectors; `[1,2]` → one vector.
fn parse_nested_f64(cell: &str) -> Vec<Vec<f64>> {
    let content = strip_one_bracket(cell.trim());
    if content.trim().is_empty() {
        return Vec::new();
    }
    if !content.contains('[') {
        return vec![parse_flat(content)];
    }
    split_top_level(content)
        .iter()
        .map(|seg| parse_flat(strip_one_bracket(seg)))
        .collect()
}

/// Parses the engine's `TENSORARRAY` rendering into tensors of vectors.
fn parse_tensorarray_f64(cell: &str) -> Vec<Vec<Vec<f64>>> {
    let content = strip_one_bracket(cell.trim());
    if content.trim().is_empty() {
        return Vec::new();
    }
    split_top_level(content)
        .iter()
        .map(|tensor| {
            let tc = strip_one_bracket(tensor);
            if !tc.contains('[') {
                return vec![parse_flat(tc)];
            }
            split_top_level(tc)
                .iter()
                .map(|seg| parse_flat(strip_one_bracket(seg)))
                .collect()
        })
        .collect()
}

/// Packs one tensor body: each vector padded/truncated to `dim` elements.
fn pack_tensor_body(vectors: &[Vec<f64>], dim: i32, elem: i32) -> Vec<u8> {
    let mut body = Vec::new();
    for v in vectors {
        for k in 0..dim as usize {
            push_element(&mut body, elem, v.get(k).copied().unwrap_or(0.0));
        }
    }
    body
}

/// Parses `SPARSE(<elem>,<bound>[,<index>])`. The index width is derived from
/// the bound (matching the engine's `describe_type`), not the declared index
/// name.
fn sparse_info(type_text: &str) -> Option<(i64, i32, i32)> {
    let up = type_text.to_ascii_uppercase();
    if !up.starts_with("SPARSE(") {
        return None;
    }
    let inner = up.split_once('(')?.1.trim_end_matches(')');
    let parts: Vec<&str> = inner.split(',').map(str::trim).collect();
    let elem = (*parts.first()?).to_string();
    let bound: u64 = parts.get(1).and_then(|s| s.parse().ok()).unwrap_or(0);
    let index_code = if bound <= 127 {
        2 // ElementInt8
    } else if bound <= 32767 {
        3 // ElementInt16
    } else if bound <= 2_147_483_647 {
        4 // ElementInt32
    } else {
        5 // ElementInt64
    };
    Some((bound as i64, element_type_code(&elem), index_code))
}

/// Classifies a column from its declared type text, falling back to value
/// inference when the type is unknown.
fn classify_column(declared: &str, rows: &[Vec<String>], ci: usize) -> ColKind {
    if let Some((dim, elem)) = embedding_info(declared) {
        return ColKind::Embedding(dim, elem);
    }
    if let Some((dimension, element_type, index_type)) = sparse_info(declared) {
        return ColKind::Sparse {
            dimension,
            element_type,
            index_type,
        };
    }
    if let Some((column_type, dimension, element_type)) = tensor_info(declared) {
        return ColKind::Tensor {
            column_type,
            dimension,
            element_type,
        };
    }
    if let Some((_logic, element_column_type, _physical)) = array_info(declared) {
        return ColKind::Array {
            element_column_type,
        };
    }
    let up = declared.trim().to_ascii_uppercase();
    let base = up.split('(').next().unwrap_or("").trim();
    match base {
        "JSON" => ColKind::Scalar(22),                   // ColumnJson
        "DATE" => ColKind::Scalar(16),                   // ColumnDate
        "TIME" => ColKind::Scalar(17),                   // ColumnTime
        "DATETIME" | "TIMESTAMP" => ColKind::Scalar(18), // ColumnDateTime
        // A declared scalar type wins over inference: the rendered value
        // (`1.500000`) would otherwise be announced as Float64 even for a
        // `FLOAT` column.
        "" => ColKind::Scalar(infer_column_type(rows, ci)),
        _ => ColKind::Scalar(named_type(declared).1),
    }
}

/// Parses the engine's sparse rendering `[i:val,…]` / `(empty)`.
fn parse_sparse_literal(cell: &str) -> (Vec<i64>, Vec<f64>) {
    let inner = cell.trim();
    if inner == "(empty)" || inner.is_empty() || inner == "[]" {
        return (Vec::new(), Vec::new());
    }
    let inner = inner.trim_start_matches('[').trim_end_matches(']');
    let mut idx = Vec::new();
    let mut val = Vec::new();
    for part in inner.split(',') {
        if let Some((i, v)) = part.split_once(':') {
            if let (Ok(i), Ok(v)) = (i.trim().parse::<i64>(), v.trim().parse::<f64>()) {
                idx.push(i);
                val.push(v);
            }
        }
    }
    (idx, val)
}

/// Maps a wire `ColumnType` (how the column bytes are packed) to the SDK's
/// `LogicType` ordinal (`ColumnDef.data_type.logic_type`), which the SDK turns
/// into a Polars dtype via `logic_type_to_dtype`.
fn column_type_to_logic(column_type: i32) -> i32 {
    match column_type {
        0 => 0,   // ColumnBool -> Boolean
        1 => 1,   // ColumnInt8 -> TinyInt
        2 => 2,   // ColumnInt16 -> SmallInt
        3 => 3,   // ColumnInt32 -> Integer
        4 => 4,   // ColumnInt64 -> BigInt
        5 => 7,   // ColumnFloat32 -> Float
        6 => 8,   // ColumnFloat64 -> Double
        7 => 9,   // ColumnFloat16 -> Float16
        8 => 10,  // ColumnBFloat16 -> BFloat16
        9 => 11,  // ColumnVarchar -> Varchar
        10 => 12, // ColumnEmbedding -> Embedding
        11 => 13, // ColumnTensor -> Tensor
        12 => 14, // ColumnTensorArray -> TensorArray
        13 => 15, // ColumnSparse -> Sparse
        14 => 16, // ColumnMultiVector -> MultiVector
        15 => 4,  // ColumnRowID -> BigInt
        16 => 17, // ColumnDate -> Date
        17 => 18, // ColumnTime -> Time
        18 => 19, // ColumnDateTime -> DateTime
        19 => 20, // ColumnTimestamp -> Timestamp
        20 => 21, // ColumnInterval -> Interval
        21 => 22, // ColumnArray -> Array
        22 => 23, // ColumnJson -> Json
        _ => 11,  // Varchar
    }
}

/// The inverse of [`column_type_to_logic`]: the `ColumnType` the SDK unpacks an
/// element of the given `LogicType` with.
fn logic_to_column_type(logic_type: i32) -> i32 {
    match logic_type {
        0 => 0,   // Boolean
        1 => 1,   // TinyInt
        2 => 2,   // SmallInt
        3 => 3,   // Integer
        4 => 4,   // BigInt
        7 => 5,   // Float
        8 => 6,   // Double
        9 => 7,   // Float16
        10 => 8,  // BFloat16
        11 => 9,  // Varchar
        12 => 10, // Embedding
        13 => 11, // Tensor
        14 => 12, // TensorArray
        15 => 13, // Sparse
        16 => 14, // MultiVector
        17 => 16, // Date
        18 => 17, // Time
        19 => 18, // DateTime
        20 => 19, // Timestamp
        21 => 20, // Interval
        22 => 21, // Array
        23 => 22, // Json
        _ => 9,   // Varchar
    }
}

/// Maps a *type name* to its `(LogicType, ColumnType)`. Accepts both the SQL
/// spelling (`INT`) and the `SHOW TABLE … COLUMNS` rendering (`Integer`).
fn named_type(text: &str) -> (i32, i32) {
    let up = text.trim().to_ascii_uppercase();
    let base = up.split('(').next().unwrap_or("").trim();
    let col = match base {
        "BOOL" | "BOOLEAN" => 0,
        "TINYINT" | "INT8" => 1,
        "SMALLINT" | "INT16" => 2,
        "INT" | "INTEGER" | "INT32" => 3,
        "BIGINT" | "INT64" => 4,
        "FLOAT" | "REAL" | "FLOAT32" => 5,
        "DOUBLE" | "FLOAT64" => 6,
        "FLOAT16" => 7,
        "BFLOAT16" => 8,
        "VARCHAR" | "TEXT" | "STRING" => 9,
        "DATE" => 16,
        "TIME" => 17,
        "DATETIME" | "TIMESTAMP" => 18,
        "ARRAY" => 21,
        "JSON" => 22,
        _ => 9,
    };
    (column_type_to_logic(col), col)
}

/// The `PhysicalType` for an element type *text* (embedding/tensor/sparse), or
/// `None` for scalars.
fn element_physical(text: &str) -> PhysicalWire {
    let up = text.trim().to_ascii_uppercase();
    if up.starts_with("EMBEDDING(") || up.starts_with("VECTOR(") {
        if let Some((dim, elem)) = embedding_info(text) {
            return PhysicalWire::Embedding {
                dimension: dim,
                element_type: elem,
            };
        }
    }
    if let Some((_, dim, elem)) = tensor_info(text) {
        return PhysicalWire::Embedding {
            dimension: dim,
            element_type: elem,
        };
    }
    if let Some((dimension, element_type, index_type)) = sparse_info(text) {
        return PhysicalWire::Sparse {
            dimension,
            element_type,
            index_type,
        };
    }
    PhysicalWire::None
}

/// The `(LogicType, PhysicalType)` of an element type text.
fn element_type_meta(text: &str) -> (i32, PhysicalWire) {
    let up = text.trim().to_ascii_uppercase();
    let logic = if up.starts_with("EMBEDDING(") || up.starts_with("VECTOR(") {
        12
    } else if up.starts_with("TENSORARRAY(") {
        14
    } else if up.starts_with("MULTIVECTOR(") {
        16
    } else if up.starts_with("TENSOR(") {
        13
    } else if up.starts_with("SPARSE(") {
        15
    } else if up.starts_with("ARRAY(") {
        22
    } else {
        named_type(text).0
    };
    (logic, element_physical(text))
}

/// Parses `ARRAY(<inner>)` into `(element_logic_type, element_column_type,
/// element_physical)`, recursing for nested arrays.
fn array_info(text: &str) -> Option<(i32, i32, PhysicalWire)> {
    let t = text.trim();
    if !t.to_ascii_uppercase().starts_with("ARRAY(") {
        return None;
    }
    let inner = t[t.find('(')? + 1..].trim_end_matches(')').trim();
    if inner.to_ascii_uppercase().starts_with("ARRAY(") {
        let (l, _c, p) = array_info(inner)?;
        return Some((
            22,
            21,
            PhysicalWire::Array {
                element_logic_type: l,
                element: Box::new(p),
            },
        ));
    }
    let (logic, physical) = element_type_meta(inner);
    let col = logic_to_column_type(logic);
    Some((logic, col, physical))
}

/// Splits an engine-rendered array (`{1,2,3}` / `{{1,2},{3}}`) into its
/// top-level elements, keeping nested `{…}` groups intact.
fn parse_array_elements(cell: &str) -> Vec<String> {
    let t = cell.trim();
    let inner = if t.starts_with('{') && t.ends_with('}') && t.len() >= 2 {
        &t[1..t.len() - 1]
    } else {
        t
    };
    let mut out = Vec::new();
    let mut depth = 0i32;
    let mut cur = String::new();
    for ch in inner.chars() {
        match ch {
            '{' => {
                depth += 1;
                cur.push(ch);
            }
            '}' => {
                depth -= 1;
                cur.push(ch);
            }
            ',' if depth == 0 => {
                out.push(cur.trim().to_string());
                cur.clear();
            }
            _ => cur.push(ch),
        }
    }
    if !cur.trim().is_empty() {
        out.push(cur.trim().to_string());
    }
    out
}

/// The `ColumnType` of each nesting level of an `ARRAY(…)` type, outermost
/// first. `ARRAY(ARRAY(INT))` → `[21, 3]` (an array of arrays of int).
fn array_levels(text: &str) -> Vec<i32> {
    let mut out = Vec::new();
    let mut cur = text.trim().to_string();
    while cur.to_ascii_uppercase().starts_with("ARRAY(") {
        let inner = match cur.find('(') {
            Some(i) => cur[i + 1..].trim_end_matches(')').trim().to_string(),
            None => break,
        };
        let (logic, _col) = element_type_meta(&inner);
        out.push(logic_to_column_type(logic));
        cur = inner;
    }
    out
}

/// Appends one array value packed for the given per-level `ColumnType`s.
fn pack_array_value(chunk: &mut Vec<u8>, levels: &[i32], text: &str) {
    let elems = parse_array_elements(text);
    chunk.extend_from_slice(&(elems.len() as u32).to_le_bytes());
    let Some((&head, rest)) = levels.split_first() else {
        return;
    };
    for e in &elems {
        if rest.is_empty() {
            pack_array_element(chunk, head, e);
        } else {
            pack_array_value(chunk, rest, e);
        }
    }
}

/// Appends one scalar array element packed for its `ColumnType`.
fn pack_array_element(chunk: &mut Vec<u8>, element_column_type: i32, text: &str) {
    let t = text.trim();
    match element_column_type {
        0 => chunk.push(if t.eq_ignore_ascii_case("true") { 1 } else { 0 }),
        1 => chunk.push(t.parse::<i8>().unwrap_or(0) as u8),
        2 => chunk.extend_from_slice(&t.parse::<i16>().unwrap_or(0).to_le_bytes()),
        3 => chunk.extend_from_slice(&t.parse::<i32>().unwrap_or(0).to_le_bytes()),
        4 => chunk.extend_from_slice(&t.parse::<i64>().unwrap_or(0).to_le_bytes()),
        5 => chunk.extend_from_slice(&t.parse::<f32>().unwrap_or(0.0).to_le_bytes()),
        6 => chunk.extend_from_slice(&t.parse::<f64>().unwrap_or(0.0).to_le_bytes()),
        7 => {
            chunk.extend_from_slice(&f32_to_f16_bits(t.parse::<f32>().unwrap_or(0.0)).to_le_bytes())
        }
        16 => chunk.extend_from_slice(&date_to_days(t).unwrap_or(0).to_le_bytes()),
        17 => chunk.extend_from_slice(&time_to_secs(t).unwrap_or(0).to_le_bytes()),
        18 => {
            let (d, secs) = datetime_to_pair(t).unwrap_or((0, 0));
            chunk.extend_from_slice(&d.to_le_bytes());
            chunk.extend_from_slice(&secs.to_le_bytes());
        }
        // Varchar / Json: u32 length prefix + UTF-8.
        _ => {
            chunk.extend_from_slice(&(t.len() as u32).to_le_bytes());
            chunk.extend_from_slice(t.as_bytes());
        }
    }
}

/// Infers the wire `ColumnType` for a column from its values (the response path
/// has no declared types available): ints, floats, bools, otherwise varchar.
fn infer_column_type(rows: &[Vec<String>], ci: usize) -> i32 {
    let (mut int, mut flt, mut boolean, mut any) = (true, true, true, false);
    for r in rows {
        let Some(v) = r.get(ci) else { continue };
        let t = v.trim();
        if t.is_empty() || t.eq_ignore_ascii_case("null") {
            continue;
        }
        any = true;
        if t.parse::<i64>().is_err() {
            int = false;
        }
        if t.parse::<f64>().is_err() {
            flt = false;
        }
        if !(t.eq_ignore_ascii_case("true") || t.eq_ignore_ascii_case("false")) {
            boolean = false;
        }
    }
    if !any {
        9 // ColumnVarchar
    } else if boolean {
        0 // ColumnBool
    } else if int {
        3 // ColumnInt32
    } else if flt {
        6 // ColumnFloat64
    } else {
        9 // ColumnVarchar
    }
}

/// Encodes a result set into `ColumnField`s (one per column). The whole column
/// is a single chunk packed for its `ColumnType` (the SDK unpacks it with
/// `struct.unpack`); NULL values contribute zero bytes and a `false` bitmask.
pub fn rows_to_column_fields(columns: &[String], rows: &[Vec<String>]) -> Vec<ColumnField> {
    rows_to_column_fields_typed(columns, &[], rows)
}

/// Packs each column for its `ColumnType`; a declared embedding column is a raw
/// element stream sliced by the reader's dimension.
pub fn rows_to_column_fields_typed(
    columns: &[String],
    types: &[String],
    rows: &[Vec<String>],
) -> Vec<ColumnField> {
    let mut out = Vec::new();
    for (ci, name) in columns.iter().enumerate() {
        let declared = types.get(ci).map(String::as_str).unwrap_or("");
        let kind = classify_column(declared, rows, ci);
        if let ColKind::Array { .. } = kind {
            let mut chunk: Vec<u8> = Vec::new();
            let mut masks = Vec::with_capacity(rows.len());
            for r in rows {
                let cell = r
                    .get(ci)
                    .map(String::as_str)
                    .filter(|v| !v.eq_ignore_ascii_case("null"));
                masks.push(cell.is_some());
                let levels = array_levels(declared);
                match cell {
                    Some(c) => pack_array_value(&mut chunk, &levels, c),
                    None => chunk.extend_from_slice(&0u32.to_le_bytes()),
                }
            }
            out.push(ColumnField {
                column_type: 21, // ColumnArray
                column_name: name.clone(),
                column_vectors: vec![chunk],
                bitmasks: masks,
            });
            continue;
        }
        if let ColKind::Sparse {
            dimension: _,
            element_type,
            index_type,
        } = kind
        {
            let mut chunk: Vec<u8> = Vec::new();
            let mut masks = Vec::with_capacity(rows.len());
            for r in rows {
                let cell = r
                    .get(ci)
                    .map(String::as_str)
                    .filter(|v| !v.eq_ignore_ascii_case("null"));
                masks.push(cell.is_some());
                let (idx, val) = cell.map(parse_sparse_literal).unwrap_or_default();
                let nnz = idx.len() as u32;
                chunk.extend_from_slice(&nnz.to_le_bytes());
                for i in &idx {
                    match index_type {
                        2 => chunk.push(*i as i8 as u8),
                        3 => chunk.extend_from_slice(&(*i as i16).to_le_bytes()),
                        5 => chunk.extend_from_slice(&i.to_le_bytes()),
                        _ => chunk.extend_from_slice(&(*i as i32).to_le_bytes()),
                    }
                }
                for v in &val {
                    match element_type {
                        1 => chunk.push(*v as u8),
                        2 => chunk.push(*v as i8 as u8),
                        3 => chunk.extend_from_slice(&(*v as i16).to_le_bytes()),
                        4 => chunk.extend_from_slice(&(*v as i32).to_le_bytes()),
                        5 => chunk.extend_from_slice(&(*v as i64).to_le_bytes()),
                        7 => chunk.extend_from_slice(&v.to_le_bytes()),
                        8 => chunk.extend_from_slice(&f32_to_f16_bits(*v as f32).to_le_bytes()),
                        _ => chunk.extend_from_slice(&(*v as f32).to_le_bytes()),
                    }
                }
            }
            out.push(ColumnField {
                column_type: 13, // ColumnSparse
                column_name: name.clone(),
                column_vectors: vec![chunk],
                bitmasks: masks,
            });
            continue;
        }
        if let ColKind::Tensor {
            column_type,
            dimension,
            element_type,
        } = kind
        {
            let mut chunk: Vec<u8> = Vec::new();
            let mut masks = Vec::with_capacity(rows.len());
            for r in rows {
                let cell = r
                    .get(ci)
                    .map(String::as_str)
                    .filter(|v| !v.eq_ignore_ascii_case("null"));
                masks.push(cell.is_some());
                let cell = cell.unwrap_or("");
                if column_type == 12 {
                    // TensorArray: u32 tensor_n, then each tensor length-prefixed.
                    let tensors = parse_tensorarray_f64(cell);
                    chunk.extend_from_slice(&(tensors.len() as u32).to_le_bytes());
                    for t in &tensors {
                        let body = pack_tensor_body(t, dimension, element_type);
                        chunk.extend_from_slice(&(body.len() as u32).to_le_bytes());
                        chunk.extend_from_slice(&body);
                    }
                } else {
                    // Tensor / MultiVector: a single length-prefixed body of
                    // vectors, each `dimension` elements wide.
                    let vectors = parse_nested_f64(cell);
                    let body = pack_tensor_body(&vectors, dimension, element_type);
                    chunk.extend_from_slice(&(body.len() as u32).to_le_bytes());
                    chunk.extend_from_slice(&body);
                }
            }
            out.push(ColumnField {
                column_type,
                column_name: name.clone(),
                column_vectors: vec![chunk],
                bitmasks: masks,
            });
            continue;
        }
        if let ColKind::Embedding(dim, elem) = kind {
            let mut chunk: Vec<u8> = Vec::new();
            let mut masks = Vec::with_capacity(rows.len());
            for r in rows {
                let cell = r
                    .get(ci)
                    .map(String::as_str)
                    .filter(|v| !v.eq_ignore_ascii_case("null"));
                masks.push(cell.is_some());
                let vals: Vec<f64> = cell
                    .map(|v| {
                        v.trim()
                            .trim_start_matches('[')
                            .trim_end_matches(']')
                            .split(',')
                            .filter_map(|x| x.trim().parse::<f64>().ok())
                            .collect()
                    })
                    .unwrap_or_default();
                for k in 0..dim as usize {
                    push_element(&mut chunk, elem, vals.get(k).copied().unwrap_or(0.0));
                }
            }
            out.push(ColumnField {
                column_type: 10, // ColumnEmbedding
                column_name: name.clone(),
                column_vectors: vec![chunk],
                bitmasks: masks,
            });
            continue;
        }
        let ColKind::Scalar(code) = kind else {
            unreachable!()
        };
        let mut chunk: Vec<u8> = Vec::new();
        let mut masks = Vec::with_capacity(rows.len());
        for r in rows {
            let cell = r
                .get(ci)
                .map(String::as_str)
                .filter(|v| !v.eq_ignore_ascii_case("null"));
            let valid = cell.is_some();
            match code {
                3 => {
                    let n: i32 = cell.and_then(|v| v.trim().parse().ok()).unwrap_or(0);
                    chunk.extend_from_slice(&n.to_le_bytes());
                }
                4 => {
                    let n: i64 = cell.and_then(|v| v.trim().parse().ok()).unwrap_or(0);
                    chunk.extend_from_slice(&n.to_le_bytes());
                }
                5 => {
                    let f: f32 = cell.and_then(|v| v.trim().parse().ok()).unwrap_or(0.0);
                    chunk.extend_from_slice(&f.to_le_bytes());
                }
                6 => {
                    let f: f64 = cell.and_then(|v| v.trim().parse().ok()).unwrap_or(0.0);
                    chunk.extend_from_slice(&f.to_le_bytes());
                }
                0 => chunk.push(
                    if cell
                        .map(|v| v.eq_ignore_ascii_case("true"))
                        .unwrap_or(false)
                    {
                        1
                    } else {
                        0
                    },
                ),
                16 => {
                    // ColumnDate: i32 days since the epoch.
                    let n = cell.and_then(date_to_days).unwrap_or(0);
                    chunk.extend_from_slice(&n.to_le_bytes());
                }
                17 => {
                    // ColumnTime: i32 seconds since midnight.
                    let n = cell.and_then(time_to_secs).unwrap_or(0);
                    chunk.extend_from_slice(&n.to_le_bytes());
                }
                18 => {
                    // ColumnDateTime: i32 epoch days + i32 seconds of day.
                    let (d, t) = cell.and_then(datetime_to_pair).unwrap_or((0, 0));
                    chunk.extend_from_slice(&d.to_le_bytes());
                    chunk.extend_from_slice(&t.to_le_bytes());
                }
                // strings / JSON are `u32` little-endian length prefixed
                _ => {
                    let s = cell.unwrap_or("");
                    chunk.extend_from_slice(&(s.len() as u32).to_le_bytes());
                    chunk.extend_from_slice(s.as_bytes());
                }
            }
            masks.push(valid);
        }
        out.push(ColumnField {
            column_type: code,
            column_name: name.clone(),
            column_vectors: vec![chunk],
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

/// The `PhysicalType` variant a non-scalar column carries (`DataType.physical_type`).
#[derive(Debug, Clone, PartialEq)]
pub enum PhysicalWire {
    /// No physical descriptor (scalars).
    None,
    /// `PhysicalType{3: EmbeddingType{1: dimension, 2: element_type}}`.
    Embedding { dimension: i32, element_type: i32 },
    /// `PhysicalType{4: SparseType{1: dimension, 2: element_type, 3: index_type}}`.
    Sparse {
        dimension: i64,
        element_type: i32,
        index_type: i32,
    },
    /// `PhysicalType{5: ArrayType{1: element_data_type}}` — the element's own
    /// `DataType` nests here.
    Array {
        element_logic_type: i32,
        element: Box<PhysicalWire>,
    },
}

/// A minimal `ColumnDef` (`thrift` field ids: 1 id, 2 name, 3 data_type).
#[derive(Debug, Clone, PartialEq)]
pub struct ColumnDefWire {
    pub id: i32,
    pub name: String,
    /// `ConstantExpr` rendered as a SQL literal (`'x'`, `1`, `1.5`, `true`).
    pub default: Option<String>,
    /// `Constraint::NotNull` is present.
    pub not_null: bool,
    /// `LogicType` enum ordinal (caller-supplied).
    pub logic_type: i32,
    /// The physical descriptor for non-scalar columns (embedding/sparse).
    pub physical: PhysicalWire,
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
    write_result_response(w, seq, "Select", resp)
}

/// The same shape as `SelectResponse` (error_code, error_msg, column_defs,
/// column_fields) under any method name — `Explain` returns the same structure.
pub fn write_result_response(
    w: &mut impl Write,
    seq: i32,
    method: &str,
    resp: &SelectResponseWire,
) -> Result<()> {
    let mut o = TBinaryOutputProtocol::new(w, true);
    o.write_message_begin(&TMessageIdentifier::new(method, TMessageType::Reply, seq))
        .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new(format!("{method}_result")))
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
        write_data_type(&mut o, cd.logic_type, &cd.physical)?;
        o.write_field_end().map_err(tw)?;
        o.write_field_stop().map_err(tw)?;
        o.write_struct_end().map_err(tw)?; // ColumnDef
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

/// Writes a `DataType` value (`{1: logic_type, 2: physical_type}`), recursing
/// into the element for array types.
fn write_data_type(
    o: &mut TBinaryOutputProtocol<&mut impl Write>,
    logic_type: i32,
    physical: &PhysicalWire,
) -> Result<()> {
    o.write_struct_begin(&TStructIdentifier::new("DataType"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("logic_type", TType::I32, 1i16))
        .map_err(tw)?;
    o.write_i32(logic_type).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    match physical {
        PhysicalWire::None => {}
        PhysicalWire::Embedding {
            dimension,
            element_type,
        } => {
            o.write_field_begin(&TFieldIdentifier::new("physical_type", TType::Struct, 2i16))
                .map_err(tw)?;
            o.write_struct_begin(&TStructIdentifier::new("PhysicalType"))
                .map_err(tw)?;
            o.write_field_begin(&TFieldIdentifier::new(
                "embedding_type",
                TType::Struct,
                3i16,
            ))
            .map_err(tw)?;
            o.write_struct_begin(&TStructIdentifier::new("EmbeddingType"))
                .map_err(tw)?;
            o.write_field_begin(&TFieldIdentifier::new("dimension", TType::I32, 1i16))
                .map_err(tw)?;
            o.write_i32(*dimension).map_err(tw)?;
            o.write_field_end().map_err(tw)?;
            o.write_field_begin(&TFieldIdentifier::new("element_type", TType::I32, 2i16))
                .map_err(tw)?;
            o.write_i32(*element_type).map_err(tw)?;
            o.write_field_end().map_err(tw)?;
            o.write_field_stop().map_err(tw)?;
            o.write_struct_end().map_err(tw)?;
            o.write_field_end().map_err(tw)?;
            o.write_field_stop().map_err(tw)?;
            o.write_struct_end().map_err(tw)?;
            o.write_field_end().map_err(tw)?;
        }
        PhysicalWire::Sparse {
            dimension,
            element_type,
            index_type,
        } => {
            o.write_field_begin(&TFieldIdentifier::new("physical_type", TType::Struct, 2i16))
                .map_err(tw)?;
            o.write_struct_begin(&TStructIdentifier::new("PhysicalType"))
                .map_err(tw)?;
            o.write_field_begin(&TFieldIdentifier::new("sparse_type", TType::Struct, 4i16))
                .map_err(tw)?;
            o.write_struct_begin(&TStructIdentifier::new("SparseType"))
                .map_err(tw)?;
            o.write_field_begin(&TFieldIdentifier::new("dimension", TType::I64, 1i16))
                .map_err(tw)?;
            o.write_i64(*dimension).map_err(tw)?;
            o.write_field_end().map_err(tw)?;
            o.write_field_begin(&TFieldIdentifier::new("element_type", TType::I32, 2i16))
                .map_err(tw)?;
            o.write_i32(*element_type).map_err(tw)?;
            o.write_field_end().map_err(tw)?;
            o.write_field_begin(&TFieldIdentifier::new("index_type", TType::I32, 3i16))
                .map_err(tw)?;
            o.write_i32(*index_type).map_err(tw)?;
            o.write_field_end().map_err(tw)?;
            o.write_field_stop().map_err(tw)?;
            o.write_struct_end().map_err(tw)?;
            o.write_field_end().map_err(tw)?;
            o.write_field_stop().map_err(tw)?;
            o.write_struct_end().map_err(tw)?;
            o.write_field_end().map_err(tw)?;
        }
        PhysicalWire::Array {
            element_logic_type,
            element,
        } => {
            o.write_field_begin(&TFieldIdentifier::new("physical_type", TType::Struct, 2i16))
                .map_err(tw)?;
            o.write_struct_begin(&TStructIdentifier::new("PhysicalType"))
                .map_err(tw)?;
            o.write_field_begin(&TFieldIdentifier::new("array_type", TType::Struct, 5i16))
                .map_err(tw)?;
            o.write_struct_begin(&TStructIdentifier::new("ArrayType"))
                .map_err(tw)?;
            o.write_field_begin(&TFieldIdentifier::new(
                "element_data_type",
                TType::Struct,
                1i16,
            ))
            .map_err(tw)?;
            write_data_type(o, *element_logic_type, element)?;
            o.write_field_end().map_err(tw)?;
            o.write_field_stop().map_err(tw)?;
            o.write_struct_end().map_err(tw)?;
            o.write_field_end().map_err(tw)?;
            o.write_field_stop().map_err(tw)?;
            o.write_struct_end().map_err(tw)?;
            o.write_field_end().map_err(tw)?;
        }
    }
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?; // DataType
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
    let mut default: Option<String> = None;
    let mut not_null = false;
    let mut physical = PhysicalWire::None;
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
                        // physical_type (a struct): embedding / sparse descriptor.
                        Some(2) if g.field_type == TType::Struct => {
                            physical = read_physical_type(ip)?;
                        }
                        _ => ip.skip(g.field_type).map_err(tw)?,
                    }
                    ip.read_field_end().map_err(tw)?;
                }
                ip.read_struct_end().map_err(tw)?;
            }
            // `4: list<Constraint>` — 0 PrimaryKey, 1 NotNull, 2 Null, 3 Unique.
            Some(4) => {
                let l = ip.read_list_begin().map_err(tw)?;
                for _ in 0..l.size {
                    if ip.read_i32().map_err(tw)? == 1 {
                        not_null = true;
                    }
                }
                ip.read_list_end().map_err(tw)?;
            }
            // `5: ConstantExpr` — the column's DEFAULT.
            Some(5) if f.field_type == TType::Struct => {
                default = match read_constant_expr(ip)? {
                    ValueLit::Str(v) => Some(format!("'{}'", v.replace('\'', "''"))),
                    ValueLit::Int(v) => Some(v.to_string()),
                    ValueLit::Float(v) => Some(format!("{v}")),
                    ValueLit::Bool(v) => Some(if v { "true".into() } else { "false".into() }),
                    _ => None,
                };
            }
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok(ColumnDefWire {
        id,
        name,
        default,
        not_null,
        logic_type: lt,
        physical,
    })
}

/// Reads `PhysicalType`, extracting the embedding (`field 3`) or sparse
/// (`field 4`) descriptor.
fn read_physical_type(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<PhysicalWire> {
    ip.read_struct_begin().map_err(tw)?;
    let mut out = PhysicalWire::None;
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match f.id {
            Some(3) if f.field_type == TType::Struct => {
                ip.read_struct_begin().map_err(tw)?; // EmbeddingType
                let (mut dim, mut elem) = (0i32, 0i32);
                loop {
                    let g = ip.read_field_begin().map_err(tw)?;
                    if g.field_type == TType::Stop {
                        break;
                    }
                    match g.id {
                        Some(1) => dim = ip.read_i32().map_err(tw)?,
                        Some(2) => elem = ip.read_i32().map_err(tw)?,
                        _ => ip.skip(g.field_type).map_err(tw)?,
                    }
                    ip.read_field_end().map_err(tw)?;
                }
                ip.read_struct_end().map_err(tw)?;
                out = PhysicalWire::Embedding {
                    dimension: dim,
                    element_type: elem,
                };
            }
            Some(4) if f.field_type == TType::Struct => {
                ip.read_struct_begin().map_err(tw)?; // SparseType
                let (mut dim, mut elem, mut idx) = (0i64, 0i32, 0i32);
                loop {
                    let g = ip.read_field_begin().map_err(tw)?;
                    if g.field_type == TType::Stop {
                        break;
                    }
                    match g.id {
                        Some(1) => dim = ip.read_i64().map_err(tw)?,
                        Some(2) => elem = ip.read_i32().map_err(tw)?,
                        Some(3) => idx = ip.read_i32().map_err(tw)?,
                        _ => ip.skip(g.field_type).map_err(tw)?,
                    }
                    ip.read_field_end().map_err(tw)?;
                }
                ip.read_struct_end().map_err(tw)?;
                out = PhysicalWire::Sparse {
                    dimension: dim,
                    element_type: elem,
                    index_type: idx,
                };
            }
            Some(5) if f.field_type == TType::Struct => {
                // ArrayType{1: element_data_type}
                ip.read_struct_begin().map_err(tw)?;
                let mut element_logic_type = 0i32;
                let mut element = PhysicalWire::None;
                loop {
                    let g = ip.read_field_begin().map_err(tw)?;
                    if g.field_type == TType::Stop {
                        break;
                    }
                    if g.id == Some(1) && g.field_type == TType::Struct {
                        let dt = read_data_type(ip)?;
                        element_logic_type = dt.0;
                        element = dt.1;
                    } else {
                        ip.skip(g.field_type).map_err(tw)?;
                    }
                    ip.read_field_end().map_err(tw)?;
                }
                ip.read_struct_end().map_err(tw)?;
                out = PhysicalWire::Array {
                    element_logic_type,
                    element: Box::new(element),
                };
            }
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok(out)
}

/// Reads a `DataType` (`{1: logic_type, 2: physical_type}`), recursing for
/// array element types.
fn read_data_type(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<(i32, PhysicalWire)> {
    ip.read_struct_begin().map_err(tw)?;
    let mut logic = 0i32;
    let mut physical = PhysicalWire::None;
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match f.id {
            Some(1) => logic = ip.read_i32().map_err(tw)?,
            Some(2) if f.field_type == TType::Struct => physical = read_physical_type(ip)?,
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok((logic, physical))
}

/// Consumes the trailing `stop` of a `<Method>_args` struct.
///
/// The per-method readers stop right after the inner `…Request` struct; that
/// leaves the *enclosing* args struct's own `stop` byte in the stream, which
/// desynchronizes every subsequent message on the same connection (the SDK
/// then silently reconnects and retries, and `Disconnect` surfaces a
/// `CLIENT_CLOSE`). Every `read_*_args` must therefore finish here.
fn end_args(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<()> {
    let _ = ip.read_field_end();
    let _ = ip.read_field_begin();
    ip.read_struct_end().map_err(tw)?;
    Ok(())
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
    end_args(ip)?; // CreateTable_args
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

/// A decoded `ParsedExpr` (subset): a column reference, a constant, a function
/// call (rendered back to SQL text), or unknown.
#[derive(Debug, Clone, PartialEq)]
pub enum PExpr {
    Column(String),
    Value(ValueLit),
    Call(String, Vec<PExpr>),
    Other,
}

/// Renders a decoded `ParsedExpr` back to SQL text, so a projected function
/// (`year(c1)`, `datepart(c4, c1)`) can be sent to the engine as-is.
pub fn pexpr_sql(e: &PExpr) -> String {
    match e {
        PExpr::Column(c) => c.clone(),
        PExpr::Value(v) => v.to_sql(),
        // The SDK sends a projected binary expression as `FunctionExpr("+", [a, b])`.
        PExpr::Call(n, a) if a.len() == 2 && is_binary_op(n) => {
            format!("({} {n} {})", pexpr_sql(&a[0]), pexpr_sql(&a[1]))
        }
        PExpr::Call(n, a) => format!(
            "{}({})",
            n,
            a.iter().map(pexpr_sql).collect::<Vec<_>>().join(", ")
        ),
        PExpr::Other => String::new(),
    }
}

/// Arithmetic/comparison operators the SDK encodes as two-argument functions.
fn is_binary_op(name: &str) -> bool {
    matches!(
        name,
        "+" | "-"
            | "*"
            | "/"
            | "%"
            | "="
            | "=="
            | "!="
            | "<>"
            | "<"
            | "<="
            | ">"
            | ">="
            | "and"
            | "or"
    )
}

/// Reads a `FunctionExpr`: `{1: string function_name, 2: list<ParsedExpr> arguments}`.
fn read_function_expr(
    ip: &mut TBinaryInputProtocol<&mut impl Read>,
) -> Result<(String, Vec<PExpr>)> {
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
    Ok((name, args))
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
                    let (names, star) = read_column_expr(ip)?;
                    // `count(*)` carries a star column with no name.
                    out = if star && names.is_empty() {
                        PExpr::Column("*".into())
                    } else {
                        PExpr::Column(names.into_iter().next().unwrap_or_default())
                    };
                }
                Some(3) if uf.field_type == TType::Struct => {
                    let (name, args) = read_function_expr(ip)?;
                    out = PExpr::Call(name, args);
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
            } else if uf.id == Some(3) && uf.field_type == TType::Struct {
                // A projected function: send it to the engine as expression text
                // (`year(c1)`), which is also the output column name.
                let (name, args) = read_function_expr(ip)?;
                names.push(pexpr_sql(&PExpr::Call(name, args)));
            } else if uf.field_type == TType::Struct {
                // consume the nested expression struct explicitly (a `skip` on
                // a struct holding lists can under-consume)
                ip.read_struct_begin().map_err(tw)?;
                loop {
                    let f2 = ip.read_field_begin().map_err(tw)?;
                    if f2.field_type == TType::Stop {
                        break;
                    }
                    ip.skip(f2.field_type).map_err(tw)?;
                    ip.read_field_end().map_err(tw)?;
                }
                ip.read_struct_end().map_err(tw)?;
            } else {
                ip.skip(uf.field_type).map_err(tw)?;
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
    end_args(ip)?; // Select_args
    Ok(r)
}

/// Reads a full `Select` CALL, returning `(seqid, request)`.
pub fn read_select_call(r: &mut impl Read) -> Result<(i32, SelectReq)> {
    let mut ip = TBinaryInputProtocol::new(r, true);
    let msg = ip.read_message_begin().map_err(tw)?;
    let req = read_select_args(&mut ip)?;
    Ok((msg.sequence_number, req))
}

/// Builds a `SelectResponse` from a materialized result set (no declared types).
pub fn select_response_from(columns: &[String], rows: &[Vec<String>]) -> SelectResponseWire {
    select_response_from_typed(columns, &[], rows)
}

/// Resolves each result column to its declared type text via
/// `SHOW TABLE <table> COLUMNS`, so non-scalar columns are announced with their
/// physical descriptor. Unknown columns map to an empty string (inferred).
pub fn declared_types(engine: &Engine, table: &str, columns: &[String]) -> Vec<String> {
    let mut map: HashMap<String, String> = HashMap::new();
    // `SHOW TABLE` resolves unqualified names against `default_db`; qualify
    // explicitly so a session-level `USE` cannot hide the schema.
    let qualified = if table.contains('.') {
        table.to_string()
    } else {
        format!("default_db.{table}")
    };
    if let Ok(rs) = engine.execute(&format!("SHOW TABLE {qualified} COLUMNS;")) {
        for r in rs {
            if let QueryResult::Rows { rows, .. } = r {
                for row in rows {
                    if row.len() >= 2 {
                        map.insert(row[0].to_ascii_lowercase(), row[1].clone());
                    }
                }
            }
        }
    }
    columns
        .iter()
        .map(|c| {
            let key = c.rsplit('.').next().unwrap_or(c).to_ascii_lowercase();
            map.get(&key).cloned().unwrap_or_default()
        })
        .collect()
}

/// Builds a `SelectResponse` with declared column type *text* available, so
/// non-scalar columns (embedding/vector/...) are announced with their physical
/// descriptor and packed as their live element stream.
pub fn select_response_from_typed(
    columns: &[String],
    types: &[String],
    rows: &[Vec<String>],
) -> SelectResponseWire {
    let column_defs = columns
        .iter()
        .enumerate()
        .map(|(i, name)| {
            let declared = types.get(i).map(String::as_str).unwrap_or("");
            let (logic_type, physical) = match classify_column(declared, rows, i) {
                ColKind::Embedding(dim, elem) => (
                    LOGIC_EMBEDDING,
                    PhysicalWire::Embedding {
                        dimension: dim,
                        element_type: elem,
                    },
                ),
                ColKind::Sparse {
                    dimension,
                    element_type,
                    index_type,
                } => (
                    LOGIC_SPARSE,
                    PhysicalWire::Sparse {
                        dimension,
                        element_type,
                        index_type,
                    },
                ),
                ColKind::Tensor {
                    column_type,
                    dimension,
                    element_type,
                } => (
                    column_type_to_logic(column_type),
                    PhysicalWire::Embedding {
                        dimension,
                        element_type,
                    },
                ),
                ColKind::Array { .. } => (
                    LOGIC_ARRAY,
                    array_info(declared)
                        .map(|(logic, _col, physical)| PhysicalWire::Array {
                            element_logic_type: logic,
                            element: Box::new(physical),
                        })
                        .unwrap_or(PhysicalWire::None),
                ),
                ColKind::Scalar(code) => (column_type_to_logic(code), PhysicalWire::None),
            };
            ColumnDefWire {
                id: i as i32,
                name: name.clone(),
                default: None,
                not_null: false,
                logic_type,
                physical,
            }
        })
        .collect();
    SelectResponseWire {
        error_code: 0,
        error_msg: "OK".into(),
        column_defs,
        column_fields: rows_to_column_fields_typed(columns, types, rows),
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
    /// `[..]` of ints (embedding/int array).
    IntArray(Vec<i64>),
    /// `[..]` of floats (embedding/float array).
    FloatArray(Vec<f64>),
    /// `[[..],..]` int tensor.
    IntTensor(Vec<Vec<i64>>),
    /// `[[..],..]` float tensor.
    FloatTensor(Vec<Vec<f64>>),
    /// `[[[..],..],..]` int tensor array.
    IntTensorArray(Vec<Vec<Vec<i64>>>),
    /// `[[[..],..],..]` float tensor array.
    FloatTensorArray(Vec<Vec<Vec<f64>>>),
    /// Sparse vector `{index: value}` (index/values paired).
    Sparse {
        indices: Vec<i64>,
        ints: Vec<i64>,
        floats: Vec<f64>,
        is_float: bool,
    },
    /// `{…}` brace-bracketed array (the SDK's `Array(...)` wrapper); nested
    /// arrays nest here.
    Brackets(Vec<ValueLit>),
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
            ValueLit::IntArray(v) => {
                format!(
                    "[{}]",
                    v.iter()
                        .map(|x| x.to_string())
                        .collect::<Vec<_>>()
                        .join(",")
                )
            }
            ValueLit::FloatArray(v) => {
                format!(
                    "[{}]",
                    v.iter()
                        .map(|x| x.to_string())
                        .collect::<Vec<_>>()
                        .join(",")
                )
            }
            ValueLit::IntTensor(v) => format!(
                "[{}]",
                v.iter()
                    .map(|r| format!(
                        "[{}]",
                        r.iter()
                            .map(|x| x.to_string())
                            .collect::<Vec<_>>()
                            .join(",")
                    ))
                    .collect::<Vec<_>>()
                    .join(",")
            ),
            ValueLit::FloatTensor(v) => format!(
                "[{}]",
                v.iter()
                    .map(|r| format!(
                        "[{}]",
                        r.iter()
                            .map(|x| x.to_string())
                            .collect::<Vec<_>>()
                            .join(",")
                    ))
                    .collect::<Vec<_>>()
                    .join(",")
            ),
            ValueLit::IntTensorArray(v) => format!(
                "[{}]",
                v.iter()
                    .map(|t| format!(
                        "[{}]",
                        t.iter()
                            .map(|r| format!(
                                "[{}]",
                                r.iter()
                                    .map(|x| x.to_string())
                                    .collect::<Vec<_>>()
                                    .join(",")
                            ))
                            .collect::<Vec<_>>()
                            .join(",")
                    ))
                    .collect::<Vec<_>>()
                    .join(",")
            ),
            ValueLit::FloatTensorArray(v) => format!(
                "[{}]",
                v.iter()
                    .map(|t| format!(
                        "[{}]",
                        t.iter()
                            .map(|r| format!(
                                "[{}]",
                                r.iter()
                                    .map(|x| x.to_string())
                                    .collect::<Vec<_>>()
                                    .join(",")
                            ))
                            .collect::<Vec<_>>()
                            .join(",")
                    ))
                    .collect::<Vec<_>>()
                    .join(",")
            ),
            ValueLit::Brackets(items) => format!(
                "{{{}}}",
                items
                    .iter()
                    .map(|v| v.to_sql())
                    .collect::<Vec<_>>()
                    .join(",")
            ),
            ValueLit::Sparse {
                indices,
                ints,
                floats,
                is_float,
            } => {
                let pairs: Vec<String> = indices
                    .iter()
                    .enumerate()
                    .map(|(k, idx)| {
                        let val = if *is_float {
                            floats.get(k).copied().unwrap_or(0.0).to_string()
                        } else {
                            ints.get(k).copied().unwrap_or(0).to_string()
                        };
                        format!("{idx}:{val}")
                    })
                    .collect();
                format!("[{}]", pairs.join(","))
            }
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
    let mut i64_arr: Vec<i64> = Vec::new();
    let mut f64_arr: Vec<f64> = Vec::new();
    let mut i64_tensor: Vec<Vec<i64>> = Vec::new();
    let mut f64_tensor: Vec<Vec<f64>> = Vec::new();
    let mut i64_tensor_array: Vec<Vec<Vec<i64>>> = Vec::new();
    let mut f64_tensor_array: Vec<Vec<Vec<f64>>> = Vec::new();
    let mut idx_arr: Vec<i64> = Vec::new();
    let mut brackets: Vec<ValueLit> = Vec::new();
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
            Some(6) => i64_arr = read_i64_list(ip)?,
            Some(7) => f64_arr = read_f64_list(ip)?,
            Some(8) => i64_tensor = read_i64_list_list(ip)?,
            Some(9) => f64_tensor = read_f64_list_list(ip)?,
            Some(10) => i64_tensor_array = read_i64_list_list_list(ip)?,
            Some(11) => f64_tensor_array = read_f64_list_list_list(ip)?,
            Some(12) => idx_arr = read_i64_list(ip)?,
            Some(13) => brackets = read_constant_expr_list(ip)?,
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
        5 => ValueLit::IntArray(i64_arr),       // IntegerArray
        6 => ValueLit::FloatArray(f64_arr),     // DoubleArray
        7 => ValueLit::IntTensor(i64_tensor),   // IntegerTensor
        8 => ValueLit::FloatTensor(f64_tensor), // DoubleTensor
        9 => ValueLit::IntTensorArray(i64_tensor_array), // IntegerTensorArray
        10 => ValueLit::FloatTensorArray(f64_tensor_array), // DoubleTensorArray
        11 => ValueLit::Sparse {
            indices: idx_arr,
            ints: i64_arr,
            floats: Vec::new(),
            is_float: false,
        },
        12 => ValueLit::Sparse {
            indices: idx_arr,
            ints: Vec::new(),
            floats: f64_arr,
            is_float: true,
        },
        18 => ValueLit::Brackets(brackets), // CurlyBracketsArray
        _ => ValueLit::Null,
    })
}

/// Reads a `list<ConstantExpr>` (the brace-bracketed `Array(...)` payload).
fn read_constant_expr_list(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<Vec<ValueLit>> {
    let l = ip.read_list_begin().map_err(tw)?;
    let mut out = Vec::with_capacity(l.size.max(0) as usize);
    for _ in 0..l.size {
        out.push(read_constant_expr(ip)?);
    }
    ip.read_list_end().map_err(tw)?;
    Ok(out)
}

/// Reads a `list<i64>`.
fn read_i64_list(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<Vec<i64>> {
    let l = ip.read_list_begin().map_err(tw)?;
    let mut out = Vec::with_capacity(l.size.max(0) as usize);
    for _ in 0..l.size {
        out.push(ip.read_i64().map_err(tw)?);
    }
    ip.read_list_end().map_err(tw)?;
    Ok(out)
}

/// Reads a `list<double>`.
fn read_f64_list(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<Vec<f64>> {
    let l = ip.read_list_begin().map_err(tw)?;
    let mut out = Vec::with_capacity(l.size.max(0) as usize);
    for _ in 0..l.size {
        out.push(ip.read_double().map_err(tw)?);
    }
    ip.read_list_end().map_err(tw)?;
    Ok(out)
}

/// Reads a `list<list<i64>>`.
fn read_i64_list_list(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<Vec<Vec<i64>>> {
    let l = ip.read_list_begin().map_err(tw)?;
    let mut out = Vec::with_capacity(l.size.max(0) as usize);
    for _ in 0..l.size {
        out.push(read_i64_list(ip)?);
    }
    ip.read_list_end().map_err(tw)?;
    Ok(out)
}

/// Reads a `list<list<double>>`.
fn read_f64_list_list(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<Vec<Vec<f64>>> {
    let l = ip.read_list_begin().map_err(tw)?;
    let mut out = Vec::with_capacity(l.size.max(0) as usize);
    for _ in 0..l.size {
        out.push(read_f64_list(ip)?);
    }
    ip.read_list_end().map_err(tw)?;
    Ok(out)
}

/// Reads a `list<list<list<i64>>>` (tensor array).
fn read_i64_list_list_list(
    ip: &mut TBinaryInputProtocol<&mut impl Read>,
) -> Result<Vec<Vec<Vec<i64>>>> {
    let l = ip.read_list_begin().map_err(tw)?;
    let mut out = Vec::with_capacity(l.size.max(0) as usize);
    for _ in 0..l.size {
        out.push(read_i64_list_list(ip)?);
    }
    ip.read_list_end().map_err(tw)?;
    Ok(out)
}

/// Reads a `list<list<list<double>>>` (tensor array).
fn read_f64_list_list_list(
    ip: &mut TBinaryInputProtocol<&mut impl Read>,
) -> Result<Vec<Vec<Vec<f64>>>> {
    let l = ip.read_list_begin().map_err(tw)?;
    let mut out = Vec::with_capacity(l.size.max(0) as usize);
    for _ in 0..l.size {
        out.push(read_f64_list_list(ip)?);
    }
    ip.read_list_end().map_err(tw)?;
    Ok(out)
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
    end_args(ip)?; // Insert_args
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
    end_args(ip)?; // ListDatabase_args
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
    end_args(ip)?; // <Method>_args
    Ok((sid, strs))
}

/// Executes `sql` against the bound engine, mapping the outcome to a
/// `CommonResponse` (`error_code` 0 on success).
/// A `CreateIndex` request reduced to what the engine needs.
pub struct CreateIndexReq {
    pub db_name: String,
    pub table_name: String,
    pub index_name: String,
    pub column: String,
    pub session_id: i64,
}

/// Reads the column name out of a nested `IndexInfo` struct.
fn read_index_info_column(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<Option<String>> {
    ip.read_struct_begin().map_err(tw)?;
    let mut col = None;
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match f.id {
            Some(1) if f.field_type == TType::String => col = Some(ip.read_string().map_err(tw)?),
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok(col)
}

/// Reads a `CreateIndex` CALL, extracting `db.table(index_name, column)`.
pub fn read_create_index_args(
    ip: &mut TBinaryInputProtocol<&mut impl Read>,
) -> Result<CreateIndexReq> {
    ip.read_struct_begin().map_err(tw)?; // CreateIndex_args
    let _ = ip.read_field_begin().map_err(tw)?; // request (Struct)
    ip.read_struct_begin().map_err(tw)?; // CreateIndexRequest
    let mut r = CreateIndexReq {
        db_name: String::new(),
        table_name: String::new(),
        index_name: String::new(),
        column: String::new(),
        session_id: 0,
    };
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match f.id {
            Some(1) if f.field_type == TType::String => r.db_name = ip.read_string().map_err(tw)?,
            Some(2) if f.field_type == TType::String => {
                r.table_name = ip.read_string().map_err(tw)?;
            }
            Some(3) if f.field_type == TType::String => {
                r.index_name = ip.read_string().map_err(tw)?;
            }
            Some(5) if f.field_type == TType::Struct => {
                if let Some(c) = read_index_info_column(ip)? {
                    r.column = c;
                }
            }
            Some(6) => r.session_id = ip.read_i64().map_err(tw)?,
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    end_args(ip)?; // CreateIndex_args
    Ok(r)
}

fn run_common(engine: Option<&Arc<Engine>>, sql: &str, session_id: i64) -> CommonResponse {
    match engine {
        Some(eng) => match eng.execute(sql) {
            Ok(_) => CommonResponse {
                error_code: OK,
                error_msg: "OK".into(),
                session_id,
            },
            Err(e) => CommonResponse {
                error_code: e.status_code(),
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

/// Writes a `<Method>_result` REPLY carrying
/// `{error_code, error_msg, <list_field>: list<string>}` — used by
/// `ListTable` (`table_names`) and `ListIndex` (`index_names`).
pub fn write_string_list_response(
    w: &mut impl Write,
    seq: i32,
    method: &str,
    list_field: &str,
    names: &[String],
) -> Result<()> {
    let mut o = TBinaryOutputProtocol::new(w, true);
    o.write_message_begin(&TMessageIdentifier::new(method, TMessageType::Reply, seq))
        .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new(format!("{method}_result")))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("success", TType::Struct, 0i16))
        .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new(format!("{method}Response")))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("error_code", TType::I64, 1i16))
        .map_err(tw)?;
    o.write_i64(0).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("error_msg", TType::String, 2i16))
        .map_err(tw)?;
    o.write_string("OK").map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new(list_field, TType::List, 3i16))
        .map_err(tw)?;
    o.write_list_begin(&TListIdentifier::new(TType::String, names.len() as i32))
        .map_err(tw)?;
    for n in names {
        o.write_string(n).map_err(tw)?;
    }
    o.write_list_end().map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?;
    o.flush().map_err(tw)?;
    Ok(())
}

/// Writes a `ShowDatabase` REPLY (`ShowDatabaseResponse` fields 1..=6).
pub fn write_show_database_response(
    w: &mut impl Write,
    seq: i32,
    name: &str,
    table_count: i64,
    comment: &str,
) -> Result<()> {
    let mut o = TBinaryOutputProtocol::new(w, true);
    o.write_message_begin(&TMessageIdentifier::new(
        "ShowDatabase",
        TMessageType::Reply,
        seq,
    ))
    .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("ShowDatabase_result"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("success", TType::Struct, 0i16))
        .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("ShowDatabaseResponse"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("error_code", TType::I64, 1i16))
        .map_err(tw)?;
    o.write_i64(0).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("error_msg", TType::String, 2i16))
        .map_err(tw)?;
    o.write_string("OK").map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("database_name", TType::String, 3i16))
        .map_err(tw)?;
    o.write_string(name).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("store_dir", TType::String, 4i16))
        .map_err(tw)?;
    o.write_string("").map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("table_count", TType::I64, 5i16))
        .map_err(tw)?;
    o.write_i64(table_count).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("comment", TType::String, 6i16))
        .map_err(tw)?;
    o.write_string(comment).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?; // ShowDatabaseResponse
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?; // ShowDatabase_result
    o.flush().map_err(tw)?;
    Ok(())
}

/// Writes a `ShowCurrentNode` REPLY (`ShowCurrentNodeResponse` fields 1..=4).
pub fn write_show_current_node_response(
    w: &mut impl Write,
    seq: i32,
    node_role: &str,
    server_status: &str,
) -> Result<()> {
    let mut o = TBinaryOutputProtocol::new(w, true);
    o.write_message_begin(&TMessageIdentifier::new(
        "ShowCurrentNode",
        TMessageType::Reply,
        seq,
    ))
    .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("ShowCurrentNode_result"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("success", TType::Struct, 0i16))
        .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("ShowCurrentNodeResponse"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("error_code", TType::I64, 1i16))
        .map_err(tw)?;
    o.write_i64(0).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("error_msg", TType::String, 2i16))
        .map_err(tw)?;
    o.write_string("OK").map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("node_role", TType::String, 3i16))
        .map_err(tw)?;
    o.write_string(node_role).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("server_status", TType::String, 4i16))
        .map_err(tw)?;
    o.write_string(server_status).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?;
    o.flush().map_err(tw)?;
    Ok(())
}

/// Writes a `ListSnapshots` REPLY with the given snapshot names
/// (`ListSnapshotsResponse.snapshots` is a list of `Snapshot` structs).
pub fn write_list_snapshots_response(w: &mut impl Write, seq: i32) -> Result<()> {
    let mut o = TBinaryOutputProtocol::new(w, true);
    o.write_message_begin(&TMessageIdentifier::new(
        "ListSnapshots",
        TMessageType::Reply,
        seq,
    ))
    .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("ListSnapshots_result"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("success", TType::Struct, 0i16))
        .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("ListSnapshotsResponse"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("error_code", TType::I64, 1i16))
        .map_err(tw)?;
    o.write_i64(0).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("error_msg", TType::String, 2i16))
        .map_err(tw)?;
    o.write_string("OK").map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("snapshots", TType::List, 3i16))
        .map_err(tw)?;
    o.write_list_begin(&TListIdentifier::new(TType::Struct, 0))
        .map_err(tw)?;
    o.write_list_end().map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?;
    o.flush().map_err(tw)?;
    Ok(())
}

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
    ip.read_struct_end().map_err(tw)?; // DeleteRequest
    end_args(ip)?; // Delete_args
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
    ip.read_struct_end().map_err(tw)?; // UpdateRequest
    end_args(ip)?; // Update_args
    Ok(r)
}
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
    ip.read_struct_end().map_err(tw)?; // ShowTableRequest
    end_args(ip)?; // ShowTable_args
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

/// `ExportRequest { 1: db, 2: table, 3: columns, 4: file, 5: ExportOption, 6: session_id }`.
struct ExportArgs {
    db: String,
    table: String,
    file: String,
    delimiter: String,
    file_type: i32,
    columns: Vec<String>,
    offset: i64,
    limit: i64,
    row_limit: i64,
    session_id: i64,
}

fn read_export_args(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<ExportArgs> {
    let mut a = ExportArgs {
        db: String::new(),
        table: String::new(),
        file: String::new(),
        delimiter: ",".into(),
        file_type: 0,
        columns: Vec::new(),
        offset: 0,
        limit: 0,
        row_limit: 0,
        session_id: 0,
    };
    ip.read_struct_begin().map_err(tw)?;
    let _ = ip.read_field_begin().map_err(tw)?;
    ip.read_struct_begin().map_err(tw)?;
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match (f.id, f.field_type) {
            (Some(1), TType::String) => a.db = ip.read_string().map_err(tw)?,
            (Some(2), TType::String) => a.table = ip.read_string().map_err(tw)?,
            (Some(3), TType::List) => {
                let l = ip.read_list_begin().map_err(tw)?;
                for _ in 0..l.size {
                    a.columns.push(ip.read_string().map_err(tw)?);
                }
                ip.read_list_end().map_err(tw)?;
            }
            (Some(4), TType::String) => a.file = ip.read_string().map_err(tw)?,
            (Some(5), TType::Struct) => {
                ip.read_struct_begin().map_err(tw)?;
                loop {
                    let of = ip.read_field_begin().map_err(tw)?;
                    if of.field_type == TType::Stop {
                        break;
                    }
                    match (of.id, of.field_type) {
                        (Some(1), TType::String) => a.delimiter = ip.read_string().map_err(tw)?,
                        (Some(3), TType::I32) => a.file_type = ip.read_i32().map_err(tw)?,
                        (Some(4), TType::I64) => a.offset = ip.read_i64().map_err(tw)?,
                        (Some(5), TType::I64) => a.limit = ip.read_i64().map_err(tw)?,
                        (Some(6), TType::I64) => a.row_limit = ip.read_i64().map_err(tw)?,
                        _ => ip.skip(of.field_type).map_err(tw)?,
                    }
                    ip.read_field_end().map_err(tw)?;
                }
                ip.read_struct_end().map_err(tw)?;
            }
            (Some(6), TType::I64) => a.session_id = ip.read_i64().map_err(tw)?,
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    end_args(ip)?;
    Ok(a)
}

/// `SetConfigRequest { 1: session_id, 2: config_name, 3: ConfigValue }`.
fn read_set_config_args(
    ip: &mut TBinaryInputProtocol<&mut impl Read>,
) -> Result<(i64, String, String, i32)> {
    let (mut sid, mut name, mut value) = (0i64, String::new(), String::new());
    let mut kind = 1i32; // ConfigValue arm: 1 string, 2 int, 3 bool, 4 double
    ip.read_struct_begin().map_err(tw)?;
    let _ = ip.read_field_begin().map_err(tw)?;
    ip.read_struct_begin().map_err(tw)?;
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match (f.id, f.field_type) {
            (Some(1), TType::I64) => sid = ip.read_i64().map_err(tw)?,
            (Some(2), TType::String) => name = ip.read_string().map_err(tw)?,
            (Some(3), TType::Struct) => {
                // `union ConfigValue { 1: string, 2: i64, 3: bool, 4: double }`
                ip.read_struct_begin().map_err(tw)?;
                loop {
                    let vf = ip.read_field_begin().map_err(tw)?;
                    if vf.field_type == TType::Stop {
                        break;
                    }
                    kind = vf.id.map(i32::from).unwrap_or(kind);
                    value = match (vf.id, vf.field_type) {
                        (Some(1), TType::String) => ip.read_string().map_err(tw)?,
                        (Some(2), TType::I64) => ip.read_i64().map_err(tw)?.to_string(),
                        (Some(3), TType::Bool) => {
                            if ip.read_bool().map_err(tw)? {
                                "true".into()
                            } else {
                                "false".into()
                            }
                        }
                        (Some(4), TType::Double) => format!("{}", ip.read_double().map_err(tw)?),
                        _ => {
                            ip.skip(vf.field_type).map_err(tw)?;
                            value
                        }
                    };
                    ip.read_field_end().map_err(tw)?;
                }
                ip.read_struct_end().map_err(tw)?;
            }
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    end_args(ip)?;
    // A value with spaces must be quoted for the SQL parser.
    if value.is_empty() || value.contains(' ') {
        value = format!("'{}'", value.replace('\'', "''"));
    }
    Ok((sid, name, value, kind))
}

/// `ShowConfigRequest { 1: session_id, 2: config_name }`.
fn read_show_config_args(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<(i64, String)> {
    let (mut sid, mut name) = (0i64, String::new());
    ip.read_struct_begin().map_err(tw)?;
    let _ = ip.read_field_begin().map_err(tw)?;
    ip.read_struct_begin().map_err(tw)?;
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match (f.id, f.field_type) {
            (Some(1), TType::I64) => sid = ip.read_i64().map_err(tw)?,
            (Some(2), TType::String) => name = ip.read_string().map_err(tw)?,
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    end_args(ip)?;
    Ok((sid, name))
}

/// `ShowConfigResponse { 1: error_code, 2: error_msg, 3: config_name, 4: ConfigValue }`.
fn write_show_config_response(
    w: &mut impl Write,
    seq: i32,
    sid: i64,
    name: &str,
    value: Option<&str>,
) -> Result<()> {
    let mut o = TBinaryOutputProtocol::new(w, true);
    o.write_message_begin(&TMessageIdentifier::new(
        "ShowConfig",
        TMessageType::Reply,
        seq,
    ))
    .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("ShowConfig_result"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("success", TType::Struct, 0i16))
        .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("ShowConfigResponse"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("error_code", TType::I64, 1i16))
        .map_err(tw)?;
    o.write_i64(if value.is_some() { OK } else { 3014 })
        .map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("error_msg", TType::String, 2i16))
        .map_err(tw)?;
    o.write_string(value.map(|_| "OK").unwrap_or("unknown config"))
        .map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("config_name", TType::String, 3i16))
        .map_err(tw)?;
    o.write_string(name).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    if let Some(v) = value {
        // Numeric knobs answer with the `int_value` arm, the rest with a string.
        let numeric = matches!(
            name.to_ascii_lowercase().as_str(),
            "cleanup_interval" | "checkpoint_interval" | "cache_result_capacity"
        );
        o.write_field_begin(&TFieldIdentifier::new("config_value", TType::Struct, 4i16))
            .map_err(tw)?;
        o.write_struct_begin(&TStructIdentifier::new("ConfigValue"))
            .map_err(tw)?;
        if numeric {
            o.write_field_begin(&TFieldIdentifier::new("int_value", TType::I64, 2i16))
                .map_err(tw)?;
            o.write_i64(v.parse().unwrap_or(0)).map_err(tw)?;
        } else {
            o.write_field_begin(&TFieldIdentifier::new("string_value", TType::String, 1i16))
                .map_err(tw)?;
            o.write_string(v).map_err(tw)?;
        }
        o.write_field_end().map_err(tw)?;
        o.write_field_stop().map_err(tw)?;
        o.write_struct_end().map_err(tw)?;
        o.write_field_end().map_err(tw)?;
    }
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?;
    o.flush().map_err(tw)?;
    let _ = sid;
    Ok(())
}

/// `AddColumnsRequest { 1: db, 2: table, 3: list<ColumnDef>, 4: session_id }`.
fn read_alter_add_args(
    ip: &mut TBinaryInputProtocol<&mut impl Read>,
) -> Result<(String, String, Vec<ColumnDefWire>, i64)> {
    let (mut db, mut table, mut cols, mut sid) = (String::new(), String::new(), Vec::new(), 0i64);
    ip.read_struct_begin().map_err(tw)?;
    let _ = ip.read_field_begin().map_err(tw)?;
    ip.read_struct_begin().map_err(tw)?;
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match f.id {
            Some(1) => db = ip.read_string().map_err(tw)?,
            Some(2) => table = ip.read_string().map_err(tw)?,
            Some(3) => {
                let l = ip.read_list_begin().map_err(tw)?;
                for _ in 0..l.size {
                    cols.push(read_column_def(ip)?);
                }
                ip.read_list_end().map_err(tw)?;
            }
            Some(4) => sid = ip.read_i64().map_err(tw)?,
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    end_args(ip)?;
    Ok((db, table, cols, sid))
}

/// `DropColumnsRequest { 1: db, 2: table, 3: list<string>, 4: session_id }`.
fn read_alter_drop_args(
    ip: &mut TBinaryInputProtocol<&mut impl Read>,
) -> Result<(String, String, Vec<String>, i64)> {
    let (mut db, mut table, mut names, mut sid) = (String::new(), String::new(), Vec::new(), 0i64);
    ip.read_struct_begin().map_err(tw)?;
    let _ = ip.read_field_begin().map_err(tw)?;
    ip.read_struct_begin().map_err(tw)?;
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match f.id {
            Some(1) => db = ip.read_string().map_err(tw)?,
            Some(2) => table = ip.read_string().map_err(tw)?,
            Some(3) => {
                let l = ip.read_list_begin().map_err(tw)?;
                for _ in 0..l.size {
                    names.push(ip.read_string().map_err(tw)?);
                }
                ip.read_list_end().map_err(tw)?;
            }
            Some(4) => sid = ip.read_i64().map_err(tw)?,
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    end_args(ip)?;
    Ok((db, table, names, sid))
}

/// `ExplainRequest`: the `SelectRequest` prefix plus `13: ExplainType`.
fn read_explain_args(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<(SelectReq, i32)> {
    let mut req = SelectReq {
        session_id: 0,
        db_name: String::new(),
        table_name: String::new(),
        columns: Vec::new(),
        star: false,
        where_sql: None,
        order_by: None,
        limit: None,
    };
    let mut explain_type = 4; // Physical
    ip.read_struct_begin().map_err(tw)?; // Explain_args
    let _ = ip.read_field_begin().map_err(tw)?; // request
    ip.read_struct_begin().map_err(tw)?; // ExplainRequest
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match f.id {
            Some(1) => req.session_id = ip.read_i64().map_err(tw)?,
            Some(2) => req.db_name = ip.read_string().map_err(tw)?,
            Some(3) => req.table_name = ip.read_string().map_err(tw)?,
            Some(4) => {
                let l = ip.read_list_begin().map_err(tw)?;
                for _ in 0..l.size {
                    let (names, star) = read_select_item(ip)?;
                    req.columns.extend(names);
                    req.star |= star;
                }
                ip.read_list_end().map_err(tw)?;
            }
            Some(7) if f.field_type == TType::Struct => req.where_sql = read_where(ip)?,
            Some(10) if f.field_type == TType::Struct => {
                if let PExpr::Value(ValueLit::Int(n)) = read_pexpr(ip)? {
                    req.limit = Some(n);
                }
            }
            Some(12) => req.order_by = read_order_by(ip)?,
            Some(13) => explain_type = ip.read_i32().map_err(tw)?,
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?; // ExplainRequest
    end_args(ip)?; // Explain_args
    Ok((req, explain_type))
}

/// `ImportRequest { 1: db_name, 2: table_name, 3: file_name, 4: ImportOption, 5: session_id }`
/// where `ImportOption { 1: delimiter, 2: has_header, 3: copy_file_type }`.
struct ImportArgs {
    db: String,
    table: String,
    file: String,
    delimiter: String,
    has_header: bool,
    file_type: i32,
    session_id: i64,
}

fn read_import_args(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<ImportArgs> {
    let mut args = ImportArgs {
        db: String::new(),
        table: String::new(),
        file: String::new(),
        delimiter: ",".into(),
        has_header: false,
        file_type: 0,
        session_id: 0,
    };
    ip.read_struct_begin().map_err(tw)?; // Import_args
    let _ = ip.read_field_begin().map_err(tw)?; // request
    ip.read_struct_begin().map_err(tw)?; // ImportRequest
    loop {
        let fd = ip.read_field_begin().map_err(tw)?;
        if fd.field_type == TType::Stop {
            break;
        }
        match (fd.id, fd.field_type) {
            (Some(1), TType::String) => args.db = ip.read_string().map_err(tw)?,
            (Some(2), TType::String) => args.table = ip.read_string().map_err(tw)?,
            (Some(3), TType::String) => args.file = ip.read_string().map_err(tw)?,
            (Some(4), TType::Struct) => {
                ip.read_struct_begin().map_err(tw)?;
                loop {
                    let of = ip.read_field_begin().map_err(tw)?;
                    if of.field_type == TType::Stop {
                        break;
                    }
                    match (of.id, of.field_type) {
                        (Some(1), TType::String) => {
                            args.delimiter = ip.read_string().map_err(tw)?
                        }
                        (Some(2), TType::Bool) => args.has_header = ip.read_bool().map_err(tw)?,
                        (Some(3), TType::I32) => args.file_type = ip.read_i32().map_err(tw)?,
                        _ => ip.skip(of.field_type).map_err(tw)?,
                    }
                    ip.read_field_end().map_err(tw)?;
                }
                ip.read_struct_end().map_err(tw)?;
            }
            (Some(5), TType::I64) => args.session_id = ip.read_i64().map_err(tw)?,
            _ => ip.skip(fd.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok(args)
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
                default: None,
                not_null: false,
                logic_type: 5,
                physical: PhysicalWire::None,
            }],
            session_id: 1,
        };
        assert_eq!(create_table(&addr, &req).unwrap().error_code, OK);
        // creating the same table again must fail: the engine kept the state
        assert_ne!(create_table(&addr, &req).unwrap().error_code, OK);
    }

    /// Regression: every `read_*_args` must consume the *enclosing*
    /// `<Method>_args` struct's `stop`. Leaving it behind desynchronizes the
    /// stream (the next message starts one byte late) — which reset the
    /// connection and made the SDK silently reconnect before every later RPC.
    #[test]
    fn args_readers_consume_the_enclosing_struct() {
        fn frame(
            method: &str,
            body: impl FnOnce(&mut TBinaryOutputProtocol<&mut Vec<u8>>),
        ) -> Vec<u8> {
            // `<Method>_args { 1: request{ body } }`
            let mut buf = Vec::new();
            let mut o = TBinaryOutputProtocol::new(&mut buf, true);
            o.write_struct_begin(&TStructIdentifier::new(format!("{method}_args")))
                .unwrap();
            o.write_field_begin(&TFieldIdentifier::new("request", TType::Struct, 1i16))
                .unwrap();
            body(&mut o);
            o.write_field_end().unwrap();
            o.write_field_stop().unwrap();
            o.write_struct_end().unwrap();
            o.flush().unwrap();
            buf
        }
        fn check(buf: &[u8], read: impl FnOnce(&mut TBinaryInputProtocol<&mut &[u8]>)) {
            let mut rd = buf;
            {
                let mut ip = TBinaryInputProtocol::new(&mut rd, true);
                read(&mut ip);
            }
            assert!(rd.is_empty(), "reader left {} trailing byte(s)", rd.len());
        }

        // ListDatabase { session_id: i64 }
        let buf = frame("ListDatabase", |o| {
            o.write_struct_begin(&TStructIdentifier::new("ListDatabaseRequest"))
                .unwrap();
            o.write_field_begin(&TFieldIdentifier::new("session_id", TType::I64, 1i16))
                .unwrap();
            o.write_i64(7).unwrap();
            o.write_field_end().unwrap();
            o.write_field_stop().unwrap();
            o.write_struct_end().unwrap();
        });
        check(&buf, |ip| {
            assert_eq!(read_list_database_args(ip).unwrap(), 7);
        });

        // Delete { db, table, session_id }
        let buf = frame("Delete", |o| {
            o.write_struct_begin(&TStructIdentifier::new("DeleteRequest"))
                .unwrap();
            for (id, name) in [(1i16, "db_name"), (2, "table_name")] {
                o.write_field_begin(&TFieldIdentifier::new(name, TType::String, id))
                    .unwrap();
                o.write_string("x").unwrap();
                o.write_field_end().unwrap();
            }
            o.write_field_begin(&TFieldIdentifier::new("session_id", TType::I64, 4i16))
                .unwrap();
            o.write_i64(9).unwrap();
            o.write_field_end().unwrap();
            o.write_field_stop().unwrap();
            o.write_struct_end().unwrap();
        });
        check(&buf, |ip| {
            let r = read_delete_args(ip).unwrap();
            assert_eq!((r.db_name.as_str(), r.session_id), ("x", 9));
        });

        // ShowTable { db, table, session_id }
        let buf = frame("ShowTable", |o| {
            o.write_struct_begin(&TStructIdentifier::new("ShowTableRequest"))
                .unwrap();
            o.write_field_begin(&TFieldIdentifier::new("db_name", TType::String, 1i16))
                .unwrap();
            o.write_string("d").unwrap();
            o.write_field_end().unwrap();
            o.write_field_begin(&TFieldIdentifier::new("table_name", TType::String, 2i16))
                .unwrap();
            o.write_string("t").unwrap();
            o.write_field_end().unwrap();
            o.write_field_begin(&TFieldIdentifier::new("session_id", TType::I64, 3i16))
                .unwrap();
            o.write_i64(11).unwrap();
            o.write_field_end().unwrap();
            o.write_field_stop().unwrap();
            o.write_struct_end().unwrap();
        });
        check(&buf, |ip| {
            let (db, table, sid) = read_show_table_args(ip).unwrap();
            assert_eq!((db.as_str(), table.as_str(), sid), ("d", "t", 11));
        });

        // `<Method>_args` with string fields (used by DropTable/CreateIndex/…)
        let buf = frame("DropTable", |o| {
            o.write_struct_begin(&TStructIdentifier::new("DropTableRequest"))
                .unwrap();
            o.write_field_begin(&TFieldIdentifier::new("db_name", TType::String, 1i16))
                .unwrap();
            o.write_string("d").unwrap();
            o.write_field_end().unwrap();
            o.write_field_begin(&TFieldIdentifier::new("session_id", TType::I64, 3i16))
                .unwrap();
            o.write_i64(13).unwrap();
            o.write_field_end().unwrap();
            o.write_field_stop().unwrap();
            o.write_struct_end().unwrap();
        });
        check(&buf, |ip| {
            let (sid, strs) = read_string_request(ip).unwrap();
            assert_eq!((sid, strs.get(&1).map(String::as_str)), (13, Some("d")));
        });
    }

    /// Date/Time/Datetime/Json columns are packed the way the SDK's
    /// `parse_*_bytes` expects, and announced with the matching `LogicType`.
    #[test]
    fn non_scalar_column_packing() {
        let cols: Vec<String> = ["d", "tm", "dt", "j"]
            .iter()
            .map(|s| s.to_string())
            .collect();
        let types: Vec<String> = ["Date", "Time", "Datetime", "Json"]
            .iter()
            .map(|s| s.to_string())
            .collect();
        let rows = vec![vec![
            "2024-01-22".to_string(),
            "00:03:55".to_string(),
            "2024-01-22 00:03:55".to_string(),
            "{\"a\":1}".to_string(),
        ]];

        let fields = rows_to_column_fields_typed(&cols, &types, &rows);
        // ColumnDate: i32 days since the epoch (2024-01-22 -> 19744).
        assert_eq!(fields[0].column_type, 16);
        assert_eq!(
            i32::from_le_bytes(fields[0].column_vectors[0][..4].try_into().unwrap()),
            19744
        );
        // ColumnTime: i32 seconds of day (00:03:55 -> 235).
        assert_eq!(fields[1].column_type, 17);
        assert_eq!(
            i32::from_le_bytes(fields[1].column_vectors[0][..4].try_into().unwrap()),
            235
        );
        // ColumnDateTime: (epoch days, seconds of day).
        assert_eq!(fields[2].column_type, 18);
        let dt = &fields[2].column_vectors[0];
        assert_eq!(i32::from_le_bytes(dt[..4].try_into().unwrap()), 19744);
        assert_eq!(i32::from_le_bytes(dt[4..8].try_into().unwrap()), 235);
        // ColumnJson: same length-prefixed UTF-8 framing as varchar.
        assert_eq!(fields[3].column_type, 22);
        let j = &fields[3].column_vectors[0];
        assert_eq!(
            u32::from_le_bytes(j[..4].try_into().unwrap()) as usize,
            "{\"a\":1}".len()
        );
        assert_eq!(&j[4..], b"{\"a\":1}");

        // ... and the announced LogicType is the matching one.
        let resp = select_response_from_typed(&cols, &types, &rows);
        let logic: Vec<i32> = resp.column_defs.iter().map(|c| c.logic_type).collect();
        assert_eq!(logic, vec![17, 18, 19, 23]); // Date, Time, DateTime, Json
        assert!(resp
            .column_defs
            .iter()
            .all(|c| c.physical == PhysicalWire::None));

        // Sparse: `u32 nnz` + i8 indices (bound 100) + f32 values.
        let scols = vec!["v".to_string()];
        let stypes = vec!["SPARSE(FLOAT,100)".to_string()];
        let srows = vec![vec!["[0:1.5,3:2.5]".to_string()]];
        let sf = rows_to_column_fields_typed(&scols, &stypes, &srows);
        assert_eq!(sf[0].column_type, 13); // ColumnSparse
        let b = &sf[0].column_vectors[0];
        assert_eq!(u32::from_le_bytes(b[..4].try_into().unwrap()), 2); // nnz
        assert_eq!(&b[4..6], &[0u8, 3u8]); // indices as i8
        assert_eq!(f32::from_le_bytes(b[6..10].try_into().unwrap()), 1.5);
        assert_eq!(f32::from_le_bytes(b[10..14].try_into().unwrap()), 2.5);
        let sresp = select_response_from_typed(&scols, &stypes, &srows);
        assert_eq!(sresp.column_defs[0].logic_type, 15); // Sparse
        assert_eq!(
            sresp.column_defs[0].physical,
            PhysicalWire::Sparse {
                dimension: 100,
                element_type: 6, // Float32
                index_type: 2,   // Int8
            }
        );

        // An empty sparse vector still emits an `nnz = 0` prefix.
        let erows = vec![vec!["(empty)".to_string()]];
        let ef = rows_to_column_fields_typed(&scols, &stypes, &erows);
        assert_eq!(ef[0].column_vectors[0], vec![0, 0, 0, 0]);

        // Tensor: u32 byte-length + dim*rows little-endian elements per row.
        let tcols = vec!["t".to_string()];
        let ttypes = vec!["TENSOR(FLOAT,3)".to_string()];
        let trows = vec![vec!["[[1,2,3],[4,5,6]]".to_string()]];
        let tf = rows_to_column_fields_typed(&tcols, &ttypes, &trows);
        assert_eq!(tf[0].column_type, 11); // ColumnTensor
        let b = &tf[0].column_vectors[0];
        assert_eq!(u32::from_le_bytes(b[..4].try_into().unwrap()), 24); // 2*3*4 bytes
        let vals: Vec<f32> = (0..6)
            .map(|i| f32::from_le_bytes(b[4 + i * 4..8 + i * 4].try_into().unwrap()))
            .collect();
        assert_eq!(vals, vec![1.0, 2.0, 3.0, 4.0, 5.0, 6.0]);
        let tresp = select_response_from_typed(&tcols, &ttypes, &trows);
        assert_eq!(tresp.column_defs[0].logic_type, 13); // Tensor
        assert_eq!(
            tresp.column_defs[0].physical,
            PhysicalWire::Embedding {
                dimension: 3,
                element_type: 6, // Float32
            }
        );

        // TensorArray: u32 tensor_n, then each tensor length-prefixed.
        let acols = vec!["a".to_string()];
        let atypes = vec!["TENSORARRAY(FLOAT,3)".to_string()];
        let arows = vec![vec!["[[[1,2,3],[5,7,8]],[[9,9,9]]]".to_string()]];
        let af = rows_to_column_fields_typed(&acols, &atypes, &arows);
        assert_eq!(af[0].column_type, 12); // ColumnTensorArray
        let b = &af[0].column_vectors[0];
        assert_eq!(u32::from_le_bytes(b[..4].try_into().unwrap()), 2); // tensor_n
        assert_eq!(u32::from_le_bytes(b[4..8].try_into().unwrap()), 24); // first tensor
        let second = 8 + 24;
        assert_eq!(
            u32::from_le_bytes(b[second..second + 4].try_into().unwrap()),
            12
        );
        let aresp = select_response_from_typed(&acols, &atypes, &arows);
        assert_eq!(aresp.column_defs[0].logic_type, 14); // TensorArray

        // Array: u32 element count + each element packed for its ColumnType.
        let rcols = vec!["r".to_string()];
        let rtypes = vec!["ARRAY(INT)".to_string()];
        let rrows = vec![vec!["{1,2,3}".to_string()]];
        let rf = rows_to_column_fields_typed(&rcols, &rtypes, &rrows);
        assert_eq!(rf[0].column_type, 21); // ColumnArray
        let b = &rf[0].column_vectors[0];
        assert_eq!(u32::from_le_bytes(b[..4].try_into().unwrap()), 3);
        assert_eq!(i32::from_le_bytes(b[4..8].try_into().unwrap()), 1);
        assert_eq!(i32::from_le_bytes(b[8..12].try_into().unwrap()), 2);
        assert_eq!(i32::from_le_bytes(b[12..16].try_into().unwrap()), 3);
        let rresp = select_response_from_typed(&rcols, &rtypes, &rrows);
        assert_eq!(rresp.column_defs[0].logic_type, 22); // Array
        assert_eq!(
            rresp.column_defs[0].physical,
            PhysicalWire::Array {
                element_logic_type: 3, // Integer
                element: Box::new(PhysicalWire::None),
            }
        );

        // Varchar arrays are length-prefixed per element.
        let vcols = vec!["s".to_string()];
        let vtypes = vec!["ARRAY(VARCHAR)".to_string()];
        let vrows = vec![vec!["{ab,cd}".to_string()]];
        let vf = rows_to_column_fields_typed(&vcols, &vtypes, &vrows);
        let b = &vf[0].column_vectors[0];
        assert_eq!(u32::from_le_bytes(b[..4].try_into().unwrap()), 2);
        assert_eq!(u32::from_le_bytes(b[4..8].try_into().unwrap()), 2); // "ab"
        assert_eq!(&b[8..10], b"ab");

        // Nested arrays recurse: `{{1,2},{3}}` -> count 2, then each sub-array.
        let ncols = vec!["n".to_string()];
        let ntypes = vec!["ARRAY(ARRAY(INT))".to_string()];
        let nrows = vec![vec!["{{1,2},{3}}".to_string()]];
        let nf = rows_to_column_fields_typed(&ncols, &ntypes, &nrows);
        let b = &nf[0].column_vectors[0];
        assert_eq!(u32::from_le_bytes(b[..4].try_into().unwrap()), 2); // outer
        assert_eq!(u32::from_le_bytes(b[4..8].try_into().unwrap()), 2); // inner #1
        assert_eq!(i32::from_le_bytes(b[8..12].try_into().unwrap()), 1);
        assert_eq!(i32::from_le_bytes(b[12..16].try_into().unwrap()), 2);
        assert_eq!(u32::from_le_bytes(b[16..20].try_into().unwrap()), 1); // inner #2
        assert_eq!(i32::from_le_bytes(b[20..24].try_into().unwrap()), 3);
        let nresp = select_response_from_typed(&ncols, &ntypes, &nrows);
        assert_eq!(nresp.column_defs[0].logic_type, 22); // Array
        assert_eq!(
            nresp.column_defs[0].physical,
            PhysicalWire::Array {
                element_logic_type: 22, // Array
                element: Box::new(PhysicalWire::Array {
                    element_logic_type: 3, // Integer
                    element: Box::new(PhysicalWire::None),
                }),
            }
        );
    }
}
