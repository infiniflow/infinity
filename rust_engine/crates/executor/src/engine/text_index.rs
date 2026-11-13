use super::*;
use storage::invertedindex::chunk_manifest::ChunkManifest;

/// Sorts a posting list by row id and collapses repeated rows, keeping the
/// **first** entry of each row. An UPDATE appends the rewritten row's terms
/// without removing the old entry (the old one must keep counting toward `df`
/// until compaction), so the query path has to collapse the duplicate; the
/// inputs are ordered newest source first (memory tail, spilled runs, sealed
/// segments), so keeping the first entry keeps the current text.
pub(crate) fn dedupe_postings(list: &mut Vec<(u64, u32)>) {
    // A stable sort: within one row the incoming order (newest first) survives.
    list.sort_by_key(|(row, _)| *row);
    let mut out: Vec<(u64, u32)> = Vec::with_capacity(list.len());
    for (row, tf) in list.drain(..) {
        match out.last() {
            Some((last, _)) if *last == row => {}
            _ => out.push((row, tf)),
        }
    }
    *list = out;
}

/// Merges duplicate rows in a `(row, tf, positions)` posting list, keeping the
/// last frequency and the union of the positions (the seal path).
pub(crate) fn dedupe_posting_entries(list: &mut Vec<(u64, u32, Vec<u32>)>) {
    list.sort_unstable_by_key(|(row, _, _)| *row);
    let mut out: Vec<(u64, u32, Vec<u32>)> = Vec::with_capacity(list.len());
    for (row, tf, mut positions) in list.drain(..) {
        match out.last_mut() {
            Some((last, last_tf, last_positions)) if *last == row => {
                *last_tf = tf;
                last_positions.append(&mut positions);
                last_positions.sort_unstable();
                last_positions.dedup();
            }
            _ => out.push((row, tf, positions)),
        }
    }
    *list = out;
}

/// Removes a spill run's transient segment files.
pub(crate) fn remove_run_files(dir: &str, base_name: &str) {
    // Through the store, so a persistence profile forgets the object parts as
    // well instead of leaving them reachable by key.
    let store = storage::chunk_store::chunk_store();
    for suffix in [
        storage::invertedindex::format::FULLTEXT_DICT_SUFFIX,
        storage::invertedindex::format::FULLTEXT_POSTING_SUFFIX,
        storage::invertedindex::format::FULLTEXT_LENGTH_SUFFIX,
    ] {
        // Best effort: a spill run nothing references any more, and the next
        // attempt removes whatever this one missed.
        let _ = store.remove(std::path::Path::new(
            &storage::invertedindex::format::fulltext_chunk_file(dir, base_name, suffix),
        ));
    }
    // The publish marker goes with them, or a later chunk that reuses this base
    // name would be validated against the lengths of the one just removed.
    let _ =
        store.remove(&storage::invertedindex::chunk_manifest::ChunkManifest::path(dir, base_name));
}

/// One spilled sorted run: the memory tail written as the regular segment file
/// family under a transient base name, then read back through a
/// [`storage::invertedindex::disk_segment_reader::SegmentIndexReader`].
///
/// C++: one run of the spill file `MemoryIndexer::CommitOffline` appends
/// (`ColumnInverter::SpillSortResults`, `memory_indexer_impl.cpp:406-412`).
/// The port writes a whole sorted run at a time through the sealed-segment
/// writer, so a run carries its position lists as well and `SortMerger` has
/// nothing to re-sort — only lists from different runs to merge.
pub(crate) struct SpilledRun {
    /// Monotonic run id: a later run holds the newer copy of a re-inserted row.
    pub(crate) id: u64,
    /// Directory holding the run's files, for cleanup.
    pub(crate) dir: String,
    /// Base name of the run's `.dic` / `.pos` / `.len` files.
    pub(crate) base_name: String,
    /// `(row id, length)` ascending, for document lengths and counts.
    pub(crate) docs: Vec<(u64, u32)>,
    /// Sum of the run's document lengths.
    pub(crate) total_len: u64,
    /// The run's postings, decoded per term on demand.
    pub(crate) reader: storage::invertedindex::disk_segment_reader::SegmentIndexReader,
}

impl SpilledRun {
    pub(crate) fn doc_len(&self, row: u64) -> Option<u32> {
        self.docs
            .binary_search_by_key(&row, |(r, _)| *r)
            .ok()
            .map(|i| self.docs[i].1)
    }

    pub(crate) fn remove(&self) {
        remove_run_files(&self.dir, &self.base_name);
    }
}

/// The reference's memory indexer: the **unsealed** segment of one text
/// column, holding `term -> [(doc, tf, positions)]`, each document's length
/// and the corpus statistics.
///
/// C++: `MemoryIndexer` (`src/storage/invertedindex/memory_indexer.cppm`).
///
/// Identity hashing for the document-length maps: their keys are row ids, and
/// the maps are probed once per posting on the query path - SipHash there cost
/// more than the lookup it protects.
#[derive(Default)]
pub(crate) struct IdHasher(pub(crate) u64);

/// `row id -> document length`, with [`IdHasher`].
pub(crate) type DocLenMap = HashMap<u64, u32, std::hash::BuildHasherDefault<IdHasher>>;

/// rustc's FxHash: a rotate-xor-multiply hash that is much cheaper than SipHash
/// for the short string keys the inverted index is keyed by.
#[derive(Default)]
pub(crate) struct FxHasher {
    pub(crate) hash: u64,
}

impl FxHasher {
    pub(crate) const SEED: u64 = 0x51_7c_c1_b7_27_22_0a_95;

    #[inline]
    pub(crate) fn add(&mut self, word: u64) {
        self.hash = (self.hash.rotate_left(5) ^ word).wrapping_mul(Self::SEED);
    }
}

/// A hash map for term-keyed state, with [FxHasher].
pub(crate) type FastMap<K, V> = HashMap<K, V, std::hash::BuildHasherDefault<FxHasher>>;

/// One term's postings in the unsealed segment: the (row, tf) pairs plus their
/// positions in an append-only arena.
///
/// A per-posting Vec of positions cost one allocation per posting - millions
/// per import - so the positions are appended to one buffer per term instead.
/// A re-indexed row appends its new positions rather than overwriting them,
/// which keeps every existing offset valid.
#[derive(Default)]
pub(crate) struct TermPostings {
    /// (row, tf, first position in the arena, position count)
    pub(crate) list: Vec<(u64, u32, u32, u32)>,
    pub(crate) pos: Vec<u32>,
}

impl TermPostings {
    pub(crate) fn positions(&self, at: usize) -> &[u32] {
        match self.list.get(at) {
            Some((_, _, start, count)) => self
                .pos
                .get(*start as usize..(*start + *count) as usize)
                .unwrap_or(&[]),
            None => &[],
        }
    }

    /// Appends a posting's positions and returns their (start, count).
    pub(crate) fn append_positions(&mut self, positions: &[u32]) -> (u32, u32) {
        let start = self.pos.len() as u32;
        self.pos.extend_from_slice(positions);
        (start, positions.len() as u32)
    }
}

#[derive(Default)]
pub(crate) struct MemoryIndexer {
    /// `doc -> length`, used for BM25's document length.
    pub(crate) docs: DocLenMap,
    /// One entry per interned term, indexed by its id: a document's tokens cost
    /// an integer lookup instead of hashing the term string per posting.
    pub(crate) postings: Vec<TermPostings>,
    /// `term id -> row -> doc payload`, parallel to `postings`. Only a
    /// `rankfeatures` index fills it: the analyzer's
    /// `SmallFloat::Float122ToUInt16` weight per `(term, document)`, which
    /// becomes the posting's doc-payload column
    /// (`write_segment_index_with_payloads`). Every other analyzer leaves the
    /// maps empty, so the cost is one empty `BTreeMap` per term.
    pub(crate) payloads: Vec<std::collections::BTreeMap<u64, u16>>,
    /// `term -> id` and `id -> term`. The dictionary outlives the tail's postings
    /// (a spill keeps using the same ids) and is rebuilt from the rows when a
    /// process opens a directory.
    pub(crate) term_ids: FastMap<String, u32>,
    pub(crate) term_names: Vec<String>,
    pub(crate) live: std::collections::HashSet<u64>,
    /// Rows this tail is responsible for and that were deleted after their text
    /// reached it (the resident postings or a spilled run).
    ///
    /// [`Self::delete`] cannot drop a posting from the in-memory lists or from a
    /// run written to disk, and a run's rows are gone from [`Self::live`] once
    /// they spill, so the seal path needs its own record to leave them out of
    /// the chunk it writes. Inserting the row again clears its entry: the new
    /// text is what the tail holds.
    pub(crate) deleted: std::collections::HashSet<u64>,
    pub(crate) n_docs: usize,
    pub(crate) total_len: u64,
    /// Approximate bytes held by this segment, for the memindex budget.
    pub(crate) bytes: usize,
    /// The row range the tail covers, for the query path's concat test: the
    /// tail appends after every sealed segment, so their ranges are disjoint
    /// unless a row was re-indexed (then the overridden set is non-empty).
    pub(crate) row_range: Option<(u64, u64)>,
    /// The sorted runs the tail was spilled into, oldest first. Their postings
    /// stay on disk and are read on demand, so a column that outgrows the
    /// budget keeps indexing instead of going dark.
    pub(crate) runs: Vec<SpilledRun>,
    /// Id of the next spill file; monotonic, so a later run sorts after an
    /// earlier one even when the tail holds fewer documents.
    pub(crate) next_run_id: u64,
    /// `row -> id of the newest run holding its current text`, only for rows
    /// that were re-inserted after they were spilled (an UPDATE rewrites the
    /// row id). [`IN_TAIL`] means the current copy sits in the memory tail.
    /// Entries from an older run are stale and are skipped, which is how an
    /// out-of-order re-insert (`InsertGap`) keeps the runs consistent.
    pub(crate) newest: HashMap<u64, u64>,
    /// A spilled segment loaded back from disk (`MemoryIndexer::Load`). Its
    /// postings are read on demand: the on-disk form stores frequencies but no
    /// position lists (see `fulltext_segment`'s module docs).
    pub(crate) spilled: Option<storage::invertedindex::disk_segment_reader::SegmentIndexReader>,
}

/// [`MemoryIndexer::newest`] marker: the row's current copy is in the memory
/// tail, so every spilled run holds a stale one.
pub(crate) const IN_TAIL: u64 = u64::MAX;

/// One document, tokenised but **not yet interned**: what [`prepare_doc`]
/// produces outside the column's write lock and [`MemoryIndexer::insert_prepared`]
/// consumes inside it.
///
/// The terms are owned `String`s (rather than the analyzer's borrowed views)
/// because they have to outlive the call that produced them; a batch therefore
/// holds one `String` per posting, which is why the caller prepares in bounded
/// batches instead of a whole import at once.
pub(crate) struct PreparedDoc {
    pub(crate) seq: u64,
    pub(crate) terms: Vec<(String, u32, u16)>,
    /// Bytes the document will add to the tail: `~64 B` per posting plus the
    /// term text, the same accounting [`MemoryIndexer::insert`] always used.
    pub(crate) cost: usize,
}

