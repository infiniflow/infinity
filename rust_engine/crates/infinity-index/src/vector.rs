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

/// Approximate kNN via a multi-layer navigable graph.
pub struct HnswIndex {
    dims: usize,
    m: usize,
    ef_construction: usize,
    ef_search: usize,
    ml: f64,
    data: Vec<Vec<f32>>,
    levels: Vec<usize>,
    /// `links[layer][node]` = neighbor node ids.
    links: Vec<Vec<Vec<usize>>>,
    entry: Option<usize>,
    max_level: usize,
    rng: Rng,
}

impl HnswIndex {
    pub fn new(dims: usize, m: usize, ef_construction: usize, ef_search: usize) -> Self {
        Self {
            dims,
            m,
            ef_construction: ef_construction.max(m),
            ef_search,
            ml: 1.0 / (m as f64).ln(),
            data: Vec::new(),
            levels: Vec::new(),
            links: Vec::new(),
            entry: None,
            max_level: 0,
            rng: Rng::new(0x9E37_79B9_7F4A_7C15),
        }
    }

    pub fn len(&self) -> usize {
        self.data.len()
    }

    pub fn is_empty(&self) -> bool {
        self.data.is_empty()
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

    /// The stored vector of `node`.
    pub fn vector(&self, node: usize) -> &[f32] {
        &self.data[node]
    }

    /// The entry point, if any vectors were added.
    pub fn entry_point(&self) -> Option<usize> {
        self.entry
    }

    /// The highest populated layer.
    pub fn max_level(&self) -> usize {
        self.max_level
    }

    /// Rebuilds an index from a serialized graph (the C++ HNSW chunk's header,
    /// vector region, graph region and labels), so a query can run over the
    /// graph that is actually on disk.
    ///
    /// `levels[i]` is how many layers above level 0 node `i` appears in,
    /// `links[layer][node]` its neighbours on that layer.
    #[allow(clippy::too_many_arguments)]
    pub fn from_parts(
        dims: usize,
        m: usize,
        ef_search: usize,
        data: Vec<Vec<f32>>,
        levels: Vec<usize>,
        mut links: Vec<Vec<Vec<usize>>>,
        entry: Option<usize>,
        max_level: usize,
    ) -> Self {
        // Every node must exist on every layer it claims, so `search`'s
        // bounds-checked lookups find an empty slice rather than nothing.
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
            data,
            levels,
            links,
            entry,
            max_level,
            rng: Rng::new(0x9E37_79B9_7F4A_7C15),
        }
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

    pub fn add(&mut self, v: &[f32]) -> usize {
        assert_eq!(v.len(), self.dims, "dimension mismatch");
        let node = self.data.len();
        self.data.push(v.to_vec());
        let level = self.random_level();
        self.levels.push(level);
        for l in 0..=level {
            self.ensure(l, node);
        }

        if self.entry.is_none() {
            self.entry = Some(node);
            self.max_level = level;
            return node;
        }

        // `Build` in the reference: descend greedily through the layers above
        // this node's level, then link it layer by layer from the top down.
        let mut ep = self.entry.unwrap();
        for l in ((level + 1)..=self.max_level).rev() {
            ep = self.greedy(v, ep, l);
        }
        for l in (0..=level.min(self.max_level)).rev() {
            let cands = self.search_layer(v, &[ep], self.ef_construction, l);
            let neighbors = self.select_neighbors_heuristic(&cands, self.m);
            if let Some((_, first)) = neighbors.first() {
                ep = *first;
            }
            self.ensure(l, node);
            self.ensure(l, ep);
            for (_, nb) in &neighbors {
                self.ensure(l, *nb);
            }
            self.links[l][node] = neighbors.iter().map(|(_, nb)| *nb).collect();
            let max_m = if l == 0 { self.m * 2 } else { self.m };
            for (_, nb) in neighbors {
                self.ensure(l, nb);
                if !self.links[l][nb].contains(&node) {
                    self.links[l][nb].push(node);
                }
                self.prune(l, nb, max_m);
            }
        }
        if level > self.max_level {
            self.max_level = level;
            self.entry = Some(node);
        }
        node
    }

    /// The reference's `SelectNeighborsHeuristic`: walk the candidates closest
    /// first and keep one only when it is closer to the query than to every
    /// neighbour already kept (this spreads the links over directions).
    fn select_neighbors_heuristic(&self, cands: &[(f32, usize)], m: usize) -> Vec<(f32, usize)> {
        let mut sorted = cands.to_vec();
        sorted.sort_by(|a, b| a.0.partial_cmp(&b.0).unwrap());
        let mut kept: Vec<(f32, usize)> = Vec::with_capacity(m);
        for (d, node) in sorted {
            if kept.len() == m {
                break;
            }
            let dominated = kept
                .iter()
                .any(|(_, s)| l2_sq(&self.data[node], &self.data[*s]) < d);
            if !dominated {
                kept.push((d, node));
            }
        }
        kept
    }

    /// Drops the worst links of `node` on `layer` beyond `max_m`, re-running the
    /// selection heuristic over the current neighbourhood.
    fn prune(&mut self, layer: usize, node: usize, max_m: usize) {
        if self.links[layer][node].len() <= max_m {
            return;
        }
        let q = self.data[node].clone();
        let cands: Vec<(f32, usize)> = self.links[layer][node]
            .iter()
            .map(|&nb| (l2_sq(&q, &self.data[nb]), nb))
            .collect();
        let kept = self.select_neighbors_heuristic(&cands, max_m);
        self.links[layer][node] = kept.into_iter().map(|(_, nb)| nb).collect();
    }

    fn greedy(&self, q: &[f32], start: usize, layer: usize) -> usize {
        let mut cur = start;
        loop {
            let mut best = cur;
            let mut best_d = l2_sq(q, &self.data[cur]);
            if layer < self.links.len() && cur < self.links[layer].len() {
                for &nb in &self.links[layer][cur] {
                    let d = l2_sq(q, &self.data[nb]);
                    if d < best_d {
                        best_d = d;
                        best = nb;
                    }
                }
            }
            if best == cur {
                return cur;
            }
            cur = best;
        }
    }

    /// Beam search at `layer` starting from `entries`, keeping the `ef` best.
    fn search_layer(
        &self,
        q: &[f32],
        entries: &[usize],
        ef: usize,
        layer: usize,
    ) -> Vec<(f32, usize)> {
        let mut visited: HashSet<usize> = entries.iter().copied().collect();
        let mut candidates: Vec<(f32, usize)> = entries
            .iter()
            .map(|&e| (l2_sq(q, &self.data[e]), e))
            .collect();
        candidates.sort_by(|a, b| a.0.partial_cmp(&b.0).unwrap());
        let mut result = candidates.clone();

        while !candidates.is_empty() {
            let (cd, c) = candidates.remove(0);
            let worst = result.last().map(|x| x.0).unwrap_or(f32::INFINITY);
            if result.len() >= ef && cd > worst {
                break;
            }
            if layer < self.links.len() && c < self.links[layer].len() {
                for &nb in &self.links[layer][c] {
                    if visited.insert(nb) {
                        let d = l2_sq(q, &self.data[nb]);
                        let worst = result.last().map(|x| x.0).unwrap_or(f32::INFINITY);
                        if result.len() < ef || d < worst {
                            candidates.push((d, nb));
                            candidates.sort_by(|a, b| a.0.partial_cmp(&b.0).unwrap());
                            result.push((d, nb));
                            result.sort_by(|a, b| a.0.partial_cmp(&b.0).unwrap());
                            result.truncate(ef);
                        }
                    }
                }
            }
        }
        result
    }

    /// Returns `(row_id, distance)` for the `k` approximate nearest, ascending.
    pub fn search(&self, q: &[f32], k: usize) -> Vec<(usize, f32)> {
        let Some(entry) = self.entry else {
            return vec![];
        };
        let mut cur = entry;
        let mut l = self.max_level as i64;
        while l > 0 {
            cur = self.greedy(q, cur, l as usize);
            l -= 1;
        }
        let ef = self.ef_search.max(k);
        let mut res = self.search_layer(q, &[cur], ef, 0);
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
