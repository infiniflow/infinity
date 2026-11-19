//! The executor's spann glue: DDL option parsing and the build entry point
//! (`docs/SPANN_DESIGN.md` §15).
//!
//! The index itself lives in `index::spann` and knows nothing about SQL; this
//! module is the only place that turns `WITH (…)` text into `Options` and the
//! engine's row ids into the index's `(row, vector)` pairs.

use std::sync::{Arc, Mutex, RwLock};

use common::{Error, Result};
use index::spann::incr::IncIndex;
use index::spann::{Encode, Options, SpannIndex};
use index::vector::Metric;

/// The `WITH (…)` keys `CREATE INDEX … USING SPANN` accepts.
pub(crate) const OPTION_KEYS: &[&str] = &[
    "METRIC",
    "MAX_PARTITION_SIZE",
    "MIN_PARTITION_SIZE",
    "FILL_FACTOR",
    "ENCODE",
    "NUM_CENTROIDS",
    "KMEANS_ITERATIONS",
    "SEED",
    "BEAM",
    "MIN_ROWS",
    "ADAPTIVE_BEAM",
];

// A freshly built `SpannIndex` used to be copied into RocksDB here (as one KV
// row per member and per centroid, so the index's *data* landed in the
// metadata store). The tree is published as a `chunk_{n}.idx` instead, exactly
// like HNSW's `chunk_{n}.idx`, and the catalog records it with the shared
// `chunk_info` row (see `publish_index_chunks`).

/// Translates the DDL options into the builder's knobs.
pub(crate) fn options_from(opts: &[(String, String)]) -> Result<Options> {
    let mut out = Options::default();
    for (key, value) in opts {
        let key_upper = key.trim().to_ascii_uppercase();
        let value = value.trim().trim_matches('\'').trim_matches('"');
        match key_upper.as_str() {
            "METRIC" => {
                out.metric = match value.to_ascii_lowercase().as_str() {
                    "l2" => Metric::L2,
                    "ip" | "inner_product" => Metric::Ip,
                    "cosine" | "cos" => Metric::Cos,
                    other => {
                        return Err(Error::Invalid(format!(
                            "invalid metric '{other}' for a SPANN index"
                        )))
                    }
                };
            }
            "MAX_PARTITION_SIZE" => out.max_partition_size = parse_usize(key, value)?,
            "MIN_PARTITION_SIZE" => out.min_partition_size = parse_usize(key, value)?,
            "FILL_FACTOR" => {
                out.fill_factor = value.parse::<f32>().map_err(|_| {
                    Error::Invalid(format!(
                        "invalid fill_factor '{value}' (expected 0.3..=1.0)"
                    ))
                })?;
            }
            "ENCODE" => {
                out.encode = Encode::parse(value).ok_or_else(|| {
                    Error::Invalid(format!(
                        "invalid encode '{value}' (expected 'rabitq' or 'none')"
                    ))
                })?;
            }
            "NUM_CENTROIDS" => out.root_fanout = parse_usize(key, value)?,
            "KMEANS_ITERATIONS" => out.kmeans_iterations = parse_usize(key, value)?,
            "SEED" => {
                out.seed = value.parse::<u64>().map_err(|_| {
                    Error::Invalid(format!("invalid seed '{value}' (expected a u64)"))
                })?;
            }
            "BEAM" => out.base_beam = parse_usize(key, value)?,
            "MIN_ROWS" => out.min_rows = parse_usize(key, value)?,
            "ADAPTIVE_BEAM" => {
                out.adaptive_beam = value.parse::<bool>().map_err(|_| {
                    Error::Invalid(format!(
                        "invalid adaptive_beam '{value}' (expected 'true' or 'false')"
                    ))
                })?;
            }
            other => {
                return Err(Error::Invalid(format!(
                    "invalid index parameter: unsupported SPANN option '{other}'"
                )))
            }
        }
    }
    // A caller that only names `max_partition_size` must not trip over the
    // default `min_partition_size` (32): the floor cannot exceed the ceiling.
    if out.min_partition_size > out.max_partition_size {
        out.min_partition_size = (out.max_partition_size / 2).max(2);
    }
    out.validate().map_err(|e| Error::Invalid(e.to_string()))?;
    Ok(out)
}

fn parse_usize(key: &str, value: &str) -> Result<usize> {
    value
        .parse::<usize>()
        .map_err(|_| Error::Invalid(format!("invalid {key} '{value}' (expected an integer)")))
}

/// How many partitions' worth of rows a spann **mem index** may hold before a
/// dump is triggered (`docs/SPANN_DESIGN.md` §9.2).
///
/// The mem index is not a write buffer like HNSW's - it is the query's
/// *candidate list*: every row in it is handed to the exact rescoring on every
/// KNN query, because the tree cannot rank rows it does not hold. Unbounded, one
/// import of a million rows between two checkpoints would turn each query into a
/// million-row decode. Sixty-four partitions is the point where folding the rows
/// into a tree (one k-means build over the segment) is cheaper than carrying
/// them through every query.
pub(crate) const MEM_INDEX_DUMP_FACTOR: usize = 64;

/// The row bound of `opts`' mem index: `max_partition_size × 64`, never zero.
pub(crate) fn mem_dump_threshold(opts: &Options) -> usize {
    opts.max_partition_size
        .saturating_mul(MEM_INDEX_DUMP_FACTOR)
        .max(1)
}

/// The rows written since the last dump, held as a **live tree**
/// (`SPANN_DESIGN.md` §0.0.3).
///
/// This replaces a `Vec<(row id, Vec<f32>)>`: that kept 6 KB per pending row at
/// 1536d and made every query re-score the whole list, because a flat list has
/// no structure to descend. A tree holds 208 B per row, answers with a bounded
/// beam, and splits/merges a partition in place - so "insert, then query" costs
/// the same per-row work as a published chunk does.
///
/// Deletes are tombstones: the tree cannot un-append a row, and it does not have
/// to - the next dump rebuilds the segment from the column, so the tombstone set
/// only has to survive until then. It is bounded by the deletes since the dump,
/// which is the same bound the flat list had.
pub(crate) struct SpannTail {
    tree: RwLock<IncIndex>,
    dead: Mutex<std::collections::HashSet<u64>>,
    dim: usize,
    opts: Options,
    /// Splits/merges this tail has run, from both the paced insert path and the
    /// background drain - the number `SHOW INDEX` reports as fixup work.
    fixup_steps: std::sync::atomic::AtomicU64,
}

/// How many queued splits one insert may run under the write lock.
///
/// One partition crossing its bound queues one key, so a healthy write path
/// never needs more than one; a bulk load would otherwise drain the whole queue
/// per row. The remainder is picked up by [`SpannTail::drain_fixups`].
pub(crate) const INSERT_FIXUP_BUDGET: usize = 2;

/// How many queued splits one background drain pass may run.
///
/// Larger than the insert budget because it is not on a request path, small
/// enough that a single pass does not hold the write lock for a whole backlog.
pub(crate) const BACKGROUND_FIXUP_BUDGET: usize = 64;

/// Set once, so a poisoned lock is reported the first time and not per query.
static POISON_REPORTED: std::sync::atomic::AtomicBool = std::sync::atomic::AtomicBool::new(false);

/// Takes a lock whose guard a panic may have poisoned.
///
/// Not `unwrap`: a panic in an insert must not turn every later query into a
/// panic of its own. Nor is `into_inner` silent - it is logged once, because a
/// half-applied split is a state an operator has to know about. The row set
/// stays readable (the tree is exact between fixups) and the next checkpoint
/// rebuilds the segment from the column, which is the repair.
fn recover_poisoned<T>(result: std::sync::LockResult<T>, what: &str) -> T {
    match result {
        Ok(guard) => guard,
        Err(poisoned) => {
            if !POISON_REPORTED.swap(true, std::sync::atomic::Ordering::Relaxed) {
                eprintln!(
                    "spann: {what} lock was poisoned by a panic; continuing from the last state"
                );
            }
            poisoned.into_inner()
        }
    }
}

