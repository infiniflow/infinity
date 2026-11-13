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

pub mod http;
pub mod peer;
pub mod pg;
pub mod ring_buffer;
pub mod service;

// The async service generated from `thrift/infinity.thrift` (plan §6.3.2).
include!(concat!(env!("OUT_DIR"), "/volo_gen.rs"));

use std::collections::HashMap;
use std::io::{Read, Write};
use std::net::{TcpListener, TcpStream};
use std::sync::atomic::{AtomicI64, Ordering};
use std::sync::{Arc, RwLock};

use common::{Error, Result};

use crate::peer::PeerManager;
use executor::{Batch, ColumnData, Engine, QueryResult, Value};

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

/// `ShowCurrentNodeResponse` (the node role and status of this server).
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct ShowCurrentNodeResponse {
    pub error_code: i64,
    pub error_msg: String,
    pub node_role: String,
    pub server_status: String,
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
    sessions: Arc<RwLock<HashMap<i64, i64>>>,
    next_session: Arc<AtomicI64>,
    engine: Option<Arc<Engine>>,
    /// The cluster state: this node's `NodeInfo`, the peer registry, the peer
    /// task queue and the event history ring. Shared with every connection.
    peer: Arc<PeerManager>,
}

impl Server {
    pub fn bind(addr: &str) -> Result<Self> {
        let listener = TcpListener::bind(addr).map_err(|e| Error::io(addr, e))?;
        let bound = listener.local_addr().map_err(|e| Error::io(addr, e))?;
        Ok(Self {
            listener,
            sessions: Arc::new(RwLock::new(HashMap::new())),
            next_session: Arc::new(AtomicI64::new(1)),
            engine: None,
            // A standalone server is a leader of an empty cluster; its peer
            // endpoint is the address it listens on.
            peer: Arc::new(PeerManager::new(
                "leader",
                bound.ip().to_string(),
                i64::from(bound.port()),
            )),
        })
    }

    /// Binds a server that serves DDL/DML/DQL against the given engine.
    pub fn bind_with_engine(addr: &str, engine: Arc<Engine>) -> Result<Self> {
        let mut s = Self::bind(addr)?;
        s.engine = Some(engine);
        Ok(s)
    }

    /// The peer/cluster manager shared with every connection. Mutating node
    /// state through it (`register_peer`, `heartbeat_peer`, `change_role`) is
    /// reflected by `SHOW CURRENT NODE`.
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

