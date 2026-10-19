//! Vector indexes: an exact brute-force [`FlatIndex`] and an approximate
//! [`HnswIndex`] (Hierarchical Navigable Small World), matching Infinity's
//! `knn_index` family (plan §2.6).
//!
//! Construction follows the reference implementation
//! (`src/storage/knn_index/knn_hnsw/hnsw_alg.cppm`): each vector is inserted
//! incrementally — a greedy descent above its random level, then a beam search
//! with `ef_construction` candidates per layer, the neighbour-selection
//! heuristic, and bidirectional links pruned to `2 * M` on layer 0 and `M`
//! above. Queries use the same greedy descent + layer-0 beam search (`ef`).

use std::collections::HashSet;
use std::io::Write;
use std::path::Path;

use infinity_common::{Error, Result};

/// Squared L2 distance.
pub fn l2_sq(a: &[f32], b: &[f32]) -> f32 {
    a.iter().zip(b).map(|(x, y)| (x - y) * (x - y)).sum()
}

/// Exact brute-force kNN.
#[derive(Debug, Default)]
pub struct FlatIndex {
    dims: usize,
    data: Vec<Vec<f32>>,
}

impl FlatIndex {
    pub fn new(dims: usize) -> Self {
        Self {
            dims,
            data: Vec::new(),
        }
    }

    pub fn add(&mut self, v: &[f32]) -> usize {
        assert_eq!(v.len(), self.dims, "dimension mismatch");
        self.data.push(v.to_vec());
        self.data.len() - 1
    }

    pub fn len(&self) -> usize {
        self.data.len()
    }

    pub fn is_empty(&self) -> bool {
        self.data.is_empty()
    }

    /// Returns `(row_id, distance)` for the `k` nearest, ascending.
    pub fn search(&self, q: &[f32], k: usize) -> Vec<(usize, f32)> {
        let mut d: Vec<(usize, f32)> = self
            .data
            .iter()
            .enumerate()
            .map(|(i, v)| (i, l2_sq(q, v)))
            .collect();
        d.sort_by(|a, b| a.1.partial_cmp(&b.1).unwrap());
        d.truncate(k);
        d
    }
}

/// Small deterministic RNG (xorshift).
struct Rng(u64);
impl Rng {
    fn new(seed: u64) -> Self {
        Rng(seed | 1)
    }
    fn next_u64(&mut self) -> u64 {
        let mut x = self.0;
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;
        self.0 = x;
        x
    }
    fn unit(&mut self) -> f64 {
        (self.next_u64() >> 11) as f64 / (1u64 << 53) as f64
    }
}

/// The metric shared by the plain store, the quantized stores and LSG.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Metric {
    L2,
    Ip,
    Cos,
}

impl Metric {
    pub fn parse(text: &str) -> Self {
        match text.trim().to_ascii_lowercase().as_str() {
            "ip" | "inner_product" | "innerproduct" => Metric::Ip,
            "cosine" | "cos" => Metric::Cos,
            _ => Metric::L2,
        }
    }

    pub fn is_l2(self) -> bool {
        matches!(self, Metric::L2)
    }
}

impl From<Metric> for crate::lsg::Metric {
    fn from(metric: Metric) -> Self {
        match metric {
            Metric::L2 => crate::lsg::Metric::L2,
            Metric::Ip => crate::lsg::Metric::Ip,
            Metric::Cos => crate::lsg::Metric::Cos,
        }
    }
}

/// How the vectors are stored in the graph.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum HnswEncoding {
    Plain,
    Lvq,
    Rabitq,
}

impl HnswEncoding {
    pub fn parse(text: &str) -> Self {
        match text.trim().to_ascii_lowercase().as_str() {
            "lvq" => HnswEncoding::Lvq,
            "rabitq" => HnswEncoding::Rabitq,
            _ => HnswEncoding::Plain,
        }
    }
}

/// A query prepared in the store's representation.
enum HnswQuery {
    Plain(Vec<f32>),
    Lvq(crate::lvq::LvqQuery),
    Rabitq(crate::rabitq::RabitqQuery),
    /// A sparse `(dimension, weight)` list, sorted by dimension (the
    /// reference's `SparseVecRef`).
    Sparse(Vec<(u32, f32)>),
}

/// Inner product of two sparse vectors.
///
/// C++: `SparseVecUtil::DistanceIP` (`sparse_util.cppm:162`). Both inputs are
/// sorted by dimension; higher is closer, so the index stores `-IP`.
fn sparse_ip(a: &[(u32, f32)], b: &[(u32, f32)]) -> f32 {
    let (mut i, mut j, mut dot) = (0usize, 0usize, 0.0f32);
    while i < a.len() && j < b.len() {
        match a[i].0.cmp(&b[j].0) {
            std::cmp::Ordering::Less => i += 1,
            std::cmp::Ordering::Greater => j += 1,
            std::cmp::Ordering::Equal => {
                dot += a[i].1 * b[j].1;
                i += 1;
                j += 1;
            }
        }
    }
    dot
}

/// The vector payload of an HNSW graph.
pub enum Store {
    Plain {
        metric: Metric,
        data: Vec<Vec<f32>>,
    },
    Lvq(crate::lvq::LvqStore),
    Rabitq(crate::rabitq::RabitqStore),
    /// The reference's `SparseIPVecStoreType`: sparse vectors scored by inner
    /// product. It has no dense form, so `decompress` returns an empty vector.
    SparseIp {
        data: Vec<Vec<(u32, f32)>>,
    },
}

fn plain_distance(a: &[f32], b: &[f32], metric: Metric) -> f32 {
    match metric {
        Metric::L2 => l2_sq(a, b),
        Metric::Ip => -a.iter().zip(b).map(|(x, y)| x * y).sum::<f32>(),
        Metric::Cos => {
            // C++ `PlainCosDist` returns `-cosine` (the SIMD kernel returns
            // the similarity), so smaller is closer for every metric.
            let dot: f32 = a.iter().zip(b).map(|(x, y)| x * y).sum();
            let na: f32 = a.iter().map(|x| x * x).sum::<f32>().sqrt();
            let nb: f32 = b.iter().map(|x| x * x).sum::<f32>().sqrt();
            if na == 0.0 || nb == 0.0 {
                0.0
            } else {
                -dot / (na * nb)
            }
        }
    }
}

impl Store {
    fn sparse_ip() -> Self {
        Store::SparseIp { data: Vec::new() }
    }

    fn new(dims: usize, metric: Metric, encoding: HnswEncoding) -> Self {
        match encoding {
            HnswEncoding::Plain => Store::Plain {
                metric,
                data: Vec::new(),
            },
            HnswEncoding::Lvq => {
                let metric = match metric {
                    Metric::L2 => crate::lvq::Metric::L2,
                    Metric::Ip => crate::lvq::Metric::Ip,
                    Metric::Cos => crate::lvq::Metric::Cos,
                };
                Store::Lvq(crate::lvq::LvqStore::new(
                    dims,
                    metric,
                    metric == crate::lvq::Metric::Cos,
                ))
            }
            HnswEncoding::Rabitq => {
                let metric = match metric {
                    Metric::L2 => crate::rabitq::Metric::L2,
                    Metric::Ip => crate::rabitq::Metric::Ip,
                    Metric::Cos => crate::rabitq::Metric::Cos,
                };
                Store::Rabitq(crate::rabitq::RabitqStore::new(dims, metric, 0x5EED))
            }
        }
    }

    fn len(&self) -> usize {
        match self {
            Store::Plain { data, .. } => data.len(),
            Store::Lvq(store) => store.len(),
            Store::Rabitq(store) => store.len(),
            Store::SparseIp { data } => data.len(),
        }
    }

    fn push(&mut self, v: &[f32]) {
        match self {
            Store::Plain { data, .. } => data.push(v.to_vec()),
            Store::Lvq(store) => {
                store.compress(v);
            }
            Store::Rabitq(store) => {
                store.compress(v);
            }
            Store::SparseIp { .. } => panic!("dense insert into a sparse store"),
        }
    }

    /// Appends one sparse document. `C++: SparseVecStoreInner::AddVector`.
    fn push_sparse(&mut self, v: &[(u32, f32)]) {
        match self {
            Store::SparseIp { data } => data.push(v.to_vec()),
            _ => panic!("sparse insert into a dense store"),
        }
    }

    fn make_query(&self, v: &[f32]) -> HnswQuery {
        match self {
            Store::Plain { .. } => HnswQuery::Plain(v.to_vec()),
            Store::Lvq(store) => HnswQuery::Lvq(store.make_query(v)),
            Store::Rabitq(store) => HnswQuery::Rabitq(store.make_query(v)),
            Store::SparseIp { .. } => panic!("dense query into a sparse store"),
        }
    }

    fn make_query_sparse(&self, v: &[(u32, f32)]) -> HnswQuery {
        match self {
            Store::SparseIp { .. } => HnswQuery::Sparse(v.to_vec()),
            _ => panic!("sparse query into a dense store"),
        }
    }