impl SpannTail {
    pub(crate) fn new(dim: usize, opts: Options) -> Result<Arc<Self>> {
        let tree = IncIndex::new(dim, opts.clone()).map_err(spann_err)?;
        Ok(Arc::new(Self {
            tree: RwLock::new(tree),
            dead: Mutex::new(std::collections::HashSet::new()),
            dim,
            opts,
            fixup_steps: std::sync::atomic::AtomicU64::new(0),
        }))
    }

    /// One row, one descent, at most [`INSERT_FIXUP_BUDGET`] splits. The write
    /// lock is held for that bounded work, never for a rebuild and never for a
    /// backlog.
    pub(crate) fn insert(&self, id: u64, vector: &[f32]) -> Result<()> {
        if vector.len() != self.dim {
            return Err(Error::Invalid(format!(
                "spann: row {id} has {} dims, the index has {}",
                vector.len(),
                self.dim
            )));
        }
        let mut tree = recover_poisoned(self.tree.write(), "tail tree");
        tree.insert(id, vector).map_err(spann_err)?;
        if tree.pending_fixups() > 0 {
            // The tail holds codes, not vectors; a split dequantizes them, the
            // same path a chunk-backed split takes (`RowSource = None`).
            //
            // Budgeted: an insert pays a couple of splits, not the queue. A
            // bulk load that crosses a partition bound on every row used to
            // drain the whole backlog inside the write lock - one insert turned
            // into a millisecond tail. The rest of the queue is the background
            // drain's job (`drain_fixups`, called by the periodic task).
            let steps = tree.run_fixups_budgeted(None, INSERT_FIXUP_BUDGET);
            self.fixup_steps
                .fetch_add(steps as u64, std::sync::atomic::Ordering::Relaxed);
        }
        Ok(())
    }

    /// Drains up to `budget` queued splits/merges, returning how many ran.
    ///
    /// Called from the background task, never from a request: the write lock is
    /// the same one inserts take, so a budget keeps it short. `0` means the
    /// queue is empty and the underfull sweep ran.
    pub(crate) fn drain_fixups(&self, budget: usize) -> usize {
        let mut tree = recover_poisoned(self.tree.write(), "tail tree");
        if tree.pending_fixups() == 0 {
            return 0;
        }
        let steps = tree.run_fixups_budgeted(None, budget);
        self.fixup_steps
            .fetch_add(steps as u64, std::sync::atomic::Ordering::Relaxed);
        steps
    }

    /// `beam = None` scans every leaf, which is the semantic the tail must keep:
    /// a row inserted a microsecond ago is *visible*, not merely covered by a
    /// beam that might not have reached its partition. It stays affordable
    /// because the tail is bounded by the dump trigger (`max_partition_size ×
    /// 64` rows) - the same bound the flat candidate list had, without the raw
    /// vectors behind it.
    pub(crate) fn search(&self, q: &[f32], k: usize, beam: Option<usize>) -> Vec<(u64, f32)> {
        let tree = recover_poisoned(self.tree.read(), "tail tree");
        let dead = recover_poisoned(self.dead.lock(), "tail tombstones");
        let budget = self.budget(&tree, beam);
        let mut hits = tree.search(q, k + dead.len(), budget);
        hits.retain(|(id, _)| !dead.contains(id));
        hits.truncate(k);
        hits
    }

    pub(crate) fn search_filtered(
        &self,
        q: &[f32],
        k: usize,
        beam: Option<usize>,
        allowed: &std::collections::HashSet<u64>,
    ) -> Vec<(u64, f32)> {
        let tree = recover_poisoned(self.tree.read(), "tail tree");
        let dead = recover_poisoned(self.dead.lock(), "tail tombstones");
        let budget = self.budget(&tree, beam);
        let mut hits = tree.search_masked(q, k + dead.len(), budget, Some(allowed));
        hits.retain(|(id, _)| !dead.contains(id));
        hits.truncate(k);
        hits
    }

    /// The leaf budget for a tail query.
    ///
    /// The tail has to answer **exactly** while it is the size the dump trigger
    /// allows (`max_partition_size × 64` rows): a row inserted a second ago is
    /// visible, not merely "covered by a beam that might not have reached its
    /// partition". Beyond that - a stalled dump, a bulk replay - the budget falls
    /// back to `base_beam` so no query turns into a full scan of the tail.
    fn budget(&self, tree: &IncIndex, beam: Option<usize>) -> usize {
        if let Some(beam) = beam {
            return beam.max(1);
        }
        let dump_threshold = self.opts.max_partition_size.saturating_mul(64);
        if tree.len() <= dump_threshold {
            usize::MAX
        } else {
            self.opts.base_beam.max(1)
        }
    }

    /// Splits/merges this tail has run - the `spann_fixup_steps` gauge.
    pub(crate) fn fixup_steps(&self) -> u64 {
        self.fixup_steps.load(std::sync::atomic::Ordering::Relaxed)
    }

    /// Queued splits/merges, i.e. leaves over (or under) their bound that the
    /// insert path's budget did not reach.
    pub(crate) fn pending_fixups(&self) -> usize {
        recover_poisoned(self.tree.read(), "tail tree").pending_fixups()
    }

    /// Live rows held here.
    pub(crate) fn len(&self) -> usize {
        let tree = recover_poisoned(self.tree.read(), "tail tree");
        let dead = recover_poisoned(self.dead.lock(), "tail tombstones");
        tree.len().saturating_sub(dead.len())
    }

    /// Live row ids, ascending - the coverage check's input (§9.7).
    pub(crate) fn row_ids(&self) -> Vec<u64> {
        let tree = recover_poisoned(self.tree.read(), "tail tree");
        let dead = recover_poisoned(self.dead.lock(), "tail tombstones");
        let mut ids: Vec<u64> = tree
            .row_ids()
            .into_iter()
            .filter(|id| !dead.contains(id))
            .collect();
        ids.sort_unstable();
        ids
    }

    /// Whether any pending row belongs to `segment`: a segment whose tail is
    /// non-empty cannot have its chunk reused, because the dump would publish
    /// nothing for it (the reuse check in `blocks/engine.rs`).
    pub(crate) fn has_segment(&self, segment: u32) -> bool {
        let tree = recover_poisoned(self.tree.read(), "tail tree");
        let dead = recover_poisoned(self.dead.lock(), "tail tombstones");
        tree.row_ids()
            .into_iter()
            .any(|id| (id >> 32) as u32 == segment && !dead.contains(&id))
    }

    /// Drops rows a commit deleted (an `UPDATE` is delete + insert, so both
    /// halves land here).
    pub(crate) fn forget(&self, ids: &std::collections::HashSet<u64>) {
        let mut dead = recover_poisoned(self.dead.lock(), "tail tombstones");
        for id in ids {
            dead.insert(*id);
        }
    }

    /// Heap bytes: the codes plus the tombstone set - what the spill ledger
    /// charges for the un-dumped rows.
    pub(crate) fn mem_usage(&self) -> usize {
        let tree = recover_poisoned(self.tree.read(), "tail tree");
        let dead = recover_poisoned(self.dead.lock(), "tail tombstones");
        tree.mem_usage() + dead.len() * std::mem::size_of::<u64>()
    }
}

impl SpannTail {
    /// Swaps in an empty tree, handing the filled one **and its tombstones** to
    /// the checkpoint: the rows are already in the segment's column, so the
    /// publisher owns them from here and the tail must stop answering for them.
    ///
    /// The tombstones travel with the tree because a delete does not un-append a
    /// row: its codes are still in the leaf the `UPDATE`/`DELETE` marked dead.
    /// A publisher that re-inserts the tree's row ids into a segment's published
    /// tree has to skip them, or the delete would be undone by the checkpoint
    /// that was supposed to make it durable.
    pub(crate) fn take_live(&self) -> Option<(IncIndex, std::collections::HashSet<u64>)> {
        let mut tree = recover_poisoned(self.tree.write(), "tail tree");
        let replacement = IncIndex::new(self.dim, self.opts.clone()).ok()?;
        let taken = std::mem::replace(&mut *tree, replacement);
        let dead = std::mem::take(&mut *recover_poisoned(self.dead.lock(), "tail tombstones"));
        if taken.is_empty() {
            None
        } else {
            Some((taken, dead))
        }
    }