    /// Serves Thrift on the **caller's** runtime, until it is shut down.
    ///
    /// `volo-thrift` runs one task per connection, so an idle client costs a
    /// task, not a thread; the statement itself is handed to the engine's
    /// scheduler by the handler. This used to build a runtime of its own (one
    /// more pool of `cores` threads beside the HTTP, PG and engine runtimes);
    /// now the front end is a task on whichever runtime spawned it, and the
    /// caller decides when the process ends.
    pub async fn serve(self) {
        use volo::net::incoming::DefaultIncoming;
        use volo_thrift::codec::default::thrift::MakeThriftCodec;
        use volo_thrift::codec::default::DefaultMakeCodec;

        self.listener
            .set_nonblocking(true)
            .expect("set the thrift listener non-blocking");
        let listener =
            tokio::net::TcpListener::from_std(self.listener).expect("adopt the thrift listener");
        let service =
            service::Service::new(self.engine, self.sessions, self.next_session, self.peer);
        // Unframed `TBinaryProtocol`, which is what the python/go SDKs speak.
        let server = volo_gen::infinity::InfinityServiceServer::new(service)
            .make_codec(DefaultMakeCodec::new(MakeThriftCodec::new()));
        if let Err(e) = server.run(DefaultIncoming::from(listener)).await {
            eprintln!("[THRIFT] server stopped: {e}");
        }
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

/// What one RPC produced, before it becomes a reply.
///
/// The Thrift front end decodes the request, hands the argument bytes to
/// [`dispatch_arm`] and turns the returned value into the generated reply
/// struct (plan §6.3). The same value feeds the byte-parity harnesses in
/// `src/bin`, which is why the arms do not write replies themselves.
pub(crate) enum ArmResponse {
    Common(CommonResponse),
    Select(SelectResponseWire),
    Delete {
        error_code: i64,
        error_msg: String,
        deleted_rows: i64,
    },
    /// `ListDatabase` / `ListTable` / `ListIndex` — the caller knows which
    /// list field of the reply carries the names.
    List(Vec<String>),
    ShowTable {
        db: String,
        table: String,
        error_code: i64,
        error_msg: String,
        column_count: i64,
        segment_count: i64,
        row_count: i64,
    },
    ShowIndex {
        db: String,
        table: String,
        index: String,
        comment: String,
        error_code: i64,
        error_msg: String,
        index_type: String,
        columns: String,
    },
    ShowDatabase {
        name: String,
        table_count: i64,
        comment: String,
    },
    ShowCurrentNode {
        node_role: String,
        server_status: String,
    },
    ShowSnapshot {
        name: String,
        found: bool,
    },
    ListSnapshots(Vec<String>),
    ShowConfig {
        name: String,
        value: Option<String>,
    },
}

/// Runs one RPC against the engine.
///
/// `reader` is the request as the wire carries it: the caller re-encodes the
/// struct it just decoded, so the SQL rendering below (filters, search
/// expressions, literals) has exactly one implementation for both the async
/// front end and the harnesses.
#[allow(clippy::too_many_lines)]
async fn dispatch_arm(
    name: &str,
    mut reader: &[u8],
    engine: Option<Arc<Engine>>,
    sessions: &RwLock<HashMap<i64, i64>>,
    next_session: &AtomicI64,
    peer: &PeerManager,
    // Kept for symmetry with the session-scoped call site: the scope itself is
    // entered by the handler (`with_session_async`), which is the only place
    // that can wrap the awaited future.
    _sid: i64,
) -> Result<ArmResponse> {
    match name {
        "Connect" => {
            let cv = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_connect_args(&mut ip)?
            };
            let sid = match engine.as_ref() {
                // The engine owns the session's current database, so
                // two clients that USE different databases do not
                // share a choice (plan 6.1). Every later RPC carries
                // the id back and runs under that session.
                Some(eng) => eng.open_session("default_db"),
                None => next_session.fetch_add(1, Ordering::SeqCst),
            };
            let resp = handshake(cv, sid);
            if resp.error_code == OK {
                sessions.write().unwrap().insert(sid, cv);
            }
            Ok(ArmResponse::Common(resp))
        }
        "Disconnect" => {
            let sid = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_common_args(&mut ip)?
            };
            sessions.write().unwrap().remove(&sid);
            if let Some(eng) = engine.as_ref() {
                // A session that is already gone still answers OK: the
                // reference's Disconnect is idempotent.
                eng.close_session(sid);
            }
            let resp = CommonResponse {
                error_code: OK,
                error_msg: "OK".into(),
                session_id: sid,
            };
            Ok(ArmResponse::Common(resp))
        }
        "CreateTable" => {
            let req = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_create_table_args(&mut ip)?
            };
            let resp = run_create_table(engine.as_deref(), &req).await;
            Ok(ArmResponse::Common(resp))
        }
        "ListDatabase" => {
            let _sid = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_list_database_args(&mut ip)?
            };
            // Kept on the worker on purpose, unlike the catalog sweeps below.
            // The list is a prefix scan over a handful of `db:` records (~30 us
            // measured), while a `spawn_blocking` round trip costs 10-20 us:
            // handing this one off cut throughput by ~9% on four cores and
            // bought nothing back. It is also the call a client makes to prove
            // it is alive (10 000 sessions poll it twice a second), which is
            // exactly the load that must stay cheap.
            let names = match &engine {
                Some(eng) => eng.catalog().list_databases().unwrap_or_default(),
                None => vec!["default_db".to_string()],
            };
            Ok(ArmResponse::List(names))
        }
        "Delete" => {
            let req = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_delete_args(&mut ip)?
            };
            let (code, msg, deleted) = match &engine {
                Some(eng) => match eng.execute(&req.to_sql()).await {
                    Ok(rs) => {
                        let deleted = rs
                            .into_iter()
                            .map(|r| {
                                if let QueryResult::Deleted { count } = r {
                                    count as i64
                                } else {
                                    0
                                }
                            })
                            .sum::<i64>();
                        (OK, "OK".to_string(), deleted.max(0))
                    }
                    // A missing table is a request error, not `0 rows`.
                    Err(e) => (e.status_code(), e.to_string(), 0),
                },
                None => (1, "no engine bound".to_string(), 0),
            };
            Ok(ArmResponse::Delete {
                error_code: code,
                error_msg: msg,
                deleted_rows: deleted,
            })
        }
        "Update" => {
            let req = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_update_args(&mut ip)?
            };
            let resp = if req.where_sql.as_deref() == Some(NON_BOOLEAN_FILTER) {
                CommonResponse {
                    error_code: 7012,
                    error_msg: "parser: the filter must be a boolean expression".into(),
                    session_id: req.session_id,
                }
            } else {
                match &engine {
                    Some(eng) => match eng.execute(&req.to_sql()).await {
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
                }
            };
            Ok(ArmResponse::Common(resp))
        }
        "ShowTable" => {
            let (db, table, _sid) = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_show_table_args(&mut ip)?
            };
            let full = full_table_name(&db, &table);
            // Same reason as `ListDatabase`: a catalog read plus a row count is
            // store I/O.
            let found = match engine {
                Some(eng) => {
                    let full = full.clone();
                    executor::scheduler()
                        .spawn_blocking(move || {
                            eng.catalog()
                                .get_table_by_name(&full)
                                .ok()
                                .flatten()
                                .map(|m| {
                                    let rows = eng.row_count(&full).unwrap_or(0) as i64;
                                    let segments = if rows == 0 {
                                        0
                                    } else {
                                        eng.segment_count(&full) as i64
                                    };
                                    (m, rows, segments)
                                })
                        })
                        .await
                }
                None => None,
            };
            let (code, msg, col_count, row_count, segments) = match found {
                Some((meta, rows, segments)) => (
                    0i64,
                    "OK".to_string(),
                    meta.columns.len() as i64,
                    rows,
                    segments,
                ),
                None => (3022i64, format!("Table: {table} doesn't exist."), 0, 0, 0),
            };
            Ok(ArmResponse::ShowTable {
                db,
                table,
                error_code: code,
                error_msg: msg,
                column_count: col_count,
                segment_count: segments,
                row_count,
            })
        }
        "ShowIndex" => {
            let (db, table, index_name, _sid) = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_show_index_args(&mut ip)?
            };
            let full = full_table_name(&db, &table);
            // Same reason as `ListDatabase`: these are catalog reads.
            let probe = full.clone();
            let index_name_owned = index_name.clone();
            let found = match engine {
                Some(eng) => {
                    executor::scheduler()
                        .spawn_blocking(move || {
                            let im = eng
                                .catalog()
                                .indexes_of_table(&probe)
                                .ok()?
                                .into_iter()
                                .find(|im| im.name.eq_ignore_ascii_case(&index_name_owned))?;
                            let comment = eng.index_comment(&probe, &im.name).unwrap_or_default();
                            Some((im, comment))
                        })
                        .await
                }
                None => None,
            };
            let (code, msg, name, kind, cols, comment) = match found {
                Some((im, comment)) => (
                    0i64,
                    "OK".to_string(),
                    im.name,
                    im.index_type,
                    im.column,
                    comment,
                ),
                None => (
                    3023i64,
                    format!("index '{index_name}' does not exist"),
                    index_name.clone(),
                    String::new(),
                    String::new(),
                    String::new(),
                ),
            };
            Ok(ArmResponse::ShowIndex {
                db,
                table,
                index: name,
                comment,
                error_code: code,
                error_msg: msg,
                index_type: kind,
                columns: cols,
            })
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
            // Identifiers are case-insensitive; the engine keeps
            // lower-case names, so existence checks must use them too.
            let qualify = |t: &str| -> String {
                let t = t.to_ascii_lowercase();
                if t.is_empty() || t.contains('.') || db.is_empty() {
                    t
                } else {
                    format!("{}.{t}", db.to_ascii_lowercase())
                }
            };
            // `ConflictType`: 0 = Ignore, 1 = Error, 2 = Replace. The option
            // sits in whichever field the request struct reserves for it
            // (3 for the database ops, 4 for table/index ops), so scan them.
            let conflict: Option<i64> = [3i16, 4, 5]
                .iter()
                .find_map(|id| fields.get(id).and_then(|v| v.parse::<i64>().ok()));
            // A database name must start with a letter and contain only
            // word characters (the reference rejects the rest with 3003).
            let mut chars = db.chars();
            let first_ok = chars
                .next()
                .map(|c| c.is_ascii_alphabetic())
                .unwrap_or(false);
            let body_ok = db.chars().all(|c| c.is_ascii_alphanumeric() || c == '_');
            let good_db = first_ok && body_ok;
            let bad_db = !db.is_empty() && !good_db;
            let resp = match name {
                "CreateDatabase" | "DropDatabase" if bad_db || db.is_empty() => {
                    CommonResponse {
                        error_code: 3003, // INVALID_IDENTIFIER_NAME
                        error_msg: format!("invalid database name '{db}'"),
                        session_id: sid,
                    }
                }
                "CreateDatabase" if !db.is_empty() => {
                    let sql = if conflict == Some(0) {
                        format!("CREATE DATABASE IF NOT EXISTS {db}")
                    } else {
                        format!("CREATE DATABASE {db}")
                    };
                    run_common(engine.as_ref(), &sql, sid).await
                }
                "GetDatabase" if !db.is_empty() => {
                    let exists = engine
                        .as_ref()
                        .and_then(|eng| eng.catalog().list_databases().ok())
                        .map(|dbs| dbs.iter().any(|d| d.eq_ignore_ascii_case(&db)))
                        .unwrap_or(false);
                    if exists {
                        CommonResponse {
                            error_code: OK,
                            error_msg: "OK".into(),
                            session_id: sid,
                        }
                    } else {
                        CommonResponse {
                            error_code: 3021, // DB_NOT_EXIST
                            error_msg: format!("Database: {db} doesn't exist."),
                            session_id: sid,
                        }
                    }
                }
                "DropDatabase" if !db.is_empty() => {
                    let sql = if conflict == Some(0) {
                        format!("DROP DATABASE IF EXISTS {db}")
                    } else {
                        format!("DROP DATABASE {db}")
                    };
                    run_common(engine.as_ref(), &sql, sid).await
                }
                "GetTable" if !name2.is_empty() => {
                    let full = qualify(&name2);
                    let exists = engine
                        .as_ref()
                        .and_then(|e| e.catalog().get_table_by_name(&full).ok().flatten())
                        .is_some();
                    if exists {
                        CommonResponse {
                            error_code: OK,
                            error_msg: "OK".into(),
                            session_id: sid,
                        }
                    } else {
                        CommonResponse {
                            error_code: 3022, // TABLE_NOT_EXIST
                            error_msg: format!("Table: {name2} doesn't exist."),
                            session_id: sid,
                        }
                    }
                }
                "DropTable" if !name2.is_empty() => {
                    let full = qualify(&name2);
                    let if_exists = conflict == Some(0);
                    // Straight to the executor: the statement is already fully
                    // described here, and formatting it back into SQL only to
                    // parse it again cost every drop a parse plus a plan.
                    run_drop_table(engine.as_ref(), &full, if_exists, sid).await
                }
                "DropIndex" if !name3.is_empty() && conflict == Some(0) => {
                    run_common(
                        engine.as_ref(),
                        &format!("DROP INDEX IF EXISTS {}", name3.to_ascii_lowercase()),
                        sid,
                    )
                    .await
                }
                "DropIndex" if !name3.is_empty() => {
                    run_common(
                        engine.as_ref(),
                        &format!("DROP INDEX {}", name3.to_ascii_lowercase()),
                        sid,
                    )
                    .await
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
            Ok(ArmResponse::Common(resp))
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
            // The reference rejects a single INSERT batch above one
            // block's worth of rows (8192).
            if req.rows.is_empty() {
                resp = CommonResponse {
                    error_code: 3014,
                    error_msg: "insert without values".into(),
                    session_id: sid,
                };
            } else if req.rows.len() > 8192 {
                resp = CommonResponse {
                    error_code: 3014,
                    error_msg: "Insert batch row limit shouldn't more than 8193".into(),
                    session_id: sid,
                };
            }
            // A scalar batch goes straight to the executor: rendering
            // 1000 rows to SQL and re-parsing them cost ~13 ms per
            // batch, more than the insert itself. Values that need the
            // parser (arrays, tensors, sparse vectors) keep the SQL path.
            let direct = req.rows.iter().all(|(_, vals)| {
                !vals.is_empty()
                    && vals.iter().all(|v| {
                        matches!(
                            v,
                            ValueLit::Int(_)
                                | ValueLit::Float(_)
                                | ValueLit::Str(_)
                                | ValueLit::Null
                                | ValueLit::Bool(_)
                                | ValueLit::IntArray(_)
                                | ValueLit::FloatArray(_)
                                | ValueLit::IntTensor(_)
                                | ValueLit::FloatTensor(_)
                                | ValueLit::Sparse { .. }
                        )
                    })
            });
            if direct {
                let mut i = 0usize;
                while resp.error_code == OK && i < req.rows.len() {
                    let cols = req.rows[i].0.clone();
                    let mut rows_lit: Vec<Vec<parser::Literal>> = Vec::new();
                    while i < req.rows.len() && req.rows[i].0 == cols {
                        rows_lit.push(
                            req.rows[i]
                                .1
                                .iter()
                                .map(|v| match v {
                                    ValueLit::Int(n) => parser::Literal::Int(*n),
                                    ValueLit::Float(f) => parser::Literal::Float(*f),
                                    ValueLit::Str(t) => parser::Literal::Str(t.clone()),
                                    ValueLit::Null => parser::Literal::Null,
                                    ValueLit::Bool(b) => parser::Literal::Str(b.to_string()),
                                    ValueLit::IntArray(v) => parser::Literal::Vector(
                                        v.iter().map(|x| *x as f64).collect(),
                                    ),
                                    ValueLit::FloatArray(v) => parser::Literal::Vector(v.clone()),
                                    ValueLit::IntTensor(rows) => parser::Literal::Vector(
                                        rows.iter().flatten().map(|x| *x as f64).collect(),
                                    ),
                                    ValueLit::FloatTensor(rows) => parser::Literal::Vector(
                                        rows.iter().flatten().copied().collect(),
                                    ),
                                    ValueLit::Sparse {
                                        indices,
                                        ints,
                                        floats,
                                        is_float,
                                    } => {
                                        let items: Vec<String> = indices
                                            .iter()
                                            .enumerate()
                                            .map(|(k, i)| {
                                                if *is_float {
                                                    format!("{i}:{}", floats[k])
                                                } else {
                                                    format!("{i}:{}", ints[k])
                                                }
                                            })
                                            .collect();
                                        parser::Literal::Array(items)
                                    }
                                    _ => parser::Literal::Null,
                                })
                                .collect(),
                        );
                        i += 1;
                    }
                    let cols_opt = if cols.is_empty() {
                        None
                    } else {
                        Some(cols.as_slice())
                    };
                    // `insert_literals_inner` is awaited **on this task**: not
                    // bridged with `block_on`, and not wrapped in
                    // `spawn_blocking`. The statement is a mixed workload - it
                    // awaits the WAL, the row allocator and the bottom half,
                    // and it encodes rows and writes index entries - so the
                    // blocking pool would put its waits and its compute in one
                    // concurrency budget (plan §6.6), and `block_on` would park
                    // this worker on a future only other workers can finish.
                    // Awaiting keeps the whole statement a future.
                    let cols_owned: Option<Vec<String>> = cols_opt.map(|c| c.to_vec());
                    resp = match engine.as_ref() {
                        Some(eng) => match eng
                            .insert_literals_inner(&table, cols_owned.as_deref(), rows_lit, false)
                            .await
                        {
                            Ok(_) => CommonResponse {
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
                }
            }
            let mut i = 0usize;
            while !direct && resp.error_code == OK && i < req.rows.len() {
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
                if std::env::var_os("INF_DEBUG_SQL").is_some() {
                    eprintln!("[INSERT] {sql}");
                }
                let t0 = std::time::Instant::now();
                resp = run_common(engine.as_ref(), &sql, sid).await;
                if std::env::var_os("INF_DEBUG_INS").is_some() {
                    eprintln!(
                        "[INS] rpc rows={} sql={}B exec={:?}",
                        batch.len(),
                        sql.len(),
                        t0.elapsed()
                    );
                }
                if resp.error_code != OK {
                    break;
                }
            }
            Ok(ArmResponse::Common(resp))
        }
        "Select" => {
            let rpc_started = std::time::Instant::now();
            let req = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                match read_select_args(&mut ip) {
                    Ok((_r, bad)) if !bad.is_empty() => {
                        let resp = SelectResponseWire {
                            error_code: 7012,
                            error_msg: bad[0].clone(),
                            column_defs: Vec::new(),
                            column_fields: Vec::new(),
                            extra_result: String::new(),
                        };
                        return Ok(ArmResponse::Select(resp));
                    }
                    Ok((r, _)) => r,
                    // A malformed select list is reported through the
                    // SelectResponse (the SDK turns error_code into an
                    // InfinityException), not a thrift exception.
                    Err(e) => {
                        let resp = SelectResponseWire {
                            error_code: e.status_code(),
                            error_msg: e.to_string(),
                            column_defs: Vec::new(),
                            column_fields: Vec::new(),
                            extra_result: String::new(),
                        };
                        // The request was fully consumed: reply with the
                        // validation error and keep the connection alive
                        // (closing here makes SDKs mark it dead even
                        // though the error was delivered).
                        return Ok(ArmResponse::Select(resp));
                    }
                }
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
            let exec_started = std::time::Instant::now();
            let srv_started = exec_started;
            // Plain projection over one table with no other clause: the
            // engine hands back typed column buffers and the writer packs
            // them without a `Value` per cell.
            let batch_resp = match &engine {
                Some(eng) if !req.total_hits => eng
                    .select_columns(&req.to_sql())
                    .ok()
                    .flatten()
                    .map(|batch| {
                        let mut columns = batch.columns.clone();
                        if !req.star && req.names.len() == columns.len() {
                            columns = req.names.clone();
                        } else if req.star
                            && !req.names.is_empty()
                            && columns.len() >= req.names.len()
                        {
                            let start = columns.len() - req.names.len();
                            for (i, n) in req.names.iter().enumerate() {
                                columns[start + i] = n.clone();
                            }
                        }
                        let types = declared_types(eng, &req.table_name, &columns);
                        select_response_from_batch(&columns, &types, &batch)
                    }),
                _ => None,
            };
            let resp = match batch_resp {
                Some(resp) => resp,
                None => match &engine {
                    Some(eng) => match eng.execute(&req.to_sql()).await {
                        Ok(rs) => {
                            let mut columns: Vec<String> = Vec::new();
                            let mut rows: Vec<Vec<Value>> = Vec::new();
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
                            if std::env::var_os("INF_DEBUG_SQL").is_some() {
                                eprintln!(
                                    "[COLS] star={} req={:?} got={:?}",
                                    req.star, req.columns, columns
                                );
                            }
                            if std::env::var_os("INF_DEBUG_SQL").is_some() {
                                eprintln!(
                                    "[COLS] star={} req={:?} got={:?}",
                                    req.star, req.columns, columns
                                );
                            }
                            // A projected expression is named by the SQL
                            // text the client sent (`(c1 + c2)`).
                            if !req.star && req.names.len() == columns.len() {
                                columns = req.names.clone();
                            } else if req.star
                                && !req.names.is_empty()
                                && columns.len() >= req.names.len()
                            {
                                // `SELECT *, distance()`: only the trailing
                                // expression columns are renamed.
                                let start = columns.len() - req.names.len();
                                for (i, n) in req.names.iter().enumerate() {
                                    columns[start + i] = n.clone();
                                }
                            }
                            let exec_elapsed = exec_started.elapsed();
                            let encode_started = std::time::Instant::now();
                            let mut resp = select_response_from_typed(
                                &columns,
                                &declared_types(eng, &req.table_name, &columns),
                                &rows,
                            );
                            if std::env::var_os("INF_DEBUG_SEL").is_some() {
                                eprintln!(
                                    "[SEL] rows={} exec={:?} encode={:?}",
                                    rows.len(),
                                    exec_elapsed,
                                    encode_started.elapsed()
                                );
                            }
                            if req.total_hits {
                                // The count is the result set *before*
                                // LIMIT/OFFSET.
                                let mut all = req.clone();
                                all.limit = None;
                                all.offset = None;
                                let total = eng
                                    .execute(&all.to_sql())
                                    .await
                                    .map(|rs| {
                                        rs.into_iter()
                                            .map(|r| {
                                                if let QueryResult::Rows { rows, .. } = r {
                                                    rows.len()
                                                } else {
                                                    0
                                                }
                                            })
                                            .sum::<usize>()
                                    })
                                    .unwrap_or(0);
                                resp.extra_result = format!("{{\"total_hits_count\":{total}}}");
                            }
                            resp
                        }
                        // Keep the engine's C++ error code (3023, 3053, …)
                        // so SDK assertions on `args[0]` hold.
                        Err(e) => SelectResponseWire {
                            error_code: e.status_code(),
                            error_msg: e.to_string(),
                            column_defs: Vec::new(),
                            column_fields: Vec::new(),
                            extra_result: String::new(),
                        },
                    },
                    None => err("no engine bound".into()),
                },
            };
            if std::env::var_os("INF_SERVER_TIMER").is_some() {
                let ms = |d: std::time::Duration| d.as_secs_f64() * 1000.0;
                eprintln!(
                    "[SRV] plan_ms {:.3} search_ms {:.3} ser_ms 0.000 total_ms {:.3}",
                    ms(srv_started.duration_since(rpc_started)),
                    ms(srv_started.elapsed()),
                    ms(rpc_started.elapsed())
                );
            }
            Ok(ArmResponse::Select(resp))
        }
        // Lifecycle / maintenance RPCs returning a plain `CommonResponse`.
        // `Flush` persists table data into the C++ block layout and the
        // indexes into their chunk files, which is what the restart
        // harness inspects on disk.
        // `Flush` persists table data into the C++ block layout and the
        // indexes into their chunk files, which is what the restart
        // harness inspects on disk. `Cleanup` removes the mirrors of
        // dropped tables/indexes, so it flushes too.
        "Flush" | "Cleanup" => {
            let (sid, _) = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_string_request(&mut ip)?
            };
            let resp = match engine.as_ref() {
                Some(eng) => match if name == "Cleanup" {
                    eng.cleanup().await
                } else {
                    eng.flush().await
                } {
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
            Ok(ArmResponse::Common(resp))
        }
        // `Compact` merges the table's segments into one.
        "Compact" => {
            let (sid, fields) = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_string_request(&mut ip)?
            };
            let (db, table) = (
                fields.get(&2).cloned().unwrap_or_default(),
                fields.get(&3).cloned().unwrap_or_default(),
            );
            let full = full_table_name(&db, &table);
            // Await the async half: `compact_table` bridges it with
            // `block_on`, which parks this worker for the whole merge. Same
            // reason as `insert_literals_inner` above.
            let resp = match engine.as_ref() {
                Some(eng) => match eng.compact_table_async(&full).await {
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
            Ok(ArmResponse::Common(resp))
        }
        // Database snapshots are stored as one blob per database; a
        // `RestoreSnapshot` with `scope="database"` restores it (the
        // table scope falls through to the SQL statement).
        "CreateDatabaseSnapshot" | "CreateSystemSnapshot" | "RestoreSnapshot" => {
            let (sid, fields) = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_string_request(&mut ip)?
            };
            let get = |id: i16| fields.get(&id).cloned().unwrap_or_default();
            let resp = match engine.as_ref() {
                Some(eng) => {
                    let res = if name == "CreateDatabaseSnapshot" {
                        // `{2: db_name, 3: snapshot_name}`
                        eng.create_database_snapshot(&get(3), &get(2)).await
                    } else if name == "CreateSystemSnapshot" {
                        // `{2: snapshot_name}`
                        eng.create_system_snapshot(&get(2)).await
                    } else if get(3).eq_ignore_ascii_case("system") {
                        // `{2: snapshot_name, 3: scope}`: a system
                        // snapshot nests its tables under each database,
                        // so it has to go through the system restore - a
                        // database restore reads the top-level
                        // `table_snapshots`, which a system snapshot
                        // does not have.
                        eng.restore_system_snapshot(&get(2)).await
                    } else if get(3).eq_ignore_ascii_case("database") {
                        eng.restore_database_snapshot(&get(2)).await
                    } else {
                        eng.execute(&format!("RESTORE TABLE SNAPSHOT {};", sql_name(&get(2))))
                            .await
                            .map(|_| ())
                    };
                    match res {
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
                    }
                }
                None => CommonResponse {
                    error_code: 1,
                    error_msg: "no engine bound".into(),
                    session_id: sid,
                },
            };
            Ok(ArmResponse::Common(resp))
        }
        // `RenameTable` is executed for real; the rest are engine-level
        // maintenance that the in-memory engine has nothing to do for, so
        // they report success rather than `unknown method`.
        "Optimize"
        | "Command"
        | "AlterIndex"
        | "DumpIndex"
        | "CreateTableSnapshot"
        | "DropSnapshot"
        | "RenameTable" => {
            let (sid, fields) = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_string_request(&mut ip)?
            };
            let get = |id: i16| fields.get(&id).cloned().unwrap_or_default();
            let resp = if name == "RenameTable" {
                let (db, table, new_name) = (get(1), get(2), get(3));
                let full = full_table_name(&db, &table);
                run_common(
                    engine.as_ref(),
                    &format!("ALTER TABLE {full} RENAME TO {new_name};"),
                    sid,
                )
                .await
            } else if name == "CreateTableSnapshot" {
                // {1: session_id, 2: db, 3: table, 4: snapshot}
                let (db, table, snap) = (get(2), get(3), get(4));
                let full = full_table_name(&db, &table);
                run_common(
                    engine.as_ref(),
                    &format!("CREATE SNAPSHOT {} ON TABLE {full};", sql_name(&snap)),
                    sid,
                )
                .await
            } else if name == "DropSnapshot" {
                // {1: session_id, 2: snapshot}
                let snap = get(2);
                run_common(
                    engine.as_ref(),
                    &format!("DROP SNAPSHOT {};", sql_name(&snap)),
                    sid,
                )
                .await
            } else if name == "DumpIndex" {
                // `DumpIndexRequest` is the one request whose session id
                // comes last: {1: db, 2: table, 3: index, 4: session_id}
                // (`thrift/infinity.thrift:685`).
                let (db, table, index_name) = (get(1), get(2), get(3));
                let full = full_table_name(&db, &table);
                match engine.as_ref() {
                    Some(eng) => match eng.dump_index_transaction(&full, &index_name).await {
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
                }
            } else if name == "Optimize" {
                // Merge the index chunks that earlier memory dumps left
                // behind, as the reference's optimize does.
                match engine.as_ref() {
                    Some(eng) => match eng.optimize_indexes_async().await {
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
                }
            } else {
                CommonResponse {
                    error_code: OK,
                    error_msg: "OK".into(),
                    session_id: sid,
                }
            };
            Ok(ArmResponse::Common(resp))
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
                Some(eng) => match eng.execute(&sql).await {
                    Ok(rs) => {
                        let mut rows: Vec<Vec<Value>> = Vec::new();
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
            Ok(ArmResponse::Select(resp))
        }
        "AddColumns" => {
            let (db, table, cols, sid) = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_alter_add_args(&mut ip)?
            };
            let full = full_table_name(&db, &table);
            let defs: Vec<String> = cols
                .iter()
                .map(|c| {
                    let mut def = format!("{} {}", c.name, column_sql_type(c));
                    if c.not_null {
                        def.push_str(" NOT NULL");
                    }
                    if let Some(d) = &c.default {
                        def.push_str(&format!(" DEFAULT {}", render_default(d)));
                    }
                    def
                })
                .collect();
            let sql = format!("ALTER TABLE {full} ADD COLUMN {}", defs.join(", "));
            let resp = run_common(engine.as_ref(), &sql, sid).await;
            Ok(ArmResponse::Common(resp))
        }
        "DropColumns" => {
            let (db, table, names, sid) = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_alter_drop_args(&mut ip)?
            };
            let full = full_table_name(&db, &table);
            let mut resp = CommonResponse {
                error_code: OK,
                error_msg: "OK".into(),
                session_id: sid,
            };
            for name in names {
                let sql = format!("ALTER TABLE {full} DROP COLUMN {name}");
                if std::env::var_os("INF_DEBUG_SQL").is_some() {
                    eprintln!("[INSERT] {sql}");
                }
                resp = run_common(engine.as_ref(), &sql, sid).await;
                if resp.error_code != OK {
                    break;
                }
            }
            Ok(ArmResponse::Common(resp))
        }
        "Export" => {
            let req = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_export_args(&mut ip)?
            };
            let full = full_table_name(&req.db, &req.table);
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
                match eng.execute(&sql).await {
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
            Ok(ArmResponse::Common(resp))
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
                    error_msg: "Invalid command: Config mem_index_capacity is read-only".into(),
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
                    run_common(engine.as_ref(), &sql, sid).await
                }
            };
            Ok(ArmResponse::Common(resp))
        }
        "ShowConfig" => {
            let (_sid, name) = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_show_config_args(&mut ip)?
            };
            let value = engine
                .as_ref()
                .and_then(|e| e.config_get(&name))
                // Read-only informational entries the reference always
                // answers, even though nothing sets them.
                .or_else(|| match name.as_str() {
                    "version" => Some("0.0.0".to_string()),
                    "profile" => Some(String::new()),
                    "mem_index_capacity" => Some("0".to_string()),
                    _ => None,
                });
            Ok(ArmResponse::ShowConfig { name, value })
        }
        "ShowSegments" => {
            let (sid, db, table) = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_table_args(&mut ip)?
            };
            let _ = (sid, &db);
            // Same reason as `ListDatabase`: the segment layer is a catalog
            // read.
            let rows = match engine {
                Some(e) => {
                    let table = table.clone();
                    text_cells(
                        executor::scheduler()
                            .spawn_blocking(move || show_segments_rows(&e, &table))
                            .await,
                    )
                }
                None => text_cells(Vec::new()),
            };
            let columns: Vec<String> = SHOW_SEGMENTS_COLUMNS
                .iter()
                .map(|c| c.to_string())
                .collect();
            let types: Vec<String> = SHOW_SEGMENTS_TYPES.iter().map(|t| t.to_string()).collect();
            let resp = select_response_from_typed(&columns, &types, &rows);
            Ok(ArmResponse::Select(resp))
        }
        "ShowBlocks" => {
            let (sid, db, table, segment) = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_block_args(&mut ip)?
            };
            let _ = (sid, &db);
            // Same reason as `ListDatabase`.
            let rows = match engine {
                Some(e) => {
                    let table = table.clone();
                    text_cells(
                        executor::scheduler()
                            .spawn_blocking(move || {
                                show_blocks_rows(&e, &table, segment.max(0) as u32)
                            })
                            .await,
                    )
                }
                None => text_cells(Vec::new()),
            };
            let columns: Vec<String> = SHOW_BLOCKS_COLUMNS.iter().map(|c| c.to_string()).collect();
            let types: Vec<String> = SHOW_BLOCKS_TYPES.iter().map(|t| t.to_string()).collect();
            let resp = select_response_from_typed(&columns, &types, &rows);
            Ok(ArmResponse::Select(resp))
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
                3 => "FVECS",
                _ => "CSV",
            };
            // C++ takes `delimiter[0]` and zsv treats NUL as `,`; an
            // unset SDK option serialises as a single NUL byte.
            let delimiter = match req.delimiter.chars().next() {
                Some(c) if c != '\0' => c.to_string(),
                _ => ",".to_string(),
            };
            let mut sql = format!(
                "COPY {table} FROM '{}' WITH ( DELIMITER '{}', FORMAT {format} )",
                req.file.replace('\'', "''"),
                delimiter.replace('\'', "''")
            );
            if req.has_header {
                sql.push_str(" ;");
            }
            if std::env::var_os("INF_DEBUG_SQL").is_some() {
                eprintln!("[IMPORT] {sql}");
            }
            let resp = match engine.as_ref() {
                Some(eng) => match eng.execute(&sql).await {
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
            Ok(ArmResponse::Common(resp))
        }
        "ShowColumns" => {
            let (_sid, fields) = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_string_request(&mut ip)?
            };
            let db = fields.get(&1).cloned().unwrap_or_default();
            let table = fields.get(&2).cloned().unwrap_or_default();
            let full = full_table_name(&db, &table);
            let resp = match &engine {
                Some(eng) => match eng.execute(&format!("SHOW TABLE {full} COLUMNS;")).await {
                    Ok(rs) => {
                        let mut rows: Vec<Vec<Value>> = Vec::new();
                        for r in rs {
                            if let QueryResult::Rows { rows: rw, .. } = r {
                                rows = rw;
                            }
                        }
                        // The SQL renderer marks an absent comment
                        // `(empty)`; the SDK sees an empty string.
                        for row in rows.iter_mut() {
                            if let Some(Value::Str(c)) = row.get_mut(3) {
                                if c == "(empty)" {
                                    c.clear();
                                }
                            }
                        }
                        select_response_from_typed(
                            &[
                                "name".to_string(),
                                "type".to_string(),
                                "default".to_string(),
                                "comment".to_string(),
                            ],
                            &vec!["VARCHAR".to_string(); 4],
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
            Ok(ArmResponse::Select(resp))
        }
        "ListTable" => {
            let (_sid, fields) = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_string_request(&mut ip)?
            };
            let db = fields.get(&1).cloned().unwrap_or_default();
            let prefix = format!("{db}.");
            // Same reason as `ListDatabase`: the catalog walk is RocksDB I/O.
            let names: Vec<String> = match engine {
                Some(eng) => executor::scheduler()
                    .spawn_blocking(move || eng.catalog().list_tables())
                    .await
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
            Ok(ArmResponse::List(names))
        }
        "ListIndex" => {
            let (_sid, fields) = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_string_request(&mut ip)?
            };
            let table = fields.get(&2).cloned().unwrap_or_default();
            // Same reason as `ListDatabase`: both branches walk the catalog
            // store, and an empty table walks all of it.
            let listed = match engine {
                Some(eng) => {
                    let named = table.clone();
                    executor::scheduler()
                        .spawn_blocking(move || {
                            // An empty table means "every index"; a named one
                            // resolves through the per-table directory.
                            if named.is_empty() {
                                eng.catalog().list_indexes()
                            } else {
                                eng.catalog().indexes_of_table(&named)
                            }
                        })
                        .await
                        .unwrap_or_default()
                }
                None => Vec::new(),
            };
            let names: Vec<String> = listed
                .into_iter()
                .filter(|im| {
                    table.is_empty()
                        || im.table == table
                        || im.table.ends_with(&format!(".{table}"))
                })
                .map(|im| im.name)
                .collect();
            Ok(ArmResponse::List(names))
        }
        "ShowDatabase" => {
            let (_sid, fields) = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_string_request(&mut ip)?
            };
            let db = fields.get(&1).cloned().unwrap_or_default();
            let prefix = format!("{db}.");
            // Same reason as `ListDatabase`.
            let table_count = match engine {
                Some(eng) => executor::scheduler()
                    .spawn_blocking(move || eng.catalog().list_tables())
                    .await
                    .unwrap_or_default()
                    .into_iter()
                    .filter(|t| db.is_empty() || t.name.starts_with(&prefix))
                    .count() as i64,
                None => 0,
            };
            // Identifiers are normalised to lower case, so `SHOW DATABASE`
            // echoes the canonical name even for `"DEFAULT_DB"`.
            Ok(ArmResponse::ShowDatabase {
                name: db.to_ascii_lowercase(),
                table_count,
                comment: String::new(),
            })
        }
        "CreateIndex" => {
            let req = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                let r = read_create_index_args(&mut ip)?;
                if std::env::var_os("INF_DEBUG_SQL").is_some() {
                    eprintln!("[CREATEINDEX] {r:?}");
                }
                r
            };
            let full = full_table_name(&req.db_name, &req.table_name);
            let target = req.function.clone().unwrap_or_else(|| req.column.clone());
            let resp = if target.is_empty() {
                CommonResponse {
                    error_code: OK,
                    error_msg: "OK".into(),
                    session_id: req.session_id,
                }
            } else {
                // Identifiers are case-insensitive; the catalog keeps the
                // canonical lower-case spelling.
                let name = req.index_name.to_ascii_lowercase();
                let mut sql = format!(
                    "CREATE INDEX {}{name} ON {full} ({target})",
                    if req.conflict == 0 {
                        "IF NOT EXISTS "
                    } else {
                        ""
                    }
                );
                if !req.index_type.is_empty() {
                    sql.push_str(&format!(" USING {}", req.index_type));
                }
                if !req.params.is_empty() {
                    // C++ `Infinity::CreateIndex` lowercases every
                    // `InitParameter` name *and* value before the
                    // statement runs, so over Thrift `analyzer='Chinese'`
                    // means `chinese` (the SQL path keeps the spelling
                    // and stays case sensitive). The value is quoted so
                    // one with spaces (`invalid compress type`) still
                    // parses.
                    let opts = req
                        .params
                        .iter()
                        .map(|(k, v)| {
                            format!(
                                "{}='{}'",
                                k.to_lowercase(),
                                v.to_lowercase().replace('\'', "''")
                            )
                        })
                        .collect::<Vec<_>>()
                        .join(",");
                    sql.push_str(&format!(" WITH ({opts})"));
                }
                sql.push(';');
                let resp = run_common(engine.as_ref(), &sql, req.session_id).await;
                if resp.error_code == OK {
                    if let Some(eng) = engine.as_ref() {
                        eng.set_index_comment(&full, &name, &req.index_comment);
                    }
                }
                resp
            };
            Ok(ArmResponse::Common(resp))
        }
        "ShowCurrentNode" => {
            {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                skip_struct(&mut ip)?;
            }
            let (node_role, server_status) = peer.node_state();
            Ok(ArmResponse::ShowCurrentNode {
                node_role,
                server_status,
            })
        }
        "ShowSnapshot" => {
            let (sid, name) = {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                read_snapshot_args(&mut ip)?
            };
            let _ = sid;
            let known = engine
                .as_ref()
                .and_then(|e| e.snapshot_names().ok())
                .map(|n| n.iter().any(|x| x == &name))
                .unwrap_or(false);
            Ok(ArmResponse::ShowSnapshot { name, found: known })
        }
        "ListSnapshots" => {
            {
                let mut ip = TBinaryInputProtocol::new(&mut reader, true);
                skip_struct(&mut ip)?;
            }
            let names = engine
                .as_ref()
                .and_then(|e| e.snapshot_names().ok())
                .unwrap_or_default();
            Ok(ArmResponse::ListSnapshots(names))
        }
        other => Err(Error::Invalid(format!("unknown method: {other}"))),
    }
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
        // `Decimal` and `Interval` are on the wire but not in the reference's
        // column-type whitelist (`VerifyColumnDataType`); render their real
        // names so the DDL layer rejects them instead of quietly creating a
        // `VARCHAR` column.
        6 => "DECIMAL".into(),
        7 => "FLOAT".into(),
        8 => "DOUBLE".into(),
        9 => "FLOAT16".into(),
        10 => "BFLOAT16".into(),
        11 => "VARCHAR".into(),
        17 => "DATE".into(),
        18 => "TIME".into(),
        19 => "DATETIME".into(),
        20 => "TIMESTAMP".into(),
        // `Interval` (`LogicType::Interval`, ordinal 21) is rejected by the
        // reference's column-type whitelist.
        21 => "INTERVAL".into(),
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
            index_type,
        } => format!(
            "SPARSE({}, {}, {dimension})",
            element_type_name(*element_type),
            element_type_name(*index_type)
        ),
        // A scalar array element's descriptor carries no shape.
        PhysicalWire::Scalar(_) | PhysicalWire::None => scalar_sql(logic_type),
    }
}

