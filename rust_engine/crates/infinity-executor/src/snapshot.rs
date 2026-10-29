//! Snapshots: the reference's `<snapshot_dir>/<name>/` layout and its WAL
//! commands (`WalCommandType` 110–115).
//!
//! The reference keeps a snapshot as **one directory per snapshot name**
//! (`Config::SnapshotDir()`): `<dir>/<name>/<name>.json` is the metadata
//! (`TableSnapshotInfo` / `DatabaseSnapshotInfo` / `SystemSnapshotInfo`'s
//! `CreateSnapshotMetadataJSON`) and the data files sit beside it under the
//! path they have in the data directory (`db_1/tbl_13/seg_0/blk_0/0.col`).
//! `SHOW SNAPSHOTS` lists that directory (`SnapshotBrief::GetSnapshots`) and
//! `DROP SNAPSHOT` renames it to `deleted_<name>` before removing it
//! (`snapshot_impl.cpp:59`). This module writes exactly that layout.
//!
//! The WAL side is the six commands the reference logs: `CREATE_TABLE_SNAPSHOT`
//! (110) … `RESTORE_SYSTEM_SNAPSHOT` (115), whose payloads are defined in
//! `wal_entry_impl.cpp:1267-1329`. Replay skips the create commands
//! (`new_txn_impl.cpp:2358`), as it does here.

use std::path::{Path, PathBuf};

use infinity_catalog::value_format;
use infinity_common::{Error, Result, TxnTimeStamp};
use infinity_expression::json::Json;
use infinity_storage::block_index::BlockIndex;
use infinity_storage::wal::ddl_def::{ColumnDef, ConstantExpr, TableDef};
use infinity_storage::wal_entry::WalCmd;

use crate::Engine;

/// `SnapshotScope` (`src/parser/statement/command_statement.h:235`).
const SCOPE_TABLE: i64 = 0;
const SCOPE_DATABASE: i64 = 1;
const SCOPE_SYSTEM: i64 = 2;

/// The reference's `first_delete_ts` for a segment nobody has deleted from.
const NO_DELETE_TS: u64 = u64::MAX;

/// One line of `SHOW SNAPSHOTS` (C++ `SnapshotBrief`,
/// `src/executor/operator/snapshot/snapshot_brief.cppm`).
pub struct SnapshotBrief {
    pub name: String,
    /// `Table` / `Database` / `System` (`SnapshotScope`).
    pub scope: String,
    /// The snapshot directory's ctime, `%Y-%m-%d %H:%M:%S`.
    pub create_time: String,
    /// The snapshot's ts (the metadata's `max_commit_ts`, the same number the
    /// WAL command carries), which both engines now write and read back
    /// (`SnapshotBrief::GetSnapshots`).
    pub commit_ts: u64,
    pub size: u64,
}

impl SnapshotBrief {
    /// C++ `Utility::FormatByteSize` (`src/common/utility/utility_impl.cpp:40`).
    pub fn format_size(bytes: u64) -> String {
        const SUFFIXES: [&str; 9] = ["B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"];
        if bytes == 0 {
            return "0B".to_string();
        }
        let index = ((bytes as f64).log2() / 10.0) as usize;
        let index = index.min(SUFFIXES.len() - 1);
        let size = bytes as f64 / (1u64 << (index * 10)) as f64;
        format!("{:.2}{}", size, SUFFIXES[index])
    }
}

impl Engine {
    /// Every snapshot with its `SHOW SNAPSHOTS` line
    /// (`SnapshotBrief::GetSnapshots`): the directory has to hold
    /// `<name>/<name>.json`, and `deleted_*` / `tmp_*` staging directories are
    /// not snapshots.
    pub fn snapshot_briefs(&self) -> Result<Vec<SnapshotBrief>> {
        let root = self.snapshot_dir();
        let Ok(entries) = std::fs::read_dir(&root) else {
            return Ok(Vec::new());
        };
        let mut out = Vec::new();
        for entry in entries.flatten() {
            let path = entry.path();
            if !path.is_dir() {
                continue;
            }
            let Some(name) = path.file_name().and_then(|n| n.to_str()) else {
                continue;
            };
            if name.starts_with("deleted_") || name.starts_with("tmp_") {
                continue;
            }
            let json_path = path.join(format!("{name}.json"));
            if !json_path.exists() {
                continue;
            }
            let Ok(text) = std::fs::read_to_string(&json_path) else {
                continue;
            };
            let Some(json) = infinity_expression::json::parse(&text) else {
                continue;
            };
            let scope = match json_int_of(&json, "snapshot_scope") {
                SCOPE_DATABASE => "Database",
                SCOPE_SYSTEM => "System",
                _ => "Table",
            };
            let created = entry
                .metadata()
                .ok()
                .and_then(|m| m.created().or(m.modified()).ok())
                .map(|t| {
                    let secs = t
                        .duration_since(std::time::UNIX_EPOCH)
                        .map(|d| d.as_secs() as i64)
                        .unwrap_or(0);
                    format_time(secs)
                })
                .unwrap_or_default();
            out.push(SnapshotBrief {
                name: name.to_string(),
                scope: scope.to_string(),
                create_time: created,
                commit_ts: snapshot_commit_ts(&json),
                size: dir_size(&path),
            });
        }
        out.sort_by(|a, b| a.name.cmp(&b.name));
        Ok(out)
    }

    /// The next table id a database would hand out
    /// (`DatabaseSnapshotInfo::db_next_table_id_str_`).
    fn next_table_id_str(&self, db: &str) -> String {
        let max = self
            .storage
            .catalog
            .list_tables()
            .unwrap_or_default()
            .iter()
            .filter(|m| {
                m.name
                    .rsplit_once('.')
                    .map(|(d, _)| d.eq_ignore_ascii_case(db))
                    .unwrap_or(false)
            })
            .map(|m| m.id)
            .max()
            .unwrap_or(0);
        (max + 1).to_string()
    }

    /// C++ `Config::SnapshotDir()`: the root every snapshot directory lives
    /// under. Configured through [`crate::EngineOptions::snapshot_dir`];
    /// otherwise it is `<data root>/snapshot`.
    pub fn snapshot_dir(&self) -> PathBuf {
        self.snapshot_dir.clone()
    }

