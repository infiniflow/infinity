//! The async Thrift front end (plan §6.3, route (c)): `volo-thrift` owns the
//! socket, decodes each request into the generated struct and calls one of the
//! 49 `InfinityService` methods below.
//!
//! The handlers never run `Engine::execute` on a reactor worker — a statement
//! is driven from the handler task under `scheduler().run` (`block_in_place`),
//! so it is no longer wrapped in a `spawn_blocking` job, and only the reply
//! encoding happens on the reactor (plan §6.5 / §6.12.4 step 3).
//!
//! Request arguments reach the shared statement logic as bytes: the request is
//! re-encoded from the struct `volo` just decoded, so the SQL rendering
//! (filters, search expressions, literals) has exactly one implementation —
//! the one the byte-parity harnesses exercise. Requests are small; the reply
//! payload (column vectors) is built as the generated struct directly and is
//! never re-parsed.

use std::collections::HashMap;
use std::sync::atomic::AtomicI64;
use std::sync::Arc;

use executor::Engine;
use pilota::thrift::Message;
use volo_thrift::{ApplicationException, ApplicationExceptionKind, ServerError};

use crate::peer::PeerManager;
use crate::volo_gen::infinity as vg;
use crate::{dispatch_arm, ArmResponse, ColumnDefWire, PhysicalWire};

/// A protocol-level failure: the SDK turns message type 3 into an
/// `InfinityException` and does not retry (plan §6.4).
fn application(msg: impl Into<String>) -> ServerError {
    ServerError::Application(ApplicationException::new(
        ApplicationExceptionKind::INTERNAL_ERROR,
        msg.into(),
    ))
}

/// The shared service behind every connection (plan §6.5: `volo` keeps one
/// instance and a task per connection, so there is no per-connection state
/// here beyond the session id the client sends back on each call).
///
/// **No request gate.** An RPC mostly *waits*: the statement awaits the WAL,
/// the row allocator, its table's bottom half and the CPU pool, and none of
/// those waits holds a core. A permit per RPC therefore caps the number of
/// **in-flight** requests, not the amount of computation, and capping that at
/// the core count made a server with many live clients unusable: 10 000
/// clients checking in twice a second need 20 000 statements/s in flight, and
/// four permits on a four-core runner left them queueing behind each other
/// (`TestManyClientsStayOnline` measured the online count falling to 8663).
/// What actually bounds the work is underneath - the decode and the statement
/// run on `spawn_blocking`, and the blocking pool is the real ceiling.
///
/// Connections need no gate either: `volo` runs each one as a task, so an idle
/// client costs a future, not a thread (measured: 1000 idle connections keep
/// the process at its 23 threads). The reference's `connection_pool_size` only
/// exists to bound threads.
pub struct Service {
    engine: Option<Arc<Engine>>,
    sessions: Arc<std::sync::RwLock<HashMap<i64, i64>>>,
    next_session: Arc<AtomicI64>,
    peer: Arc<PeerManager>,
}

impl Service {
    pub fn new(
        engine: Option<Arc<Engine>>,
        sessions: Arc<std::sync::RwLock<HashMap<i64, i64>>>,
        next_session: Arc<AtomicI64>,
        peer: Arc<PeerManager>,
    ) -> Self {
        Self {
            engine,
            sessions,
            next_session,
            peer,
        }
    }

