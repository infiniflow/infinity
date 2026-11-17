//! cspann: the cluster-based vector index of `docs/CSPANN_DESIGN.md`.
//!
//! The crate boundary follows §3.2: this module depends only on `crate::vector`
//! (`Metric`, `l2_sq`), `crate::rabitq` (the sign-flip RNG shared with the HNSW
//! port) and the standard library — no storage, no executor, no mmap. That is
//! what lets the whole algorithm run against `MemStore` in unit tests, and what
//! lets the chunk reader in `crates/storage` reuse it unchanged.
//!
//! Shape of the tree (§5):
//!
//! ```text
//! root (level = height, holds the child centroids as members)
//!   ├── branch (level = 2)
//!   └── leaf  (level = 1, holds row ids)
//! ```
//!
//! Every partition's codes are measured against **its own** centroid, so a leaf
//! scan needs one `QueryCode` per partition, and the centroid distance doubles
//! as the radius pruning bound (§8.3).

pub mod build;
pub mod kmeans;
pub mod partition;
pub mod quantize;
pub mod search;
pub mod source;
pub mod store;

pub use build::{build_tree, BuiltTree};
pub use partition::{Member, Partition};
pub use quantize::{Code, QueryCode, Rotation};
pub use search::{Beam, SearchSet};
pub use source::{Memory, VectorSource};
pub use store::{CspannStore, MemStore};

use std::fmt::{self, Display};

use crate::vector::Metric;

/// Partition identifier. `u32` because the metadata table is 64 B per partition
/// and the root is fixed at 0.
pub type PartitionKey = u32;

/// The root is always key 0 (pre-order numbering).
pub const ROOT_KEY: PartitionKey = 0;

/// Slot encoding (`CSPANN_DESIGN.md` §6.2 / D1).
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Encode {
    /// Raw `f32` vectors, for validating the tree against a brute-force scan.
    None,
    /// 1 bit per dimension, the production encoding.
    Rabitq,
}

impl Encode {
    pub fn parse(text: &str) -> Option<Self> {
        match text.trim().to_ascii_lowercase().as_str() {
            "none" | "raw" | "f32" => Some(Encode::None),
            "rabitq" | "rabit" | "bit" => Some(Encode::Rabitq),
            _ => None,
        }
    }

    pub fn as_str(self) -> &'static str {
        match self {
            Encode::None => "none",
            Encode::Rabitq => "rabitq",
        }
    }

    /// Bytes of the payload column group (§6.2): codes, or the raw vector.
    pub fn code_bytes(self, dim: usize) -> usize {
        match self {
            Encode::None => dim * 4,
            Encode::Rabitq => quantize::code_bytes(dim),
        }
    }

    /// Bytes one member occupies: meta + payload.
    pub fn record_bytes(self, dim: usize) -> usize {
        match self {
            Encode::None => dim * 4,
            Encode::Rabitq => quantize::META_BYTES + quantize::code_bytes(dim),
        }
    }
}

/// Build-time knobs (`CSPANN_DESIGN.md` §11.3, D3/D4).
#[derive(Clone, Debug, PartialEq)]
pub struct Options {
    pub metric: Metric,
    /// `max_partition_size` of a leaf (§11.3: 128 ≈ a 4 KB page at 1536d).
    pub max_partition_size: usize,
    /// Splits below this are refused (§9.5's anti-thrash bound).
    pub min_partition_size: usize,
    /// Target fill ratio; the leaf count follows `n / (fill · max)`.
    pub fill_factor: f32,
    pub encode: Encode,
    pub kmeans_iterations: usize,
    /// Persisted in the chunk header; rebuilds the same rotation (§18).
    pub seed: u64,
    /// Partitions probed per level when the statement does not override it.
    pub base_beam: usize,
    /// Root fan-out: `num_centroids` of §11.2. The bisection tree is collapsed
    /// above `log2(root_fanout)` levels into one wide root (`build::collapse_top`).
    pub root_fanout: usize,
}

impl Default for Options {
    fn default() -> Self {
        Self {
            metric: Metric::L2,
            max_partition_size: 128,
            min_partition_size: 32,
            fill_factor: 0.9,
            encode: Encode::Rabitq,
            kmeans_iterations: 8,
            seed: 0x5EED_1234_ABCD_0001,
            base_beam: 8,
            root_fanout: 64,
        }
    }
}

