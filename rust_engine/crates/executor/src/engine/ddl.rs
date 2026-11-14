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
        // One lookup serves both: whether a crashed `DROP TABLE` left orphans
        // and whether the name is already taken. Inside a capture each
        // `get_table_by_name` re-reads the store (the cache is bypassed by
        // design there), so the second one was a repeated catalog read.
        let existing = self.storage.catalog.get_table_by_name(name)?;
        if existing.is_none() {
            self.clear_orphan_rows(name)?;
        }
        let meta = self.storage.catalog.create_table_typed_existing(
            name,
            existing.as_ref(),
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
        // One lookup for the whole drop: the id and the column list both come
        // from it, and inside a capture every `get_table_by_name` re-resolves
        // the name against the store (the cache is bypassed there, because the
        // statement's own uncommitted DDL has to be visible). The reference's
        // `DropTable` resolves once (`DBMeta::GetTableMeta`) and uses that.
        let meta = self.storage.catalog.get_table_by_name(name)?;
        let table_id = meta.as_ref().map(|m| m.id);
        let key = self.table_key(name);
        // Full-text chunks live in `<dir>/fulltext/<db>.<table>.<column>`, keyed by
        // name rather than by table id: a table recreated under the same name
        // would otherwise reopen the dropped one's chunks and score those
        // documents a second time.
        let meta_ref = meta;
        let fulltext_columns = meta_ref
            .as_ref()
            .map(|m| m.columns.clone())
            .unwrap_or_default();
        // Take this table's charge out of the engine-wide total instead of
        // walking every table: a run of `DROP TABLE`s is otherwise quadratic.
        let released = self
            .tables
            .lock()
            .unwrap()
            .remove(&key)
            .map(|entry| entry.memindex_bytes)
            .unwrap_or(0);
        crate::blocks::misc::forget_mirror_write_lock(&key);
        self.release_memindex_bytes(released);
        // A dropped table's blocks are gone: so are the mappings of them.
        if let Some(id) = table_id {
            self.drop_block_maps(id);
        }
        let existed = self
            .storage
            .catalog
            .drop_table_meta(name, meta_ref.as_ref())?;
        // Rows are the port's own key space (`row:`/`del:`), not the
        // reference's, so nothing else would ever remove them.
        self.clear_orphan_rows(name)?;
        for column in &fulltext_columns {
            let dir = self
                .dir
                .join("fulltext")
                .join(format!("{}.{column}", key.replace('/', "_")));
            if dir.is_dir() {
                storage::chunk_store::remove_tree(dir);
            }
        }
        if self.storage.catalog.is_catalog_only() {
            return Ok(existed);
        }
        // The native catalog keeps its records in the same store, so a native
        // drop is the physical removal: the reference's cleanup step has
        // nothing to do here.
        // The C++ block mirror of a dropped table is removed with it, so a
        // later restart cannot resurrect the data. The directory is named by
        // the ids the catalog recorded (`db_{id}/tbl_{id}`), so it is resolved
        // rather than searched for.
        if let Some(id) = table_id {
            let db_id = self.database_id_of(&key);
            let dir = self.blocks_root().join(format!("db_{db_id}/tbl_{id}"));
            if dir.is_dir() {
                storage::chunk_store::remove_tree(dir);
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
        // Scoped to this database: `catalog|tbl|` for every table in the engine
        // makes a run of `DROP TABLE`s walk the whole catalog once per drop
        // (0.44 ms each against the reference's 0.1). `catalog|db|{name}|` is
        // one record per database.
        let db_id_of_name = self
            .storage
            .catalog
            .store()
            .scan_prefix(storage::catalog_format::catalog_db_prefix(&db_name).as_bytes())?
            .into_iter()
            .next()
            .map(|(_, v)| String::from_utf8_lossy(&v).into_owned());
        let scan_prefix = match &db_id_of_name {
            Some(id) => format!("catalog|tbl|{id}|{bare}|"),
            None => "catalog|tbl|".to_string(),
        };
        for (k, v) in self
            .storage
            .catalog
            .store()
            .scan_prefix(scan_prefix.as_bytes())?
        {
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

    /// Drops the in-memory state of `table`'s indexes, as far as `scope`
    /// reaches.
    ///
    /// One entry for every statement that removes an index - `DROP INDEX`
    /// (`DropScope::Record`), `DROP COLUMN` (`DropScope::Column`) and
    /// `DROP TABLE` (`DropScope::Table`) - so a new registry only has to be
    /// added in one place.
    ///
    /// There is deliberately **no** index-type parameter: the registries are
    /// keyed by index id, an id is unique within the table, and each registry
    /// holds only its own ids. Removing the same id from all of them therefore
    /// hits at most the one record that owns it, while a type-dispatched version
    /// has to spell every kind correctly (`SECONDARY` vs `SECONDARYFUNCTIONAL`,
    /// `EMVB` vs `MULTIVECTOR`) and silently misses the entry when it does not -
    /// which is how a dropped HNSW graph used to stay registered.
    ///
    /// A column scope still resolves through each registry's own kind, because
    /// there the ids come *from* the catalog and a plain column sweep would hand
    /// one registry another kind's ids.
    pub(crate) fn drop_index_storage(&self, table: &str, scope: DropScope<'_>) {
        let tkey = self.table_key(table);
        let secondary = scope.ids(|c| self.secondary_index_ids_on_column(table, c));
        let sparse = scope.ids(|c| self.index_ids_on_column_of_kind(table, c, &["BMP"]));
        let text = scope.ids(|c| self.index_ids_on_column_of_kind(table, c, &["FULLTEXT"]));
        let multi = scope.ids(|c| {
            self.index_ids_on_column_of_kind(table, c, &["PLAID", "SMVE", "EMVB", "MULTIVECTOR"])
        });
        // The ids a registry should lose: the scope's list, or - for a table
        // scope - everything that registry holds right now.
        fn pick(wanted: &Option<Vec<u64>>, held: Vec<u64>) -> Vec<u64> {
            match wanted {
                Some(ids) => ids.clone(),
                None => held,
            }
        }

        // The `Arc`s leave the maps under `tables`; their read guards are taken
        // after it is unlocked. A scan holds one of those guards while it waits
        // for `tables` (see `Engine::text_index_for`), so locking an index under
        // `tables` closes a cycle whenever a writer is pending.
        let (texts, sparse_indexes, multi_indexes) = {
            let mut tables = self.tables.lock().unwrap();
            let Some(entry) = tables.get_mut(&tkey) else {
                return;
            };
            let secondary_ids = pick(
                &secondary,
                entry.secondary.indexes.keys().copied().collect(),
            );
            for id in &secondary_ids {
                entry.secondary.indexes.remove(id);
                entry.secondary.disk.remove(id);
                entry.secondary.low.remove(id);
                entry.secondary.sealing.remove(id);
                entry.secondary.chunk_meta.remove(id);
                entry.secondary.disk_backed.remove(id);
                entry.secondary.declared_types.remove(id);
            }
            let sparse_ids = pick(&sparse, entry.sparse.keys().copied().collect());
            let sparse_indexes: Vec<Arc<std::sync::RwLock<SparseIndex>>> = sparse_ids
                .iter()
                .filter_map(|id| entry.sparse.remove(id))
                .collect();
            let text_ids = pick(&text, entry.text.keys().copied().collect());
            // A build that is running right now may target an index being
            // dropped; the bump keeps it from publishing after the fact.
            if !text_ids.is_empty() {
                entry.text_epoch = entry.text_epoch.wrapping_add(1);
            }
            for id in &text_ids {
                entry.text_built.remove(id);
            }
            let texts: Vec<SharedTextIndex> = text_ids
                .iter()
                .filter_map(|id| entry.text.remove(id))
                .collect();
            let multi_ids = pick(&multi, entry.mv.keys().copied().collect());
            let multi_indexes: Vec<Arc<std::sync::RwLock<MultiVectorIndex>>> = multi_ids
                .iter()
                .filter_map(|id| entry.mv.remove(id))
                .collect();
            // The remaining maps hold no handle to close and no lock to take,
            // so the same id can be removed from all of them: an index id is
            // unique within the table, which is what makes the type-free call
            // correct. HNSW's two are included here - they are not secondary
            // entries, so keying them off `secondary_ids` left a dropped HNSW
            // graph registered.
            let mut any_hnsw = false;
            for id in secondary_ids
                .iter()
                .chain(&sparse_ids)
                .chain(&text_ids)
                .chain(&multi_ids)
            {
                any_hnsw |= entry.hnsw_mem.remove(id).is_some();
                entry.index_pending_bytes.remove(id);
                entry.vector.remove(id);
                entry.ivf.remove(id);
                entry.ivf_mem.remove(id);
            }
            // The probe is a one-shot "have I looked for an HNSW index yet"
            // flag; a graph just went away, so the next write probes again.
            if any_hnsw {
                entry.hnsw_topup = true;
            }
            (texts, sparse_indexes, multi_indexes)
        };

        for index in texts {
            if let Ok(g) = index.read() {
                storage::chunk_store::remove_tree(g.dir.clone());
            }
        }
        for index in sparse_indexes {
            if let Ok(dir) = index.read().map(|g| g.dir.clone()) {
                storage::chunk_store::remove_tree(dir);
            }
        }
        for index in multi_indexes {
            let dir = index.read().map(|g| g.dir.clone()).ok();
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
        self.recompute_memindex_total();
    }

    /// Every catalog index id built on `column` of `table`, of **any** kind.
    ///
    /// The in-memory registries are keyed by index id (`MemIndex` identity in
    /// the reference), so a caller that names a column has to resolve it to ids
    /// first - but an index id is unique within the **table**, not within one
    /// index type, so a column carrying an `HNSW` and a `BMP` index yields both
    /// ids here. A caller that owns one kind's registry must use
    /// [`Self::index_ids_on_column_of_kind`] instead.
    pub(crate) fn index_ids_on_column(&self, table: &str, column: &str) -> Vec<u64> {
        self.storage
            .catalog
            .indexes_of_table(&self.qualified_table(table))
            .unwrap_or_default()
            .into_iter()
            .filter(|im| im.column == column)
            .map(|im| im.id)
            .filter(|id| *id > 0)
            .collect()
    }

    /// Every index id of one `kind` on `column`.
    ///
    /// `kind` is matched case-insensitively against the record's type, or (for
    /// "secondary" callers) accepted through `kinds` - see
    /// [`Self::secondary_index_ids_on_column`] for the secondary family, which
    /// spans several spellings (`SECONDARY`, `SECONDARYFUNCTIONAL`).
    pub(crate) fn index_ids_on_column_of_kind(
        &self,
        table: &str,
        column: &str,
        kinds: &[&str],
    ) -> Vec<u64> {
        self.storage
            .catalog
            .indexes_of_table(&self.qualified_table(table))
            .unwrap_or_default()
            .into_iter()
            .filter(|im| {
                im.column == column && kinds.iter().any(|k| im.index_type.eq_ignore_ascii_case(k))
            })
            .map(|im| im.id)
            .filter(|id| *id > 0)
            .collect()
    }

    /// The secondary index ids on `column` (both the plain and the functional
    /// kind, which is the family `is_secondary_kind` names).
    pub(crate) fn secondary_index_ids_on_column(&self, table: &str, column: &str) -> Vec<u64> {
        self.index_ids_on_column_of_kind(table, column, &["SECONDARY", "SECONDARYFUNCTIONAL"])
    }
}
