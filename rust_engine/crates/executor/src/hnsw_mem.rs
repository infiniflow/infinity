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
    pub fn new_with(dims: usize, options: &HnswOptions) -> Self {
        Self {
            dims,
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

    /// `Add`: returns false when the vector does not match the segment's dims.
    pub fn insert(&mut self, seq: u64, vector: &[f32]) -> bool {
        if vector.len() != self.dims {
            return false;
        }
        if self.handler.insert_vecs(std::iter::once(vector.to_vec())) != 1 {
            return false;
        }
        self.rows.push(seq);
        true
    }

    /// `InsertVecs` over a whole batch: the reference stores the batch and
    /// builds it in parallel buckets, one tokio task each.
    pub fn insert_batch<V: AsRef<[f32]> + Sync>(
        &mut self,
        rows: &[(u64, V)],
        threads: usize,
    ) -> bool {
        // Borrowed, not copied: the batch is the caller's `rows` column, and a
        // 20k x 128 copy here (plus the one the caller used to make) was pure
        // overhead on the way to the store, which copies each vector once into
        // its code regardless.
        let mut batch: Vec<&[f32]> = Vec::with_capacity(rows.len());
        for (_, v) in rows {
            let v = v.as_ref();
            if v.len() != self.dims {
                return false;
            }
            batch.push(v);
        }
        self.train_from_first_batch(&batch);
        if self.handler.insert_vecs_threaded(batch, threads) != rows.len() {
            return false;
        }
        self.rows.extend(rows.iter().map(|(seq, _)| *seq));
        true
    }

    /// `IndexHnsw::InitAbstractIndexT`: the quantizer (LVQ's per-dimension mean,
    /// RaBitQ's centroid) and the LSG averages are trained from the whole column
    /// *before* any row is inserted. A code written with an untrained (all-zero)
    /// mean carries none of the vector's magnitude, and an untrained LSG leaves
    /// every row scaled by zero.
    ///
    /// Only from the first non-empty batch, and only while the store is still
    /// empty: re-training after rows exist would invalidate their codes.
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
