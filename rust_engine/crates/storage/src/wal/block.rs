//! Reference-compatible `DataBlock` serialization for `WalCmdAppendV2`.
//!
//! Ported from `DataBlock::WriteAdv` / `ColumnVector::WriteAdv` /
//! `VectorBuffer::WriteAdv` / `RoaringBitmap<true>::WriteAdv`:
//!
//! ```text
//! DataBlock = [i32 ncols] + ncols × ColumnVector
//! ColumnVector =
//!     DataType::WriteAdv
//!   + ColumnVectorType (i8: kFlat=1 / kConstant=2 / kCompactBit=3)
//!   + i32 tail_index
//!   + fixed: tail × data_type_size bytes (kCompactBit: (tail+7)/8 bit-packed)
//!   + VectorBuffer::WriteAdv  (only var-length types: [i32 heap_len] + bytes)
//!   + NullBitmap (= RoaringBitmap<true>)::WriteAdv
//! ```
//!
//! `NullBitmap::WriteAdv` = `[u32 count_]` + `[u8 all_true]`, and only when
//! `all_true == false` a `[i32 roaring_size]` + roaring bytes follow.
//! `count_` is the vector capacity (`DEFAULT_VECTOR_SIZE`, 8192).
//!
//! The null bitmap holds the **true** (non-NULL) rows of the whole capacity,
//! not just the written ones: the reference starts from `all_true` and, on the
//! first `SetFalse`, fills `[0, count_)` and then removes the NULL rows
//! (`RoaringBitmap::TryRemoveAllTrueFlag`, `roaring_bitmap.cppm:369`).
//!
//! Fixed slot widths come from `LogicalTypeWidth` (`logical_type.cpp:183`)
//! except `Embedding`, whose width is `EmbeddingInfo::Size()` — `dim × element
//! width`, or `ceil(dim / 8)` for a `Bit` element.

use ::common::LogicalType as PortLogicalType;
use roaring::RoaringBitmap;

use super::ddl_def::{
    put_i32, put_i64, put_u8, DataType, EmbeddingDataType, LogicalType, Reader, TypeInfo,
};
use crate::data_block::{BlockValue, Column, DataBlock};

/// `DEFAULT_VECTOR_SIZE` (`default_values.cppm`).
pub const DEFAULT_VECTOR_SIZE: u32 = 8192;

/// C++ `ColumnVectorType` (`column_vector.cppm:49`, `int8_t`).
pub const VECTOR_FLAT: u8 = 1;
pub const VECTOR_CONSTANT: u8 = 2;
pub const VECTOR_COMPACT_BIT: u8 = 3;

/// `EmbeddingDataType` element width in bytes; `Bit` has no byte width.
pub fn element_width(elem: EmbeddingDataType) -> Option<usize> {
    Some(match elem {
        EmbeddingDataType::Bit => return None,
        EmbeddingDataType::Int8 | EmbeddingDataType::UInt8 => 1,
        EmbeddingDataType::Int16 | EmbeddingDataType::Float16 | EmbeddingDataType::BFloat16 => 2,
        EmbeddingDataType::Int32 | EmbeddingDataType::Float => 4,
        EmbeddingDataType::Int64 | EmbeddingDataType::Double => 8,
        EmbeddingDataType::Invalid => return None,
    })
}

/// `EmbeddingType::EmbeddingSize` (`embedding_type.h:129`).
fn embedding_width(elem: EmbeddingDataType, dimension: i32) -> Option<usize> {
    let dim = usize::try_from(dimension).ok()?;
    Some(match elem {
        EmbeddingDataType::Bit => dim.div_ceil(8),
        other => dim * element_width(other)?,
    })
}

/// A column's values, in the representation the reference writes.
#[derive(Clone, Debug, PartialEq)]
pub enum ColumnValues {
    /// `Boolean`, stored bit-packed (`kCompactBit`).
    Bools(Vec<bool>),
    TinyInts(Vec<i8>),
    SmallInts(Vec<i16>),
    /// `Integer` as well as `Date` and `Time` (all four bytes wide).
    Ints(Vec<i32>),
    I64(Vec<i64>),
    /// `Decimal`: `{i64 upper, i64 lower}` of the 128-bit scaled value.
    Decimals(Vec<[u8; 16]>),
    F32(Vec<f32>),
    F64(Vec<f64>),
    /// `Float16` / `BFloat16`, kept as their 16 raw bits.
    Half(Vec<u16>),
    /// `DateTime` / `Timestamp`: `{i32 date, i32 time}`.
    DateTimes(Vec<(i32, i32)>),
    /// `Varchar`: 16-byte slots plus the outline heap.
    Varchar {
        slots: Vec<[u8; 16]>,
        heap: Vec<u8>,
    },
    /// `Json`: `{u64 length, u64 offset}` slots plus the BSON heap.
    Json {
        slots: Vec<[u8; 16]>,
        heap: Vec<u8>,
    },
    /// `Tensor` / `TensorArray` / `MultiVector`: `{u16 count, u48 offset}`
    /// slots plus the raw element heap.
    Tensor {
        slots: Vec<u64>,
        heap: Vec<u8>,
    },
    /// `Sparse`: `{i64 nnz, i64 offset}` slots plus `indices ++ values` per row.
    Sparse {
        slots: Vec<[u8; 16]>,
        heap: Vec<u8>,
    },
    /// `Embedding`: inline bytes, `rows × dim × element width`.
    Embedding {
        data: Vec<u8>,
    },
}

impl ColumnValues {
    fn len(&self) -> usize {
        match self {
            ColumnValues::Bools(v) => v.len(),
            ColumnValues::TinyInts(v) => v.len(),
            ColumnValues::SmallInts(v) => v.len(),
            ColumnValues::Ints(v) => v.len(),
            ColumnValues::I64(v) => v.len(),
            ColumnValues::Decimals(v) => v.len(),
            ColumnValues::F32(v) => v.len(),
            ColumnValues::F64(v) => v.len(),
            ColumnValues::Half(v) => v.len(),
            ColumnValues::DateTimes(v) => v.len(),
            ColumnValues::Varchar { slots, .. } => slots.len(),
            ColumnValues::Json { slots, .. } => slots.len(),
            ColumnValues::Tensor { slots, .. } => slots.len(),
            ColumnValues::Sparse { slots, .. } => slots.len(),
            ColumnValues::Embedding { .. } => 0,
        }
    }

    /// Builds `Varchar` values, laying the outline out the way the reference
    /// does: at most 13 bytes inline, longer payloads appended to the heap.
    pub fn varchars(values: &[Option<&str>]) -> Self {
        let mut slots = Vec::with_capacity(values.len());
        let mut heap = Vec::new();
        for text in values {
            let Some(text) = text else {
                slots.push([0u8; 16]);
                continue;
            };
            let bytes = text.as_bytes();
            let mut slot = [0u8; 16];
            slot[..3].copy_from_slice(&(bytes.len() as u32).to_le_bytes()[..3]);
            if bytes.len() <= 13 {
                slot[3..3 + bytes.len()].copy_from_slice(bytes);
            } else {
                slot[3..8].copy_from_slice(&bytes[..5]);
                slot[8..16].copy_from_slice(&(heap.len() as u64).to_le_bytes());
                heap.extend_from_slice(bytes);
            }
            slots.push(slot);
        }
        ColumnValues::Varchar { slots, heap }
    }

