use super::*;

impl Engine {
    /// `CREATE TABLE` deletes the row keys a crashed `DROP TABLE` of the same
    /// name left behind, but only when no table by that name exists: a
    /// duplicate name must fail in the catalog, never lose its rows here.
    #[allow(clippy::too_many_arguments)]
    pub(crate) fn create_table_clearing_orphans(
        &self,
        name: &str,
        columns: &[String],
        defaults: &[Option<String>],
        not_nulls: &[bool],
        types: &[String],
        comments: &[Option<String>],
        sparse_stores: &[Option<catalog::SparseStore>],
    ) -> Result<catalog::TableMeta> {
        // The DDL lock is held by [`Self::ddl_commit`], which is the only
        // caller: the orphan sweep and the catalog write have to happen inside
        // the same captured batch.
        // C++ `VerifyColumnDataType` (`logical_planner_impl.cpp:551`): the
        // reference refuses these column types at DDL time, so a table holding
        // them can never reach the WAL on either side.
        for type_text in types {
            if let Some(message) = unsupported_column_type(type_text) {
                return Err(Error::Invalid(message));
            }
        }
        if self.storage.catalog.get_table_by_name(name)?.is_none() {
            self.clear_orphan_rows(name)?;
        }
        let meta = self.storage.catalog.create_table_typed_with_sparse_stores(
            name,
            columns,
            defaults,
            not_nulls,
            types,
            comments,
            sparse_stores,
        )?;
        self.register_table_ids(&meta);
        Ok(meta)
    }

    /// Drops a table and purges its stored rows, tombstones, secondary-index
    /// entries and index declarations, so a later CREATE TABLE with the same
    /// name starts empty.
    ///
    /// In the C++ key space the catalog write is only the `drop|tbl|…` marker;
    /// the records and files stay until `Cleanup` reclaims them
    /// ([`Self::reclaim_dropped_entities`]). What is unconditional is the
    /// engine's own state: the row keys are the port's, and the in-memory
    /// registry must forget a dropped table at once.
    pub(crate) fn drop_table_all(&self, name: &str) -> Result<bool> {
        let table_id = self.storage.catalog.get_table_by_name(name)?.map(|m| m.id);
        let key = self.table_key(name);
        // Full-text chunks live in `<dir>/fulltext/<db>.<table>.<column>`, keyed by
        // name rather than by table id: a table recreated under the same name
        // would otherwise reopen the dropped one's chunks and score those
        // documents a second time.
        let fulltext_columns = self.table_schema(&key).unwrap_or_default();
        // Dropping the entry drops its charge with it, so the engine-wide
        // total has to be rebuilt once (a DDL is rare; the scan is not on a
        // hot path).
        self.tables.lock().unwrap().remove(&key);
        crate::blocks::misc::forget_mirror_write_lock(&key);
        self.recompute_memindex_total();
        // A dropped table's blocks are gone: so are the mappings of them.
        if let Some(id) = table_id {
            self.drop_block_maps(id);
        }
        let existed = self.storage.catalog.drop_table(name)?;
        // Rows are the port's own key space (`row:`/`del:`), not the
        // reference's, so nothing else would ever remove them.
        self.clear_orphan_rows(name)?;
        for column in &fulltext_columns {
            let dir = self
                .dir
                .join("fulltext")
                .join(format!("{}.{column}", key.replace('/', "_")));
            if dir.is_dir() {
                let _ = storage::chunk_store::remove_tree(dir);
            }
        }
        if self.storage.catalog.is_catalog_only() {
            return Ok(existed);
        }
        // The native catalog keeps its records in the same store, so a native
        // drop is the physical removal: the reference's cleanup step has
        // nothing to do here.
        // The C++ block mirror of a dropped table is removed with it, so a
        // later restart cannot resurrect the data.
        if let Some(id) = table_id {
            let root = self.blocks_root();
            if let Ok(dbs) = std::fs::read_dir(&root) {
                for db in dbs.flatten() {
                    let dir = db.path().join(format!("tbl_{id}"));
                    if dir.is_dir() {
                        let _ = storage::chunk_store::remove_tree(dir);
                    }
                }
            }
        }
        // The table entry (index registries included) was removed above.
        for im in self.storage.catalog.indexes_of_table(name)? {
            let _ = self.storage.catalog.drop_index_on(&im.table, &im.name);
        }
        Ok(existed)
    }

