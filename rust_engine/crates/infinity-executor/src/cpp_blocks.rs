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

use infinity_catalog::cpp_layout::{
    CppBlock, CppIndexChunk, CppIndexSegment, CppSegment, CppSegmentSet, CPP_MIRROR_TS,
};
use infinity_common::{Error, Result};
use infinity_expression::Value;
use infinity_index::hnsw_handler::HnswHandler;
use infinity_index::plaid::PlaidIndex;
use infinity_index::vector::HnswIndex;
use infinity_storage::cpp_format::{
    self, date_to_days, datetime_to_pair, element_width, embedding_size, pack_column_with_layout,
    push_embedding_element, time_to_secs, BlockColumn, BlockId, BlockVersionFile, ColumnLayout,
    RawBlockColumn, DEFAULT_BLOCK_CAPACITY,
};
use infinity_storage::knn_format;

use crate::Engine;

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

/// A table whose estimated encoded size exceeds this keeps only the engine's
/// own row store. The C++ block mirror duplicates every row, so mirroring a
/// multi-gigabyte table would double its disk footprint; the mirror is a
/// best-effort interop artifact, not the system of record.
const MIRROR_BUDGET_BYTES: usize = 1024 * 1024 * 1024;

/// The sealed chunks of a private mirror, as `(chunk id, row count, path)`.
///
/// The secondary mirror names its dumps `seg_{segment}_{block}_{rows}.idx`: the
/// block is the chunk the reference publishes, and the trailing field is how
/// many rows the dump contributed. A block can be covered by several dumps, so
/// `publish_secondary_chunks` merges them rather than copying one file.

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
fn publish_secondary_chunks(
    seg_dir: &Path,
    dir: &Path,
    kind: infinity_storage::secondary_segment::KeyKind,
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
            base_row_id: block * DEFAULT_BLOCK_CAPACITY as u64,
            row_count: entries.len() as u64,
            term_count: 0,
            index_size: 0,
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
) -> Vec<CppIndexChunk> {
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
        let bytes = infinity_storage::bmp_cpp::encode_bmp(
            infinity_index::sparse::BMP_BLOCK_SIZE,
            width,
            &docs,
            term_num,
        );
        let path = seg_dir.join(format!("chunk_{block}.idx"));
        if std::fs::write(&path, &bytes).is_err() {
            continue;
        }
        chunks.push(CppIndexChunk {
            chunk_id: block as u32,
            base_name: String::new(),
            base_row_id: block * DEFAULT_BLOCK_CAPACITY as u64,
            row_count: docs.len() as u64,
            term_count: 0,
            index_size: 0,
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
            chunk_id: (base_row_id / DEFAULT_BLOCK_CAPACITY as u64) as u32,
            base_name: base,
            base_row_id,
            row_count,
            term_count,
            index_size: 0,
        });
    }
    chunks.sort_by_key(|c| c.chunk_id);
    chunks
}