    /// Builds `Json` values from their BSON encodings.
    pub fn jsons(values: &[Option<&[u8]>]) -> Self {
        let mut slots = Vec::with_capacity(values.len());
        let mut heap = Vec::new();
        for bson in values {
            let Some(bson) = bson else {
                slots.push([0u8; 16]);
                continue;
            };
            let mut slot = [0u8; 16];
            slot[..8].copy_from_slice(&(bson.len() as u64).to_le_bytes());
            slot[8..].copy_from_slice(&(heap.len() as u64).to_le_bytes());
            heap.extend_from_slice(bson);
            slots.push(slot);
        }
        ColumnValues::Json { slots, heap }
    }

    /// Builds `Tensor`/`MultiVector` values from `(embedding count, raw bytes)`
    /// pairs, appending the raw bytes to the shared heap.
    pub fn tensors(values: &[Option<(u16, Vec<u8>)>]) -> Self {
        let mut slots = Vec::with_capacity(values.len());
        let mut heap = Vec::new();
        for value in values {
            let Some((count, raw)) = value else {
                slots.push(0);
                continue;
            };
            // `{u16 embedding_num, u48 file_offset}`, packed little-endian.
            let offset = heap.len() as u64;
            slots.push(*count as u64 | offset << 16);
            heap.extend_from_slice(raw);
        }
        ColumnValues::Tensor { slots, heap }
    }

    /// Builds `Sparse` values from `(nnz, indices, values)` triples.
    pub fn sparses(values: &[Option<(i64, Vec<u8>, Vec<u8>)>]) -> Self {
        let mut slots = Vec::with_capacity(values.len());
        let mut heap = Vec::new();
        for value in values {
            let Some((nnz, indices, raw)) = value else {
                slots.push([0u8; 16]);
                continue;
            };
            let mut slot = [0u8; 16];
            slot[..8].copy_from_slice(&nnz.to_le_bytes());
            slot[8..].copy_from_slice(&(heap.len() as u64).to_le_bytes());
            heap.extend_from_slice(indices);
            heap.extend_from_slice(raw);
            slots.push(slot);
        }
        ColumnValues::Sparse { slots, heap }
    }

    /// The text of one `Varchar` row, for the replay path.
    pub fn varchar(&self, row: usize) -> Option<String> {
        let ColumnValues::Varchar { slots, heap } = self else {
            return None;
        };
        let slot = slots.get(row)?;
        let len = u32::from_le_bytes([slot[0], slot[1], slot[2], 0]) as usize;
        let bytes = if len <= 13 {
            slot[3..3 + len].to_vec()
        } else {
            let offset = u64::from_le_bytes(slot[8..16].try_into().ok()?) as usize;
            heap.get(offset..offset + len)?.to_vec()
        };
        String::from_utf8(bytes).ok()
    }

    /// The BSON bytes of one `Json` row.
    pub fn json(&self, row: usize) -> Option<&[u8]> {
        let ColumnValues::Json { slots, heap } = self else {
            return None;
        };
        let slot = slots.get(row)?;
        let len = u64::from_le_bytes(slot[..8].try_into().ok()?) as usize;
        let offset = u64::from_le_bytes(slot[8..].try_into().ok()?) as usize;
        heap.get(offset..offset + len)
    }
}

/// One column of the block.
#[derive(Clone, Debug, PartialEq)]
pub struct BlockColumn {
    pub data_type: DataType,
    pub vector_type: u8,
    pub values: ColumnValues,
    /// Row indices whose value is NULL. Empty means the `all_true` fast path.
    pub nulls: Vec<u32>,
    /// Row count of an `Embedding` column, whose values carry no per-row
    /// structure of their own.
    pub embedding_rows: usize,
}

impl BlockColumn {
    pub fn new(data_type: DataType, values: ColumnValues) -> Self {
        let vector_type = match values {
            ColumnValues::Bools(_) => VECTOR_COMPACT_BIT,
            _ => VECTOR_FLAT,
        };
        Self {
            data_type,
            vector_type,
            values,
            nulls: Vec::new(),
            embedding_rows: 0,
        }
    }

    /// An `Embedding` column with `rows` vectors of `data_type`'s dimension.
    pub fn embedding(data_type: DataType, rows: usize, data: Vec<u8>) -> Self {
        Self {
            data_type,
            vector_type: VECTOR_FLAT,
            values: ColumnValues::Embedding { data },
            nulls: Vec::new(),
            embedding_rows: rows,
        }
    }

    fn rows(&self) -> usize {
        match &self.values {
            ColumnValues::Embedding { data } => {
                if self.embedding_rows > 0 {
                    return self.embedding_rows;
                }
                let width = match &self.data_type.type_info {
                    TypeInfo::Embedding {
                        element_type,
                        dimension,
                    } => embedding_width(*element_type, *dimension).unwrap_or(0),
                    _ => 0,
                };
                if width == 0 {
                    0
                } else {
                    data.len() / width
                }
            }
            other => other.len(),
        }
    }

    fn write_adv(&self, out: &mut Vec<u8>) {
        self.data_type.write_adv(out);
        put_u8(out, self.vector_type);
        let rows = self.rows();
        put_i32(out, rows as i32);
        match &self.values {
            ColumnValues::Bools(v) => {
                let mut bytes = vec![0u8; rows.div_ceil(8)];
                for (row, value) in v.iter().enumerate() {
                    if *value {
                        bytes[row / 8] |= 1 << (row % 8);
                    }
                }
                out.extend_from_slice(&bytes);
            }
            ColumnValues::TinyInts(v) => {
                for value in v {
                    put_u8(out, *value as u8);
                }
            }
            ColumnValues::SmallInts(v) => {
                for value in v {
                    out.extend_from_slice(&value.to_le_bytes());
                }
            }
            ColumnValues::Ints(v) => {
                for value in v {
                    put_i32(out, *value);
                }
            }
            ColumnValues::I64(v) => {
                for value in v {
                    put_i64(out, *value);
                }
            }
            ColumnValues::Decimals(v) => {
                for slot in v {
                    out.extend_from_slice(slot);
                }
            }
            ColumnValues::F32(v) => {
                for value in v {
                    out.extend_from_slice(&value.to_le_bytes());
                }
            }
            ColumnValues::F64(v) => {
                for value in v {
                    out.extend_from_slice(&value.to_le_bytes());
                }
            }
            ColumnValues::Half(v) => {
                for value in v {
                    out.extend_from_slice(&value.to_le_bytes());
                }
            }
            ColumnValues::DateTimes(v) => {
                for (date, time) in v {
                    put_i32(out, *date);
                    put_i32(out, *time);
                }
            }
            ColumnValues::Varchar { slots, heap } => {
                for slot in slots {
                    out.extend_from_slice(slot);
                }
                put_i32(out, heap.len() as i32);
                out.extend_from_slice(heap);
            }
            ColumnValues::Json { slots, heap } => {
                for slot in slots {
                    out.extend_from_slice(slot);
                }
                put_i32(out, heap.len() as i32);
                out.extend_from_slice(heap);
            }
            ColumnValues::Tensor { slots, heap } => {
                for slot in slots {
                    out.extend_from_slice(&slot.to_le_bytes());
                }
                put_i32(out, heap.len() as i32);
                out.extend_from_slice(heap);
            }
            ColumnValues::Sparse { slots, heap } => {
                for slot in slots {
                    out.extend_from_slice(slot);
                }
                put_i32(out, heap.len() as i32);
                out.extend_from_slice(heap);
            }
            ColumnValues::Embedding { data } => out.extend_from_slice(data),
        }
        write_nulls(out, &self.nulls);
    }
}