    /// Runs one RPC: encodes the arguments, executes the statement on the
    /// scheduler's pool and maps the outcome into the generated reply.
    async fn run<A, R>(
        &self,
        name: &'static str,
        args: A,
        sid: i64,
        map: fn(ArmResponse) -> Result<R, ServerError>,
    ) -> Result<R, ServerError>
    where
        A: Message + Send + 'static,
        R: Send + 'static,
    {
        let mut buf = pilota::BytesMut::with_capacity(512);
        {
            let mut proto = pilota::thrift::binary::TBinaryProtocol::new(&mut buf, false);
            args.encode(&mut proto)
                .map_err(|e| application(format!("encode {name}: {e}")))?;
        }
        let buf = buf.freeze();

        let engine = self.engine.clone();
        let sessions = Arc::clone(&self.sessions);
        let next_session = Arc::clone(&self.next_session);
        let peer = Arc::clone(&self.peer);
        let job = async move {
            if std::env::var_os("INF_DEBUG_SQL").is_some() {
                eprintln!("[RPC] {name}");
            }
            // `NewTxnManager::BeginTxnShared` runs once per statement, so every
            // RPC but the session handshake spends a transaction id — a
            // read-only one (`SELECT`, `SHOW …`, `GetDatabase`) included. The
            // reference's WAL carries the id in its frame header, so the two
            // engines only agree on that byte when both spend ids at the same
            // points.
            if !matches!(name, "Connect" | "Disconnect") {
                if let Some(eng) = engine.as_ref() {
                    eng.begin_statement();
                }
            }
            // The statement runs in the client's session, so `USE` and
            // unqualified table names resolve per client (plan §6.1).
            //
            // A session id the server does not know is `SessionNotFound`, as in
            // the reference (`Connect`/`Disconnect` are the handshake and are
            // exempt). Without this check a client that kept a stale id after
            // `Disconnect` kept working, because `ListDatabase` and friends
            // ignored the id they were handed.
            let out = match engine.as_ref() {
                Some(eng) if sid != 0 => {
                    if !matches!(name, "Connect" | "Disconnect")
                        && !sessions.read().unwrap().contains_key(&sid)
                    {
                        Ok(ArmResponse::Common(crate::CommonResponse {
                            error_code: 3034, // kSessionNotFound
                            error_msg: format!("Session id: {sid} isn't found"),
                            session_id: sid,
                        }))
                    } else {
                        // The session scope must wrap the **awaited** statement:
                        // the chain suspends on the WAL, the allocator, the
                        // bottom half and the CPU pool, and a task-local scope
                        // keeps the binding across those suspensions (a
                        // thread-local would not).
                        eng.with_session_async(
                            sid,
                            dispatch_arm(
                                name,
                                &buf,
                                engine.clone(),
                                &sessions,
                                &next_session,
                                &peer,
                                sid,
                            ),
                        )
                        .await
                    }
                }
                _ => {
                    dispatch_arm(
                        name,
                        &buf,
                        engine.clone(),
                        &sessions,
                        &next_session,
                        &peer,
                        0,
                    )
                    .await
                }
            };
            if let Some(eng) = engine.as_ref() {
                eng.end_statement();
            }
            out.map_err(|e| e.to_string())
        };
        // The statement chain is awaited on this task: no blocking thread is
        // held for the request's lifetime, so 100k in-flight connections remain
        // futures rather than threads (plan 6.12).
        let arm = job.await.map_err(application)?;
        map(arm)
    }
}

// -------- reply mapping --------

fn as_common(arm: ArmResponse) -> Result<vg::CommonResponse, ServerError> {
    match arm {
        ArmResponse::Common(r) => Ok(vg::CommonResponse {
            error_code: Some(r.error_code),
            error_msg: Some(r.error_msg.into()),
            session_id: Some(r.session_id),
        }),
        other => Err(unexpected("CommonResponse", &other)),
    }
}

fn unexpected(want: &str, arm: &ArmResponse) -> ServerError {
    let got = match arm {
        ArmResponse::Common(_) => "CommonResponse",
        ArmResponse::Select(_) => "SelectResponse",
        ArmResponse::Delete { .. } => "DeleteResponse",
        ArmResponse::List(_) => "list",
        ArmResponse::ShowTable { .. } => "ShowTableResponse",
        ArmResponse::ShowIndex { .. } => "ShowIndexResponse",
        ArmResponse::ShowDatabase { .. } => "ShowDatabaseResponse",
        ArmResponse::ShowCurrentNode { .. } => "ShowCurrentNodeResponse",
        ArmResponse::ShowSnapshot { .. } => "ShowSnapshotResponse",
        ArmResponse::ListSnapshots(_) => "ListSnapshotsResponse",
        ArmResponse::ShowConfig { .. } => "ShowConfigResponse",
    };
    application(format!("{want} expected, got {got}"))
}