/// Tokenises one document for a [`TextIndex`]. Takes no index state, so it runs
/// without the write lock - the analyzer is the only input.
pub(crate) fn prepare_doc(seq: u64, text: &str, tokenizer: &TextTokenizer) -> PreparedDoc {
    let mut terms: Vec<(String, u32, u16)> = Vec::new();
    let mut cost = 64usize;
    for_each_index_term(text, tokenizer, |term, pos, payload| {
        cost += term.len() + 32;
        terms.push((term.to_string(), pos, payload));
    });
    PreparedDoc { seq, terms, cost }
}

impl MemoryIndexer {
    pub(crate) fn new() -> Self {
        Self::default()
    }

    /// C++: `MemoryIndexer::Insert` for one document. Returns `false` when the
    /// document would push the segment past `budget`, leaving it unchanged.
    ///
    /// The write path tokenises outside the lock and calls
    /// [`Self::insert_prepared`]; this is the one-document convenience the unit
    /// tests use.
    #[cfg(test)]
    pub(crate) fn insert(
        &mut self,
        seq: u64,
        text: &str,
        tokenizer: &TextTokenizer,
        budget: usize,
    ) -> bool {
        let doc = prepare_doc(seq, text, tokenizer);
        self.insert_prepared(&doc, budget)
    }

    /// [`Self::insert`] with the document already tokenised. The caller can run
    /// [`prepare_doc`] without holding the column's write lock, which is what
    /// turns a batch into "tokenise N documents, then take the lock once".
    ///
    /// Returns `false` when the document would push the segment past `budget`,
    /// leaving it unchanged (the caller spills and retries).
    pub(crate) fn insert_prepared(&mut self, doc: &PreparedDoc, budget: usize) -> bool {
        if self.bytes + doc.cost > budget {
            return false;
        }
        let seq = doc.seq;
        // The row is live again: whatever it held before the delete is stale.
        self.deleted.remove(&seq);
        let mut toks: Vec<(u32, u32, u16)> = Vec::with_capacity(doc.terms.len());
        for (term, pos, payload) in &doc.terms {
            let id = match self.term_ids.get(term.as_str()) {
                Some(id) => *id,
                None => {
                    let id = self.term_names.len() as u32;
                    self.term_names.push(term.clone());
                    self.term_ids.insert(term.clone(), id);
                    self.postings.push(TermPostings::default());
                    self.payloads.push(std::collections::BTreeMap::new());
                    id
                }
            };
            toks.push((id, *pos, *payload));
        }
        self.bytes += doc.cost;
        // A re-indexed row (UPDATE rewrites in place) replaces its length
        // rather than adding a second document to the corpus statistics.
        match self.docs.insert(seq, toks.len() as u32) {
            Some(old) => self.total_len = self.total_len.saturating_sub(old as u64),
            None => self.n_docs += 1,
        }
        self.row_range = Some(match self.row_range {
            None => (seq, seq),
            Some((lo, hi)) => (lo.min(seq), hi.max(seq)),
        });
        self.total_len += toks.len() as u64;
        self.live.insert(seq);
        // A row that is rewritten after it was spilled leaves a stale copy in
        // an older run; mark the tail as its current source.
        if !self.runs.is_empty() && self.run_containing(seq).is_some() {
            self.newest.insert(seq, IN_TAIL);
        }
        // Group the document's tokens by term without a hash map: sorting once
        // and walking the runs costs no allocation, where the per-document map
        // and its per-term `Vec`s were ~10% of an import (measured, see
        // `docs/FULLTEXT_BENCH.md` §8.1). The compare includes the position so
        // equal terms come out ascending, which the position encoder relies on.
        toks.sort_unstable_by(|a, b| a.0.cmp(&b.0).then(a.1.cmp(&b.1)));
        let mut positions: Vec<u32> = Vec::new();
        let mut at = 0usize;
        while at < toks.len() {
            let first = at;
            while at < toks.len() && toks[at].0 == toks[first].0 {
                at += 1;
            }
            let id = toks[first].0 as usize;
            // The `rankfeatures` weight of this `(term, document)`; a re-indexed
            // row replaces its old value (or clears it when the new text carries
            // no payload).
            let payload = toks[first].2;
            if payload != 0 {
                self.payloads[id].insert(seq, payload);
            } else {
                self.payloads[id].remove(&seq);
            }
            positions.clear();
            positions.extend(toks[first..at].iter().map(|entry| entry.1));
            let list = &mut self.postings[id];
            let (start, count) = list.append_positions(&positions);
            let entry = (seq, positions.len() as u32, start, count);
            // Keep the list ascending and unique: the query path walks it as a
            // sorted posting source, and a re-indexed row replaces its copy
            // instead of showing up twice.
            match list.list.last().map(|p| p.0) {
                None => list.list.push(entry),
                Some(last) if last < seq => list.list.push(entry),
                Some(last) if last == seq => {
                    *list.list.last_mut().expect("the list is not empty") = entry
                }
                _ => match list.list.binary_search_by_key(&seq, |p| p.0) {
                    Ok(i) => list.list[i] = entry,
                    Err(i) => list.list.insert(i, entry),
                },
            }
        }
        true
    }

    /// The tail's postings for a term, if the term was ever indexed here.
    pub(crate) fn postings_of(&self, term: &str) -> Option<&TermPostings> {
        self.term_ids
            .get(term)
            .map(|id| &self.postings[*id as usize])
    }

    /// C++: deleting a document only drops its live marker; the postings and
    /// length stay until the segment is compacted.
    ///
    /// The row is also recorded as deleted, so the seal that writes this tail
    /// leaves its postings out of the chunk instead of publishing a document no
    /// predicate can match.
    pub(crate) fn delete(&mut self, seq: u64) {
        self.live.remove(&seq);
        self.deleted.insert(seq);
    }

    /// C++: `GetDocCount` of the resident tail. The spilled runs are counted by
    /// [`Self::pending_docs`], because the segment they will be merged into has
    /// not been written yet.
    #[allow(dead_code)] // the unit tests read the tail's count
    pub(crate) fn doc_count(&self) -> usize {
        self.n_docs
    }

    /// Documents the next sealed segment will hold: the tail plus every run,
    /// minus the rows deleted since their text reached it.
    pub(crate) fn pending_docs(&self) -> usize {
        let mut docs = self.n_docs + self.runs.iter().map(|run| run.docs.len()).sum::<usize>();
        if !self.deleted.is_empty() {
            docs -= self
                .deleted
                .iter()
                .filter(|row| self.doc_len_of(**row).is_some())
                .count();
        }
        docs
    }

    /// Token count the next sealed segment will hold, over the live documents.
    pub(crate) fn pending_len(&self) -> u64 {
        let mut total = self.total_len + self.runs.iter().map(|run| run.total_len).sum::<u64>();
        if !self.deleted.is_empty() {
            for row in &self.deleted {
                if let Some(len) = self.doc_len_of(*row) {
                    total = total.saturating_sub(u64::from(len));
                }
            }
        }
        total
    }

    /// C++: `MemUsed` — the resident bytes, i.e. the tail's. The runs live on
    /// disk, so they do not count against the budget.
    pub(crate) fn mem_used(&self) -> usize {
        self.bytes
    }

    /// Drops the tail, keeping the spilled runs.
    pub(crate) fn clear_tail(&mut self) {
        self.postings.clear();
        self.postings.shrink_to_fit();
        self.payloads.clear();
        self.payloads.shrink_to_fit();
        self.term_ids.clear();
        self.term_names.clear();
        self.term_names.shrink_to_fit();
        self.docs.clear();
        self.live.clear();
        self.row_range = None;
        self.bytes = 0;
        self.n_docs = 0;
        self.total_len = 0;
    }

    /// C++: `MemoryIndexer::Reset` — drops the segment and its spill files.
    pub(crate) fn reset(&mut self) {
        self.clear_tail();
        self.spilled = None;
        self.newest.clear();
        // The runs go with it, so nothing is left for the record to describe.
        self.deleted.clear();
        for run in self.runs.drain(..) {
            run.remove();
        }
    }

    /// The newest run whose documents include `row`, if any. Runs only hold the
    /// rows written since the previous spill, so this is looked up only when the
    /// column has spilled at all (a handful of runs in practice).
    pub(crate) fn run_containing(&self, row: u64) -> Option<u64> {
        self.runs
            .iter()
            .rev()
            .find(|run| run.doc_len(row).is_some())
            .map(|run| run.id)
    }

    /// `false` when a newer source already holds `row`'s current text, i.e. the
    /// run's entry is a stale copy of a re-inserted document.
    pub(crate) fn run_entry_is_current(&self, run_id: u64, row: u64) -> bool {
        match self.newest.get(&row) {
            Some(newest) => *newest <= run_id && *newest != IN_TAIL,
            None => true,
        }
    }

    /// C++: the spill half of `MemoryIndexer::Dump` (`Dump(false, spill = true)`)
    /// and `CommitOffline` — writes the resident tail as one sorted run under a
    /// transient base name and resets the budget accounting, so indexing
    /// continues past the budget instead of stopping.
    ///
    /// Returns `false`, leaving the tail untouched, when the run cannot be
    /// written (no directory, disk error): the caller then keeps the documents
    /// in memory rather than lose them.
    pub(crate) fn spill(&mut self, dir: &str) -> bool {
        let Some(docs) = self.tail_docs() else {
            return false;
        };
        let total_len: u64 = docs.iter().map(|(_, len)| u64::from(*len)).sum();
        let base_row_id = docs[0].0;
        let base_name = format!(
            "spill_{:08x}{}",
            self.next_run_id,
            storage::invertedindex::format::FULLTEXT_SPILL_SUFFIX
        );
        let flags = storage::invertedindex::format::OPTION_FLAG_ALL;
        // A spill run is the realtime dump written sideways: the tail's own
        // postings, streamed, with no merge involved.
        if storage::invertedindex::disk_segment_reader::write_segment_index_streaming(
            dir,
            &base_name,
            base_row_id,
            &docs,
            flags,
            &mut |emit| {
                self.for_each_tail_term(|term, hits, table| {
                    let payload = |row: u64| table.get(&row).copied().unwrap_or(0);
                    emit(term, hits, &payload);
                });
            },
        )
        .is_err()
        {
            return false;
        }
        let Ok(reader) = storage::invertedindex::disk_segment_reader::SegmentIndexReader::open(
            dir,
            &base_name,
            base_row_id,
            flags,
        ) else {
            remove_run_files(dir, &base_name);
            return false;
        };
        let run_id = self.next_run_id;
        self.runs.push(SpilledRun {
            id: run_id,
            dir: dir.to_string(),
            base_name,
            docs,
            total_len,
            reader,
        });
        self.next_run_id += 1;
        // The rows that were waiting for a newer copy have one now: this run.
        let spilled: Vec<u64> = self
            .runs
            .last()
            .map(|run| run.docs.iter().map(|(row, _)| *row).collect())
            .unwrap_or_default();
        for row in spilled {
            if self.newest.contains_key(&row) {
                self.newest.insert(row, run_id);
            }
        }
        self.clear_tail();
        true
    }