/// `NullBitmap::WriteAdv` for a column of `rows` values.
fn write_nulls(out: &mut Vec<u8>, nulls: &[u32]) {
    put_i32(out, DEFAULT_VECTOR_SIZE as i32);
    if nulls.is_empty() {
        put_u8(out, 1);
        return;
    }
    put_u8(out, 0);
    let mut bitmap = RoaringBitmap::new();
    bitmap.insert_range(0..DEFAULT_VECTOR_SIZE);
    for row in nulls {
        bitmap.remove(*row);
    }
    let mut bytes = Vec::new();
    bitmap
        .serialize_into(&mut bytes)
        .expect("serializing a roaring bitmap into a Vec cannot fail");
    put_i32(out, bytes.len() as i32);
    out.extend_from_slice(&bytes);
}

/// `DataBlock::WriteAdv`.
pub fn encode_block(columns: &[BlockColumn]) -> Vec<u8> {
    let mut out = Vec::new();
    put_i32(&mut out, columns.len() as i32);
    for c in columns {
        c.write_adv(&mut out);
    }
    out
}

/// `DataBlock::ReadAdv`: the inverse of [`encode_block`]. Returns the columns
/// and the number of bytes consumed.
pub fn decode_block(buf: &[u8]) -> Option<(Vec<BlockColumn>, usize)> {
    let mut r = Reader::new(buf);
    let ncols = r.i32().ok()?;
    let mut cols = Vec::new();
    for _ in 0..ncols {
        let data_type = DataType::read_adv(&mut r).ok()?;
        let vector_type = r.u8().ok()?;
        let tail = r.i32().ok()? as usize;
        let values = read_values(&mut r, &data_type, vector_type, tail)?;
        // NullBitmap::ReadAdv
        let count = r.i32().ok()? as u32;
        let all_true = r.u8().ok()?;
        let mut nulls = Vec::new();
        if all_true == 0 {
            let size = r.i32().ok()? as usize;
            let bytes = r.take(size).ok()?;
            let bitmap = RoaringBitmap::deserialize_from(bytes).ok()?;
            for row in 0..count {
                if !bitmap.contains(row) {
                    nulls.push(row);
                }
            }
        }
        let is_embedding = matches!(values, ColumnValues::Embedding { .. });
        cols.push(BlockColumn {
            data_type,
            vector_type,
            values,
            nulls,
            embedding_rows: if is_embedding { tail } else { 0 },
        });
    }
    Some((cols, r.pos()))
}

fn read_values(
    r: &mut Reader<'_>,
    data_type: &DataType,
    vector_type: u8,
    tail: usize,
) -> Option<ColumnValues> {
    let bits = vector_type == VECTOR_COMPACT_BIT;
    Some(match data_type.logical_type {
        LogicalType::Boolean => {
            if bits {
                let bytes = r.take(tail.div_ceil(8)).ok()?;
                ColumnValues::Bools(
                    (0..tail)
                        .map(|row| bytes[row / 8] >> (row % 8) & 1 == 1)
                        .collect(),
                )
            } else {
                let bytes = r.take(tail).ok()?;
                ColumnValues::Bools(bytes.iter().map(|b| *b != 0).collect())
            }
        }
        LogicalType::TinyInt => {
            ColumnValues::TinyInts(r.take(tail).ok()?.iter().map(|b| *b as i8).collect())
        }
        LogicalType::SmallInt => {
            let mut v = Vec::with_capacity(tail);
            for _ in 0..tail {
                v.push(i16::from_le_bytes(r.take(2).ok()?.try_into().ok()?));
            }
            ColumnValues::SmallInts(v)
        }
        LogicalType::Integer | LogicalType::Date | LogicalType::Time => {
            let mut v = Vec::with_capacity(tail);
            for _ in 0..tail {
                v.push(r.i32().ok()?);
            }
            ColumnValues::Ints(v)
        }
        LogicalType::BigInt => {
            let mut v = Vec::with_capacity(tail);
            for _ in 0..tail {
                v.push(r.i64().ok()?);
            }
            ColumnValues::I64(v)
        }
        LogicalType::Decimal => {
            let mut v = Vec::with_capacity(tail);
            for _ in 0..tail {
                v.push(r.take(16).ok()?.try_into().ok()?);
            }
            ColumnValues::Decimals(v)
        }
        LogicalType::Float => {
            let mut v = Vec::with_capacity(tail);
            for _ in 0..tail {
                v.push(f32::from_le_bytes(r.take(4).ok()?.try_into().ok()?));
            }
            ColumnValues::F32(v)
        }
        LogicalType::Double => {
            let mut v = Vec::with_capacity(tail);
            for _ in 0..tail {
                v.push(f64::from_le_bytes(r.take(8).ok()?.try_into().ok()?));
            }
            ColumnValues::F64(v)
        }
        LogicalType::Float16 | LogicalType::BFloat16 => {
            let mut v = Vec::with_capacity(tail);
            for _ in 0..tail {
                v.push(u16::from_le_bytes(r.take(2).ok()?.try_into().ok()?));
            }
            ColumnValues::Half(v)
        }
        LogicalType::DateTime | LogicalType::Timestamp => {
            let mut v = Vec::with_capacity(tail);
            for _ in 0..tail {
                v.push((r.i32().ok()?, r.i32().ok()?));
            }
            ColumnValues::DateTimes(v)
        }
        LogicalType::Varchar => {
            let mut slots = Vec::with_capacity(tail);
            for _ in 0..tail {
                slots.push(r.take(16).ok()?.try_into().ok()?);
            }
            let heap_len = r.i32().ok()? as usize;
            let heap = r.take(heap_len).ok()?.to_vec();
            ColumnValues::Varchar { slots, heap }
        }
        LogicalType::Json => {
            let mut slots = Vec::with_capacity(tail);
            for _ in 0..tail {
                slots.push(r.take(16).ok()?.try_into().ok()?);
            }
            let heap_len = r.i32().ok()? as usize;
            let heap = r.take(heap_len).ok()?.to_vec();
            ColumnValues::Json { slots, heap }
        }
        LogicalType::Tensor | LogicalType::TensorArray | LogicalType::MultiVector => {
            let mut slots = Vec::with_capacity(tail);
            for _ in 0..tail {
                slots.push(u64::from_le_bytes(r.take(8).ok()?.try_into().ok()?));
            }
            let heap_len = r.i32().ok()? as usize;
            let heap = r.take(heap_len).ok()?.to_vec();
            ColumnValues::Tensor { slots, heap }
        }
        LogicalType::Sparse => {
            let mut slots = Vec::with_capacity(tail);
            for _ in 0..tail {
                slots.push(r.take(16).ok()?.try_into().ok()?);
            }
            let heap_len = r.i32().ok()? as usize;
            let heap = r.take(heap_len).ok()?.to_vec();
            ColumnValues::Sparse { slots, heap }
        }
        LogicalType::Embedding => {
            let width = match &data_type.type_info {
                TypeInfo::Embedding {
                    element_type,
                    dimension,
                } => embedding_width(*element_type, *dimension)?,
                _ => return None,
            };
            ColumnValues::Embedding {
                data: r.take(tail * width).ok()?.to_vec(),
            }
        }
        _ => return None,
    })
}

