//! Where the builder reads its vectors from.
//!
//! The first version took `&[(u64, Vec<f32>)]`, which forces every vector into
//! RAM before the tree can start: at 1536d that is 6 KB per row, i.e. 6 GB for
//! a 1M-row build and 300 GB at the scale spann exists for. Building only ever
//! needs *random read* of one vector at a time (k-means centroids, split
//! assignment, encoding), so the builder takes this trait instead and the caller
//! decides what is resident:
//!
//! * [`Memory`] - the in-process form, used by tests and by `CREATE INDEX` on
//!   small tables;
//! * `storage::knn_index::spann::MappedSource` - the production form, an
//!   `f32` scratch file the builder maps, so peak RSS is the tree's codes plus
//!   one page-cache window rather than the whole column.
//!
//! `Send + Sync` because `build_tree` fans subtrees out to worker threads.

/// Random read access to the vectors being indexed.
pub trait VectorSource: Send + Sync {
    /// Number of rows.
    fn len(&self) -> usize;

    /// Row id the index will store for position `index`.
    fn id(&self, index: usize) -> u64;

    /// Vector at position `index`; must be `dims()` long.
    fn vector(&self, index: usize) -> &[f32];

    fn is_empty(&self) -> bool {
        self.len() == 0
    }
}

/// Vectors already in memory, flattened into one allocation.
///
/// Flattening matters: `Vec<Vec<f32>>` pays a `Vec` header and an allocator
/// block per row (48 B of slack at 1536d, ~5% of the payload, and 50M
/// allocations at 50M rows).
pub struct Memory {
    ids: Vec<u64>,
    /// `offsets[i]..offsets[i + 1]` is row `i`; `len() + 1` entries.
    ///
    /// An offset table rather than a uniform `dim` so a ragged column reaches
    /// the builder as-is: it is the builder's job to report
    /// `DimensionMismatch`, and a source that silently assumed the first row's
    /// width would panic on the rows that disagree.
    offsets: Vec<usize>,
    data: Vec<f32>,
}

impl Memory {
    /// Flattens `(row id, vector)` pairs. Each row is copied and dropped as it
    /// goes, so the peak is one row above the flat buffer.
    pub fn new(rows: Vec<(u64, Vec<f32>)>) -> Self {
        let mut ids = Vec::with_capacity(rows.len());
        let mut offsets = Vec::with_capacity(rows.len() + 1);
        let mut data: Vec<f32> = Vec::new();
        offsets.push(0);
        for (id, v) in rows {
            ids.push(id);
            data.extend_from_slice(&v);
            offsets.push(data.len());
        }
        Self { ids, offsets, data }
    }

    /// From pieces that are already flat and uniform (`storage` uses this after
    /// mapping its scratch file).
    pub fn from_parts(ids: Vec<u64>, dim: usize, data: Vec<f32>) -> Self {
        let offsets: Vec<usize> = (0..=ids.len()).map(|i| i * dim).collect();
        debug_assert_eq!(data.len(), ids.len() * dim);
        Self { ids, offsets, data }
    }

    /// The first row's width; ragged rows are reported by the builder.
    pub fn dims(&self) -> usize {
        self.offsets.get(1).copied().unwrap_or(0)
    }
}

impl VectorSource for Memory {
    fn len(&self) -> usize {
        self.ids.len()
    }

    fn id(&self, index: usize) -> u64 {
        self.ids[index]
    }

    fn vector(&self, index: usize) -> &[f32] {
        &self.data[self.offsets[index]..self.offsets[index + 1]]
    }
}

/// A [`crate::spann::incr::RowSource`] over any [`VectorSource`].
///
/// The builder reads **positionally** (`id(i)`, `vector(i)`); a split of an
/// incremental leaf knows its rows only by **id**, because that is all a leaf
/// stores beside the codes. This bridges the two once: the returned closure
/// owns a sorted `id → position` table, so a split costs one binary search per
/// row instead of a scan of the source, and the caller builds it a single time
/// and hands the same closure to every insert.
///
/// The vectors are copied out. A split takes `max_partition_size` of them
/// (128 at the design's default), which is one leaf's worth, not the source's;
/// lending instead would need the source to outlive the borrow for the whole
/// `insert`, and the production sources map blocks whose `&[f32]` is shorter
/// than the leaf's lifetime anyway.
pub fn row_source<'a>(source: &'a dyn VectorSource) -> impl Fn(u64) -> Option<Vec<f32>> + 'a {
    let mut table: Vec<(u64, usize)> = (0..source.len()).map(|i| (source.id(i), i)).collect();
    table.sort_unstable_by_key(|(id, _)| *id);
    move |id| {
        table
            .binary_search_by_key(&id, |(row, _)| *row)
            .ok()
            .map(|at| source.vector(table[at].1).to_vec())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn flattens_and_answers_by_position() {
        let src = Memory::new(vec![(7u64, vec![1.0, 2.0]), (9, vec![3.0, 4.0])]);
        assert_eq!(src.len(), 2);
        assert_eq!(src.id(0), 7);
        assert_eq!(src.vector(1), &[3.0, 4.0]);
        assert_eq!(src.dims(), 2);
    }

    #[test]
    fn row_source_answers_by_id_whatever_the_order() {
        let src = Memory::new(vec![(9u64, vec![3.0, 4.0]), (7, vec![1.0, 2.0])]);
        let rows = row_source(&src);
        assert_eq!(rows(7).as_deref(), Some(&[1.0, 2.0][..]));
        assert_eq!(rows(9).as_deref(), Some(&[3.0, 4.0][..]));
        assert!(rows(8).is_none(), "a row the source does not hold");
    }

    #[test]
    fn an_empty_source_is_empty() {
        let src = Memory::new(Vec::new());
        assert!(src.is_empty());
        assert_eq!(src.dims(), 0);
    }
}
