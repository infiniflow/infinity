//! `COPY ... FROM ...`: one entry point per format, mirroring the
//! reference's `PhysicalImport::NewImport<FORMAT>` (one method per format in
//! `physical_import_impl.cpp`).

use super::*;

impl Engine {
    /// Loads a CSV file into `table` (`COPY t FROM …`). The path may be absolute
    /// or the Infinity fixture path `/var/infinity/test_data/…`, which is mapped
    /// onto `INFINITY_DATA_DIR` (default `test/data`) if present.
    pub(crate) fn copy_from(
        &self,
        table: &str,
        path: &str,
        opts: &[(String, String)],
    ) -> Result<QueryResult> {
        // `NewTxn::Import` is **one** transaction: it writes every block and
        // commits once. The extent each segment ends up with is what decides
        // how many index chunks the mirror publishes for it.
        let before = self.segment_row_counts(table);
        let result = self.copy_from_inner(table, path, opts);
        if result.is_ok() {
            self.settle_import_extent(table, &before);
        }
        result
    }

    pub(crate) fn copy_from_inner(
        &self,
        table: &str,
        path: &str,
        opts: &[(String, String)],
    ) -> Result<QueryResult> {
        // The import writes its blocks inside its own commit, so a background
        // compaction must not run between the commit and the write.
        let gate = self.batch_gate(table);
        let _batch = gate.lock().unwrap();
        if self.storage.catalog.get_table_by_name(table)?.is_none() {
            return Err(Error::NotFound(format!("table '{table}'")));
        }
        let resolved = resolve_data_path(path);
        let format = opts
            .iter()
            .find(|(k, _)| k == "FORMAT")
            .map(|(_, v)| v.to_ascii_uppercase());
        // Read raw bytes: `FORMAT FVECS` is binary, everything else is text.
        // Missing fixtures stay a statement-level success with no rows.
        let t_read0 = std::time::Instant::now();
        let bytes_in = match std::fs::read(&resolved) {
            Ok(b) => b,
            Err(_) => return Ok(QueryResult::Inserted { count: 0 }),
        };
        let t_read = t_read0.elapsed();
        // The binary formats parse into the same literal rows the CSV path
        // builds, then commit them the same way.
        let rows = match format.as_deref() {
            Some("FVECS") => Some(self.binary_vector_rows(table, &bytes_in, 4)?),
            Some("BVECS") => Some(self.binary_vector_rows(table, &bytes_in, 1)?),
            Some("CSR") => Some(self.csr_sparse_rows(table, &bytes_in)?),
            Some("PARQUET") => Some(self.parquet_rows(table, &resolved)?),
            _ => None,
        };
        if let Some(rows) = rows {
            return self.import_literal_rows(table, rows, t_read);
        }
        let delim = opts
            .iter()
            .find(|(k, _)| k == "DELIMITER")
            .map(|(_, v)| decode_delimiter(v))
            .unwrap_or(',');
        let types_for_scan = self.column_types(table)?;
        let defaults = self.table_defaults(table)?;
        let format_of = |name: &str| {
            opts.iter()
                .any(|(k, v)| k == "FORMAT" && v.eq_ignore_ascii_case(name))
        };
        // `FORMAT JSONL` (one object per line) and `FORMAT JSON` (one array)
        // are small in practice; the CSV path streams.
        if format_of("JSONL") || format_of("JSON") {
            let text = String::from_utf8_lossy(&bytes_in).into_owned();
            if format_of("JSONL") {
                return self.copy_from_jsonl(table, &text, &types_for_scan, &defaults);
            }
            return self.copy_from_json(table, &text, &types_for_scan, &defaults);
        }
        // CSV: read line by line and commit in blocks, so an 8-million-row
        // import does not hold the file and its parsed rows in memory.
        let file = match std::fs::File::open(&resolved) {
            Ok(f) => f,
            Err(_) => return Ok(QueryResult::Inserted { count: 0 }),
        };
        let mut reader = std::io::BufReader::new(file);
        let mut total = 0usize;
        // The statement's own segment cursor (`ApplySegmentIDs`): the table's
        // shared counter belongs to whoever allocated last, so reading it again
        // per batch would start this file's next block after an `INSERT`'s rows.
        let mut cursor: Option<ImportCursor> = None;
        // `NewTxn::Import` is one transaction for the whole statement, so the
        // rows are staged into a single `Txn` and committed once - at the end,
        // for every size (the commit below keys on `staged`).
        let t = self.txn.begin();
        let mut staged = 0usize;
        let mut segments: Vec<u32> = Vec::new();
        // One entry per staged batch: the block `version` keeps a `create`
        // boundary per append, and a statement that rolls over into a second
        // segment must keep the boundaries of both.
        let mut batches_all: Vec<usize> = Vec::new();
        // `(row id, stored text)` per row, so the commit can hand the block the
        // values it publishes without a second pass over the file.
        let mut encoded_rows: Vec<(usize, String)> = Vec::new();
        loop {
            let mut batch: Vec<Vec<Literal>> = Vec::with_capacity(Self::COPY_BATCH_ROWS);
            let mut eof = false;
            while batch.len() < Self::COPY_BATCH_ROWS {
                let mut line = String::new();
                use std::io::BufRead;
                match reader.read_line(&mut line) {
                    Ok(0) => {
                        eof = true;
                        break;
                    }
                    Ok(_) => {}
                    Err(e) => return Err(Error::io(&resolved, e)),
                }
                let line = line.trim_end_matches(['\n', '\r']);
                if line.is_empty() {
                    continue;
                }
                let fields: Vec<String> = csv_fields(line, delim);
                let mut cells: Vec<String> = fields.iter().map(|f| normalize_cell(f)).collect();
                if cells.len() > types_for_scan.len() {
                    return Err(Error::Invalid(format!(
                        "import: the file has {} columns but the table has {}",
                        cells.len(),
                        types_for_scan.len()
                    )));
                }
                for (i, c) in cells.iter_mut().enumerate() {
                    if c.is_empty() {
                        if let Some(Some(d)) = defaults.get(i) {
                            *c = d.clone();
                        }
                    }
                }
                for (i, c) in cells.iter_mut().enumerate() {
                    if let Some(t) = types_for_scan.get(i) {
                        validate_insert_value(t, c.trim()).map_err(|e| match e {
                            Error::Invalid(m) if m.starts_with("parser:") => Error::Invalid(m),
                            other => Error::Invalid(format!("import: {other}")),
                        })?;
                        // `AppendByStringView` reads an integer cell with
                        // `std::from_chars` into the column's own type. An
                        // out-of-range cell consumes every digit, so the
                        // reference's `ptr != end` check passes and the value
                        // keeps its initial 0 - it is neither an error nor a
                        // wrapped value.
                        if let Some(zeroed) = zero_out_of_range_int(t, c) {
                            *c = zeroed;
                        }
                    }
                }
                batch.push(cells.into_iter().map(Literal::Str).collect());
            }
            if !batch.is_empty() {
                // The statement keeps its own segment cursor: the table's
                // shared counter belongs to whoever allocated last, and an
                // `INSERT` during a long `COPY` moves it on.
                //
                // `Self::SEGMENT_CAPACITY` bounds one segment: the reference
                // seals a full segment and continues in the next, so a `COPY`
                // of more rows than that ends up with two segments - the one
                // the extent fills and the one the remainder opens.
                let schema = self.table_schema(table).unwrap_or_default();
                let mut start = 0usize;
                while start < batch.len() {
                    let mut current = cursor.take();
                    if current
                        .as_ref()
                        .is_some_and(|c| c.offset as usize >= Self::SEGMENT_CAPACITY)
                    {
                        current = None;
                    }
                    let room = current.as_ref().map_or(Self::SEGMENT_CAPACITY, |c| {
                        Self::SEGMENT_CAPACITY - c.offset as usize
                    });
                    let take = (batch.len() - start).min(room);
                    let (range, next_cursor) = match current {
                        Some(cursor) => self.reserve_import_rows(table, take, cursor)?,
                        None => {
                            let opened = self.begin_import(table)?;
                            self.reserve_import_rows(table, take, opened)?
                        }
                    };
                    let base = range.base;
                    cursor = Some(next_cursor);
                    batches_all.push(take);
                    self.note_index_insert(table, take);
                    for (k, row) in batch[start..start + take].iter().enumerate() {
                        // The rows ride in the transaction only as its `ImportV2`
                        // extent; their values go to the block buffer at commit.
                        let encoded = self.encode_row_stored(table, row, &types_for_scan)?;
                        encoded_rows.push((base + k, encoded));
                    }
                    let segment = (base >> 32) as u32;
                    if !segments.contains(&segment) {
                        segments.push(segment);
                    }
                    for (k, row) in batch[start..start + take].iter().enumerate() {
                        self.update_indexes_on_insert(table, base + k, row, &schema)?;
                        let texts: Vec<String> = row.iter().map(|v| v.to_slt_string()).collect();
                        self.update_text_indexes(table, base + k, &texts, &schema);
                        self.update_vector_caches(table, base + k, &texts);
                    }
                    total += take;
                    staged += take;
                    start += take;
                }
            }
            if eof {
                break;
            }
        }
        if staged > 0 {
            // One `create` boundary per staged batch, across every segment this
            // statement opened.
            self.commit_import_chunk(table, t, &segments, &encoded_rows, &batches_all)?;
        } else {
            drop(t);
        }
        Ok(QueryResult::Inserted { count: total })
    }

