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
    metric: infinity_index::vector::Metric,
    encoding: infinity_index::vector::HnswEncoding,
    lsg: Option<infinity_index::lsg::LsgConfig>,
}

impl Default for HnswOptions {
    fn default() -> Self {
        Self {
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

/// `SPARSE(<elem>,<bound>)` → `(element_type, index_type, index_width)`.
///
/// The index width is derived from the bound, exactly as the engine's
/// `describe_type` does (spec §8.3): ≤127 int8, ≤32767 int16, ≤2^31-1 int32,
/// otherwise int64.
pub fn sparse_shape(type_text: &str) -> Option<(i32, i32, usize)> {
    let up = type_text.trim().to_ascii_uppercase();
    let rest = up.strip_prefix("SPARSE(")?;
    let inner = rest.trim_end_matches(')');
    let parts: Vec<&str> = inner.split(',').map(str::trim).collect();
    let elem = element_code(parts.first()?)?;
    let bound: u64 = parts.get(1).and_then(|s| s.parse().ok()).unwrap_or(0);
    let (index_type, width) = if bound <= 127 {
        (2, 1)
    } else if bound <= 32767 {
        (3, 2)
    } else if bound <= 2_147_483_647 {
        (4, 4)
    } else {
        (5, 8)
    };
    Some((elem, index_type, width))
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
        16 => date_to_days(value).ok_or_else(bad)?.to_le_bytes().to_vec(),
        17 => time_to_secs(value).ok_or_else(bad)?.to_le_bytes().to_vec(),
        18 => {
            let (d, secs) = datetime_to_pair(value).ok_or_else(bad)?;
            let mut v = d.to_le_bytes().to_vec();
            v.extend_from_slice(&secs.to_le_bytes());
            v
        }
        // Varchar / Json keep their UTF-8 bytes.
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
                // `Value::to_slt_string` renders floats with six decimals.
                5 => format!("{:.6}", f32::from_le_bytes(slot.try_into().unwrap())),
                6 => format!("{:.6}", f64::from_le_bytes(slot.try_into().unwrap())),
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
            if len <= 13 {
                let inline = &body.get(at + 3..at + 16)?[..len];
                Some(String::from_utf8_lossy(inline).to_string())
            } else {
                let offset =
                    u64::from_le_bytes(body.get(at + 8..at + 16)?.try_into().ok()?) as usize;
                let bytes = outline.get(offset..offset + len)?;
                Some(String::from_utf8_lossy(bytes).to_string())
            }
        }
        ColumnLayout::VarLen(_) => None,
    }
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

        let rows = self.fetch_rows_with_ids(&table)?;
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
        for (block_index, chunk) in rows.chunks(DEFAULT_BLOCK_CAPACITY).enumerate() {
            let texts: Vec<Vec<String>> = chunk
                .iter()
                .map(|(_, row)| row.iter().map(|v| v.to_slt_string()).collect())
                .collect();
            let block_columns =
                self.encode_block_columns(&meta.types, &meta.ids, &types, &layouts, &texts)?;
            let version = BlockVersionFile::new(
                &[(block_index as u64 + 1, chunk.len() as u32)],
                DEFAULT_BLOCK_CAPACITY,
            );
            let id = BlockId {
                db_id,
                table_id: meta.id,
                segment_id: 0,
                block_id: block_index as u16,
            };
            self.context.buffer.lock().unwrap().invalidate(&id);
            cpp_format::write_block(&root, id, &block_columns, &version).map_err(|e| {
                Error::Io {
                    path: root.join(cpp_format::block_dir(db_id, meta.id, 0, id.block_id)),
                    source: e,
                }
            })?;
            written += chunk.len();
        }
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
                let k = im
                    .options
                    .iter()
                    .find(|(key, _)| key.eq_ignore_ascii_case("CENTROID_COUNT"))
                    .and_then(|(_, v)| v.trim().parse().ok())
                    .unwrap_or(16);
                let _ = self.write_cpp_ivf_index(&im.table, &im.name, k);
            }
        }
        Ok(())
    }

    /// Loads a table's rows back **from its C++ block files** into the engine's
    /// own row store, returning how many rows were inserted.
    ///
    /// This is the interop direction §2.6.1 cares about: a `db_*/tbl_*/seg_*/
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

    /// The C++ index id of `index_name` within `table`: its 1-based position
    /// among the table's indexes, which is how `write_cpp_layout` numbers them
    /// (`next_index_id`, `cpp_layout.rs:545-550`).
    fn cpp_index_position(&self, table: &str, index_name: &str) -> Result<usize> {
        let short = table.split('.').next_back().unwrap_or(table);
        let ids: Vec<(String, u64)> = self
            .storage
            .catalog
            .list_indexes()?
            .into_iter()
            .filter(|im| im.table == short || im.table == table)
            .map(|im| (im.name, im.id))
            .collect();
        ids.iter()
            .position(|(n, _)| n == index_name)
            .map(|p| match ids[p].1 {
                0 => p + 1,
                id => id as usize,
            })
            .ok_or_else(|| Error::NotFound(format!("index '{index_name}' on '{table}'")))
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
        let mut index =
            HnswIndex::with_encoding(dim, 16, 200, 200, options.metric, options.encoding);
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
        let (count, chunk) = match from_memory {
            Some(v) => v,
            None => self.cpp_hnsw_chunk(&table, &column, &self.hnsw_options(&table, index_name))?,
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
        if !new_chunk && !force_append && !chunks.is_empty() {
            return Ok(count);
        }
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

    /// Optimizes every HNSW mirror: when the newest chunk does not cover all
    /// live rows and the merge is affordable, writes a merged chunk with the
    /// whole table (the reference's optimize step). A mirror whose newest
    /// chunk already covers the table is left untouched, so repeated optimize
    /// ticks are no-ops.
    pub fn optimize_cpp_index_chunks(&self) -> Result<()> {
        for im in self.storage.catalog.list_indexes()? {
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

    /// Removes the HNSW chunks that a merged chunk supersedes. The reference
    /// only cleans up chunks after `optimize` merged every live row into the
    /// newest one; when the newest chunk does not cover the table, every
    /// chunk is still needed and is kept.
    pub fn cleanup_cpp_index_chunks(&self) -> Result<()> {
        for im in self.storage.catalog.list_indexes()? {
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
        use infinity_storage::inverted_format as inv;

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
        let delims = self.index_delims(&table, &im.column);
        let rows = self.fetch_rows_with_ids(&table)?;

        // term -> document ordinals, in term order (the FST requires sorted keys)
        let mut postings: std::collections::BTreeMap<String, Vec<u32>> =
            std::collections::BTreeMap::new();
        let mut lengths: Vec<u32> = Vec::with_capacity(rows.len());
        for (doc, (_, row)) in rows.iter().enumerate() {
            let text = row.get(col).map(|v| v.to_slt_string()).unwrap_or_default();
            let terms = crate::tokenize_index_text(&text, &delims);
            lengths.push(terms.len() as u32);
            for t in terms {
                postings.entry(t).or_default().push(doc as u32);
            }
        }

        let flags = inv::TermMetaFlags {
            has_term_frequency: false,
            has_term_payload: false,
        };
        let mut pos = Vec::new();
        let mut metas = Vec::with_capacity(postings.len());
        let terms: Vec<(String, Vec<u32>)> = postings.into_iter().collect();
        for (_, docs) in &terms {
            let start = pos.len() as u64;
            let tp = inv::TermPostings {
                doc_skiplist: Vec::new(),
                doc_list: inv::encode_u32_block(docs),
                pos_skiplist: Vec::new(),
                pos_list: Vec::new(),
            };
            pos.extend_from_slice(&tp.encode());
            metas.push(inv::TermMeta {
                doc_freq: docs.len() as u32,
                total_tf: docs.len() as u32,
                payload: 0,
                doc_start: start,
                pos_start: start,
                pos_end: pos.len() as u64,
            });
        }
        let (region, offsets) = inv::encode_term_meta_region_with_offsets(&metas, flags);
        let mut fst = inv::FstBuilder::new();
        for (i, (term, _)) in terms.iter().enumerate() {
            fst.insert(term.as_bytes(), offsets[i]);
        }
        let dic = inv::encode_dic(&region, &fst.finish());
        let len = inv::encode_column_lengths(&lengths);

        let dir = self
            .blocks_root()
            .join(format!("db_{db_id}/tbl_{}/idx_{index_id}/seg_0", meta.id));
        std::fs::create_dir_all(&dir).map_err(|e| Error::Io {
            path: dir.clone(),
            source: e,
        })?;
        let base = cpp_format::fulltext_base_name(0, None);
        for (name, bytes) in [
            (format!("{base}.dic"), &dic),
            (format!("{base}.pos"), &pos),
            (format!("{base}.len"), &len),
        ] {
            let path = dir.join(&name);
            std::fs::write(&path, bytes).map_err(|e| Error::Io { path, source: e })?;
        }
        Ok(rows.len())
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
        centroid_count: usize,
    ) -> Result<(usize, Vec<u8>)> {
        let vectors = self.cpp_column_vectors(table, column)?;
        if vectors.is_empty() {
            return Err(Error::Invalid(format!(
                "no vectors to index in '{table}.{column}'"
            )));
        }
        let dim = vectors[0].1.len();
        let k = centroid_count.clamp(1, vectors.len());
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

        let mut parts = vec![knn_format::IvfPlainPart::default(); k];
        for (i, (label, v)) in vectors.iter().enumerate() {
            let part = &mut parts[assign[i]];
            part.segment_offsets.push(*label);
            for x in v {
                part.vectors.extend_from_slice(&x.to_le_bytes());
            }
        }
        let header = knn_format::IvfChunkHeader {
            row_count: vectors.len() as u32,
            embedding_count: vectors.len() as u32,
            dimension: dim as u32,
            centroid_count: k as u32,
        };
        let flat: Vec<f32> = centroids.into_iter().flatten().collect();
        let chunk = knn_format::encode_ivf_plain_chunk(&header, &flat, &parts)
            .map_err(|e| Error::Invalid(format!("cannot encode IVF chunk: {e}")))?;
        Ok((vectors.len(), chunk))
    }

    /// Writes `table`'s index as a C++ IVF chunk file (`kPlain`) under
    /// `…/idx_{id}/seg_0/chunk_0.idx`, returning how many vectors were written.
    pub fn write_cpp_ivf_index(
        &self,
        table: &str,
        index_name: &str,
        centroid_count: usize,
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
        let (count, chunk) = self.cpp_ivf_chunk(&table, &column, centroid_count)?;
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
    ) -> Vec<infinity_index::ivf::IvfIndex> {
        let Some(dir) = self.cpp_hnsw_dir(table, index_name) else {
            return Vec::new();
        };
        let mut out = Vec::new();
        for (_, path) in hnsw_chunks(&dir) {
            let Ok(bytes) = std::fs::read(&path) else {
                continue;
            };
            // Plain storage only; SQ/PQ chunks are not mirrored yet.
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
            out.push(infinity_index::ivf::IvfIndex::new(
                dims,
                header.centroid_count as usize,
                flat,
                parts,
            ));
        }
        out
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

    /// Writes `table`'s PLAID index as `idx_{id}/seg_0/chunk_0.idx`.
    pub fn write_cpp_plaid_index(&self, table: &str, index_name: &str) -> Result<usize> {
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
        if elem != 6 {
            return Err(Error::Invalid(
                "PLAID only indexes TENSOR(FLOAT, dim) columns".into(),
            ));
        }
        let options = self.plaid_options(&table, index_name);
        let (count, bytes) = self.cpp_plaid_chunk(&table, &column, dims, &options)?;
        let db_id = self.database_id_of(&table);
        let dir = self
            .blocks_root()
            .join(format!("db_{db_id}/tbl_{}/idx_{index_id}/seg_0", meta.id));
        std::fs::create_dir_all(&dir).map_err(|e| Error::Io {
            path: dir.clone(),
            source: e,
        })?;
        std::fs::write(dir.join("chunk_0.idx"), &bytes).map_err(|e| Error::Io {
            path: dir.clone(),
            source: e,
        })?;
        Ok(count)
    }

    /// Reads back a mirror written by [`Self::write_cpp_plaid_index`].
    pub fn read_cpp_plaid_index(&self, table: &str, index_name: &str) -> Result<PlaidIndex> {
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
        let (_, dims, _) = tensor_shape(&type_text)
            .ok_or_else(|| Error::Invalid(format!("not a tensor column: '{type_text}'")))?;
        let options = self.plaid_options(&table, index_name);
        let db_id = self.database_id_of(&table);
        let path = self.blocks_root().join(format!(
            "db_{db_id}/tbl_{}/idx_{index_id}/seg_0/chunk_0.idx",
            meta.id
        ));
        let bytes = std::fs::read(&path)
            .map_err(|_| Error::NotFound(format!("no PLAID mirror at {}", path.display())))?;
        let (index, _) = PlaidIndex::from_bytes(0, dims as u32, options.nbits, &bytes)
            .map_err(|e| Error::Invalid(format!("bad PLAID chunk: {e}")))?;
        Ok(index)
    }
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
    let ef_construction = 200u64.max(m);
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
            .execute("CREATE INDEX iv_idx ON iv (v) USING IVF;")
            .unwrap();

        assert_eq!(engine.write_cpp_ivf_index("iv", "iv_idx", 2).unwrap(), 4);
        let (centroids, back) = engine.read_cpp_ivf_index("iv", "iv_idx").unwrap();
        assert_eq!(centroids.len(), 4); // 2 centroids * dimension 2
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
}
