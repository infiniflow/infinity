//! The executor's cspann glue: DDL option parsing and the build entry point
//! (`docs/CSPANN_DESIGN.md` §15).
//!
//! The index itself lives in `index::cspann` and knows nothing about SQL; this
//! module is the only place that turns `WITH (…)` text into `Options` and the
//! engine's row ids into the index's `(row, vector)` pairs.

use std::sync::Arc;

use common::{Error, Result};
use index::cspann::{CspannIndex, Encode, Options};
use index::vector::Metric;

/// The `WITH (…)` keys `CREATE INDEX … USING CSPANN` accepts.
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
];

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
                            "invalid metric '{other}' for a CSPANN index"
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
            other => {
                return Err(Error::Invalid(format!(
                    "invalid index parameter: unsupported CSPANN option '{other}'"
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

/// How many partitions' worth of rows a cspann **mem index** may hold before a
/// dump is triggered (`docs/CSPANN_DESIGN.md` §9.2).
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

/// A queryable cspann index: the tree a `CREATE INDEX` built, or the chunk a
/// later checkpoint published (§6.4).
///
/// The two forms must answer identically — `storage::knn_index::cspann` has a
/// test that runs the same queries through both — so the rest of the engine only
/// ever sees this enum.
pub(crate) enum CspannHandle {
    /// Built in this process and not yet published to a chunk.
    Memory(Arc<CspannIndex>),
    /// Served straight out of `chunk_{n}.idx`, **one chunk per segment**: the
    /// slot bytes are page cache, which is what makes 50M rows fit in 8 GB
    /// (§19.3), and per-segment chunks let a checkpoint republish only the
    /// segment it touched (§9.4).
    File(Vec<Arc<storage::knn_index::cspann::FileIndex>>),
}

/// Orders a union of per-chunk answers the way the metric ranks, so truncating
/// it keeps the global best rather than each chunk's best.
fn merge_chunks(
    chunks: &[Arc<storage::knn_index::cspann::FileIndex>],
    merged: &mut Vec<(u64, f32)>,
) {
    let l2 = chunks.first().is_none_or(|c| c.metric().is_l2());
    merged.sort_by(|a, b| {
        let ord = a.1.partial_cmp(&b.1).unwrap_or(std::cmp::Ordering::Equal);
        if l2 {
            ord
        } else {
            ord.reverse()
        }
    });
}

impl CspannHandle {
    pub(crate) fn len(&self) -> usize {
        match self {
            CspannHandle::Memory(index) => index.len(),
            CspannHandle::File(chunks) => chunks.iter().map(|c| c.len()).sum(),
        }
    }

    pub(crate) fn dim(&self) -> usize {
        match self {
            CspannHandle::Memory(index) => index.dim(),
            CspannHandle::File(chunks) => chunks.first().map(|c| c.dims()).unwrap_or(0),
        }
    }

    pub(crate) fn search(&self, q: &[f32], k: usize, beam: Option<usize>) -> Vec<(u64, f32)> {
        match self {
            CspannHandle::Memory(index) => index.search(q, k, beam),
            CspannHandle::File(chunks) => {
                // Every segment answers with its own `k`, and the union is what
                // the caller's exact rescoring sees: truncating per segment
                // instead would drop rows that only lose to another segment's.
                let mut merged: Vec<(u64, f32)> = Vec::new();
                for chunk in chunks {
                    merged.extend(chunk.search(q, k, beam));
                }
                merge_chunks(chunks, &mut merged);
                merged.truncate(k);
                merged
            }
        }
    }

    /// The row ids the tree holds, ascending - the coverage check's input
    /// (`docs/CSPANN_DESIGN.md` §9.7).
    pub(crate) fn row_ids(&self) -> Vec<u64> {
        match self {
            CspannHandle::Memory(index) => index.row_ids(),
            CspannHandle::File(chunks) => {
                let mut ids: Vec<u64> = chunks.iter().flat_map(|c| c.row_ids()).collect();
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
        let ids: std::collections::HashSet<u64> = allowed.iter().map(|id| *id as u64).collect();
        let mut width = beam.unwrap_or(DEFAULT_SEARCH_BEAM).max(1);
        for attempt in 0..3 {
            let mut hits = match self {
                CspannHandle::Memory(index) => index.search_filtered(q, k, Some(width), &ids),
                CspannHandle::File(chunks) => {
                    let mut merged: Vec<(u64, f32)> = Vec::new();
                    for chunk in chunks {
                        merged.extend(chunk.search_filtered(q, k, Some(width), &ids));
                    }
                    merge_chunks(chunks, &mut merged);
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

    /// `(partitions, height)`, for the bench and the `SHOW INDEX` telemetry
    /// (the latter is not wired yet, hence the allowance).
    #[allow(dead_code)]
    pub(crate) fn shape(&self) -> (usize, u32) {
        match self {
            CspannHandle::Memory(index) => (index.partitions().len(), index.height()),
            CspannHandle::File(chunks) => (
                chunks.iter().map(|c| c.partitions()).sum(),
                chunks.iter().map(|c| c.height()).max().unwrap_or(0),
            ),
        }
    }

    /// Heap bytes this handle holds. A file-backed index holds only its header
    /// and section table, so the answer is 0 and the slot pages show up as page
    /// cache instead (`RssFile`, §19.6).
    ///
    /// Only the bench reads it today; the `cspann_bytes` gauge (§16.3) will.
    #[allow(dead_code)]
    pub(crate) fn mem_usage(&self) -> usize {
        match self {
            CspannHandle::Memory(index) => index.mem_usage(),
            CspannHandle::File(_) => 0,
        }
    }

    #[allow(dead_code)]
    pub(crate) fn is_file_backed(&self) -> bool {
        matches!(self, CspannHandle::File(_))
    }
}

/// The SQL-layer default search width: Cockroach's `vector_search_beam_size`,
/// whose global default is 32. The index's own `Options::base_beam` stays 8 for
/// direct callers; the statement path is the one that pays for recall, because
/// a 1-bit code leaves the partition choice as the only lever.
pub(crate) const DEFAULT_SEARCH_BEAM: usize = 32;

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
    (results + extra).clamp(results, MAX_RERANK_CANDIDATES)
}

impl crate::Engine {
    /// Builds a cspann tree over `column` **without holding the column in RAM**.
    ///
    /// Two passes over the data, both cheap in memory:
    ///
    /// 1. [`Self::cspann_scratch`] streams the column block by block, parses
    ///    each cell and writes the `f32`s to a scratch file - the text of a row
    ///    (~15 KB at 1536d) never outlives its block;
    /// 2. the tree is built over a *mapping* of that file
    ///    (`storage::knn_index::cspann::build_from_scratch`), so the 6 KB/row of
    ///    vectors are page cache, not anonymous memory.
    ///
    /// Peak RSS is therefore the finished tree's codes (208 B/row at 1536d) plus
    /// one block, instead of `text + f32` for the whole table.
    pub(crate) fn cspann_build(
        &self,
        table: &str,
        column: usize,
        opts: index::cspann::Options,
        dir: &std::path::Path,
        segment: Option<u32>,
    ) -> Option<index::cspann::CspannIndex> {
        let (ids, dim, path) = self.cspann_scratch(table, column, segment, dir).ok()??;
        let built = storage::knn_index::cspann::build_from_scratch(&path, ids, dim, opts).ok();
        let _ = std::fs::remove_file(&path);
        built
    }

    /// Builds `segment`'s tree and **streams** it into `path`, returning the
    /// chunk's size.
    ///
    /// One chunk per segment is what bounds a dump: the tree that has to be
    /// resident is one segment's, not the table's, and a checkpoint that only
    /// touched one segment can republish just that chunk.
    pub(crate) fn cspann_write_chunk(
        &self,
        table: &str,
        column: usize,
        opts: index::cspann::Options,
        segment: u32,
        path: &std::path::Path,
    ) -> Option<u64> {
        let index = self.cspann_build(table, column, opts, &self.dir.join("tmp"), Some(segment))?;
        let len = storage::knn_index::cspann::write_to(&index, path).ok()?;
        // `write_to` streams through a buffered handle; a crash before the data
        // reaches the platter would leave a chunk the catalog already lists.
        std::fs::File::open(path).and_then(|f| f.sync_all()).ok()?;
        Some(len)
    }

    /// Writes `column`'s vectors to `dir/cspann-scratch-<pid>-<n>.f32` in row
    /// order, returning `(row ids, dims, path)`; `None` when the table is empty.
    ///
    /// Rows whose width disagrees with the first row are an error, not silently
    /// dropped: the vector column has a declared dimension, so a ragged cell
    /// means the data is wrong and a short index would hide it.
    fn cspann_scratch(
        &self,
        table: &str,
        column: usize,
        segment: Option<u32>,
        dir: &std::path::Path,
    ) -> Result<Option<(Vec<u64>, usize, std::path::PathBuf)>> {
        use std::io::Write;
        use std::sync::atomic::{AtomicUsize, Ordering};
        static NEXT: AtomicUsize = AtomicUsize::new(0);
        std::fs::create_dir_all(dir)
            .map_err(|e| Error::Invalid(format!("cspann scratch dir: {e}")))?;
        let path = dir.join(format!(
            "cspann-scratch-{}-{}.f32",
            std::process::id(),
            NEXT.fetch_add(1, Ordering::Relaxed)
        ));
        let mut out = std::io::BufWriter::with_capacity(
            1 << 20,
            std::fs::File::create(&path)
                .map_err(|e| Error::Invalid(format!("cspann scratch: {e}")))?,
        );
        let mut ids: Vec<u64> = Vec::new();
        let mut dim = 0usize;
        let mut ragged: Option<(usize, usize)> = None;
        let mut failure: Option<std::io::Error> = None;
        let mut bytes: Vec<u8> = Vec::new();
        self.for_each_block_cell(table, column, segment, |seq, text| {
            if ragged.is_some() || failure.is_some() {
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
                "cspann build: row of column {column} has {got} dims, expected {want}"
            )));
        }
        if let Some(e) = failure {
            let _ = std::fs::remove_file(&path);
            return Err(Error::Invalid(format!("cspann scratch write: {e}")));
        }
        out.flush()
            .map_err(|e| Error::Invalid(format!("cspann scratch flush: {e}")))?;
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
        let dir = std::env::temp_dir().join(format!("cspann-merge-{}", std::process::id()));
        std::fs::create_dir_all(&dir).unwrap();
        let options = options_from(&pairs(&[("max_partition_size", "32")])).unwrap();
        let dim = 8;
        let chunk = |name: &str, base: u64, sign: f32| {
            let rows: Vec<(u64, Vec<f32>)> = (0..40)
                .map(|i| {
                    let mut v = vec![0.0f32; dim];
                    v[0] = sign * (1.0 + i as f32 * 0.01);
                    v[1] = (i % 5) as f32 * 0.1;
                    (base + i as u64, v)
                })
                .collect();
            let index = CspannIndex::build(rows, options.clone()).unwrap();
            let path = dir.join(name);
            storage::knn_index::cspann::write_to(&index, &path).unwrap();
            let opened = storage::knn_index::cspann::FileIndex::open(&path, &options).unwrap();
            Arc::new(opened)
        };
        let handle = CspannHandle::File(vec![
            chunk("seg0.idx", 0, 1.0),
            chunk("seg1.idx", 1000, -1.0),
        ]);
        assert_eq!(handle.len(), 80, "coverage is the sum of the chunks");

        // The origin is between the two clusters, so the six nearest rows are
        // three from each side: a merge that stopped at one chunk would answer
        // with a single sign.
        let hits = handle.search(&vec![0.0f32; dim], 6, Some(8));
        assert_eq!(hits.len(), 6);
        let first = hits.iter().filter(|(id, _)| *id < 1000).count();
        let second = hits.iter().filter(|(id, _)| *id >= 1000).count();
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
        let index = CspannIndex::build(rows.clone(), options).unwrap();
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
        assert!(CspannIndex::build(Vec::new(), Options::default()).is_err());
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
}