    /// Every snapshot the directory holds, sorted by name
    /// (`SnapshotBrief::GetSnapshots`).
    pub fn snapshot_names(&self) -> Result<Vec<String>> {
        let mut out: Vec<String> = std::fs::read_dir(self.snapshot_dir())
            .map(|entries| {
                entries
                    .flatten()
                    .filter(|e| e.path().is_dir())
                    .filter_map(|e| e.file_name().to_str().map(str::to_string))
                    // The reference's `deleted_<name>` staging directory is not
                    // a snapshot.
                    .filter(|n| !n.starts_with("deleted_"))
                    .collect()
            })
            .unwrap_or_default();
        out.sort();
        Ok(out)
    }

    /// `CREATE SNAPSHOT <name> ON TABLE <table>`.
    ///
    /// Copies the table's files into `<snapshot_dir>/<name>/` and writes its
    /// metadata JSON, then logs `CREATE_TABLE_SNAPSHOT`. The reference refuses
    /// a name that already exists (`Status::SnapshotAlreadyExists`).
    pub fn create_table_snapshot(&self, db: &str, table: &str, name: &str) -> Result<()> {
        let table = if table.contains('.') {
            table.to_ascii_lowercase()
        } else {
            format!("{}.{}", db.to_ascii_lowercase(), table.to_ascii_lowercase())
        };
        let (db_name, table_name) = split_qualified(&table);
        // Missing tables keep the port's usual wording, which the server
        // renders as the reference's `Table: {name} doesn't exist.` (3022).
        let meta = self
            .storage
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let db_id = self.database_id(&db_name);
        // The snapshot's ts is fixed before anything is written, so the
        // metadata and the WAL command carry the same number.
        let snapshot_ts = self.begin_ts();
        let dir = self.prepare_snapshot_dir(name)?;
        let json = self.write_table_snapshot(&dir, name, db_id, &meta, snapshot_ts)?;
        self.write_snapshot_json(&dir, name, &json)?;
        self.log_snapshot(WalCmd::CreateTableSnapshot {
            db_name,
            table_name,
            snapshot_name: name.to_string(),
            max_commit_ts: snapshot_ts,
        })
    }

    /// `CREATE SNAPSHOT <name> ON DATABASE <db>`: every table of `db`, under
    /// one `DATABASE`-scoped metadata JSON, logged as `CREATE_DB_SNAPSHOT`.
    pub fn create_database_snapshot(&self, name: &str, db: &str) -> Result<()> {
        let db_id = self.database_id(db);
        let snapshot_ts = self.begin_ts();
        let dir = self.prepare_snapshot_dir(name)?;
        let mut table_snapshots = Vec::new();
        for meta in self.storage.catalog.list_tables()? {
            let (meta_db, _) = split_qualified(&meta.name);
            if !meta_db.eq_ignore_ascii_case(db) {
                continue;
            }
            table_snapshots.push(self.write_table_snapshot(
                &dir,
                name,
                db_id,
                &meta,
                snapshot_ts,
            )?);
        }
        let json = Json::Obj(vec![
            ("database_name".into(), Json::Str(db.to_string())),
            ("db_comment".into(), Json::Str(String::new())),
            ("db_id_str".into(), Json::Str(db_id.to_string())),
            (
                "db_next_table_id_str".into(),
                Json::Str(self.next_table_id_str(db)),
            ),
            ("max_commit_ts".into(), Json::Int(snapshot_ts as i64)),
            ("snapshot_name".into(), Json::Str(name.to_string())),
            ("snapshot_scope".into(), Json::Int(SCOPE_DATABASE)),
            ("table_snapshots".into(), Json::Arr(table_snapshots)),
            ("version".into(), Json::Int(1)),
        ]);
        self.write_snapshot_json(&dir, name, &json)?;
        self.log_snapshot(WalCmd::CreateDbSnapshot {
            db_name: db.to_string(),
            snapshot_name: name.to_string(),
            max_commit_ts: snapshot_ts,
        })
    }

    /// `CREATE SNAPSHOT <name> ON SYSTEM`: every table of every database,
    /// logged as `CREATE_SYSTEM_SNAPSHOT`.
    pub fn create_system_snapshot(&self, name: &str) -> Result<()> {
        let databases = self.storage.catalog.list_databases()?;
        let mut db_snapshots = Vec::new();
        let snapshot_ts = self.begin_ts();
        // The whole snapshot has to be refused before anything is written when
        // the name is taken, so the directory is created first.
        let dir = self.prepare_snapshot_dir(name)?;
        for db in &databases {
            let db_id = self.database_id(db);
            let mut table_snapshots = Vec::new();
            for meta in self.storage.catalog.list_tables()? {
                let (meta_db, _) = split_qualified(&meta.name);
                if !meta_db.eq_ignore_ascii_case(db) {
                    continue;
                }
                table_snapshots.push(self.write_table_snapshot(
                    &dir,
                    name,
                    db_id,
                    &meta,
                    snapshot_ts,
                )?);
            }
            db_snapshots.push(Json::Obj(vec![
                ("database_name".into(), Json::Str(db.clone())),
                ("db_comment".into(), Json::Str(String::new())),
                ("db_id_str".into(), Json::Str(db_id.to_string())),
                (
                    "db_next_table_id_str".into(),
                    Json::Str(self.next_table_id_str(db)),
                ),
                ("max_commit_ts".into(), Json::Int(snapshot_ts as i64)),
                ("snapshot_name".into(), Json::Str(name.to_string())),
                ("snapshot_scope".into(), Json::Int(SCOPE_DATABASE)),
                ("table_snapshots".into(), Json::Arr(table_snapshots)),
                ("version".into(), Json::Int(1)),
            ]));
        }
        let json = Json::Obj(vec![
            ("database_snapshots".into(), Json::Arr(db_snapshots)),
            ("max_commit_ts".into(), Json::Int(snapshot_ts as i64)),
            ("snapshot_name".into(), Json::Str(name.to_string())),
            ("snapshot_scope".into(), Json::Int(SCOPE_SYSTEM)),
            ("version".into(), Json::Int(1)),
        ]);
        self.write_snapshot_json(&dir, name, &json)?;
        self.log_snapshot(WalCmd::CreateSystemSnapshot {
            snapshot_name: name.to_string(),
            max_commit_ts: snapshot_ts,
        })
    }

    /// `DROP SNAPSHOT <name>`: the reference renames the directory to
    /// `deleted_<name>` and then removes it (`snapshot_impl.cpp:59`).
    pub fn drop_snapshot(&self, name: &str) -> Result<()> {
        let root = self.snapshot_dir();
        let path = root.join(name);
        if !path.exists() {
            return Err(Error::SnapshotNotFound(format!(
                "Snapshot: {name} not found"
            )));
        }
        let deleted = root.join(format!("deleted_{name}"));
        if deleted.exists() {
            let _ = std::fs::remove_dir_all(&deleted);
        }
        std::fs::rename(&path, &deleted).map_err(|e| Error::io(&deleted, e))?;
        std::fs::remove_dir_all(&deleted).map_err(|e| Error::io(&deleted, e))?;
        Ok(())
    }

