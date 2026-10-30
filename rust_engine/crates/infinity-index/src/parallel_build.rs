//! Bucket-parallel graph construction, the reference's `HnswHandler::InsertVecs`.
//!
//! `InsertVecs` stores the whole batch first, then splits `[start, end)` into
//! buckets of at least `kBuildBucketSize` vertices and pushes one `Build(j)`
//! loop per bucket onto the dense-index thread pool. This module is that
//! path: `HnswIndex::add_batch` stores the vectors, hands the graph to
//! [`build_parallel`], and takes the links back.
//!
//! Safety comes from one lock per vertex, as in the reference's
//! `DataStore::UniqueLock(vertex)`: a thread writes only the vertex it is
//! building plus the reverse links of neighbours it already found, each under
//! that neighbour's write lock. `Build` only ever links to vertices with a
//! lower index (its search results are the ones already inserted), so the wait
//! graph is acyclic and there is no lock-order deadlock.

use std::cmp::{Ordering, Reverse};
use std::collections::BinaryHeap;
use std::sync::RwLock;

use crate::lsg::LsgIndex;
use crate::vector::{Cand, SearchScratch, Store};

/// The graph with one lock per vertex, the shape a concurrent build needs.
pub(crate) type LockedLinks = Vec<Vec<RwLock<Vec<usize>>>>;

/// What a bucket needs to build a vertex: the stored vectors, the levels the
/// caller already sampled, and the locked graph being filled in.
pub(crate) struct Builder<'a> {
    store: &'a Store,
    lsg: Option<&'a LsgIndex>,
    m: usize,
    ef_construction: usize,
    levels: &'a [usize],
    links: &'a LockedLinks,
    entry: usize,
    max_level: usize,
}

impl<'a> Builder<'a> {
    #[allow(clippy::too_many_arguments)]
    pub(crate) fn new(
        store: &'a Store,
        lsg: Option<&'a LsgIndex>,
        m: usize,
        ef_construction: usize,
        levels: &'a [usize],
        links: &'a LockedLinks,
        entry: usize,
        max_level: usize,
    ) -> Self {
        Self {
            store,
            lsg,
            m,
            ef_construction,
            levels,
            links,
            entry,
            max_level,
        }
    }

    /// Runs `Build` for every vertex in `[start, end)`.
    pub(crate) fn build_range(&self, start: usize, end: usize) {
        let mut scratch = SearchScratch::new();
        for node in start..end {
            self.build(node, &mut scratch);
        }
    }

    fn adjust(&self, d: f32, a: Option<usize>, b: usize) -> f32 {
        match (a, self.lsg) {
            (Some(a), Some(lsg)) => lsg.adjust(d, a, b),
            _ => d,
        }
    }

    fn distance_to(&self, q: &crate::vector::HnswQuery, self_id: Option<usize>, i: usize) -> f32 {
        let d = self.store.distance_query(q, i);
        self.adjust(d, self_id, i)
    }

    fn distance_nodes(&self, a: usize, b: usize) -> f32 {
        let d = self.store.distance_nodes(a, b);
        self.adjust(d, Some(a), b)
    }

    /// `GetNeighbors`: a copy, so the lock is released before the caller
    /// computes distances (the reference holds a `shared_lock` for the span of
    /// its neighbour loop; copying is cheaper than holding it across the
    /// distance calls because a writer only ever appends to its own vertex).
    fn neighbors(&self, layer: usize, node: usize) -> Vec<usize> {
        match self.links.get(layer).and_then(|l| l.get(node)) {
            Some(cell) => cell.read().unwrap_or_else(|e| e.into_inner()).clone(),
            None => Vec::new(),
        }
    }

    /// `Build(vertex_i)`: the vertex is already stored and levelled.
    fn build(&self, node: usize, scratch: &mut SearchScratch) {
        let q_layer = self.levels[node];
        let query = self.store.make_query_at(node);
        let mut ep = self.entry;
        let mut cur = self.max_level as i64;
        while cur > q_layer as i64 {
            ep = self.search_layer_nearest(&query, ep, cur as usize);
            cur -= 1;
        }
        let mut l = q_layer.min(self.max_level) as i64;
        while l >= 0 {
            let layer = l as usize;
            let cands = self.search_layer(&query, ep, self.ef_construction, layer, node, scratch);
            let neighbors = self.select_neighbors(&cands, self.m);
            if neighbors.is_empty() {
                l -= 1;
                continue;
            }
            ep = neighbors[0];
            {
                let mut own = self.links[layer][node]
                    .write()
                    .unwrap_or_else(|e| e.into_inner());
                own.clear();
                own.extend_from_slice(&neighbors);
            }
            for &nb in &neighbors {
                self.connect_neighbors(nb, node, layer);
            }
            l -= 1;
        }
    }

    /// `ConnectNeighbors`: append, or prune the list when it is at `Mmax`.
    fn connect_neighbors(&self, target: usize, node: usize, layer: usize) {
        let max_m = if layer == 0 { self.m * 2 } else { self.m };
        let mut list = self.links[layer][target]
            .write()
            .unwrap_or_else(|e| e.into_inner());
        if list.contains(&node) {
            // Already a neighbour. The serial path cannot see this (a vertex is
            // only ever linked once, by its own build), but in a batch the entry
            // vertex is built before its neighbours exist, so its list can
            // already hold the vertex whose build now looks at it.
            return;
        }
        if list.len() < max_m {
            list.push(node);
            return;
        }
        let mut cands: Vec<(f32, usize)> = Vec::with_capacity(list.len() + 1);
        cands.push((self.distance_nodes(target, node), node));
        for &nb in list.iter() {
            cands.push((self.distance_nodes(target, nb), nb));
        }
        let kept = self.select_neighbors(&cands, max_m);
        list.clear();
        list.extend(kept);
    }