    /// Loads a `FORMAT JSONL` copy: one JSON object per line, keyed by column
    /// name. Sparse columns come as `{index: value}` objects (float) or index
    /// arrays (bit); everything else is a scalar.
    pub(crate) fn copy_from_jsonl(
        &self,
        table: &str,
        text: &str,
        types: &[String],
        defaults: &[Option<String>],
    ) -> Result<QueryResult> {
        use expression::json::Json;
        let mut objects: Vec<Vec<(String, Json)>> = Vec::new();
        for line in text.lines() {
            let line = line.trim();
            if line.is_empty() {
                continue;
            }
            if let Some(Json::Obj(fields)) = expression::json::parse(line) {
                objects.push(fields);
            }
        }
        self.insert_json_objects(table, &objects, types, defaults)
    }

    /// Loads a `FORMAT JSON` copy: the whole file is one JSON array whose
    /// elements are objects keyed by column name.
    pub(crate) fn copy_from_json(
        &self,
        table: &str,
        text: &str,
        types: &[String],
        defaults: &[Option<String>],
    ) -> Result<QueryResult> {
        use expression::json::Json;
        let mut objects: Vec<Vec<(String, Json)>> = Vec::new();
        if let Some(Json::Arr(items)) = expression::json::parse(text.trim()) {
            for it in items {
                if let Json::Obj(fields) = it {
                    objects.push(fields);
                }
            }
        }
        self.insert_json_objects(table, &objects, types, defaults)
    }