    /// One term's `(row, tf, positions)` list out of a sealed segment or a
    /// spill run, positions included (`Lookup(term, fetch_position = true)`).
    ///
    /// `positions` already decodes the document list to delimit each document's
    /// occurrences, so its length *is* the term frequency and the second lookup
    /// is only needed for a term written without a position list.
    pub(crate) fn reader_posting_entries(
        reader: &storage::invertedindex::disk_segment_reader::SegmentIndexReader,
        term: &str,
    ) -> Option<Vec<storage::invertedindex::disk_segment_reader::PostingEntry>> {
        if let Ok(Some(list)) = reader.positions(term) {
            if !list.is_empty() {
                return Some(
                    list.into_iter()
                        .map(|(row, positions)| (row, positions.len() as u32, positions))
                        .collect(),
                );
            }
        }
        let hits = reader.postings(term).ok()?;
        if hits.is_empty() {
            return None;
        }
        Some(
            hits.into_iter()
                .map(|(row, tf)| (row, tf, Vec::new()))
                .collect(),
        )
    }

    /// The `of_doc_payload` column of one term, keyed by row id and with the
    /// zero entries dropped (zero is "no payload"). Empty unless the index was
    /// built by the `rankfeatures` analyzer.
    fn reader_payload_map(
        reader: &storage::invertedindex::disk_segment_reader::SegmentIndexReader,
        term: &str,
    ) -> std::collections::BTreeMap<u64, u16> {
        reader
            .doc_payloads(term)
            .unwrap_or_default()
            .into_iter()
            .filter(|(_, payload)| *payload != 0)
            .collect()
    }

    /// The resident tail's document lengths, ascending by row: the input of a
    /// realtime dump and of a spill run (`Dump(offline = false, spill = true)`).
    pub(crate) fn tail_docs(&self) -> Option<Vec<(u64, u32)>> {
        if self.docs.is_empty() {
            return None;
        }
        let mut docs: Vec<(u64, u32)> = self.docs.iter().map(|(row, len)| (*row, *len)).collect();
        // A deleted row is not a document of the segment this tail is about to
        // become: keeping it would publish text no predicate can match and count
        // it in BM25's `N`.
        docs.retain(|(row, _)| !self.deleted.contains(row));
        if docs.is_empty() {
            return None;
        }
        docs.sort_unstable_by_key(|(row, _)| *row);
        Some(docs)
    }

    /// The first row id the unsealed segment holds - the tail, its spill runs or
    /// a spill loaded back from disk. `None` when it holds nothing.
    ///
    /// One chunk is written under a single `base_row_id` and its `.len` covers
    /// `base_row_id..=last_row`, so the unsealed segment must not mix rows of two
    /// `ROW_ID` segments (they are 2^32 apart); the caller compares
    /// [`Self::unsealed_first_row`]` >> 32` with the segment of the rows it is
    /// about to insert and seals the old tail first.
    pub(crate) fn unsealed_first_row(&self) -> Option<u64> {
        let from_runs = self
            .runs
            .iter()
            .filter_map(|run| run.docs.first().map(|(row, _)| *row))
            .min();
        let from_spill = self
            .spilled
            .as_ref()
            .and_then(|reader| reader.row_range().map(|(first, _)| first));
        let from_tail = self.row_range.map(|(first, _)| first);
        [from_tail, from_runs, from_spill]
            .into_iter()
            .flatten()
            .min()
    }

    /// The merged document lengths, oldest source first so the newest write wins.
    /// `None` when the unsealed segment holds nothing.
    pub(crate) fn merged_docs(&self) -> Option<Vec<(u64, u32)>> {
        if self.docs.is_empty() && self.runs.is_empty() && self.spilled.is_none() {
            return None;
        }
        let mut lengths: std::collections::BTreeMap<u64, u32> = std::collections::BTreeMap::new();
        if let Some(reader) = &self.spilled {
            for row in reader.rows() {
                if let Some(len) = reader.doc_len_of_row(row) {
                    lengths.insert(row, len);
                }
            }
        }
        for run in &self.runs {
            lengths.extend(run.docs.iter().copied());
        }
        lengths.extend(self.docs.iter().map(|(row, len)| (*row, *len)));
        // Deleted rows are left out of the chunk the seal is about to write, so
        // they must not decide its `base_row_id`/span either.
        if !self.deleted.is_empty() {
            lengths.retain(|row, _| !self.deleted.contains(row));
        }
        Some(lengths.into_iter().collect())
    }

    /// The **realtime** dump path (`MemoryIndexer::Dump(offline=false)`): the
    /// resident tail written straight out, terms in dictionary order.
    ///
    /// C++ keeps the online postings in a `std::map` (`GetOrAddPosting` is
    /// `postings_`), so they are already ordered when `Dump` walks them; the tail
    /// here is insertion ordered, so the indices are sorted instead (the names
    /// are compared in place, never copied). No spill file and no merge is
    /// involved — this is the path a table with no spill run seals through.
    pub(crate) fn for_each_tail_term(
        &self,
        mut f: impl FnMut(
            &str,
            &[storage::invertedindex::disk_segment_reader::PostingEntry],
            &std::collections::BTreeMap<u64, u16>,
        ),
    ) {
        let mut order: Vec<usize> = (0..self.term_names.len()).collect();
        order.sort_unstable_by(|a, b| self.term_names[*a].cmp(&self.term_names[*b]));
        for id in order {
            let list = &self.postings[id];
            if list.list.is_empty() {
                continue;
            }
            let mut hits: Vec<storage::invertedindex::disk_segment_reader::PostingEntry> = list
                .list
                .iter()
                .enumerate()
                .map(|(i, (row, tf, _, _))| (*row, *tf, list.positions(i).to_vec()))
                .collect();
            // A row re-indexed inside the tail appears twice; the segment holds
            // exactly one list per document.
            dedupe_posting_entries(&mut hits);
            if !self.deleted.is_empty() {
                hits.retain(|(row, _, _)| !self.deleted.contains(row));
            }
            if hits.is_empty() {
                continue;
            }
            let empty = std::collections::BTreeMap::new();
            let payloads = self.payloads.get(id).unwrap_or(&empty);
            f(&self.term_names[id], &hits, payloads);
        }
    }

    /// C++: `SortMerger` (`OfflineDump`, `memory_indexer_impl.cpp:792-812`) —
    /// merges the spilled runs and the resident tail (plus a `Load`ed spill
    /// segment) into one posting stream in ascending term order.
    ///
    /// The reference merges k sorted term tuples, so only one term's postings
    /// exist at a time. This walks the same order and holds only the term
    /// *names* for the whole merge, so sealing a large tail never materialises
    /// the segment's positions.
    ///
    /// Sources are ranked oldest to newest and a re-inserted row keeps only its
    /// newest entry, so a row that was spilled and then rewritten ends up with
    /// exactly one posting list (the `InsertGap` case: an out-of-order or
    /// re-inserted document must not leave a stale copy in a run).
    pub(crate) fn for_each_merged_term(
        &self,
        mut f: impl FnMut(
            &str,
            &[storage::invertedindex::disk_segment_reader::PostingEntry],
            &std::collections::BTreeMap<u64, u16>,
        ),
    ) {
        let tail_rank = self.runs.len() as u32 + 1;
        // The union of the sources' dictionaries, in ascending byte order -
        // the order the FST builder requires.
        let mut names: std::collections::BTreeSet<String> = std::collections::BTreeSet::new();
        if let Some(reader) = &self.spilled {
            names.extend(reader.terms().unwrap_or_default());
        }
        for run in &self.runs {
            names.extend(run.reader.terms().unwrap_or_default());
        }
        names.extend(self.term_names.iter().cloned());
        for term in names {
            // `spilled` (a `Load`ed dump) is the oldest source, then the runs in
            // creation order, then the tail.
            let mut entries: Vec<SourcedEntry> = Vec::new();
            if let Some(reader) = &self.spilled {
                if let Some(hits) = Self::reader_posting_entries(reader, &term) {
                    let payloads = Self::reader_payload_map(reader, &term);
                    entries.extend(hits.into_iter().map(|(row, tf, pos)| {
                        (0, row, tf, pos, *payloads.get(&row).unwrap_or(&0))
                    }));
                }
            }
            for (i, run) in self.runs.iter().enumerate() {
                let rank = i as u32 + 1;
                let Some(hits) = Self::reader_posting_entries(&run.reader, &term) else {
                    continue;
                };
                let payloads = Self::reader_payload_map(&run.reader, &term);
                entries.extend(
                    hits.into_iter()
                        // A re-inserted row's older copy is a stale posting: the
                        // segment must hold exactly one list per document.
                        .filter(|(row, _, _)| self.run_entry_is_current(run.id, *row))
                        .map(|(row, tf, pos)| {
                            (rank, row, tf, pos, *payloads.get(&row).unwrap_or(&0))
                        }),
                );
            }
            if let Some(id) = self.term_ids.get(&term) {
                let id = *id as usize;
                let list = &self.postings[id];
                let tail_payloads = self.payloads.get(id);
                let positions: Vec<(u64, u32, &[u32])> = list
                    .list
                    .iter()
                    .enumerate()
                    .map(|(i, (row, tf, _, _))| (*row, *tf, list.positions(i)))
                    .collect();
                entries.extend(positions.into_iter().map(|(row, tf, pos)| {
                    (
                        tail_rank,
                        row,
                        tf,
                        pos.to_vec(),
                        tail_payloads
                            .and_then(|map| map.get(&row).copied())
                            .unwrap_or(0),
                    )
                }));
            }
            if !entries.is_empty() {
                let (mut hits, table) = dedupe_sourced_entries(entries);
                // Rows deleted after their text reached this tail (or one of its
                // runs) are not documents of the chunk being written.
                if !self.deleted.is_empty() {
                    hits.retain(|(row, _, _)| !self.deleted.contains(row));
                }
                if !hits.is_empty() {
                    f(&term, &hits, &table);
                }
            }
        }
    }

    /// `row -> payload` for `term`, oldest source first so the newest write
    /// wins: a `Load`ed spill segment, then the spill runs, then the memory
    /// tail. Only a `rankfeatures` column has entries.
    pub(crate) fn doc_payloads(&self, term: &str) -> std::collections::BTreeMap<u64, u16> {
        let mut out = std::collections::BTreeMap::new();
        let mut absorb =
            |reader: &storage::invertedindex::disk_segment_reader::SegmentIndexReader| {
                out.extend(
                    reader
                        .doc_payloads(term)
                        .unwrap_or_default()
                        .into_iter()
                        .filter(|(_, payload)| *payload != 0),
                );
            };
        if let Some(reader) = &self.spilled {
            absorb(reader);
        }
        for run in &self.runs {
            absorb(&run.reader);
        }
        if let Some(id) = self.term_ids.get(term) {
            if let Some(map) = self.payloads.get(*id as usize) {
                out.extend(map.iter().map(|(row, payload)| (*row, *payload)));
            }
        }
        out
    }

    /// C++: `GetColumnLength` for the unsealed segment — the tail wins over the
    /// runs, the runs over each other newest first, and a stale copy of a
    /// re-inserted row is ignored.
    pub(crate) fn doc_len_of(&self, row: u64) -> Option<u32> {
        if let Some(len) = self.docs.get(&row) {
            return Some(*len);
        }
        self.runs
            .iter()
            .rev()
            .filter(|run| self.run_entry_is_current(run.id, row))
            .find_map(|run| run.doc_len(row))
    }