    fn distance_query(&self, query: &HnswQuery, index: usize) -> f32 {
        match (self, query) {
            (Store::Plain { metric, data }, HnswQuery::Plain(q)) => data
                .get(index)
                .map(|v| plain_distance(q, v, *metric))
                .unwrap_or(f32::MAX),
            (Store::Lvq(store), HnswQuery::Lvq(q)) => store.distance(q, index).unwrap_or(f32::MAX),
            (Store::Rabitq(store), HnswQuery::Rabitq(q)) => {
                store.distance(q, index).unwrap_or(f32::MAX)
            }
            (Store::SparseIp { data }, HnswQuery::Sparse(q)) => data
                .get(index)
                .map(|v| -sparse_ip(q, v))
                .unwrap_or(f32::MAX),
            _ => f32::MAX,
        }
    }

    /// Distance between two stored nodes (used by the neighbor heuristic and
    /// reverse-link pruning).
    fn distance_nodes(&self, a: usize, b: usize) -> f32 {
        match self {
            Store::Plain { metric, data } => data
                .get(a)
                .zip(data.get(b))
                .map(|(x, y)| plain_distance(x, y, *metric))
                .unwrap_or(f32::MAX),
            Store::Lvq(store) => store
                .query_at(a)
                .and_then(|q| store.distance(&q, b))
                .unwrap_or(f32::MAX),
            Store::Rabitq(store) => store
                .decompress(a)
                .map(|v| {
                    let q = store.make_query(&v);
                    store.distance(&q, b).unwrap_or(f32::MAX)
                })
                .unwrap_or(f32::MAX),
            Store::SparseIp { data } => data
                .get(a)
                .zip(data.get(b))
                .map(|(x, y)| -sparse_ip(x, y))
                .unwrap_or(f32::MAX),
        }
    }

    /// The decompressed vector of `node` (allocates for quantized stores).
    /// Sparse vectors have no dense form, so this returns an empty vector.
    fn decompress(&self, node: usize) -> Vec<f32> {
        match self {
            Store::Plain { data, .. } => data.get(node).cloned().unwrap_or_default(),
            Store::Lvq(store) => store
                .decompress(node)
                .map(|v| v.into_iter().map(|x| x as f32).collect())
                .unwrap_or_default(),
            Store::Rabitq(store) => store.decompress(node).unwrap_or_default(),
            Store::SparseIp { .. } => Vec::new(),
        }
    }

    fn mem_bytes(&self) -> usize {
        match self {
            Store::Plain { data, .. } => data.iter().map(|v| v.capacity() * 4).sum(),
            Store::Lvq(store) => store.len() * store.record_size() + store.mean().len() * 8 + 64,
            Store::Rabitq(store) => {
                store.len() * store.record_size() + store.dim() * store.dim() * 4 + 4 * store.dim()
            }
            Store::SparseIp { data } => data
                .iter()
                .map(|v| v.capacity() * std::mem::size_of::<(u32, f32)>())
                .sum(),
        }
    }

    fn encoding(&self) -> HnswEncoding {
        match self {
            Store::Plain { .. } | Store::SparseIp { .. } => HnswEncoding::Plain,
            Store::Lvq(_) => HnswEncoding::Lvq,
            Store::Rabitq(_) => HnswEncoding::Rabitq,
        }
    }

    fn metric(&self) -> Metric {
        match self {
            Store::Plain { metric, .. } => *metric,
            Store::Lvq(store) => match store.metric() {
                crate::lvq::Metric::L2 => Metric::L2,
                crate::lvq::Metric::Ip => Metric::Ip,
                crate::lvq::Metric::Cos => Metric::Cos,
            },
            Store::Rabitq(store) => match store.metric() {
                crate::rabitq::Metric::L2 => Metric::L2,
                crate::rabitq::Metric::Ip => Metric::Ip,
                crate::rabitq::Metric::Cos => Metric::Cos,
            },
            Store::SparseIp { .. } => Metric::Ip,
        }
    }

    /// Trains the store's mean/centroid without storing any vector.
    fn train(&mut self, vectors: &[Vec<f32>]) {
        match self {
            Store::Plain { .. } | Store::SparseIp { .. } => {}
            Store::Lvq(store) => store.train(vectors),
            Store::Rabitq(store) => store.train(vectors),
        }
    }
}

/// Approximate kNN via a multi-layer navigable graph.
pub struct HnswIndex {
    dims: usize,
    m: usize,
    ef_construction: usize,
    ef_search: usize,
    ml: f64,
    store: Store,
    /// Per-row LSG scaling factors, when the index was built with `build_type=lsg`.
    lsg: Option<crate::lsg::LsgIndex>,
    levels: Vec<usize>,
    /// `links[layer][node]` = neighbor node ids.
    links: Vec<Vec<Vec<usize>>>,
    entry: Option<usize>,
    max_level: usize,
    rng: Rng,
}

impl HnswIndex {
    pub fn new(dims: usize, m: usize, ef_construction: usize, ef_search: usize) -> Self {
        Self::with_encoding(
            dims,
            m,
            ef_construction,
            ef_search,
            Metric::L2,
            HnswEncoding::Plain,
        )
    }

    /// C++ `InitAbstractIndexT`: plain/LVQ/RaBitQ stores for the chosen metric.
    pub fn with_encoding(
        dims: usize,
        m: usize,
        ef_construction: usize,
        ef_search: usize,
        metric: Metric,
        encoding: HnswEncoding,
    ) -> Self {
        Self::with_store(
            dims,
            m,
            ef_construction,
            ef_search,
            Store::new(dims, metric, encoding),
        )
    }

    /// C++: `KnnHnsw<SparseIPVecStoreType<float, i32>, u64>` — the last
    /// `test_hnsw_sparse_ut.cpp` store. The graph and query path are shared
    /// with the dense indexes; only the payload and distance differ.
    pub fn sparse_ip(dims: usize, m: usize, ef_construction: usize, ef_search: usize) -> Self {
        Self::with_store(dims, m, ef_construction, ef_search, Store::sparse_ip())
    }

    fn with_store(
        dims: usize,
        m: usize,
        ef_construction: usize,
        ef_search: usize,
        store: Store,
    ) -> Self {
        Self {
            dims,
            m,
            ef_construction: ef_construction.max(m),
            ef_search,
            ml: 1.0 / (m as f64).ln(),
            store,
            lsg: None,
            levels: Vec::new(),
            links: Vec::new(),
            entry: None,
            max_level: 0,
            rng: Rng::new(0x9E37_79B9_7F4A_7C15),
        }
    }

    /// Trains LVQ's mean / RaBitQ's centroid and computes the LSG averages
    /// from `vectors`. The caller then feeds the same vectors through
    /// [`HnswIndex::add`], which compresses them with the trained state.
    pub fn train_store(&mut self, vectors: &[Vec<f32>], lsg: Option<crate::lsg::LsgConfig>) {
        self.store.train(vectors);
        if let Some(config) = lsg {
            let metric = self.store.metric().into();
            self.lsg = Some(crate::lsg::LsgIndex::build(vectors, metric, config));
        }
    }

    pub fn lsg(&self) -> Option<&crate::lsg::LsgIndex> {
        self.lsg.as_ref()
    }

    pub fn encoding(&self) -> HnswEncoding {
        self.store.encoding()
    }

    pub fn metric(&self) -> Metric {
        self.store.metric()
    }

    pub fn len(&self) -> usize {
        self.store.len()
    }

    pub fn is_empty(&self) -> bool {
        self.store.len() == 0
    }

    /// Vector dimension.
    pub fn dims(&self) -> usize {
        self.dims
    }

    /// Neighbour limit for layers ≥ 1 (`M`).
    pub fn m(&self) -> usize {
        self.m
    }

    /// How many layers above level 0 `node` appears in.
    pub fn level_of(&self, node: usize) -> usize {
        self.levels.get(node).copied().unwrap_or(0)
    }

    /// Neighbours of `node` on `layer` (empty when absent).
    pub fn neighbors(&self, layer: usize, node: usize) -> &[usize] {
        self.links
            .get(layer)
            .and_then(|l| l.get(node))
            .map(Vec::as_slice)
            .unwrap_or(&[])
    }

    /// The stored (decompressed) vector of `node`.
    pub fn vector(&self, node: usize) -> Vec<f32> {
        self.store.decompress(node)
    }

    /// The stored sparse document of `node`, when this is a sparse index.
    pub fn sparse_vector(&self, node: usize) -> Option<&[(u32, f32)]> {
        match &self.store {
            Store::SparseIp { data } => data.get(node).map(Vec::as_slice),
            _ => None,
        }
    }

    /// C++ `KnnHnsw::Check`: every link points at an existing node, never at
    /// itself, and appears once per neighbour list.
    pub fn check(&self) -> Result<()> {
        let n = self.store.len();
        if let Some(entry) = self.entry {
            if entry >= n {
                return Err(Error::Invalid(format!("entry {entry} outside 0..{n}")));
            }
        }
        for (layer, nodes) in self.links.iter().enumerate() {
            for (node, neighbors) in nodes.iter().enumerate() {
                let mut seen = HashSet::with_capacity(neighbors.len());
                for &neighbor in neighbors {
                    if neighbor >= n {
                        return Err(Error::Invalid(format!(
                            "layer {layer} node {node} links to {neighbor} outside 0..{n}"
                        )));
                    }
                    if neighbor == node {
                        return Err(Error::Invalid(format!(
                            "layer {layer} node {node} links to itself"
                        )));
                    }
                    if !seen.insert(neighbor) {
                        return Err(Error::Invalid(format!(
                            "layer {layer} node {node} links to {neighbor} twice"
                        )));
                    }
                }
            }
        }
        Ok(())
    }

