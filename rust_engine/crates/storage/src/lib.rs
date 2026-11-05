//! `infinity-storage`: the storage kernel (RUST_REWRITE_PLAN.md §3 D2 + §附录 A).
//!
//! Implements the three independent substrates at their trait boundaries:
//!   * [`wal`]      — custom append-only redo log (CRC-32/IEEE framed)
//!   * [`mvcc`]     — timestamp-array MVCC (`BlockVersion`)
//!   * [`metastore`] — the `MetaStore` trait, backed by RocksDB
//!
//! Invariants preserved from C++ (see plan 附录 A): the substrates are separate;
//! MVCC is timestamp-array based (not KV snapshots); WAL writes are serialized.

// `#![deny(unsafe_code)]` with per-site `#[allow(unsafe_code)]`: the only
// `unsafe` here is `mmap` (mapping an index segment is far cheaper than reading
// it into the heap - see PORTING.md §2.8) and two narrow FFI calls
// (`malloc_trim`, `posix_fadvise`) that keep RSS and the page cache bounded
// during large imports. Every site documents why no safe API can express it.
#![deny(unsafe_code)]
// The workspace pins a nightly toolchain (see `rust-toolchain.toml`) because
// `std::simd` is still unstable: the posting decoder unpacks fastpfor's four
// lanes with portable SIMD, so the crate needs neither `std::arch` intrinsics
// (which would need `unsafe`) nor a bit-at-a-time scalar loop.
#![feature(portable_simd)]

/// Releases free heap pages back to the OS (glibc). A no-op on other targets.
/// Long imports allocate per-batch buffers; without this the allocator retains
/// them and RSS looks like a leak (Gate C).
#[allow(unsafe_code)]
pub fn trim_allocator() {
    #[cfg(all(target_os = "linux", target_env = "gnu"))]
    {
        unsafe extern "C" {
            fn malloc_trim(pad: usize) -> i32;
        }
        // SAFETY: glibc's `malloc_trim` only returns already-free heap pages.
        unsafe {
            malloc_trim(0);
        }
    }
}

/// Drops clean page cache for one file after writing it back. Long imports
/// write gigabytes of segment/sidecar files; without this the cache grows
/// until the OOM killer fires (the reference bounds it with its buffer manager).
#[allow(unsafe_code)]
pub fn drop_file_cache(path: &std::path::Path) {
    use std::os::unix::io::AsRawFd;
    let Ok(file) = std::fs::File::open(path) else {
        return;
    };
    let _ = file.sync_data();
    // SAFETY: `posix_fadvise` only advises the kernel about this fd's cache.
    unsafe {
        posix_fadvise(file.as_raw_fd(), 0, 0, 4 /* POSIX_FADV_DONTNEED */);
    }
}

#[allow(unsafe_code)]
unsafe extern "C" {
    fn posix_fadvise(fd: i32, offset: i64, len: i64, advice: i32) -> i32;
}

/// Recursively drops page cache under `root` (best effort).
pub fn drop_tree_cache(root: &std::path::Path) {
    let Ok(entries) = std::fs::read_dir(root) else {
        return;
    };
    for entry in entries.flatten() {
        let path = entry.path();
        if path.is_dir() {
            drop_tree_cache(&path);
        } else {
            drop_file_cache(&path);
        }
    }
}

pub mod bg_task;
pub mod buffer;
pub mod catalog;
pub mod column_vector;
pub mod common;
pub mod data_block;
pub mod definition;
pub mod fast_rough_filter;
pub mod invertedindex;
pub mod io;
pub mod knn_index;
pub mod new_txn;
pub mod persistence;
pub mod result_cache_manager;
pub mod secondary_index;
pub mod wal;

