use super::*;

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
    for suffix in [
        storage::invertedindex::format::FULLTEXT_DICT_SUFFIX,
        storage::invertedindex::format::FULLTEXT_POSTING_SUFFIX,
        storage::invertedindex::format::FULLTEXT_LENGTH_SUFFIX,
        storage::invertedindex::disk_segment_reader::FULLTEXT_ROW_SUFFIX,
    ] {
        let _ = std::fs::remove_file(storage::invertedindex::format::fulltext_chunk_file(
            dir, base_name, suffix,
        ));
    }
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
    /// Base name of the run's `.dic` / `.pos` / `.len` / `.row` files.
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
    /// `term -> id` and `id -> term`. The dictionary outlives the tail's postings
    /// (a spill keeps using the same ids) and is rebuilt from the rows when a
    /// process opens a directory.
    pub(crate) term_ids: FastMap<String, u32>,
    pub(crate) term_names: Vec<String>,
    pub(crate) live: std::collections::HashSet<u64>,
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

impl MemoryIndexer {
    pub(crate) fn new() -> Self {
        Self::default()
    }

    /// C++: `MemoryIndexer::Insert` for one document. Returns `false` when the
    /// document would push the segment past `budget`, leaving it unchanged.
    pub(crate) fn insert(
        &mut self,
        seq: u64,
        text: &str,
        tokenizer: &TextTokenizer,
        budget: usize,
    ) -> bool {
        // Terms are streamed and interned on the way in: no `String` per term
        // (a 20k-row column is ~3.7M of them) and no per-document list of
        // borrowed strings to keep alive. Interning happens before the budget
        // check, so a rejected document may leave an unused dictionary entry;
        // its posting list stays empty, which every consumer already skips.
        let mut toks: Vec<(u32, u32)> = Vec::new();
        // ~32 B per posting entry plus the positions and the term string.
        let mut cost = 64usize;
        for_each_index_term(text, tokenizer, |term, pos| {
            cost += term.len() + 32;
            let id = match self.term_ids.get(term) {
                Some(id) => *id,
                None => {
                    let id = self.term_names.len() as u32;
                    self.term_names.push(term.to_string());
                    self.term_ids.insert(term.to_string(), id);
                    self.postings.push(TermPostings::default());
                    id
                }
            };
            toks.push((id, pos));
        });
        if self.bytes + cost > budget {
            return false;
        }
        self.bytes += cost;
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
    pub(crate) fn delete(&mut self, seq: u64) {
        self.live.remove(&seq);
    }

    /// C++: `GetDocCount` of the resident tail. The spilled runs are counted by
    /// [`Self::pending_docs`], because the segment they will be merged into has
    /// not been written yet.
    #[allow(dead_code)] // the unit tests read the tail's count
    pub(crate) fn doc_count(&self) -> usize {
        self.n_docs
    }

    /// Documents the next sealed segment will hold: the tail plus every run.
    pub(crate) fn pending_docs(&self) -> usize {
        self.n_docs + self.runs.iter().map(|run| run.docs.len()).sum::<usize>()
    }

    /// Token count the next sealed segment will hold.
    pub(crate) fn pending_len(&self) -> u64 {
        self.total_len + self.runs.iter().map(|run| run.total_len).sum::<u64>()
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

    /// The tail as the segment writer wants it: documents by row id and the
    /// postings by term, a re-inserted row collapsed to one entry.
    #[allow(clippy::type_complexity)]
    pub(crate) fn segment_payload(
        &self,
    ) -> Option<(
        Vec<(u64, u32)>,
        std::collections::BTreeMap<
            String,
            Vec<storage::invertedindex::disk_segment_reader::PostingEntry>,
        >,
    )> {
        if self.docs.is_empty() {
            return None;
        }
        let mut docs: Vec<(u64, u32)> = self.docs.iter().map(|(row, len)| (*row, *len)).collect();
        docs.sort_unstable_by_key(|(row, _)| *row);
        let mut postings: std::collections::BTreeMap<
            String,
            Vec<storage::invertedindex::disk_segment_reader::PostingEntry>,
        > = std::collections::BTreeMap::new();
        for (id, term) in self.term_names.iter().enumerate() {
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
            dedupe_posting_entries(&mut hits);
            // The map is keyed by the term text, so its iteration order is the
            // ascending byte order the FST builder requires.
            postings.insert(term.clone(), hits);
        }
        Some((docs, postings))
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
        let Some((docs, postings)) = self.segment_payload() else {
            return false;
        };
        let total_len: u64 = docs.iter().map(|(_, len)| u64::from(*len)).sum();
        let base_row_id = docs[0].0;
        let base_name = format!(
            "spill_{:08x}{}",
            self.next_run_id,
            storage::invertedindex::format::FULLTEXT_SPILL_SUFFIX
        );
        let flags = storage::invertedindex::format::TermMetaFlags::default();
        if storage::invertedindex::disk_segment_reader::write_segment_index(
            dir,
            &base_name,
            base_row_id,
            &docs,
            &postings,
            flags,
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

    /// C++: `SortMerger` (`OfflineDump`, `memory_indexer_impl.cpp:792-812`) —
    /// merges the spilled runs and the resident tail (plus a `Load`ed spill
    /// segment) into one sorted posting stream, which is what the seal path
    /// writes into the segment.
    ///
    /// Sources are ranked oldest to newest and a re-inserted row keeps only its
    /// newest entry, so a row that was spilled and then rewritten ends up with
    /// exactly one posting list (the `InsertGap` case: an out-of-order or
    /// re-inserted document must not leave a stale copy in a run).
    #[allow(clippy::type_complexity)]
    pub(crate) fn merged_segment(
        &self,
    ) -> Option<(
        Vec<(u64, u32)>,
        std::collections::BTreeMap<
            String,
            Vec<storage::invertedindex::disk_segment_reader::PostingEntry>,
        >,
    )> {
        if self.docs.is_empty() && self.runs.is_empty() && self.spilled.is_none() {
            return None;
        }
        // Document lengths, oldest source first so the newest write wins.
        let mut lengths: std::collections::BTreeMap<u64, u32> = std::collections::BTreeMap::new();
        if let Some(reader) = &self.spilled {
            for row in reader.rows() {
                if let Some(len) = reader.doc_len_of_row(*row) {
                    lengths.insert(*row, len);
                }
            }
        }
        for run in &self.runs {
            lengths.extend(run.docs.iter().copied());
        }
        lengths.extend(self.docs.iter().map(|(row, len)| (*row, *len)));
        let docs: Vec<(u64, u32)> = lengths.into_iter().collect();

        // `spilled` (a `Load`ed dump) is the oldest source, then the runs in
        // creation order, then the tail. Each source's dictionary is walked
        // once, so every term is decoded at most once per source.
        let tail_rank = self.runs.len() as u32 + 1;
        let mut entries_by_term: std::collections::BTreeMap<String, Vec<SourcedEntry>> =
            std::collections::BTreeMap::new();
        if let Some(reader) = &self.spilled {
            for term in reader.terms().unwrap_or_default() {
                if let Some(hits) = Self::reader_posting_entries(reader, &term) {
                    entries_by_term
                        .entry(term)
                        .or_default()
                        .extend(hits.into_iter().map(|(row, tf, pos)| (0, row, tf, pos)));
                }
            }
        }
        for (i, run) in self.runs.iter().enumerate() {
            let rank = i as u32 + 1;
            for term in run.reader.terms().unwrap_or_default() {
                let Some(hits) = Self::reader_posting_entries(&run.reader, &term) else {
                    continue;
                };
                let current: Vec<SourcedEntry> = hits
                    .into_iter()
                    // A re-inserted row's older copy is a stale posting: the
                    // segment must hold exactly one list per document.
                    .filter(|(row, _, _)| self.run_entry_is_current(run.id, *row))
                    .map(|(row, tf, pos)| (rank, row, tf, pos))
                    .collect();
                if !current.is_empty() {
                    entries_by_term.entry(term).or_default().extend(current);
                }
            }
        }
        for (id, term) in self.term_names.iter().enumerate() {
            let list = &self.postings[id];
            let positions: Vec<(u64, u32, &[u32])> = list
                .list
                .iter()
                .enumerate()
                .map(|(i, (row, tf, _, _))| (*row, *tf, list.positions(i)))
                .collect();
            entries_by_term.entry(term.clone()).or_default().extend(
                positions
                    .into_iter()
                    .map(|(row, tf, pos)| (tail_rank, row, tf, pos.to_vec())),
            );
        }

        let mut postings: std::collections::BTreeMap<
            String,
            Vec<storage::invertedindex::disk_segment_reader::PostingEntry>,
        > = std::collections::BTreeMap::new();
        for (term, entries) in entries_by_term {
            if !entries.is_empty() {
                postings.insert(term, dedupe_sourced_entries(entries));
            }
        }
        Some((docs, postings))
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
        let Some((docs, postings)) = self.merged_segment() else {
            return false;
        };
        if docs.is_empty() {
            return false;
        }
        storage::invertedindex::disk_segment_reader::write_segment_index(
            dir,
            base_name,
            docs[0].0,
            &docs,
            &postings,
            storage::invertedindex::format::TermMetaFlags::default(),
        )
        .is_ok()
    }

    /// C++: `MemoryIndexer::Load` — reads a spilled segment back. Document
    /// lengths and live rows are restored eagerly; postings stay in the
    /// reader and are served by [`Self::segment_posting`].
    #[allow(dead_code)] // the product spill path; exercised by `memory_indexer_ut`.
    pub(crate) fn load(&mut self, dir: &str, base_name: &str) -> bool {
        let flags = storage::invertedindex::format::TermMetaFlags::default();
        let Ok(reader) = storage::invertedindex::disk_segment_reader::SegmentIndexReader::open(
            dir, base_name, 0, flags,
        ) else {
            return false;
        };
        self.reset();
        let rows = reader.rows().to_vec();
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
pub(crate) type SourcedEntry = (u32, u64, u32, Vec<u32>);

/// Merges one term's entries from several sources: for a re-inserted row the
/// entry with the highest source rank wins (the newest spill run, or the
/// in-memory tail), so a stale copy in an older run is dropped. Entries from
/// the same source keep the [`dedupe_posting_entries`] rule (last frequency,
/// union of the positions), which is what a duplicate inside one tail means.
pub(crate) fn dedupe_sourced_entries(
    mut entries: Vec<SourcedEntry>,
) -> Vec<storage::invertedindex::disk_segment_reader::PostingEntry> {
    entries.sort_by_key(|(rank, row, _, _)| (*row, *rank));
    let mut out: Vec<SourcedEntry> = Vec::with_capacity(entries.len());
    for (rank, row, tf, mut positions) in entries {
        match out.last_mut() {
            Some((last_rank, last_row, last_tf, last_positions)) if *last_row == row => {
                if rank > *last_rank {
                    *last_rank = rank;
                    *last_tf = tf;
                    *last_positions = positions;
                } else {
                    *last_tf = tf;
                    last_positions.append(&mut positions);
                    last_positions.sort_unstable();
                    last_positions.dedup();
                }
            }
            _ => out.push((rank, row, tf, positions)),
        }
    }
    out.into_iter()
        .map(|(_, row, tf, positions)| (row, tf, positions))
        .collect()
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
}

/// One sealed segment's index: the mmap'd reader plus the corpus counts.
pub(crate) struct SealedSegment {
    pub(crate) docs: u32,
    pub(crate) total_len: u64,
    pub(crate) reader: storage::invertedindex::disk_segment_reader::SegmentIndexReader,
}

impl SealedSegment {
    pub(crate) fn contains(&self, row: u64) -> bool {
        self.reader.ordinal(row).is_some()
    }
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
    /// C++: `GetDocCount` over the segment readers.
    pub(crate) fn doc_count(&self) -> usize {
        self.segments.iter().map(|s| s.docs as usize).sum()
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
            budget: TEXT_INDEX_BUDGET_BYTES,
            reader: ColumnIndexReader::default(),
            dir,
            overridden: Default::default(),
        }
    }

    /// Maps the chunks a previous process sealed under `dir`
    /// (`ft_{first_row:016x}.{dic,pos,len,row}`) instead of re-indexing the rows
    /// they cover, and returns the first row id they do **not** cover — 0 when
    /// nothing could be loaded, so the caller rebuilds from the rows.
    ///
    /// A chunk that cannot be mapped (a torn write at a crash) discards the
    /// whole set: indexing half the table from disk and rebuilding the other
    /// half would count the same documents twice.
    pub(crate) fn load_sealed(&mut self) -> u64 {
        let Ok(entries) = std::fs::read_dir(&self.dir) else {
            return 0;
        };
        let mut bases: Vec<String> = entries
            .flatten()
            .filter_map(|entry| {
                let name = entry.file_name().into_string().ok()?;
                name.strip_suffix(".dic").map(|base| base.to_string())
            })
            .collect();
        if bases.is_empty() {
            return 0;
        }
        // The base name is the chunk's first row id in hex, zero padded, so the
        // name order is the row order.
        bases.sort_unstable();
        let mut segments = Vec::with_capacity(bases.len());
        let mut next = 0u64;
        for base in &bases {
            let Ok(reader) = storage::invertedindex::disk_segment_reader::SegmentIndexReader::open(
                &self.dir,
                base,
                0,
                storage::invertedindex::format::TermMetaFlags::default(),
            ) else {
                return 0;
            };
            let docs = reader.doc_count();
            if docs == 0 {
                continue;
            }
            let total_len: u64 = (0..docs as u32)
                .filter_map(|doc| reader.doc_len(doc))
                .map(u64::from)
                .sum();
            if let Some((_, last)) = reader.row_range() {
                next = next.max(last + 1);
            }
            segments.push(SealedSegment {
                docs: docs as u32,
                total_len,
                reader,
            });
        }
        if segments.is_empty() {
            return 0;
        }
        self.reader.segments = segments;
        next
    }

    /// Documents in the column across the sealed segments and the unsealed
    /// segment (the memory tail plus its spill runs).
    pub(crate) fn total_docs(&self) -> usize {
        self.mem.pending_docs() + self.reader.doc_count()
    }

    /// Writes the current (unsealed) segment's index to disk and clears the
    /// in-memory postings. The spill runs are merged into the segment first.
    /// Returns `false` (leaving everything in memory) if the files cannot be
    /// written, so a failure never loses results.
    pub(crate) fn seal(&mut self) -> bool {
        let Some((docs, postings)) = self.mem.merged_segment() else {
            return false;
        };
        if docs.is_empty() {
            return false;
        }
        let base_row_id = docs[0].0;
        let total_len = self.mem.pending_len();
        let base_name = format!("ft_{base_row_id:016x}");
        let flags = storage::invertedindex::format::TermMetaFlags::default();
        if storage::invertedindex::disk_segment_reader::write_segment_index(
            &self.dir,
            &base_name,
            base_row_id,
            &docs,
            &postings,
            flags,
        )
        .is_err()
        {
            return false;
        }
        let Ok(reader) = storage::invertedindex::disk_segment_reader::SegmentIndexReader::open(
            &self.dir,
            &base_name,
            base_row_id,
            flags,
        ) else {
            return false;
        };
        // The new segment now carries the current text of every row it holds,
        // so those rows are no longer shadowed by an older sealed copy.
        for (row, _) in &docs {
            self.overridden.remove(row);
        }
        self.reader.segments.push(SealedSegment {
            docs: docs.len() as u32,
            total_len,
            reader,
        });
        self.mem.reset();
        true
    }

    /// Seals once the unsealed segment (runs included) reaches its size budget
    /// or the configured `mem_index_capacity` row count.
    pub(crate) fn maybe_seal(&mut self, capacity: usize) {
        if self.mem.mem_used() >= text_segment_bytes()
            || (capacity > 0 && self.mem.pending_docs() >= capacity)
        {
            self.seal();
        }
    }

    /// Total token count of the column across the sealed segments and the
    /// unsealed segment (BM25's `avgdl`).
    pub(crate) fn corpus_len(&self) -> u64 {
        self.mem.pending_len() + self.reader.total_len()
    }

    /// BM25's document length for `row`, or `None` when no segment of this
    /// column holds it (the scan path scores such a row with length 0).
    pub(crate) fn doc_len_opt(&self, row: u64) -> Option<u32> {
        self.mem
            .doc_len_of(row)
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

    /// The sources of one term, newest first: the unsealed tail, the spilled
    /// runs, then the sealed segments.
    pub(crate) fn term_sources(&self, term: &str) -> (Vec<Source<'_>>, usize) {
        let mut sources: Vec<Source<'_>> = Vec::new();
        if let Some(list) = self.mem.postings_of(term) {
            if !list.list.is_empty() {
                sources.push(Source::Mem {
                    list: &list.list,
                    pos: &list.pos,
                    lens: &self.mem.docs,
                    at: 0,
                    range: self.mem.row_range.unwrap_or((0, 0)),
                });
            }
        }
        for run in self.mem.runs.iter().rev() {
            if let Ok(Some(cur)) = run.reader.term_doc_cursor(term) {
                sources.push(Source::Segment(cur));
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

    /// Postings and corpus statistics for `terms`, merging every sealed segment
    /// with the unsealed one. Stale copies of re-indexed rows are dropped.
    pub(crate) fn merge(&self, terms: &[String]) -> MergedIndex {
        let n_docs = self.mem.pending_docs() + self.reader.doc_count();
        let total_len = self.mem.pending_len() + self.reader.total_len();
        let mut postings: HashMap<String, Vec<(u64, u32)>> = HashMap::new();
        for term in terms {
            let mut list = self.mem.postings_newest_first(term);
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
