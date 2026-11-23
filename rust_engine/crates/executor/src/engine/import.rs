// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//! `COPY ... FROM ...`: one entry point per format, mirroring the
//! reference's `PhysicalImport::NewImport<FORMAT>` (one method per format in
//! `physical_import_impl.cpp`).

use super::*;

/// Stored text an import keeps in memory before it spills to disk.
///
/// The import commits once, so its rows cannot reach the block writer early;
/// holding every row's stored text until then made the peak the *statement*
/// (11 KiB per row at 1536d - 5.8 GiB for one 500k-row segment, measured on
/// `spann_scale_1536d`). Past this much text the rows go to a scratch file and
/// are replayed a batch at a time, so the import's memory is a batch whatever
/// the file's size.
const IMPORT_SPILL_BYTES: usize = 64 * 1024 * 1024;

/// The rows one `COPY` stages for the block writer, spilled past
/// [`IMPORT_SPILL_BYTES`].
pub(crate) struct StagedRows {
    /// Rows kept in memory (the whole set while it stays under the threshold).
    memory: Vec<(usize, String)>,
    /// The scratch file once the threshold is crossed: path and writer.
    spill: Option<(std::path::PathBuf, std::io::BufWriter<std::fs::File>)>,
    /// Rows staged so far, in both modes.
    count: usize,
    /// Bytes of stored text held so far, in both modes.
    bytes: usize,
}

impl StagedRows {
    pub(crate) fn new() -> Self {
        Self {
            memory: Vec::new(),
            spill: None,
            count: 0,
            bytes: 0,
        }
    }

    pub(crate) fn is_empty(&self) -> bool {
        self.count == 0
    }

    /// Stages one row's stored text, moving the set to disk when it grows past
    /// the threshold. `dir` is where the scratch file lives.
    fn push(&mut self, seq: usize, text: String, dir: &std::path::Path) -> Result<()> {
        self.count += 1;
        self.bytes += text.len();
        if self.spill.is_none() {
            if self.bytes <= IMPORT_SPILL_BYTES {
                self.memory.push((seq, text));
                return Ok(());
            }
            std::fs::create_dir_all(dir).map_err(|e| Error::io(dir, e))?;
            let path = dir.join(format!(
                "import-spill-{}-{}.rows",
                std::process::id(),
                self.count
            ));
            let file = std::fs::File::create(&path).map_err(|e| Error::io(&path, e))?;
            let mut writer = std::io::BufWriter::with_capacity(1 << 20, file);
            for (seq, text) in self.memory.drain(..) {
                Self::write_row(&mut writer, &path, seq, &text)?;
            }
            self.spill = Some((path, writer));
            // The row that crossed the threshold goes to the file too, below:
            // returning here dropped exactly one row per segment, and the block
            // writer caught it ("block 0.0 holds 8191 of the 8192 rows
            // committed at ts 10").
        }
        let (path, writer) = self.spill.as_mut().unwrap();
        let path = path.clone();
        Self::write_row(writer, &path, seq, &text)
    }

    fn write_row(
        writer: &mut impl std::io::Write,
        path: &std::path::Path,
        seq: usize,
        text: &str,
    ) -> Result<()> {
        let len = u32::try_from(text.len())
            .map_err(|_| Error::Invalid("import: a row's stored text exceeds 4 GiB".into()))?;
        writer
            .write_all(&(seq as u64).to_le_bytes())
            .and_then(|()| writer.write_all(&len.to_le_bytes()))
            .and_then(|()| writer.write_all(text.as_bytes()))
            .map_err(|e| Error::io(path, e))
    }

    /// Pushes what the writer still holds to the file, so [`Self::replay`] can
    /// read it back.
    fn flush(&mut self) -> Result<()> {
        use std::io::Write;
        if let Some((path, writer)) = self.spill.as_mut() {
            let path = path.clone();
            writer.flush().map_err(|e| Error::io(&path, e))?;
        }
        Ok(())
    }

    /// Hands every staged row to `row`, in staging order, calling `batch_done`
    /// at each boundary in `batches` (one entry per import batch).
    ///
    /// One pass for the whole set: the caller publishes a batch, writes the
    /// block it landed in, and drops it, so the append buffer never holds more
    /// than one batch either.
    pub(crate) fn replay_batches(
        &self,
        batches: &[usize],
        mut row: impl FnMut(usize, &str) -> Result<()>,
        mut batch_done: impl FnMut(usize) -> Result<()>,
    ) -> Result<()> {
        let mut batch = 0usize;
        let mut left = batches.first().copied().unwrap_or(0);
        let mut end_batch = |batch: &mut usize, left: &mut usize| -> Result<()> {
            batch_done(*batch)?;
            *batch += 1;
            *left = batches.get(*batch).copied().unwrap_or(0);
            Ok(())
        };
        let Some((path, _)) = self.spill.as_ref() else {
            for (seq, text) in &self.memory {
                row(*seq, text.as_str())?;
                left = left.saturating_sub(1);
                if left == 0 {
                    end_batch(&mut batch, &mut left)?;
                }
            }
            return Ok(());
        };
        use std::io::Read;
        let file = std::fs::File::open(path).map_err(|e| Error::io(path.as_path(), e))?;
        let mut reader = std::io::BufReader::with_capacity(1 << 20, file);
        let mut buf: Vec<u8> = Vec::new();
        loop {
            let mut head = [0u8; 12];
            match reader.read_exact(&mut head) {
                Ok(()) => {}
                Err(e) if e.kind() == std::io::ErrorKind::UnexpectedEof => break,
                Err(e) => return Err(Error::io(path.as_path(), e)),
            }
            let seq = u64::from_le_bytes(head[..8].try_into().unwrap()) as usize;
            let len = u32::from_le_bytes(head[8..].try_into().unwrap()) as usize;
            buf.clear();
            buf.resize(len, 0);
            reader
                .read_exact(&mut buf)
                .map_err(|e| Error::io(path.as_path(), e))?;
            let text = String::from_utf8_lossy(&buf);
            row(seq, text.as_ref())?;
            left = left.saturating_sub(1);
            if left == 0 {
                end_batch(&mut batch, &mut left)?;
            }
        }
        Ok(())
    }
}