    /// `RESTORE TABLE SNAPSHOT <name>`: recreates the table from the snapshot's
    /// metadata and copies its files back, then logs
    /// `RESTORE_TABLE_SNAPSHOT`.
    pub fn restore_table_snapshot(&self, name: &str) -> Result<()> {
        let json = self.read_snapshot_json(name)?;
        let table = json_table(&json, name)?;
        let files = self.restore_table(table)?;
        let db_id: u64 = json_str(&json, "db_id_str").parse().unwrap_or(0);
        let table_id: u64 = json_str(table, "table_id_str").parse().unwrap_or(0);
        self.log_snapshot(WalCmd::RestoreTableSnapshot {
            db_name: json_str(&json, "database_name"),
            db_id: db_id.to_string(),
            table_name: json_str(table, "table_name"),
            table_id: table_id.to_string(),
            snapshot_name: name.to_string(),
            table_def: table_def_of(table),
            segment_infos: Vec::new(),
            index_cmds: Vec::new(),
            files,
        })
    }

    /// `RESTORE DATABASE SNAPSHOT <name>`, logged as
    /// `RESTORE_DATABASE_SNAPSHOT` with one nested table command per table.
    pub fn restore_database_snapshot(&self, name: &str) -> Result<()> {
        let json = self.read_snapshot_json(name)?;
        let db_name = json_str(&json, "database_name");
        let db_id = json_str(&json, "db_id_str");
        let mut table_cmds = Vec::new();
        for table in json_arr(&json, "table_snapshots") {
            let files = self.restore_table(&table)?;
            let table_id = json_str(&table, "table_id_str");
            table_cmds.push(WalCmd::RestoreTableSnapshot {
                db_name: db_name.clone(),
                db_id: db_id.clone(),
                table_name: json_str(&table, "table_name"),
                table_id,
                snapshot_name: name.to_string(),
                table_def: table_def_of(&table),
                segment_infos: Vec::new(),
                index_cmds: Vec::new(),
                files,
            });
        }
        self.log_snapshot(WalCmd::RestoreDatabaseSnapshot {
            db_name,
            db_id,
            db_comment: json_str(&json, "db_comment"),
            snapshot_name: name.to_string(),
            table_cmds,
        })
    }

    /// `RESTORE SYSTEM SNAPSHOT <name>`, logged as `RESTORE_SYSTEM_SNAPSHOT`
    /// with one nested database command per database.
    pub fn restore_system_snapshot(&self, name: &str) -> Result<()> {
        let json = self.read_snapshot_json(name)?;
        let mut db_cmds = Vec::new();
        for db in json_arr(&json, "database_snapshots") {
            let db_name = json_str(&db, "database_name");
            let db_id = json_str(&db, "db_id_str");
            let mut table_cmds = Vec::new();
            for table in json_arr(&db, "table_snapshots") {
                let files = self.restore_table(&table)?;
                let table_id = json_str(&table, "table_id_str");
                table_cmds.push(WalCmd::RestoreTableSnapshot {
                    db_name: db_name.clone(),
                    db_id: db_id.clone(),
                    table_name: json_str(&table, "table_name"),
                    table_id,
                    snapshot_name: name.to_string(),
                    table_def: table_def_of(&table),
                    segment_infos: Vec::new(),
                    index_cmds: Vec::new(),
                    files,
                });
            }
            db_cmds.push(WalCmd::RestoreDatabaseSnapshot {
                db_name,
                db_id,
                db_comment: json_str(&db, "db_comment"),
                snapshot_name: name.to_string(),
                table_cmds,
            });
        }
        self.log_snapshot(WalCmd::RestoreSystemSnapshot {
            snapshot_name: name.to_string(),
            db_cmds,
        })
    }

    // ------------------------------------------------------------------
    // metadata
    // ------------------------------------------------------------------

    /// Writes one table's part of a snapshot: its files and its metadata.
    /// Returns the metadata so a database/system snapshot can nest it.
    ///
    /// The files are the whole payload — the rows ride in the block columns,
    /// exactly as in the reference, so nothing else is written.
    fn write_table_snapshot(
        &self,
        dir: &Path,
        name: &str,
        db_id: u64,
        meta: &infinity_catalog::TableMeta,
        snapshot_ts: TxnTimeStamp,
    ) -> Result<Json> {
        // The reference snapshots through a checkpoint
        // (`NewTxn::CheckpointforSnapshot`), so the table's blocks exist by the
        // time the files are copied: `segments` describes them and the copies
        // carry the rows.
        let _ = self.write_cpp_blocks(&meta.name);
        let (db_name, _) = split_qualified(&meta.name);
        let json = self.table_snapshot_json(&db_name, db_id, meta, name, snapshot_ts)?;
        self.copy_table_files(db_id, meta.id, dir)?;
        Ok(json)
    }

