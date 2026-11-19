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

//! PLAID multi-vector (tensor) index.
//!
//! C++: `src/storage/knn_index/plaid/*.cppm` — residual quantizer, global
//! centroids, in-memory index, on-disk index and disk merger. Each Rust type
//! keeps the reference's public methods and byte-level persistence format.

pub mod centroids;
pub mod index;
pub mod merger;
pub mod quantizer;

pub use centroids::PlaidGlobalCentroids;
pub use index::{Bitmask, PlaidIndex, PlaidQueryResult};
pub use merger::{
    MergeProgress, PlaidIndexDiskMerger, PlaidMergeManifest, PlaidMergeManifestChunk,
    PlaidMergeSizeInfo,
};
pub use quantizer::PlaidQuantizer;