fn select_response(r: crate::SelectResponseWire) -> vg::SelectResponse {
    vg::SelectResponse {
        error_code: Some(r.error_code),
        error_msg: Some(r.error_msg.into()),
        column_defs: Some(r.column_defs.iter().map(column_def).collect()),
        column_fields: Some(r.column_fields.iter().map(column_field).collect()),
        extra_result: Some(r.extra_result.into()),
    }
}

fn as_select(arm: ArmResponse) -> Result<vg::SelectResponse, ServerError> {
    match arm {
        ArmResponse::Select(r) => Ok(select_response(r)),
        other => Err(unexpected("SelectResponse", &other)),
    }
}

fn column_field(f: &crate::ColumnField) -> vg::ColumnField {
    vg::ColumnField {
        column_type: Some(vg::ColumnType::from(f.column_type)),
        column_vectors: Some(
            f.column_vectors
                .iter()
                .map(|v| pilota::Bytes::from(v.clone()))
                .collect(),
        ),
        column_name: Some(f.column_name.clone().into()),
        bitmasks: Some(f.bitmasks.clone()),
    }
}

fn column_def(c: &ColumnDefWire) -> vg::ColumnDef {
    vg::ColumnDef {
        id: Some(c.id),
        name: Some(c.name.clone().into()),
        data_type: Some(vg::DataType {
            logic_type: Some(vg::LogicType::from(c.logic_type)),
            physical_type: physical_type(&c.physical),
        }),
        // The reference's `SelectResponse` carries the type only; the
        // constraint/default/comment fields stay absent, as before.
        constraints: None,
        constant_expr: None,
        comment: None,
    }
}

fn physical_type(p: &PhysicalWire) -> Option<Box<vg::PhysicalType>> {
    Some(Box::new(match p {
        PhysicalWire::None => return None,
        PhysicalWire::Scalar(logic) => {
            if matches!(logic, 11 | 22 | 23) {
                vg::PhysicalType::VarcharType(vg::VarcharType {})
            } else {
                vg::PhysicalType::NumberType(vg::NumberType {})
            }
        }
        PhysicalWire::Embedding {
            dimension,
            element_type,
        } => vg::PhysicalType::EmbeddingType(vg::EmbeddingType {
            dimension: Some(*dimension),
            element_type: Some(vg::ElementType::from(*element_type)),
        }),
        PhysicalWire::Sparse {
            dimension,
            element_type,
            index_type,
        } => vg::PhysicalType::SparseType(vg::SparseType {
            dimension: Some(*dimension),
            element_type: Some(vg::ElementType::from(*element_type)),
            index_type: Some(vg::ElementType::from(*index_type)),
        }),
        PhysicalWire::Array {
            element_logic_type,
            element,
        } => vg::PhysicalType::ArrayType(vg::ArrayType {
            element_data_type: Some(Box::new(vg::DataType {
                logic_type: Some(vg::LogicType::from(*element_logic_type)),
                physical_type: physical_type(element),
            })),
        }),
    }))
}

fn as_delete(arm: ArmResponse) -> Result<vg::DeleteResponse, ServerError> {
    match arm {
        ArmResponse::Delete {
            error_code,
            error_msg,
            deleted_rows,
        } => Ok(vg::DeleteResponse {
            error_code: Some(error_code),
            error_msg: Some(error_msg.into()),
            deleted_rows: Some(deleted_rows),
        }),
        other => Err(unexpected("DeleteResponse", &other)),
    }
}

fn names(arm: ArmResponse) -> Result<Vec<String>, ServerError> {
    match arm {
        ArmResponse::List(names) => Ok(names),
        other => Err(unexpected("a name list", &other)),
    }
}