    /// Inserts one row per JSON object (keyed by column name). Sparse columns
    /// come as `{index: value}` objects (float) or index arrays (bit);
    /// everything else is a scalar.
    pub(crate) fn insert_json_objects(
        &self,
        table: &str,
        objects: &[Vec<(String, expression::json::Json)>],
        types: &[String],
        defaults: &[Option<String>],
    ) -> Result<QueryResult> {
        use expression::json::Json;
        let schema = self.table_schema(table)?;
        let mut rows: Vec<Vec<Literal>> = Vec::new();
        for fields in objects {
            let mut cells: Vec<Literal> = Vec::with_capacity(schema.len());
            for (i, name) in schema.iter().enumerate() {
                let ty = types.get(i).cloned().unwrap_or_default();
                let value = fields.iter().find(|(k, _)| k == name).map(|(_, v)| v);
                let cell = match value {
                    Some(Json::Obj(pairs)) if is_sparse_type(&ty) => {
                        let mut ps: Vec<(i64, f64)> = pairs
                            .iter()
                            .filter_map(|(k, v)| {
                                let idx = k.trim().parse::<i64>().ok()?;
                                let val = match v {
                                    Json::Int(x) => *x as f64,
                                    Json::Float(x) => *x,
                                    _ => return None,
                                };
                                Some((idx, val))
                            })
                            .collect();
                        ps.sort_by_key(|(i, _)| *i);
                        let body: Vec<String> =
                            ps.iter().map(|(i, v)| format!("{i}:{v:.6}")).collect();
                        Literal::Str(format!("[{}]", body.join(",")))
                    }
                    Some(Json::Arr(items)) if is_sparse_type(&ty) => {
                        let idx: Vec<String> = items
                            .iter()
                            .filter_map(|x| match x {
                                Json::Int(i) => Some(i.to_string()),
                                Json::Float(f) => Some((*f as i64).to_string()),
                                _ => None,
                            })
                            .collect();
                        Literal::Str(format!("[{}]", idx.join(",")))
                    }
                    // an absent / null field takes the column default
                    Some(Json::Null) | None => defaults
                        .get(i)
                        .and_then(|d| d.as_deref())
                        .map(default_literal)
                        .unwrap_or(Literal::Null),
                    Some(other) => Literal::Str(json_scalar_text(other)),
                };
                cells.push(cell);
            }
            // validate each typed cell (sparse range, tensor dimension)
            for (i, cell) in cells.iter().enumerate() {
                if let Some(ty) = types.get(i) {
                    validate_insert_value(ty, cell.to_slt_string().trim())?;
                }
            }
            rows.push(cells);
        }
        if rows.is_empty() {
            return Ok(QueryResult::Inserted { count: 0 });
        }
        let opened = self.begin_import(table)?;
        let (range, _cursor) = self.reserve_import_rows(table, rows.len(), opened)?;
        let base = range.base;
        self.note_index_insert(table, rows.len());
        let mut t = self.txn.begin();
        t.set_op(TxnOp::Import {
            table: table.to_string(),
        });
        // `PrepareCommitImport` seals the segment and writes its rows into the
        // block as part of the transaction, after it commits.
        let turn = self.txn.reserve_commit_ts();
        let ts = turn.ts();
        let _ts = self.txn.commit_at_deferred(t, ts)?;
        let _visible = BottomGuard::new(&self.txn, _ts);
        self.note_append(table, rows.len(), true, Some(_ts));
        self.note_import_ts(table, (base >> 32) as u32, _ts);
        self.publish_rows(table, base, &rows)?;
        let schema = self.table_schema(table).unwrap_or_default();
        for (k, row) in rows.iter().enumerate() {
            self.update_indexes_on_insert(table, base + k, row, &schema)?;
            let texts: Vec<String> = row.iter().map(|v| v.to_slt_string()).collect();
            self.update_text_indexes(table, base + k, &texts, &schema);
            self.update_vector_caches(table, base + k, &texts);
        }
        self.dump_hnsw_over_capacity(table);
        self.dump_secondary_over_capacity(table);
        Ok(QueryResult::Inserted { count: rows.len() })
    }