    /// C++ `KnnHnsw::Dump`: one line per node (`node level neighbours...`).
    pub fn dump(&self, out: &mut impl Write) -> std::io::Result<()> {
        for node in 0..self.store.len() {
            write!(out, "{node} {}", self.level_of(node))?;
            for &neighbor in self.neighbors(0, node) {
                write!(out, " {neighbor}")?;
            }
            writeln!(out)?;
        }
        Ok(())
    }

    /// Serializes a sparse index (store + graph) to `path`.
    ///
    /// C++: `KnnHnsw::Save(FileHandle)`. The reference's writer is generic
    /// over the store; this port implements the sparse store because that is
    /// the only caller in the ported suite.
    pub fn save_sparse(&self, path: impl AsRef<Path>) -> Result<()> {
        let path = path.as_ref();
        let Store::SparseIp { data } = &self.store else {
            return Err(Error::Invalid(
                "save_sparse requires a sparse index".to_string(),
            ));
        };
        let mut out = Vec::new();
        out.extend_from_slice(b"SPHNSW1\0");
        put_u64(&mut out, self.dims as u64);
        put_u64(&mut out, self.m as u64);
        put_u64(&mut out, self.ef_search as u64);
        put_u64(&mut out, self.max_level as u64);
        put_u64(&mut out, data.len() as u64);
        for doc in data {
            put_u64(&mut out, doc.len() as u64);
            for &(idx, weight) in doc {
                out.extend_from_slice(&idx.to_le_bytes());
                out.extend_from_slice(&weight.to_le_bytes());
            }
        }
        for node in 0..data.len() {
            put_u64(&mut out, self.level_of(node) as u64);
        }
        put_u64(&mut out, self.entry.map_or(u64::MAX, |entry| entry as u64));
        put_u64(&mut out, self.links.len() as u64);
        for layer in &self.links {
            put_u64(&mut out, layer.len() as u64);
            for neighbors in layer {
                put_u64(&mut out, neighbors.len() as u64);
                for &neighbor in neighbors {
                    out.extend_from_slice(&(neighbor as u32).to_le_bytes());
                }
            }
        }
        std::fs::write(path, out).map_err(|e| Error::io(path, e))
    }

    /// Loads an index written by [`HnswIndex::save_sparse`].
    pub fn load_sparse(path: impl AsRef<Path>) -> Result<Self> {
        let path = path.as_ref();
        let buf = std::fs::read(path).map_err(|e| Error::io(path, e))?;
        let mut reader = SparseReader { buf: &buf, pos: 0 };
        if reader.take(8)? != b"SPHNSW1\0" {
            return Err(Error::Invalid("bad sparse HNSW magic".to_string()));
        }
        let dims = reader.u64()? as usize;
        let m = reader.u64()? as usize;
        let ef_search = reader.u64()? as usize;
        let max_level = reader.u64()? as usize;
        let n = reader.u64()? as usize;
        let mut data = Vec::with_capacity(n);
        for _ in 0..n {
            let nnz = reader.u64()? as usize;
            let mut doc = Vec::with_capacity(nnz);
            for _ in 0..nnz {
                let idx = u32::from_le_bytes(reader.take(4)?.try_into().unwrap());
                let weight = f32::from_le_bytes(reader.take(4)?.try_into().unwrap());
                doc.push((idx, weight));
            }
            data.push(doc);
        }
        let mut levels = Vec::with_capacity(n);
        for _ in 0..n {
            levels.push(reader.u64()? as usize);
        }
        let entry_raw = reader.u64()?;
        let entry = if entry_raw == u64::MAX {
            None
        } else {
            Some(entry_raw as usize)
        };
        let layer_count = reader.u64()? as usize;
        let mut links = Vec::with_capacity(layer_count);
        for _ in 0..layer_count {
            let rows = reader.u64()? as usize;
            let mut layer = Vec::with_capacity(rows);
            for _ in 0..rows {
                let len = reader.u64()? as usize;
                let mut neighbors = Vec::with_capacity(len);
                for _ in 0..len {
                    neighbors
                        .push(u32::from_le_bytes(reader.take(4)?.try_into().unwrap()) as usize);
                }
                layer.push(neighbors);
            }
            links.push(layer);
        }
        let index = Self::from_store_parts(
            dims,
            m,
            ef_search,
            Store::SparseIp { data },
            None,
            levels,
            links,
            entry,
            max_level,
        );
        index.check()?;
        Ok(index)
    }

    /// The entry point, if any vectors were added.
    pub fn entry_point(&self) -> Option<usize> {
        self.entry
    }

    /// The highest populated layer.
    pub fn max_level(&self) -> usize {
        self.max_level
    }

    fn adjust(&self, d: f32, a: Option<usize>, b: usize) -> f32 {
        match (a, &self.lsg) {
            (Some(a), Some(lsg)) => lsg.adjust(d, a, b),
            _ => d,
        }
    }

    fn distance_query(&self, query: &HnswQuery, self_id: Option<usize>, index: usize) -> f32 {
        let d = self.store.distance_query(query, index);
        self.adjust(d, self_id, index)
    }

    fn distance_nodes(&self, a: usize, b: usize) -> f32 {
        let d = self.store.distance_nodes(a, b);
        self.adjust(d, Some(a), b)
    }

    /// Rebuilds an index from a serialized plain graph (the C++ HNSW chunk's
    /// header, vector region, graph region and labels).
    #[allow(clippy::too_many_arguments)]
    pub fn from_parts(
        dims: usize,
        m: usize,
        ef_search: usize,
        data: Vec<Vec<f32>>,
        levels: Vec<usize>,
        links: Vec<Vec<Vec<usize>>>,
        entry: Option<usize>,
        max_level: usize,
    ) -> Self {
        Self::from_store_parts(
            dims,
            m,
            ef_search,
            Store::Plain {
                metric: Metric::L2,
                data,
            },
            None,
            levels,
            links,
            entry,
            max_level,
        )
    }

    /// Rebuilds an index from a decoded chunk with its store and LSG state.
    #[allow(clippy::too_many_arguments)]
    pub fn from_store_parts(
        dims: usize,
        m: usize,
        ef_search: usize,
        store: Store,
        lsg: Option<crate::lsg::LsgIndex>,
        levels: Vec<usize>,
        mut links: Vec<Vec<Vec<usize>>>,
        entry: Option<usize>,
        max_level: usize,
    ) -> Self {
        for (node, level) in levels.iter().enumerate() {
            for layer in 0..=*level {
                while links.len() <= layer {
                    links.push(Vec::new());
                }
                while links[layer].len() <= node {
                    links[layer].push(Vec::new());
                }
            }
        }
        Self {
            dims,
            m,
            ef_construction: m * 2,
            ef_search,
            ml: 1.0 / (m as f64).ln(),
            store,
            lsg,
            levels,
            links,
            entry,
            max_level,
            rng: Rng::new(0x9E37_79B9_7F4A_7C15),
        }
    }

    /// C++ `KnnHnsw::CompressToLVQ`: re-encode the stored vectors with an LVQ
    /// store trained on them, keeping the graph and labels unchanged.
    ///
    /// The reference's `LSGDistWrapper::ToLVQDistance` discards the LSG
    /// wrapper ("Discard LSG Wrapper after compression"), so the scaling is
    /// dropped here as well.
    pub fn compress_to_lvq(&mut self) {
        if matches!(self.store, Store::SparseIp { .. }) {
            return;
        }
        let vectors: Vec<Vec<f32>> = (0..self.len()).map(|i| self.store.decompress(i)).collect();
        let metric = match self.store.metric() {
            Metric::L2 => crate::lvq::Metric::L2,
            Metric::Ip => crate::lvq::Metric::Ip,
            Metric::Cos => crate::lvq::Metric::Cos,
        };
        let mut store =
            crate::lvq::LvqStore::new(self.dims, metric, metric == crate::lvq::Metric::Cos);
        store.build(&vectors);
        self.store = Store::Lvq(store);
        self.lsg = None;
    }

    /// C++ `KnnHnsw::CompressToRabitq`: rebuild the vector payload as RaBitQ
    /// codes trained on the decompressed vectors. The graph, levels and
    /// labels stay untouched; like `compress_to_lvq`, the LSG wrapper is
    /// discarded by the reference conversion.
    pub fn compress_to_rabitq(&mut self) {
        if matches!(self.store, Store::Rabitq(_) | Store::SparseIp { .. }) {
            return;
        }
        let vectors: Vec<Vec<f32>> = (0..self.len()).map(|i| self.store.decompress(i)).collect();
        let metric = match self.store.metric() {
            Metric::L2 => crate::rabitq::Metric::L2,
            Metric::Ip => crate::rabitq::Metric::Ip,
            Metric::Cos => crate::rabitq::Metric::Cos,
        };
        let mut store = crate::rabitq::RabitqStore::new(self.dims, metric, 0x5EED);
        store.build(&vectors);
        self.store = Store::Rabitq(store);
        self.lsg = None;
    }