    /// `SelectNeighborsHeuristic`: closer-than-a-kept-neighbour candidates are
    /// dropped and the list comes back farthest first.
    fn select_neighbors(&self, cands: &[(f32, usize)], m: usize) -> Vec<usize> {
        let mut sorted = cands.to_vec();
        if sorted.len() < m {
            sorted.sort_unstable_by(|a, b| b.0.total_cmp(&a.0));
            return sorted.into_iter().map(|(_, n)| n).collect();
        }
        sorted.sort_unstable_by(|a, b| a.0.total_cmp(&b.0));
        let mut kept: Vec<(f32, usize)> = Vec::with_capacity(m);
        for (d, node) in sorted {
            if kept.len() == m {
                break;
            }
            if !kept.iter().any(|(_, s)| self.distance_nodes(node, *s) < d) {
                kept.push((d, node));
            }
        }
        kept.reverse();
        kept.into_iter().map(|(_, n)| n).collect()
    }

    /// `SearchLayerNearest`: greedy descent on one upper layer.
    fn search_layer_nearest(
        &self,
        q: &crate::vector::HnswQuery,
        start: usize,
        layer: usize,
    ) -> usize {
        let mut cur = start;
        let mut cur_d = self.distance_to(q, None, cur);
        loop {
            let mut changed = false;
            for nb in self.neighbors(layer, cur).into_iter().rev() {
                if nb >= self.store.len() {
                    continue;
                }
                let d = self.distance_to(q, None, nb);
                if d < cur_d {
                    cur = nb;
                    cur_d = d;
                    changed = true;
                }
            }
            if !changed {
                return cur;
            }
        }
    }

    /// `SearchLayer`: the beam search that feeds `select_neighbors`.
    fn search_layer(
        &self,
        q: &crate::vector::HnswQuery,
        ep: usize,
        ef: usize,
        layer: usize,
        self_id: usize,
        scratch: &mut SearchScratch,
    ) -> Vec<(f32, usize)> {
        let nodes = self.store.len();
        scratch.begin(nodes, ep);
        let ep_d = self.distance_to(q, Some(self_id), ep);
        let mut candidate: BinaryHeap<Reverse<Cand>> = BinaryHeap::with_capacity(ef);
        candidate.push(Reverse(Cand(ep_d, ep)));
        let mut result: BinaryHeap<Cand> = BinaryHeap::with_capacity(ef);
        // The vertex being built must never enter its own result set. The
        // serial path is safe by construction (a vertex has no links until it
        // is built, so nothing points at it); in a batch the entry vertex is
        // built first and back-links into vertices that are still unbuilt, so
        // searching from it does reach the vertex currently being built.
        if ep != self_id {
            result.push(Cand(ep_d, ep));
        }
        while let Some(Reverse(Cand(cd, c))) = candidate.pop() {
            let worst = result.peek().map(|x| x.0).unwrap_or(f32::INFINITY);
            if result.len() >= ef && cd > worst {
                break;
            }
            for nb in self.neighbors(layer, c) {
                if nb >= nodes || nb == self_id || !scratch.visit(nb) {
                    continue;
                }
                let d = self.distance_to(q, Some(self_id), nb);
                let worst = result.peek().map(|x| x.0).unwrap_or(f32::INFINITY);
                if result.len() < ef || d <= worst {
                    candidate.push(Reverse(Cand(d, nb)));
                    if result.len() == ef {
                        if result.peek().is_some_and(|top| Cand(d, nb) < *top) {
                            result.pop();
                            result.push(Cand(d, nb));
                        }
                    } else {
                        result.push(Cand(d, nb));
                    }
                }
            }
        }
        let mut out: Vec<(f32, usize)> = result.into_iter().map(|c| (c.0, c.1)).collect();
        out.sort_unstable_by(|a, b| a.0.total_cmp(&b.0));
        out
    }
}

/// Splits `[from, to)` into `threads` buckets and builds them concurrently.
///
/// The bucket size follows the reference:
/// `max(kBuildBucketSize, (n - 1) / threads + 1)`, so a small batch still
/// builds in chunks of `kBuildBucketSize` instead of one vertex per thread.
/// `kBuildBucketSize` (`hnsw_handler.cppm:151`): the smallest bucket a batch is
/// split into. A batch of a few hundred vertices therefore builds in one
/// bucket — serially — exactly as the reference does, which also keeps a small
/// index's graph reproducible.
const MIN_BUCKET_SIZE: usize = 1024;

pub(crate) fn build_parallel_range(builder: &Builder<'_>, from: usize, to: usize, threads: usize) {
    if threads <= 1 || to <= from {
        builder.build_range(from, to);
        return;
    }
    let n = to - from;
    let bucket = ((n - 1) / threads + 1).max(MIN_BUCKET_SIZE);
    let buckets: Vec<(usize, usize)> = (from..to)
        .step_by(bucket)
        .map(|start| (start + bucket).min(to))
        .zip((from..to).step_by(bucket))
        .map(|(end, start)| (start, end))
        .collect();
    if buckets.len() <= 1 {
        builder.build_range(from, to);
        return;
    }
    std::thread::scope(|scope| {
        for (start, end) in buckets {
            scope.spawn(move || builder.build_range(start, end));
        }
    });
}

/// Orders two candidates the way `CompareMax` does, for tests.
#[allow(dead_code)]
pub(crate) fn compare_max(a: Cand, b: Cand) -> Ordering {
    a.cmp(&b)
}
