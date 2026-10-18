//! The reference's HNSW handler: the search/insert entry point over a graph.
//!
//! A sealed chunk loads an [`HnswHandler`] from its `chunk_n.idx`; the memory
//! segment owns one as well (`HnswIndexInMem`). Callers insert a batch of
//! vectors (`InsertVecs`) and query through `SearchIndex`.
//!
//! C++: `src/storage/knn_index/knn_hnsw/hnsw_handler.cppm`.

use crate::vector::HnswIndex;

/// The subset of `KnnSearchOption` the port applies.
/// C++: `KnnSearchOption` (`knn_hnsw/hnsw_common.cppm`).
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub struct KnnSearchOption {
    /// `ef_`: candidate list size for the bottom layer.
    pub ef: Option<usize>,
}

/// C++: `HnswHandler`.
pub struct HnswHandler {
    index: HnswIndex,
}

impl HnswHandler {
    /// C++: `HnswHandler(index_base, column_def)` / `Make`.
    pub fn new(dims: usize, m: usize, ef_construction: usize, ef_search: usize) -> Self {
        Self {
            index: HnswIndex::new(dims, m, ef_construction, ef_search),
        }
    }

    /// C++: `InitAbstractIndex` / the read path — adopt an already built graph.
    pub fn from_index(index: HnswIndex) -> Self {
        Self { index }
    }

    /// C++: `InsertVecs(Iter, config)`.
    ///
    /// The reference builds in buckets across the dense-index thread pool; the
    /// port inserts in order (scheduler design change, plan §4.4 D4). Returns
    /// the number of vectors inserted; vectors with the wrong dimension are
    /// skipped.
    pub fn insert_vecs<I>(&mut self, vectors: I) -> usize
    where
        I: IntoIterator<Item = Vec<f32>>,
    {
        let dims = self.index.dims();
        let mut inserted = 0;
        for vector in vectors {
            if vector.len() != dims {
                continue;
            }
            self.index.add(&vector);
            inserted += 1;
        }
        inserted
    }

    /// C++: `SearchIndex(q, k, option)` without a filter.
    pub fn search_index(
        &self,
        query: &[f32],
        k: usize,
        option: KnnSearchOption,
    ) -> Vec<(usize, f32)> {
        match option.ef {
            Some(ef) => self.index.search_with_ef(query, k, ef),
            None => self.index.search(query, k),
        }
    }

    /// C++: `MemUsage`.
    pub fn mem_usage(&self) -> usize {
        self.index.mem_usage()
    }

    /// C++: `GetDocNum`.
    pub fn len(&self) -> usize {
        self.index.len()
    }

    pub fn is_empty(&self) -> bool {
        self.index.is_empty()
    }

    /// Vector dimension.
    pub fn dims(&self) -> usize {
        self.index.dims()
    }

    /// The wrapped graph (serialization, graph readers).
    pub fn index(&self) -> &HnswIndex {
        &self.index
    }

    /// Consumes the handler and returns the graph.
    pub fn into_index(self) -> HnswIndex {
        self.index
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn vectors(n: usize, dims: usize) -> Vec<Vec<f32>> {
        (0..n)
            .map(|i| (0..dims).map(|d| (i * dims + d) as f32).collect())
            .collect()
    }

    #[test]
    fn insert_vecs_counts_only_matching_dimensions() {
        let mut handler = HnswHandler::new(3, 8, 32, 32);
        let mut batch = vectors(10, 3);
        batch.push(vec![1.0, 2.0]); // wrong dimension
        assert_eq!(handler.insert_vecs(batch), 10);
        assert_eq!(handler.len(), 10);
        assert_eq!(handler.dims(), 3);
    }

    #[test]
    fn search_index_honours_the_ef_option() {
        let mut handler = HnswHandler::new(2, 4, 16, 4);
        handler.insert_vecs(vectors(64, 2));
        let query = vec![0.0f32, 0.0];
        let default = handler.search_index(&query, 3, KnnSearchOption::default());
        let wide = handler.search_index(&query, 3, KnnSearchOption { ef: Some(64) });
        assert_eq!(default.len(), 3);
        assert_eq!(wide.len(), 3);
        // The exact three nearest are known for this dense layout.
        let ids: Vec<usize> = wide.iter().map(|(id, _)| *id).collect();
        assert_eq!(ids, vec![0, 1, 2]);
        assert_eq!(handler.mem_usage() > 0, true);
    }
}