impl Drop for StagedRows {
    fn drop(&mut self) {
        if let Some((path, _)) = self.spill.take() {
            let _ = std::fs::remove_file(path);
        }
    }
}

impl Engine {
    /// Marks `table` as having an `IMPORT` in flight until the guard drops.
    ///
    /// The reference's `NewTxn::Import` builds every index's chunk **once**, at
    /// the end of the transaction (`PopulateIndex`), so nothing in the staging
    /// loop may dump. The port feeds the same per-batch index hooks an `INSERT`
    /// does, and those hooks carry the append path's triggers (the row capacity
    /// and the memory quota's waterlines); this flag turns them off for the
    /// table being imported into.
    /// Marks `table` as having an import in flight, until the guard drops.
    ///
    /// **The marker and an index seal share one exclusion.** The dump takes
    /// `flush_lock` *after* it has checked `import_in_flight`
    /// (`index_build.rs::dump_index_transaction`), so a marker set without that
    /// lock can land between the check and the seal: the dump then publishes a
    /// chunk over a slice of the import's rows, which is exactly the split
    /// `index_build.rs` says an import must avoid. Taking the lock here orders
    /// the two - a dump either sees the marker or has already finished - and
    /// the guard holds no guard across its life, only the marker.
    ///
    /// The marker means "these rows are not settled yet", not "uncommitted":
    /// the import commits before it inserts into the tails (`import_literal_rows`
    /// commits, then stages). What it protects is the segment's shape, so a
    /// partial chunk cannot be published under it.
    async fn stage_import(&self, table: &str) -> ImportStaging<'_> {
        {
            let _serial = self.storage.flush_lock.read().await;
            let key = self.table_key(table);
            self.tables
                .lock()
                .unwrap()
                .entry(key)
                .or_default()
                .importing += 1;
        }
        ImportStaging {
            engine: self,
            key: self.table_key(table),
            table: self.qualified_table(table),
            staged: Vec::new(),
            committed: false,
        }
    }

    /// Loads a CSV file into `table` (`COPY t FROM …`). The path may be absolute
    /// or the Infinity fixture path `/var/infinity/test_data/…`, which is mapped
    /// onto `INFINITY_DATA_DIR` (default `test/data`) if present.
    pub(crate) async fn copy_from(
        &self,
        table: &str,
        path: &str,
        opts: &[(String, String)],
    ) -> Result<QueryResult> {
        // `NewTxn::Import` is **one** transaction: it writes every block and
        // commits once. The extent each segment ends up with is what decides
        // how many index chunks the mirror publishes for it.
        let before = self.segment_row_counts(table);
        let result = self.copy_from_inner(table, path, opts).await;
        if result.is_ok() {
            self.settle_import_extent(table, &before);
        }
        result
    }

    pub(crate) async fn copy_from_inner(
        &self,
        table: &str,
        path: &str,
        opts: &[(String, String)],
    ) -> Result<QueryResult> {
        // The import writes its blocks inside its own commit, so a background
        // compaction must not run between the commit and the write.
        let gate = self.batch_gate(table);
        let _batch = gate.lock().await;
        if self.storage.catalog.get_table_by_name(table)?.is_none() {
            return Err(Error::NotFound(format!("table '{table}'")));
        }
        let resolved = resolve_data_path(path);
        let format = opts
            .iter()
            .find(|(k, _)| k == "FORMAT")
            .map(|(_, v)| v.to_ascii_uppercase());
        // The whole file is only read for the formats that need it in one
        // piece (the JSON forms). CSV streams, and so does every binary vector
        // format: a 3.8 GB fvecs read with `std::fs::read` held the bytes, the
        // text rows, the published values and the append buffer at once.
        let whole_file = |path: &std::path::Path| -> Option<Vec<u8>> { std::fs::read(path).ok() };
        // Missing fixtures stay a statement-level success with no rows.
        // The binary formats parse into the same literal rows the CSV path
        // builds, then commit them the same way.
        let elem_width = match format.as_deref() {
            Some("FVECS") => Some(4usize),
            Some("BVECS") => Some(1usize),
            _ => None,
        };
        if let Some(elem_width) = elem_width {
            return self
                .copy_from_binary_vectors(table, &resolved, elem_width)
                .await;
        }
        let rows = match format.as_deref() {
            Some("CSR") => Some(self.csr_sparse_rows(
                table,
                &whole_file(&resolved).ok_or_else(|| {
                    Error::Invalid(format!("cannot read '{}'", resolved.display()))
                })?,
            )?),
            Some("PARQUET") => Some(self.parquet_rows(table, &resolved)?),
            _ => None,
        };
        if let Some(rows) = rows {
            return self
                .import_literal_rows(table, rows, std::time::Duration::ZERO)
                .await;
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
            let Some(bytes_in) = whole_file(&resolved) else {
                return Ok(QueryResult::Inserted { count: 0 });
            };
            let text = String::from_utf8_lossy(&bytes_in).into_owned();
            if format_of("JSONL") {
                return self
                    .copy_from_jsonl(table, &text, &types_for_scan, &defaults)
                    .await;
            }
            return self
                .copy_from_json(table, &text, &types_for_scan, &defaults)
                .await;
        }
        // CSV: read line by line and commit in blocks, so an 8-million-row
        // import does not hold the file and its parsed rows in memory.
        //
        // The read is **async**: an 8M-row file read line by line on a worker
        // would hold that worker for the whole statement (`AGENTS.md` §7.2),
        // and the statement's own commit work needs the same runtime. The
        // buffer is `tokio::io::BufReader`, so a slow disk yields the worker
        // between batches instead of parking it.
        let file = match tokio::fs::File::open(&resolved).await {
            Ok(f) => f,
            Err(_) => return Ok(QueryResult::Inserted { count: 0 }),
        };
        let mut reader = tokio::io::BufReader::new(file);
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
        // values it publishes without a second pass over the file. Spilled past
        // a batch's worth: the whole statement's text does not stay resident.
        let mut staged_rows = StagedRows::new();
        let spill_dir = self.dir.join("tmp");

        // `HEADER true` makes the first line a header: the reference validates
        // its column count against the table and then drops it without counting
        // it as a row (`PhysicalImport::NewCSVHeaderHandler`).
        let mut pending_header = opts
            .iter()
            .any(|(k, v)| k == "HEADER" && v.eq_ignore_ascii_case("true"));
        // `NewTxn::Import`: the reference stages the whole statement into one
        // transaction and only populates the indexes at its end, so nothing
        // dumps from the staging loop. Marking the table for the duration keeps
        // the append path's capacity/quota triggers off - see
        // `TableEntry::importing`.
        let mut staging = self.stage_import(table).await;
        loop {
            let mut batch: Vec<Vec<Literal>> = Vec::with_capacity(Self::COPY_BATCH_ROWS);
            let mut eof = false;
            while batch.len() < Self::COPY_BATCH_ROWS {
                let mut line = String::new();
                use tokio::io::AsyncBufReadExt;
                match reader.read_line(&mut line).await {
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
                if pending_header {
                    pending_header = false;
                    // The reference requires the header to have exactly the
                    // table's column count and then drops it without counting
                    // it as a row. Its cell-by-cell name comparison is
                    // inverted (`!strcmp` aborts on *equal* names) under a
                    // comment that says the names are not checked; that is a
                    // bug, so only the count is enforced here.
                    if fields.len() != types_for_scan.len() {
                        return Err(Error::Invalid(format!(
                            "import: the header has {} columns but the table has {}",
                            fields.len(),
                            types_for_scan.len()
                        )));
                    }
                    continue;
                }
                // One allocation per cell, not two: `normalize_cell` hands back
                // the field unchanged for the plain numeric/text cells that
                // make up most of a `COPY`.
                let mut cells: Vec<String> = fields
                    .into_iter()
                    .map(|f| match normalize_cell_cow(&f) {
                        std::borrow::Cow::Borrowed(_) => f,
                        std::borrow::Cow::Owned(o) => o,
                    })
                    .collect();
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
                        Some(cursor) => self.reserve_import_rows(table, take, cursor).await?,
                        None => {
                            let opened = self.begin_import(table).await?;
                            self.reserve_import_rows(table, take, opened).await?
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
                        staged_rows.push(base + k, encoded, &spill_dir)?;
                    }
                    let segment = (base >> 32) as u32;
                    if !segments.contains(&segment) {
                        segments.push(segment);
                    }
                    // Both hooks take a lock and allocate per row; on a table
                    // with no secondary index or no text/vector index they find
                    // nothing eight million times over.
                    let secondary_targets = if self.has_secondary_indexes(table) {
                        self.secondary_targets(table, &schema)
                    } else {
                        Vec::new()
                    };
                    let needs_texts = self.has_text_or_vector_indexes(table);
                    let mut texts_by_row: Vec<(usize, Vec<String>)> = Vec::with_capacity(take);
                    staging.note((0..take).map(|k| base + k));
                    for (k, row) in batch[start..start + take].iter().enumerate() {
                        self.insert_secondary_keys(&secondary_targets, base + k, row)?;
                        if needs_texts {
                            texts_by_row
                                .push((base + k, row.iter().map(|v| v.to_slt_string()).collect()));
                        }
                    }
                    if needs_texts {
                        // One batch for the full-text columns: tokenise without
                        // the column lock, insert under it once.
                        let text_batch: Vec<(usize, &[String])> = texts_by_row
                            .iter()
                            .map(|(seq, cells)| (*seq, cells.as_slice()))
                            .collect();
                        self.update_text_indexes_batch_async(table, &text_batch, &schema, None)
                            .await;
                        // One batch for the HNSW graphs, so the build uses the
                        // bucket fan-out (`note_hnsw_insert_batch`); the IVF
                        // segment and the query cache are per row.
                        self.note_hnsw_insert_batch(table, &text_batch).await;
                        for (seq, texts) in &texts_by_row {
                            self.note_ivf_insert(table, *seq as u64, texts);
                        }
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
            // A spilled set has to be on the file before the commit reads it
            // back; the in-memory one is already complete.
            staged_rows.flush()?;
            // One `create` boundary per staged batch, across every segment this
            // statement opened.
            self.commit_import_chunk(table, t, &segments, &staged_rows, &batches_all)
                .await?;
            // `live_rows` is what the query path's cache freshness and index
            // coverage gates compare against, and it adds a *delta* to a
            // published count - so every write path has to report its rows.
            // `INSERT` does (`note_live_rows` in the commit hook), `COPY` did
            // not: a table loaded by `COPY` kept the count of whatever last
            // published it (`row_count`, or a vector cache build over the rows
            // of the time), which made a stale vector cache look fresh and let
            // an index that covers part of the table pass the coverage gate.
            self.note_live_rows(table, staged as isize);
            staging.committed();
        } else {
            drop(t);
        }
        Ok(QueryResult::Inserted { count: total })
    }

    /// Loads a `FORMAT JSONL` copy: one JSON object per line, keyed by column
    /// name. Sparse columns come as `{index: value}` objects (float) or index
    /// arrays (bit); everything else is a scalar.
    pub(crate) async fn copy_from_jsonl(
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
            .await
    }

    /// Loads a `FORMAT JSON` copy: the whole file is one JSON array whose
    /// elements are objects keyed by column name.
    pub(crate) async fn copy_from_json(
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
            .await
    }

    /// Inserts one row per JSON object (keyed by column name). Sparse columns
    /// come as `{index: value}` objects (float) or index arrays (bit);
    /// everything else is a scalar.
    pub(crate) async fn insert_json_objects(
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
        let opened = self.begin_import(table).await?;
        let (range, _cursor) = self.reserve_import_rows(table, rows.len(), opened).await?;
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
        let _ts = self.txn.commit_at_deferred(t, ts).await?;
        let _visible = BottomGuard::new(&self.txn, _ts);
        self.note_append(table, rows.len(), true, Some(_ts));
        self.note_import_ts(table, (base >> 32) as u32, _ts);
        self.publish_rows(table, base, &rows)?;
        let schema = self.table_schema(table).unwrap_or_default();
        // `NewTxn::Import`: the indexes are populated once, at the end of the
        // transaction, not by the append path's triggers (see `stage_import`).
        let _staging = self.stage_import(table).await;
        let mut texts_by_row: Vec<(usize, Vec<String>)> = Vec::with_capacity(rows.len());
        for (k, row) in rows.iter().enumerate() {
            self.update_indexes_on_insert(table, base + k, row, &schema)?;
            texts_by_row.push((base + k, row.iter().map(|v| v.to_slt_string()).collect()));
        }
        let text_batch: Vec<(usize, &[String])> = texts_by_row
            .iter()
            .map(|(seq, cells)| (*seq, cells.as_slice()))
            .collect();
        self.update_text_indexes_batch_async(table, &text_batch, &schema, None)
            .await;
        // One batch for the HNSW graphs, so the build uses the bucket fan-out
        // (see `note_hnsw_insert_batch`).
        let vector_batch: Vec<(usize, &[String])> = texts_by_row
            .iter()
            .map(|(seq, cells)| (*seq, cells.as_slice()))
            .collect();
        self.note_hnsw_insert_batch(table, &vector_batch).await;
        for (seq, texts) in &texts_by_row {
            self.note_ivf_insert(table, *seq as u64, texts);
        }
        let segment = (base >> 32) as u32;
        drop(_staging);
        crate::engine::state::with_read_ts_override_async(u64::MAX, async {
            self.populate_import_indexes(table, &[segment], _ts).await;
        })
        .await;
        Ok(QueryResult::Inserted { count: rows.len() })
    }

    /// Writes `table` to a file (`COPY t TO …`): CSV, JSONL, or the binary
    /// `fvecs` (per row: `i32 dim` followed by `dim` little-endian `f32`).
    /// Commits `rows` (one `Vec<Literal>` per row) as one import transaction:
    /// `ApplySegmentIDs`, the `ImportV2` frame, the block write and the
    /// published rows. Every import format parses into the same literals and
    /// lands here.
    async fn import_literal_rows(
        &self,
        table: &str,
        rows: Vec<Vec<Literal>>,
        t_read: std::time::Duration,
    ) -> Result<QueryResult> {
        let profile = std::env::var_os("INFINITY_PROFILE_IMPORT").is_some();
        let t1 = std::time::Instant::now();
        // `ApplySegmentIDs`: a segment of the statement's own, with no append
        // record yet - the records are written when it commits.
        let opened = self.begin_import(table).await?;
        let (range, _cursor) = self.reserve_import_rows(table, rows.len(), opened).await?;
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
        let ts = self.txn.commit_at_deferred(t, ts).await?;
        let t_commit = t2.elapsed();
        let t3 = std::time::Instant::now();
        let _visible = BottomGuard::new(&self.txn, ts);
        self.note_append(table, rows.len(), true, Some(ts));
        // The published live-row count, as `INSERT` reports it: the cache
        // freshness and index coverage gates compare against it.
        self.note_live_rows(table, rows.len() as isize);
        let segment = (base >> 32) as u32;
        let imported = self.segment_rows(table, segment as usize) as u64;
        self.note_import_rows(table, segment, imported);
        self.note_import_ts(table, segment, ts);
        // The published rows and the block they land in are one step: a
        // checkpoint that ran between them would compute the block's bitmap
        // coverage from a journal that already had these rows but wrote the
        // block from the buffer before it - one row off. Take the table's
        // bottom gate (and mark the thread as its bottom, so the block writer
        // does not re-enter it).
        let gate = self.bottom_gate(table);
        let _turn = gate.lock().await;
        let key = self.table_key(table);
        // The gate is held for the whole block below, so this import *is* the
        // table's bottom for its duration: marked, so the block writer and the
        // index builders do not take the gate a second time.
        crate::engine::statements::HeldBottom::scope(&key, async {
            self.publish_rows(table, base, &rows)?;
            // `AppendInBlock`: the import's block is readable as soon as the
            // statement commits, exactly like the CSV path.
            if self.has_block_layout(table) {
                self.write_blocks_commit(table, ts)?;
            }
            // `PopulateIndex(…, kImport)`: the chunks are written inside the
            // import transaction, with its ts, before the rows are visible. The
            // read is unbounded so the builders see the rows this commit just
            // wrote (`with_read_ts_override`).
            crate::engine::state::with_read_ts_override_async(u64::MAX, async {
                self.populate_import_indexes(table, &[segment], ts).await;
            })
            .await;
            Ok::<(), ::common::Error>(())
        })
        .await?;
        drop(_turn);
        drop(_visible);
        // §9.2's row bound. `COPY` has a commit path of its own, and it is the
        // one statement that can put a whole segment into the spann mem index
        // at once - so it checks the bound too, here, with no gate held.
        self.dump_spann_over_capacity(table).await;
        // `COPY` is also the statement that can outrun the checkpoint timer: keep
        // the append buffer inside its budget the same way.
        self.seal_pending_over_budget(table).await;
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

    /// Stages one batch of parsed rows into the import transaction.
    ///
    /// Returns how many rows it took. The statement keeps its own segment
    /// cursor (`ImportCursor`): the table's shared counter belongs to whoever
    /// allocated last, and an `INSERT` during a long `COPY` moves it on.
    /// `SEGMENT_CAPACITY` bounds one segment - the reference seals a full
    /// segment and continues in the next, so a `COPY` of more rows than that
    /// ends up with two.
    ///
    /// Shared by the CSV reader and the binary vector stream: both produce the
    /// same literal rows and must stage them identically.
    #[allow(clippy::too_many_arguments)]
    async fn stage_import_batch(
        &self,
        table: &str,
        batch: &[Vec<Literal>],
        slots: &[Option<Vec<Option<Vec<u8>>>>],
        cursor: &mut Option<ImportCursor>,
        types_for_scan: &[String],
        segments: &mut Vec<u32>,
        batches_all: &mut Vec<usize>,
        buffered: &mut Vec<usize>,
        staging: &mut ImportStaging<'_>,
        staging_root: &std::path::Path,
    ) -> Result<usize> {
        let schema = self.table_schema(table).unwrap_or_default();
        // Resolved once per batch, not per cell: both take the catalog lock.
        let defaults = self.table_defaults(table).unwrap_or_default();
        let mut done = 0usize;
        while done < batch.len() {
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
            let take = (batch.len() - done).min(room);
            let (range, next_cursor) = match current {
                Some(cursor) => self.reserve_import_rows(table, take, cursor).await?,
                None => {
                    let opened = self.begin_import(table).await?;
                    self.reserve_import_rows(table, take, opened).await?
                }
            };
            let base = range.base;
            *cursor = Some(next_cursor);
            batches_all.push(take);
            self.note_index_insert(table, take);
            // The batch's values go into the append buffer **now**, not at the
            // commit: keeping the stored text until then meant holding every
            // row twice (its text and, after the commit, its values). On GIST
            // 1M that text is 6.6 KB per row - 6.6 GB - and the peak was
            // 13.5 GiB. The rows are stamped only when the commit succeeds
            // (`note_append` with `ts: None` here, `settle_append_at` there), so
            // a reader sees none of them until then, exactly like the
            // reference's `AppendInBlock` before `CommitBottomAppend`.
            // Row by row, straight into the buffer: a whole batch of parsed
            // values held first and copied in afterwards doubles the working
            // set of an import that is already carrying its text.
            // The blocks this run fills completely are written from their
            // slots first: no text, no `Value`, and their rows never enter the
            // append buffer. Only what is left (a block still filling, the
            // statement's tail) is published through text.
            let slot_staged =
                self.stage_complete_slot_blocks(table, done, take, base, slots, staging_root)?;
            let mut staged_all = slot_staged.clone();
            let slot_staged: std::collections::HashSet<usize> = slot_staged.into_iter().collect();
            let mut published: Vec<(usize, Vec<Value>)> = Vec::with_capacity(take);
            for (k, row) in batch[done..done + take].iter().enumerate() {
                if slot_staged.contains(&(base + k)) {
                    continue;
                }
                // A record that arrived as bytes (`fvecs`) has no literal row;
                // its cell text comes from the slot, which is the same text the
                // block writer stores. Everything else (CSV) already has one.
                let encoded = if row.is_empty() {
                    match slots.get(done + k).and_then(|s| s.as_ref()) {
                        Some(slot_bytes) => slot_to_cell_text(types_for_scan, slot_bytes),
                        None => String::new(),
                    }
                } else {
                    self.encode_row_stored(table, row, types_for_scan)?
                };
                let values = match self.parse_stored_row(
                    table,
                    &defaults,
                    types_for_scan,
                    encoded.as_bytes(),
                ) {
                    Ok(values) => values,
                    // Never drop a row because its text form would not parse:
                    // the cells are that text, which is what the writer stores.
                    Err(_) => common::row_text::split(&encoded)
                        .into_iter()
                        .map(|c| Value::Str(c.into_owned()))
                        .collect(),
                };
                published.push((base + k, values));
            }
            // One buffer call for the batch: the hook resolves the schema,
            // the coercions and the append log once per call, and paying that
            // per row made the staging loop's own bookkeeping quadratic.
            if !published.is_empty() {
                self.refresh_block_mirror_extra(table, &published)?;
            }
            // Every block this batch completed goes to the staging directory
            // now, and its rows leave the append buffer: that is what bounds an
            // import's memory. The reference does the same - one `DataBlock` per
            // `DEFAULT_BLOCK_CAPACITY` rows, written to
            // `<temp>/import<txn>/db_/tbl_/seg_/blk_/` and moved into the data
            // directory by the commit (`NewTxn::Import`).
            //
            // Without it the whole file stays resident as parsed values: GIST
            // 1M is 960 dimensions per row, so that is ~6.6 GB of text and the
            // values built from it, and the import peaked at 14.5 GiB.
            // A row the reader already has as block-file bytes (`fvecs`,
            // `bvecs`) is written from those rather than from its text: the
            // bytes are the same, and the `encode_value_with_shape` parse it
            // would have taken is skipped. `slots` is indexed by batch position
            // while `published` is keyed by row id, and a run can span a segment
            // boundary, hence the `seq - base` hop.
            let flushed =
                self.stage_complete_import_blocks_with(table, &published, staging_root, |_| None)?;
            staged_all.extend(flushed.iter().copied());
            buffered.extend(staged_all.iter().copied());
            let segment = (base >> 32) as u32;
            if !segments.contains(&segment) {
                segments.push(segment);
            }
            // Both hooks take a lock and allocate per row; on a table with no
            // secondary index or no text/vector index they find nothing eight
            // million times over.
            let secondary_targets = if self.has_secondary_indexes(table) {
                self.secondary_targets(table, &schema)
            } else {
                Vec::new()
            };
            let needs_texts = self.has_text_or_vector_indexes(table);
            let mut texts_by_row: Vec<(usize, Vec<String>)> = Vec::with_capacity(take);
            staging.note((0..take).map(|k| base + k));
            for (k, row) in batch[done..done + take].iter().enumerate() {
                self.insert_secondary_keys(&secondary_targets, base + k, row)?;
                if needs_texts {
                    texts_by_row.push((base + k, row.iter().map(|v| v.to_slt_string()).collect()));
                }
            }
            if needs_texts {
                // One batch for the full-text columns: tokenise without the
                // column lock, insert under it once.
                let text_batch: Vec<(usize, &[String])> = texts_by_row
                    .iter()
                    .map(|(seq, cells)| (*seq, cells.as_slice()))
                    .collect();
                self.update_text_indexes_batch_async(table, &text_batch, &schema, None)
                    .await;
                // The HNSW graphs take the whole run at once: `insert_batch`
                // is what carries the bucket fan-out (`InsertVecs` in the
                // reference), and feeding it row by row made a 1M-row import a
                // single-core graph build.
                let vector_batch: Vec<(usize, &[String])> = texts_by_row
                    .iter()
                    .map(|(seq, cells)| (*seq, cells.as_slice()))
                    .collect();
                self.note_hnsw_insert_batch(table, &vector_batch).await;
                for (seq, texts) in &texts_by_row {
                    self.note_ivf_insert(table, *seq as u64, texts);
                }
            }
            done += take;
        }
        Ok(done)
    }

    /// `COPY … FROM … WITH (FORMAT fvecs|bvecs)`: records are streamed and
    /// staged in `COPY_BATCH_ROWS` batches inside **one** transaction.
    ///
    /// `NewTxn::Import` is one transaction for the whole statement, so every
    /// batch stages into a single `Txn` exactly like the CSV path. What changes
    /// is residency: neither the file's bytes nor its formatted text are ever
    /// all held. A 3.8 GB GIST import used to keep the file, the text rows, the
    /// published values and the append buffer at the same time.
    async fn copy_from_binary_vectors(
        &self,
        table: &str,
        path: &std::path::Path,
        elem_width: usize,
    ) -> Result<QueryResult> {
        let types_for_scan = self.column_types(table)?;
        let mut reader = self.open_binary_vector_reader(table, path, elem_width)?;
        let t = self.txn.begin();
        let mut cursor: Option<ImportCursor> = None;
        let mut staged = 0usize;
        let mut total = 0usize;
        let mut segments: Vec<u32> = Vec::new();
        let mut batches_all: Vec<usize> = Vec::new();
        // Every row this statement put in the append buffer, so a rollback can
        // drop them.
        let mut buffered: Vec<usize> = Vec::new();
        // Where the blocks this statement completes are written until it
        // commits (`NewTxn::Import`'s `TempDir/import<txn>`).
        let staging_root = self.import_staging_root(t.id);
        let mut staging = self.stage_import(table).await;
        loop {
            // The records as their block-file bytes. A complete block is
            // written from these and never becomes text at all.
            let mut slots: Vec<Option<Vec<Option<Vec<u8>>>>> =
                Vec::with_capacity(Self::COPY_BATCH_ROWS);
            while slots.len() < Self::COPY_BATCH_ROWS {
                match reader.next_slot()? {
                    Some(slot) => slots.push(Some(slot)),
                    None => break,
                }
            }
            if slots.is_empty() {
                break;
            }
            let n = slots.len();
            // The reader gives slots; a row only needs its text when it lands
            // in the append buffer, which `stage_import_batch` decides per
            // block. `batch` therefore carries one placeholder per record.
            let batch: Vec<Vec<Literal>> = vec![Vec::new(); n];
            let staged_now = self
                .stage_import_batch(
                    table,
                    &batch,
                    &slots,
                    &mut cursor,
                    &types_for_scan,
                    &mut segments,
                    &mut batches_all,
                    &mut buffered,
                    &mut staging,
                    &staging_root,
                )
                .await?;
            total += staged_now;
            staged += staged_now;
            if n < Self::COPY_BATCH_ROWS {
                break;
            }
        }
        if staged > 0 {
            // This path has no staged text: every row went to a block through
            // its slot bytes (`stage_complete_slot_blocks`), or is the
            // statement's tail in the append buffer, which `write_blocks_import`
            // writes. An empty set is what tells `commit_import_chunk` there is
            // nothing to replay; the `?` on the commit is also what makes a
            // failure surface here instead of being swallowed.
            let no_staged_text = StagedRows::new();
            self.commit_import_chunk(table, t, &segments, &no_staged_text, &batches_all)
                .await?;
            self.note_live_rows(table, staged as isize);
            staging.committed();
        } else {
            drop(t);
        }
        Ok(QueryResult::Inserted { count: total })
    }

    /// Opens a binary vector file for record-by-record reading (`FORMAT
    /// FVECS`, `FORMAT BVECS`): each record is an `i32 dim` followed by `dim`
    /// elements, `elem_width` bytes each.
    ///
    /// The reader is pull-based rather than callback-based so the caller can
    /// `.await` between batches: a 3.8 GB GIST import must stage 8192 rows,
    /// commit their block and drop them, not hold the file's text.
    fn open_binary_vector_reader(
        &self,
        table: &str,
        path: &std::path::Path,
        elem_width: usize,
    ) -> Result<BinaryVectorReader> {
        let types = self.column_types(table)?;
        if types.len() != 1 {
            return Err(Error::Invalid(format!(
                "import: the format needs a table with exactly one vector column, got {}",
                types.len()
            )));
        }
        let declared = type_dim(&types[0]).unwrap_or(0);
        let file = match std::fs::File::open(path) {
            Ok(f) => f,
            // A missing fixture is a statement-level success with no rows,
            // like the CSV path.
            Err(_) => {
                return Ok(BinaryVectorReader {
                    inner: None,
                    declared,
                    elem_width,
                    path: path.to_path_buf(),
                })
            }
        };
        Ok(BinaryVectorReader {
            inner: Some(std::io::BufReader::with_capacity(1 << 20, file)),
            declared,
            elem_width,
            path: path.to_path_buf(),
        })
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

/// [`Engine::stage_import`]'s guard: clears the table's import marker on drop,
/// including an early return from the staging loop.
///
/// It also **undoes the index hooks** of a statement that failed: the staging
/// loop feeds every index batch by batch while it reads the file and only
/// commits at the end (`NewTxn::Import` is one transaction), so a cell that
/// turns invalid past the first batch would otherwise leave postings for rows
/// no block holds - and the next dump would write them into a chunk. The rows
/// are recorded as they are staged ([`ImportStaging::note`]) and removed here
/// unless the statement committed.
struct ImportStaging<'a> {
    engine: &'a Engine,
    key: String,
    table: String,
    /// Rows the staging loop handed to the index hooks.
    staged: Vec<usize>,
    /// Set once the import has committed: its rows are real then.
    committed: bool,
}

impl ImportStaging<'_> {
    /// Records rows this staging loop has just indexed.
    fn note(&mut self, seqs: impl IntoIterator<Item = usize>) {
        self.staged.extend(seqs);
    }

    /// The statement committed; its index entries must stay.
    fn committed(&mut self) {
        self.committed = true;
    }
}

impl Drop for ImportStaging<'_> {
    fn drop(&mut self) {
        if !self.committed && !self.staged.is_empty() {
            self.engine.rollback_index_rows(&self.table, &self.staged);
        }
        if let Some(entry) = self.engine.tables.lock().unwrap().get_mut(&self.key) {
            entry.importing = entry.importing.saturating_sub(1);
        }
    }
}

/// The cell text of a one-column embedding row, from its block-file bytes.
///
/// The inverse of what `next_slot` builds: `push_embedding_element` wrote
/// `dim` elements, and this renders them the way `encode_cell_stored` would
/// have (`[a,b,c]`, each `f32` in its shortest round-tripping form). Only the
/// rows that reach the append buffer need it.
fn slot_to_cell_text(types: &[String], slots: &[Option<Vec<u8>>]) -> String {
    let (Some(Some(slot)), Some(ty)) = (slots.first(), types.first()) else {
        return String::new();
    };
    // A scalar fixed-width column: the slot *is* the value, little-endian.
    if let Some(column_type) = crate::blocks::column_type_of(ty) {
        let width = slot.len();
        if let Some(value) = crate::blocks::decode_scalar_value(column_type, width, slot, 0) {
            return crate::blocks::mirror_cell_text(&value);
        }
        if column_type == 0 {
            return (slot.first().copied().unwrap_or(0) != 0).to_string();
        }
    }
    let Some((dim, elem)) = crate::blocks::embedding_shape(ty) else {
        return String::from_utf8_lossy(slot).into_owned();
    };
    let width = storage::definition::format::embedding_size(elem, 1).max(1);
    let mut vals: Vec<String> = Vec::with_capacity(dim);
    for i in 0..dim {
        let at = i * width;
        let Some(bytes) = slot.get(at..at + width) else {
            break;
        };
        let Some(v) = storage::definition::format::read_embedding_element(bytes, elem) else {
            break;
        };
        vals.push(if elem == 6 {
            fmt_f32_cpp(v as f32)
        } else {
            crate::engine::state::fmt_f64_cpp(v)
        });
    }
    format!("[{}]", vals.join(","))
}

/// One record at a time out of an `fvecs`/`bvecs` file.
///
/// The caller stages a batch, commits its block and asks for the next one, so
/// the whole file is never resident. `inner` is `None` when the file does not
/// exist: the reference treats a missing fixture as an empty import.
pub(crate) struct BinaryVectorReader {
    inner: Option<std::io::BufReader<std::fs::File>>,
    declared: usize,
    elem_width: usize,
    path: std::path::PathBuf,
}

impl BinaryVectorReader {
    /// The next record as both its literal row and its block-file bytes.
    ///
    /// One read, two forms, so they cannot drift apart. The text is what the
    /// append buffer and every reader use; the slot is what the block file
    /// stores, and a complete block is written from it without ever going
    /// through `Value` or `encode_value_with_shape`.
    fn next_slot(&mut self) -> Result<Option<Vec<Option<Vec<u8>>>>> {
        use std::io::Read;
        let BinaryVectorReader {
            inner,
            declared,
            elem_width,
            path,
        } = self;
        let Some(reader) = inner.as_mut() else {
            return Ok(None);
        };
        let mut header = [0u8; 4];
        match reader.read_exact(&mut header) {
            Ok(()) => {}
            Err(e) if e.kind() == std::io::ErrorKind::UnexpectedEof => return Ok(None),
            Err(e) => return Err(Error::io(path.as_path(), e)),
        }
        let dim = i32::from_le_bytes(header);
        let Ok(dim) = usize::try_from(dim) else {
            return Err(Error::Invalid(
                "import: the vector file has a negative dimension".into(),
            ));
        };
        if dim == 0 {
            return Err(Error::Invalid(
                "import: the vector file is truncated".into(),
            ));
        }
        if *declared != 0 && *declared != dim {
            return Err(Error::Invalid(format!(
                "import: the file's dimension ({dim}) does not match the column's ({})",
                declared
            )));
        }
        let mut record = vec![0u8; dim * *elem_width];
        reader
            .read_exact(&mut record)
            .map_err(|_| Error::Invalid("import: the vector file is truncated".into()))?;
        let element_type = if *elem_width == 4 { 6 } else { 1 };
        let mut slot = Vec::with_capacity(storage::definition::format::embedding_size(
            element_type,
            dim,
        ));
        for i in 0..dim {
            let at = i * *elem_width;
            let value = if *elem_width == 4 {
                f64::from(f32::from_le_bytes(
                    record[at..at + 4].try_into().expect("4 bytes"),
                ))
            } else {
                f64::from(record[at])
            };
            storage::definition::format::push_embedding_element(&mut slot, element_type, value);
        }
        Ok(Some(vec![Some(slot)]))
    }
}
