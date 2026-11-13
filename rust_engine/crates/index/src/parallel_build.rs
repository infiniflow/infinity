//! Bucket-parallel graph construction, the reference's `HnswHandler::InsertVecs`.
//!
//! `InsertVecs` stores the whole batch first, then splits `[start, end)` into
//! buckets of at least `kBuildBucketSize` vertices and builds one `Build(j)`
//! loop per bucket in parallel. This module is that path: `HnswIndex::add_batch`
//! stores the vectors, splits the range with [`buckets_of`], and builds each
//! bucket with [`build_serial`] on its own tokio task.
//!
//! Safety comes from one lock per vertex, as in the reference's
//! `DataStore::UniqueLock(vertex)`: a thread writes only the vertex it is
//! building plus the reverse links of neighbours it already found, each under
//! that neighbour's write lock. `Build` only ever links to vertices with a
//! lower index (its search results are the ones already inserted), so the wait
//! graph is acyclic and there is no lock-order deadlock.

use std::cmp::{Ordering, Reverse};
use std::sync::RwLock;

use crate::lsg::LsgIndex;
use crate::vector::{Cand, SearchScratch, Store};

/// The graph with one lock per vertex, the shape a concurrent build needs.
pub(crate) type LockedLinks = Vec<Vec<RwLock<Vec<usize>>>>;

/// The query a build searches with.
///
/// A vertex is searched for by its own stored vector, so the graph's own copy
/// can stand in for it. Stores that derive a query from a code (LVQ, RaBitQ)
/// keep the owned form, because their query is not the stored bytes.
enum QueryRef {
    Owned(crate::vector::HnswQuery),
    Node(usize),
}

impl QueryRef {
    /// The query a build searches with, in the cheapest form the store allows.
    fn for_node(store: &crate::vector::Store, node: usize) -> QueryRef {
        if store.queries_are_stored_vectors() {
            QueryRef::Node(node)
        } else {
            QueryRef::Owned(store.make_query_at(node))
        }
    }
}

/// The working set of `SelectNeighborsHeuristic`.
#[derive(Default)]
pub(crate) struct SelectScratch {
    sorted: Vec<(f32, usize)>,
    kept: Vec<(f32, usize)>,
    /// The selected neighbours, farthest first (the reference writes them
    /// straight into the vertex's list; this is the buffer it writes from).
    out: Vec<usize>,
}

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

/// The state a bucket build shares: everything [`Builder`] borrows, owned so a
/// task can hold it across an `.await`.
///
/// A bucket only reads the store and the levels, and writes the graph under the
/// per-vertex locks, so sharing these by `Arc` is exactly the reference's
/// `DataStore::UniqueLock` shape.
pub(crate) struct SharedBuild {
    pub(crate) store: std::sync::Arc<Store>,
    pub(crate) lsg: Option<std::sync::Arc<LsgIndex>>,
    pub(crate) levels: std::sync::Arc<Vec<usize>>,
    pub(crate) links: std::sync::Arc<LockedLinks>,
    pub(crate) m: usize,
    pub(crate) ef_construction: usize,
    pub(crate) entry: usize,
    pub(crate) max_level: usize,
}

