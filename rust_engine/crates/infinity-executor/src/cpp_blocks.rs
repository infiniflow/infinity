//! Persist a table's rows in the **C++ block layout** (RUST_REWRITE_PLAN.md
//! §2.6.1, task #68).
//!
//! `write_cpp_blocks` mirrors every row of a table into
//! `{data_dir}/blocks/db_{db}/tbl_{table}/seg_{seg}/blk_{blk}/` using the byte
//! layouts frozen in `rust_engine/docs/CPP_FORMAT_SPEC.md` §1/§8 — the same
//! files the C++ engine opens. `read_cpp_blocks` reads them back **without**
//! touching the KV store, which is how the round-trip tests prove the mirror is
//! self-sufficient.
//!
//! The mirror is additive: queries still run off the engine's own row store, so
//! turning it on cannot change SQL results.

use std::path::PathBuf;

use infinity_common::{Error, Result};
use infinity_storage::cpp_format::{
    self, date_to_days, datetime_to_pair, element_width, embedding_column_layout,
    pack_column_with_layout, push_embedding_element, read_embedding_element, time_to_secs,
    BlockColumn, BlockId, BlockVersionFile, ColumnLayout, DEFAULT_BLOCK_CAPACITY,
};

use crate::Engine;

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
        return None;
    } else {
        6 // Float32
    })
}

/// `EMBEDDING(<elem>,<dim>)` / `VECTOR(...)` → `(dimension, ElementType code)`.
///
/// Bit elements are rejected (`None`) because their rows are bit-packed, which
/// this writer does not cover yet.
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
/// `TENSORARRAY` is excluded: its outline body is a concatenation of tensors
/// whose per-tensor length is not recorded in the slot (§8.3 records only a
/// total count), so the split point is ambiguous. Bit elements are excluded for
/// the same reason as embeddings.
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
    Some((column_type, dim, code))
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
    for v in &values {
        cpp_format::push_embedding_element(outline, elem, *v);
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
    let elem_width = element_width(elem);
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
        let v =
            cpp_format::read_embedding_element(outline.get(val_at..val_at + elem_width)?, elem)?;
        val_at += elem_width;
        parts.push(format!("{}:{}", i as i64, v));
    }
    Some(format!("[{}]", parts.join(",")))
}

/// `ARRAY(<scalar>)` → the element's `ColumnType`.
///
/// Only one level of fixed-width scalar elements is supported: the slot records
/// just the outer element count (§8.3), so a nested array's inner counts or a
/// variable-length element's payload offsets would be unrecoverable. Those
/// return `None` and are reported as unsupported by the caller.
pub fn array_shape(type_text: &str) -> Option<i32> {
    let up = type_text.trim().to_ascii_uppercase();
    let rest = up.strip_prefix("ARRAY(")?;
    let inner = rest.trim_end_matches(')').trim();
    if inner.starts_with("ARRAY(") {
        return None; // nested arrays: the inner counts are not recorded
    }
    let elem = column_type_of(inner)?;
    // Variable-length or nested elements are out of scope for this writer.
    match cpp_format::column_layout(elem) {
        Some(ColumnLayout::Fixed(_)) | Some(ColumnLayout::CompactBit) => Some(elem),
        _ => None,
    }
}

