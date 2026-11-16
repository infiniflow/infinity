//! The inverted index, laid out like the reference's
//! src/storage/invertedindex/: the on-disk formats under format/ and the
//! sealed segment reader next to it.
//!
//! Only the parts the engine reaches are here; the reference's fst/, ring and
//! in-memory indexer live elsewhere (see docs/STRUCTURE.md).

pub mod chunk_manifest;
pub mod common;
pub mod disk_segment_reader;
pub mod format;
pub mod fst;
pub mod index_merger;