/// Maps a port `LogicalType` to the reference's `ddl_def::LogicalType`.
fn to_ref_type(lt: &PortLogicalType) -> Option<LogicalType> {
    Some(match lt {
        PortLogicalType::Boolean => LogicalType::Boolean,
        PortLogicalType::TinyInt => LogicalType::TinyInt,
        PortLogicalType::SmallInt => LogicalType::SmallInt,
        PortLogicalType::Integer => LogicalType::Integer,
        PortLogicalType::BigInt => LogicalType::BigInt,
        PortLogicalType::HugeInt => LogicalType::HugeInt,
        PortLogicalType::Float => LogicalType::Float,
        PortLogicalType::Double => LogicalType::Double,
        PortLogicalType::Varchar => LogicalType::Varchar,
        PortLogicalType::Date => LogicalType::Date,
        PortLogicalType::Time => LogicalType::Time,
        PortLogicalType::DateTime => LogicalType::DateTime,
        PortLogicalType::Timestamp => LogicalType::Timestamp,
        PortLogicalType::Json => LogicalType::Json,
        _ => return None,
    })
}

/// Converts a port `DataBlock` into reference `BlockColumn`s.
///
/// The port's block does not carry a column's declared parameters, so types
/// whose wire form depends on them (`Decimal`'s precision/scale, an
/// `Embedding`'s element type and dimension) are not inferred here; the caller
/// builds those columns itself. `NULL` slots are dropped from the value list
/// and recorded in [`BlockColumn::nulls`].
///
/// Returns `None` when a column has a type this encoder does not cover, so the
/// caller can fall back to the text block.
pub fn from_data_block(block: &DataBlock) -> Option<Vec<BlockColumn>> {
    from_data_block_with(block, None)
}

/// [`from_data_block`] with the declared column types supplied, which is what
/// `Decimal` and `Embedding` columns need.
pub fn from_data_block_with(
    block: &DataBlock,
    declared: Option<&[DataType]>,
) -> Option<Vec<BlockColumn>> {
    if let Some(declared) = declared {
        if declared.len() != block.column_count() {
            return None;
        }
    }
    let rows = block.row_count();
    let mut cols = Vec::with_capacity(block.column_count());
    for ci in 0..block.column_count() {
        let column = block.column(ci)?;
        let data_type = match declared {
            Some(declared) => declared[ci].clone(),
            None => DataType {
                logical_type: to_ref_type(&column.logical_type())?,
                type_info: TypeInfo::None,
            },
        };
        // `None` is a NULL row: the reference leaves its slot zeroed and
        // records the row in the null bitmap.
        let mut cells: Vec<Option<BlockValue>> = Vec::with_capacity(rows);
        let mut nulls = Vec::new();
        for row in 0..rows {
            // The port's `get_value` answers `None` for a NULL slot, not
            // `BlockValue::Null`.
            match block.get_value(ci, row) {
                None | Some(BlockValue::Null) => {
                    nulls.push(row as u32);
                    cells.push(None);
                }
                Some(value) => cells.push(Some(value)),
            }
        }
        let mut embedding_rows = 0;
        let coverage = match data_type.logical_type {
            LogicalType::Boolean => ColumnValues::Bools(collect_scalar(&cells, |v| match v {
                BlockValue::Bool(x) => Some(*x),
                _ => None,
            })?),
            LogicalType::TinyInt => ColumnValues::TinyInts(collect_scalar(&cells, |v| match v {
                BlockValue::TinyInt(x) => Some(*x),
                _ => None,
            })?),
            LogicalType::SmallInt => {
                ColumnValues::SmallInts(collect_scalar(&cells, |v| match v {
                    BlockValue::SmallInt(x) => Some(*x),
                    _ => None,
                })?)
            }
            LogicalType::Integer => ColumnValues::Ints(collect_scalar(&cells, |v| match v {
                BlockValue::Int32(x) => Some(*x),
                _ => None,
            })?),
            LogicalType::BigInt => ColumnValues::I64(collect_scalar(&cells, |v| match v {
                BlockValue::Int(x) => Some(*x),
                _ => None,
            })?),
            LogicalType::Float => ColumnValues::F32(collect_scalar(&cells, |v| match v {
                BlockValue::Float(x) => Some(*x),
                _ => None,
            })?),
            LogicalType::Double => ColumnValues::F64(collect_scalar(&cells, |v| match v {
                BlockValue::Double(x) => Some(*x),
                _ => None,
            })?),
            LogicalType::Float16 => ColumnValues::Half(collect_scalar(&cells, |v| match v {
                BlockValue::Float(x) => Some(crate::format::f32_to_f16_bits(*x)),
                _ => None,
            })?),
            LogicalType::BFloat16 => ColumnValues::Half(collect_scalar(&cells, |v| match v {
                BlockValue::Float(x) => Some((x.to_bits() >> 16) as u16),
                _ => None,
            })?),
            LogicalType::Decimal => ColumnValues::Decimals(
                cells
                    .iter()
                    .map(|cell| match cell {
                        None => Some([0u8; 16]),
                        Some(BlockValue::Decimal(x)) => Some(decimal_slot(x.value)),
                        _ => None,
                    })
                    .collect::<Option<Vec<_>>>()?,
            ),
            LogicalType::Varchar => {
                let texts: Vec<Option<&str>> = cells
                    .iter()
                    .map(|cell| match cell {
                        None => Some(None),
                        Some(BlockValue::Str(x)) => Some(Some(x.as_str())),
                        _ => None,
                    })
                    .collect::<Option<Vec<_>>>()?;
                ColumnValues::varchars(&texts)
            }
            LogicalType::Date => ColumnValues::Ints(collect_scalar(&cells, |v| match v {
                BlockValue::Date(x) => Some(x.value),
                _ => None,
            })?),
            LogicalType::Time => ColumnValues::Ints(collect_scalar(&cells, |v| match v {
                BlockValue::Time(x) => Some(x.value),
                _ => None,
            })?),
            LogicalType::DateTime => {
                ColumnValues::DateTimes(collect_scalar(&cells, |v| match v {
                    BlockValue::DateTime(x) => Some((x.date, x.time)),
                    _ => None,
                })?)
            }
            LogicalType::Timestamp => {
                ColumnValues::DateTimes(collect_scalar(&cells, |v| match v {
                    BlockValue::Timestamp(x) => Some((x.date, x.time)),
                    _ => None,
                })?)
            }
            LogicalType::Json => {
                let docs: Vec<Option<Vec<u8>>> = cells
                    .iter()
                    .map(|cell| match cell {
                        None => Some(None),
                        Some(BlockValue::Json(x)) => Some(Some(x.to_bson())),
                        _ => None,
                    })
                    .collect::<Option<Vec<_>>>()?;
                ColumnValues::jsons(
                    &docs
                        .iter()
                        .map(|d| d.as_deref())
                        .collect::<Vec<Option<&[u8]>>>(),
                )
            }
            LogicalType::Embedding => {
                embedding_rows = rows;
                let width = match &data_type.type_info {
                    TypeInfo::Embedding {
                        element_type,
                        dimension,
                    } => embedding_width(*element_type, *dimension)?,
                    _ => return None,
                };
                let mut data = Vec::with_capacity(rows * width);
                for cell in &cells {
                    match cell {
                        None => data.extend(std::iter::repeat(0u8).take(width)),
                        Some(value) => data
                            .extend_from_slice(&encode_embedding_row(value, &data_type.type_info)?),
                    }
                }
                ColumnValues::Embedding { data }
            }
            _ => return None,
        };
        cols.push(BlockColumn {
            data_type,
            vector_type: match coverage {
                ColumnValues::Bools(_) => VECTOR_COMPACT_BIT,
                _ => VECTOR_FLAT,
            },
            values: coverage,
            nulls,
            embedding_rows,
        });
    }
    Some(cols)
}

