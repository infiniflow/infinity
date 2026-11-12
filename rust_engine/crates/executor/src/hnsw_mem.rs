//! The reference's `HnswIndexInMem`: the HNSW memory segment holding the rows
//! written since the last dump (`src/storage/knn_index/knn_hnsw/hnsw_alg.cppm`).
//! It owns an [`HnswHandler`] and the row id of every added vector; the
//! executor merges it with every sealed `chunk_n.idx`.
//!
//! Multi-vector columns insert one graph vertex per token, all labelled with
//! the row id — the reference's `MemIndexInserterIter1<MultiVectorRef>`
//! (`hnsw_handler_impl.cpp:253`). `rows.len()` is therefore the token count,
//! not the row count, which is why [`HnswIndexInMem::labelled_rows`] tracks the
//! distinct rows separately.

use std::collections::HashSet;

use index::hnsw_handler::{HnswHandler, KnnSearchOption};
use index::vector::{HnswEncoding, HnswIndex, Metric};

/// Splits a stored cell into graph vertices: one `dims`-sized token per
/// sub-vector for a multi-vector column, one vertex for anything else.
///
/// A cell whose length is not a multiple of `dims` is not a multi-vector
/// payload and indexes as a single vertex — the write path and
/// `CREATE INDEX`'s rebuild have to agree on that, or a row indexed twice would
/// land twice in the graph.
pub(crate) fn split_tokens(v: &[f32], dims: usize) -> Vec<Vec<f32>> {
    if dims > 0 && !v.is_empty() && v.len().is_multiple_of(dims) {
        v.chunks_exact(dims).map(<[f32]>::to_vec).collect()
    } else {
        vec![v.to_vec()]
    }
}

pub(crate) struct HnswIndexInMem {
    pub dims: usize,
    handler: HnswHandler,
    /// Row id per added vector (the graph's labels).
    rows: Vec<u64>,
    /// Distinct row ids, maintained only for multi-vector columns where one row
    /// contributes several vertices.
    distinct_rows: Option<HashSet<u64>>,
}

impl HnswIndexInMem {
    /// `InitAbstractIndexT`: the memory segment uses the index's encoding and
    /// metric (plain / LVQ / RaBitQ, l2 / ip / cosine).
    pub fn new_with(
        dims: usize,
        m: usize,
        ef_construction: usize,
        metric: Metric,
        encoding: HnswEncoding,
    ) -> Self {
        Self {
            dims,
            handler: HnswHandler::from_index(HnswIndex::with_encoding(
                dims,
                m,
                ef_construction,
                64,
                metric,
                encoding,
            )),
            rows: Vec::new(),
            distinct_rows: None,
        }
    }

    /// A memory segment for a multi-vector column: one vertex per token.
    pub fn new_multivector(
        dims: usize,
        m: usize,
        ef_construction: usize,
        metric: Metric,
        encoding: HnswEncoding,
    ) -> Self {
        let mut segment = Self::new_with(dims, m, ef_construction, metric, encoding);
        segment.distinct_rows = Some(HashSet::new());
        segment
    }

    pub fn is_multivector(&self) -> bool {
        self.distinct_rows.is_some()
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
        if let Some(distinct) = &mut self.distinct_rows {
            distinct.insert(seq);
        }
        true
    }

    /// `InsertVecs` over a whole batch: the reference stores the batch and
    /// builds it in buckets on the dense-index pool.
    pub fn insert_batch(&mut self, rows: &[(u64, Vec<f32>)], threads: usize) -> bool {
        let batch: Vec<Vec<f32>> = rows
            .iter()
            .filter(|(_, v)| v.len() == self.dims)
            .map(|(_, v)| v.clone())
            .collect();
        if batch.len() != rows.len() {
            return false;
        }
        if self.handler.insert_vecs_threaded(batch, threads) != rows.len() {
            return false;
        }
        self.rows.extend(rows.iter().map(|(seq, _)| *seq));
        if let Some(distinct) = &mut self.distinct_rows {
            distinct.extend(rows.iter().map(|(seq, _)| *seq));
        }
        true
    }

    /// `InsertVecs` over a whole batch of multi-vector rows: every token of
    /// every row becomes one vertex labelled with its row id, and the flat
    /// token list is built in one call, exactly like the reference's
    /// `MemIndexInserterIter1<MultiVectorRef<DataType>>` yields one
    /// `(token, label)` pair per token (`hnsw_handler_impl.cpp:252-256`).
    ///
    /// Returns `false` when any token does not match the segment's dimension;
    /// nothing is inserted then, so a half-built batch cannot desynchronise
    /// `rows` from the graph.
    pub fn insert_token_batch(&mut self, rows: &[(u64, Vec<Vec<f32>>)], threads: usize) -> bool {
        let mut batch: Vec<Vec<f32>> = Vec::new();
        let mut labels: Vec<u64> = Vec::new();
        for (seq, tokens) in rows {
            for token in tokens {
                if token.len() != self.dims {
                    return false;
                }
                batch.push(token.clone());
                labels.push(*seq);
            }
        }
        if batch.is_empty() {
            return false;
        }
        if self.handler.insert_vecs_threaded(batch, threads) != labels.len() {
            return false;
        }
        self.rows.extend(labels.iter().copied());
        if let Some(distinct) = &mut self.distinct_rows {
            distinct.extend(labels);
        }
        true
    }