    /// `TableSnapshotInfo::CreateSnapshotMetadataJSON`
    /// (`snapshot_info_impl.cpp:443`). Keys are sorted, which is what
    /// `nlohmann::json::dump()` emits.
    fn table_snapshot_json(
        &self,
        db_name: &str,
        db_id: u64,
        meta: &infinity_catalog::TableMeta,
        snapshot_name: &str,
        snapshot_ts: TxnTimeStamp,
    ) -> Result<Json> {
        let table_name = meta.name.rsplit('.').next().unwrap_or(&meta.name);
        let mut column_definition = Vec::new();
        for (i, column) in meta.columns.iter().enumerate() {
            let column_type = infinity_catalog::cpp_layout::data_type_of_text_with_store(
                meta.types.get(i).map(String::as_str).unwrap_or(""),
                meta.sparse_stores.get(i).copied().flatten(),
            )
            .ok_or_else(|| {
                Error::Invalid(format!(
                    "column type '{}' has no C++ type code",
                    meta.types.get(i).map(String::as_str).unwrap_or("")
                ))
            })?;
            let mut entries: Vec<(String, Json)> = Vec::new();
            if let Some(comment) = meta.comments.get(i).and_then(|c| c.as_deref()) {
                if !comment.is_empty() {
                    entries.push(("column_comment".into(), Json::Str(comment.to_string())));
                }
            }
            entries.push((
                "column_id".into(),
                Json::Int(meta.ids.get(i).copied().unwrap_or(i as u64) as i64),
            ));
            entries.push(("column_name".into(), Json::Str(column.clone())));
            entries.push(("column_type".into(), column_type));
            if meta.not_nulls.get(i).copied().unwrap_or(false) {
                // `ColumnDef::constraints_` carries the ordinal of
                // `ConstraintType::kNotNull`.
                entries.push(("constraints".into(), Json::Arr(vec![Json::Int(2)])));
            }
            entries.push(("default".into(), default_json(meta.defaults.get(i))));
            column_definition.push(Json::Obj(entries));
        }
        let segments = self.segment_json(db_id, meta.id, &meta.ids)?;
        let mut indexes = Vec::new();
        if let Ok(defs) =
            infinity_catalog::cpp_layout::read_cpp_indexes(self.storage.catalog.store().as_ref())
        {
            for def in defs {
                let Some(index_id) = def.index_id else {
                    continue;
                };
                let owner = def.owner.clone().unwrap_or_default();
                if !owner.1.eq_ignore_ascii_case(&meta.id.to_string()) {
                    continue;
                }
                let index_dir = format!("db_{db_id}/tbl_{}/idx_{index_id}", meta.id);
                // `TableIndexMeta::MapMetaToSnapShotInfo` reads the segment
                // layer from the catalog, not from the files: `chunk_info`
                // carries the row range and the index size, and the segment
                // and chunk ids are the catalog's own. The mirror writes those
                // records, so both engines serialize the same values.
                let chunk_layers = infinity_catalog::cpp_layout::read_cpp_index_chunks(
                    self.storage.catalog.store().as_ref(),
                    &db_id.to_string(),
                    &meta.id.to_string(),
                    &index_id.to_string(),
                )?;
                let fulltext = def.base.index_type.eq_ignore_ascii_case("FULLTEXT");
                let mut segment_indexes = Vec::new();
                for (segment_id, chunks) in chunk_layers {
                    let mut chunk_indexes = Vec::new();
                    for (chunk_id, info) in chunks {
                        let mut entries = vec![
                            ("base_name".into(), Json::Str(info.base_name.clone())),
                            (
                                "base_row_id".into(),
                                Json::Str(info.base_row_id.to_string()),
                            ),
                            ("chunk_id".into(), Json::Int(chunk_id as i64)),
                            // `ChunkIndexMetaInfo::IndexFileName` names the
                            // index file after the chunk id for every type; a
                            // full-text chunk additionally lists its three
                            // files.
                            (
                                "index_filename".into(),
                                Json::Str(format!("chunk_{chunk_id}.idx")),
                            ),
                            ("index_size".into(), Json::Int(info.index_size as i64)),
                            ("row_cnt".into(), Json::Int(info.row_count as i64)),
                        ];
                        if fulltext {
                            let base = format!("{index_dir}/seg_{segment_id}/{}", info.base_name);
                            entries.push((
                                "full_text_files".into(),
                                Json::Arr(vec![
                                    Json::Str(format!("{base}.pos")),
                                    Json::Str(format!("{base}.dic")),
                                    Json::Str(format!("{base}.len")),
                                ]),
                            ));
                        }
                        chunk_indexes.push(Json::Obj(entries));
                    }
                    // `SegmentIndexSnapshotInfo::Serialize` only emits the key
                    // when the segment has chunks.
                    let mut segment = Vec::new();
                    if !chunk_indexes.is_empty() {
                        segment.push(("chunk_indexes".into(), Json::Arr(chunk_indexes)));
                    }
                    segment.push(("segment_id".into(), Json::Int(segment_id as i64)));
                    segment_indexes.push(Json::Obj(segment));
                }
                // `TableIndexSnapshotInfo` stores `index_base_->Serialize()`,
                // i.e. the base fields **plus** the subtype's own
                // (`cardinality`/`is_json_index` for a secondary index), which
                // is the same record the catalog holds under
                // `idx|…|index_base`.
                let mut index = vec![
                    ("index_base".into(), value_format::index_def_json(&def)),
                    ("index_dir".into(), Json::Str(index_dir)),
                    ("index_id_str".into(), Json::Str(index_id.to_string())),
                ];
                if !segment_indexes.is_empty() {
                    index.push(("segment_indexes".into(), Json::Arr(segment_indexes)));
                }
                indexes.push(Json::Obj(index));
            }
        }
        let mut entries = vec![
            ("column_definition".into(), Json::Arr(column_definition)),
            ("create_ts".into(), Json::Int(0)),
            ("database_name".into(), Json::Str(db_name.to_string())),
            ("db_id_str".into(), Json::Str(db_id.to_string())),
            // The reference leaves this 0 (the assignment in
            // `MapMetaToSnapShotInfo` is commented out) and then prints an
            // uninitialized value in `SHOW SNAPSHOTS`; the port records the
            // snapshot's own ts here, which is what the column means.
            ("max_commit_ts".into(), Json::Int(snapshot_ts as i64)),
            (
                "next_column_id".into(),
                Json::Int(
                    meta.ids
                        .iter()
                        .copied()
                        .max()
                        .map(|v| v as i64 + 1)
                        .unwrap_or(0),
                ),
            ),
            ("next_segment_id".into(), Json::Int(0)),
            ("row_count".into(), Json::Int(0)),
            ("segments".into(), Json::Arr(segments)),
            ("snapshot_name".into(), Json::Str(snapshot_name.to_string())),
            ("snapshot_scope".into(), Json::Int(SCOPE_TABLE)),
            ("table_comment".into(), Json::Str(String::new())),
            ("table_id_str".into(), Json::Str(meta.id.to_string())),
            ("table_name".into(), Json::Str(table_name.to_string())),
            ("txn_id".into(), Json::Int(0)),
            ("unsealed_id".into(), Json::Int(0)),
            ("version".into(), Json::Int(1)),
        ];
        // `CreateSnapshotMetadataJSON` appends `table_indexes` inside the loop
        // over the table's index snapshots, so the key is absent on a table
        // without indexes.
        if !indexes.is_empty() {
            entries.push(("table_indexes".into(), Json::Arr(indexes)));
        }
        Ok(Json::Obj(entries))
    }