/// Renders a declared `ColumnDef` back into the engine's SQL type text, so a
/// `CreateTable` RPC preserves the column's real type (embedding included)
/// instead of flattening everything to `VARCHAR`.
fn column_sql_type(c: &ColumnDefWire) -> String {
    type_sql(c.logic_type, &c.physical)
}

/// Applies a `CreateTable` request to the engine (or errors if no engine bound).
/// Renders a wire `DEFAULT` value as SQL, quoting a bare string so that an empty
/// or identifier-like value does not leave `DEFAULT` without a value.
pub(crate) fn render_default(d: &str) -> String {
    let t = d.trim();
    if t.is_empty() {
        "''".to_string()
    } else if t.starts_with('\'')
        || t.parse::<f64>().is_ok()
        || t == "null"
        || t.eq_ignore_ascii_case("true")
        || t.eq_ignore_ascii_case("false")
        || t.starts_with('[')
    {
        t.to_string()
    } else {
        format!("'{}'", t.replace('\'', "''"))
    }
}

/// Renders `name` as a SQL identifier: plain names pass through, anything
/// else (dashes, dots, spaces) is double-quoted, which the parser accepts.
fn sql_name(name: &str) -> String {
    let plain = !name.is_empty()
        && name
            .chars()
            .next()
            .map(|c| c.is_ascii_alphabetic() || c == '_')
            .unwrap_or(false)
        && name.chars().all(|c| c.is_ascii_alphanumeric() || c == '_');
    if plain {
        name.to_string()
    } else {
        format!("\"{}\"", name.replace('"', ""))
    }
}