fn as_list_database(arm: ArmResponse) -> Result<vg::ListDatabaseResponse, ServerError> {
    // A failed arm (the session check rejecting an unknown id) keeps its code:
    // the reply type has `error_code` fields, so nothing forces an empty-but-OK
    // answer.
    if let ArmResponse::Common(error) = &arm {
        return Ok(vg::ListDatabaseResponse {
            error_code: Some(error.error_code),
            error_msg: Some(error.error_msg.clone().into()),
            db_names: None,
            db_dirs: None,
            db_comments: None,
        });
    }
    Ok(vg::ListDatabaseResponse {
        error_code: Some(0),
        error_msg: Some("OK".into()),
        db_names: Some(names(arm)?.into_iter().map(Into::into).collect()),
        db_dirs: None,
        db_comments: None,
    })
}

fn as_list_table(arm: ArmResponse) -> Result<vg::ListTableResponse, ServerError> {
    Ok(vg::ListTableResponse {
        error_code: Some(0),
        error_msg: Some("OK".into()),
        table_names: Some(names(arm)?.into_iter().map(Into::into).collect()),
    })
}

fn as_list_index(arm: ArmResponse) -> Result<vg::ListIndexResponse, ServerError> {
    Ok(vg::ListIndexResponse {
        error_code: Some(0),
        error_msg: Some("OK".into()),
        index_names: Some(names(arm)?.into_iter().map(Into::into).collect()),
    })
}

fn as_show_table(arm: ArmResponse) -> Result<vg::ShowTableResponse, ServerError> {
    match arm {
        ArmResponse::ShowTable {
            db,
            table,
            error_code,
            error_msg,
            column_count,
            segment_count,
            row_count,
        } => Ok(vg::ShowTableResponse {
            error_code: Some(error_code),
            error_msg: Some(error_msg.into()),
            database_name: Some(db.into()),
            table_name: Some(table.into()),
            store_dir: Some("".into()),
            column_count: Some(column_count),
            segment_count: Some(segment_count),
            row_count: Some(row_count),
        }),
        other => Err(unexpected("ShowTableResponse", &other)),
    }
}

fn as_show_index(arm: ArmResponse) -> Result<vg::ShowIndexResponse, ServerError> {
    match arm {
        ArmResponse::ShowIndex {
            db,
            table,
            index,
            comment,
            error_code,
            error_msg,
            index_type,
            columns,
        } => Ok(vg::ShowIndexResponse {
            error_code: Some(error_code),
            error_msg: Some(error_msg.into()),
            db_name: Some(db.into()),
            table_name: Some(table.into()),
            index_name: Some(index.into()),
            index_comment: Some(comment.into()),
            index_type: Some(index_type.into()),
            index_column_names: Some(columns.into()),
            index_column_ids: Some("".into()),
            index_function_info: Some("".into()),
            other_parameters: Some("".into()),
            store_dir: Some("".into()),
            segment_index_count: Some("".into()),
        }),
        other => Err(unexpected("ShowIndexResponse", &other)),
    }
}

fn as_show_database(arm: ArmResponse) -> Result<vg::ShowDatabaseResponse, ServerError> {
    match arm {
        ArmResponse::ShowDatabase {
            name,
            table_count,
            comment,
        } => Ok(vg::ShowDatabaseResponse {
            error_code: Some(0),
            error_msg: Some("OK".into()),
            database_name: Some(name.into()),
            store_dir: Some("".into()),
            table_count: Some(table_count),
            comment: Some(comment.into()),
        }),
        other => Err(unexpected("ShowDatabaseResponse", &other)),
    }
}

fn as_show_current_node(arm: ArmResponse) -> Result<vg::ShowCurrentNodeResponse, ServerError> {
    match arm {
        ArmResponse::ShowCurrentNode {
            node_role,
            server_status,
        } => Ok(vg::ShowCurrentNodeResponse {
            error_code: Some(0),
            error_msg: Some("OK".into()),
            node_role: Some(node_role.into()),
            server_status: Some(server_status.into()),
        }),
        other => Err(unexpected("ShowCurrentNodeResponse", &other)),
    }
}