    /// Puts tombstones back after a checkpoint abandoned its batch.
    ///
    /// [`Self::take_live`] clears them, and the rows they cover are still in the
    /// tail (the checkpoint only removed the ones it published). Dropping them
    /// would publish a deleted-before-publish row on the next pass.
    pub(crate) fn restore_dead(&self, dead: std::collections::HashSet<u64>) {
        if dead.is_empty() {
            return;
        }
        recover_poisoned(self.dead.lock(), "tail tombstones").extend(dead);
    }
}

fn spann_err(e: index::spann::SpannError) -> Error {
    Error::Invalid(format!("spann: {e}"))
}

/// A queryable spann index: the tree a `CREATE INDEX` built, or the chunk a
/// later checkpoint published (§6.4).
///
/// The two forms must answer identically — `storage::knn_index::spann` has a
/// test that runs the same queries through both — so the rest of the engine only
/// ever sees this enum.
pub(crate) enum SpannHandle {
    /// Built in this process and not yet published to a chunk: what
    /// `CREATE INDEX` installs, and what the first query after it answers from.
    Memory(Arc<SpannIndex>),
    /// Served straight out of `chunk_{n}.idx`, **one chain per segment**: the
    /// slot bytes are page cache, which is what makes 50M rows fit in 8 GB
    /// (§19.3), and a segment's chunks are a chain whose newest layer wins per
    /// key (§5.4) - a checkpoint appends the pages it rewrote.
    File(Vec<Arc<storage::knn_index::spann::ChunkChain>>),
}

/// Orders a union of per-chunk answers the way the metric ranks, so truncating
/// it keeps the global best rather than each chunk's best.
fn merge_chunks(merged: &mut [(u64, f32)]) {
    // Ascending, for every metric: `FileIndex::search` already answers "smaller is
    // better" (ip is `−⟨o,q⟩`, cosine is `−cos`), so reversing the order for those
    // two turned `truncate(k)` into "keep the k worst".
    merged.sort_by(|a, b| a.1.partial_cmp(&b.1).unwrap_or(std::cmp::Ordering::Equal));
}

impl SpannHandle {
    pub(crate) fn len(&self) -> usize {
        match self {
            SpannHandle::Memory(index) => index.len(),
            SpannHandle::File(chains) => chains.iter().map(|c| c.len()).sum(),
        }
    }

    pub(crate) fn dim(&self) -> usize {
        match self {
            SpannHandle::Memory(index) => index.dim(),
            SpannHandle::File(chains) => chains.first().map(|c| c.dims()).unwrap_or(0),
        }
    }

    pub(crate) fn search(&self, q: &[f32], k: usize, beam: Option<usize>) -> Vec<(u64, f32)> {
        match self {
            SpannHandle::Memory(index) => index.search(q, k, beam),
            SpannHandle::File(chains) => {
                // One tree per segment (§3 I6) and the trees share nothing, so a
                // query is N independent descents (§8.1). A single segment keeps
                // the serial loop - a fan-out for one item buys no parallelism.
                let per_chunk: Vec<Vec<(u64, f32)>> = if chains.len() > 1
                    && !crate::fanout::on_a_runtime_worker()
                {
                    let items: Vec<Arc<storage::knn_index::spann::ChunkChain>> = chains.to_vec();
                    let shared_query: Arc<[f32]> = Arc::from(q);
                    let workers = chains.len();
                    match crate::fanout::map_blocking(items, workers, move |chain| {
                        Ok(chain.search(shared_query.as_ref(), k, beam))
                    }) {
                        Ok(answers) => answers,
                        // A fan-out that cannot spawn falls back to this
                        // thread: a missing segment is a wrong answer, not a
                        // slow one.
                        Err(_) => chains.iter().map(|c| c.search(q, k, beam)).collect(),
                    }
                } else {
                    chains.iter().map(|c| c.search(q, k, beam)).collect()
                };
                // Every segment answers with its own `k`, and the union is what
                // the caller's exact rescoring sees: truncating per segment
                // instead would drop rows that only lose to another segment's.
                let mut merged: Vec<(u64, f32)> = per_chunk.into_iter().flatten().collect();
                merge_chunks(&mut merged);
                merged.truncate(k);
                merged
            }
        }
    }

    /// The row ids the tree holds, ascending - the coverage check's input
    /// (`docs/SPANN_DESIGN.md` §9.7).
    pub(crate) fn row_ids(&self) -> Vec<u64> {
        match self {
            SpannHandle::Memory(index) => index.row_ids(),
            SpannHandle::File(chains) => {
                let mut ids: Vec<u64> = chains.iter().flat_map(|c| c.row_ids()).collect();
                ids.sort_unstable();
                ids
            }
        }
    }

    /// [`Self::search`] restricted to the rows a statement admits.
    ///
    /// The tree's descent is not filter-aware - a partition either holds rows
    /// the bitmask admits or it does not, and the reference compares the slot's
    /// `row_id` while it reads the slot (`§12.5`). This asks for a bigger
    /// candidate list first and keeps the admitted rows, which is the same
    /// answer for a filter whose selectivity is not tiny, and the caller's
    /// `uncovered` fallback covers the rest.
    pub(crate) fn search_filtered(
        &self,
        q: &[f32],
        k: usize,
        beam: Option<usize>,
        allowed: &std::collections::HashSet<usize>,
    ) -> Vec<(u64, f32)> {
        if k == 0 || allowed.is_empty() {
            return Vec::new();
        }
        // The mask is tested per slot inside the tree now (`search_filtered`),
        // so a short answer means the **beam** was too narrow, not the candidate
        // pool. Widening the beam costs centroid distances; widening the pool
        // costs decoded rows - which is why the old `k / selectivity` version
        // asked for the whole table at 1% selectivity.
        // One mask for every segment: the bitmap is rows of the whole table, and
        // a per-chunk copy would rebuild it once per segment per attempt.
        let ids: Arc<std::collections::HashSet<u64>> =
            Arc::new(allowed.iter().map(|id| *id as u64).collect());
        let mut width = beam.unwrap_or(DEFAULT_SEARCH_BEAM).max(1);
        for attempt in 0..3 {
            let mut hits = match self {
                SpannHandle::Memory(index) => index.search_filtered(q, k, Some(width), &ids),
                SpannHandle::File(chains) => {
                    let per_chunk: Vec<Vec<(u64, f32)>> =
                        if chains.len() > 1 && !crate::fanout::on_a_runtime_worker() {
                            let items: Vec<Arc<storage::knn_index::spann::ChunkChain>> =
                                chains.to_vec();
                            let shared_query: Arc<[f32]> = Arc::from(q);
                            let shared_ids = Arc::clone(&ids);
                            let workers = chains.len();
                            match crate::fanout::map_blocking(items, workers, move |chain| {
                                Ok(chain.search_filtered(
                                    shared_query.as_ref(),
                                    k,
                                    Some(width),
                                    &shared_ids,
                                ))
                            }) {
                                Ok(answers) => answers,
                                Err(_) => chains
                                    .iter()
                                    .map(|c| c.search_filtered(q, k, Some(width), &ids))
                                    .collect(),
                            }
                        } else {
                            chains
                                .iter()
                                .map(|c| c.search_filtered(q, k, Some(width), &ids))
                                .collect()
                        };
                    let mut merged: Vec<(u64, f32)> = per_chunk.into_iter().flatten().collect();
                    merge_chunks(&mut merged);
                    merged.truncate(k);
                    merged
                }
            };
            if hits.len() >= k || attempt == 2 {
                hits.truncate(k);
                return hits;
            }
            width = (width * 2).min(MAX_RERANK_CANDIDATES);
        }
        Vec::new()
    }

    /// `(partitions, height)`, reported by `SHOW INDEX` (§16.4).
    pub(crate) fn shape(&self) -> (usize, u32) {
        match self {
            SpannHandle::Memory(index) => (index.partitions().len(), index.height()),
            SpannHandle::File(chains) => (
                chains.iter().map(|c| c.partitions()).sum(),
                chains.iter().map(|c| c.height()).max().unwrap_or(0),
            ),
        }
    }