    /// The term's `(row, tf)` list from the unsealed segment, newest source
    /// first: the tail, then the runs from the newest to the oldest. A stale
    /// copy in an older run is dropped, so a term that only lived in the old
    /// text of a re-inserted row disappears.
    pub(crate) fn postings_newest_first(&self, term: &str) -> Vec<(u64, u32)> {
        let mut hits: Vec<(u64, u32)> = self
            .postings_of(term)
            .map(|list| {
                list.list
                    .iter()
                    .map(|(row, tf, _, _)| (*row, *tf))
                    .collect()
            })
            .unwrap_or_default();
        for run in self.runs.iter().rev() {
            let Ok(list) = run.reader.postings(term) else {
                continue;
            };
            hits.extend(
                list.into_iter()
                    .filter(|(row, _)| self.run_entry_is_current(run.id, *row)),
            );
        }
        hits
    }

    /// C++: `MemoryIndexer::Dump(false, spill)` — writes the segment as the
    /// C++ file family ([`storage::invertedindex::disk_segment_reader::write_segment_index`]).
    /// Positions travel with each posting (`.pos`), so a sealed segment can
    /// answer phrase queries. The spilled runs are merged in first.
    #[allow(dead_code)] // the product spill path; exercised by `memory_indexer_ut`.
    pub(crate) fn dump(&self, dir: &str, base_name: &str) -> bool {
        let Some(docs) = self.merged_docs() else {
            return false;
        };
        if docs.is_empty() {
            return false;
        }
        // Same fork as `seal`: `Dump(offline = false)` vs `OfflineDump()`.
        let realtime = self.runs.is_empty() && self.spilled.is_none();
        storage::invertedindex::disk_segment_reader::write_segment_index_streaming(
            dir,
            base_name,
            docs[0].0,
            &docs,
            storage::invertedindex::format::OPTION_FLAG_ALL,
            &mut |emit| {
                if realtime {
                    self.for_each_tail_term(|term, hits, table| {
                        let payload = |row: u64| table.get(&row).copied().unwrap_or(0);
                        emit(term, hits, &payload);
                    });
                } else {
                    self.for_each_merged_term(|term, hits, table| {
                        let payload = |row: u64| table.get(&row).copied().unwrap_or(0);
                        emit(term, hits, &payload);
                    });
                }
            },
        )
        .is_ok()
    }

    /// C++: `MemoryIndexer::Load` — reads a spilled segment back. Document
    /// lengths and live rows are restored eagerly; postings stay in the
    /// reader and are served by [`Self::segment_posting`].
    #[allow(dead_code)] // the product spill path; exercised by `memory_indexer_ut`.
    pub(crate) fn load(&mut self, dir: &str, base_name: &str) -> bool {
        let flags = storage::invertedindex::format::OPTION_FLAG_ALL;
        let Ok(reader) = storage::invertedindex::disk_segment_reader::SegmentIndexReader::open(
            dir, base_name, 0, flags,
        ) else {
            return false;
        };
        self.reset();
        let rows = reader.rows();
        for row in &rows {
            let len = reader.doc_len_of_row(*row).unwrap_or(0);
            self.docs.insert(*row, len);
            self.total_len += u64::from(len);
            self.live.insert(*row);
        }
        self.n_docs = rows.len();
        self.bytes = rows.len() * std::mem::size_of::<(u64, u32, Vec<u32>)>();
        self.spilled = Some(reader);
        true
    }

    /// C++: `InMemIndexSegmentReader::GetSegmentPosting` — the term's
    /// `(row id, term frequency)` list from the in-memory tail, the spill runs
    /// and/or the loaded reader (a re-inserted row wins over its older copies).
    #[allow(dead_code)] // `InMemIndexSegmentReader`; exercised by `memory_indexer_ut`.
    pub(crate) fn segment_posting(&self, term: &str) -> Option<Vec<(u64, u32)>> {
        let mut hits: Vec<(u64, u32)> = self.postings_newest_first(term);
        if let Some(reader) = &self.spilled {
            if let Ok(mut list) = reader.postings(term) {
                hits.append(&mut list);
            }
        }
        if hits.is_empty() {
            return None;
        }
        dedupe_postings(&mut hits);
        Some(hits)
    }
}

/// One posting entry tagged with the source that produced it: `(source rank,
/// row id, term frequency, positions)`. A higher rank is newer.
pub(crate) type SourcedEntry = (u32, u64, u32, Vec<u32>, u16);

/// Merges one term's entries from several sources: for a re-inserted row the
/// entry with the highest source rank wins (the newest spill run, or the
/// in-memory tail), so a stale copy in an older run is dropped. Entries from
/// the same source keep the [`dedupe_posting_entries`] rule (last frequency,
/// union of the positions), which is what a duplicate inside one tail means.
pub(crate) fn dedupe_sourced_entries(
    mut entries: Vec<SourcedEntry>,
) -> (
    Vec<storage::invertedindex::disk_segment_reader::PostingEntry>,
    std::collections::BTreeMap<u64, u16>,
) {
    entries.sort_by_key(|(rank, row, _, _, _)| (*row, *rank));
    let mut out: Vec<SourcedEntry> = Vec::with_capacity(entries.len());
    for (rank, row, tf, mut positions, payload) in entries {
        match out.last_mut() {
            Some((last_rank, last_row, last_tf, last_positions, last_payload))
                if *last_row == row =>
            {
                if rank > *last_rank {
                    *last_rank = rank;
                    *last_tf = tf;
                    *last_positions = positions;
                    *last_payload = payload;
                } else {
                    *last_tf = tf;
                    last_positions.append(&mut positions);
                    last_positions.sort_unstable();
                    last_positions.dedup();
                    if payload != 0 {
                        *last_payload = payload;
                    }
                }
            }
            _ => out.push((rank, row, tf, positions, payload)),
        }
    }
    let mut payloads = std::collections::BTreeMap::new();
    let hits = out
        .into_iter()
        .map(|(_, row, tf, positions, payload)| {
            if payload != 0 {
                payloads.insert(row, payload);
            }
            (row, tf, positions)
        })
        .collect();
    (hits, payloads)
}

/// How a full-text column's documents and queries are tokenised: the legacy
/// delimiter split (`keyword` / `whitespace`, including `whitespace-<chars>`)
/// plus, when the index was created with a real analyzer family, that analyzer.
///
/// C++: the analyzer `AnalyzerPool::GetAnalyzer` returns for the index's
/// `ANALYZER` option (`column_inverter_impl.cpp` builds the inverter with it).
/// `delims` alone could not express it: `None` doubles for both "standard" and
/// every CJK/n-gram family, which tokenised an `ik` index with the standard
/// analyzer on both the write and the query side.
#[derive(Clone, Debug, Default, PartialEq)]
pub(crate) struct TextTokenizer {
    /// `Some([])` splits on whitespace, `Some(chars)` only on `chars`,
    /// `None` uses the standard tokenizer.
    pub(crate) delims: Option<Vec<char>>,
    /// The analyzer the index was created with, when it is not one of the
    /// families `delims` already reproduces.
    pub(crate) analyzer: Option<String>,
    /// `standard-<language>`: the stemmer that replaces the English default.
    /// `None` for plain `standard` and for a suffix the reference does not
    /// know, both of which keep `STEM_LANG_ENGLISH`.
    pub(crate) stemmer: Option<analyzer::Stemmer>,
}

impl TextTokenizer {
    pub(crate) fn new(delims: Option<Vec<char>>, analyzer: Option<String>) -> Self {
        Self {
            delims,
            analyzer,
            stemmer: None,
        }
    }

    /// The `standard` analyzer this column tokenises with: the English default,
    /// or - for `standard-<language>` - that language's stemmer with
    /// `AnalyzerPool::GetAnalyzer`'s `standard` tokenizer config (divides
    /// `@#$`, unites `/`).
    pub(crate) fn standard(&self, stem_only: bool) -> analyzer::StandardAnalyzer {
        let mut analyzer = analyzer::StandardAnalyzer::new();
        match self.stemmer {
            Some(stemmer) => {
                let mut config = analyzer::TokenizeConfig::default();
                config.add_divides("@#$");
                config.add_unites("/");
                analyzer.set_tokenizer_config(config);
                analyzer.init_stemmer(stemmer);
            }
            None => analyzer.init_stemmer_english(),
        }
        analyzer.set_stem_only(stem_only);
        analyzer
    }

    /// A `keyword` / `whitespace` field splits only on its delimiters and
    /// scores a constant boost per matching document.
    pub(crate) fn keyword(&self) -> bool {
        matches!(self.delims, Some(ref delims) if delims.is_empty())
    }

    /// C++ `Analyzer::Analyze`: the real analyzer's terms, when one is
    /// configured **and** can be constructed (the dictionary-driven families
    /// need their resource files).
    /// Streams this column's terms when the analyzer can produce them by
    /// reference. `false` means nothing was emitted and the caller has to use
    /// [`tokenize_index_positions`].
    pub(crate) fn for_each_term(
        &self,
        text: &str,
        f: &mut dyn FnMut(analyzer::TermView<'_>),
    ) -> bool {
        if let Some(name) = self.analyzer.as_deref() {
            let Ok(analyzer) = analyzer::AnalyzerPool::instance().get_analyzer(name) else {
                return false;
            };
            return analyzer.for_each_term(text, f);
        }
        if self.delims.is_some() {
            return false;
        }
        self.standard(false).for_each_term(text, f);
        true
    }

    pub(crate) fn analyze(&self, text: &str) -> Option<Vec<analyzer::Term>> {
        let name = self.analyzer.as_deref()?;
        analyzer::AnalyzerPool::instance()
            .get_analyzer(name)
            .ok()
            .map(|analyzer| analyzer.analyze(text))
    }
}

/// In-memory inverted index for one text column: a [`MemoryIndexer`] for the
/// unsealed segment plus the sealed on-disk segments.
/// C++: the `MemoryIndexer` + `IndexSegmentReader` pair held by a column.
pub(crate) struct TextIndex {
    pub(crate) tokenizer: TextTokenizer,
    /// The unsealed segment: the memory tail plus the spill runs it was
    /// written to when the budget was reached.
    pub(crate) mem: MemoryIndexer,
    /// Bytes the memory tail may hold before it is spilled to a sorted run.
    /// `TEXT_INDEX_BUDGET_BYTES` in production; the tests lower it so a handful
    /// of documents crosses it.
    pub(crate) budget: usize,
    /// The sealed segments, oldest first: each owns one on-disk index
    /// (`.dic`, `.pos`, `.len`) exactly like the reference's per-segment
    /// indexes.
    pub(crate) reader: ColumnIndexReader,
    /// Directory holding this column's sealed segment files.
    pub(crate) dir: String,
    /// Rows re-indexed after their segment was sealed; the sealed copy is stale
    /// and must not be scored (the memory index holds the new text).
    pub(crate) overridden: std::collections::HashSet<u64>,
    /// Publication barrier: the first row id of every run a writer announced
    /// (`announce_publish`) but has not published yet, with the number of
    /// writers holding it.
    ///
    /// A chunk's `.len` covers **every** row of `base_row_id..=last_row`, so a
    /// seal must never span a row that is still being tokenised somewhere else:
    /// that row would get length 0 now, and the chunk that publishes it later
    /// would start before the end of this one - two chunks over one row, which
    /// [`Self::optimize_chunks`] then reports as an overlap for good.
    pub(crate) pending_publish: std::collections::BTreeMap<u64, u32>,
    /// The tail taken out for a seal that is being written **without the write
    /// lock**, kept readable in the meantime.
    ///
    /// Sealing is the one operation that does real time inside the column's
    /// lock - FST construction, posting encoding and `fsync` of three or four
    /// files, measured at ~0.9 s for a 20k-row tail - and a query holds the read
    /// guard for its whole run. Taking the tail out of [`Self::mem`] and
    /// publishing it as an extra read source lets the write happen with no lock
    /// held while the rows stay searchable; [`Self::complete_seal`] turns it
    /// into a sealed segment.
    ///
    /// At most one tail is in flight: a seal that failed leaves it here to be
    /// retried instead of merging it back into a tail that other writers may
    /// have appended to in the meantime.
    pub(crate) inflight: Option<InflightTail>,
    /// Duration of the last seal's file write, in microseconds
    /// ([`crate::diagnostics::FulltextStats`]).
    pub(crate) last_seal_us: u64,
}

/// A detached tail being written out (see [`TextIndex::inflight`]).
pub(crate) struct InflightTail {
    pub(crate) mem: std::sync::Arc<MemoryIndexer>,
    /// `true` while a thread owns the write, so a second sealer leaves it alone
    /// instead of writing the same files twice.
    pub(crate) writing: bool,
}

impl TextIndex {
    /// The tails, newest first: the resident one, then the tail a seal is
    /// currently writing. Every read path merges them in this order.
    pub(crate) fn tails(&self) -> impl Iterator<Item = &MemoryIndexer> {
        std::iter::once(&self.mem).chain(self.inflight.iter().map(|inflight| &*inflight.mem))
    }