fn snapshot_info(name: &str) -> vg::SnapshotInfo {
    vg::SnapshotInfo {
        name: Some(name.to_string().into()),
        scope: Some("table".into()),
        time: Some("".into()),
        commit: Some(0),
        size: Some("".into()),
    }
}

fn as_show_snapshot(arm: ArmResponse) -> Result<vg::ShowSnapshotResponse, ServerError> {
    match arm {
        ArmResponse::ShowSnapshot { name, found } => Ok(vg::ShowSnapshotResponse {
            error_code: Some(if found { 0 } else { 3022 }),
            error_msg: Some(if found {
                "OK".to_string().into()
            } else {
                format!("snapshot '{name}' does not exist").into()
            }),
            snapshot: found.then(|| snapshot_info(&name)),
        }),
        other => Err(unexpected("ShowSnapshotResponse", &other)),
    }
}

fn as_list_snapshots(arm: ArmResponse) -> Result<vg::ListSnapshotsResponse, ServerError> {
    match arm {
        ArmResponse::ListSnapshots(names) => Ok(vg::ListSnapshotsResponse {
            error_code: Some(0),
            error_msg: Some("OK".into()),
            snapshots: Some(names.iter().map(|n| snapshot_info(n)).collect()),
        }),
        other => Err(unexpected("ListSnapshotsResponse", &other)),
    }
}

fn as_show_config(arm: ArmResponse) -> Result<vg::ShowConfigResponse, ServerError> {
    match arm {
        ArmResponse::ShowConfig { name, value } => Ok(vg::ShowConfigResponse {
            error_code: Some(if value.is_some() { 0 } else { 3014 }),
            error_msg: Some(value.as_ref().map_or("unknown config", |_| "OK").into()),
            config_value: value.map(|v| {
                // The reference's `IntegerOption`s answer with `int_value`;
                // everything else carries a string.
                if crate::http::config_is_numeric(&name) {
                    vg::ConfigValue::IntValue(v.parse().unwrap_or(0))
                } else {
                    vg::ConfigValue::StringValue(v.into())
                }
            }),
            config_name: Some(name.into()),
        }),
        other => Err(unexpected("ShowConfigResponse", &other)),
    }
}

impl vg::InfinityService for Service {
    async fn connect(
        &self,
        request: vg::ConnectRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        self.run(
            "Connect",
            vg::InfinityServiceConnectArgsRecv { request },
            0,
            as_common,
        )
        .await
    }

    async fn disconnect(
        &self,
        request: vg::CommonRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "Disconnect",
            vg::InfinityServiceDisconnectArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn create_database(
        &self,
        request: vg::CreateDatabaseRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "CreateDatabase",
            vg::InfinityServiceCreateDatabaseArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn drop_database(
        &self,
        request: vg::DropDatabaseRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "DropDatabase",
            vg::InfinityServiceDropDatabaseArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn create_table(
        &self,
        request: vg::CreateTableRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "CreateTable",
            vg::InfinityServiceCreateTableArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn drop_table(
        &self,
        request: vg::DropTableRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "DropTable",
            vg::InfinityServiceDropTableArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn rename_table(
        &self,
        request: vg::RenameTableRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "RenameTable",
            vg::InfinityServiceRenameTableArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn insert(&self, request: vg::InsertRequest) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "Insert",
            vg::InfinityServiceInsertArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn import(&self, request: vg::ImportRequest) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "Import",
            vg::InfinityServiceImportArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn export(&self, request: vg::ExportRequest) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "Export",
            vg::InfinityServiceExportArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn select(&self, request: vg::SelectRequest) -> Result<vg::SelectResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "Select",
            vg::InfinityServiceSelectArgsRecv { request },
            sid,
            as_select,
        )
        .await
    }

    async fn explain(
        &self,
        request: vg::ExplainRequest,
    ) -> Result<vg::SelectResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "Explain",
            vg::InfinityServiceExplainArgsRecv { request },
            sid,
            as_select,
        )
        .await
    }

    async fn delete(&self, request: vg::DeleteRequest) -> Result<vg::DeleteResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "Delete",
            vg::InfinityServiceDeleteArgsRecv { request },
            sid,
            as_delete,
        )
        .await
    }