// The module tree above mirrors the reference's src/storage/. The re-exports
// below keep the flat names the rest of the workspace already uses, so a file
// can be moved to its reference-named directory without touching callers.
pub use bg_task::memindex_tracer;
pub use buffer::{mapped, mapped_block};
pub use catalog::{catalog_format, metastore, metastore_rocks};
pub use column_vector::selection;
pub use common::{block_index, crc, rcu_map};
pub use definition::format;
pub use fast_rough_filter::binary_fuse_filter;
pub use invertedindex::common::external_sort;
pub use invertedindex::fst::registry as fst_registry;
pub use io::{s3, virtual_store};
pub use knn_index::emvb::emvb_segment;
pub use knn_index::knn_format;
pub use knn_index::multivector::multivector_segment;
pub use knn_index::sparse::{bmp, bmp_segment};
pub use new_txn::{mvcc, txn_kv};
pub use secondary_index::{
    secondary_index_data, secondary_lowcard, secondary_segment_pgm as secondary_segment,
};
pub use wal::wal_entry;

/// Opens the store at the given path (creating it if needed).
///
/// RocksDB is the only substrate: it is what the C++ engine writes, so a
/// directory either engine produced opens with the same code and the same
/// bytes.
pub fn open_store(path: &std::path::Path) -> ::common::Result<std::sync::Arc<dyn MetaStore>> {
    let store = metastore_rocks::RocksMetaStore::open(path)?;
    Ok(std::sync::Arc::new(store))
}

pub use crc::crc32_ieee;
pub use format::{
    block_dir, chunk_index_file, column_file, column_layout, column_outline_file, data_file_body,
    date_to_days, datetime_to_pair, decode_data_file, element_width, embedding_column_layout,
    encode_data_file, fulltext_base_name, fulltext_files, pack_column, pack_column_with_layout,
    pack_tensor_body, parse_flat, parse_nested_f64, parse_tensorarray_f64, push_embedding_element,
    read_block, read_embedding_element, segment_index_dir, sparse_slot, split_body,
    split_body_for_layout, split_top_level, strip_one_bracket, time_to_secs,
    varlen_count_offset_slot, write_block, BlockColumn, BlockColumns, BlockId, BlockVersionFile,
    ColumnBody, ColumnLayout, FormatError, RawBlockColumn, DATA_FILE_MAGIC, DEFAULT_BLOCK_CAPACITY,
    VERSION_FILE,
};
pub use invertedindex::format::{
    append_fst_trailer, decode_column_lengths, decode_term_meta, encode_column_lengths,
    encode_term_meta, encode_term_meta_region, fst_checksum, read_vint, verify_fst, write_vint,
    TermMeta, TermMetaFlags, TermPostings, FST_TRAILER_LEN, FST_VERSION,
};
pub use io::{
    ByteSlice, ByteSliceList, ByteSliceListIterator, ByteSliceReader, ByteSliceWriter, FileReader,
    FileWriter, StreamReader,
};
pub use knn_format::{
    decode_graph_region, decode_ivf_plain_chunk, decode_ivf_pq_chunk, decode_ivf_sq_chunk,
    encode_graph_region, encode_ivf_plain_chunk, encode_ivf_pq_chunk, encode_ivf_sq_chunk,
    sq_bytes_per_vector, HnswChunk, HnswGraphVertex, HnswHeader, IvfChunkHeader, IvfPlainPart,
    IvfPqChunk, IvfPqCodebook, IvfPqPart, IvfSqChunk, IvfSqPart, HNSW_HEADER_LEN, VERTEX_L0_SIZE,
    VERTEX_LX_SIZE,
};
pub use metastore::MetaStore;
pub use metastore_rocks::RocksMetaStore;
pub use mvcc::{BlockVersion, CreateField};
pub use virtual_store::{
    string_to_storage_type, Compressor, FileAccessMode, FileHandle, StorageType, VirtualStore,
};
pub use wal::WalManager;
pub use wal_entry::{
    append_entries, KvBatchKind, WalBlockInfo, WalChunkIndexInfo, WalCmd, WalColumnDef,
    WalCommandType, WalEntry, WalEntryIterator, WalIndexBase, WalListIterator, WalRowId,
    WalSegmentIndexInfo, WalSegmentInfo, WalTableDef, WAL_ENTRY_HEADER_LEN,
};

#[cfg(test)]
mod mixed_column;