    /// `row -> feature weight` for `term`: the `of_doc_payload` column across
    /// every source, newest write winning. A row re-indexed after its segment
    /// was sealed keeps the memory payload and must not fall back to the
    /// segment's stale one.
    pub(crate) fn doc_payloads(&self, term: &str) -> std::collections::BTreeMap<u64, u16> {
        let mut out = std::collections::BTreeMap::new();
        for segment in &self.reader.segments {
            out.extend(
                segment
                    .reader
                    .doc_payloads(term)
                    .unwrap_or_default()
                    .into_iter()
                    .filter(|(_, payload)| *payload != 0),
            );
        }
        // Newest tail last, so it overwrites the older copies.
        let mut tail = std::collections::BTreeMap::new();
        for mem in self.tails().collect::<Vec<_>>().into_iter().rev() {
            tail.extend(mem.doc_payloads(term));
        }
        for row in &self.overridden {
            if !tail.contains_key(row) {
                out.remove(row);
            }
        }
        out.extend(tail);
        out
    }
}

/// One sealed segment's index: the mmap'd reader plus the corpus counts.
///
/// `docs` is the row **span** the chunk covers (its `.len` slots, which a
/// posting's `docid_t` is an offset into) while `live_docs` is what a predicate
/// still sees. They differ once a merge has dropped the rows deleted before it:
/// the slots cannot move, so the span stays and only the statistics shrink.
/// BM25's `N` is the live count - scoring a chunk by its span would divide by
/// documents no query can return.
pub(crate) struct SealedSegment {
    pub(crate) docs: u32,
    /// Documents a predicate still sees (BM25 `N`).
    pub(crate) live_docs: u32,
    /// `sum(dl)` over the live documents (BM25's corpus length).
    pub(crate) total_len: u64,
    pub(crate) reader: storage::invertedindex::disk_segment_reader::SegmentIndexReader,
}

impl SealedSegment {
    pub(crate) fn contains(&self, row: u64) -> bool {
        self.reader.ordinal(row).is_some()
    }
}

/// The unsealed segment, claimed out of its [`TextIndex`] so the files can be
/// written with no lock held ([`TextIndex::claim_seal`] /
/// [`TextIndex::complete_seal`]).
///
/// `Dump(offline = false)` vs `Dump(offline = true)` → `OfflineDump()`
/// (`memory_indexer_impl.cpp:592-601`): a table with no spill run writes the
/// resident tail straight out, one with runs merges them. Both paths exist - the
/// merge does not replace the realtime dump.
///
/// The job only **borrows** the tail through an [`std::sync::Arc`]: the index
/// keeps serving the same postings while the write runs, and the tail's memory
/// (and its run files) are released in [`TextIndex::complete_seal`].
pub(crate) struct SealPlan {
    pub(crate) docs: Vec<(u64, u32)>,
    pub(crate) base_row_id: u64,
    pub(crate) docs_span: u32,
    pub(crate) total_len: u64,
    /// Rows the segment will hold, so the index can clear its override marks.
    pub(crate) overridden_rows: Vec<u64>,
}

/// One group of chunks that `optimize` concatenates into a single chunk.
pub(crate) struct MergeRun {
    /// `(base_name, base_row_id)` of every chunk in the run, in row order.
    sources: Vec<(String, u64)>,
    dst: String,
}

/// The chunk merges `optimize` would perform, computed from the segment headers
/// alone so the file work can run **without the column's lock**.
pub(crate) struct MergePlan {
    /// The chunk list the plan was made from, re-checked before it is applied.
    signature: Vec<(String, u64)>,
    runs: Vec<MergeRun>,
    /// Rows that were deleted: the merge drops their postings, which is the only
    /// point in a chunk's life where they can be reclaimed.
    deleted: std::collections::HashSet<u64>,
}

pub(crate) struct SealJob {
    pub(crate) tail: std::sync::Arc<MemoryIndexer>,
    pub(crate) docs: Vec<(u64, u32)>,
    pub(crate) base_row_id: u64,
    pub(crate) docs_span: u32,
    pub(crate) total_len: u64,
    pub(crate) dir: String,
    pub(crate) flags: storage::invertedindex::format::TermMetaFlags,
    /// Rows the segment will hold, so the index can clear its override marks.
    pub(crate) overridden_rows: Vec<u64>,
}

impl SealJob {
    /// Writes the segment's `.dic` / `.pos` / `.len` and opens it. `None` on an
    /// I/O error; the caller then leaves the tail in flight for a retry rather
    /// than merging it back into a tail other writers may have appended to.
    pub(crate) fn write(&self) -> Option<SealedSegment> {
        let base_name = format!("ft_{:016x}", self.base_row_id);
        let mem = &*self.tail;
        let realtime = mem.runs.is_empty() && mem.spilled.is_none();
        let docs = &self.docs;
        let written = storage::invertedindex::disk_segment_reader::write_segment_index_streaming(
            &self.dir,
            &base_name,
            self.base_row_id,
            docs,
            self.flags,
            &mut |emit| {
                if realtime {
                    mem.for_each_tail_term(|term, hits, table| {
                        let payload = |row: u64| table.get(&row).copied().unwrap_or(0);
                        emit(term, hits, &payload);
                    });
                } else {
                    mem.for_each_merged_term(|term, hits, table| {
                        let payload = |row: u64| table.get(&row).copied().unwrap_or(0);
                        emit(term, hits, &payload);
                    });
                }
            },
        );
        if written.is_err() {
            return None;
        }
        let reader = storage::invertedindex::disk_segment_reader::SegmentIndexReader::open(
            &self.dir,
            &base_name,
            self.base_row_id,
            self.flags,
        )
        .ok()?;
        Some(SealedSegment {
            docs: self.docs_span,
            // `docs` was filtered of the rows deleted before the seal, so the
            // chunk it describes holds exactly these documents.
            live_docs: self.docs.len() as u32,
            total_len: self.total_len,
            reader,
        })
    }
}

/// One sealed chunk the catalog lists, where the reference wrote it.
///
/// `dir` is the segment's index directory
/// (`db_{db}/tbl_{tbl}/idx_{idx}/seg_{seg}`) and `base_name` the chunk's
/// `ft_{base_row_id:016x}`; together they name the three files
/// (`ChunkIndexMetaInfo::full_text_files`). The port's runtime index keeps its
/// chunks in one directory per column, so this is only used to adopt them.
#[derive(Debug, Clone)]
pub(crate) struct CatalogChunk {
    pub(crate) dir: String,
    pub(crate) base_name: String,
    pub(crate) base_row_id: u64,
}

/// The per-column reader over the sealed segments.
///
/// C++: `ColumnIndexReader` (`src/storage/invertedindex/column_index_reader.cppm`).
/// It owns one `IndexSegmentReader` per sealed segment and answers term
/// postings and document lengths across all of them.
#[derive(Default)]
pub(crate) struct ColumnIndexReader {
    pub(crate) segments: Vec<SealedSegment>,
}

impl ColumnIndexReader {
    /// C++: `GetDocCount` over the segment readers — over the documents a
    /// predicate still sees, which is what BM25's `N` means.
    pub(crate) fn doc_count(&self) -> usize {
        self.segments.iter().map(|s| s.live_docs as usize).sum()
    }

    /// Total token count across the segment readers.
    pub(crate) fn total_len(&self) -> u64 {
        self.segments.iter().map(|s| s.total_len).sum()
    }

    /// C++: `GetIndexReader(term)` — postings from every sealed segment.
    pub(crate) fn postings(&self, term: &str) -> Vec<(u64, u32)> {
        let mut list = Vec::new();
        for seg in &self.segments {
            if let Ok(mut hits) = seg.reader.postings(term) {
                list.append(&mut hits);
            }
        }
        list
    }

    /// C++: `GetColumnLength` — the length recorded by the newest segment
    /// that contains `row`.
    pub(crate) fn doc_len(&self, row: u64) -> Option<u32> {
        self.segments
            .iter()
            .rev()
            .find(|s| s.contains(row))
            .and_then(|s| s.reader.doc_len_of_row(row))
    }

    pub(crate) fn contains(&self, row: u64) -> bool {
        self.segments.iter().any(|s| s.contains(row))
    }
}

/// A block's token offsets, one run after another: a per-block arena costs one
/// allocation instead of one per posting.
#[derive(Default)]
pub(crate) struct PosArena {
    pub(crate) starts: Vec<u32>,
    pub(crate) lens: Vec<u32>,
    pub(crate) buf: Vec<u32>,
}

impl PosArena {
    pub(crate) fn push(&mut self, positions: &[u32]) {
        self.starts.push(self.buf.len() as u32);
        self.lens.push(positions.len() as u32);
        self.buf.extend_from_slice(positions);
    }

    pub(crate) fn get(&self, at: usize) -> &[u32] {
        match (self.starts.get(at), self.lens.get(at)) {
            (Some(start), Some(len)) => self
                .buf
                .get(*start as usize..(*start + *len) as usize)
                .unwrap_or(&[]),
            _ => &[],
        }
    }

    pub(crate) fn clear(&mut self) {
        self.starts.clear();
        self.lens.clear();
        self.buf.clear();
    }
}

impl TextIndex {
    pub(crate) fn new(dir: String, tokenizer: TextTokenizer) -> Self {
        Self {
            tokenizer,
            mem: MemoryIndexer::new(),
            budget: text_index_budget_bytes(),
            reader: ColumnIndexReader::default(),
            dir,
            overridden: Default::default(),
            pending_publish: Default::default(),
            inflight: None,
            last_seal_us: 0,
        }
    }

    /// Announces that rows starting at `from_row` are being tokenised and will
    /// be published into the tail with [`Self::finish_publish`].
    ///
    /// Taken **before** tokenising and released **after** the insert: the window
    /// between the two is exactly the one in which the tail may hold a higher
    /// row id whose text is published while these rows are not, which is what
    /// [`Self::claim_seal`] must not seal across.
    pub(crate) fn announce_publish(&mut self, from_row: u64) {
        *self.pending_publish.entry(from_row).or_insert(0) += 1;
    }