    /// Heap bytes this handle holds. A file-backed index holds only its header
    /// and section table, so the answer is 0 and the slot pages show up as page
    /// cache instead (`RssFile`, §19.6). `SHOW INDEX` reports it as
    /// `spann_mem_bytes`.
    pub(crate) fn mem_usage(&self) -> usize {
        match self {
            SpannHandle::Memory(index) => index.mem_usage(),
            // The file form owns no heap the watermark can charge: its slot
            // pages are the page cache.
            SpannHandle::File(_) => 0,
        }
    }

    /// Whether the tree is served from a chunk file (page cache) rather than
    /// from this process's heap - `SHOW INDEX`'s `spann_file_backed`.
    pub(crate) fn is_file_backed(&self) -> bool {
        matches!(self, SpannHandle::File(_))
    }

    /// Heap bytes held by the **directory**: the file form's owner tables and
    /// per-chunk headers, the memory form's tree. `spann_directory_bytes`.
    pub(crate) fn directory_bytes(&self) -> usize {
        match self {
            SpannHandle::Memory(index) => index.mem_usage(),
            SpannHandle::File(chains) => chains.iter().map(|c| c.heap_bytes()).sum(),
        }
    }

    /// How many chunks answer for this index (a chain per segment).
    pub(crate) fn layers(&self) -> usize {
        match self {
            SpannHandle::Memory(_) => 1,
            SpannHandle::File(chains) => chains.iter().map(|c| c.layers()).sum(),
        }
    }

    /// Drops the resident pages of every mapped chunk, returning the bytes
    /// handed back. A memory-form handle owns no mapping and answers 0.
    ///
    /// Correct while the index is answering: the chunks are immutable, so the
    /// pages are re-read from the file on the next query. This is what keeps an
    /// idle engine's RSS from tracking the index size.
    pub(crate) fn release_pages(&self) -> usize {
        match self {
            SpannHandle::Memory(_) => 0,
            SpannHandle::File(chains) => chains.iter().map(|c| c.release_pages()).sum(),
        }
    }
}

/// The per-index counters behind `SHOW INDEX` (§16.4).
///
/// They live on the engine, keyed by index id, rather than on the tree: a tree is
/// dropped and rebuilt by every dump, and a counter that resets on a dump cannot
/// answer "what does a query cost on this index".
#[derive(Default)]
pub(crate) struct SpannStats {
    queries: std::sync::atomic::AtomicU64,
    candidates: std::sync::atomic::AtomicU64,
    beam: std::sync::atomic::AtomicU64,
    /// Queries whose search returned fewer candidates than the budget allowed:
    /// the beam ran out of partitions before the pool filled, i.e. the search
    /// stopped early instead of being cut off by the budget.
    early_stops: std::sync::atomic::AtomicU64,
    /// Full rebuilds published: a new chunk that supersedes the segment's chain.
    rebuilds: std::sync::atomic::AtomicU64,
    /// Delta layers published: one chunk of rewritten pages on top of a chain.
    deltas: std::sync::atomic::AtomicU64,
    /// Publications refused by the coverage/dangling check. A non-zero rate
    /// means the walk and the directory disagree - the old chain keeps
    /// answering, so this is a correctness alarm, not a lost write.
    refused: std::sync::atomic::AtomicU64,
    /// Orphan chunk files (written, never recorded, crash window) reclaimed.
    orphans: std::sync::atomic::AtomicU64,
    /// Mapped bytes returned to the kernel by `release_pages`.
    released: std::sync::atomic::AtomicU64,
    /// Splits/merges run by the tail, from the insert path and the drain.
    fixup_steps: std::sync::atomic::AtomicU64,
    /// Rows a published chain still holds but the column no longer does, as of
    /// the last checkpoint: what a rebuild would reclaim.
    dead_rows: std::sync::atomic::AtomicU64,
    /// Progress of the running full build, in permille (1000 = idle).
    build_progress: std::sync::atomic::AtomicU64,
    /// The query counter as of the last page-release decision: an index whose
    /// count has not moved since then answered nothing and may give its pages
    /// back.
    last_release_queries: std::sync::atomic::AtomicU64,
    /// Set by an operator asking the running build to stop. Taken (swapped
    /// back to false) at the next segment boundary: a build already streaming a
    /// segment finishes it, since half a chunk is not a state anything reads.
    build_cancel: std::sync::atomic::AtomicBool,
}

/// [`SpannStats`] read out once, so one `SHOW INDEX` sees one instant.
#[derive(Clone, Copy, Debug, Default, PartialEq)]
pub(crate) struct SpannStatsSnapshot {
    pub queries: u64,
    pub candidates_per_query: f64,
    pub beam_actual: f64,
    pub early_stop_ratio: f64,
    pub rebuilds: u64,
    pub deltas: u64,
    pub refused: u64,
    pub orphans: u64,
    pub released_bytes: u64,
    pub fixup_steps: u64,
    pub dead_rows: u64,
    pub build_progress: u64,
}

impl SpannStats {
    pub(crate) fn record(&self, candidates: usize, beam: usize, budget: usize) {
        use std::sync::atomic::Ordering;
        self.queries.fetch_add(1, Ordering::Relaxed);
        self.candidates
            .fetch_add(candidates as u64, Ordering::Relaxed);
        self.beam.fetch_add(beam as u64, Ordering::Relaxed);
        if candidates < budget {
            self.early_stops.fetch_add(1, Ordering::Relaxed);
        }
    }

    pub(crate) fn snapshot(&self) -> SpannStatsSnapshot {
        use std::sync::atomic::Ordering;
        let queries = self.queries.load(Ordering::Relaxed);
        let per = |total: u64| {
            if queries == 0 {
                0.0
            } else {
                total as f64 / queries as f64
            }
        };
        SpannStatsSnapshot {
            queries,
            candidates_per_query: per(self.candidates.load(Ordering::Relaxed)),
            beam_actual: per(self.beam.load(Ordering::Relaxed)),
            early_stop_ratio: per(self.early_stops.load(Ordering::Relaxed)),
            rebuilds: self.rebuilds.load(Ordering::Relaxed),
            deltas: self.deltas.load(Ordering::Relaxed),
            refused: self.refused.load(Ordering::Relaxed),
            orphans: self.orphans.load(Ordering::Relaxed),
            released_bytes: self.released.load(Ordering::Relaxed),
            fixup_steps: self.fixup_steps.load(Ordering::Relaxed),
            dead_rows: self.dead_rows.load(Ordering::Relaxed),
            build_progress: self.build_progress.load(Ordering::Relaxed),
        }
    }

    /// A segment's tree was rewritten from the column (`superseded` non-empty).
    pub(crate) fn record_rebuild(&self, dead_rows: u64) {
        use std::sync::atomic::Ordering;
        self.rebuilds.fetch_add(1, Ordering::Relaxed);
        // A rebuild reclaims every dead row of the segment, so the gauge is the
        // checkpoint's fresh count, not a subtraction from the old one.
        self.dead_rows.store(dead_rows, Ordering::Relaxed);
    }

    /// One delta layer was published on top of a chain.
    pub(crate) fn record_delta(&self, dead_rows: u64) {
        use std::sync::atomic::Ordering;
        self.deltas.fetch_add(1, Ordering::Relaxed);
        self.dead_rows.store(dead_rows, Ordering::Relaxed);
    }

    /// The coverage/dangling check refused a layer.
    pub(crate) fn record_refused(&self) {
        self.refused
            .fetch_add(1, std::sync::atomic::Ordering::Relaxed);
    }

    /// Orphan chunk files reclaimed by cleanup.
    pub(crate) fn record_orphans(&self, files: u64) {
        self.orphans
            .fetch_add(files, std::sync::atomic::Ordering::Relaxed);
    }

    /// Mapped bytes returned to the kernel.
    pub(crate) fn record_released(&self, bytes: u64) {
        self.released
            .fetch_add(bytes, std::sync::atomic::Ordering::Relaxed);
    }

    /// `permille` of the running build; `0` clears it (build finished or never
    /// started).
    pub(crate) fn set_build_progress(&self, permille: u64) {
        self.build_progress
            .store(permille.min(1000), std::sync::atomic::Ordering::Relaxed);
    }