/// Width of one array element slot.
fn array_element_width(elem_col: i32) -> Option<usize> {
    match cpp_format::column_layout(elem_col)? {
        ColumnLayout::Fixed(w) => Some(w),
        ColumnLayout::CompactBit => Some(1),
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
    let elem_col = array_shape(type_text)
        .ok_or_else(|| Error::Invalid(format!("not a supported array type: '{type_text}'")))?;
    let width = array_element_width(elem_col)
        .ok_or_else(|| Error::Invalid("array element is not fixed-width".into()))?;
    let elems = parse_braces(value);
    let offset = outline.len() as u64;
    for e in &elems {
        match encode_value("", elem_col, e)? {
            Some(bytes) => outline.extend_from_slice(&bytes),
            // A NULL element is a zeroed slot (the element bitmap lives in the
            // inner column, which the C++ reader supplies for nested arrays).
            None => outline.extend_from_slice(&vec![0u8; width]),
        }
    }
    Ok(Some(
        cpp_format::varlen_count_offset_slot(elems.len() as u64, offset).to_vec(),
    ))
}

/// Decodes one array slot out of the outline.
fn decode_array_value(type_text: &str, slot: &[u8], outline: &[u8]) -> Option<String> {
    let elem_col = array_shape(type_text)?;
    let word = u64::from_le_bytes(slot.try_into().ok()?);
    let count = (word & 0xffff) as usize;
    let offset = (word >> 16) as usize;
    let width = array_element_width(elem_col)?;
    let tail = outline.get(offset..offset + count * width)?;
    let mut parts = Vec::with_capacity(count);
    for k in 0..count {
        parts.push(decode_value("", elem_col, tail, k)?);
    }
    Some(format!("{{{}}}", parts.join(",")))
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
    Sparse,
    Array,
}

fn outline_kind(type_text: &str) -> OutlineKind {
    if tensor_shape(type_text).is_some() {
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
        return Some(embedding_column_layout(dim, element_width(elem)));
    }
    if tensor_shape(type_text).is_some() {
        // Tensor / MultiVector: a `{count:16; offset:48}` slot into the outline.
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
            elems.push(cpp_format::read_embedding_element(s, elem)?.to_string());
        }
        parts.push(format!("[{}]", elems.join(",")));
    }
    Some(format!("[{}]", parts.join(",")))
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
        // `[a,b,c]` → `dim` elements, zero-padded/truncated, raw little-endian.
        let nums: Vec<f64> = value
            .trim()
            .trim_start_matches('[')
            .trim_end_matches(']')
            .split(',')
            .filter_map(|x| x.trim().parse::<f64>().ok())
            .collect();
        let mut bytes = Vec::with_capacity(dim * element_width(elem));
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
fn decode_value(type_text: &str, column_type: i32, body: &[u8], row: usize) -> Option<String> {
    if let Some((dim, elem)) = embedding_shape(type_text) {
        // Inline `dim` elements per row.
        let width = element_width(elem);
        let at = row * dim * width;
        let mut parts = Vec::with_capacity(dim);
        for k in 0..dim {
            let s = body.get(at + k * width..at + (k + 1) * width)?;
            parts.push(read_embedding_element(s, elem)?.to_string());
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
                5 => f32::from_le_bytes(slot.try_into().unwrap()).to_string(),
                6 => f64::from_le_bytes(slot.try_into().unwrap()).to_string(),
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
            // Varchar/Json: word0 = 24-bit length | 5 prefix bytes,
            // word1 = outline offset.
            let at = row * 16;
            let w0 = u64::from_le_bytes(body.get(at..at + 8)?.try_into().ok()?);
            let len = (w0 & 0x00ff_ffff) as usize;
            if len <= 13 {
                // Inline payload: the first `len` of the 13 inline bytes.
                let inline = &body.get(at + 3..at + 16)?[..len];
                Some(String::from_utf8_lossy(inline).to_string())
            } else {
                // Longer payloads live in the outline; the reader needs the
                // outline file, so report the length instead of guessing.
                Some(format!("<outline:{}>", len))
            }
        }
        ColumnLayout::VarLen(_) => None,
    }
}

impl Engine {
    /// Root of the C++-format block mirror.
    fn blocks_root(&self) -> PathBuf {
        self.dir.join("blocks")
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

    /// Writes `table`'s rows into `{data_dir}/blocks/db_{db}/tbl_{id}/seg_0/…`
    /// in the C++ block layout, returning the number of rows written.
    ///
    /// Rows are chunked `DEFAULT_BLOCK_CAPACITY` (8192) per block, as C++ does.
    pub fn write_cpp_blocks(&self, table: &str) -> Result<usize> {
        let table = self.qualified_table(table);
        let meta = self
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
        let db_id = self.database_id_of(&table);
        let root = self.blocks_root();
        // Drop any previous mirror of this table's first segment so a rewrite
        // cannot leave stale blocks behind.
        let seg_dir = root.join(format!("db_{db_id}/tbl_{}/seg_0", meta.id));
        let _ = std::fs::remove_dir_all(&seg_dir);

        let mut written = 0usize;
        for (block_index, chunk) in rows.chunks(DEFAULT_BLOCK_CAPACITY).enumerate() {
            let mut block_columns = Vec::with_capacity(types.len());
            for (col, column_type) in types.iter().enumerate() {
                let kind = outline_kind(&meta.types[col]);
                let mut slots: Vec<Option<Vec<u8>>> = Vec::with_capacity(chunk.len());
                // Self-outlining columns build their own outline; the generic
                // packer produces one for varchar/json.
                let mut self_outline = Vec::new();
                for (_, row) in chunk {
                    let text = row.get(col).map(|v| v.to_slt_string()).unwrap_or_default();
                    match kind {
                        OutlineKind::Tensor => slots.push(encode_tensor_slot(
                            &meta.types[col],
                            &text,
                            &mut self_outline,
                        )?),
                        OutlineKind::Sparse => slots.push(encode_sparse_slot(
                            &meta.types[col],
                            &text,
                            &mut self_outline,
                        )?),
                        OutlineKind::Array => slots.push(encode_array_slot(
                            &meta.types[col],
                            &text,
                            &mut self_outline,
                        )?),
                        OutlineKind::None => {
                            slots.push(encode_value(&meta.types[col], *column_type, &text)?)
                        }
                    }
                }
                let bytes: Vec<Option<&[u8]>> = slots.iter().map(|s| s.as_deref()).collect();
                let (body, packed_outline) = pack_column_with_layout(
                    *column_type,
                    layouts[col],
                    &bytes,
                    DEFAULT_BLOCK_CAPACITY,
                )
                .map_err(|e| Error::Storage(format!("pack column {col}: {e}")))?;
                let outline = if kind == OutlineKind::None {
                    packed_outline
                } else {
                    self_outline
                };
                block_columns.push(BlockColumn {
                    column_id: col as u64,
                    body,
                    outline,
                });
            }
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
            cpp_format::write_block(&root, id, &block_columns, &version).map_err(|e| {
                Error::Io {
                    path: root.join(cpp_format::block_dir(db_id, meta.id, 0, id.block_id)),
                    source: e,
                }
            })?;
            written += chunk.len();
        }
        Ok(written)
    }

    /// Mirrors every table into the C++ block layout.
    ///
    /// Called from [`Engine::flush`], so a normal clean shutdown leaves the data
    /// directory readable by the C++ engine. Tables whose columns are not yet
    /// packable (array/tensor/sparse/embedding) are skipped rather than
    /// mis-encoded.
    pub fn flush_cpp_blocks(&self) -> Result<()> {
        for meta in self.catalog.list_tables()? {
            if meta.types.iter().all(|t| column_type_of(t).is_some()) {
                self.write_cpp_blocks(&meta.name)?;
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
        for meta in self.catalog.list_tables()? {
            if !self.fetch_rows_with_ids(&meta.name)?.is_empty() {
                continue;
            }
            if self.read_cpp_blocks(&meta.name).is_ok() {
                self.load_cpp_blocks(&meta.name)?;
            }
        }
        Ok(())
    }

    /// Reads the block mirror back, returning `(columns, rows)`.
    ///
    /// This never consults the KV store, so a successful read proves the mirror
    /// carries the table on its own.
    pub fn read_cpp_blocks(&self, table: &str) -> Result<(Vec<String>, Vec<Vec<String>>)> {
        let table = self.qualified_table(table);
        let meta = self
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
            let (version, columns) = match cpp_format::read_block(&root, id) {
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
                    let Some(raw) = columns.iter().find(|c| c.column_id == col as u64) else {
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
                            OutlineKind::None => {
                                decode_value(&meta.types[col], *column_type, &body.data, row)
                                    .unwrap_or_else(|| "null".into())
                            }
                        }
                    };
                    out.push(value);
                }
                rows.push(out);
            }
        }
        Ok((meta.columns.clone(), rows))
    }
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
        // Bit embeddings and tensor arrays are not packable yet.
        assert_eq!(column_type_of("EMBEDDING(BIT,8)"), None);
        assert_eq!(column_type_of("TENSORARRAY(FLOAT,4)"), None);
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
        let db_dir = dir.path().join("blocks").join("db_0");
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
    fn array_shape_parsing() {
        assert_eq!(array_shape("ARRAY(INT)"), Some(3));
        assert_eq!(array_shape("ARRAY(DOUBLE)"), Some(6));
        // Nested arrays and variable-length elements are out of scope: the slot
        // records only the outer count.
        assert_eq!(array_shape("ARRAY(ARRAY(INT))"), None);
        assert_eq!(array_shape("ARRAY(VARCHAR)"), None);
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
        let db_dir = dir.path().join("blocks").join("db_0");
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
    fn blocks_reject_unsupported_column_types() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute("CREATE TABLE v (id INT, ta TENSORARRAY(FLOAT,2));")
            .unwrap();
        let err = engine.write_cpp_blocks("v").unwrap_err();
        assert!(err.to_string().contains("not supported"), "{err}");
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
        assert_eq!(embedding_shape("EMBEDDING(BIT,8)"), None);
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