impl Options {
    pub fn validate(&self) -> Result<(), CspannError> {
        if self.max_partition_size < 4 {
            return Err(CspannError::InvalidOption(
                "max_partition_size",
                "must be >= 4".to_string(),
            ));
        }
        if self.min_partition_size < 2 || self.min_partition_size > self.max_partition_size {
            return Err(CspannError::InvalidOption(
                "min_partition_size",
                "must be in 2..=max_partition_size".to_string(),
            ));
        }
        if !(0.3..=1.0).contains(&self.fill_factor) {
            return Err(CspannError::InvalidOption(
                "fill_factor",
                "must be within 0.3..=1.0".to_string(),
            ));
        }
        if self.kmeans_iterations == 0 || self.kmeans_iterations > 64 {
            return Err(CspannError::InvalidOption(
                "kmeans_iterations",
                "must be in 1..=64".to_string(),
            ));
        }
        if self.base_beam == 0 || self.base_beam > 256 {
            return Err(CspannError::InvalidOption(
                "beam",
                "must be in 1..=256".to_string(),
            ));
        }
        if self.root_fanout < 2 || self.root_fanout > 4096 {
            return Err(CspannError::InvalidOption(
                "num_centroids",
                "must be in 2..=4096".to_string(),
            ));
        }
        Ok(())
    }
}

#[derive(Debug)]
pub enum CspannError {
    Empty,
    DimensionMismatch {
        expected: usize,
        actual: usize,
    },
    InvalidOption(&'static str, String),
    /// A chunk could not be parsed; the message names the failing section.
    Corrupt(String),
}

impl Display for CspannError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            CspannError::Empty => write!(f, "cspann: no rows to index"),
            CspannError::DimensionMismatch { expected, actual } => {
                write!(f, "cspann: expected dim {expected}, got {actual}")
            }
            CspannError::InvalidOption(name, why) => write!(f, "cspann: invalid {name}: {why}"),
            CspannError::Corrupt(what) => write!(f, "cspann: corrupt chunk: {what}"),
        }
    }
}

impl std::error::Error for CspannError {}

/// A built cspann index.
#[derive(Debug)]
pub struct CspannIndex {
    opts: Options,
    dim: usize,
    rotation: Rotation,
    store: MemStore,
}

impl CspannIndex {
    /// Builds the tree from `(row id, vector)` pairs held in memory.
    ///
    /// Convenience wrapper over [`Self::build_from_source`] for callers that
    /// already have the column resident (tests, small `CREATE INDEX`).
    pub fn build(rows: Vec<(u64, Vec<f32>)>, opts: Options) -> Result<Self, CspannError> {
        Self::build_from_source(&Memory::new(rows), opts)
    }

    /// Builds the tree over any [`VectorSource`].
    ///
    /// This is the out-of-core entry point: the builder only ever reads one
    /// vector at a time, so a source backed by a mapped scratch file keeps the
    /// 6 KB/row (1536d) column out of RSS.
    pub fn build_from_source(src: &dyn VectorSource, opts: Options) -> Result<Self, CspannError> {
        opts.validate()?;
        if src.is_empty() {
            return Err(CspannError::Empty);
        }
        let dim = src.vector(0).len();
        if dim == 0 {
            return Err(CspannError::InvalidOption(
                "dims",
                "vector dimension must be > 0".to_string(),
            ));
        }
        for i in 0..src.len() {
            let actual = src.vector(i).len();
            if actual != dim {
                return Err(CspannError::DimensionMismatch {
                    expected: dim,
                    actual,
                });
            }
        }
        let rotation = Rotation::new(dim, opts.seed);
        let tree = build::collapse_top(
            build_tree(src, dim, &opts, &rotation),
            dim,
            &opts,
            &rotation,
        );
        Ok(Self {
            opts,
            dim,
            rotation,
            store: MemStore::new(dim, tree),
        })
    }