    /// Runs a DDL as one write transaction, the way the reference does.
    ///
    /// Every statement in the reference gets a transaction
    /// (`QueryContext::BeginTxn`) and the executor commits it once
    /// (`QueryContext::CommitTxn`), so a DDL's catalog records carry its commit
    /// timestamp (`KeyEncode::CatalogDbKey(db_name, commit_ts)`).
    ///
    /// A statement that staged nothing is a **read-only** transaction in the
    /// reference (`NewTxn::Commit` -> `GetReadCommitTS` = `current_ts + 1`): it
    /// writes no WAL record and moves neither clock. `DROP TABLE IF EXISTS` on
    /// a name that does not exist is the common case, and spending a write
    /// timestamp on it would shift every later one.
    ///
    /// The catalog's C++-layout writes are collected instead of applied
    /// ([`Catalog::capture`]), stamped with the reserved commit timestamp
    /// ([`Catalog::restamp`]) and committed together with the WAL entry, so the
    /// key's ts and the entry's ts are the same value.
    /// Builds the reference's `WalCmdDropColumnsV2` from the column's catalog
    /// record (`tbl|col|{db_id}|{table_id}|{name}|{ts}` → column id). Must be
    /// called before the drop removes the record.
    pub(crate) fn drop_columns_command(
        &self,
        table: &str,
        column: &str,
    ) -> Result<Option<storage::WalCmd>> {
        let Some((db_id, table_id, table_key)) = self.table_ident(table)? else {
            return Ok(None);
        };
        let bare = table.rsplit('.').next().unwrap_or(table).to_string();
        let prefix = format!("tbl|col|{db_id}|{table_id}|{column}|");
        if let Some((k, v)) = self
            .storage
            .catalog
            .store()
            .scan_prefix(prefix.as_bytes())?
            .into_iter()
            .next()
        {
            let key = String::from_utf8_lossy(&k).into_owned();
            let id = crate::wal_bridge::ColumnRecord::from_json(&String::from_utf8_lossy(&v))
                .map(|r| r.id() as u32)
                .unwrap_or(0);
            return Ok(Some(storage::WalCmd::DropColumns {
                db_name: "default_db".to_string(),
                db_id,
                table_name: bare,
                table_id,
                column_names: vec![column.to_string()],
                column_ids: vec![id],
                table_key,
                column_keys: vec![key],
            }));
        }
        Ok(None)
    }

    /// Builds the reference's `WalCmdRenameTableV2` from the table's catalog
    /// key (before the rename rewrites it).
    pub(crate) fn rename_table_command(
        &self,
        old: &str,
        new: &str,
    ) -> Result<Option<storage::WalCmd>> {
        let bare_old = old.rsplit('.').next().unwrap_or(old);
        let bare_new = new.rsplit('.').next().unwrap_or(new);
        for (k, v) in self.storage.catalog.store().scan_prefix(b"catalog|tbl|")? {
            let key = String::from_utf8_lossy(&k).into_owned();
            let parts: Vec<&str> = key.split('|').collect();
            if parts.len() >= 5 && parts[3] == bare_old {
                return Ok(Some(storage::WalCmd::RenameTable {
                    db_name: "default_db".to_string(),
                    db_id: parts[2].to_string(),
                    table_name: bare_old.to_string(),
                    table_id: String::from_utf8_lossy(&v).into_owned(),
                    new_table_name: bare_new.to_string(),
                    old_table_key: key,
                }));
            }
        }
        Ok(None)
    }

    /// Builds the reference's `WalCmdDropIndexV2` from the index's catalog
    /// record (`catalog|idx|{db_id}|{table_id}|{index_name}|{ts}` → index id);
    /// the table name is resolved by matching the table id.
    pub(crate) fn drop_index_command(&self, index_name: &str) -> Result<Option<storage::WalCmd>> {
        let mut found: Option<(String, String, u64, String, String)> = None;
        for (k, v) in self.storage.catalog.store().scan_prefix(b"catalog|idx|")? {
            let key = String::from_utf8_lossy(&k).into_owned();
            let parts: Vec<&str> = key.split('|').collect();
            // catalog|idx|{db_id}|{table_id}|{index_name}|{ts}
            if parts.len() >= 6 && parts[4] == index_name {
                found = Some((
                    parts[2].to_string(),
                    parts[3].to_string(),
                    parts[5].parse::<u64>().unwrap_or(0),
                    String::from_utf8_lossy(&v).into_owned(),
                    key,
                ));
                break;
            }
        }
        let Some((db_id, table_id, create_ts, index_id, index_key)) = found else {
            return Ok(None);
        };
        let mut table_name = String::new();
        for (k, v) in self.storage.catalog.store().scan_prefix(b"catalog|tbl|")? {
            let key = String::from_utf8_lossy(&k).into_owned();
            let parts: Vec<&str> = key.split('|').collect();
            if parts.len() >= 5
                && parts[2] == db_id
                && String::from_utf8_lossy(&v).as_ref() == table_id.as_str()
            {
                table_name = parts[3].to_string();
                break;
            }
        }
        Ok(Some(storage::WalCmd::DropIndex {
            db_name: "default_db".to_string(),
            db_id,
            table_name,
            table_id,
            index_name: index_name.to_string(),
            index_id,
            create_ts,
            index_key,
        }))
    }

    /// Builds the reference's `WalCmdDropDatabaseV2` from the database's
    /// catalog key (`catalog|db|{name}|{ts}` → db id). Must be called before
    /// the drop deletes the record.
    pub(crate) fn drop_database_command(&self, db: &str) -> Result<Option<storage::WalCmd>> {
        // Point lookup by name: scanning every database record made a DROP
        // DATABASE O(databases) on a catalog with thousands of them.
        let Some((db_id, create_ts)) =
            catalog::layout::db_record(self.storage.catalog.store().as_ref(), db)?
        else {
            return Ok(None);
        };
        Ok(Some(storage::WalCmd::DropDatabase {
            db_name: db.to_string(),
            db_id: db_id.to_string(),
            create_ts,
        }))
    }