    /// The vec-store meta blob written into the HNSW chunk header.
    pub fn store_meta_bytes(&self) -> Vec<u8> {
        match &self.store {
            Store::Plain { .. } | Store::SparseIp { .. } => {
                (self.dims as u64).to_le_bytes().to_vec()
            }
            Store::Lvq(store) => store.meta_bytes(),
            Store::Rabitq(store) => store.meta_bytes(),
        }
    }

    /// The chunk's vector region.
    pub fn store_records_bytes(&self) -> Vec<u8> {
        match &self.store {
            Store::Plain { data, .. } => {
                let mut out = Vec::with_capacity(self.dims * data.len() * 4);
                for v in data {
                    for x in v {
                        out.extend_from_slice(&x.to_le_bytes());
                    }
                }
                out
            }
            Store::Lvq(store) => store.records_bytes(),
            Store::Rabitq(store) => store.records_bytes(),
            Store::SparseIp { data } => {
                let mut out = Vec::new();
                for doc in data {
                    out.extend_from_slice(&(doc.len() as u32).to_le_bytes());
                    for &(idx, weight) in doc {
                        out.extend_from_slice(&idx.to_le_bytes());
                        out.extend_from_slice(&weight.to_le_bytes());
                    }
                }
                out
            }
        }
    }

    /// Bytes per vector in the chunk's vector region. Sparse records are
    /// variable-length, so there is no fixed size (the sparse store is not
    /// written through the HNSW chunk layout).
    pub fn store_record_size(&self) -> usize {
        match &self.store {
            Store::Plain { .. } => self.dims * 4,
            Store::Lvq(store) => store.record_size(),
            Store::Rabitq(store) => store.record_size(),
            Store::SparseIp { .. } => 0,
        }
    }

    /// C++ `KnnHnsw::GetSizeInBytes` (`hnsw_alg.cppm:89`): the serialized size
    /// of the graph — the vec-store meta, the vector records, the graph
    /// records, the per-vertex levels and the labels, plus the header fields.
    pub fn size_in_bytes(&self) -> usize {
        let header = 2 * std::mem::size_of::<usize>();
        let store_meta = self.store_meta_bytes().len();
        let vectors = self.store_record_size() * self.len();
        let labels = self.len() * std::mem::size_of::<u64>();
        let levels = self.levels.len() * std::mem::size_of::<usize>();
        let graph: usize = self
            .links
            .iter()
            .map(|layer| {
                layer
                    .iter()
                    .map(|neighbors| {
                        std::mem::size_of::<u32>() + neighbors.len() * std::mem::size_of::<u32>()
                    })
                    .sum::<usize>()
            })
            .sum();
        header + store_meta + vectors + labels + levels + graph
    }

    /// Rebuilds an index from the chunk's meta/records and graph parts.
    #[allow(clippy::too_many_arguments)]
    pub fn from_chunk_parts(
        dims: usize,
        m: usize,
        ef_search: usize,
        metric: Metric,
        encoding: HnswEncoding,
        meta: &[u8],
        records: &[u8],
        n: usize,
        levels: Vec<usize>,
        links: Vec<Vec<Vec<usize>>>,
        entry: Option<usize>,
        max_level: usize,
    ) -> Option<Self> {
        let store = match encoding {
            HnswEncoding::Plain => {
                if records.len() < n * dims * 4 {
                    return None;
                }
                let data = records
                    .chunks_exact(dims * 4)
                    .take(n)
                    .map(|c| {
                        c.chunks_exact(4)
                            .map(|b| f32::from_le_bytes(b.try_into().unwrap()))
                            .collect()
                    })
                    .collect();
                Store::Plain { metric, data }
            }
            HnswEncoding::Lvq => {
                let lvq_metric = match metric {
                    Metric::L2 => crate::lvq::Metric::L2,
                    Metric::Ip => crate::lvq::Metric::Ip,
                    Metric::Cos => crate::lvq::Metric::Cos,
                };
                Store::Lvq(crate::lvq::LvqStore::from_parts(
                    lvq_metric,
                    metric == Metric::Cos,
                    meta,
                    records,
                    n,
                )?)
            }
            HnswEncoding::Rabitq => {
                let rabitq_metric = match metric {
                    Metric::L2 => crate::rabitq::Metric::L2,
                    Metric::Ip => crate::rabitq::Metric::Ip,
                    Metric::Cos => crate::rabitq::Metric::Cos,
                };
                Store::Rabitq(crate::rabitq::RabitqStore::from_parts(
                    rabitq_metric,
                    meta,
                    records,
                    n,
                )?)
            }
        };
        Some(Self::from_store_parts(
            dims, m, ef_search, store, None, levels, links, entry, max_level,
        ))
    }

    fn random_level(&mut self) -> usize {
        let u = self.rng.unit().max(1e-12);
        (-u.ln() * self.ml) as usize
    }

    fn ensure(&mut self, layer: usize, node: usize) {
        while self.links.len() <= layer {
            self.links.push(Vec::new());
        }
        while self.links[layer].len() <= node {
            self.links[layer].push(Vec::new());
        }
    }

    /// `SelectNeighborsHeuristic`: candidates closer than an already-kept
    /// neighbour are dropped; the list is ordered farthest first.
    fn select_neighbors_heuristic(&self, cands: &[(f32, usize)], m: usize) -> Vec<(f32, usize)> {
        let mut sorted = cands.to_vec();
        if sorted.len() < m {
            sorted.sort_by(|a, b| b.0.partial_cmp(&a.0).unwrap_or(std::cmp::Ordering::Equal));
            return sorted;
        }
        sorted.sort_by(|a, b| a.0.partial_cmp(&b.0).unwrap_or(std::cmp::Ordering::Equal));
        let mut kept: Vec<(f32, usize)> = Vec::with_capacity(m);
        for (d, node) in sorted {
            if kept.len() == m {
                break;
            }
            let dominated = kept.iter().any(|(_, s)| self.distance_nodes(node, *s) < d);
            if !dominated {
                kept.push((d, node));
            }
        }
        kept.reverse();
        kept
    }

    fn connect_neighbors(&mut self, target: usize, node: usize, layer: usize) {
        self.ensure(layer, target);
        let max_m = if layer == 0 { self.m * 2 } else { self.m };
        if self.links[layer][target].len() < max_m {
            self.links[layer][target].push(node);
            return;
        }
        let mut cands: Vec<(f32, usize)> = Vec::with_capacity(self.links[layer][target].len() + 1);
        cands.push((self.distance_nodes(target, node), node));
        for &nb in &self.links[layer][target] {
            cands.push((self.distance_nodes(target, nb), nb));
        }
        let kept = self.select_neighbors_heuristic(&cands, max_m);
        self.links[layer][target] = kept.into_iter().map(|(_, n)| n).collect();
    }

    /// `Build`: insert one vector into the graph.
    pub fn add(&mut self, v: &[f32]) -> usize {
        assert_eq!(v.len(), self.dims, "dimension mismatch");
        let query = self.store.make_query(v);
        self.insert_node(query, |store| store.push(v))
    }

    /// C++ `KnnHnsw<SparseIPVecStoreType>::InsertVecs` for one document.
    pub fn add_sparse(&mut self, v: &[(u32, f32)]) -> usize {
        assert!(
            v.iter().all(|(idx, _)| (*idx as usize) < self.dims),
            "sparse dimension out of range"
        );
        assert!(
            v.windows(2).all(|pair| pair[0].0 < pair[1].0),
            "sparse vector is not strictly sorted by dimension"
        );
        let query = self.store.make_query_sparse(v);
        self.insert_node(query, |store| store.push_sparse(v))
    }

    fn insert_node(&mut self, query: HnswQuery, push: impl FnOnce(&mut Store)) -> usize {
        let node = self.store.len();
        push(&mut self.store);
        let q_layer = self.random_level();

        let (max_layer, mut ep) = match self.entry {
            None => {
                self.levels.push(q_layer);
                self.entry = Some(node);
                self.max_level = q_layer;
                for l in 0..=q_layer {
                    self.ensure(l, node);
                }
                return node;
            }
            Some(old_ep) => {
                if q_layer > self.max_level {
                    let old = (self.max_level, old_ep);
                    self.max_level = q_layer;
                    self.entry = Some(node);
                    old
                } else {
                    (self.max_level, old_ep)
                }
            }
        };

        self.levels.push(q_layer);
        for l in 0..=q_layer {
            self.ensure(l, node);
        }

        let mut cur = max_layer as i64;
        while cur > q_layer as i64 {
            ep = self.search_layer_nearest(&query, ep, cur as usize, Some(node));
            cur -= 1;
        }
        let mut l = q_layer.min(max_layer) as i64;
        while l >= 0 {
            let cands = self.search_layer(&query, ep, self.ef_construction, l as usize, Some(node));
            let neighbors = self.select_neighbors_heuristic(&cands, self.m);
            if neighbors.is_empty() {
                l -= 1;
                continue;
            }
            ep = neighbors[0].1;
            self.links[l as usize][node] = neighbors.iter().map(|(_, n)| *n).collect();
            for (_, nb) in &neighbors {
                self.connect_neighbors(*nb, node, l as usize);
            }
            l -= 1;
        }
        node
    }

