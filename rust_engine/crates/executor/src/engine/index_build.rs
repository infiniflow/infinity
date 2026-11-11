use super::*;
use catalog::IndexMeta;

impl Engine {
    /// Rebuilds all declared indexes from stored rows (called on open).
    pub(crate) fn rebuild_indexes(&self) -> Result<()> {
        // A new index may have appeared since the last write.
        for e in self.tables.lock().unwrap().values_mut() {
            e.hnsw_probed = false;
            // Sealed chunk readers are reloaded from the mirror below.
            e.vector.clear();
            e.ivf.clear();
            e.ivf_mem.clear();
        }
        for im in self.storage.catalog.list_indexes()? {
            if is_secondary_kind(&im.index_type) {
                // A functional index's column is its expression: restore how to
                // evaluate it from the record (`FUNC`/`FUNC_COLUMN`/`FUNC_RETURN`,
                // the reference's `func_col_params` and `func_return_type`), then
                // rebuild it from its base column.
                if let Some(function) = self.functional_from_meta(&im) {
                    self.register_functional_index(
                        &im.table,
                        &im.column,
                        &function.base,
                        &function.func,
                        &function.returns,
                    );
                    self.build_index(&im.table, &im.column)?;
                } else if self
                    .table_schema(&im.table)?
                    .iter()
                    .any(|c| c == &im.column)
                {
                    self.build_index(&im.table, &im.column)?;
                }
            }
            // A vector index's persisted HNSW graph, loaded so the search path
            // can walk it (best-effort: a table without a mirror just brute
            // forces).
            if im.index_type.eq_ignore_ascii_case("HNSW") {
                let segments = self.load_vector_indexes(&im.table, &im.name);
                if !segments.is_empty() {
                    let tkey = self.table_key(&im.table);
                    self.tables
                        .lock()
                        .unwrap()
                        .entry(tkey)
                        .or_default()
                        .vector
                        .insert(
                            im.column.clone(),
                            segments.into_iter().map(Arc::new).collect(),
                        );
                }
            }
            if im.index_type.eq_ignore_ascii_case("IVF") {
                let segments = self.load_ivf_indexes(&im.table, &im.name);
                let tkey = self.table_key(&im.table);
                let mut tables = self.tables.lock().unwrap();
                let entry = tables.entry(tkey).or_default();
                entry.ivf_mem.entry(im.column.clone()).or_default();
                if !segments.is_empty() {
                    entry.ivf.insert(
                        im.column.clone(),
                        segments.into_iter().map(Arc::new).collect(),
                    );
                }
            }
        }
        Ok(())
    }

    /// `NewTxn::RecoverMemIndex`: rebuilds an index's memory tail from the rows
    /// its sealed chunks do not cover.
    ///
    /// A checkpoint only flushes blocks (`NewTxn::CheckpointTable`); a memory
    /// index keeps its rows until a dump transaction seals them
    /// (`DumpMemIndex`). After a restart the reference walks every segment's
    /// blocks against the chunk coverage (`CountMemIndexGapInSegment`) and
    /// appends only the rows no chunk covers (`AppendIndex`), so the tail comes
    /// back without re-indexing the table.
    ///
    /// The multi-vector and sparse indexes have no chunk mapping of their own:
    /// `multivector_index_for` / `sparse_index_for` rebuild them from the rows
    /// on first use, which closes their gap there. A secondary index is rebuilt
    /// from every row by `rebuild_indexes`, which already covers the gap.
    pub(crate) fn recover_mem_index(&self) -> Result<()> {
        let indexes = self.storage.catalog.list_indexes()?;
        if indexes.is_empty() {
            return Ok(());
        }
        // The rows of a table are shared by every index on it.
        let mut rows: HashMap<String, Vec<(usize, Vec<Value>)>> = HashMap::new();
        for meta in indexes {
            let kind = meta.index_type.trim();
            // A secondary index is rebuilt from the rows by `rebuild_indexes`
            // (`build_index`), which rewrites its chunk files; adding the
            // chunk-gap rows to the tail here as well would count them twice.
            let handled = !is_secondary_kind(kind)
                && (kind.eq_ignore_ascii_case("HNSW")
                    || kind.eq_ignore_ascii_case("IVF")
                    || kind.eq_ignore_ascii_case("FULLTEXT"));
            if !handled {
                continue;
            }
            let table = self.qualified_table(&meta.table);
            let Some(table_meta) = self.storage.catalog.get_table_by_name(&table)? else {
                continue;
            };
            let Ok(schema) = self.table_schema(&table) else {
                continue;
            };
            let Some(ci) = schema.iter().position(|c| c == &meta.column) else {
                continue;
            };
            let chunks = catalog::layout::read_index_chunks(
                self.storage.catalog.store().as_ref(),
                &self.database_id_of(&table).to_string(),
                &table_meta.id.to_string(),
                &meta.id.to_string(),
            )?;
            let covered = merge_covered_ranges(&chunks);
            let covered_rows: u64 = covered.iter().map(|(begin, end)| end - begin).sum();
            // Every row is sealed: nothing for this index to recover.
            let total_rows: u64 = self
                .segment_row_counts(&table)
                .iter()
                .map(|(_, rows)| *rows)
                .sum();
            if covered_rows >= total_rows {
                continue;
            }
            if !rows.contains_key(&table) {
                let fetched = self.fetch_rows_including_deleted_from(&table, 0)?;
                rows.insert(table.clone(), fetched);
            }
            let table_rows = rows.get(&table).unwrap();
            let gap: Vec<&(usize, Vec<Value>)> = table_rows
                .iter()
                .filter(|(seq, _)| {
                    let seq = *seq as u64;
                    !covered
                        .iter()
                        .any(|(begin, end)| seq >= *begin && seq < *end)
                })
                .collect();
            if gap.is_empty() {
                continue;
            }
            if kind.eq_ignore_ascii_case("HNSW") {
                let segments = self.hnsw_mem_segments(&table, &meta.column)?;
                for mem in segments {
                    let Ok(mut mem) = mem.lock() else { continue };
                    for (seq, cells) in &gap {
                        let Some(value) = cells.get(ci) else { continue };
                        let text = value.to_slt_string();
                        let vector: Vec<f32> =
                            parse_vec_str(&text).iter().map(|x| *x as f32).collect();
                        if vector.len() == mem.dims {
                            mem.insert(*seq as u64, &vector);
                        }
                    }
                }
            } else if kind.eq_ignore_ascii_case("IVF") {
                let dims = self
                    .column_types(&table)
                    .ok()
                    .and_then(|types| types.get(ci).and_then(|ty| type_dim(ty)))
                    .unwrap_or(0);
                let mut recovered: Vec<(u64, Vec<f32>)> = Vec::new();
                for (seq, cells) in &gap {
                    let Some(value) = cells.get(ci) else { continue };
                    let text = value.to_slt_string();
                    let vector: Vec<f32> = parse_vec_str(&text).iter().map(|x| *x as f32).collect();
                    if !vector.is_empty() && (dims == 0 || vector.len() == dims) {
                        recovered.push((*seq as u64, vector));
                    }
                }
                if !recovered.is_empty() {
                    self.tables
                        .lock()
                        .unwrap()
                        .entry(self.table_key(&table))
                        .or_default()
                        .ivf_mem
                        .entry(meta.column.clone())
                        .or_default()
                        .extend(recovered);
                }
            } else if kind.eq_ignore_ascii_case("FULLTEXT") {
                // Maps the chunks already on disk and indexes from the first row
                // they do not cover (TextIndex::load_sealed).
                let tokenizer = self.index_tokenizer(&table, &meta.column);
                let _ = self.text_index_for(&table, &meta.column, tokenizer);
            }
        }
        Ok(())
    }

    pub(crate) fn build_index(&self, table: &str, column: &str) -> Result<()> {
        let schema = self.table_schema(table)?;
        // Every secondary column has a chunk form, so the build only reads the
        // indexed column (not the wide vectors in the same row): `index_lookup`
        // merges the chunks with the in-memory tail.
        let tkey = self.table_key(table);
        // A functional index's `column` is its expression: the rows are read
        // from the base column, and the key domain is the function's return type
        // (`IndexSecondaryFunctional::GetFuncReturnType`).
        let functional = {
            let tables = self.tables.lock().unwrap();
            tables
                .get(&tkey)
                .and_then(|e| e.secondary.functional.get(column).cloned())
        };
        let (col_idx, type_upper) = match &functional {
            Some(f) => {
                let pos = schema
                    .iter()
                    .position(|c| c == &f.base)
                    .ok_or_else(|| Error::Invalid(format!("unknown column '{}'", f.base)))?;
                (pos, f.returns.clone())
            }
            None => {
                let pos = schema
                    .iter()
                    .position(|c| c == column)
                    .ok_or_else(|| Error::Invalid(format!("unknown column '{column}'")))?;
                let ty = self
                    .column_types(table)
                    .ok()
                    .and_then(|t| t.get(pos).cloned())
                    .map(|t| t.trim().to_ascii_uppercase())
                    .unwrap_or_default();
                (pos, ty)
            }
        };
        // One row's indexed value: the cell itself, or `func(cell)` for a
        // functional index. A NULL has no key (`InsertInner` skips it).
        let evaluate = |row: &[Value]| -> Option<Value> {
            let value = match &functional {
                Some(f) => functional_value(&f.func, col_idx, row)?,
                None => row.get(col_idx)?.clone(),
            };
            (!matches!(value, Value::Null)).then_some(value)
        };
        self.tables
            .lock()
            .unwrap()
            .entry(tkey.clone())
            .or_default()
            .secondary
            .disk
            .remove(column);
        let int_column = type_upper.starts_with("INT") || type_upper.starts_with("UINT");
        let key_kind = secondary_key_kind(&type_upper);
        let low_card = self.secondary_low_cardinality(table, column, &type_upper);
        if low_card {
            // `SecondaryIndexDataLowCardinalityT`: unique keys, each with one
            // bitmap of segment offsets (`secondary_index_data_impl.cpp:284`).
            // BOOL and JSON are always low (`NewSecondaryIndexInMem`), and
            // `WITH (cardinality='low')` asks for it on any other type.
            let mut grouped: HashMap<u64, std::collections::BTreeMap<Vec<u8>, Vec<u32>>> =
                HashMap::new();
            for (seq, row) in self.fetch_rows_with_ids_projected_all(table, col_idx)? {
                let Some(value) = evaluate(&row) else {
                    continue;
                };
                let text = match &value {
                    Value::Str(s) => s.clone(),
                    other => other.to_slt_string(),
                };
                // A JSON document contributes one key per flattened term; every
                // other column contributes exactly one.
                let raw_keys: Vec<Vec<u8>> = if key_kind == KeyKind::Bytes {
                    storage::json_index::flatten_json_terms(&text)
                        .into_iter()
                        .map(String::into_bytes)
                        .collect()
                } else {
                    value_to_key(&type_upper, &value)
                };
                for raw in raw_keys {
                    grouped
                        .entry((seq as u64) >> 32)
                        .or_default()
                        .entry(raw)
                        .or_default()
                        .push((seq as u64 & 0xffff_ffff) as u32);
                }
            }
            let (readers, meta) =
                self.write_secondary_low_chunks(table, column, key_kind, grouped, "");
            let mut tables = self.tables.lock().unwrap();
            let entry = tables.entry(tkey.clone()).or_default();
            entry.secondary.low.insert(column.to_string(), readers);
            entry.secondary.chunk_meta.insert(column.to_string(), meta);
        } else if int_column {
            let mut by_segment: HashMap<u64, Vec<(Key, u32)>> = HashMap::new();
            for (seq, row) in self.fetch_rows_with_ids_projected_all(table, col_idx)? {
                let Some(Value::Int(v)) = evaluate(&row) else {
                    continue;
                };
                let ordered = integer_secondary_key(&type_upper, v);
                by_segment
                    .entry((seq as u64) >> 32)
                    .or_default()
                    .push((ordered, (seq as u64 & 0xffff_ffff) as u32));
            }
            self.write_secondary_pgm_chunks(table, column, key_kind, by_segment);
        } else if key_kind != KeyKind::U64 {
            // FLOAT/DOUBLE keep the raw value ordered, like the reference's
            // `KeepOrderedSelf` (`secondary_index_data.cppm:38`): a 4-byte f32
            // or an 8-byte f64, compared as a float. DATE/TIME and
            // DATETIME/TIMESTAMP are the `ConvertToOrderedI32`/`I64` kinds: the
            // ordered day (or day+second) count, also raw.
            let mut by_segment: HashMap<u64, Vec<(Key, u32)>> = HashMap::new();
            for (seq, row) in self.fetch_rows_with_ids_projected_all(table, col_idx)? {
                let key = match key_kind {
                    KeyKind::F32 | KeyKind::F64 => {
                        let value = match evaluate(&row) {
                            Some(Value::Float(f)) => f,
                            Some(Value::Int(i)) => i as f64,
                            _ => continue,
                        };
                        if key_kind == KeyKind::F32 {
                            Key::F32(value as f32)
                        } else {
                            Key::F64(value)
                        }
                    }
                    _ => {
                        let text = match evaluate(&row) {
                            Some(Value::Str(s)) => s,
                            Some(other) => other.to_slt_string(),
                            None => continue,
                        };
                        let Some(value) = temporal_key(&type_upper, &text) else {
                            continue;
                        };
                        if key_kind == KeyKind::I32 {
                            Key::I32(value as i32)
                        } else {
                            Key::I64(value)
                        }
                    }
                };
                by_segment
                    .entry((seq as u64) >> 32)
                    .or_default()
                    .push((key, (seq as u64 & 0xffff_ffff) as u32));
            }
            self.write_secondary_pgm_chunks(table, column, key_kind, by_segment);
        } else {
            // VARCHAR (and JSON) keys are hashed to u64, exactly like the
            // reference's `std::hash<std::string_view>` (`ConvertToOrderedKeyValue`).
            let mut by_segment: HashMap<u64, Vec<(Key, u32)>> = HashMap::new();
            for (seq, row) in self.fetch_rows_with_ids_projected_all(table, col_idx)? {
                let Some(value) = evaluate(&row) else {
                    continue;
                };
                let ordered = storage::secondary_segment::hash_varchar(&value.to_slt_string());
                by_segment
                    .entry((seq as u64) >> 32)
                    .or_default()
                    .push((Key::U64(ordered), (seq as u64 & 0xffff_ffff) as u32));
            }
            self.write_secondary_pgm_chunks(table, column, KeyKind::U64, by_segment);
        }
        // The memory map is only the tail written since the last dump.
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(tkey).or_default();
        entry.secondary.disk_backed.insert(column.to_string());
        entry
            .secondary
            .declared_types
            .insert(column.to_string(), type_upper.clone());
        entry
            .secondary
            .indexes
            .insert(column.to_string(), Arc::new(SecondaryIndex::new()));
        Ok(())
    }