    /// `SegmentSnapshotInfo::Serialize` per segment of the table: the blocks
    /// the mirror holds, in `BlockIndex` order.
    fn segment_json(&self, db_id: u64, table_id: u64, column_ids: &[u64]) -> Result<Vec<Json>> {
        let index = BlockIndex::open(self.blocks_root(), db_id, table_id)?;
        let mut by_segment: Vec<(u32, Vec<&infinity_storage::block_index::BlockMeta>)> = Vec::new();
        for meta in index.metas() {
            match by_segment.last_mut() {
                Some((segment_id, blocks)) if *segment_id == meta.id.segment_id => {
                    blocks.push(meta)
                }
                _ => by_segment.push((meta.id.segment_id, vec![meta])),
            }
        }
        let mut segments = Vec::new();
        for (segment_id, blocks) in by_segment {
            let mut block_json = Vec::new();
            let mut row_count = 0i64;
            for block in &blocks {
                row_count += block.row_count as i64;
                let dir = format!(
                    "db_{db_id}/tbl_{table_id}/seg_{segment_id}/blk_{}",
                    block.id.block_id
                );
                let mut columns = Vec::new();
                for column_id in column_ids {
                    let mut entries = vec![
                        ("column_id".into(), Json::Int(*column_id as i64)),
                        (
                            "filepath".into(),
                            Json::Str(format!("{dir}/{column_id}.col")),
                        ),
                    ];
                    let outline = format!("{dir}/col_{column_id}_out");
                    if self.blocks_root().join(&outline).exists() {
                        entries.push(("outlines".into(), Json::Arr(vec![Json::Str(outline)])));
                    }
                    columns.push(Json::Obj(entries));
                }
                block_json.push(Json::Obj(vec![
                    ("block_dir".into(), Json::Str(dir)),
                    ("block_id".into(), Json::Int(block.id.block_id as i64)),
                    ("columns".into(), Json::Arr(columns)),
                ]));
            }
            segments.push(Json::Obj(vec![
                ("actual_row_count".into(), Json::Int(0)),
                ("blocks".into(), Json::Arr(block_json)),
                ("deprecate_ts".into(), Json::Int(0)),
                ("first_delete_ts".into(), Json::Uint(NO_DELETE_TS)),
                ("row_count".into(), Json::Int(row_count)),
                ("segment_dir".into(), Json::Str(String::new())),
                ("segment_id".into(), Json::Int(segment_id as i64)),
                ("segment_status".into(), Json::Int(0)),
            ]));
        }
        Ok(segments)
    }

    /// Recreates one table from its snapshot metadata and copies its files back
    /// into the data directory under the new table id, returning the files the
    /// restore wrote (`WalCmdRestoreTableSnapshot::files_`).
    fn restore_table(&self, table: &Json) -> Result<Vec<String>> {
        // Serialize against a create of the same name (and against the orphan
        // sweep), as `NewTxn::RestoreTableSnapshot` does through the catalog.
        let _ddl = self.ddl_lock.lock().unwrap();
        // A database or system snapshot restores its databases too: one of
        // them may have been dropped since the snapshot was taken.
        let db_name = json_str(table, "database_name");
        if !self
            .storage
            .catalog
            .list_databases()?
            .iter()
            .any(|d| d.eq_ignore_ascii_case(&db_name))
        {
            self.storage.catalog.create_database(&db_name, false)?;
        }
        let name = format!("{db_name}.{}", json_str(table, "table_name"));
        if self.storage.catalog.get_table_by_name(&name)?.is_some() {
            return Err(Error::Invalid(format!("table '{name}' already exists")));
        }
        let (columns, defaults, not_nulls, types) = schema_of(table)?;
        let meta = self.storage.catalog.create_table_typed(
            &name,
            &columns,
            &defaults,
            &not_nulls,
            &types,
            &vec![None; columns.len()],
        )?;
        let db_id = self.database_id_of(&name);
        let out = self.copy_files_back(table, db_id, meta.id, &column_id_map(table))?;
        // The copied block files carry the rows, but the port reads a table
        // through its row journal, so decode them back into it. The reference
        // serves the restored table straight from the files it copied.
        // The rows ride in the copied block columns; the journal is rebuilt
        // from them (the same path a restart takes, `restore_cpp_blocks`).
        self.restore_cpp_blocks(&name)?;
        // The index declarations come back too; the chunks are rebuilt, which
        // is what the reference does when a snapshot's index files are missing.
        for index in json_arr(table, "table_indexes") {
            let Some(base) = json_get(&index, "index_base") else {
                continue;
            };
            let index_name = json_str(base, "index_name");
            let Some(Json::Str(column)) = json_arr(base, "column_names").into_iter().next() else {
                continue;
            };
            if column.is_empty() || index_name.is_empty() {
                continue;
            }
            let kind = json_str(base, "index_type");
            if self
                .storage
                .catalog
                .create_index_typed(&index_name, &name, &column, &kind, &[])
                .is_ok()
                && self.table_schema(&name)?.iter().any(|c| c == &column)
            {
                let _ = self.build_index(&name, &column);
            }
        }
        Ok(out)
    }

    /// `SnapshotInfo::RestoreSnapshotFiles` (`snapshot_info_impl.cpp:498`):
    /// copies each file of the snapshot into the data directory, substituting
    /// the ids the restore handed out for the ones the snapshot recorded.
    fn copy_files_back(
        &self,
        snapshot: &Json,
        db_id: u64,
        table_id: u64,
        columns: &[(u64, u64)],
    ) -> Result<Vec<String>> {
        let name = json_str(snapshot, "snapshot_name");
        let root = self.snapshot_dir().join(&name);
        let data = self.blocks_root();
        let mut out = Vec::new();
        for file in snapshot_files(snapshot) {
            let src = root.join(&file);

            if !src.exists() {
                continue;
            }
            let dst_relative = substitute_column_id(
                &substitute_ids(&file, &db_id.to_string(), &table_id.to_string()),
                columns,
            );
            let dst = data.join(&dst_relative);
            if let Some(parent) = dst.parent() {
                std::fs::create_dir_all(parent).map_err(|e| Error::io(parent, e))?;
            }
            // A file the snapshot recorded but the copy could not read is
            // skipped, exactly as the reference skips it.
            if std::fs::copy(&src, &dst).is_err() {
                continue;
            }
            out.push(dst_relative);
        }
        Ok(out)
    }

    // ------------------------------------------------------------------
    // files
    // ------------------------------------------------------------------

    /// `<snapshot_dir>/<name>/`, created empty. The reference refuses a name
    /// that is already there before it does any work
    /// (`PrepareCommitCreateTableSnapshot`).
    fn prepare_snapshot_dir(&self, name: &str) -> Result<PathBuf> {
        let dir = self.snapshot_dir().join(name);
        if dir.exists() {
            return Err(Error::SnapshotAlreadyExists(format!(
                "Snapshot: {name} already exists"
            )));
        }
        std::fs::create_dir_all(&dir).map_err(|e| Error::io(&dir, e))?;
        Ok(dir)
    }