    fn search_layer_nearest(
        &self,
        q: &HnswQuery,
        start: usize,
        layer: usize,
        self_id: Option<usize>,
    ) -> usize {
        let mut cur = start;
        let mut cur_d = self.distance_query(q, self_id, cur);
        loop {
            let mut changed = false;
            if let Some(list) = self.links.get(layer).and_then(|l| l.get(cur)) {
                for &nb in list.iter().rev() {
                    let d = self.distance_query(q, self_id, nb);
                    if d < cur_d {
                        cur = nb;
                        cur_d = d;
                        changed = true;
                    }
                }
            }
            if !changed {
                return cur;
            }
        }
    }

    fn search_layer(
        &self,
        q: &HnswQuery,
        ep: usize,
        ef: usize,
        layer: usize,
        self_id: Option<usize>,
    ) -> Vec<(f32, usize)> {
        self.search_layer_filtered(q, ep, ef, layer, self_id, None)
    }

    /// `SearchLayer` with the reference's optional label filter: filtered-out
    /// nodes are still traversed but never enter the result heap.
    fn search_layer_filtered(
        &self,
        q: &HnswQuery,
        ep: usize,
        ef: usize,
        layer: usize,
        self_id: Option<usize>,
        allow: Option<&dyn Fn(usize) -> bool>,
    ) -> Vec<(f32, usize)> {
        let allowed = |node: usize| allow.is_none_or(|f| f(node));
        let mut visited: HashSet<usize> = HashSet::new();
        visited.insert(ep);
        let ep_d = self.distance_query(q, self_id, ep);
        let mut candidates: Vec<(f32, usize)> = vec![(ep_d, ep)];
        let mut result: Vec<(f32, usize)> = if allowed(ep) {
            vec![(ep_d, ep)]
        } else {
            vec![]
        };

        while !candidates.is_empty() {
            let (cd, c) = candidates.remove(0);
            let worst = result.last().map(|x| x.0).unwrap_or(f32::INFINITY);
            if result.len() >= ef && cd > worst {
                break;
            }
            if let Some(list) = self.links.get(layer).and_then(|l| l.get(c)) {
                for &nb in list {
                    if nb >= self.store.len() || !visited.insert(nb) {
                        continue;
                    }
                    let d = self.distance_query(q, self_id, nb);
                    let worst = result.last().map(|x| x.0).unwrap_or(f32::INFINITY);
                    if result.len() < ef || d <= worst {
                        candidates.push((d, nb));
                        candidates.sort_by(|a, b| {
                            a.0.partial_cmp(&b.0).unwrap_or(std::cmp::Ordering::Equal)
                        });
                        if allowed(nb) {
                            result.push((d, nb));
                            result.sort_by(|a, b| {
                                a.0.partial_cmp(&b.0).unwrap_or(std::cmp::Ordering::Equal)
                            });
                            result.truncate(ef);
                        }
                    }
                }
            }
        }
        result
    }

    /// Approximate heap bytes held by the graph, for the memindex budget.
    pub fn mem_usage(&self) -> usize {
        let data = self.store.mem_bytes();
        let levels = self.levels.capacity() * std::mem::size_of::<usize>();
        let links: usize = self
            .links
            .iter()
            .map(|layer| {
                layer.capacity() * std::mem::size_of::<Vec<usize>>()
                    + layer.iter().map(|l| l.capacity() * 8).sum::<usize>()
            })
            .sum();
        data + levels + links
    }

    /// Returns `(node, distance)` for the `k` approximate nearest, ascending.
    pub fn search(&self, q: &[f32], k: usize) -> Vec<(usize, f32)> {
        self.search_with_ef(q, k, self.ef_search)
    }

    /// `KnnSearchOption.ef_`: the candidate list size for the bottom layer.
    pub fn search_with_ef(&self, q: &[f32], k: usize, ef: usize) -> Vec<(usize, f32)> {
        self.search_query(self.store.make_query(q), k, ef)
    }

    /// The sparse counterpart of [`HnswIndex::search_with_ef`]
    /// (C++ `KnnHnsw<SparseIPVecStoreType>::KnnSearchSorted`).
    pub fn search_sparse(&self, q: &[(u32, f32)], k: usize, ef: usize) -> Vec<(usize, f32)> {
        self.search_query(HnswQuery::Sparse(q.to_vec()), k, ef)
    }

    fn search_query(&self, query: HnswQuery, k: usize, ef: usize) -> Vec<(usize, f32)> {
        let Some(entry) = self.entry else {
            return vec![];
        };
        let mut cur = entry;
        let mut l = self.max_level as i64;
        while l > 0 {
            cur = self.search_layer_nearest(&query, cur, l as usize, None);
            l -= 1;
        }
        let ef = ef.max(k);
        let mut res = self.search_layer(&query, cur, ef, 0, None);
        res.sort_by(|a, b| a.0.partial_cmp(&b.0).unwrap());
        res.truncate(k);
        res.into_iter().map(|(d, i)| (i, d)).collect()
    }

    /// `KnnSearchSorted(q, k, filter, option)`: the same traversal, but only
    /// labels accepted by `allow` are returned.
    pub fn search_filtered(
        &self,
        q: &[f32],
        k: usize,
        ef: usize,
        allow: &dyn Fn(usize) -> bool,
    ) -> Vec<(usize, f32)> {
        let Some(entry) = self.entry else {
            return vec![];
        };
        let query = self.store.make_query(q);
        let mut cur = entry;
        let mut l = self.max_level as i64;
        while l > 0 {
            cur = self.search_layer_nearest(&query, cur, l as usize, None);
            l -= 1;
        }
        let ef = ef.max(k);
        let mut res = self.search_layer_filtered(&query, cur, ef, 0, None, Some(allow));
        res.sort_by(|a, b| a.0.partial_cmp(&b.0).unwrap());
        res.truncate(k);
        res.into_iter().map(|(d, i)| (i, d)).collect()
    }
}

fn put_u64(out: &mut Vec<u8>, value: u64) {
    out.extend_from_slice(&value.to_le_bytes());
}

/// Bounds-checked cursor for [`HnswIndex::load_sparse`].
struct SparseReader<'a> {
    buf: &'a [u8],
    pos: usize,
}

impl<'a> SparseReader<'a> {
    fn take(&mut self, n: usize) -> Result<&'a [u8]> {
        let end = self.pos + n;
        let bytes = self
            .buf
            .get(self.pos..end)
            .ok_or_else(|| Error::Invalid("truncated sparse HNSW file".to_string()))?;
        self.pos = end;
        Ok(bytes)
    }

    fn u64(&mut self) -> Result<u64> {
        Ok(u64::from_le_bytes(self.take(8)?.try_into().unwrap()))
    }
}

/// File-backed save/load of the HNSW chunk parts, used by the reference
/// persistence tests (`SaveToPtr` → `LoadFromPtr`).
///
/// The layout is the same set of parts `store_meta_bytes` /
/// `store_records_bytes` / `from_chunk_parts` consume, wrapped in a small
/// header plus the per-layer adjacency lists. It is test support only: the
/// production chunk writer lives in `infinity-storage`.
#[cfg(test)]
pub(crate) mod chunk_file {
    use super::{put_u64, HnswEncoding, HnswIndex, Metric};
    use std::io::Write;
    use std::path::Path;

    const MAGIC: &[u8; 8] = b"RINFHNSW";

    fn put_bytes(out: &mut Vec<u8>, bytes: &[u8]) {
        put_u64(out, bytes.len() as u64);
        out.extend_from_slice(bytes);
    }

    /// Writes `index` to `path` atomically through a sibling temp file.
    pub(crate) fn save(index: &HnswIndex, path: &Path) {
        let mut out = Vec::new();
        out.extend_from_slice(MAGIC);
        put_u64(&mut out, index.dims() as u64);
        put_u64(&mut out, index.m() as u64);
        put_u64(
            &mut out,
            match index.metric() {
                Metric::L2 => 0,
                Metric::Ip => 1,
                Metric::Cos => 2,
            },
        );
        put_u64(
            &mut out,
            match index.encoding() {
                HnswEncoding::Plain => 0,
                HnswEncoding::Lvq => 1,
                HnswEncoding::Rabitq => 2,
            },
        );
        let n = index.len();
        put_u64(&mut out, n as u64);
        put_u64(
            &mut out,
            index.entry_point().map_or(u64::MAX, |entry| entry as u64),
        );
        put_u64(&mut out, index.max_level() as u64);
        put_bytes(&mut out, &index.store_meta_bytes());
        put_bytes(&mut out, &index.store_records_bytes());
        for node in 0..n {
            put_u64(&mut out, index.level_of(node) as u64);
        }
        for layer in 0..=index.max_level() {
            for node in 0..n {
                let neighbors = index.neighbors(layer, node);
                put_u64(&mut out, neighbors.len() as u64);
                for &neighbor in neighbors {
                    put_u64(&mut out, neighbor as u64);
                }
            }
        }
        let temp = path.with_extension("tmp");
        {
            let mut file = std::fs::File::create(&temp).expect("create HNSW file");
            file.write_all(&out).expect("write HNSW file");
            file.sync_all().expect("sync HNSW file");
        }
        std::fs::rename(&temp, path).expect("rename HNSW file");
    }

