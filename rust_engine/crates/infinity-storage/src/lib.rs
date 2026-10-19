//! `infinity-storage`: the storage kernel (RUST_REWRITE_PLAN.md §3 D2 + §附录 A).
//!
//! Implements the three independent substrates at their trait boundaries:
//!   * [`wal`]      — custom append-only redo log (CRC-32/IEEE framed)
//!   * [`mvcc`]     — timestamp-array MVCC (`BlockVersion`)
//!   * [`metastore`]— the `MetaStore` trait + pure-Rust `fjall` implementation
//!
//! Invariants preserved from C++ (see plan 附录 A): the substrates are separate;
//! MVCC is timestamp-array based (not KV snapshots); WAL writes are serialized.

// Maps sealed on-disk indexes. `mmap` is the one operation that needs `unsafe`
// (the mapping outlives the `File`); it is isolated in
// [`fulltext_segment::SegmentIndexReader::open`], which documents why the region
// cannot change while mapped.
#![deny(unsafe_code)]

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

pub mod binary_fuse_filter;
pub mod block_index;
pub mod bmp_cpp;
pub mod bmp_segment;
pub mod buffer;
pub mod catalog_format;
pub mod cpp_format;
pub mod crc;
pub mod data_block;
pub mod emvb_segment;
pub mod external_sort;
pub mod fst_registry;
pub mod fulltext_segment;
pub mod inverted_format;
pub mod io;
pub mod knn_format;
pub mod memindex_tracer;
pub mod metastore;
/// RocksDB substrate for a C++-compatible catalog (plan §2.6.1 item 4), behind
/// the `rocksdb-store` feature.
#[cfg(feature = "rocksdb-store")]
pub mod metastore_rocks;
pub mod multivector_segment;
pub mod mvcc;
pub mod persistence;
pub mod rcu_map;
pub mod result_cache;
pub mod s3;
pub mod secondary_index_data;
pub mod secondary_lowcard;
pub mod secondary_segment;
pub mod selection;
pub mod var_buffer;
pub mod virtual_store;
pub mod wal;
pub mod wal_entry;

pub use cpp_format::{
    block_dir, chunk_index_file, column_file, column_layout, column_outline_file, date_to_days,
    datetime_to_pair, decode_data_file, element_width, embedding_column_layout, encode_data_file,
    fulltext_base_name, fulltext_files, pack_column, pack_column_with_layout, pack_tensor_body,
    parse_flat, parse_nested_f64, parse_tensorarray_f64, push_embedding_element, read_block,
    read_embedding_element, segment_index_dir, sparse_slot, split_body, split_body_for_layout,
    split_top_level, strip_one_bracket, time_to_secs, varlen_count_offset_slot, write_block,
    BlockColumn, BlockId, BlockVersionFile, ColumnBody, ColumnLayout, FormatError, RawBlockColumn,
    DATA_FILE_MAGIC, DEFAULT_BLOCK_CAPACITY, VERSION_FILE,
};
pub use crc::crc32_ieee;
pub use inverted_format::{
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
pub use metastore::{FjallMetaStore, MetaStore};
#[cfg(feature = "rocksdb-store")]
pub use metastore_rocks::RocksMetaStore;
pub use mvcc::{BlockVersion, CreateField};
pub use virtual_store::{
    string_to_storage_type, Compressor, FileAccessMode, FileHandle, StorageType, VirtualStore,
};
pub use wal::{WalManager, WalRecord, WAL_MAGIC};
pub use wal_entry::{
    append_entries, WalBlockInfo, WalChunkIndexInfo, WalCmd, WalColumnDef, WalCommandType,
    WalEntry, WalEntryIterator, WalIndexBase, WalListIterator, WalRowId, WalSegmentIndexInfo,
    WalSegmentInfo, WalTableDef, WAL_ENTRY_HEADER_LEN,
};