    /// Rebuilds an index from storage. `rotation` must be `Rotation::new(dim,
    /// opts.seed)` — the chunk only persists the seed (§6.1), and `from_parts`
    /// deliberately does not reconstruct it silently, so a mismatched seed shows
    /// up as wrong distances instead of a silent rebuild with another rotation.
    pub fn from_parts(
        opts: Options,
        dim: usize,
        rotation: Rotation,
        parts: Vec<Partition>,
        height: u32,
        root: PartitionKey,
    ) -> Self {
        let store = MemStore::from_parts(dim, root, height, parts);
        Self {
            opts,
            dim,
            rotation,
            store,
        }
    }

    pub fn opts(&self) -> &Options {
        &self.opts
    }

    pub fn dim(&self) -> usize {
        self.dim
    }

    pub fn height(&self) -> u32 {
        self.store.height()
    }

    pub fn len(&self) -> usize {
        self.store.len()
    }

    pub fn is_empty(&self) -> bool {
        self.store.is_empty()
    }

    pub fn rotation(&self) -> &Rotation {
        &self.rotation
    }

    pub fn store(&self) -> &MemStore {
        &self.store
    }

    pub fn partitions(&self) -> &[Partition] {
        self.store.partitions()
    }

    /// Heap bytes the tree owns, for §10's budget and the `cspann_bytes` gauge.
    ///
    /// Counts the two centroids per partition, the child key table and the
    /// members (meta + code); it excludes the allocator's slack and the raw
    /// vectors `encode = none` keeps, which is why the benchmark reports it next
    /// to the process RSS rather than instead of it.
    pub fn mem_usage(&self) -> usize {
        let mut total = 0usize;
        for part in self.store.partitions() {
            total += part.dim() * 4 * 2;
            total += part.children.len() * 4;
            total += part.members.len() * std::mem::size_of::<Member>();
            for member in &part.members {
                total += member.code.bits.len();
            }
        }
        total
    }

    /// `(key, level, centroid, radius)` in key order — what the format writer
    /// turns into section B and the metadata table.
    pub fn centroids(&self) -> Vec<(PartitionKey, u32, &[f32], f32)> {
        self.store
            .partitions()
            .iter()
            .map(|p| (p.key, p.level, p.centroid.as_slice(), p.radius))
            .collect()
    }

    /// Every covered row id, ascending (the format's section F).
    pub fn row_ids(&self) -> Vec<u64> {
        let mut ids: Vec<u64> = self
            .store
            .partitions()
            .iter()
            .filter(|p| p.is_leaf())
            .flat_map(|p| p.members.iter().map(|m| m.id))
            .collect();
        ids.sort_unstable();
        ids
    }

    /// Approximate k-NN. `beam = None` uses `Options::base_beam`; `Some(usize::MAX)`
    /// disables the beam truncation (only the radius bound prunes), which is the
    /// `encode = none` exactness check.
    pub fn search(&self, q: &[f32], k: usize, beam: Option<usize>) -> Vec<(u64, f32)> {
        self.search_masked(q, k, beam, None)
    }

    /// [`Self::search`] restricted to the rows `allowed` admits, with the mask
    /// tested per slot (`Partition::search_into`).
    pub fn search_filtered(
        &self,
        q: &[f32],
        k: usize,
        beam: Option<usize>,
        allowed: &std::collections::HashSet<u64>,
    ) -> Vec<(u64, f32)> {
        self.search_masked(q, k, beam, Some(allowed))
    }

