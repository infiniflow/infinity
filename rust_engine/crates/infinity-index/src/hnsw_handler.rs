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

    /// C++: `HnswHandler::GetInfo` — `(mem_usage, row_count)`.
    pub fn get_info(&self) -> (usize, usize) {
        (self.mem_usage(), self.len())
    }

    /// C++: `HnswHandler::GetSizeInBytes` — the graph's serialized size.
    pub fn size_in_bytes(&self) -> usize {
        self.index.size_in_bytes()
    }

    /// C++: `HnswHandler::CompressToLVQ` — re-encode the stored vectors,
    /// keeping the graph and labels.
    pub fn compress_to_lvq(&mut self) {
        self.index.compress_to_lvq();
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
        assert!(handler.mem_usage() > 0);
    }
}

#[cfg(test)]
mod reference_suite {
    //! Ports of `HnswHandlerTest.test_memory` / `test_compress`
    //! (`src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_handler_ut.cpp`):
    //! 2048 vectors of dim 16, M=8, ef_construction=200, ef_search=10, L2.
    //! The C++ fixture prints `GetInfo`/`GetSizeInBytes`; the port asserts
    //! them, and checks top-1 self-recall after a chunk save/load round trip.

    use super::*;
    use crate::vector::{HnswEncoding, HnswIndex, Metric};

    const DIM: usize = 16;
    const M: usize = 8;
    const EF_CONSTRUCTION: usize = 200;
    const EF_SEARCH: usize = 10;
    /// `max_chunk_n * chunk_size` from the fixture.
    const N: usize = 16 * 128;

    fn data() -> Vec<Vec<f32>> {
        // Uniform [0, 1), like C++'s `uniform_real_distribution<float>`.
        let mut state = 0x9E37_79B9_7F4A_7C15u64;
        let mut next = || {
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            state
        };
        (0..N)
            .map(|_| {
                (0..DIM)
                    .map(|_| ((next() >> 40) as f32) / ((1u64 << 24) as f32))
                    .collect()
            })
            .collect()
    }

    fn handler(data: &[Vec<f32>]) -> HnswHandler {
        let mut handler = HnswHandler::new(DIM, M, EF_CONSTRUCTION, EF_SEARCH);
        assert_eq!(handler.insert_vecs(data.iter().cloned()), data.len());
        handler
    }

    /// C++ `SearchHnswHandler`: top-1 self-recall with `ef_ = 10`.
    fn top1_self_recall(handler: &HnswHandler, data: &[Vec<f32>]) -> f32 {
        let mut correct = 0usize;
        for (i, query) in data.iter().enumerate() {
            let hits = handler.search_index(
                query,
                1,
                KnnSearchOption {
                    ef: Some(EF_SEARCH),
                },
            );
            if hits.first().is_some_and(|(id, _)| *id == i) {
                correct += 1;
            }
        }
        correct as f32 / data.len() as f32
    }

    /// The port's `SaveToPtr`/`LoadFromPtr`: rebuild from the chunk's
    /// meta/records and graph parts (same layout as
    /// `vector::reference_suite::chunk_roundtrip`).
    fn roundtrip(handler: &HnswHandler, encoding: HnswEncoding) -> HnswHandler {
        let index = handler.index();
        let n = index.len();
        let levels: Vec<usize> = (0..n).map(|i| index.level_of(i)).collect();
        let max_level = index.max_level();
        let mut links = vec![vec![Vec::new(); n]; max_level + 1];
        for node in 0..n {
            for (layer, nodes) in links.iter_mut().enumerate() {
                nodes[node] = index.neighbors(layer, node).to_vec();
            }
        }
        let rebuilt = HnswIndex::from_chunk_parts(
            DIM,
            M,
            EF_SEARCH,
            Metric::L2,
            encoding,
            &index.store_meta_bytes(),
            &index.store_records_bytes(),
            n,
            levels,
            links,
            index.entry_point(),
            max_level,
        )
        .expect("chunk parts");
        HnswHandler::from_index(rebuilt)
    }

    /// C++: `HnswHandlerTest.test_memory`.
    #[test]
    fn test_memory() {
        let data = data();
        let handler = handler(&data);

        let (mem_usage, vec_num) = handler.get_info();
        assert_eq!(vec_num, N);
        assert!(mem_usage > 0, "MemUsage must account the graph");
        assert!(
            handler.size_in_bytes() > 0,
            "GetSizeInBytes must account the serialized graph"
        );
        let recall = top1_self_recall(&handler, &data);
        assert!(recall >= 0.95, "plain top-1 recall {recall}");

        let loaded = roundtrip(&handler, HnswEncoding::Plain);
        assert_eq!(loaded.len(), N);
        assert_eq!(loaded.get_info().1, N);
        assert!(loaded.get_info().0 > 0);
        let recall = top1_self_recall(&loaded, &data);
        assert!(recall >= 0.95, "loaded plain top-1 recall {recall}");
    }

    /// C++: `HnswHandlerTest.test_compress`.
    #[test]
    fn test_compress() {
        let data = data();
        let mut handler = handler(&data);
        let plain_size = handler.size_in_bytes();

        handler.compress_to_lvq();
        assert_eq!(handler.index().encoding(), HnswEncoding::Lvq);
        // 64 bytes/vector plain -> 16 + dim = 32 bytes LVQ at dim 16.
        assert!(
            handler.size_in_bytes() < plain_size,
            "LVQ must shrink the serialized vectors: {} !< {plain_size}",
            handler.size_in_bytes()
        );
        let recall = top1_self_recall(&handler, &data);
        assert!(recall >= 0.95, "compressed top-1 recall {recall}");

        let loaded = roundtrip(&handler, HnswEncoding::Lvq);
        assert_eq!(loaded.index().encoding(), HnswEncoding::Lvq);
        assert!(loaded.size_in_bytes() < plain_size);
        let recall = top1_self_recall(&loaded, &data);
        assert!(recall >= 0.95, "loaded LVQ top-1 recall {recall}");
    }
}
