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
use infinity_expression::json::Json;
use infinity_common::{Error, Result};
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
    /// The reference never sets `SnapshotBrief::commit_ts_` (the assignment is
    /// commented out at `snapshot_brief_impl.cpp:94`), so it prints whatever
    /// the default-constructed member holds. The port prints 0.
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
                commit_ts: 0,
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
        let dir = self.prepare_snapshot_dir(name)?;
        let json = self.write_table_snapshot(&dir, name, db_id, &meta)?;
        self.write_snapshot_json(&dir, name, &json)?;
        let begin_ts = self.begin_ts();
        self.log_snapshot(WalCmd::CreateTableSnapshot {
            db_name,
            table_name,
            snapshot_name: name.to_string(),
            max_commit_ts: begin_ts,
        })
    }

    /// `CREATE SNAPSHOT <name> ON DATABASE <db>`: every table of `db`, under
    /// one `DATABASE`-scoped metadata JSON, logged as `CREATE_DB_SNAPSHOT`.
    pub fn create_database_snapshot(&self, name: &str, db: &str) -> Result<()> {
        let db_id = self.database_id(db);
        let dir = self.prepare_snapshot_dir(name)?;
        let mut table_snapshots = Vec::new();
        for meta in self.storage.catalog.list_tables()? {
            let (meta_db, _) = split_qualified(&meta.name);
            if !meta_db.eq_ignore_ascii_case(db) {
                continue;
            }
            table_snapshots.push(self.write_table_snapshot(&dir, name, db_id, &meta)?);
        }
        let json = Json::Obj(vec![
            ("database_name".into(), Json::Str(db.to_string())),
            ("db_comment".into(), Json::Str(String::new())),
            ("db_id_str".into(), Json::Str(db_id.to_string())),
            (
                "db_next_table_id_str".into(),
                Json::Str(self.next_table_id_str(db)),
            ),
            ("snapshot_name".into(), Json::Str(name.to_string())),
            ("snapshot_scope".into(), Json::Int(SCOPE_DATABASE)),
            ("table_snapshots".into(), Json::Arr(table_snapshots)),
            ("version".into(), Json::Int(1)),
        ]);
        self.write_snapshot_json(&dir, name, &json)?;
        let begin_ts = self.begin_ts();
        self.log_snapshot(WalCmd::CreateDbSnapshot {
            db_name: db.to_string(),
            snapshot_name: name.to_string(),
            max_commit_ts: begin_ts,
        })
    }

    /// `CREATE SNAPSHOT <name> ON SYSTEM`: every table of every database,
    /// logged as `CREATE_SYSTEM_SNAPSHOT`.
    pub fn create_system_snapshot(&self, name: &str) -> Result<()> {
        let databases = self.storage.catalog.list_databases()?;
        let mut db_snapshots = Vec::new();
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
                table_snapshots.push(self.write_table_snapshot(&dir, name, db_id, &meta)?);
            }
            db_snapshots.push(Json::Obj(vec![
                ("database_name".into(), Json::Str(db.clone())),
                ("db_comment".into(), Json::Str(String::new())),
                ("db_id_str".into(), Json::Str(db_id.to_string())),
                (
                    "db_next_table_id_str".into(),
                    Json::Str(self.next_table_id_str(db)),
                ),
                ("snapshot_name".into(), Json::Str(name.to_string())),
                ("snapshot_scope".into(), Json::Int(SCOPE_DATABASE)),
                ("table_snapshots".into(), Json::Arr(table_snapshots)),
                ("version".into(), Json::Int(1)),
            ]));
        }
        let json = Json::Obj(vec![
            ("database_snapshots".into(), Json::Arr(db_snapshots)),
            ("snapshot_name".into(), Json::Str(name.to_string())),
            ("snapshot_scope".into(), Json::Int(SCOPE_SYSTEM)),
            ("version".into(), Json::Int(1)),
        ]);
        self.write_snapshot_json(&dir, name, &json)?;
        let begin_ts = self.begin_ts();
        self.log_snapshot(WalCmd::CreateSystemSnapshot {
            snapshot_name: name.to_string(),
            max_commit_ts: begin_ts,
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
        let files = self.restore_table(name, table)?;
        let db_id: u64 = json_str(&json, "db_id_str").parse().unwrap_or(0);
        let table_id: u64 = json_str(&table, "table_id_str").parse().unwrap_or(0);
        self.log_snapshot(WalCmd::RestoreTableSnapshot {
            db_name: json_str(&json, "database_name"),
            db_id: db_id.to_string(),
            table_name: json_str(&table, "table_name"),
            table_id: table_id.to_string(),
            snapshot_name: name.to_string(),
            table_def: table_def_of(&table),
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
            let files = self.restore_table(name, &table)?;
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
                let files = self.restore_table(name, &table)?;
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

    /// Writes one table's part of a snapshot: its files, its metadata JSON and
    /// its row journal. Returns the metadata so a database/system snapshot can
    /// nest it.
    fn write_table_snapshot(
        &self,
        dir: &Path,
        name: &str,
        db_id: u64,
        meta: &infinity_catalog::TableMeta,
    ) -> Result<Json> {
        // The reference snapshots through a checkpoint
        // (`NewTxn::CheckpointforSnapshot`), so the table's blocks exist by the
        // time the files are copied and `segments` describes them.
        let _ = self.write_cpp_blocks(&meta.name);
        let (db_name, _) = split_qualified(&meta.name);
        let json = self.table_snapshot_json(&db_name, db_id, meta, name)?;
        self.copy_table_files(db_id, meta.id, dir)?;
        self.write_snapshot_rows(dir, db_id, meta.id, &meta.name)?;
        Ok(json)
    }

    /// The row journal, which the reference keeps inside the block files it
    /// copies. The port's rows live in `row:{table}:{seq}` at snapshot time,
    /// because the block mirror is only rewritten at a checkpoint, so they are
    /// written next to the metadata as `<name>/<table>.rows`.
    fn write_snapshot_rows(
        &self,
        dir: &Path,
        db_id: u64,
        table_id: u64,
        table: &str,
    ) -> Result<()> {
        let rows = self.committed_row_text(table)?;
        let path = dir.join(rows_file(db_id, table_id));
        let mut text = String::new();
        for row in &rows {
            text.push_str(row);
            text.push('\n');
        }
        std::fs::write(&path, text).map_err(|e| Error::io(&path, e))
    }

    /// The table's live rows, as the text the row journal stores them in. A
    /// tombstoned row is skipped, so the snapshot holds committed state only.
    fn committed_row_text(&self, table: &str) -> Result<Vec<String>> {
        let key = self.table_key(table);
        let deleted: std::collections::HashSet<Vec<u8>> = self
            .storage
            .store
            .scan_prefix(&crate::del_prefix(&key))?
            .into_iter()
            .map(|(k, _)| k)
            .collect();
        let mut rows: Vec<(usize, String)> = Vec::new();
        let prefix = crate::row_prefix(&key);
        for (k, v) in self.storage.store.scan_prefix(&prefix)? {
            let seq = String::from_utf8_lossy(&k[prefix.len()..])
                .trim_start_matches('0')
                .parse::<usize>()
                .unwrap_or(0);
            if deleted.contains(&crate::del_key(&key, seq)) {
                continue;
            }
            rows.push((seq, String::from_utf8_lossy(&v).to_string()));
        }
        rows.sort_by_key(|(seq, _)| *seq);
        Ok(rows.into_iter().map(|(_, text)| text).collect())
    }

    /// [`write_snapshot_rows`] read back, as the row text of each row.
    fn read_snapshot_rows(&self, name: &str, db_id: &str, table_id: &str) -> Vec<Vec<String>> {
        let file = rows_file(db_id.parse().unwrap_or(0), table_id.parse().unwrap_or(0));
        let path = self.snapshot_dir().join(name).join(file);
        let Ok(text) = std::fs::read_to_string(&path) else {
            return Vec::new();
        };
        text.lines()
            .filter(|l| !l.is_empty())
            .map(|l| l.split('\t').map(str::to_string).collect())
            .collect()
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
        if let Ok(defs) = infinity_catalog::cpp_layout::read_cpp_indexes(
            self.storage.catalog.store().as_ref(),
        ) {
            for def in defs {
                let Some(index_id) = def.index_id else { continue };
                let owner = def.owner.clone().unwrap_or_default();
                if !owner.1.eq_ignore_ascii_case(&meta.id.to_string()) {
                    continue;
                }
                let root = self.blocks_root();
                let chunk_files = self
                    .index_files(&meta.name, &def.base.index_name)
                    .into_iter()
                    .filter_map(|p| file_relative(&root, &p))
                    .collect::<Vec<_>>();
                let mut chunk_indexes = Vec::new();
                for (i, file) in chunk_files.iter().enumerate() {
                    let size = self
                        .blocks_root()
                        .join(file)
                        .metadata()
                        .map(|m| m.len() as i64)
                        .unwrap_or(0);
                    let name = file.rsplit('/').next().unwrap_or(file);
                    chunk_indexes.push(Json::Obj(vec![
                        ("base_name".into(), Json::Str(name.to_string())),
                        ("base_row_id".into(), Json::Str("0".to_string())),
                        ("chunk_id".into(), Json::Int(i as i64)),
                        ("index_filename".into(), Json::Str(name.to_string())),
                        ("index_size".into(), Json::Int(size)),
                        ("row_cnt".into(), Json::Int(0)),
                    ]));
                }
                indexes.push(Json::Obj(vec![
                    ("index_base".into(), value_format::index_base_json(&def.base)),
                    (
                        "index_dir".into(),
                        Json::Str(format!("db_{db_id}/tbl_{}/idx_{index_id}", meta.id)),
                    ),
                    ("index_id_str".into(), Json::Str(index_id.to_string())),
                    (
                        "segment_indexes".into(),
                        Json::Arr(vec![Json::Obj(vec![
                            ("chunk_indexes".into(), Json::Arr(chunk_indexes)),
                            ("segment_id".into(), Json::Int(0)),
                        ])]),
                    ),
                ]));
            }
        }
        Ok(Json::Obj(vec![
            ("column_definition".into(), Json::Arr(column_definition)),
            ("create_ts".into(), Json::Int(0)),
            ("database_name".into(), Json::Str(db_name.to_string())),
            ("db_id_str".into(), Json::Str(db_id.to_string())),
            ("max_commit_ts".into(), Json::Int(0)),
            ("next_column_id".into(), Json::Int(
                meta.ids.iter().copied().max().map(|v| v as i64 + 1).unwrap_or(0),
            )),
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
        ]))
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
                let dir = format!("db_{db_id}/tbl_{table_id}/seg_{segment_id}/blk_{}", block.id.block_id);
                let mut columns = Vec::new();
                for column_id in column_ids {
                    let mut entries = vec![
                        ("column_id".into(), Json::Int(*column_id as i64)),
                        ("filepath".into(), Json::Str(format!("{dir}/{column_id}.col"))),
                    ];
                    let outline = format!("{dir}/col_{column_id}_out");
                    if self.blocks_root().join(&outline).exists() {
                        entries.push((
                            "outlines".into(),
                            Json::Arr(vec![Json::Str(outline)]),
                        ));
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
    fn restore_table(&self, snapshot: &str, table: &Json) -> Result<Vec<String>> {
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
        let out = self.copy_files_back(table, db_id, meta.id)?;
        // The copied block files carry the rows, but the port reads a table
        // through its row journal, so decode them back into it. The reference
        // serves the restored table straight from the files it copied.
        let rows = self.read_snapshot_rows(
            snapshot,
            &json_str(table, "db_id_str"),
            &json_str(table, "table_id_str"),
        );
        for (seq, row) in rows.iter().enumerate() {
            self.storage
                .store
                .put(&crate::row_key(&name, seq), row.join("\t").as_bytes())?;
        }
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
    fn copy_files_back(&self, snapshot: &Json, db_id: u64, table_id: u64) -> Result<Vec<String>> {
        let name = json_str(snapshot, "snapshot_name");
        let root = self.snapshot_dir().join(&name);
        let data = self.blocks_root();
        let mut out = Vec::new();
        for file in snapshot_files(snapshot) {
            let src = root.join(&file);
            if !src.exists() {
                continue;
            }
            let dst_relative = substitute_ids(&file, &db_id.to_string(), &table_id.to_string());
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
        let src = self.blocks_root().join(format!("db_{db_id}/tbl_{table_id}"));
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
        let commit_ts = self.txn.reserve_commit_ts();
        let mut txn = self.txn.begin();
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

/// The `WalCmdRestoreTableSnapshot` payload's `TableDef`, built from the
/// snapshot's `column_definition`.
fn table_def_of(table: &Json) -> TableDef {
    let mut columns = Vec::new();
    for column in json_arr(table, "column_definition") {
        let default = json_get(&column, "default");
        let literal = default.as_ref().and_then(|d| json_get(d, "type")).map(json_int);
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

/// The name of a table's row journal inside a snapshot directory. A database
/// or system snapshot holds one per table, so it cannot be named after the
/// snapshot alone.
fn rows_file(db_id: u64, table_id: u64) -> String {
    format!("db_{db_id}_tbl_{table_id}.rows")
}

/// `"db.tbl"` -> `("db", "tbl")`.
fn split_qualified(name: &str) -> (String, String) {
    match name.rsplit_once('.') {
        Some((db, table)) => (db.to_string(), table.to_string()),
        None => (String::new(), name.to_string()),
    }
}

/// `path` relative to `root`, with `/` separators (the form the snapshot
/// metadata records).
fn file_relative(root: &Path, path: &Path) -> Option<String> {
    let rel = path.strip_prefix(root).ok()?;
    let out: Vec<String> = rel
        .components()
        .map(|c| c.as_os_str().to_string_lossy().to_string())
        .collect();
    Some(out.join("/"))
}

/// The reference's path rewrite: the `db_*` / `tbl_*` ids of the snapshot are
/// replaced by the ones the restore handed out (`snapshot_info_impl.cpp:511`).
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
        }
    }
    for index in json_arr(json, "table_indexes") {
        for segment_index in json_arr(&index, "segment_indexes") {
            for chunk in json_arr(&segment_index, "chunk_indexes") {
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
        not_nulls.push(json_arr(&column, "constraints").iter().any(|c| json_int(c) == 2));
        types.push(type_text(&column)?);
    }
    Ok((columns, defaults, not_nulls, types))
}

/// The declared type text of a snapshot column (`INT`, `VARCHAR`, …).
fn type_text(column: &Json) -> Result<String> {
    let Some(kind) = json_get(column, "column_type").and_then(|t| json_get(t, "data_type")) else {
        return Err(Error::Invalid("snapshot column has no type".to_string()));
    };
    let code = json_int(&kind);
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
            let p = info.as_ref().and_then(|i| json_get(i, "precision")).map(|v| json_int(&v));
            let s = info.as_ref().and_then(|i| json_get(i, "scale")).map(|v| json_int(&v));
            return Ok(format!(
                "DECIMAL({},{})",
                p.unwrap_or(18),
                s.unwrap_or(3)
            ));
        }
        15 => {
            let inner = info
                .as_ref()
                .and_then(|i| json_get(i, "element_type"))
                .map(|v| json_int(&v));
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
                .map(|v| json_int(&v))
                .unwrap_or(5);
            let dim = info
                .as_ref()
                .and_then(|i| json_get(i, "dimension"))
                .map(|v| json_int(&v))
                .unwrap_or(0);
            return Ok(format!("{kind}({},{dim})", element_text(elem)));
        }
        30 => {
            let elem = info
                .as_ref()
                .and_then(|i| json_get(i, "embedding_type"))
                .map(|v| json_int(&v))
                .unwrap_or(5);
            let dim = info
                .as_ref()
                .and_then(|i| json_get(i, "dimension"))
                .map(|v| json_int(&v))
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
    match json_get(&default, "value") {
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

/// `%Y-%m-%d %H:%M:%S` in local time (C++ `std::strftime` with
/// `std::localtime`).
fn format_time(secs: i64) -> String {
    // Days from the Unix epoch, civil-from-days (Howard Hinnant's algorithm).
    let (days, secs_of_day) = (secs.div_euclid(86400), secs.rem_euclid(86400));
    let z = days + 719468;
    let era = if z >= 0 { z } else { z - 146096 } / 146097;
    let doe = (z - era * 146097) as i64;
    let yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    let y = yoe + era * 400;
    let doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    let mp = (5 * doy + 2) / 153;
    let d = (doy - (153 * mp + 2) / 5 + 1) as i64;
    let m = if mp < 10 { mp + 3 } else { mp - 9 } as i64;
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