    /// One multi-vector row: every token becomes a vertex labelled `seq`, and
    /// they are built as one batch (the reference's `InsertVecs` over a
    /// `MultiVectorRef`). Returns how many tokens were inserted.
    pub fn insert_tokens(&mut self, seq: u64, tokens: &[Vec<f32>], threads: usize) -> usize {
        let batch: Vec<Vec<f32>> = tokens
            .iter()
            .filter(|v| v.len() == self.dims)
            .cloned()
            .collect();
        if batch.is_empty() {
            return 0;
        }
        let inserted = self.handler.insert_vecs_threaded(batch, threads);
        if inserted == 0 {
            return 0;
        }
        self.rows.extend(std::iter::repeat_n(seq, inserted));
        if let Some(distinct) = &mut self.distinct_rows {
            distinct.insert(seq);
        }
        inserted
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

    /// `SearchIndex(..., bitmask)`: the filter is pushed into the graph walk,
    /// so `k` rows are returned even when the filter rejects many vertices.
    pub fn search_with_ef_filtered(
        &self,
        query: &[f32],
        k: usize,
        ef: Option<usize>,
        allow: &dyn Fn(u64) -> bool,
    ) -> Vec<(u64, f32)> {
        if query.len() != self.dims {
            return Vec::new();
        }
        let rows = &self.rows;
        let accept = |node: usize| rows.get(node).is_some_and(|row| allow(*row));
        self.handler
            .search_index_filtered(query, k, KnnSearchOption { ef }, &accept)
            .into_iter()
            .filter_map(|(node, score)| self.rows.get(node).map(|row| (*row, score)))
            .collect()
    }

    /// Multi-vector `KnnSearch`: per row, the best distance over its tokens.
    pub fn search_multivector(
        &self,
        queries: &[Vec<f32>],
        k: usize,
        ef: Option<usize>,
        allow: Option<&dyn Fn(u64) -> bool>,
    ) -> Vec<(u64, f32)> {
        if queries.iter().any(|q| q.len() != self.dims) {
            return Vec::new();
        }
        let rows = &self.rows;
        let row_of = |node: usize| rows.get(node).copied().unwrap_or(u64::MAX);
        // The filter is pushed into the graph walk (the reference's
        // `MultiVectorResultHandler` also ignores rejected labels), so `k`
        // accepted rows come back even when the predicate is selective.
        self.handler
            .search_multivector(queries, k, KnnSearchOption { ef }, &row_of, allow)
    }

    /// The graph, for dumping the segment as a C++ HNSW chunk.
    pub fn index(&self) -> &index::vector::HnswIndex {
        self.handler.index()
    }

    /// The row id of every node, in insertion order (the chunk's labels).
    pub fn rows(&self) -> &[u64] {
        &self.rows
    }

    /// Distinct rows the graph labels — the coverage check's numerator.
    pub fn labelled_rows(&self) -> usize {
        match &self.distinct_rows {
            Some(distinct) => distinct.len(),
            None => self.rows.len(),
        }
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

#[cfg(test)]
mod tests {
    use super::*;

    /// A `MULTIVECTOR` cell splits into one token per sub-vector; anything else
    /// is one vertex, so a malformed cell cannot silently vanish.
    #[test]
    fn a_cell_splits_into_one_token_per_sub_vector() {
        assert_eq!(
            split_tokens(&[1.0, 2.0, 3.0, 4.0], 2),
            vec![vec![1.0, 2.0], vec![3.0, 4.0]]
        );
        assert_eq!(split_tokens(&[1.0, 2.0], 2), vec![vec![1.0, 2.0]]);
        // Not a multiple of the dimension: one vertex, as the write path does.
        assert_eq!(split_tokens(&[1.0, 2.0, 3.0], 2), vec![vec![1.0, 2.0, 3.0]]);
        // An all-zero dimension never divides.
        assert_eq!(split_tokens(&[1.0, 2.0], 0), vec![vec![1.0, 2.0]]);
    }

    /// `InsertVecs` over multi-vector rows: one vertex per token, every one
    /// labelled with its row id, and the coverage numerator counts *rows*.
    #[test]
    fn a_token_batch_labels_every_token_with_its_row() {
        let mut segment =
            HnswIndexInMem::new_multivector(2, 4, 16, Metric::L2, HnswEncoding::Plain);
        let rows = vec![
            (7u64, vec![vec![1.0, 0.0], vec![0.0, 1.0], vec![1.0, 1.0]]),
            (9, vec![vec![0.0, 1.0]]),
        ];
        assert!(segment.insert_token_batch(&rows, 1));
        assert_eq!(segment.len(), 4, "one vertex per token");
        assert_eq!(segment.rows(), &[7, 7, 7, 9]);
        assert_eq!(segment.labelled_rows(), 2, "two distinct rows");
    }

    /// A dimension mismatch rejects the whole batch, so `rows` can never get
    /// ahead of the graph.
    #[test]
    fn a_bad_token_rejects_the_whole_batch() {
        let mut segment =
            HnswIndexInMem::new_multivector(2, 4, 16, Metric::L2, HnswEncoding::Plain);
        let rows = vec![(7u64, vec![vec![1.0, 0.0], vec![1.0]])];
        assert!(!segment.insert_token_batch(&rows, 1));
        assert_eq!(segment.len(), 0);
        assert_eq!(segment.labelled_rows(), 0);
    }
}