/// One column's values with NULL rows filled by the type's zero value.
fn collect_scalar<T: Copy + Default>(
    cells: &[Option<BlockValue>],
    extract: impl Fn(&BlockValue) -> Option<T>,
) -> Option<Vec<T>> {
    cells
        .iter()
        .map(|cell| match cell {
            None => Some(T::default()),
            Some(value) => extract(value),
        })
        .collect()
}

/// `DecimalType{upper, lower}` of a scaled 128-bit value.
fn decimal_slot(value: i128) -> [u8; 16] {
    let mut slot = [0u8; 16];
    slot[..8].copy_from_slice(&((value >> 64) as i64).to_le_bytes());
    slot[8..].copy_from_slice(&(value as i64).to_le_bytes());
    slot
}

/// Packs one float vector into the declared element type's raw bytes.
fn encode_embedding_row(value: &BlockValue, type_info: &TypeInfo) -> Option<Vec<u8>> {
    let TypeInfo::Embedding {
        element_type,
        dimension,
    } = type_info
    else {
        return None;
    };
    let dim = *dimension as usize;
    let mut out = Vec::new();
    {
        let BlockValue::Embedding(vector) = value else {
            return None;
        };
        if vector.len() != dim {
            return None;
        }
        match element_type {
            EmbeddingDataType::Bit => {
                let mut bytes = vec![0u8; dim.div_ceil(8)];
                for (i, x) in vector.iter().enumerate() {
                    if *x != 0.0 {
                        bytes[i / 8] |= 1 << (i % 8);
                    }
                }
                out.extend_from_slice(&bytes);
            }
            EmbeddingDataType::Float => {
                for x in vector {
                    out.extend_from_slice(&x.to_le_bytes());
                }
            }
            EmbeddingDataType::Double => {
                for x in vector {
                    out.extend_from_slice(&(*x as f64).to_le_bytes());
                }
            }
            EmbeddingDataType::Float16 => {
                for x in vector {
                    out.extend_from_slice(&crate::format::f32_to_f16_bits(*x).to_le_bytes());
                }
            }
            EmbeddingDataType::BFloat16 => {
                for x in vector {
                    out.extend_from_slice(&((x.to_bits() >> 16) as u16).to_le_bytes());
                }
            }
            EmbeddingDataType::Int8 => {
                for x in vector {
                    out.push(*x as i8 as u8);
                }
            }
            EmbeddingDataType::UInt8 => {
                for x in vector {
                    out.push(*x as u8);
                }
            }
            EmbeddingDataType::Int16 => {
                for x in vector {
                    out.extend_from_slice(&(*x as i16).to_le_bytes());
                }
            }
            EmbeddingDataType::Int32 => {
                for x in vector {
                    out.extend_from_slice(&(*x as i32).to_le_bytes());
                }
            }
            EmbeddingDataType::Int64 => {
                for x in vector {
                    out.extend_from_slice(&(*x as i64).to_le_bytes());
                }
            }
            EmbeddingDataType::Invalid => return None,
        }
    }
    Some(out)
}

/// The inverse of [`from_data_block`]: rebuild a port `DataBlock`.
///
/// A column the port's block model has no type for (`Decimal`, `Tensor`,
/// `TensorArray`, `MultiVector`, `Sparse`) becomes a text column: the replay
/// path only reads the cells as text, and the rendering mirrors what the
/// insert path stores in the KV (`fmt_num`, six decimals for a float sparse
/// value, the decimal point at the declared scale).
pub fn to_data_block(columns: &[BlockColumn]) -> Option<DataBlock> {
    let rows = columns.first().map(|c| c.rows()).unwrap_or(0);
    let mut port_columns = Vec::with_capacity(columns.len());
    for c in columns {
        let port_type = match c.data_type.logical_type {
            LogicalType::Boolean => PortLogicalType::Boolean,
            LogicalType::TinyInt => PortLogicalType::TinyInt,
            LogicalType::SmallInt => PortLogicalType::SmallInt,
            LogicalType::Integer => PortLogicalType::Integer,
            LogicalType::BigInt => PortLogicalType::BigInt,
            LogicalType::Float => PortLogicalType::Float,
            LogicalType::Double => PortLogicalType::Double,
            // The port has no half-precision column; replay renders the value
            // as text, so a float column carries it.
            LogicalType::Float16 | LogicalType::BFloat16 => PortLogicalType::Float,
            LogicalType::Varchar => PortLogicalType::Varchar,
            LogicalType::Date => PortLogicalType::Date,
            LogicalType::Time => PortLogicalType::Time,
            LogicalType::DateTime => PortLogicalType::DateTime,
            LogicalType::Timestamp => PortLogicalType::Timestamp,
            LogicalType::Json => PortLogicalType::Json,
            LogicalType::Embedding => PortLogicalType::Embedding(0),
            LogicalType::Decimal
            | LogicalType::Tensor
            | LogicalType::TensorArray
            | LogicalType::MultiVector
            | LogicalType::Sparse => PortLogicalType::Varchar,
            _ => return None,
        };
        port_columns.push(Column::new(port_type)?);
    }
    let mut block = DataBlock::new(port_columns, rows);
    for row in 0..rows {
        for (ci, c) in columns.iter().enumerate() {
            let value = if c.nulls.contains(&(row as u32)) {
                BlockValue::Null
            } else {
                value_at(c, row)?
            };
            block.append_value(ci, value).ok()?;
        }
    }
    block.finalize();
    Some(block)
}