impl SharedBuild {
    pub(crate) fn builder(&self) -> Builder<'_> {
        Builder {
            store: &self.store,
            lsg: self.lsg.as_deref(),
            m: self.m,
            ef_construction: self.ef_construction,
            levels: &self.levels,
            links: &self.links,
            entry: self.entry,
            max_level: self.max_level,
        }
    }
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
        let mut select = SelectScratch::default();
        for node in start..end {
            self.build(node, &mut scratch, &mut select);
        }
    }

    fn adjust(&self, d: f32, a: Option<usize>, b: usize) -> f32 {
        match (a, self.lsg) {
            (Some(a), Some(lsg)) => lsg.adjust(d, a, b),
            _ => d,
        }
    }

    fn distance_to(&self, q: &QueryRef, self_id: Option<usize>, i: usize) -> f32 {
        let d = match q {
            QueryRef::Owned(q) => self.store.distance_query(q, i),
            // The query of a build is the vertex's own stored vector, so
            // scoring against it is a node-to-node distance. Copying it into
            // an `HnswQuery` first costs one allocation per vertex built.
            QueryRef::Node(node) => self.store.distance_nodes(*node, i),
        };
        self.adjust(d, self_id, i)
    }

    fn distance_nodes(&self, a: usize, b: usize) -> f32 {
        let d = self.store.distance_nodes(a, b);
        self.adjust(d, Some(a), b)
    }

    /// `GetNeighbors` with the read lock held for the span of `f`, which is
    /// what the reference does (`shared_lock` around its neighbour loop).
    ///
    /// Copying the list first cost a `memmove` per vertex expanded - 10% of a
    /// 200k build under `perf`. Holding the read lock is safe because the
    /// closure only reads memory and computes distances: it takes no other
    /// lock, so a writer waiting for this vertex cannot block the holder.
    fn with_neighbors<R>(&self, layer: usize, node: usize, f: impl FnOnce(&[usize]) -> R) -> R {
        match self.links.get(layer).and_then(|l| l.get(node)) {
            Some(cell) => {
                let list = cell.read().unwrap_or_else(|e| e.into_inner());
                f(&list)
            }
            None => f(&[]),
        }
    }

    /// `Build(vertex_i)`: the vertex is already stored and levelled.
    fn build(&self, node: usize, scratch: &mut SearchScratch, select: &mut SelectScratch) {
        let q_layer = self.levels[node];
        let query = QueryRef::for_node(self.store, node);
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
            let neighbors = self.select_neighbors(&cands, self.m, select);
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
                own.extend_from_slice(neighbors);
            }
            // A reverse-link prune runs the heuristic again, which reuses
            // `select`'s buffers, so the loop walks its own copy of the list
            // (the slice borrows `select` and cannot outlive that call).
            let neighbors = neighbors.to_vec();
            for nb in neighbors {
                self.connect_neighbors(nb, node, layer, select);
            }
            l -= 1;
        }
    }

    /// `ConnectNeighbors`: append, or prune the list when it is at `Mmax`.
    fn connect_neighbors(
        &self,
        target: usize,
        node: usize,
        layer: usize,
        select: &mut SelectScratch,
    ) {
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
        let kept = self.select_neighbors(&cands, max_m, select);
        list.clear();
        list.extend(kept);
    }

    /// `SelectNeighborsHeuristic`: closer-than-a-kept-neighbour candidates are
    /// dropped and the list comes back farthest first.
    ///
    /// `scratch` carries the sort buffer and the kept list; a build runs this
    /// once per layer per vertex plus once per pruned reverse link, so two
    /// fresh `Vec`s per call were most of its allocations.
    fn select_neighbors<'s>(
        &self,
        cands: &[(f32, usize)],
        m: usize,
        scratch: &'s mut SelectScratch,
    ) -> &'s [usize] {
        let SelectScratch { sorted, kept, out } = scratch;
        sorted.clear();
        sorted.extend_from_slice(cands);
        out.clear();
        if sorted.len() < m {
            sorted.sort_unstable_by(|a, b| b.0.total_cmp(&a.0));
            out.extend(sorted.iter().map(|(_, n)| *n));
            return out;
        }
        sorted.sort_unstable_by(|a, b| a.0.total_cmp(&b.0));
        kept.clear();
        for &(d, node) in sorted.iter() {
            if kept.len() == m {
                break;
            }
            if !kept.iter().any(|(_, s)| self.distance_nodes(node, *s) < d) {
                kept.push((d, node));
            }
        }
        out.extend(kept.iter().rev().map(|(_, n)| *n));
        out
    }

    /// `SearchLayerNearest`: greedy descent on one upper layer.
    fn search_layer_nearest(&self, q: &QueryRef, start: usize, layer: usize) -> usize {
        let mut cur = start;
        let mut cur_d = self.distance_to(q, None, cur);
        loop {
            let nodes = self.store.len();
            let next = self.with_neighbors(layer, cur, |list| {
                let mut best = (cur, cur_d);
                // Farthest first, as the reference walks its list.
                for &nb in list.iter().rev() {
                    if nb >= nodes {
                        continue;
                    }
                    let d = self.distance_to(q, None, nb);
                    if d < best.1 {
                        best = (nb, d);
                    }
                }
                best
            });
            if next.0 == cur {
                return cur;
            }
            cur = next.0;
            cur_d = next.1;
        }
    }

    /// `SearchLayer`: the beam search that feeds `select_neighbors`.
    fn search_layer(
        &self,
        q: &QueryRef,
        ep: usize,
        ef: usize,
        layer: usize,
        self_id: usize,
        scratch: &mut SearchScratch,
    ) -> Vec<(f32, usize)> {
        let nodes = self.store.len();
        scratch.begin(nodes, ep);
        let ep_d = self.distance_to(q, Some(self_id), ep);
        // The heaps and the neighbour buffer belong to the scratch: this runs
        // once per vertex per layer, so a fresh allocation per call is the
        // build's dominant allocator traffic.
        let SearchScratch {
            visited,
            candidate,
            result,
        } = scratch;
        candidate.push(Reverse(Cand(ep_d, ep)));
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
            // The neighbour list is walked under its read lock instead of
            // being copied out: `memmove` of these lists was 10% of a 200k
            // build. The closure takes no other lock, so a writer waiting on
            // this vertex cannot block the holder.
            self.with_neighbors(layer, c, |list| {
                for &nb in list {
                    if nb >= nodes || nb == self_id || !visited.visit(nb) {
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
            });
        }
        let mut out: Vec<(f32, usize)> = result.drain().map(|c| (c.0, c.1)).collect();
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

/// Splits `[from, to)` into buckets of at least [`MIN_BUCKET_SIZE`].
pub(crate) fn buckets_of(from: usize, to: usize, threads: usize) -> Vec<(usize, usize)> {
    if to <= from {
        return Vec::new();
    }
    let n = to - from;
    let bucket = ((n - 1) / threads.max(1) + 1).max(MIN_BUCKET_SIZE);
    (from..to)
        .step_by(bucket)
        .map(|start| (start, start + bucket.min(to - start)))
        .collect()
}

/// Builds `[from, to)` on the calling thread.
pub(crate) fn build_serial(builder: &Builder<'_>, from: usize, to: usize) {
    builder.build_range(from, to);
}

/// Orders two candidates the way `CompareMax` does, for tests.
#[allow(dead_code)]
pub(crate) fn compare_max(a: Cand, b: Cand) -> Ordering {
    a.cmp(&b)
}