    /// Releases an [`Self::announce_publish`] ticket.
    pub(crate) fn finish_publish(&mut self, from_row: u64) {
        if let Some(writers) = self.pending_publish.get_mut(&from_row) {
            *writers -= 1;
            if *writers == 0 {
                self.pending_publish.remove(&from_row);
            }
        }
    }

    /// The lowest row id some writer has announced but not published yet.
    ///
    /// `None` when every announced row is in the tail. Everything at or above
    /// the barrier is "text still pending", everything below it is settled.
    pub(crate) fn publish_barrier(&self) -> Option<u64> {
        self.pending_publish.keys().next().copied()
    }

    /// Maps the chunks a previous process sealed under `dir`
    /// (`ft_{first_row:016x}.{dic,pos,len,row}`) instead of re-indexing the rows
    /// they cover, and returns the first row id they do **not** cover — 0 when
    /// nothing could be loaded, so the caller rebuilds from the rows.
    ///
    /// A merged chunk (`ft_{first_row:016x}_{rows:x}`, written by
    /// [`Self::optimize_chunks`]) covers a range: plain chunks starting inside it
    /// are leftovers of a merge that crashed before deleting its sources, and are
    /// dropped rather than loaded twice.
    ///
    /// A chunk that cannot be mapped (a torn write at a crash) discards the
    /// whole set: indexing half the table from disk and rebuilding the other
    /// half would count the same documents twice.
    #[allow(dead_code)] // the unit tests open a directory on its own
    pub(crate) fn load_sealed(&mut self) -> u64 {
        self.load_sealed_scoped(None, &[])
    }

    /// Copies chunks the **reference** wrote into this index's own directory.
    ///
    /// The reference stores a chunk in the segment's directory
    /// (`db_{db}/tbl_{tbl}/idx_{idx}/seg_{seg}/ft_{base}.{dic,pos,len}`) and
    /// keeps the list in the catalog (`ChunkIndexMetaInfo`); it never scans a
    /// directory for them. The port's runtime index lives in one directory per
    /// column, so a directory the reference produced has no chunk the loader
    /// would find — this copies them over, through the chunk store, and the
    /// normal load path then maps them.
    ///
    /// Copying rather than reading in place keeps a single directory per index
    /// (which is what the merge and the run-file cleanup assume) at the cost of
    /// one copy when a reference directory is opened by the port for the first
    /// time. A chunk already present under this directory is left alone.
    pub(crate) fn adopt(&mut self, chunks: &[CatalogChunk]) -> usize {
        let store = storage::chunk_store::chunk_store();
        // The per-column directory may not exist at all: a directory the
        // reference wrote has no such directory.
        if std::fs::create_dir_all(&self.dir).is_err() {
            return 0;
        }
        let mut adopted = 0;
        for chunk in chunks {
            let target_dic = storage::invertedindex::format::fulltext_chunk_file(
                &self.dir,
                &chunk.base_name,
                storage::invertedindex::format::FULLTEXT_DICT_SUFFIX,
            );
            if store.length(std::path::Path::new(&target_dic)).is_some() {
                continue;
            }
            let mut complete = true;
            for suffix in [
                storage::invertedindex::format::FULLTEXT_DICT_SUFFIX,
                storage::invertedindex::format::FULLTEXT_POSTING_SUFFIX,
                storage::invertedindex::format::FULLTEXT_LENGTH_SUFFIX,
            ] {
                let source = storage::invertedindex::format::fulltext_chunk_file(
                    &chunk.dir,
                    &chunk.base_name,
                    suffix,
                );
                let Ok(bytes) = store.read(std::path::Path::new(&source)) else {
                    complete = false;
                    break;
                };
                let target = storage::invertedindex::format::fulltext_chunk_file(
                    &self.dir,
                    &chunk.base_name,
                    suffix,
                );
                if let Err(error) = store.write_dedicated(std::path::Path::new(&target), &bytes) {
                    let _ = error;
                    complete = false;
                    break;
                }
            }
            if !complete {
                // A reference chunk whose file family is incomplete is left
                // where it is; the caller rebuilds from the blocks.
                remove_run_files(&self.dir, &chunk.base_name);
                continue;
            }
            // The reference has no publish marker; copy the one a port-written
            // chunk may carry so the live statistics survive the move.
            let source_manifest = storage::invertedindex::format::fulltext_chunk_file(
                &chunk.dir,
                &chunk.base_name,
                ".ok",
            );
            if let Ok(bytes) = store.read(std::path::Path::new(&source_manifest)) {
                let target = storage::invertedindex::chunk_manifest::ChunkManifest::path(
                    &self.dir,
                    &chunk.base_name,
                );
                let _ = store.write_dedicated(&target, &bytes);
            }
            adopted += 1;
        }
        adopted
    }

    /// [`Self::load_sealed`] with the `ROW_ID` segments the catalog still
    /// lists. A chunk of any other segment belongs to rows a compaction moved,
    /// so it is removed instead of loaded: its row ids name no live row.
    ///
    /// `catalog` is the chunk list the catalog holds for this index, in the
    /// reference's layout. It is the fallback when this index's own directory
    /// has nothing: that is the shape of a directory the reference wrote, and
    /// adopting those chunks beats re-indexing the whole column.
    pub(crate) fn load_sealed_scoped(
        &mut self,
        live_segments: Option<&std::collections::HashSet<u32>>,
        catalog: &[CatalogChunk],
    ) -> u64 {
        let scan = |dir: &str| -> Vec<String> {
            let store = storage::chunk_store::chunk_store();
            let mut bases: Vec<String> = store
                .list(std::path::Path::new(dir), "ft_")
                .into_iter()
                .filter_map(|name| name.strip_suffix(".dic").map(|base| base.to_string()))
                .collect();
            bases.sort_unstable();
            bases
        };
        let mut bases = scan(&self.dir);
        if bases.is_empty() && !catalog.is_empty() && self.adopt(catalog) > 0 {
            bases = scan(&self.dir);
        }
        if bases.is_empty() {
            return 0;
        }
        // The base name is the chunk's first row id in hex, zero padded, so the
        // name order is the row order.
        bases.sort_unstable();
        // A merged chunk (`ft_{base:016x}_{rows:x}`, `optimize_chunks`) supersedes
        // the plain chunks its range covers. A crash between writing the merged
        // files and deleting the sources leaves both on disk; loading both would
        // count every row in the range twice, only a rebuild would not notice. The
        // covered plain chunks are dropped here, and their files removed through
        // `remove_run_files` so the next open does not see them either.
        let mut merged_ranges: Vec<(u64, u64)> = Vec::new();
        for base in &bases {
            let Some(rest) = base.strip_prefix("ft_") else {
                continue;
            };
            let Some((start_hex, rows_hex)) = rest.split_once('_') else {
                continue;
            };
            let (Ok(start), Ok(rows)) = (
                u64::from_str_radix(start_hex, 16),
                u64::from_str_radix(rows_hex, 16),
            ) else {
                continue;
            };
            merged_ranges.push((start, start + rows));
        }
        if !merged_ranges.is_empty() {
            bases.retain(|base| {
                let Some(rest) = base.strip_prefix("ft_") else {
                    return true;
                };
                // The merged chunk is not covered by its own range.
                if rest.contains('_') {
                    return true;
                }
                let Ok(start) = u64::from_str_radix(rest, 16) else {
                    return true;
                };
                let covered = merged_ranges
                    .iter()
                    .any(|(from, to)| start >= *from && start < *to);
                if covered {
                    remove_run_files(&self.dir, base);
                }
                !covered
            });
        }
        let mut segments = Vec::with_capacity(bases.len());
        let mut next = 0u64;
        // A chunk whose files do not validate is the remains of a publish that
        // never finished (or of two generations mixed by a retried seal). Its
        // rows are committed rows whose postings are gone, so the only correct
        // answer is to re-index the column from the blocks - and that rebuild
        // starts at row 0, which would overlap every chunk already loaded. So
        // one broken chunk retires the whole set: the files are removed (once,
        // not on every open) and the caller rebuilds from scratch.
        let mut broken = false;
        // Chunks of a `ROW_ID` segment the catalog no longer lists were moved
        // away by a compaction: their row ids name no row any more. They go too,
        // for the same overlap reason - the rows were renumbered, so the rebuild
        // writes a chunk that covers what they claim.
        let mut retired = false;
        for base in &bases {
            // The chunk's first row id is part of its name
            // (`fulltext_base_name`), and it is what turns a posting's dense
            // ordinal back into a row id: opening with 0 would make every chunk
            // claim rows `0..doc_count` and the caller would re-index the rest.
            let Some(base_row_id) = base
                .strip_prefix("ft_")
                .and_then(|hex| hex.split('_').next())
                .and_then(|hex| u64::from_str_radix(hex, 16).ok())
            else {
                broken = true;
                break;
            };
            if let Some(live) = live_segments {
                if !live.contains(&((base_row_id >> 32) as u32)) {
                    retired = true;
                    continue;
                }
            }
            let Ok(reader) = storage::invertedindex::disk_segment_reader::SegmentIndexReader::open(
                &self.dir,
                base,
                base_row_id,
                storage::invertedindex::format::OPTION_FLAG_ALL,
            ) else {
                broken = true;
                break;
            };
            let docs = reader.doc_count();
            if docs == 0 {
                continue;
            }
            // The marker carries what the files cannot: after a merge drops the
            // rows deleted before it, the span (and therefore `docs`) still
            // covers them while BM25's `N` and `sum(dl)` do not.
            let (live_docs, total_len) = match ChunkManifest::read(&self.dir, base) {
                Ok(Some(manifest)) => (manifest.live_docs, manifest.total_len),
                Ok(None) => (
                    docs as u32,
                    (0..docs as u32)
                        .filter_map(|doc| reader.doc_len(doc))
                        .map(u64::from)
                        .sum(),
                ),
                // The reader above loads and validates the same marker, so a
                // failure here is a chunk that changed under us: refuse it
                // instead of reconstructing `N`/`sum(dl)` from span it could not
                // verify.
                Err(_) => {
                    broken = true;
                    break;
                }
            };
            if let Some((_, last)) = reader.row_range() {
                next = next.max(last + 1);
            }
            segments.push(SealedSegment {
                docs: docs as u32,
                live_docs,
                total_len,
                reader,
            });
        }
        if broken || retired {
            drop(segments);
            for base in &bases {
                storage::invertedindex::chunk_manifest::remove_chunk(&self.dir, base);
            }
            return 0;
        }
        if segments.is_empty() {
            return 0;
        }
        self.reader.segments = segments;
        next
    }

    /// Documents in the column across the sealed segments and the unsealed
    /// segment (the memory tail, a tail in flight and their spill runs).
    pub(crate) fn total_docs(&self) -> usize {
        self.tails().map(|mem| mem.pending_docs()).sum::<usize>() + self.reader.doc_count()
    }

    /// Writes the current (unsealed) segment's index to disk and clears the
    /// in-memory postings. The spill runs are merged into the segment first.
    /// Returns `false` (leaving the tail readable) if the files cannot be
    /// written, so a failure never loses results.
    ///
    /// This variant writes **while the caller holds the write lock**; the insert
    /// and checkpoint paths use [`Self::claim_seal`] + [`Self::complete_seal`]
    /// instead so the I/O runs with no lock held.
    pub(crate) fn seal(&mut self) -> bool {
        let Some(job) = self.claim_seal() else {
            return false;
        };
        let started = std::time::Instant::now();
        let written = job.write();
        self.last_seal_us = started.elapsed().as_micros() as u64;
        match written {
            Some(segment) => {
                self.complete_seal(job, segment);
                true
            }
            None => {
                self.abort_seal(job);
                false
            }
        }
    }