    /// Whether no query ran since the previous cleanup pass, recording `queries`
    /// as the new baseline.
    ///
    /// Releasing the pages of a hot index would fault every slot page back in on
    /// the next query, so the decision is "answered nothing last interval", not
    /// "is file backed". The swap makes the check and the baseline one atomic
    /// step, so two cleanup passes cannot both see "idle".
    pub(crate) fn take_idle_interval(&self, queries: u64) -> bool {
        let last = self
            .last_release_queries
            .swap(queries, std::sync::atomic::Ordering::Relaxed);
        queries == last
    }

    /// Asks the running build to stop at the next segment boundary.
    pub(crate) fn request_cancel(&self) {
        self.build_cancel
            .store(true, std::sync::atomic::Ordering::Relaxed);
    }

    /// Whether a cancel was requested, clearing it: a cancel is one build's,
    /// not a property of the index from then on.
    pub(crate) fn take_cancel(&self) -> bool {
        self.build_cancel
            .swap(false, std::sync::atomic::Ordering::Relaxed)
    }
}

/// The SQL-layer default search width: Cockroach's `vector_search_beam_size`,
/// whose global default is 32. The index's own `Options::base_beam` stays 8 for
/// direct callers; the statement path is the one that pays for recall, because
/// a 1-bit code leaves the partition choice as the only lever.
pub(crate) const DEFAULT_SEARCH_BEAM: usize = 32;

/// The share of the leaves the search width probes, in percent.
///
/// Recall in a SPANN tree is governed by the *share* of leaves visited, not by an
/// absolute count. Measured on 200k x 1536d against a brute force, 600 queries:
/// a fixed beam of 32 is 0.79% of the 4033 leaves and gives recall@10 0.918; 3%
/// of them gives 0.948; the same 3.3% at 961 leaves (40k rows) gives 0.973. So
/// the width tracks the leaf count, and partition size being capped
/// (`max_partition_size`) means 3% of the leaves is ~3% of the table's rows.
pub(crate) const LEAF_SHARE: usize = 3;

/// Ceiling on [`LEAF_SHARE`]-derived width: the share is O(rows), so past this the
/// probe would cost milliseconds per query at 1536d. Recall decays with size again
/// beyond it - `WITH (beam = …, nprobe = …)` names an exact width when it matters.
pub(crate) const LEAF_SHARE_CAP: usize = 512;

/// `vector_search_rerank_multiplier`: how many extra candidates the 1-bit
/// ordering hands to the exact rescoring, per log2(beam)·log2(topn).
pub(crate) const DEFAULT_RERANK_MULTIPLIER: usize = 50;

/// Hard ceiling on the rerank pool.
///
/// This is an **I/O** budget, not an allocation: the rescoring is streamed in
/// batches, so the pool size costs reads (1024 rows × 1536d × 4 B ≈ 6 MiB) and
/// never resident rows. Without the ceiling the formula grows without bound
/// with `beam` (Cockroach allows beam up to 2048, which asks for 3650 rows at
/// k = 100).
pub(crate) const MAX_RERANK_CANDIDATES: usize = 1024;

/// The candidate budget for one query: Cockroach's `IncreaseRerankResults`.
///
/// `maxResults = max(ceil(1.2·topn), 10)` (the 1.2 covers rows the primary
/// index has already deleted), `maxExtraResults =⌊log2(beam)·log2(topn)⌋ ·
/// multiplier`. A fixed `10 × topn` — what this used to be — is the same number
/// only at one point of the curve and cannot represent "widen the beam, not the
/// pool", which is the trade the recall monitor needs.
pub(crate) fn candidate_budget(topn: usize, beam: usize, multiplier: usize) -> usize {
    let results = (((topn as f64) * 1.2).ceil() as usize).max(10);
    let log = (beam.max(2) as f64).log2() * (topn.max(2) as f64).log2();
    let extra = (log as usize).saturating_mul(multiplier);
    // A `topn` of 1000 asks for a pool of at least 1200, which is already past the
    // ceiling: clamping to `(1200, 1024)` panics, and a pool under `results` would
    // return a short top-k. The floor wins.
    let ceiling = MAX_RERANK_CANDIDATES.max(results);
    (results + extra).clamp(results, ceiling)
}

impl crate::Engine {
    /// The counters `SHOW INDEX` reads for `index_id`, created on first use.
    pub(crate) fn spann_stats(&self, index_id: u64) -> Arc<SpannStats> {
        let mut map = self.context.spann_stats.lock().unwrap();
        map.entry(index_id).or_default().clone()
    }

    /// Drains up to `budget` queued splits per tail, returning the total run.
    ///
    /// The periodic task calls this: the insert path only takes
    /// [`INSERT_FIXUP_BUDGET`] keys, so a bulk load would otherwise leave a
    /// backlog of over-full leaves behind it (visible as a lower recall, not as
    /// an error). Cheap when there is nothing to do - one lock per tail.
    pub(crate) fn spann_drain_fixups(&self, budget: usize) -> usize {
        let tails: Vec<Arc<SpannTail>> = {
            let tables = self.tables.lock().unwrap();
            tables
                .values()
                .flat_map(|entry| entry.spann_mem.values().cloned())
                .collect()
        };
        let mut total = 0usize;
        for tail in tails {
            total += tail.drain_fixups(budget);
        }
        total
    }

    /// Asks a running spann build to stop after the segment it is streaming.
    ///
    /// The chunk being written is discarded (it was never recorded), so the
    /// index keeps answering from what it had. The cancel is consumed by the
    /// build that sees it: an operator canceling a `CREATE INDEX` must not also
    /// cancel the next checkpoint's publication.
    pub fn cancel_spann_build(&self, index_id: u64) {
        self.spann_stats(index_id).request_cancel();
    }

    /// Whether any tail has queued splits/merges left.
    ///
    /// The tick asks this before it takes a write lock, so an idle engine pays
    /// one short read per tail and nothing else.
    pub(crate) fn has_spann_fixups(&self) -> bool {
        let tails: Vec<Arc<SpannTail>> = {
            let tables = self.tables.lock().unwrap();
            tables
                .values()
                .flat_map(|entry| entry.spann_mem.values().cloned())
                .collect()
        };
        tails.iter().any(|tail| tail.pending_fixups() > 0)
    }

    /// Drops the resident mapped pages of **every** file-backed index, idle or
    /// not. Used when the process is over its memory limit: a hot index will
    /// fault its pages back in on the next query, which is the cost of staying
    /// alive.
    pub(crate) fn spann_release_pages_forced(&self) -> usize {
        let handles: Vec<(u64, Arc<SpannHandle>)> = {
            let tables = self.tables.lock().unwrap();
            tables
                .values()
                .flat_map(|entry| {
                    entry
                        .spann
                        .iter()
                        .map(|(id, handle)| (*id, Arc::clone(handle)))
                })
                .collect()
        };
        let mut total = 0usize;
        for (id, handle) in handles {
            let bytes = handle.release_pages();
            if bytes > 0 {
                self.spann_stats(id).record_released(bytes as u64);
                total += bytes;
            }
        }
        total
    }

    /// Drops the resident mapped pages of every **idle** file-backed index,
    /// returning the bytes handed back to the kernel.
    ///
    /// Called by the periodic cleanup pass. Idle means "answered no query since
    /// the previous pass": releasing a hot index's pages would fault them all
    /// back in on its next query, which is the cost this is meant to avoid. The
    /// chunks are immutable, so a concurrent reader is unaffected either way.
    pub(crate) fn spann_release_pages(&self) -> usize {
        let handles: Vec<(u64, Arc<SpannHandle>)> = {
            let tables = self.tables.lock().unwrap();
            tables
                .values()
                .flat_map(|entry| {
                    entry
                        .spann
                        .iter()
                        .map(|(id, handle)| (*id, Arc::clone(handle)))
                })
                .collect()
        };
        let mut total = 0usize;
        for (id, handle) in handles {
            let stats = self.spann_stats(id);
            let queries = stats.snapshot().queries;
            if !stats.take_idle_interval(queries) {
                continue;
            }
            let bytes = handle.release_pages();
            if bytes > 0 {
                stats.record_released(bytes as u64);
                total += bytes;
            }
        }
        total
    }