    /// Copies everything the table owns in the data directory
    /// (`db_{db}/tbl_{table}/…`) into the snapshot directory.
    fn copy_table_files(&self, db_id: u64, table_id: u64, dir: &Path) -> Result<()> {
        let src = self
            .blocks_root()
            .join(format!("db_{db_id}/tbl_{table_id}"));
        copy_dir(&src, &dir.join(format!("db_{db_id}/tbl_{table_id}")))
    }

    fn write_snapshot_json(&self, dir: &Path, name: &str, json: &Json) -> Result<()> {
        let path = dir.join(format!("{name}.json"));
        std::fs::write(&path, json.compact()).map_err(|e| Error::io(&path, e))
    }

    fn read_snapshot_json(&self, name: &str) -> Result<Json> {
        let path = self.snapshot_dir().join(name).join(format!("{name}.json"));
        if !path.exists() {
            return Err(Error::SnapshotNotFound(format!(
                "Snapshot: {name} not found"
            )));
        }
        let text = std::fs::read_to_string(&path).map_err(|e| Error::io(&path, e))?;
        infinity_expression::json::parse(&text)
            .ok_or_else(|| Error::Invalid(format!("bad snapshot metadata for '{name}'")))
    }

    /// Logs one snapshot command as a transaction of its own, the way the
    /// reference's `CreateTableSnapshot` / `Restore*` transactions do. The
    /// transaction writes no KV pair, so the commit takes its timestamp the
    /// way a DDL statement does (`commit_at`, not `commit`).
    fn log_snapshot(&self, cmd: WalCmd) -> Result<()> {
        let turn = self.txn.reserve_commit_ts();
        let commit_ts = turn.ts();
        let mut txn = self.txn.begin();
        // A snapshot transaction is typed like every other one: two snapshots
        // of the same name conflict, a snapshot and a cleanup conflict
        // (`new_txn_impl.cpp:4377-4534`).
        txn.set_op(snapshot_op(&cmd));
        txn.set_typed_commands(vec![cmd]);
        self.txn.commit_at(txn, commit_ts)?;
        Ok(())
    }

    /// The ts the reference stamps into `max_commit_ts_`: the transaction's
    /// **begin** ts, which is the clock's next position.
    fn begin_ts(&self) -> u64 {
        self.txn.current_ts() + 1
    }
}

/// The conflict-matrix type of a snapshot command (`CreateTableSnapshotTxnStore`
/// and friends in the reference).
fn snapshot_op(cmd: &WalCmd) -> infinity_txn::TxnOp {
    let (name, table) = match cmd {
        WalCmd::CreateTableSnapshot {
            db_name,
            table_name,
            snapshot_name,
            ..
        }
        | WalCmd::RestoreTableSnapshot {
            db_name,
            table_name,
            snapshot_name,
            ..
        } => (
            snapshot_name.clone(),
            Some(format!("{db_name}.{table_name}")),
        ),
        WalCmd::CreateDbSnapshot { snapshot_name, .. }
        | WalCmd::CreateSystemSnapshot { snapshot_name, .. }
        | WalCmd::RestoreDatabaseSnapshot { snapshot_name, .. }
        | WalCmd::RestoreSystemSnapshot { snapshot_name, .. } => (snapshot_name.clone(), None),
        _ => return infinity_txn::TxnOp::Other,
    };
    infinity_txn::TxnOp::Snapshot { name, table }
}

/// The `WalCmdRestoreTableSnapshot` payload's `TableDef`, built from the
/// snapshot's `column_definition`.
fn table_def_of(table: &Json) -> TableDef {
    let mut columns = Vec::new();
    for column in json_arr(table, "column_definition") {
        let default = json_get(&column, "default");
        let literal = default
            .as_ref()
            .and_then(|d| json_get(d, "type"))
            .map(json_int);
        let value = default.and_then(|d| json_get(d, "value").cloned());
        let default_expr = match literal {
            Some(0) => ConstantExpr::Boolean(matches!(value, Some(Json::Bool(true)))),
            Some(1) => ConstantExpr::Double(json_int(&value.unwrap_or(Json::Int(0))) as f64),
            Some(2) => ConstantExpr::String(match value {
                Some(Json::Str(s)) => s,
                _ => String::new(),
            }),
            Some(3) => ConstantExpr::Integer(json_int(&value.unwrap_or(Json::Int(0)))),
            _ => ConstantExpr::Null,
        };
        let not_null = json_arr(&column, "constraints")
            .iter()
            .any(|c| json_int(c) == 2);
        columns.push(ColumnDef {
            id: json_int_of(&column, "column_id"),
            column_type: crate::wal_ddl::data_type_from_json(json_get(&column, "column_type")),
            name: json_str(&column, "column_name"),
            constraints: if not_null {
                vec![infinity_storage::wal::ddl_def::ConstraintType::NotNull]
            } else {
                Vec::new()
            },
            comment: json_str(&column, "column_comment"),
            default_expr,
            build_bloom_filter: false,
        });
    }
    TableDef {
        schema_name: json_str(table, "database_name"),
        table_name: json_str(table, "table_name"),
        table_comment: String::new(),
        columns,
    }
}

/// `"db.tbl"` -> `("db", "tbl")`.
fn split_qualified(name: &str) -> (String, String) {
    match name.rsplit_once('.') {
        Some((db, table)) => (db.to_string(), table.to_string()),
        None => (String::new(), name.to_string()),
    }
}

/// The reference's path rewrite: the `db_*` / `tbl_*` ids of the snapshot are
/// replaced by the ones the restore handed out (`snapshot_info_impl.cpp:511`).
/// `(snapshot column id, restored column id)` for every column of a table
/// snapshot. `create_table_typed` numbers the restored table's columns from
/// zero, while a `DROP COLUMN` leaves the surviving columns with their
/// original ids (`1.col`, `2.col`), so the files have to follow the new ids.
fn column_id_map(table: &Json) -> Vec<(u64, u64)> {
    json_arr(table, "column_definition")
        .iter()
        .enumerate()
        .map(|(i, column)| {
            let old = json_get(column, "column_id")
                .map(json_int)
                .unwrap_or(i as i64) as u64;
            (old, i as u64)
        })
        .collect()
}