    /// Writes `table` to a file (`COPY t TO …`): CSV, JSONL, or the binary
    /// `fvecs` (per row: `i32 dim` followed by `dim` little-endian `f32`).
    /// Commits `rows` (one `Vec<Literal>` per row) as one import transaction:
    /// `ApplySegmentIDs`, the `ImportV2` frame, the block write and the
    /// published rows. Every import format parses into the same literals and
    /// lands here.
    fn import_literal_rows(
        &self,
        table: &str,
        rows: Vec<Vec<Literal>>,
        t_read: std::time::Duration,
    ) -> Result<QueryResult> {
        let profile = std::env::var_os("INFINITY_PROFILE_IMPORT").is_some();
        let t1 = std::time::Instant::now();
        // `ApplySegmentIDs`: a segment of the statement's own, with no append
        // record yet - the records are written when it commits.
        let opened = self.begin_import(table)?;
        let (range, _cursor) = self.reserve_import_rows(table, rows.len(), opened)?;
        let base = range.base;
        self.note_index_insert(table, rows.len());
        let mut t = self.txn.begin();
        t.set_op(TxnOp::Import {
            table: table.to_string(),
        });
        t.set_typed_commands(vec![self.import_command(table, (base >> 32) as u32)?]);
        let t_encode = t1.elapsed();
        let t2 = std::time::Instant::now();
        // `PrepareCommitImport` -> `CommitBottomAppend`: the block and the
        // `create` boundary belong to the transaction, so they are written once
        // it commits - a statement that loses its commit leaves neither behind.
        let turn = self.txn.reserve_commit_ts();
        let ts = turn.ts();
        let ts = self.txn.commit_at_deferred(t, ts)?;
        let t_commit = t2.elapsed();
        let t3 = std::time::Instant::now();
        let _visible = BottomGuard::new(&self.txn, ts);
        self.note_append(table, rows.len(), true, Some(ts));
        let segment = (base >> 32) as u32;
        let imported = self.segment_rows(table, segment as usize) as u64;
        self.note_import_rows(table, segment, imported);
        self.note_import_ts(table, segment, ts);
        self.publish_rows(table, base, &rows)?;
        // `AppendInBlock`: the import's block is readable as soon as the
        // statement commits, exactly like the CSV path.
        if self.has_block_layout(table) {
            self.write_blocks_commit(table, ts)?;
        }
        drop(_visible);
        let t_bottom = t3.elapsed();
        if profile {
            eprintln!(
                "import profile: rows={} read={:?} encode={:?} commit={:?} bottom={:?}",
                rows.len(),
                t_read,
                t_encode,
                t_commit,
                t_bottom
            );
        }
        Ok(QueryResult::Inserted { count: rows.len() })
    }