    /// `max_open_chunks_per_index` (§18): the live-chunk count that makes an
    /// optimize pass fold a segment's chunks into one.
    pub(crate) fn max_open_chunks_per_index(&self) -> usize {
        self.context
            .max_open_chunks_per_index
            .load(std::sync::atomic::Ordering::Relaxed)
            .max(1)
    }

    pub fn set_max_open_chunks_per_index(&self, chunks: usize) {
        self.context
            .max_open_chunks_per_index
            .store(chunks.max(1), std::sync::atomic::Ordering::Relaxed);
    }

    /// The directory a segment's spann chunk is written to
    /// (`idx_{index_id}/seg_{segment}` - the same layout HNSW uses).
    pub(crate) fn spann_chunk_dir(
        &self,
        table: &str,
        index_name: &str,
        segment: u32,
    ) -> Option<std::path::PathBuf> {
        let table = self.qualified_table(table);
        let meta = self.storage.catalog.get_table_by_name(&table).ok()??;
        let index_id = self.index_position(&table, index_name).ok()?;
        let db_id = self.database_id_of(&table);
        Some(
            self.blocks_root()
                .join(format!("db_{db_id}/tbl_{}", meta.id))
                .join(format!("idx_{index_id}/seg_{segment}")),
        )
    }

    /// The `SHOW INDEX` telemetry for one spann index (§16.4), appended to the
    /// parameter string the RPC returns.
    ///
    /// Every number here is a property of **this** index: `spann_degraded` says
    /// whether it answers from a tree or from the exact scan, and
    /// `spann_compaction_backlog` counts the chunks its segments have not folded
    /// yet - the state an operator needs to decide "is this index thin or fat",
    /// which the C++ `segment_index_count` alone does not say.
    pub(crate) fn spann_telemetry_text(
        &self,
        table: &str,
        im: &catalog::IndexMeta,
        base: String,
    ) -> String {
        let Ok(opts) = options_from(&im.options) else {
            return base;
        };
        let tkey = self.table_key(table);
        let rows = self.row_count(&tkey).unwrap_or(0) + self.buffered_row_count(&tkey);
        let (
            partitions,
            height,
            tree_rows,
            mem_rows,
            mem_bytes,
            file_backed,
            directory_bytes,
            layers,
            tail_fixups,
        ) = {
            let tables = self.tables.lock().unwrap();
            let entry = tables.get(&tkey);
            let (mem_rows, tail_fixups) = entry
                .and_then(|e| e.spann_mem.get(&im.id))
                .map(|tail| (tail.len(), tail.fixup_steps()))
                .unwrap_or((0, 0));
            match entry.and_then(|e| e.spann.get(&im.id)) {
                Some(handle) => {
                    let (partitions, height) = handle.shape();
                    (
                        partitions,
                        height,
                        handle.len(),
                        mem_rows,
                        handle.mem_usage(),
                        handle.is_file_backed(),
                        handle.directory_bytes(),
                        handle.layers(),
                        tail_fixups,
                    )
                }
                None => (0, 0, 0, mem_rows, 0, false, 0, 0, tail_fixups),
            }
        };
        let mut chunks = 0usize;
        let mut backlog = 0usize;
        for (segment, _) in self.segment_row_counts(&tkey) {
            let Some(dir) = self.spann_chunk_dir(&tkey, &im.name, segment) else {
                continue;
            };
            let live = self.live_chunk_files(&tkey, &im.name, segment, &dir);
            chunks += live.len();
            backlog += live.len().saturating_sub(1);
        }
        // `flat` is not a failure: below `min_rows` a tree costs a k-means for a
        // scan the exact path already answers in one pass (§18's open question).
        let degraded = if rows < opts.min_rows || tree_rows == 0 {
            "flat"
        } else {
            "tree"
        };
        let stats = self.spann_stats(im.id).snapshot();
        let recall = self
            .context
            .recall
            .lock()
            .unwrap()
            .get(&(tkey.clone(), im.id))
            .and_then(|monitor| monitor.estimate())
            .map(|value| format!("{value:.3}"))
            .unwrap_or_else(|| "n/a".to_string());
        let metric = match opts.metric {
            Metric::L2 => "l2",
            Metric::Ip => "ip",
            Metric::Cos => "cosine",
        };
        let encode = match opts.encode {
            Encode::Rabitq => "rabitq",
            Encode::None => "none",
        };
        format!(
            "{base}, metric = {metric}, encode_type = {encode}, \
             max_partition_size = {}, min_rows = {}, adaptive_beam = {}, \
             spann_partitions = {partitions}, spann_height = {height}, \
             spann_rows = {tree_rows}, spann_mem_rows = {mem_rows}, \
             spann_mem_bytes = {mem_bytes}, spann_file_backed = {file_backed}, \
             spann_chunks = {chunks}, \
             spann_max_open_chunks = {}, spann_degraded = {degraded}, \
             spann_compaction_backlog = {backlog}, \
             spann_candidates_per_query = {:.1}, spann_beam_actual = {:.1}, \
             spann_early_stop_ratio = {:.3}, spann_recall_estimate = {recall}, \
             spann_directory_bytes = {directory_bytes}, spann_chain_layers = {layers}, \
             spann_mapped_bytes = {}, spann_pages_released = {}, \
             spann_rebuilds = {}, spann_deltas = {}, spann_refused = {}, \
             spann_orphan_chunks = {}, spann_fixup_steps = {}, \
             spann_dead_rows = {}, spann_build_progress = {}",
            opts.max_partition_size,
            opts.min_rows,
            opts.adaptive_beam,
            self.max_open_chunks_per_index(),
            stats.candidates_per_query,
            stats.beam_actual,
            stats.early_stop_ratio,
            storage::buffer::mapped::mapped_bytes(),
            storage::buffer::mapped::pages_released_bytes(),
            stats.rebuilds,
            stats.deltas,
            stats.refused,
            stats.orphans,
            stats.fixup_steps + tail_fixups,
            stats.dead_rows,
            stats.build_progress,
        )
    }

    /// Builds a spann tree over `column` **without holding the column in RAM**.
    ///
    /// The vectors are read where they already are: `EMBEDDING(FLOAT, dim)` is
    /// `ColumnLayout::Fixed(dim * 4)` of inline `f32`, so `blocks::spann_source`
    /// hands the builder a slice of the block that holds the row - no text
    /// rendering, no second copy of the column on disk, no mapping of one.
    ///
    /// Two cases still go through [`Self::spann_scratch`]: a non-`f32` element
    /// type (`FLOAT16`/`DOUBLE`/`BIT`, whose body is not an `f32` array) and rows
    /// still in the append buffer (they have no block body yet). Both are rare
    /// next to a sealed column of `FLOAT` embeddings, and both need the same
    /// "one random read per member" contract the builder has.
    ///
    /// Peak heap is therefore the finished tree's codes (208 B/row at 1536d)
    /// plus one block, instead of `text + f32` for the whole table.
    pub(crate) fn spann_build(
        &self,
        table: &str,
        column: usize,
        opts: index::spann::Options,
        dir: &std::path::Path,
        segment: Option<u32>,
    ) -> Option<index::spann::SpannIndex> {
        let segments = segment.map(|segment| std::collections::HashSet::from([segment]));
        self.spann_build_in(table, column, opts, dir, segments.as_ref())
    }

    /// [`Self::spann_build`] over a **set** of segments (compaction rebuilds
    /// from the merged segment only: the source segments are dropped at the
    /// compaction's ts, which the bottom half is still below).
    pub(crate) fn spann_build_in(
        &self,
        table: &str,
        column: usize,
        opts: index::spann::Options,
        dir: &std::path::Path,
        segments: Option<&std::collections::HashSet<u32>>,
    ) -> Option<index::spann::SpannIndex> {
        // The vectors are already in the column: reading them in place - a
        // borrow of the block's `f32` payload - is the build's whole input, so
        // no scratch copy of the column is written or mapped (§19.3).
        if let Some(source) = self.spann_vector_source(table, column, segments) {
            return index::spann::SpannIndex::build_from_source(&source, opts).ok();
        }
        // A non-`f32` element type or rows still in the append buffer: both are
        // the cases the scratch exists for, and both are rare next to a sealed
        // column of `FLOAT` embeddings.
        let (ids, dim, path) = self.spann_scratch(table, column, segments, dir).ok()??;
        let built = storage::knn_index::spann::build_from_scratch(&path, ids, dim, opts).ok();
        let _ = std::fs::remove_file(&path);
        built
    }

