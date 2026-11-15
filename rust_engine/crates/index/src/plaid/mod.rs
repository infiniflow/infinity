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
