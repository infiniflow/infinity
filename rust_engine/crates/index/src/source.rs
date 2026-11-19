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

//! Where a builder reads its vectors from.
//!
//! The first version of this took `&[(u64, Vec<f32>)]`, which forces every
//! vector into RAM before the tree can start: at 1536d that is 6 KB per row,
//! i.e. 6 GB for a 1M-row build and 300 GB at the scale the vector indexes
//! exist for. A build only ever needs a **random read of one vector at a time**
//! (k-means centroids, split assignment, encoding), so the builder takes this
//! trait and the caller decides what is resident.
//!
//! Nothing here belongs to one index type. [`Memory`] is the in-process form
//! (tests, small tables, and a build whose rows are still in the append buffer);
//! `executor::blocks::VectorColumn` is the production form, which borrows each
//! vector out of the block file that already holds it. `spann` is the first
//! consumer, not the owner.
//!
//! `Send + Sync` because a build fans subtrees out to worker threads.

/// Random read access to the vectors being indexed.
pub trait VectorSource: Send + Sync {
    /// Number of rows.
    fn len(&self) -> usize;

    /// Row id the index will store for position `index`.
    fn id(&self, index: usize) -> u64;

    /// Vector at position `index`; must be `dims()` long.
    ///
    /// `Cow`, not `&[f32]`: a column read where it already is borrows (the
    /// common case - an `f32` embedding body is the array the builder wants),
    /// while a body that is not `f32` (`FLOAT16`/`BFLOAT16`/`DOUBLE`/`BIT`, or a
    /// row that starts off-alignment) has to be widened, and a builder that
    /// keeps a whole leaf's vectors alive would otherwise force the source to
    /// hold every widened block it ever answered for.
    fn vector(&self, index: usize) -> std::borrow::Cow<'_, [f32]>;

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

    /// From pieces that are already flat and uniform.
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

    fn vector(&self, index: usize) -> std::borrow::Cow<'_, [f32]> {
        std::borrow::Cow::Borrowed(&self.data[self.offsets[index]..self.offsets[index + 1]])
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
        assert_eq!(src.vector(1).as_ref(), &[3.0, 4.0]);
        assert_eq!(src.dims(), 2);
    }

    #[test]
    fn an_empty_source_is_empty() {
        let src = Memory::new(Vec::new());
        assert!(src.is_empty());
        assert_eq!(src.dims(), 0);
    }
}