    /// Builds the reference's `WalCmdDropTableV2` for `table` from its catalog
    /// key (`catalog|tbl|{db_id}|{name}|{ts}` → table id), or `None` if the
    /// table has no layout record.
    pub(crate) fn drop_table_command(&self, table: &str) -> Result<Option<storage::WalCmd>> {
        let bare = table.rsplit('.').next().unwrap_or(table);
        let db_name = if table.contains('.') {
            table
                .rsplit_once('.')
                .map(|(d, _)| d.to_string())
                .unwrap_or_else(|| "default_db".into())
        } else {
            "default_db".to_string()
        };
        for (k, v) in self.storage.catalog.store().scan_prefix(b"catalog|tbl|")? {
            let key = String::from_utf8_lossy(&k).into_owned();
            let parts: Vec<&str> = key.split('|').collect();
            if parts.len() >= 5 && parts[3] == bare {
                let db_id = parts[2].to_string();
                let create_ts = parts[4].parse::<u64>().unwrap_or(0);
                let table_id = String::from_utf8_lossy(&v).into_owned();
                return Ok(Some(storage::WalCmd::DropTable {
                    db_name,
                    db_id,
                    table_name: bare.to_string(),
                    table_id,
                    create_ts,
                    table_key: key,
                }));
            }
        }
        Ok(None)
    }

    /// Removes the multi-vector chunks of `table` (or one column) from disk and
    /// from the cache; called when the table or its index is dropped.
    pub(crate) fn drop_multivector_storage(&self, table: &str, column: Option<&str>) {
        let tkey = self.table_key(table);
        // The `Arc`s leave the map under `tables`; their read guards are taken
        // after it is unlocked. A scan holds one of those guards while it waits
        // for `tables` (see `Engine::text_index_for`), so locking an index
        // under `tables` closes a cycle whenever a writer is pending.
        let removed: Vec<Arc<std::sync::RwLock<MultiVectorIndex>>> = {
            let mut tables = self.tables.lock().unwrap();
            let Some(entry) = tables.get_mut(&tkey) else {
                return;
            };
            let columns: Vec<String> = match column {
                Some(c) => vec![c.to_string()],
                None => entry.mv.keys().cloned().collect(),
            };
            columns.iter().filter_map(|c| entry.mv.remove(c)).collect()
        };
        for idx in removed {
            let dir = idx.read().map(|g| g.dir.clone()).ok();
            if let Some(dir) = dir {
                if let Ok(entries) = std::fs::read_dir(&dir) {
                    for entry in entries.flatten() {
                        let p = entry.path();
                        if p.extension().is_some_and(|x| x == "mvec" || x == "emvb") {
                            let _ = storage::chunk_store::remove_file(p);
                        }
                    }
                }
            }
        }
    }

    /// Removes the sparse chunks of `table` (or one column) from disk and cache.
    pub(crate) fn drop_sparse_storage(&self, table: &str, column: Option<&str>) {
        let tkey = self.table_key(table);
        // See `drop_multivector_storage`: the map is unlocked before the index
        // is read.
        let removed: Vec<Arc<std::sync::RwLock<SparseIndex>>> = {
            let mut tables = self.tables.lock().unwrap();
            let Some(entry) = tables.get_mut(&tkey) else {
                return;
            };
            let columns: Vec<String> = match column {
                Some(c) => vec![c.to_string()],
                None => entry.sparse.keys().cloned().collect(),
            };
            columns
                .iter()
                .filter_map(|c| entry.sparse.remove(c))
                .collect()
        };
        for idx in removed {
            if let Ok(dir) = idx.read().map(|g| g.dir.clone()) {
                let _ = storage::chunk_store::remove_tree(dir);
            }
        }
    }

    /// Removes the cached inverted indexes of `table` (or of one column) and
    /// their sealed segment files. Called when the table or its full-text index
    /// is dropped, so segments never outlive the catalog entry.
    pub(crate) fn drop_text_index_storage(&self, table: &str, column: Option<&str>) {
        let tkey = self.table_key(table);
        // See `drop_multivector_storage`: the map is unlocked before the
        // indexes are read.
        let indexes: Vec<SharedTextIndex> = match self.tables.lock().unwrap().get_mut(&tkey) {
            Some(entry) => {
                let columns: Vec<String> = match column {
                    Some(c) => vec![c.to_string()],
                    None => entry.text.keys().cloned().collect(),
                };
                columns
                    .iter()
                    .filter_map(|c| entry.text.remove(c))
                    .collect()
            }
            None => Vec::new(),
        };
        for idx in indexes {
            if let Ok(g) = idx.read() {
                let _ = storage::chunk_store::remove_tree(g.dir.clone());
            }
        }
        self.drop_multivector_storage(table, column);
        self.drop_sparse_storage(table, column);
    }
}