    /// Reloads the HNSW/IVF chunk readers after new chunks were written, so a
    /// query in the same process sees what `flush()` just mirrored.
    pub(crate) fn reload_vector_indexes(&self) {
        let Ok(indexes) = self.storage.catalog.list_indexes() else {
            return;
        };
        // The reference has one buffer budget shared by every index; give each
        // HNSW index an equal share so the total stays within it.
        let hnsw_count = indexes
            .iter()
            .filter(|im| im.index_type.eq_ignore_ascii_case("HNSW"))
            .count()
            .max(1);
        for im in indexes {
            if im.index_type.eq_ignore_ascii_case("HNSW") {
                // Reconcile with the live chunk set and the buffer budget:
                // cleanup deletes the chunks a merged graph replaced, a rewrite
                // under the same id changes the file stamp, and the decoded
                // graphs must stay within `[buffer] buffer_manager_size` (the
                // reference's buffer manager). Without this a long import grows
                // without bound. Partial coverage is safe: `knn_candidates`
                // falls back to the exact scan when labels do not cover a row.
                let stamps = self.hnsw_chunk_stamps(&im.table, &im.name);
                if stamps.is_empty() {
                    continue;
                }
                let budget = self
                    .context
                    .hnsw_budget
                    .load(std::sync::atomic::Ordering::Relaxed)
                    .max(1)
                    / hnsw_count;
                let tkey = self.table_key(&im.table);
                let (mut total, have, kept) = {
                    let mut tables = self.tables.lock().unwrap();
                    let list = tables
                        .entry(tkey.clone())
                        .or_default()
                        .vector
                        .entry(im.column.clone())
                        .or_default();
                    list.retain(|s| stamps.get(&s.chunk_id).is_some_and(|st| *st == s.stamp));
                    let total: usize = list.iter().map(|s| s.bytes).sum();
                    let have: std::collections::HashSet<u64> =
                        list.iter().map(|s| s.chunk_id).collect();
                    (total, have, list.len())
                };
                // Newest chunks first: they hold the rows written since the
                // last merge, which are the ones a fresh scan needs most.
                let mut missing: Vec<u64> = stamps
                    .keys()
                    .copied()
                    .filter(|id| !have.contains(id))
                    .collect();
                missing.sort_unstable_by(|a, b| b.cmp(a));
                let mut loaded = Vec::new();
                for id in missing {
                    if total >= budget && kept + loaded.len() > 0 {
                        break;
                    }
                    if let Some(index) = self.load_vector_index_chunk(&im.table, &im.name, id) {
                        total += index.bytes;
                        loaded.push(Arc::new(index));
                    }
                }
                if !loaded.is_empty() {
                    self.tables
                        .lock()
                        .unwrap()
                        .entry(tkey)
                        .or_default()
                        .vector
                        .entry(im.column.clone())
                        .or_default()
                        .extend(loaded);
                }
                // Still over budget (or the budget shrank): evict the largest
                // decoded graphs, which frees the most memory.
                if total > budget {
                    let mut tables = self.tables.lock().unwrap();
                    let list = tables
                        .entry(self.table_key(&im.table))
                        .or_default()
                        .vector
                        .entry(im.column.clone())
                        .or_default();
                    list.sort_by_key(|s| std::cmp::Reverse(s.bytes));
                    while total > budget && list.len() > 1 {
                        let gone = list.pop().unwrap();
                        total = total.saturating_sub(gone.bytes);
                    }
                }
            } else if im.index_type.eq_ignore_ascii_case("IVF") {
                let segments = self.load_ivf_indexes(&im.table, &im.name);
                if !segments.is_empty() {
                    let tkey = self.table_key(&im.table);
                    self.tables
                        .lock()
                        .unwrap()
                        .entry(tkey)
                        .or_default()
                        .ivf
                        .insert(
                            im.column.clone(),
                            segments.into_iter().map(Arc::new).collect(),
                        );
                }
            }
        }
    }

    /// The catalog state a `DumpMemIndex` transaction must still see at commit
    /// (C++ `TableMeta` / `TableIndexMeta` versions).
    pub(crate) fn dump_version(&self, table: &str, index: &str) -> Result<DumpVersion> {
        let table = self.table_key(table);
        let table_meta = self
            .storage
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let index_meta = self
            .storage
            .catalog
            .list_indexes()?
            .into_iter()
            .find(|im| self.table_key(&im.table) == table && im.name == index)
            .ok_or_else(|| Error::NotFound(format!("index '{index}' on '{table}'")))?;
        Ok(DumpVersion {
            table: table_meta,
            index: index_meta,
            optimize_epoch: self.index_epoch.load(std::sync::atomic::Ordering::SeqCst),
        })
    }

    /// Whether `index` still holds rows in its memory tail.
    ///
    /// `NewTxn::DumpMemIndex` skips before it opens a transaction when the
    /// memory index is empty, so an explicit dump of an index whose checkpoint
    /// already published everything spends no commit ts and writes no WAL
    /// entry. A kind whose tail is not modelled here reports `true`, so the
    /// dump keeps its old behaviour.
    fn index_has_memory_tail(&self, table: &str, meta: &IndexMeta) -> bool {
        let key = self.table_key(table);
        let kind = meta.index_type.trim();
        // Every branch clones the handle out of `tables` and locks it only
        // after the map is released: a scan holds one of these guards while it
        // waits for `tables` (see `Engine::text_index_for`), so locking an
        // index under `tables` would close that cycle.
        if is_secondary_kind(kind) {
            let tables = self.tables.lock().unwrap();
            return tables
                .get(&key)
                .and_then(|e| e.secondary.indexes.get(&meta.column))
                .map(|i| i.approx_bytes() > 0)
                .unwrap_or(false);
        }
        if kind.eq_ignore_ascii_case("HNSW") {
            let mem = {
                let tables = self.tables.lock().unwrap();
                tables
                    .get(&key)
                    .and_then(|e| e.hnsw_mem.get(&meta.column).cloned())
            };
            return mem
                .and_then(|m| m.lock().ok().map(|g| !g.is_empty()))
                .unwrap_or(false);
        }
        if kind.eq_ignore_ascii_case("IVF") {
            let tables = self.tables.lock().unwrap();
            return tables
                .get(&key)
                .and_then(|e| e.ivf_mem.get(&meta.column))
                .map(|rows| !rows.is_empty())
                .unwrap_or(false);
        }
        if kind.eq_ignore_ascii_case("FULLTEXT") {
            let text = {
                let tables = self.tables.lock().unwrap();
                tables
                    .get(&key)
                    .and_then(|e| e.text.get(&meta.column).cloned())
            };
            return text
                .and_then(|t| t.read().ok().map(|g| g.mem.pending_docs() > 0))
                .unwrap_or(false);
        }
        if kind.eq_ignore_ascii_case("BMP") {
            let sparse = {
                let tables = self.tables.lock().unwrap();
                tables
                    .get(&key)
                    .and_then(|e| e.sparse.get(&meta.column).cloned())
            };
            return sparse
                .and_then(|s| s.read().ok().map(|g| !g.tail.is_empty()))
                .unwrap_or(false);
        }
        if kind.eq_ignore_ascii_case("PLAID")
            || kind.eq_ignore_ascii_case("SMVE")
            || kind.eq_ignore_ascii_case("EMVB")
            || kind.eq_ignore_ascii_case("MULTIVECTOR")
        {
            let mv = {
                let tables = self.tables.lock().unwrap();
                tables
                    .get(&key)
                    .and_then(|e| e.mv.get(&meta.column).cloned())
            };
            return mv
                .and_then(|m| m.read().ok().map(|g| !g.tail.is_empty()))
                .unwrap_or(false);
        }
        true
    }

    /// Whether the index has chunk files the catalog does not record yet.
    ///
    /// The checkpoint mirrors an index's chunk files but has no commit ts to
    /// name them with (`write_index_segments` drops a record whose ts is
    /// unknown), so a chunk the mirror wrote stays unpublished until a dump
    /// publishes it. A dump must not be skipped in that state even when the
    /// memory tail is empty - it is the transaction that gives the chunk its ts.
    fn index_has_unpublished_chunks(&self, table: &str, meta: &IndexMeta) -> bool {
        let Ok(Some(table_meta)) = self.storage.catalog.get_table_by_name(table) else {
            return false;
        };
        let db_id = self.database_id_of(table).to_string();
        let table_id = table_meta.id.to_string();
        let index_id = meta.id.to_string();
        let Ok(records) = catalog::layout::read_index_chunks(
            self.storage.catalog.store().as_ref(),
            &db_id,
            &table_id,
            &index_id,
        ) else {
            return false;
        };
        let recorded: usize = records.iter().map(|(_, chunks)| chunks.len()).sum();
        let fulltext = meta.index_type.eq_ignore_ascii_case("FULLTEXT");
        let root = self
            .blocks_root()
            .join(format!("db_{db_id}/tbl_{table_id}/idx_{index_id}"));
        let Ok(segments) = std::fs::read_dir(&root) else {
            return false;
        };
        let mut files = 0usize;
        for segment in segments.flatten() {
            let Ok(entries) = std::fs::read_dir(segment.path()) else {
                continue;
            };
            for entry in entries.flatten() {
                let name = entry.file_name().to_string_lossy().to_string();
                let chunk = if fulltext {
                    name.starts_with("ft_") && name.ends_with(".dic")
                } else {
                    name.starts_with("chunk_") && name.ends_with(".idx")
                };
                if chunk {
                    files += 1;
                }
            }
        }
        files > recorded
    }

