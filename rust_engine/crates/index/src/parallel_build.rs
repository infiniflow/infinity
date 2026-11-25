// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
enum QueryRef<'a> {
    Owned(crate::vector::HnswQuery<'a>),
    /// The query is the vertex's own stored row, already resolved.
    ///
    /// Holding the slice rather than the node id is the difference between one
    /// bounds check per `build()` and one per distance: a 1M-row build scores
    /// ~200 candidates per vertex, and each of those used to re-derive the
    /// row's offset (`i * dims`) and re-check the slice.
    Row {
        row: &'a [f32],
        metric: crate::vector::Metric,
    },
}

impl<'a> QueryRef<'a> {
    /// The query a build searches with, in the cheapest form the store allows.
    fn for_node(store: &'a crate::vector::Store, node: usize) -> QueryRef<'a> {
        match (store.plain_row(node), store.plain_metric()) {
            (Some(row), Some(metric)) => QueryRef::Row { row, metric },
            // A quantized store derives its query from the code, and a plain
            // store asked for a node past its end has nothing to search with;
            // `Owned` carries a default vector that scores as `f32::MAX` below.
            _ => QueryRef::Owned(store.make_query_at(node)),
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

    /// Runs `Build` for every vertex in `[start, end)` except `skip`.
    pub(crate) fn build_range(&self, start: usize, end: usize, skip: Option<usize>) {
        let mut scratch = SearchScratch::new();
        let mut select = SelectScratch::default();
        for node in start..end {
            if Some(node) == skip {
                continue;
            }
            self.build(node, &mut scratch, &mut select);
        }
    }

    /// Touches the rows that the distance loop is about to read, so their
    /// cache lines are in flight before it gets there.
    ///
    /// The reference does the same in its `SearchLayer`
    /// (`hnsw_alg.cppm:181`), keeping `prefetch_step_` rows ahead of the one
    /// being scored:
    ///
    /// ```cpp
    /// for (i32 i = 0; i < neighbor_size; ++i) {
    ///     for (size_t j = prefetch_step_; prefetch_start < neighbor_size && j > 0; --j) {
    ///         data_store_.PrefetchVec(neighbors_p[prefetch_start++]);
    ///     }
    ///     ... distance_(query, n_idx) ...
    /// }
    /// ```
    ///
    /// Measured on 300k x 128 rows without it: IPC 1.01 and a 60% LLC
    /// load-miss rate. The candidates are scattered over a 154 MB vector
    /// table, so each `vsubps -0xe0(%rax,%r10,1)` in the kernel stalled on a
    /// line nobody had asked for, and `perf` put 34% of the build on that one
    /// instruction. Bringing the line in a few iterations early overlaps the
    /// latency with the arithmetic instead of stalling on it.
    ///
    /// This reads the first element of each row rather than issuing
    /// `_mm_prefetch`: the crate forbids `unsafe`, and a real load through
    /// `black_box` cannot be elided, whereas a hint LLVM decides is dead
    /// would simply disappear.
    ///
    /// One line per row, not all eight. Touching every cache line of the
    /// 512-byte row measured **slower** (15.4-15.6 s against 14.4 s for one
    /// line on 300k): eight loads per candidate cost more than the misses they
    /// hide, and the hardware prefetcher already follows the stride within a
    /// row once the first line is in. The reference's `PrefetchVec` likewise
    /// touches one line (`cache_prefetch`, one hint per vector).
    #[inline(always)]
    fn prefetch_rows(&self, nodes: &[usize], from: usize, count: usize) {
        for &node in nodes.iter().take(from + count).skip(from) {
            if let Some(row) = self.store.plain_row(node) {
                // `black_box` keeps the load alive without letting the value
                // reach the distance loop's own expression.
                let _ = std::hint::black_box(row.first().copied());
            }
        }
    }

    fn adjust(&self, d: f32, a: Option<usize>, b: usize) -> f32 {
        match (a, self.lsg) {
            (Some(a), Some(lsg)) => lsg.adjust(d, a, b),
            _ => d,
        }
    }

    fn distance_to(&self, q: &QueryRef<'_>, self_id: Option<usize>, i: usize) -> f32 {
        let d = match q {
            QueryRef::Owned(q) => self.store.distance_query(q, i),
            QueryRef::Row { row, metric } => match self.store.plain_row(i) {
                Some(other) => crate::vector::Store::distance_rows(row, other, *metric),
                None => f32::MAX,
            },
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
            ep = self.search_layer_nearest(&query, ep, cur as usize, node);
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
        // Ties break on the vertex id in both directions, as they do in
        // `search_layer`. A quantized store makes equal distances common
        // (RaBitQ answers with a quantized estimate), and `sort_unstable` is
        // free to order equal elements however the input happened to be
        // laid out — which made the built graph vary between runs.
        if sorted.len() < m {
            sorted.sort_unstable_by(|a, b| b.0.total_cmp(&a.0).then(b.1.cmp(&a.1)));
            out.extend(sorted.iter().map(|(_, n)| *n));
            return out;
        }
        sorted.sort_unstable_by(|a, b| a.0.total_cmp(&b.0).then(a.1.cmp(&b.1)));
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
    ///
    /// `self_id` is the vertex being built, and it is the LSG scaling's left
    /// operand: `Build` passes `vertex_i` to every distance it takes
    /// (`hnsw_alg.cppm:401`), so a scaled graph descends in the scaled space on
    /// the upper layers too. Passing `None` here mixed raw and scaled
    /// distances inside one build.
    fn search_layer_nearest(
        &self,
        q: &QueryRef,
        start: usize,
        layer: usize,
        self_id: usize,
    ) -> usize {
        let mut cur = start;
        let mut cur_d = self.distance_to(q, Some(self_id), cur);
        loop {
            let nodes = self.store.len();
            let next = self.with_neighbors(layer, cur, |list| {
                let mut best = (cur, cur_d);
                // Farthest first, as the reference walks its list.
                for &nb in list.iter().rev() {
                    if nb >= nodes {
                        continue;
                    }
                    let d = self.distance_to(q, Some(self_id), nb);
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
            ..
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
                // Keep `PREFETCH_AHEAD` rows in flight, as the reference does
                // (`hnsw_alg.cppm:181`). Touching them up front is what stops
                // the kernel's first `vsubps` from stalling on a cold line.
                for (at, &nb) in list.iter().enumerate() {
                    if at.is_multiple_of(PREFETCH_AHEAD) {
                        self.prefetch_rows(list, at, PREFETCH_AHEAD);
                    }
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
        // Break ties by node id, exactly as the serial path does
        // (`HnswIndex::search_layer_filtered`). A quantized store makes ties
        // common: RaBitQ answers with a quantized estimate, so many candidates
        // land on the same `f32`. `result` is a `BinaryHeap`, whose internal
        // layout is not a function of its contents, so `drain()` hands the ties
        // over in an arbitrary order; without this `then` the candidate list -
        // and therefore the graph it builds - changed from build to build.
        out.sort_unstable_by(|a, b| a.0.total_cmp(&b.0).then(a.1.cmp(&b.1)));
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

/// How many candidate rows to keep in flight ahead of the distance loop.
///
/// The reference's `DEFAULT_PREFETCH_SIZE` (`default_values.cppm:181`). It is
/// the number of rows whose lines are touched before the next distance is
/// taken; four is enough to cover a load-to-L1 latency at this loop's rate.
const PREFETCH_AHEAD: usize = 4;

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

/// The buckets a batch build runs, skipping `prebuilt`.
///
/// `prebuilt` is the vertex `HnswIndex::add_batch` built on its own before the
/// buckets start (Phase A). Scheduling it again would run `Build` on it a
/// second time, and a rebuild replaces the vertex's neighbour list - so links
/// the first build established can disappear, and reverse pruning while
/// rebuilding it can drop other vertices' links to it.
///
/// `prebuilt` is marked with `skip` rather than cut out of the range, because a
/// bucket is a **contiguous** running range: splitting one would put two
/// adjacent ranges in flight, and their reverse-link writes are exactly what
/// makes a build order-dependent.
pub(crate) fn build_buckets(
    from: usize,
    to: usize,
    prebuilt: Option<usize>,
    threads: usize,
) -> Vec<(usize, usize, Option<usize>)> {
    buckets_of(from, to, threads)
        .into_iter()
        .map(|(start, end)| {
            let skip = prebuilt.filter(|p| (start..end).contains(p));
            (start, end, skip)
        })
        .collect()
}

/// Builds `[from, to)` on the calling thread.
pub(crate) fn build_serial(builder: &Builder<'_>, from: usize, to: usize, skip: Option<usize>) {
    builder.build_range(from, to, skip);
}

/// Orders two candidates the way `CompareMax` does, for tests.
#[allow(dead_code)]
pub(crate) fn compare_max(a: Cand, b: Cand) -> Ordering {
    a.cmp(&b)
}