    fn search_masked(
        &self,
        q: &[f32],
        k: usize,
        beam: Option<usize>,
        allowed: Option<&std::collections::HashSet<u64>>,
    ) -> Vec<(u64, f32)> {
        if k == 0 || q.len() != self.dim {
            return Vec::new();
        }
        let Some(root) = self.store.partition(self.store.root()) else {
            return Vec::new();
        };
        let width = beam.unwrap_or(self.opts.base_beam).max(1);
        let mut current: Vec<(f32, PartitionKey)> = vec![(0.0, root.key)];
        let mut level = root.level;
        while level > 1 {
            // Interior levels narrow: the root of a 1M-row tree holds a handful
            // of children, so spending the leaf width there only scans
            // centroids (see `search::level_width`).
            let mut next = Beam::new(search::level_width(width, level - 1));
            for (_, key) in &current {
                let Some(p) = self.store.partition(*key) else {
                    continue;
                };
                for child in &p.children {
                    if let Some(c) = self.store.partition(*child) {
                        next.push(c.lower_bound(q, self.opts.metric), *child);
                    }
                }
            }
            current = next.into_items();
            if current.is_empty() {
                return Vec::new();
            }
            level -= 1;
        }

        let mut set = SearchSet::with_default_capacity(k);
        // `current` is sorted by lower bound, so the first partition that cannot
        // beat the incumbent k-th result ends the leaf scan: that is the
        // descent's early stop, and it is what lets a wide beam stay cheap when
        // the query is unambiguous.
        for (bound, key) in &current {
            let Some(p) = self.store.partition(*key) else {
                continue;
            };
            // Radius pruning needs the metric's distance to be bounded below by
            // ‖q−c‖ − R, which only holds for L2 (§8.3).
            if self.opts.metric.is_l2() {
                if let Some(kth) = set.kth_upper() {
                    if !search::l2_can_beat(*bound, kth) {
                        break;
                    }
                }
            }
            let query = p.make_query(&self.rotation, q);
            p.search_into(&query, q, self.opts.metric, &mut set, allowed);
        }
        set.into_sorted()
    }