async fn run_create_table(engine: Option<&Engine>, req: &CreateTableReq) -> CommonResponse {
    let Some(engine) = engine else {
        return CommonResponse {
            error_code: 1,
            error_msg: "no engine bound".into(),
            session_id: req.session_id,
        };
    };
    // A table needs at least one column (`Status::NoColumnDefined`).
    if req.columns.is_empty() {
        return CommonResponse {
            error_code: 3048,
            error_msg: "no column defined".into(),
            session_id: req.session_id,
        };
    }
    // `ConflictType::kReplace` is not supported by the reference engine.
    if req.conflict == 2 {
        return CommonResponse {
            error_code: 3032,
            error_msg: "not supported: ConflictType::kReplace".into(),
            session_id: req.session_id,
        };
    }
    // A column name must start with a letter and contain only word characters.
    for c in &req.columns {
        if c.name.is_empty() {
            return CommonResponse {
                error_code: 3042, // EMPTY_COLUMN_NAME
                error_msg: "empty column name".into(),
                session_id: req.session_id,
            };
        }
        let valid = c
            .name
            .chars()
            .next()
            .map(|ch| ch.is_ascii_alphabetic())
            .unwrap_or(false)
            && c.name
                .chars()
                .all(|ch| ch.is_ascii_alphanumeric() || ch == '_');
        if !valid {
            return CommonResponse {
                error_code: 3005, // INVALID_COLUMN_NAME
                error_msg: format!("invalid column name '{}'", c.name),
                session_id: req.session_id,
            };
        }
    }
    let cols: Vec<String> = req
        .columns
        .iter()
        .map(|c| {
            // The reference lowercases identifiers.
            let mut col = format!("{} {}", c.name.to_ascii_lowercase(), column_sql_type(c));
            if c.not_null {
                col.push_str(" NOT NULL");
            }
            if let Some(d) = &c.default {
                col.push_str(&format!(" DEFAULT {}", render_default(d)));
            }
            if !c.comment.is_empty() {
                col.push_str(&format!(" COMMENT '{}'", c.comment.replace('\'', "''")));
            }
            col
        })
        .collect();
    // `db_name` names the target database; an unqualified table would otherwise
    // land in `default_db`.
    let table = if req.db_name.is_empty() {
        req.table_name.clone()
    } else {
        format!("{}.{}", req.db_name, req.table_name)
    };
    // `ConflictType::kIgnore`: creating an existing table is a no-op.
    if req.conflict == 0 && engine.table_exists(&table) {
        return CommonResponse {
            error_code: OK,
            error_msg: "OK".into(),
            session_id: req.session_id,
        };
    }
    let sql = format!("CREATE TABLE {table} ({});", cols.join(", "));
    // The SDK's `sparse,0,…` type carries dimension 0, which SQL DDL rejects.
    // The flag is thread-local, so it is set around the **await-free** call
    // below: `execute_async` reads it during DDL planning, before any of its
    // dispatch points.
    match engine.execute(&sql).await {
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

/// A client-side `ShowCurrentNode`.
pub fn show_current_node(addr: &str, session_id: i64) -> Result<ShowCurrentNodeResponse> {
    let mut stream = TcpStream::connect(addr).map_err(|e| Error::io(addr, e))?;
    // `ShowCurrentNodeRequest { 1: session_id }` has the `CommonRequest` shape.
    write_common_call(&mut stream, "ShowCurrentNode", 3, session_id)?;
    let mut reader = stream.try_clone().map_err(io)?;
    read_show_current_node_reply(&mut reader)
}

/// Reads a `ShowCurrentNode` REPLY.
pub fn read_show_current_node_reply(r: &mut impl Read) -> Result<ShowCurrentNodeResponse> {
    let mut i = TBinaryInputProtocol::new(r, true);
    let _msg = i.read_message_begin().map_err(tw)?;
    i.read_struct_begin().map_err(tw)?; // ShowCurrentNode_result
    let success = i.read_field_begin().map_err(tw)?;
    if success.field_type != TType::Struct {
        return Err(Error::Invalid("expected success struct".into()));
    }
    i.read_struct_begin().map_err(tw)?; // ShowCurrentNodeResponse
    let mut response = ShowCurrentNodeResponse::default();
    loop {
        let f = i.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match (f.id, f.field_type) {
            (Some(1), TType::I64) => response.error_code = i.read_i64().map_err(tw)?,
            (Some(2), TType::String) => response.error_msg = i.read_string().map_err(tw)?,
            (Some(3), TType::String) => response.node_role = i.read_string().map_err(tw)?,
            (Some(4), TType::String) => response.server_status = i.read_string().map_err(tw)?,
            _ => i.skip(f.field_type).map_err(tw)?,
        }
        i.read_field_end().map_err(tw)?;
    }
    i.read_struct_end().map_err(tw)?;
    Ok(response)
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
    } else {
        let r = up.strip_prefix("TENSOR(")?;
        (11, r)
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
        // `bfloat16_t` keeps the top half of the `f32` (truncation).
        9 => chunk.extend_from_slice(&(((f as f32).to_bits() >> 16) as u16).to_le_bytes()),
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

/// Parses `SPARSE(<elem>,<bound>)` / `SPARSE(<elem>,<index>,<bound>)`. Without
/// an explicit index the width is derived from the bound; with one the declared
/// width wins (`sparse,8,float,int` stores 4-byte indices).
fn sparse_info(type_text: &str) -> Option<(i64, i32, i32)> {
    let up = type_text.to_ascii_uppercase();
    if !up.starts_with("SPARSE(") {
        return None;
    }
    let inner = up.split_once('(')?.1.trim_end_matches(')');
    let parts: Vec<&str> = inner.split(',').map(str::trim).collect();
    let elem = (*parts.first()?).to_string();
    let (explicit_index, bound_text) = if parts.len() >= 3 {
        (Some(parts[1]), parts[2])
    } else {
        (None, *parts.get(1)?)
    };
    let bound: u64 = bound_text.parse().unwrap_or(0);
    let derived_index = if bound <= 127 {
        2 // ElementInt8
    } else if bound <= 32767 {
        3 // ElementInt16
    } else if bound <= 2_147_483_647 {
        4 // ElementInt32
    } else {
        5 // ElementInt64
    };
    let index_code = match explicit_index.and_then(sparse_index_code) {
        Some(code) => code,
        None => derived_index,
    };
    Some((bound as i64, element_type_code(&elem), index_code))
}

/// `int8`/`tinyint`/… -> the `ColKind::Sparse` index-width selector
/// (`src/parser/type/complex/embedding_type.h:33-45`).
fn sparse_index_code(name: &str) -> Option<i32> {
    Some(match name {
        "INT8" | "TINYINT" | "UINT8" => 2,
        "INT16" | "SMALLINT" => 3,
        "INT" | "INTEGER" | "INT32" => 4,
        "INT64" | "BIGINT" => 5,
        _ => return None,
    })
}

/// Classifies a column from its declared type text, falling back to value
/// inference when the type is unknown.
fn classify_column(declared: &str, rows: &[Vec<Value>], ci: usize) -> ColKind {
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
    let physical = match physical {
        PhysicalWire::None => PhysicalWire::Scalar(logic),
        p => p,
    };
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
/// The text of a cell, borrowed when it already is text.
fn cell_text(value: &Value) -> std::borrow::Cow<'_, str> {
    match value {
        Value::Str(s) => std::borrow::Cow::Borrowed(s),
        other => std::borrow::Cow::Owned(other.to_slt_string()),
    }
}

/// A cell as an integer, straight from the value (a scan no longer hands the
/// protocol layer rendered text, so there is nothing to parse for the common
/// case).
fn cell_int(value: Option<&Value>) -> Option<i64> {
    match value? {
        Value::Int(i) => Some(*i),
        Value::Float(f) => Some(*f as i64),
        Value::Bool(b) => Some(*b as i64),
        Value::Str(s) => s.trim().parse().ok(),
        Value::Null => None,
    }
}

fn cell_float(value: Option<&Value>) -> Option<f64> {
    match value? {
        Value::Int(i) => Some(*i as f64),
        Value::Float(f) => Some(*f),
        Value::Bool(b) => Some(*b as i64 as f64),
        Value::Str(s) => s.trim().parse().ok(),
        Value::Null => None,
    }
}

fn cell_bool(value: Option<&Value>) -> Option<bool> {
    match value? {
        Value::Bool(b) => Some(*b),
        Value::Int(i) => Some(*i != 0),
        Value::Str(s) => Some(s.trim().eq_ignore_ascii_case("true")),
        _ => None,
    }
}

fn infer_column_type(rows: &[Vec<Value>], ci: usize) -> i32 {
    let (mut int, mut flt, mut boolean, mut any) = (true, true, true, false);
    for r in rows {
        let Some(v) = r.get(ci).filter(|v| !matches!(v, Value::Null)) else {
            continue;
        };
        any = true;
        match v {
            Value::Int(_) => {
                boolean = false;
            }
            Value::Float(_) => {
                int = false;
                boolean = false;
            }
            Value::Bool(_) => {
                int = false;
                flt = false;
            }
            // A text cell is inferred from its content: a computed expression
            // can still produce text.
            Value::Str(s) => {
                let t = s.trim();
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
            Value::Null => {}
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
    // No declared type is known here, so the SQL NULL marker is the spelling
    // the text pipeline used: "null" in any case.
    let values: Vec<Vec<Value>> = rows
        .iter()
        .map(|r| {
            r.iter()
                .map(|c| {
                    if c.eq_ignore_ascii_case("null") {
                        Value::Null
                    } else {
                        Value::Str(c.clone())
                    }
                })
                .collect()
        })
        .collect();
    rows_to_column_fields_typed(columns, &[], &values)
}

/// Packs each column for its `ColumnType`; a declared embedding column is a raw
/// element stream sliced by the reader's dimension.
pub fn rows_to_column_fields_typed(
    columns: &[String],
    types: &[String],
    rows: &[Vec<Value>],
) -> Vec<ColumnField> {
    let mut out = Vec::new();
    for (ci, name) in columns.iter().enumerate() {
        let declared = types.get(ci).map(String::as_str).unwrap_or("");
        // A NULL is the `Value::Null` the scan produced; a varchar cell that
        // happens to spell "null" is data.
        let kind = classify_column(declared, rows, ci);
        if let ColKind::Array { .. } = kind {
            let mut chunk: Vec<u8> = Vec::new();
            let mut masks = Vec::with_capacity(rows.len());
            for r in rows {
                let value = r.get(ci).filter(|v| !matches!(v, Value::Null));
                let text = value.map(cell_text);
                let cell = text.as_deref();
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
                bitmasks: if masks.iter().all(|m| *m) {
                    Vec::new()
                } else {
                    masks
                },
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
                let value = r.get(ci).filter(|v| !matches!(v, Value::Null));
                let text = value.map(cell_text);
                let cell = text.as_deref();
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
                bitmasks: if masks.iter().all(|m| *m) {
                    Vec::new()
                } else {
                    masks
                },
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
                let value = r.get(ci).filter(|v| !matches!(v, Value::Null));
                let text = value.map(cell_text);
                let cell = text.as_deref();
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
                bitmasks: if masks.iter().all(|m| *m) {
                    Vec::new()
                } else {
                    masks
                },
            });
            continue;
        }
        if let ColKind::Embedding(dim, elem) = kind {
            let mut chunk: Vec<u8> = Vec::new();
            let mut masks = Vec::with_capacity(rows.len());
            for r in rows {
                let value = r.get(ci).filter(|v| !matches!(v, Value::Null));
                let text = value.map(cell_text);
                let cell = text.as_deref();
                masks.push(cell.is_some());
                // A bit vector is packed `dim` bits into `ceil(dim/8)` bytes
                // (LSB first), which is what the SDK unpacks.
                if elem == 0 {
                    let bits: Vec<u8> = cell
                        .unwrap_or("")
                        .chars()
                        .filter(|c| *c == '0' || *c == '1')
                        .map(|c| u8::from(c == '1'))
                        .collect();
                    let nbytes = (dim as usize).div_ceil(8);
                    for b in 0..nbytes {
                        let mut byte = 0u8;
                        for k in 0..8 {
                            let i = b * 8 + k;
                            if i < bits.len() && bits[i] == 1 {
                                byte |= 1 << k;
                            }
                        }
                        chunk.push(byte);
                    }
                    continue;
                }
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
                bitmasks: if masks.iter().all(|m| *m) {
                    Vec::new()
                } else {
                    masks
                },
            });
            continue;
        }
        let ColKind::Scalar(code) = kind else {
            unreachable!()
        };
        let mut chunk: Vec<u8> = Vec::new();
        let mut masks = Vec::with_capacity(rows.len());
        for r in rows {
            let value = r.get(ci).filter(|v| !matches!(v, Value::Null));
            let text = value.map(cell_text);
            let cell = text.as_deref();
            let valid = value.is_some();
            match code {
                // The integers and floats come straight from the value: the
                // scan hands over `Value`s, so there is no text to parse.
                1 => chunk.push(cell_int(value).unwrap_or(0) as i8 as u8),
                2 => chunk.extend_from_slice(&(cell_int(value).unwrap_or(0) as i16).to_le_bytes()),
                3 => chunk.extend_from_slice(&(cell_int(value).unwrap_or(0) as i32).to_le_bytes()),
                4 => chunk.extend_from_slice(&cell_int(value).unwrap_or(0).to_le_bytes()),
                5 => chunk
                    .extend_from_slice(&(cell_float(value).unwrap_or(0.0) as f32).to_le_bytes()),
                6 => chunk.extend_from_slice(&cell_float(value).unwrap_or(0.0).to_le_bytes()),
                7 => {
                    // ColumnFloat16: IEEE half precision.
                    let f = cell_float(value).unwrap_or(0.0) as f32;
                    chunk.extend_from_slice(&f32_to_f16_bits(f).to_le_bytes());
                }
                8 => {
                    // ColumnBFloat16: the C++ bfloat16_t keeps the top 16 bits.
                    let f = cell_float(value).unwrap_or(0.0) as f32;
                    chunk.extend_from_slice(&((f.to_bits() >> 16) as u16).to_le_bytes());
                }
                0 => chunk.push(u8::from(cell_bool(value).unwrap_or(false))),
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
            bitmasks: if masks.iter().all(|m| *m) {
                Vec::new()
            } else {
                masks
            },
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
    /// A scalar array element still carries an (empty) physical descriptor:
    /// the reference sends `NumberType`/`VarcharType` and SDKs require
    /// `physical_type` to be present to decode the element stream.
    Scalar(i32),
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
    /// `ColumnDef.comment`
    pub comment: String,
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
        PhysicalWire::Scalar(logic) => {
            // `PhysicalType{1: NumberType{}}` / `{2: VarcharType{}}`: the
            // payload is empty, but SDKs check that the field is present.
            let (field, name) = if matches!(logic, 11 | 22 | 23) {
                (2i16, "varchar_type")
            } else {
                (1i16, "number_type")
            };
            o.write_field_begin(&TFieldIdentifier::new("physical_type", TType::Struct, 2i16))
                .map_err(tw)?;
            o.write_struct_begin(&TStructIdentifier::new("PhysicalType"))
                .map_err(tw)?;
            o.write_field_begin(&TFieldIdentifier::new(name, TType::Struct, field))
                .map_err(tw)?;
            o.write_struct_begin(&TStructIdentifier::new(name))
                .map_err(tw)?;
            o.write_field_stop().map_err(tw)?;
            o.write_struct_end().map_err(tw)?;
            o.write_field_end().map_err(tw)?;
            o.write_field_stop().map_err(tw)?;
            o.write_struct_end().map_err(tw)?;
            o.write_field_end().map_err(tw)?;
        }
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
    /// `CreateOption.conflict_type`: 0 ignore, 1 error, 2 replace.
    pub conflict: i32,
}

fn read_column_def(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<ColumnDefWire> {
    ip.read_struct_begin().map_err(tw)?;
    let (mut id, mut name, mut lt) = (0i32, String::new(), 0i32);
    let mut default: Option<String> = None;
    let mut comment = String::new();
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
            Some(6) if f.field_type == TType::String => {
                comment = ip.read_string().map_err(tw)?;
            }
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
        comment,
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
        conflict: 1,
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
            Some(7) => {
                // CreateOption { 1: conflict_type, 2: properties }
                ip.read_struct_begin().map_err(tw)?;
                loop {
                    let o = ip.read_field_begin().map_err(tw)?;
                    if o.field_type == TType::Stop {
                        break;
                    }
                    if o.id == Some(1) && o.field_type == TType::I32 {
                        req.conflict = ip.read_i32().map_err(tw)?;
                    } else {
                        ip.skip(o.field_type).map_err(tw)?;
                    }
                    ip.read_field_end().map_err(tw)?;
                }
                ip.read_struct_end().map_err(tw)?;
            }
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
    /// The client-facing names, which follow the C++ `Name()` printer (a cast
    /// prints as `Cast(c3 AS Varchar)`, string arguments lose their quotes).
    pub names: Vec<String>,
    /// The same names without catalog-name canonicalisation, used for
    /// projections that are also group keys (`abs(...)` stays lower case).
    pub raw_names: Vec<String>,
    pub star: bool,
    /// Optional `WHERE` clause already rendered as SQL.
    pub where_sql: Option<String>,
    /// Optional `(column, ascending)` order key.
    pub order_by: Vec<(String, bool)>,
    /// Group keys, already rendered as SQL text.
    pub group_by: Vec<String>,
    /// Optional `HAVING` clause already rendered as SQL.
    pub having_sql: Option<String>,
    pub limit: Option<i64>,
    pub offset: Option<i64>,
    /// `total_hits_count` option: report how many rows matched before LIMIT.
    pub total_hits: bool,
    /// `(alias, expression SQL)` pairs, so WHERE/ORDER BY can name a projection.
    pub aliases: Vec<(String, String)>,
    /// `HIGHLIGHT` columns.
    pub highlight: Vec<String>,
    /// Rendered `SEARCH ...` tail (match clauses + fusions).
    pub search_sql: Option<String>,
}

impl SelectReq {
    /// Builds the SQL for this request.
    pub fn to_sql(&self) -> String {
        let cols = if self.star {
            // `SELECT *, distance()` keeps the trailing projection.
            if self.columns.is_empty() {
                "*".to_string()
            } else {
                format!("*, {}", self.columns.join(", "))
            }
        } else if self.columns.is_empty() {
            "*".to_string()
        } else {
            self.columns.join(", ")
        };
        let table = if self.db_name.is_empty() {
            self.table_name.clone()
        } else {
            format!("{}.{}", self.db_name, self.table_name)
        };
        let mut sql = format!("SELECT {cols}");
        if !self.highlight.is_empty() {
            sql.push_str(&format!(" HIGHLIGHT {}", self.highlight.join(", ")));
        }
        sql.push_str(&format!(" FROM {table}"));
        if let Some(srch) = &self.search_sql {
            sql.push_str(&format!(" SEARCH {srch}"));
        }
        if let Some(w) = &self.where_sql {
            sql.push_str(&format!(" WHERE {}", substitute_aliases(w, &self.aliases)));
        }
        if !self.group_by.is_empty() {
            sql.push_str(&format!(" GROUP BY {}", self.group_by.join(", ")));
        }
        if let Some(h) = &self.having_sql {
            sql.push_str(&format!(" HAVING {}", substitute_aliases(h, &self.aliases)));
        }
        if !self.order_by.is_empty() {
            let keys: Vec<String> = self
                .order_by
                .iter()
                .map(|(c, asc)| format!("{c} {}", if *asc { "ASC" } else { "DESC" }))
                .collect();
            sql.push_str(&format!(" ORDER BY {}", keys.join(", ")));
        }
        if let Some(n) = self.limit {
            sql.push_str(&format!(" LIMIT {n}"));
        } else if self.offset.is_some() {
            // The reference's planner keeps LIMIT and OFFSET as independent
            // fields, so an OFFSET without a limit means "every row after n".
            // The SQL grammar requires a LIMIT first, so name an unbounded one.
            sql.push_str(" LIMIT 9223372036854775807");
        }
        if let Some(n) = self.offset {
            sql.push_str(&format!(" OFFSET {n}"));
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
    /// `CAST(expr AS type)`: the carried text is the C++ `DataType::ToString()`
    /// spelling (`Varchar`, `Integer`, `Double`, ...).
    Cast(Box<PExpr>, String),
    Other,
}

/// Renders a decoded `ParsedExpr` back to SQL text, so a projected function
/// (`year(c1)`, `datepart(c4, c1)`) can be sent to the engine as-is.
pub fn pexpr_sql(e: &PExpr) -> String {
    match e {
        PExpr::Column(c) => c.clone(),
        PExpr::Value(v) => v.to_sql(),
        // `in(a, 1, 2, 3)` / `not_in(...)` render as SQL `IN` lists.
        PExpr::Call(n, a)
            if (n.eq_ignore_ascii_case("in") || n.eq_ignore_ascii_case("not_in"))
                && a.len() >= 2 =>
        {
            let op = if n.eq_ignore_ascii_case("not_in") {
                "NOT IN"
            } else {
                "IN"
            };
            let list = a[1..].iter().map(pexpr_sql).collect::<Vec<_>>().join(", ");
            format!("({} {op} ({list}))", pexpr_sql(&a[0]))
        }
        // The SDK sends a projected binary expression as `FunctionExpr("+", [a, b])`.
        PExpr::Call(n, a) if a.len() == 2 && is_binary_op(n) => {
            format!("({} {n} {})", pexpr_sql(&a[0]), pexpr_sql(&a[1]))
        }
        PExpr::Call(n, a) => format!(
            "{}({})",
            canonical_fn_name(n),
            a.iter().map(pexpr_sql).collect::<Vec<_>>().join(", ")
        ),
        PExpr::Cast(inner, ty) => format!("CAST({} AS {})", pexpr_sql(inner), ty),
        PExpr::Other => String::new(),
    }
}

/// The C++ `BaseExpression::Name()` spelling of a projected expression.
///
/// Unlike [`pexpr_sql`] this is *display* text: string constants lose their
/// SQL quotes, a cast prints as `Cast(<arg> AS <type>)`, and the registered
/// function name is used (the catalog knows `ABS`, not `abs`).
pub fn pexpr_name(e: &PExpr) -> String {
    pexpr_name_impl(e, true)
}

/// The name a *group key* keeps: the function name is the one the client sent
/// (`abs(...)`), not the catalog's registered spelling (`ABS(...)`).
pub fn pexpr_name_raw(e: &PExpr) -> String {
    pexpr_name_impl(e, false)
}

fn pexpr_name_impl(e: &PExpr, canonical: bool) -> String {
    let fname = |n: &str| {
        if canonical {
            canonical_fn_name(n).to_string()
        } else {
            n.to_string()
        }
    };
    match e {
        PExpr::Column(c) => c.clone(),
        PExpr::Value(v) => v.to_display(),
        // `filter_text` and `filter_fulltext` both analyse to the special
        // `FilterFulltextExpression`, whose `ToString()` is fixed.
        PExpr::Call(n, a) if is_filter_fn(n) => {
            let text = |i: usize| match a.get(i) {
                Some(PExpr::Value(ValueLit::Str(s))) => s.clone(),
                Some(other) => pexpr_name_impl(other, canonical),
                None => String::new(),
            };
            let options = if a.len() > 2 {
                format!(", '{}'", text(2))
            } else {
                String::new()
            };
            format!("FILTER_FULLTEXT('{}', '{}'{options})", text(0), text(1))
        }
        PExpr::Call(n, a) if a.len() == 2 && is_binary_op(n) => {
            format!(
                "({} {} {})",
                pexpr_name_impl(&a[0], canonical),
                canonical_op_name(n),
                pexpr_name_impl(&a[1], canonical)
            )
        }
        // Pseudo-columns (`_score`, `_distance`, …) are named upper-case and
        // without the empty argument list.
        PExpr::Call(n, a)
            if a.is_empty()
                && matches!(
                    n.to_ascii_lowercase().as_str(),
                    "score" | "distance" | "similarity" | "row_id"
                ) =>
        {
            n.to_ascii_uppercase()
        }
        // The analysed AVG is `sum(x) / count(x)`, and so is its name.
        PExpr::Call(n, a) if n.eq_ignore_ascii_case("avg") && a.len() == 1 => {
            let arg = match &a[0] {
                PExpr::Column(c) if c == "*" => "star".to_string(),
                other => pexpr_name_impl(other, canonical),
            };
            format!("(sum({arg}) / count({arg}))")
        }
        PExpr::Call(n, a) => {
            // A star argument is named `star` by the C++ printer
            // (`count(*)` comes back as `count(star)`).
            let args = a
                .iter()
                .map(|x| match x {
                    PExpr::Column(c) if c == "*" => "star".to_string(),
                    other => pexpr_name_impl(other, canonical),
                })
                .collect::<Vec<_>>()
                .join(", ");
            format!("{}({args})", fname(n))
        }
        PExpr::Cast(inner, ty) => {
            format!("Cast({} AS {})", pexpr_name_impl(inner, canonical), ty)
        }
        PExpr::Other => String::new(),
    }
}

/// True for the two spellings of the full-text filter function.
fn is_filter_fn(name: &str) -> bool {
    name.eq_ignore_ascii_case("filter_text") || name.eq_ignore_ascii_case("filter_fulltext")
}

/// Connectives are registered in upper case, so the printer spells them so.
fn canonical_op_name(name: &str) -> &str {
    if name.eq_ignore_ascii_case("and") {
        "AND"
    } else if name.eq_ignore_ascii_case("or") {
        "OR"
    } else if name.eq_ignore_ascii_case("not") {
        "NOT"
    } else {
        name
    }
}

/// The C++ catalog registers most scalars in lower case but a few in upper
/// case, and the projection name comes from those registered names.
fn canonical_fn_name(name: &str) -> &str {
    if name.eq_ignore_ascii_case("abs") {
        "ABS"
    } else if name.eq_ignore_ascii_case("pow") {
        "POW"
    } else {
        name
    }
}

/// Functions that return a boolean, usable directly as a filter.
fn is_boolean_fn(name: &str) -> bool {
    matches!(
        name.to_ascii_lowercase().as_str(),
        "regex"
            | "filter_text"
            | "filter_fulltext"
            | "filter_sparse"
            | "contains"
            | "startswith"
            | "endswith"
    )
}

/// True when the expression is a predicate (a comparison, a connective, `IN`
/// or `NOT`) rather than a value.
fn is_predicate_pexpr(e: &PExpr) -> bool {
    match e {
        PExpr::Call(n, args) => {
            let n = n.to_ascii_lowercase();
            is_boolean_fn(&n)
                || matches!(
                    n.as_str(),
                    "=" | "=="
                        | "!="
                        | "<>"
                        | "<"
                        | "<="
                        | ">"
                        | ">="
                        | "and"
                        | "or"
                        | "not"
                        | "in"
                        | "not_in"
                        | "between"
                        | "is_null"
                        | "is_not_null"
                ) && !args.is_empty()
        }
        _ => false,
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
                // `InExpr` nested inside another expression: `c1 IN (1, 2)`.
                Some(11) if uf.field_type == TType::Struct => {
                    ip.read_struct_begin().map_err(tw)?;
                    let mut left = PExpr::Other;
                    let mut vals: Vec<PExpr> = Vec::new();
                    let mut in_type = true;
                    loop {
                        let g = ip.read_field_begin().map_err(tw)?;
                        if g.field_type == TType::Stop {
                            break;
                        }
                        match (g.id, g.field_type) {
                            (Some(1), TType::Struct) => left = read_pexpr(ip)?,
                            (Some(2), TType::List) => {
                                let l = ip.read_list_begin().map_err(tw)?;
                                for _ in 0..l.size {
                                    vals.push(read_pexpr(ip)?);
                                }
                                ip.read_list_end().map_err(tw)?;
                            }
                            (Some(3), TType::Bool) => in_type = ip.read_bool().map_err(tw)?,
                            _ => ip.skip(g.field_type).map_err(tw)?,
                        }
                        ip.read_field_end().map_err(tw)?;
                    }
                    ip.read_struct_end().map_err(tw)?;
                    let mut args = vec![left];
                    args.extend(vals);
                    out = PExpr::Call(if in_type { "in" } else { "not_in" }.to_string(), args);
                }
                // `CastExpr`: `{1: expr, 2: data_type}`.
                Some(12) if uf.field_type == TType::Struct => out = read_cast_pexpr(ip)?,
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

/// Reads a `CastExpr` (`{1: expr, 2: data_type}`) into a [`PExpr::Cast`].
fn read_cast_pexpr(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<PExpr> {
    ip.read_struct_begin().map_err(tw)?;
    let mut inner = PExpr::Other;
    let mut ty = String::new();
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match f.id {
            Some(1) if f.field_type == TType::Struct => inner = read_pexpr(ip)?,
            Some(2) if f.field_type == TType::Struct => {
                let (logic, _) = read_data_type(ip)?;
                ty = logic_type_name(logic);
            }
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok(PExpr::Cast(Box::new(inner), ty))
}

/// The thrift `LogicType` name for a wire ordinal, i.e. the spelling C++'s
/// `DataType::ToString()` uses for a cast target (`Varchar`, `Integer`, ...).
fn logic_type_name(logic: i32) -> String {
    match logic {
        0 => "Boolean",
        1 => "TinyInt",
        2 => "SmallInt",
        3 => "Integer",
        4 => "BigInt",
        5 => "HugeInt",
        6 => "Decimal",
        7 => "Float",
        8 => "Double",
        9 => "Float16",
        10 => "BFloat16",
        11 => "Varchar",
        12 => "Embedding",
        13 => "Tensor",
        14 => "TensorArray",
        15 => "Sparse",
        16 => "MultiVector",
        17 => "Date",
        18 => "Time",
        19 => "DateTime",
        20 => "Timestamp",
        21 => "Interval",
        22 => "Array",
        23 => "Json",
        _ => "Varchar",
    }
    .to_string()
}

/// Replaces bare identifiers that name a projection alias with the aliased
/// expression text, leaving string literals untouched.
pub(crate) fn substitute_aliases(text: &str, aliases: &[(String, String)]) -> String {
    if aliases.is_empty() {
        return text.to_string();
    }
    let mut out = String::with_capacity(text.len());
    let bytes: Vec<char> = text.chars().collect();
    let mut i = 0usize;
    let is_ident = |c: char| c.is_ascii_alphanumeric() || c == '_';
    while i < bytes.len() {
        let c = bytes[i];
        if c == '\'' || c == '"' {
            // copy the quoted run verbatim
            let quote = c;
            out.push(c);
            i += 1;
            while i < bytes.len() {
                out.push(bytes[i]);
                if bytes[i] == quote {
                    i += 1;
                    break;
                }
                i += 1;
            }
            continue;
        }
        if c.is_ascii_alphabetic() || c == '_' {
            let start = i;
            while i < bytes.len() && is_ident(bytes[i]) {
                out.push(bytes[i]);
                i += 1;
            }
            let word: String = bytes[start..i].iter().collect();
            if let Some((_, expr)) = aliases.iter().find(|(a, _)| a.eq_ignore_ascii_case(&word)) {
                out.truncate(out.len() - word.len());
                out.push('(');
                out.push_str(expr);
                out.push(')');
            }
            continue;
        }
        out.push(c);
        i += 1;
    }
    out
}

/// Reads a `SearchExpr` (`{1: list<GenericMatchExpr>, 2: list<FusionExpr>}`)
/// and renders the engine's `SEARCH …` tail.
fn read_search_expr(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<Option<String>> {
    ip.read_struct_begin().map_err(tw)?;
    let mut parts: Vec<String> = Vec::new();
    let mut fusions: Vec<String> = Vec::new();
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if std::env::var_os("INF_DEBUG_SQL").is_some() {
            eprintln!("[SEARCH] field {:?} {:?}", f.id, f.field_type);
        }
        if f.field_type == TType::Stop {
            break;
        }
        match (f.id, f.field_type) {
            (Some(1), TType::List) => {
                let l = ip.read_list_begin().map_err(tw)?;
                for _ in 0..l.size {
                    if let Some(m) = read_generic_match(ip)? {
                        parts.push(m);
                    }
                }
                ip.read_list_end().map_err(tw)?;
            }
            (Some(2), TType::List) => {
                let l = ip.read_list_begin().map_err(tw)?;
                for _ in 0..l.size {
                    if let Some(fs) = read_fusion_expr(ip)? {
                        fusions.push(fs);
                    }
                }
                ip.read_list_end().map_err(tw)?;
            }
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    if parts.is_empty() && fusions.is_empty() {
        return Ok(None);
    }
    parts.extend(fusions);
    Ok(Some(parts.join(", ")))
}

/// Reads a `GenericMatchExpr` union and renders one `MATCH …` clause.
fn read_generic_match(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<Option<String>> {
    ip.read_struct_begin().map_err(tw)?;
    let mut out = None;
    let f = ip.read_field_begin().map_err(tw)?;
    if std::env::var_os("INF_DEBUG_SQL").is_some() {
        eprintln!("[MATCH] field {:?} {:?}", f.id, f.field_type);
    }
    match (f.id, f.field_type) {
        (Some(4), TType::Struct) => out = Some(read_match_text_sql(ip)?),
        (Some(1), TType::Struct) => out = read_knn_sql(ip)?,
        (Some(2), TType::Struct) => out = read_sparse_sql(ip)?,
        (Some(3), TType::Struct) => out = read_tensor_sql(ip)?,
        _ => ip.skip(f.field_type).map_err(tw)?,
    }
    let _ = ip.read_field_end();
    let _ = ip.read_field_begin(); // stop of the union
    ip.read_struct_end().map_err(tw)?;
    Ok(out)
}

/// `MATCH TEXT ('fields', 'text'[, 'options'])`.
fn read_match_text_sql(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<String> {
    ip.read_struct_begin().map_err(tw)?;
    let (mut fields, mut text, mut opts) = (String::new(), String::new(), String::new());
    let mut text_filter: Option<String> = None;
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match f.id {
            Some(1) => fields = ip.read_string().map_err(tw)?,
            Some(2) => text = ip.read_string().map_err(tw)?,
            Some(3) => opts = ip.read_string().map_err(tw)?,
            Some(4) if f.field_type == TType::Struct => text_filter = read_where(ip)?,
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    let tail = if opts.is_empty() {
        String::new()
    } else {
        format!(", '{}'", opts.replace('\'', "''"))
    };
    let filt = text_filter
        .map(|p| format!(", WHERE {p}"))
        .unwrap_or_default();
    Ok(format!(
        "MATCH TEXT ('{}', '{}'{tail}{filt})",
        fields.replace('\'', "''"),
        text.replace('\'', "''")
    ))
}

/// `FUSION('method'[, 'options'])`.
fn read_fusion_expr(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<Option<String>> {
    ip.read_struct_begin().map_err(tw)?;
    let (mut method, mut opts) = (String::new(), String::new());
    let mut tensor: Option<(String, String, i32, String)> = None;
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match f.id {
            Some(1) => method = ip.read_string().map_err(tw)?,
            Some(2) => opts = ip.read_string().map_err(tw)?,
            // `optional_match_tensor_expr`: the fused MaxSim query.
            Some(3) if f.field_type == TType::Struct => tensor = read_tensor_parts(ip)?,
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    if let Some((col, vec, elem, _)) = tensor {
        let extra = format!(
            "column_name={col};search_tensor={vec};tensor_data_type={}",
            knn_element(elem)
        );
        opts = if opts.is_empty() {
            extra
        } else {
            format!("{opts};{extra}")
        };
    }
    if method.is_empty() {
        return Ok(None);
    }
    let tail = if opts.is_empty() {
        String::new()
    } else {
        format!(", '{}'", opts.replace('\'', "''"))
    };
    Ok(Some(format!(
        "FUSION('{}'{tail})",
        method.replace('\'', "''")
    )))
}

/// Reads `EmbeddingData` and renders `[v0,v1,…]`.
fn read_embedding_literal(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<String> {
    ip.read_struct_begin().map_err(tw)?;
    let mut vals: Vec<String> = Vec::new();
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        if let TType::List = f.field_type {
            let l = ip.read_list_begin().map_err(tw)?;
            // The *element* type is in the list header, not the field header.
            let elem_type = l.element_type;
            vals.clear();
            for _ in 0..l.size {
                match elem_type {
                    TType::Bool => vals.push(ip.read_bool().map_err(tw)?.to_string()),
                    TType::I16 => vals.push(ip.read_i16().map_err(tw)?.to_string()),
                    TType::I32 => vals.push(ip.read_i32().map_err(tw)?.to_string()),
                    TType::I64 => vals.push(ip.read_i64().map_err(tw)?.to_string()),
                    TType::Double => vals.push(fmt_number(ip.read_double().map_err(tw)?)),
                    _ => {
                        ip.skip(elem_type).map_err(tw)?;
                    }
                }
            }
            ip.read_list_end().map_err(tw)?;
        } else {
            ip.skip(f.field_type).map_err(tw)?;
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok(format!("[{}]", vals.join(",")))
}

/// A compact decimal for a float literal (always with a fractional part, so
/// the SQL lexer reads it as a float rather than an integer).
fn fmt_number(f: f64) -> String {
    let s = format!("{f}");
    if s.contains('.') || s.contains('e') || s.contains("inf") || s.contains("NaN") {
        s
    } else {
        format!("{s}.0")
    }
}

/// Reads a list of `InitParameter` into `k=v` pairs.
fn read_init_params(
    ip: &mut TBinaryInputProtocol<&mut impl Read>,
) -> Result<Vec<(String, String)>> {
    let l = ip.read_list_begin().map_err(tw)?;
    let mut out: Vec<(String, String)> = Vec::new();
    for _ in 0..l.size {
        let (mut k, mut v) = (String::new(), String::new());
        ip.read_struct_begin().map_err(tw)?;
        loop {
            let f = ip.read_field_begin().map_err(tw)?;
            if f.field_type == TType::Stop {
                break;
            }
            match f.id {
                Some(1) => k = ip.read_string().map_err(tw)?,
                Some(2) => v = ip.read_string().map_err(tw)?,
                _ => ip.skip(f.field_type).map_err(tw)?,
            }
            ip.read_field_end().map_err(tw)?;
        }
        ip.read_struct_end().map_err(tw)?;
        if !k.is_empty() {
            out.push((k, v));
        }
    }
    ip.read_list_end().map_err(tw)?;
    Ok(out)
}

/// Splits `opt_params` into the plain `WITH` options and the index hints the
/// SQL grammar takes outside the parentheses.
fn split_index_hints(params: &[(String, String)]) -> (String, String) {
    let mut opts: Vec<String> = Vec::new();
    let mut hints = String::new();
    for (k, v) in params {
        if k.eq_ignore_ascii_case("index_name") {
            hints.push_str(&format!(" USING INDEX ({v})"));
        } else if k.eq_ignore_ascii_case("ignore_index") {
            if v.eq_ignore_ascii_case("true") {
                hints.push_str(" IGNORE INDEX");
            }
        } else {
            opts.push(format!("{k}={v}"));
        }
    }
    (opts.join(","), hints)
}

fn knn_metric(code: i32) -> &'static str {
    match code {
        0 => "l2",
        1 => "cosine",
        2 => "ip",
        3 => "hamming",
        _ => "l2",
    }
}

fn knn_element(code: i32) -> &'static str {
    // Thrift `ElementType`: 0 bit, 1 uint8, 2 int8, 3 int16, 4 int32,
    // 5 int64, 6 float32, 7 float64, 8 float16, 9 bfloat16.
    match code {
        0 => "bit",
        1 => "uint8",
        2 => "int8",
        3 => "int16",
        4 => "int32",
        5 => "int64",
        6 => "float",
        7 => "double",
        8 => "float16",
        9 => "bfloat16",
        _ => "float",
    }
}

/// `MATCH VECTOR (col, [..], 'elem', 'metric', topn[, 'opts'])`.
fn read_knn_sql(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<Option<String>> {
    ip.read_struct_begin().map_err(tw)?;
    let mut col = String::new();
    let mut vec = String::from("[]");
    let mut elem = 6i32;
    let mut metric = 0i32;
    let mut topn = 10i64;
    let mut params: Vec<(String, String)> = Vec::new();
    let mut knn_filter: Option<String> = None;
    let mut query_expr: Option<String> = None;
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match f.id {
            Some(1) if f.field_type == TType::Struct => {
                let (names, _) = read_column_expr(ip)?;
                col = names.into_iter().next().unwrap_or_default();
            }
            Some(2) if f.field_type == TType::Struct => vec = read_embedding_literal(ip)?,
            Some(3) => elem = ip.read_i32().map_err(tw)?,
            Some(4) => metric = ip.read_i32().map_err(tw)?,
            Some(5) => topn = ip.read_i64().map_err(tw)?,
            Some(6) if f.field_type == TType::List => params = read_init_params(ip)?,
            Some(7) if f.field_type == TType::Struct => knn_filter = read_where(ip)?,
            Some(8) if f.field_type == TType::Struct => {
                let (name, args) = read_function_expr(ip)?;
                query_expr = Some(pexpr_sql(&PExpr::Call(name, args)));
            }
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    if col.is_empty() {
        return Ok(None);
    }
    // `index_name` and `ignore_index` are mutually exclusive.
    let named = params
        .iter()
        .any(|(k, _)| k.eq_ignore_ascii_case("index_name"));
    let ignoring = params
        .iter()
        .any(|(k, v)| k.eq_ignore_ascii_case("ignore_index") && v.eq_ignore_ascii_case("true"));
    let vector = query_expr.unwrap_or(vec);
    if named && ignoring {
        // Reject it in the *engine* (not while decoding), so the whole request
        // is consumed first and the connection stays usable for the next call.
        let name = params
            .iter()
            .find(|(k, _)| k.eq_ignore_ascii_case("index_name"))
            .map(|(_, v)| v.clone())
            .unwrap_or_default();
        return Ok(Some(format!(
            "MATCH VECTOR ({col}, {vector}, '{}', '{}', {topn}, 'index_name={};ignore_index=true')",
            knn_element(elem),
            knn_metric(metric),
            name.replace('\'', "''")
        )));
    }
    let (opts, hints) = split_index_hints(&params);
    let tail = if opts.is_empty() {
        String::new()
    } else {
        format!(", '{}'", opts.replace('\'', "''"))
    };
    let filt = knn_filter
        .map(|p| format!(", WHERE {p}"))
        .unwrap_or_default();
    Ok(Some(format!(
        "MATCH VECTOR ({col}, {vector}, '{}', '{}', {topn}{tail}{filt}){hints}",
        knn_element(elem),
        knn_metric(metric)
    )))
}

/// `MATCH SPARSE (col, {i:v,…}, 'metric', topn[, 'opts'])`.
fn read_sparse_sql(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<Option<String>> {
    ip.read_struct_begin().map_err(tw)?;
    let mut col = String::new();
    let mut sparse = String::from("{}");
    let mut metric = String::new();
    let mut topn = 10i64;
    let mut params: Vec<(String, String)> = Vec::new();
    let mut sparse_filter: Option<String> = None;
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match f.id {
            Some(1) if f.field_type == TType::Struct => {
                let (names, _) = read_column_expr(ip)?;
                col = names.into_iter().next().unwrap_or_default();
            }
            Some(2) if f.field_type == TType::Struct => {
                sparse = read_constant_expr(ip)?.to_sql();
            }
            Some(3) => metric = ip.read_string().map_err(tw)?,
            Some(4) => topn = ip.read_i64().map_err(tw)?,
            Some(5) if f.field_type == TType::List => params = read_init_params(ip)?,
            Some(6) if f.field_type == TType::Struct => sparse_filter = read_where(ip)?,
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    if col.is_empty() {
        return Ok(None);
    }
    let (opts, hints) = split_index_hints(&params);
    let tail = if opts.is_empty() {
        String::new()
    } else {
        format!(", '{}'", opts.replace('\'', "''"))
    };
    let filt = sparse_filter
        .map(|p| format!(", WHERE {p}"))
        .unwrap_or_default();
    Ok(Some(format!(
        "MATCH SPARSE ({col}, {sparse}, '{metric}', {topn}{tail}{filt}){hints}"
    )))
}

/// Reads a `MatchTensorExpr` as `(column, [vector], element type, options)`.
#[allow(clippy::type_complexity)]
fn read_tensor_parts(
    ip: &mut TBinaryInputProtocol<&mut impl Read>,
) -> Result<Option<(String, String, i32, String)>> {
    ip.read_struct_begin().map_err(tw)?;
    let mut method = String::new();
    let mut col = String::new();
    let mut elem = 6i32;
    let mut vec = String::from("[]");
    let mut opts = String::new();
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match f.id {
            Some(1) => method = ip.read_string().map_err(tw)?,
            Some(2) if f.field_type == TType::Struct => {
                let (names, _) = read_column_expr(ip)?;
                col = names.into_iter().next().unwrap_or_default();
            }
            Some(3) => elem = ip.read_i32().map_err(tw)?,
            Some(4) if f.field_type == TType::Struct => vec = read_embedding_literal(ip)?,
            Some(5) => opts = ip.read_string().map_err(tw)?,
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    if col.is_empty() {
        return Ok(None);
    }
    let _ = method; // the search method is passed positionally in the SQL
    Ok(Some((col, vec, elem, opts)))
}

/// `MATCH TENSOR (col, [..], 'elem', 'method'[, 'opts'])`.
fn read_tensor_sql(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<Option<String>> {
    let Some((col, vec, elem, opts)) = read_tensor_parts(ip)? else {
        return Ok(None);
    };
    let _ = &opts;
    let tail = if opts.is_empty() {
        String::new()
    } else {
        format!(", '{}'", opts.replace('\'', "''"))
    };
    Ok(Some(format!(
        "MATCH TENSOR ({col}, {vec}, '{}', 'maxsim'{tail})",
        knn_element(elem)
    )))
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
    // A ColumnExpr/ConstantExpr is not a predicate ("filter('c1')").
    let mut not_predicate = false;
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
                // Either side may be an expression (`sqrt(c1) = 2`), and the
                // connectives arrive as functions too.
                match (sql_op(&name), &args[..]) {
                    (Some(op), [l, r]) => {
                        pred = Some(format!("{} {op} {}", pexpr_sql(l), pexpr_sql(r)))
                    }
                    // `c1 IN (1, 2, 3)` arrives as `in(c1, 1, 2, 3)`.
                    (None, [head, rest @ ..])
                        if name.eq_ignore_ascii_case("in") && !rest.is_empty() =>
                    {
                        let list = rest.iter().map(pexpr_sql).collect::<Vec<_>>().join(", ");
                        let op = if name.eq_ignore_ascii_case("not_in") {
                            "NOT IN"
                        } else {
                            "IN"
                        };
                        pred = Some(format!("({} {op} ({list}))", pexpr_sql(head)));
                    }
                    (None, [only]) if name.eq_ignore_ascii_case("not") => {
                        pred = Some(format!("(NOT {})", pexpr_sql(only)));
                    }
                    // `IS [NOT] NULL` arrive as `is_null`/`is_not_null`.
                    (None, [only]) if name.eq_ignore_ascii_case("is_null") => {
                        pred = Some(format!("({} IS NULL)", pexpr_sql(only)));
                    }
                    (None, [only]) if name.eq_ignore_ascii_case("is_not_null") => {
                        pred = Some(format!("({} IS NOT NULL)", pexpr_sql(only)));
                    }
                    (None, [l, r])
                        if (name.eq_ignore_ascii_case("and")
                            || name.eq_ignore_ascii_case("or"))
                            && is_predicate_pexpr(l)
                            && is_predicate_pexpr(r) =>
                    {
                        let joiner = name.to_ascii_lowercase();
                        pred = Some(format!("({} {joiner} {})", pexpr_sql(l), pexpr_sql(r)));
                    }
                    // A boolean-valued function (`regex(c1, 'bc')`) is a
                    // predicate on its own.
                    (None, _) if is_boolean_fn(&name) => {
                        pred = Some(pexpr_sql(&PExpr::Call(name.clone(), args.clone())));
                    }
                    _ => {
                        // A non-boolean function (`_row_id()`, `*`, …) — report
                        // it after the request is fully read.
                        not_predicate = true;
                    }
                }
            } else if uf.id == Some(11) && uf.field_type == TType::Struct {
                // `InExpr { 1: left_operand, 2: arguments, 3: in_type }` —
                // `c1 IN (1, 2, 3)` / `NOT IN`.
                ip.read_struct_begin().map_err(tw)?;
                let mut left = PExpr::Other;
                let mut vals: Vec<PExpr> = Vec::new();
                let mut in_type = true;
                loop {
                    let g = ip.read_field_begin().map_err(tw)?;
                    if g.field_type == TType::Stop {
                        break;
                    }
                    match (g.id, g.field_type) {
                        (Some(1), TType::Struct) => left = read_pexpr(ip)?,
                        (Some(2), TType::List) => {
                            let l = ip.read_list_begin().map_err(tw)?;
                            for _ in 0..l.size {
                                vals.push(read_pexpr(ip)?);
                            }
                            ip.read_list_end().map_err(tw)?;
                        }
                        (Some(3), TType::Bool) => in_type = ip.read_bool().map_err(tw)?,
                        _ => ip.skip(g.field_type).map_err(tw)?,
                    }
                    ip.read_field_end().map_err(tw)?;
                }
                ip.read_struct_end().map_err(tw)?;
                let list = vals.iter().map(pexpr_sql).collect::<Vec<_>>().join(", ");
                let op = if in_type { "IN" } else { "NOT IN" };
                pred = Some(format!("({} {op} ({list}))", pexpr_sql(&left)));
            } else {
                // A bare column / star is not a predicate; report it after the
                // whole request is read (see NON_BOOLEAN_FILTER).
                not_predicate = true;
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
    if not_predicate && pred.is_none() {
        // The whole request is already consumed; the caller turns this marker
        // into a 7012 response without desynchronising the stream.
        return Ok(Some(NON_BOOLEAN_FILTER.to_string()));
    }
    Ok(pred)
}

/// Marker for a filter that is not a boolean expression.
const NON_BOOLEAN_FILTER: &str = "__NON_BOOLEAN_FILTER__";

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
#[allow(clippy::type_complexity)] // (sql, display, raw, star)
fn read_select_item(
    ip: &mut TBinaryInputProtocol<&mut impl Read>,
    bad: &mut Vec<String>,
) -> Result<(Vec<String>, Vec<String>, Vec<String>, bool)> {
    ip.read_struct_begin().map_err(tw)?; // ParsedExpr
    let (mut names, mut display, mut raw, mut star) = (Vec::new(), Vec::new(), Vec::new(), false);
    let mut alias: Option<String> = None;
    loop {
        let fd = ip.read_field_begin().map_err(tw)?;
        if fd.field_type == TType::Stop {
            break;
        }
        if fd.id == Some(2) && fd.field_type == TType::String {
            // `ParsedExpr.alias_name`: `sum(c1) AS sum1`.
            alias = Some(ip.read_string().map_err(tw)?);
        } else if fd.id == Some(1) {
            ip.read_struct_begin().map_err(tw)?; // ParsedExprType union
            let uf = ip.read_field_begin().map_err(tw)?;
            if uf.id == Some(2) && uf.field_type == TType::Struct {
                let (n, s) = read_column_expr(ip)?;
                display.extend(n.iter().cloned());
                raw.extend(n.iter().cloned());
                names = n;
                star = s;
            } else if uf.id == Some(3) && uf.field_type == TType::Struct {
                // A projected function: send it to the engine as expression text
                // (`year(c1)`), which is also the output column name.
                let (name, args) = read_function_expr(ip)?;
                // A projection like `c1 + 0.1 AND c2 - 1.0` is a type error.
                // Record it and keep parsing: bailing out here would leave the
                // request half-read and desynchronise the connection.
                if (name.eq_ignore_ascii_case("and") || name.eq_ignore_ascii_case("or"))
                    && !args.iter().all(is_predicate_pexpr)
                {
                    bad.push("parser: AND/OR operands must be boolean expressions".to_string());
                }
                let e = PExpr::Call(name, args);
                names.push(pexpr_sql(&e));
                display.push(pexpr_name(&e));
                raw.push(pexpr_name_raw(&e));
            } else if uf.id == Some(12) && uf.field_type == TType::Struct {
                // A cast: the engine gets SQL, the client the C++ `Name()`
                // spelling (`Cast(c3 AS Varchar)`).
                let e = read_cast_pexpr(ip)?;
                names.push(pexpr_sql(&e));
                display.push(pexpr_name(&e));
                raw.push(pexpr_name_raw(&e));
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
    // An empty `alias_name` is not an alias.
    let alias = alias.filter(|a| !a.trim().is_empty());
    if let Some(alias) = alias {
        if !names.is_empty() {
            if let Some(sql) = names.last_mut() {
                *sql = format!("{sql} AS {alias}");
            }
        }
        if let Some(d) = display.last_mut() {
            *d = alias.clone();
        }
        if let Some(r) = raw.last_mut() {
            *r = alias;
        }
    }
    Ok((names, display, raw, star))
}

/// Reads an `order_by_list` (`list<OrderByExpr{1: ParsedExpr, 2: bool asc}>`),
/// returning the first `(column, ascending)`.
fn read_order_by(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<Vec<(String, bool)>> {
    let l = ip.read_list_begin().map_err(tw)?;
    let mut out: Vec<(String, bool)> = Vec::new();
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
        if !col.is_empty() {
            out.push((col, asc));
        }
    }
    ip.read_list_end().map_err(tw)?;
    Ok(out)
}

/// Reads the args of a `Select` CALL (after the message head). An empty
/// `select_list` is treated as `SELECT *`.
pub fn read_select_args(
    ip: &mut TBinaryInputProtocol<&mut impl Read>,
) -> Result<(SelectReq, Vec<String>)> {
    ip.read_struct_begin().map_err(tw)?; // Select_args
    let _ = ip.read_field_begin().map_err(tw)?; // request (Struct)
    ip.read_struct_begin().map_err(tw)?; // SelectRequest
                                         // Projections that are type errors, collected while the whole request is
                                         // still read (so the connection stays in sync).
    let mut bad: Vec<String> = Vec::new();
    let mut r = SelectReq {
        session_id: 0,
        db_name: String::new(),
        table_name: String::new(),
        columns: Vec::new(),
        names: Vec::new(),
        raw_names: Vec::new(),
        star: false,
        where_sql: None,
        order_by: Vec::new(),
        group_by: Vec::new(),
        having_sql: None,
        limit: None,
        offset: None,
        total_hits: false,
        aliases: Vec::new(),
        highlight: Vec::new(),
        search_sql: None,
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
                    let (names, display, raw, star) = read_select_item(ip, &mut bad)?;
                    r.columns.extend(names);
                    r.names.extend(display);
                    r.raw_names.extend(raw);
                    r.star |= star;
                }
                ip.read_list_end().map_err(tw)?;
            }
            Some(5) if f.field_type == TType::List => {
                let l = ip.read_list_begin().map_err(tw)?;
                for _ in 0..l.size {
                    if let PExpr::Column(c) = read_pexpr(ip)? {
                        r.highlight.push(c);
                    }
                }
                ip.read_list_end().map_err(tw)?;
            }
            Some(6) if f.field_type == TType::Struct => r.search_sql = read_search_expr(ip)?,
            Some(7) if f.field_type == TType::Struct => {
                r.where_sql = read_where(ip)?;
                if r.where_sql.as_deref() == Some(NON_BOOLEAN_FILTER) {
                    bad.push("parser: the filter must be a boolean expression".to_string());
                    r.where_sql = None;
                }
            }
            Some(8) if f.field_type == TType::List => {
                // group_by_list: one ParsedExpr per group key.
                let l = ip.read_list_begin().map_err(tw)?;
                for _ in 0..l.size {
                    let e = read_pexpr(ip)?;
                    let sql = pexpr_sql(&e);
                    if !sql.is_empty() {
                        r.group_by.push(sql);
                    }
                }
                ip.read_list_end().map_err(tw)?;
            }
            Some(9) if f.field_type == TType::Struct => {
                r.having_sql = read_where(ip)?;
                if r.having_sql.as_deref() == Some(NON_BOOLEAN_FILTER) {
                    bad.push("parser: the filter must be a boolean expression".to_string());
                    r.having_sql = None;
                }
            }
            Some(10) if f.field_type == TType::Struct => {
                if let PExpr::Value(ValueLit::Int(n)) = read_pexpr(ip)? {
                    r.limit = Some(n);
                }
            }
            Some(11) if f.field_type == TType::Struct => {
                if let PExpr::Value(ValueLit::Int(n)) = read_pexpr(ip)? {
                    r.offset = Some(n);
                }
            }
            Some(13) if f.field_type == TType::Bool => {
                r.total_hits = ip.read_bool().map_err(tw)?;
            }
            Some(12) => r.order_by = read_order_by(ip)?,
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?; // SelectRequest
    end_args(ip)?; // Select_args
    if !r.star && r.columns.is_empty() {
        // 3050 EMPTY_SELECT_FIELDS: `output([])` must not degrade into `*`.
        return Err(Error::Invalid("empty select fields".into()));
    }
    // `SELECT expr AS alias` lets WHERE/ORDER BY/HVING say `alias`.
    for c in &r.columns {
        if let Some((expr, alias)) = c.rsplit_once(" AS ") {
            r.aliases
                .push((alias.trim().to_string(), expr.trim().to_string()));
        }
    }
    // A projection that is also a group key keeps the client's spelling.
    for (i, sql) in r.columns.iter().enumerate() {
        if !r.group_by.iter().any(|g| g.eq_ignore_ascii_case(sql)) {
            continue;
        }
        if let (Some(raw), true) = (r.raw_names.get(i).cloned(), i < r.names.len()) {
            r.names[i] = raw;
        }
    }
    Ok((r, bad))
}

/// Reads a full `Select` CALL, returning `(seqid, request)`.
pub fn read_select_call(r: &mut impl Read) -> Result<(i32, SelectReq)> {
    let mut ip = TBinaryInputProtocol::new(r, true);
    let msg = ip.read_message_begin().map_err(tw)?;
    let (req, _) = read_select_args(&mut ip)?;
    Ok((msg.sequence_number, req))
}

/// Text rows as `Value::Str` cells: the SHOW builders still produce text.
fn text_cells(rows: Vec<Vec<String>>) -> Vec<Vec<Value>> {
    rows.into_iter()
        .map(|r| r.into_iter().map(Value::Str).collect())
        .collect()
}

/// Builds a `SelectResponse` from a materialized result set (no declared types).
pub fn select_response_from(columns: &[String], rows: &[Vec<String>]) -> SelectResponseWire {
    select_response_from_typed(columns, &[], &text_cells(rows.to_vec()))
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
    // Straight from the catalog, not by running `SHOW TABLE ... COLUMNS` through
    // the executor: that executed a second statement (its own parse, bind, plan
    // and catalog scan) inside every `Select` RPC, which `perf` showed as 5% of
    // the query. The catalog read is the same data without the round trip.
    if let Ok(Some(meta)) = engine.catalog().get_table_by_name(&qualified) {
        for (name, ty) in meta.columns.iter().zip(meta.types.iter()) {
            map.insert(name.to_ascii_lowercase(), ty.clone());
        }
    }
    columns
        .iter()
        .map(|c| {
            let key = c.rsplit('.').next().unwrap_or(c).to_ascii_lowercase();
            // `unnest(col)` yields the array's element type, not the array.
            let inner = c
                .trim()
                .strip_prefix("unnest(")
                .and_then(|r| r.strip_suffix(')'))
                .map(|r| r.trim())
                .unwrap_or("");
            if !inner.is_empty() {
                if let Some(t) = map.get(&inner.to_ascii_lowercase()) {
                    let up = t.to_ascii_uppercase();
                    if let Some(rest) = up.strip_prefix("ARRAY(") {
                        return rest.trim_end_matches(')').to_string();
                    }
                }
            }
            if let Some(t) = map.get(&key) {
                return t.clone();
            }
            // Pseudo-columns: 32-bit scores, 64-bit row id.
            if matches!(key.as_str(), "score" | "distance" | "similarity") {
                return "FLOAT".to_string();
            }
            if key == "row_id" {
                return "BIGINT".to_string();
            }
            // `Cast(x AS T)` is announced as `T`.
            if let Some(rest) = key
                .strip_prefix("cast(")
                .or_else(|| key.strip_prefix("try_cast("))
            {
                if let Some((_, ty)) = rest.rsplit_once(" as ") {
                    return ty.trim_end_matches(')').trim().to_ascii_uppercase();
                }
            }
            // An aggregate result takes its argument's type, widening the sum
            // (and count/avg) to 64 bits, exactly as the C++ engine does.
            if let Some((func, arg)) = key.strip_suffix(')').and_then(|s| s.split_once('(')) {
                let arg = arg.trim();
                let base = map.get(arg).cloned().unwrap_or_default();
                let up = base.to_ascii_uppercase();
                let is_float = up.contains("FLOAT") || up.contains("DOUBLE") || up.contains("REAL");
                let is_int = up.contains("INT") || up.contains("HUGEINT");
                return match func.trim().to_ascii_lowercase().as_str() {
                    "count" => "BIGINT".to_string(),
                    "avg" => "DOUBLE".to_string(),
                    "sum" if is_float => "DOUBLE".to_string(),
                    "sum" if is_int => "BIGINT".to_string(),
                    // Date parts and string metrics are integers.
                    "year" | "month" | "day" | "hour" | "minute" | "second" | "dayofmonth"
                    | "dayofyear" | "dayofweek" | "weekday" | "week" | "weekofyear" | "quarter"
                    | "century" | "decade" | "epoch" | "era" | "length" | "position" | "strpos" => {
                        "BIGINT".to_string()
                    }
                    // `char_length` is a 32-bit count in the reference
                    // (`UnaryFunctionWithFailure<VarcharT, IntegerT, ...>`).
                    "char_length" | "character_length" => "INTEGER".to_string(),
                    // Text functions return text.
                    "substring"
                    | "substr"
                    | "trim"
                    | "ltrim"
                    | "rtrim"
                    | "upper"
                    | "lower"
                    | "concat"
                    | "replace"
                    | "lpad"
                    | "rpad"
                    | "reverse"
                    | "md5"
                    | "json_extract"
                    | "json_extract_string" => "VARCHAR".to_string(),
                    // JSON extractors return their declared target type.
                    "json_extract_int" => "BIGINT".to_string(),
                    "json_extract_double" | "json_extract_float" => "DOUBLE".to_string(),
                    "json_extract_bool"
                    | "json_extract_isnull"
                    | "json_exists_path"
                    | "json_contains" => "BOOLEAN".to_string(),
                    // `trunc` formats the value as text at a fixed precision,
                    // so it is the one numeric-looking function returning
                    // VARCHAR (`TruncFunction ... -> DataType(kVarchar)`).
                    "trunc" | "truncate" => "VARCHAR".to_string(),
                    // Scalar numeric functions.
                    // `round`/`abs` keep the argument's type; the
                    // transcendental functions widen to DOUBLE.
                    "sqrt" | "pow" | "power" | "exp" | "ln" | "log" | "log2" | "log10" | "sin"
                    | "cos" | "tan" => "DOUBLE".to_string(),
                    // Pseudo-columns are 32-bit scores.
                    "score" | "distance" | "similarity" => "FLOAT".to_string(),
                    // `min`/`max` keep the argument's type.
                    _ => base,
                };
            }
            String::new()
        })
        .collect()
}

// ---------------------------------------------------------------------------
// `SHOW SEGMENTS` / `SHOW BLOCKS` metadata
// (C++ `physical_show_segments_impl.cpp`, `physical_show_blocks_impl.cpp`)
// ---------------------------------------------------------------------------

/// `Utility::FormatByteSize` (`src/common/utility/utility_impl.cpp:40`): `0`
/// renders as `0B`, everything else as `%.2f` with a binary suffix.
pub fn format_byte_size(byte_size: u64) -> String {
    const SUFFIXES: [&str; 9] = ["B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"];
    if byte_size == 0 {
        return "0B".to_string();
    }
    let mut index = 0usize;
    let mut size = byte_size as f64;
    while size >= 1024.0 && index + 1 < SUFFIXES.len() {
        size /= 1024.0;
        index += 1;
    }
    format!("{size:.2}{}", SUFFIXES[index])
}

/// Bytes held by the segment and block directories of `table`'s C++ mirror
/// (`data/db_x/tbl_y/seg_z/blk_n/{column}.col`).
///
/// The reference sums the sizes of the files a `SegmentInfo`/`BlockInfo`
/// carries (`physical_show_blocks_impl.cpp:60`, `Utility::FormatByteSize`).
pub fn segment_block_bytes(
    engine: &Engine,
    table: &str,
) -> (HashMap<u32, u64>, HashMap<(u32, u32), u64>) {
    let mut segments: HashMap<u32, u64> = HashMap::new();
    let mut blocks: HashMap<(u32, u32), u64> = HashMap::new();
    for path in engine.table_files(table) {
        let bytes = std::fs::metadata(&path).map(|m| m.len()).unwrap_or(0);
        let parts: Vec<String> = path
            .components()
            .map(|c| c.as_os_str().to_string_lossy().into_owned())
            .collect();
        let Some(segment) = parts
            .iter()
            .find_map(|c| c.strip_prefix("seg_").and_then(|v| v.parse::<u32>().ok()))
        else {
            // `idx_*` mirror directories have no block layout.
            continue;
        };
        *segments.entry(segment).or_insert(0) += bytes;
        if let Some(block) = parts
            .iter()
            .find_map(|c| c.strip_prefix("blk_").and_then(|v| v.parse::<u32>().ok()))
        {
            *blocks.entry((segment, block)).or_insert(0) += bytes;
        }
    }
    (segments, blocks)
}

/// The column set of `SHOW SEGMENTS`, in the reference's order
/// (`physical_show_impl.cpp:112`: `id`, `status`, `size`).
///
/// `row_count` is appended: the reference drops it from the list form only
/// because its `status`/`size` cells are stubs (`"No value"` / `"TODO"`),
/// while `SHOW SEGMENT <id>` does report it
/// (`physical_show_segment_detail_impl.cpp`).
pub const SHOW_SEGMENTS_COLUMNS: [&str; 4] = ["id", "status", "size", "row_count"];
pub const SHOW_SEGMENTS_TYPES: [&str; 4] = ["bigint", "varchar", "varchar", "bigint"];

/// The column set of `SHOW BLOCKS` (`physical_show_impl.cpp:130`).
pub const SHOW_BLOCKS_COLUMNS: [&str; 3] = ["id", "size", "row_count"];
pub const SHOW_BLOCKS_TYPES: [&str; 3] = ["bigint", "varchar", "bigint"];

/// Rows of `SHOW SEGMENTS`: one row per live segment, with its real row count
/// and the bytes its directory occupies.
pub fn show_segments_rows(engine: &Engine, table: &str) -> Vec<Vec<String>> {
    let (segment_bytes, _) = segment_block_bytes(engine, table);
    engine
        .segments(table)
        .into_iter()
        .map(|(segment_id, rows)| {
            vec![
                segment_id.to_string(),
                // `physical_show_segments_impl.cpp:37`: the status cell reads
                // the removed `SegmentEntry::status_`, so the reference always
                // prints this string. Reporting it keeps the column truthful
                // instead of inventing a sealed/unsealed state the engine does
                // not track per segment.
                "No value".to_string(),
                format_byte_size(segment_bytes.get(&segment_id).copied().unwrap_or(0)),
                rows.to_string(),
            ]
        })
        .collect()
}

/// Rows of `SHOW BLOCKS <segment>`: the segment's real blocks
/// (`BlockIndex`/`BlockMeta`) with their row counts and on-disk sizes.
///
/// Blocks exist once the C++-format mirror has been written (a flush or
/// checkpoint), exactly as in the reference where blocks appear at dump time.
pub fn show_blocks_rows(engine: &Engine, table: &str, segment: u32) -> Vec<Vec<String>> {
    let (_, block_bytes) = segment_block_bytes(engine, table);
    engine
        .block_ids(table, segment)
        .into_iter()
        .map(|block_id| {
            let rows = engine
                .block_row_count(table, segment, block_id as u16)
                .unwrap_or(0);
            vec![
                block_id.to_string(),
                format_byte_size(block_bytes.get(&(segment, block_id)).copied().unwrap_or(0)),
                rows.to_string(),
            ]
        })
        .collect()
}

/// Builds a `SelectResponse` with declared column type *text* available, so
/// non-scalar columns (embedding/vector/...) are announced with their physical
/// descriptor and packed as their live element stream.
pub fn select_response_from_typed(
    columns: &[String],
    types: &[String],
    rows: &[Vec<Value>],
) -> SelectResponseWire {
    let column_defs = columns
        .iter()
        .enumerate()
        .map(|(i, name)| {
            let declared = types.get(i).map(String::as_str).unwrap_or("");
            if std::env::var_os("INF_DEBUG_SQL").is_some() {
                eprintln!(
                    "[COLDEF] name={name} declared={declared:?} value={:?}",
                    rows.first().and_then(|r| r.get(i))
                );
            }
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
                comment: String::new(),
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

/// The wire `ColumnDef` of result column `i`.
fn column_def_of(columns: &[String], types: &[String], i: usize, kind: ColKind) -> ColumnDefWire {
    let name = &columns[i];
    let declared = types.get(i).map(String::as_str).unwrap_or("");
    let (logic_type, physical) = match kind {
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
        comment: String::new(),
    }
}

/// The result column's kind when the rows are typed column buffers.
fn classify_batch_column(declared: &str, batch: &Batch, i: usize) -> ColKind {
    let kind = classify_column(declared, &[], i);
    if declared.trim().is_empty() {
        if let ColKind::Scalar(_) = kind {
            return ColKind::Scalar(match batch.data.get(i) {
                Some(ColumnData::Int(_)) => 3,
                Some(ColumnData::Float(_)) => 6,
                Some(ColumnData::Bool(_)) => 0,
                Some(ColumnData::Text(_)) | None => 9,
            });
        }
    }
    kind
}

/// Packs typed column buffers into `ColumnField`s: one contiguous chunk per
/// column, no per-cell `Value` and no text round trip.
pub fn column_fields_from_batch(
    columns: &[String],
    types: &[String],
    batch: &Batch,
) -> Vec<ColumnField> {
    let mut out = Vec::with_capacity(columns.len());
    for (i, name) in columns.iter().enumerate() {
        let declared = types.get(i).map(String::as_str).unwrap_or("");
        let code = match classify_batch_column(declared, batch, i) {
            ColKind::Scalar(code) => code,
            // The batch path only ever holds scalars and text.
            _ => 9,
        };
        let valid = batch.valid.get(i).cloned().unwrap_or_default();
        let mut chunk: Vec<u8> = Vec::with_capacity(batch.row_count * 8);
        match batch.data.get(i) {
            Some(ColumnData::Int(v)) => {
                for n in v {
                    match code {
                        1 => chunk.push(*n as i8 as u8),
                        2 => chunk.extend_from_slice(&(*n as i16).to_le_bytes()),
                        3 => chunk.extend_from_slice(&(*n as i32).to_le_bytes()),
                        16 => chunk.extend_from_slice(&(*n as i32).to_le_bytes()),
                        17 => chunk.extend_from_slice(&(*n as i32).to_le_bytes()),
                        18 => chunk.extend_from_slice(&(*n as i32).to_le_bytes()),
                        _ => chunk.extend_from_slice(&n.to_le_bytes()),
                    }
                }
            }
            Some(ColumnData::Float(v)) => {
                for f in v {
                    match code {
                        5 => chunk.extend_from_slice(&(*f as f32).to_le_bytes()),
                        7 => chunk.extend_from_slice(&f32_to_f16_bits(*f as f32).to_le_bytes()),
                        8 => chunk.extend_from_slice(
                            &(((*f as f32).to_bits() >> 16) as u16).to_le_bytes(),
                        ),
                        _ => chunk.extend_from_slice(&f.to_le_bytes()),
                    }
                }
            }
            Some(ColumnData::Bool(v)) => {
                for b in v {
                    chunk.push(u8::from(*b));
                }
            }
            Some(ColumnData::Text(v)) => {
                // The column is one buffer plus offsets; each cell is a slice of
                // it, so packing is a length prefix and a copy.
                for i in 0..v.len() {
                    let s = v.get(i);
                    match code {
                        16 => chunk.extend_from_slice(&date_to_days(s).unwrap_or(0).to_le_bytes()),
                        17 => chunk.extend_from_slice(&time_to_secs(s).unwrap_or(0).to_le_bytes()),
                        18 => {
                            let (d, t) = datetime_to_pair(s).unwrap_or((0, 0));
                            chunk.extend_from_slice(&d.to_le_bytes());
                            chunk.extend_from_slice(&t.to_le_bytes());
                        }
                        _ => {
                            chunk.extend_from_slice(&(s.len() as u32).to_le_bytes());
                            chunk.extend_from_slice(s.as_bytes());
                        }
                    }
                }
            }
            None => {}
        }
        out.push(ColumnField {
            column_type: code,
            column_name: name.clone(),
            column_vectors: vec![chunk],
            bitmasks: if valid.iter().all(|v| *v) {
                Vec::new()
            } else {
                valid
            },
        });
    }
    out
}

/// A `SelectResponse` built from typed column buffers.
pub fn select_response_from_batch(
    columns: &[String],
    types: &[String],
    batch: &Batch,
) -> SelectResponseWire {
    let column_defs = columns
        .iter()
        .enumerate()
        .map(|(i, _)| {
            column_def_of(
                columns,
                types,
                i,
                classify_batch_column(types.get(i).map(String::as_str).unwrap_or(""), batch, i),
            )
        })
        .collect();
    SelectResponseWire {
        error_code: 0,
        error_msg: "OK".into(),
        column_defs,
        column_fields: column_fields_from_batch(columns, types, batch),
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
    /// The C++ `ConstantExpr::ToString()` spelling: strings are unquoted.
    pub fn to_display(&self) -> String {
        match self {
            ValueLit::Int(i) => i.to_string(),
            ValueLit::Float(f) => f.to_string(),
            ValueLit::Str(s) => s.clone(),
            ValueLit::Bool(b) => b.to_string(),
            ValueLit::Null => "Null".to_string(),
            other => other.to_sql(),
        }
    }

    pub fn to_sql(&self) -> String {
        match self {
            ValueLit::Int(i) => i.to_string(),
            // A float must render as a *float* literal. `f.to_string()` renders
            // `-0.0` as `-0`, and the lexer reads `-0` as an **integer** (`-0`
            // has no '.', so it becomes `Token::Int(0)`) - the sign of a
            // negative zero is then lost, and the reference stores the f32
            // `-0.0` (0x800000) where the port stored `+0.0` (0x000000).
            // `fmt_number` keeps the fractional part, so the lexer sees a
            // float; it is the same helper the array path uses.
            ValueLit::Float(f) => fmt_number(*f),
            ValueLit::Str(s) => format!("'{}'", s.replace('\'', "''")),
            ValueLit::Bool(b) => {
                if *b {
                    "true".to_string()
                } else {
                    "false".to_string()
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
        // Date(13)/Time(14)/Interval(15)/DateTime(16)/Timestamp(17): the SDK
        // carries the rendered text, which the engine parses back.
        13..=17 => match (s, i) {
            (Some(text), _) => ValueLit::Str(text),
            (None, Some(days)) => ValueLit::Int(days),
            _ => ValueLit::Null,
        },
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
#[derive(Debug)]
pub struct CreateIndexReq {
    pub db_name: String,
    pub table_name: String,
    pub index_name: String,
    pub index_comment: String,
    pub column: String,
    pub index_type: String,
    pub params: Vec<(String, String)>,
    /// `IndexInfo.function_expr`: a functional index's expression text.
    pub function: Option<String>,
    pub conflict: i64,
    pub session_id: i64,
}

/// The engine's index-kind name for the thrift `IndexType` ordinal.
fn index_type_sql(code: i32) -> &'static str {
    match code {
        0 => "IVF",
        1 => "HNSW",
        2 => "FULLTEXT",
        3 => "BMP",
        // functional secondary indexes share the SECONDARY kind
        4 | 5 => "SECONDARY",
        6 => "EMVB",
        7 => "DISKANN",
        8 => "PLAID",
        9 => "SMVE",
        _ => "",
    }
}

/// Reads an `IndexInfo` struct: column, index type, parameters, expression.
fn read_index_info(
    ip: &mut TBinaryInputProtocol<&mut impl Read>,
    out: &mut CreateIndexReq,
) -> Result<()> {
    ip.read_struct_begin().map_err(tw)?;
    loop {
        let f = ip.read_field_begin().map_err(tw)?;
        if f.field_type == TType::Stop {
            break;
        }
        match f.id {
            Some(1) if f.field_type == TType::String => {
                out.column = ip.read_string().map_err(tw)?;
            }
            Some(2) if f.field_type == TType::I32 => {
                // `IndexType` is an enum on the wire.
                out.index_type = index_type_sql(ip.read_i32().map_err(tw)?).to_string();
            }
            Some(3) if f.field_type == TType::List => {
                let l = ip.read_list_begin().map_err(tw)?;
                for _ in 0..l.size {
                    let mut k = String::new();
                    let mut v = String::new();
                    ip.read_struct_begin().map_err(tw)?;
                    loop {
                        let g = ip.read_field_begin().map_err(tw)?;
                        if g.field_type == TType::Stop {
                            break;
                        }
                        match g.id {
                            Some(1) => k = ip.read_string().map_err(tw)?,
                            Some(2) => v = ip.read_string().map_err(tw)?,
                            _ => ip.skip(g.field_type).map_err(tw)?,
                        }
                        ip.read_field_end().map_err(tw)?;
                    }
                    ip.read_struct_end().map_err(tw)?;
                    out.params.push((k, v));
                }
                ip.read_list_end().map_err(tw)?;
            }
            Some(4) if f.field_type == TType::Struct => {
                // A functional index carries a bare `FunctionExpr`; some clients
                // send an empty one, which is not an expression.
                let (name, args) = read_function_expr(ip)?;
                if !name.trim().is_empty() {
                    out.function = Some(pexpr_sql(&PExpr::Call(name, args)));
                }
            }
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    Ok(())
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
        index_comment: String::new(),
        column: String::new(),
        index_type: String::new(),
        params: Vec::new(),
        function: None,
        conflict: 1,
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
            Some(4) if f.field_type == TType::String => {
                r.index_comment = ip.read_string().map_err(tw)?;
            }
            Some(5) if f.field_type == TType::Struct => read_index_info(ip, &mut r)?,
            Some(6) => r.session_id = ip.read_i64().map_err(tw)?,
            Some(7) if f.field_type == TType::Struct => {
                // `CreateOption { 1: conflict_type, 2: properties }`
                ip.read_struct_begin().map_err(tw)?;
                loop {
                    let g = ip.read_field_begin().map_err(tw)?;
                    if g.field_type == TType::Stop {
                        break;
                    }
                    if g.id == Some(1) {
                        r.conflict = ip.read_i32().map_err(tw)? as i64;
                    } else {
                        ip.skip(g.field_type).map_err(tw)?;
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
    end_args(ip)?; // CreateIndex_args
    Ok(r)
}

/// The reference lower-cases every identifier at its API boundary
/// (`infinity_impl.cpp`), so a Thrift request may name `db.Table` in any case
/// while the engine keeps lower-case names.
fn full_table_name(db: &str, table: &str) -> String {
    let db = db.trim().to_ascii_lowercase();
    let table = table.trim().to_ascii_lowercase();
    if db.is_empty() || table.contains('.') {
        table
    } else {
        format!("{db}.{table}")
    }
}

/// Runs `sql` on the caller's task and maps the outcome to a `CommonResponse`.
///
/// **Async on purpose.** It used to call `execute_blocking`, which bridges
/// `Engine::execute` with `scheduler::block_on`: on a runtime worker that is a
/// park/unpark loop holding the worker, and the statement it waits for needs
/// workers of its own (the WAL reply, the allocator, the bottom half). Under
/// concurrent DDL and inserts that parked every worker and the server sat at
/// 8% CPU with all threads asleep while the client timed out.
/// Runs `DROP TABLE` on the engine without an SQL round trip.
///
/// A missing name is `TABLE_NOT_EXIST` unless the request said `IF EXISTS`,
/// which is the pair the reference's handler produces.
async fn run_drop_table(
    engine: Option<&Arc<Engine>>,
    name: &str,
    if_exists: bool,
    session_id: i64,
) -> CommonResponse {
    let Some(eng) = engine else {
        return CommonResponse {
            error_code: 1,
            error_msg: "no engine bound".into(),
            session_id,
        };
    };
    match eng.drop_table_direct(name, if_exists).await {
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
    }
}

async fn run_common(engine: Option<&Arc<Engine>>, sql: &str, session_id: i64) -> CommonResponse {
    match engine {
        Some(eng) => match eng.execute(sql).await {
            Ok(_) => CommonResponse {
                error_code: OK,
                error_msg: "OK".into(),
                session_id,
            },
            Err(e) => {
                if std::env::var_os("INF_DEBUG_SQL").is_some() {
                    eprintln!("[SQL-ERR] {sql} -> {e}");
                }
                CommonResponse {
                    error_code: e.status_code(),
                    error_msg: e.to_string(),
                    session_id,
                }
            }
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
/// `ShowSnapshotRequest { 1: session_id, 2: snapshot_name }`.
fn read_snapshot_args(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<(i64, String)> {
    ip.read_struct_begin().map_err(tw)?;
    let _ = ip.read_field_begin().map_err(tw)?;
    ip.read_struct_begin().map_err(tw)?;
    let (mut sid, mut name) = (0i64, String::new());
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

pub fn write_list_snapshots_response(w: &mut impl Write, seq: i32, names: &[String]) -> Result<()> {
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
    o.write_list_begin(&TListIdentifier::new(TType::Struct, names.len() as i32))
        .map_err(tw)?;
    for n in names {
        // SnapshotInfo {1: name, 2: scope, 3: time, 4: commit, 5: size}
        o.write_struct_begin(&TStructIdentifier::new("SnapshotInfo"))
            .map_err(tw)?;
        for (field, id, value) in [
            ("name", 1i16, n.clone()),
            ("scope", 2i16, "table".to_string()),
            ("time", 3i16, String::new()),
            ("size", 5i16, String::new()),
        ] {
            o.write_field_begin(&TFieldIdentifier::new(field, TType::String, id))
                .map_err(tw)?;
            o.write_string(&value).map_err(tw)?;
            o.write_field_end().map_err(tw)?;
        }
        o.write_field_begin(&TFieldIdentifier::new("commit", TType::I64, 4i16))
            .map_err(tw)?;
        o.write_i64(0).map_err(tw)?;
        o.write_field_end().map_err(tw)?;
        o.write_field_stop().map_err(tw)?;
        o.write_struct_end().map_err(tw)?;
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
pub fn write_delete_response(
    w: &mut impl Write,
    seq: i32,
    error_code: i64,
    error_msg: &str,
    deleted_rows: i64,
) -> Result<()> {
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
    o.write_i64(error_code).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("error_msg", TType::String, 2i16))
        .map_err(tw)?;
    o.write_string(error_msg).map_err(tw)?;
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

// -------- ShowIndex --------

/// Reads the args of a `ShowIndex` CALL.
pub fn read_show_index_args(
    ip: &mut TBinaryInputProtocol<&mut impl Read>,
) -> Result<(String, String, String, i64)> {
    ip.read_struct_begin().map_err(tw)?; // ShowIndex_args
    let _ = ip.read_field_begin().map_err(tw)?; // request (Struct)
    ip.read_struct_begin().map_err(tw)?; // ShowIndexRequest
    let (mut db, mut table, mut index, mut sid) =
        (String::new(), String::new(), String::new(), 0i64);
    loop {
        let fd = ip.read_field_begin().map_err(tw)?;
        if fd.field_type == TType::Stop {
            break;
        }
        match fd.id {
            Some(1) => db = ip.read_string().map_err(tw)?,
            Some(2) => table = ip.read_string().map_err(tw)?,
            Some(3) => index = ip.read_string().map_err(tw)?,
            Some(4) => sid = ip.read_i64().map_err(tw)?,
            _ => ip.skip(fd.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?; // ShowIndexRequest
    end_args(ip)?; // ShowIndex_args
    Ok((db, table, index, sid))
}

/// Writes a `ShowIndex` REPLY.
#[allow(clippy::too_many_arguments)]
pub fn write_show_index_response(
    w: &mut impl Write,
    seq: i32,
    db: &str,
    table: &str,
    index: &str,
    comment: &str,
    code: i64,
    msg: &str,
    kind: &str,
    columns: &str,
) -> Result<()> {
    let mut o = TBinaryOutputProtocol::new(w, true);
    o.write_message_begin(&TMessageIdentifier::new(
        "ShowIndex",
        TMessageType::Reply,
        seq,
    ))
    .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("ShowIndex_result"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("success", TType::Struct, 0i16))
        .map_err(tw)?;
    o.write_struct_begin(&TStructIdentifier::new("ShowIndexResponse"))
        .map_err(tw)?;
    o.write_field_begin(&TFieldIdentifier::new("error_code", TType::I64, 1i16))
        .map_err(tw)?;
    o.write_i64(code).map_err(tw)?;
    o.write_field_end().map_err(tw)?;
    for (name, id, val) in [
        ("error_msg", 2i16, msg),
        ("db_name", 3i16, db),
        ("table_name", 4i16, table),
        ("index_name", 5i16, index),
        ("index_comment", 6i16, comment),
        ("index_type", 7i16, kind),
        ("index_column_names", 8i16, columns),
        ("index_column_ids", 9i16, ""),
        ("index_function_info", 10i16, ""),
        ("other_parameters", 11i16, ""),
        ("store_dir", 12i16, ""),
        ("segment_index_count", 13i16, ""),
    ] {
        o.write_field_begin(&TFieldIdentifier::new(name, TType::String, id))
            .map_err(tw)?;
        o.write_string(val).map_err(tw)?;
        o.write_field_end().map_err(tw)?;
    }
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?; // ShowIndexResponse
    o.write_field_end().map_err(tw)?;
    o.write_field_stop().map_err(tw)?;
    o.write_struct_end().map_err(tw)?; // ShowIndex_result
                                       // The reply sits in the buffered writer until it is flushed; without this
                                       // the caller waits for a frame that never leaves the server.
    o.flush().map_err(tw)?;
    Ok(())
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
    code: i64,
    msg: &str,
    column_count: i64,
    segment_count: i64,
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
        ("error_code", 1i16, code),
        ("row_count", 8i16, row_count),
        ("column_count", 6i16, column_count),
        ("segment_count", 7i16, segment_count),
    ] {
        o.write_field_begin(&TFieldIdentifier::new(name, TType::I64, id))
            .map_err(tw)?;
        o.write_i64(val).map_err(tw)?;
        o.write_field_end().map_err(tw)?;
    }
    for (name, id, val) in [
        ("error_msg", 2i16, msg),
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

/// `ShowSegmentsRequest { 1: session_id, 2: db_name, 3: table_name }`.
fn read_table_args(ip: &mut TBinaryInputProtocol<&mut impl Read>) -> Result<(i64, String, String)> {
    let (mut sid, mut db, mut table) = (0i64, String::new(), String::new());
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
            (Some(2), TType::String) => db = ip.read_string().map_err(tw)?,
            (Some(3), TType::String) => table = ip.read_string().map_err(tw)?,
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    end_args(ip)?;
    Ok((sid, db, table))
}

/// `ShowBlocksRequest { 1: session_id, 2: db_name, 3: table_name, 4: segment_id }`.
fn read_block_args(
    ip: &mut TBinaryInputProtocol<&mut impl Read>,
) -> Result<(i64, String, String, i64)> {
    let (mut sid, mut db, mut table, mut segment) = (0i64, String::new(), String::new(), 0i64);
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
            (Some(2), TType::String) => db = ip.read_string().map_err(tw)?,
            (Some(3), TType::String) => table = ip.read_string().map_err(tw)?,
            (Some(4), TType::I64) => segment = ip.read_i64().map_err(tw)?,
            _ => ip.skip(f.field_type).map_err(tw)?,
        }
        ip.read_field_end().map_err(tw)?;
    }
    ip.read_struct_end().map_err(tw)?;
    end_args(ip)?;
    Ok((sid, db, table, segment))
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
        names: Vec::new(),
        raw_names: Vec::new(),
        star: false,
        where_sql: None,
        order_by: Vec::new(),
        group_by: Vec::new(),
        having_sql: None,
        limit: None,
        offset: None,
        total_hits: false,
        aliases: Vec::new(),
        highlight: Vec::new(),
        search_sql: None,
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
                    let (names, _, _, star) = read_select_item(ip, &mut Vec::new())?;
                    req.columns.extend(names);
                    req.star |= star;
                }
                ip.read_list_end().map_err(tw)?;
            }
            Some(7) if f.field_type == TType::Struct => req.where_sql = read_where(ip)?,
            Some(8) if f.field_type == TType::List => {
                let l = ip.read_list_begin().map_err(tw)?;
                for _ in 0..l.size {
                    let sql = pexpr_sql(&read_pexpr(ip)?);
                    if !sql.is_empty() {
                        req.group_by.push(sql);
                    }
                }
                ip.read_list_end().map_err(tw)?;
            }
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
    // Consume the enclosing `Import_args` stop byte: leaving it unread shifts
    // every later request on the connection by one byte.
    end_args(ip)?;
    Ok(args)
}

#[cfg(test)]
mod tests {
    use super::*;

    /// A header that never ends must not grow the server's buffer (plan 6.2.1).
    #[test]
    fn an_endless_header_is_rejected() {
        use std::io::{Read, Write};
        let dir = std::env::temp_dir().join(format!("inf_http_431_{}", std::process::id()));
        let _ = std::fs::remove_dir_all(&dir);
        std::fs::create_dir_all(&dir).unwrap();
        let engine = Engine::open(&dir).unwrap();
        let server = crate::http::HttpServer::bind("127.0.0.1:0", Arc::new(engine)).unwrap();
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
        std::thread::sleep(std::time::Duration::from_millis(100));

        let mut sock = std::net::TcpStream::connect(&addr).unwrap();
        sock.set_read_timeout(Some(std::time::Duration::from_secs(5)))
            .unwrap();
        sock.write_all(b"GET / HTTP/1.1\r\nHost: x\r\n").unwrap();
        // 96 KiB of header bytes with no terminator.
        sock.write_all(&vec![b'a'; 96 * 1024]).unwrap();
        let mut buf = [0u8; 128];
        let n = sock.read(&mut buf).unwrap();
        let head = String::from_utf8_lossy(&buf[..n]);
        assert!(
            head.starts_with("HTTP/1.1 431"),
            "expected 431, got {head:?}"
        );
        let _ = std::fs::remove_dir_all(&dir);
    }

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

        let ok = connect(&addr, CURRENT_VERSION_INDEX).unwrap();
        assert_eq!(ok.error_code, OK);
        assert!(ok.session_id >= 1);

        let bad = connect(&addr, 1).unwrap();
        assert_eq!(bad.error_code, VERSION_MISMATCH);

        let dc = disconnect(&addr, ok.session_id).unwrap();
        assert_eq!(dc.error_code, OK);
        assert_eq!(dc.session_id, ok.session_id);
    }

    /// The peer manager owns the node `SHOW CURRENT NODE` reports: register,
    /// heartbeat and change-role all go through the manager `Server` holds, and
    /// every mutation is visible either on the wire or in the registry.
    #[test]
    fn show_current_node_reflects_the_peer_manager() {
        use crate::peer::{NodeRole, NodeStatus};

        let server = Server::bind("127.0.0.1:0").unwrap();
        let addr = server.local_addr().unwrap();
        let peer = server.peer();
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

        let node = show_current_node(&addr, 1).unwrap();
        assert_eq!(node.error_code, OK);
        assert_eq!(node.node_role, "leader");
        assert_eq!(node.server_status, "alive");

        // Register a follower; the local node stays the leader and the
        // registry records the new peer.
        peer.set_now_secs(100);
        peer.register_peer("follower0", NodeRole::Follower, "127.0.0.2", 23818, 10)
            .unwrap();
        assert_eq!(
            peer.node_state(),
            ("leader".to_string(), "alive".to_string())
        );
        let nodes = peer.list_nodes();
        assert_eq!(nodes.len(), 2);
        assert_eq!(nodes[1].node_name(), "follower0");
        assert_eq!(nodes[1].node_status(), NodeStatus::Alive);

        // A heartbeat advances the follower's replicated timestamp while the
        // leader keeps answering `SHOW CURRENT NODE` from its own `NodeInfo`.
        peer.set_now_secs(101);
        peer.heartbeat_peer("follower0", "127.0.0.2", 23818, 20)
            .unwrap();
        let follower = peer
            .list_nodes()
            .into_iter()
            .find(|node| node.node_name() == "follower0")
            .unwrap();
        assert_eq!(follower.txn_ts(), 20);
        assert_eq!(follower.heartbeat_count(), 1);
        let node = show_current_node(&addr, 1).unwrap();
        assert_eq!(node.node_role, "leader");
        assert_eq!(node.server_status, "alive");

        // The role transition reaches the client through the same NodeInfo.
        peer.change_role(NodeRole::Admin).unwrap();
        let node = show_current_node(&addr, 1).unwrap();
        assert_eq!(node.node_role, "admin");
        assert_eq!(node.server_status, "alive");
        assert_eq!(
            peer.events(),
            vec![
                "register@follower0, 127.0.0.2:23818, follower".to_string(),
                "heartbeat@follower0, 20".to_string(),
                "change-role@admin".to_string(),
            ]
        );
    }

    /// The registry history is a real `RingBufferIterator`: it records the
    /// register / heartbeat / terminate sequence, keeps only the newest 64
    /// records and captures leader-side timeouts.
    #[test]
    fn peer_registry_records_register_heartbeat_and_terminate_events() {
        use crate::peer::{NodeRole, PEER_EVENT_CAPACITY};

        let server = Server::bind("127.0.0.1:0").unwrap();
        let peer = server.peer();
        peer.set_now_secs(100);

        peer.register_peer("follower0", NodeRole::Follower, "127.0.0.2", 23818, 10)
            .unwrap();
        peer.register_peer("learner0", NodeRole::Learner, "127.0.0.3", 23819, 11)
            .unwrap();
        peer.heartbeat_peer("follower0", "127.0.0.2", 23818, 12)
            .unwrap();
        let follower = peer
            .list_nodes()
            .into_iter()
            .find(|node| node.node_name() == "follower0")
            .unwrap();
        assert_eq!(follower.txn_ts(), 12);
        assert_eq!(follower.heartbeat_count(), 1);

        // A duplicate registration is rejected and leaves the log untouched.
        assert!(peer
            .register_peer("learner0", NodeRole::Learner, "127.0.0.9", 23820, 13)
            .is_err());

        peer.terminate_peer("follower0").unwrap();
        assert!(peer
            .list_nodes()
            .iter()
            .all(|node| node.node_name() != "follower0"));
        assert_eq!(
            peer.events(),
            vec![
                "register@follower0, 127.0.0.2:23818, follower".to_string(),
                "register@learner0, 127.0.0.3:23819, learner".to_string(),
                "heartbeat@follower0, 12".to_string(),
                "terminate@follower0".to_string(),
            ]
        );

        // The ring keeps the newest 64 events. Re-register the peer, then push
        // enough heartbeats to wrap the buffer several times over.
        peer.set_now_secs(100);
        peer.register_peer("follower0", NodeRole::Follower, "127.0.0.2", 23818, 0)
            .unwrap();
        for txn in 0..80 {
            peer.heartbeat_peer("follower0", "127.0.0.2", 23818, txn)
                .unwrap();
        }
        let events = peer.events();
        assert_eq!(events.len(), PEER_EVENT_CAPACITY);
        // 4 earlier records + re-register + 80 heartbeats = 85; the oldest 21
        // are overwritten, so the history starts at heartbeat #16.
        assert_eq!(events.first().unwrap(), "heartbeat@follower0, 16");
        assert_eq!(events.last().unwrap(), "heartbeat@follower0, 79");

        // Four silent seconds mark both silent peers timed out and record it.
        peer.set_now_secs(1000);
        assert_eq!(
            peer.check_heartbeat().unwrap(),
            vec!["follower0".to_string(), "learner0".to_string()]
        );
        assert_eq!(peer.events().last().unwrap(), "timeout@learner0");

        // The client terminates last: after this the task queue is closed.
        peer.shutdown();
        assert_eq!(peer.events().last().unwrap(), "terminate@leader");
    }

    #[test]
    fn tcp_create_table_round_trip_with_engine() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Arc::new(Engine::open(dir.path()).unwrap());
        let server = Server::bind_with_engine("127.0.0.1:0", engine).unwrap();
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

        // A real session: the server rejects an id it never handed out
        // (`SessionNotFound`), so the test connects first like any client.
        let session_id = connect(&addr, CURRENT_VERSION_INDEX).unwrap().session_id;
        assert!(session_id > 0);
        let req = CreateTableReq {
            db_name: "default_db".into(),
            table_name: "t".into(),
            columns: vec![ColumnDefWire {
                id: 0,
                name: "id".into(),
                default: None,
                not_null: false,
                // `LogicType::Integer`; `HugeInt` (5) is one of the types the
                // reference's column whitelist rejects.
                logic_type: 3,
                physical: PhysicalWire::None,
                comment: String::new(),
            }],
            session_id,
            conflict: 1,
        };
        assert_eq!(create_table(&addr, &req).unwrap().error_code, OK);
        // creating the same table again must fail: the engine kept the state
        assert_ne!(create_table(&addr, &req).unwrap().error_code, OK);

        // The reference refuses `HugeInt` at DDL time; the RPC reports its
        // `NOT_SUPPORTED` code instead of quietly creating a `VARCHAR`.
        let mut huge = req.clone();
        huge.table_name = "h".into();
        huge.columns[0].logic_type = 5;
        assert_eq!(create_table(&addr, &huge).unwrap().error_code, 3032);
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

        let fields = rows_to_column_fields_typed(&cols, &types, &text_cells(rows.clone()));
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
        let resp = select_response_from_typed(&cols, &types, &text_cells(rows.clone()));
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
        let sf = rows_to_column_fields_typed(&scols, &stypes, &text_cells(srows.clone()));
        assert_eq!(sf[0].column_type, 13); // ColumnSparse
        let b = &sf[0].column_vectors[0];
        assert_eq!(u32::from_le_bytes(b[..4].try_into().unwrap()), 2); // nnz
        assert_eq!(&b[4..6], &[0u8, 3u8]); // indices as i8
        assert_eq!(f32::from_le_bytes(b[6..10].try_into().unwrap()), 1.5);
        assert_eq!(f32::from_le_bytes(b[10..14].try_into().unwrap()), 2.5);
        let sresp = select_response_from_typed(&scols, &stypes, &text_cells(srows.clone()));
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
        let ef = rows_to_column_fields_typed(&scols, &stypes, &text_cells(erows.clone()));
        assert_eq!(ef[0].column_vectors[0], vec![0, 0, 0, 0]);

        // Tensor: u32 byte-length + dim*rows little-endian elements per row.
        let tcols = vec!["t".to_string()];
        let ttypes = vec!["TENSOR(FLOAT,3)".to_string()];
        let trows = vec![vec!["[[1,2,3],[4,5,6]]".to_string()]];
        let tf = rows_to_column_fields_typed(&tcols, &ttypes, &text_cells(trows.clone()));
        assert_eq!(tf[0].column_type, 11); // ColumnTensor
        let b = &tf[0].column_vectors[0];
        assert_eq!(u32::from_le_bytes(b[..4].try_into().unwrap()), 24); // 2*3*4 bytes
        let vals: Vec<f32> = (0..6)
            .map(|i| f32::from_le_bytes(b[4 + i * 4..8 + i * 4].try_into().unwrap()))
            .collect();
        assert_eq!(vals, vec![1.0, 2.0, 3.0, 4.0, 5.0, 6.0]);
        let tresp = select_response_from_typed(&tcols, &ttypes, &text_cells(trows.clone()));
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
        let af = rows_to_column_fields_typed(&acols, &atypes, &text_cells(arows.clone()));
        assert_eq!(af[0].column_type, 12); // ColumnTensorArray
        let b = &af[0].column_vectors[0];
        assert_eq!(u32::from_le_bytes(b[..4].try_into().unwrap()), 2); // tensor_n
        assert_eq!(u32::from_le_bytes(b[4..8].try_into().unwrap()), 24); // first tensor
        let second = 8 + 24;
        assert_eq!(
            u32::from_le_bytes(b[second..second + 4].try_into().unwrap()),
            12
        );
        let aresp = select_response_from_typed(&acols, &atypes, &text_cells(arows.clone()));
        assert_eq!(aresp.column_defs[0].logic_type, 14); // TensorArray

        // Array: u32 element count + each element packed for its ColumnType.
        let rcols = vec!["r".to_string()];
        let rtypes = vec!["ARRAY(INT)".to_string()];
        let rrows = vec![vec!["{1,2,3}".to_string()]];
        let rf = rows_to_column_fields_typed(&rcols, &rtypes, &text_cells(rrows.clone()));
        assert_eq!(rf[0].column_type, 21); // ColumnArray
        let b = &rf[0].column_vectors[0];
        assert_eq!(u32::from_le_bytes(b[..4].try_into().unwrap()), 3);
        assert_eq!(i32::from_le_bytes(b[4..8].try_into().unwrap()), 1);
        assert_eq!(i32::from_le_bytes(b[8..12].try_into().unwrap()), 2);
        assert_eq!(i32::from_le_bytes(b[12..16].try_into().unwrap()), 3);
        let rresp = select_response_from_typed(&rcols, &rtypes, &text_cells(rrows.clone()));
        assert_eq!(rresp.column_defs[0].logic_type, 22); // Array
        assert_eq!(
            rresp.column_defs[0].physical,
            PhysicalWire::Array {
                element_logic_type: 3,                      // Integer
                element: Box::new(PhysicalWire::Scalar(3)), // ColumnInt32
            }
        );

        // Varchar arrays are length-prefixed per element.
        let vcols = vec!["s".to_string()];
        let vtypes = vec!["ARRAY(VARCHAR)".to_string()];
        let vrows = vec![vec!["{ab,cd}".to_string()]];
        let vf = rows_to_column_fields_typed(&vcols, &vtypes, &text_cells(vrows.clone()));
        let b = &vf[0].column_vectors[0];
        assert_eq!(u32::from_le_bytes(b[..4].try_into().unwrap()), 2);
        assert_eq!(u32::from_le_bytes(b[4..8].try_into().unwrap()), 2); // "ab"
        assert_eq!(&b[8..10], b"ab");

        // Nested arrays recurse: `{{1,2},{3}}` -> count 2, then each sub-array.
        let ncols = vec!["n".to_string()];
        let ntypes = vec!["ARRAY(ARRAY(INT))".to_string()];
        let nrows = vec![vec!["{{1,2},{3}}".to_string()]];
        let nf = rows_to_column_fields_typed(&ncols, &ntypes, &text_cells(nrows.clone()));
        let b = &nf[0].column_vectors[0];
        assert_eq!(u32::from_le_bytes(b[..4].try_into().unwrap()), 2); // outer
        assert_eq!(u32::from_le_bytes(b[4..8].try_into().unwrap()), 2); // inner #1
        assert_eq!(i32::from_le_bytes(b[8..12].try_into().unwrap()), 1);
        assert_eq!(i32::from_le_bytes(b[12..16].try_into().unwrap()), 2);
        assert_eq!(u32::from_le_bytes(b[16..20].try_into().unwrap()), 1); // inner #2
        assert_eq!(i32::from_le_bytes(b[20..24].try_into().unwrap()), 3);
        let nresp = select_response_from_typed(&ncols, &ntypes, &text_cells(nrows.clone()));
        assert_eq!(nresp.column_defs[0].logic_type, 22); // Array
        assert_eq!(
            nresp.column_defs[0].physical,
            PhysicalWire::Array {
                element_logic_type: 22, // Array
                element: Box::new(PhysicalWire::Array {
                    element_logic_type: 3, // Integer
                    // SDKs need a physical descriptor even for scalar elements.
                    element: Box::new(PhysicalWire::Scalar(3)),
                }),
            }
        );
    }

    // -------- `SHOW SEGMENTS` / `SHOW BLOCKS` over the wire --------

    /// Decodes one `ColumnField` inside a `SelectResponse` into its values.
    fn read_column_field<R: Read>(
        ip: &mut TBinaryInputProtocol<R>,
    ) -> Result<(String, Vec<String>)> {
        let (mut column_type, mut name) = (0i32, String::new());
        let mut chunk: Vec<u8> = Vec::new();
        ip.read_struct_begin().map_err(tw)?;
        loop {
            let f = ip.read_field_begin().map_err(tw)?;
            if f.field_type == TType::Stop {
                break;
            }
            match f.id {
                Some(1) if f.field_type == TType::I32 => {
                    column_type = ip.read_i32().map_err(tw)?;
                }
                Some(2) if f.field_type == TType::List => {
                    let list = ip.read_list_begin().map_err(tw)?;
                    if list.size > 0 {
                        let first = ip.read_bytes().map_err(tw)?;
                        chunk = first;
                        for _ in 1..list.size {
                            let _ = ip.read_bytes().map_err(tw)?;
                        }
                    }
                }
                Some(3) if f.field_type == TType::String => {
                    name = ip.read_string().map_err(tw)?;
                }
                _ => ip.skip(f.field_type).map_err(tw)?,
            }
            ip.read_field_end().map_err(tw)?;
        }
        ip.read_struct_end().map_err(tw)?;
        Ok((name, decode_column(column_type, &chunk)))
    }

    /// Unpacks the column chunk for the two types `SHOW SEGMENTS`/`SHOW BLOCKS`
    /// declare (BigInt, Varchar).
    fn decode_column(column_type: i32, chunk: &[u8]) -> Vec<String> {
        match column_type {
            4 => chunk
                .as_chunks::<8>()
                .0
                .iter()
                .map(|b| i64::from_le_bytes(*b).to_string())
                .collect(),
            9 => {
                let mut out = Vec::new();
                let mut i = 0usize;
                while i + 4 <= chunk.len() {
                    let n = u32::from_le_bytes(chunk[i..i + 4].try_into().unwrap()) as usize;
                    i += 4;
                    if i + n > chunk.len() {
                        break;
                    }
                    out.push(String::from_utf8_lossy(&chunk[i..i + n]).into_owned());
                    i += n;
                }
                out
            }
            _ => Vec::new(),
        }
    }

    /// Reads a `<Method>_result` REPLY and returns the `SelectResponse`'s
    /// column fields as `(name, values)`.
    fn read_show_reply<R: Read>(r: &mut R, method: &str) -> Result<Vec<(String, Vec<String>)>> {
        let mut ip = TBinaryInputProtocol::new(r, true);
        let msg = ip.read_message_begin().map_err(tw)?;
        assert_eq!(msg.name, method);
        ip.read_struct_begin().map_err(tw)?;
        let mut out = Vec::new();
        loop {
            let f = ip.read_field_begin().map_err(tw)?;
            if f.field_type == TType::Stop {
                break;
            }
            if f.id == Some(0) && f.field_type == TType::Struct {
                ip.read_struct_begin().map_err(tw)?;
                loop {
                    let g = ip.read_field_begin().map_err(tw)?;
                    if g.field_type == TType::Stop {
                        break;
                    }
                    if g.id == Some(4) && g.field_type == TType::List {
                        let list = ip.read_list_begin().map_err(tw)?;
                        for _ in 0..list.size {
                            out.push(read_column_field(&mut ip)?);
                        }
                    } else {
                        ip.skip(g.field_type).map_err(tw)?;
                    }
                    ip.read_field_end().map_err(tw)?;
                }
                ip.read_struct_end().map_err(tw)?;
            } else {
                ip.skip(f.field_type).map_err(tw)?;
            }
            ip.read_field_end().map_err(tw)?;
        }
        ip.read_struct_end().map_err(tw)?;
        Ok(out)
    }

    /// Writes a `Show*Request { session_id, db_name, table_name, segment_id? }`
    /// CALL and returns the decoded reply.
    fn show_call(
        addr: &str,
        method: &str,
        table: &str,
        segment: Option<i64>,
    ) -> Result<Vec<(String, Vec<String>)>> {
        use std::net::TcpStream;
        // A real session: the server rejects an id it never handed out.
        let session_id = connect(addr, CURRENT_VERSION_INDEX)?.session_id;
        let mut stream = TcpStream::connect(addr).unwrap();
        let mut buf = Vec::new();
        {
            let mut o = TBinaryOutputProtocol::new(&mut buf, true);
            o.write_message_begin(&TMessageIdentifier::new(method, TMessageType::Call, 7))
                .map_err(tw)?;
            o.write_struct_begin(&TStructIdentifier::new(format!("{method}_args")))
                .map_err(tw)?;
            o.write_field_begin(&TFieldIdentifier::new("request", TType::Struct, 1i16))
                .map_err(tw)?;
            o.write_struct_begin(&TStructIdentifier::new(format!("{method}Request")))
                .map_err(tw)?;
            o.write_field_begin(&TFieldIdentifier::new("session_id", TType::I64, 1i16))
                .map_err(tw)?;
            o.write_i64(session_id).map_err(tw)?;
            o.write_field_end().map_err(tw)?;
            o.write_field_begin(&TFieldIdentifier::new("db_name", TType::String, 2i16))
                .map_err(tw)?;
            o.write_string("default_db").map_err(tw)?;
            o.write_field_end().map_err(tw)?;
            o.write_field_begin(&TFieldIdentifier::new("table_name", TType::String, 3i16))
                .map_err(tw)?;
            o.write_string(table).map_err(tw)?;
            o.write_field_end().map_err(tw)?;
            if let Some(segment) = segment {
                o.write_field_begin(&TFieldIdentifier::new("segment_id", TType::I64, 4i16))
                    .map_err(tw)?;
                o.write_i64(segment).map_err(tw)?;
                o.write_field_end().map_err(tw)?;
            }
            o.write_field_stop().map_err(tw)?;
            o.write_struct_end().map_err(tw)?;
            o.write_field_end().map_err(tw)?;
            o.write_field_stop().map_err(tw)?;
            o.write_struct_end().map_err(tw)?;
            o.flush().map_err(tw)?;
        }
        stream.write_all(&buf).map_err(io)?;
        read_show_reply(&mut stream, method)
    }

    /// `ShowSegments`/`ShowBlocks` report the real segment and block metadata
    /// (`physical_show_segments_impl.cpp`, `physical_show_blocks_impl.cpp`):
    /// one row per live segment with its row count, and one row per block of
    /// that segment with the block's row count — not `rows / 8192`.
    /// `ShowSegments`/`ShowBlocks` report the real segment and block metadata
    /// A session id the server never handed out is `SessionNotFound` (3034),
    /// and `Disconnect` really removes one - the reference keeps sessions in a
    /// static map keyed by id, so a stale id must stop working.
    #[test]
    fn an_unknown_session_is_rejected() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Arc::new(Engine::open(dir.path()).unwrap());
        let server = Server::bind_with_engine("127.0.0.1:0", engine).unwrap();
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

        // `ListDatabase` is a business RPC: it takes the session id and its
        // reply carries `error_code`, so a rejected session is observable here
        // (unlike `Disconnect`, which is idempotent by design).
        let call_with = |session_id: i64| -> Result<(i64, String)> {
            use std::net::TcpStream;
            let mut stream = TcpStream::connect(&addr).map_err(io)?;
            let mut buf = Vec::new();
            {
                let mut o = TBinaryOutputProtocol::new(&mut buf, true);
                o.write_message_begin(&TMessageIdentifier::new(
                    "ListDatabase",
                    TMessageType::Call,
                    9,
                ))
                .map_err(tw)?;
                o.write_struct_begin(&TStructIdentifier::new("ListDatabase_args"))
                    .map_err(tw)?;
                o.write_field_begin(&TFieldIdentifier::new("request", TType::Struct, 1i16))
                    .map_err(tw)?;
                o.write_struct_begin(&TStructIdentifier::new("ListDatabaseRequest"))
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
            }
            use std::io::Write;
            stream.write_all(&buf).map_err(io)?;
            let mut i = TBinaryInputProtocol::new(&mut stream, true);
            i.read_message_begin().map_err(tw)?;
            i.read_struct_begin().map_err(tw)?; // ListDatabase_result
            let success = i.read_field_begin().map_err(tw)?;
            if success.field_type != TType::Struct {
                return Err(Error::Invalid("expected success struct".into()));
            }
            i.read_struct_begin().map_err(tw)?; // ListDatabaseResponse
            let (mut code, mut msg) = (0i64, String::new());
            loop {
                let f = i.read_field_begin().map_err(tw)?;
                if f.field_type == TType::Stop {
                    break;
                }
                match f.id {
                    Some(1) => code = i.read_i64().map_err(tw)?,
                    Some(2) => msg = i.read_string().map_err(tw)?,
                    _ => i.skip(f.field_type).map_err(tw)?,
                }
                i.read_field_end().map_err(tw)?;
            }
            Ok((code, msg))
        };

        // An id nobody opened.
        let (code, msg) = call_with(999_999).unwrap();
        assert_eq!(code, 3034);
        assert!(msg.contains("isn't found"), "{msg}");

        // A session that exists answers ...
        let session_id = connect(&addr, CURRENT_VERSION_INDEX).unwrap().session_id;
        assert_ne!(call_with(session_id).unwrap().0, 3034);

        // ... and stops answering once it is disconnected.
        let _ = disconnect(&addr, session_id);
        assert_eq!(call_with(session_id).unwrap().0, 3034);
    }

    /// `ShowSegments`/`ShowBlocks` report the real segment and block metadata
    /// (`physical_show_segments_impl.cpp`, `physical_show_blocks_impl.cpp`):
    /// one row per live segment with its row count, and one row per block of
    /// that segment with the block's row count — not `rows / 8192`.
    #[test]
    fn thrift_show_segments_and_blocks_report_real_metadata() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Arc::new(Engine::open(dir.path()).unwrap());
        engine.execute_blocking("CREATE TABLE t (id INT);").unwrap();
        // An `INSERT` appends to the open segment; a `COPY` seals the segment
        // it writes, so `INSERT` + `COPY` leaves two segments of one row each
        // (the C++ row-id allocator behaves the same way).
        engine
            .execute_blocking("INSERT INTO t VALUES (1);")
            .unwrap();
        let csv = dir.path().join("one_row.csv");
        std::fs::write(&csv, "2\n").unwrap();
        engine
            .execute_blocking(&format!(
                "COPY t FROM '{}' WITH (DELIMITER ',', FORMAT CSV);",
                csv.display()
            ))
            .unwrap();
        let segments = engine.segments("t");
        assert_eq!(segments.len(), 2, "{segments:?}");
        // Blocks are materialised by the C++-format mirror (a flush or
        // checkpoint), as in the reference.
        executor::block_on(engine.flush()).unwrap();
        let segment0 = segments[0].0;
        let blocks = engine.block_ids("t", segment0);
        assert!(!blocks.is_empty(), "flush must create blocks");
        let block_rows = engine.block_row_count("t", segment0, blocks[0] as u16);

        let server = Server::bind_with_engine("127.0.0.1:0", engine.clone()).unwrap();
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

        let cols = show_call(&addr, "ShowSegments", "t", None).unwrap();
        let names: Vec<&str> = cols.iter().map(|(n, _)| n.as_str()).collect();
        assert_eq!(
            names,
            SHOW_SEGMENTS_COLUMNS.to_vec(),
            "C++ physical_show_impl.cpp:112 column set"
        );
        let ids = &cols[0].1;
        let status = &cols[1].1;
        let sizes = &cols[2].1;
        let rows = &cols[3].1;
        assert_eq!(ids.len(), 2, "{cols:?}");
        assert_eq!(rows, &vec!["1".to_string(), "1".to_string()], "{cols:?}");
        // Real on-disk size, formatted like `Utility::FormatByteSize`.
        assert!(sizes.iter().all(|s| s.ends_with('B')), "{sizes:?}");
        // The reference's status cell is the removed `SegmentEntry::status_`.
        assert_eq!(status, &vec!["No value".to_string(); 2]);

        let cols = show_call(&addr, "ShowBlocks", "t", Some(i64::from(segment0))).unwrap();
        let names: Vec<&str> = cols.iter().map(|(n, _)| n.as_str()).collect();
        assert_eq!(
            names,
            SHOW_BLOCKS_COLUMNS.to_vec(),
            "C++ physical_show_impl.cpp:130 column set"
        );
        let ids = &cols[0].1;
        assert_eq!(ids.len(), blocks.len(), "{cols:?}");
        assert_eq!(ids[0], blocks[0].to_string());
        assert_eq!(
            cols[2].1[0],
            block_rows.unwrap_or(0).to_string(),
            "the block's real row count"
        );
        assert!(cols[1].1[0].ends_with('B'), "{:?}", cols[1].1);
    }

    /// A float value must reach the engine as a *float*, or the sign of a
    /// negative zero is lost: `f.to_string()` renders `-0.0` as `-0`, and the
    /// lexer reads `-0` as an **integer** (`Token::Int(0)`). The reference
    /// stores the f32 `-0.0` (0x800000); the port stored `+0.0` (0x000000).
    #[test]
    fn a_negative_zero_stays_a_float() {
        assert_eq!(ValueLit::Float(-0.0).to_sql(), "-0.0");
        // The lexer must read it back as a float, not as the integer 0.
        let tokens = parser::lexer::tokenize("-0.0").unwrap();
        assert!(
            tokens.iter().any(|t| matches!(
                t,
                parser::lexer::Token::Float(f) if f.is_sign_negative() && *f == 0.0
            )),
            "{tokens:?}"
        );
        // A whole float keeps its fractional part too, so it is a float literal.
        assert_eq!(ValueLit::Float(1.0).to_sql(), "1.0");
        assert_eq!(ValueLit::Float(1.5).to_sql(), "1.5");
    }
}