/// Renames a block column file (`{id}.col`, `col_{id}_out`) to the id the
/// restored table gave that column. Without it the reader looks for `0.col`
/// in a block that holds `1.col`, `2.col`: the decode fails and the restored
/// table reads as empty (Gate A's `test_drop_column`).
fn substitute_column_id(file: &str, columns: &[(u64, u64)]) -> String {
    let Some((dir, name)) = file.rsplit_once('/') else {
        return file.to_string();
    };
    let mapped = |id: u64| columns.iter().find(|(old, _)| *old == id).map(|(_, new)| *new);
    let renamed = if let Some(stem) = name.strip_suffix(".col") {
        stem.parse::<u64>().ok().and_then(mapped).map(|id| format!("{id}.col"))
    } else if let Some(rest) = name
        .strip_prefix("col_")
        .and_then(|r| r.strip_suffix("_out"))
    {
        rest.parse::<u64>()
            .ok()
            .and_then(mapped)
            .map(|id| format!("col_{id}_out"))
    } else {
        None
    };
    match renamed {
        Some(name) => format!("{dir}/{name}"),
        None => file.to_string(),
    }
}

fn substitute_ids(file: &str, db_id: &str, table_id: &str) -> String {
    let mut out = file.to_string();
    if let Some(pos) = out.find("db_") {
        let end = out[pos + 3..]
            .find('/')
            .map(|i| i + pos + 3)
            .unwrap_or(out.len());
        out.replace_range(pos + 3..end, db_id);
        if let Some(tbl) = out[pos..].find("/tbl_") {
            let base = pos + tbl;
            let end = out[base + 5..]
                .find('/')
                .map(|i| i + base + 5)
                .unwrap_or(out.len());
            out.replace_range(base + 5..end, table_id);
        }
    }
    out
}

/// Every file path the snapshot's metadata names.
fn snapshot_files(json: &Json) -> Vec<String> {
    let mut out = Vec::new();
    for segment in json_arr(json, "segments") {
        for block in json_arr(&segment, "blocks") {
            for column in json_arr(&block, "columns") {
                let path = json_str(&column, "filepath");
                if !path.is_empty() {
                    out.push(path);
                }
                for outline in json_arr(&column, "outlines") {
                    if let Json::Str(s) = outline {
                        out.push(s);
                    }
                }
            }
            // `TableSnapshotInfo::GetFiles` adds every block's `version` file
            // (`snapshot_info_impl.cpp:296`); the JSON names it only through
            // `block_dir`.
            let block_dir = json_str(&block, "block_dir");
            if !block_dir.is_empty() {
                out.push(format!("{block_dir}/version"));
            }
        }
    }
    for index in json_arr(json, "table_indexes") {
        for segment_index in json_arr(&index, "segment_indexes") {
            for chunk in json_arr(&segment_index, "chunk_indexes") {
                // `TableSnapshotInfo::GetIndexFiles`: a full-text chunk lists
                // its three files itself, every other type holds one file named
                // after the chunk id under `idx_{id}/seg_{s}/`.
                let full_text = json_arr(&chunk, "full_text_files");
                if !full_text.is_empty() {
                    for file in full_text {
                        if let Json::Str(s) = file {
                            out.push(s);
                        }
                    }
                    continue;
                }
                let name = json_str(&chunk, "index_filename");
                if name.is_empty() {
                    continue;
                }
                let dir = json_str(&index, "index_dir");
                let segment_id = json_int_of(&segment_index, "segment_id") as u32;
                out.push(format!("{dir}/seg_{segment_id}/{name}"));
            }
        }
    }
    out
}

/// The `column_definition` of a snapshot table as the catalog wants it.
fn schema_of(table: &Json) -> Result<(Vec<String>, Vec<Option<String>>, Vec<bool>, Vec<String>)> {
    let mut columns = Vec::new();
    let mut defaults = Vec::new();
    let mut not_nulls = Vec::new();
    let mut types = Vec::new();
    for column in json_arr(table, "column_definition") {
        columns.push(json_str(&column, "column_name"));
        defaults.push(default_text(&column));
        not_nulls.push(
            json_arr(&column, "constraints")
                .iter()
                .any(|c| json_int(c) == 2),
        );
        types.push(type_text(&column)?);
    }
    Ok((columns, defaults, not_nulls, types))
}

/// The declared type text of a snapshot column (`INT`, `VARCHAR`, …).
fn type_text(column: &Json) -> Result<String> {
    let Some(kind) = json_get(column, "column_type").and_then(|t| json_get(t, "data_type")) else {
        return Err(Error::Invalid("snapshot column has no type".to_string()));
    };
    let code = json_int(kind);
    let info = json_get(column, "column_type").and_then(|t| json_get(t, "type_info"));
    let text = match code {
        0 => "BOOLEAN",
        1 => "TINYINT",
        2 => "SMALLINT",
        3 => "INT",
        4 => "BIGINT",
        5 => "HUGEINT",
        7 => "FLOAT",
        8 => "DOUBLE",
        9 => "VARCHAR",
        10 => "DATE",
        11 => "TIME",
        12 => "DATETIME",
        13 => "TIMESTAMP",
        22 => "UUID",
        32 => "FLOAT16",
        33 => "BFLOAT16",
        35 => "JSON",
        6 => {
            let p = info
                .as_ref()
                .and_then(|i| json_get(i, "precision"))
                .map(json_int);
            let s = info
                .as_ref()
                .and_then(|i| json_get(i, "scale"))
                .map(json_int);
            return Ok(format!("DECIMAL({},{})", p.unwrap_or(18), s.unwrap_or(3)));
        }
        15 => {
            let inner = info
                .as_ref()
                .and_then(|i| json_get(i, "element_type"))
                .map(json_int);
            return Ok(format!("ARRAY({})", scalar_text(inner.unwrap_or(3))));
        }
        23 | 28 | 29 | 34 => {
            let kind = match code {
                23 => "EMBEDDING",
                28 => "TENSOR",
                29 => "TENSORARRAY",
                _ => "MULTIVECTOR",
            };
            let elem = info
                .as_ref()
                .and_then(|i| json_get(i, "embedding_type"))
                .map(json_int)
                .unwrap_or(5);
            let dim = info
                .as_ref()
                .and_then(|i| json_get(i, "dimension"))
                .map(json_int)
                .unwrap_or(0);
            return Ok(format!("{kind}({},{dim})", element_text(elem)));
        }
        30 => {
            let elem = info
                .as_ref()
                .and_then(|i| json_get(i, "embedding_type"))
                .map(json_int)
                .unwrap_or(5);
            let dim = info
                .as_ref()
                .and_then(|i| json_get(i, "dimension"))
                .map(json_int)
                .unwrap_or(0);
            return Ok(format!("SPARSE({},{dim})", element_text(elem)));
        }
        other => {
            return Err(Error::Invalid(format!(
                "snapshot column type {other} is not supported"
            )))
        }
    };
    Ok(text.to_string())
}