    async fn update(&self, request: vg::UpdateRequest) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "Update",
            vg::InfinityServiceUpdateArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn list_database(
        &self,
        request: vg::ListDatabaseRequest,
    ) -> Result<vg::ListDatabaseResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "ListDatabase",
            vg::InfinityServiceListDatabaseArgsRecv { request },
            sid,
            as_list_database,
        )
        .await
    }

    async fn list_table(
        &self,
        request: vg::ListTableRequest,
    ) -> Result<vg::ListTableResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "ListTable",
            vg::InfinityServiceListTableArgsRecv { request },
            sid,
            as_list_table,
        )
        .await
    }

    async fn list_index(
        &self,
        request: vg::ListIndexRequest,
    ) -> Result<vg::ListIndexResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "ListIndex",
            vg::InfinityServiceListIndexArgsRecv { request },
            sid,
            as_list_index,
        )
        .await
    }

    async fn show_table(
        &self,
        request: vg::ShowTableRequest,
    ) -> Result<vg::ShowTableResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "ShowTable",
            vg::InfinityServiceShowTableArgsRecv { request },
            sid,
            as_show_table,
        )
        .await
    }

    async fn show_columns(
        &self,
        request: vg::ShowColumnsRequest,
    ) -> Result<vg::SelectResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "ShowColumns",
            vg::InfinityServiceShowColumnsArgsRecv { request },
            sid,
            as_select,
        )
        .await
    }

    async fn show_database(
        &self,
        request: vg::ShowDatabaseRequest,
    ) -> Result<vg::ShowDatabaseResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "ShowDatabase",
            vg::InfinityServiceShowDatabaseArgsRecv { request },
            sid,
            as_show_database,
        )
        .await
    }

    async fn show_segments(
        &self,
        request: vg::ShowSegmentsRequest,
    ) -> Result<vg::SelectResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "ShowSegments",
            vg::InfinityServiceShowSegmentsArgsRecv { request },
            sid,
            as_select,
        )
        .await
    }

    async fn show_segment(
        &self,
        _request: vg::ShowSegmentRequest,
    ) -> Result<vg::ShowSegmentResponse, ServerError> {
        // Segment/block introspection was never served by this engine; the
        // reference answers from its own storage layers.
        Err(application("unknown method: ShowSegment"))
    }

    async fn show_blocks(
        &self,
        request: vg::ShowBlocksRequest,
    ) -> Result<vg::SelectResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "ShowBlocks",
            vg::InfinityServiceShowBlocksArgsRecv { request },
            sid,
            as_select,
        )
        .await
    }

    async fn show_block(
        &self,
        _request: vg::ShowBlockRequest,
    ) -> Result<vg::ShowBlockResponse, ServerError> {
        Err(application("unknown method: ShowBlock"))
    }

    async fn show_block_column(
        &self,
        _request: vg::ShowBlockColumnRequest,
    ) -> Result<vg::ShowBlockColumnResponse, ServerError> {
        Err(application("unknown method: ShowBlockColumn"))
    }

    async fn show_current_node(
        &self,
        request: vg::ShowCurrentNodeRequest,
    ) -> Result<vg::ShowCurrentNodeResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "ShowCurrentNode",
            vg::InfinityServiceShowCurrentNodeArgsRecv { request },
            sid,
            as_show_current_node,
        )
        .await
    }

    async fn get_database(
        &self,
        request: vg::GetDatabaseRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "GetDatabase",
            vg::InfinityServiceGetDatabaseArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn get_table(
        &self,
        request: vg::GetTableRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "GetTable",
            vg::InfinityServiceGetTableArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn create_index(
        &self,
        request: vg::CreateIndexRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "CreateIndex",
            vg::InfinityServiceCreateIndexArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn drop_index(
        &self,
        request: vg::DropIndexRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "DropIndex",
            vg::InfinityServiceDropIndexArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn show_index(
        &self,
        request: vg::ShowIndexRequest,
    ) -> Result<vg::ShowIndexResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "ShowIndex",
            vg::InfinityServiceShowIndexArgsRecv { request },
            sid,
            as_show_index,
        )
        .await
    }

    async fn optimize(
        &self,
        request: vg::OptimizeRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "Optimize",
            vg::InfinityServiceOptimizeArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn alter_index(
        &self,
        request: vg::AlterIndexRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "AlterIndex",
            vg::InfinityServiceAlterIndexArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn add_columns(
        &self,
        request: vg::AddColumnsRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "AddColumns",
            vg::InfinityServiceAddColumnsArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn drop_columns(
        &self,
        request: vg::DropColumnsRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "DropColumns",
            vg::InfinityServiceDropColumnsArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn cleanup(&self, request: vg::CommonRequest) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "Cleanup",
            vg::InfinityServiceCleanupArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn dump_index(
        &self,
        request: vg::DumpIndexRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "DumpIndex",
            vg::InfinityServiceDumpIndexArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn command(
        &self,
        request: vg::CommandRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "Command",
            vg::InfinityServiceCommandArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn flush(&self, request: vg::FlushRequest) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "Flush",
            vg::InfinityServiceFlushArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn compact(
        &self,
        request: vg::CompactRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "Compact",
            vg::InfinityServiceCompactArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn create_table_snapshot(
        &self,
        request: vg::CreateTableSnapshotRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "CreateTableSnapshot",
            vg::InfinityServiceCreateTableSnapshotArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn create_database_snapshot(
        &self,
        request: vg::CreateDatabaseSnapshotRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "CreateDatabaseSnapshot",
            vg::InfinityServiceCreateDatabaseSnapshotArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn create_system_snapshot(
        &self,
        request: vg::CreateSystemSnapshotRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "CreateSystemSnapshot",
            vg::InfinityServiceCreateSystemSnapshotArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn restore_snapshot(
        &self,
        request: vg::RestoreSnapshotRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "RestoreSnapshot",
            vg::InfinityServiceRestoreSnapshotArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn show_snapshot(
        &self,
        request: vg::ShowSnapshotRequest,
    ) -> Result<vg::ShowSnapshotResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "ShowSnapshot",
            vg::InfinityServiceShowSnapshotArgsRecv { request },
            sid,
            as_show_snapshot,
        )
        .await
    }

    async fn list_snapshots(
        &self,
        request: vg::ListSnapshotsRequest,
    ) -> Result<vg::ListSnapshotsResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "ListSnapshots",
            vg::InfinityServiceListSnapshotsArgsRecv { request },
            sid,
            as_list_snapshots,
        )
        .await
    }

    async fn drop_snapshot(
        &self,
        request: vg::DropSnapshotRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "DropSnapshot",
            vg::InfinityServiceDropSnapshotArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn set_config(
        &self,
        request: vg::SetConfigRequest,
    ) -> Result<vg::CommonResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "SetConfig",
            vg::InfinityServiceSetConfigArgsRecv { request },
            sid,
            as_common,
        )
        .await
    }

    async fn show_config(
        &self,
        request: vg::ShowConfigRequest,
    ) -> Result<vg::ShowConfigResponse, ServerError> {
        let sid = request.session_id.unwrap_or(0);
        self.run(
            "ShowConfig",
            vg::InfinityServiceShowConfigArgsRecv { request },
            sid,
            as_show_config,
        )
        .await
    }
}