    /// The statement path: `ef` is the candidate budget the executor asks for.
    pub fn search_candidates(&self, q: &[f32], ef: usize) -> Vec<(u64, f32)> {
        self.search(q, ef.max(1), None)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::vector::l2_sq;

    fn clustered(n: usize, dim: usize, clusters: usize) -> Vec<(u64, Vec<f32>)> {
        (0..n)
            .map(|i| {
                let c = i % clusters;
                let v: Vec<f32> = (0..dim)
                    .map(|d| {
                        let base = ((c * 31 + d * 7) % 41) as f32 - 20.0;
                        // Intra-cluster spread comparable to the inter-cluster
                        // distance: a 1-bit code is relative, so a test with
                        // near-identical points inside a cluster measures noise,
                        // not recall.
                        base + ((i * 13 + d * 5) % 17) as f32 * 0.5
                    })
                    .collect();
                (i as u64, v)
            })
            .collect()
    }

    /// Independent pseudo-random value per `(row, dim)`: the realistic shape for
    /// a recall test (the clustered fixture is a 1-D manifold, which a 1-bit
    /// code cannot represent).
    fn noise(i: usize, d: usize) -> f32 {
        let mut x = (i as u64).wrapping_mul(0x9E37_79B9_7F4A_7C15)
            ^ (d as u64).wrapping_mul(0xC2B2_AE3D_27D4_EB4F);
        x ^= x >> 30;
        x = x.wrapping_mul(0xBF58_476D_1CE4_E5B9);
        x ^= x >> 27;
        ((x >> 40) as f32 / 16_777_216.0) * 2.0 - 1.0
    }

    fn random_points(n: usize, dim: usize, clusters: usize) -> Vec<(u64, Vec<f32>)> {
        (0..n)
            .map(|i| {
                let c = i % clusters;
                let v: Vec<f32> = (0..dim)
                    .map(|d| noise(c * 7919 + d, 3) * 20.0 + noise(i, d) * 3.0)
                    .collect();
                (i as u64, v)
            })
            .collect()
    }

    fn exact(rows: &[(u64, Vec<f32>)], q: &[f32], k: usize) -> Vec<u64> {
        let mut scored: Vec<(u64, f32)> = rows.iter().map(|(id, v)| (*id, l2_sq(q, v))).collect();
        scored.sort_by(|a, b| a.1.total_cmp(&b.1));
        scored.into_iter().take(k).map(|(id, _)| id).collect()
    }

    fn opts(max_partition_size: usize, encode: Encode) -> Options {
        Options {
            metric: Metric::L2,
            max_partition_size,
            min_partition_size: 8,
            fill_factor: 0.9,
            encode,
            kmeans_iterations: 8,
            seed: 12345,
            base_beam: 8,
            root_fanout: 64,
        }
    }

    #[test]
    fn tree_invariants_hold() {
        let rows = clustered(700, 8, 7);
        let index = CspannIndex::build(rows.clone(), opts(64, Encode::Rabitq)).unwrap();
        assert_eq!(index.len(), 700);
        assert!(index.height() >= 2, "height {}", index.height());

        let mut covered = 0usize;
        for p in index.partitions() {
            if p.is_leaf() {
                assert!(p.len() <= 64, "leaf too large: {}", p.len());
                covered += p.len();
            } else {
                assert!(!p.children.is_empty(), "branch without children");
                for child in &p.children {
                    let c = index
                        .store()
                        .partition(*child)
                        .unwrap_or_else(|| panic!("dangling child {child}"));
                    assert_eq!(p.level, c.level + 1, "level(§3 I3) violated");
                }
            }
        }
        assert_eq!(covered, 700, "every row is in exactly one leaf");
        let root = index.store().partition(ROOT_KEY).unwrap();
        assert_eq!(root.level, index.height());
    }

    #[test]
    fn encode_none_reproduces_the_exact_top_k() {
        let rows = clustered(600, 8, 6);
        let index = CspannIndex::build(rows.clone(), opts(48, Encode::None)).unwrap();
        for query in rows.iter().take(12) {
            let got: Vec<u64> = index
                .search(&query.1, 10, Some(usize::MAX))
                .into_iter()
                .map(|(id, _)| id)
                .collect();
            assert_eq!(got, exact(&rows, &query.1, 10), "query row {}", query.0);
        }
    }

    /// The same exactness check in the *tight* distance regime, where every
    /// squared distance is below 1 (normalised embeddings look like this).
    ///
    /// It is a regression guard for the radius prune: that test used to compare
    /// the partition's **distance** bound against the **squared** k-th score,
    /// which silently dropped partitions whose bound sat between `kth` and
    /// `√kth` - the band this fixture fills, and the reason a cosine index lost
    /// recall that an L2 fixture never showed.
    /// A filtered search answers from the admitted rows only, with the mask
    /// tested per slot (§12.5).
    ///
    /// The old path asked for `k / selectivity` candidates and filtered them
    /// afterwards, so a 1%-selective filter over 20k rows asked for the whole
    /// table and decoded it. Here the answer must equal the exact best among the
    /// admitted rows for every mask.
    #[test]
    fn filtered_search_answers_from_the_admitted_rows() {
        let rows = random_points(400, 8, 6);
        let index = CspannIndex::build(rows.clone(), opts(32, Encode::Rabitq)).unwrap();

        // One admitted row: whatever the beam, that row is the only answer.
        let one: std::collections::HashSet<u64> = [rows[7].0].into_iter().collect();
        let hits = index.search_filtered(&rows[200].1, 5, Some(usize::MAX), &one);
        assert_eq!(hits.len(), 1);
        assert_eq!(hits[0].0, rows[7].0);

        // A sparse mask: the answer must be the exact best *among the mask*.
        let mask: std::collections::HashSet<u64> = rows
            .iter()
            .map(|(id, _)| *id)
            .filter(|id| id % 97 == 0)
            .collect();
        let query = &rows[123].1;
        let mut want: Vec<(u64, f32)> = rows
            .iter()
            .filter(|(id, _)| mask.contains(id))
            .map(|(id, v)| {
                let d: f32 = v
                    .iter()
                    .zip(query.iter())
                    .map(|(a, b)| (a - b) * (a - b))
                    .sum();
                (*id, d)
            })
            .collect();
        want.sort_by(|a, b| a.1.partial_cmp(&b.1).unwrap());
        let got = index.search_filtered(query, 3, Some(usize::MAX), &mask);
        assert_eq!(got.len(), 3, "the mask has enough rows: {got:?}");
        assert_eq!(
            got.iter().map(|(id, _)| *id).collect::<Vec<_>>(),
            want.iter().take(3).map(|(id, _)| *id).collect::<Vec<_>>()
        );
        assert!(got.iter().all(|(id, _)| mask.contains(id)));
    }

    #[test]
    fn tight_cluster_reproduces_the_exact_top_k() {
        let rows: Vec<(u64, Vec<f32>)> = random_points(400, 16, 5)
            .into_iter()
            .map(|(id, v)| (id, v.into_iter().map(|x| x / 100.0).collect()))
            .collect();
        let index = CspannIndex::build(rows.clone(), opts(32, Encode::None)).unwrap();
        for query in rows.iter().take(12) {
            let got: Vec<u64> = index
                .search(&query.1, 10, Some(usize::MAX))
                .into_iter()
                .map(|(id, _)| id)
                .collect();
            assert_eq!(got, exact(&rows, &query.1, 10), "query row {}", query.0);
        }
    }

    #[test]
    fn rabitq_keeps_recall_high() {
        // 64d: a 1-bit-per-dimension code only becomes accurate once the
        // dimension is high enough for the sign approximation to average out
        // (at 8d the estimator is genuinely noisy — see §11.3's dim thresholds).
        let rows = random_points(600, 64, 8);
        // `max_partition_size = 128` with 600 rows gives ~8 leaves, which the
        // default beam of 8 covers completely: the only loss left is the codec.
        let index = CspannIndex::build(rows.clone(), opts(128, Encode::Rabitq)).unwrap();
        let mut hits = 0usize;
        let mut total = 0usize;
        for query in rows.iter().take(20) {
            let want = exact(&rows, &query.1, 10);
            let candidates = index.search(&query.1, 100, None);
            let got = reranked_top_k(&rows, candidates, &query.1, 10);
            hits += got.iter().filter(|id| want.contains(id)).count();
            total += want.len();
        }
        let recall = hits as f32 / total as f32;
        assert!(recall >= 0.9, "recall@10 after rerank = {recall}");
    }

    /// The production shape of a query (`D1`): the index returns `ef`
    /// candidates ranked by the 1-bit estimate, and the caller re-scores them
    /// exactly. A 1-bit code is accurate to ~1% of the cosine, which is far too
    /// coarse to order distances that differ by 1%, but it is plenty to pick the
    /// right handful of rows.
    fn reranked_top_k(
        rows: &[(u64, Vec<f32>)],
        candidates: Vec<(u64, f32)>,
        q: &[f32],
        k: usize,
    ) -> Vec<u64> {
        let mut scored: Vec<(u64, f32)> = candidates
            .into_iter()
            .map(|(id, _)| (id, l2_sq(q, &rows[id as usize].1)))
            .collect();
        scored.sort_by(|a, b| a.1.total_cmp(&b.1));
        scored.into_iter().take(k).map(|(id, _)| id).collect()
    }

    #[test]
    fn build_is_deterministic() {
        let rows = clustered(300, 6, 5);
        let a = CspannIndex::build(rows.clone(), opts(32, Encode::Rabitq)).unwrap();
        let b = CspannIndex::build(rows, opts(32, Encode::Rabitq)).unwrap();
        let ca: Vec<(u32, Vec<f32>)> = a
            .partitions()
            .iter()
            .map(|p| (p.key, p.centroid.clone()))
            .collect();
        let cb: Vec<(u32, Vec<f32>)> = b
            .partitions()
            .iter()
            .map(|p| (p.key, p.centroid.clone()))
            .collect();
        assert_eq!(ca, cb);
    }

    #[test]
    fn small_input_stays_a_single_leaf() {
        let rows = clustered(10, 4, 2);
        let index = CspannIndex::build(rows, opts(128, Encode::Rabitq)).unwrap();
        assert_eq!(index.height(), 1);
        assert_eq!(index.partitions().len(), 1);
        assert!(index.search(&[0.0, 0.0, 0.0, 0.0], 3, None).len() == 3);
    }

    #[test]
    fn rejects_bad_input() {
        let err = CspannIndex::build(Vec::new(), Options::default()).unwrap_err();
        assert!(matches!(err, CspannError::Empty));

        let rows = vec![(1u64, vec![1.0f32, 2.0]), (2, vec![1.0])];
        let err = CspannIndex::build(rows, Options::default()).unwrap_err();
        assert!(matches!(err, CspannError::DimensionMismatch { .. }));

        let bad = Options {
            base_beam: 0,
            ..Options::default()
        };
        assert!(matches!(
            CspannIndex::build(vec![(1, vec![1.0])], bad).unwrap_err(),
            CspannError::InvalidOption("beam", _)
        ));
    }

    #[test]
    fn search_tolerates_a_wrong_query_dimension() {
        let rows = clustered(20, 4, 2);
        let index = CspannIndex::build(rows, opts(16, Encode::Rabitq)).unwrap();
        assert!(index.search(&[1.0, 2.0], 5, None).is_empty());
    }
}