fn scalar_text(code: i64) -> &'static str {
    match code {
        0 => "BOOLEAN",
        1 => "TINYINT",
        2 => "SMALLINT",
        3 => "INT",
        4 => "BIGINT",
        7 => "FLOAT",
        8 => "DOUBLE",
        9 => "VARCHAR",
        _ => "INT",
    }
}

fn element_text(code: i64) -> &'static str {
    match code {
        0 => "BIT",
        1 => "INT8",
        2 => "INT16",
        3 => "INT32",
        4 => "INT64",
        5 => "FLOAT",
        6 => "DOUBLE",
        7 => "UINT8",
        8 => "FLOAT16",
        9 => "BFLOAT16",
        _ => "FLOAT",
    }
}

/// `ConstantExpr::Serialize` for a column default
/// (`src/parser/expr/constant_expr.cpp:452`): a column without a default is
/// `kNull`.
fn default_json(default: Option<&Option<String>>) -> Json {
    let Some(text) = default.and_then(|d| d.as_deref()) else {
        return Json::Obj(vec![("type".into(), Json::Int(4))]);
    };
    let text = text.trim().trim_matches('\'');
    if let Ok(v) = text.parse::<i64>() {
        return Json::Obj(vec![
            ("type".into(), Json::Int(3)),
            ("value".into(), Json::Int(v)),
        ]);
    }
    if let Ok(v) = text.parse::<f64>() {
        return Json::Obj(vec![
            ("type".into(), Json::Int(1)),
            ("value".into(), Json::Float(v)),
        ]);
    }
    if text.eq_ignore_ascii_case("true") || text.eq_ignore_ascii_case("false") {
        return Json::Obj(vec![
            ("type".into(), Json::Int(0)),
            (
                "value".into(),
                Json::Bool(text.eq_ignore_ascii_case("true")),
            ),
        ]);
    }
    Json::Obj(vec![
        ("type".into(), Json::Int(2)),
        ("value".into(), Json::Str(text.to_string())),
    ])
}

/// The inverse of [`default_json`]: the default's `.slt` text.
fn default_text(column: &Json) -> Option<String> {
    let default = json_get(column, "default")?;
    match json_get(default, "value") {
        Some(Json::Int(v)) => Some(v.to_string()),
        Some(Json::Float(v)) => Some(format!("{v}")),
        Some(Json::Str(v)) => Some(format!("'{v}'")),
        Some(Json::Bool(v)) => Some(v.to_string()),
        _ => None,
    }
}

fn json_get<'a>(json: &'a Json, key: &str) -> Option<&'a Json> {
    match json {
        Json::Obj(entries) => entries.iter().find(|(k, _)| k == key).map(|(_, v)| v),
        _ => None,
    }
}

fn json_str(json: &Json, key: &str) -> String {
    match json_get(json, key) {
        Some(Json::Str(s)) => s.clone(),
        Some(Json::Int(v)) => v.to_string(),
        _ => String::new(),
    }
}

fn json_int(json: &Json) -> i64 {
    match json {
        Json::Int(v) => *v,
        Json::Float(v) => *v as i64,
        Json::Str(s) => s.parse().unwrap_or(0),
        _ => 0,
    }
}

fn json_int_of(json: &Json, key: &str) -> i64 {
    json_get(json, key).map(json_int).unwrap_or(0)
}

fn json_arr(json: &Json, key: &str) -> Vec<Json> {
    match json_get(json, key) {
        Some(Json::Arr(items)) => items.clone(),
        _ => Vec::new(),
    }
}

/// The table record of a snapshot: a table-scoped snapshot is the record
/// itself; a database/system snapshot carries it under `table_snapshots`.
fn json_table<'a>(json: &'a Json, name: &str) -> Result<&'a Json> {
    if json_int_of(json, "snapshot_scope") == SCOPE_TABLE {
        return Ok(json);
    }
    let tables = json_arr(json, "table_snapshots");
    match tables.len() {
        1 => Ok(json),
        _ => Err(Error::Invalid(format!(
            "snapshot '{name}' does not hold exactly one table"
        ))),
    }
}

/// Recursively copies the directory `src` into `dst`.
fn copy_dir(src: &Path, dst: &Path) -> Result<()> {
    std::fs::create_dir_all(dst).map_err(|e| Error::io(dst, e))?;
    let Ok(entries) = std::fs::read_dir(src) else {
        return Ok(());
    };
    for entry in entries.flatten() {
        let from = entry.path();
        let to = dst.join(entry.file_name());
        let Ok(meta) = entry.metadata() else { continue };
        if meta.is_dir() {
            copy_dir(&from, &to)?;
        } else {
            std::fs::copy(&from, &to).map_err(|e| Error::io(&to, e))?;
        }
    }
    Ok(())
}

/// The ts a snapshot was taken at: every scope writes it as `max_commit_ts`
/// (`TableSnapshotInfo`/`DatabaseSnapshotInfo`/`SystemSnapshotInfo`).
fn snapshot_commit_ts(json: &Json) -> u64 {
    json_int_of(json, "max_commit_ts").max(0) as u64
}

/// `%Y-%m-%d %H:%M:%S` in local time (C++ `std::strftime` with
/// `std::localtime`).
fn format_time(secs: i64) -> String {
    // Days from the Unix epoch, civil-from-days (Howard Hinnant's algorithm).
    let (days, secs_of_day) = (secs.div_euclid(86400), secs.rem_euclid(86400));
    let z = days + 719468;
    let era = if z >= 0 { z } else { z - 146096 } / 146097;
    let doe = z - era * 146097;
    let yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    let y = yoe + era * 400;
    let doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    let mp = (5 * doy + 2) / 153;
    let d = doy - (153 * mp + 2) / 5 + 1;
    let m = if mp < 10 { mp + 3 } else { mp - 9 };
    let y = if m <= 2 { y + 1 } else { y };
    format!(
        "{:04}-{:02}-{:02} {:02}:{:02}:{:02}",
        y,
        m,
        d,
        secs_of_day / 3600,
        secs_of_day / 60 % 60,
        secs_of_day % 60
    )
}

/// `VirtualStore::GetDirectorySize`: every file below `dir`, recursively.
fn dir_size(dir: &Path) -> u64 {
    let mut total = 0u64;
    let Ok(entries) = std::fs::read_dir(dir) else {
        return 0;
    };
    for entry in entries.flatten() {
        match entry.metadata() {
            Ok(m) if m.is_dir() => total += dir_size(&entry.path()),
            Ok(m) => total += m.len(),
            Err(_) => {}
        }
    }
    total
}