    /// The `DumpIndex` RPC: C++ `NewTxn::DumpMemIndex` inside a transaction of
    /// its own (`DumpIndexProcessor::DoDump` opens one with
    /// `BeginTxnShared(…, kDumpMemIndex)`).
    ///
    /// `NewTxn::DumpMemIndex` returns before it opens that transaction when the
    /// memory index has nothing new ("skipped dumping MemIndex … since it
    /// doesn't exist"), so an empty dump spends no commit ts and writes no WAL
    /// entry. Otherwise the tail is sealed **inside** the transaction, so the
    /// chunk it writes carries this statement's ts, exactly like the
    /// reference's `DumpMemIndexTxnStore`.
    pub fn dump_index_transaction(&self, table: &str, index: &str) -> Result<()> {
        let version = self.dump_version(table, index)?;
        let table = self.qualified_table(table);
        // The reference skips a dump when the memory index has nothing new.
        //
        // The port's checkpoint also mirrors an index's chunk **files** without
        // a commit ts to name them with, so a dump is still required while one
        // of those is unpublished: it is the transaction that gives the chunk
        // its ts.
        if !self.index_has_memory_tail(&table, &version.index)
            && !self.index_has_unpublished_chunks(&table, &version.index)
        {
            return Ok(());
        }
        let turn = self.txn.reserve_commit_ts();
        let ts = turn.ts();
        // The reference writes the chunk files and their catalog records inside
        // the dump transaction. The port seals this index's tail into its spill
        // files and then mirrors them into the C++ chunk layout, so the command
        // below names what it published and replay re-creates exactly those
        // records with this ts. The checkpoint publishes the same files, so it
        // takes the same lock.
        {
            let _serial = self.storage.flush_lock.lock().unwrap();
            self.seal_index_tail(&table, &version.index);
            if let Some(meta) = self.storage.catalog.get_table_by_name(&table)? {
                let _ = self.publish_index_chunks(&table, meta.id, Some((index, ts)));
            }
        }
        let mut dump_txn = self.txn.begin();
        // `NewTxn::DumpMemIndex`: two dumps never run together (the reference
        // calls it unrecoverable), and an update on the same segment conflicts
        // with it.
        dump_txn.set_op(TxnOp::DumpMemIndex {
            table: table.to_string(),
            index: index.to_string(),
            segments: self.segment_ids(&table),
        });
        // `DumpMemIndexTxnStore::ToWalEntry`: the chunks this dump seals ride in
        // the transaction's own `DumpIndex` command, so replay re-creates their
        // `idx_chunk|…` records with this frame's commit ts
        // (`LoadFlushedChunkIndex1`). Without the command a directory killed
        // after the dump reopens with the value the last checkpoint published.
        let commands = self.dump_index_commands_for(&table, index)?;
        if !commands.is_empty() {
            dump_txn.set_typed_commands(commands);
        }
        self.txn.commit_at(dump_txn, ts)?;
        // `CommitBottom` of the dump transaction, on the table's worker: the
        // dump ts record, the chunk stamps, the checkpoint cover and the
        // sealed chunk write.
        let bottom_table = self.table_key(&table);
        let shard_key = bottom_table.clone();
        let index_name = index.to_string();
        let secondary = is_secondary_kind(&version.index.index_type);
        self.run_commit_bottom(&shard_key, move |engine| {
            // A checkpoint mirrors the chunk layer again and would compute the
            // ts of every chunk from this value (`publish_index_chunks`).
            engine.storage.catalog.store().put(
                &crate::dump_ts_key(&bottom_table, &index_name),
                &ts.to_le_bytes(),
            )?;
            // The chunk record the reference writes in this transaction
            // (`AddChunkIndexID1`): the chunks already published for the index
            // now belong to `ts`.
            engine.stamp_chunk_ts(&bottom_table, &index_name, ts)?;
            if secondary {
                engine.dump_index(&bottom_table, &index_name)?;
            }
            Ok(())
        })
    }

    /// The `DumpIndex` payload of an explicit dump
    /// (`DumpMemIndexTxnStore::ToWalEntry`): one command per segment with the
    /// chunks it seals.
    ///
    /// The reference writes the chunk records inside the dump transaction, so
    /// the frame's commit ts is the value they carry. The port mirrors the
    /// chunk layer, so the caller publishes this index just before the commit
    /// and the command names the records that publish wrote; replay rewrites
    /// exactly those with this transaction's ts. A segment with no chunk files
    /// to publish has nothing to name either, so it contributes nothing.
    ///
    /// Nothing is deprecated: the dump re-publishes the chunks a checkpoint
    /// already mirrored instead of replacing them, and a `deprecate_ids` entry
    /// would leave a `drop|idx_chunk` marker the reference never has.
    fn dump_index_commands_for(&self, table: &str, index: &str) -> Result<Vec<storage::WalCmd>> {
        let table = self.qualified_table(table);
        let Some(meta) = self.storage.catalog.get_table_by_name(&table)? else {
            return Ok(Vec::new());
        };
        let index_id = self.index_position(&table, index)?;
        let db_id = self.database_id_of(&table).to_string();
        let table_id = meta.id.to_string();
        let (db_name, table_name) = split_table_name(&table);
        let table_key = self.table_key(&table);
        let mut out = Vec::new();
        for (segment, chunks) in catalog::layout::read_index_chunks(
            self.storage.catalog.store().as_ref(),
            &db_id,
            &table_id,
            &index_id.to_string(),
        )? {
            if chunks.is_empty() {
                continue;
            }
            let chunk_infos: Vec<storage::wal_entry::WalChunkIndexInfo> = chunks
                .iter()
                .map(|(chunk_id, info)| storage::wal_entry::WalChunkIndexInfo {
                    chunk_id: *chunk_id as u32,
                    base_name: info.base_name.clone(),
                    base_row_id: storage::wal_entry::WalRowId::from_u64(info.base_row_id),
                    row_count: info.row_count as u32,
                    term_count: info.term_count as u32,
                    index_size: info.index_size as u32,
                    deprecate_ts: u64::MAX,
                })
                .collect();
            out.push(storage::WalCmd::DumpIndex {
                db_name: db_name.clone(),
                db_id: db_id.clone(),
                table_name: table_name.clone(),
                table_id: table_id.clone(),
                index_name: index.to_string(),
                index_id: index_id.to_string(),
                segment_id: segment,
                chunk_infos,
                deprecate_ids: Vec::new(),
                table_key: table_key.clone(),
            });
        }
        Ok(out)
    }

    /// Applies one `DumpMemIndex`: seals the target index's memory tail.
    pub fn dump_index(&self, table: &str, index: &str) -> Result<()> {
        let version = self.dump_version(table, index)?;
        if is_secondary_kind(&version.index.index_type) {
            self.seal_secondary_index(table, &version.index.column);
            return Ok(());
        }
        Err(Error::Invalid(format!(
            "DumpMemIndex is not supported for index type '{}'",
            version.index.index_type
        )))
    }

    /// Seals one index's memory tail into its spill/chunk files, in the form
    /// its kind dumps it: a secondary tail becomes `seg_{s}_tail{n}.idx`, a
    /// full-text tail `ft_{first_row:016x}.*`, a BMP tail its `.bmp` chunk, a
    /// multi-vector tail its `.mvec` chunk, an HNSW memory graph a
    /// `chunk_{id}.idx`, and IVF rewrites its per-segment `chunk_0.idx`.
    ///
    /// The dump transaction calls this before `publish_index_chunks`, so the
    /// chunks it publishes are the ones this statement sealed and carry its
    /// commit ts.
    fn seal_index_tail(&self, table: &str, meta: &IndexMeta) {
        let table = self.qualified_table(table);
        if is_secondary_kind(&meta.index_type) {
            self.seal_secondary_index(&table, &meta.column);
        } else if meta.index_type.eq_ignore_ascii_case("FULLTEXT") {
            self.seal_text_index(&table, &meta.column);
        } else if meta.index_type.eq_ignore_ascii_case("BMP") {
            self.seal_sparse_index(&table, &meta.column);
        } else if meta.index_type.eq_ignore_ascii_case("HNSW") {
            if self.dump_hnsw_index(&table, &meta.name).is_ok() {
                let key = self.table_key(&table);
                let mut tables = self.tables.lock().unwrap();
                if let Some(e) = tables.get_mut(&key) {
                    e.hnsw_mem.remove(&meta.column);
                    e.index_pending_bytes.insert(meta.column.clone(), 0);
                    e.hnsw_probed = false;
                }
            }
        } else if meta.index_type.eq_ignore_ascii_case("IVF") {
            let _ = self.write_ivf_index(&table, &meta.name);
            let key = self.table_key(&table);
            let mut tables = self.tables.lock().unwrap();
            if let Some(rows) = tables
                .get_mut(&key)
                .and_then(|e| e.ivf_mem.get_mut(&meta.column))
            {
                rows.clear();
            }
        } else if meta.index_type.eq_ignore_ascii_case("PLAID")
            || meta.index_type.eq_ignore_ascii_case("SMVE")
            || meta.index_type.eq_ignore_ascii_case("EMVB")
            || meta.index_type.eq_ignore_ascii_case("MULTIVECTOR")
        {
            self.seal_multivector_index(&table, &meta.column);
        }
    }

