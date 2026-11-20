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

//! The reference's `HnswIndexInMem`: the HNSW memory segment holding the rows
//! written since the last dump (`src/storage/knn_index/knn_hnsw/hnsw_alg.cppm`).
//! It owns an [`HnswHandler`] and the row id of every added vector; the
//! executor merges it with every sealed `chunk_n.idx`.

use crate::blocks::index_chunk::HnswOptions;
use index::hnsw_handler::{HnswHandler, KnnSearchOption};
use index::lsg::LsgConfig;
use index::vector::HnswIndex;

pub(crate) struct HnswIndexInMem {
    pub dims: usize,
    /// The `ROW_ID` segment this graph covers.
    ///
    /// The reference keys `MemIndex` by `(index_id, segment)`
    /// (`idx_seg|{db}|{tbl}|{index_id}|{seg}|mem_index`): every segment owns a
    /// graph and a chunk of its own, and a query merges them. One graph per
    /// index cannot be split back into segments at dump time, which is why a
    /// multi-segment table had to rebuild per segment there.
    pub segment: u32,
    /// `HnswIndexInMem::begin_row_id_`: the first row this graph holds. A
    /// chunk's record names it, so an incremental dump of one segment starts
    /// where the previous chunk ended.
    ///
    /// Set when the graph is created (the segment's base) and advanced as rows
    /// arrive, so it always names the first row actually inserted.
    begin_row_id: u64,
    handler: HnswHandler,
    /// Row id per added vector (the graph's labels).
    rows: Vec<u64>,
    /// `IndexHnsw::lsg_config_`, handed to the store with the first batch.
    ///
    /// [`HnswIndex::train_store`] records it and does not activate the scaling,
    /// exactly as the reference does (see that method's `lsg` section): the
    /// reference builds an `HnswLSGBuilder` for `build_type=lsg` but never
    /// calls `InsertSampleVecs`/`InsertLSAvg`/`SetLSGParam` outside its unit
    /// tests, so its `LSGDistWrapper::avg_` stays null and queries rank by the
    /// raw distance.
    lsg: Option<LsgConfig>,
}

impl HnswIndexInMem {
    /// `InitAbstractIndexT`: the memory segment uses the index's encoding and
    /// metric (plain / LVQ / RaBitQ, l2 / ip / cosine), and its LSG config.
    pub fn new_with(dims: usize, options: &HnswOptions, segment: u32, begin_row_id: u64) -> Self {
        Self {
            dims,
            segment,
            begin_row_id,
            handler: HnswHandler::from_index(HnswIndex::with_encoding(
                dims,
                options.m,
                options.ef_construction,
                64,
                options.metric,
                options.encoding,
            )),
            rows: Vec::new(),
            lsg: options.lsg,
        }
    }

    /// `HnswIndexInMem::begin_row_id_`: the first row this graph holds.
    pub fn begin_row_id(&self) -> u64 {
        self.begin_row_id
    }

    /// `Add`: returns false when the vector does not match the segment's dims.
    ///
    /// Async like [`Self::insert_batch`]: the store's insert awaits its own
    /// bucket fan-out, so the caller's lock guard has to be one that may cross
    /// an await (`tokio::sync::Mutex`).
    pub async fn insert(&mut self, seq: u64, vector: &[f32]) -> bool {
        if vector.len() != self.dims {
            return false;
        }
        if self
            .handler
            .insert_vecs(std::iter::once(vector.to_vec()))
            .await
            != 1
        {
            return false;
        }
        if self.rows.is_empty() {
            self.begin_row_id = seq;
        }
        self.rows.push(seq);
        true
    }

    /// `InsertVecs` over a whole batch: the reference stores the batch and
    /// builds it in parallel buckets, one task each.
    ///
    /// **Async, taking the lock itself.** The bucket fan-out awaits
    /// `spawn_blocking`, so the graph has to be locked with a guard that may
    /// cross an await - which is `tokio::sync::Mutex`, not `std::sync::Mutex`.
    /// The lock lives here rather than in the caller so no guard outlives this
    /// call into code that cannot await.
    pub async fn insert_batch<V: AsRef<[f32]> + Sync>(
        graph: &std::sync::Arc<tokio::sync::Mutex<Self>>,
        rows: &[(u64, V)],
        threads: usize,
    ) -> bool {
        let mut this = graph.lock().await;
        // Borrowed, not copied: the batch is the caller's `rows` column, and a
        // 20k x 128 copy here (plus the one the caller used to make) was pure
        // overhead on the way to the store, which copies each vector once into
        // its code regardless.
        let mut batch: Vec<&[f32]> = Vec::with_capacity(rows.len());
        for (_, v) in rows {
            let v = v.as_ref();
            if v.len() != this.dims {
                return false;
            }
            batch.push(v);
        }
        this.train_from_first_batch(&batch);
        if this.handler.insert_vecs_threaded(batch, threads).await != rows.len() {
            return false;
        }
        if let Some((first, _)) = rows.first() {
            if this.rows.is_empty() {
                this.begin_row_id = *first;
            }
        }
        this.rows.extend(rows.iter().map(|(seq, _)| *seq));
        true
    }

