//! `infinity-storage`: the storage kernel (RUST_REWRITE_PLAN.md §3 D2 + §附录 A).
//!
//! Implements the three independent substrates at their trait boundaries:
//!   * [`wal`]      — custom append-only redo log (CRC-32/IEEE framed)
//!   * [`mvcc`]     — timestamp-array MVCC (`BlockVersion`)
//!   * [`metastore`]— the `MetaStore` trait + pure-Rust `fjall` implementation
//!
//! Invariants preserved from C++ (see plan 附录 A): the substrates are separate;
//! MVCC is timestamp-array based (not KV snapshots); WAL writes are serialized.

#![forbid(unsafe_code)]

pub mod cpp_format;
pub mod crc;
pub mod metastore;
pub mod mvcc;
pub mod wal;

pub use cpp_format::{
    block_dir, chunk_index_file, column_file, column_layout, column_outline_file, decode_data_file,
    encode_data_file, fulltext_base_name, fulltext_files, pack_column, read_block,
    segment_index_dir, split_body, write_block, BlockColumn, BlockId, BlockVersionFile, ColumnBody,
    ColumnLayout, FormatError, RawBlockColumn, DATA_FILE_MAGIC, DEFAULT_BLOCK_CAPACITY,
    VERSION_FILE,
};
pub use crc::crc32_ieee;
pub use metastore::{FjallMetaStore, MetaStore};
pub use mvcc::{BlockVersion, CreateField};
pub use wal::{WalManager, WalRecord, WAL_MAGIC};
