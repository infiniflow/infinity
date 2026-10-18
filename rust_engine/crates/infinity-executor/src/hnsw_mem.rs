//! The reference's `HnswIndexInMem`: the HNSW memory segment holding the rows
//! written since the last dump (`src/storage/knn_index/knn_hnsw/hnsw_alg.cppm`).
//! It owns an [`HnswHandler`] and the row id of every added vector; the
//! executor merges it with every sealed `chunk_n.idx`.

use infinity_index::hnsw_handler::{HnswHandler, KnnSearchOption};

pub(crate) struct HnswIndexInMem {
    pub dims: usize,
    handler: HnswHandler,
    /// Row id per added vector (the graph's labels).
    rows: Vec<u64>,
}

impl HnswIndexInMem {
    pub fn new(dims: usize, m: usize, ef_construction: usize) -> Self {
        Self {
            dims,
            handler: HnswHandler::new(dims, m, ef_construction, 64),
            rows: Vec::new(),
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

    /// `Search`: `(row id, distance)` for the nearest nodes in this segment.
    pub fn search(&self, query: &[f32], k: usize) -> Vec<(u64, f32)> {
        if query.len() != self.dims {
            return Vec::new();
        }
        self.handler
            .search_index(query, k, KnnSearchOption::default())
            .into_iter()
            .filter_map(|(node, score)| self.rows.get(node).map(|row| (*row, score)))
            .collect()
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