    /// `IndexHnsw::InitAbstractIndexT`: the quantizer (LVQ's per-dimension mean,
    /// RaBitQ's centroid) and the LSG averages are trained *before* any row is
    /// inserted. A code written with an untrained (all-zero) mean carries none
    /// of the vector's magnitude, and an untrained LSG leaves every row scaled
    /// by zero.
    ///
    /// Only from the first non-empty batch, and only while the store is still
    /// empty: re-training after rows exist would invalidate their codes - which
    /// is why a caller that feeds the rows in several batches trains from the
    /// **first block**, not from the whole column. The mean is not part of the
    /// chunk (`lvq_mean` has no writer), so this changes the graph's build
    /// distances, not the file.
    fn train_from_first_batch<V: AsRef<[f32]> + Sync>(&mut self, batch: &[V]) {
        if batch.is_empty() || !self.handler.is_empty() {
            return;
        }
        let lsg = self.lsg;
        self.handler.index_mut().train_store(batch, lsg);
    }

    /// `Search` with `WITH (ef = ...)` honoured, as the sealed segments do.
    pub fn search_with_ef(&self, query: &[f32], k: usize, ef: Option<usize>) -> Vec<(u64, f32)> {
        if query.len() != self.dims {
            return Vec::new();
        }
        self.handler
            .search_index(query, k, KnnSearchOption { ef })
            .into_iter()
            .filter_map(|(node, score)| self.rows.get(node).map(|row| (*row, score)))
            .collect()
    }

    /// [`Self::search_with_ef`] with the caller's filter: `allow` names the
    /// **row ids** the statement admits, and a rejected vertex is still
    /// traversed. This is the memory segment's half of the filter push-down
    /// (`docs/SPANN_DESIGN.md` §12.5) - the sealed chunks go through
    /// [`HnswHandler::search_index_filtered`].
    pub fn search_filtered_with_ef(
        &self,
        query: &[f32],
        k: usize,
        ef: Option<usize>,
        allow: &dyn Fn(u64) -> bool,
    ) -> Vec<(u64, f32)> {
        if query.len() != self.dims {
            return Vec::new();
        }
        let ef = ef.unwrap_or_else(|| self.handler.index().ef_search());
        self.handler
            .search_index_filtered(query, k, KnnSearchOption { ef: Some(ef) }, &|node| {
                self.rows.get(node).is_some_and(|row| allow(*row))
            })
            .into_iter()
            .filter_map(|(node, score)| self.rows.get(node).map(|row| (*row, score)))
            .collect()
    }

    /// The tail as the maintenance layer sees it (`SPANN_DESIGN.md` §12.4).
    ///
    /// The byte figure is the graph's own (`mem_usage`), **not** the charge the
    /// write path accumulates in `index_pending_bytes`: a graph built by
    /// `CREATE INDEX` charges nothing there, so it used to look empty to the
    /// quota and could never be released however many rows it held.
    pub(crate) fn maintainer(&self) -> HnswMaintainer<'_> {
        HnswMaintainer { tail: self }
    }

    /// The graph, for dumping the segment as a C++ HNSW chunk.
    pub fn index(&self) -> &index::vector::HnswIndex {
        self.handler.index()
    }

    /// The row id of every node, in insertion order (the chunk's labels).
    pub fn rows(&self) -> &[u64] {
        &self.rows
    }

    /// C++: `HnswIndexInMem::MemUsage` (the handler's graph).
    #[allow(dead_code)]
    pub fn mem_usage(&self) -> usize {
        self.handler.mem_usage() + self.rows.capacity() * 8
    }

    #[allow(dead_code)]
    pub fn len(&self) -> usize {
        self.rows.len()
    }

    #[allow(dead_code)]
    pub fn is_empty(&self) -> bool {
        self.rows.is_empty()
    }
}

/// A borrow of the in-memory HNSW segment, as the maintenance layer sees it.
///
/// A separate type rather than an impl on `HnswIndexInMem` because the segment is
/// always behind the executor's `Arc<Mutex<…>>`; handing out a borrow keeps the
/// trait off the lock and lets the caller read the numbers while holding it.
pub(crate) struct HnswMaintainer<'a> {
    pub(crate) tail: &'a HnswIndexInMem,
}

impl index::maintain::IndexMaintainer for HnswMaintainer<'_> {
    fn kind(&self) -> &'static str {
        "HNSW"
    }

    fn pending_bytes(&self) -> usize {
        self.tail.mem_usage()
    }

    /// The segment *is* the tail: sealing it writes a chunk and drops the graph,
    /// so nothing of it stays on the heap.
    fn resident_bytes(&self) -> usize {
        0
    }

    /// Serialising the graph is the cheap seal: no rebuild, no k-means.
    fn optimize_cost(&self) -> f32 {
        0.25
    }

    fn can_degrade(&self) -> bool {
        true
    }
}