    /// The `ROW_ID` segment (`row >> 32`) the unsealed chunk belongs to, if it
    /// holds any row. The writer puts one `base_row_id` and one contiguous `.len`
    /// per chunk, so an unsealed chunk may only carry rows of one segment.
    pub(crate) fn unsealed_segment(&self) -> Option<u64> {
        // The oldest unsealed row decides: a tail in flight is written first,
        // and the resident tail may only cross into another `ROW_ID` segment
        // after it is gone.
        self.tails()
            .filter_map(|mem| mem.unsealed_first_row())
            .min()
            .map(|row| row >> 32)
    }

    /// True when the unsealed segment has reached either budget and should be
    /// sealed (`MemoryIndexer::MemUsed` or `mem_index_capacity` rows), or when a
    /// previous seal failed and its tail is still waiting to be written.
    pub(crate) fn should_seal(&self, capacity: usize) -> bool {
        if self.inflight.is_some() {
            return true;
        }
        self.mem.mem_used() >= text_segment_bytes()
            || (capacity > 0 && self.mem.pending_docs() >= capacity)
    }

    /// Takes the work of one seal, so the caller can [`SealJob::write`] it with
    /// **no lock held** and then [`Self::complete_seal`] it.
    ///
    /// The tail moves into [`Self::inflight`], where the read paths still see
    /// it: the rows stay searchable while their segment is written, which is
    /// what makes the unlocked write safe. A tail left in flight by a failed
    /// write is claimed again here (retry) before a new one is detached.
    ///
    /// `None` when there is nothing to seal, another thread is already writing
    /// the in-flight tail, **or when a row of the span is still being indexed by
    /// another writer** ([`Self::publish_barrier`]): the tail then keeps growing
    /// and the next attempt seals the rows in row-id order instead of writing a
    /// chunk that the delayed publication would overlap.
    pub(crate) fn claim_seal(&mut self) -> Option<SealJob> {
        let dir = self.dir.clone();
        if let Some(inflight) = &mut self.inflight {
            if inflight.writing {
                return None;
            }
            let plan = Self::seal_plan(&inflight.mem)?;
            inflight.writing = true;
            return Some(Self::seal_job(plan, inflight.mem.clone(), dir));
        }
        // Validate *before* taking the tail out, so a refusal leaves it where it
        // is: an empty tail, or rows of two `ROW_ID` segments.
        let plan = Self::seal_plan(&self.mem)?;
        // Rows below the barrier are settled; a row at or above it has its text
        // in another writer's hands. Sealing across it would write length 0 for
        // that row and then overlap the chunk that publishes it.
        let last_row = plan.base_row_id + u64::from(plan.docs_span) - 1;
        if self
            .publish_barrier()
            .is_some_and(|pending| last_row >= pending)
        {
            return None;
        }
        let tail = std::sync::Arc::new(std::mem::take(&mut self.mem));
        self.inflight = Some(InflightTail {
            mem: tail.clone(),
            writing: true,
        });
        Some(Self::seal_job(plan, tail, dir))
    }

    /// What a chunk would contain, computed without moving the tail: `None` when
    /// there is nothing expressible.
    fn seal_plan(tail: &MemoryIndexer) -> Option<SealPlan> {
        let docs = tail.merged_docs()?;
        if docs.is_empty() {
            return None;
        }
        let base_row_id = docs[0].0;
        let last_row = docs[docs.len() - 1].0;
        // The segment covers `base_row_id..base_row_id + docs_span`: the
        // reference's `ChunkIndexMetaInfo::row_cnt`. A row with no text (the
        // reference's `InsertGap`) is a zero-length document inside that range,
        // which is why the span, not the document count, is what the chunk
        // records.
        //
        // The span is only expressible when every row belongs to the same
        // `ROW_ID` segment: two segments are 2^32 apart, so their span neither
        // fits `u32` nor is meant to be allocated. The insert path seals a tail
        // before a row of the next segment arrives, so this is a guard against
        // writing (and allocating) across a segment gap rather than a case to
        // handle - refuse instead of truncating the span.
        let span = last_row - base_row_id + 1;
        if (last_row >> 32) != (base_row_id >> 32) || span > u64::from(u32::MAX) {
            return None;
        }
        Some(SealPlan {
            total_len: tail.pending_len(),
            overridden_rows: docs.iter().map(|(row, _)| *row).collect(),
            docs,
            base_row_id,
            docs_span: span as u32,
        })
    }

    fn seal_job(plan: SealPlan, tail: std::sync::Arc<MemoryIndexer>, dir: String) -> SealJob {
        SealJob {
            tail,
            docs: plan.docs,
            base_row_id: plan.base_row_id,
            docs_span: plan.docs_span,
            total_len: plan.total_len,
            dir,
            flags: storage::invertedindex::format::OPTION_FLAG_ALL,
            overridden_rows: plan.overridden_rows,
        }
    }

    /// Publishes a segment written by [`SealJob::write`]. The new segment now
    /// carries the current text of every row it holds, so those rows are no
    /// longer shadowed by an older sealed copy; the tail's memory and its
    /// transient spill files are released.
    pub(crate) fn complete_seal(&mut self, job: SealJob, reader: SealedSegment) {
        for row in &job.overridden_rows {
            self.overridden.remove(row);
        }
        // The runs were merged into the segment (`MemoryIndexer::Reset`).
        for run in &job.tail.runs {
            run.remove();
        }
        if self
            .inflight
            .as_ref()
            .is_some_and(|inflight| std::sync::Arc::ptr_eq(&inflight.mem, &job.tail))
        {
            self.inflight = None;
        }
        self.reader.segments.push(reader);
    }

    /// Releases the claim after a failed write. The tail **stays in flight**, so
    /// its rows remain searchable and the next seal retries it; merging it back
    /// into `self.mem` would race with writers that appended to the fresh tail.
    pub(crate) fn abort_seal(&mut self, job: SealJob) {
        if let Some(inflight) = &mut self.inflight {
            if std::sync::Arc::ptr_eq(&inflight.mem, &job.tail) {
                inflight.writing = false;
            }
        }
    }

    /// `NewTxn::OptimizeFtIndex` (`new_txn_index_impl.cpp:2179-2270`): merges
    /// the sealed chunks of one segment into
    /// `ft_{base:016x}_{rows:x}` chunks and drops the sources.
    ///
    /// **Deviation from the reference, on purpose.** The reference walks the
    /// chunk list against the running offset and **stops at the first gap**
    /// (`:2219-2227`), so after one hole the rest of the list is never merged
    /// again and the segment count only grows. Rows are append-only here, so a
    /// gap is rare - but one is enough to freeze the chunk list forever, and a
    /// frozen list is paid for by every query. This merges **every** run of
    /// contiguous dense chunks instead, which produces chunks the reference
    /// would also accept (same naming, same `.len` shape) and leaves the
    /// chunk list bounded.
    ///
    /// A run of one chunk is skipped (the reference's
    /// `if chunk_ids.size() <= 1 { return }`). A chunk that starts *before* the
    /// run being built is an overlap, which is unrecoverable (`:2228-2233`) and
    /// still refused. Returns the number of merges.
    ///
    /// The three phases are split so the I/O can run with no lock held
    /// ([`Self::plan_merge`] / [`Self::merge_runs`] / [`Self::apply_merge`]);
    /// this wrapper is the lock-held form the unit tests and any caller that
    /// already owns the write guard use.
    ///
    /// Production goes through [`crate::engine::state::optimize_text_index_unlocked`],
    /// which runs the I/O outside the lock.
    #[cfg(test)]
    pub(crate) fn optimize_chunks(&mut self) -> crate::Result<usize> {
        self.optimize_chunks_with(&std::collections::HashSet::new())
    }

    /// [`Self::optimize_chunks`] with the table's deleted rows, which is what
    /// production passes: the merge is where their postings are reclaimed.
    #[cfg(test)]
    pub(crate) fn optimize_chunks_with(
        &mut self,
        deleted: &std::collections::HashSet<u64>,
    ) -> crate::Result<usize> {
        let Some(plan) = self.plan_merge(deleted)? else {
            return Ok(0);
        };
        let merged = Self::merge_runs(&self.dir, &plan);
        Ok(self.apply_merge(&plan, merged))
    }

    /// The chunk list's identity: re-checked under the lock before a merge that
    /// ran without it is applied.
    pub(crate) fn segment_signature(&self) -> Vec<(String, u64)> {
        self.reader
            .segments
            .iter()
            .map(|segment| {
                (
                    segment.reader.base_name().to_string(),
                    segment.reader.base_row_id(),
                )
            })
            .collect()
    }

    /// Groups the sealed chunks into the runs `optimize` would concatenate, in
    /// the order it merges them. `None` when no run holds two chunks (nothing to
    /// do); `Err` on an overlap that cannot be merged.
    ///
    /// Pure: it reads the segment headers and touches no files, so the plan can
    /// be made under the lock and executed outside it.
    pub(crate) fn plan_merge(
        &self,
        deleted: &std::collections::HashSet<u64>,
    ) -> crate::Result<Option<MergePlan>> {
        let segments = &self.reader.segments;
        if segments.len() < 2 {
            return Ok(None);
        }
        let mut order: Vec<usize> = (0..segments.len()).collect();
        order.sort_by_key(|index| segments[*index].reader.base_row_id());

        let mut runs: Vec<MergeRun> = Vec::new();
        let mut current: Vec<usize> = Vec::new();
        let mut current_end = 0u64;
        for index in order {
            let segment = &segments[index];
            let base = segment.reader.base_row_id();
            // A run is a set of chunks of **one** `ROW_ID` segment that start
            // exactly where the previous one ended. A chunk may itself hold rows
            // without text (an `InsertGap`): its `.len` covers the whole span and
            // the merger re-pads those slots, so density inside a chunk is not a
            // requirement.
            let Some((_, last)) = segment.reader.row_range() else {
                // A chunk that spans no row has nothing to merge.
                Self::flush_run(&mut runs, &mut current, segments);
                continue;
            };
            if !current.is_empty()
                && (base >> 32) != (segments[current[0]].reader.base_row_id() >> 32)
            {
                // Two `ROW_ID` segments are 2^32 apart: a merged `docid_t` is a
                // `u32`, so they cannot share one chunk.
                Self::flush_run(&mut runs, &mut current, segments);
            }
            if !current.is_empty() && base < current_end {
                return Err(crate::Error::Invalid(format!(
                    "full-text chunks overlap: chunk '{}' starts at row {base}, but the run \
                     already covers up to row {}",
                    segment.reader.base_name(),
                    current_end - 1
                )));
            }
            if !current.is_empty() && base > current_end {
                // A hole between two chunks closes the run: the merged `.len`
                // would have to span the rows in between, and a hole of many
                // rows inflates it (4 bytes per row) for nothing.
                Self::flush_run(&mut runs, &mut current, segments);
            }
            current.push(index);
            current_end = last + 1;
        }
        Self::flush_run(&mut runs, &mut current, segments);
        if runs.is_empty() {
            return Ok(None);
        }
        Ok(Some(MergePlan {
            signature: self.segment_signature(),
            runs,
            deleted: deleted.clone(),
        }))
    }