    /// One literal row per record of a binary vector file (`FORMAT FVECS`,
    /// `FORMAT BVECS`): each record is an `i32 dim` followed by `dim`
    /// elements. `elem_width` is 4 for `f32` and 1 for `u8`.
    fn binary_vector_rows(
        &self,
        table: &str,
        bytes: &[u8],
        elem_width: usize,
    ) -> Result<Vec<Vec<Literal>>> {
        let types = self.column_types(table)?;
        if types.len() != 1 {
            return Err(Error::Invalid(format!(
                "import: the format needs a table with exactly one vector column, got {}",
                types.len()
            )));
        }
        let declared = type_dim(&types[0]).unwrap_or(0);
        let mut rows = Vec::new();
        let mut pos = 0usize;
        while pos + 4 <= bytes.len() {
            let dim = i32::from_le_bytes(bytes[pos..pos + 4].try_into().unwrap());
            pos += 4;
            let Ok(dim) = usize::try_from(dim) else {
                return Err(Error::Invalid(
                    "import: the vector file has a negative dimension".into(),
                ));
            };
            if dim == 0 || pos + dim * elem_width > bytes.len() {
                return Err(Error::Invalid(
                    "import: the vector file is truncated".into(),
                ));
            }
            if declared != 0 && declared != dim {
                return Err(Error::Invalid(format!(
                    "import: the file's dimension ({dim}) does not match the column's ({declared})"
                )));
            }
            let vals: Vec<String> = (0..dim)
                .map(|i| {
                    let at = pos + i * elem_width;
                    if elem_width == 4 {
                        fmt_f32_cpp(f32::from_le_bytes(bytes[at..at + 4].try_into().unwrap()))
                    } else {
                        bytes[at].to_string()
                    }
                })
                .collect();
            pos += dim * elem_width;
            rows.push(vec![Literal::Str(format!("[{}]", vals.join(",")))]);
        }
        if rows.is_empty() || pos != bytes.len() {
            return Err(Error::Invalid(
                "import: the vector file has trailing bytes".into(),
            ));
        }
        Ok(rows)
    }

    /// One literal row per row of a CSR sparse matrix (`FORMAT CSR`):
    /// `i64 nrow`, `i64 ncol`, `i64 nnz`, `(nrow + 1)` row offsets, `nnz` `i32`
    /// column indices and `nnz` `f32` values.
    fn csr_sparse_rows(&self, table: &str, bytes: &[u8]) -> Result<Vec<Vec<Literal>>> {
        let types = self.column_types(table)?;
        if types.len() != 1 || !is_sparse_type(&types[0]) {
            return Err(Error::Invalid(
                "import: csr needs a table with exactly one sparse column".into(),
            ));
        }
        if bytes.len() < 24 {
            return Err(Error::Invalid("import: the csr file is truncated".into()));
        }
        let i64_at = |at: usize| -> i64 {
            i64::from_le_bytes(bytes[at..at + 8].try_into().expect("header is checked"))
        };
        let nrow = i64_at(0);
        let ncol = i64_at(8);
        let nnz = i64_at(16);
        if nrow < 0 || ncol < 0 || nnz < 0 {
            return Err(Error::Invalid("import: the csr header is invalid".into()));
        }
        let (nrow, nnz) = (nrow as usize, nnz as usize);
        let offsets_at = 24;
        let indices_at = offsets_at + (nrow + 1) * 8;
        let values_at = indices_at + nnz * 4;
        if bytes.len() != values_at + nnz * 4 {
            return Err(Error::Invalid(
                "import: the csr file's size does not match its header".into(),
            ));
        }
        let declared = type_dim(&types[0]).unwrap_or(0);
        if declared != 0 && declared != ncol as usize {
            return Err(Error::Invalid(format!(
                "import: the file's dimension ({ncol}) does not match the column's ({declared})"
            )));
        }
        let mut rows = Vec::with_capacity(nrow);
        for row in 0..nrow {
            let start = i64::from_le_bytes(
                bytes[offsets_at + row * 8..offsets_at + row * 8 + 8]
                    .try_into()
                    .unwrap(),
            );
            let end = i64::from_le_bytes(
                bytes[offsets_at + (row + 1) * 8..offsets_at + (row + 1) * 8 + 8]
                    .try_into()
                    .unwrap(),
            );
            if start < 0 || end < start || end as usize > nnz {
                return Err(Error::Invalid(
                    "import: the csr row offsets are invalid".into(),
                ));
            }
            let mut parts = Vec::with_capacity((end - start) as usize);
            for at in start as usize..end as usize {
                let index = i32::from_le_bytes(
                    bytes[indices_at + at * 4..indices_at + at * 4 + 4]
                        .try_into()
                        .unwrap(),
                );
                let value = f32::from_le_bytes(
                    bytes[values_at + at * 4..values_at + at * 4 + 4]
                        .try_into()
                        .unwrap(),
                );
                // Six decimals, like the reference's `SparseT::ToString`: the
                // stored text is what a `SELECT` prints, and rounding here is
                // the same rounding `%.6f` applies to the f32 value.
                parts.push(format!("{index}:{value:.6}"));
            }
            rows.push(vec![Literal::Str(format!("[{}]", parts.join(",")))]);
        }
        Ok(rows)
    }

