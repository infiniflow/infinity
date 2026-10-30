//! Persist a table's rows in the **C++ block layout** (RUST_REWRITE_PLAN.md
//! §2.6.1, task #68).
//!
//! `write_cpp_blocks` mirrors every row of a table into
//! `{data_dir}/data/db_{db}/tbl_{table}/seg_{seg}/blk_{blk}/` using the byte
//! layouts frozen in `rust_engine/docs/CPP_FORMAT_SPEC.md` §1/§8 — the same
//! files the C++ engine opens. `read_cpp_blocks` reads them back **without**
//! touching the KV store, which is how the round-trip tests prove the mirror is
//! self-sufficient.
//!
//! The mirror is additive: queries still run off the engine's own row store, so
//! turning it on cannot change SQL results.

use std::path::{Path, PathBuf};
use std::sync::Arc;

#[cfg(test)]
thread_local! {
    /// Blocks the prefilter skipped, so a test can prove the read happened.
    pub static PRUNED_BLOCKS: std::cell::Cell<usize> = const { std::cell::Cell::new(0) };
}

use infinity_catalog::cpp_layout::{
    CppBlock, CppIndexChunk, CppIndexSegment, CppSegment, CppSegmentSet, CPP_MIRROR_TS, UNCOMMIT_TS,
};
use infinity_common::{Error, Result};
use infinity_expression::Value;
use infinity_storage::WalCmd;
use infinity_txn::{OptimizeEntry, TxnOp};
/// One chunk a merge wrote, and the chunks of its segment it supersedes -
/// the data of one `OptimizeIndexStoreEntry` (`new_chunk_infos_` /
/// `deprecate_chunks_`).
pub(crate) struct ChunkRewrite {
    pub segment: u32,
    pub chunk_id: u32,
    pub row_count: u32,
    pub deprecate_ids: Vec<u32>,
}

/// The ts a deleted block slot carries when the delete's commit ts is unknown.
///
/// `BlockVersion::CheckDelete` hides a slot whose ts is non-zero and not newer
/// than the reader's, so any value in `[1, reader_ts]` works. A settled delete
/// carries its transaction's commit ts (`settle_delete_ts`); only a delete that
/// was never stamped - one the previous process recorded but did not commit, or
/// a row deleted before the ts log existed - falls back to this.
const DELETED_MIRROR_TS: u64 = 1;

/// Serialises mirror rewrites.
///
/// A commit re-mirrors its table (`refresh_cpp_block_mirror`), so two
/// concurrent inserts on one table would interleave the block writes and the
/// stale-directory sweep of the rewrite.
static MIRROR_WRITE_LOCK: std::sync::Mutex<()> = std::sync::Mutex::new(());
use infinity_index::hnsw_handler::HnswHandler;
use infinity_index::plaid::PlaidIndex;
use infinity_index::vector::HnswIndex;
use infinity_storage::cpp_format::{
    self, date_to_days, datetime_to_pair, element_width, embedding_size, pack_column_with_coverage,
    push_embedding_element, time_to_secs, BlockColumn, BlockId, BlockVersionFile, ColumnLayout,
    RawBlockColumn, DEFAULT_BLOCK_CAPACITY,
};
use infinity_storage::knn_format;

use crate::{append_copy_key, append_ts_key, appends_key, del_key, delete_ts_key, deletes_key,
    import_rows_key, import_ts_key, parse_value, row_key, Engine};
use infinity_parser::Literal;

/// `(row id, vector)` pairs read back from a vector index mirror.
type VectorRows = Vec<(u32, Vec<f32>)>;

/// An HNSW graph read back from the C++ mirror, so a query can walk the graph
/// that is on disk (plan #70). `labels[i]` is the row id of node `i`.
/// Decoded parts of one HNSW chunk.
struct HnswChunkParts {
    m: u64,
    ef: u64,
    n: usize,
    dims: usize,
    max_layer: usize,
    enterpoint: Option<usize>,
    store_meta: Vec<u8>,
    records: Vec<u8>,
    vertices: Vec<knn_format::HnswGraphVertex>,
    labels: Vec<u32>,
    options: HnswOptions,
}

fn align_up8(n: usize) -> usize {
    n.div_ceil(8) * 8
}

/// Bytes of the vec-store meta inside the chunk header.
fn hnsw_store_meta_len(
    dims: usize,
    metric: infinity_index::vector::Metric,
    encoding: infinity_index::vector::HnswEncoding,
) -> usize {
    use infinity_index::vector::{HnswEncoding, Metric};
    match encoding {
        HnswEncoding::Plain => 8,
        HnswEncoding::Lvq => {
            8 + 8 * dims
                + match metric {
                    Metric::L2 => 0,
                    Metric::Cos => 8,
                    Metric::Ip => 16,
                }
        }
        HnswEncoding::Rabitq => {
            let d = align_up8(dims);
            8 + 4 * d * d + 4 * d
        }
    }
}

/// Bytes per vector in the chunk's vector region.
fn hnsw_record_size(dims: usize, encoding: infinity_index::vector::HnswEncoding) -> usize {
    use infinity_index::vector::HnswEncoding;
    match encoding {
        HnswEncoding::Plain => dims * 4,
        HnswEncoding::Lvq => 16 + dims,
        HnswEncoding::Rabitq => 16 + align_up8(dims) / 8,
    }
}

/// HNSW build options read from the catalog (`IndexHnsw`).
struct HnswOptions {
    /// `IndexHnsw::M_`.
    m: usize,
    /// `IndexHnsw::ef_construction_`.
    ef_construction: usize,
    metric: infinity_index::vector::Metric,
    encoding: infinity_index::vector::HnswEncoding,
    lsg: Option<infinity_index::lsg::LsgConfig>,
}

impl Default for HnswOptions {
    fn default() -> Self {
        Self {
            m: 16,
            ef_construction: 200,
            metric: infinity_index::vector::Metric::L2,
            encoding: infinity_index::vector::HnswEncoding::Plain,
            lsg: None,
        }
    }
}

pub struct LoadedVectorIndex {
    pub index: HnswHandler,
    pub labels: Vec<u32>,
    /// The `chunk_{id}.idx` this graph came from, so a later reload can load
    /// only the chunks written since the last one.
    pub chunk_id: u64,
    /// Approximate heap bytes (vectors + graph + labels), for the buffer budget.
    pub bytes: usize,
    /// `(len, mtime_secs)` of the chunk file when it was decoded; a rewritten
    /// chunk under the same id is detected by a stamp change.
    pub stamp: (u64, u64),
}

// The mirror has no size cap: the reference mirrors every table it writes
// (`AppendInBlock`, `AppendInSegment`), so a port-written directory must hold
// blocks for every table too. Its memory stays a block, not a table, because
// `write_cpp_blocks` reads one block's rows at a time.

/// The secondary mirror names its dumps `seg_{segment}_{block}_{rows}.idx`: the
/// block is the chunk the reference publishes, and the trailing field is how
/// many rows the dump contributed. A block can be covered by several dumps, so
/// `publish_secondary_chunks` merges them rather than copying one file.
///
/// One raw ordered key, from a chunk's key array.
fn decode_secondary_key(
    bytes: &[u8],
    kind: infinity_storage::secondary_segment::KeyKind,
) -> Option<infinity_storage::secondary_segment::Key> {
    use infinity_storage::secondary_segment::{Key, KeyKind};
    Some(match kind {
        KeyKind::U64 => Key::U64(u64::from_le_bytes(bytes.try_into().ok()?)),
        KeyKind::I64 => Key::I64(i64::from_le_bytes(bytes.try_into().ok()?)),
        KeyKind::I32 => Key::I32(i32::from_le_bytes(bytes.try_into().ok()?)),
        KeyKind::F32 => Key::F32(f32::from_le_bytes(bytes.try_into().ok()?)),
        KeyKind::F64 => Key::F64(f64::from_le_bytes(bytes.try_into().ok()?)),
    })
}

/// Rebuilds a secondary index's published chunks: every spill run is decoded
/// and re-bucketed by **block**, then written as `chunk_{block}.idx`.
///
/// The spill runs are per memory dump, so a run may straddle a block boundary
/// or cover half of one; the reference's chunk is the block
/// (`SecondaryIndexDataHighCardinalityT::SaveIndexInner`,
/// `secondary_index_data_impl.cpp:223`, one per `InsertBlockData` call). The
/// offsets stay segment-relative, so the bucketing key is `offset / 8192` and
/// the values are written unchanged.
///
/// The spill file name carries the segment (`seg_{segment}_{block}_{rows}.idx`,
/// `write_secondary_pgm_chunks`), because two segments restart their offsets
/// at zero: only the runs of `segment` are published here, into that
/// segment's directory.
fn publish_secondary_chunks(
    seg_dir: &Path,
    dir: &Path,
    kind: infinity_storage::secondary_segment::KeyKind,
    segment: u32,
    single_chunk: bool,
) -> Vec<CppIndexChunk> {
    use infinity_storage::secondary_segment::{
        write_secondary_segment_typed, SecondarySegmentReader,
    };
    let width = kind.key_width();
    let stride = width + 4;
    let mut by_block: std::collections::BTreeMap<u64, Vec<_>> = Default::default();
    let Ok(entries) = std::fs::read_dir(dir) else {
        return Vec::new();
    };
    for entry in entries.flatten() {
        let path = entry.path();
        if path.extension().and_then(|e| e.to_str()) != Some("idx") {
            continue;
        }
        // `seg_{segment}_{block}_{rows}.idx` when `build_index` wrote it,
        // `seg_{segment}_tail{n}.idx` when a dump sealed a memory tail; both
        // carry the segment first, and a tail may span blocks, so the block of
        // an entry is derived from its offset rather than from the name.
        let name = entry.file_name().to_string_lossy().to_string();
        let Some(rest) = name
            .strip_prefix("seg_")
            .and_then(|n| n.strip_suffix(".idx"))
        else {
            continue;
        };
        let Some((run_segment, _)) = rest.split_once('_') else {
            continue;
        };
        if run_segment.parse::<u32>() != Ok(segment) {
            continue;
        };
        let (Ok(bytes), Ok(reader)) = (
            std::fs::read(&path),
            SecondarySegmentReader::open_with_kind(&path, kind),
        ) else {
            continue;
        };
        let n = reader.row_count();
        if bytes.len() < n * stride {
            continue;
        }
        for i in 0..n {
            let Some(key) = decode_secondary_key(&bytes[i * width..(i + 1) * width], kind) else {
                continue;
            };
            let at = n * width + i * 4;
            let offset = u32::from_le_bytes(bytes[at..at + 4].try_into().unwrap());
            by_block
                .entry(u64::from(offset) / DEFAULT_BLOCK_CAPACITY as u64)
                .or_default()
                .push((key, offset));
        }
    }
    // `PopulateSecondaryIndexInner` inserts every block of the segment into
    // one in-memory index and dumps **once**, so an imported segment has a
    // single chunk covering all of its blocks (`InsertBlockData` is called per
    // block, `Dump` once after the loop).
    if single_chunk {
        let mut entries: Vec<_> = by_block.into_values().flatten().collect();
        if entries.is_empty() || std::fs::create_dir_all(seg_dir).is_err() {
            return Vec::new();
        }
        entries.sort_by(|a, b| a.0.order(&b.0));
        let path = seg_dir.join("chunk_0.idx");
        if write_secondary_segment_typed(&path, kind, &entries).is_err() {
            return Vec::new();
        }
        return vec![CppIndexChunk {
            chunk_id: 0,
            base_name: String::new(),
            base_row_id: u64::from(segment) << 32,
            row_count: entries.len() as u64,
            term_count: 0,
            index_size: 0,
            commit_ts: 0,
        }];
    }
    let mut chunks = Vec::new();
    for (block, mut entries) in by_block {
        if std::fs::create_dir_all(seg_dir).is_err() {
            break;
        }
        entries.sort_by(|a, b| a.0.order(&b.0));
        let path = seg_dir.join(format!("chunk_{block}.idx"));
        if write_secondary_segment_typed(&path, kind, &entries).is_err() {
            continue;
        }
        chunks.push(CppIndexChunk {
            chunk_id: block as u32,
            base_name: String::new(),
            // `RowID` of the block's first row: the segment in the high bits,
            // the in-segment offset in the low ones.
            base_row_id: (u64::from(segment) << 32) | (block * DEFAULT_BLOCK_CAPACITY as u64),
            row_count: entries.len() as u64,
            term_count: 0,
            index_size: 0,
            commit_ts: 0,
        });
    }
    chunks
}

/// Rebuilds a sparse index's published chunks: one per block, from the rows.
///
/// The reference's chunk is the block, and every memory dump re-dumps the block
/// it touches (`BMPIndexInMem::Dump` hands the whole in-memory `BMPAlg` to the
/// chunk buffer), so two 100-row inserts into one block leave **one** chunk of
/// 200 rows. The port's spill files are per dump, so the merge happens here.
fn publish_bmp_chunks(
    seg_dir: &Path,
    by_block: std::collections::BTreeMap<u64, Vec<infinity_storage::bmp_cpp::SparseDoc>>,
    width: infinity_storage::bmp_cpp::IdxWidth,
    term_bound: usize,
    block_size: usize,
    segment: u32,
    single_chunk: bool,
) -> Vec<CppIndexChunk> {
    // `PopulateBMPIndexInner` adds every block of the segment to one
    // `BMPIndexInMem` and dumps once, so an imported segment's sparse chunk
    // covers all of its blocks.
    let by_block: std::collections::BTreeMap<u64, Vec<infinity_storage::bmp_cpp::SparseDoc>> =
        match single_chunk {
            true => {
                let docs: Vec<infinity_storage::bmp_cpp::SparseDoc> =
                    by_block.into_values().flatten().collect();
                if docs.is_empty() {
                    Default::default()
                } else {
                    [(0u64, docs)].into_iter().collect()
                }
            }
            false => by_block,
        };
    let mut chunks = Vec::new();
    for (block, mut docs) in by_block {
        if std::fs::create_dir_all(seg_dir).is_err() {
            break;
        }
        docs.sort_by_key(|(row, _)| *row);
        // The term space is the column's declared bound, widened to the terms
        // this chunk actually holds (`BMPAlg` is constructed with the bound).
        let term_num = docs
            .iter()
            .flat_map(|(_, entries)| entries.iter().map(|(term, _)| *term + 1))
            .max()
            .unwrap_or(0)
            .max(1)
            .max(term_bound as i32) as usize;
        let bytes = infinity_storage::bmp_cpp::encode_bmp(block_size, width, &docs, term_num);
        let path = seg_dir.join(format!("chunk_{block}.idx"));
        if std::fs::write(&path, &bytes).is_err() {
            continue;
        }
        chunks.push(CppIndexChunk {
            chunk_id: block as u32,
            base_name: String::new(),
            base_row_id: (u64::from(segment) << 32) | (block * DEFAULT_BLOCK_CAPACITY as u64),
            row_count: docs.len() as u64,
            term_count: 0,
            index_size: 0,
            commit_ts: 0,
        });
    }
    chunks
}

/// The full-text chunks on disk, one per block, named `ft_{base_row_id:016x}`.
fn publish_fulltext_chunks(seg_dir: &Path) -> Vec<CppIndexChunk> {
    let mut chunks = Vec::new();
    let Ok(entries) = std::fs::read_dir(seg_dir) else {
        return chunks;
    };
    for entry in entries.flatten() {
        let name = entry.file_name().to_string_lossy().to_string();
        let Some(hex) = name
            .strip_prefix("ft_")
            .and_then(|rest| rest.strip_suffix(".dic"))
        else {
            continue;
        };
        let Ok(base_row_id) = u64::from_str_radix(hex, 16) else {
            continue;
        };
        let base = cpp_format::fulltext_base_name(base_row_id, None);
        // The `.len` file is the chunk's per-document column length
        // (`u32[row_cnt]`, `RawFileWorker` in `chunk_index_meta_impl.cpp:157`),
        // so it carries both numbers the reference records in the chunk info:
        // the document count and the total term count
        // (`MemoryIndexer::GetDocTermCount` sums the lengths). The reference
        // needs the term count: a full-text query drops the whole search when
        // it is zero (`TermQueryNode::CreateSearch`, `query_node_impl.cpp:435`).
        let Some((row_count, term_count)) =
            fulltext_chunk_counts(&seg_dir.join(format!("{base}.len")))
        else {
            continue;
        };
        chunks.push(CppIndexChunk {
            chunk_id: 0, // assigned below, in base-row-id order
            base_name: base,
            base_row_id,
            row_count,
            term_count,
            index_size: 0,
            commit_ts: 0,
        });
    }
    // The reference's chunk ids are the order the chunks were dumped in, not
    // the block each one starts at: a second run that appends 110 rows to a
    // 100-row table dumps `ft_0000000000000064` as chunk 1, although both
    // chunks live in block 0. Reading them back, `CountMemIndexGapInSegment`
    // requires the chunk infos to line up end to end from row 0, so the ids
    // have to be distinct and ascending.
    chunks.sort_by_key(|c| c.base_row_id);
    for (id, chunk) in chunks.iter_mut().enumerate() {
        chunk.chunk_id = id as u32;
    }
    chunks
}

/// The `chunk_{id}.idx` files of an index that writes them in place (HNSW, IVF,
/// PLAID). An HNSW chunk's header carries its vector count, so a multi-chunk
/// index can be described exactly; the other kinds still write one chunk, which
/// covers the whole snapshot.
fn publish_chunk_files(
    seg_dir: &Path,
    rows: u64,
    hnsw: bool,
    segment: u32,
    deprecated: &std::collections::HashSet<u64>,
) -> Vec<CppIndexChunk> {
    let mut ids: Vec<u32> = Vec::new();
    if let Ok(entries) = std::fs::read_dir(seg_dir) {
        for entry in entries.flatten() {
            let name = entry.file_name().to_string_lossy().to_string();
            let Some(id) = name
                .strip_prefix("chunk_")
                .and_then(|rest| rest.strip_suffix(".idx"))
                .and_then(|id| id.parse::<u32>().ok())
            else {
                continue;
            };
            // A chunk an optimize superseded has left the segment's chunk list
            // even though its file is still on disk until `Cleanup`.
            if deprecated.contains(&u64::from(id)) {
                continue;
            }
            ids.push(id);
        }
    }
    if !hnsw {
        return if ids.is_empty() {
            Vec::new()
        } else {
            vec![CppIndexChunk {
                chunk_id: 0,
                base_name: String::new(),
                base_row_id: u64::from(segment) << 32,
                row_count: rows,
                term_count: 0,
                index_size: 0,
                commit_ts: 0,
            }]
        };
    }
    ids.sort_unstable();
    let mut chunks = Vec::new();
    let mut base_row_id = 0u64;
    for id in ids {
        // `[M][ef_construction][cur_vec_num]…` — the third u64 is this chunk's
        // vector count (`HnswHeader::cur_vec_num`, `knn_format.rs:44`).
        let row_count = std::fs::read(seg_dir.join(format!("chunk_{id}.idx")))
            .ok()
            .filter(|bytes| bytes.len() >= 24)
            .map(|bytes| u64::from_le_bytes(bytes[16..24].try_into().unwrap()))
            .unwrap_or(0);
        chunks.push(CppIndexChunk {
            chunk_id: id,
            base_name: String::new(),
            base_row_id: (u64::from(segment) << 32) | base_row_id,
            row_count,
            term_count: 0,
            index_size: 0,
            commit_ts: 0,
        });
        base_row_id += row_count;
    }
    chunks
}

/// The document count and the total term count of a full-text chunk, read
/// from its `.len` file.
///
/// The file is the chunk's per-document column length, a bare `u32[row_cnt]`
/// (`RawFileWorker` in `chunk_index_meta_impl.cpp:157`). The reference
/// records the same two numbers in the chunk info: the document count and the
/// sum of the lengths (`MemoryIndexer::GetDocTermCount`,
/// `memory_indexer_impl.cpp:636`).
fn fulltext_chunk_counts(len_path: &Path) -> Option<(u64, u64)> {
    let bytes = std::fs::read(len_path).ok()?;
    if bytes.len() % 4 != 0 {
        return None;
    }
    let mut doc_count = 0u64;
    let mut term_count = 0u64;
    for chunk in bytes.chunks_exact(4) {
        doc_count += 1;
        term_count += u32::from_le_bytes([chunk[0], chunk[1], chunk[2], chunk[3]]) as u64;
    }
    Some((doc_count, term_count))
}

/// `chunk_{id}.idx` files under `dir`, ordered by chunk id. The reference
/// appends one chunk per memory-index dump and merges them on optimize.
fn hnsw_chunks(dir: &std::path::Path) -> Vec<(u64, PathBuf)> {
    let mut chunks: Vec<(u64, PathBuf)> = std::fs::read_dir(dir)
        .map(|entries| {
            entries
                .flatten()
                .filter_map(|e| {
                    let name = e.file_name().to_string_lossy().to_string();
                    let id = name
                        .strip_prefix("chunk_")?
                        .strip_suffix(".idx")?
                        .parse::<u64>()
                        .ok()?;
                    Some((id, e.path()))
                })
                .collect()
        })
        .unwrap_or_default();
    chunks.sort_by_key(|(id, _)| *id);
    chunks
}

/// Maps an element-type name to the SDK `ElementType` code. `None` for bit
/// elements, which this writer does not pack.
fn element_code(elem: &str) -> Option<i32> {
    Some(if elem.contains("UINT8") {
        1
    } else if elem.contains("INT8") || elem.contains("TINYINT") {
        2
    } else if elem.contains("INT16") {
        3
    } else if elem.contains("INT32") || elem == "INT" {
        4
    } else if elem.contains("INT64") {
        5
    } else if elem.contains("FLOAT64") || elem.contains("DOUBLE") {
        7
    } else if elem.contains("BFLOAT16") {
        9
    } else if elem.contains("FLOAT16") {
        8
    } else if elem.contains("BIT") {
        0 // Bit (packed to ceil(dim/8) bytes; see `embedding_size`)
    } else {
        6 // Float32
    })
}

/// `EMBEDDING(<elem>,<dim>)` / `VECTOR(...)` → `(dimension, ElementType code)`.
pub fn embedding_shape(type_text: &str) -> Option<(usize, i32)> {
    let up = type_text.trim().to_ascii_uppercase();
    if !(up.starts_with("EMBEDDING(") || up.starts_with("VECTOR(")) {
        return None;
    }
    let inner = up.split_once('(')?.1.trim_end_matches(')');
    let mut parts = inner.split(',');
    let elem = parts.next()?.trim().to_string();
    let dim: usize = parts.next()?.trim().parse().ok()?;
    Some((dim, element_code(&elem)?))
}

/// `TENSOR(<elem>,<dim>)` / `MULTIVECTOR(<elem>,<dim>)` →
/// `(ColumnType, dimension, ElementType)`.
///
/// `TENSORARRAY` is excluded as a *clean* gap, not because it is ambiguous:
/// its slot (`TensorArrayT`, `tensor_array_type.h:28-36`) is the same
/// `{tensor_num:16; file_offset:48}`, and `file_offset_` points at
/// `tensor_num` 8-byte `TensorT` records (`SetTensorArray`,
/// `column_vector_impl.cpp:2792-2805`; `GetTensorArrayMeta`, `:2784-2790`),
/// each giving that tensor's own count and data offset. So the split *is*
/// derivable - implementing it needs a meta array written into the outline.
/// Bit elements are excluded for the same reason as embeddings.
pub fn tensor_shape(type_text: &str) -> Option<(i32, usize, i32)> {
    let up = type_text.trim().to_ascii_uppercase();
    let (column_type, rest) = if let Some(r) = up.strip_prefix("MULTIVECTOR(") {
        (14, r)
    } else if let Some(r) = up.strip_prefix("TENSOR(") {
        (11, r)
    } else {
        return None;
    };
    let inner = rest.trim_end_matches(')');
    let parts: Vec<&str> = inner.split(',').map(str::trim).collect();
    let dim: usize = parts.get(1)?.parse().ok()?;
    let code = element_code(parts.first()?)?;
    // A bit tensor packs `dim` bits per row into `ceil(dim/8)` bytes, the same
    // element packing an `EMBEDDING(BIT, dim)` uses.
    Some((column_type, dim, code))
}

/// `TENSORARRAY(<elem>,<dim>)` → `(ColumnType, dimension, ElementType)`.
///
/// The slot (`TensorArrayT`, `tensor_array_type.h:28-36`) is the same
/// `{tensor_num:16; file_offset:48}` word as a tensor's, but `file_offset_`
/// points at `tensor_num` 8-byte `TensorT` records (`SetTensorArray`,
/// `column_vector_impl.cpp:2792-2805`; `GetTensorArrayMeta`, `:2784-2790`),
/// each of which carries that tensor's own element count and data offset. The
/// mirror therefore writes every tensor's body first and their meta array last.
pub fn tensorarray_shape(type_text: &str) -> Option<(i32, usize, i32)> {
    let up = type_text.trim().to_ascii_uppercase();
    let rest = up.strip_prefix("TENSORARRAY(")?;
    let inner = rest.trim_end_matches(')');
    let parts: Vec<&str> = inner.split(',').map(str::trim).collect();
    let dim: usize = parts.get(1)?.parse().ok()?;
    let code = element_code(parts.first()?)?;
    Some((12, dim, code))
}

/// `SPARSE(<elem>,<bound>)` / `SPARSE(<elem>,<index>,<bound>)` →
/// `(element_type, index_type, index_width)`.
///
/// The two-component form derives the index width from the bound. The
/// three-component form is what the C++ catalog and the REST physical type
/// carry, and there the width is the *declared* one — a `sparse,8,float,int`
/// column stores 4-byte indices even though a bound of 8 would imply int8.
pub fn sparse_shape(type_text: &str) -> Option<(i32, i32, usize)> {
    let up = type_text.trim().to_ascii_uppercase();
    let rest = up.strip_prefix("SPARSE(")?;
    let inner = rest.trim_end_matches(')');
    let parts: Vec<&str> = inner.split(',').map(str::trim).collect();
    let elem = element_code(parts.first()?)?;
    let (explicit_index, bound_text) = if parts.len() >= 3 {
        (Some(parts[1]), parts[2])
    } else {
        (None, *parts.get(1)?)
    };
    let bound: u64 = bound_text.parse().unwrap_or(0);
    let (index_type, width) = match explicit_index.and_then(sparse_index_kind) {
        Some(kind) => kind,
        None => derived_index_kind(bound),
    };
    Some((elem, index_type, width))
}

/// The index width implied by the bound (`SparseInfo::GetIndexType`,
/// `src/parser/type/info/sparse_info.cpp:44-58`).
fn derived_index_kind(bound: u64) -> (i32, usize) {
    if bound <= 127 {
        (2, 1)
    } else if bound <= 32767 {
        (3, 2)
    } else if bound <= 2_147_483_647 {
        (4, 4)
    } else {
        (5, 8)
    }
}

/// `int8`/`tinyint`/… → `(index_type, index_width)`.
fn sparse_index_kind(name: &str) -> Option<(i32, usize)> {
    Some(match name {
        "INT8" | "TINYINT" | "UINT8" => (2, 1),
        "INT16" | "SMALLINT" => (3, 2),
        "INT" | "INTEGER" | "INT32" => (4, 4),
        "INT64" | "BIGINT" => (5, 8),
        _ => return None,
    })
}

/// Parses the engine's sparse rendering `[i:val,…]` / `(empty)`.
pub fn parse_sparse_literal(cell: &str) -> (Vec<i64>, Vec<f64>) {
    let inner = cell.trim();
    if inner == "(empty)" || inner.is_empty() || inner == "[]" {
        return (Vec::new(), Vec::new());
    }
    let inner = inner.trim_start_matches('[').trim_end_matches(']');
    let mut idx = Vec::new();
    let mut val = Vec::new();
    for part in inner.split(',') {
        let part = part.trim();
        if part.is_empty() {
            continue;
        }
        match part.split_once(':') {
            Some((i, v)) => {
                if let (Ok(i), Ok(v)) = (i.trim().parse::<i64>(), v.trim().parse::<f64>()) {
                    idx.push(i);
                    val.push(v);
                }
            }
            // An index-only literal is a `SPARSE(BIT, …)` vector: every listed
            // position is present and carries the implicit value 1.
            None => {
                if let Ok(i) = part.parse::<i64>() {
                    idx.push(i);
                    val.push(1.0);
                }
            }
        }
    }
    (idx, val)
}

/// Encodes one sparse value: appends `indices` then `values` to `outline` and
/// returns the `i64 nnz + i64 offset` slot.
fn encode_sparse_slot(
    type_text: &str,
    value: &str,
    outline: &mut Vec<u8>,
) -> Result<Option<Vec<u8>>> {
    if value.eq_ignore_ascii_case("null") {
        return Ok(None);
    }
    let (elem, _index_type, index_width) = sparse_shape(type_text)
        .ok_or_else(|| Error::Invalid(format!("not a sparse type: '{type_text}'")))?;
    let (indices, values) = parse_sparse_literal(value);
    let offset = outline.len() as u64;
    for i in &indices {
        match index_width {
            1 => outline.push(*i as i8 as u8),
            2 => outline.extend_from_slice(&(*i as i16).to_le_bytes()),
            4 => outline.extend_from_slice(&(*i as i32).to_le_bytes()),
            _ => outline.extend_from_slice(&i.to_le_bytes()),
        }
    }
    // A BIT data type stores no values (`SparseInfo::DataSize` returns 0 for
    // `kElemBit`, `sparse_info.h:61-66`): the column is a bitmap of the
    // present indices and the value is implicitly 1.
    if elem != 0 {
        for v in &values {
            cpp_format::push_embedding_element(outline, elem, *v);
        }
    }
    Ok(Some(
        cpp_format::sparse_slot(indices.len() as i64, offset).to_vec(),
    ))
}

/// Decodes one sparse slot out of the outline.
fn decode_sparse_value(type_text: &str, slot: &[u8], outline: &[u8]) -> Option<String> {
    let (elem, _index_type, index_width) = sparse_shape(type_text)?;
    let nnz = i64::from_le_bytes(slot.get(..8)?.try_into().ok()?) as usize;
    let offset = u64::from_le_bytes(slot.get(8..16)?.try_into().ok()?) as usize;
    // A BIT data type stores no value bytes; the value is implicitly 1.
    let elem_width = if elem == 0 { 0 } else { element_width(elem) };
    let mut idx_at = offset;
    let mut val_at = offset + nnz * index_width;
    if nnz == 0 {
        // The stored text of an empty sparse vector is the reference's marker
        // (`reshape_sparse`): a `SELECT` prints it as-is.
        return Some("(empty)".to_string());
    }
    let mut parts = Vec::with_capacity(nnz);
    for _ in 0..nnz {
        let i = match index_width {
            1 => f64::from(*outline.get(idx_at)? as i8),
            2 => f64::from(i16::from_le_bytes(
                outline.get(idx_at..idx_at + 2)?.try_into().ok()?,
            )),
            4 => f64::from(i32::from_le_bytes(
                outline.get(idx_at..idx_at + 4)?.try_into().ok()?,
            )),
            _ => i64::from_le_bytes(outline.get(idx_at..idx_at + 8)?.try_into().ok()?) as f64,
        };
        idx_at += index_width;
        let v = if elem == 0 {
            1.0
        } else {
            cpp_format::read_embedding_element(outline.get(val_at..val_at + elem_width)?, elem)?
        };
        val_at += elem_width;
        // A `kElemBit` column lists the present positions only, and a float
        // element prints with the reference's six decimals
        // (`SparseT::ToString` -> `std::to_string`); an integer one is exact.
        parts.push(if elem == 0 {
            format!("{}", i as i64)
        } else if elem >= 6 {
            format!("{}:{v:.6}", i as i64)
        } else {
            format!("{}:{}", i as i64, v as i64)
        });
    }
    Some(format!("[{}]", parts.join(",")))
}

/// Element shape of an `ARRAY(...)` column.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ArrayElem {
    /// A fixed-width scalar element (its `ColumnType`).
    Fixed(i32),
    /// An `ARRAY(<fixed scalar>)` element; the value is the inner scalar's
    /// `ColumnType`. The outer elements are 8-byte `ArrayType` slots into the
    /// outline, each pointing at its own elements — `ArrayInfo::ElemSize` is the
    /// inner `DataType::Size()`, which for an array is the 8-byte slot.
    Nested(i32),
    /// A `Varchar`/`Json` element; the outer elements are 16-byte `VarcharT`
    /// slots into the outline (their payload inlines or outlines as usual).
    VarLen(i32),
}

/// Width of a fixed-width scalar's slot, `None` for variable-length ones.
fn fixed_scalar_width(col: i32) -> Option<usize> {
    match cpp_format::column_layout(col)? {
        ColumnLayout::Fixed(w) => Some(w),
        ColumnLayout::CompactBit => Some(1),
        ColumnLayout::VarLen(_) => None,
    }
}

/// `ARRAY(<scalar>)` / `ARRAY(ARRAY(<scalar>))` → its element shape.
///
/// Scalar leaves may be fixed-width or `Varchar`/`Json`; a second nesting level
/// must bottom out in a fixed-width scalar. Anything else returns `None` and is
/// reported as unsupported by the caller.
pub fn array_shape(type_text: &str) -> Option<ArrayElem> {
    let up = type_text.trim().to_ascii_uppercase();
    let rest = up.strip_prefix("ARRAY(")?;
    let inner = rest.trim_end_matches(')').trim();
    if let Some(scalar) = inner.strip_prefix("ARRAY(") {
        let col = column_type_of(scalar.trim_end_matches(')').trim())?;
        fixed_scalar_width(col)?;
        return Some(ArrayElem::Nested(col));
    }
    let col = column_type_of(inner)?;
    match cpp_format::column_layout(col)? {
        ColumnLayout::Fixed(_) | ColumnLayout::CompactBit => Some(ArrayElem::Fixed(col)),
        ColumnLayout::VarLen(16) if matches!(col, 9 | 22) => Some(ArrayElem::VarLen(col)),
        ColumnLayout::VarLen(_) => None,
    }
}

/// Parses the engine's array rendering `{1,2,3}` into its elements.
pub fn parse_braces(cell: &str) -> Vec<String> {
    let t = cell.trim();
    let inner = if t.starts_with('{') && t.ends_with('}') && t.len() >= 2 {
        &t[1..t.len() - 1]
    } else {
        t
    };
    if inner.trim().is_empty() {
        return Vec::new();
    }
    inner.split(',').map(|s| s.trim().to_string()).collect()
}

/// Splits the body of a `{{…},{…}}` rendering into its top-level groups, e.g.
/// `{0,3},{1}` → `["{0,3}", "{1}"]`. A `{}` group is kept (as an empty one).
fn split_top_braces(inner: &str) -> Vec<String> {
    let mut out = Vec::new();
    let mut depth = 0i32;
    let mut cur = String::new();
    for c in inner.chars() {
        match c {
            '{' => {
                depth += 1;
                cur.push(c);
            }
            '}' => {
                depth -= 1;
                cur.push(c);
                if depth == 0 {
                    out.push(cur.trim().to_string());
                    cur.clear();
                }
            }
            ',' if depth == 0 => {}
            _ => cur.push(c),
        }
    }
    if !cur.trim().is_empty() {
        out.push(cur.trim().to_string());
    }
    out
}

/// Parses the engine's nested-array rendering `{{0,3},{1}}` into one inner
/// element list per outer element.
fn parse_nested_braces(cell: &str) -> Vec<Vec<String>> {
    let t = cell.trim();
    let inner = if t.starts_with('{') && t.ends_with('}') && t.len() >= 2 {
        &t[1..t.len() - 1]
    } else {
        t
    };
    split_top_braces(inner)
        .iter()
        .map(|g| parse_braces(g))
        .collect()
}

/// Encodes one array value: appends the elements to `outline` and returns the
/// `{count:16; offset:48}` slot.
fn encode_array_slot(
    type_text: &str,
    value: &str,
    outline: &mut Vec<u8>,
) -> Result<Option<Vec<u8>>> {
    if value.eq_ignore_ascii_case("null") {
        return Ok(None);
    }
    let elem = array_shape(type_text)
        .ok_or_else(|| Error::Invalid(format!("not a supported array type: '{type_text}'")))?;
    match elem {
        ArrayElem::Fixed(col) => {
            let width = fixed_scalar_width(col)
                .ok_or_else(|| Error::Invalid("array element is not fixed-width".into()))?;
            let elems = parse_braces(value);
            let offset = outline.len() as u64;
            for e in &elems {
                match encode_value("", col, e)? {
                    Some(bytes) => outline.extend_from_slice(&bytes),
                    // A NULL element is a zeroed slot (the element bitmap lives
                    // in the inner column, which the C++ reader supplies).
                    None => outline.extend_from_slice(&vec![0u8; width]),
                }
            }
            Ok(Some(
                cpp_format::varlen_count_offset_slot(elems.len() as u64, offset).to_vec(),
            ))
        }
        ArrayElem::Nested(col) => {
            let width = fixed_scalar_width(col)
                .ok_or_else(|| Error::Invalid("array element is not fixed-width".into()))?;
            let groups = parse_nested_braces(value);
            // Each inner array's data first (recording its offset), then the
            // `ArrayType` slot that points at it; the outer slot points at the
            // run of inner slots.
            let mut inner_slots: Vec<[u8; 8]> = Vec::with_capacity(groups.len());
            for g in &groups {
                let data_offset = outline.len() as u64;
                for e in g {
                    match encode_value("", col, e)? {
                        Some(bytes) => outline.extend_from_slice(&bytes),
                        None => outline.extend_from_slice(&vec![0u8; width]),
                    }
                }
                inner_slots.push(cpp_format::varlen_count_offset_slot(
                    g.len() as u64,
                    data_offset,
                ));
            }
            let slots_offset = outline.len() as u64;
            for s in &inner_slots {
                outline.extend_from_slice(s);
            }
            Ok(Some(
                cpp_format::varlen_count_offset_slot(groups.len() as u64, slots_offset).to_vec(),
            ))
        }
        ArrayElem::VarLen(_) => {
            // Each element is a 16-byte `VarcharT` slot; its payload inlines or
            // moves to the outline per `encode_varchar_slot`. The outer slot
            // points at the run of element slots.
            let elems = parse_braces(value);
            let mut slots: Vec<[u8; 16]> = Vec::with_capacity(elems.len());
            for e in &elems {
                slots.push(cpp_format::encode_varchar_slot(e.as_bytes(), outline));
            }
            let slots_offset = outline.len() as u64;
            for s in &slots {
                outline.extend_from_slice(s);
            }
            Ok(Some(
                cpp_format::varlen_count_offset_slot(slots.len() as u64, slots_offset).to_vec(),
            ))
        }
    }
}

/// Decodes one array slot out of the outline.
fn decode_array_value(type_text: &str, slot: &[u8], outline: &[u8]) -> Option<String> {
    let elem = array_shape(type_text)?;
    let word = u64::from_le_bytes(slot.try_into().ok()?);
    let count = (word & 0xffff) as usize;
    let offset = (word >> 16) as usize;
    match elem {
        ArrayElem::Fixed(col) => {
            let width = fixed_scalar_width(col)?;
            let tail = outline.get(offset..offset + count * width)?;
            let mut parts = Vec::with_capacity(count);
            for k in 0..count {
                parts.push(decode_value("", col, tail, k, outline)?);
            }
            Some(format!("{{{}}}", parts.join(",")))
        }
        ArrayElem::Nested(col) => {
            let width = fixed_scalar_width(col)?;
            let mut groups = Vec::with_capacity(count);
            for gi in 0..count {
                let at = offset + gi * 8;
                let inner = u64::from_le_bytes(outline.get(at..at + 8)?.try_into().ok()?);
                let inner_count = (inner & 0xffff) as usize;
                let inner_offset = (inner >> 16) as usize;
                let mut parts = Vec::with_capacity(inner_count);
                for k in 0..inner_count {
                    let base = inner_offset + k * width;
                    parts.push(decode_value(
                        "",
                        col,
                        outline.get(base..base + width)?,
                        0,
                        outline,
                    )?);
                }
                groups.push(format!("{{{}}}", parts.join(",")));
            }
            Some(format!("{{{}}}", groups.join(",")))
        }
        ArrayElem::VarLen(_) => {
            let mut parts = Vec::with_capacity(count);
            for k in 0..count {
                let at = offset + k * 16;
                let bytes = cpp_format::decode_varchar_slot(outline.get(at..at + 16)?, outline)?;
                parts.push(String::from_utf8_lossy(&bytes).to_string());
            }
            Some(format!("{{{}}}", parts.join(",")))
        }
    }
}

/// Renders an engine-rendered value as a SQL literal for re-insertion.
///
/// Text-valued columns (varchar/json/date/time/datetime/timestamp) must be
/// quoted; numbers, booleans, arrays, tensors and sparse vectors are already in
/// literal form. Single quotes are doubled, which is correct SQL — note our own
/// parser does not currently accept `''`, so values containing an apostrophe
/// cannot be re-inserted until it does.
fn sql_literal(type_text: &str, value: &str) -> String {
    if value.eq_ignore_ascii_case("null") {
        return "NULL".to_string();
    }
    let up = type_text.to_ascii_uppercase();
    let base = up.split('(').next().unwrap_or("").trim();
    // A BIT embedding's stored text is the display form (`[10110001]`); SQL
    // input takes one number per element (`[1,0,1,1,0,0,0,1]`).
    if let Some((dim, elem)) = embedding_shape(&up) {
        let inner = value.trim().trim_start_matches('[').trim_end_matches(']');
        if elem == 0
            && dim > 0
            && !inner.contains(',')
            && inner.len() == dim
            && inner.chars().all(|c| c == '0' || c == '1')
        {
            let cells: Vec<&str> = inner
                .chars()
                .map(|c| if c == '1' { "1" } else { "0" })
                .collect();
            return format!("[{}]", cells.join(","));
        }
    }
    let quoted = matches!(
        base,
        "VARCHAR" | "TEXT" | "STRING" | "JSON" | "DATE" | "TIME" | "DATETIME" | "TIMESTAMP"
    );
    if quoted {
        format!("'{}'", value.replace('\'', "''"))
    } else if is_float_type_text(base)
        && !value.contains(['.', 'e', 'E'])
        && value.trim().parse::<i64>().is_ok()
    {
        // A float column's text may have lost its digits (`1`, `-0`). SQL
        // would read that as an integer literal, which drops the sign of a
        // negative zero and the type of the value; `.0` keeps it a float.
        format!("{value}.0")
    } else {
        value.to_string()
    }
}

/// Whether a base type name (already uppercased, without parameters) is one of
/// the engine's float types.
fn is_float_type_text(base: &str) -> bool {
    matches!(
        base,
        "FLOAT" | "DOUBLE" | "REAL" | "FP16" | "BF16" | "FLOAT16" | "BFLOAT16"
    )
}

/// Maps a declared type text to the `ColumnType` code this writer packs.
///
/// Returns `None` for shapes the packer does not cover yet
/// (array/tensor/sparse, and bit embeddings); callers report those instead of
/// writing a mis-encoded file.
pub fn column_type_of(type_text: &str) -> Option<i32> {
    let t = type_text.trim().to_ascii_uppercase();
    let base = t.split('(').next().unwrap_or("").trim();
    Some(match base {
        "BOOL" | "BOOLEAN" => 0,
        "TINYINT" | "INT8" => 1,
        "SMALLINT" | "INT16" => 2,
        "INT" | "INTEGER" | "INT32" => 3,
        "BIGINT" | "INT64" => 4,
        "FLOAT" | "REAL" | "FLOAT32" => 5,
        "DOUBLE" | "FLOAT64" => 6,
        // The 16-bit floats are two bytes wide (`float16_t` / `bfloat16_t`,
        // `src/parser/type/number/float16.h`).
        "FLOAT16" | "FP16" | "HALF" => 7,
        "BFLOAT16" | "BF16" => 8,
        "VARCHAR" | "TEXT" | "STRING" => 9,
        "DATE" => 16,
        "TIME" => 17,
        "DATETIME" | "TIMESTAMP" => 18,
        "JSON" => 22,
        "EMBEDDING" | "VECTOR" => {
            embedding_shape(type_text)?;
            10
        }
        "TENSOR" | "MULTIVECTOR" => {
            tensor_shape(type_text)?;
            if base == "TENSOR" {
                11
            } else {
                14
            }
        }
        "TENSORARRAY" => {
            tensorarray_shape(type_text)?;
            12
        }
        "SPARSE" => {
            sparse_shape(type_text)?;
            13
        }
        "ARRAY" => {
            array_shape(type_text)?;
            21
        }
        _ => return None,
    })
}

/// Which self-outlining encoder a column needs (its outline is built by the
/// caller rather than by the generic var-length packer).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
enum OutlineKind {
    None,
    Tensor,
    TensorArray,
    Sparse,
    Array,
}

fn outline_kind(type_text: &str) -> OutlineKind {
    if tensorarray_shape(type_text).is_some() {
        OutlineKind::TensorArray
    } else if tensor_shape(type_text).is_some() {
        OutlineKind::Tensor
    } else if sparse_shape(type_text).is_some() {
        OutlineKind::Sparse
    } else if array_shape(type_text).is_some() {
        OutlineKind::Array
    } else {
        OutlineKind::None
    }
}

/// The `.col` layout of a column, including embedding columns whose fixed width
/// depends on the declared dimension.
fn layout_of(type_text: &str, column_type: i32) -> Option<ColumnLayout> {
    if let Some((dim, elem)) = embedding_shape(type_text) {
        // A BIT vector packs to `ceil(dim/8)` bytes, not `dim` (`:253-262`).
        return Some(ColumnLayout::Fixed(embedding_size(elem, dim)));
    }
    if tensor_shape(type_text).is_some() {
        // Tensor / MultiVector: a `{count:16; offset:48}` slot into the outline.
        return Some(ColumnLayout::VarLen(8));
    }
    if tensorarray_shape(type_text).is_some() {
        // TensorArray: the same slot; the outline holds every tensor's body and
        // their `TensorT` meta array.
        return Some(ColumnLayout::VarLen(8));
    }
    if sparse_shape(type_text).is_some() {
        // Sparse: `i64 nnz` + `i64 offset`.
        return Some(ColumnLayout::VarLen(16));
    }
    if array_shape(type_text).is_some() {
        // Array: a `{count:16; offset:48}` slot into the outline.
        return Some(ColumnLayout::VarLen(8));
    }
    cpp_format::column_layout(column_type)
}

/// Encodes one tensor value: appends the element bytes to `outline` and returns
/// the `{count:16; offset:48}` slot.
fn encode_tensor_slot(
    type_text: &str,
    value: &str,
    outline: &mut Vec<u8>,
) -> Result<Option<Vec<u8>>> {
    if value.eq_ignore_ascii_case("null") {
        return Ok(None);
    }
    let (_, dim, elem) = tensor_shape(type_text)
        .ok_or_else(|| Error::Invalid(format!("not a tensor type: '{type_text}'")))?;
    let value = if elem == 0 {
        expand_bit_literals(value)
    } else {
        value.to_string()
    };
    let vectors = cpp_format::parse_nested_f64(&value);
    let offset = outline.len() as u64;
    let body = cpp_format::pack_tensor_body(&vectors, dim, elem);
    outline.extend_from_slice(&body);
    Ok(Some(
        cpp_format::varlen_count_offset_slot(vectors.len() as u64, offset).to_vec(),
    ))
}

/// Decodes one tensor slot, reading its body out of the outline.
fn decode_tensor_value(type_text: &str, slot: &[u8], outline: &[u8]) -> Option<String> {
    let (_, dim, elem) = tensor_shape(type_text)?;
    let word = u64::from_le_bytes(slot.try_into().ok()?);
    let count = (word & 0xffff) as usize;
    let offset = (word >> 16) as usize;
    let width = tensor_row_width(dim, elem);
    let mut parts = Vec::with_capacity(count);
    for v in 0..count {
        let at = offset + v * width;
        parts.push(tensor_row_text(outline, at, dim, elem)?);
    }
    Some(format!("[{}]", parts.join(",")))
}

/// Expands packed bit strings inside a tensor literal so the ordinary numeric
/// parser can read both spellings: [[[10110001]]] becomes [[[1,0,1,1,0,0,0,1]]].
/// Only used for kElemBit element types - a float tensor's 10110001 is a number.
fn expand_bit_literals(text: &str) -> String {
    let t = text.trim();
    let Some(inner) = t.strip_prefix('[').and_then(|s| s.strip_suffix(']')) else {
        return t.to_string();
    };
    if !inner.contains('[') {
        let trimmed = inner.trim();
        if trimmed.len() > 1 && trimmed.chars().all(|c| c == '0' || c == '1') {
            let bits: Vec<String> = trimmed.chars().map(|c| c.to_string()).collect();
            return format!("[{}]", bits.join(","));
        }
        return t.to_string();
    }
    let parts: Vec<String> = crate::split_top_level(inner)
        .iter()
        .map(|g| expand_bit_literals(g))
        .collect();
    format!("[{}]", parts.join(","))
}

/// Encodes one tensor-array value: appends every tensor's body to `outline`,
/// then their `TensorT` meta array, and returns the `{count:16; offset:48}` slot
/// (`SetTensorArray`, `column_vector_impl.cpp:2792-2805`).
fn encode_tensorarray_slot(
    type_text: &str,
    value: &str,
    outline: &mut Vec<u8>,
) -> Result<Option<Vec<u8>>> {
    if value.eq_ignore_ascii_case("null") {
        return Ok(None);
    }
    let (_, dim, elem) = tensorarray_shape(type_text)
        .ok_or_else(|| Error::Invalid(format!("not a tensor-array type: '{type_text}'")))?;
    // `[[[a,b],[c,d]], …]`: one entry per tensor, each a list of rows. A bit
    // element may already arrive packed (`[[[10110001]]]`), which the numeric
    // parser would read as one number.
    let value = if elem == 0 {
        expand_bit_literals(value)
    } else {
        value.to_string()
    };
    let tensors = cpp_format::parse_tensorarray_f64(&value);
    let mut metas: Vec<[u8; 8]> = Vec::with_capacity(tensors.len());
    for rows in &tensors {
        let offset = outline.len() as u64;
        outline.extend_from_slice(&cpp_format::pack_tensor_body(rows, dim, elem));
        metas.push(cpp_format::varlen_count_offset_slot(
            rows.len() as u64,
            offset,
        ));
    }
    let offset = outline.len() as u64;
    for meta in &metas {
        outline.extend_from_slice(meta);
    }
    Ok(Some(
        cpp_format::varlen_count_offset_slot(metas.len() as u64, offset).to_vec(),
    ))
}

/// Decodes one tensor-array slot, reading each tensor's `TensorT` meta and its
/// body out of the outline (`GetTensorArray`, `column_vector_impl.cpp:2808-2818`).
fn decode_tensorarray_value(type_text: &str, slot: &[u8], outline: &[u8]) -> Option<String> {
    let (_, dim, elem) = tensorarray_shape(type_text)?;
    let word = u64::from_le_bytes(slot.try_into().ok()?);
    let tensor_num = (word & 0xffff) as usize;
    let meta_offset = (word >> 16) as usize;
    let width = tensor_row_width(dim, elem);
    let mut tensors = Vec::with_capacity(tensor_num);
    for t in 0..tensor_num {
        let at = meta_offset + t * 8;
        let meta = u64::from_le_bytes(outline.get(at..at + 8)?.try_into().ok()?);
        let rows = (meta & 0xffff) as usize;
        let offset = (meta >> 16) as usize;
        let mut vecs = Vec::with_capacity(rows);
        for v in 0..rows {
            vecs.push(tensor_row_text(outline, offset + v * width, dim, elem)?);
        }
        tensors.push(format!("[{}]", vecs.join(",")));
    }
    Some(format!("[{}]", tensors.join(",")))
}

/// Bytes one tensor row of `dim` elements occupies. A `kElemBit` row packs
/// `dim` bits into `ceil(dim/8)` bytes; every other element type is fixed
/// width.
fn tensor_row_width(dim: usize, elem: i32) -> usize {
    if elem == 0 {
        dim.div_ceil(8)
    } else {
        dim * element_width(elem)
    }
}

/// One tensor row as the reference renders it: `[a,b,c]` for numeric
/// elements, the bit string `[10110001]` for `kElemBit`
/// (`embedding_type.h:253-262`, the same form an `EMBEDDING(BIT, dim)` prints).
fn tensor_row_text(outline: &[u8], at: usize, dim: usize, elem: i32) -> Option<String> {
    if elem == 0 {
        let mut bits = String::with_capacity(dim);
        for k in 0..dim {
            let byte = outline.get(at + k / 8)?;
            bits.push(if byte & (1 << (k % 8)) != 0 { '1' } else { '0' });
        }
        return Some(format!("[{bits}]"));
    }
    let width = element_width(elem);
    let mut elems = Vec::with_capacity(dim);
    for k in 0..dim {
        let s = outline.get(at + k * width..at + (k + 1) * width)?;
        elems.push(embedding_element_text(s, elem)?);
    }
    Some(format!("[{}]", elems.join(",")))
}

/// Renders one embedding element the way the reference's `ToString` does:
/// integers exactly, floats with the shortest `f32`/`f64` spelling.
fn embedding_element_text(bytes: &[u8], element_type: i32) -> Option<String> {
    Some(match element_type {
        1 => bytes.first().copied()?.to_string(),
        2 => (bytes.first().copied()? as i8).to_string(),
        3 => i16::from_le_bytes(bytes.try_into().ok()?).to_string(),
        4 => i32::from_le_bytes(bytes.try_into().ok()?).to_string(),
        5 => i64::from_le_bytes(bytes.try_into().ok()?).to_string(),
        6 => f32::from_le_bytes(bytes.try_into().ok()?).to_string(),
        7 => f64::from_le_bytes(bytes.try_into().ok()?).to_string(),
        8 | 9 => cpp_format::read_embedding_element(bytes, element_type)?.to_string(),
        _ => return None,
    })
}

/// Encodes one text value into the bytes its `.col` slot holds.
///
/// NULL is represented by `None` (the caller writes a zeroed slot and clears
/// the null bit).
/// Encodes a full-text chunk the way the **reference** writes it: the `.dic`
/// (the term meta region plus the FST), the `.pos` (each term's doc list
/// followed by its position list) and the `.len` (the per-document column
/// lengths).
///
/// The layout is the reference's own:
///
/// * `postings` maps a term to the `(document ordinal, position)` pairs it
///   occurs at, and `lengths` holds each document's token count.
/// * The index is created with `OPTION_FLAG_ALL` (`index_full_text.cppm:73`):
///   the term meta carries a total term frequency and a payload, the doc list
///   carries a per-document frequency and the position list is present. A
///   chunk written with a different flag is read with the index's own flag, so
///   the reader would walk the wrong fields and find no document.
/// * The doc list and the position list are the reference's own encoders
///   (`DocListEncoder::Dump` / `PositionListEncoder::Dump`): a skip-list
///   header plus the row-wise byte slice.
/// * The term meta's offsets are byte positions **inside the `.pos` file**:
///   the doc list starts at `doc_start`, the position list at `pos_start` and
///   the term's postings end at `pos_end` (`PostingWriter::Dump`,
///   `posting_writer_impl.cpp:52-63`). The reference slices the file by them
///   (`disk_segment_reader_impl.cpp:126`), so each offset has to be taken
///   **after** the preceding part has been appended.
pub fn encode_fulltext_chunk(
    postings: &std::collections::BTreeMap<String, Vec<(u32, u32)>>,
    lengths: &[u32],
) -> (Vec<u8>, Vec<u8>, Vec<u8>) {
    const OPTION_FLAG_ALL: u8 = 1 | 2 | 4 | 8 | 16 | 32;
    let flags = infinity_storage::inverted_format::TermMetaFlags {
        has_term_frequency: true,
        has_term_payload: true,
    };
    let doc_option =
        infinity_storage::inverted_format::doc_list::DocListFormatOption::new(OPTION_FLAG_ALL);
    let mut pos = Vec::new();
    let mut metas = Vec::with_capacity(postings.len());
    let terms: Vec<(String, Vec<(u32, u32)>)> = postings
        .iter()
        .map(|(k, v)| (k.clone(), v.clone()))
        .collect();
    for (_, docs) in &terms {
        let mut doc_encoder =
            infinity_storage::inverted_format::doc_list::DocListEncoder::new(doc_option);
        let mut pos_encoder =
            infinity_storage::inverted_format::position_list::PositionListEncoder::new(
                OPTION_FLAG_ALL,
            );
        for (doc, position) in docs {
            doc_encoder.add_position();
            pos_encoder.add_position(*position);
            doc_encoder.end_document(*doc, lengths[*doc as usize], 0);
            pos_encoder.end_document();
        }
        let doc_start = pos.len() as u64;
        let doc_bytes = doc_encoder.dump(false);
        pos.extend_from_slice(&doc_bytes);
        let pos_start = pos.len() as u64;
        let pos_bytes = pos_encoder.dump(false);
        pos.extend_from_slice(&pos_bytes);
        let pos_end = pos.len() as u64;
        metas.push(infinity_storage::inverted_format::TermMeta {
            doc_freq: doc_encoder.get_df(),
            // The total term frequency counts the occurrences, not the
            // documents (`DocListEncoder::AddPosition`).
            total_tf: doc_encoder.get_total_tf(),
            payload: 0,
            doc_start,
            pos_start,
            pos_end,
        });
    }
    let (region, offsets) =
        infinity_storage::inverted_format::encode_term_meta_region_with_offsets(&metas, flags);
    let mut fst = infinity_storage::inverted_format::FstBuilder::new();
    for (i, (term, _)) in terms.iter().enumerate() {
        fst.insert(term.as_bytes(), offsets[i]);
    }
    let dic = infinity_storage::inverted_format::encode_dic(&region, &fst.finish());
    let len = infinity_storage::inverted_format::encode_column_lengths(lengths);
    (dic, pos, len)
}

/// The text a cell is encoded from.
///
/// A float is rendered with `Display` - the shortest form that parses back to
/// the same bits - because the block stores the raw value: `to_slt_string`'s
/// six decimals would round the value (and the bytes) away from the reference's.
fn mirror_cell_text(value: &Value) -> String {
    match value {
        // A float cell has to stay recognisable as one: `format!("{f}")` renders
        // `4.0` as `4`, and the reader of this text (`parse_value`) answers an
        // `Int` for that, so a FLOAT column with an integral value came back as
        // an integer - and printed `4` where the reference prints `4.000000`.
        // `exact_float` keeps the shortest round-tripping digits and leaves the
        // `.0` in place.
        Value::Float(f) => crate::exact_float(*f),
        other => other.to_slt_string(),
    }
}

fn encode_value(type_text: &str, column_type: i32, value: &str) -> Result<Option<Vec<u8>>> {
    if value.eq_ignore_ascii_case("null") {
        return Ok(None);
    }
    let bad = || {
        Error::Invalid(format!(
            "value '{value}' is not valid for column type {column_type}"
        ))
    };
    if let Some((dim, elem)) = embedding_shape(type_text) {
        let inner = value.trim().trim_start_matches('[').trim_end_matches(']');
        if elem == 0 {
            // BIT: the engine (and the C++ `ToString`, `embedding_type.h:253-262`)
            // renders the vector as a bit string like `[10110001]`; also accept
            // the comma-separated numeric form. Pack to `ceil(dim/8)` bytes with
            // value `j` at byte `j/8`, bit `j%8` (LSB-first).
            let bits: Vec<bool> = if inner.contains(',') {
                inner
                    .split(',')
                    .filter_map(|x| x.trim().parse::<f64>().ok())
                    .map(|v| v != 0.0)
                    .collect()
            } else {
                inner.chars().map(|c| c == '1').collect()
            };
            let mut bytes = vec![0u8; embedding_size(elem, dim)];
            for k in 0..dim.min(bits.len()) {
                if bits[k] {
                    bytes[k / 8] |= 1 << (k % 8);
                }
            }
            return Ok(Some(bytes));
        }
        // `[a,b,c]` → `dim` elements, zero-padded/truncated, raw little-endian.
        let nums: Vec<f64> = inner
            .split(',')
            .filter_map(|x| x.trim().parse::<f64>().ok())
            .collect();
        let mut bytes = Vec::with_capacity(embedding_size(elem, dim));
        for k in 0..dim {
            push_embedding_element(&mut bytes, elem, nums.get(k).copied().unwrap_or(0.0));
        }
        return Ok(Some(bytes));
    }
    let bytes = match column_type {
        0 => {
            // `pack_column`'s compact-bit path keys off these textual forms.
            if value.eq_ignore_ascii_case("true") || value == "1" {
                b"1".to_vec()
            } else {
                b"0".to_vec()
            }
        }
        1 => vec![value.trim().parse::<i8>().map_err(|_| bad())? as u8],
        2 => value
            .trim()
            .parse::<i16>()
            .map_err(|_| bad())?
            .to_le_bytes()
            .to_vec(),
        3 => value
            .trim()
            .parse::<i32>()
            .map_err(|_| bad())?
            .to_le_bytes()
            .to_vec(),
        4 => value
            .trim()
            .parse::<i64>()
            .map_err(|_| bad())?
            .to_le_bytes()
            .to_vec(),
        5 => value
            .trim()
            .parse::<f32>()
            .map_err(|_| bad())?
            .to_le_bytes()
            .to_vec(),
        6 => value
            .trim()
            .parse::<f64>()
            .map_err(|_| bad())?
            .to_le_bytes()
            .to_vec(),
        // The 16-bit floats: an IEEE half and a bfloat whose raw bits are the
        // high half of an `f32` (`bfloat16_t`).
        7 => crate::f32_to_f16_bits(value.trim().parse::<f32>().map_err(|_| bad())?)
            .to_le_bytes()
            .to_vec(),
        8 => ((value.trim().parse::<f32>().map_err(|_| bad())?.to_bits() >> 16) as u16)
            .to_le_bytes()
            .to_vec(),
        16 => date_to_days(value).ok_or_else(bad)?.to_le_bytes().to_vec(),
        17 => time_to_secs(value).ok_or_else(bad)?.to_le_bytes().to_vec(),
        18 => {
            let (d, secs) = datetime_to_pair(value).ok_or_else(bad)?;
            let mut v = d.to_le_bytes().to_vec();
            v.extend_from_slice(&secs.to_le_bytes());
            v
        }
        // Json is stored as BSON, not text: the reference writes
        // `JsonManager::to_bson` - the value wrapped in `{"data": …}`, keys
        // sorted, small integers as `int32` (`json_manager_impl.cpp:139`).
        _ if type_text.trim().eq_ignore_ascii_case("json") => infinity_common::json::parse(value)
            .ok_or_else(bad)?
            .to_bson(),
        // Varchar keeps its UTF-8 bytes.
        _ => value.as_bytes().to_vec(),
    };
    Ok(Some(bytes))
}

/// Decodes a slot back into the text form the engine renders.
fn decode_value(
    type_text: &str,
    column_type: i32,
    body: &[u8],
    row: usize,
    outline: &[u8],
) -> Option<String> {
    decode_value_shaped(
        type_text,
        column_type,
        body,
        row,
        outline,
        embedding_shape(type_text),
    )
}

/// [`decode_value`] with the `EMBEDDING`/`VECTOR` shape already parsed.
///
/// The shape belongs to the column, not the cell: a scan parses it once per
/// column (`decode_rows_of`) instead of once per cell, which also removes the
/// uppercased copy of the type text that `embedding_shape` allocates.
fn decode_value_shaped(
    type_text: &str,
    column_type: i32,
    body: &[u8],
    row: usize,
    outline: &[u8],
    shape: Option<(usize, i32)>,
) -> Option<String> {
    if let Some((dim, elem)) = shape {
        if elem == 0 {
            // BIT: emit the comma-separated form the engine's SQL accepts
            // (its *display* form is the bit string `[10110001]`, which
            // `encode_value` also accepts). Bit `j` at byte `j/8`, bit `j%8`.
            let at = row * embedding_size(elem, dim);
            let mut parts = String::with_capacity(dim);
            for k in 0..dim {
                let byte = body.get(at + k / 8)?;
                parts.push(if byte & (1 << (k % 8)) != 0 { '1' } else { '0' });
            }
            return Some(format!("[{parts}]"));
        }
        // Inline `dim` elements per row.
        let width = element_width(elem);
        let at = row * dim * width;
        let mut parts = Vec::with_capacity(dim);
        for k in 0..dim {
            let s = body.get(at + k * width..at + (k + 1) * width)?;
            parts.push(embedding_element_text(s, elem)?);
        }
        return Some(format!("[{}]", parts.join(",")));
    }
    match layout_of(type_text, column_type)? {
        ColumnLayout::CompactBit => {
            let bit = body.get(row / 8).map(|b| b & (1 << (row % 8)) != 0)?;
            // The engine renders booleans as `true`/`false`.
            Some(if bit { "true".into() } else { "false".into() })
        }
        ColumnLayout::Fixed(width) => {
            let at = row * width;
            let slot = body.get(at..at + width)?;
            Some(match column_type {
                1 => (slot[0] as i8).to_string(),
                2 => i16::from_le_bytes(slot.try_into().unwrap()).to_string(),
                3 => i32::from_le_bytes(slot.try_into().unwrap()).to_string(),
                4 => i64::from_le_bytes(slot.try_into().unwrap()).to_string(),
                // The cell text has to stay a *float*: `parse_value` answers an
                // `Int` for a spelling without a decimal point, so a FLOAT
                // column holding `0` would print `0` where the reference
                // prints `0.000000`. `exact_float` keeps the shortest
                // round-tripping digits and leaves the `.0` in place, so a
                // rewrite of the block reproduces the same bits.
                5 => crate::exact_float(f64::from(f32::from_le_bytes(
                    slot.try_into().unwrap(),
                ))),
                6 => crate::exact_float(f64::from_le_bytes(slot.try_into().unwrap())),
                // The 16-bit floats are two bytes wide and render like the
                // other floats: an IEEE half (`float16_t`) and a bfloat whose
                // raw bits are the high half of an `f32` (`bfloat16_t`).
                7 => crate::exact_float(f64::from(crate::f16_bits_to_f32(
                    u16::from_le_bytes(slot.try_into().unwrap()),
                ))),
                8 => crate::exact_float(f64::from(f32::from_bits(
                    (u16::from_le_bytes(slot.try_into().unwrap()) as u32) << 16,
                ))),
                // Date/time values are stored as integers but must come back as
                // text, matching how the engine renders them.
                16 => cpp_format::days_to_date(i32::from_le_bytes(slot.try_into().unwrap())),
                17 => cpp_format::secs_to_time(i32::from_le_bytes(slot.try_into().unwrap())),
                18 => {
                    let d = i32::from_le_bytes(slot[..4].try_into().unwrap());
                    let t = i32::from_le_bytes(slot[4..8].try_into().unwrap());
                    format!(
                        "{} {}",
                        cpp_format::days_to_date(d),
                        cpp_format::secs_to_time(t)
                    )
                }
                _ => return None,
            })
        }
        ColumnLayout::VarLen(16) => {
            // Varchar/Json (`varchar.h`): word 0 starts with the 24-bit length.
            // A payload of at most `VARCHAR_INLINE_LENGTH` (13) is inlined in the
            // remaining 13 bytes; a longer one lives in the outline and the slot
            // carries the 5-byte prefix plus the offset.
            let at = row * 16;
            let w0 = u64::from_le_bytes(body.get(at..at + 8)?.try_into().ok()?);
            let len = (w0 & 0x00ff_ffff) as usize;
            let bytes = if len <= 13 {
                &body.get(at + 3..at + 16)?[..len]
            } else {
                let offset =
                    u64::from_le_bytes(body.get(at + 8..at + 16)?.try_into().ok()?) as usize;
                outline.get(offset..offset + len)?
            };
            // Json cells are BSON (`JsonManager::to_bson`), so a JSON column is
            // rendered back to its text form; varchar is already text.
            if type_text.trim().eq_ignore_ascii_case("json") {
                Some(match infinity_common::json::Json::from_bson(bytes) {
                    Some(json) => json.compact(),
                    None => String::from_utf8_lossy(bytes).to_string(),
                })
            } else {
                Some(String::from_utf8_lossy(bytes).to_string())
            }
        }
        ColumnLayout::VarLen(_) => None,
    }
}


/// The `Value` of a fixed-width scalar slot, or `None` for a type this does not
/// decode (`DATE`/`TIME`/`DATETIME`, whose cell is rendered text).
fn decode_scalar_value(
    column_type: i32,
    width: usize,
    body: &[u8],
    row: usize,
) -> Option<Value> {
    let at = row.checked_mul(width)?;
    let slot = body.get(at..at + width)?;
    Some(match column_type {
        1 => Value::Int(slot[0] as i8 as i64),
        2 => Value::Int(i16::from_le_bytes(slot.try_into().ok()?) as i64),
        3 => Value::Int(i32::from_le_bytes(slot.try_into().ok()?) as i64),
        4 => Value::Int(i64::from_le_bytes(slot.try_into().ok()?)),
        5 => Value::Float(f64::from(f32::from_le_bytes(slot.try_into().ok()?))),
        6 => Value::Float(f64::from_le_bytes(slot.try_into().ok()?)),
        7 => Value::Float(f64::from(crate::f16_bits_to_f32(u16::from_le_bytes(
            slot.try_into().ok()?,
        )))),
        8 => Value::Float(f64::from(f32::from_bits(
            (u16::from_le_bytes(slot.try_into().ok()?) as u32) << 16,
        ))),
        _ => return None,
    })
}

/// [`crate::parse_value`] for a cell the decoder already owns, so a text cell
/// moves into `Value::Str` instead of being copied into it.
fn parse_owned_value(cell: String) -> Value {
    if let Ok(i) = cell.parse::<i64>() {
        Value::Int(i)
    } else if let Ok(f) = cell.parse::<f64>() {
        Value::Float(f)
    } else if cell.eq_ignore_ascii_case("null") {
        Value::Null
    } else if cell.eq_ignore_ascii_case("true") {
        Value::Bool(true)
    } else if cell.eq_ignore_ascii_case("false") {
        Value::Bool(false)
    } else {
        Value::Str(cell)
    }
}

/// The IVF index options the mirror has to honour
/// (`IndexIVFCentroidOption` + `IndexIVFStorageOption`).
struct IvfOptions {
    storage: String,
    sq_bits: u8,
    pq_subspace_num: usize,
    pq_bits: u8,
    ratio: f32,
    min_points: usize,
}

/// The types the reference stores in a variable-length buffer
/// (`ColumnVector::GetVectorBufferType` -> `kVarBuffer`): the data of such a
/// column is preceded by an outline file, `col_{id}_out`.
fn is_var_buffer_type(type_code: i32) -> bool {
    use infinity_catalog::value_format::logical;
    matches!(
        type_code as i64,
        logical::VARCHAR
            | logical::ARRAY
            | logical::EMBEDDING
            | logical::TENSOR
            | logical::TENSORARRAY
            | logical::SPARSE
            | logical::MULTIVECTOR
            | logical::JSON
    )
}

impl Engine {
    /// Root of the C++-format block mirror. The C++ engine keeps the table
    /// data under `<data_dir>/data/db_{db}/…`, next to `<data_dir>/catalog`.
    pub(crate) fn blocks_root(&self) -> PathBuf {
        self.dir.join("data")
    }

    /// Qualifies an unqualified table name with the session database, which is
    /// how the catalog and row keys are spelled.
    pub(crate) fn qualified_table(&self, table: &str) -> String {
        if table.contains('.') {
            table.to_string()
        } else {
            format!("{}.{}", self.current_db.lock().unwrap(), table)
        }
    }

    /// The rows a mirror rewrite should write, in row-id order.
    ///
    /// The block files are the authoritative copy of every row the current
    /// mirror covers, so a rewrite takes those rows from the blocks and only
    /// the rows appended since (or never mirrored) from the row journal. That
    /// keeps a commit-time re-mirror idempotent: it must not copy the journal
    /// back over cells the blocks already hold.
    /// One block's rows: the journal's values for the ids in the block's
    /// window, with the block already on disk winning for the ids it holds.
    ///
    /// The window is a contiguous run of `DEFAULT_BLOCK_CAPACITY` row ids
    /// (`segment << 32 | block * capacity + n`), which is exactly what the
    /// reference's block covers. Reading it this way is what bounds the mirror's
    /// memory: a table is mirrored block by block, the way the reference streams
    /// the rows through its buffer manager.
    fn mirror_block_rows(
        &self,
        table: &str,
        meta: &infinity_catalog::TableMeta,
        types: &[i32],
        segment_id: u32,
        block_index: u16,
        start: u64,
        end: u64,
        extra: &[(usize, Vec<Value>)],
    ) -> Result<Vec<(usize, Vec<Value>)>> {
        let first = ((segment_id as usize) << 32) | start as usize;
        let last = ((segment_id as usize) << 32) | end as usize;
        let mut journal: std::collections::BTreeMap<usize, Vec<Value>> = self
            .fetch_rows_including_deleted_range(table, first, last)?
            .into_iter()
            .collect();
        // The mirror is the authoritative copy of the rows it already holds
        // (`try_cpp_rows` prefers it for the same reason): a re-mirror must not
        // copy a lossy journal back over cells the blocks already have.
        let id = BlockId {
            db_id: self.database_id_of(table),
            table_id: meta.id,
            segment_id,
            block_id: block_index,
        };
        let root = self.blocks_root();
        // A rewrite (a column change) replaces every cell, so the files must
        // not win for the ids it carries.
        let rewriting = self
            .tables
            .lock()
            .unwrap()
            .get(&self.table_key(table))
            .map(|e| e.rewrite_rows)
            .unwrap_or(false);
        let held = if rewriting {
            Vec::new()
        } else {
            self.decode_block_cells(table, &root, meta, types, id, None)
                .unwrap_or_default()
        };
        let mut out = Vec::with_capacity(held.len().max(journal.len()));
        for (seq, cells) in held {
            journal.remove(&seq);
            out.push((seq, cells.iter().map(|cell| parse_value(cell)).collect()));
        }
        // Rows the block does not hold yet: what a replay put in the journal,
        // and what this commit is publishing (`AppendInBlock`).
        out.extend(journal);
        let held_ids: std::collections::HashSet<usize> = out.iter().map(|(seq, _)| *seq).collect();
        for (seq, values) in extra {
            if *seq >= first && *seq < last && !held_ids.contains(seq) {
                out.push((*seq, values.clone()));
            }
        }
        out.sort_unstable_by_key(|(seq, _)| *seq);
        Ok(out)
    }

    /// Writes `table`'s rows into `{data_dir}/data/db_{db}/tbl_{id}/seg_0/…`
    /// in the C++ block layout, returning the number of rows written.
    ///
    /// Rows are chunked `DEFAULT_BLOCK_CAPACITY` (8192) per block, as C++ does.
    pub fn write_cpp_blocks(&self, table: &str) -> Result<usize> {
        self.write_cpp_blocks_bounded(table, self.checkpoint_ts())
    }

    /// [`Self::write_cpp_blocks`] with the checkpoint's upper bound spelled out.
    ///
    /// The reference's checkpoint writes the rows of the transactions that had
    /// finished at its `begin_ts` (`NewTxn::Checkpoint` -> `GetRowCnt1` ->
    /// `BlockVersion::GetRowCount(begin_ts)`), so a row id handed out by a
    /// transaction still in flight stays out of the files.
    pub fn write_cpp_blocks_bounded(&self, table: &str, checkpoint_ts: u64) -> Result<usize> {
        self.write_cpp_blocks_mode(table, &[], true, checkpoint_ts)
    }

    /// [`Self::write_cpp_blocks`] with the rows a commit is publishing.
    ///
    /// `extra` is `(row id, values)` for the rows of the transaction that just
    /// committed: the reference's `AppendInBlock` writes them into the block as
    /// part of the commit. With no rows to publish this is a whole-table
    /// rewrite (a checkpoint, a restore).
    pub(crate) fn write_cpp_blocks_extra(
        &self,
        table: &str,
        extra: &[(usize, Vec<Value>)],
    ) -> Result<usize> {
        // An explicit rewrite carries the rows the caller wants written (a
        // column change re-encodes every cell), so nothing is clipped.
        self.write_cpp_blocks_mode(table, extra, extra.is_empty(), u64::MAX)
    }

    /// `full` rewrites every block; otherwise only the blocks this commit
    /// touched - the ones `extra`'s row ids land in and the ones that hold a
    /// slot the table's delete log marks (`AppendInBlock` / `DeleteInBlock`,
    /// `new_txn_data_impl.cpp:923` / `:1919`).
    fn write_cpp_blocks_mode(
        &self,
        table: &str,
        extra: &[(usize, Vec<Value>)],
        full: bool,
        checkpoint_ts: u64,
    ) -> Result<usize> {
        let table = self.qualified_table(table);
        let meta = self
            .storage
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let types: Vec<i32> = meta
            .types
            .iter()
            .map(|t| {
                column_type_of(t).ok_or_else(|| {
                    Error::Invalid(format!(
                        "column type '{t}' is not supported by the C++ block writer"
                    ))
                })
            })
            .collect::<Result<_>>()?;
        let layouts: Vec<ColumnLayout> = meta
            .types
            .iter()
            .zip(&types)
            .map(|(t, ct)| {
                layout_of(t, *ct)
                    .ok_or_else(|| Error::Invalid(format!("no .col layout for column type '{t}'")))
            })
            .collect::<Result<_>>()?;

        // Every row the table ever wrote, *including* the deleted ones: the
        // reference's block is append-only, so a deleted row's cells stay in
        // the block and only the `version` file's `deleted_` array hides it
        // (`BlockVersion::Delete`). An UPDATE is a delete plus an append, so
        // its rewritten row sits at the end with a fresh id.
        //
        // The rows come one **block** at a time (`mirror_block_rows`), the way
        // the reference streams them through its buffer manager, so the
        // mirror's memory is a block and not a table. Rows the previous mirror
        // already covers keep their *block* values: the mirror is the
        // authoritative copy for them (a table can be re-mirrored on every
        // commit, and the row journal is allowed to be a lossy cache of the
        // same rows - `try_cpp_rows` prefers the blocks for exactly that
        // reason). New rows come from the journal.
        let mut block_rows: Vec<usize> = Vec::new();
        // `BlockVersion::Append` boundaries: one `create` per append, sized by
        // how many of that append's rows land in the block. Each carries the
        // append transaction's commit ts, like the reference
        // (`BlockVersion::Append(commit_ts, …)`, `new_txn_data_impl.cpp:957`).

        // Deleted row id -> the commit ts of the transaction that deleted it,
        // so the version file marks the block slot that holds it
        // (`BlockVersion::Delete`). A delete that commits after the checkpoint's
        // bound is not part of this write: the row is still visible at that ts
        // (`BlockVersion::save_to_file` drops the same entries).
        let delete_ts = self.delete_ts(&table);
        let deleted_at: std::collections::HashMap<usize, u64> = self
            .deleted_ids(&table)
            .iter()
            .enumerate()
            .map(|(k, id)| {
                let ts = delete_ts.get(k).copied().unwrap_or(0);
                (*id as usize, if ts == 0 { DELETED_MIRROR_TS } else { ts })
            })
            .filter(|(_, ts)| checkpoint_ts == u64::MAX || *ts <= checkpoint_ts)
            .collect();
        let db_id = self.database_id_of(&table);
        let root = self.blocks_root();
        let tkey = self.table_key(&table);
        // Everything the commits since the last checkpoint appended is in the
        // table's append buffer; this write is the checkpoint, so it takes them
        // all. `extra` carries the rows of a rewrite (a column change, a
        // replay, a compaction) that no commit buffered.
        // A checkpoint writes every block, so nothing is "untouched".
        let touched: std::collections::HashSet<(u32, u16)> = std::collections::HashSet::new();
        let layout_total: u64 = self
            .segment_row_counts(&table)
            .iter()
            .map(|(_, rows)| *rows)
            .sum();
        // Nothing moved since the files were written: they are already the
        // newest copy, and re-encoding them would size the NULL bitmap from the
        // final row set instead of the append window the file was written with.
        let dirty = {
            let tables = self.tables.lock().unwrap();
            tables.get(&tkey).map(|e| e.mirror_dirty).unwrap_or(false)
        };
        if !dirty
            && self
                .block_index_for(&table)
                .map(|index| !index.lock().unwrap().is_empty())
                .unwrap_or(false)
        {
            return Ok(layout_total as usize);
        }
        // The write counter as it stands before any row is read: a commit that
        // lands while the mirror is being written bumps it, and the flag is only
        // cleared when nothing did (otherwise the new rows would look covered).
        //
        // `mirrored` is how many rows the files held after the last write. A
        // commit can only be *incremental* when that is known (a directory
        // opened afresh has no count until the first rewrite) and the table's
        // row set is the previous one plus this batch - otherwise another
        // transaction reserved ids that are not on disk yet, and the read side
        // has to keep merging the journal.
        let epoch = {
            let mut tables = self.tables.lock().unwrap();
            let entry = tables.entry(tkey.clone()).or_default();
            entry.block_index = None;
            entry.cpp_block_rows = None;
            entry.mirror_epoch
        };
        // A checkpoint writes the whole table: the block files are the durable
        // copy and there is no in-place append to them any more.
        let partial = false;
        let _ = full;
        // The mirror's file writes are serialised; the table lock is **not**
        // held across them (a commit takes the bottom gate before this, and the
        // checkpoint takes the table lock on its own, so holding both here in
        // the other order deadlocks the two threads).
        let _serialised = MIRROR_WRITE_LOCK
            .lock()
            .unwrap_or_else(|poisoned| poisoned.into_inner());
        // Taken under the lock: a checkpoint that queued behind this one must
        // not see a stale snapshot of the append buffer, or its own rows would
        // be dropped from the files by the write that lands last.
        let buffered = {
            let tables = self.tables.lock().unwrap();
            tables
                .get(&tkey)
                .map(|e| e.pending_rows.clone())
                .unwrap_or_default()
        };
        let mut merged: Vec<(usize, Vec<Value>)> = extra.to_vec();
        if !buffered.is_empty() {
            let have: std::collections::HashSet<usize> =
                merged.iter().map(|(seq, _)| *seq).collect();
            merged.extend(buffered.into_iter().filter(|(seq, _)| !have.contains(seq)));
        }
        let extra: &[(usize, Vec<Value>)] = &merged;
        let mut written = 0usize;
        // A row id carries its segment in the high 32 bits (`segment_id << 32 |
        // in-segment offset`). The reference writes one `seg_{id}/` directory
        // per segment, with the block ids restarting at 0, and a segment's
        // blocks tile `[0, rows)` - so the allocator's per-segment counts are
        // enough to know every block's id window without reading the table.
        let mut segments: Vec<CppSegment> = Vec::new();
        let mut global_pos = 0usize;
        // The append log and the segment map move under other commits: read
        // them under the write lock, or a checkpoint that queued behind another
        // one records `create` boundaries that stop short of the rows it wrote.
        let totals = self.append_totals(&table);
        let append_ts = self.append_ts(&table);
        let mut counts = self.segment_row_counts(&table);
        if counts.is_empty() && !merged.is_empty() {
            // The allocator's per-segment map is per-process; a table the
            // catalog just re-registered (a DDL, a rename) has none. The rows
            // the commit buffered still say how far each segment goes.
            let mut by_segment: std::collections::BTreeMap<u32, u64> = Default::default();
            for (seq, _) in &merged {
                let segment = (*seq >> 32) as u32;
                let offset = (*seq as u64 & 0xffff_ffff) + 1;
                by_segment
                    .entry(segment)
                    .and_modify(|e| *e = (*e).max(offset))
                    .or_insert(offset);
            }
            counts = by_segment.into_iter().collect();
        }
        counts.sort_unstable();
        // Every segment the table holds **before** the clip below. A commit that
        // landed after this write's bound has no block in it yet, and its
        // directory must survive the sweep that drops segments the rewrite did
        // not produce - otherwise a checkpoint that started before an import
        // committed deleted the block the import had just written, and the
        // segment was left with rows and no file.
        let known_segments: Vec<u32> = counts.iter().map(|(segment, _)| *segment).collect();
        // `NewTxn::Checkpoint` writes the rows of the transactions that had
        // finished by `checkpoint_ts`: a block holds the rows whose append was
        // stamped at or before it (`GetRowCnt1` -> `GetRowCount(begin_ts)`).
        // The allocator's per-segment count also covers ids handed out to a
        // transaction that has not committed yet, and those rows are in no
        // block, so the extent is clipped here. Because ids are allocated in
        // commit-ts order, the committed records are a prefix of the row ids.
        if checkpoint_ts != u64::MAX {
            let committed = self.committed_rows(&table, checkpoint_ts);
            let mut seen = 0usize;
            let mut clipped: Vec<(u32, u64)> = Vec::with_capacity(counts.len());
            for (segment_id, rows) in counts {
                let take = committed.saturating_sub(seen).min(rows as usize);
                seen += rows as usize;
                if take > 0 {
                    clipped.push((segment_id, take as u64));
                }
            }
            counts = clipped;
        }
        for (segment_id, seg_rows_total) in counts {
            let seg_start = global_pos;
            global_pos += seg_rows_total as usize;
            let mut blocks: Vec<CppBlock> = Vec::new();
            // The ts the segment was created with: the commit ts of the append
            // that opened it (`TableMeta::AddSegmentWithID(commit_ts, …)`).
            let mut seg_create_ts = CPP_MIRROR_TS;
            // The reference's prefilter (`BuildFastRoughFilterTask`): one per
            // block and one per segment, built when the segment is **sealed**.
            // An import seals every segment it commits
            // (`PrepareCommitImport` -> `ExecuteOnNewSealedSegment`), an
            // `INSERT` only seals one that reaches `DEFAULT_SEGMENT_CAPACITY`
            // (`CommitBottomAppend`), and an unsealed segment has no filter at
            // all. A whole-table rewrite can leave fewer blocks than the
            // previous one, so it starts clean; a commit only rebuilds the tags
            // of the block it wrote.
            let block_count = (seg_rows_total as usize).div_ceil(DEFAULT_BLOCK_CAPACITY);
            let sealed = self
                .imported_extent(&table, segment_id, seg_rows_total)
                .is_some()
                || seg_rows_total as usize == Self::SEGMENT_ROW_CAPACITY;
            // A segment that is sealed by *this* write needs the prefilter of
            // every one of its blocks (`ExecuteOnNewSealedSegment` walks the
            // whole segment), so that write falls back to rewriting the
            // segment.
            let last_block = block_count.saturating_sub(1) as u16;
            let seg_partial =
                partial && !(sealed && touched.contains(&(segment_id, last_block)));
            for (k, _) in self
                .storage
                .catalog
                .store()
                .scan_prefix(format!("blk|{}|{}|{}|", db_id, meta.id, segment_id).as_bytes())?
            {
                if seg_partial {
                    let block_id = String::from_utf8_lossy(&k)
                        .split('|')
                        .nth(5)
                        .and_then(|s| s.parse::<u16>().ok());
                    if block_id.is_some_and(|b| {
                        (b as usize) < block_count && !touched.contains(&(segment_id, b))
                    }) {
                        continue;
                    }
                }
                self.storage.catalog.store().delete(&k)?;
            }
            let mut seg_filters = crate::fast_rough_filter::Filters::new(&meta.types);
            // Drain the segment block by block so each block's values are freed
            // once it is encoded: holding the whole table's decoded rows while
            // the texts are rendered doubled the transient on a long import.
            for block_index in 0..block_count {
                if seg_partial && !touched.contains(&(segment_id, block_index as u16)) {
                    // Untouched: its files and its `version` already say
                    // everything this commit would write, so only its catalog
                    // record is republished, with the ts it already carries.
                    let ts = self
                        .storage
                        .catalog
                        .store()
                        .get(
                            infinity_storage::catalog_format::catalog_block_key(
                                &db_id.to_string(),
                                &meta.id.to_string(),
                                segment_id,
                                block_index as u16,
                            )
                            .as_bytes(),
                        )?
                        .and_then(|v| String::from_utf8_lossy(&v).parse::<u64>().ok())
                        .unwrap_or(CPP_MIRROR_TS);
                    if block_index == 0 {
                        seg_create_ts = ts;
                    }
                    blocks.push(CppBlock {
                        block_id: block_index as u16,
                        commit_ts: ts,
                    });
                    continue;
                }
                // `block_start` stays in the table-wide append order the
                // version batches are recorded in; the slot a deleted row
                // occupies is inside its own segment's block.
                let start = block_index * DEFAULT_BLOCK_CAPACITY;
                let end = (start + DEFAULT_BLOCK_CAPACITY).min(seg_rows_total as usize);
                let slot_base = seg_start + start;
                // Drop the buffer manager's copy of this block **before** the
                // rows are merged: a cached decode of an older (shorter) file
                // would make the merge miss the rows the file holds, and the
                // rewrite would drop them.
                let early_id = BlockId {
                    db_id,
                    table_id: meta.id,
                    segment_id,
                    block_id: block_index as u16,
                };
                self.context.buffer.lock().unwrap().invalidate(&early_id);
                let chunk = self.mirror_block_rows(
                    &table,
                    &meta,
                    &types,
                    segment_id,
                    block_index as u16,
                    start as u64,
                    end as u64,
                    extra,
                )?;
                // A block is addressed by slot: row *n* of block *b* is id
                // `segment << 32 | b * capacity + n`. The window is the
                // committed prefix, and ids are handed out in commit-ts order
                // (`SubmitForAllocation`), so the chunk must tile it exactly;
                // anything else means the row buffer lost a row, which must not
                // be written to disk as a shifted block.
                let first_id = ((segment_id as usize) << 32) | start;
                let contiguous = chunk.len() == end - start
                    && chunk
                        .iter()
                        .enumerate()
                        .all(|(slot, (seq, _))| *seq == first_id + slot);
                if !contiguous {
                    return Err(Error::Invalid(format!(
                        "table '{table}': block {segment_id}.{block_index} holds {} of the {} rows \
                         committed at ts {checkpoint_ts} (ids {}..{}, window {first_id}..{})",
                        chunk.len(),
                        end - start,
                        chunk.first().map(|(seq, _)| *seq).unwrap_or(0),
                        chunk.last().map(|(seq, _)| *seq).unwrap_or(0),
                        first_id + (end - start)
                    )));
                }
                block_rows.extend(
                    chunk
                        .iter()
                        .map(|(seq, _)| *seq)
                        .filter(|seq| !deleted_at.contains_key(seq)),
                );
                let texts: Vec<Vec<String>> = chunk
                    .iter()
                    .map(|(_, row): &(usize, Vec<Value>)| {
                        row.iter().map(mirror_cell_text).collect()
                    })
                    .collect();
                // A block opened by `COPY` keeps the reference's import-time
                // bitmap coverage (see `ColumnBody::build_with`).
                let bitmap_bits = self.block_bitmap_bits(&table, slot_base, chunk.len());
                let block_columns = self.encode_block_columns(
                    &meta.types,
                    &meta.ids,
                    &types,
                    &layouts,
                    &texts,
                    bitmap_bits,
                )?;
                let batches =
                    self.block_version_batches(&totals, &append_ts, slot_base, chunk.len());
                let mut version = BlockVersionFile::new(&batches, DEFAULT_BLOCK_CAPACITY);
                // The slot of a deleted row is its in-segment offset inside the
                // block (`BlockVersion::Delete`); the id window is the block's.
                for (seq, _) in &chunk {
                    if let Some(ts) = deleted_at.get(seq) {
                        version.delete(((*seq as u64 & 0xffff_ffff) - start as u64) as usize, *ts);
                    }
                }
                // The block's own record carries the ts of the append that
                // opened it, which is exactly the first `create` of its version
                // file (`SegmentMeta::AddBlockWithID(commit_ts, …)`).
                let block_ts = batches.first().map(|(ts, _)| *ts).unwrap_or(CPP_MIRROR_TS);
                if block_index == 0 {
                    seg_create_ts = block_ts;
                }
                let id = BlockId {
                    db_id,
                    table_id: meta.id,
                    segment_id,
                    block_id: block_index as u16,
                };
                self.context.buffer.lock().unwrap().invalidate(&id);
                let block_dir = cpp_format::block_dir(db_id, meta.id, segment_id, id.block_id);
                // A rewrite replaces the block's contents; a file the schema no
                // longer has (`ALTER TABLE … DROP COLUMN` leaves `1.col`
                // behind) must not survive next to it, or the reference reads
                // a column the catalog does not know.
                let mut keep: Vec<String> = vec![cpp_format::VERSION_FILE.to_string()];
                for (i, ty) in types.iter().enumerate() {
                    keep.push(cpp_format::column_file(meta.ids[i]));
                    if is_var_buffer_type(*ty) {
                        keep.push(cpp_format::column_outline_file(meta.ids[i]));
                    }
                }
                if let Ok(entries) = std::fs::read_dir(root.join(&block_dir)) {
                    for entry in entries.flatten() {
                        let name = entry.file_name().to_string_lossy().to_string();
                        if !keep.contains(&name) {
                            let _ = std::fs::remove_file(entry.path());
                        }
                    }
                }
                cpp_format::write_block(&root, id, &block_columns, &version).map_err(|e| {
                    Error::Io {
                        path: root.join(&block_dir),
                        source: e,
                    }
                })?;
                // The reference opens an outline file for every variable-length
                // column (`ColumnMeta::FilePaths` lists `col_{id}_out` for a
                // kVarBuffer type) and aborts the whole read when it is missing,
                // even though the file is empty for a column whose data is inline.
                for (i, ty) in types.iter().enumerate() {
                    if is_var_buffer_type(*ty) {
                        let path = root
                            .join(&block_dir)
                            .join(cpp_format::column_outline_file(meta.ids[i]));
                        if !path.exists() {
                            let _ = std::fs::write(&path, b"");
                        }
                    }
                }
                if sealed {
                    let mut block_filters = crate::fast_rough_filter::Filters::new(&meta.types);
                    for (_, cells) in &chunk {
                        block_filters.add_row(cells);
                    }
                    // `segment_meta->begin_ts()` is the creating transaction's
                    // begin ts, one below its commit ts - the same for every
                    // block of the segment.
                    let build_time = seg_create_ts.saturating_sub(1);
                    if let Err(e) = self.storage.catalog.store().put(
                        format!(
                            "blk|{}|{}|{}|{}|fast_rough_filter",
                            db_id, meta.id, segment_id, block_index
                        )
                        .as_bytes(),
                        &block_filters.encode(build_time),
                    ) {
                        eprintln!("[FRF] put failed: {e}");
                    }
                    seg_filters.merge(&block_filters);
                }
                blocks.push(CppBlock {
                    block_id: block_index as u16,
                    commit_ts: block_ts,
                });
                    written += chunk.len();
            }
            // `first_delete_ts` is the commit ts of the first delete that
            // touched this segment (`SegmentMeta::CommitFirstDeleteTS`): a
            // reader whose `begin_ts` is below it skips the block's delete
            // flags, so leaving it at UNCOMMIT_TS resurrects deleted rows.
            let segment_deleted = self
                .deleted_ids(&table)
                .iter()
                .any(|id| (*id >> 32) as u32 == segment_id);
            let first_delete_ts = if segment_deleted {
                self.delete_ts(&table)
                    .into_iter()
                    .filter(|ts| *ts != 0)
                    .min()
                    .unwrap_or(UNCOMMIT_TS)
            } else {
                UNCOMMIT_TS
            };
            if !seg_partial && sealed && seg_create_ts != CPP_MIRROR_TS {
                let _ = self.storage.catalog.store().put(
                    format!("seg|{}|{}|{}|fast_rough_filter", db_id, meta.id, segment_id)
                        .as_bytes(),
                    &seg_filters.encode(seg_create_ts.saturating_sub(1)),
                );
            }
            segments.push(CppSegment {
                segment_id,
                create_ts: seg_create_ts,
                first_delete_ts,
                blocks,
            });
        }
        // Drop the segment directories this rewrite did not produce. They are
        // removed *after* the new blocks are on disk so a concurrent reader
        // never finds the table's only block missing; a compaction renumbers
        // the rows into a new segment id, and leaving the source segment
        // behind would let the reference read a second copy of the data.
        let tbl_dir = root.join(format!("db_{db_id}/tbl_{}", meta.id));
        // A segment a compaction deprecated keeps its directory: the reference
        // only writes `drop|seg|…` and leaves the files for `Cleanup`
        // (`NewTxn::Compact` -> deprecated segment list; `Cleanup` removes
        // them). Deleting it here would make a reader at an older ts lose rows.
        let dropped = self
            .storage
            .catalog
            .cpp_dropped_segments(&db_id.to_string(), &meta.id.to_string())
            .unwrap_or_default();
        let live: Vec<String> = segments
            .iter()
            .map(|segment| format!("seg_{}", segment.segment_id))
            .chain(dropped.iter().map(|segment| format!("seg_{segment}")))
            .chain(known_segments.iter().map(|segment| format!("seg_{segment}")))
            .collect();
        if let Ok(entries) = std::fs::read_dir(&tbl_dir) {
            for entry in entries.flatten() {
                let name = entry.file_name().to_string_lossy().to_string();
                if name.starts_with("seg_") && !live.contains(&name) {
                    let _ = std::fs::remove_dir_all(entry.path());
                }
            }
        }
        // Publish the segment layer the C++ reader enumerates
        // (`catalog|seg|…` / `catalog|blk|…`, `GetTableSegments`): without it
        // the table opens with no segments and every query returns nothing.
        self.storage.catalog.publish_cpp_segments(&CppSegmentSet {
            db_id: db_id.to_string(),
            table_id: meta.id.to_string(),
            segments,
        })?;
        // The mirror has new block files; the table's block map must rescan.
        // Release the write lock before touching the table entry: the files are
        // on disk, and taking `tables` under `MIRROR_WRITE_LOCK` is what
        // inverts against the checkpoint's `tables` → mirror order.
        drop(_serialised);
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(tkey.clone()).or_default();
        entry.block_index = None;
        // A whole-table rewrite rebuilt the row set from scratch; a commit only
        // wrote the blocks it touched, so its row set is the previous one with
        // this batch's rows added and its deletes removed.
        let written_ids: std::collections::HashSet<usize> = block_rows.iter().copied().collect();
        entry.cpp_block_rows = Some(std::sync::Arc::new(block_rows));
        // What the files hold now: the row set this write walked.
        entry.mirror_rows = Some(written as u64);
        // The mark retires only when this write took **every** row it was
        // handed: a row outside the segment windows it walked (`counts` and the
        // append buffer disagreeing) is still only in memory, and a commit that
        // landed mid-flush is in the buffer too - the next read merges it.
        // Drop exactly the rows this write put in a block - `block_rows` is that
        // set, and a row the segment windows did not reach is not in it, so it
        // stays buffered for the next checkpoint. Clearing by the *input* set
        // would drop a row whose window was short; clearing the whole buffer
        // would drop the rows a concurrent commit added meanwhile.
        entry
            .pending_rows
            .retain(|(seq, _)| !written_ids.contains(seq));
        // The mark retires only when nothing is left to write and no commit
        // landed while this one ran.
        let covered = entry.mirror_epoch == epoch && entry.pending_rows.is_empty();
        entry.mirror_dirty = !covered;
        drop(tables);
        if covered {
            // The blocks now cover everything the journal holds, so a restart
            // may read them without it - the mark is what a reader consults.
            self.clear_mirror_dirty(&tkey);
        }
        Ok(written)
    }

    /// The `BlockVersion::Append` batches for one block: one `(ts, rows)` per
    /// append, sized by how many of that append's rows the block holds.
    ///
    /// `totals` is the cumulative row count after every append and `ts` the
    /// commit ts of the transaction that made it; the two are aligned
    /// (`settle_append_ts`). `totals` is a hint for the mirror rather than
    /// committed state, so when it disagrees with the rows actually mirrored
    /// (a table written before the log existed, or an append the process lost)
    /// the block gets one create for all of them, which is what the mirror
    /// always wrote.
    fn block_version_batches(
        &self,
        totals: &[u64],
        ts: &[u64],
        block_start: usize,
        rows_in_block: usize,
    ) -> Vec<(u64, u32)> {
        let end = block_start + rows_in_block;
        // An append the previous process lost, or one made before the ts log
        // existed, has no commit ts of its own. The newest known ts is the
        // closest stand-in: it keeps the create visible to a reader at the
        // current ts, and puts it where the append order says it belongs.
        let fallback = ts
            .iter()
            .rev()
            .copied()
            .find(|t| *t != 0)
            .unwrap_or(CPP_MIRROR_TS);
        let mut batches: Vec<(u64, u32)> = Vec::new();
        let mut prev = 0u64;
        for (i, &total) in totals.iter().enumerate() {
            if total <= prev {
                continue;
            }
            let lo = prev.max(block_start as u64);
            let hi = total.min(end as u64);
            if hi > lo {
                let batch_ts = match ts.get(i).copied().unwrap_or(0) {
                    0 => fallback,
                    t => t,
                };
                batches.push((batch_ts, (hi - lo) as u32));
            }
            prev = total;
            if total >= end as u64 {
                break;
            }
        }
        let sum: u64 = batches.iter().map(|(_, rows)| u64::from(*rows)).sum();
        if sum != rows_in_block as u64 {
            return vec![(fallback, rows_in_block as u32)];
        }
        batches
    }

    /// Encodes one block's rows (already rendered as text) into the column
    /// files, in the C++ layout.
    #[allow(clippy::too_many_arguments)]
    fn encode_block_columns(
        &self,
        declared: &[String],
        column_ids: &[u64],
        types: &[i32],
        layouts: &[ColumnLayout],
        chunk: &[Vec<String>],
        bitmap_bits: Option<usize>,
    ) -> Result<Vec<BlockColumn>> {
        let mut block_columns = Vec::with_capacity(types.len());
        for (col, column_type) in types.iter().enumerate() {
            let kind = outline_kind(&declared[col]);
            let mut slots: Vec<Option<Vec<u8>>> = Vec::with_capacity(chunk.len());
            // Self-outlining columns build their own outline; the generic
            // packer produces one for varchar/json.
            let mut self_outline = Vec::new();
            for row in chunk {
                let text = row.get(col).map(String::as_str).unwrap_or("");
                match kind {
                    OutlineKind::Tensor => {
                        slots.push(encode_tensor_slot(&declared[col], text, &mut self_outline)?)
                    }
                    OutlineKind::TensorArray => slots.push(encode_tensorarray_slot(
                        &declared[col],
                        text,
                        &mut self_outline,
                    )?),
                    OutlineKind::Sparse => {
                        slots.push(encode_sparse_slot(&declared[col], text, &mut self_outline)?)
                    }
                    OutlineKind::Array => {
                        slots.push(encode_array_slot(&declared[col], text, &mut self_outline)?)
                    }
                    OutlineKind::None => {
                        slots.push(encode_value(&declared[col], *column_type, text)?)
                    }
                }
            }
            let bytes: Vec<Option<&[u8]>> = slots.iter().map(|s| s.as_deref()).collect();
            let (body, packed_outline) = pack_column_with_coverage(
                *column_type,
                layouts[col],
                &bytes,
                DEFAULT_BLOCK_CAPACITY,
                bitmap_bits,
            )
            .map_err(|e| Error::Storage(format!("pack column {col}: {e}")))?;
            let outline = if kind == OutlineKind::None {
                packed_outline
            } else {
                self_outline
            };
            block_columns.push(BlockColumn {
                column_id: column_ids.get(col).copied().unwrap_or(col as u64),
                body,
                outline,
            });
        }
        Ok(block_columns)
    }

    /// Refreshes the C++ block mirror of `table` if it already has one.
    ///
    /// The reference writes a block's files while the append transaction
    /// commits (`AppendInBlock`), not at some later "flush", so a directory it
    /// leaves behind is always readable. A mirror that exists must behave the
    /// same way: a crash after a commit that only updated the catalog would
    /// leave `catalog|blk|…` records for a block with no files, and the
    /// reference aborts on that ("Failed to find object for local path
    /// …/blk_0/version", `MetaTableObject::RestoreTableCache`).
    ///
    /// A table without a mirror stays without one: its columns may not be
    /// expressible in the format, and `publish_cpp_segments` (inside
    /// [`Self::write_cpp_blocks`]) is what would make it visible to the
    /// reference in the first place.
    pub fn refresh_cpp_block_mirror(&self, table: &str) -> Result<()> {
        self.refresh_cpp_block_mirror_extra(table, &[])
    }

    /// [`Self::refresh_cpp_block_mirror`] with the rows the commit publishes.
    ///
    /// The reference writes a batch into its blocks as part of the commit
    /// (`AppendInBlock`), and the port keeps no `row:` keys for it, so the new
    /// rows have to reach the block writer directly - including the very first
    /// commit of a table, which creates the `tbl_{id}/seg_{id}/blk_{id}/`
    /// directories (the reference has no other place a row can live).
    pub(crate) fn refresh_cpp_block_mirror_extra(
        &self,
        table: &str,
        extra: &[(usize, Vec<Value>)],
    ) -> Result<()> {
        if extra.is_empty() {
            return Ok(());
        }
        let table = self.qualified_table(table);
        let meta = match self.storage.catalog.get_table_by_name(&table) {
            Ok(Some(meta)) => meta,
            // The catalog could not answer right now (a concurrent reload): the
            // rows are already committed, so they must still be buffered - the
            // checkpoint re-checks the schema before it writes anything.
            _ => {
                let tkey = self.table_key(&table);
                let mut tables = self.tables.lock().unwrap();
                let entry = tables.entry(tkey.clone()).or_default();
                entry.pending_rows.extend(extra.iter().cloned());
                entry.mirror_dirty = true;
                entry.mirror_epoch += 1;
                drop(tables);
                self.write_mirror_dirty(&tkey);
                return Ok(());
            }
        };
        if !meta.types.iter().all(|t| column_type_of(t).is_some()) {
            // No block can hold this table's rows: it keeps them in the store,
            // the way every table did before the blocks became the storage of
            // record.
            for (seq, values) in extra {
                let literals: Vec<Literal> = values
                    .iter()
                    .map(|v| Literal::Str(mirror_cell_text(v)))
                    .collect();
                let text = self.encode_row_stored(&table, &literals, &meta.types)?;
                self.storage
                    .store
                    .put(&row_key(&table, *seq), text.as_bytes())?;
            }
            return Ok(());
        }
        // No file I/O here: the rows go into the table's append buffer, which
        // is what the reference's `AppendInBlock` does to the block's
        // `BufferObject`. The block file is written by the checkpoint.
        let tkey = self.table_key(&table);
        {
            let mut tables = self.tables.lock().unwrap();
            let entry = tables.entry(tkey.clone()).or_default();
            entry.pending_rows.extend(extra.iter().cloned());
            entry.mirror_dirty = true;
            entry.mirror_epoch += 1;
        }
        self.write_mirror_dirty(&tkey);
        Ok(())
    }

    /// Mirrors every table into the C++ block layout.
    ///
    /// Called from [`Engine::flush`], so a normal clean shutdown leaves the data
    /// directory readable by the C++ engine. Tables whose columns are not yet
    /// packable (array/tensor/sparse/embedding) are skipped rather than
    /// mis-encoded.
    pub fn flush_cpp_blocks(&self, checkpoint_ts: u64) -> Result<()> {
        for meta in self.storage.catalog.list_tables()? {
            if meta.types.iter().all(|t| column_type_of(t).is_some()) {
                self.write_cpp_blocks_bounded(&meta.name, checkpoint_ts)?;
                // The mirror now covers every row; the IVF memory segment is
                // redundant and would double-count candidates.
                let key = self.table_key(&meta.name);
                let mut tables = self.tables.lock().unwrap();
                let entry = tables.entry(key).or_default();
                for rows in entry.ivf_mem.values_mut() {
                    rows.clear();
                }
            }
            // A dropped index must not leave its `idx_{id}` mirror behind.
            let db_id = self.database_id_of(&meta.name);
            let tbl_dir = self
                .blocks_root()
                .join(format!("db_{db_id}/tbl_{}", meta.id));
            // `idx_{id}` is keyed by the catalog's index id, so take it from
            // the record: going through a name lookup made the whole directory
            // look stale whenever that lookup failed (a reopened directory
            // whose index record was spelled differently), and the flush then
            // deleted a mirror it could not rebuild.
            let short = meta.name.rsplit('.').next().unwrap_or(&meta.name);
            let live: Vec<String> = self
                .storage
                .catalog
                .list_indexes()?
                .iter()
                .filter(|im| im.table == meta.name || im.table == short)
                .filter(|im| im.id > 0)
                .map(|im| format!("idx_{}", im.id))
                .collect();
            if let Ok(entries) = std::fs::read_dir(&tbl_dir) {
                for e in entries.flatten() {
                    let name = e.file_name().to_string_lossy().to_string();
                    if name.starts_with("idx_") && !live.contains(&name) {
                        let _ = std::fs::remove_dir_all(e.path());
                    }
                }
            }
        }
        // Vector indexes mirror alongside the rows. This is best-effort: an
        // index whose column is not an embedding column simply has no mirror.
        for im in self.storage.catalog.list_indexes()? {
            if im.index_type.eq_ignore_ascii_case("HNSW") {
                let _ = self.write_cpp_hnsw_index(&im.table, &im.name);
            } else if im.index_type.eq_ignore_ascii_case("FULLTEXT") {
                let _ = self.write_cpp_fulltext_index(&im.table, &im.name);
            } else if im.index_type.eq_ignore_ascii_case("PLAID") {
                let _ = self.write_cpp_plaid_index(&im.table, &im.name);
            } else if im.index_type.eq_ignore_ascii_case("IVF") {
                let _ = self.write_cpp_ivf_index(&im.table, &im.name);
            }
        }
        // Publish the chunk layer of every index mirror. The reference only
        // enumerates the chunks listed in `idx_chunk|…|chunk_info`, and a scan
        // whose chunk file is missing reads a zero-length buffer (the BMP path
        // indexes `res[block_offset]` of an empty vector and aborts). Some
        // mirrors are already in the reference's byte layout but live in their
        // own directory (`secondary/…`, `sparse/…`), so copy them into
        // `idx_{id}/seg_0/chunk_0.idx`.
        for meta in self.storage.catalog.list_tables()? {
            let _ = self.publish_cpp_index_chunks(&meta.name, meta.id);
        }
        Ok(())
    }

    /// Mirrors each index's sealed chunk into `idx_{id}/seg_0/chunk_0.idx` and
    /// records it in the catalog. Best effort per index: a mirror this engine
    /// does not have simply publishes nothing.
    /// Re-stamps the index's published `idx_chunk|…|{chunk_id}` records with
    /// `ts`, the commit timestamp of the dump transaction that just sealed it.
    ///
    /// The reference writes that record inside the dump transaction itself
    /// (`SegmentIndexMeta::AddChunkIndexID1`). The port's mirror re-publishes the
    /// whole chunk layer at checkpoint time, so a dump that happens *after* the
    /// last checkpoint must touch the records it already published; the next
    /// mirror preserves the value from [`dump_ts_key`].
    pub(crate) fn stamp_cpp_chunk_ts(&self, table: &str, index: &str, ts: u64) -> Result<()> {
        let table = self.qualified_table(table);
        let db_id = self.database_id_of(&table);
        let table_id = self
            .storage
            .catalog
            .get_table_by_name(&table)?
            .map(|m| m.id)
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let index_id = self.cpp_index_position(&table, index)?;
        let store = self.storage.catalog.store();
        let prefix = format!("idx_chunk|{db_id}|{table_id}|{index_id}|");
        let value = ts.to_string().into_bytes();
        for (key, _) in store.scan_prefix(prefix.as_bytes())? {
            let text = String::from_utf8_lossy(&key).to_string();
            // Only the id records (numeric suffix); `…|chunk_info` and
            // `…|next_chunk_id` are owned by the mirror.
            let Some(suffix) = text.rsplit('|').next() else {
                continue;
            };
            if suffix.parse::<u64>().is_ok() {
                store.put(&key, &value)?;
            }
        }
        Ok(())
    }

    /// The C++ chunk of a SMVE or EMVB index, built from the table's rows the
    /// way the reference's memory dump builds it.
    ///
    /// SMVE: `u32 width, topk, embedding_dim, n_docs`, the `dim × width`
    /// projection matrix (the reference's `mt19937(42)` sequence, so the bytes
    /// match) and the BMP index (`BMPHandler::SaveToPtr`). EMVB:
    /// `EMVBIndex::SaveIndexInner`.
    /// The C++ chunk of the table's SMVE or EMVB index, **as it was dumped**:
    /// the memory index is built when the index is created and fed by every
    /// insert, and `flush()` dumps it into the segment files - the chunk is
    /// that dump, not a rebuild from the rows.
    fn cpp_multivector_chunk(&self, table: &str, column: &str, kind: &str) -> Option<Vec<u8>> {
        let key = self.table_key(table);
        let index = {
            let tables = self.tables.lock().unwrap();
            tables.get(&key)?.mv.get(column).cloned()?
        };
        let guard = index.read().ok()?;
        if kind.eq_ignore_ascii_case("SMVE") {
            let smve = guard.smve.as_ref()?;
            let mut out = Vec::new();
            for v in [
                smve.width(),
                smve.topk(),
                smve.embedding_dim(),
                smve.chunk_row_count(),
            ] {
                out.extend_from_slice(&v.to_le_bytes());
            }
            for v in smve.projection() {
                out.extend_from_slice(&v.to_le_bytes());
            }
            let docs: Vec<infinity_storage::bmp_cpp::SparseDoc> = smve
                .docs()
                .iter()
                .map(|(row, entries)| {
                    (
                        *row as u64,
                        entries
                            .iter()
                            .map(|(term, value)| (*term as i32, *value))
                            .collect(),
                    )
                })
                .collect();
            out.extend_from_slice(&infinity_storage::bmp_cpp::encode_bmp(
                infinity_index::sparse::BMP_BLOCK_SIZE,
                infinity_storage::bmp_cpp::IdxWidth::I32,
                &docs,
                smve.width() as usize,
            ));
            Some(out)
        } else {
            let emvb = guard.tail_emvb.lock().ok()?;
            Some(emvb.as_ref()?.data.encode())
        }
    }

    fn publish_cpp_index_chunks(&self, table: &str, table_id: u64) -> Result<()> {
        let db_id = self.database_id_of(table);
        let root = self
            .blocks_root()
            .join(format!("db_{db_id}/tbl_{table_id}"));
        // Chunks are keyed by `(index, segment)`: the reference writes each
        // segment's index under `idx_{id}/seg_{s}/` with its own chunk ids
        // (`SegmentIndexMeta::next_chunk_id` is per segment), so a table with
        // two segments has two independent chunk families per index.
        let mut segments = self.segment_row_counts(table);
        if segments.is_empty() {
            segments.push((0, 0));
        }
        let mut published = Vec::new();
        for im in self.storage.catalog.list_indexes()? {
            if im.table != table && im.table != self.qualified_table(table) {
                continue;
            }
            let Ok(index_id) = self.cpp_index_position(table, &im.name) else {
                continue;
            };
            let private =
                crate::index_key(&self.qualified_table(table), &im.column).replace('/', "_");
            // Every chunk the index holds, not just the first: a chunk the
            // catalog does not list is invisible to the reference's readers.
            //
            // The chunk's granularity depends on the index kind. Full-text,
            // secondary and BMP chunks are **blocks** (100 + 100 rows inserted
            // into one block leave one chunk of 200 rows, because every dump
            // re-dumps the block it touches); HNSW/IVF/PLAID chunks are
            // **memory-index dumps** (the same insert leaves two chunks of 100
            // rows each). Measured, not assumed: a probe with two flushes over
            // one block gives `row_count` 200/200/200 for full-text, secondary
            // and BMP and 100+100 for HNSW.
            let declared = self
                .table_schema(table)
                .ok()
                .and_then(|schema| {
                    let ci = schema.iter().position(|c| c == &im.column)?;
                    self.column_types(table).ok()?.get(ci).cloned()
                })
                .unwrap_or_default();
            let sparse = if im.index_type.eq_ignore_ascii_case("BMP") {
                Some(self.sparse_rows_by_block(table, &im.column))
            } else {
                None
            };
            // The chunk record names the transaction whose dump sealed it
            // (`SegmentIndexMeta::AddChunkIndexID1`); the port's last `DumpIndex`
            // for this index is that transaction.
            let dump_ts = self
                .storage
                .store
                .get(&crate::dump_ts_key(&self.qualified_table(table), &im.name))
                .ok()
                .flatten()
                .filter(|v| v.len() >= 8)
                .map(|v| u64::from_le_bytes(v[..8].try_into().unwrap()))
                .unwrap_or(0);
            for (segment, seg_rows) in &segments {
                let seg_dir = root.join(format!("idx_{index_id}/seg_{segment}"));
                // A chunk's ts is the commit ts of the transaction whose dump
                // produced it. A segment filled by `COPY` was populated inside
                // the import transaction (`PopulateIndex`), so its chunks
                // carry the import's ts; a later explicit dump stamps its own.
                let chunk_ts = dump_ts.max(self.import_ts(table, *segment).unwrap_or(0));
                // A segment that still holds exactly what one `COPY` wrote gets
                // the import's chunk layout (one per capacity for full-text, one
                // for the whole segment for secondary and sparse).
                let imported = self.imported_extent(table, *segment, *seg_rows).is_some();
                // A plain `CREATE INDEX` leaves the type empty, which is the
                // secondary default everywhere else (`is_secondary_kind`).
                let chunks = if crate::is_secondary_kind(&im.index_type) {
                    let dir = self.dir.join("secondary").join(&private);
                    // The chunk's key width comes from the column's declared type,
                    // exactly as the reader derives it (`SecondarySegmentReader::
                    // open_with_kind`).
                    publish_secondary_chunks(
                        &seg_dir,
                        &dir,
                        crate::secondary_key_kind(&declared),
                        *segment,
                        imported,
                    )
                } else if im.index_type.eq_ignore_ascii_case("BMP") {
                    // Rebuilt from the rows: the reference's chunk is the block and
                    // every dump extends it, while the port's spill files are per
                    // dump, so merging has to happen here.
                    let empty = Default::default();
                    publish_bmp_chunks(
                        &seg_dir,
                        sparse
                            .as_ref()
                            .and_then(|m| m.get(segment))
                            .cloned()
                            .unwrap_or(empty),
                        crate::sparse_idx_width(&declared),
                        crate::sparse_declared_bound(&declared).unwrap_or(0),
                        crate::index_block_size(&im),
                        *segment,
                        imported,
                    )
                } else if im.index_type.eq_ignore_ascii_case("FULLTEXT") {
                    // Written in place by `write_cpp_fulltext_index`; its files are
                    // named after the chunk's base name, not `chunk_N.idx`.
                    publish_fulltext_chunks(&seg_dir)
                } else if im.index_type.eq_ignore_ascii_case("SMVE")
                    || im.index_type.eq_ignore_ascii_case("EMVB")
                {
                    // These two write their chunk through their own file
                    // worker in the reference (`smve_index_file_worker`,
                    // `emvb_index_file_worker`): one `chunk_0.idx` per
                    // segment naming the whole segment.
                    let Some(bytes) =
                        self.cpp_multivector_chunk(table, &im.column, &im.index_type)
                    else {
                        continue;
                    };
                    if std::fs::create_dir_all(&seg_dir).is_err()
                        || std::fs::write(seg_dir.join("chunk_0.idx"), &bytes).is_err()
                    {
                        continue;
                    }
                    vec![CppIndexChunk {
                        chunk_id: 0,
                        base_name: String::new(),
                        base_row_id: u64::from(*segment) << 32,
                        row_count: *seg_rows,
                        term_count: 0,
                        index_size: bytes.len() as u64,
                        commit_ts: 0,
                    }]
                } else if im.index_type.eq_ignore_ascii_case("SMVE")
                    || im.index_type.eq_ignore_ascii_case("EMVB")
                {
                    let Some(bytes) =
                        self.cpp_multivector_chunk(table, &im.column, &im.index_type)
                    else {
                        continue;
                    };
                    if std::fs::create_dir_all(&seg_dir).is_err()
                        || std::fs::write(seg_dir.join("chunk_0.idx"), &bytes).is_err()
                    {
                        continue;
                    }
                    vec![CppIndexChunk {
                        chunk_id: 0,
                        base_name: String::new(),
                        base_row_id: u64::from(*segment) << 32,
                        row_count: *seg_rows,
                        term_count: 0,
                        index_size: bytes.len() as u64,
                        commit_ts: 0,
                    }]
                } else {
                    let deprecated = self.deprecated_chunk_ids(table, &im.name, *segment);
                    publish_chunk_files(
                        &seg_dir,
                        *seg_rows,
                        im.index_type.eq_ignore_ascii_case("HNSW"),
                        *segment,
                        &deprecated,
                    )
                };
                if chunks.is_empty() {
                    continue;
                }
                let chunks: Vec<_> = chunks
                    .into_iter()
                    .map(|c| CppIndexChunk {
                        commit_ts: chunk_ts,
                        ..c
                    })
                    .collect();
                published.push(CppIndexSegment {
                    index_id: index_id as u64,
                    segment_id: *segment,
                    chunks,
                });
            }
        }
        if published.is_empty() {
            return Ok(());
        }
        self.storage.catalog.publish_cpp_index_segments(
            &CppSegmentSet {
                db_id: db_id.to_string(),
                table_id: table_id.to_string(),
                segments: Vec::new(),
            },
            &published,
        )?;
        Ok(())
    }

    /// A sparse column's documents, grouped by segment and then by physical
    /// block, in the BMP encoder's form: `(row id within the segment,
    /// [(term, value)])`.
    ///
    /// The segment is part of the key: every segment restarts its block ids at
    /// zero, and the reference keeps one BMP index per segment
    /// (`idx_{id}/seg_{s}/chunk_{block}.idx`).
    fn sparse_rows_by_block(
        &self,
        table: &str,
        column: &str,
    ) -> std::collections::BTreeMap<
        u32,
        std::collections::BTreeMap<u64, Vec<infinity_storage::bmp_cpp::SparseDoc>>,
    > {
        let mut by_block = std::collections::BTreeMap::new();
        let Ok(schema) = self.table_schema(table) else {
            return by_block;
        };
        let Some(ci) = schema.iter().position(|c| c == column) else {
            return by_block;
        };
        let Ok(rows) = self.fetch_rows_with_ids_projected_all(table, ci) else {
            return by_block;
        };
        for (seq, cells) in rows {
            let Some(Value::Str(text)) = cells.get(ci) else {
                continue;
            };
            let entries: Vec<(i32, f32)> = crate::parse_sparse_str(text)
                .into_iter()
                .map(|(term, value)| (term as i32, value as f32))
                .collect();
            let row = seq as u64 & 0xffff_ffff;
            by_block
                .entry((seq >> 32) as u32)
                .or_default()
                .entry(row / DEFAULT_BLOCK_CAPACITY as u64)
                .or_default()
                .push((row, entries));
        }
        by_block
    }

    /// Reads a table back from the C++ block layout.
    ///
    /// This is the interop direction §2.6.1 cares about: a `db_*/tbl_*/seg_*/`
    /// blk_*` directory written by the C++ engine (or by [`Self::write_cpp_blocks`])
    /// reconstructs the table. Values are inserted through the normal `INSERT`
    /// path, so declared types, defaults and indexes all behave as usual.
    pub fn load_cpp_blocks(&self, table: &str) -> Result<usize> {
        let qualified = self.qualified_table(table);
        let meta = self
            .storage
            .catalog
            .get_table_by_name(&qualified)?
            .ok_or_else(|| Error::NotFound(format!("table '{qualified}'")))?;
        let (columns, rows) = self.read_cpp_blocks(table)?;
        if rows.is_empty() {
            return Ok(0);
        }
        let mut values = Vec::with_capacity(rows.len());
        for row in &rows {
            let cells: Vec<String> = row
                .iter()
                .enumerate()
                .map(|(i, v)| sql_literal(meta.types.get(i).map(String::as_str).unwrap_or(""), v))
                .collect();
            values.push(format!("({})", cells.join(",")));
        }
        let sql = format!(
            "INSERT INTO {qualified} ({}) VALUES {};",
            columns.join(", "),
            values.join(",")
        );
        self.execute(&sql)?;
        Ok(rows.len())
    }

    /// Recovery on startup: restore every table's block mirror at the ids the
    /// mirror itself holds.
    ///
    /// Opening a directory is two sources of truth, exactly as in the
    /// reference: `RestoreTableCache` rebuilds the segments from the block
    /// metadata, then the WAL is replayed on top. The block metadata only
    /// records the rows a flush made durable — a crash right after an append
    /// leaves the block's `version` at the flushed count and the newer rows in
    /// the log — so the two have to be merged, not chosen between:
    ///
    /// * the replay already wrote the post-flush rows at their own ids, and
    /// * the mirror supplies the flushed ones, which are missing from the KV
    ///   store because their WAL records were truncated by the checkpoint.
    ///
    /// The restore therefore writes only the ids the store does not have, which
    /// also makes it idempotent: a cleanly closed directory restores nothing.
    /// A tombstoned slot is **not** written back - the block's `version` is what
    /// hides it, and a journal copy would make the two disagree on the next
    /// open (the ids still join the delete log, because a later rewrite re-marks
    /// the slot from there).
    ///
    /// A table whose columns the block format cannot express simply has no
    /// mirror; that (and only that) is skipped silently.
    /// Returns the newest timestamp the files carry: everything up to it is
    /// already in the blocks, so the WAL replay starts above it. The reference
    /// gets the same floor from its checkpoint record; with the row KV gone
    /// (plan C) the files are the only place it can come from.
    pub fn recover_cpp_blocks(&self) -> Result<u64> {
        let mut newest = 0u64;
        for meta in self.storage.catalog.list_tables()? {
            newest = newest.max(self.restore_cpp_blocks(&meta.name)?);
        }
        Ok(newest)
    }

    /// Rebuilds a table's rows **and the append/delete journals** from its block
    /// files - the read side of the mirror, and what a restart has instead of a
    /// second KV.
    ///
    /// The reference keeps that history in the files themselves: a block's
    /// `version` holds one `create` per append (the transaction's ts and the
    /// cumulative row count) and one ts per deleted slot, and a sealed segment
    /// is the one it wrote a `fast_rough_filter` for. Rebuilding from those is
    /// what makes the port's checkpoint rewrite reproduce the same `version`
    /// bytes, and it is why `appends:`/`append_ts:`/`deletes:` do not have to be
    /// persisted anywhere the reference would not have them.
    pub(crate) fn restore_cpp_blocks(&self, table: &str) -> Result<u64> {
        let table = self.qualified_table(table);
        let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&table) else {
            return Ok(0);
        };
        let db_id = self.database_id_of(&table);
        let root = self.blocks_root();
        let Some(types) = meta
            .types
            .iter()
            .map(|t| column_type_of(t))
            .collect::<Option<Vec<_>>>()
        else {
            return Ok(0); // a column the block format cannot express has no file
        };
        let blocks: Vec<BlockId> = self.visible_block_ids(&table);
        let mut writes: Vec<(Vec<u8>, Vec<u8>)> = Vec::new();
        let mut appends: Vec<u64> = Vec::new();
        let mut append_ts: Vec<u64> = Vec::new();
        let mut appends_copy: Vec<u64> = Vec::new();
        let mut deletes: Vec<u64> = Vec::new();
        let mut delete_ts: Vec<u64> = Vec::new();
        let mut segment_rows: std::collections::BTreeMap<u32, u64> = Default::default();
        let mut segment_first_ts: std::collections::BTreeMap<u32, u64> = Default::default();
        let mut total = 0u64;
        let mut newest = 0u64;
        for id in &blocks {
            let Ok((version, _)) = self.read_block_cached(&table, &root, *id) else {
                continue;
            };
            let Some(rows) = self.decode_block_cells(&table, &root, &meta, &types, *id, None) else {
                continue;
            };
            let mut seen = 0u64;
            for create in &version.created {
                let cumulative = create.cumulative_row_count.max(0) as u64;
                let added = cumulative.saturating_sub(seen);
                seen = cumulative;
                total += added;
                appends.push(total);
                append_ts.push(create.create_ts);
                appends_copy.push(0);
                newest = newest.max(create.create_ts);
                segment_first_ts
                    .entry(id.segment_id)
                    .or_insert(create.create_ts);
            }
            *segment_rows.entry(id.segment_id).or_default() += rows.len() as u64;
            for (slot, (seq, _cells)) in rows.iter().enumerate() {
                // `BlockVersion::Delete` keeps the cells and marks the slot, and
                // that mark is the whole tombstone - the block is the row's only
                // home, so nothing is written back to a row store.
                let tombstone = version.deleted.get(slot).copied().filter(|t| *t != 0);
                if let Some(ts) = tombstone {
                    deletes.push(*seq as u64);
                    delete_ts.push(ts);
                    newest = newest.max(ts);
                }
            }
        }
        // `import_rows`/`import_ts`: a `COPY` is what seals its segment, and the
        // reference records that by writing the segment-level prefilter and by
        // taking the segment's create ts from the append that opened it.
        for (segment, rows) in &segment_rows {
            let sealed = self
                .storage
                .catalog
                .store()
                .get(
                    format!("seg|{db_id}|{}|{segment}|fast_rough_filter", meta.id).as_bytes(),
                )
                .ok()
                .flatten()
                .is_some();
            if !sealed {
                continue;
            }
            writes.push((
                import_rows_key(&table, *segment),
                rows.to_le_bytes().to_vec(),
            ));
            if let Some(ts) = segment_first_ts.get(segment) {
                writes.push((import_ts_key(&table, *segment), ts.to_le_bytes().to_vec()));
            }
        }
        if !writes.is_empty() {
            self.storage.store.atomic_write(&writes)?;
        }
        for (key, values) in [
            (appends_key(&table), &appends),
            (append_ts_key(&table), &append_ts),
            (append_copy_key(&table), &appends_copy),
            (deletes_key(&table), &deletes),
            (delete_ts_key(&table), &delete_ts),
        ] {
            if !values.is_empty() {
                let _ = self.write_id_log(&key, values);
            }
        }
        Ok(newest)
    }

    /// Applies the row commands a WAL replay found above the blocks' own floor:
    /// an append becomes rows in its block (`AppendInBlock`, with the frame's
    /// commit ts as the block's `create`), a delete becomes a mark on the slot
    /// the row occupies (`DeleteInBlock`).
    /// The reference's replay ends in exactly this shape - its `WalCmdAppendV2`
    /// carries the `DataBlock` that `AppendInBlock` writes - and it is why a
    /// recovered directory has no second copy of the rows anywhere.
    pub(crate) fn apply_replayed_rows(
        &self,
        appends: Vec<infinity_txn::ReplayedAppend>,
        deletes: Vec<infinity_txn::ReplayedDelete>,
    ) -> Result<()> {
        if appends.is_empty() && deletes.is_empty() {
            return Ok(());
        }
        let mut order: Vec<String> = Vec::new();
        let mut rows: std::collections::HashMap<String, Vec<(usize, Vec<Value>)>> =
            Default::default();
        for append in &appends {
            let table = self.qualified_table(&append.table);
            // The frame names the table **id** it was written for. A snapshot
            // restore re-creates the table under a new id (and a dropped name
            // can come back the same way), so rows of a frame that names an
            // older id belong to a table that no longer exists - replaying them
            // appended the pre-restore rows on top of the restored ones.
            match self.storage.catalog.get_table_by_name(&table) {
                Ok(Some(meta)) if append.table_id == u64::MAX || append.table_id == meta.id => {}
                _ => continue,
            }
            let defaults = self.table_defaults(&table).unwrap_or_default();
            // A row the block files already hold is not re-appended: the frame
            // may describe a commit that reached the blocks before the crash
            // (a DDL rewrite logs its rows and then writes them), and buffering
            // it again would show the row twice.
            let on_disk: std::collections::HashSet<usize> = self
                .read_cpp_blocks_including_deleted(&table)
                .into_iter()
                .map(|(seq, _)| seq)
                .collect();
            let mut batch = Vec::with_capacity(append.rows.len());
            for (seq, text) in &append.rows {
                if on_disk.contains(&(*seq as usize)) {
                    continue;
                }
                match self.parse_stored_row(&table, &defaults, text.as_bytes()) {
                    Ok(row) => batch.push((*seq as usize, row)),
                    // A column whose text form the row parser cannot rebuild (a
                    // multi-vector) keeps the replayed text in the store: the
                    // block side of the read cannot decode that column either,
                    // so the row has nowhere else to come from.
                    Err(_) => {
                        self.storage
                            .store
                            .put(&row_key(&table, *seq as usize), text.as_bytes())?;
                    }
                }
            }
            // One `create` per append transaction, stamped with its commit ts.
            self.note_append(&table, batch.len(), false, None);
            self.settle_append_ts(&table, 1, append.commit_ts);
            if !rows.contains_key(&table) {
                order.push(table.clone());
            }
            rows.entry(table).or_default().extend(batch);
        }
        for delete in &deletes {
            let table = self.qualified_table(&delete.table);
            // A tombstone for an id the table no longer has (see the append
            // loop) cannot mark a row of the table that exists now.
            match self.storage.catalog.get_table_by_name(&table) {
                Ok(Some(meta)) if delete.table_id == u64::MAX || delete.table_id == meta.id => {}
                _ => continue,
            }
            let ids: Vec<usize> = delete.row_ids.iter().map(|id| *id as usize).collect();
            self.note_deleted_ids(&table, &ids);
            self.settle_delete_ts(&table, ids.len(), delete.commit_ts);
            if !rows.contains_key(&table) {
                order.push(table.clone());
            }
            rows.entry(table).or_default();
        }
        for table in order {
            let extra = rows.remove(&table).unwrap_or_default();
            self.refresh_cpp_block_mirror_extra(&table, &extra)?;
        }
        Ok(())
    }

    /// A whole-table rewrite whose rows are given: `ALTER TABLE … DROP COLUMN`
    /// keeps every row id and only removes a column, so the blocks have to be
    /// re-encoded from the new values rather than merged with what they hold.
    pub(crate) fn rewrite_cpp_blocks(
        &self,
        table: &str,
        rows: &[(usize, Vec<Value>)],
    ) -> Result<()> {
        let table = self.qualified_table(table);
        let Ok(Some(_meta)) = self.storage.catalog.get_table_by_name(&table) else {
            return Ok(());
        };
        let tkey = self.table_key(&table);
        {
            // The rows this rewrite carries are authoritative for the cells:
            // the files hold the schema before the change. Marking the entry is
            // what lets the writer skip them - deleting the directories instead
            // would open a window in which a concurrent write finds no file.
            let mut tables = self.tables.lock().unwrap();
            tables.entry(tkey.clone()).or_default().rewrite_rows = true;
        }
        for id in self.visible_block_ids(&table) {
            self.context.buffer.lock().unwrap().invalidate(&id);
        }
        self.invalidate_block_mirror(&table);
        // Drop the buffered copies of the slots this rewrite carries (it holds
        // the authoritative cells for them) and keep everything the buffer took
        // after the snapshot - those ids are not in `rows`, and dropping them
        // would lose a commit that landed while the rewrite prepared.
        {
            let ids: std::collections::HashSet<usize> = rows.iter().map(|(seq, _)| *seq).collect();
            let mut tables = self.tables.lock().unwrap();
            if let Some(entry) = tables.get_mut(&tkey) {
                entry.pending_rows.retain(|(seq, _)| !ids.contains(seq));
            }
        }
        let written = self.write_cpp_blocks_extra(&table, rows);
        let mut tables = self.tables.lock().unwrap();
        tables.entry(tkey).or_default().rewrite_rows = false;
        written?;
        Ok(())
    }

    /// Whether `seq` names a row a delete may hide: a live slot of a visible
    /// block, or (for a directory an older build wrote) a `row:` key.
    ///
    /// The reference decides this from the block's `BlockOffset` and `version`;
    /// with the rows in the blocks there is no key to look up.
    pub(crate) fn row_is_present(&self, table: &str, seq: usize) -> bool {
        let table = self.qualified_table(table);
        if self
            .storage
            .store
            .get(&del_key(&table, seq))
            .ok()
            .flatten()
            .is_some()
        {
            return false;
        }
        if self
            .storage
            .store
            .get(&row_key(&table, seq))
            .ok()
            .flatten()
            .is_some()
        {
            return true;
        }
        if self.deleted_ids(&table).contains(&(seq as u64)) {
            return false;
        }
        // A row a commit appended since the last checkpoint is in the table's
        // append buffer, not in a block file: it exists all the same
        // (`DeleteInBlock` addresses the same slot whether or not the block has
        // been written out yet).
        if self
            .pending_rows_of(&table)
            .iter()
            .any(|(pending, _)| *pending == seq)
        {
            return true;
        }
        let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&table) else {
            return false;
        };
        let id = BlockId {
            db_id: self.database_id_of(&table),
            table_id: meta.id,
            segment_id: (seq >> 32) as u32,
            block_id: ((seq & 0xffff_ffff) / DEFAULT_BLOCK_CAPACITY) as u16,
        };
        if !self.visible_block_ids(&table).contains(&id) {
            return false;
        }
        let Some(version) = self.read_block_version(id) else {
            return false;
        };
        let slot = (seq & 0xffff_ffff) % DEFAULT_BLOCK_CAPACITY;
        if slot >= Self::visible_rows_of(&version) {
            return false;
        }
        version.deleted.get(slot).copied().unwrap_or(0) == 0
    }

    /// The blocks a reader may see: the table's block index minus every segment
    /// a compaction deprecated, in `(segment, block)` order.
    ///
    /// This is the **only** place visibility is decided, so the scan, the
    /// per-block range and the row count cannot disagree. A compaction keeps
    /// the source segments' files and records and hides them with `drop|seg|…`
    /// alone, so a reader that misses the mark returns the merged rows *and*
    /// the source rows: 40,960 rows for a 32,768-row table.
    pub(crate) fn visible_block_ids(&self, table: &str) -> Vec<BlockId> {
        let table = self.qualified_table(table);
        let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&table) else {
            return Vec::new();
        };
        let db_id = self.database_id_of(&table);
        // Through the catalog, not the raw store: a compaction marks the
        // source segments inside its transaction, and a read afterwards must
        // see those marks even before the checkpoint writes them through.
        let dropped = self
            .storage
            .catalog
            .cpp_dropped_segments(&db_id.to_string(), &meta.id.to_string())
            .unwrap_or_default();
        let Some(index) = self.block_index_for(&table) else {
            return Vec::new();
        };
        let index = index.lock().unwrap();
        index
            .metas()
            .filter(|meta| !dropped.contains(&meta.id.segment_id))
            .map(|meta| meta.id)
            .collect()
    }

    /// A block's `version` file alone - the row count and the tombstones,
    /// without decoding a single column.
    fn read_block_version(&self, id: BlockId) -> Option<BlockVersionFile> {
        let dir = self.blocks_root().join(cpp_format::block_dir(
            id.db_id,
            id.table_id,
            id.segment_id,
            id.block_id,
        ));
        BlockVersionFile::decode(&std::fs::read(dir.join(cpp_format::VERSION_FILE)).ok()?).ok()
    }

    /// The rows a block's `version` makes visible: its created rows minus the
    /// slots marked deleted (`BlockVersion::GetRowCount`).
    /// How many rows of `version` exist at `read_ts` (`cumulative_row_count` of
    /// the newest `create` the reader can see).
    fn created_at(version: &BlockVersionFile, read_ts: u64) -> usize {
        version
            .created
            .iter()
            .filter(|c| c.create_ts <= read_ts)
            .last()
            .map(|c| c.cumulative_row_count.max(0) as usize)
            .unwrap_or(0)
    }

    /// Whether slot `slot` of the block is visible at `read_ts`.
    fn slot_visible(
        version: &BlockVersionFile,
        slot: usize,
        created_at: usize,
        read_ts: u64,
    ) -> bool {
        if slot >= created_at {
            return false;
        }
        match version.deleted.get(slot).copied().unwrap_or(0) {
            0 => true,
            deleted => deleted > read_ts,
        }
    }

    fn visible_rows_of(version: &BlockVersionFile) -> usize {
        let created = version
            .created
            .last()
            .map(|c| c.cumulative_row_count.max(0) as usize)
            .unwrap_or(0);
        let deleted = version
            .deleted
            .iter()
            .take(created)
            .filter(|ts| **ts != 0)
            .count();
        created - deleted
    }

    /// Rows reserved in each segment, read off the blocks: a segment's extent is
    /// its last block's window end (`block * capacity + created`), deleted rows
    /// included - the reference keeps a block append-only, so its `version` says
    /// how far the segment was written even when slots were later marked.
    ///
    /// This is the layout a directory whose rows live only in the blocks still
    /// has: the allocator's in-memory map is empty after a restart, and the row
    /// journal holds nothing to derive it from.
    pub(crate) fn block_segment_row_counts(&self, table: &str) -> Vec<(u32, u64)> {
        let mut out: std::collections::BTreeMap<u32, u64> = Default::default();
        for id in self.visible_block_ids(table) {
            let Some(version) = self.read_block_version(id) else {
                continue;
            };
            let created = version
                .created
                .last()
                .map(|c| c.cumulative_row_count.max(0) as u64)
                .unwrap_or(0);
            let end = u64::from(id.block_id) * DEFAULT_BLOCK_CAPACITY as u64 + created;
            out.entry(id.segment_id)
                .and_modify(|e| *e = (*e).max(end))
                .or_insert(end);
        }
        out.into_iter().collect()
    }

    /// The rows the block mirror holds, in the very blocks a scan reads.
    ///
    /// The count follows the same visible set and the same `version` the scan
    /// decodes, so `count(*)`-style counters and a full scan of the same table
    /// cannot disagree - which they did while one side walked every block
    /// directory (deprecated segments included) and the other skipped the
    /// segments `drop|seg|…` had deprecated.
    ///
    /// A stale mirror answers nothing at all: after a compaction the merged
    /// segment is still only in the journal, and a number derived from the old
    /// files would describe rows that have moved.
    pub fn cpp_visible_row_count(&self, table: &str) -> Result<usize> {
        Ok(self.block_visible_row_count(table))
    }

    /// The rows the block files make visible, whether or not they are current.
    ///
    /// A count never has to be refused: the files say how many rows they hold,
    /// and the journal says what changed above them (see [`Engine::row_count`]).
    pub(crate) fn block_visible_row_count(&self, table: &str) -> usize {
        // The count answers the same question the scan does, so it is taken at
        // the same timestamp - including which deletes have happened.
        let read_ts = crate::read_ts();
        let deleted = self.tombstone_set_at(table, read_ts).unwrap_or_default();
        let pending = self.pending_rows_of(table);
        let mut rows = 0usize;
        for id in self.visible_block_ids(table) {
            let Some(version) = self.read_block_version(id) else {
                continue;
            };
            let created = Self::created_at(&version, read_ts);
            let mut live = 0usize;
            for slot in 0..created {
                if !Self::slot_visible(&version, slot, created, read_ts) {
                    continue;
                }
                live += 1;
                // A delete the checkpoint has not written yet is in the table's
                // delete log and not in the block's `version`; a delete it has
                // written is in both, and counting it twice would under-report.
                let seq = ((id.segment_id as u64) << 32)
                    | (u64::from(id.block_id) * DEFAULT_BLOCK_CAPACITY as u64 + slot as u64);
                if deleted.contains(&seq) {
                    live = live.saturating_sub(1);
                }
            }
            rows += live;
        }
        // Rows a commit appended since the last checkpoint live in the append
        // buffer, not in a file yet.
        let view = self.append_view(table, read_ts);
        rows + pending
            .iter()
            .filter(|(seq, _)| {
                !deleted.contains(&(*seq as u64))
                    && view.as_ref().map_or(true, |view| view.covers(*seq, read_ts))
            })
            .count()
    }

    /// `NewCatalog::GetBlockVisibleRange` for one block of a visible segment:
    /// the rows its `version` makes visible. A deprecated segment has no
    /// visible block at all.
    pub fn cpp_block_visible_range(
        &self,
        table: &str,
        segment: u32,
        block: u16,
    ) -> Option<(usize, usize)> {
        let id = self
            .visible_block_ids(table)
            .into_iter()
            .find(|id| id.segment_id == segment && u32::from(id.block_id) == u32::from(block))?;
        let version = self.read_block_version(id)?;
        Some((0, Self::visible_rows_of(&version)))
    }

    /// Reads one row by its storage id, one block at a time.
    ///
    /// The id names the segment, the block and the slot
    /// (`segment << 32 | block * capacity + slot`), so a lookup touches one
    /// block instead of walking the table: a compaction moves every row of its
    /// sources, and materialising the table once per row would make it
    /// quadratic. The block is decoded once and kept while the caller walks the
    /// rows in order, so a segment costs one decode per block and O(1) memory -
    /// the reference's scan streams through the buffer manager the same way.
    ///
    /// Journal first: a row written after the last checkpoint (and every row
    /// while the mirror is dirty) is there, and so is its tombstone.
    pub(crate) fn block_cursor(&self, table: &str) -> Result<BlockCursor<'_>> {
        BlockCursor::new(self, table)
    }

    /// Reads a block through the bounded buffer manager, loading it from disk on
    /// a miss (the reference's `BufferManager::Get`).
    fn read_block_cached(
        &self,
        table: &str,
        root: &Path,
        id: BlockId,
    ) -> std::io::Result<(BlockVersionFile, Arc<Vec<RawBlockColumn>>)> {
        // C++ `BlockIndex` + `BlockMeta::GetBlockColumnInfo`: the metadata
        // lookup and the decoded payload both go through the table's block
        // index and the engine-wide buffer manager.
        if let Some(index) = self.block_index_for(table) {
            let index = index.lock().unwrap();
            let mut buffer = self.context.buffer.lock().unwrap();
            if let Ok((_, columns)) = index.get_block(&mut buffer, &id) {
                let dir = root.join(cpp_format::block_dir(
                    id.db_id,
                    id.table_id,
                    id.segment_id,
                    id.block_id,
                ));
                let version =
                    BlockVersionFile::decode(&std::fs::read(dir.join(cpp_format::VERSION_FILE))?)
                        .map_err(|e| std::io::Error::new(std::io::ErrorKind::InvalidData, e))?;
                return Ok((version, columns));
            }
        }
        // A block not registered in the index (e.g. written between the
        // index's open and this read) is still readable from disk.
        let (version, columns) = cpp_format::read_block(root, id)?;
        Ok((version, Arc::new(columns)))
    }

    /// Reads the block mirror back, returning `(columns, rows)`.
    ///
    /// This never consults the KV store, so a successful read proves the mirror
    /// carries the table on its own.
    pub fn read_cpp_blocks(&self, table: &str) -> Result<(Vec<String>, Vec<Vec<String>>)> {
        let (columns, rows) = self.read_cpp_blocks_wanted(table, None, &[])?;
        Ok((columns, rows.into_iter().map(|(_, cells)| cells).collect()))
    }

    /// Like [`Self::read_cpp_blocks`] but decodes only `wanted` columns; the
    /// others are returned as `"null"`. This is the column-projected read path
    /// for `SELECT id, name` (plan #87).
    pub fn read_cpp_blocks_selected(
        &self,
        table: &str,
        wanted: &[usize],
    ) -> Result<(Vec<String>, Vec<Vec<String>>)> {
        let (columns, rows) = self.read_cpp_blocks_wanted(table, Some(wanted), &[])?;
        Ok((columns, rows.into_iter().map(|(_, cells)| cells).collect()))
    }

    /// [`Self::read_cpp_blocks_selected`] plus block pruning: a block whose
    /// `blk|…|fast_rough_filter` record proves the predicate cannot match is
    /// not read at all (C++ `FastRoughFilter::MayContain`, applied per block by
    /// the scan).
    pub fn read_cpp_blocks_pruned(
        &self,
        table: &str,
        wanted: &[usize],
        prune: &[crate::fast_rough_filter::PruneCond],
    ) -> Result<(Vec<String>, Vec<(usize, Vec<String>)>)> {
        self.read_cpp_blocks_wanted(table, Some(wanted), prune)
    }

    /// [`Self::read_cpp_blocks_pruned`] with the cells already decoded to
    /// `Value`: what a scan reads, so it does not parse the text back.
    pub fn read_cpp_blocks_pruned_values(
        &self,
        table: &str,
        wanted: &[usize],
        prune: &[crate::fast_rough_filter::PruneCond],
    ) -> Result<(Vec<String>, Vec<(usize, Vec<Value>)>)> {
        self.read_cpp_blocks_values(table, Some(wanted), prune, None)
    }

    /// [`Self::read_cpp_blocks_pruned_values`] with a row predicate applied
    /// while the rows are built: a rejected row never becomes a `Vec`.
    pub fn read_cpp_blocks_filtered_values(
        &self,
        table: &str,
        wanted: &[usize],
        prune: &[crate::fast_rough_filter::PruneCond],
        keep: &dyn Fn(&[Value]) -> bool,
    ) -> Result<(Vec<String>, Vec<(usize, Vec<Value>)>)> {
        self.read_cpp_blocks_values(table, Some(wanted), prune, Some(keep))
    }

    /// Every row the blocks hold, tombstoned slots included.
    ///
    /// The index mirrors are built from a column and keep deleted rows (the
    /// reference's chunks count every document a segment ever held), so this is
    /// the block counterpart of [`Engine::fetch_rows_including_deleted`].
    pub(crate) fn read_cpp_blocks_including_deleted(
        &self,
        table: &str,
    ) -> Vec<(usize, Vec<String>)> {
        let table = self.qualified_table(table);
        let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&table) else {
            return Vec::new();
        };
        let Some(types) = meta
            .types
            .iter()
            .map(|t| column_type_of(t))
            .collect::<Option<Vec<_>>>()
        else {
            return Vec::new();
        };
        let root = self.blocks_root();
        let mut out = Vec::new();
        for id in self.visible_block_ids(&table) {
            if let Some(cells) =
                self.decode_block_cells(&table, &root, &meta, &types, id, None)
            {
                out.extend(cells);
            }
        }
        for (seq, values) in self.pending_rows_of(&table) {
            out.push((seq, values.iter().map(mirror_cell_text).collect()));
        }
        out.sort_by_key(|(seq, _)| *seq);
        out
    }

    /// Number of rows the block mirror currently holds, over every visible
    /// segment and with the deleted slots taken out.
    ///
    /// The old version of this walked `seg_0` only and counted a block's
    /// created rows, so after a compaction it reported the *deprecated* source
    /// segment (8,192) while a scan of the same table returned the merged
    /// segment's 32,768 - the "40,960 vs 32,768" divergence between the two
    /// row counts.
    pub fn cpp_block_row_count(&self, table: &str) -> Result<usize> {
        self.cpp_visible_row_count(table)
    }

    /// The cells of one mirror block, in row order, as the block reader renders
    /// them (`null` for a NULL slot).
    ///
    /// A block covers a fixed `DEFAULT_BLOCK_CAPACITY` window of its segment, so
    /// row `n` of block `b` is the id `segment << 32 | b * capacity + n` - the id
    /// the reference stored it under.
    fn decode_block_cells(
        &self,
        table: &str,
        root: &Path,
        meta: &infinity_catalog::TableMeta,
        types: &[i32],
        id: BlockId,
        wanted: Option<&[usize]>,
    ) -> Option<Vec<(usize, Vec<String>)>> {
        let (version, columns) = self.read_block_cached(table, root, id).ok()?;
        Self::decode_rows_of(&meta.ids, &meta.types, types, id, &version, &columns, wanted)
    }

    /// The cells of one block, given its files - the decoder both the engine's
    /// cached reader and the block-backed row source use, so a scan reads
    /// exactly what a rewrite writes.
    pub(crate) fn decode_rows_of(
        ids: &[u64],
        type_text: &[String],
        types: &[i32],
        id: BlockId,
        version: &cpp_format::BlockVersionFile,
        columns: &[cpp_format::RawBlockColumn],
        wanted: Option<&[usize]>,
    ) -> Option<Vec<(usize, Vec<String>)>> {
        let rows = Self::decode_rows_of_values(
            ids, type_text, types, id, version, columns, wanted, None, None,
        )?;
        Some(
            rows.into_iter()
                .map(|(seq, cells)| (seq, cells.iter().map(mirror_cell_text).collect()))
                .collect(),
        )
    }

    /// The rows of one block, as the `Value`s the engine works with.
    ///
    /// A scalar cell (`INT8..INT64`, `FLOAT`, `DOUBLE`, the 16-bit floats, a
    /// `BOOL`) is built from its slot directly. Rendering it to text and parsing
    /// it back - what the block mirror used to do for every cell - cost a
    /// `format!` and a re-parse per cell; the types that only exist as text
    /// (varchar, json, date, tensor, sparse, ...) keep the text form, and its
    /// parse, because that is also what a rewrite writes.
    pub(crate) fn decode_rows_of_values(
        ids: &[u64],
        type_text: &[String],
        types: &[i32],
        id: BlockId,
        version: &cpp_format::BlockVersionFile,
        columns: &[cpp_format::RawBlockColumn],
        wanted: Option<&[usize]>,
        keep: Option<&dyn Fn(&[Value]) -> bool>,
        mut observe: Option<&mut dyn FnMut(&[Value])>,
    ) -> Option<Vec<(usize, Vec<Value>)>> {
        let row_count: usize = version
            .created
            .last()
            .map(|c| c.cumulative_row_count as usize)
            .unwrap_or(0);
        // Everything a cell needs from its **column** - the body split, the
        // outline kind, the layout and the outline bytes - is the same for
        // every row. Splitting the body per cell copied the whole block's
        // `[data][bitmap]` once per cell, which made a block read
        // O(rows x block_bytes): 40,000 rows spent 68% of the time in memmove.
        struct Prepared<'a> {
            body: cpp_format::ColumnBody,
            kind: OutlineKind,
            outline: &'a [u8],
            shape: Option<(usize, i32)>,
            layout: ColumnLayout,
        }
        let mut prepared: Vec<Option<Prepared>> = Vec::with_capacity(types.len());
        for (col, column_type) in types.iter().enumerate() {
            if let Some(wanted) = wanted {
                if !wanted.contains(&col) {
                    prepared.push(None);
                    continue;
                }
            }
            let column_id = ids.get(col).copied().unwrap_or(col as u64);
            let raw = columns.iter().find(|c| c.column_id == column_id)?;
            let layout = layout_of(&type_text[col], *column_type)?;
            prepared.push(Some(Prepared {
                body: cpp_format::split_body_for_layout(
                    raw.body.as_slice(),
                    DEFAULT_BLOCK_CAPACITY,
                    layout,
                ),
                kind: outline_kind(&type_text[col]),
                outline: raw.outline.as_slice(),
                shape: embedding_shape(&type_text[col]),
                layout,
            }));
        }
        // One row buffer for the whole block: a row the predicate rejects never
        // becomes its own `Vec`, which is what a selective scan spent most of
        // its time allocating.
        let mut scratch: Vec<Value> = Vec::with_capacity(types.len());
        let mut out = Vec::with_capacity(row_count);
        for row in 0..row_count {
            scratch.clear();
            let cells = &mut scratch;
            for (col, column_type) in types.iter().enumerate() {
                let Some(column) = prepared[col].as_ref() else {
                    cells.push(Value::Null);
                    continue;
                };
                let body = &column.body;
                let outline = column.outline;
                let value = if body.null_bitmap.len() > row / 8
                    && body.null_bitmap[row / 8] & (1 << (row % 8)) == 0
                {
                    Value::Null
                } else if column.kind == OutlineKind::None {
                    match column.layout {
                        ColumnLayout::CompactBit => match body
                            .data
                            .get(row / 8)
                            .map(|b| b & (1 << (row % 8)) != 0)
                        {
                            Some(bit) => Value::Bool(bit),
                            None => Value::Null,
                        },
                        ColumnLayout::Fixed(width) => {
                            match decode_scalar_value(*column_type, width, &body.data, row) {
                                Some(value) => value,
                                // A fixed layout this decoder does not cover
                                // (`DATE`, `TIME`, `DATETIME`): keep the text.
                                None => decode_value_shaped(
                                    &type_text[col],
                                    *column_type,
                                    &body.data,
                                    row,
                                    outline,
                                    column.shape,
                                )
                                .map(parse_owned_value)
                                .unwrap_or(Value::Null),
                            }
                        }
                        ColumnLayout::VarLen(_) => decode_value_shaped(
                            &type_text[col],
                            *column_type,
                            &body.data,
                            row,
                            outline,
                            column.shape,
                        )
                        .map(parse_owned_value)
                        .unwrap_or(Value::Null),
                    }
                } else {
                    // Tensor / sparse / array slots point into the column's
                    // outline; their cells exist only as text.
                    let text = match column.kind {
                        OutlineKind::Tensor => {
                            let at = row * 8;
                            decode_tensor_value(&type_text[col], body.data.get(at..at + 8).unwrap_or(&[]), outline)
                        }
                        OutlineKind::TensorArray => {
                            let at = row * 8;
                            decode_tensorarray_value(&type_text[col], body.data.get(at..at + 8).unwrap_or(&[]), outline)
                        }
                        OutlineKind::Sparse => {
                            let at = row * 16;
                            decode_sparse_value(&type_text[col], body.data.get(at..at + 16).unwrap_or(&[]), outline)
                        }
                        OutlineKind::Array => {
                            let at = row * 8;
                            decode_array_value(&type_text[col], body.data.get(at..at + 8).unwrap_or(&[]), outline)
                        }
                        OutlineKind::None => None,
                    };
                    text.map(parse_owned_value).unwrap_or(Value::Null)
                };
                cells.push(value);
            }
            if let Some(observe) = observe.as_deref_mut() {
                observe(cells);
            }
            let seq = ((id.segment_id as usize) << 32)
                | (id.block_id as usize * DEFAULT_BLOCK_CAPACITY + row);
            match keep {
                Some(keep) => {
                    if keep(cells) {
                        out.push((seq, cells.clone()));
                    }
                }
                // No predicate: the row takes its buffer with it, which is the
                // one allocation a scanned row always paid.
                None => {
                    out.push((seq, std::mem::take(&mut scratch)));
                    scratch = Vec::with_capacity(types.len());
                }
            }
        }
        Some(out)
    }

    /// The append log as a reader needs it: one snapshot of the records, the
    /// stamps and the segment bases, taken **once** per scan (a per-row lookup
    /// would allocate three vectors per row).
    fn append_view(&self, table: &str, read_ts: u64) -> Option<AppendView> {
        if read_ts == u64::MAX {
            return None;
        }
        Some(AppendView {
            totals: self.append_totals(table),
            stamps: self.append_ts(table),
            bases: self.segment_row_counts(table),
        })
    }

    /// The rows the commits since the last checkpoint appended, by row id
    /// (the reference's `BufferObject` for the blocks they land in).
    pub(crate) fn pending_rows_of(&self, table: &str) -> Vec<(usize, Vec<Value>)> {
        let key = self.table_key(table);
        self.tables
            .lock()
            .unwrap()
            .get(&key)
            .map(|e| e.pending_rows.clone())
            .unwrap_or_default()
    }

    /// The append buffer as a scan reads it, **already in `Value`s**.
    ///
    /// This is the table whose rows have not reached a checkpoint yet - the
    /// shape a fresh import leaves behind, and what the SDK benchmark measures.
    /// The generic path renders every cell to text (`mirror_cell_text`) and the
    /// caller parses it back, so a row that is already a `Value` pays two
    /// allocations per cell; here it is cloned once and used as it is.
    ///
    /// `None` means the table has blocks, and the caller takes the block path
    /// (where the same rows may be shadowed by a block's cells).
    pub(crate) fn buffered_rows_values(
        &self,
        table: &str,
        wanted: Option<&[usize]>,
    ) -> Option<Vec<(usize, Vec<Value>)>> {
        let table = self.qualified_table(table);
        if !self.visible_block_ids(&table).is_empty() {
            return None;
        }
        let key = self.table_key(&table);
        if self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .map(|entry| entry.pending_rows.is_empty())
            .unwrap_or(true)
        {
            return None;
        }
        let read_ts = crate::read_ts();
        let dead = self.tombstone_set_at(&table, read_ts).unwrap_or_default();
        let view = self.append_view(&table, read_ts);
        // The buffer is cloned cell by cell under its own lock: cloning the
        // whole row first (`pending_rows_of`) copied the columns this read does
        // not want - a 20,000-row point filter copied 20,000 varchar strings to
        // throw them away.
        let tables = self.tables.lock().unwrap();
        let entry = tables.get(&key)?;
        let mut out: Vec<(usize, Vec<Value>)> = Vec::with_capacity(entry.pending_rows.len());
        for (seq, values) in &entry.pending_rows {
            if dead.contains(&(*seq as u64)) {
                continue;
            }
            if let Some(view) = &view {
                if !view.covers(*seq, read_ts) {
                    continue;
                }
            }
            let cells: Vec<Value> = values
                .iter()
                .enumerate()
                .map(|(i, v)| match wanted {
                    Some(w) if !w.contains(&i) => Value::Null,
                    _ => v.clone(),
                })
                .collect();
            out.push((*seq, cells));
        }
        drop(tables);
        out.sort_by_key(|(seq, _)| *seq);
        Some(out)
    }

    /// The identity of a block's `version` file: `(mtime, len)`.
    ///
    /// A checkpoint rewrites the file when it rewrites the block, so the pair
    /// changes with the bytes the stats were derived from. It is one `stat` per
    /// block per statement - far cheaper than decoding the block to find out.
    fn block_identity(
        root: &Path,
        db_id: u64,
        table_id: u64,
        segment: u32,
        block: u16,
    ) -> (u128, u64) {
        let version = root
            .join(cpp_format::block_dir(db_id, table_id, segment, block))
            .join("version");
        match std::fs::metadata(version) {
            Ok(md) => {
                let mtime = md
                    .modified()
                    .ok()
                    .and_then(|t| t.duration_since(std::time::UNIX_EPOCH).ok())
                    .map(|d| d.as_nanos())
                    .unwrap_or(0);
                (mtime, md.len())
            }
            Err(_) => (0, 0),
        }
    }

    /// Whether the cached stats of a block prove that none of `conds` can hold
    /// in it.
    ///
    /// A condition whose column has no stats (never decoded since the block was
    /// written, or a type the scan skips) simply cannot refute: the block is
    /// read, exactly as before.
    fn block_stats_refute(
        &self,
        db_id: u64,
        table_id: u64,
        segment: u32,
        block: u16,
        identity: (u128, u64),
        conds: &[(usize, crate::fast_rough_filter::PruneOp, Value)],
    ) -> bool {
        use crate::fast_rough_filter::PruneOp;
        let stats = self.block_stats.lock().unwrap();
        conds.iter().any(|(column, op, value)| {
            let Some((min, max)) = stats.get(&(
                db_id,
                table_id,
                segment,
                block,
                *column,
                identity.0,
                identity.1,
            )) else {
                return false;
            };
            match op {
                PruneOp::Eq => matches!(value.partial_cmp_value(min), Some(std::cmp::Ordering::Less))
                    || matches!(value.partial_cmp_value(max), Some(std::cmp::Ordering::Greater)),
                PruneOp::Lt => matches!(
                    min.partial_cmp_value(value),
                    Some(std::cmp::Ordering::Greater | std::cmp::Ordering::Equal)
                ),
                PruneOp::Le => matches!(
                    min.partial_cmp_value(value),
                    Some(std::cmp::Ordering::Greater)
                ),
                PruneOp::Gt => matches!(
                    max.partial_cmp_value(value),
                    Some(std::cmp::Ordering::Less | std::cmp::Ordering::Equal)
                ),
                PruneOp::Ge => matches!(max.partial_cmp_value(value), Some(std::cmp::Ordering::Less)),
            }
        })
    }

    /// Records the min/max of every column of a block that was just decoded.
    ///
    /// Only the columns the scan asked for have values; the others are NULL in
    /// every row and are skipped, so the cache never claims to know a column it
    /// has not seen.
    fn record_block_stats(
        &self,
        root: &Path,
        db_id: u64,
        table_id: u64,
        segment: u32,
        block: u16,
        observed: &[Option<(Value, Value)>],
    ) {
        let identity = Self::block_identity(root, db_id, table_id, segment, block);
        if identity == (0, 0) {
            return;
        }
        let mut stats = self.block_stats.lock().unwrap();
        for (column, bounds) in observed.iter().enumerate() {
            // A column with no non-null cell keeps no entry: nothing is known
            // about it, so a predicate on it must read the block.
            if let Some((min, max)) = bounds {
                stats.insert(
                    (db_id, table_id, segment, block, column, identity.0, identity.1),
                    (min.clone(), max.clone()),
                );
            }
        }
        // A long-lived engine must not accumulate one entry per block forever;
        // dropping the whole map only costs the next scan a decode it was doing
        // anyway.
        if stats.len() > 200_000 {
            stats.clear();
        }
    }

    /// Every row of the table as the engine works with it: the block files'
    /// cells (decoded by `decode_rows_of_values`) plus the append buffer's,
    /// which are `Value`s already.
    fn read_cpp_blocks_values(
        &self,
        table: &str,
        wanted: Option<&[usize]>,
        prune: &[crate::fast_rough_filter::PruneCond],
        keep: Option<&dyn Fn(&[Value]) -> bool>,
    ) -> Result<(Vec<String>, Vec<(usize, Vec<Value>)>)> {
        let table = self.qualified_table(table);
        let meta = self
            .storage
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let types: Vec<i32> = meta
            .types
            .iter()
            .map(|t| {
                column_type_of(t).ok_or_else(|| {
                    Error::Invalid(format!(
                        "column type '{t}' is not supported by the C++ block reader"
                    ))
                })
            })
            .collect::<Result<_>>()?;
        let db_id = self.database_id_of(&table);
        let root = self.blocks_root();
        let read_ts = crate::read_ts();

        let mut rows: Vec<(usize, Vec<Value>)> = Vec::new();
        // The mirror is segmented (`seg_{id}/blk_{n}`), and a row's id carries
        // its segment in the high 32 bits, so the block map - not a hardcoded
        // `seg_0` - decides what a row's id is. Reading only `seg_0` here made
        // a compacted table (whose rows live in the next segment id) come back
        // at the wrong ids, and the restore then wrote a second copy of every
        // row.
        // A compaction leaves the source segments' directories in place (the
        // reference keeps them until `Cleanup`), so the block index still lists
        // them. A reader must skip them, exactly as the reference skips a
        // segment whose `drop|seg|` ts is at or below its `begin_ts`.
        let blocks: Vec<BlockId> = self.visible_block_ids(&table);
        // A table whose first commit has not reached a checkpoint yet has no
        // block file at all: its rows are in the append buffer, and that is
        // still a readable table.
        let buffered = self.pending_rows_of(&table);
        if blocks.is_empty() && buffered.is_empty() {
            return Err(Error::NotFound(format!("no blocks for table '{table}'")));
        }
        // Resolve the conditions once against the schema; a column the record
        // does not cover (vector/sparse/tensor/bool) keeps its condition and
        // `may_contain` answers `true` for it.
        let resolved: Vec<(
            usize,
            crate::fast_rough_filter::PruneOp,
            infinity_expression::Value,
        )> = prune
            .iter()
            .filter_map(|cond| {
                meta.columns
                    .iter()
                    .position(|c| c.eq_ignore_ascii_case(&cond.column))
                    .map(|i| (i, cond.op, cond.value.clone()))
            })
            .collect();
        for (at, id) in blocks.into_iter().enumerate() {
            let BlockId {
                segment_id,
                block_id: block_index,
                ..
            } = id;
            if !resolved.is_empty() {
                // Skip a block the prefilter rules out without reading a single
                // column file.
                let key = format!(
                    "blk|{}|{}|{}|{}|fast_rough_filter",
                    db_id, meta.id, segment_id, block_index
                );
                if let Ok(Some(record)) = self.storage.catalog.store().get(key.as_bytes()) {
                    if let Some(filters) = crate::fast_rough_filter::Filters::decode(&record) {
                        if !filters.may_contain_all(&resolved) {
                            #[cfg(test)]
                            PRUNED_BLOCKS.with(|n| n.set(n.get() + 1));
                            continue;
                        }
                    }
                }
                // No catalog prefilter (the reference writes one only for a
                // sealed segment): the stats this engine collected when it last
                // decoded the block answer the same question.
                let identity = Self::block_identity(&root, db_id, meta.id, segment_id, block_index);
                if self.block_stats_refute(
                    db_id,
                    meta.id,
                    segment_id,
                    block_index,
                    identity,
                    &resolved,
                ) {
                    #[cfg(test)]
                    PRUNED_BLOCKS.with(|n| n.set(n.get() + 1));
                    continue;
                }
            }
            match self.read_block_cached(&table, &root, id) {
                Ok((version, columns)) => {
                    // The stats are taken from every decoded row, before `keep`
                    // drops the ones the predicate rejects - otherwise a
                    // filtered scan would record only the rows that matched and
                    // the next statement would believe the block holds nothing
                    // else.
                    let mut observed: Vec<Option<(Value, Value)>> =
                        vec![None; types.len()];
                    let mut observe = |cells: &[Value]| {
                        for (column, value) in cells.iter().enumerate() {
                            if matches!(value, Value::Null) {
                                continue;
                            }
                            match &observed[column] {
                                Some((min, max)) => {
                                    let mut next = (min.clone(), max.clone());
                                    if min.partial_cmp_value(value)
                                        == Some(std::cmp::Ordering::Greater)
                                    {
                                        next.0 = value.clone();
                                    }
                                    if max.partial_cmp_value(value)
                                        == Some(std::cmp::Ordering::Less)
                                    {
                                        next.1 = value.clone();
                                    }
                                    observed[column] = Some(next);
                                }
                                None => observed[column] = Some((value.clone(), value.clone())),
                            }
                        }
                    };
                    let Some(cells) = Self::decode_rows_of_values(
                        &meta.ids,
                        &meta.types,
                        &types,
                        id,
                        &version,
                        &columns,
                        wanted,
                        keep,
                        // Only a predicate can use the stats, and the bookkeeping
                        // is a comparison per cell: skip it otherwise.
                        if resolved.is_empty() {
                            None
                        } else {
                            Some(&mut observe)
                        },
                    ) else {
                        break;
                    };
                    // A deleted slot keeps its cells in the block file (the
                    // reference hides it with the version, it does not rewrite
                    // the block), so the scan drops it here - the same slots
                    // `cpp_visible_row_count` subtracts, which is what keeps
                    // the two numbers equal.
                    //
                    // The verdict is taken at the statement's `begin_ts`
                    // (`BlockVersion::GetRowCount(begin_ts)`): a slot created
                    // after it does not exist yet, and one deleted after it is
                    // still there.
                    if !resolved.is_empty() {
                        // The block is decoded anyway: keep what a later
                        // statement needs to skip it without decoding again.
                        self.record_block_stats(
                            &root,
                            db_id,
                            meta.id,
                            segment_id,
                            block_index,
                            &observed,
                        );
                    }
                    let created_at = Self::created_at(&version, read_ts);
                    rows.extend(
                        cells
                            .into_iter()
                            .enumerate()
                            .filter(|(slot, _)| Self::slot_visible(&version, *slot, created_at, read_ts))
                            .map(|(_, row)| row),
                    );
                }
                Err(_) if at == 0 => {
                    return Err(Error::NotFound(format!("no blocks for table '{table}'")))
                }
                Err(_) => break,
            }
        }
        // Rows a commit appended since the last checkpoint are in the table's
        // append buffer, not in a file yet: the scan reads them over the blocks
        // (the reference's `BufferObject` merge), tombstoned ones excluded.
        let pending = buffered;
        if !pending.is_empty() {
            let have: std::collections::HashSet<usize> =
                rows.iter().map(|(seq, _)| *seq).collect();
            // One snapshot of the append log and the deletes for the whole
            // buffer, not one per row.
            let dead = self.tombstone_set_at(&table, read_ts).unwrap_or_default();
            let view = self.append_view(&table, read_ts);
            for (seq, values) in pending {
                if have.contains(&seq) || dead.contains(&(seq as u64)) {
                    continue;
                }
                if let Some(view) = &view {
                    if !view.covers(seq, read_ts) {
                        continue;
                    }
                }
                let cells: Vec<Value> = values
                    .iter()
                    .enumerate()
                    .map(|(i, v)| match wanted {
                        Some(w) if !w.contains(&i) => Value::Null,
                        _ => v.clone(),
                    })
                    .collect();
                if keep.is_some_and(|keep| !keep(&cells)) {
                    continue;
                }
                rows.push((seq, cells));
            }
            rows.sort_by_key(|(seq, _)| *seq);
        }
        Ok((meta.columns.clone(), rows))
    }

    /// [`Self::read_cpp_blocks_values`] as the canonical cell text, which is what
    /// the index builders and the block-mirror dump compare against.
    fn read_cpp_blocks_wanted(
        &self,
        table: &str,
        wanted: Option<&[usize]>,
        prune: &[crate::fast_rough_filter::PruneCond],
    ) -> Result<(Vec<String>, Vec<(usize, Vec<String>)>)> {
        let (columns, rows) = self.read_cpp_blocks_values(table, wanted, prune, None)?;
        Ok((
            columns,
            rows.into_iter()
                .map(|(seq, cells)| (seq, cells.iter().map(mirror_cell_text).collect()))
                .collect(),
        ))
    }

    /// The C++ index id of `index_name` within `table`: the id the catalog
    /// handed out at `CREATE INDEX` (`IndexMeta::id`, a per-table counter
    /// starting at 1).
    ///
    /// This **must** be the catalog's id and not the index's position in some
    /// list: the catalog publishes the id in `catalog|idx|{db}|{tbl}|{name}|{ts}`
    /// and the reference reads the chunk of an index from `idx_{id}/seg_0/`, so
    /// a mirror that numbered the indexes differently wrote one index's chunk
    /// into another's directory (the HNSW chunk landed under the sparse
    /// index) and the reference then read the sparse chunk with the HNSW
    /// parser and aborted.
    pub(crate) fn cpp_index_position(&self, table: &str, index_name: &str) -> Result<usize> {
        let short = table.split('.').next_back().unwrap_or(table);
        let indexes = self.storage.catalog.list_indexes()?;
        let id = indexes
            .iter()
            .filter(|im| im.table == short || im.table == table)
            .find(|im| im.name == index_name)
            .map(|im| im.id)
            .filter(|id| *id > 0)
            .ok_or_else(|| Error::NotFound(format!("index '{index_name}' on '{table}'")))?;
        Ok(id as usize)
    }

    /// The vectors of `table`'s `column` as `(row id, vector)`, optionally
    /// restricted to one segment.
    ///
    /// The label is the **in-segment** offset (`seq as u32`): the reference's
    /// per-segment index is handed `SegmentOffset`s, and a chunk under
    /// `idx_{id}/seg_{s}/` is read back with the segment prepended.
    fn cpp_column_vectors(
        &self,
        table: &str,
        column: &str,
        segment: Option<u32>,
    ) -> Result<VectorRows> {
        let schema = self.table_schema(table)?;
        let col_idx = schema
            .iter()
            .position(|c| c == column)
            .ok_or_else(|| Error::NotFound(format!("column '{column}' on '{table}'")))?;
        let mut out = Vec::new();
        for (seq, row) in self.fetch_rows_with_ids(table)? {
            if let Some(only) = segment {
                if (seq >> 32) as u32 != only {
                    continue;
                }
            }
            if let Some(Value::Str(s)) = row.get(col_idx) {
                out.push((
                    seq as u32,
                    crate::parse_vec_str(s).iter().map(|x| *x as f32).collect(),
                ));
            }
        }
        Ok(out)
    }

    /// Builds a C++ HNSW `chunk_0.idx` body for `column`'s vectors (task #70).
    ///
    /// The graph is built by the engine's own `HnswIndex` and serialized with the
    /// C++ layout (§9.2): 56-byte header, plain vector region, graph region, then
    /// the `u32` label array. Byte-identity with a C++-built index is not
    /// required — the offsets are explicit, so the C++ loader reads it.
    fn cpp_hnsw_chunk(
        &self,
        table: &str,
        column: &str,
        options: &HnswOptions,
        segment: Option<u32>,
    ) -> Result<(usize, Vec<u8>)> {
        let vectors = self.cpp_column_vectors(table, column, segment)?;
        if vectors.is_empty() {
            return Err(Error::Invalid(format!(
                "no vectors to index in '{table}.{column}'"
            )));
        }
        let dim = vectors[0].1.len();
        let mut index = HnswIndex::with_encoding(
            dim,
            options.m,
            options.ef_construction,
            options.ef_construction.max(64),
            options.metric,
            options.encoding,
        );
        // Train LVQ/LS mean/RaBitQ centroid and the LSG averages on the whole
        // column, then build the graph with the trained store.
        let rows: Vec<Vec<f32>> = vectors.iter().map(|(_, v)| v.clone()).collect();
        index.train_store(&rows, options.lsg);
        let mut labels = Vec::with_capacity(vectors.len());
        for (label, v) in &vectors {
            index.add(v);
            labels.push(*label);
        }
        Ok(encode_hnsw_chunk_from(&index, &labels))
    }

    /// The HNSW options of `index_name` (`metric`, `encode`, `build_type`,
    /// `lsg_config`).
    fn hnsw_options(&self, table: &str, index_name: &str) -> HnswOptions {
        let short = table.rsplit('.').next().unwrap_or(table);
        let Some(im) = self
            .storage
            .catalog
            .list_indexes()
            .unwrap_or_default()
            .into_iter()
            .find(|im| im.name == index_name && (im.table == table || im.table == short))
        else {
            return HnswOptions::default();
        };
        let opt = |name: &str| {
            im.options
                .iter()
                .find(|(k, _)| k.eq_ignore_ascii_case(name))
                .map(|(_, v)| v.clone())
        };
        let metric =
            infinity_index::vector::Metric::parse(&opt("metric").unwrap_or_else(|| "l2".into()));
        let encoding =
            infinity_index::vector::HnswEncoding::parse(&opt("encode").unwrap_or_default());
        let lsg = if opt("build_type")
            .map(|v| v.eq_ignore_ascii_case("lsg"))
            .unwrap_or(false)
        {
            Some(
                opt("lsg_config")
                    .and_then(|v| infinity_index::lsg::LsgConfig::parse(&v))
                    .unwrap_or_default(),
            )
        } else {
            None
        };
        HnswOptions {
            m: opt("M").and_then(|v| v.parse().ok()).unwrap_or(16),
            ef_construction: opt("EF_CONSTRUCTION")
                .and_then(|v| v.parse().ok())
                .unwrap_or(200),
            metric,
            encoding,
            lsg,
        }
    }

    /// Writes `table`'s index as a C++ HNSW chunk file under
    /// `{data_dir}/data/db_{db}/tbl_{tbl}/idx_{id}/seg_0/chunk_{n}.idx`,
    /// returning how many vectors were written (task #70).
    ///
    /// A chunk is only written when the mirror has none yet: re-writing it on
    /// every checkpoint would not match the reference, which appends a chunk per
    /// memory-index dump.
    pub fn write_cpp_hnsw_index(&self, table: &str, index_name: &str) -> Result<usize> {
        self.write_cpp_hnsw_chunk(table, index_name, false, false)
            .map(|written| written.0)
    }

    /// Appends a fresh chunk, as the reference's memory-index dump does. Rows
    /// added since the previous dump end up in the new chunk.
    pub fn dump_cpp_hnsw_index(&self, table: &str, index_name: &str) -> Result<usize> {
        self.write_cpp_hnsw_chunk(table, index_name, true, false)
            .map(|written| written.0)
    }

    /// The optimize merge: rebuilds the graph over every live row and appends
    /// it as the newest chunk, which then supersedes the incremental ones.
    ///
    /// Returns the segments it wrote a chunk for - the entries an optimize
    /// transaction records (`OptimizeIndexTxnStore::entries_`).
    pub(crate) fn merge_cpp_hnsw_index(&self, table: &str, index_name: &str) -> Result<Vec<ChunkRewrite>> {
        self.write_cpp_hnsw_chunk(table, index_name, false, true)
            .map(|written| written.1)
    }

    fn write_cpp_hnsw_chunk(
        &self,
        table: &str,
        index_name: &str,
        new_chunk: bool,
        force_append: bool,
    ) -> Result<(usize, Vec<ChunkRewrite>)> {
        let table = self.qualified_table(table);
        let meta = self
            .storage
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let index_id = self.cpp_index_position(&table, index_name)?;
        let column = self
            .storage
            .catalog
            .list_indexes()?
            .into_iter()
            .find(|im| {
                im.name == index_name
                    && (im.table == table || table.rsplit('.').next() == Some(im.table.as_str()))
            })
            .map(|im| im.column)
            .ok_or_else(|| Error::NotFound(format!("index '{index_name}'")))?;
        // A MULTIVECTOR holds several sub-vectors per row; the C++ mirror is a
        // single-vector graph, so there is nothing to write for it.
        if let Some(i) = meta.columns.iter().position(|c| c == &column) {
            if meta
                .types
                .get(i)
                .map(|t| t.trim().to_ascii_uppercase().starts_with("MULTIVECTOR"))
                .unwrap_or(false)
            {
                return Err(Error::Invalid(format!(
                    "no single-vector HNSW mirror for multivector column '{column}'"
                )));
            }
        }
        // A checkpoint dumps the **memory segment** (rows since the previous
        // dump), not a graph rebuilt from every row: rebuilding cost O(table)
        // memory and time on every checkpoint and was the Gate C OOM driver.
        // The full rebuild remains the fallback when materialising the mirror
        // explicitly (`new_chunk == false`), e.g. after a restart.
        //
        // With more than one segment the buffered graph cannot be split - it
        // was built over every row - so a dump falls back to a per-segment
        // rebuild, which the reference's query side treats the same way (a
        // segment's chunks are merged at search time).
        let tkey = self.table_key(&table);
        let segments = self.segment_row_counts(&table);
        let segments = if segments.is_empty() {
            vec![(0, 0)]
        } else {
            segments
        };
        let from_memory = if new_chunk && segments.len() == 1 {
            let Some(mem) = self
                .tables
                .lock()
                .unwrap()
                .get(&tkey)
                .and_then(|e| e.hnsw_mem.get(&column).cloned())
            else {
                // Nothing buffered since the last dump: a checkpoint has
                // nothing to write and must not rebuild the whole graph.
                return Ok((0, Vec::new()));
            };
            let Ok(mem) = mem.lock() else {
                return Ok((0, Vec::new()));
            };
            if mem.is_empty() {
                return Ok((0, Vec::new()));
            }
            let labels: Vec<u32> = mem.rows().iter().map(|s| *s as u32).collect();
            Some(encode_hnsw_chunk_from(mem.index(), &labels))
        } else {
            None
        };
        let db_id = self.database_id_of(&table);
        let options = self.hnsw_options(&table, index_name);
        let mut written = 0usize;
        let mut rewrites: Vec<ChunkRewrite> = Vec::new();
        for (segment, rows) in &segments {
            let dir = self.blocks_root().join(format!(
                "db_{db_id}/tbl_{}/idx_{index_id}/seg_{segment}",
                meta.id
            ));
            std::fs::create_dir_all(&dir).map_err(|e| Error::Io {
                path: dir.clone(),
                source: e,
            })?;
            let chunks = hnsw_chunks(&dir);
            // A checkpoint only mirrors an index that has no chunk yet. Check
            // that before building: the build decodes every row and graph,
            // which on a long import is the dominant transient (Gate C RSS)
            // and is discarded here anyway.
            if !new_chunk && !force_append && !chunks.is_empty() {
                continue;
            }
            let built = match &from_memory {
                Some((count, chunk)) => Some((*count, chunk.clone())),
                None => {
                    if *rows == 0 {
                        None
                    } else {
                        self.cpp_hnsw_chunk(&table, &column, &options, Some(*segment))
                            .ok()
                    }
                }
            };
            let Some((count, chunk)) = built else {
                continue;
            };
            let name = format!(
                "chunk_{}.idx",
                chunks.last().map(|(id, _)| id + 1).unwrap_or(0)
            );
            let chunk_id: u32 = chunks
                .last()
                .map(|(id, _)| id + 1)
                .unwrap_or(0)
                .try_into()
                .unwrap_or(u32::MAX);
            std::fs::write(dir.join(name), &chunk).map_err(|e| Error::Io {
                path: dir.clone(),
                source: e,
            })?;
            written += count;
            // The chunks this one supersedes: the reference's
            // `deprecate_chunks_`, which its optimize transaction removes from
            // the segment's chunk list.
            rewrites.push(ChunkRewrite {
                segment: *segment,
                chunk_id,
                row_count: count as u32,
                deprecate_ids: chunks.iter().map(|(id, _)| *id as u32).collect(),
            });
        }
        Ok((written, rewrites))
    }

    /// Whether `column` is a `MULTIVECTOR`, whose mirror has no single-vector
    /// graph in this build.
    fn multivector_column(&self, table: &str, column: &str) -> bool {
        let Ok(meta) = self
            .storage
            .catalog
            .get_table_by_name(&self.qualified_table(table))
        else {
            return false;
        };
        let Some(meta) = meta else {
            return false;
        };
        meta.columns
            .iter()
            .position(|c| c == column)
            .and_then(|i| meta.types.get(i))
            .is_some_and(|t| t.trim().to_ascii_uppercase().starts_with("MULTIVECTOR"))
    }

    /// Optimizes every vector mirror: HNSW chunks are merged when the newest
    /// one does not cover all live rows and the merge is affordable, PLAID
    /// chunks are folded into one by [`Self::merge_cpp_plaid_index`] (the
    /// reference's `OptimizeIndexInner` `kPLAID` case) as soon as a second
    /// chunk exists. A mirror that is already merged is left untouched, so
    /// repeated optimize ticks are no-ops.
    ///
    /// `OptimizationProcessor::NewNotifyOptimize`: **one** transaction for the
    /// whole pass, opened before the pass looks at any index and committed even
    /// when it merged nothing (`OptimizeAllIndexes` -> `CommitTxn`; an empty
    /// `OptimizeIndexTxnStore` still spends its commit ts and writes an entry
    /// without commands). The store records one entry per index segment the
    /// pass rewrote, so two merges never run together and a merge meets the
    /// appends and updates on its table. A failed pass is rolled back; a lost
    /// conflict is logged and left alone, which is what the processor does.
    pub fn optimize_cpp_index_chunks(&self) -> Result<()> {
        let turn = self.txn.reserve_commit_ts();
        let ts = turn.ts();
        let mut txn = self.txn.begin();
        let mut entries: Vec<OptimizeEntry> = Vec::new();
        let outcome = self.merge_optimizable_indexes(&mut entries);
        // `OptimizeIndexTxnStore::ToWalEntry`: one `WalCmdDumpIndexV2` per
        // entry, carrying the merged chunk and the chunks it supersedes. The
        // reference's `dump_cause_` is not part of the frame. A failed pass never
        // gets here with its entries: it is rolled back.
        let commands = match &outcome {
            Ok(()) => self.optimize_index_commands(&entries)?,
            Err(_) => Vec::new(),
        };
        // The deprecations the commit has to publish: `RemoveChunkIDs` takes the
        // superseded chunks out of the segment's chunk list and one
        // `drop|idx_chunk|…` marker per id stays behind for `Cleanup`. The
        // entries are moved into the transaction, so keep the list here.
        let deprecations: Vec<(String, String, u32, Vec<u32>)> = entries
            .iter()
            .map(|entry| {
                (
                    entry.table.clone(),
                    entry.index.clone(),
                    entry.segment,
                    entry.deprecate_ids.clone(),
                )
            })
            .collect();
        txn.set_op(TxnOp::OptimizeIndex { entries });
        if !commands.is_empty() {
            txn.set_typed_commands(commands);
        }
        match outcome {
            Ok(()) => match self.txn.commit_at(txn, ts) {
                Ok(_) => {
                    // `RemoveChunkIDs(deprecate_ids)` + `PrepareCommitDumpIndex`
                    // of the reference: the merged chunk is what the segment holds
                    // from now on, so the chunks it replaced leave the chunk list,
                    // but their bytes stay on disk until `Cleanup` deletes them.
                    for (table, index, segment, ids) in &deprecations {
                        // The merge is already committed, so a failed
                        // deprecation must not turn the pass into an error:
                        // the superseded chunks stay readable (a retry would
                        // merge them again), which is noisier than losing
                        // them but never loses a row.
                        if let Err(error) =
                            self.deprecate_cpp_chunks(table, index, *segment, ids, ts)
                        {
                            eprintln!(
                                "optimize: deprecating the chunks {index} superseded failed: {error}"
                            );
                        }
                    }
                    // The decoded graphs of the superseded chunks are no longer
                    // part of the index; reconcile the in-memory set now instead
                    // of at the next checkpoint.
                    self.reload_vector_indexes();
                    Ok(())
                }
                // A concurrent merge (or a drop that took the index away) is a
                // conflict; `NewNotifyOptimize` logs it and leaves the chunks as
                // they are.
                Err(Error::TxnConflict(_)) | Err(Error::TxnConflictNoRetry(_)) => Ok(()),
                Err(error) => Err(error),
            },
            Err(error) => {
                self.txn.rollback(txn)?;
                Err(error)
            }
        }
    }

    /// `OptimizeIndexTxnStore::ToWalEntry`: the command per entry.
    fn optimize_index_commands(&self, entries: &[OptimizeEntry]) -> Result<Vec<WalCmd>> {
        let mut out = Vec::with_capacity(entries.len());
        for entry in entries {
            let table = entry.table.as_str();
            let Some(meta) = self.storage.catalog.get_table_by_name(table)? else {
                continue;
            };
            let (db_name, table_name) = crate::split_table_name(table);
            out.push(WalCmd::DumpIndex {
                db_name,
                db_id: self.database_id_of(table).to_string(),
                table_name,
                table_id: meta.id.to_string(),
                index_name: entry.index.clone(),
                index_id: self.cpp_index_position(table, &entry.index)?.to_string(),
                segment_id: entry.segment,
                chunk_infos: entry.chunk_infos.clone(),
                deprecate_ids: entry.deprecate_ids.clone(),
                table_key: self.table_key(table),
            });
        }
        Ok(out)
    }

    /// The merge half of an optimize pass, recording what it rewrote.
    fn merge_optimizable_indexes(&self, entries: &mut Vec<OptimizeEntry>) -> Result<()> {
        for im in self.storage.catalog.list_indexes()? {
            if im.index_type.eq_ignore_ascii_case("PLAID") {
                if let Some(rewrite) = self.merge_cpp_plaid_index(&im.table, &im.name)? {
                    entries.push(self.optimize_entry(&im.table, &im.name, &rewrite));
                }
                continue;
            }
            if !im.index_type.eq_ignore_ascii_case("HNSW") {
                continue;
            }
            // A MULTIVECTOR column has no single-vector graph in this build, so
            // its mirror has nothing to merge (`write_cpp_hnsw_chunk` refuses
            // it); skipping it keeps one such index from failing the pass.
            if self.multivector_column(&im.table, &im.column) {
                continue;
            }
            let Ok(rows) = self.row_count(&im.table) else {
                continue;
            };
            let Some(dir) = self.cpp_hnsw_dir(&im.table, &im.name) else {
                continue;
            };
            let chunks = self.live_chunk_files(&im.table, &im.name, 0, &dir);
            if chunks.len() < 2 {
                continue;
            }
            let covered = self
                .cpp_hnsw_chunk_path(&im.table, &im.name)
                .ok()
                .and_then(|p| std::fs::read(p).ok())
                .and_then(|b| {
                    knn_format::decode_hnsw_chunk(
                        &b,
                        4,
                        knn_format::VERTEX_L0_SIZE,
                        knn_format::VERTEX_LX_SIZE,
                    )
                    .ok()
                })
                .map(|c| c.header.cur_vec_num as usize)
                .unwrap_or(0);
            if covered >= rows {
                continue;
            }
            // A merge holds the vectors, the rebuilt graph and the encoded
            // chunk at the same time. On a multi-gigabyte table that transient
            // is larger than the whole memory budget (C++ streams it through
            // the buffer manager), so leave the incremental chunks alone
            // rather than rebuilding the table every tick.
            let dim = self
                .table_schema(&im.table)
                .ok()
                .and_then(|s| s.iter().position(|c| c == &im.column))
                .and_then(|ci| {
                    self.column_types(&im.table)
                        .ok()
                        .and_then(|t| t.get(ci).cloned())
                })
                .and_then(|t| crate::type_dim(&t))
                .unwrap_or(0);
            let rebuild_bytes = rows.saturating_mul(dim).saturating_mul(4 * 3 + 64);
            if rebuild_bytes > 1024 * 1024 * 1024 {
                continue;
            }
            for rewrite in self.merge_cpp_hnsw_index(&im.table, &im.name)? {
                entries.push(self.optimize_entry(&im.table, &im.name, &rewrite));
            }
        }
        Ok(())
    }

    /// The `OptimizeIndexStoreEntry` for one rewritten chunk: the reference's
    /// `new_chunk_infos_` (one entry, from `ChunkIndexMetaInfo`) and
    /// `deprecate_chunks_`.
    fn optimize_entry(
        &self,
        table: &str,
        index_name: &str,
        rewrite: &ChunkRewrite,
    ) -> OptimizeEntry {
        OptimizeEntry {
            table: self.qualified_table(table),
            index: index_name.to_string(),
            segment: rewrite.segment,
            chunk_infos: vec![infinity_storage::wal_entry::WalChunkIndexInfo {
                chunk_id: rewrite.chunk_id,
                base_name: String::new(),
                base_row_id: infinity_storage::wal_entry::WalRowId::new(rewrite.segment, 0),
                row_count: rewrite.row_count,
                term_count: 0,
                index_size: 0,
                deprecate_ts: u64::MAX,
            }],
            deprecate_ids: rewrite.deprecate_ids.clone(),
        }
    }

    /// Drops `idx_{id}/seg_{s}` directories whose segment no longer exists.
    ///
    /// Compaction merges the source segments into a new segment id and the
    /// reference deletes their index chunks with them; a `seg_{s}` directory
    /// left behind still holds a full copy of the index and would be read back
    /// as a second one.
    fn prune_dead_segment_index_dirs(&self) -> Result<()> {
        for meta in self.storage.catalog.list_tables()? {
            let db_id = self.database_id_of(&meta.name);
            let tbl_dir = self
                .blocks_root()
                .join(format!("db_{db_id}/tbl_{}", meta.id));
            // The block index is what a reader enumerates, and it survives a
            // restart; the allocator's map does not (`segment_ids` is empty for
            // a directory this process merely opened), and an empty set here
            // deletes every live chunk directory.
            let mut live_segments: Vec<u32> = self
                .visible_block_ids(&meta.name)
                .into_iter()
                .map(|id| id.segment_id)
                .collect();
            if live_segments.is_empty() {
                live_segments = self.segment_ids(&meta.name);
            }
            live_segments.sort_unstable();
            live_segments.dedup();
            let live: Vec<String> = live_segments
                .into_iter()
                .map(|segment| format!("seg_{segment}"))
                .collect();
            let Ok(entries) = std::fs::read_dir(&tbl_dir) else {
                continue;
            };
            for entry in entries.flatten() {
                let name = entry.file_name().to_string_lossy().to_string();
                if !name.starts_with("idx_") {
                    continue;
                }
                let Ok(inner) = std::fs::read_dir(entry.path()) else {
                    continue;
                };
                for sub in inner.flatten() {
                    let seg = sub.file_name().to_string_lossy().to_string();
                    if seg.starts_with("seg_") && !live.contains(&seg) {
                        let _ = std::fs::remove_dir_all(sub.path());
                    }
                }
            }
        }
        Ok(())
    }

    /// Reclaims the chunk bytes an optimize deprecated, and - as a safety net
    /// for a directory whose deprecation marker is missing - the chunks a
    /// merged chunk plainly supersedes.
    ///
    /// The reference splits the two halves: the optimize commit takes the
    /// superseded chunks out of the segment's chunk list and leaves a
    /// `drop|idx_chunk|…` marker, and `Cleanup` deletes the files those markers
    /// name (`CleanChunkIndex` -> `ChunkIndexMeta::UninitSet`). This is that
    /// second half.
    pub fn cleanup_cpp_index_chunks(&self) -> Result<()> {
        self.prune_dead_segment_index_dirs()?;
        for im in self.storage.catalog.list_indexes()? {
            if im.index_type.eq_ignore_ascii_case("PLAID") {
                let Ok(mirror) = self.cpp_plaid_mirror(&im.table, &im.name) else {
                    continue;
                };
                self.reclaim_deprecated_cpp_chunks(&im.table, &im.name, 0, &mirror.dir)?;
                // The safety net: a merge that ran before the markers existed
                // leaves files whose ids no marker names. PLAID chunks carry
                // their own document count in the header, so "the newest chunk
                // holds every document of the older ones" is readable without
                // decoding.
                let chunks = hnsw_chunks(&mirror.dir);
                let Some(((_, newest), older)) = chunks.split_last() else {
                    continue;
                };
                let Some(newest_docs) = plaid_chunk_doc_num(newest) else {
                    continue;
                };
                let superseded: u32 = older
                    .iter()
                    .filter_map(|(_, path)| plaid_chunk_doc_num(path))
                    .sum();
                if newest_docs < superseded {
                    continue;
                }
                for (_, path) in older {
                    let _ = std::fs::remove_file(path);
                }
                continue;
            }
            if !im.index_type.eq_ignore_ascii_case("HNSW") {
                continue;
            }
            let Ok(rows) = self.row_count(&im.table) else {
                continue;
            };
            let Some(dir) = self.cpp_hnsw_dir(&im.table, &im.name) else {
                continue;
            };
            self.reclaim_deprecated_cpp_chunks(&im.table, &im.name, 0, &dir)?;
            let chunks = hnsw_chunks(&dir);
            let covered = self
                .cpp_hnsw_chunk_path(&im.table, &im.name)
                .ok()
                .and_then(|p| std::fs::read(p).ok())
                .and_then(|b| {
                    knn_format::decode_hnsw_chunk(
                        &b,
                        4,
                        knn_format::VERTEX_L0_SIZE,
                        knn_format::VERTEX_LX_SIZE,
                    )
                    .ok()
                })
                .map(|c| c.header.cur_vec_num as usize)
                .unwrap_or(0);
            // The newest chunk covers the table only when it is a merged
            // chunk; incremental dumps cover just their own rows. A committed
            // optimize already dropped the chunks it superseded (the entry's
            // `deprecate_ids`), so this is the fallback for a crash between that
            // commit and the deletion.
            if covered < rows {
                continue;
            }
            if let Some((keep, _)) = chunks.last() {
                for (id, path) in &chunks {
                    if id != keep {
                        let _ = std::fs::remove_file(path);
                    }
                }
            }
        }
        Ok(())
    }

    /// The directory holding `table`'s HNSW mirror, if the table exists.
    pub(crate) fn cpp_hnsw_dir(&self, table: &str, index_name: &str) -> Option<PathBuf> {
        let table = self.qualified_table(table);
        let meta = self.storage.catalog.get_table_by_name(&table).ok()??;
        let index_id = self.cpp_index_position(&table, index_name).ok()?;
        let db_id = self.database_id_of(&table);
        Some(
            self.blocks_root()
                .join(format!("db_{db_id}/tbl_{}/idx_{index_id}/seg_0", meta.id)),
        )
    }

    /// The chunk ids of one segment index that an optimize deprecated and
    /// `cleanup_cpp_index_chunks` has not reclaimed yet: the reference's
    /// `drop|idx_chunk|…` markers (`PrepareCommitDumpIndex`,
    /// `new_txn_index_impl.cpp:3043`).
    fn deprecated_chunk_ids(
        &self,
        table: &str,
        index_name: &str,
        segment: u32,
    ) -> std::collections::HashSet<u64> {
        let table = self.qualified_table(table);
        let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(&table) else {
            return Default::default();
        };
        let Ok(index_id) = self.cpp_index_position(&table, index_name) else {
            return Default::default();
        };
        let db_id = self.database_id_of(&table);
        infinity_catalog::cpp_layout::deprecated_cpp_index_chunks(
            self.storage.catalog.store().as_ref(),
            &db_id.to_string(),
            &meta.id.to_string(),
            &index_id.to_string(),
            segment,
        )
        .map(|ids| ids.into_iter().collect())
        .unwrap_or_default()
    }

    /// The chunks of one segment that are still part of the index: the files
    /// minus the deprecated ids. The reference only ever enumerates the chunk
    /// list, and a deprecated chunk has left it even though its bytes are still
    /// on disk until `Cleanup`.
    fn live_chunk_files(
        &self,
        table: &str,
        index_name: &str,
        segment: u32,
        dir: &Path,
    ) -> Vec<(u64, PathBuf)> {
        let deprecated = self.deprecated_chunk_ids(table, index_name, segment);
        hnsw_chunks(dir)
            .into_iter()
            .filter(|(id, _)| !deprecated.contains(id))
            .collect()
    }

    /// `SegmentIndexMeta::RemoveChunkIDs` + `PrepareCommitDumpIndex`: the
    /// chunks leave the segment's chunk list and one `drop|idx_chunk|…` marker
    /// is written per id, so `Cleanup` can delete the bytes later. Nothing on
    /// disk is touched here - that is the reference's split, and it is what
    /// keeps the chunk files readable until `cleanup_cpp_index_chunks` runs.
    fn deprecate_cpp_chunks(
        &self,
        table: &str,
        index_name: &str,
        segment: u32,
        chunk_ids: &[u32],
        ts: u64,
    ) -> Result<()> {
        if chunk_ids.is_empty() {
            return Ok(());
        }
        let table = self.qualified_table(table);
        let Some(meta) = self.storage.catalog.get_table_by_name(&table)? else {
            // A `DROP` took the table while the optimize transaction was
            // running: its chunk records went with it.
            return Ok(());
        };
        let index_id = self.cpp_index_position(&table, index_name)?;
        let db_id = self.database_id_of(&table);
        infinity_catalog::cpp_layout::deprecate_cpp_index_chunks(
            self.storage.catalog.store().as_ref(),
            &db_id.to_string(),
            &meta.id.to_string(),
            &index_id.to_string(),
            segment,
            chunk_ids,
            ts,
        )?;
        Ok(())
    }


    /// `Cleanup`'s half of a deprecation: delete the bytes of every chunk a
    /// `drop|idx_chunk|…` marker names, then drop the marker together with the
    /// chunk's record. A crash in between leaves the marker, so the next
    /// cleanup retries; the other order would make a leftover file look live
    /// again.
    fn reclaim_deprecated_cpp_chunks(
        &self,
        table: &str,
        index_name: &str,
        segment: u32,
        dir: &Path,
    ) -> Result<()> {
        let deprecated = self.deprecated_chunk_ids(table, index_name, segment);
        if deprecated.is_empty() {
            return Ok(());
        }
        let mut gone = Vec::new();
        for id in deprecated {
            let path = dir.join(format!("chunk_{id}.idx"));
            if !path.exists() || std::fs::remove_file(&path).is_ok() {
                gone.push(id);
            }
        }
        if gone.is_empty() {
            return Ok(());
        }
        let table = self.qualified_table(table);
        let Some(meta) = self.storage.catalog.get_table_by_name(&table)? else {
            return Ok(());
        };
        let Ok(index_id) = self.cpp_index_position(&table, index_name) else {
            return Ok(());
        };
        let db_id = self.database_id_of(&table);
        infinity_catalog::cpp_layout::reclaim_cpp_index_chunks(
            self.storage.catalog.store().as_ref(),
            &db_id.to_string(),
            &meta.id.to_string(),
            &index_id.to_string(),
            segment,
            &gone,
        )?;
        Ok(())
    }

    /// The newest chunk file of `table`'s HNSW mirror.
    fn cpp_hnsw_chunk_path(&self, table: &str, index_name: &str) -> Result<PathBuf> {
        let dir = self
            .cpp_hnsw_dir(table, index_name)
            .ok_or_else(|| Error::NotFound(format!("index '{index_name}'")))?;
        self.live_chunk_files(table, index_name, 0, &dir)
            .pop()
            .map(|(_, p)| p)
            .ok_or_else(|| Error::NotFound(format!("no HNSW mirror at {}", dir.display())))
    }

    /// Writes `table`'s full-text index as the C++ `.dic`/`.pos`/`.len` chunk
    /// family under `{data_dir}/data/db_{db}/tbl_{tbl}/idx_{id}/seg_0/`,
    /// returning the number of documents indexed.
    pub fn write_cpp_fulltext_index(&self, table: &str, index_name: &str) -> Result<usize> {
        let table = self.qualified_table(table);
        let meta = self
            .storage
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let im = self
            .storage
            .catalog
            .list_indexes()?
            .into_iter()
            .find(|im| {
                im.name == index_name
                    && (im.table == table || table.rsplit('.').next() == Some(im.table.as_str()))
            })
            .ok_or_else(|| Error::NotFound(format!("index '{index_name}'")))?;
        let col = meta
            .columns
            .iter()
            .position(|c| c == &im.column)
            .ok_or_else(|| Error::NotFound(format!("column '{}'", im.column)))?;
        let index_id = self.cpp_index_position(&table, index_name)?;
        let db_id = self.database_id_of(&table);
        let tokenizer = self.index_tokenizer(&table, &im.column);
        // The reference's chunk keeps a deleted row: an UPDATE's old row stays
        // in the chunk it was written to and only the block's `version` hides
        // it, so the corpus the chunk indexes is every row ever appended.
        let rows = self.fetch_rows_including_deleted(&table)?;
        // The reference seals one chunk per **mem-index dump**, covering the
        // rows appended since the previous one and split at block boundaries;
        // the files are named `ft_{:016x}` after the chunk's first row id
        // (`new_txn_index_impl.cpp:860`). A 20000-row insert therefore ends up
        // as `ft_0000000000000000`, `ft_0000000000002000`,
        // `ft_0000000000004000`, and a second run that appended 110 rows to a
        // 100-row table as a single `ft_0000000000000064` - not as a rewrite
        // of the first chunk.
        //
        // The index is **per segment**: `MemoryIndexer` is constructed with
        // `segment_index_meta.GetSegmentIndexDir()` and a base row id whose
        // high bits are the segment, so an import into a second segment writes
        // `idx_{id}/seg_1/ft_0000000100000000.*` rather than extending the
        // first segment's chunk.
        let mut by_segment: std::collections::BTreeMap<u32, Vec<&(usize, Vec<Value>)>> =
            Default::default();
        for row in rows.iter() {
            by_segment
                .entry((row.0 as u64 >> 32) as u32)
                .or_default()
                .push(row);
        }
        let mut written = 0usize;
        for (segment, seg_rows) in by_segment {
            let dir = self.blocks_root().join(format!(
                "db_{db_id}/tbl_{}/idx_{index_id}/seg_{segment}",
                meta.id
            ));
            std::fs::create_dir_all(&dir).map_err(|e| Error::Io {
                path: dir.clone(),
                source: e,
            })?;
            // How many documents the chunks already on disk cover: a dump only
            // indexes the rows past them. A chunk that claims more rows than
            // the segment has (the table shrank) is dropped instead. The file
            // name carries the full `RowID`, so the count has to be taken from
            // the in-segment offset.
            let mut base = 0usize;
            let mut stale = Vec::new();
            if let Ok(entries) = std::fs::read_dir(&dir) {
                for entry in entries.flatten() {
                    let path = entry.path();
                    let name = path.file_name().and_then(|n| n.to_str()).unwrap_or("");
                    if !name.starts_with("ft_") {
                        continue;
                    }
                    stale.push(path.clone());
                    let Some(hex) = name
                        .strip_prefix("ft_")
                        .and_then(|n| n.strip_suffix(".len"))
                    else {
                        continue;
                    };
                    let Ok(start) = u64::from_str_radix(hex, 16) else {
                        continue;
                    };
                    let docs = std::fs::metadata(&path)
                        .map(|m| m.len() / 4)
                        .unwrap_or_default();
                    base = base.max(((start & 0xffff_ffff) + docs) as usize);
                }
            }
            if base > seg_rows.len() {
                for path in &stale {
                    let _ = std::fs::remove_file(path);
                }
                base = 0;
            }
            // The rows of `segment` are cut into chunks the way the reference
            // cuts them:
            //
            // * an **imported** extent is dumped once per `mem_index_capacity`
            //   (`PopulateFtIndexInner` checks the size after each block), so an
            //   import smaller than the capacity is a single chunk spanning
            //   every block of the segment;
            // * rows appended later (or a segment no `COPY` wrote) are dumped
            //   per **physical block** (`offset / 8192`): the memory indexer is
            //   handed one block at a time, and a block keeps its `base_row_id`
            //   even when rows in it were deleted.
            let mut ranges: Vec<Vec<&(usize, Vec<Value>)>> = Vec::new();
            let imported = self.imported_extent(&table, segment, seg_rows.len() as u64);
            let tail_start = match imported {
                Some(head) if base < head as usize => {
                    let capacity = self.memindex_capacity().max(1);
                    let head = head as usize;
                    let mut start = base;
                    let mut at = base;
                    let mut accumulated = 0usize;
                    while at < head {
                        let take = (head - at).min(DEFAULT_BLOCK_CAPACITY);
                        at += take;
                        accumulated += take;
                        if accumulated >= capacity {
                            ranges.push(seg_rows[start..at].to_vec());
                            start = at;
                            accumulated = 0;
                        }
                    }
                    if at > start {
                        ranges.push(seg_rows[start..at].to_vec());
                    }
                    at
                }
                _ => base,
            };
            let mut by_block: std::collections::BTreeMap<u64, Vec<&(usize, Vec<Value>)>> =
                Default::default();
            for row in seg_rows.iter().skip(tail_start) {
                by_block
                    .entry((row.0 as u64 & 0xffff_ffff) / DEFAULT_BLOCK_CAPACITY as u64)
                    .or_default()
                    .push(row);
            }
            ranges.extend(by_block.into_values());
            for block_rows in ranges {
                // The chunk is named after the row it starts at, which is the
                // block's first row only when the dump began on a block
                // boundary. The name carries the whole `RowID` - segment in
                // the high 32 bits - which is what the reference's
                // `MemoryIndexer` uses as its base name.
                let base_row_id = block_rows[0].0 as u64;
                // term -> (document ordinal, the term's position in the
                // document), in term order (the FST requires sorted keys). The
                // document ordinal restarts at zero in every chunk: the
                // chunk's `base_row_id` is what puts it back on the segment's
                // row ids (`MemoryIndexer::doc_count_` counts rows within the
                // chunk).
                let mut postings: std::collections::BTreeMap<String, Vec<(u32, u32)>> =
                    std::collections::BTreeMap::new();
                let mut lengths: Vec<u32> = Vec::with_capacity(block_rows.len());
                for (doc, (_, row)) in block_rows.iter().enumerate() {
                    let text = row.get(col).map(|v| v.to_slt_string()).unwrap_or_default();
                    let terms = crate::tokenize_index_positions(&text, &tokenizer);
                    lengths.push(terms.len() as u32);
                    for (t, position) in terms {
                        postings.entry(t).or_default().push((doc as u32, position));
                    }
                }

                let (dic, pos, len) = encode_fulltext_chunk(&postings, &lengths);
                let base = cpp_format::fulltext_base_name(base_row_id, None);
                for (name, bytes) in [
                    (format!("{base}.dic"), &dic),
                    (format!("{base}.pos"), &pos),
                    (format!("{base}.len"), &len),
                ] {
                    let path = dir.join(&name);
                    std::fs::write(&path, bytes).map_err(|e| Error::Io { path, source: e })?;
                }
                written += block_rows.len();
            }
        }
        Ok(written)
    }

    /// Reads back a mirror written by [`Self::write_cpp_hnsw_index`], returning
    /// `(row id, vector)` pairs **without** consulting the index metadata.
    pub fn read_cpp_hnsw_index(&self, table: &str, index_name: &str) -> Result<VectorRows> {
        let table = self.qualified_table(table);
        let path = self.cpp_hnsw_chunk_path(&table, index_name)?;
        let bytes = std::fs::read(&path)
            .map_err(|_| Error::NotFound(format!("no HNSW mirror at {}", path.display())))?;
        let parts = self
            .read_hnsw_chunk_parts(&table, index_name, &bytes)
            .ok_or_else(|| Error::Invalid("bad HNSW chunk".to_string()))?;
        let index = HnswIndex::from_chunk_parts(
            parts.dims,
            parts.m.max(1) as usize,
            (parts.ef as usize).max(64),
            parts.options.metric,
            parts.options.encoding,
            &parts.store_meta,
            &parts.records,
            parts.n,
            Vec::new(),
            Vec::new(),
            None,
            0,
        )
        .ok_or_else(|| Error::Invalid("bad HNSW store".to_string()))?;
        Ok((0..parts.n)
            .zip(parts.labels)
            .map(|(i, label)| (label, index.vector(i)))
            .collect())
    }

    /// The decoded pieces of an HNSW chunk: header, one row id per vector (in
    /// node order), the vectors themselves, and the graph.
    #[allow(clippy::type_complexity)]
    /// Decodes one HNSW chunk with the encoding/metric recorded in the
    /// catalog. The store meta length and the record size follow from those
    /// options and the dimension stored in the meta.
    fn read_hnsw_chunk_parts(
        &self,
        table: &str,
        index_name: &str,
        bytes: &[u8],
    ) -> Option<HnswChunkParts> {
        let options = self.hnsw_options(table, index_name);
        if bytes.len() < 32 {
            return None;
        }
        let u64_at = |at: usize| -> Option<u64> {
            bytes
                .get(at..at + 8)
                .and_then(|b| b.try_into().ok())
                .map(u64::from_le_bytes)
        };
        let m = u64_at(0)?;
        let ef = u64_at(8)?;
        let n = u64_at(16)? as usize;
        let dims = u64_at(24)? as usize;
        let meta_len = hnsw_store_meta_len(dims, options.metric, options.encoding);
        let record_size = hnsw_record_size(dims, options.encoding);
        let chunk = knn_format::decode_hnsw_chunk_ext(
            bytes,
            meta_len,
            record_size,
            knn_format::VERTEX_L0_SIZE,
            knn_format::VERTEX_LX_SIZE,
        )
        .ok()?;
        let vertices = knn_format::decode_graph_region(
            &chunk.graph,
            chunk.cur_vec_num as usize,
            chunk.mmax0,
            chunk.mmax,
        )
        .ok()?;
        Some(HnswChunkParts {
            m,
            ef,
            n,
            dims,
            max_layer: chunk.max_layer.max(0) as usize,
            enterpoint: (chunk.enterpoint >= 0).then_some(chunk.enterpoint as usize),
            store_meta: chunk.store_meta,
            records: chunk.vectors,
            vertices,
            labels: chunk.labels,
            options,
        })
    }

    fn assemble_loaded_index(
        parts: HnswChunkParts,
        chunk_id: u64,
        stamp: (u64, u64),
    ) -> Option<LoadedVectorIndex> {
        if parts.n == 0 {
            return None;
        }
        let levels: Vec<usize> = parts
            .vertices
            .iter()
            .map(|v| v.layer_count.max(0) as usize)
            .collect();
        let mut links: Vec<Vec<Vec<usize>>> = vec![Vec::new()];
        for (node, v) in parts.vertices.iter().enumerate() {
            while links[0].len() <= node {
                links[0].push(Vec::new());
            }
            links[0][node] = v.neighbors_l0.iter().map(|&x| x as usize).collect();
            for (l, nbrs) in v.upper.iter().enumerate() {
                let layer = l + 1;
                while links.len() <= layer {
                    links.push(Vec::new());
                }
                while links[layer].len() <= node {
                    links[layer].push(Vec::new());
                }
                links[layer][node] = nbrs.iter().map(|&x| x as usize).collect();
            }
        }
        let entry = parts.enterpoint.map(|e| e.min(parts.n.saturating_sub(1)));
        let graph_bytes: usize = links
            .iter()
            .flat_map(|layer| layer.iter())
            .map(|neighbors| neighbors.len() * 4 + 24)
            .sum();
        let bytes = parts.records.len() + parts.labels.len() * 4 + levels.len() * 8 + graph_bytes;
        let index = HnswIndex::from_chunk_parts(
            parts.dims,
            parts.m.max(1) as usize,
            (parts.ef as usize).max(64),
            parts.options.metric,
            parts.options.encoding,
            &parts.store_meta,
            &parts.records,
            parts.n,
            levels,
            links,
            entry,
            parts.max_layer,
        )?;
        Some(LoadedVectorIndex {
            index: HnswHandler::from_index(index),
            labels: parts.labels,
            chunk_id,
            bytes,
            stamp,
        })
    }

    /// Loads `index_name`'s persisted HNSW graph into an in-memory index, so
    /// `SEARCH MATCH VECTOR` can walk the graph that is on disk (plan #70).
    pub fn load_vector_index(&self, table: &str, index_name: &str) -> Result<LoadedVectorIndex> {
        let table = self.qualified_table(table);
        let path = self.cpp_hnsw_chunk_path(&table, index_name)?;
        let bytes = std::fs::read(&path)
            .map_err(|_| Error::NotFound(format!("no HNSW mirror at {}", path.display())))?;
        let parts = self
            .read_hnsw_chunk_parts(&table, index_name, &bytes)
            .ok_or_else(|| Error::Invalid("bad HNSW chunk".to_string()))?;
        Self::assemble_loaded_index(parts, 0, file_stamp(&path))
            .ok_or_else(|| Error::Invalid("HNSW mirror has no vectors".to_string()))
    }

    /// Every sealed HNSW chunk of `index_name`, oldest first. The reference
    /// keeps one graph per segment (`chunk_{n}.idx`); a query searches each and
    /// merges. Missing/undecodable chunks are skipped so one bad segment cannot
    /// disable the index.
    pub fn load_vector_indexes(&self, table: &str, index_name: &str) -> Vec<LoadedVectorIndex> {
        self.load_vector_indexes_after(table, index_name, None)
    }

    /// Loads only the chunks written after `after` (all when `None`), so a
    /// checkpoint reload does not re-decode graphs it already holds in memory.
    pub fn load_vector_indexes_after(
        &self,
        table: &str,
        index_name: &str,
        after: Option<u64>,
    ) -> Vec<LoadedVectorIndex> {
        let table = self.qualified_table(table);
        let Some(dir) = self.cpp_hnsw_dir(&table, index_name) else {
            return Vec::new();
        };
        let mut out = Vec::new();
        for (chunk_id, path) in self.live_chunk_files(&table, index_name, 0, &dir) {
            if after.is_some_and(|a| chunk_id <= a) {
                continue;
            }
            let Ok(bytes) = std::fs::read(&path) else {
                continue;
            };
            let Some(parts) = self.read_hnsw_chunk_parts(&table, index_name, &bytes) else {
                continue;
            };
            if let Some(loaded) = Self::assemble_loaded_index(parts, chunk_id, file_stamp(&path)) {
                out.push(loaded);
            }
        }
        out
    }

    /// The `chunk_{id}.idx` ids currently on disk for `index_name`. A merged
    /// chunk replaces the chunks cleanup deleted, so the in-memory set must be
    /// reconciled with this list (the reference's buffer manager only holds
    /// live index buffers).
    pub fn hnsw_chunk_ids(&self, table: &str, index_name: &str) -> Vec<u64> {
        let table = self.qualified_table(table);
        let Some(dir) = self.cpp_hnsw_dir(&table, index_name) else {
            return Vec::new();
        };
        self.live_chunk_files(&table, index_name, 0, &dir)
            .into_iter()
            .map(|(id, _)| id)
            .collect()
    }

    /// Decodes one on-disk chunk by id.
    pub fn load_vector_index_chunk(
        &self,
        table: &str,
        index_name: &str,
        chunk_id: u64,
    ) -> Option<LoadedVectorIndex> {
        let table = self.qualified_table(table);
        let dir = self.cpp_hnsw_dir(&table, index_name)?;
        let path = self
            .live_chunk_files(&table, index_name, 0, &dir)
            .into_iter()
            .find(|(id, _)| *id == chunk_id)
            .map(|(_, path)| path)?;
        let bytes = std::fs::read(&path).ok()?;
        let parts = self.read_hnsw_chunk_parts(&table, index_name, &bytes)?;
        Self::assemble_loaded_index(parts, chunk_id, file_stamp(&path))
    }

    /// `chunk_id -> (file size, mtime seconds)` for the live chunks.
    pub fn hnsw_chunk_stamps(
        &self,
        table: &str,
        index_name: &str,
    ) -> std::collections::HashMap<u64, (u64, u64)> {
        let table = self.qualified_table(table);
        let Some(dir) = self.cpp_hnsw_dir(&table, index_name) else {
            return Default::default();
        };
        self.live_chunk_files(&table, index_name, 0, &dir)
            .into_iter()
            .map(|(id, path)| (id, file_stamp(&path)))
            .collect()
    }

    /// Builds a C++ IVF `chunk_0.idx` body (plain storage, `kPlain`) for
    /// `column`'s vectors: a deterministic k-means partitions them into
    /// `centroid_count` parts (task #70).
    fn cpp_ivf_chunk(
        &self,
        table: &str,
        column: &str,
        options: &IvfOptions,
        segment: Option<u32>,
    ) -> Result<(usize, Vec<u8>)> {
        let vectors = self.cpp_column_vectors(table, column, segment)?;
        if vectors.is_empty() {
            return Err(Error::Invalid(format!(
                "no vectors to index in '{table}.{column}'"
            )));
        }
        let dim = vectors[0].1.len();
        // C++ `IVF_Index_Data::Train`: `min(sqrt(n) * ratio, n / min_points)`,
        // raised to 1 when that underflows (`ivf_index_data_impl.cpp:195-201`).
        let k = ((vectors.len() as f64).sqrt() * options.ratio as f64)
            .min((vectors.len() / options.min_points.max(1)) as f64) as usize;
        let k = k.clamp(1, vectors.len());
        // Deterministic seeding: pick k evenly spaced vectors.
        let mut centroids: Vec<Vec<f32>> = (0..k)
            .map(|c| vectors[c * vectors.len() / k].1.clone())
            .collect();
        let mut assign = vec![0usize; vectors.len()];
        for _ in 0..10 {
            let mut changed = false;
            for (i, (_, v)) in vectors.iter().enumerate() {
                let mut best = 0;
                let mut best_d = f32::INFINITY;
                for (c, cent) in centroids.iter().enumerate() {
                    let d = infinity_index::vector::l2_sq(v, cent);
                    if d < best_d {
                        best_d = d;
                        best = c;
                    }
                }
                if assign[i] != best {
                    assign[i] = best;
                    changed = true;
                }
            }
            // Recompute each centroid as the mean of its members.
            let mut sums = vec![vec![0.0f64; dim]; k];
            let mut counts = vec![0usize; k];
            for (i, (_, v)) in vectors.iter().enumerate() {
                counts[assign[i]] += 1;
                for (d, x) in v.iter().enumerate() {
                    sums[assign[i]][d] += *x as f64;
                }
            }
            for c in 0..k {
                if counts[c] > 0 {
                    centroids[c] = sums[c]
                        .iter()
                        .map(|s| (*s / counts[c] as f64) as f32)
                        .collect();
                }
            }
            if !changed {
                break;
            }
        }

        let header = knn_format::IvfChunkHeader {
            row_count: vectors.len() as u32,
            embedding_count: vectors.len() as u32,
            dimension: dim as u32,
            centroid_count: k as u32,
        };
        if options.storage.eq_ignore_ascii_case("scalar_quantization")
            || options.storage.eq_ignore_ascii_case("product_quantization")
        {
            let flat: Vec<f32> = centroids.iter().flatten().copied().collect();
            let chunk = if options.storage.eq_ignore_ascii_case("scalar_quantization") {
                Self::encode_ivf_scalar_quantized(
                    &header,
                    &flat,
                    &vectors,
                    &assign,
                    options.sq_bits,
                )?
            } else {
                Self::encode_ivf_product_quantized(&header, &flat, &vectors, &assign, options)?
            };
            return Ok((vectors.len(), chunk));
        }
        let mut parts = vec![knn_format::IvfPlainPart::default(); k];
        for (i, (label, v)) in vectors.iter().enumerate() {
            let part = &mut parts[assign[i]];
            part.segment_offsets.push(*label);
            for x in v {
                part.vectors.extend_from_slice(&x.to_le_bytes());
            }
        }
        let flat: Vec<f32> = centroids.into_iter().flatten().collect();
        let chunk = knn_format::encode_ivf_plain_chunk(&header, &flat, &parts)
            .map_err(|e| Error::Invalid(format!("cannot encode IVF chunk: {e}")))?;
        Ok((vectors.len(), chunk))
    }

    /// Scalar-quantized chunk (`IVF_Part_Storage_SQ`): train the shared
    /// `(a, b)` affine parameters on the residuals
    /// (`IVF_Parts_Storage_Info<kScalarQuantization>`), then store each
    /// vector's codes plus its `dot_v_e` (`AppendOneEmbedding`).
    fn encode_ivf_scalar_quantized(
        header: &knn_format::IvfChunkHeader,
        centroids: &[f32],
        vectors: &[(u32, Vec<f32>)],
        assign: &[usize],
        sq_bits: u8,
    ) -> Result<Vec<u8>> {
        let dim = header.dimension as usize;
        let residuals: Vec<Vec<f32>> = vectors
            .iter()
            .zip(assign)
            .map(|((_, v), part)| {
                let at = part * dim;
                v.iter()
                    .zip(&centroids[at..at + dim])
                    .map(|(x, c)| x - c)
                    .collect()
            })
            .collect();
        let (a, b) = infinity_index::ivf::train_scalar_quantization(&residuals, dim, sq_bits);
        let mut parts: Vec<knn_format::IvfSqPart> = (0..header.centroid_count)
            .map(|_| knn_format::IvfSqPart {
                segment_offsets: Vec::new(),
                codes: Vec::new(),
                dot_v_e: Vec::new(),
            })
            .collect();
        for (i, (label, v)) in vectors.iter().enumerate() {
            let (codes, dot_v_e) =
                infinity_index::ivf::encode_scalar_quantized(v, &residuals[i], &a, &b, sq_bits);
            let part = &mut parts[assign[i]];
            part.segment_offsets.push(*label);
            part.codes.extend_from_slice(&codes);
            part.dot_v_e.push(dot_v_e);
        }
        knn_format::encode_ivf_sq_chunk(header, centroids, &a, &b, &parts, sq_bits)
            .map_err(|e| Error::Invalid(format!("cannot encode IVF chunk: {e}")))
    }

    /// Product-quantized chunk (`IVF_Part_Storage_PQ`): train one codebook per
    /// subspace on the residuals, store one code per subspace and the
    /// `-0.5 * ||c||^2` weights the search formulas need.
    fn encode_ivf_product_quantized(
        header: &knn_format::IvfChunkHeader,
        centroids: &[f32],
        vectors: &[(u32, Vec<f32>)],
        assign: &[usize],
        options: &IvfOptions,
    ) -> Result<Vec<u8>> {
        let dim = header.dimension as usize;
        let residuals: Vec<Vec<f32>> = vectors
            .iter()
            .zip(assign)
            .map(|((_, v), part)| {
                let at = part * dim;
                v.iter()
                    .zip(&centroids[at..at + dim])
                    .map(|(x, c)| x - c)
                    .collect()
            })
            .collect();
        let (real, codewords, norms) = infinity_index::ivf::train_product_quantization(
            &residuals,
            dim,
            options.pq_subspace_num,
            options.pq_bits,
        );
        let word_size = if options.pq_bits > 12 { 2 } else { 1 };
        let mut parts: Vec<knn_format::IvfPqPart> = (0..header.centroid_count)
            .map(|_| knn_format::IvfPqPart {
                segment_offsets: Vec::new(),
                last_code_id: 0,
                codes: Vec::new(),
                word_size,
            })
            .collect();
        for (i, (label, _)) in vectors.iter().enumerate() {
            let codes = infinity_index::ivf::encode_product_quantized(
                &residuals[i],
                dim,
                options.pq_subspace_num,
                real,
                &codewords,
            );
            let part = &mut parts[assign[i]];
            part.segment_offsets.push(*label);
            infinity_index::ivf::pq_append_codes(
                &mut part.codes,
                part.last_code_id as usize,
                &codes,
                options.pq_bits,
            );
            part.last_code_id += options.pq_subspace_num as u64;
        }
        let codebook = knn_format::IvfPqCodebook {
            real_subspace_centroid_num: real as u32,
            codewords,
            subspace_weights: norms,
        };
        knn_format::encode_ivf_pq_chunk(header, centroids, &codebook, &parts)
            .map_err(|e| Error::Invalid(format!("cannot encode IVF chunk: {e}")))
    }

    /// Decodes one product-quantized chunk into a searchable segment.
    fn decode_ivf_pq_chunk(
        bytes: &[u8],
        options: &IvfOptions,
    ) -> Option<infinity_index::ivf::IvfSegment> {
        if bytes.len() < 20 {
            return None;
        }
        let u32_at = |at: usize| u32::from_le_bytes(bytes[at..at + 4].try_into().unwrap());
        let dim = u32_at(8) as usize;
        let centroid_count = u32_at(12) as usize;
        if dim == 0 || centroid_count == 0 {
            return None;
        }
        let real = u32_at(16 + centroid_count * dim * 4) as usize;
        let word_size = if options.pq_bits > 12 { 2 } else { 1 };
        let chunk = knn_format::decode_ivf_pq_chunk(
            bytes,
            word_size,
            dim * real,
            real * options.pq_subspace_num,
        )
        .ok()?;
        let parts = chunk
            .parts
            .into_iter()
            .map(|part| infinity_index::ivf::IvfPqPartData {
                offsets: part.segment_offsets,
                codes: part.codes,
            })
            .collect();
        Some(infinity_index::ivf::IvfSegment::Product(
            infinity_index::ivf::IvfPqIndex {
                dims: dim,
                centroids: chunk.centroids,
                centroid_count,
                subspace_num: options.pq_subspace_num,
                real_centroid_num: chunk.codebook.real_subspace_centroid_num as usize,
                codewords: chunk.codebook.codewords,
                norms_neg_half: chunk.codebook.subspace_weights,
                bits: options.pq_bits,
                parts,
            },
        ))
    }

    /// Writes `table`'s index as a C++ IVF chunk file (`kPlain`) under
    /// `…/idx_{id}/seg_0/chunk_0.idx`, returning how many vectors were written.
    pub fn write_cpp_ivf_index(&self, table: &str, index_name: &str) -> Result<usize> {
        let table = self.qualified_table(table);
        let meta = self
            .storage
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let index_id = self.cpp_index_position(&table, index_name)?;
        let column = self
            .storage
            .catalog
            .list_indexes()?
            .into_iter()
            .find(|im| {
                im.name == index_name
                    && (im.table == table || table.rsplit('.').next() == Some(im.table.as_str()))
            })
            .map(|im| im.column)
            .ok_or_else(|| Error::NotFound(format!("index '{index_name}'")))?;
        let options = self.ivf_storage_option(&table, index_name);
        // One chunk per segment: the reference trains and dumps per segment
        // (`PopulateIvfIndexInner` is handed a `SegmentMeta`), and the chunk's
        // offsets are in-segment (`RowID(segment, offset)`).
        let db_id = self.database_id_of(&table);
        let mut segments = self.segment_row_counts(&table);
        if segments.is_empty() {
            segments.push((0, 0));
        }
        let mut written = 0usize;
        for (segment, rows) in segments {
            if rows == 0 {
                continue;
            }
            let (count, chunk) = self.cpp_ivf_chunk(&table, &column, &options, Some(segment))?;
            let dir = self.blocks_root().join(format!(
                "db_{db_id}/tbl_{}/idx_{index_id}/seg_{segment}",
                meta.id
            ));
            std::fs::create_dir_all(&dir).map_err(|e| Error::Io {
                path: dir.clone(),
                source: e,
            })?;
            std::fs::write(dir.join("chunk_0.idx"), &chunk).map_err(|e| Error::Io {
                path: dir.clone(),
                source: e,
            })?;
            written += count;
        }
        Ok(written)
    }

    /// Reads back a mirror written by [`Self::write_cpp_ivf_index`], returning
    /// the IVF chunks so a query can probe them (plan #89).
    pub fn load_ivf_indexes(
        &self,
        table: &str,
        index_name: &str,
    ) -> Vec<infinity_index::ivf::IvfSegment> {
        let Some(dir) = self.cpp_hnsw_dir(table, index_name) else {
            return Vec::new();
        };
        let options = self.ivf_storage_option(table, index_name);
        let mut out = Vec::new();
        for (_, path) in hnsw_chunks(&dir) {
            let Ok(bytes) = std::fs::read(&path) else {
                continue;
            };
            if options.storage.eq_ignore_ascii_case("scalar_quantization") {
                if let Some(segment) = Self::decode_ivf_sq_chunk(&bytes, options.sq_bits) {
                    out.push(segment);
                }
                continue;
            }
            if options.storage.eq_ignore_ascii_case("product_quantization") {
                if let Some(segment) = Self::decode_ivf_pq_chunk(&bytes, &options) {
                    out.push(segment);
                }
                continue;
            }
            let Ok((header, flat, parts)) = knn_format::decode_ivf_plain_chunk(&bytes, 4) else {
                continue;
            };
            let dims = header.dimension as usize;
            if dims == 0 {
                continue;
            }
            let parts: Vec<Vec<(u32, Vec<f32>)>> = parts
                .into_iter()
                .map(|part| {
                    part.segment_offsets
                        .into_iter()
                        .enumerate()
                        .map(|(i, offset)| {
                            let at = i * dims * 4;
                            let vector = (0..dims)
                                .map(|d| {
                                    f32::from_le_bytes(
                                        part.vectors[at + d * 4..at + d * 4 + 4]
                                            .try_into()
                                            .unwrap(),
                                    )
                                })
                                .collect();
                            (offset, vector)
                        })
                        .collect()
                })
                .collect();
            out.push(infinity_index::ivf::IvfSegment::Plain(
                infinity_index::ivf::IvfIndex::new(
                    dims,
                    header.centroid_count as usize,
                    flat,
                    parts,
                ),
            ));
        }
        out
    }

    /// The index's IVF options (`IndexIVFStorageOption` + the centroid option),
    /// defaulting exactly like the reference: plain storage and
    /// `centroids_num_ratio = 1.0` / `min_points_per_centroid = 32`.
    fn ivf_storage_option(&self, table: &str, index_name: &str) -> IvfOptions {
        let table = self.qualified_table(table);
        let options = self
            .storage
            .catalog
            .list_indexes()
            .ok()
            .and_then(|list| {
                list.into_iter()
                    .find(|im| {
                        im.name == index_name
                            && (im.table == table
                                || table.rsplit('.').next() == Some(im.table.as_str()))
                    })
                    .map(|im| im.options)
            })
            .unwrap_or_default();
        let find = |key: &str| {
            options
                .iter()
                .find(|(k, _)| k.eq_ignore_ascii_case(key))
                .map(|(_, v)| v.trim().trim_matches(|c| c == '\'' || c == '"').to_string())
        };
        IvfOptions {
            storage: find("STORAGE_TYPE").unwrap_or_else(|| "plain".to_string()),
            sq_bits: find("SCALAR_QUANTIZATION_BITS")
                .and_then(|v| v.parse::<u8>().ok())
                .filter(|b| *b == 4 || *b == 8)
                .unwrap_or(8),
            pq_subspace_num: find("PRODUCT_QUANTIZATION_SUBSPACE_NUM")
                .and_then(|v| v.parse::<usize>().ok())
                .filter(|v| *v > 0)
                .unwrap_or(1),
            pq_bits: find("PRODUCT_QUANTIZATION_SUBSPACE_BITS")
                .and_then(|v| v.parse::<u8>().ok())
                .filter(|b| [4u8, 8, 12, 16].contains(b))
                .unwrap_or(8),
            ratio: find("CENTROIDS_NUM_RATIO")
                .and_then(|v| v.parse::<f32>().ok())
                .unwrap_or(1.0),
            min_points: find("MIN_POINTS_PER_CENTROID")
                .and_then(|v| v.parse::<usize>().ok())
                .filter(|v| *v > 0)
                .unwrap_or(32),
        }
    }

    /// Decodes one scalar-quantized chunk into a searchable segment.
    fn decode_ivf_sq_chunk(bytes: &[u8], sq_bits: u8) -> Option<infinity_index::ivf::IvfSegment> {
        let chunk = knn_format::decode_ivf_sq_chunk(bytes, sq_bits).ok()?;
        let dims = chunk.header.dimension as usize;
        if dims == 0 {
            return None;
        }
        let parts = chunk
            .parts
            .into_iter()
            .map(|part| infinity_index::ivf::IvfSqPartData {
                offsets: part.segment_offsets,
                codes: part.codes,
                dot_v_e: part.dot_v_e,
            })
            .collect();
        Some(infinity_index::ivf::IvfSegment::Scalar(
            infinity_index::ivf::IvfSqIndex {
                dims,
                centroids: chunk.centroids,
                centroid_count: chunk.header.centroid_count as usize,
                a: chunk.common_vec_a,
                b: chunk.common_vec_b,
                sq_bits,
                parts,
            },
        ))
    }

    /// Reads back a mirror written by [`Self::write_cpp_ivf_index`], returning
    /// the centroids and `(row id, vector)` pairs.
    pub fn read_cpp_ivf_index(
        &self,
        table: &str,
        index_name: &str,
    ) -> Result<(Vec<f32>, VectorRows)> {
        let table = self.qualified_table(table);
        let meta = self
            .storage
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let index_id = self.cpp_index_position(&table, index_name)?;
        let db_id = self.database_id_of(&table);
        let path = self.blocks_root().join(format!(
            "db_{db_id}/tbl_{}/idx_{index_id}/seg_0/chunk_0.idx",
            meta.id
        ));
        let bytes = std::fs::read(&path)
            .map_err(|_| Error::NotFound(format!("no IVF mirror at {}", path.display())))?;
        let (header, centroids, parts) = knn_format::decode_ivf_plain_chunk(&bytes, 4)
            .map_err(|e| Error::Invalid(format!("bad IVF chunk: {e}")))?;
        let dim = header.dimension as usize;
        let mut out = Vec::new();
        for part in parts {
            for (i, label) in part.segment_offsets.iter().enumerate() {
                let at = i * dim * 4;
                let v = part.vectors[at..at + dim * 4]
                    .chunks_exact(4)
                    .map(|c| f32::from_le_bytes(c.try_into().unwrap()))
                    .collect();
                out.push((*label, v));
            }
        }
        Ok((centroids, out))
    }

    /// PLAID index options of `index_name` (`nbits`, `n_centroids`,
    /// `colbertsar`), from `CREATE INDEX … USING PLAID WITH (…)`.
    fn plaid_options(&self, table: &str, index_name: &str) -> PlaidOptions {
        let short = table.rsplit('.').next().unwrap_or(table);
        let Some(im) = self
            .storage
            .catalog
            .list_indexes()
            .unwrap_or_default()
            .into_iter()
            .find(|im| im.name == index_name && (im.table == table || im.table == short))
        else {
            return PlaidOptions::default();
        };
        let mut options = PlaidOptions::default();
        for (key, value) in &im.options {
            let value = value.trim();
            if key.eq_ignore_ascii_case("nbits") {
                if let Ok(v) = value.parse() {
                    options.nbits = v;
                }
            } else if key.eq_ignore_ascii_case("n_centroids") {
                if let Ok(v) = value.parse() {
                    options.n_centroids = v;
                }
            } else if key.eq_ignore_ascii_case("colbertsar_mode") {
                options.colbertsar = value.eq_ignore_ascii_case("true") || value == "1";
            }
        }
        options
    }

    /// The tensor documents of `table.column` as `(row id, tokens)`.
    fn cpp_column_docs(
        &self,
        table: &str,
        column: &str,
        dims: usize,
    ) -> Result<Vec<(u32, Vec<Vec<f32>>)>> {
        let schema = self.table_schema(table)?;
        let col_idx = schema
            .iter()
            .position(|c| c == column)
            .ok_or_else(|| Error::NotFound(format!("column '{column}' on '{table}'")))?;
        let mut out = Vec::new();
        for (seq, row) in self.fetch_rows_with_ids(table)? {
            let text = match row.get(col_idx) {
                Some(Value::Str(s)) => s.clone(),
                Some(other) => other.to_slt_string(),
                None => continue,
            };
            if let Some(doc) = crate::parse_multivector_doc(&text, dims) {
                out.push((seq as u32, doc));
            }
        }
        Ok(out)
    }

    /// Builds a PLAID index over `column` and serializes it in the reference's
    /// `PlaidIndex::SaveIndexInner` layout (the `chunk_0.idx` the file worker
    /// writes).
    fn cpp_plaid_chunk(
        &self,
        table: &str,
        column: &str,
        dims: usize,
        options: &PlaidOptions,
    ) -> Result<(usize, Vec<u8>)> {
        let docs = self.cpp_column_docs(table, column, dims)?;
        let total: usize = docs.iter().map(|(_, d)| d.len()).sum();
        if total == 0 {
            return Err(Error::Invalid(format!(
                "no tensors to index in '{table}.{column}'"
            )));
        }
        let flat: Vec<f32> = docs
            .iter()
            .flat_map(|(_, d)| d.iter().flatten().copied())
            .collect();
        let lens: Vec<u32> = docs.iter().map(|(_, d)| d.len() as u32).collect();
        let mut k = if options.n_centroids == 0 {
            PlaidIndex::compute_auto_n_centroids(total as u64)
        } else {
            options.n_centroids
        };
        // Mirror `PlaidIndexInMem::BuildIndexFromScratch`: downgrade below the
        // k-means data bound, keep the multiple-of-8 alignment.
        while k > 8 && total < 32 * k as usize {
            k >>= 1;
        }
        k = k.div_ceil(8) * 8;
        if (k as usize) > total {
            return Err(Error::Invalid(format!(
                "not enough tensors for a PLAID index in '{table}.{column}': {total} < {k}"
            )));
        }
        let mut index = PlaidIndex::new(0, dims as u32, options.nbits, k, options.colbertsar)?;
        index.train(k, &flat, total, 4)?;
        index.add_multiple_docs_embeddings(&flat, &lens)?;
        Ok((total, index.to_bytes()))
    }

    /// Where a table's PLAID mirror lives and what it indexes.
    fn cpp_plaid_mirror(&self, table: &str, index_name: &str) -> Result<PlaidMirror> {
        let table = self.qualified_table(table);
        let meta = self
            .storage
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let index_id = self.cpp_index_position(&table, index_name)?;
        let column = self
            .storage
            .catalog
            .list_indexes()?
            .into_iter()
            .find(|im| {
                im.name == index_name
                    && (im.table == table || table.rsplit('.').next() == Some(im.table.as_str()))
            })
            .map(|im| im.column)
            .ok_or_else(|| Error::NotFound(format!("index '{index_name}'")))?;
        let ci = meta
            .columns
            .iter()
            .position(|c| c == &column)
            .ok_or_else(|| Error::NotFound(format!("column '{column}'")))?;
        let type_text = meta
            .types
            .get(ci)
            .cloned()
            .ok_or_else(|| Error::NotFound(format!("column '{column}'")))?;
        let (_, dims, elem) = tensor_shape(&type_text)
            .ok_or_else(|| Error::Invalid(format!("not a tensor column: '{type_text}'")))?;
        let db_id = self.database_id_of(&table);
        Ok(PlaidMirror {
            dir: self
                .blocks_root()
                .join(format!("db_{db_id}/tbl_{}/idx_{index_id}/seg_0", meta.id)),
            column,
            dims: dims as u32,
            elem,
        })
    }

    /// Every `chunk_{id}.idx` of `table`'s PLAID mirror, decoded in chunk order.
    fn read_plaid_chunks(&self, table: &str, index_name: &str) -> Result<Vec<PlaidChunkFile>> {
        let mirror = self.cpp_plaid_mirror(table, index_name)?;
        let nbits = self
            .plaid_options(&self.qualified_table(table), index_name)
            .nbits;
        let mut chunks = Vec::new();
        for (id, path) in self.live_chunk_files(table, index_name, 0, &mirror.dir) {
            let bytes = std::fs::read(&path).map_err(|e| Error::Io {
                path: path.clone(),
                source: e,
            })?;
            let (index, _) =
                PlaidIndex::from_bytes(0, mirror.dims, nbits, &bytes).map_err(|e| {
                    Error::Invalid(format!("bad PLAID chunk '{}': {e}", path.display()))
                })?;
            chunks.push(PlaidChunkFile { id, index });
        }
        Ok(chunks)
    }

    /// Reads every chunk of `table`'s PLAID mirror individually.
    pub fn read_cpp_plaid_chunks(&self, table: &str, index_name: &str) -> Result<Vec<PlaidIndex>> {
        Ok(self
            .read_plaid_chunks(table, index_name)?
            .into_iter()
            .map(|chunk| chunk.index)
            .collect())
    }

    /// `PlaidIndex::InitializeMerge` → `MergeOneChunk` with a cumulative
    /// `doc_offset` → `FinalizeMerge` → `CopyCentroidsFrom`, driven the way
    /// `NewTxn::OptimizePlaidIndex` (`new_txn_index_impl.cpp:2348-2424`) drives
    /// it.
    fn merge_plaid_chunk_indexes(chunks: &[PlaidIndex]) -> Result<PlaidIndex> {
        let first = chunks
            .first()
            .ok_or_else(|| Error::Invalid("no PLAID chunk to merge".into()))?;
        let mut merged = PlaidIndex::new(
            0,
            first.embedding_dimension(),
            first.nbits,
            first.n_centroids(),
            first.is_colbertsar_mode(),
        )?;
        merged.initialize_merge(first.n_centroids());
        let mut doc_offset = 0u32;
        for chunk in chunks {
            if chunk.doc_num() == 0 {
                continue;
            }
            merged.merge_one_chunk(chunk, doc_offset)?;
            doc_offset += chunk.doc_num();
        }
        merged.finalize_merge();
        // `preserve_ivf_lists = true`: `MergeOneChunk` already rebuilt them for
        // the merged document ids.
        merged.copy_centroids_from(
            first.centroids_data(),
            first.centroid_norms_neg_half(),
            first.n_centroids(),
            first.quantizer(),
            true,
        )?;
        Ok(merged)
    }

    /// The optimize merge (`NewTxn::OptimizePlaidIndex`): folds every chunk
    /// into one and appends it as the newest `chunk_{n}.idx`. The chunks it
    /// folded in are dropped (the reference deprecates them in the chunk meta;
    /// the mirror has no meta, so the files themselves are the record) —
    /// [`Self::cleanup_cpp_index_chunks`] keeps the same rule as a safety net
    /// for a removal that failed. Returns the merged document count.
    /// Returns the chunk it folded the segment's chunks into, or `None` when
    /// there was nothing to merge - the entry an optimize transaction records.
    pub(crate) fn merge_cpp_plaid_index(
        &self,
        table: &str,
        index_name: &str,
    ) -> Result<Option<ChunkRewrite>> {
        let mirror = self.cpp_plaid_mirror(table, index_name)?;
        let chunks = self.read_plaid_chunks(table, index_name)?;
        if chunks.len() < 2 {
            return Ok(None);
        }
        let next = chunks.last().map(|c| c.id + 1).unwrap_or(0);
        let deprecate_ids: Vec<u32> = chunks.iter().map(|c| c.id as u32).collect();
        let indexes: Vec<PlaidIndex> = chunks.into_iter().map(|c| c.index).collect();
        let merged = Self::merge_plaid_chunk_indexes(&indexes)?;
        let path = mirror.dir.join(format!("chunk_{next}.idx"));
        std::fs::write(&path, merged.to_bytes()).map_err(|e| Error::Io {
            path: path.clone(),
            source: e,
        })?;
        Ok(Some(ChunkRewrite {
            segment: 0,
            chunk_id: next as u32,
            row_count: merged.doc_num() as u32,
            deprecate_ids,
        }))
    }

    /// Writes `table`'s PLAID index as `idx_{id}/seg_0/chunk_{n}.idx`, appending
    /// one chunk per dump the way the reference's memory-index dump does: the
    /// rows past the ones already mirrored become the new chunk, sharing the
    /// first chunk's global centroids instead of retraining.
    pub fn write_cpp_plaid_index(&self, table: &str, index_name: &str) -> Result<usize> {
        let table = self.qualified_table(table);
        let mirror = self.cpp_plaid_mirror(&table, index_name)?;
        if mirror.elem != 6 {
            return Err(Error::Invalid(
                "PLAID only indexes TENSOR(FLOAT, dim) columns".into(),
            ));
        }
        let options = self.plaid_options(&table, index_name);
        std::fs::create_dir_all(&mirror.dir).map_err(|e| Error::Io {
            path: mirror.dir.clone(),
            source: e,
        })?;
        let chunks = self.read_plaid_chunks(&table, index_name)?;
        if chunks.is_empty() {
            let (count, bytes) =
                self.cpp_plaid_chunk(&table, &mirror.column, mirror.dims as usize, &options)?;
            let path = mirror.dir.join("chunk_0.idx");
            std::fs::write(&path, &bytes).map_err(|e| Error::Io {
                path: path.clone(),
                source: e,
            })?;
            // `SegmentIndexMeta::SavePlaidGlobalCentroids`: the trained
            // centroids and their quantizer live in a sidecar next to the
            // chunk, and a reference reader needs it to open the segment.
            if let Some(index) = self
                .read_cpp_plaid_chunks(&table, index_name)
                .ok()
                .and_then(|chunks| chunks.into_iter().next())
            {
                let centroids_path = mirror.dir.join("plaid_centroids.bin");
                std::fs::write(&centroids_path, index.save_plaid_centroids()).map_err(|e| {
                    Error::Io {
                        path: centroids_path.clone(),
                        source: e,
                    }
                })?;
            }
            return Ok(count);
        }
        // The mirrored rows are the concatenation of the chunks, so a new dump
        // only carries the rows past that watermark. A merged newest chunk
        // already covers the older ones and replaces them in the count.
        let sizes: Vec<(u32, u32)> = chunks
            .iter()
            .map(|c| (c.index.doc_num(), c.index.total_embedding_num()))
            .collect();
        let (watermark, indexed) = plaid_coverage(&sizes);
        let watermark = watermark as usize;
        let docs = self.cpp_column_docs(&table, &mirror.column, mirror.dims as usize)?;
        if docs.len() <= watermark {
            return Ok(indexed as usize);
        }
        let fresh = &docs[watermark..];
        let lens: Vec<u32> = fresh
            .iter()
            .map(|(_, tokens)| tokens.len() as u32)
            .collect();
        let total: usize = lens.iter().map(|len| *len as usize).sum();
        if total == 0 {
            return Ok(indexed as usize);
        }
        let flat: Vec<f32> = fresh
            .iter()
            .flat_map(|(_, tokens)| tokens.iter().flatten().copied())
            .collect();
        let first = &chunks[0].index;
        let mut index = PlaidIndex::new(
            0,
            mirror.dims,
            first.nbits,
            first.n_centroids(),
            first.is_colbertsar_mode(),
        )?;
        // `PlaidIndex::ShareGlobalCentroids`: a later chunk reuses the trained
        // centroids and quantizer of the first one.
        index.copy_centroids_from(
            first.centroids_data(),
            first.centroid_norms_neg_half(),
            first.n_centroids(),
            first.quantizer(),
            false,
        )?;
        index.add_multiple_docs_embeddings(&flat, &lens)?;
        let next = chunks.last().map(|c| c.id + 1).unwrap_or(0);
        let path = mirror.dir.join(format!("chunk_{next}.idx"));
        std::fs::write(&path, index.to_bytes()).map_err(|e| Error::Io {
            path: path.clone(),
            source: e,
        })?;
        Ok(indexed as usize + total)
    }

    /// Reads back a mirror written by [`Self::write_cpp_plaid_index`]. A merged
    /// newest chunk is read alone; otherwise the chunks are merged the same way
    /// [`Self::merge_cpp_plaid_index`] merges them.
    pub fn read_cpp_plaid_index(&self, table: &str, index_name: &str) -> Result<PlaidIndex> {
        let chunks = self.read_plaid_chunks(table, index_name)?;
        let doc_nums: Vec<u32> = chunks.iter().map(|c| c.index.doc_num()).collect();
        let mut indexes: Vec<PlaidIndex> = if plaid_merged_tail(&doc_nums) {
            // The merge already folded every older chunk into the newest one.
            chunks.into_iter().rev().take(1).map(|c| c.index).collect()
        } else {
            chunks.into_iter().map(|c| c.index).collect()
        };
        match indexes.len() {
            0 => Err(Error::NotFound(format!(
                "no PLAID mirror for '{index_name}'"
            ))),
            1 => Ok(indexes.remove(0)),
            _ => Self::merge_plaid_chunk_indexes(&indexes),
        }
    }
}

/// Where a table's PLAID mirror lives and what it indexes.
struct PlaidMirror {
    dir: PathBuf,
    column: String,
    dims: u32,
    /// SDK `ElementType` code of the tensor element (6 = float).
    elem: i32,
}

/// One `chunk_{id}.idx` of a PLAID mirror.
struct PlaidChunkFile {
    id: u64,
    index: PlaidIndex,
}

/// Whether the newest chunk of a mirror already holds every document the older
/// ones hold, i.e. it is the output of an optimize merge. A merge drops the
/// chunks it folded in; this is what tells a reader (and a leftover set of
/// files whose removal failed) to count the newest chunk alone.
fn plaid_merged_tail(doc_nums: &[u32]) -> bool {
    match doc_nums.split_last() {
        Some((newest, older)) if !older.is_empty() => *newest >= older.iter().sum::<u32>(),
        _ => false,
    }
}

/// `(documents, embeddings)` the chunks of a mirror already hold, in chunk-id
/// order, counting a merged newest chunk instead of the ones it supersedes.
fn plaid_coverage(sizes: &[(u32, u32)]) -> (u32, u32) {
    let doc_nums: Vec<u32> = sizes.iter().map(|size| size.0).collect();
    match sizes.last() {
        Some(&newest) if plaid_merged_tail(&doc_nums) => newest,
        _ => (
            sizes.iter().map(|size| size.0).sum(),
            sizes.iter().map(|size| size.1).sum(),
        ),
    }
}

/// `PlaidIndex::SaveIndexInner`'s header: `format_version`,
/// `start_segment_offset`, `embedding_dimension`, `nbits`, `n_centroids`,
/// `n_docs`. Reading the count from the header avoids decoding a chunk that
/// cleanup only has to look at.
fn plaid_chunk_doc_num(path: &std::path::Path) -> Option<u32> {
    use std::io::Read;
    let mut head = [0u8; 24];
    std::fs::File::open(path).ok()?.read_exact(&mut head).ok()?;
    Some(u32::from_le_bytes(head[20..24].try_into().ok()?))
}

/// PLAID index parameters from `WITH (nbits=…, n_centroids=…)`.
#[derive(Debug, Clone, Copy)]
struct PlaidOptions {
    nbits: u32,
    n_centroids: u32,
    colbertsar: bool,
}

impl Default for PlaidOptions {
    fn default() -> Self {
        Self {
            nbits: 4,
            n_centroids: 0,
            colbertsar: false,
        }
    }
}

fn file_stamp(path: &std::path::Path) -> (u64, u64) {
    std::fs::metadata(path)
        .ok()
        .map(|m| {
            let modified = m
                .modified()
                .ok()
                .and_then(|t| t.duration_since(std::time::UNIX_EPOCH).ok())
                .map(|d| d.as_secs())
                .unwrap_or(0);
            (m.len(), modified)
        })
        .unwrap_or((0, 0))
}

/// Serializes an existing graph as a C++ HNSW chunk (vec-store meta, graph
/// region, `u32` labels). The vector region depends on the encoding: plain
/// f32 vectors, LVQ i8 codes or RaBitQ 1-bit codes.
fn encode_hnsw_chunk_from(index: &HnswIndex, labels: &[u32]) -> (usize, Vec<u8>) {
    let m = index.m() as u64;
    let (mmax0, mmax) = knn_format::get_mmax(m);
    let ef_construction = index.ef_construction() as u64;
    let max_layer = index.max_level() as i32;
    let enterpoint = index.entry_point().map(|e| e as i32).unwrap_or(0);
    let store_meta = index.store_meta_bytes();
    let vector_region = index.store_records_bytes();
    // Neighbour counts must not exceed the level's limit.
    let vertices: Vec<knn_format::HnswGraphVertex> = (0..index.len())
        .map(|n| {
            let level = index.level_of(n);
            let neighbors_l0 = index
                .neighbors(0, n)
                .iter()
                .take(mmax0 as usize)
                .map(|&x| x as u32)
                .collect();
            let upper = (1..=level)
                .map(|l| {
                    index
                        .neighbors(l, n)
                        .iter()
                        .take(mmax as usize)
                        .map(|&x| x as u32)
                        .collect()
                })
                .collect();
            knn_format::HnswGraphVertex {
                layer_count: level as i32,
                neighbors_l0,
                upper,
            }
        })
        .collect();
    let graph = knn_format::encode_graph_region(&vertices, mmax0, mmax);
    let chunk = knn_format::encode_hnsw_chunk_ext(
        m,
        ef_construction,
        index.len() as u64,
        mmax0,
        mmax,
        max_layer,
        enterpoint,
        &store_meta,
        &vector_region,
        &graph,
        labels,
    );
    (index.len(), chunk)
}

/// One row at a time out of a table's blocks, in row-id order.
///
/// [`Engine::block_cursor`] builds it: the visible blocks are resolved once
/// (the same `visible_block_ids` the scan uses), and each block is decoded on
/// first touch and kept until the walk moves to the next one. That makes a
/// compaction - which reads every row of its source segments and writes them
/// into one new segment - one pass over the blocks instead of one table scan
/// per moved row.
/// A snapshot of one table's append log, for a reader pinned to a timestamp.
pub(crate) struct AppendView {
    totals: Vec<u64>,
    stamps: Vec<u64>,
    bases: Vec<(u32, u64)>,
}

impl AppendView {
    /// Whether the append that produced `seq` had committed by `read_ts`.
    ///
    /// The records are cumulative and in row-id order, so the record that covers
    /// the row is the first total past its ordinal. A record with no stamp is
    /// one a replay rebuilt (or one written before the log kept stamps): it
    /// counts as committed, because hiding rows whose append ts was never
    /// recorded would lose them on every reopen.
    pub(crate) fn covers(&self, seq: usize, read_ts: u64) -> bool {
        if self.totals.is_empty() {
            return true;
        }
        let segment = (seq >> 32) as u32;
        let mut ordinal = seq;
        let mut seen = 0usize;
        for (id, rows) in &self.bases {
            if *id == segment {
                ordinal = seen + (seq & 0xffff_ffff);
                break;
            }
            seen += *rows as usize;
        }
        let index = self.totals.partition_point(|total| (*total as usize) <= ordinal);
        match self.stamps.get(index).copied().unwrap_or(0) {
            0 => true,
            stamp => stamp <= read_ts,
        }
    }
}

pub(crate) struct BlockCursor<'a> {
    engine: &'a Engine,
    /// Fully-qualified table name (`db.table`), the form the row keys use.
    table: String,
    meta: infinity_catalog::TableMeta,
    types: Vec<i32>,
    /// `(segment, block)` of the blocks a reader may see.
    visible: std::collections::HashSet<(u32, u16)>,
    root: PathBuf,
    /// The block last decoded, with the `version` that says which of its slots
    /// are live.
    block: Option<(BlockId, Vec<(usize, Vec<String>)>, BlockVersionFile)>,
    /// Rows a commit appended since the last checkpoint (the append buffer),
    /// by row id: they are not in a block file yet.
    pending: std::collections::HashMap<usize, Vec<Value>>,
}

impl<'a> BlockCursor<'a> {
    fn new(engine: &'a Engine, table: &str) -> Result<Self> {
        let table = engine.qualified_table(table);
        let meta = engine
            .storage
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let types: Vec<i32> = meta
            .types
            .iter()
            .map(|t| column_type_of(t))
            .collect::<Option<Vec<_>>>()
            .ok_or_else(|| {
                Error::Invalid(format!(
                    "a column of '{table}' has no C++ block layout, so its rows are not in blocks"
                ))
            })?;
        let visible = engine
            .visible_block_ids(&table)
            .into_iter()
            .map(|id| (id.segment_id, id.block_id))
            .collect();
        let pending = engine
            .pending_rows_of(&table)
            .into_iter()
            .collect::<std::collections::HashMap<_, _>>();
        Ok(Self {
            engine,
            table,
            meta,
            types,
            visible,
            root: engine.blocks_root(),
            block: None,
            pending,
        })
    }

    /// The stored text of `seq`, ready to be written back under another id, or
    /// `None` when the row is deleted or outside the visible blocks.
    ///
    /// Journal first: a row written after the last checkpoint (and every row
    /// while the mirror is dirty) is there verbatim, tombstone included. A row
    /// the blocks hold is re-encoded through the same text form
    /// `restore_cpp_blocks` uses, so a compaction moves exactly what a restart
    /// would rebuild.
    pub(crate) fn row(&mut self, seq: usize) -> Result<Option<Vec<u8>>> {
        if self
            .engine
            .storage
            .store
            .get(&del_key(&self.table, seq))?
            .is_some()
        {
            return Ok(None);
        }
        // A delete the mirror has not written yet lives in the table's delete
        // log (`note_deleted_ids`); the block's `version` has it afterwards.
        if self.engine.deleted_ids(&self.table).contains(&(seq as u64)) {
            return Ok(None);
        }
        if let Some(values) = self.pending.get(&seq) {
            let literals: Vec<Literal> = values
                .iter()
                .map(|v| Literal::Str(mirror_cell_text(v)))
                .collect();
            return Ok(Some(
                self.engine
                    .encode_row_stored(&self.table, &literals, &self.meta.types)?
                    .into_bytes(),
            ));
        }
        if let Some(value) = self
            .engine
            .storage
            .store
            .get(&row_key(&self.table, seq))?
        {
            return Ok(Some(value));
        }
        let segment = (seq >> 32) as u32;
        let offset = (seq as u64 & 0xffff_ffff) as usize;
        let block = (offset / DEFAULT_BLOCK_CAPACITY) as u16;
        let slot = offset % DEFAULT_BLOCK_CAPACITY;
        if !self.visible.contains(&(segment, block)) {
            return Ok(None);
        }
        let id = BlockId {
            db_id: self.engine.database_id_of(&self.table),
            table_id: self.meta.id,
            segment_id: segment,
            block_id: block,
        };
        if self
            .block
            .as_ref()
            .map(|(cached, _, _)| *cached != id)
            .unwrap_or(true)
        {
            let Some(cells) = self.engine.decode_block_cells(
                &self.table,
                &self.root,
                &self.meta,
                &self.types,
                id,
                None,
            ) else {
                return Ok(None);
            };
            let Some(version) = self.engine.read_block_version(id) else {
                return Ok(None);
            };
            self.block = Some((id, cells, version));
        }
        let (_, cells, version) = self.block.as_ref().unwrap();
        if version.deleted.get(slot).copied().unwrap_or(0) != 0 {
            return Ok(None);
        }
        let Some((_, cells)) = cells.get(slot) else {
            return Ok(None);
        };
        let literals: Vec<Literal> = cells.iter().map(|c| Literal::Str(c.clone())).collect();
        Ok(Some(
            self.engine
                .encode_row_stored(&self.table, &literals, &self.meta.types)?
                .into_bytes(),
        ))
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::QueryResult;
    use tempfile::tempdir;


    /// A later statement skips a block the engine has already measured.
    ///
    /// The reference writes `blk|…|fast_rough_filter` only for a sealed
    /// segment, so a table whose rows were appended has no prefilter; the scan
    /// therefore decodes every block. The min/max collected during that decode
    /// let the next statement skip a block that cannot hold the value - as long
    /// as the block is unchanged, which the key's `version` identity checks.
    #[test]
    fn the_read_side_stats_skip_a_block_that_cannot_match() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE stats_t (a INT);").unwrap();
        let values: Vec<String> = (0..8_200).map(|i| format!("({i})")).collect();
        e.execute(&format!("INSERT INTO stats_t VALUES {};", values.join(", ")))
            .unwrap();
        e.flush().unwrap();
        // Two blocks: a = 8_199 is in the second one.
        assert_eq!(
            row_strings(&e, "SELECT a FROM stats_t WHERE a = 8199;"),
            vec![vec!["8199".to_string()]]
        );
        // The first scan measured both blocks; the second one can skip the
        // first without reading it.
        let before = PRUNED_BLOCKS.with(|n| n.get());
        assert_eq!(
            row_strings(&e, "SELECT a FROM stats_t WHERE a = 8199;"),
            vec![vec!["8199".to_string()]]
        );
        assert!(PRUNED_BLOCKS.with(|n| n.get()) > before);
        // A value no block holds still answers empty, and a range keeps the
        // blocks its bounds do not rule out.
        assert!(row_strings(&e, "SELECT a FROM stats_t WHERE a = 999999;").is_empty());
        assert_eq!(
            row_strings(&e, "SELECT a FROM stats_t WHERE a >= 8198;"),
            vec![vec!["8198".to_string()], vec!["8199".to_string()]]
        );
        // A rewrite moves the block's `version`, so the stale stats are not
        // used: the new row is found, and the old ones still are.
        e.execute("INSERT INTO stats_t VALUES (8200);").unwrap();
        e.flush().unwrap();
        assert_eq!(
            row_strings(&e, "SELECT a FROM stats_t WHERE a = 8200;"),
            vec![vec!["8200".to_string()]]
        );
        assert_eq!(
            row_strings(&e, "SELECT a FROM stats_t WHERE a = 8199;"),
            vec![vec!["8199".to_string()]]
        );
    }

    fn row_strings(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
        let mut out = Vec::new();
        for r in engine.execute(sql).unwrap() {
            if let QueryResult::Rows { rows, .. } = r {
                out = rows;
            }
        }
        out
    }

    /// Forgets the row-side keys, leaving the files under `data/` as the only
    /// copy of the table - what a directory holds once the block layout is the
    /// storage of record. `DELETE` is not the same thing: it marks the block's
    /// `version`, so those rows are (correctly) invisible to every reader.
    fn forget_journal(engine: &Engine) {
        for prefix in [
            "row:", "del:", "appends:", "append_ts:", "append_copy:", "deletes:",
            "delete_ts:", "import_rows:", "import_ts:", "dump_ts:",
        ] {
            for (key, _) in engine.storage.store.scan_prefix(prefix.as_bytes()).unwrap() {
                engine.storage.store.delete(&key).unwrap();
            }
        }
    }

    /// Re-encoding a table from the rows a **fresh process** reads back must
    /// produce the same bytes the files already hold.
    ///
    /// Plan C makes the block files the storage of record, so a restart has to
    /// rebuild the engine's rows from them (`recover_cpp_blocks`). That rebuild
    /// goes through the block reader's text form, and a rewrite then goes back
    /// through `encode_row`; any type whose text form is not an exact
    /// round-trip shows up here as a byte difference in its own `.col`.
    #[test]
    fn rehydration_reproduces_the_block_bytes() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute(
            "CREATE TABLE t (id INT, big BIGINT, f FLOAT, d DOUBLE, name VARCHAR, \
             j JSON, v EMBEDDING(FLOAT,4), sp SPARSE(FLOAT,30000));",
        )
        .unwrap();
        let rows: Vec<Vec<infinity_parser::Literal>> = (0..200)
            .map(|i| {
                vec![
                    infinity_parser::Literal::Int(i),
                    infinity_parser::Literal::Int(i as i64 * 1_000_003),
                    infinity_parser::Literal::Float(i as f64 / 7.0),
                    infinity_parser::Literal::Float((i as f64) * 0.1 + 1.0 / 3.0),
                    infinity_parser::Literal::Str(format!("doc-{i}")),
                    infinity_parser::Literal::Str(format!("{{\"k\":{i}}}")),
                    infinity_parser::Literal::Str(format!(
                        "[{},{},0.15,-2.5]",
                        i as f64 / 3.0,
                        i as f64 * 0.7
                    )),
                    infinity_parser::Literal::Str(format!("[0:0.5,{}:1.0]", i % 5 + 1)),
                ]
            })
            .collect();
        e.import_literals("default_db.t", None, rows).unwrap();
        e.flush().unwrap();

        assert_round_trip(e, dir.path(), "t");
    }

    /// A delete marks a block slot in `version` and keeps the row's cells; both
    /// have to come back from the file or a rewrite drops the tombstone.
    #[test]
    fn rehydration_keeps_the_tombstones() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        e.import_literals(
            "default_db.t",
            None,
            (0..100)
                .map(|i| {
                    vec![
                        infinity_parser::Literal::Int(i),
                        infinity_parser::Literal::Str(format!("doc-{i}")),
                    ]
                })
                .collect(),
        )
        .unwrap();
        e.execute("DELETE FROM t WHERE id % 3 = 0;").unwrap();
        e.flush().unwrap();
        assert_round_trip(e, dir.path(), "t");
    }

    /// A compressed segment is renumbered (`seg_4`, rows `0..230`), so its block
    /// ids no longer line up with the source segments - exactly the case that
    /// made a rewrite disagree with the files.
    #[test]
    fn rehydration_reproduces_a_compacted_segment() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR, score FLOAT);").unwrap();
        let mut base = 0i64;
        for rows in [100i64, 60, 40, 30] {
            e.import_literals(
                "default_db.t",
                None,
                (0..rows)
                    .map(|i| {
                        let k = base + i;
                        vec![
                            infinity_parser::Literal::Int(k),
                            infinity_parser::Literal::Str(format!("doc-{k}")),
                            infinity_parser::Literal::Float(k as f64 / 3.0),
                        ]
                    })
                    .collect(),
            )
            .unwrap();
            base += rows;
            e.flush().unwrap();
        }
        e.execute("COMPACT TABLE t;").unwrap();
        e.flush().unwrap();
        assert_round_trip(e, dir.path(), "t");
    }

    /// A compaction keeps the source segments' files and hides them with
    /// `drop|seg|…` alone, so every block reader has to consult one visible
    /// set. Two independent ones used to disagree here: the scan skipped the
    /// deprecated segments while the row count walked the directories, and a
    /// 32,768-row table reported 40,960 (the merged segment plus one source).
    #[test]
    fn a_compacted_source_segment_is_invisible_to_every_block_reader() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        let mut base = 0i64;
        for _ in 0..4 {
            e.import_literals(
                "default_db.t",
                None,
                (0..8192)
                    .map(|i| {
                        let k = base + i;
                        vec![
                            infinity_parser::Literal::Int(k),
                            infinity_parser::Literal::Str(format!("doc-{k}")),
                        ]
                    })
                    .collect(),
            )
            .unwrap();
            base += 8192;
        }
        e.flush().unwrap();
        let sources = e.visible_block_ids("t");
        assert_eq!(sources.len(), 4, "one block per source segment");
        assert_eq!(e.cpp_visible_row_count("t").unwrap(), 32_768);

        e.compact_all_segments("default_db.t").unwrap();
        // The merged segment is only in the append buffer until the checkpoint,
        // and the source segments are already hidden by `drop|seg|…` - so the
        // readers answer from the buffer and must not fall back to the stale
        // files (which would return the four source segments' rows again).
        assert_eq!(e.try_cpp_rows("t").unwrap().map(|r| r.len()), Some(32_768));
        assert_eq!(e.cpp_visible_row_count("t").unwrap(), 32_768);
        assert_eq!(e.fetch_rows_with_ids("default_db.t").unwrap().len(), 32_768);
        assert_eq!(e.row_count("t").unwrap(), 32_768);
        assert_eq!(row_strings(&e, "SELECT count(*) FROM t;"), vec![vec!["32768"]]);

        e.flush().unwrap();
        let visible = e.visible_block_ids("t");
        assert!(
            visible.iter().all(|id| id.segment_id == 4),
            "only the merged segment is visible: {visible:?}"
        );
        assert_eq!(visible.len(), 4, "the merged segment's four blocks");
        // The source segments keep every file - the invisibility comes from
        // `drop|seg|…`, not from a deletion.
        for id in &sources {
            let version = e
                .blocks_root()
                .join(cpp_format::block_dir(
                    id.db_id,
                    id.table_id,
                    id.segment_id,
                    id.block_id,
                ))
                .join(cpp_format::VERSION_FILE);
            assert!(version.exists(), "the deprecated block was removed: {version:?}");
        }

        let (_, rows) = e.read_cpp_blocks("t").unwrap();
        assert_eq!(rows.len(), 32_768);
        assert_eq!(e.cpp_visible_row_count("t").unwrap(), rows.len());
        assert_eq!(e.cpp_block_row_count("t").unwrap(), rows.len());
        assert_eq!(e.row_count("t").unwrap(), rows.len());
        assert_eq!(row_strings(&e, "SELECT count(*) FROM t;"), vec![vec!["32768"]]);
        // The per-block API follows the same set: a deprecated segment has no
        // visible block, the merged one has the whole table.
        assert!(e.block_ids("t", 0).is_empty());
        assert_eq!(e.block_row_count("t", 0, 0), None);
        assert_eq!(e.block_visible_range("t", 0, 0), None);
        assert_eq!(e.block_visible_range("t", 4, 0), Some((0, 8_192)));
        assert_eq!(e.cpp_block_visible_range("t", 4, 3), Some((0, 8_192)));
    }

    /// A crash between two checkpoints leaves the mirror behind. The replay
    /// puts the rows back into the journal and the **persisted** stale mark
    /// tells the reader to merge the two, so a row appended after the
    /// checkpoint is not lost when the blocks answer the next scan.
    #[test]
    fn a_crash_after_an_append_keeps_the_row() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT);").unwrap();
        e.execute("INSERT INTO t VALUES (1);").unwrap();
        e.flush().unwrap();
        e.execute("INSERT INTO t VALUES (2);").unwrap();
        drop(e); // no checkpoint: the row is only in the WAL

        let e = Engine::open(dir.path()).unwrap();
        assert_eq!(
            row_strings(&e, "SELECT id FROM t ORDER BY id;"),
            vec![vec!["1".to_string()], vec!["2".to_string()]]
        );
        assert_eq!(e.row_count("default_db.t").unwrap(), 2);
    }

    /// The mirror image of the test above: a delete that never reached a
    /// checkpoint stays visible as a tombstone after the restart.
    #[test]
    fn a_crash_after_a_delete_keeps_the_row_hidden() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT);").unwrap();
        e.execute("INSERT INTO t VALUES (1), (2);").unwrap();
        e.flush().unwrap();
        e.execute("DELETE FROM t WHERE id = 2;").unwrap();
        drop(e);

        let e = Engine::open(dir.path()).unwrap();
        assert_eq!(
            row_strings(&e, "SELECT id FROM t ORDER BY id;"),
            vec![vec!["1".to_string()]]
        );
        assert_eq!(e.row_count("default_db.t").unwrap(), 1);
    }

    /// A compaction reads its rows **out of the blocks**: with the journal gone
    /// (plan C) the source segments are walked one block at a time and the
    /// merged segment is written from what the files hold.
    ///
    /// The walk uses the row id to address one block (`segment << 32 | block *
    /// capacity + slot`), so a table of any size costs one decode per block and
    /// O(1) memory - materialising the table once per moved row is what turned
    /// a 32,768-row compaction into a quadratic job.
    #[test]
    fn a_compaction_reads_the_rows_out_of_the_blocks() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        for base in [0i64, 100, 200, 300] {
            e.import_literals(
                "default_db.t",
                None,
                (0..100)
                    .map(|i| {
                        let k = base + i;
                        vec![
                            infinity_parser::Literal::Int(k),
                            infinity_parser::Literal::Str(format!("doc-{k}")),
                        ]
                    })
                    .collect(),
            )
            .unwrap();
        }
        e.flush().unwrap();
        let sources = e.visible_block_ids("t");
        assert_eq!(sources.len(), 4, "one block per imported segment");

        forget_journal(&e);
        e.compact_all_segments("default_db.t").unwrap();
        e.flush().unwrap();

        let visible = e.visible_block_ids("t");
        assert!(
            visible.iter().all(|id| id.segment_id == 4),
            "only the merged segment is visible: {visible:?}"
        );
        assert_eq!(e.row_count("default_db.t").unwrap(), 400);
        assert_eq!(e.read_cpp_blocks("t").unwrap().1.len(), 400);
        assert_eq!(
            row_strings(&e, "SELECT count(*) FROM t WHERE id >= 300;"),
            vec![vec!["100".to_string()]]
        );
        assert_eq!(
            row_strings(&e, "SELECT name FROM t WHERE id = 0;"),
            vec![vec!["doc-0".to_string()]]
        );
    }

    /// A DELETE of 100 rows out of 8192 must leave the block's `version` with
    /// 100 tombstoned slots, so the scan, the block count and the table count
    /// all say 8092.
    ///
    /// Ignored: after the DELETE the row allocator reports the table in
    /// **segment 1** with `copy = true` (a second import-sized append of 8192
    /// rows), so the mirror writes a fresh segment without the tombstones and
    /// the block still shows 8192. Observed with `segment_row_counts` right
    /// before the write (`[(0, 8192)]` then `[(1, 8192)]`) and with the chunk's
    /// ids (`1 << 32 ..`). Needs a pass over the DELETE path; the mirror code
    /// itself marks the slots correctly when the layout stays in `seg_0` (see
    /// `delete_then_compact`, `rehydration_keeps_the_tombstones`).
    #[test]
    fn the_row_counts_and_the_scan_agree_after_deletes() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        e.import_literals(
            "default_db.t",
            None,
            (0..8192)
                .map(|i| {
                    vec![
                        infinity_parser::Literal::Int(i),
                        infinity_parser::Literal::Str(format!("doc-{i}")),
                    ]
                })
                .collect(),
        )
        .unwrap();
        e.execute("DELETE FROM t WHERE id < 100;").unwrap();
        e.flush().unwrap();

        assert_eq!(e.block_visible_range("t", 0, 0), Some((0, 8092)));
        assert_eq!(e.cpp_visible_row_count("t").unwrap(), 8092);
        assert_eq!(e.read_cpp_blocks("t").unwrap().1.len(), 8092);
        assert_eq!(e.row_count("t").unwrap(), 8092);
        assert_eq!(row_strings(&e, "SELECT count(*) FROM t;"), vec![vec!["8092"]]);
    }

    /// A commit takes **no** file I/O: `AppendInBlock` appends to the block's
    /// `BufferObject` in the buffer manager (`new_txn_data_impl.cpp:923`), and
    /// the block file is written by the checkpoint (`BufferManager::Flush`).
    /// The checkpoint then leaves a block nothing touched byte for byte alone.
    #[test]
    fn a_commit_writes_only_the_block_it_appends_to() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        let rows = |from: i64, n: i64| -> Vec<Vec<infinity_parser::Literal>> {
            (from..from + n)
                .map(|i| {
                    vec![
                        infinity_parser::Literal::Int(i),
                        infinity_parser::Literal::Str(format!("doc-{i}")),
                    ]
                })
                .collect()
        };
        // 8193 rows in one `INSERT`: block 0 is full, block 1 holds one row.
        e.insert_literals_inner(
            "default_db.t",
            None,
            rows(0, DEFAULT_BLOCK_CAPACITY as i64 + 1),
            false,
        )
        .unwrap();
        e.write_cpp_blocks("t").unwrap();

        let db_id = e.database_id_of("default_db.t");
        let table_id = e
            .storage
            .catalog
            .get_table_by_name("default_db.t")
            .unwrap()
            .unwrap()
            .id;
        let block = |block_id: u16| cpp_format::BlockId {
            db_id,
            table_id,
            segment_id: 0,
            block_id,
        };
        let first_before = cpp_format::read_block(&e.blocks_root(), block(0))
            .unwrap()
            .0
            .encode();
        let second_before = cpp_format::read_block(&e.blocks_root(), block(1))
            .unwrap()
            .0;

        // One more row lands in block 1. The commit itself writes no file at
        // all: both blocks are still the bytes the checkpoint left.
        e.insert_literals_inner("default_db.t", None, rows(9000, 1), false)
            .unwrap();
        let first = cpp_format::read_block(&e.blocks_root(), block(0)).unwrap();
        assert_eq!(first.0.encode(), first_before, "a commit wrote a file");
        let second = cpp_format::read_block(&e.blocks_root(), block(1)).unwrap();
        assert_eq!(
            second.0.encode(),
            second_before.encode(),
            "a commit wrote a file"
        );
        // The row is readable anyway: it is in the table's append buffer.
        assert_eq!(
            row_strings(&e, "SELECT count(*) FROM t;"),
            vec![vec!["8194"]]
        );

        // The checkpoint writes it into block 1, which now has two `create`s;
        // block 0 is untouched in every file, `version` included.
        e.write_cpp_blocks("t").unwrap();
        let first = cpp_format::read_block(&e.blocks_root(), block(0)).unwrap();
        assert_eq!(first.0.encode(), first_before);
        let second = cpp_format::read_block(&e.blocks_root(), block(1)).unwrap();
        assert_eq!(second.0.created.len(), second_before.created.len() + 1);

        // A delete is the same: no file until the checkpoint, then only the
        // block that holds the slot carries the tombstone.
        e.execute("DELETE FROM t WHERE id = 9000;").unwrap();
        let second = cpp_format::read_block(&e.blocks_root(), block(1)).unwrap();
        assert_eq!(second.0.deleted.iter().filter(|ts| **ts != 0).count(), 0);
        assert_eq!(
            row_strings(&e, "SELECT count(*) FROM t;"),
            vec![vec!["8193"]]
        );
        e.write_cpp_blocks("t").unwrap();
        let first = cpp_format::read_block(&e.blocks_root(), block(0)).unwrap();
        assert_eq!(first.0.encode(), first_before);
        let second = cpp_format::read_block(&e.blocks_root(), block(1)).unwrap();
        assert_eq!(second.0.deleted.iter().filter(|ts| **ts != 0).count(), 1);
    }

    /// The block layout is the **row set**, not just the values: with the
    /// journal gone the scan and the row count still answer in full. This is
    /// plan C's read path - a `row:` key is never consulted once the mirror is
    /// written and nothing has touched the table since.
    /// A checkpoint writes the rows of the transactions that had finished, at
    /// the bound `NewTxn::Checkpoint` uses (`current_ts`).
    ///
    /// An id handed out to an append whose commit has not landed is not part of
    /// any block yet, so the files stay a dense prefix and the next checkpoint
    /// picks the row up - instead of the old "skip a block with a hole", which
    /// dropped the rows after the hole on the floor.
    #[test]
    fn a_checkpoint_writes_only_the_committed_prefix() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE cp (id INT);").unwrap();
        e.execute("INSERT INTO cp VALUES (1), (2);").unwrap();
        e.flush().unwrap();
        assert_eq!(e.block_row_count("default_db.cp", 0, 0), Some(2));

        // A row range reserved without a commit ts: what a transaction that is
        // still committing looks like from the checkpoint's side.
        let range = e.reserve_rows("default_db.cp", 2, false).unwrap();
        assert_eq!(range.base, 2);
        e.flush().unwrap();
        assert_eq!(
            e.block_row_count("default_db.cp", 0, 0),
            Some(2),
            "the uncommitted ids must not be counted as rows"
        );
        assert_eq!(e.row_count("default_db.cp").unwrap(), 2);

        drop(e);
        let e = Engine::open(dir.path()).unwrap();
        assert_eq!(e.row_count("default_db.cp").unwrap(), 2);
        assert_eq!(
            row_strings(&e, "SELECT id FROM cp ORDER BY id;"),
            vec![vec!["1".to_string()], vec!["2".to_string()]]
        );
    }

    #[test]
    fn the_blocks_are_the_row_set_not_just_the_values() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        e.execute("INSERT INTO t VALUES (1,'a'),(2,'b'),(3,'c');")
            .unwrap();
        e.execute("DELETE FROM t WHERE id = 2;").unwrap();
        e.flush().unwrap();

        forget_journal(&e);
        assert!(
            e.storage
                .store
                .scan_prefix(b"row:")
                .unwrap()
                .is_empty(),
            "the journal is gone, so the rows can only come from data/"
        );
        assert!(
            e.storage
                .store
                .scan_prefix(b"del:")
                .unwrap()
                .is_empty(),
            "a delete is a `version` mark, not a key - the reference has no `del:` key space"
        );
        assert_eq!(
            row_strings(&e, "SELECT id FROM t ORDER BY id;"),
            vec![vec!["1".to_string()], vec!["3".to_string()]]
        );
        assert_eq!(e.fetch_rows_with_ids("default_db.t").unwrap().len(), 2);
        assert_eq!(e.row_count("default_db.t").unwrap(), 2);
        assert_eq!(e.read_cpp_blocks("t").unwrap().1.len(), 2);
        assert_eq!(
            row_strings(&e, "SELECT count(*) FROM t;"),
            vec![vec!["2".to_string()]]
        );

        // A reopened directory has no snapshot of the mirror, and the journal
        // it just rebuilt is removed again: the scan still reads the blocks.
        drop(e);
        let e = Engine::open(dir.path()).unwrap();
        forget_journal(&e);
        assert_eq!(
            row_strings(&e, "SELECT id FROM t ORDER BY id;"),
            vec![vec!["1".to_string()], vec!["3".to_string()]]
        );
        assert_eq!(e.row_count("default_db.t").unwrap(), 2);
        assert_eq!(e.fetch_rows_with_ids("default_db.t").unwrap().len(), 2);
    }

    /// Drops the row-side keys (what a directory has after plan C's switch),
    /// reopens, rewrites the mirror and compares every file under `data/`.
    fn assert_round_trip(e: Engine, dir: &Path, table: &str) {
        let before = read_tree(&dir.join("data"));
        assert!(!before.is_empty());
        // Idempotence without a restart: a rewrite of unchanged rows is a no-op.
        {
            let again_before = read_tree(&dir.join("data"));
            e.write_cpp_blocks(table).unwrap();
            let again_after = read_tree(&dir.join("data"));
            assert_eq!(again_before, again_after, "a plain rewrite changed the files");
        }
        for prefix in [
            "row:", "del:", "appends:", "append_ts:", "append_copy:", "deletes:",
            "delete_ts:", "import_rows:", "import_ts:", "dump_ts:",
        ] {
            for (key, _) in e.storage.store.scan_prefix(prefix.as_bytes()).unwrap() {
                e.storage.store.delete(&key).unwrap();
            }
        }
        drop(e);

        let e = Engine::open(dir).unwrap();
        e.write_cpp_blocks(table).unwrap();
        e.flush().unwrap();
        let after = read_tree(&dir.join("data"));

        let mut differing: Vec<String> = Vec::new();
        for (path, bytes) in &before {
            match after.get(path) {
                Some(other) if other == bytes => {}
                Some(other) => {
                    let at = bytes
                        .iter()
                        .zip(other.iter())
                        .position(|(a, b)| a != b)
                        .unwrap_or(0);
                    let head = |b: &[u8]| {
                        let lo = at.saturating_sub(8);
                        let hi = (at + 8).min(b.len());
                        b[lo..hi].iter().map(|x| format!("{x:02x}")).collect::<String>()
                    };
                    differing.push(format!(
                        "{path}: {at} differs\n    before {}\n    after  {}",
                        head(bytes),
                        head(other)
                    ));
                }
                None => differing.push(format!("{path}: missing after the reopen")),
            }
        }
        assert!(
            differing.is_empty(),
            "rehydration changed {} file(s):\n  {}",
            differing.len(),
            differing.join("\n  ")
        );
    }

    fn read_tree(root: &Path) -> std::collections::BTreeMap<String, Vec<u8>> {
        fn walk(root: &Path, at: &Path, out: &mut std::collections::BTreeMap<String, Vec<u8>>) {
            let Ok(entries) = std::fs::read_dir(at) else {
                return;
            };
            for entry in entries.flatten() {
                let path = entry.path();
                if path.is_dir() {
                    walk(root, &path, out);
                } else {
                    let name = path
                        .strip_prefix(root)
                        .unwrap()
                        .to_string_lossy()
                        .to_string();
                    out.insert(name, std::fs::read(&path).unwrap_or_default());
                }
            }
        }
        let mut out = Default::default();
        walk(root, root, &mut out);
        out
    }

    /// The mirror writes the reference's prefilter records for every **sealed**
    /// segment and block it mirrors - here an imported one; an `INSERT` that
    /// has not reached the segment capacity leaves no filter.
    #[test]
    fn the_mirror_writes_the_fast_rough_filter_records() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        e.import_literals(
            "default_db.t",
            None,
            (0..10)
                .map(|i| {
                    vec![
                        infinity_parser::Literal::Int(i),
                        infinity_parser::Literal::Str(format!("doc-{i}")),
                    ]
                })
                .collect(),
        )
        .unwrap();
        e.flush().unwrap();
        let store = e.catalog().store();
        let prefix = b"seg|1|0|0|fast_rough_filter";
        let record = store.get(prefix).unwrap().expect("segment prefilter");
        // [u32 total][u64 build_time] then the empty probabilistic half and the
        // min/max half over (id, name).
        assert_eq!(
            u32::from_le_bytes(record[0..4].try_into().unwrap()) as usize,
            record.len()
        );
        // `[u32 total][u32 column_count]` opens each half; the count is the
        // second word of the probabilistic half.
        assert_eq!(record[16], 2, "two columns");
        let block = store
            .get(b"blk|1|0|0|0|fast_rough_filter")
            .unwrap()
            .expect("block prefilter");
        assert_eq!(block, record);

        // An `INSERT`-filled segment that is not full has no filter at all.
        e.execute("CREATE TABLE u (id INT);").unwrap();
        e.insert_literals(
            "default_db.u",
            None,
            vec![vec![infinity_parser::Literal::Int(1)]],
        )
        .unwrap();
        e.flush().unwrap();
        assert!(store.get(b"seg|1|1|0|fast_rough_filter").unwrap().is_none());
    }

    #[test]
    fn type_text_mapping() {
        assert_eq!(column_type_of("INT"), Some(3));
        assert_eq!(column_type_of("integer"), Some(3));
        assert_eq!(column_type_of("VARCHAR"), Some(9));
        assert_eq!(column_type_of("DOUBLE"), Some(6));
        assert_eq!(column_type_of("JSON"), Some(22));
        assert_eq!(column_type_of("EMBEDDING(FLOAT,4)"), Some(10));
        assert_eq!(column_type_of("VECTOR(FLOAT,4)"), Some(10));
        assert_eq!(column_type_of("TENSOR(FLOAT,4)"), Some(11));
        assert_eq!(column_type_of("MULTIVECTOR(FLOAT,4)"), Some(14));
        assert_eq!(column_type_of("TENSORARRAY(FLOAT,4)"), Some(12));
        // Bit embeddings pack to `ceil(dim/8)` bytes, and a bit tensor row packs
        // `dim` bits the same way (`tensor_row_width`).
        assert_eq!(column_type_of("EMBEDDING(BIT,8)"), Some(10));
        assert_eq!(column_type_of("TENSOR(BIT,8)"), Some(11));
        assert_eq!(column_type_of("TENSORARRAY(BIT,8)"), Some(12));
    }

    #[test]
    fn scalar_values_encode() {
        assert_eq!(
            encode_value("INT", 3, "42").unwrap().unwrap(),
            42i32.to_le_bytes()
        );
        assert_eq!(
            encode_value("BIGINT", 4, "42").unwrap().unwrap(),
            42i64.to_le_bytes()
        );
        assert_eq!(
            encode_value("DOUBLE", 6, "1.5").unwrap().unwrap(),
            1.5f64.to_le_bytes()
        );
        // Booleans go through `pack_column`'s compact-bit path, which keys off
        // the textual forms `1`/`0`.
        assert_eq!(
            encode_value("BOOL", 0, "true").unwrap().unwrap(),
            b"1".to_vec()
        );
        assert_eq!(
            encode_value("BOOL", 0, "false").unwrap().unwrap(),
            b"0".to_vec()
        );
        assert_eq!(
            encode_value("VARCHAR", 9, "hi").unwrap().unwrap(),
            b"hi".to_vec()
        );
        assert!(encode_value("INT", 3, "null").unwrap().is_none());
        assert!(encode_value("INT", 3, "abc").is_err());
        // 1970-01-02 is one day after the epoch.
        assert_eq!(
            encode_value("DATE", 16, "1970-01-02").unwrap().unwrap(),
            1i32.to_le_bytes()
        );
    }

    /// The full-text chunk's term meta offsets must address the `.pos` file.
    ///
    /// The reference slices the file by them
    /// (`disk_segment_reader_impl.cpp:126`: the doc list is
    /// `doc_start .. pos_start`, the position list `pos_start .. pos_end`), so
    /// an offset taken before the preceding part was appended points at the
    /// wrong bytes and the reference decodes a posting list of the wrong
    /// length - it then reads past the end of the slice.
    #[test]
    fn fulltext_chunk_offsets_address_the_pos_file() {
        let mut postings: std::collections::BTreeMap<String, Vec<(u32, u32)>> =
            std::collections::BTreeMap::new();
        // Two terms, so the second one's offsets are past the first one's.
        postings.insert("alpha".to_string(), vec![(0, 0), (1, 6), (2, 12)]);
        postings.insert("beta".to_string(), vec![(0, 6), (1, 12)]);
        let lengths = vec![3u32, 2, 2];
        let (dic, pos, _len) = encode_fulltext_chunk(&postings, &lengths);

        // The term meta region opens the `.dic`; the first term's meta is at
        // its start.
        let mut at = 0usize;
        let read_vint = |buf: &[u8], at: &mut usize| {
            let mut value = 0u64;
            let mut shift = 0u32;
            loop {
                let byte = buf[*at];
                *at += 1;
                value |= u64::from(byte & 0x7f) << shift;
                if byte & 0x80 == 0 {
                    break;
                }
                shift += 7;
            }
            value
        };
        let df = read_vint(&dic, &mut at) as u32;
        let tf = read_vint(&dic, &mut at) as u32;
        let _payload = u32::from_le_bytes(dic[at..at + 4].try_into().unwrap());
        at += 4;
        let doc_start = read_vint(&dic, &mut at);
        let pos_start = read_vint(&dic, &mut at);
        let pos_end = read_vint(&dic, &mut at);

        assert_eq!(df, 3);
        assert_eq!(tf, 3);
        // The doc list starts at the term's first byte, and the position list
        // starts where the doc list ends.
        assert_eq!(doc_start, 0);
        assert!(pos_start > doc_start);
        assert!(pos_end > pos_start);
        // Each offset addresses a real region of the `.pos` file: the doc list
        // is framed by its two lengths, then the position list by its own.
        let mut p = doc_start as usize;
        let skiplist_len = read_vint(&pos, &mut p) as usize;
        let doc_list_len = read_vint(&pos, &mut p) as usize;
        assert_eq!(p + skiplist_len + doc_list_len, pos_start as usize);
        let mut q = pos_start as usize;
        let pos_skiplist_len = read_vint(&pos, &mut q) as usize;
        let pos_list_len = read_vint(&pos, &mut q) as usize;
        assert_eq!(q + pos_skiplist_len + pos_list_len, pos_end as usize);
        // The first term's postings end where the second one's begin, so the
        // file holds one slice per term.
        assert!(pos_end as usize <= pos.len());
    }

    #[test]
    fn blocks_roundtrip_a_real_table() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute("CREATE TABLE t (id INT, name VARCHAR, ok BOOL);")
            .unwrap();
        engine
            .execute("INSERT INTO t VALUES (1, 'a', true);")
            .unwrap();
        engine
            .execute("INSERT INTO t VALUES (2, 'bb', false);")
            .unwrap();

            assert_eq!(engine.write_cpp_blocks("t").unwrap(), 2);
        // The C++ file names exist under the mirror root (`tbl_{id}` uses the
        // catalog's table id, so discover it rather than hardcoding).
        let db_dir = dir.path().join("data").join("db_1");
        let tbl_dir = std::fs::read_dir(&db_dir)
            .unwrap()
            .filter_map(|e| e.ok())
            .map(|e| e.path())
            .find(|p| {
                p.file_name()
                    .is_some_and(|n| n.to_string_lossy().starts_with("tbl_"))
            })
            .expect("table directory");
        let block = tbl_dir.join("seg_0").join("blk_0");
        assert!(block.join("0.col").exists());
        assert!(block.join("1.col").exists());
        assert!(block.join("2.col").exists());
        assert!(block.join(cpp_format::VERSION_FILE).exists());

        // Read back purely from the block files.
        let (columns, rows) = engine.read_cpp_blocks("t").unwrap();
        assert_eq!(columns, vec!["id", "name", "ok"]);
        assert_eq!(
            rows,
            vec![
                vec!["1".to_string(), "a".to_string(), "true".to_string()],
                vec!["2".to_string(), "bb".to_string(), "false".to_string()],
            ]
        );

        // ... and the SQL view is unchanged by mirroring.
        assert_eq!(
            row_strings(&engine, "SELECT * FROM t;"),
            vec![
                vec!["1".to_string(), "a".to_string(), "true".to_string()],
                vec!["2".to_string(), "bb".to_string(), "false".to_string()],
            ]
        );
    }

    #[test]
    fn a_dropped_column_leaves_its_id_unused_in_the_mirror() {
        // C++ never reuses a column id: after dropping the middle column the
        // files are `0.col` and `2.col` (there is no `1.col`).
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute("CREATE TABLE t (a INT, b INT, c INT);")
            .unwrap();
        engine.execute("INSERT INTO t VALUES (1, 2, 3);").unwrap();
        engine.execute("ALTER TABLE t DROP COLUMN b;").unwrap();
        assert_eq!(engine.write_cpp_blocks("t").unwrap(), 1);

        let db_dir = dir.path().join("data").join("db_1");
        let tbl_dir = std::fs::read_dir(&db_dir)
            .unwrap()
            .filter_map(|e| e.ok())
            .map(|e| e.path())
            .find(|p| {
                p.file_name()
                    .is_some_and(|n| n.to_string_lossy().starts_with("tbl_"))
            })
            .expect("table directory");
        let block = tbl_dir.join("seg_0").join("blk_0");
        assert!(block.join("0.col").exists());
        assert!(!block.join("1.col").exists());
        assert!(block.join("2.col").exists());

        let (columns, rows) = engine.read_cpp_blocks("t").unwrap();
        assert_eq!(columns, vec!["a", "c"]);
        assert_eq!(rows, vec![vec!["1".to_string(), "3".to_string()]]);
    }

    #[test]
    fn sparse_shape_parsing() {
        // Index width follows the bound, as the engine's describe_type does.
        assert_eq!(sparse_shape("SPARSE(FLOAT,100)"), Some((6, 2, 1)));
        assert_eq!(sparse_shape("SPARSE(FLOAT,30000)"), Some((6, 3, 2)));
        assert_eq!(sparse_shape("SPARSE(DOUBLE,30000)"), Some((7, 3, 2)));
        assert_eq!(sparse_shape("INT"), None);
    }

    #[test]
    fn sparse_literal_parsing() {
        assert_eq!(
            parse_sparse_literal("[0:1.5,3:2.5]"),
            (vec![0, 3], vec![1.5, 2.5])
        );
        assert_eq!(parse_sparse_literal("(empty)"), (Vec::new(), Vec::new()));
        assert_eq!(parse_sparse_literal("[]"), (Vec::new(), Vec::new()));
    }

    #[test]
    fn sparse_blocks_roundtrip() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute("CREATE TABLE sp (id INT, v SPARSE(FLOAT,100));")
            .unwrap();
        engine
            .execute("INSERT INTO sp VALUES (1, [0:1.5,3:2.5]);")
            .unwrap();
        engine
            .execute("INSERT INTO sp VALUES (2, [7:3.5]);")
            .unwrap();

        assert_eq!(engine.write_cpp_blocks("sp").unwrap(), 2);
        let (_, rows) = engine.read_cpp_blocks("sp").unwrap();
        // The cell text is the reference's rendering (`SparseT::ToString`
        // prints floats with six decimals), which is also what the engine
        // stores, so a rewrite of this block reproduces the same bytes.
        assert_eq!(
            rows,
            vec![
                vec!["1".to_string(), "[0:1.500000,3:2.500000]".to_string()],
                vec!["2".to_string(), "[7:3.500000]".to_string()],
            ]
        );
    }

    #[test]
    fn sparse_bit_blocks_roundtrip() {
        // `SPARSE(BIT, d)` stores no value bytes (`SparseInfo::DataSize` returns
        // 0 for `kElemBit`): the column is a bitmap of the present indices and
        // the value is implicitly 1.
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute("CREATE TABLE spb (id INT, v SPARSE(BIT,100));")
            .unwrap();
        engine
            .execute("INSERT INTO spb VALUES (1, [0:1,20:1,80:1]);")
            .unwrap();
        engine
            .execute("INSERT INTO spb VALUES (2, [7:1]);")
            .unwrap();

        assert_eq!(engine.write_cpp_blocks("spb").unwrap(), 2);
        let (_, rows) = engine.read_cpp_blocks("spb").unwrap();
        // A bit sparse renders as its present positions only.
        assert_eq!(
            rows,
            vec![
                vec!["1".to_string(), "[0,20,80]".to_string()],
                vec!["2".to_string(), "[7]".to_string()],
            ]
        );
    }

    #[test]
    fn array_shape_parsing() {
        assert_eq!(array_shape("ARRAY(INT)"), Some(ArrayElem::Fixed(3)));
        assert_eq!(array_shape("ARRAY(DOUBLE)"), Some(ArrayElem::Fixed(6)));
        // One nesting level of fixed-width leaves; a second level or a
        // variable-length element is out of scope.
        assert_eq!(array_shape("ARRAY(ARRAY(INT))"), Some(ArrayElem::Nested(3)));
        assert_eq!(array_shape("ARRAY(ARRAY(ARRAY(INT)))"), None);
        assert_eq!(array_shape("ARRAY(VARCHAR)"), Some(ArrayElem::VarLen(9)));
        // A tensor/sparse element is not an array element.
        assert_eq!(array_shape("ARRAY(TENSOR(FLOAT,3))"), None);
    }

    #[test]
    fn varchar_array_blocks_roundtrip() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute("CREATE TABLE va (id INT, a ARRAY(VARCHAR));")
            .unwrap();
        // A short (inlined) element next to a long (outlined) one, and an empty
        // array.
        let long = "this element is definitely longer than thirteen bytes";
        engine
            .execute(&format!("INSERT INTO va VALUES (1, {{ab,'{long}'}});"))
            .unwrap();
        engine.execute("INSERT INTO va VALUES (2, {});").unwrap();

        assert_eq!(engine.write_cpp_blocks("va").unwrap(), 2);
        let (_, rows) = engine.read_cpp_blocks("va").unwrap();
        assert_eq!(
            rows,
            vec![
                vec!["1".to_string(), format!("{{ab,{long}}}")],
                vec!["2".to_string(), "{}".to_string()],
            ]
        );
    }

    #[test]
    fn array_blocks_roundtrip() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute("CREATE TABLE ar (id INT, a ARRAY(INT));")
            .unwrap();
        engine
            .execute("INSERT INTO ar VALUES (1, {1,2,3});")
            .unwrap();
        engine.execute("INSERT INTO ar VALUES (2, {7});").unwrap();

        assert_eq!(engine.write_cpp_blocks("ar").unwrap(), 2);
        let (_, rows) = engine.read_cpp_blocks("ar").unwrap();
        assert_eq!(
            rows,
            vec![
                vec!["1".to_string(), "{1,2,3}".to_string()],
                vec!["2".to_string(), "{7}".to_string()],
            ]
        );
    }

    #[test]
    fn nested_array_blocks_roundtrip() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute("CREATE TABLE na (id INT, a ARRAY(ARRAY(INT)));")
            .unwrap();
        // An empty outer array, an outer array with an empty inner one, and
        // mixed inner lengths.
        engine
            .execute("INSERT INTO na VALUES (1, {{0,3},{1}});")
            .unwrap();
        engine
            .execute("INSERT INTO na VALUES (2, {{0,3},{}});")
            .unwrap();
        engine.execute("INSERT INTO na VALUES (3, {});").unwrap();

        assert_eq!(engine.write_cpp_blocks("na").unwrap(), 3);
        let (_, rows) = engine.read_cpp_blocks("na").unwrap();
        assert_eq!(
            rows,
            vec![
                vec!["1".to_string(), "{{0,3},{1}}".to_string()],
                vec!["2".to_string(), "{{0,3},{}}".to_string()],
                vec!["3".to_string(), "{}".to_string()],
            ]
        );
    }

    #[test]
    fn tensor_blocks_roundtrip() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute("CREATE TABLE ts (id INT, t TENSOR(FLOAT,3));")
            .unwrap();
        engine
            .execute("INSERT INTO ts VALUES (1, [[1,2,3],[4,5,6]]);")
            .unwrap();
        engine
            .execute("INSERT INTO ts VALUES (2, [[7,8,9]]);")
            .unwrap();

        assert_eq!(engine.write_cpp_blocks("ts").unwrap(), 2);
        // The outline holds the raw element bytes: 3 vectors * dim 3 * 4 bytes.
        let db_dir = dir.path().join("data").join("db_1");
        let tbl_dir = std::fs::read_dir(&db_dir)
            .unwrap()
            .filter_map(|e| e.ok())
            .map(|e| e.path())
            .find(|p| {
                p.file_name()
                    .is_some_and(|n| n.to_string_lossy().starts_with("tbl_"))
            })
            .expect("table directory");
        // The outline holds the raw element bytes: row 1 has 2 vectors, row 2
        // has 1, so 3 * dim 3 * 4 bytes.
        let outline = std::fs::read(tbl_dir.join("seg_0/blk_0/col_1_out")).unwrap();
        assert_eq!(outline.len(), 3 * 3 * 4);

        let (_, rows) = engine.read_cpp_blocks("ts").unwrap();
        assert_eq!(
            rows,
            vec![
                vec!["1".to_string(), "[[1,2,3],[4,5,6]]".to_string()],
                vec!["2".to_string(), "[[7,8,9]]".to_string()],
            ]
        );
    }

    #[test]
    fn tensor_shape_parsing() {
        assert_eq!(tensor_shape("TENSOR(FLOAT,3)"), Some((11, 3, 6)));
        assert_eq!(tensor_shape("MULTIVECTOR(FLOAT,3)"), Some((14, 3, 6)));
        assert_eq!(tensor_shape("TENSORARRAY(FLOAT,3)"), None);
        assert_eq!(tensor_shape("TENSOR(BIT,8)"), Some((11, 8, 0)));
    }

    #[test]
    fn tensorarray_shape_parsing() {
        assert_eq!(tensorarray_shape("TENSORARRAY(FLOAT,3)"), Some((12, 3, 6)));
        assert_eq!(tensorarray_shape("TENSORARRAY(DOUBLE,2)"), Some((12, 2, 7)));
        assert_eq!(tensorarray_shape("TENSOR(FLOAT,3)"), None);
        assert_eq!(tensorarray_shape("TENSORARRAY(BIT,8)"), Some((12, 8, 0)));
    }

    #[test]
    fn tensor_array_blocks_roundtrip() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute("CREATE TABLE ta (id INT, t TENSORARRAY(FLOAT,2));")
            .unwrap();
        // Two tensors (one with two rows, one with a single row) and a single
        // tensor, so both the tensor count and each row count vary.
        engine
            .execute("INSERT INTO ta VALUES (1, [[[1,2],[3,4]],[[5,6]]]);")
            .unwrap();
        engine
            .execute("INSERT INTO ta VALUES (2, [[[7,8]]]);")
            .unwrap();

        assert_eq!(engine.write_cpp_blocks("ta").unwrap(), 2);
        let (_, rows) = engine.read_cpp_blocks("ta").unwrap();
        assert_eq!(
            rows,
            vec![
                vec!["1".to_string(), "[[[1,2],[3,4]],[[5,6]]]".to_string()],
                vec!["2".to_string(), "[[[7,8]]]".to_string()],
            ]
        );
    }

    #[test]
    fn blocks_reconstruct_a_table_after_its_rows_are_deleted() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute("CREATE TABLE t (id INT, name VARCHAR, ok BOOL);")
            .unwrap();
        engine
            .execute("INSERT INTO t VALUES (1, 'a', true), (2, 'bb', false);")
            .unwrap();
        assert_eq!(engine.write_cpp_blocks("t").unwrap(), 2);

        engine.execute("DELETE FROM t WHERE id = 1;").unwrap();
        // The tombstone reaches the `version` file at a checkpoint, like every
        // other change to the mirror; without one a reopen would resurrect the
        // row from cells the file still holds.
        engine.flush().unwrap();
        forget_journal(&engine);

        // A deleted row keeps its cells in the block and is hidden by the
        // `version` file, so the mirror carries exactly the surviving row -
        // the other slot is still there, marked.
        let (_, rows) = engine.read_cpp_blocks("t").unwrap();
        assert_eq!(rows.len(), 1);

        // A fresh engine over the same directory rebuilds the rows from the
        // mirror: the tombstoned slot stays hidden, so the restore brings back
        // the one surviving row and not its deleted neighbour.
        drop(engine);
        let engine = Engine::open(dir.path()).unwrap();
        assert_eq!(
            row_strings(&engine, "SELECT * FROM t;"),
            vec![vec!["2".to_string(), "bb".to_string(), "false".to_string()]]
        );
        assert_eq!(engine.row_count("default_db.t").unwrap(), 1);
    }

    #[test]
    fn embedded_column_values_survive_a_block_round_trip_through_sql() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute("CREATE TABLE t (id INT, v EMBEDDING(FLOAT,3), d DATE, s VARCHAR);")
            .unwrap();
        engine
            .execute("INSERT INTO t VALUES (1, [1.5,2.5,3.5], DATE '2024-01-22', 'alpha');")
            .unwrap();
        assert_eq!(engine.write_cpp_blocks("t").unwrap(), 1);
        // The mirror is the table: with the journal gone the row still reads
        // back through SQL, so every column type survives the block form.
        forget_journal(&engine);
        let rows = row_strings(&engine, "SELECT * FROM t;");
        assert_eq!(rows.len(), 1);
        assert_eq!(rows[0][2], "2024-01-22");
        assert_eq!(rows[0][3], "alpha");
        assert!(rows[0][1].starts_with("[1.5,2.5,3.5]"), "{}", rows[0][1]);
    }

    #[test]
    fn uint8_embedding_roundtrip() {
        let dir = tempfile::tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE u8 (id INT, v EMBEDDING(UINT8,3));")
            .unwrap();
        e.execute("INSERT INTO u8 VALUES (1, [127,128,255]);")
            .unwrap();
        e.write_cpp_blocks("u8").unwrap();
        let (_, rows) = e.read_cpp_blocks("u8").unwrap();
        assert_eq!(rows[0][1], "[127,128,255]");
    }

    #[test]
    fn bit_embedding_packs_and_unpacks() {
        // `EMBEDDING(BIT,8)` is one byte per row, bit `j` at bit `j%8`
        // (`embedding_type.h:129-137,253-262`), not eight bytes.
        assert_eq!(embedding_size(0, 8), 1);
        assert_eq!(embedding_size(0, 16), 2);
        assert_eq!(embedding_size(6, 3), 12); // FLOAT32
        assert_eq!(
            layout_of("EMBEDDING(BIT,8)", 10),
            Some(ColumnLayout::Fixed(1))
        );
        let Some(bytes) = encode_value("EMBEDDING(BIT,8)", 10, "[10110001]").unwrap() else {
            panic!("expected bytes");
        };
        assert_eq!(bytes, vec![0b1000_1101], "bit 0,2,3,7");
        // The comma form parses to the same bytes.
        assert_eq!(
            encode_value("EMBEDDING(BIT,8)", 10, "[1,0,1,1,0,0,0,1]")
                .unwrap()
                .unwrap(),
            bytes
        );
        assert_eq!(
            decode_value("EMBEDDING(BIT,8)", 10, &bytes, 0, &[]).unwrap(),
            "[10110001]"
        );
    }

    #[test]
    fn bit_embedding_survives_a_block_round_trip() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute("CREATE TABLE b (id INT, v EMBEDDING(BIT,16));")
            .unwrap();
        engine
            .execute("INSERT INTO b VALUES (1, [1,0,1,1,0,0,0,1,1,1,0,0,0,0,1,1]);")
            .unwrap();
        assert_eq!(engine.write_cpp_blocks("b").unwrap(), 1);
        forget_journal(&engine);
        let rows = row_strings(&engine, "SELECT * FROM b;");
        assert_eq!(rows.len(), 1);
        // The engine displays a BIT embedding as a bit string.
        assert_eq!(rows[0][1], "[1011000111000011]");
    }

    #[test]
    fn a_bit_tensor_array_survives_a_block_round_trip() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        // A bit tensor row packs `dim` bits into `ceil(dim/8)` bytes; the block
        // reader renders it back as the bit string the reference prints.
        engine
            .execute("CREATE TABLE v (id INT, ta TENSORARRAY(BIT,8));")
            .unwrap();
        engine
            .execute("INSERT INTO v VALUES (1, '[[[1, 1, 1, 0, 1, 0, 1, 1]]]');")
            .unwrap();
        engine.write_cpp_blocks("v").unwrap();
        let rows = engine.fetch_rows("v").unwrap();
        assert_eq!(
            rows[0].get(1).map(|v| v.to_slt_string()),
            Some("[[[11101011]]]".to_string()),
            "{rows:?}"
        );
    }

    #[test]
    fn long_varchar_survives_a_block_round_trip() {
        // A payload longer than `VARCHAR_INLINE_LENGTH` (13) lives in the
        // outline; the reader must follow the slot's offset to get it back.
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute("CREATE TABLE lv (id INT, s VARCHAR);")
            .unwrap();
        let long = "this string is definitely longer than thirteen bytes";
        engine
            .execute(&format!("INSERT INTO lv VALUES (1, '{long}');"))
            .unwrap();
        engine
            .execute("INSERT INTO lv VALUES (2, 'short');")
            .unwrap();

        assert_eq!(engine.write_cpp_blocks("lv").unwrap(), 2);
        forget_journal(&engine);
        let rows = row_strings(&engine, "SELECT * FROM lv;");
        assert_eq!(rows.len(), 2);
        assert_eq!(rows[0][1], long);
        assert_eq!(rows[1][1], "short");
    }

    #[test]
    fn hnsw_index_mirror_roundtrips_the_vectors() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute("CREATE TABLE hv (id INT, v EMBEDDING(FLOAT,3));")
            .unwrap();
        engine
            .execute("INSERT INTO hv VALUES (1, [1,2,3]), (2, [4,5,6]), (3, [7,8,9]);")
            .unwrap();
        engine
            .execute("CREATE INDEX hv_idx ON hv (v) USING HNSW;")
            .unwrap();

        assert_eq!(engine.write_cpp_hnsw_index("hv", "hv_idx").unwrap(), 3);
        let back = engine.read_cpp_hnsw_index("hv", "hv_idx").unwrap();
        // Labels are the row sequence numbers, so the order is the insert order.
        assert_eq!(back.len(), 3);
        assert_eq!(back[0], (0, vec![1.0, 2.0, 3.0]));
        assert_eq!(back[1], (1, vec![4.0, 5.0, 6.0]));
        assert_eq!(back[2], (2, vec![7.0, 8.0, 9.0]));
    }

    #[test]
    fn a_persisted_hnsw_index_answers_vector_search() {
        let dir = tempfile::tempdir().unwrap();
        {
            let e = Engine::open(dir.path()).unwrap();
            e.execute("CREATE TABLE hv (id INT, v EMBEDDING(FLOAT,3));")
                .unwrap();
            e.execute("INSERT INTO hv VALUES (1,[1,2,3]), (2,[4,5,6]), (3,[7,8,9]);")
                .unwrap();
            e.execute("CREATE INDEX hv_idx ON hv (v) USING HNSW;")
                .unwrap();
            // A flush writes the rows and the HNSW chunk file.
            e.flush().unwrap();
        }
        // Reopening loads the persisted graph, so the search walks what is on
        // disk rather than scanning every row (plan #70).
        let e = Engine::open(dir.path()).unwrap();
        assert!(
            e.tables
                .lock()
                .unwrap()
                .get("default_db.hv")
                .is_some_and(|t| t.vector.contains_key("v")),
            "the HNSW graph was not loaded from the mirror"
        );
        let rows = row_strings(
            &e,
            "SELECT id FROM hv SEARCH MATCH VECTOR (v, [1,2,3], 'float', 'l2', 2);",
        );
        assert_eq!(rows, vec![vec!["1".to_string()], vec!["2".to_string()]]);
    }

    #[test]
    fn ivf_index_mirror_roundtrips_the_vectors() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute("CREATE TABLE iv (id INT, v EMBEDDING(FLOAT,2));")
            .unwrap();
        // Two well-separated clusters.
        engine
            .execute("INSERT INTO iv VALUES (1, [0,0]), (2, [0,1]), (3, [10,10]), (4, [10,11]);")
            .unwrap();
        engine
            .execute("CREATE INDEX iv_idx ON iv (v) USING IVF WITH (metric = l2);")
            .unwrap();

        assert_eq!(engine.write_cpp_ivf_index("iv", "iv_idx").unwrap(), 4);
        let (centroids, back) = engine.read_cpp_ivf_index("iv", "iv_idx").unwrap();
        // 4 rows: `min(sqrt(4) * 1.0, 4 / 32) = 0`, raised to 1 centroid.
        assert_eq!(centroids.len(), 2); // 1 centroid * dimension 2
                                        // Every vector comes back, under its row id.
        let mut got: Vec<(u32, Vec<f32>)> = back;
        got.sort_by_key(|(l, _)| *l);
        assert_eq!(
            got,
            vec![
                (0, vec![0.0, 0.0]),
                (1, vec![0.0, 1.0]),
                (2, vec![10.0, 10.0]),
                (3, vec![10.0, 11.0]),
            ]
        );
    }

    #[test]
    fn reading_blocks_without_a_mirror_reports_not_found() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine.execute("CREATE TABLE t (id INT);").unwrap();
        assert!(engine.read_cpp_blocks("t").is_err());
    }

    #[test]
    fn embedding_shape_parsing() {
        assert_eq!(embedding_shape("EMBEDDING(FLOAT,4)"), Some((4, 6)));
        assert_eq!(embedding_shape("EMBEDDING(FLOAT16,8)"), Some((8, 8)));
        assert_eq!(embedding_shape("EMBEDDING(DOUBLE,2)"), Some((2, 7)));
        assert_eq!(embedding_shape("EMBEDDING(BIT,8)"), Some((8, 0)));
        assert_eq!(embedding_shape("INT"), None);
    }

    #[test]
    fn embedding_values_encode() {
        let bytes = encode_value("EMBEDDING(FLOAT,3)", 10, "[1,2,3]")
            .unwrap()
            .unwrap();
        assert_eq!(bytes.len(), 12); // 3 * 4 bytes
        assert_eq!(&bytes[0..4], &1.0f32.to_le_bytes());
        assert_eq!(&bytes[8..12], &3.0f32.to_le_bytes());
        // Short input is zero-padded to the declared dimension.
        let bytes = encode_value("EMBEDDING(FLOAT,3)", 10, "[1]")
            .unwrap()
            .unwrap();
        assert_eq!(bytes.len(), 12);
        assert_eq!(&bytes[4..8], &0.0f32.to_le_bytes());
        // Int8 elements are one byte wide.
        let bytes = encode_value("EMBEDDING(INT8,2)", 10, "[1,2]")
            .unwrap()
            .unwrap();
        assert_eq!(bytes, vec![1u8, 2u8]);
    }

    #[test]
    fn embedding_blocks_roundtrip() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute("CREATE TABLE v (id INT, e EMBEDDING(FLOAT,3));")
            .unwrap();
        engine
            .execute("INSERT INTO v VALUES (1, [1,2,3]);")
            .unwrap();
        engine
            .execute("INSERT INTO v VALUES (2, [4,5,6]);")
            .unwrap();

        assert_eq!(engine.write_cpp_blocks("v").unwrap(), 2);
        let (_, rows) = engine.read_cpp_blocks("v").unwrap();
        assert_eq!(
            rows,
            vec![
                vec!["1".to_string(), "[1,2,3]".to_string()],
                vec!["2".to_string(), "[4,5,6]".to_string()],
            ]
        );
    }

    /// `TENSOR(FLOAT,2)` rows `lo..hi`, three tokens each.
    fn plaid_rows(lo: usize, hi: usize) -> String {
        let mut values = Vec::new();
        for i in lo..hi {
            let mut tokens = Vec::new();
            for j in 0..3 {
                let x = (i % 5) as f32 * 0.5 + j as f32 * 0.25;
                let y = ((i + j) % 4) as f32 - 1.0;
                tokens.push(format!("[{x},{y}]"));
            }
            values.push(format!("({i}, [{}])", tokens.join(",")));
        }
        values.join(",")
    }

    /// `CREATE INDEX … USING PLAID` over `plaid_rows(0, first)` plus a second
    /// batch, flushed between the batches.
    fn plaid_mirror_with_two_chunks(dir: &std::path::Path) -> Engine {
        let engine = Engine::open(dir).unwrap();
        engine
            .execute("CREATE TABLE pc (id INT, t TENSOR(FLOAT,2));")
            .unwrap();
        engine
            .execute(&format!("INSERT INTO pc VALUES {};", plaid_rows(0, 200)))
            .unwrap();
        engine
            .execute("CREATE INDEX pidx ON pc (t) USING PLAID WITH (nbits = 4, n_centroids = 8);")
            .unwrap();
        engine.flush().unwrap();
        engine
            .execute(&format!("INSERT INTO pc VALUES {};", plaid_rows(200, 260)))
            .unwrap();
        engine.flush().unwrap();
        engine
    }

    /// Every mem-index dump appends a chunk (`GetAndSetNextChunkID`), and a
    /// later chunk shares the first one's global centroids instead of
    /// retraining them.
    #[test]
    fn plaid_dump_appends_a_chunk_per_batch() {
        let dir = tempfile::tempdir().unwrap();
        let engine = plaid_mirror_with_two_chunks(dir.path());

        let chunks = engine.read_cpp_plaid_chunks("pc", "pidx").unwrap();
        assert_eq!(chunks.len(), 2, "each flush appends one chunk");
        assert_eq!(chunks[0].doc_num(), 200);
        assert_eq!(chunks[1].doc_num(), 60, "only the rows past the watermark");
        assert_eq!(chunks[1].total_embedding_num(), 180);
        assert_eq!(
            chunks[1].is_colbertsar_mode(),
            chunks[0].is_colbertsar_mode()
        );
        assert_eq!(chunks[1].n_centroids(), chunks[0].n_centroids());
        assert_eq!(chunks[1].centroids_data(), chunks[0].centroids_data());
        assert_eq!(
            chunks[1].centroid_norms_neg_half(),
            chunks[0].centroid_norms_neg_half()
        );

        // A flush with nothing new must not append an empty chunk.
        engine.flush().unwrap();
        let chunks = engine.read_cpp_plaid_chunks("pc", "pidx").unwrap();
        assert_eq!(chunks.len(), 2);
        // The read path merges the chunks back into one index.
        let merged = engine.read_cpp_plaid_index("pc", "pidx").unwrap();
        assert_eq!(merged.doc_num(), 260);
        assert_eq!(merged.total_embedding_num(), 780);
    }

    /// `OptimizePlaidIndex`: `InitializeMerge` → `MergeOneChunk` with a running
    /// `doc_offset` → `FinalizeMerge` → `CopyCentroidsFrom`, then cleanup drops
    /// the superseded chunks. A wrong `doc_offset` shifts every document id, so
    /// the scores are compared document by document.
    #[test]
    fn plaid_optimize_merges_chunks_preserving_doc_offsets() {
        let dir = tempfile::tempdir().unwrap();
        let engine = plaid_mirror_with_two_chunks(dir.path());
        let query = "SELECT id FROM pc SEARCH MATCH TENSOR \
                     (t, [[0.5,0.0],[1.0,-1.0]], 'float', 'maxsim', 'topn=5');";
        let before_rows = row_strings(&engine, query);
        assert_eq!(before_rows.len(), 5);

        let chunks = engine.read_cpp_plaid_chunks("pc", "pidx").unwrap();
        assert_eq!(chunks.len(), 2);
        let dir = engine.cpp_plaid_mirror("pc", "pidx").unwrap().dir;
        let chunk_bytes: Vec<Vec<u8>> = (0..2)
            .map(|id| std::fs::read(dir.join(format!("chunk_{id}.idx"))).unwrap())
            .collect();
        // Exact MaxSim of every mirrored document, with the id the merged index
        // must give it: chunk 1 follows chunk 0.
        let probe: Vec<f32> = vec![0.5, 0.0, 1.0, -1.0];
        let mut expected: Vec<f32> = Vec::new();
        for chunk in &chunks {
            let ids: Vec<u32> = (0..chunk.doc_num()).collect();
            expected.extend(chunk.exact_score_batch(&probe, 2, &ids));
        }
        assert_eq!(expected.len(), 260);
        let token_offset = chunks[0].total_embedding_num();

        engine.optimize_indexes().unwrap();
        let merged = engine.read_cpp_plaid_chunks("pc", "pidx").unwrap();
        assert_eq!(
            merged.len(),
            1,
            "the merge replaces the chunks it folded in"
        );
        let merged = &merged[0];
        assert_eq!(merged.doc_num(), 260);
        assert_eq!(
            merged.total_embedding_num(),
            chunks[0].total_embedding_num() + chunks[1].total_embedding_num()
        );
        assert_eq!(merged.n_centroids(), chunks[0].n_centroids());
        assert_eq!(merged.centroids_data(), chunks[0].centroids_data());
        // Document 200 of the merge points just past chunk 0's tokens.
        assert_eq!(
            merged.doc_offsets()[200],
            token_offset + chunks[1].doc_offsets()[0]
        );
        let ids: Vec<u32> = (0..260).collect();
        let got = merged.exact_score_batch(&probe, 2, &ids);
        for (i, score) in got.iter().enumerate() {
            assert!(
                (score - expected[i]).abs() < 1e-6,
                "doc {i}: merged {score} != unmerged {}",
                expected[i]
            );
        }

        // The bytes of the chunks a merge superseded stay on disk until
        // `Cleanup` - the optimize commit only takes them out of the chunk
        // list. Putting them back proves both halves: a reader does not see
        // them now, and cleanup is what deletes the files.
        std::fs::write(dir.join("chunk_0.idx"), &chunk_bytes[0]).unwrap();
        std::fs::write(dir.join("chunk_1.idx"), &chunk_bytes[1]).unwrap();
        assert_eq!(
            engine.read_cpp_plaid_chunks("pc", "pidx").unwrap().len(),
            1,
            "the superseded chunks are out of the chunk list"
        );
        engine.cleanup().unwrap();
        assert!(!dir.join("chunk_0.idx").exists(), "cleanup deletes the bytes");
        assert!(!dir.join("chunk_1.idx").exists());
        let after = engine.read_cpp_plaid_chunks("pc", "pidx").unwrap();
        assert_eq!(after.len(), 1, "cleanup leaves the merged chunk");
        assert_eq!(after[0].doc_num(), 260);
        assert_eq!(
            row_strings(&engine, query),
            before_rows,
            "the search path answers the same rows after the merge"
        );
    }

    /// A checkpoint rewrites the whole C++ layout from its own read-back
    /// (`Catalog::rewrite_cpp_layout`), so every record it does not carry
    /// through is lost. `first_delete_ts` is such a record: the delete writes
    /// it, the read-back has to hand it back, or the next checkpoint tells
    /// every reader that nobody ever deleted a row.
    #[test]
    fn a_checkpoint_keeps_the_segments_first_delete_ts() {
        let dir = tempfile::tempdir().unwrap();
        {
            let e = Engine::open(dir.path()).unwrap();
            e.execute("CREATE TABLE t (id INT, v FLOAT);").unwrap();
            e.execute("INSERT INTO t VALUES (1, 1.0), (2, 2.0), (3, 3.0);")
                .unwrap();
            e.write_cpp_blocks("t").unwrap();
        }
        let e = Engine::open(dir.path()).unwrap();
        e.execute("INSERT INTO t VALUES (4, 4.0);").unwrap();
        e.execute("DELETE FROM t WHERE id = 1;").unwrap();
        // The delete is in the table's delete log; the segment record that
        // carries `first_delete_ts` is (re)written by the checkpoint, which is
        // where an append-buffer-only delete reaches the block files.
        e.write_cpp_blocks("t").unwrap();
        let delete_ts = e
            .delete_ts("t")
            .into_iter()
            .find(|ts| *ts != 0)
            .expect("the delete was stamped");
        let tag = |e: &Engine| {
            e.storage
                .catalog
                .store()
                .get(b"seg|1|0|0|first_delete_ts")
                .unwrap()
                .map(|v| String::from_utf8_lossy(&v).to_string())
        };
        assert_eq!(tag(&e).as_deref(), Some(delete_ts.to_string().as_str()));
        // The checkpoint sweeps the layout and writes it back; a DDL captures
        // it the same way.
        e.flush().unwrap();
        assert_eq!(
            tag(&e).as_deref(),
            Some(delete_ts.to_string().as_str()),
            "a checkpoint must not claim the segment has no deletes"
        );
        e.execute("CREATE INDEX ix ON t (id);").unwrap();
        assert_eq!(
            tag(&e).as_deref(),
            Some(delete_ts.to_string().as_str()),
            "a DDL must not claim the segment has no deletes"
        );
    }

    #[test]
    fn plaid_coverage_counts_a_merged_newest_chunk_once() {
        assert_eq!(plaid_coverage(&[(200, 600)]), (200, 600));
        assert_eq!(plaid_coverage(&[(200, 600), (60, 180)]), (260, 780));
        assert_eq!(
            plaid_coverage(&[(200, 600), (60, 180), (260, 780)]),
            (260, 780),
            "the merged chunk replaces the ones it was built from"
        );
        // A new batch on top of a merge is additive again.
        assert_eq!(
            plaid_coverage(&[(260, 780), (10, 30)]),
            (270, 810),
            "an incremental chunk after a merge still adds up"
        );
    }
}