fn value_at(column: &BlockColumn, row: usize) -> Option<BlockValue> {
    Some(match &column.values {
        ColumnValues::Bools(v) => BlockValue::Bool(*v.get(row)?),
        ColumnValues::TinyInts(v) => BlockValue::TinyInt(*v.get(row)?),
        ColumnValues::SmallInts(v) => BlockValue::SmallInt(*v.get(row)?),
        ColumnValues::Ints(v) => match column.data_type.logical_type {
            LogicalType::Date => BlockValue::Date(::common::DateT::new(*v.get(row)?)),
            LogicalType::Time => BlockValue::Time(::common::TimeT::new(*v.get(row)?)),
            _ => BlockValue::Int32(*v.get(row)?),
        },
        ColumnValues::I64(v) => BlockValue::Int(*v.get(row)?),
        ColumnValues::F32(v) => BlockValue::Float(*v.get(row)?),
        ColumnValues::F64(v) => BlockValue::Double(*v.get(row)?),
        ColumnValues::Half(v) => match column.data_type.logical_type {
            LogicalType::Float16 => BlockValue::Float(f16_to_f32(*v.get(row)?)),
            LogicalType::BFloat16 => BlockValue::Float(bf16_to_f32(*v.get(row)?)),
            _ => return None,
        },
        ColumnValues::DateTimes(v) => {
            let (date, time) = *v.get(row)?;
            match column.data_type.logical_type {
                LogicalType::Timestamp => {
                    BlockValue::Timestamp(::common::TimestampT::new(date, time))
                }
                _ => BlockValue::DateTime(::common::DateTimeT::new(date, time)),
            }
        }
        ColumnValues::Varchar { .. } => BlockValue::Str(column.values.varchar(row)?),
        ColumnValues::Json { .. } => {
            BlockValue::Json(::common::json::Json::from_bson(column.values.json(row)?)?)
        }
        ColumnValues::Embedding { data } => {
            let width = match &column.data_type.type_info {
                TypeInfo::Embedding {
                    element_type,
                    dimension,
                } => embedding_width(*element_type, *dimension)?,
                _ => return None,
            };
            let start = row * width;
            let raw = data.get(start..start + width)?;
            BlockValue::Embedding(decode_embedding(raw, &column.data_type.type_info)?)
        }
        ColumnValues::Decimals(v) => {
            let slot = v.get(row)?;
            let value = i128::from_le_bytes(*slot);
            let scale = match &column.data_type.type_info {
                TypeInfo::Decimal { scale, .. } => *scale,
                _ => 0,
            };
            BlockValue::Str(decimal_text(value, scale))
        }
        ColumnValues::Tensor { slots, heap } => {
            BlockValue::Str(tensor_text(column, *slots.get(row)?, heap)?)
        }
        ColumnValues::Sparse { slots, heap } => {
            BlockValue::Str(sparse_text(column, *slots.get(row)?, heap)?)
        }
    })
}

/// A scaled 128-bit decimal as the insert path stores it (`3.25`).
fn decimal_text(value: i128, scale: i64) -> String {
    if scale <= 0 {
        return value.to_string();
    }
    let scale = scale as u32;
    let divisor = 10u128.pow(scale);
    let magnitude = value.unsigned_abs();
    let sign = if value < 0 { "-" } else { "" };
    format!(
        "{sign}{}.{:0width$}",
        magnitude / divisor,
        magnitude % divisor,
        width = scale as usize
    )
}

/// `fmt_num` of the insert path: whole numbers lose their `.0`.
fn num_text(value: f64) -> String {
    if value.fract() == 0.0 && value.abs() < 1e15 {
        format!("{}", value as i64)
    } else {
        format!("{value}")
    }
}

/// `TensorType` / `MultiVectorType` rows as `[[..],[..]]`
/// (`TensorType::Tensor2String`), which is what the insert path stores.
fn tensor_text(column: &BlockColumn, slot: u64, heap: &[u8]) -> Option<String> {
    let TypeInfo::Embedding {
        element_type,
        dimension,
    } = &column.data_type.type_info
    else {
        return None;
    };
    let count = (slot & 0xffff) as usize;
    let offset = (slot >> 16) as usize;
    let dim = usize::try_from(*dimension).ok()?;
    let width = embedding_width(*element_type, *dimension)?;
    let raw = heap.get(offset..offset + count * dim * width)?;
    let values = decode_embedding(raw, &column.data_type.type_info)?;
    let groups: Vec<String> = values
        .chunks(dim.max(1))
        .map(|chunk| {
            let cells: Vec<String> = chunk.iter().map(|v| num_text(*v as f64)).collect();
            format!("[{}]", cells.join(","))
        })
        .collect();
    Some(format!("[{}]", groups.join(",")))
}

/// `SparseType` rows as `[i:v,i:v]` (`reshape_sparse`: six decimals for a
/// floating element, the plain number otherwise) or `[i,i]` for a bit vector.
fn sparse_text(column: &BlockColumn, slot: [u8; 16], heap: &[u8]) -> Option<String> {
    let TypeInfo::Sparse {
        data_type: value_type,
        index_type,
        ..
    } = &column.data_type.type_info
    else {
        return None;
    };
    let nnz = i64::from_le_bytes(slot[..8].try_into().ok()?) as usize;
    let offset = u64::from_le_bytes(slot[8..].try_into().ok()?) as usize;
    let index_width = element_width(*index_type)?;
    let value_width = element_width(*value_type).unwrap_or(0);
    let index_bytes = heap.get(offset..offset + nnz * index_width)?;
    let value_bytes = if *value_type == EmbeddingDataType::Bit {
        &[][..]
    } else {
        heap.get(offset + nnz * index_width..offset + nnz * index_width + nnz * value_width)?
    };
    let mut cells = Vec::with_capacity(nnz);
    for i in 0..nnz {
        let index = read_integer(&index_bytes[i * index_width..], *index_type)?;
        if *value_type == EmbeddingDataType::Bit {
            cells.push(index.to_string());
        } else {
            let raw = &value_bytes[i * value_width..];
            let value = read_float(raw, *value_type)?;
            if matches!(
                value_type,
                EmbeddingDataType::Float | EmbeddingDataType::Double
            ) {
                cells.push(format!("{index}:{value:.6}"));
            } else {
                cells.push(format!("{index}:{}", num_text(value)));
            }
        }
    }
    Some(format!("[{}]", cells.join(",")))
}

fn read_integer(raw: &[u8], kind: EmbeddingDataType) -> Option<i64> {
    Some(match kind {
        EmbeddingDataType::Int8 => *raw.first()? as i8 as i64,
        EmbeddingDataType::UInt8 => *raw.first()? as i64,
        EmbeddingDataType::Int16 => i16::from_le_bytes(raw.get(..2)?.try_into().ok()?) as i64,
        EmbeddingDataType::Int32 => i32::from_le_bytes(raw.get(..4)?.try_into().ok()?) as i64,
        EmbeddingDataType::Int64 => i64::from_le_bytes(raw.get(..8)?.try_into().ok()?),
        _ => return None,
    })
}

fn read_float(raw: &[u8], kind: EmbeddingDataType) -> Option<f64> {
    Some(match kind {
        EmbeddingDataType::Float => f32::from_le_bytes(raw.get(..4)?.try_into().ok()?) as f64,
        EmbeddingDataType::Double => f64::from_le_bytes(raw.get(..8)?.try_into().ok()?),
        EmbeddingDataType::Float16 => {
            f16_to_f32(u16::from_le_bytes(raw.get(..2)?.try_into().ok()?)) as f64
        }
        EmbeddingDataType::BFloat16 => {
            bf16_to_f32(u16::from_le_bytes(raw.get(..2)?.try_into().ok()?)) as f64
        }
        other => read_integer(raw, other)? as f64,
    })
}