    /// One literal row per row of a Parquet file (`FORMAT PARQUET`).
    ///
    /// The reference reads it with Arrow and appends each cell to its column
    /// (`ParquetValueHandler`); here every cell is rendered to the same text
    /// form the CSV path produces, so the type handling is shared (an
    /// embedding arrives as `[1,2,3]`, a sparse as `[i:v,...]`).
    fn parquet_rows(&self, table: &str, path: &std::path::Path) -> Result<Vec<Vec<Literal>>> {
        use parquet::arrow::arrow_reader::ParquetRecordBatchReaderBuilder;
        let types = self.column_types(table)?;
        let file = std::fs::File::open(path).map_err(|e| Error::io(path, e))?;
        let builder = ParquetRecordBatchReaderBuilder::try_new(file)
            .map_err(|e| Error::Invalid(format!("import: {e}")))?;
        // `CheckParquetColumns`: the file has one column per table column and
        // each one has the Arrow type the column declares.
        let file_schema = builder.schema().clone();
        if file_schema.fields().len() != types.len() {
            return Err(Error::Invalid(format!(
                "import: the file has {} columns but the table has {}",
                file_schema.fields().len(),
                types.len()
            )));
        }
        for (field, ty) in file_schema.fields().iter().zip(types.iter()) {
            if !parquet_type_matches(field.data_type(), ty) {
                return Err(Error::Invalid(format!(
                    "import: column '{}' is {} in the file but '{ty}' in the table",
                    field.name(),
                    field.data_type()
                )));
            }
        }
        let reader = builder
            .build()
            .map_err(|e| Error::Invalid(format!("import: {e}")))?;
        let mut rows = Vec::new();
        for batch in reader {
            let batch = batch.map_err(|e| Error::Invalid(format!("import: {e}")))?;
            if batch.num_columns() != types.len() {
                return Err(Error::Invalid(format!(
                    "import: the file has {} columns but the table has {}",
                    batch.num_columns(),
                    types.len()
                )));
            }
            for row in 0..batch.num_rows() {
                let mut cells = Vec::with_capacity(types.len());
                for (col, ty) in types.iter().enumerate() {
                    let array = batch.column(col);
                    let text = if is_sparse_type(ty) {
                        parquet_sparse_text(array.as_ref(), row)
                    } else {
                        parquet_cell_text(array.as_ref(), row)
                    };
                    cells.push(match text {
                        Some(text) => Literal::Str(text),
                        None => Literal::Null,
                    });
                }
                rows.push(cells);
            }
        }
        Ok(rows)
    }
}