    /// Seals the full-text memory tail of one column into its segment files
    /// (`ft_{first_row:016x}.{dic,pos,len,row}`).
    ///
    /// The write runs with the column's lock released
    /// ([`crate::engine::state::seal_text_index_unlocked`]): a dump must not make
    /// every query on the column wait for the segment's fsync. The caller holds
    /// `flush_lock`, which serialises dumps and checkpoints but is never taken
    /// on the query path.
    pub(crate) fn seal_text_index(&self, table: &str, column: &str) {
        let key = self.table_key(table);
        let shared = self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|entry| entry.text.get(column).cloned());
        if let Some(shared) = shared {
            crate::engine::state::seal_text_index_unlocked(&shared);
        }
    }

    pub(crate) fn seal_secondary_indexes(&self) {
        self.seal_secondary_indexes_filtered(None);
    }

    /// Dumps one secondary index's memory tail (`DumpMemIndex` for `index`).
    pub(crate) fn seal_secondary_index(&self, table: &str, column: &str) {
        self.seal_secondary_indexes_filtered(Some((table, column)));
    }

    pub(crate) fn seal_secondary_indexes_filtered(&self, only: Option<(&str, &str)>) {
        let wanted = only.map(|(t, c)| (self.table_key(t), c.to_string()));
        let mut backed: Vec<(String, String)> = {
            let tables = self.tables.lock().unwrap();
            tables
                .iter()
                .flat_map(|(t, e)| {
                    e.secondary
                        .disk_backed
                        .iter()
                        .map(move |c| (t.clone(), c.clone()))
                })
                .collect()
        };
        if let Some((tkey, column)) = &wanted {
            backed.retain(|(t, c)| t == tkey && c == column);
        }
        if backed.is_empty() {
            return;
        }
        let Ok(catalog_indexes) = self.storage.catalog.list_indexes() else {
            return;
        };
        for (tkey, column) in backed {
            let Some(im) = catalog_indexes
                .iter()
                .find(|im| self.table_key(&im.table) == tkey && im.column == column)
            else {
                continue;
            };
            let Ok(schema) = self.table_schema(&im.table) else {
                continue;
            };
            let Some(ci) = schema.iter().position(|c| c == &im.column) else {
                continue;
            };
            let Ok(types) = self.column_types(&im.table) else {
                continue;
            };
            let ty = types
                .get(ci)
                .map(|t| t.trim().to_ascii_uppercase())
                .unwrap_or_default();
            let low_card = self.secondary_low_cardinality(&im.table, &column, &ty);
            let tail_kind = secondary_key_kind(&ty);
            let key = index_key(&im.table, &column);
            // `seal` empties the tail, and the chunk below only becomes visible
            // after it is written; the frozen entries stay in `sealing`, where
            // readers merge them, so the window in between loses no row.
            let sealed = {
                let mut tables = self.tables.lock().unwrap();
                let Some(entry) = tables.get_mut(&tkey) else {
                    continue;
                };
                let Some(index) = entry.secondary.indexes.get(&column) else {
                    continue;
                };
                let sealed = Arc::new(index.seal());
                if sealed.is_empty() {
                    continue;
                }
                entry
                    .secondary
                    .sealing
                    .entry(column.clone())
                    .or_default()
                    .push(Arc::clone(&sealed));
                sealed
            };
            let tail = sealed.entries();
            // `ChunkIndexMetaInfo`: the first row id in the dump and its row
            // count. Recorded per dump (one chunk per seal call).
            let base = tail
                .iter()
                .flat_map(|(_, ids)| ids.iter())
                .min()
                .copied()
                .unwrap_or(0);
            // `ChunkIndexMetaInfo::row_count_` counts **data rows**, not entries
            // (`SecondaryIndexInMemT::GetRowCount`): a JSON row contributes one
            // entry per flattened term but one row.
            let row_count = {
                let mut ids: Vec<u64> = tail
                    .iter()
                    .flat_map(|(_, ids)| ids.iter().copied())
                    .collect();
                ids.sort_unstable();
                ids.dedup();
                ids.len()
            };
            if !low_card {
                let mut by_segment: HashMap<u64, Vec<(Key, u32)>> = HashMap::new();
                for (raw_key, ids) in &tail {
                    // The tail was written in the column's ordered encoding
                    // (`Key::ordered_bytes`), which is exactly the domain the
                    // chunk stores, so a dump decodes instead of parsing text.
                    let Some(ordered) = Key::from_ordered_bytes(tail_kind, raw_key) else {
                        continue;
                    };
                    for id in ids {
                        by_segment
                            .entry(id >> 32)
                            .or_default()
                            .push((ordered, (*id & 0xffff_ffff) as u32));
                    }
                }
                let dir = self.dir.join("secondary").join(key.replace('/', "_"));
                let start = std::fs::read_dir(&dir)
                    .map(|d| {
                        d.filter_map(|e| e.ok())
                            .filter(|e| e.path().extension().is_some_and(|x| x == "idx"))
                            .count()
                    })
                    .unwrap_or(0);
                let mut readers = Vec::new();
                for (offset, (segment, mut entries)) in by_segment.into_iter().enumerate() {
                    entries.sort_by(|a, b| a.0.order(&b.0));
                    let path = dir.join(format!("seg_{segment}_tail{}.idx", start + offset));
                    if storage::secondary_segment::write_secondary_segment_typed(
                        &path, tail_kind, &entries,
                    )
                    .is_err()
                    {
                        continue;
                    }
                    if let Ok(reader) =
                        storage::secondary_segment::SecondarySegmentReader::open_with_kind(
                            &path, tail_kind,
                        )
                    {
                        readers.push((segment << 32, Arc::new(reader)));
                    }
                }
                {
                    let mut tables = self.tables.lock().unwrap();
                    let entry = tables.entry(tkey.clone()).or_default();
                    entry
                        .secondary
                        .disk
                        .entry(column.clone())
                        .or_default()
                        .extend(readers);
                    entry
                        .secondary
                        .chunk_meta
                        .entry(column.clone())
                        .or_default()
                        .push((base, row_count));
                    // The chunk now covers the frozen entries, atomically with
                    // the readers above: a reader either sees the chunk or the
                    // frozen tail.
                    if let Some(pending) = entry.secondary.sealing.get_mut(&column) {
                        pending.retain(|tail| !Arc::ptr_eq(tail, &sealed));
                    }
                }
            } else {
                // `SecondaryIndexDataLowCardinalityT::InsertData`: one bitmap of
                // segment offsets per unique key. A dump feeds it the tail's
                // grouped keys, one chunk file per segment.
                let mut grouped: HashMap<u64, std::collections::BTreeMap<Vec<u8>, Vec<u32>>> =
                    HashMap::new();
                for (raw_key, ids) in &tail {
                    for id in ids {
                        grouped
                            .entry(id >> 32)
                            .or_default()
                            .entry(raw_key.clone())
                            .or_default()
                            .push((*id & 0xffff_ffff) as u32);
                    }
                }
                let (readers, meta) =
                    self.write_secondary_low_chunks(&im.table, &column, tail_kind, grouped, "tail");
                let mut tables = self.tables.lock().unwrap();
                let entry = tables.entry(tkey.clone()).or_default();
                entry
                    .secondary
                    .low
                    .entry(column.clone())
                    .or_default()
                    .extend(readers);
                entry
                    .secondary
                    .chunk_meta
                    .entry(column.clone())
                    .or_default()
                    .extend(meta);
                entry
                    .secondary
                    .chunk_meta
                    .entry(column.clone())
                    .or_default()
                    .push((base, row_count));
                // Atomic with the readers above: a reader sees either the chunk
                // or the frozen tail.
                if let Some(pending) = entry.secondary.sealing.get_mut(&column) {
                    pending.retain(|frozen| !Arc::ptr_eq(frozen, &sealed));
                }
            }
        }
    }

    /// Total bytes buffered by every in-memory index form. The unified budget
    /// covers full-text, HNSW, sparse, multi-vector and secondary tails.
    pub(crate) fn memindex_bytes(&self) -> usize {
        // The `Arc`s are cloned out of `tables` before any index is locked: a
        // scan holds one of these read guards while it fetches rows, and that
        // fetch needs `tables`, so locking an index under `tables` closes the
        // cycle `Engine::text_index_for` documents.
        let (text, sparse, mv) = {
            let tables = self.tables.lock().unwrap();
            let mut text = Vec::new();
            let mut sparse = Vec::new();
            let mut mv = Vec::new();
            for e in tables.values() {
                text.extend(e.text.values().cloned());
                sparse.extend(e.sparse.values().cloned());
                mv.extend(e.mv.values().cloned());
            }
            (text, sparse, mv)
        };
        let mut total = 0usize;
        for idx in text {
            if let Ok(g) = idx.read() {
                total += g.mem.mem_used();
            }
        }
        for idx in sparse {
            if let Ok(g) = idx.read() {
                total += g.tail_bytes;
            }
        }
        for idx in mv {
            if let Ok(g) = idx.read() {
                total += g.tail_bytes;
            }
        }
        // HNSW is not counted here: `note_index_insert` owns its budget (the
        // reference charges the tracer per appended block, and this counter
        // would double it), so the per-row check below is about the text,
        // sparse, multivector and secondary tails only.
        total
            + self
                .tables
                .lock()
                .unwrap()
                .values()
                .flat_map(|e| e.secondary.indexes.values())
                .map(|index| index.approx_bytes())
                .sum::<usize>()
    }

    /// `SecondaryIndexCardinality` of one indexed column.
    ///
    /// `BOOL` is inherently low cardinality and a JSON index exists only in the
    /// low form (`NewSecondaryIndexInMem`, `secondary_index_in_mem_impl.cpp:
    /// 250-334`); `WITH (cardinality='low')` asks for the low layout on any
    /// other column.
    pub(crate) fn secondary_low_cardinality(
        &self,
        table: &str,
        column: &str,
        declared: &str,
    ) -> bool {
        let declared = declared.trim().to_ascii_uppercase();
        if declared.starts_with("BOOL") || declared.starts_with("JSON") {
            return true;
        }
        let tkey = self.table_key(table);
        self.storage
            .catalog
            .list_indexes()
            .ok()
            .and_then(|list| {
                list.into_iter()
                    .find(|im| self.table_key(&im.table) == tkey && im.column == column)
            })
            .is_some_and(|im| {
                im.options.iter().any(|(key, value)| {
                    key.eq_ignore_ascii_case("CARDINALITY") && value.eq_ignore_ascii_case("low")
                })
            })
    }

    /// Writes one low-cardinality chunk per segment and returns the readers with
    /// their base row id (`segment << 32`).
    ///
    /// `grouped` maps a segment to its `raw key -> offsets`; a raw key is the
    /// column's ordered encoding — whose lexicographic order *is* the key order —
    /// or, for a JSON column, the term itself. `suffix` keeps a dump's chunks
    /// distinct from a rebuild's (`tail` versus empty).
    /// Returns the readers and the `(base row id, data row count)` of every chunk
    /// it wrote, the `ChunkIndexMetaInfo` the reference records per chunk.
    fn write_secondary_low_chunks(
        &self,
        table: &str,
        column: &str,
        kind: KeyKind,
        grouped: HashMap<u64, std::collections::BTreeMap<Vec<u8>, Vec<u32>>>,
        suffix: &str,
    ) -> (SecondaryLowChunks, Vec<(u64, usize)>) {
        use storage::secondary_lowcard::{write_lowcard_segment_typed, LowCardKey, LowCardReader};
        let key = index_key(table, column);
        let dir = self.dir.join("secondary_low").join(key.replace('/', "_"));
        let start = std::fs::read_dir(&dir)
            .map(|d| {
                d.filter_map(|e| e.ok())
                    .filter(|e| e.path().extension().is_some_and(|x| x == "idx"))
                    .count()
            })
            .unwrap_or(0);
        let mut readers: SecondaryLowChunks = Vec::new();
        let mut meta: Vec<(u64, usize)> = Vec::new();
        for (offset, (segment, keys)) in grouped.into_iter().enumerate() {
            let entries: Vec<(LowCardKey, Vec<u32>)> = keys
                .into_iter()
                .filter_map(|(raw, mut offsets)| {
                    let key = if kind == KeyKind::Bytes {
                        LowCardKey::Bytes(raw)
                    } else {
                        LowCardKey::Fixed(Key::from_ordered_bytes(kind, &raw)?)
                    };
                    offsets.sort_unstable();
                    offsets.dedup();
                    Some((key, offsets))
                })
                .collect();
            if entries.is_empty() {
                continue;
            }
            let path = dir.join(format!("seg_{segment}_{suffix}{}.idx", start + offset));
            if write_lowcard_segment_typed(&path, &entries).is_err() {
                continue;
            }
            if let Ok(reader) = LowCardReader::open_with_kind(&path, kind) {
                // The chunk covers one data row per distinct offset, not one per
                // unique key (a JSON row contributes several terms).
                let mut offsets: Vec<u32> = entries
                    .iter()
                    .flat_map(|(_, list)| list.iter().copied())
                    .collect();
                offsets.sort_unstable();
                offsets.dedup();
                meta.push((segment << 32, offsets.len()));
                readers.push((segment << 32, Arc::new(reader)));
            }
        }
        (readers, meta)
    }

    /// The functional index an `IndexMeta` describes, from the parameters its
    /// record carries: `FUNC`, `FUNC_COLUMN` and `FUNC_RETURN`, falling back to
    /// parsing the expression for a record written by another engine.
    fn functional_from_meta(&self, im: &IndexMeta) -> Option<FunctionalIndex> {
        if !im.index_type.eq_ignore_ascii_case("SECONDARYFUNCTIONAL") {
            return None;
        }
        let opt = |name: &str| {
            im.options
                .iter()
                .find(|(k, _)| k.eq_ignore_ascii_case(name))
                .map(|(_, v)| v.trim().to_string())
                .filter(|v| !v.is_empty())
        };
        let (func, base) = match (opt("FUNC"), opt("FUNC_COLUMN")) {
            (Some(func), Some(base)) => (func.to_ascii_lowercase(), base.to_ascii_lowercase()),
            _ => functional_parts(&im.column)?,
        };
        let returns = opt("FUNC_RETURN").or_else(|| {
            let base_type = self
                .table_schema(&im.table)
                .ok()?
                .iter()
                .position(|c| c == &base)
                .and_then(|i| self.column_types(&im.table).ok()?.get(i).cloned())?;
            functional_return_type(&func, &base_type).map(str::to_string)
        })?;
        Some(FunctionalIndex {
            base,
            func,
            returns,
        })
    }

    /// Records how to evaluate a functional index: its base column, its scalar
    /// function and the function's return type (`func_return_type` in the
    /// reference's index record).
    pub(crate) fn register_functional_index(
        &self,
        table: &str,
        expression: &str,
        base: &str,
        func: &str,
        returns: &str,
    ) {
        let key = self.table_key(table);
        self.tables
            .lock()
            .unwrap()
            .entry(key)
            .or_default()
            .secondary
            .functional
            .insert(
                expression.to_string(),
                FunctionalIndex {
                    base: base.to_string(),
                    func: func.to_string(),
                    returns: returns.to_string(),
                },
            );
    }

    /// The declared type of one indexed column (`INT`, `VARCHAR(64)`, `JSON`, …).
    fn secondary_declared_type(&self, table: &str, column: &str) -> String {
        self.table_schema(table)
            .ok()
            .and_then(|schema| {
                let ci = schema.iter().position(|c| c == column)?;
                self.column_types(table).ok()?.get(ci).cloned()
            })
            .unwrap_or_default()
    }

    /// The segment a spill file belongs to: `seg_{segment}_…idx`.
    fn secondary_spill_segment(path: &std::path::Path) -> Option<u64> {
        path.file_name()?
            .to_string_lossy()
            .strip_prefix("seg_")?
            .split('_')
            .next()?
            .parse()
            .ok()
    }

    /// `OptimizeIndexInner`'s secondary case for every secondary index
    /// (`new_txn_index_impl.cpp:336-340` -> `OptimizeSecondaryIndex`).
    pub(crate) fn optimize_all_secondary_indexes(&self) -> Result<()> {
        for im in self.storage.catalog.list_indexes()? {
            if !is_secondary_kind(&im.index_type) {
                continue;
            }
            if let Err(error) = self.merge_secondary_chunks(&im.table, &im.column) {
                eprintln!(
                    "optimize: merging the secondary chunks of {} failed: {error}",
                    im.column
                );
            }
        }
        Ok(())
    }

    /// Folds one secondary column's chunks into one per segment.
    ///
    /// The reference merges a segment's chunks by feeding every old chunk's
    /// `(key, offset)` pairs to `InsertMergeData` and dumping the result as a
    /// single new chunk. The offsets are already segment-relative, so the merge
    /// is a sort over the concatenation and nothing is rebased. In this port a
    /// column's chunk set *is* its spill-file set (`publish_secondary_chunks`
    /// derives the chunk records from those files), so folding the files is the
    /// merge; a segment left with one file publishes one chunk, which is the
    /// layout the reference has after an optimize.
    pub(crate) fn merge_secondary_chunks(&self, table: &str, column: &str) -> Result<bool> {
        let declared = self.secondary_declared_type(table, column);
        if self.secondary_low_cardinality(table, column, &declared) {
            return self.merge_low_card_chunks(table, column, &declared);
        }
        use storage::secondary_segment::{write_secondary_segment_typed, SecondarySegmentReader};
        let kind = secondary_key_kind(&declared);
        let dir = self
            .dir
            .join("secondary")
            .join(index_key(table, column).replace('/', "_"));
        let Ok(files) = std::fs::read_dir(&dir) else {
            return Ok(false);
        };
        let mut by_segment: std::collections::BTreeMap<u64, Vec<std::path::PathBuf>> =
            Default::default();
        for entry in files.flatten() {
            let path = entry.path();
            if path.extension().is_none_or(|e| e != "idx") {
                continue;
            }
            if let Some(segment) = Self::secondary_spill_segment(&path) {
                by_segment.entry(segment).or_default().push(path);
            }
        }
        let mut merged = false;
        for (segment, paths) in by_segment {
            if paths.len() < 2 {
                continue;
            }
            let mut pairs: Vec<(Key, u32)> = Vec::new();
            for path in &paths {
                if let Ok(reader) = SecondarySegmentReader::open_with_kind(path, kind) {
                    pairs.extend(reader.pairs());
                }
            }
            if pairs.is_empty() {
                continue;
            }
            pairs.sort_by(|a, b| a.0.order(&b.0).then(a.1.cmp(&b.1)));
            let path = dir.join(format!("seg_{segment}_merged{}.idx", pairs.len()));
            if write_secondary_segment_typed(&path, kind, &pairs).is_err() {
                continue;
            }
            for old in &paths {
                let _ = std::fs::remove_file(old);
            }
            merged = true;
        }
        if merged {
            self.reload_secondary_chunks(table, column, kind);
        }
        Ok(merged)
    }

    /// Re-reads a high-cardinality column's spill files into the registry.
    fn reload_secondary_chunks(&self, table: &str, column: &str, kind: KeyKind) {
        use storage::secondary_segment::SecondarySegmentReader;
        let dir = self
            .dir
            .join("secondary")
            .join(index_key(table, column).replace('/', "_"));
        let mut readers: SecondaryDiskChunks = Vec::new();
        let mut meta: std::collections::BTreeMap<u64, usize> = Default::default();
        if let Ok(files) = std::fs::read_dir(&dir) {
            for entry in files.flatten() {
                let path = entry.path();
                if path.extension().is_none_or(|e| e != "idx") {
                    continue;
                }
                let Some(segment) = Self::secondary_spill_segment(&path) else {
                    continue;
                };
                if let Ok(reader) = SecondarySegmentReader::open_with_kind(&path, kind) {
                    *meta.entry(segment).or_default() += reader.row_count();
                    readers.push((segment << 32, Arc::new(reader)));
                }
            }
        }
        let tkey = self.table_key(table);
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(tkey).or_default();
        entry.secondary.disk.insert(column.to_string(), readers);
        entry.secondary.chunk_meta.insert(
            column.to_string(),
            meta.into_iter()
                .map(|(segment, rows)| (segment << 32, rows))
                .collect(),
        );
    }

    /// The low-cardinality half of [`Self::merge_secondary_chunks`]: the union of
    /// the chunks' offset bitmaps per key, written as one chunk.
    fn merge_low_card_chunks(&self, table: &str, column: &str, declared: &str) -> Result<bool> {
        use storage::secondary_lowcard::{write_lowcard_segment_typed, LowCardKey, LowCardReader};
        let kind = secondary_key_kind(declared);
        let dir = self
            .dir
            .join("secondary_low")
            .join(index_key(table, column).replace('/', "_"));
        let Ok(files) = std::fs::read_dir(&dir) else {
            return Ok(false);
        };
        let mut by_segment: std::collections::BTreeMap<u64, Vec<std::path::PathBuf>> =
            Default::default();
        for entry in files.flatten() {
            let path = entry.path();
            if path.extension().is_none_or(|e| e != "idx") {
                continue;
            }
            if let Some(segment) = Self::secondary_spill_segment(&path) {
                by_segment.entry(segment).or_default().push(path);
            }
        }
        let mut merged = false;
        for (segment, paths) in by_segment {
            if paths.len() < 2 {
                continue;
            }
            let mut grouped: std::collections::BTreeMap<Vec<u8>, Vec<u32>> = Default::default();
            for path in &paths {
                let Ok(reader) = LowCardReader::open_with_kind(path, kind) else {
                    continue;
                };
                for i in 0..reader.len() {
                    let Some(key) = reader.key_at(i) else {
                        continue;
                    };
                    let raw = match &key {
                        LowCardKey::Fixed(key) => key.ordered_bytes(),
                        LowCardKey::Bytes(bytes) => bytes.clone(),
                    };
                    grouped.entry(raw).or_default().extend(reader.lookup(key));
                }
            }
            if grouped.is_empty() {
                continue;
            }
            let entries: Vec<(LowCardKey, Vec<u32>)> = grouped
                .into_iter()
                .filter_map(|(raw, mut offsets)| {
                    let key = if kind == KeyKind::Bytes {
                        LowCardKey::Bytes(raw)
                    } else {
                        LowCardKey::Fixed(Key::from_ordered_bytes(kind, &raw)?)
                    };
                    offsets.sort_unstable();
                    offsets.dedup();
                    Some((key, offsets))
                })
                .collect();
            let path = dir.join(format!("seg_{segment}_merged{}.idx", entries.len()));
            if write_lowcard_segment_typed(&path, &entries).is_err() {
                continue;
            }
            for old in &paths {
                let _ = std::fs::remove_file(old);
            }
            merged = true;
        }
        if merged {
            self.reload_secondary_low_chunks(table, column, kind);
        }
        Ok(merged)
    }

    /// Re-reads a low-cardinality column's spill files into the registry.
    fn reload_secondary_low_chunks(&self, table: &str, column: &str, kind: KeyKind) {
        use storage::secondary_lowcard::LowCardReader;
        let dir = self
            .dir
            .join("secondary_low")
            .join(index_key(table, column).replace('/', "_"));
        let mut readers: SecondaryLowChunks = Vec::new();
        let mut meta: std::collections::BTreeMap<u64, usize> = Default::default();
        if let Ok(files) = std::fs::read_dir(&dir) {
            for entry in files.flatten() {
                let path = entry.path();
                if path.extension().is_none_or(|e| e != "idx") {
                    continue;
                }
                let Some(segment) = Self::secondary_spill_segment(&path) else {
                    continue;
                };
                if let Ok(reader) = LowCardReader::open_with_kind(&path, kind) {
                    *meta.entry(segment).or_default() += reader.len();
                    readers.push((segment << 32, Arc::new(reader)));
                }
            }
        }
        let tkey = self.table_key(table);
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(tkey).or_default();
        entry.secondary.low.insert(column.to_string(), readers);
        entry.secondary.chunk_meta.insert(
            column.to_string(),
            meta.into_iter()
                .map(|(segment, rows)| (segment << 32, rows))
                .collect(),
        );
    }

    pub(crate) fn index_lookup(
        &self,
        table: &str,
        column: &str,
        value: &Literal,
    ) -> Option<Vec<u64>> {
        if let Some(ids) = self.index_lookup_one(table, column, value) {
            return Some(ids);
        }
        // Same fallback as the range read: a functional index's catalog column
        // is its expression, so an equality on the base column has to be probed
        // against every function over it (the residual predicate proves
        // membership).
        let expressions = self.functional_over(table, column);
        if expressions.is_empty() {
            return None;
        }
        let mut out: Vec<u64> = Vec::new();
        let mut answered = false;
        for expression in expressions {
            if let Some(mut ids) = self.index_lookup_one(table, &expression, value) {
                answered = true;
                out.append(&mut ids);
            }
        }
        answered.then(|| {
            out.sort_unstable();
            out.dedup();
            out
        })
    }

    /// One column's (or one functional expression's) rows whose key is `value`.
    fn index_lookup_one(&self, table: &str, column: &str, value: &Literal) -> Option<Vec<u64>> {
        let tkey = self.table_key(table);
        let want = value.to_slt_string();
        // Snapshot the readers (cheap Arc clones) and the memory tail so the
        // table lock is not held across the row lookup. The tail's declared
        // type comes from the registry rather than the catalog: it is the one
        // the writer encoded the keys with.
        let (disk, low, tail_index, declared, sealing): (
            Option<SecondaryDiskChunks>,
            Option<SecondaryLowChunks>,
            Option<Arc<SecondaryIndex>>,
            Option<String>,
            Vec<Arc<index::SealedTail>>,
        ) = {
            let tables = self.tables.lock().unwrap();
            let reg = tables.get(&tkey).map(|e| &e.secondary)?;
            let known = reg.indexes.contains_key(column)
                || reg.disk.contains_key(column)
                || reg.low.contains_key(column)
                || reg.sealing.contains_key(column);
            if !known {
                return None;
            }
            (
                reg.disk.get(column).cloned(),
                reg.low.get(column).cloned(),
                reg.indexes.get(column).cloned(),
                reg.declared_types.get(column).cloned(),
                reg.sealing
                    .get(column)
                    .map(|tails| tails.iter().map(Arc::clone).collect())
                    .unwrap_or_default(),
            )
        };
        // The tail holds the column's ordered key encoding, so the probe is the
        // same `text_to_key` the writer used, not the literal's text. A tail a
        // dump has just frozen is still authoritative for the rows its chunk
        // does not cover yet.
        let tail: Vec<u64> = {
            let mut out = Vec::new();
            if let Some(declared) = &declared {
                if let Some(key) = text_to_key(declared, &want) {
                    let bytes = key.ordered_bytes();
                    if let Some(index) = &tail_index {
                        out.extend(index.lookup(&bytes));
                    }
                    for frozen in &sealing {
                        out.extend(frozen.lookup(&bytes));
                    }
                }
            }
            out
        };
        let schema = self.table_schema(table).unwrap_or_default();
        let col = schema.iter().position(|c| c == column);
        // Candidate ids from the sealed chunks and the memory tail. The
        // liveness/`want` re-check runs afterwards, over one pass of row
        // texts: asking for a row inside these loops built a block cursor and
        // decoded its block per id, which made a lookup over half a table
        // quadratic.
        let mut candidates: Vec<u64> = Vec::new();
        let mut seen: std::collections::HashSet<u64> = Default::default();
        // Sealed chunks are authoritative for the rows they cover; the memory
        // map only holds the tail written since the last dump.
        //
        // A low-cardinality column - BOOL, JSON's term index, or any column
        // created `WITH (cardinality='low')` - lives in bitmap chunks keyed by
        // the column's own ordered encoding, so the probe is the same
        // `text_to_key` the writer used. (A JSON term is not a document value, so
        // only its ranges are meaningful and those go through
        // `low_card_term_range`.)
        if let Some(segments) = &low {
            // As below: the registry's declared type is authoritative.
            let column_type = declared.clone().unwrap_or_else(|| {
                col.and_then(|ci| {
                    self.column_types(table)
                        .ok()
                        .and_then(|t| t.get(ci).cloned())
                })
                .unwrap_or_default()
            });
            if !column_type.trim().to_ascii_uppercase().starts_with("JSON") {
                if let Some(probe) = text_to_key(&column_type, &want)
                    .map(storage::secondary_lowcard::LowCardKey::Fixed)
                {
                    for (base, reader) in segments {
                        for off in reader.lookup(&probe) {
                            let id = base | u64::from(off);
                            if seen.insert(id) {
                                candidates.push(id);
                            }
                        }
                    }
                }
            }
        }
        if let Literal::Int(v) = value {
            // The registry's declared type is authoritative: the catalog lookup
            // is keyed by a qualified table name, so an unqualified `table`
            // would classify the key as a `u64` and miss every `i32` chunk.
            let ty = declared.clone().unwrap_or_else(|| {
                self.column_types(table)
                    .ok()
                    .and_then(|types| col.and_then(|c| types.get(c).cloned()))
                    .unwrap_or_default()
            });
            let ordered = integer_secondary_key(&ty, *v);
            if let Some(segments) = &disk {
                for (base, reader) in segments {
                    for off in reader.lookup(ordered) {
                        let id = base | u64::from(off);
                        if seen.insert(id) {
                            candidates.push(id);
                        }
                    }
                }
            }
        } else if let Literal::Float(f) = value {
            // FLOAT/DOUBLE chunks keep the raw key, and each reader knows its
            // width (`SecondarySegmentReader::kind`).
            if let Some(segments) = &disk {
                for (base, reader) in segments {
                    let key = match reader.kind() {
                        KeyKind::F32 => Key::F32(*f as f32),
                        KeyKind::F64 => Key::F64(*f),
                        // Neither a hashed varchar nor a date/time chunk can
                        // hold a float key; the narrow integers and the
                        // low-cardinality kinds cannot either.
                        KeyKind::U64
                        | KeyKind::I32
                        | KeyKind::I64
                        | KeyKind::I8
                        | KeyKind::I16
                        | KeyKind::Bool
                        | KeyKind::Bytes => continue,
                    };
                    for off in reader.lookup(key) {
                        let id = base | u64::from(off);
                        if seen.insert(id) {
                            candidates.push(id);
                        }
                    }
                }
            }
        } else if let Literal::Str(_) = value {
            {
                // VARCHAR/JSON chunks store the reference's u64 key hash; a
                // DATE/TIME chunk stores the ordered day/second count, so the
                // literal text is converted the same way the writer did.
                // The declared type the registry holds is the one the writer
                // encoded with; the catalog lookup is only a fallback (it is
                // keyed by a qualified table name, so it misses a bare one).
                let column_type = declared.clone().unwrap_or_else(|| {
                    col.and_then(|ci| {
                        self.column_types(table)
                            .ok()
                            .and_then(|t| t.get(ci).cloned())
                    })
                    .unwrap_or_default()
                });
                let hashed = Key::U64(storage::secondary_segment::hash_varchar(&want));
                let temporal = temporal_key(&column_type, &want);
                if let Some(segments) = &disk {
                    for (base, reader) in segments {
                        let key = match (reader.kind(), key_domain(&column_type)) {
                            (KeyKind::I32, KeyDomain::Temporal) => match temporal {
                                Some(v) => Key::I32(v as i32),
                                None => continue,
                            },
                            (KeyKind::I64, KeyDomain::Temporal) => match temporal {
                                Some(v) => Key::I64(v),
                                None => continue,
                            },
                            // An integer column stores the value itself.
                            (KeyKind::I32, KeyDomain::Integer) => {
                                match want.trim().parse::<i64>() {
                                    Ok(v) => Key::I32(v as i32),
                                    Err(_) => continue,
                                }
                            }
                            (KeyKind::I64, KeyDomain::Integer) => {
                                match want.trim().parse::<i64>() {
                                    Ok(v) => Key::I64(v),
                                    Err(_) => continue,
                                }
                            }
                            _ => hashed,
                        };
                        for off in reader.lookup(key) {
                            let id = base | u64::from(off);
                            if seen.insert(id) {
                                candidates.push(id);
                            }
                        }
                    }
                }
            }
        }
        // The tail: rows inserted after the last dump.
        for id in tail {
            if seen.insert(id) {
                candidates.push(id);
            }
        }
        // The candidates are not re-checked against the stored rows here: both
        // callers apply the predicate to the rows they fetch (the scan's
        // `IndexScan` evaluator and the columnar select's filter), and a stale
        // id - a delete the index has not seen - simply fetches no row. Doing
        // it here decoded one row per candidate only to throw it away, which
        // made an indexed lookup cost the same as scanning the rows it named.
        candidates.sort_unstable();
        Some(candidates)
    }

    /// The row ids of a range predicate the planner already pushed down: the
    /// `Secondary` evaluator carries the merged intervals, so nothing is parsed
    /// here. `None` means "not answerable from the index", and the caller keeps
    /// the full scan.
    pub(crate) fn index_scan_range_rows(
        &self,
        table: &str,
        evaluator: &planner::index_scan::IndexFilterEvaluator,
        scan_columns: &[String],
    ) -> Option<Vec<u64>> {
        let planner::index_scan::IndexFilterEvaluator::Secondary(secondary) = evaluator else {
            return None;
        };
        if secondary.json || secondary.ranges.is_empty() {
            return None;
        }
        // A single-point range is `col = v`. Deliberately left to
        // `index_lookup` above: that path predates the range read and stays
        // authoritative for equality, so the two cannot diverge. Deleting this
        // branch makes `=` take the range path, which is a behaviour change to
        // make on purpose, not by accident.
        if secondary.ranges.len() == 1 {
            let (lo, hi) = &secondary.ranges[0];
            if lo == hi {
                return None;
            }
        }
        let column = scan_columns.get(secondary.column_id)?;
        self.index_range_lookup(table, column, &secondary.ranges)
    }

    /// Rows whose indexed key falls in one of `ranges` (inclusive bounds,
    /// already merged by `FilterExpressionPushDown`).
    ///
    /// `INT`/`UINT` keys are the value itself, `FLOAT`/`DOUBLE` the raw value
    /// and `DATE`/`TIME`/`DATETIME`/`TIMESTAMP` the reference's ordered day (or
    /// day+second) count (`KeyKind`, matching `ConvertToOrderedType`), so all of
    /// them can be range-scanned. A `VARCHAR` key is a hash, so only an
    /// equality range is answerable; a JSON key is a term, answered by
    /// [`Self::low_card_term_range`]. Anything else returns `None`, and the
    /// caller keeps the full scan.
    pub(crate) fn index_range_lookup(
        &self,
        table: &str,
        column: &str,
        ranges: &[(Value, Value)],
    ) -> Option<Vec<u64>> {
        if let Some(ids) = self.index_range_lookup_one(table, column, ranges) {
            return Some(ids);
        }
        // A functional index's catalog column is its expression, so the
        // planner's `column_id` (the base column) does not name it. Its ranges
        // are already the function's results, so every functional index over the
        // column is a candidate; the residual predicate proves membership, which
        // keeps the union safe even when two functions share one column.
        let expressions = self.functional_over(table, column);
        if expressions.is_empty() {
            return None;
        }
        let mut out: Vec<u64> = Vec::new();
        let mut answered = false;
        for expression in expressions {
            if let Some(mut ids) = self.index_range_lookup_one(table, &expression, ranges) {
                answered = true;
                out.append(&mut ids);
            }
        }
        answered.then(|| {
            out.sort_unstable();
            out.dedup();
            out
        })
    }

    /// The functional indexes whose base column is `column`.
    fn functional_over(&self, table: &str, column: &str) -> Vec<String> {
        let tkey = self.table_key(table);
        let tables = self.tables.lock().unwrap();
        tables
            .get(&tkey)
            .map(|entry| {
                entry
                    .secondary
                    .functional
                    .iter()
                    .filter(|(_, function)| function.base == column)
                    .map(|(expression, _)| expression.clone())
                    .collect()
            })
            .unwrap_or_default()
    }

    /// One column's (or one functional expression's) rows in `ranges`.
    fn index_range_lookup_one(
        &self,
        table: &str,
        column: &str,
        ranges: &[(Value, Value)],
    ) -> Option<Vec<u64>> {
        let tkey = self.table_key(table);
        let schema = self.table_schema(table).unwrap_or_default();
        let col = schema.iter().position(|c| c == column);
        // Same classification as the chunk writer. The registry's declared type
        // is authoritative — it is what the writer encoded the keys with — and
        // the catalog lookup is keyed by a qualified table name, so it can miss
        // an unqualified one.
        let declared = {
            let tables = self.tables.lock().unwrap();
            tables
                .get(&tkey)
                .and_then(|e| e.secondary.declared_types.get(column).cloned())
        };
        let column_type = declared
            .or_else(|| {
                col.and_then(|ci| {
                    self.column_types(table)
                        .ok()
                        .and_then(|types| types.get(ci).cloned())
                })
            })
            .map(|ty| ty.trim().to_ascii_uppercase())
            .unwrap_or_default();
        if column_type.is_empty() {
            return None;
        }
        let kind = secondary_key_kind(&column_type);
        if kind == KeyKind::Bytes {
            // A JSON index is low-cardinality and keyed by term, so each bound is
            // the term text itself (the planner's `$.a:i:…` range).
            return self.low_card_term_range(&tkey, column, ranges);
        }
        // `ConvertToHashU64`: a hashed key has no order to scan, so only an
        // *equality* range has an exact key. The reference still resolves it
        // through the chunk's lower/upper_bound pair
        // (`index_filter_evaluators_impl.cpp:770-772`), and a `<>` range is left
        // to the residual filter.
        let hashed = kind == KeyKind::U64
            && !(column_type.starts_with("INT") || column_type.starts_with("UINT"));
        // Bounds: `Value::Int(i64::MIN/MAX)` for the planner's open ends,
        // `Value::Float` for a float column and the literal's text for a
        // date/time one. `bound_key` puts each in the chunk's own domain.
        let keys: Vec<(Key, Key)> = if hashed {
            let mut out = Vec::new();
            for (lo, hi) in ranges {
                let (lo_text, hi_text) = (lo.to_slt_string(), hi.to_slt_string());
                if lo_text != hi_text {
                    return None;
                }
                let key = Key::U64(storage::secondary_segment::hash_varchar(&lo_text));
                out.push((key, key));
            }
            out
        } else {
            ranges
                .iter()
                .filter_map(|(lo, hi)| {
                    let lo = bound_key(kind, &column_type, lo)?;
                    let hi = bound_key(kind, &column_type, hi)?;
                    (lo.order(&hi) != std::cmp::Ordering::Greater).then_some((lo, hi))
                })
                .collect()
        };
        if keys.is_empty() {
            return None;
        }
        // Snapshot the sealed readers and the memory tail (cheap `Arc` clones),
        // so the key lookups run without the table lock.
        let (disk, low, tail, sealing): (
            Option<SecondaryDiskChunks>,
            Option<SecondaryLowChunks>,
            Option<Arc<SecondaryIndex>>,
            Vec<Arc<index::SealedTail>>,
        ) = {
            let tables = self.tables.lock().unwrap();
            let reg = tables.get(&tkey).map(|e| &e.secondary)?;
            let known = reg.indexes.contains_key(column)
                || reg.disk.contains_key(column)
                || reg.low.contains_key(column)
                || reg.sealing.contains_key(column);
            if !known {
                return None;
            }
            (
                reg.disk.get(column).cloned(),
                reg.low.get(column).cloned(),
                reg.indexes.get(column).cloned(),
                reg.sealing
                    .get(column)
                    .map(|tails| tails.iter().map(Arc::clone).collect())
                    .unwrap_or_default(),
            )
        };
        let mut candidates: Vec<u64> = Vec::new();
        if let Some(segments) = &disk {
            for (base, reader) in segments {
                for (start, end) in &keys {
                    for off in reader.range(*start, *end) {
                        candidates.push(base | u64::from(off));
                    }
                }
            }
        }
        // A `cardinality='low'` column lives in bitmap chunks; its keys are the
        // same ordered encoding, so the same bounds apply.
        if let Some(segments) = &low {
            for (base, reader) in segments {
                for (start, end) in &keys {
                    for off in reader.range(
                        storage::secondary_lowcard::LowCardKey::Fixed(*start),
                        storage::secondary_lowcard::LowCardKey::Fixed(*end),
                    ) {
                        candidates.push(base | u64::from(off));
                    }
                }
            }
        }
        // The tail carries the same ordered encoding as the chunks, so a bound
        // applies to it directly: one ordered slice per range, with no key
        // decoded and compared.
        for (start, end) in &keys {
            let lo = tail_bound(kind, *start);
            let hi = tail_bound(kind, *end);
            if let Some(index) = &tail {
                for (_, ids) in index.range(Some(&lo), Some(&hi)) {
                    candidates.extend_from_slice(&ids);
                }
            }
            // A frozen tail is still authoritative until its chunk is
            // registered.
            for frozen in &sealing {
                for (_, ids) in frozen.range(Some(&lo), Some(&hi)) {
                    candidates.extend_from_slice(&ids);
                }
            }
        }
        // No per-candidate row decode here, for the same reason as
        // `index_lookup`: both callers re-apply the range to the rows they
        // fetch, and a stale id contributes no row. Dedupe by sorting instead
        // of hashing every id: a 1000-row range spent 11% of the query in
        // SipHash.
        candidates.sort_unstable();
        candidates.dedup();
        Some(candidates)
    }

    /// Term ranges over a JSON column's low-cardinality chunks.
    ///
    /// `flatten_json_terms` keeps the reference's `path:type:value` shape
    /// (`json_flattener_impl.cpp`), so a term range is a byte range over the
    /// chunk's sorted keys and the same range applies to the memory tail.
    fn low_card_term_range(
        &self,
        tkey: &str,
        column: &str,
        ranges: &[(Value, Value)],
    ) -> Option<Vec<u64>> {
        use storage::secondary_lowcard::LowCardKey;
        let terms: Vec<(Vec<u8>, Vec<u8>)> = ranges
            .iter()
            .filter_map(|(lo, hi)| match (lo, hi) {
                (Value::Str(lo), Value::Str(hi)) if lo.as_bytes() <= hi.as_bytes() => {
                    Some((lo.clone().into_bytes(), hi.clone().into_bytes()))
                }
                _ => None,
            })
            .collect();
        if terms.is_empty() {
            return None;
        }
        let (low, tail, sealing): (
            Option<SecondaryLowChunks>,
            Option<Arc<SecondaryIndex>>,
            Vec<Arc<index::SealedTail>>,
        ) = {
            let tables = self.tables.lock().unwrap();
            let reg = tables.get(tkey).map(|e| &e.secondary)?;
            (
                reg.low.get(column).cloned(),
                reg.indexes.get(column).cloned(),
                reg.sealing
                    .get(column)
                    .map(|tails| tails.iter().map(Arc::clone).collect())
                    .unwrap_or_default(),
            )
        };
        let mut candidates: Vec<u64> = Vec::new();
        if let Some(segments) = &low {
            for (base, reader) in segments {
                for (lo, hi) in &terms {
                    for off in
                        reader.range(LowCardKey::Bytes(lo.clone()), LowCardKey::Bytes(hi.clone()))
                    {
                        candidates.push(base | u64::from(off));
                    }
                }
            }
        }
        for (lo, hi) in &terms {
            if let Some(index) = &tail {
                for (_, ids) in index.range(Some(lo), Some(hi)) {
                    candidates.extend_from_slice(&ids);
                }
            }
            for frozen in &sealing {
                for (_, ids) in frozen.range(Some(lo), Some(hi)) {
                    candidates.extend_from_slice(&ids);
                }
            }
        }
        candidates.sort_unstable();
        candidates.dedup();
        Some(candidates)
    }

    /// The index catalog of one scan: the columns the pruned plan reads, their
    /// declared types, and which of them carry a secondary index (C++
    /// `IndexScanBuilder` reads `new_candidate_column_index_map_`).
    pub(crate) fn index_catalog(
        &self,
        table: &str,
        columns: &[String],
    ) -> planner::index_scan::IndexCatalog {
        use planner::index_scan::{ColumnKind, IndexCatalog, SecondaryIndexInfo};
        let key = self.table_key(table);
        let meta = self.storage.catalog.get_table_by_name(&key).ok().flatten();
        let kinds: Vec<ColumnKind> = columns
            .iter()
            .map(|column| {
                let declared = meta.as_ref().and_then(|meta| {
                    meta.columns
                        .iter()
                        .position(|name| name == column)
                        .and_then(|i| meta.types.get(i))
                });
                column_kind_of(declared.map(String::as_str).unwrap_or(""))
            })
            .collect();
        let mut catalog = IndexCatalog {
            column_names: columns.to_vec(),
            column_kinds: kinds.clone(),
            ..Default::default()
        };
        let tables = self.tables.lock().unwrap();
        if let Some(entry) = tables.get(&key) {
            for (i, column) in columns.iter().enumerate() {
                let indexed = entry.secondary.indexes.contains_key(column)
                    || entry.secondary.disk.contains_key(column)
                    || entry.secondary.low.contains_key(column);
                if indexed {
                    catalog.secondary.insert(
                        column.clone(),
                        SecondaryIndexInfo {
                            column_id: i,
                            // A JSON index only exists as a term chunk; the
                            // planner needs the flag to treat `json_*`
                            // functions as its predicates
                            // (`index_scan.rs:831`).
                            json: kinds[i] == ColumnKind::Json,
                            kind: kinds[i],
                        },
                    );
                }
            }
            // A functional index answers `func(col)` predicates: the planner
            // keys it by `ExtractFunctionInfo`'s spelling, `func(c<column id>)`
            // (`index_scan.rs:143`), and reads the base column from the record.
            for (expression, function) in &entry.secondary.functional {
                let Some(pos) = columns.iter().position(|c| c == &function.base) else {
                    continue;
                };
                catalog.functional.insert(
                    format!("{}(c{})", function.func, pos),
                    function.base.clone(),
                );
                catalog.secondary.insert(
                    function.base.clone(),
                    SecondaryIndexInfo {
                        column_id: pos,
                        json: false,
                        kind: column_kind_of(&function.returns),
                    },
                );
                let _ = expression;
            }
        }
        catalog
    }

    pub(crate) fn index_name_pairs(&self, table: &str) -> Result<Vec<(String, String)>> {
        let short = table.split('.').next_back().unwrap_or(table);
        Ok(self
            .storage
            .catalog
            .list_indexes()?
            .into_iter()
            .filter(|i| i.table == short || i.table == table)
            .map(|i| (i.column, i.name))
            .collect())
    }

    /// Dumps the sparse memory tail of one column.
    pub(crate) fn seal_sparse_index(&self, table: &str, column: &str) {
        let key = self.table_key(table);
        let shared = self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|e| e.sparse.get(column).cloned());
        if let Some(shared) = shared {
            shared.write().unwrap().seal();
        }
    }

    /// Dumps the multi-vector memory tail of one column.
    pub(crate) fn seal_multivector_index(&self, table: &str, column: &str) {
        let key = self.table_key(table);
        let shared = self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|e| e.mv.get(column).cloned());
        if let Some(shared) = shared {
            shared.write().unwrap().seal();
        }
    }

    /// The comment recorded for `index` on `table`, if any.
    pub fn index_comment(&self, table: &str, index: &str) -> Option<String> {
        let key = self.table_key(table);
        self.tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|e| e.comments.get(index).cloned())
    }

    /// Every file under an index's C++ mirror directory
    /// (C++ `NewTxn::GetTableIndexFilePaths`).
    pub fn index_files(&self, table: &str, index: &str) -> Vec<PathBuf> {
        let table = self.table_key(table);
        let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&table) else {
            return Vec::new();
        };
        let index_id = self
            .storage
            .catalog
            .list_indexes()
            .unwrap_or_default()
            .into_iter()
            .find(|im| im.table == table && im.name == index)
            .map(|im| im.id);
        let Some(index_id) = index_id else {
            return Vec::new();
        };
        let dir = self.blocks_root().join(format!(
            "db_{}/tbl_{}/idx_{index_id}",
            self.database_id_of(&table),
            meta.id
        ));
        files_under(&dir)
    }

    /// The tokeniser delimiters implied by a column's full-text analyzer:
    /// `keyword`/`whitespace` split on whitespace (`Some([])`), their
    /// `-<chars>` form splits on the listed characters, and any other analyzer
    /// uses the standard tokeniser (`None`).
    pub(crate) fn index_delims(&self, table: &str, column: &str) -> Option<Vec<char>> {
        let name = self.column_analyzer_name(table, column)?;
        // C++ `WhitespaceAnalyzer`: the delimiter set is everything after the
        // first '-' of a `keyword`/`whitespace` name, verbatim (so `-A` is the
        // uppercase byte), and empty means "split on whitespace".
        match analyzer::parse_analyzer_name(&name) {
            Ok(analyzer::AnalyzerName::Whitespace { delimiters }) => {
                Some(delimiters.chars().collect())
            }
            _ => None,
        }
    }

    /// The analyzer a query written against `column` is tokenised with.
    ///
    /// C++ `SearchDriver::GetAnalyzerName`: a query that names no `@index`
    /// walks the column's indexes in name order and takes the first analyzer
    /// that is *not* a sparse-gram one, so a regular analyzer wins whenever the
    /// column has both; only a column whose indexes are all sparse-gram falls
    /// back to one of those.
    pub(crate) fn column_analyzer_name(&self, table: &str, column: &str) -> Option<String> {
        let key = self.table_key(table);
        let guard = self.tables.lock().unwrap();
        let entry = guard.get(&key)?;
        if let Some(by_index) = entry.column_index_analyzers.get(column) {
            let mut sparsegram: Option<String> = None;
            for analyzer_name in by_index.values() {
                let is_sparsegram = matches!(
                    analyzer::parse_analyzer_name(analyzer_name),
                    Ok(analyzer::AnalyzerName::SparseGram { .. })
                );
                if !is_sparsegram {
                    return Some(analyzer_name.clone());
                }
                sparsegram.get_or_insert_with(|| analyzer_name.clone());
            }
            if sparsegram.is_some() {
                return sparsegram;
            }
        }
        entry.analyzers.get(column).cloned()
    }

    /// The tokeniser a column's full-text index uses: the delimiters
    /// `index_delims` derives plus the analyzer the index was created with.
    /// C++: `AnalyzerPool::GetAnalyzer` handed to the column's inverter.
    pub(crate) fn index_tokenizer(&self, table: &str, column: &str) -> TextTokenizer {
        let mut tokenizer = TextTokenizer::new(
            self.index_delims(table, column),
            self.index_analyzer(table, column),
        );
        tokenizer.stemmer = self.index_stemmer(table, column);
        tokenizer
    }

    /// The stemmer of a `standard-<language>` column, when the suffix selects
    /// something other than the English default (which `standard` itself and
    /// every unknown suffix keep, like the reference's if-chain).
    pub(crate) fn index_stemmer(&self, table: &str, column: &str) -> Option<analyzer::Stemmer> {
        let name = self.column_analyzer_name(table, column)?;
        let analyzer::AnalyzerName::Standard { language } =
            analyzer::parse_analyzer_name(&name).ok()?
        else {
            return None;
        };
        match analyzer::standard_stemmer(language) {
            analyzer::Stemmer::Snowball(rust_stemmers::Algorithm::English) => None,
            stemmer => Some(stemmer),
        }
    }

    /// The analyzer name a column was indexed with, when it is a family the
    /// delimiter split does not reproduce (`ik`, `rag-chinese`, `ngram`,
    /// `japanese`, …). `standard` *is* the default path and `keyword` /
    /// `whitespace` are carried by the delimiters.
    pub(crate) fn index_analyzer(&self, table: &str, column: &str) -> Option<String> {
        let name = self.column_analyzer_name(table, column)?;
        match analyzer::parse_analyzer_name(&name) {
            Ok(analyzer::AnalyzerName::Standard { .. })
            | Ok(analyzer::AnalyzerName::Whitespace { .. })
            | Err(_) => None,
            Ok(_) => Some(name),
        }
    }

    /// `WalCmdDumpIndexV2` per index over `segment`: the chunk the dump that
    /// runs inside the import/compaction transaction produced
    /// (`ImportTxnStore::ToWalEntry`, `base_txn_store_impl.cpp:387`).
    ///
    /// The reference fills the chunk info from `ChunkIndexMetaInfo`, which only
    /// a full-text dump populates (`base_name` = the chunk's `ft_<base>` and
    /// `term_count`); the other kinds leave it empty. `index_size` is 0 in both.
    /// `table_key` is the table's catalog key, and empty for a compaction
    /// (`CompactTxnStore` never sets it).
    pub(crate) fn dump_index_commands(
        &self,
        table: &str,
        segment: u32,
        rows: usize,
        table_key: &str,
    ) -> Result<Vec<storage::WalCmd>> {
        let meta = self
            .storage
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let mut indexes = self
            .storage
            .catalog
            .list_indexes()?
            .into_iter()
            .filter(|im| im.table == table || im.table == self.qualified_table(table))
            .collect::<Vec<_>>();
        indexes.sort_by_key(|im| im.id);
        let (db_name, table_name) = split_table_name(table);
        let db_id = self.database_id_of(table).to_string();
        let table_id = meta.id.to_string();
        let base_row_id = ((segment as u64) << 32) as usize;
        let mut out = Vec::with_capacity(indexes.len());
        for im in indexes {
            let fulltext = im.index_type.eq_ignore_ascii_case("FULLTEXT");
            let (base_name, term_count) = if fulltext {
                (
                    format!("ft_{base_row_id:016x}"),
                    self.text_term_count(table, &im.column),
                )
            } else {
                (String::new(), 0)
            };
            out.push(storage::WalCmd::DumpIndex {
                db_name: db_name.clone(),
                db_id: db_id.clone(),
                table_name: table_name.clone(),
                table_id: table_id.clone(),
                index_name: im.name.clone(),
                index_id: im.id.to_string(),
                segment_id: segment,
                chunk_infos: vec![storage::wal_entry::WalChunkIndexInfo {
                    chunk_id: 0,
                    base_name,
                    base_row_id: storage::wal_entry::WalRowId::new(segment, 0),
                    row_count: rows as u32,
                    term_count,
                    index_size: 0,
                    deprecate_ts: u64::MAX,
                }],
                deprecate_ids: Vec::new(),
                table_key: table_key.to_string(),
            });
        }
        Ok(out)
    }

    /// Dumps every in-memory index (the body of the reference's dump task).
    ///
    /// The reference dumps each index inside a transaction of its own
    /// (`DumpIndexProcessor::DoDump` -> `kDumpMemIndex`), so the chunk it
    /// seals names that transaction's commit ts and rides in its WAL entry.
    /// Each dump seals its own tail ([`Self::seal_index_tail`]) under
    /// `flush_lock`, and an index with an empty tail is skipped without
    /// spending a timestamp.
    pub(crate) fn dump_all_memory_indexes(&self) {
        let indexes = self.storage.catalog.list_indexes().unwrap_or_default();
        for im in indexes {
            let _ = self.dump_index_transaction(&im.table, &im.name);
        }
        self.seal_orphan_memory_tails();
    }

    /// Seals the memory tails the catalog has no index record for.
    ///
    /// A plain `MULTIVECTOR` or `SPARSE` column gets an in-memory index for
    /// queries (`multivector_index_for` / `sparse_index_for`) without a
    /// `CREATE INDEX`, so no `DumpMemIndex` transaction can name it. Sealing
    /// them here keeps their footprint bounded; the checkpoint publishes the
    /// files with the rest of the mirror.
    fn seal_orphan_memory_tails(&self) {
        let known: std::collections::HashSet<(String, String)> = self
            .storage
            .catalog
            .list_indexes()
            .unwrap_or_default()
            .into_iter()
            .map(|im| (self.table_key(&im.table), im.column))
            .collect();
        let mut text = Vec::new();
        let mut sparse = Vec::new();
        let mut mv = Vec::new();
        {
            let tables = self.tables.lock().unwrap();
            for (table, entry) in tables.iter() {
                for column in entry.text.keys() {
                    if !known.contains(&(table.clone(), column.clone())) {
                        text.push((table.clone(), column.clone()));
                    }
                }
                for column in entry.sparse.keys() {
                    if !known.contains(&(table.clone(), column.clone())) {
                        sparse.push((table.clone(), column.clone()));
                    }
                }
                for column in entry.mv.keys() {
                    if !known.contains(&(table.clone(), column.clone())) {
                        mv.push((table.clone(), column.clone()));
                    }
                }
            }
        }
        for (table, column) in text {
            self.seal_text_index(&table, &column);
        }
        for (table, column) in sparse {
            self.seal_sparse_index(&table, &column);
        }
        for (table, column) in mv {
            self.seal_multivector_index(&table, &column);
        }
    }

    pub(crate) fn memindex_capacity(&self) -> usize {
        self.context
            .memindex_capacity
            .load(std::sync::atomic::Ordering::Relaxed)
    }

    /// C++ `AppendMemIndex`: after a write batch, dump every HNSW memory
    /// segment that has reached `mem_index_capacity` rows. This is the row
    /// bound the reference uses to keep an index's memory footprint constant
    /// on a long import, independently of the byte quota.
    pub(crate) fn dump_hnsw_over_capacity(&self, table: &str) {
        let capacity = self.memindex_capacity();
        if capacity == 0 {
            return;
        }
        let tkey = self.table_key(table);
        let full: Vec<String> = {
            let tables = self.tables.lock().unwrap();
            tables
                .get(&tkey)
                .map(|e| {
                    e.hnsw_mem
                        .iter()
                        .filter(|(_, m)| m.lock().map(|g| g.len() >= capacity).unwrap_or(false))
                        .map(|(c, _)| c.clone())
                        .collect()
                })
                .unwrap_or_default()
        };
        if full.is_empty() {
            return;
        }
        let Ok(indexes) = self.storage.catalog.list_indexes() else {
            return;
        };
        for im in indexes {
            if !im.index_type.eq_ignore_ascii_case("HNSW") || !full.contains(&im.column) {
                continue;
            }
            let same_table = im.table == table
                || (im.table.rsplit('.').next() == table.rsplit('.').next()
                    && (!im.table.contains('.') || !table.contains('.')));
            if !same_table {
                continue;
            }
            if self.dump_hnsw_index(&im.table, &im.name).is_ok() {
                let mut tables = self.tables.lock().unwrap();
                if let Some(e) = tables.get_mut(&tkey) {
                    e.hnsw_mem.remove(&im.column);
                    e.index_pending_bytes.insert(im.column.clone(), 0);
                    e.hnsw_probed = false;
                }
            }
        }
    }

    /// C++ `AppendMemIndex`: after a write batch, dump every secondary memory
    /// tail that reached `mem_index_capacity` rows. This is the row bound the
    /// reference applies at the end of the batch (the sibling of
    /// [`Self::dump_hnsw_over_capacity`]).
    pub(crate) fn dump_secondary_over_capacity(&self, table: &str) {
        let capacity = self.memindex_capacity();
        if capacity == 0 {
            return;
        }
        let tkey = self.table_key(table);
        let full = {
            let tables = self.tables.lock().unwrap();
            tables
                .get(&tkey)
                .map(|entry| {
                    entry
                        .secondary
                        .indexes
                        .values()
                        .any(|index| index.row_count() >= capacity)
                })
                .unwrap_or(false)
        };
        if full {
            self.seal_secondary_indexes();
        }
    }
}

/// The row ranges an index's sealed chunks cover, sorted and merged into
/// disjoint ascending intervals.
///
/// C++ `ComputeMemIndexGapRanges` merges the chunk ranges the same way before
/// it compares them with the blocks; a chunk's range is
/// `[base_row_id, base_row_id + row_count)`.
fn merge_covered_ranges(chunks: &[catalog::layout::SegmentIndex]) -> Vec<(u64, u64)> {
    let mut ranges: Vec<(u64, u64)> = chunks
        .iter()
        .flat_map(|(_, chunks)| {
            chunks
                .iter()
                .map(|(_, info)| (info.base_row_id, info.base_row_id + info.row_count))
        })
        .filter(|(begin, end)| end > begin)
        .collect();
    ranges.sort_unstable();
    let mut merged: Vec<(u64, u64)> = Vec::with_capacity(ranges.len());
    for (begin, end) in ranges {
        match merged.last_mut() {
            Some(last) if begin <= last.1 => last.1 = last.1.max(end),
            _ => merged.push((begin, end)),
        }
    }
    merged
}
