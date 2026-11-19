//! Reading a build's vectors: [`VectorSource`] is generic
//! ([`crate::source`]); what stays here is the one **spann**-specific bridge.
//!
//! `VectorSource` and `Memory` moved to [`crate::source`] when the second
//! consumer appeared: nothing about "this column's vectors, by row id" is
//! spann's, and a trait that lives under `spann` reads as if it were.

pub use crate::source::{Memory, VectorSource};

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
    fn row_source_answers_by_id_whatever_the_order() {
        let src = Memory::new(vec![(9u64, vec![3.0, 4.0]), (7, vec![1.0, 2.0])]);
        let rows = row_source(&src);
        assert_eq!(rows(7).as_deref(), Some(&[1.0, 2.0][..]));
        assert_eq!(rows(9).as_deref(), Some(&[3.0, 4.0][..]));
        assert!(rows(8).is_none(), "a row the source does not hold");
    }
}