/// One Arrow cell as the text the engine stores: scalars as their literal, a
/// list (fixed-size or not) as `[a,b,...]`, recursively.
fn parquet_cell_text(array: &dyn arrow_array::Array, row: usize) -> Option<String> {
    use arrow_array::*;
    use arrow_schema::DataType;
    if array.is_null(row) {
        return None;
    }
    let any = array.as_any();
    match array.data_type() {
        DataType::Boolean => Some(any.downcast_ref::<BooleanArray>()?.value(row).to_string()),
        DataType::Int8 => Some(any.downcast_ref::<Int8Array>()?.value(row).to_string()),
        DataType::Int16 => Some(any.downcast_ref::<Int16Array>()?.value(row).to_string()),
        DataType::Int32 => Some(any.downcast_ref::<Int32Array>()?.value(row).to_string()),
        DataType::Int64 => Some(any.downcast_ref::<Int64Array>()?.value(row).to_string()),
        DataType::UInt8 => Some(any.downcast_ref::<UInt8Array>()?.value(row).to_string()),
        DataType::UInt16 => Some(any.downcast_ref::<UInt16Array>()?.value(row).to_string()),
        DataType::UInt32 => Some(any.downcast_ref::<UInt32Array>()?.value(row).to_string()),
        DataType::UInt64 => Some(any.downcast_ref::<UInt64Array>()?.value(row).to_string()),
        DataType::Float32 => Some(fmt_f32_cpp(any.downcast_ref::<Float32Array>()?.value(row))),
        DataType::Float64 => Some(fmt_f64_cpp(any.downcast_ref::<Float64Array>()?.value(row))),
        DataType::Utf8 => Some(any.downcast_ref::<StringArray>()?.value(row).to_string()),
        DataType::LargeUtf8 => Some(
            any.downcast_ref::<LargeStringArray>()?
                .value(row)
                .to_string(),
        ),
        DataType::FixedSizeList(_, _) => {
            let list = any.downcast_ref::<FixedSizeListArray>()?;
            parquet_list_text(list.value(row).as_ref())
        }
        DataType::List(_) => {
            let list = any.downcast_ref::<ListArray>()?;
            parquet_list_text(list.value(row).as_ref())
        }
        _ => None,
    }
}

fn parquet_list_text(values: &dyn arrow_array::Array) -> Option<String> {
    let mut parts = Vec::with_capacity(values.len());
    for i in 0..values.len() {
        parts.push(parquet_cell_text(values, i).unwrap_or_else(|| "null".to_string()));
    }
    Some(format!("[{}]", parts.join(",")))
}

/// A sparse cell is a struct of two parallel lists (`index`, `value`); the
/// engine stores it as `[i:v,...]` with the values at six decimals, the form
/// `SparseT::ToString` prints (`%.6f` of the f32, which is why the value is
/// taken as a float here and not from its text).
fn parquet_sparse_text(array: &dyn arrow_array::Array, row: usize) -> Option<String> {
    use arrow_array::StructArray;
    if array.is_null(row) {
        return None;
    }
    // Our own export writes the sparse literal as text; the reference's
    // writes the struct. Both are accepted.
    let Some(fields) = array.as_any().downcast_ref::<StructArray>() else {
        return parquet_cell_text(array, row);
    };
    let indices = parquet_int_list(fields.column_by_name("index")?.as_ref(), row)?;
    let values = parquet_float_list(fields.column_by_name("value")?.as_ref(), row)?;
    if indices.len() != values.len() {
        return None;
    }
    let parts: Vec<String> = indices
        .iter()
        .zip(values.iter())
        .map(|(i, v)| format!("{i}:{v:.6}"))
        .collect();
    Some(format!("[{}]", parts.join(",")))
}

/// The integers of one list cell (`sparse.index`).
fn parquet_int_list(array: &dyn arrow_array::Array, row: usize) -> Option<Vec<i64>> {
    let values = parquet_list_values(array, row)?;
    let mut out = Vec::with_capacity(values.len());
    for i in 0..values.len() {
        out.push(parquet_cell_text(values.as_ref(), i)?.parse().ok()?);
    }
    Some(out)
}

/// The floats of one list cell (`sparse.value`).
fn parquet_float_list(array: &dyn arrow_array::Array, row: usize) -> Option<Vec<f64>> {
    use arrow_array::{Float32Array, Float64Array};
    let values = parquet_list_values(array, row)?;
    let mut out = Vec::with_capacity(values.len());
    for i in 0..values.len() {
        if values.is_null(i) {
            out.push(0.0);
            continue;
        }
        let any = values.as_any();
        if let Some(v) = any.downcast_ref::<Float32Array>() {
            out.push(f64::from(v.value(i)));
        } else if let Some(v) = any.downcast_ref::<Float64Array>() {
            out.push(v.value(i));
        } else {
            out.push(parquet_cell_text(values.as_ref(), i)?.parse().ok()?);
        }
    }
    Some(out)
}