/// The `chunk_{id}.idx` files of an index that writes them in place (HNSW, IVF,
/// PLAID). An HNSW chunk's header carries its vector count, so a multi-chunk
/// index can be described exactly; the other kinds still write one chunk, which
/// covers the whole snapshot.
fn publish_chunk_files(seg_dir: &Path, rows: u64, hnsw: bool) -> Vec<CppIndexChunk> {
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
                base_row_id: 0,
                row_count: rows,
                term_count: 0,
                index_size: 0,
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
            base_row_id,
            row_count,
            term_count: 0,
            index_size: 0,
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

/// Rough encoded bytes per row, from the declared column types.
fn estimated_row_bytes(types: &[String]) -> usize {
    types
        .iter()
        .map(|t| {
            let up = t.trim().to_ascii_uppercase();
            let dim = crate::type_dim(t).unwrap_or(0);
            if dim > 0
                && (up.starts_with("VECTOR")
                    || up.starts_with("EMBEDDING")
                    || up.starts_with("MULTIVECTOR")
                    || up.starts_with("TENSOR"))
            {
                return dim * 4;
            }
            if up.starts_with("SPARSE") {
                return dim * 8;
            }
            64
        })
        .sum::<usize>()
        .max(1)
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
    if code == 0 {
        // Bit-packed *tensors* are not covered here (only embeddings are).
        return None;
    }
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
/// Bit elements are excluded for the same reason as tensors.
pub fn tensorarray_shape(type_text: &str) -> Option<(i32, usize, i32)> {
    let up = type_text.trim().to_ascii_uppercase();
    let rest = up.strip_prefix("TENSORARRAY(")?;
    let inner = rest.trim_end_matches(')');
    let parts: Vec<&str> = inner.split(',').map(str::trim).collect();
    let dim: usize = parts.get(1)?.parse().ok()?;
    let code = element_code(parts.first()?)?;
    if code == 0 {
        // Bit-packed tensors are not covered here (only embeddings are).
        return None;
    }
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
        if let Some((i, v)) = part.split_once(':') {
            if let (Ok(i), Ok(v)) = (i.trim().parse::<i64>(), v.trim().parse::<f64>()) {
                idx.push(i);
                val.push(v);
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
        parts.push(format!("{}:{}", i as i64, v));
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
    } else {
        value.to_string()
    }
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
    let vectors = cpp_format::parse_nested_f64(value);
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
    let width = element_width(elem);
    let mut parts = Vec::with_capacity(count);
    for v in 0..count {
        let at = offset + v * dim * width;
        let mut elems = Vec::with_capacity(dim);
        for k in 0..dim {
            let s = outline.get(at + k * width..at + (k + 1) * width)?;
            elems.push(embedding_element_text(s, elem)?);
        }
        parts.push(format!("[{}]", elems.join(",")));
    }
    Some(format!("[{}]", parts.join(",")))
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
    // `[[[a,b],[c,d]], …]`: one entry per tensor, each a list of rows.
    let tensors = cpp_format::parse_tensorarray_f64(value);
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
    let width = element_width(elem);
    let mut tensors = Vec::with_capacity(tensor_num);
    for t in 0..tensor_num {
        let at = meta_offset + t * 8;
        let meta = u64::from_le_bytes(outline.get(at..at + 8)?.try_into().ok()?);
        let rows = (meta & 0xffff) as usize;
        let offset = (meta >> 16) as usize;
        let mut vecs = Vec::with_capacity(rows);
        for v in 0..rows {
            let base = offset + v * dim * width;
            let mut elems = Vec::with_capacity(dim);
            for k in 0..dim {
                let s = outline.get(base + k * width..base + (k + 1) * width)?;
                elems.push(embedding_element_text(s, elem)?);
            }
            vecs.push(format!("[{}]", elems.join(",")));
        }
        tensors.push(format!("[{}]", vecs.join(",")));
    }
    Some(format!("[{}]", tensors.join(",")))
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
            infinity_storage::inverted_format::doc_list::DocListEncoder::new(doc_option.clone());
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
        Value::Float(f) => format!("{f}"),
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
    if let Some((dim, elem)) = embedding_shape(type_text) {
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
                // `Value::to_slt_string` renders floats with six decimals. That
                // is fine for display, but a block stores the raw value: a
                // six-decimal rendering would both lose digits here and make
                // the port's blocks differ from the reference's.
                5 => format!("{}", f32::from_le_bytes(slot.try_into().unwrap())),
                6 => format!("{}", f64::from_le_bytes(slot.try_into().unwrap())),
                // The 16-bit floats are two bytes wide and render like the
                // other floats: an IEEE half (`float16_t`) and a bfloat whose
                // raw bits are the high half of an `f32` (`bfloat16_t`).
                7 => format!(
                    "{}",
                    crate::f16_bits_to_f32(u16::from_le_bytes(slot.try_into().unwrap()))
                ),
                8 => format!(
                    "{}",
                    f32::from_bits((u16::from_le_bytes(slot.try_into().unwrap()) as u32) << 16)
                ),
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
    fn qualified_table(&self, table: &str) -> String {
        if table.contains('.') {
            table.to_string()
        } else {
            format!("{}.{}", self.current_db.lock().unwrap(), table)
        }
    }

    /// Writes `table`'s rows into `{data_dir}/data/db_{db}/tbl_{id}/seg_0/…`
    /// in the C++ block layout, returning the number of rows written.
    ///
    /// Rows are chunked `DEFAULT_BLOCK_CAPACITY` (8192) per block, as C++ does.
    pub fn write_cpp_blocks(&self, table: &str) -> Result<usize> {
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

        let mut rows = self.fetch_rows_with_ids(&table)?;
        // The row ids this snapshot covers, in block order. Reads use them to
        // tell block-resident rows from rows written after the checkpoint.
        let block_rows: Vec<usize> = rows.iter().map(|(seq, _)| *seq).collect();
        let db_id = self.database_id_of(&table);
        let root = self.blocks_root();
        // Drop any previous mirror of this table's first segment so a rewrite
        // cannot leave stale blocks behind. The recorded row ids go with it:
        // until every block is rewritten, the mirror matches no snapshot.
        let seg_dir = root.join(format!("db_{db_id}/tbl_{}/seg_0", meta.id));
        let tkey = self.table_key(&table);
        {
            let mut tables = self.tables.lock().unwrap();
            let entry = tables.entry(tkey.clone()).or_default();
            entry.block_index = None;
            entry.cpp_block_rows = None;
        }
        let _ = std::fs::remove_dir_all(&seg_dir);

        let mut written = 0usize;
        let mut blocks: Vec<CppBlock> = Vec::new();
        // Drain the snapshot block by block so each block's values are freed
        // once it is encoded: holding the whole table's decoded rows while the
        // texts are rendered doubled the transient on a long import.
        let mut block_index = 0usize;
        while !rows.is_empty() {
            let take = rows.len().min(DEFAULT_BLOCK_CAPACITY);
            let chunk: Vec<(usize, Vec<Value>)> = rows.drain(..take).collect();
            let texts: Vec<Vec<String>> = chunk
                .iter()
                .map(|(_, row)| row.iter().map(mirror_cell_text).collect())
                .collect();
            let block_columns =
                self.encode_block_columns(&meta.types, &meta.ids, &types, &layouts, &texts)?;
            // The mirror is a full rewrite at one checkpoint, so its rows carry
            // no per-append commit order: publishing them with ts 0 keeps them
            // visible to a reader whose `begin_ts` is 1 (the C++
            // `BlockVersion::GetRowCount` returns 0 when the first create entry
            // is *newer* than `begin_ts`).
            let version = BlockVersionFile::new(
                &[(CPP_MIRROR_TS, chunk.len() as u32)],
                DEFAULT_BLOCK_CAPACITY,
            );
            let id = BlockId {
                db_id,
                table_id: meta.id,
                segment_id: 0,
                block_id: block_index as u16,
            };
            self.context.buffer.lock().unwrap().invalidate(&id);
            let block_dir = cpp_format::block_dir(db_id, meta.id, 0, id.block_id);
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
            blocks.push(CppBlock {
                block_id: block_index as u16,
                commit_ts: CPP_MIRROR_TS,
            });
            written += chunk.len();
            block_index += 1;
        }
        // Publish the segment layer the C++ reader enumerates
        // (`catalog|seg|…` / `catalog|blk|…`, `GetTableSegments`): without it
        // the table opens with no segments and every query returns nothing.
        // One segment (`seg_0`), as the mirror writes all blocks into it.
        self.storage.catalog.publish_cpp_segments(&CppSegmentSet {
            db_id: db_id.to_string(),
            table_id: meta.id.to_string(),
            segments: vec![CppSegment {
                segment_id: 0,
                create_ts: CPP_MIRROR_TS,
                blocks,
            }],
        })?;
        // The mirror has new block files; the table's block map must rescan.
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(tkey).or_default();
        entry.block_index = None;
        entry.cpp_block_rows = Some(std::sync::Arc::new(block_rows));
        Ok(written)
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
            let (body, packed_outline) =
                pack_column_with_layout(*column_type, layouts[col], &bytes, DEFAULT_BLOCK_CAPACITY)
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

    /// Mirrors every table into the C++ block layout.
    ///
    /// Called from [`Engine::flush`], so a normal clean shutdown leaves the data
    /// directory readable by the C++ engine. Tables whose columns are not yet
    /// packable (array/tensor/sparse/embedding) are skipped rather than
    /// mis-encoded.
    pub fn flush_cpp_blocks(&self) -> Result<()> {
        for meta in self.storage.catalog.list_tables()? {
            let rows = self.approx_row_count(&meta.name);
            let per_row = estimated_row_bytes(&meta.types);
            if meta.types.iter().all(|t| column_type_of(t).is_some())
                && rows.saturating_mul(per_row) <= MIRROR_BUDGET_BYTES
            {
                self.write_cpp_blocks(&meta.name)?;
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
            let live: Vec<String> = self
                .storage
                .catalog
                .list_indexes()?
                .iter()
                .filter(|im| im.table == meta.name)
                .filter_map(|im| self.cpp_index_position(&meta.name, &im.name).ok())
                .map(|id| format!("idx_{id}"))
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
    fn publish_cpp_index_chunks(&self, table: &str, table_id: u64) -> Result<()> {
        let db_id = self.database_id_of(table);
        let root = self
            .blocks_root()
            .join(format!("db_{db_id}/tbl_{table_id}"));
        let rows = self.approx_row_count(table);
        let mut published = Vec::new();
        for im in self.storage.catalog.list_indexes()? {
            if im.table != table && im.table != self.qualified_table(table) {
                continue;
            }
            let Ok(index_id) = self.cpp_index_position(table, &im.name) else {
                continue;
            };
            let seg_dir = root.join(format!("idx_{index_id}/seg_0"));
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
            let chunks = if im.index_type.eq_ignore_ascii_case("SECONDARY") {
                let dir = self.dir.join("secondary").join(&private);
                // The chunk's key width comes from the column's declared type,
                // exactly as the reader derives it (`SecondarySegmentReader::
                // open_with_kind`).
                publish_secondary_chunks(&seg_dir, &dir, crate::secondary_key_kind(&declared))
            } else if im.index_type.eq_ignore_ascii_case("BMP") {
                // Rebuilt from the rows: the reference's chunk is the block and
                // every dump extends it, while the port's spill files are per
                // dump, so merging has to happen here.
                publish_bmp_chunks(
                    &seg_dir,
                    self.sparse_rows_by_block(table, &im.column),
                    crate::sparse_idx_width(&declared),
                    crate::sparse_declared_bound(&declared).unwrap_or(0),
                )
            } else if im.index_type.eq_ignore_ascii_case("FULLTEXT") {
                // Written in place by `write_cpp_fulltext_index`; its files are
                // named after the chunk's base name, not `chunk_N.idx`.
                publish_fulltext_chunks(&seg_dir)
            } else {
                publish_chunk_files(
                    &seg_dir,
                    rows as u64,
                    im.index_type.eq_ignore_ascii_case("HNSW"),
                )
            };
            if chunks.is_empty() {
                continue;
            }
            published.push(CppIndexSegment {
                index_id: index_id as u64,
                segment_id: 0,
                chunks,
            });
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

    /// A sparse column's documents, grouped by physical block, in the BMP
    /// encoder's form: `(row id within the segment, [(term, value)])`.
    fn sparse_rows_by_block(
        &self,
        table: &str,
        column: &str,
    ) -> std::collections::BTreeMap<u64, Vec<infinity_storage::bmp_cpp::SparseDoc>> {
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

    /// Best-effort recovery on startup: for every table with **no rows** but an
    /// existing block mirror, load the mirror back.
    ///
    /// A table whose columns the block format cannot express simply has no
    /// mirror; that (and only that) is skipped silently.
    pub fn recover_cpp_blocks(&self) -> Result<()> {
        for meta in self.storage.catalog.list_tables()? {
            if !self.fetch_rows_with_ids(&meta.name)?.is_empty() {
                continue;
            }
            if self.read_cpp_blocks(&meta.name).is_ok() {
                self.load_cpp_blocks(&meta.name)?;
            }
        }
        Ok(())
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
        self.read_cpp_blocks_wanted(table, None)
    }

    /// Like [`Self::read_cpp_blocks`] but decodes only `wanted` columns; the
    /// others are returned as `"null"`. This is the column-projected read path
    /// for `SELECT id, name` (plan #87).
    pub fn read_cpp_blocks_selected(
        &self,
        table: &str,
        wanted: &[usize],
    ) -> Result<(Vec<String>, Vec<Vec<String>>)> {
        self.read_cpp_blocks_wanted(table, Some(wanted))
    }

    /// Number of rows the block mirror currently holds.
    pub fn cpp_block_row_count(&self, table: &str) -> Result<usize> {
        let table = self.qualified_table(table);
        let meta = self
            .storage
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let db_id = self.database_id_of(&table);
        let root = self.blocks_root();
        let mut rows = 0usize;
        for block_index in 0..u16::MAX {
            let dir = root.join(cpp_format::block_dir(db_id, meta.id, 0, block_index));
            let Ok(bytes) = std::fs::read(dir.join(cpp_format::VERSION_FILE)) else {
                break;
            };
            let Ok(version) = cpp_format::BlockVersionFile::decode(&bytes) else {
                break;
            };
            rows += version
                .created
                .last()
                .map(|c| c.cumulative_row_count as usize)
                .unwrap_or(0);
        }
        Ok(rows)
    }

    fn read_cpp_blocks_wanted(
        &self,
        table: &str,
        wanted: Option<&[usize]>,
    ) -> Result<(Vec<String>, Vec<Vec<String>>)> {
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

        let mut rows: Vec<Vec<String>> = Vec::new();
        for block_index in 0..u16::MAX {
            let id = BlockId {
                db_id,
                table_id: meta.id,
                segment_id: 0,
                block_id: block_index,
            };
            let (version, columns) = match self.read_block_cached(&table, &root, id) {
                Ok(v) => v,
                Err(_) if block_index == 0 => {
                    return Err(Error::NotFound(format!("no blocks for table '{table}'")))
                }
                Err(_) => break,
            };
            let row_count: usize = version
                .created
                .last()
                .map(|c| c.cumulative_row_count as usize)
                .unwrap_or(0);
            for row in 0..row_count {
                let mut out = Vec::with_capacity(types.len());
                for (col, column_type) in types.iter().enumerate() {
                    if let Some(wanted) = wanted {
                        if !wanted.contains(&col) {
                            out.push("null".to_string());
                            continue;
                        }
                    }
                    let column_id = meta.ids.get(col).copied().unwrap_or(col as u64);
                    let Some(raw) = columns.iter().find(|c| c.column_id == column_id) else {
                        return Err(Error::Invalid(format!("block is missing column {col}")));
                    };
                    let layout = layout_of(&meta.types[col], *column_type)
                        .ok_or_else(|| Error::Invalid("unsupported column layout".into()))?;
                    let body = cpp_format::split_body_for_layout(
                        &raw.body,
                        DEFAULT_BLOCK_CAPACITY,
                        layout,
                    );
                    let value = if body.null_bitmap.len() > row / 8
                        && body.null_bitmap[row / 8] & (1 << (row % 8)) == 0
                    {
                        "null".to_string()
                    } else {
                        // Tensor / sparse slots point into the column's outline.
                        match outline_kind(&meta.types[col]) {
                            OutlineKind::Tensor => {
                                let at = row * 8;
                                let slot = body.data.get(at..at + 8).unwrap_or(&[]);
                                decode_tensor_value(&meta.types[col], slot, &raw.outline)
                                    .unwrap_or_else(|| "null".into())
                            }
                            OutlineKind::TensorArray => {
                                let at = row * 8;
                                let slot = body.data.get(at..at + 8).unwrap_or(&[]);
                                decode_tensorarray_value(&meta.types[col], slot, &raw.outline)
                                    .unwrap_or_else(|| "null".into())
                            }
                            OutlineKind::Sparse => {
                                let at = row * 16;
                                let slot = body.data.get(at..at + 16).unwrap_or(&[]);
                                decode_sparse_value(&meta.types[col], slot, &raw.outline)
                                    .unwrap_or_else(|| "null".into())
                            }
                            OutlineKind::Array => {
                                let at = row * 8;
                                let slot = body.data.get(at..at + 8).unwrap_or(&[]);
                                decode_array_value(&meta.types[col], slot, &raw.outline)
                                    .unwrap_or_else(|| "null".into())
                            }
                            OutlineKind::None => decode_value(
                                &meta.types[col],
                                *column_type,
                                &body.data,
                                row,
                                &raw.outline,
                            )
                            .unwrap_or_else(|| "null".into()),
                        }
                    };
                    out.push(value);
                }
                rows.push(out);
            }
        }
        Ok((meta.columns.clone(), rows))
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
    fn cpp_index_position(&self, table: &str, index_name: &str) -> Result<usize> {
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

    /// The vectors of `table`'s `column` as `(row id, vector)`.
    fn cpp_column_vectors(&self, table: &str, column: &str) -> Result<VectorRows> {
        let schema = self.table_schema(table)?;
        let col_idx = schema
            .iter()
            .position(|c| c == column)
            .ok_or_else(|| Error::NotFound(format!("column '{column}' on '{table}'")))?;
        let mut out = Vec::new();
        for (seq, row) in self.fetch_rows_with_ids(table)? {
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
    ) -> Result<(usize, Vec<u8>)> {
        let vectors = self.cpp_column_vectors(table, column)?;
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
    }

    /// Appends a fresh chunk, as the reference's memory-index dump does. Rows
    /// added since the previous dump end up in the new chunk.
    pub fn dump_cpp_hnsw_index(&self, table: &str, index_name: &str) -> Result<usize> {
        self.write_cpp_hnsw_chunk(table, index_name, true, false)
    }

    /// The optimize merge: rebuilds the graph over every live row and appends
    /// it as the newest chunk, which then supersedes the incremental ones.
    pub fn merge_cpp_hnsw_index(&self, table: &str, index_name: &str) -> Result<usize> {
        self.write_cpp_hnsw_chunk(table, index_name, false, true)
    }

    fn write_cpp_hnsw_chunk(
        &self,
        table: &str,
        index_name: &str,
        new_chunk: bool,
        force_append: bool,
    ) -> Result<usize> {
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
        let tkey = self.table_key(&table);
        let from_memory = if new_chunk {
            let Some(mem) = self
                .tables
                .lock()
                .unwrap()
                .get(&tkey)
                .and_then(|e| e.hnsw_mem.get(&column).cloned())
            else {
                // Nothing buffered since the last dump: a checkpoint has
                // nothing to write and must not rebuild the whole graph.
                return Ok(0);
            };
            let Ok(mem) = mem.lock() else {
                return Ok(0);
            };
            if mem.is_empty() {
                return Ok(0);
            }
            let labels: Vec<u32> = mem.rows().iter().map(|s| *s as u32).collect();
            Some(encode_hnsw_chunk_from(mem.index(), &labels))
        } else {
            None
        };
        let db_id = self.database_id_of(&table);
        let dir = self
            .blocks_root()
            .join(format!("db_{db_id}/tbl_{}/idx_{index_id}/seg_0", meta.id));
        std::fs::create_dir_all(&dir).map_err(|e| Error::Io {
            path: dir.clone(),
            source: e,
        })?;
        let chunks = hnsw_chunks(&dir);
        // A checkpoint only mirrors an index that has no chunk yet. Check that
        // before building: the build decodes every row and graph, which on a
        // long import is the dominant transient (Gate C RSS) and is discarded
        // here anyway.
        if !new_chunk && !force_append && !chunks.is_empty() {
            return Ok(0);
        }
        let (count, chunk) = match from_memory {
            Some(v) => v,
            None => self.cpp_hnsw_chunk(&table, &column, &self.hnsw_options(&table, index_name))?,
        };
        let name = format!(
            "chunk_{}.idx",
            chunks.last().map(|(id, _)| id + 1).unwrap_or(0)
        );
        std::fs::write(dir.join(name), &chunk).map_err(|e| Error::Io {
            path: dir.clone(),
            source: e,
        })?;
        Ok(count)
    }

    /// Optimizes every vector mirror: HNSW chunks are merged when the newest
    /// one does not cover all live rows and the merge is affordable, PLAID
    /// chunks are folded into one by [`Self::merge_cpp_plaid_index`] (the
    /// reference's `OptimizeIndexInner` `kPLAID` case) as soon as a second
    /// chunk exists. A mirror that is already merged is left untouched, so
    /// repeated optimize ticks are no-ops.
    pub fn optimize_cpp_index_chunks(&self) -> Result<()> {
        for im in self.storage.catalog.list_indexes()? {
            if im.index_type.eq_ignore_ascii_case("PLAID") {
                let _ = self.merge_cpp_plaid_index(&im.table, &im.name);
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
            let chunks = hnsw_chunks(&dir);
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
            let _ = self.merge_cpp_hnsw_index(&im.table, &im.name);
        }
        Ok(())
    }

    /// Removes the index chunks that a merged chunk supersedes. The reference
    /// only cleans up chunks after `optimize` merged every live row into the
    /// newest one; when the newest chunk does not cover the table, every
    /// chunk is still needed and is kept.
    pub fn cleanup_cpp_index_chunks(&self) -> Result<()> {
        for im in self.storage.catalog.list_indexes()? {
            if im.index_type.eq_ignore_ascii_case("PLAID") {
                // PLAID chunks carry their own document count in the header,
                // so this mirrors `OptimizePlaidIndex`'s "the merged chunk
                // holds every document of the older ones" without decoding.
                // The merge drops its inputs itself; this reclaims them when
                // that removal failed.
                let Ok(mirror) = self.cpp_plaid_mirror(&im.table, &im.name) else {
                    continue;
                };
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
            // chunk; incremental dumps cover just their own rows.
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
    fn cpp_hnsw_dir(&self, table: &str, index_name: &str) -> Option<PathBuf> {
        let table = self.qualified_table(table);
        let meta = self.storage.catalog.get_table_by_name(&table).ok()??;
        let index_id = self.cpp_index_position(&table, index_name).ok()?;
        let db_id = self.database_id_of(&table);
        Some(
            self.blocks_root()
                .join(format!("db_{db_id}/tbl_{}/idx_{index_id}/seg_0", meta.id)),
        )
    }

    /// The newest chunk file of `table`'s HNSW mirror.
    fn cpp_hnsw_chunk_path(&self, table: &str, index_name: &str) -> Result<PathBuf> {
        let dir = self
            .cpp_hnsw_dir(table, index_name)
            .ok_or_else(|| Error::NotFound(format!("index '{index_name}'")))?;
        hnsw_chunks(&dir)
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
        let rows = self.fetch_rows_with_ids(&table)?;
        // The reference seals one index chunk per **block**: the mem indexer is
        // handed a block at a time, and the chunk's files are named
        // `ft_{:016x}` after the block's first row id
        // (`new_txn_index_impl.cpp:860`). A 20000-row table therefore has three
        // chunks, `ft_0000000000000000`, `ft_0000000000002000`,
        // `ft_0000000000004000` - not one chunk over the whole segment.
        let dir = self
            .blocks_root()
            .join(format!("db_{db_id}/tbl_{}/idx_{index_id}/seg_0", meta.id));
        std::fs::create_dir_all(&dir).map_err(|e| Error::Io {
            path: dir.clone(),
            source: e,
        })?;
        // A rewrite covers the whole snapshot, so a chunk left by an earlier,
        // longer one would be published as a live chunk.
        if let Ok(entries) = std::fs::read_dir(&dir) {
            for entry in entries.flatten() {
                let path = entry.path();
                let is_fulltext = path
                    .file_name()
                    .and_then(|n| n.to_str())
                    .is_some_and(|n| n.starts_with("ft_"));
                if is_fulltext {
                    let _ = std::fs::remove_file(path);
                }
            }
        }
        let mut written = 0usize;
        // One chunk per **physical block** (`row_id / 8192`), not one per 8192
        // live rows: a block keeps its `base_row_id` even when rows in it were
        // deleted, so the split follows the row ids.
        let mut by_block: std::collections::BTreeMap<u64, Vec<&(usize, Vec<Value>)>> =
            Default::default();
        for row in &rows {
            by_block
                .entry((row.0 as u64 & 0xffff_ffff) / DEFAULT_BLOCK_CAPACITY as u64)
                .or_default()
                .push(row);
        }
        for (block, block_rows) in by_block {
            let base_row_id = block * DEFAULT_BLOCK_CAPACITY as u64;
            // term -> (document ordinal, the term's position in the document),
            // in term order (the FST requires sorted keys). The document
            // ordinal restarts at zero in every chunk: the chunk's `base_row_id`
            // is what puts it back on the segment's row ids
            // (`MemoryIndexer::doc_count_` counts rows within the chunk).
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
        for (chunk_id, path) in hnsw_chunks(&dir) {
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
        hnsw_chunks(&dir).into_iter().map(|(id, _)| id).collect()
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
        let path = hnsw_chunks(&dir)
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
        hnsw_chunks(&dir)
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
    ) -> Result<(usize, Vec<u8>)> {
        let vectors = self.cpp_column_vectors(table, column)?;
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
        let (count, chunk) = self.cpp_ivf_chunk(&table, &column, &options)?;
        let db_id = self.database_id_of(&table);
        let dir = self
            .blocks_root()
            .join(format!("db_{db_id}/tbl_{}/idx_{index_id}/seg_0", meta.id));
        std::fs::create_dir_all(&dir).map_err(|e| Error::Io {
            path: dir.clone(),
            source: e,
        })?;
        std::fs::write(dir.join("chunk_0.idx"), &chunk).map_err(|e| Error::Io {
            path: dir.clone(),
            source: e,
        })?;
        Ok(count)
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
        for (id, path) in hnsw_chunks(&mirror.dir) {
            let bytes = std::fs::read(&path).map_err(|e| Error::Io {
                path: path.clone(),
                source: e,
            })?;
            let (index, _) =
                PlaidIndex::from_bytes(0, mirror.dims, nbits, &bytes).map_err(|e| {
                    Error::Invalid(format!("bad PLAID chunk '{}': {e}", path.display()))
                })?;
            chunks.push(PlaidChunkFile { id, path, index });
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
    pub fn merge_cpp_plaid_index(&self, table: &str, index_name: &str) -> Result<usize> {
        let mirror = self.cpp_plaid_mirror(table, index_name)?;
        let chunks = self.read_plaid_chunks(table, index_name)?;
        if chunks.len() < 2 {
            return Ok(chunks
                .first()
                .map(|c| c.index.doc_num() as usize)
                .unwrap_or(0));
        }
        let next = chunks.last().map(|c| c.id + 1).unwrap_or(0);
        let paths: Vec<PathBuf> = chunks.iter().map(|c| c.path.clone()).collect();
        let indexes: Vec<PlaidIndex> = chunks.into_iter().map(|c| c.index).collect();
        let merged = Self::merge_plaid_chunk_indexes(&indexes)?;
        let path = mirror.dir.join(format!("chunk_{next}.idx"));
        std::fs::write(&path, merged.to_bytes()).map_err(|e| Error::Io {
            path: path.clone(),
            source: e,
        })?;
        for old in paths {
            let _ = std::fs::remove_file(old);
        }
        Ok(merged.doc_num() as usize)
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
    path: PathBuf,
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

#[cfg(test)]
mod tests {
    use super::*;
    use crate::QueryResult;

    fn row_strings(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
        let mut out = Vec::new();
        for r in engine.execute(sql).unwrap() {
            if let QueryResult::Rows { rows, .. } = r {
                out = rows;
            }
        }
        out
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
        // Bit embeddings pack to `ceil(dim/8)` bytes; bit *tensors* (and hence
        // bit tensor arrays) are not covered by the block writer.
        assert_eq!(column_type_of("EMBEDDING(BIT,8)"), Some(10));
        assert_eq!(column_type_of("TENSOR(BIT,8)"), None);
        assert_eq!(column_type_of("TENSORARRAY(BIT,8)"), None);
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
        assert_eq!(
            rows,
            vec![
                vec!["1".to_string(), "[0:1.5,3:2.5]".to_string()],
                vec!["2".to_string(), "[7:3.5]".to_string()],
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
        assert_eq!(
            rows,
            vec![
                vec!["1".to_string(), "[0:1,20:1,80:1]".to_string()],
                vec!["2".to_string(), "[7:1]".to_string()],
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
        assert_eq!(tensor_shape("TENSOR(BIT,8)"), None);
    }

    #[test]
    fn tensorarray_shape_parsing() {
        assert_eq!(tensorarray_shape("TENSORARRAY(FLOAT,3)"), Some((12, 3, 6)));
        assert_eq!(tensorarray_shape("TENSORARRAY(DOUBLE,2)"), Some((12, 2, 7)));
        assert_eq!(tensorarray_shape("TENSOR(FLOAT,3)"), None);
        assert_eq!(tensorarray_shape("TENSORARRAY(BIT,8)"), None);
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

        // Drop the engine's own rows, leaving only the C++ block mirror.
        engine.execute("DELETE FROM t;").unwrap();
        assert_eq!(row_strings(&engine, "SELECT * FROM t;").len(), 0);

        // The mirror reconstructs them (this is what a C++-written directory
        // gives us on startup).
        assert_eq!(engine.load_cpp_blocks("t").unwrap(), 2);
        assert_eq!(
            row_strings(&engine, "SELECT * FROM t;"),
            vec![
                vec!["1".to_string(), "a".to_string(), "true".to_string()],
                vec!["2".to_string(), "bb".to_string(), "false".to_string()],
            ]
        );

        // A fresh engine over the same directory recovers the rows on open
        // (kv rows are present here, so recovery is a no-op, but it must not
        // duplicate them).
        drop(engine);
        let engine = Engine::open(dir.path()).unwrap();
        assert_eq!(row_strings(&engine, "SELECT * FROM t;").len(), 2);
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
        engine.execute("DELETE FROM t;").unwrap();
        assert_eq!(engine.load_cpp_blocks("t").unwrap(), 1);
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
        engine.execute("DELETE FROM b;").unwrap();
        assert_eq!(engine.load_cpp_blocks("b").unwrap(), 1);
        let rows = row_strings(&engine, "SELECT * FROM b;");
        assert_eq!(rows.len(), 1);
        // The engine displays a BIT embedding as a bit string.
        assert_eq!(rows[0][1], "[1011000111000011]");
    }

    #[test]
    fn blocks_reject_unsupported_column_types() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        // A bit tensor array: bit tensors are outside the block writer's scope.
        engine
            .execute("CREATE TABLE v (id INT, ta TENSORARRAY(BIT,8));")
            .unwrap();
        let err = engine.write_cpp_blocks("v").unwrap_err();
        assert!(err.to_string().contains("not supported"), "{err}");
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
        engine.execute("DELETE FROM lv;").unwrap();
        assert_eq!(engine.load_cpp_blocks("lv").unwrap(), 2);
        let rows = row_strings(&engine, "SELECT * FROM lv;");
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

        // A merge whose removal failed leaves the superseded chunks behind; the
        // cleanup pass is what reclaims them.
        std::fs::write(dir.join("chunk_0.idx"), &chunk_bytes[0]).unwrap();
        std::fs::write(dir.join("chunk_1.idx"), &chunk_bytes[1]).unwrap();
        assert_eq!(engine.read_cpp_plaid_chunks("pc", "pidx").unwrap().len(), 3);
        engine.cleanup().unwrap();
        let after = engine.read_cpp_plaid_chunks("pc", "pidx").unwrap();
        assert_eq!(after.len(), 1, "cleanup drops the superseded chunks");
        assert_eq!(after[0].doc_num(), 260);
        assert_eq!(
            row_strings(&engine, query),
            before_rows,
            "the search path answers the same rows after the merge"
        );
    }

    /// The watermark a dump resumes from: chunks are additive until a merged
    /// newest chunk covers them.
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
