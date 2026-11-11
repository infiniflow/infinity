//! Block files and index chunks in the reference byte layout.
//!
//! Was a single blocks/; the files follow what they touch: the block
//! itself (column encoding), the index chunks published next to it, and the
//! Engine methods that write and read them.

use std::path::{Path, PathBuf};
use std::sync::Arc;

use crate::*;
use ::common::{Error, Result};
use storage::wal::wal_entry::WalCmd;
use txn::{OptimizeEntry, TxnOp};

/// One chunk a merge wrote, and the chunks of its segment it supersedes -
/// the data of one `OptimizeIndexStoreEntry` (`new_chunk_infos_` /
/// `deprecate_chunks_`).
pub(crate) struct ChunkRewrite {
    pub segment: u32,
    pub chunk_id: u32,
    pub row_count: u32,
    pub deprecate_ids: Vec<u32>,
}

/// `(row id, vector)` pairs read back from a vector index mirror.
type VectorRows = Vec<(u32, Vec<f32>)>;
use expression::Expr;
use parser::{CmpOp, Literal};
use storage::definition::format::*;

pub(crate) use crate::engine::{parse_value, Engine};
pub(crate) use crate::{ColumnData, Value};
pub(crate) use catalog::layout::IndexChunk;
pub(crate) use catalog::layout::{
    BlockRecord, IndexSegment, SegmentRecord, SegmentSet, CPP_MIRROR_TS, UNCOMMIT_TS,
};
pub(crate) use index::hnsw_handler::HnswHandler;
pub use index::plaid::PlaidIndex;
pub(crate) use index::vector::HnswIndex;
use storage::buffer::mapped_block::MappedBlock;
use storage::knn_index::knn_format;

/// that is on disk (plan #70). `labels[i]` is the row id of node `i`.
/// Decoded parts of one HNSW chunk.
pub(crate) struct HnswChunkParts {
    pub(crate) m: u64,
    pub(crate) ef: u64,
    pub(crate) n: usize,
    pub(crate) dims: usize,
    pub(crate) max_layer: usize,
    pub(crate) enterpoint: Option<usize>,
    pub(crate) store_meta: Vec<u8>,
    pub(crate) records: Vec<u8>,
    /// Set when the chunk is **mapped**: the file, the vector region's offset and
    /// one vector's stride. `records` is empty then — the plain store borrows the
    /// mapping instead of copying the payload into the heap
    /// (`HnswIndex::from_chunk_region_mapped`).
    pub(crate) mapped: Option<(index::vector::ByteSource, usize, usize)>,
    /// The raw `GraphStoreInner` region, decoded straight into the compact
    /// graph (`HnswIndex::from_chunk_region`).
    pub(crate) graph_region: Vec<u8>,
    pub(crate) labels: Vec<u32>,
    pub(crate) options: HnswOptions,
}

/// Distinct rows a chunk's labels name.
///
/// A single-vector chunk labels node `i` with segment offset `i`, so the labels
/// are strictly increasing and the count is `labels.len()`; a `MULTIVECTOR`
/// chunk repeats a row once per token. The ordering test keeps the common case
/// allocation-free.
pub(crate) fn labelled_rows(labels: &[u32]) -> usize {
    if labels.windows(2).all(|pair| pair[0] < pair[1]) {
        return labels.len();
    }
    labels
        .iter()
        .collect::<std::collections::HashSet<_>>()
        .len()
}

pub(crate) mod chunk;
pub(crate) mod column;
pub(crate) mod engine;
pub(crate) mod index_chunk;
pub(crate) mod misc;
pub(crate) mod value_shape;

pub use chunk::encode_fulltext_chunk;
pub(crate) use chunk::*;
pub(crate) use column::*;
pub(crate) use index_chunk::*;
pub(crate) use misc::*;
pub(crate) use value_shape::*;
