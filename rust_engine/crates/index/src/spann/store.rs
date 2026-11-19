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

//! The read-side store abstraction (`SPANN_DESIGN.md` §7).
//!
//! The search algorithm only needs four things: the root key, the height, a
//! lookup by key, and the partitions themselves. `MemStore` is the in-memory
//! implementation used by the algorithm tests and by the build path; the chunk
//! store (`crates/storage/src/knn_index/spann`) implements the same trait over
//! an mmap, which is what makes §6.4's out-of-core claim testable without a
//! second copy of the algorithms.
//!
//! Mutation is deliberately absent: the tail lives in the executor as a
//! `PendingTail` of codes (§9.2) and never rewrites a built tree, so there is
//! no `SpannMutStore` yet.

use std::collections::HashMap;

use super::build::BuiltTree;
use super::partition::Partition;
use super::PartitionKey;

/// Read access to a built tree.
pub trait SpannStore {
    fn dims(&self) -> usize;
    fn root(&self) -> PartitionKey;
    fn height(&self) -> u32;
    fn partition(&self, key: PartitionKey) -> Option<&Partition>;
    fn partitions(&self) -> &[Partition];
    /// Number of rows the tree covers (subtree range, §3 I4).
    fn len(&self) -> usize;

    fn is_empty(&self) -> bool {
        self.len() == 0
    }
}

/// A tree held entirely in memory.
#[derive(Debug)]
pub struct MemStore {
    dims: usize,
    root: PartitionKey,
    height: u32,
    partitions: Vec<Partition>,
    by_key: HashMap<PartitionKey, usize>,
    rows: usize,
}

impl MemStore {
    pub fn new(dims: usize, tree: BuiltTree) -> Self {
        let rows = tree.row_count();
        let by_key = super::build::key_index(&tree.partitions);
        Self {
            dims,
            root: tree.root,
            height: tree.height,
            partitions: tree.partitions,
            by_key,
            rows,
        }
    }

    /// Wraps partitions that came from storage (the chunk reader).
    pub fn from_parts(
        dims: usize,
        root: PartitionKey,
        height: u32,
        partitions: Vec<Partition>,
    ) -> Self {
        let rows = partitions
            .iter()
            .filter(|p| p.is_leaf())
            .map(|p| p.len())
            .sum();
        let by_key = super::build::key_index(&partitions);
        Self {
            dims,
            root,
            height,
            partitions,
            by_key,
            rows,
        }
    }
}

impl SpannStore for MemStore {
    fn dims(&self) -> usize {
        self.dims
    }

    fn root(&self) -> PartitionKey {
        self.root
    }

    fn height(&self) -> u32 {
        self.height
    }

    fn partition(&self, key: PartitionKey) -> Option<&Partition> {
        self.by_key.get(&key).map(|i| &self.partitions[*i])
    }

    fn partitions(&self) -> &[Partition] {
        &self.partitions
    }

    fn len(&self) -> usize {
        self.rows
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::spann::{Encode, Options, SpannIndex};
    use crate::vector::Metric;

    #[test]
    fn mem_store_exposes_the_built_tree() {
        let rows: Vec<(u64, Vec<f32>)> = (0..300)
            .map(|i| (i as u64, vec![(i % 30) as f32, (i / 30) as f32, 1.0]))
            .collect();
        let opts = Options {
            metric: Metric::L2,
            max_partition_size: 32,
            encode: Encode::Rabitq,
            ..Options::default()
        };
        let index = SpannIndex::build(rows, opts).unwrap();
        let store = index.store();
        assert_eq!(store.len(), 300);
        assert!(store.height() >= 2);
        assert!(store.partition(store.root()).is_some());
        assert!(store.partition(999).is_none());
        assert!(store.partitions().len() >= 2);
    }
}