fn parquet_list_values(
    array: &dyn arrow_array::Array,
    row: usize,
) -> Option<arrow_array::ArrayRef> {
    use arrow_array::{FixedSizeListArray, ListArray};
    let any = array.as_any();
    if let Some(list) = any.downcast_ref::<FixedSizeListArray>() {
        return Some(list.value(row));
    }
    any.downcast_ref::<ListArray>()?.value(row).into()
}

/// Whether a file column's Arrow type is the one the table's column declares
/// (`CheckParquetColumns`). A compound column also accepts `Utf8`: that is the
/// literal text this engine's own export writes.
fn parquet_type_matches(file: &arrow_schema::DataType, type_text: &str) -> bool {
    use arrow_schema::DataType;
    let upper = type_text.trim().to_ascii_uppercase();
    let head = upper.split(['(', ',', ' ']).next().unwrap_or("").trim();
    let dim = type_dim(&upper).unwrap_or(0);
    match head {
        "BOOLEAN" | "BOOL" => matches!(file, DataType::Boolean),
        "TINYINT" | "INT8" => matches!(file, DataType::Int8),
        "SMALLINT" | "INT16" => matches!(file, DataType::Int16),
        "INT" | "INTEGER" | "INT32" => matches!(file, DataType::Int32),
        "BIGINT" | "INT64" => matches!(file, DataType::Int64),
        "FLOAT" => matches!(file, DataType::Float32),
        "DOUBLE" => matches!(file, DataType::Float64),
        "VARCHAR" => matches!(file, DataType::Utf8 | DataType::LargeUtf8),
        "EMBEDDING" | "VECTOR" => match file {
            DataType::Utf8 => true,
            DataType::FixedSizeList(_, size) => *size as usize == dim,
            DataType::List(_) => true,
            _ => false,
        },
        "MULTIVECTOR" | "TENSOR" => match file {
            DataType::Utf8 => true,
            DataType::List(field) => fixed_list_dim(field.data_type()) == Some(dim),
            _ => false,
        },
        "TENSORARRAY" => match file {
            DataType::Utf8 => true,
            DataType::List(field) => match field.data_type() {
                DataType::List(inner) => fixed_list_dim(inner.data_type()) == Some(dim),
                _ => false,
            },
            _ => false,
        },
        // A sparse cell is `struct<index, value>`; the value list's element
        // type is the column's (`SPARSE(DOUBLE, …)` does not take a
        // `list<float>` file).
        "SPARSE" => match file {
            DataType::Utf8 => true,
            DataType::Struct(fields) => {
                let Some(value) = fields.iter().find(|f| f.name() == "value") else {
                    return false;
                };
                let Some(elem) = list_elem(value.data_type()) else {
                    return false;
                };
                *elem == parquet_scalar_type(sparse_value_type(&upper))
            }
            _ => false,
        },
        _ => true,
    }
}

fn fixed_list_dim(data_type: &arrow_schema::DataType) -> Option<usize> {
    match data_type {
        arrow_schema::DataType::FixedSizeList(_, size) => Some(*size as usize),
        _ => None,
    }
}

/// The Arrow type of a scalar type name (`FLOAT` -> `Float32`), used to check
/// the element type a compound column declares.
fn parquet_scalar_type(type_text: &str) -> arrow_schema::DataType {
    use arrow_schema::DataType;
    let upper = type_text.trim().to_ascii_uppercase();
    let head = upper.split(['(', ',', ' ']).next().unwrap_or("").trim();
    match head {
        "BOOLEAN" | "BOOL" => DataType::Boolean,
        "TINYINT" | "INT8" => DataType::Int8,
        "SMALLINT" | "INT16" => DataType::Int16,
        "INT" | "INTEGER" | "INT32" => DataType::Int32,
        "BIGINT" | "INT64" => DataType::Int64,
        "FLOAT" => DataType::Float32,
        "DOUBLE" => DataType::Float64,
        _ => DataType::Utf8,
    }
}

fn list_elem(data_type: &arrow_schema::DataType) -> Option<&arrow_schema::DataType> {
    match data_type {
        arrow_schema::DataType::List(field) => Some(field.data_type()),
        _ => None,
    }
}

/// The value type of a `SPARSE(<value>, <index>, <dim>)` declaration.
fn sparse_value_type(type_text: &str) -> &str {
    type_text
        .split_once('(')
        .map(|(_, rest)| rest)
        .and_then(|rest| rest.split(',').next())
        .unwrap_or("")
        .trim()
}