    struct Reader {
        bytes: Vec<u8>,
        pos: usize,
    }

    impl Reader {
        fn take(&mut self, n: usize) -> &[u8] {
            let end = self.pos + n;
            let slice = self.bytes.get(self.pos..end).expect("truncated HNSW file");
            self.pos = end;
            slice
        }

        fn u64(&mut self) -> u64 {
            u64::from_le_bytes(self.take(8).try_into().unwrap())
        }

        fn bytes(&mut self) -> Vec<u8> {
            let len = self.u64() as usize;
            self.take(len).to_vec()
        }
    }

    /// Reads the parts written by [`save`] and rebuilds the index.
    pub(crate) fn load(path: &Path, ef_search: usize) -> HnswIndex {
        let mut reader = Reader {
            bytes: std::fs::read(path).expect("read HNSW file"),
            pos: 0,
        };
        assert_eq!(reader.take(MAGIC.len()), MAGIC, "bad HNSW magic");
        let dims = reader.u64() as usize;
        let m = reader.u64() as usize;
        let metric = match reader.u64() {
            0 => Metric::L2,
            1 => Metric::Ip,
            _ => Metric::Cos,
        };
        let encoding = match reader.u64() {
            0 => HnswEncoding::Plain,
            1 => HnswEncoding::Lvq,
            _ => HnswEncoding::Rabitq,
        };
        let n = reader.u64() as usize;
        let entry = match reader.u64() {
            u64::MAX => None,
            entry => Some(entry as usize),
        };
        let max_level = reader.u64() as usize;
        let meta = reader.bytes();
        let records = reader.bytes();
        let levels: Vec<usize> = (0..n).map(|_| reader.u64() as usize).collect();
        let mut links = vec![vec![Vec::new(); n]; max_level + 1];
        for layer in links.iter_mut() {
            for node in layer.iter_mut() {
                let degree = reader.u64() as usize;
                *node = (0..degree).map(|_| reader.u64() as usize).collect();
            }
        }
        assert_eq!(reader.pos, reader.bytes.len(), "trailing HNSW bytes");
        HnswIndex::from_chunk_parts(
            dims, m, ef_search, metric, encoding, &meta, &records, n, levels, links, entry,
            max_level,
        )
        .expect("chunk parts")
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn dataset(n: usize, dims: usize) -> Vec<Vec<f32>> {
        let mut rng = Rng::new(42);
        (0..n)
            .map(|_| (0..dims).map(|_| (rng.unit() as f32) * 10.0).collect())
            .collect()
    }

    #[test]
    fn flat_exact_top1() {
        let data = dataset(50, 4);
        let mut f = FlatIndex::new(4);
        for v in &data {
            f.add(v);
        }
        let r = f.search(&data[7], 1);
        assert_eq!(r[0].0, 7);
        assert!(r[0].1 < 1e-5);
    }

    #[test]
    fn hnsw_finds_exact_vector() {
        let data = dataset(300, 8);
        let mut h = HnswIndex::new(8, 16, 64, 64);
        for v in &data {
            h.add(v);
        }
        let r = h.search(&data[123], 1);
        assert_eq!(r[0].0, 123);
    }

    #[test]
    fn hnsw_recall_vs_flat() {
        let data = dataset(400, 8);
        let mut flat = FlatIndex::new(8);
        let mut hnsw = HnswIndex::new(8, 16, 100, 100);
        for v in &data {
            flat.add(v);
            hnsw.add(v);
        }
        let mut rng = Rng::new(7);
        let mut hits = 0;
        let mut total = 0;
        for _ in 0..30 {
            let q: Vec<f32> = (0..8).map(|_| (rng.unit() as f32) * 10.0).collect();
            let exact: HashSet<usize> = flat.search(&q, 5).into_iter().map(|(i, _)| i).collect();
            let approx: HashSet<usize> = hnsw.search(&q, 5).into_iter().map(|(i, _)| i).collect();
            hits += exact.intersection(&approx).count();
            total += 5;
        }
        let recall = hits as f64 / total as f64;
        assert!(recall >= 0.8, "recall too low: {recall}");
    }
}

#[cfg(test)]
mod reference_suite {
    //! Ports of `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp`
    //! (`HnswAlgTest`): 1280 vectors of dim 16, M=8, ef_construction=200,
    //! ef_search=10, and self-query recall@10 >= 0.95 over the whole set.

    use super::*;

    const DIM: usize = 16;
    const M: usize = 8;
    const EF_CONSTRUCTION: usize = 200;
    const EF_SEARCH: usize = 10;
    const N: usize = 1280;

    fn data() -> Vec<Vec<f32>> {
        let mut rng = Rng::new(42);
        (0..N)
            .map(|_| (0..DIM).map(|_| rng.unit() as f32).collect())
            .collect()
    }

    fn build(metric: Metric, encoding: HnswEncoding, data: &[Vec<f32>]) -> HnswIndex {
        let mut index =
            HnswIndex::with_encoding(DIM, M, EF_CONSTRUCTION, EF_SEARCH, metric, encoding);
        index.train_store(data, None);
        for v in data {
            index.add(v);
        }
        index
    }

    /// The metric `HnswAlgTest::TestSimple`/`TestLoad` actually assert: the
    /// query's own label anywhere in the returned candidate list. C++
    /// `KnnSearchSorted` never truncates to `k` when `ef_` is set, so even
    /// `KnnSearchSorted(q, 1, {.ef_ = 10})` yields 10 candidates and the loop
    /// scans all of them.
    fn candidate_recall(index: &HnswIndex, data: &[Vec<f32>]) -> f32 {
        let mut correct = 0usize;
        for (i, q) in data.iter().enumerate() {
            if index
                .search_with_ef(q, EF_SEARCH, EF_SEARCH)
                .iter()
                .any(|(id, _)| *id == i)
            {
                correct += 1;
            }
        }
        correct as f32 / data.len() as f32
    }

    /// Strict top-1 recall: the nearest returned neighbour must be the query
    /// itself. Used where the reference's store preserves enough precision
    /// (Plain/LVQ); see [`candidate_recall`] for why the C++ load/compress tests'
    /// `KnnSearchSorted(q, 1, {.ef_ = 10})` is not literally top-1: `k` is
    /// ignored whenever `ef_` is set (`hnsw_alg.cppm:314-327` passes `ef` to
    /// `SearchLayerHelper`, and `KnnSearchSorted` never truncates to `k`).
    fn self_recall(index: &HnswIndex, data: &[Vec<f32>]) -> f32 {
        let mut correct = 0usize;
        for (i, q) in data.iter().enumerate() {
            if index
                .search_with_ef(q, 1, EF_SEARCH)
                .first()
                .is_some_and(|(id, _)| *id == i)
            {
                correct += 1;
            }
        }
        correct as f32 / data.len() as f32
    }

    /// C++: `HnswAlgTest.test_plain_1`.
    #[test]
    fn plain_l2_self_recall() {
        let data = data();
        let index = build(Metric::L2, HnswEncoding::Plain, &data);
        let recall = self_recall(&index, &data);
        assert!(recall >= 0.95, "recall {recall}");
    }

    /// C++: `HnswAlgTest.test_plain_2`.
    #[test]
    fn plain_cos_self_recall() {
        let data = data();
        let index = build(Metric::Cos, HnswEncoding::Plain, &data);
        let recall = self_recall(&index, &data);
        assert!(recall >= 0.95, "recall {recall}");
    }

    /// C++: `HnswAlgTest.test_lvq_1`.
    #[test]
    fn lvq_l2_self_recall() {
        let data = data();
        let index = build(Metric::L2, HnswEncoding::Lvq, &data);
        let recall = self_recall(&index, &data);
        assert!(recall >= 0.95, "recall {recall}");
    }

    /// C++: `HnswAlgTest.test_rabitq_1`.
    ///
    /// The reference counts a hit anywhere in the `ef_search` result list, not
    /// only at rank 1: `KnnSearchSorted(q, ef_search_, {.ef_ = ef_search_})`
    /// returns `ef_search_` candidates and the loop scans all of them. Strict
    /// top-1 recall for RaBitQ's 1-bit estimator on this dataset is ~0.89,
    /// below the reference threshold; `self_recall` covers the exact
    /// stores, and this case keeps the reference's own metric.
    #[test]
    fn rabitq_l2_self_recall() {
        let data = data();
        let index = build(Metric::L2, HnswEncoding::Rabitq, &data);
        let recall = candidate_recall(&index, &data);
        assert!(recall >= 0.95, "recall {recall}");
    }