    /// Turns the gathered chunk indices into a [`MergeRun`] when the run holds
    /// two or more chunks; a single chunk is left alone.
    fn flush_run(runs: &mut Vec<MergeRun>, current: &mut Vec<usize>, segments: &[SealedSegment]) {
        let members = std::mem::take(current);
        if members.len() < 2 {
            return;
        }
        let base_row_id = segments[members[0]].reader.base_row_id();
        // The merged chunk spans from the first member's base to the last
        // member's last row - `segments.docs` sums the members' spans, which
        // under-counts once a hole between them is padded, and the `.len` array
        // the merger writes is the whole span.
        let last_row = members
            .last()
            .and_then(|index| segments[*index].reader.row_range().map(|(_, last)| last))
            .unwrap_or(base_row_id);
        let rows = last_row + 1 - base_row_id;
        runs.push(MergeRun {
            sources: members
                .iter()
                .map(|i| {
                    (
                        segments[*i].reader.base_name().to_string(),
                        segments[*i].reader.base_row_id(),
                    )
                })
                .collect(),
            dst: storage::fulltext_base_name(base_row_id, Some(rows)),
        });
    }

    /// Writes every run's merged chunk and opens it. **Touches no in-memory
    /// state**, so it runs with the lock released; a run that fails maps to
    /// `None` and keeps its sources.
    pub(crate) fn merge_runs(dir: &str, plan: &MergePlan) -> Vec<Option<SealedSegment>> {
        let flags = storage::invertedindex::format::OPTION_FLAG_ALL;
        plan.runs
            .iter()
            .map(|run| {
                let sources: Vec<storage::invertedindex::index_merger::ColumnSource> = run
                    .sources
                    .iter()
                    .map(|(base_name, base_row_id)| {
                        storage::invertedindex::index_merger::ColumnSource {
                            base_name: base_name.clone(),
                            base_row_id: *base_row_id,
                        }
                    })
                    .collect();
                let base_row_id = run.sources[0].1;
                // The deleted rows are dropped here, which is the only point
                // where their postings can be reclaimed: a chunk is immutable
                // once published, and the `.len` slot of a row cannot move
                // because a posting's id is `row - base_row_id`.
                let merged =
                    storage::invertedindex::index_merger::ColumnIndexMerger::new(dir, flags)
                        .merge_with(&sources, &run.dst, Some(&plan.deleted))
                        .and_then(|stats| {
                            storage::invertedindex::disk_segment_reader::SegmentIndexReader::open(
                                dir,
                                &run.dst,
                                base_row_id,
                                flags,
                            )
                            .map(|reader| (stats, reader))
                        });
                match merged {
                    Ok((stats, reader)) => Some(SealedSegment {
                        // The merger reports what it actually wrote, not the
                        // plan's estimate: a hole between two sources widens the
                        // span the `.len` array covers.
                        docs: stats.span_docs,
                        live_docs: stats.live.docs,
                        total_len: stats.live.total_len,
                        reader,
                    }),
                    Err(_) => {
                        // The merger has already renamed its output into place,
                        // so a failure in the *last* step must remove those
                        // files: leaving them beside the intact sources would
                        // make the next `load_sealed` count every row in the
                        // range twice.
                        remove_run_files(dir, &run.dst);
                        None
                    }
                }
            })
            .collect()
    }

    /// Swaps the merged chunks in, if the chunk list is still the one the plan
    /// was made from. Returns how many merges were applied.
    ///
    /// A list that moved (a concurrent seal appended a chunk, an earlier optimize
    /// replaced one) is left untouched and the outputs are removed: the next tick
    /// plans again from the current list.
    pub(crate) fn apply_merge(
        &mut self,
        plan: &MergePlan,
        mut merged: Vec<Option<SealedSegment>>,
    ) -> usize {
        if self.segment_signature() != plan.signature {
            for (run, result) in plan.runs.iter().zip(merged.iter_mut()) {
                if result.take().is_some() {
                    remove_run_files(&self.dir, &run.dst);
                }
            }
            return 0;
        }
        let mut applied = 0usize;
        let mut done = vec![false; plan.runs.len()];
        // Which run a chunk belongs to, by base name.
        let owner: std::collections::HashMap<&str, usize> = plan
            .runs
            .iter()
            .enumerate()
            .flat_map(|(index, run)| {
                run.sources
                    .iter()
                    .map(move |(name, _)| (name.as_str(), index))
            })
            .collect();
        let segments = std::mem::take(&mut self.reader.segments);
        let mut out: Vec<SealedSegment> = Vec::with_capacity(segments.len());
        for segment in segments {
            let base_name = segment.reader.base_name().to_string();
            let Some(&run_index) = owner.get(base_name.as_str()) else {
                out.push(segment);
                continue;
            };
            // The merged chunk takes the place of the run's first member; the
            // rest of the run's chunks disappear into it.
            if plan.runs[run_index].sources[0].0 != base_name {
                continue;
            }
            match merged[run_index].take() {
                Some(reader) => {
                    out.push(reader);
                    done[run_index] = true;
                    applied += 1;
                }
                // The merge failed: its sources stay as they are.
                None => out.push(segment),
            }
        }
        // Rename-then-unlink, not one atomic step: a crash between the two leaves
        // a complete merged chunk next to some of its sources. That is exactly the
        // state `load_sealed` resolves - it drops plain chunks whose base row id
        // falls inside a merged chunk's range - so the window costs one cleanup on
        // the next open instead of a double count.
        for (run_index, run) in plan.runs.iter().enumerate() {
            if !done[run_index] {
                continue;
            }
            for (base_name, _) in &run.sources {
                remove_run_files(&self.dir, base_name);
            }
        }
        self.reader.segments = out;
        applied
    }

    /// Seals once the unsealed segment (runs included) reaches its size budget
    /// or the configured `mem_index_capacity` row count.
    pub(crate) fn maybe_seal(&mut self, capacity: usize) {
        if self.should_seal(capacity) {
            self.seal();
        }
    }

    /// Total token count of the column across the sealed segments and the
    /// unsealed segment (BM25's `avgdl`).
    pub(crate) fn corpus_len(&self) -> u64 {
        self.tails().map(|mem| mem.pending_len()).sum::<u64>() + self.reader.total_len()
    }

    /// BM25's document length for `row`, or `None` when no segment of this
    /// column holds it (the scan path scores such a row with length 0).
    pub(crate) fn doc_len_opt(&self, row: u64) -> Option<u32> {
        self.tails()
            .find_map(|mem| mem.doc_len_of(row))
            .or_else(|| self.reader.doc_len(row))
    }

    /// BM25's document length for `row`: the unsealed segment's map first
    /// (it holds the current text), then the sealed segments.
    /// One term's postings as a lazy stream: the unsealed tail, the spilled
    /// runs (newest first) and the sealed segments (newest first).
    ///
    /// Nothing is materialised: each source is a cursor the WAND iterator seeks
    /// in place, so an unread part of a posting list costs nothing.
    pub(crate) fn term_stream(&self, term: &str) -> TermStream<'_> {
        let (sources, segment_from) = self.term_sources(term);
        TermStream::new(MergeCursor::new(
            sources,
            segment_from,
            Some(&self.overridden),
        ))
    }

    /// [`Self::term_stream`] with the postings' token offsets captured.
    pub(crate) fn term_stream_capture(&self, term: &str, capture: bool) -> TermStream<'_> {
        let (sources, segment_from) = self.term_sources(term);
        TermStream::with_positions(
            MergeCursor::new(sources, segment_from, Some(&self.overridden)),
            capture,
        )
    }

    /// Documents containing `term` (the reference's `doc_freq`).
    pub(crate) fn term_df(&self, term: &str) -> usize {
        self.term_stream(term).df()
    }

    /// One term's postings as a plain cursor: the phrase path walks documents
    /// one at a time and reads their positions, so it needs no block cache.
    pub(crate) fn term_cursor(&self, term: &str) -> MergeCursor<'_> {
        let (sources, segment_from) = self.term_sources(term);
        MergeCursor::new(sources, segment_from, Some(&self.overridden))
    }

    /// The sources of one term, newest first: the resident tail and its spilled
    /// runs, the tail a seal is writing and its runs, then the sealed segments.
    pub(crate) fn term_sources(&self, term: &str) -> (Vec<Source<'_>>, usize) {
        let mut sources: Vec<Source<'_>> = Vec::new();
        for mem in self.tails() {
            if let Some(list) = mem.postings_of(term) {
                if !list.list.is_empty() {
                    sources.push(Source::Mem {
                        list: &list.list,
                        pos: &list.pos,
                        lens: &mem.docs,
                        at: 0,
                        range: mem.row_range.unwrap_or((0, 0)),
                    });
                }
            }
            for run in mem.runs.iter().rev() {
                if let Ok(Some(cur)) = run.reader.term_doc_cursor(term) {
                    sources.push(Source::Segment(cur));
                }
            }
        }
        let segment_from = sources.len();
        for seg in self.reader.segments.iter().rev() {
            if let Ok(Some(cur)) = seg.reader.term_doc_cursor(term) {
                sources.push(Source::Segment(cur));
            }
        }
        (sources, segment_from)
    }

    /// Drops the sealed segments every slot of which is an overridden row.
    ///
    /// A compaction renumbers the rows it merges: their new text lands in the
    /// tail and their old sealed copies are overridden, so a segment that a
    /// merge took whole can never answer again. Keeping it would still count its
    /// documents in BM25's `N` and its tokens in the corpus length (no query can
    /// return them), so `N` would grow with every compaction the table takes.
    /// Only a segment whose rows are **all** retired goes: a partial merge
    /// leaves the live rows of a mixed segment in place.
    pub(crate) fn retire_fully_overridden_segments(&mut self) {
        if self.overridden.is_empty() || self.reader.segments.is_empty() {
            return;
        }
        let mut retired: Vec<u64> = Vec::new();
        let overridden = &self.overridden;
        self.reader.segments.retain(|segment| {
            let rows = segment.reader.rows();
            if rows.iter().any(|row| !overridden.contains(row)) {
                return true;
            }
            retired.extend(rows);
            false
        });
        // Their ids can never come back (a row id is never reused), so nothing
        // has to remember the overrides any more.
        for row in retired {
            self.overridden.remove(&row);
        }
    }

    /// Postings and corpus statistics for `terms`, merging every sealed segment
    /// with the unsealed one. Stale copies of re-indexed rows are dropped.
    pub(crate) fn merge(&self, terms: &[String]) -> MergedIndex {
        let n_docs = self.total_docs();
        let total_len = self.corpus_len();
        let mut postings: HashMap<String, Vec<(u64, u32)>> = HashMap::new();
        for term in terms {
            let mut list: Vec<(u64, u32)> = Vec::new();
            for mem in self.tails() {
                list.extend(mem.postings_newest_first(term));
            }
            // A re-indexed row's *sealed* copy is stale, but the copy in the
            // unsealed segment is the current text: only the sealed one is
            // dropped (the memory index already resolved its own duplicates).
            list.extend(
                self.reader
                    .postings(term)
                    .into_iter()
                    .filter(|(d, _)| !self.overridden.contains(d)),
            );
            dedupe_postings(&mut list);
            postings.insert(term.clone(), list);
        }
        MergedIndex {
            n_docs,
            total_len,
            postings,
        }
    }
}
