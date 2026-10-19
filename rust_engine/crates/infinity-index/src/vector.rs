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
}

/// The vector payload of an HNSW graph.
pub enum Store {
    Plain { metric: Metric, data: Vec<Vec<f32>> },
    Lvq(crate::lvq::LvqStore),
    Rabitq(crate::rabitq::RabitqStore),
}

fn plain_distance(a: &[f32], b: &[f32], metric: Metric) -> f32 {
    match metric {
        Metric::L2 => l2_sq(a, b),
        Metric::Ip => -a.iter().zip(b).map(|(x, y)| x * y).sum::<f32>(),
        Metric::Cos => {
            let dot: f32 = a.iter().zip(b).map(|(x, y)| x * y).sum();
            let na: f32 = a.iter().map(|x| x * x).sum::<f32>().sqrt();
            let nb: f32 = b.iter().map(|x| x * x).sum::<f32>().sqrt();
            if na == 0.0 || nb == 0.0 {
                1.0
            } else {
                1.0 - dot / (na * nb)
            }
        }
    }
}

impl Store {
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
        }
    }

    fn make_query(&self, v: &[f32]) -> HnswQuery {
        match self {
            Store::Plain { .. } => HnswQuery::Plain(v.to_vec()),
            Store::Lvq(store) => HnswQuery::Lvq(store.make_query(v)),
            Store::Rabitq(store) => HnswQuery::Rabitq(store.make_query(v)),
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
        }
    }

    /// The decompressed vector of `node` (allocates for quantized stores).
    fn decompress(&self, node: usize) -> Vec<f32> {
        match self {
            Store::Plain { data, .. } => data.get(node).cloned().unwrap_or_default(),
            Store::Lvq(store) => store
                .decompress(node)
                .map(|v| v.into_iter().map(|x| x as f32).collect())
                .unwrap_or_default(),
            Store::Rabitq(store) => store.decompress(node).unwrap_or_default(),
        }
    }

    fn mem_bytes(&self) -> usize {
        match self {
            Store::Plain { data, .. } => data.iter().map(|v| v.capacity() * 4).sum(),
            Store::Lvq(store) => store.len() * store.record_size() + store.mean().len() * 8 + 64,
            Store::Rabitq(store) => {
                store.len() * store.record_size() + store.dim() * store.dim() * 4 + 4 * store.dim()
            }
        }
    }

    fn encoding(&self) -> HnswEncoding {
        match self {
            Store::Plain { .. } => HnswEncoding::Plain,
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
        }
    }

    /// Trains the store's mean/centroid without storing any vector.
    fn train(&mut self, vectors: &[Vec<f32>]) {
        match self {
            Store::Plain { .. } => {}
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
        Self {
            dims,
            m,
            ef_construction: ef_construction.max(m),
            ef_search,
            ml: 1.0 / (m as f64).ln(),
            store: Store::new(dims, metric, encoding),
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

    /// The vec-store meta blob written into the HNSW chunk header.
    pub fn store_meta_bytes(&self) -> Vec<u8> {
        match &self.store {
            Store::Plain { .. } => (self.dims as u64).to_le_bytes().to_vec(),
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
        }
    }

    /// Bytes per vector in the chunk's vector region.
    pub fn store_record_size(&self) -> usize {
        match &self.store {
            Store::Plain { .. } => self.dims * 4,
            Store::Lvq(store) => store.record_size(),
            Store::Rabitq(store) => store.record_size(),
        }
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
        let node = self.store.len();
        self.store.push(v);
        let query = self.store.make_query(v);
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
        let mut visited: HashSet<usize> = HashSet::new();
        visited.insert(ep);
        let mut candidates: Vec<(f32, usize)> = vec![(self.distance_query(q, self_id, ep), ep)];
        let mut result: Vec<(f32, usize)> = candidates.clone();

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
                        result.push((d, nb));
                        result.sort_by(|a, b| {
                            a.0.partial_cmp(&b.0).unwrap_or(std::cmp::Ordering::Equal)
                        });
                        result.truncate(ef);
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
        let mut res = self.search_layer(&query, cur, ef, 0, None);
        res.sort_by(|a, b| a.0.partial_cmp(&b.0).unwrap());
        res.truncate(k);
        res.into_iter().map(|(d, i)| (i, d)).collect()
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