    /// Builds `segment`'s tree and **streams** it into `path`, returning the
    /// chunk's size.
    ///
    /// One chunk per segment is what bounds a dump: the tree that has to be
    /// resident is one segment's, not the table's, and a checkpoint that only
    /// touched one segment can republish just that chunk.
    pub(crate) fn spann_write_chunk(
        &self,
        table: &str,
        column: usize,
        opts: index::spann::Options,
        segment: u32,
        path: &std::path::Path,
    ) -> Option<u64> {
        let index = self.spann_build(table, column, opts, &self.dir.join("tmp"), Some(segment))?;
        // `write_to` publishes through `storage::chunk_store`: the bytes are
        // fsynced and the directory entry renamed into place there, and on a
        // persistence profile the chunk becomes an object - so there is no
        // separate sync to do here.
        storage::knn_index::spann::write_to(&index, path, segment).ok()
    }

    /// Writes `column`'s vectors to `dir/spann-scratch-<pid>-<n>.f32` in row
    /// order, returning `(row ids, dims, path)`; `None` when the table is empty.
    ///
    /// Rows whose width disagrees with the first row are an error, not silently
    /// dropped: the vector column has a declared dimension, so a ragged cell
    /// means the data is wrong and a short index would hide it.
    fn spann_scratch(
        &self,
        table: &str,
        column: usize,
        segments: Option<&std::collections::HashSet<u32>>,
        dir: &std::path::Path,
    ) -> Result<Option<(Vec<u64>, usize, std::path::PathBuf)>> {
        use std::io::Write;
        use std::sync::atomic::{AtomicUsize, Ordering};
        static NEXT: AtomicUsize = AtomicUsize::new(0);
        std::fs::create_dir_all(dir)
            .map_err(|e| Error::Invalid(format!("spann scratch dir: {e}")))?;
        let path = dir.join(format!(
            "spann-scratch-{}-{}.f32",
            std::process::id(),
            NEXT.fetch_add(1, Ordering::Relaxed)
        ));
        let mut out = std::io::BufWriter::with_capacity(
            1 << 20,
            std::fs::File::create(&path)
                .map_err(|e| Error::Invalid(format!("spann scratch: {e}")))?,
        );
        let mut ids: Vec<u64> = Vec::new();
        let mut dim = 0usize;
        let mut ragged: Option<(usize, usize)> = None;
        let mut failure: Option<std::io::Error> = None;
        let mut bytes: Vec<u8> = Vec::new();
        // A row can be in a block *and* in the append buffer: a commit
        // publishes the block, the in-memory copy is cleared by the next
        // checkpoint. Compaction's bottom half hits exactly that window, and a
        // duplicate would put the same row id in two leaves.
        let mut seen: std::collections::HashSet<u64> = std::collections::HashSet::new();
        self.for_each_block_cell(table, column, segments, |seq, text| {
            if ragged.is_some() || failure.is_some() {
                return;
            }
            if !seen.insert(seq as u64) {
                return;
            }
            let vector = crate::engine::state::parse_vec_f32(text);
            if ids.is_empty() {
                dim = vector.len();
            } else if vector.len() != dim {
                ragged = Some((dim, vector.len()));
                return;
            }
            bytes.clear();
            bytes.reserve(vector.len() * 4);
            for x in &vector {
                bytes.extend_from_slice(&x.to_le_bytes());
            }
            if let Err(e) = out.write_all(&bytes) {
                failure = Some(e);
                return;
            }
            ids.push(seq as u64);
        });
        if let Some((want, got)) = ragged {
            let _ = std::fs::remove_file(&path);
            return Err(Error::Invalid(format!(
                "spann build: row of column {column} has {got} dims, expected {want}"
            )));
        }
        if let Some(e) = failure {
            let _ = std::fs::remove_file(&path);
            return Err(Error::Invalid(format!("spann scratch write: {e}")));
        }
        out.flush()
            .map_err(|e| Error::Invalid(format!("spann scratch flush: {e}")))?;
        drop(out);
        if ids.is_empty() {
            let _ = std::fs::remove_file(&path);
            return Ok(None);
        }
        Ok(Some((ids, dim, path)))
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn pairs(items: &[(&str, &str)]) -> Vec<(String, String)> {
        items
            .iter()
            .map(|(k, v)| (k.to_string(), v.to_string()))
            .collect()
    }

    /// A file-backed handle holds one chunk per segment, and the query's answer
    /// is the merge of all of them (§6, §19.3).
    ///
    /// Each chunk only covers its own rows, so a handle that looked at one chunk
    /// would miss half the table - which is what a segment-scoped dump makes
    /// possible in the first place.
    #[test]
    fn a_file_handle_merges_every_segment_chunk() {
        let dir = std::env::temp_dir().join(format!("spann-merge-{}", std::process::id()));
        std::fs::create_dir_all(&dir).unwrap();
        let options = options_from(&pairs(&[("max_partition_size", "32")])).unwrap();
        let dim = 8;
        let chunk = |name: &str, segment: u32, sign: f32| {
            // Row ids are `segment << 32 | offset`, and the chunk stores only
            // the offset (§6.2) - `write_to` refuses an id from another segment.
            let rows: Vec<(u64, Vec<f32>)> = (0..40)
                .map(|i| {
                    let mut v = vec![0.0f32; dim];
                    v[0] = sign * (1.0 + i as f32 * 0.01);
                    v[1] = (i % 5) as f32 * 0.1;
                    ((u64::from(segment) << 32) | i as u64, v)
                })
                .collect();
            let index = SpannIndex::build(rows, options.clone()).unwrap();
            let path = dir.join(name);
            storage::knn_index::spann::write_to(&index, &path, segment).unwrap();
            let opened = storage::knn_index::spann::FileIndex::open(&path, &options).unwrap();
            Arc::new(storage::knn_index::spann::ChunkChain::from_layers(
                vec![Arc::new(opened)],
                &options,
            ))
        };
        let handle = SpannHandle::File(vec![chunk("seg0.idx", 0, 1.0), chunk("seg1.idx", 1, -1.0)]);
        assert_eq!(handle.len(), 80, "coverage is the sum of the chains");

        // The origin is between the two clusters, so the six nearest rows are
        // three from each side: a merge that stopped at one chunk would answer
        // with a single sign.
        let hits = handle.search(&vec![0.0f32; dim], 6, Some(8));
        assert_eq!(hits.len(), 6);
        let first = hits.iter().filter(|(id, _)| *id >> 32 == 0).count();
        let second = hits.iter().filter(|(id, _)| *id >> 32 == 1).count();
        assert!(
            first > 0 && second > 0,
            "both segments must contribute: {hits:?}"
        );
        assert_eq!(handle.row_ids().len(), 80);
        let _ = std::fs::remove_dir_all(&dir);
    }

    #[test]
    fn parses_every_documented_option() {
        let opts = options_from(&pairs(&[
            ("metric", "cosine"),
            ("max_partition_size", "64"),
            ("min_partition_size", "16"),
            ("fill_factor", "0.75"),
            ("encode", "rabitq"),
            ("num_centroids", "32"),
            ("kmeans_iterations", "4"),
            ("seed", "42"),
            ("beam", "6"),
        ]))
        .unwrap();
        assert_eq!(opts.metric, Metric::Cos);
        assert_eq!(opts.max_partition_size, 64);
        assert_eq!(opts.min_partition_size, 16);
        assert!((opts.fill_factor - 0.75).abs() < 1e-6);
        assert_eq!(opts.encode, Encode::Rabitq);
        assert_eq!(opts.root_fanout, 32);
        assert_eq!(opts.kmeans_iterations, 4);
        assert_eq!(opts.seed, 42);
        assert_eq!(opts.base_beam, 6);
    }

    #[test]
    fn defaults_are_the_designs_p0_values() {
        let opts = options_from(&[]).unwrap();
        assert_eq!(opts.metric, Metric::L2);
        assert_eq!(opts.max_partition_size, 128);
        assert_eq!(opts.base_beam, 8);
        assert_eq!(opts.encode, Encode::Rabitq);
    }

    #[test]
    fn rejects_unknown_and_out_of_range_options() {
        assert!(options_from(&pairs(&[("M", "16")])).is_err());
        assert!(options_from(&pairs(&[("METRIC", "hamming")])).is_err());
        assert!(options_from(&pairs(&[("BEAM", "0")])).is_err());
        assert!(options_from(&pairs(&[("FILL_FACTOR", "2.0")])).is_err());
        assert!(options_from(&pairs(&[("SEED", "abc")])).is_err());
    }

    #[test]
    fn builds_and_answers() {
        let rows: Vec<(u64, Vec<f32>)> = (0..200)
            .map(|i| {
                (
                    i as u64,
                    (0..32).map(|d| ((i * 7 + d * 3) % 29) as f32).collect(),
                )
            })
            .collect();
        let options = options_from(&pairs(&[("max_partition_size", "32")])).unwrap();
        let index = SpannIndex::build(rows.clone(), options).unwrap();
        assert_eq!(index.len(), 200);
        let hits = index.search(&rows[0].1, 10, None);
        assert_eq!(hits.len(), 10);
        assert!(
            hits.iter().any(|(id, _)| *id == 0),
            "the row itself is a hit"
        );
    }

    #[test]
    fn empty_input_is_reported_as_invalid() {
        assert!(SpannIndex::build(Vec::new(), Options::default()).is_err());
    }

    #[test]
    fn the_mem_index_row_bound_follows_max_partition_size() {
        let opts = options_from(&pairs(&[("max_partition_size", "32")])).unwrap();
        assert_eq!(mem_dump_threshold(&opts), 32 * MEM_INDEX_DUMP_FACTOR);
        // The default (128) is 8192 rows: small enough that a checkpoint-less
        // import dumps long before the list dominates the query.
        assert_eq!(
            mem_dump_threshold(&Options::default()),
            128 * MEM_INDEX_DUMP_FACTOR
        );
        // Never zero, so the check cannot fire on an empty list forever.
        let mut tiny = opts.clone();
        tiny.max_partition_size = 0;
        assert_eq!(mem_dump_threshold(&tiny), 1);
    }

    #[test]
    fn candidate_budget_follows_the_reference_formula() {
        // k=10, beam=32, multiplier=50: 12 + ⌊5·log2(10)⌋·50 = 812.
        assert_eq!(candidate_budget(10, 32, 50), 812);
        // A narrower beam narrows the pool; multiplier 0 keeps only `results`.
        assert_eq!(candidate_budget(10, 8, 50), 462);
        assert_eq!(candidate_budget(10, 32, 0), 12);
        // Never below the requested top-n, never above the I/O ceiling.
        assert_eq!(candidate_budget(1, 2, 0), 10);
        assert_eq!(candidate_budget(100, 2048, 100), MAX_RERANK_CANDIDATES);
    }

    fn tail_rows(n: usize, dim: usize, clusters: usize) -> Vec<(u64, Vec<f32>)> {
        let mut state = 0x2545_F491_4F6C_DD1Du64;
        let mut next = move || {
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            (state >> 40) as f32 / (1u32 << 24) as f32
        };
        let centres: Vec<Vec<f32>> = (0..clusters)
            .map(|_| (0..dim).map(|_| next() * 2.0 - 1.0).collect())
            .collect();
        (0..n)
            .map(|i| {
                let centre = &centres[i % clusters];
                // Spread comparable to the inter-centre distance: a 1-bit code
                // is a *direction* estimate, so a cluster of near-identical
                // points measures the quantizer's noise, not the index.
                let v: Vec<f32> = (0..dim)
                    .map(|d| centre[d] + (next() * 2.0 - 1.0) * 0.8)
                    .collect();
                (i as u64, v)
            })
            .collect()
    }

    #[test]
    fn a_tail_answers_for_every_row_it_holds() {
        let dim = 64;
        let rows = tail_rows(600, dim, 16);
        let opts = Options::default();
        let tail = SpannTail::new(dim, opts).unwrap();
        for (id, v) in &rows {
            tail.insert(*id, v).unwrap();
        }
        assert_eq!(tail.len(), 600);
        assert_eq!(tail.row_ids().len(), 600);
        // A row inserted a moment ago is *visible*: the tail scans to the end
        // while it is below the dump threshold.
        // What the engine consumes is a *candidate set*: the executor rescores
        // every candidate exactly, so the assertion is that a row is among the
        // candidates its own vector asks for, and that top-1 stays high enough
        // to catch a broken walk.
        let mut top1 = 0;
        let mut found = 0;
        for (id, v) in &rows {
            let hits = tail.search(v, 16, None);
            if hits.first().map(|(got, _)| *got) == Some(*id) {
                top1 += 1;
            }
            if hits.iter().any(|(got, _)| *got == *id) {
                found += 1;
            }
        }
        assert!(top1 >= 510, "top-1 self-query {top1}/600");
        assert_eq!(found, 600, "candidate recall {found}/600");
    }

    #[test]
    fn a_deleted_row_leaves_the_tail() {
        let dim = 32;
        let rows = tail_rows(200, dim, 8);
        let tail = SpannTail::new(dim, Options::default()).unwrap();
        for (id, v) in &rows {
            tail.insert(*id, v).unwrap();
        }
        let gone: std::collections::HashSet<u64> = rows[..50].iter().map(|(id, _)| *id).collect();
        tail.forget(&gone);
        assert_eq!(tail.len(), 150);
        assert_eq!(tail.row_ids().len(), 150);
        // The deleted rows must not come back through a query either.
        for (id, v) in &rows[..50] {
            let answered = tail
                .search(v, 10, None)
                .into_iter()
                .any(|(got, _)| got == *id);
            assert!(!answered, "row {id} was deleted but a query returned it");
        }
    }

    #[test]
    fn concurrent_inserts_and_queries_keep_every_row() {
        let dim = 48;
        let rows = std::sync::Arc::new(tail_rows(1600, dim, 16));
        let tail = SpannTail::new(
            dim,
            Options {
                kmeans_iterations: 4,
                ..Options::default()
            },
        )
        .unwrap();

        let writers: Vec<_> = (0..4)
            .map(|w| {
                let tail = Arc::clone(&tail);
                let rows = Arc::clone(&rows);
                std::thread::spawn(move || {
                    for (i, (id, v)) in rows.iter().enumerate() {
                        if i % 4 == w {
                            tail.insert(*id, v).unwrap();
                        }
                    }
                })
            })
            .collect();
        let readers: Vec<_> = (0..2)
            .map(|_| {
                let tail = Arc::clone(&tail);
                let rows = Arc::clone(&rows);
                std::thread::spawn(move || {
                    for probe in 0..64 {
                        let (id, v) = &rows[(probe * 7) % rows.len()];
                        let hits = tail.search(v, 3, None);
                        assert!(hits.len() <= 3);
                        // A row either is not inserted yet or must be found;
                        // nothing may be answered with a wrong id, and the
                        // walk must not panic on a concurrent split.
                        if !hits.is_empty() {
                            assert!(hits.iter().all(|(got, _)| *got < rows.len() as u64));
                        }
                        let _ = id;
                    }
                })
            })
            .collect();
        for handle in writers {
            handle.join().unwrap();
        }
        for handle in readers {
            handle.join().unwrap();
        }

        assert_eq!(tail.len(), rows.len(), "no insert may be lost");
        assert_eq!(tail.row_ids().len(), rows.len());
        // 1600 rows over 16 clusters at 48d is where a 1-bit code starts to
        // blur inside a cluster; the engine's own budget is wider than 32
        // candidates, so this is a floor, not the operating point.
        let mut found = 0;
        for (id, v) in rows.iter() {
            if tail.search(v, 32, None).iter().any(|(got, _)| *got == *id) {
                found += 1;
            }
        }
        // This test is about concurrency, not the quantizer: it checks that a
        // parallel insert loses nothing and that a query never panics on a
        // split. The recall floor is loose on purpose.
        assert!(
            found >= rows.len() * 9 / 10,
            "candidate recall {found}/{}",
            rows.len()
        );
    }
}