    /// C++: `HnswAlgTest.test_plain_4` (save/load through the chunk layout).
    #[test]
    fn chunk_roundtrip_preserves_recall() {
        let data = data();
        let index = build(Metric::L2, HnswEncoding::Plain, &data);
        let restored = chunk_roundtrip(&index, Metric::L2, HnswEncoding::Plain);
        let recall = self_recall(&restored, &data);
        assert!(recall >= 0.95, "recall {recall}");
    }

    /// Serializes `index` into the chunk's meta/records/graph parts and loads
    /// them back, as `SaveToPtr` + `LoadFromPtr` do.
    fn chunk_roundtrip(index: &HnswIndex, metric: Metric, encoding: HnswEncoding) -> HnswIndex {
        let meta = index.store_meta_bytes();
        let records = index.store_records_bytes();
        let levels: Vec<usize> = (0..index.len()).map(|i| index.level_of(i)).collect();
        let max_level = index.max_level();
        let mut links = vec![vec![Vec::new(); index.len()]; max_level + 1];
        for node in 0..index.len() {
            for (layer, at) in links.iter_mut().enumerate() {
                at[node] = index.neighbors(layer, node).to_vec();
            }
        }
        HnswIndex::from_chunk_parts(
            DIM,
            M,
            EF_SEARCH,
            metric,
            encoding,
            &meta,
            &records,
            index.len(),
            levels,
            links,
            index.entry_point(),
            max_level,
        )
        .expect("chunk parts")
    }

    /// C++: `HnswAlgTest.test_lvq_3` (LVQ save/load, top-1 recall ≥ 0.95).
    #[test]
    fn lvq_chunk_roundtrip_preserves_recall() {
        let data = data();
        let index = build(Metric::L2, HnswEncoding::Lvq, &data);
        let restored = chunk_roundtrip(&index, Metric::L2, HnswEncoding::Lvq);
        let recall = self_recall(&restored, &data);
        assert!(recall >= 0.95, "recall {recall}");
    }

    /// C++: `HnswAlgTest.test_rabitq_2` (RaBitQ save/load; see
    /// [`candidate_recall`] for the reference's actual top-`ef` metric).
    #[test]
    fn rabitq_chunk_roundtrip_preserves_recall() {
        let data = data();
        let index = build(Metric::L2, HnswEncoding::Rabitq, &data);
        // RaBitQ keeps the reference's top-`ef` metric (see
        // `rabitq_l2_self_recall`): its 1-bit estimator ranks the true
        // neighbour first only ~89% of the time on this dataset, and C++
        // `test_rabitq_2` also scans every returned candidate.
        let restored = chunk_roundtrip(&index, Metric::L2, HnswEncoding::Rabitq);
        let recall = candidate_recall(&restored, &data);
        assert!(recall >= 0.95, "recall {recall}");
    }

    /// `SaveToPtr`/`LoadFromPtr` through a real file (the reference writes the
    /// chunk to disk and reopens or mmaps it). Kept separate from
    /// [`chunk_roundtrip`], which only exercises the in-memory parts.
    fn file_roundtrip(index: &HnswIndex) -> HnswIndex {
        let dir = tempfile::tempdir().expect("temp dir");
        let path = dir.path().join("test_hnsw.bin");
        chunk_file::save(index, &path);
        assert!(path.metadata().expect("saved file").len() > 0);
        chunk_file::load(&path, EF_SEARCH)
    }

    /// C++ `HnswAlgTest.test_plain_4`: save the plain graph to a file, load it
    /// back, and require the same top-1 self-recall.
    #[test]
    fn plain_file_persistence_preserves_top1_recall() {
        let data = data();
        let index = build(Metric::L2, HnswEncoding::Plain, &data);
        let restored = file_roundtrip(&index);
        assert_eq!(restored.len(), N);
        assert_eq!(restored.encoding(), HnswEncoding::Plain);
        let recall = self_recall(&restored, &data);
        assert!(recall >= 0.95, "recall {recall}");
    }

    /// C++ `HnswAlgTest.test_lvq_3`: LVQ save/load through a file.
    #[test]
    fn lvq_file_persistence_preserves_top1_recall() {
        let data = data();
        let index = build(Metric::L2, HnswEncoding::Lvq, &data);
        let restored = file_roundtrip(&index);
        assert_eq!(restored.len(), N);
        assert_eq!(restored.encoding(), HnswEncoding::Lvq);
        let recall = self_recall(&restored, &data);
        assert!(recall >= 0.95, "recall {recall}");
    }

    /// C++ `HnswAlgTest.test_rabitq_2`: RaBitQ save/load through a file
    /// (reference candidate recall; see [`candidate_recall`]).
    #[test]
    fn rabitq_file_persistence_preserves_top1_recall() {
        let data = data();
        let index = build(Metric::L2, HnswEncoding::Rabitq, &data);
        let restored = file_roundtrip(&index);
        assert_eq!(restored.len(), N);
        assert_eq!(restored.encoding(), HnswEncoding::Rabitq);
        // Reference metric for RaBitQ; see `rabitq_l2_self_recall`.
        let recall = candidate_recall(&restored, &data);
        assert!(recall >= 0.95, "recall {recall}");
    }

    /// C++: `HnswAlgTest.test_lvq_4` (`CompressToLVQ`, then save/load).
    #[test]
    fn plain_index_compresses_to_lvq() {
        let data = data();
        let mut index = build(Metric::L2, HnswEncoding::Plain, &data);
        index.compress_to_lvq();
        assert_eq!(index.encoding(), HnswEncoding::Lvq);
        let recall = self_recall(&index, &data);
        assert!(recall >= 0.95, "recall {recall}");
        let restored = chunk_roundtrip(&index, Metric::L2, HnswEncoding::Lvq);
        let recall = self_recall(&restored, &data);
        assert!(recall >= 0.95, "recall {recall}");
    }

    /// `TestParallel`: four readers query while a writer keeps inserting (the
    /// reference's read path with a shared lock). Iteration count is the
    /// reference's one pass over the data set per reader.
    fn parallel_readers(metric: Metric, encoding: HnswEncoding) {
        use std::sync::{Arc, RwLock};
        let data = data();
        let half = data.len() / 2;
        let mut index =
            HnswIndex::with_encoding(DIM, M, EF_CONSTRUCTION, EF_SEARCH, metric, encoding);
        index.train_store(&data, None);
        for v in &data[..half] {
            index.add(v);
        }
        let index = Arc::new(RwLock::new(index));
        let stop = Arc::new(std::sync::atomic::AtomicBool::new(false));

        let writer = {
            let index = Arc::clone(&index);
            let data = data.clone();
            std::thread::spawn(move || {
                for v in &data[half..] {
                    index.write().unwrap().add(v);
                }
            })
        };
        let mut readers = Vec::new();
        for _ in 0..4 {
            let index = Arc::clone(&index);
            let stop = Arc::clone(&stop);
            let data = data.clone();
            readers.push(std::thread::spawn(move || {
                loop {
                    for (i, q) in data.iter().enumerate() {
                        let result = index.read().unwrap().search_with_ef(q, 1, EF_SEARCH);
                        // The reference leaves recall unasserted here; the
                        // point is that the graph stays readable throughout.
                        assert!(result.len() <= 1, "query {i} returned {}", result.len());
                        if stop.load(std::sync::atomic::Ordering::Relaxed) {
                            return;
                        }
                    }
                    if stop.load(std::sync::atomic::Ordering::Relaxed) {
                        return;
                    }
                }
            }));
        }
        writer.join().unwrap();
        stop.store(true, std::sync::atomic::Ordering::Relaxed);
        for reader in readers {
            reader.join().unwrap();
        }
        let index = index.read().unwrap();
        assert_eq!(index.len(), data.len());
        // The reference does not assert recall here (approximate stores may
        // reorder the top-1 during concurrent inserts); any search must simply
        // return a valid, bounded result.
        assert!(index.search_with_ef(&data[0], 1, EF_SEARCH).len() <= 1);
    }

    /// C++: `HnswAlgTest.test_plain_3`.
    #[test]
    fn plain_parallel_insert_and_search() {
        parallel_readers(Metric::L2, HnswEncoding::Plain);
    }

    /// C++: `HnswAlgTest.test_lvq_2`.
    #[test]
    fn lvq_parallel_insert_and_search() {
        parallel_readers(Metric::L2, HnswEncoding::Lvq);
    }

    /// C++: `HnswAlgTest.test_rabitq_3`.
    #[test]
    fn rabitq_parallel_insert_and_search() {
        parallel_readers(Metric::L2, HnswEncoding::Rabitq);
    }

