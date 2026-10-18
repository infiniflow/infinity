//! The reference's `HnswIndexInMem`: the HNSW graph holding the rows written
//! since the last dump (`src/storage/knn_index/knn_hnsw/hnsw_alg.cppm`).
//! The executor merges this segment with every sealed `chunk_n.idx`.

use infinity_index::vector::HnswIndex;

pub(crate) struct HnswIndexInMem {
    pub dims: usize,
    index: HnswIndex,
    /// Row id per added vector (the graph's labels).
    rows: Vec<u64>,
}

impl HnswIndexInMem {
    pub fn new(dims: usize, m: usize, ef_construction: usize) -> Self {
        Self {
            dims,
            index: HnswIndex::new(dims, m, ef_construction, 64),
            rows: Vec::new(),
        }
    }

    /// `Add`: returns false when the vector does not match the segment's dims.
    pub fn insert(&mut self, seq: u64, vector: &[f32]) -> bool {
        if vector.len() != self.dims {
            return false;
        }
        self.index.add(vector);
        self.rows.push(seq);
        true
    }

    /// `Search`: `(row id, distance)` for the nearest nodes in this segment.
    pub fn search(&self, query: &[f32], k: usize) -> Vec<(u64, f32)> {
        if query.len() != self.dims {
            return Vec::new();
        }
        self.index
            .search(query, k)
            .into_iter()
            .filter_map(|(node, score)| self.rows.get(node).map(|row| (*row, score)))
            .collect()
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