/// Rebuilds a float vector from the reference's raw element bytes.
fn decode_embedding(raw: &[u8], type_info: &TypeInfo) -> Option<Vec<f32>> {
    let TypeInfo::Embedding {
        element_type,
        dimension,
    } = type_info
    else {
        return None;
    };
    let dim = *dimension as usize;
    let mut out = Vec::with_capacity(dim);
    match element_type {
        EmbeddingDataType::Bit => {
            for i in 0..dim {
                out.push((raw[i / 8] >> (i % 8) & 1) as f32);
            }
        }
        EmbeddingDataType::Float => {
            for chunk in raw.chunks_exact(4) {
                out.push(f32::from_le_bytes(chunk.try_into().ok()?));
            }
        }
        EmbeddingDataType::Double => {
            for chunk in raw.chunks_exact(8) {
                out.push(f64::from_le_bytes(chunk.try_into().ok()?) as f32);
            }
        }
        EmbeddingDataType::Float16 | EmbeddingDataType::BFloat16 => {
            for chunk in raw.chunks_exact(2) {
                let bits = u16::from_le_bytes(chunk.try_into().ok()?);
                out.push(if *element_type == EmbeddingDataType::Float16 {
                    f16_to_f32(bits)
                } else {
                    bf16_to_f32(bits)
                });
            }
        }
        _ => {
            let width = element_width(*element_type)?;
            for chunk in raw.chunks_exact(width) {
                out.push(match (*element_type, width) {
                    (EmbeddingDataType::Int8, 1) => chunk[0] as i8 as f32,
                    (EmbeddingDataType::UInt8, 1) => chunk[0] as f32,
                    (EmbeddingDataType::Int16, 2) => {
                        i16::from_le_bytes(chunk.try_into().ok()?) as f32
                    }
                    (EmbeddingDataType::Int32, 4) => {
                        i32::from_le_bytes(chunk.try_into().ok()?) as f32
                    }
                    (EmbeddingDataType::Int64, 8) => {
                        i64::from_le_bytes(chunk.try_into().ok()?) as f32
                    }
                    _ => return None,
                });
            }
        }
    }
    Some(out)
}

fn f16_to_f32(bits: u16) -> f32 {
    let sign = ((bits >> 15) & 1) as u32;
    let exp = ((bits >> 10) & 0x1f) as i32;
    let mant = (bits & 0x3ff) as u32;
    if exp == 0 {
        return f32::from_bits((sign << 31) | (mant << 13)) * 2f32.powi(-14);
    }
    if exp == 0x1f {
        return f32::from_bits((sign << 31) | 0x7f80_0000 | (mant << 13));
    }
    f32::from_bits((sign << 31) | (((exp - 15 + 127) as u32) << 23) | (mant << 13))
}