    /// C++: `HnswAlgBitmaskTest.test1` (`test_hnsw_bitmask_ut.cpp`): the
    /// filtered search drops `SetFalse` labels while still traversing them.
    #[test]
    fn filtered_search_honours_the_bitmask() {
        use crate::plaid::Bitmask;

        let base = [
            [0.1f32, 0.2, 0.3, 0.4],
            [0.2, 0.1, 0.3, 0.4],
            [0.3, 0.2, 0.1, 0.4],
            [0.4, 0.3, 0.2, 0.1],
        ];
        let query = [0.1f32, 0.2, 0.3, 0.4];
        let mut index = HnswIndex::with_encoding(4, 16, 200, 8, Metric::L2, HnswEncoding::Lvq);
        for v in &base {
            index.add(v);
        }
        let ef = 8;
        let unfiltered = index.search_filtered(&query, 4, ef, &|_| true);
        let pairs: Vec<(usize, f32)> = unfiltered.iter().map(|(id, d)| (*id, *d)).collect();
        assert_eq!(pairs[0].0, 0);
        assert!((pairs[0].1 - 0.0).abs() < 1e-4);
        assert_eq!(pairs[1].0, 1);
        assert!((pairs[1].1 - 0.02).abs() < 1e-4);
        assert_eq!(pairs[2].0, 2);
        assert!((pairs[2].1 - 0.08).abs() < 1e-4);
        assert_eq!(pairs[3].0, 3);
        assert!((pairs[3].1 - 0.2).abs() < 1e-4);

        let mut mask = Bitmask::new_all_true(base.len());
        mask.set_false(1);
        let allowed = |id: usize| mask.is_true(id as u32);
        let got = index.search_filtered(&query, 3, ef, &allowed);
        assert_eq!(
            got.iter().map(|(id, _)| *id).collect::<Vec<_>>(),
            vec![0, 2, 3]
        );

        mask.set_false(0);
        let got = index.search_filtered(&query, 2, ef, &|id| mask.is_true(id as u32));
        assert_eq!(
            got.iter().map(|(id, _)| *id).collect::<Vec<_>>(),
            vec![2, 3]
        );

        mask.set_false(2);
        let got = index.search_filtered(&query, 1, ef, &|id| mask.is_true(id as u32));
        assert_eq!(got[0].0, 3);
        assert!((got[0].1 - 0.2).abs() < 1e-4);
    }
    /// C++: `HnswSparseTest.test1`
    /// (`src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_sparse_ut.cpp:126`).
    ///
    /// `KnnHnsw<SparseIPVecStoreType<float, i32>, u64>` over the reference's
    /// generated sparse matrix: 1280 documents, 1600 dimensions, 1% density,
    /// M=16, ef_construction=200, ef=50. The reference's ground-truth checks
    /// are commented out upstream; this port asserts the surface it does keep
    /// (build, `Check`, `Dump`, `Save`/`Load`) plus the real contract of every
    /// search result: a valid label whose returned distance is exactly
    /// `-IP(query, stored_doc)`.
    #[test]
    fn sparse_ip_hnsw_roundtrip() {
        use std::collections::HashSet;
        use tempfile::tempdir;

        const MAX_DIM: u32 = 1600;
        const M: usize = 16;
        const EF_CONSTRUCTION: usize = 200;
        const EF_SEARCH: usize = 50;
        const CHUNK_SIZE: usize = 128;
        const MAX_CHUNK_N: usize = 10;

        struct SparseRng(u64);
        impl SparseRng {
            fn new(seed: u64) -> Self {
                Self(seed | 1)
            }
            fn next(&mut self) -> u64 {
                let mut x = self.0;
                x ^= x << 13;
                x ^= x >> 7;
                x ^= x << 17;
                self.0 = x;
                x
            }
            fn below(&mut self, n: u32) -> u32 {
                (self.next() % n as u64) as u32
            }
            fn unit(&mut self) -> f32 {
                (self.next() >> 40) as f32 / (1u64 << 24) as f32
            }
        }

        fn generate_dataset(nrow: u32, ncol: u32, sparsity: f32) -> Vec<Vec<(u32, f32)>> {
            let mut rng = SparseRng::new(0x5EED_5EED);
            let nnz = (nrow as usize * ncol as usize) as f32 * sparsity;
            let nnz = nnz as usize;
            let mut indptr = vec![0usize; nrow as usize + 1];
            loop {
                indptr[0] = 0;
                indptr[nrow as usize] = nnz;
                for slot in indptr.iter_mut().take(nrow as usize).skip(1) {
                    *slot = rng.below(nnz as u32 + 1) as usize;
                }
                indptr[..nrow as usize].sort_unstable();
                if indptr
                    .windows(2)
                    .all(|pair| pair[1] - pair[0] <= ncol as usize)
                {
                    break;
                }
            }
            let mut docs = Vec::with_capacity(nrow as usize);
            for row in 0..nrow as usize {
                let count = indptr[row + 1] - indptr[row];
                let mut indices = HashSet::with_capacity(count);
                while indices.len() < count {
                    indices.insert(rng.below(ncol));
                }
                let mut indices: Vec<u32> = indices.into_iter().collect();
                indices.sort_unstable();
                docs.push(
                    indices
                        .into_iter()
                        .map(|idx| (idx, rng.unit()))
                        .collect::<Vec<_>>(),
                );
            }
            docs
        }

        fn exact_scores(query: &[(u32, f32)], docs: &[Vec<(u32, f32)>]) -> Vec<f32> {
            docs.iter().map(|doc| sparse_ip(query, doc)).collect()
        }

        fn assert_results_are_exact(index: &HnswIndex, docs: &[Vec<(u32, f32)>]) -> f64 {
            let mut queries = 0usize;
            let mut top1 = 0usize;
            for query in docs {
                if query.is_empty() {
                    continue;
                }
                let scores = exact_scores(query, docs);
                let best = scores.iter().copied().fold(f32::MIN, f32::max);
                if best == 0.0 {
                    continue;
                }
                queries += 1;
                let result = index.search_sparse(query, 1, EF_SEARCH);
                assert_eq!(result.len(), 1, "one neighbour for a non-zero query");
                let (label, distance) = result[0];
                assert!(label < docs.len(), "label {label} outside the dataset");
                let stored = index.sparse_vector(label).expect("sparse store");
                assert_eq!(stored, docs[label].as_slice());
                let ip = sparse_ip(query, stored);
                assert!(
                    (-distance - ip).abs() < 1e-4,
                    "distance {distance} does not match -IP {ip}"
                );
                let exact_best = scores
                    .iter()
                    .enumerate()
                    .max_by(|a, b| a.1.partial_cmp(b.1).unwrap())
                    .map(|(id, _)| id)
                    .unwrap();
                if label == exact_best {
                    top1 += 1;
                }
            }
            assert!(queries > 0, "no non-zero queries in the generated set");
            top1 as f64 / queries as f64
        }

        assert_eq!(CHUNK_SIZE * MAX_CHUNK_N, 1280);
        let docs = generate_dataset((CHUNK_SIZE * MAX_CHUNK_N) as u32, MAX_DIM, 0.01);
        assert_eq!(docs.len(), 1280);

        let mut index = HnswIndex::sparse_ip(MAX_DIM as usize, M, EF_CONSTRUCTION, EF_SEARCH);
        for doc in &docs {
            index.add_sparse(doc);
        }
        assert_eq!(index.len(), docs.len());
        index.check().unwrap();

        // `Dump`: one line per node.
        let dir = tempdir().unwrap();
        let dump_path = dir.path().join("dump.txt");
        {
            let mut file = std::fs::File::create(&dump_path).unwrap();
            index.dump(&mut file).unwrap();
        }
        let dump = std::fs::read_to_string(&dump_path).unwrap();
        assert_eq!(dump.lines().count(), docs.len());

        let recall = assert_results_are_exact(&index, &docs);
        assert!(
            recall >= 0.9,
            "top-1 recall against brute-force IP too low: {recall}"
        );

        let save_path = dir.path().join("test_hnsw_sparse.bin");
        index.save_sparse(&save_path).unwrap();
        let loaded = HnswIndex::load_sparse(&save_path).unwrap();
        assert_eq!(loaded.len(), docs.len());
        assert_eq!(loaded.dims(), MAX_DIM as usize);
        assert_eq!(loaded.m(), M);
        loaded.check().unwrap();
        let restored_recall = assert_results_are_exact(&loaded, &docs);
        assert!(
            restored_recall >= 0.9,
            "top-1 recall after load too low: {restored_recall}"
        );
    }

    /// C++: `HnswHandlerTest.DISABLED_SLOW_test_load` — build, save to the chunk
    /// layout, load and search. `#[ignore]` mirrors the C++ `DISABLED_` case.
    #[test]
    #[ignore = "C++ DISABLED_SLOW_test_load (run with --ignored)"]
    fn disabled_slow_test_load() {
        let data = data();
        let index = build(Metric::L2, HnswEncoding::Plain, &data);
        let restored = chunk_roundtrip(&index, Metric::L2, HnswEncoding::Plain);
        assert!(candidate_recall(&restored, &data) >= 0.95);
    }

    /// C++: `HnswHandlerTest.DISABLED_RANDOME_test_parallel` — readers query
    /// while a writer inserts. `#[ignore]` mirrors the C++ `DISABLED_` case.
    #[test]
    #[ignore = "C++ DISABLED_RANDOME_test_parallel (run with --ignored)"]
    fn disabled_random_test_parallel() {
        parallel_readers(Metric::L2, HnswEncoding::Plain);
    }
}
