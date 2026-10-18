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
        // `GenerateRandomLayer` (`hnsw_alg.cppm`): `-log(U(0,1)) * mult`,
        // `mult = 1 / ln(M)`.
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

    /// `SelectNeighborsHeuristic` (`hnsw_alg.cppm:231`). Candidates closer than
    /// an already-kept neighbour are dropped; the returned list is ordered
    /// **farthest first**, which is what `Build` uses for its next entry point.
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
            // Keep the candidate unless some kept neighbour is closer to it
            // than the query is.
            let dominated = kept
                .iter()
                .any(|(_, s)| l2_sq(&self.data[node], &self.data[*s]) < d);
            if !dominated {
                kept.push((d, node));
            }
        }
        kept.reverse();
        kept
    }

    /// `ConnectNeighbors` (`hnsw_alg.cppm:259`): add the reverse link, pruning
    /// the target's list to `2 * M` on layer 0 and `M` above.
    fn connect_neighbors(&mut self, target: usize, node: usize, layer: usize) {
        self.ensure(layer, target);
        let max_m = if layer == 0 { self.m * 2 } else { self.m };
        if self.links[layer][target].len() < max_m {
            self.links[layer][target].push(node);
            return;
        }
        let mut cands: Vec<(f32, usize)> = Vec::with_capacity(self.links[layer][target].len() + 1);
        cands.push((l2_sq(&self.data[target], &self.data[node]), node));
        for &nb in &self.links[layer][target] {
            cands.push((l2_sq(&self.data[target], &self.data[nb]), nb));
        }
        let kept = self.select_neighbors_heuristic(&cands, max_m);
        self.links[layer][target] = kept.into_iter().map(|(_, n)| n).collect();
    }

    /// `Build` (`hnsw_alg.cppm:390`): take the new random layer, update the
    /// entry point **before** searching (the old entry stays the start), descend
    /// greedily to the node's layer, then link it layer by layer from the top.
    pub fn add(&mut self, v: &[f32]) -> usize {
        assert_eq!(v.len(), self.dims, "dimension mismatch");
        let node = self.data.len();
        self.data.push(v.to_vec());
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
        let query = self.data[node].to_vec();

        let mut cur = max_layer as i64;
        while cur > q_layer as i64 {
            ep = self.search_layer_nearest(&query, ep, cur as usize);
            cur -= 1;
        }
        let mut l = q_layer.min(max_layer) as i64;
        while l >= 0 {
            let cands = self.search_layer(&query, ep, self.ef_construction, l as usize);
            let neighbors = self.select_neighbors_heuristic(&cands, self.m);
            if neighbors.is_empty() {
                l -= 1;
                continue;
            }
            // The reference continues from the **farthest** kept neighbour.
            ep = neighbors[0].1;
            self.links[l as usize][node] = neighbors.iter().map(|(_, n)| *n).collect();
            for (_, nb) in &neighbors {
                self.connect_neighbors(*nb, node, l as usize);
            }
            l -= 1;
        }
        node
    }

    /// `SearchLayerNearest` (`hnsw_alg.cppm:204`): greedy walk, scanning a
    /// node's neighbours in reverse order and moving to any closer one.
    fn search_layer_nearest(&self, q: &[f32], start: usize, layer: usize) -> usize {
        let mut cur = start;
        let mut cur_d = l2_sq(q, &self.data[cur]);
        loop {
            let mut changed = false;
            if let Some(list) = self.links.get(layer).and_then(|l| l.get(cur)) {
                for &nb in list.iter().rev() {
                    let d = l2_sq(q, &self.data[nb]);
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

    /// `SearchLayer` (`hnsw_alg.cppm:124`): best-first beam keeping `ef`
    /// results. A candidate is expanded while the result set is not full or its
    /// distance does not exceed the current worst (`<=`, as in the reference).
    fn search_layer(&self, q: &[f32], ep: usize, ef: usize, layer: usize) -> Vec<(f32, usize)> {
        let mut visited: HashSet<usize> = HashSet::new();
        visited.insert(ep);
        let mut candidates: Vec<(f32, usize)> = vec![(l2_sq(q, &self.data[ep]), ep)];
        let mut result: Vec<(f32, usize)> = candidates.clone();

        while !candidates.is_empty() {
            let (cd, c) = candidates.remove(0);
            let worst = result.last().map(|x| x.0).unwrap_or(f32::INFINITY);
            if result.len() >= ef && cd > worst {
                break;
            }
            if let Some(list) = self.links.get(layer).and_then(|l| l.get(c)) {
                for &nb in list {
                    if nb >= self.data.len() || !visited.insert(nb) {
                        continue;
                    }
                    let d = l2_sq(q, &self.data[nb]);
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

    /// Returns `(row_id, distance)` for the `k` approximate nearest, ascending.
    pub fn search(&self, q: &[f32], k: usize) -> Vec<(usize, f32)> {
        let Some(entry) = self.entry else {
            return vec![];
        };
        let mut cur = entry;
        let mut l = self.max_level as i64;
        while l > 0 {
            cur = self.search_layer_nearest(q, cur, l as usize);
            l -= 1;
        }
        let ef = self.ef_search.max(k);
        let mut res = self.search_layer(q, cur, ef, 0);
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