fn bf16_to_f32(bits: u16) -> f32 {
    f32::from_bits((bits as u32) << 16)
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::wal::ddl_def::SparseStoreType;

    fn hex(s: &str) -> Vec<u8> {
        (0..s.len())
            .step_by(2)
            .map(|i| u8::from_str_radix(&s[i..i + 2], 16).unwrap())
            .collect()
    }

    fn flat(logical_type: LogicalType, values: ColumnValues) -> BlockColumn {
        BlockColumn::new(
            DataType {
                logical_type,
                type_info: TypeInfo::None,
            },
            values,
        )
    }

    fn embedding_type(logical_type: LogicalType, elem: EmbeddingDataType, dim: i32) -> DataType {
        DataType {
            logical_type,
            type_info: TypeInfo::Embedding {
                element_type: elem,
                dimension: dim,
            },
        }
    }

    #[test]
    fn one_int_column_matches_the_reference_block() {
        // The block portion of the captured `Append` for `ix(id int)` with one
        // row `7` (payload offset 51..70).
        let expected = hex("01000000030101000000070000000020000001");
        let block = encode_block(&[flat(LogicalType::Integer, ColumnValues::Ints(vec![7]))]);
        assert_eq!(block, expected);
    }

    #[test]
    fn int_and_varchar_matches_the_reference_block() {
        // `ix(id int, name varchar)` with one row `(7, "aa")`: the block
        // portion of the captured payload (offset 51..101).
        let expected = hex("02000000030101000000070000000020000001\
             09010100000002000061610000000000000000000000\
             000000000020000001");
        let block = encode_block(&[
            flat(LogicalType::Integer, ColumnValues::Ints(vec![7])),
            flat(LogicalType::Varchar, ColumnValues::varchars(&[Some("aa")])),
        ]);
        assert_eq!(block, expected);
    }

    /// Each fixture is the reference's block for one row of one column, as
    /// captured from the reference `wal.log` with a one-column table per type.
    #[test]
    fn every_type_matches_the_reference_block() {
        let cases: Vec<(&str, BlockColumn)> = vec![
            (
                "01000000000301000000010020000001",
                flat(LogicalType::Boolean, ColumnValues::Bools(vec![true])),
            ),
            (
                "01000000010101000000010020000001",
                flat(LogicalType::TinyInt, ColumnValues::TinyInts(vec![1])),
            ),
            (
                "0100000002010100000002000020000001",
                flat(LogicalType::SmallInt, ColumnValues::SmallInts(vec![2])),
            ),
            (
                "0100000004010100000004000000000000000020000001",
                flat(LogicalType::BigInt, ColumnValues::I64(vec![4])),
            ),
            (
                "010000000701010000000000c03f0020000001",
                flat(LogicalType::Float, ColumnValues::F32(vec![1.5])),
            ),
            (
                "0100000008010100000000000000000004400020000001",
                flat(LogicalType::Double, ColumnValues::F64(vec![2.5])),
            ),
            (
                "01000000200101000000003e0020000001",
                flat(LogicalType::Float16, ColumnValues::Half(vec![0x3e00])),
            ),
            (
                "0100000021010100000020400020000001",
                flat(LogicalType::BFloat16, ColumnValues::Half(vec![0x4020])),
            ),
            (
                "0100000009010100000003000061626300000000000000000000000000000020000001",
                flat(
                    LogicalType::Varchar,
                    ColumnValues::varchars(&[Some("abc")]),
                ),
            ),
            (
                "010000000a01010000000c4d00000020000001",
                flat(LogicalType::Date, ColumnValues::Ints(vec![19724])),
            ),
            (
                "010000000b0101000000252b00000020000001",
                flat(LogicalType::Time, ColumnValues::Ints(vec![11045])),
            ),
            (
                "010000000c01010000000c4d0000252b00000020000001",
                flat(
                    LogicalType::DateTime,
                    ColumnValues::DateTimes(vec![(19724, 11045)]),
                ),
            ),
            (
                "010000000d01010000000c4d0000252b00000020000001",
                flat(
                    LogicalType::Timestamp,
                    ColumnValues::DateTimes(vec![(19724, 11045)]),
                ),
            ),
            (
                "010000002301010000001700000000000000000000000000000017000000170000000364617461000c0000001061000100000000000020000001",
                flat(
                    LogicalType::Json,
                    ColumnValues::jsons(&[Some(&hex(
                        "170000000364617461000c000000106100010000000000",
                    )[..])]),
                ),
            ),
            (
                "0100000017050400000001010000000000803f0000004000004040000080400020000001",
                BlockColumn::embedding(
                    embedding_type(LogicalType::Embedding, EmbeddingDataType::Float, 4),
                    1,
                    hex("0000803f000000400000404000008040"),
                ),
            ),
            (
                "010000001c050400000001010000000100000000000000100000000000803f0000004000004040000080400020000001",
                BlockColumn::new(
                    embedding_type(LogicalType::Tensor, EmbeddingDataType::Float, 4),
                    ColumnValues::tensors(&[Some((
                        1,
                        hex("0000803f000000400000404000008040"),
                    ))]),
                ),
            ),
            (
                "0100000022050200000001010000000200000000000000100000000000803f0000004000004040000080400020000001",
                BlockColumn::new(
                    embedding_type(LogicalType::MultiVector, EmbeddingDataType::Float, 2),
                    ColumnValues::tensors(&[Some((
                        2,
                        hex("0000803f000000400000404000008040"),
                    ))]),
                ),
            ),
            (
                "010000001e05030800000000000000000101000000020000000000000000000000000000001000000001000000030000000000003f0000803e0020000001",
                BlockColumn::new(
                    DataType {
                        logical_type: LogicalType::Sparse,
                        type_info: TypeInfo::Sparse {
                            data_type: EmbeddingDataType::Float,
                            index_type: EmbeddingDataType::Int32,
                            dimension: 8,
                            store_type: SparseStoreType::Sort,
                        },
                    },
                    ColumnValues::sparses(&[Some((
                        2,
                        hex("0100000003000000"),
                        hex("0000003f0000803e"),
                    ))]),
                ),
            ),
        ];
        for (expected, column) in cases {
            assert_eq!(
                encode_block(&[column.clone()]),
                hex(expected),
                "{} column",
                column.data_type.logical_type as i8
            );
        }
    }

    #[test]
    fn the_null_bitmap_matches_the_reference_block() {
        // `c_null(int32)` with one NULL row: on a fresh process the reference
        // leaves the fixed slot zero and writes the bitmap of rows 1..=8191.
        let expected =
            hex("010000000301010000000000000000200000000f0000003b300000010000fe1f01000100fe1f");
        let mut column = flat(LogicalType::Integer, ColumnValues::Ints(vec![0]));
        column.nulls = vec![0];
        assert_eq!(encode_block(&[column]), expected);
    }

    #[test]
    fn decodes_the_reference_block_bytes() {
        // The block portion (offset 51..101) of the captured reference
        // `Append` for `ix(id int, name varchar)` with one row `(7, "aa")`.
        let bytes = hex("02000000030101000000070000000020000001\
             09010100000002000061610000000000000000000000\
             000000000020000001");
        let (cols, used) = decode_block(&bytes).unwrap();
        assert_eq!(used, bytes.len());
        assert_eq!(cols.len(), 2);
        assert_eq!(cols[0].values, ColumnValues::Ints(vec![7]));
        assert_eq!(cols[1].values.varchar(0).as_deref(), Some("aa"));
    }

    #[test]
    fn blocks_roundtrip_through_read_adv() {
        let cols = vec![
            flat(LogicalType::Integer, ColumnValues::Ints(vec![7])),
            flat(
                LogicalType::Varchar,
                ColumnValues::varchars(&[
                    Some("aa"),
                    Some("this string is definitely longer than thirteen bytes"),
                ]),
            ),
        ];
        let bytes = encode_block(&cols);
        let (decoded, used) = decode_block(&bytes).unwrap();
        assert_eq!(used, bytes.len());
        assert_eq!(decoded, cols);
    }

    #[test]
    fn every_type_roundtrips_through_the_codec() {
        let cols = vec![
            flat(
                LogicalType::Boolean,
                ColumnValues::Bools(vec![true, false, true]),
            ),
            flat(LogicalType::TinyInt, ColumnValues::TinyInts(vec![-1, 2])),
            flat(LogicalType::SmallInt, ColumnValues::SmallInts(vec![-3, 4])),
            flat(LogicalType::Integer, ColumnValues::Ints(vec![-5, 6])),
            flat(LogicalType::BigInt, ColumnValues::I64(vec![-7, 8])),
            flat(LogicalType::Float, ColumnValues::F32(vec![-1.5, 2.25])),
            flat(LogicalType::Double, ColumnValues::F64(vec![-2.5, 3.125])),
            flat(
                LogicalType::Float16,
                ColumnValues::Half(vec![0x3e00, 0x4000]),
            ),
            flat(
                LogicalType::BFloat16,
                ColumnValues::Half(vec![0x4020, 0x4040]),
            ),
            flat(LogicalType::Date, ColumnValues::Ints(vec![1, 2])),
            flat(LogicalType::Time, ColumnValues::Ints(vec![3, 4])),
            flat(
                LogicalType::DateTime,
                ColumnValues::DateTimes(vec![(1, 2), (3, 4)]),
            ),
            flat(
                LogicalType::Timestamp,
                ColumnValues::DateTimes(vec![(5, 6), (7, 8)]),
            ),
            flat(
                LogicalType::Varchar,
                ColumnValues::varchars(&[Some("aa"), Some("a string longer than thirteen")]),
            ),
            flat(
                LogicalType::Json,
                ColumnValues::jsons(&[Some(&hex("0500000000")[..]), None]),
            ),
            BlockColumn::embedding(
                embedding_type(LogicalType::Embedding, EmbeddingDataType::Float, 2),
                1,
                hex("0000803f00000040"),
            ),
            BlockColumn::new(
                embedding_type(LogicalType::Tensor, EmbeddingDataType::Float, 2),
                ColumnValues::tensors(&[Some((1, hex("0000803f00000040")))]),
            ),
            BlockColumn::new(
                DataType {
                    logical_type: LogicalType::Sparse,
                    type_info: TypeInfo::Sparse {
                        data_type: EmbeddingDataType::Float,
                        index_type: EmbeddingDataType::Int32,
                        dimension: 8,
                        store_type: SparseStoreType::Sort,
                    },
                },
                ColumnValues::sparses(&[Some((1, hex("01000000"), hex("0000003f")))]),
            ),
        ];
        let bytes = encode_block(&cols);
        let (decoded, used) = decode_block(&bytes).unwrap();
        assert_eq!(used, bytes.len());
        assert_eq!(decoded, cols);
    }

    #[test]
    fn the_null_bitmap_roundtrips() {
        let mut column = flat(LogicalType::Integer, ColumnValues::Ints(vec![7, 8, 9]));
        column.nulls = vec![1];
        let bytes = encode_block(&[column.clone()]);
        let (decoded, _) = decode_block(&bytes).unwrap();
        assert_eq!(decoded[0].nulls, vec![1]);
        assert_eq!(decoded[0].values, column.values);
    }

    #[test]
    fn port_block_roundtrips_through_the_reference_codec() {
        let mut block = DataBlock::new(
            vec![
                Column::new(PortLogicalType::Integer).unwrap(),
                Column::new(PortLogicalType::Varchar).unwrap(),
            ],
            2,
        );
        block.append_value(0, BlockValue::Int32(7)).unwrap();
        block.append_value(1, BlockValue::Str("aa".into())).unwrap();
        block.append_value(0, BlockValue::Int32(8)).unwrap();
        block.append_value(1, BlockValue::Str("bb".into())).unwrap();
        block.finalize();
        let cols = from_data_block(&block).unwrap();
        let bytes = encode_block(&cols);
        let (decoded, _) = decode_block(&bytes).unwrap();
        let back = to_data_block(&decoded).unwrap();
        assert_eq!(back.row_count(), 2);
        assert_eq!(back.get_value(0, 0), Some(BlockValue::Int32(7)));
        assert_eq!(back.get_value(1, 1), Some(BlockValue::Str("bb".into())));
    }
}
