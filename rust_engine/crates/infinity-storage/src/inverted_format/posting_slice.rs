//! Posting-buffer / byte-slice / skip-list primitives.
//!
//! Port of `src/storage/invertedindex/format/{posting_buffer,posting_byte_slice,
//! posting_byte_slice_reader,skiplist_writer,skiplist_reader}*` and the tests
//! `src/unit_test/storage/invertedindex/format/*`.
//!
//! The byte framing matches the C++ `IntEncoder` dispatch:
//!   * `IntEncoder<u32, SIMDBitPacking>` writes a `u8` word count followed by the
//!     `CompositeCodec<SIMDBinaryPacking, VariableByte>` payload.
//!   * `IntEncoder<u16, NewPForDeltaCompressor>` writes the self-describing NPF
//!     payload with no length prefix.

use std::sync::Arc;

use super::{
    decode_u16_block, decode_u32_block_all, encode_u16_block, encode_u32_block, read_vint,
    write_vint,
};
use crate::cpp_format::FormatError;

/// `MAX_DOC_PER_RECORD` / `MAX_POS_PER_RECORD` (`index_defines.cppm:43,46`).
pub const MAX_DOC_PER_RECORD: usize = 128;
/// `MAX_DOC_PER_RECORD_BIT_NUM` (`index_defines.cppm:44`).
pub const MAX_DOC_PER_RECORD_BIT_NUM: u32 = 7;
/// `SKIP_LIST_BUFFER_SIZE` (`index_defines.cppm:53`).
pub const SKIP_LIST_BUFFER_SIZE: usize = 32;

/// The element width of one posting field.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum FieldType {
    U32,
    U16,
}

impl FieldType {
    pub fn width(self) -> usize {
        match self {
            FieldType::U32 => 4,
            FieldType::U16 => 2,
        }
    }
}

/// One row of a posting format (`PostingField`, `posting_field.cppm:16-32`).
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct PostingField {
    pub location: usize,
    pub offset: usize,
    pub ty: FieldType,
}

/// An ordered set of rows (`PostingFields`, `posting_field.cppm:109-131`).
#[derive(Clone, Debug, Default, PartialEq, Eq)]
pub struct PostingFields {
    pub fields: Vec<PostingField>,
}

impl PostingFields {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn add(&mut self, field: PostingField) {
        self.fields.push(field);
    }

    pub fn len(&self) -> usize {
        self.fields.len()
    }

    pub fn is_empty(&self) -> bool {
        self.fields.is_empty()
    }

    pub fn get(&self, index: usize) -> &PostingField {
        &self.fields[index]
    }

    pub fn total_size(&self) -> usize {
        self.fields.iter().map(|field| field.ty.width()).sum()
    }
}

/// Build a uniform format of `rows` `u32` rows with contiguous offsets.
pub fn u32_fields(rows: usize) -> PostingFields {
    let mut fields = PostingFields::new();
    for i in 0..rows {
        fields.add(PostingField {
            location: i,
            offset: i * 4,
            ty: FieldType::U32,
        });
    }
    fields
}

#[derive(Clone, Debug)]
enum RowData {
    U32(Vec<u32>),
    U16(Vec<u16>),
}

#[derive(Clone, Debug)]
struct Row {
    ty: FieldType,
    slots: RowData,
}

impl Row {
    fn new(ty: FieldType) -> Self {
        let slots = match ty {
            FieldType::U32 => RowData::U32(Vec::new()),
            FieldType::U16 => RowData::U16(Vec::new()),
        };
        Self { ty, slots }
    }

    fn resize(&mut self, capacity: usize) {
        match &mut self.slots {
            RowData::U32(v) => v.resize(capacity, 0),
            RowData::U16(v) => v.resize(capacity, 0),
        }
    }

    fn write_u32(&mut self, at: usize, value: u32) {
        match &mut self.slots {
            RowData::U32(v) => v[at] = value,
            RowData::U16(_) => panic!("push_back_u32 on a u16 row"),
        }
    }

    fn write_u16(&mut self, at: usize, value: u16) {
        match &mut self.slots {
            RowData::U16(v) => v[at] = value,
            RowData::U32(_) => panic!("push_back_u16 on a u32 row"),
        }
    }
}

/// Compact column-major posting storage (`PostingBuffer`,
/// `posting_buffer.cppm:17-79`): `rows` rows of `capacity` slots, where each row
/// holds values of its own width. Growth is `0 -> 2 -> 16 -> 128`.
#[derive(Clone, Debug)]
pub struct PostingBuffer {
    rows: Vec<Row>,
    fields: PostingFields,
    capacity: u8,
    size: u8,
}

impl Default for PostingBuffer {
    fn default() -> Self {
        Self::new()
    }
}

impl PostingBuffer {
    pub fn new() -> Self {
        Self {
            rows: Vec::new(),
            fields: PostingFields::new(),
            capacity: 0,
            size: 0,
        }
    }

    /// `PostingBuffer::Init`: allocate one row per field.
    pub fn init(&mut self, fields: &PostingFields) {
        self.fields = fields.clone();
        self.rows = fields.fields.iter().map(|f| Row::new(f.ty)).collect();
        self.capacity = 0;
        self.size = 0;
    }

    pub fn capacity(&self) -> u8 {
        self.capacity
    }

    pub fn size(&self) -> u8 {
        self.size
    }

    pub fn row_count(&self) -> usize {
        self.rows.len()
    }

    pub fn allocate_plan(current: u8) -> u8 {
        if current < 2 {
            2
        } else if current < 16 {
            16
        } else {
            MAX_DOC_PER_RECORD as u8
        }
    }

    fn is_full(&self) -> bool {
        self.size >= self.capacity
    }

    fn reallocate(&mut self) -> bool {
        let plan = Self::allocate_plan(self.capacity);
        if plan == self.capacity {
            return false;
        }
        self.reserve(plan);
        true
    }

    fn reserve(&mut self, required: u8) {
        if required == 0 || self.capacity >= required {
            return;
        }
        for row in &mut self.rows {
            row.resize(usize::from(required));
        }
        self.capacity = required;
    }

    fn check_row(&self, row: usize, ty: FieldType) {
        assert!(row < self.rows.len(), "row {row} out of range");
        assert_eq!(self.rows[row].ty, ty, "row {row} type mismatch");
    }

    /// `PostingBuffer::template PushBack<u32>`.
    pub fn push_back_u32(&mut self, row: usize, value: u32) -> bool {
        self.check_row(row, FieldType::U32);
        if self.is_full() && !self.reallocate() {
            return false;
        }
        self.rows[row].write_u32(usize::from(self.size), value);
        true
    }

    /// `PostingBuffer::template PushBack<u16>`.
    pub fn push_back_u16(&mut self, row: usize, value: u16) -> bool {
        self.check_row(row, FieldType::U16);
        if self.is_full() && !self.reallocate() {
            return false;
        }
        self.rows[row].write_u16(usize::from(self.size), value);
        true
    }

    /// `PostingBuffer::EndPushBack`: advance to the next column.
    pub fn end_push_back(&mut self) {
        self.size = self.size.saturating_add(1);
    }

    pub fn clear(&mut self) {
        self.size = 0;
    }

    pub fn row_u32(&self, row: usize) -> &[u32] {
        match &self.rows[row].slots {
            RowData::U32(v) => &v[..usize::from(self.size)],
            RowData::U16(_) => panic!("row_u32 on a u16 row"),
        }
    }

    pub fn row_u16(&self, row: usize) -> &[u16] {
        match &self.rows[row].slots {
            RowData::U16(v) => &v[..usize::from(self.size)],
            RowData::U32(_) => panic!("row_u16 on a u32 row"),
        }
    }

    pub fn get_size_in_bytes(&self) -> usize {
        usize::from(self.capacity) * self.fields.total_size()
    }

    /// `PostingBuffer::SnapShot`.
    pub fn snapshot_into(&self, target: &mut PostingBuffer) -> bool {
        target.clear();
        if target.row_count() != self.rows.len() {
            return false;
        }
        if self.size == 0 {
            return true;
        }
        target.reserve(self.size);
        for (dst, src) in target.rows.iter_mut().zip(&self.rows) {
            match (&mut dst.slots, &src.slots) {
                (RowData::U32(d), RowData::U32(s)) => {
                    d[..usize::from(self.size)].copy_from_slice(&s[..usize::from(self.size)]);
                }
                (RowData::U16(d), RowData::U16(s)) => {
                    d[..usize::from(self.size)].copy_from_slice(&s[..usize::from(self.size)]);
                }
                _ => panic!("snapshot row type mismatch"),
            }
        }
        target.size = self.size;
        true
    }

    /// `PostingBuffer::Dump`: `VInt size, VInt capacity, rows...` with raw
    /// little-endian values.
    pub fn dump(&self) -> Vec<u8> {
        let mut out = Vec::new();
        write_vint(&mut out, u64::from(self.size));
        write_vint(&mut out, u64::from(self.capacity));
        if self.size > 0 {
            for row in &self.rows {
                match &row.slots {
                    RowData::U32(v) => {
                        for value in &v[..usize::from(self.size)] {
                            out.extend_from_slice(&value.to_le_bytes());
                        }
                    }
                    RowData::U16(v) => {
                        for value in &v[..usize::from(self.size)] {
                            out.extend_from_slice(&value.to_le_bytes());
                        }
                    }
                }
            }
        }
        out
    }

    /// `PostingBuffer::Load`; returns the bytes consumed.
    pub fn load(&mut self, buf: &[u8]) -> Result<usize, FormatError> {
        let (size, used) = read_vint(buf)?;
        let (capacity, used2) = read_vint(&buf[used..])?;
        let mut pos = used + used2;
        self.size = size as u8;
        self.capacity = capacity as u8;
        if self.size > 0 {
            self.reserve(self.capacity);
            for (row, field) in self.rows.iter_mut().zip(&self.fields.fields) {
                let nbytes = usize::from(self.size) * field.ty.width();
                let bytes = buf.get(pos..pos + nbytes).ok_or(FormatError::Truncated {
                    need: pos + nbytes,
                    got: buf.len(),
                })?;
                match &mut row.slots {
                    RowData::U32(v) => {
                        for (slot, chunk) in v.iter_mut().take(usize::from(self.size)).zip(bytes.chunks_exact(4)) {
                            *slot = u32::from_le_bytes(chunk.try_into().unwrap());
                        }
                    }
                    RowData::U16(v) => {
                        for (slot, chunk) in v.iter_mut().take(usize::from(self.size)).zip(bytes.chunks_exact(2)) {
                            *slot = u16::from_le_bytes(chunk.try_into().unwrap());
                        }
                    }
                }
                pos += nbytes;
            }
        }
        Ok(pos)
    }
}

/// Packed flush counters (`FlushInfo`, `flush_info.cppm:13-57`): bit 0 valid,
/// bits 1..31 length, bits 32..63 count.
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub struct FlushInfo {
    pub flush_count: u32,
    pub flush_length: u32,
    pub valid_posting_buffer: bool,
}

impl FlushInfo {
    pub fn get_flush_count(&self) -> u32 {
        self.flush_count
    }

    pub fn get_flush_length(&self) -> u32 {
        self.flush_length
    }

    pub fn is_valid_posting_buffer(&self) -> bool {
        self.valid_posting_buffer
    }

    pub fn raw(&self) -> u64 {
        u64::from(self.valid_posting_buffer)
            | (u64::from(self.flush_length & 0x7fff_ffff) << 1)
            | (u64::from(self.flush_count) << 32)
    }

    pub fn from_raw(raw: u64) -> Self {
        Self {
            flush_count: (raw >> 32) as u32,
            flush_length: ((raw >> 1) & 0x7fff_ffff) as u32,
            valid_posting_buffer: raw & 1 != 0,
        }
    }
}

/// A buffer plus the flushed compressed stream (`PostingByteSlice`,
/// `posting_byte_slice.cppm:14-83`).
#[derive(Clone, Debug)]
pub struct PostingByteSlice {
    pub fields: PostingFields,
    pub buffer: PostingBuffer,
    /// The flushed byte stream. Cloned snapshots share it (`Arc::ptr_eq`), as the
    /// C++ snapshot shares the `ByteSliceList`.
    pub bytes: Arc<Vec<u8>>,
    pub flush_info: FlushInfo,
}

impl Default for PostingByteSlice {
    fn default() -> Self {
        Self::new()
    }
}

impl PostingByteSlice {
    pub fn new() -> Self {
        Self {
            fields: PostingFields::new(),
            buffer: PostingBuffer::new(),
            bytes: Arc::new(Vec::new()),
            flush_info: FlushInfo::default(),
        }
    }

    pub fn init(&mut self, fields: &PostingFields) {
        self.fields = fields.clone();
        self.buffer.init(fields);
        self.bytes = Arc::new(Vec::new());
        self.flush_info = FlushInfo::default();
    }

    pub fn push_back_u32(&mut self, row: usize, value: u32) -> bool {
        self.buffer.push_back_u32(row, value)
    }

    pub fn push_back_u16(&mut self, row: usize, value: u16) -> bool {
        self.buffer.push_back_u16(row, value)
    }

    /// `PostingByteSlice::EndPushBack`.
    pub fn end_push_back(&mut self) {
        self.flush_info.valid_posting_buffer = true;
        self.buffer.end_push_back();
    }

    pub fn need_flush(&self, threshold: usize) -> bool {
        usize::from(self.buffer.size()) == threshold
    }

    /// `PostingByteSlice::DoFlush`: encode every field's current column run.
    pub fn flush(&mut self) -> usize {
        let size = usize::from(self.buffer.size());
        if size == 0 {
            return 0;
        }
        let mut appended = Vec::new();
        for field in &self.fields.fields {
            match field.ty {
                FieldType::U32 => {
                    let values = self.buffer.row_u32(field.location);
                    let payload = encode_u32_block(values);
                    appended.push(payload.len().div_ceil(4) as u8);
                    appended.extend_from_slice(&payload);
                }
                FieldType::U16 => {
                    let values = self.buffer.row_u16(field.location);
                    appended.extend_from_slice(&encode_u16_block(values));
                }
            }
        }
        let flushed = appended.len();
        Arc::make_mut(&mut self.bytes).extend_from_slice(&appended);
        self.flush_info.flush_count = self.flush_info.flush_count.saturating_add(size as u32);
        self.flush_info.flush_length = self
            .flush_info
            .flush_length
            .saturating_add(flushed as u32);
        self.flush_info.valid_posting_buffer = false;
        self.buffer.clear();
        flushed
    }

    pub fn total_count(&self) -> usize {
        self.flush_info.get_flush_count() as usize + usize::from(self.buffer.size())
    }

    pub fn buffer_size(&self) -> usize {
        usize::from(self.buffer.size())
    }

    pub fn is_posting_buffer_valid(&self) -> bool {
        self.flush_info.is_valid_posting_buffer()
    }

    /// `PostingByteSlice::SnapShot` (shares the flushed byte stream).
    pub fn snapshot(&self) -> PostingByteSlice {
        let mut target = PostingByteSlice::new();
        target.fields = self.fields.clone();
        target.buffer.init(&self.fields);
        self.buffer.snapshot_into(&mut target.buffer);
        target.bytes = Arc::clone(&self.bytes);
        target.flush_info = self.flush_info;
        target
    }

    /// `PostingByteSlice::Dump`: without spill only the byte stream; with spill
    /// `buffer, VLong flush_info, VInt slice_size, bytes`.
    pub fn dump(&self, spill: bool) -> Vec<u8> {
        if !spill {
            return (*self.bytes).clone();
        }
        let mut out = self.buffer.dump();
        write_vint(&mut out, self.flush_info.raw());
        write_vint(&mut out, self.bytes.len() as u64);
        out.extend_from_slice(&self.bytes);
        out
    }

    /// `PostingByteSlice::Load`; returns the bytes consumed.
    pub fn load(&mut self, buf: &[u8], spill: bool) -> Result<usize, FormatError> {
        if !spill {
            self.bytes = Arc::new(buf.to_vec());
            return Ok(buf.len());
        }
        let mut pos = self.buffer.load(buf)?;
        let (raw, used) = read_vint(&buf[pos..])?;
        pos += used;
        let (len, used) = read_vint(&buf[pos..])?;
        pos += used;
        let len = len as usize;
        let bytes = buf.get(pos..pos + len).ok_or(FormatError::Truncated {
            need: pos + len,
            got: buf.len(),
        })?;
        self.flush_info = FlushInfo::from_raw(raw);
        self.bytes = Arc::new(bytes.to_vec());
        Ok(pos + len)
    }
}

/// Cursor over a [`PostingByteSlice`] (`PostingByteSliceReader`,
/// `posting_byte_slice_reader.cppm:16-101`).
#[derive(Clone, Debug, Default)]
pub struct ReaderState {
    tell: usize,
    location_cursor: usize,
    posting_buffer_cursor: usize,
}

impl ReaderState {
    pub fn open(&mut self) {
        self.tell = 0;
        self.location_cursor = 0;
        self.posting_buffer_cursor = 0;
    }

    pub fn tell(&self) -> usize {
        self.tell
    }

    /// `PostingByteSliceReader::Seek`: backward seeks are rejected and leave the
    /// cursor untouched (`ByteSliceReader::Seek` returns `BYTE_SLICE_EOF`).
    pub fn seek(&mut self, slice: &PostingByteSlice, pos: usize) -> bool {
        self.location_cursor = 0;
        self.posting_buffer_cursor = 0;
        if pos < self.tell || pos > slice.bytes.len() {
            return false;
        }
        self.tell = pos;
        true
    }

    fn is_valid_posting_buffer(&self, slice: &PostingByteSlice) -> bool {
        slice.buffer_size() > 0
            && self.posting_buffer_cursor < slice.fields.len()
            && slice.is_posting_buffer_valid()
    }

    fn advance(&mut self, slice: &PostingByteSlice) {
        self.location_cursor += 1;
        if self.location_cursor == slice.fields.len() {
            self.location_cursor = 0;
        }
    }

    /// `PostingByteSliceReader::Decode<u32>`: `None` is the C++ `false`.
    pub fn decode_u32(&mut self, slice: &PostingByteSlice, count: usize) -> Option<Vec<u32>> {
        if count == 0 {
            return None;
        }
        let flush_len = slice.flush_info.get_flush_length() as usize;
        if self.tell >= flush_len && !self.is_valid_posting_buffer(slice) {
            return None;
        }
        let field = *slice.fields.get(self.location_cursor);
        let values = if self.tell >= flush_len {
            let buffer_size = slice.buffer_size();
            if buffer_size > count {
                return None;
            }
            let values = slice.buffer.row_u32(field.location).to_vec();
            self.posting_buffer_cursor += 1;
            values
        } else {
            if field.ty != FieldType::U32 {
                panic!("decode_u32 on a u16 row");
            }
            let bytes = &slice.bytes;
            let words = usize::from(*bytes.get(self.tell)?);
            let start = self.tell + 1;
            let end = start + words * 4;
            let payload = bytes.get(start..end)?;
            let values = decode_u32_block_all(payload).ok()?;
            self.tell = end;
            values
        };
        if values.len() > count {
            return None;
        }
        self.advance(slice);
        Some(values)
    }

    /// `PostingByteSliceReader::Decode<u16>`: `None` is the C++ `false`.
    pub fn decode_u16(&mut self, slice: &PostingByteSlice, count: usize) -> Option<Vec<u16>> {
        if count == 0 {
            return None;
        }
        let flush_len = slice.flush_info.get_flush_length() as usize;
        if self.tell >= flush_len && !self.is_valid_posting_buffer(slice) {
            return None;
        }
        let field = *slice.fields.get(self.location_cursor);
        let values = if self.tell >= flush_len {
            let buffer_size = slice.buffer_size();
            if buffer_size > count {
                return None;
            }
            let values = slice.buffer.row_u16(field.location).to_vec();
            self.posting_buffer_cursor += 1;
            values
        } else {
            if field.ty != FieldType::U16 {
                panic!("decode_u16 on a u32 row");
            }
            let buf = slice.bytes.get(self.tell..)?;
            let (values, used) = decode_u16_block(buf, count).ok()?;
            self.tell += used;
            values
        };
        if values.len() > count {
            return None;
        }
        self.advance(slice);
        Some(values)
    }
}

/// The row layout of a skip list (`DocSkipListFormat` /
/// `PositionSkipListFormat`).
#[derive(Clone, Debug, PartialEq, Eq)]
pub struct SkipListFormat {
    pub fields: PostingFields,
    pub has_tf_list: bool,
    pub has_block_max: bool,
}

impl SkipListFormat {
    /// Doc skip list: `[u32 doc][u32 tf][u32 block_max_tf, u16 pct][u32 offset]`
    /// (`doc_list_format_option.cppm:72-128`).
    pub fn for_doc(has_tf_list: bool, has_block_max: bool) -> Self {
        let mut fields = PostingFields::new();
        let mut offset = 0;
        let push = |fields: &mut PostingFields, ty: FieldType, offset: &mut usize| {
            fields.add(PostingField {
                location: fields.len(),
                offset: *offset,
                ty,
            });
            *offset += ty.width();
        };
        push(&mut fields, FieldType::U32, &mut offset);
        if has_tf_list {
            push(&mut fields, FieldType::U32, &mut offset);
        }
        if has_block_max {
            push(&mut fields, FieldType::U32, &mut offset);
            push(&mut fields, FieldType::U16, &mut offset);
        }
        push(&mut fields, FieldType::U32, &mut offset);
        Self {
            fields,
            has_tf_list,
            has_block_max,
        }
    }

    /// Position skip list: `[u32 total_pos][u32 offset]`
    /// (`position_list_format_option.cppm:31-58`). The reader is built from
    /// `PositionListFormatOption`, whose `has_tf_list_` is always false, so the
    /// second row is read as the offset, not as a tf list.
    pub fn for_position() -> Self {
        let mut fields = PostingFields::new();
        fields.add(PostingField {
            location: 0,
            offset: 0,
            ty: FieldType::U32,
        });
        fields.add(PostingField {
            location: 1,
            offset: 4,
            ty: FieldType::U32,
        });
        Self {
            fields,
            has_tf_list: false,
            has_block_max: false,
        }
    }
}

/// `SkipListWriter` (`skiplist_writer.cppm:11-35`).
#[derive(Clone, Debug)]
pub struct SkipListWriter {
    slice: PostingByteSlice,
    last_key: u32,
    last_value1: u32,
}

impl Default for SkipListWriter {
    fn default() -> Self {
        Self::new()
    }
}

impl SkipListWriter {
    pub fn new() -> Self {
        Self {
            slice: PostingByteSlice::new(),
            last_key: u32::MAX,
            last_value1: u32::MAX,
        }
    }

    pub fn init(&mut self, format: &SkipListFormat) {
        self.slice.init(&format.fields);
        self.last_key = u32::MAX;
        self.last_value1 = u32::MAX;
    }

    fn after_add(&mut self) {
        self.slice.end_push_back();
        if self.slice.need_flush(SKIP_LIST_BUFFER_SIZE) {
            self.slice.flush();
        }
    }

    /// `AddItem(u32 delta_value1)`.
    pub fn add_item_1(&mut self, value_delta: u32) {
        if self.last_value1 == u32::MAX {
            self.last_value1 = 0;
        }
        self.last_value1 = self.last_value1.wrapping_add(value_delta);
        self.slice.push_back_u32(0, self.last_value1);
        self.after_add();
    }

    /// `AddItem(u32 key, u32 value1)`: the key is delta-coded, the value is not.
    pub fn add_item_2(&mut self, key: u32, value1: u32) {
        assert!(
            self.last_key == u32::MAX || key > self.last_key,
            "AddItem failed, key: {key}, last_key: {}",
            self.last_key
        );
        let last_key = if self.last_key == u32::MAX { 0 } else { self.last_key };
        self.slice.push_back_u32(0, key - last_key);
        self.slice.push_back_u32(1, value1);
        self.last_key = key;
        self.after_add();
    }

    /// `AddItem(u32 key, u32 value1, u32 value2)`: both counters delta-coded.
    pub fn add_item_3(&mut self, key: u32, value1: u32, value2: u32) {
        assert!(
            self.last_key == u32::MAX || key > self.last_key,
            "AddItem failed, key: {key}, last_key: {}",
            self.last_key
        );
        assert!(
            self.last_value1 == u32::MAX || value1 > self.last_value1,
            "AddItem failed, value1: {value1}, last_value1: {}",
            self.last_value1
        );
        let last_key = if self.last_key == u32::MAX { 0 } else { self.last_key };
        let last_value1 = if self.last_value1 == u32::MAX {
            0
        } else {
            self.last_value1
        };
        self.slice.push_back_u32(0, key - last_key);
        self.slice.push_back_u32(1, value1 - last_value1);
        self.slice.push_back_u32(2, value2);
        self.last_key = key;
        self.last_value1 = value1;
        self.after_add();
    }

    /// `AddItem(doc_id, total_tf, block_max_tf, block_max_percentage,
    /// item_size)`.
    pub fn add_item_4(
        &mut self,
        doc_id: u32,
        total_tf: u32,
        block_max_tf: u32,
        block_max_percentage: u16,
        item_size: u32,
    ) {
        assert!(
            self.last_key == u32::MAX || doc_id > self.last_key,
            "AddItem failed, last_doc_id: {doc_id}, last_key: {}",
            self.last_key
        );
        assert!(
            self.last_value1 == u32::MAX || total_tf > self.last_value1,
            "AddItem failed, total_tf: {total_tf}, last_value1: {}",
            self.last_value1
        );
        let last_key = if self.last_key == u32::MAX { 0 } else { self.last_key };
        let last_value1 = if self.last_value1 == u32::MAX {
            0
        } else {
            self.last_value1
        };
        self.slice.push_back_u32(0, doc_id - last_key);
        self.slice.push_back_u32(1, total_tf - last_value1);
        self.slice.push_back_u32(2, block_max_tf);
        self.slice.push_back_u16(3, block_max_percentage);
        self.slice.push_back_u32(4, item_size);
        self.last_key = doc_id;
        self.last_value1 = total_tf;
        self.after_add();
    }

    pub fn flush(&mut self) {
        self.slice.flush();
    }

    /// The flushed stream (C++ `GetByteSliceList()->GetTotalSize()` bytes).
    pub fn bytes(&self) -> &[u8] {
        &self.slice.bytes
    }

    pub fn slice(&self) -> &PostingByteSlice {
        &self.slice
    }
}

/// The observable result of `SkipListReader::SkipTo`.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct SkipToResult {
    pub doc_id: u32,
    pub prev_doc_id: u32,
    pub offset: u32,
    pub delta: u32,
}

enum Source {
    None,
    Bytes {
        data: Vec<u8>,
        pos: usize,
        end: usize,
    },
    Posting {
        slice: PostingByteSlice,
        state: ReaderState,
    },
}

#[derive(Default)]
struct Loaded {
    docs: Vec<u32>,
    ttf: Vec<u32>,
    block_max_tf: Vec<u32>,
    block_max_pct: Vec<u16>,
    offsets: Vec<u32>,
}

/// `SkipListReader` (`skiplist_reader.cppm:21-99`,
/// `skiplist_reader_impl.cpp:36-92`), reading either a byte range or a
/// `PostingByteSlice`.
pub struct SkipListReader {
    format: SkipListFormat,
    source: Source,
    skipped_item_count: i32,
    current_doc_id: u32,
    current_offset: u32,
    current_ttf: u32,
    current_block_max_tf: u32,
    current_block_max_tf_percentage: u16,
    prev_doc_id: u32,
    prev_offset: u32,
    prev_ttf: u32,
    current_cursor: usize,
    num_in_buffer: usize,
    doc_id_buffer: [u32; SKIP_LIST_BUFFER_SIZE],
    offset_buffer: [u32; SKIP_LIST_BUFFER_SIZE],
    ttf_buffer: [u32; SKIP_LIST_BUFFER_SIZE],
    block_max_tf_buffer: [u32; SKIP_LIST_BUFFER_SIZE],
    block_max_tf_percentage_buffer: [u16; SKIP_LIST_BUFFER_SIZE],
}

fn read_u32_field(data: &[u8], pos: &mut usize) -> Option<Vec<u32>> {
    let words = usize::from(*data.get(*pos)?);
    *pos += 1;
    let end = *pos + words * 4;
    let payload = data.get(*pos..end)?;
    let values = decode_u32_block_all(payload).ok()?;
    *pos = end;
    Some(values)
}

fn read_u16_field(data: &[u8], pos: &mut usize, count: usize) -> Option<Vec<u16>> {
    let (values, used) = decode_u16_block(data.get(*pos..)?, count).ok()?;
    *pos += used;
    Some(values)
}

impl SkipListReader {
    pub fn new(format: &SkipListFormat) -> Self {
        Self {
            format: format.clone(),
            source: Source::None,
            skipped_item_count: 0,
            current_doc_id: 0,
            current_offset: 0,
            current_ttf: 0,
            current_block_max_tf: 0,
            current_block_max_tf_percentage: 0,
            prev_doc_id: 0,
            prev_offset: 0,
            prev_ttf: 0,
            current_cursor: 0,
            num_in_buffer: 0,
            doc_id_buffer: [0; SKIP_LIST_BUFFER_SIZE],
            offset_buffer: [0; SKIP_LIST_BUFFER_SIZE],
            ttf_buffer: [0; SKIP_LIST_BUFFER_SIZE],
            block_max_tf_buffer: [0; SKIP_LIST_BUFFER_SIZE],
            block_max_tf_percentage_buffer: [0; SKIP_LIST_BUFFER_SIZE],
        }
    }

    /// `SkipListReaderByteSlice::Load`.
    pub fn load_byte_slice(&mut self, bytes: &[u8], start: usize, end: usize) {
        assert!(start <= bytes.len(), "start > byte_slice_list->GetTotalSize().");
        assert!(end <= bytes.len(), "end > byte_slice_list->GetTotalSize().");
        self.source = Source::Bytes {
            data: bytes.to_vec(),
            pos: start,
            end,
        };
    }

    /// `SkipListReaderPostingByteSlice::Load`.
    pub fn load_posting_byte_slice(&mut self, slice: &PostingByteSlice) {
        let mut state = ReaderState::default();
        state.open();
        self.source = Source::Posting {
            slice: slice.snapshot(),
            state,
        };
    }

    pub fn has_tf_list(&self) -> bool {
        self.format.has_tf_list
    }

    fn load_buffer(&mut self) -> bool {
        let has_tf = self.format.has_tf_list;
        let has_block_max = self.format.has_block_max;
        let loaded = match &mut self.source {
            Source::None => return false,
            Source::Bytes { data, pos, end } => {
                if *pos >= *end {
                    return false;
                }
                let Some(docs) = read_u32_field(data, pos) else {
                    return false;
                };
                let mut out = Loaded {
                    docs,
                    ..Loaded::default()
                };
                if has_tf {
                    let Some(ttf) = read_u32_field(data, pos) else {
                        return false;
                    };
                    out.ttf = ttf;
                }
                if has_block_max {
                    let Some(bm) = read_u32_field(data, pos) else {
                        return false;
                    };
                    let Some(pct) =
                        read_u16_field(data, pos, SKIP_LIST_BUFFER_SIZE)
                    else {
                        return false;
                    };
                    out.block_max_tf = bm;
                    out.block_max_pct = pct;
                }
                let Some(offsets) = read_u32_field(data, pos) else {
                    return false;
                };
                out.offsets = offsets;
                out
            }
            Source::Posting { slice, state } => {
                let flush_count = slice.total_count();
                let decode_count = if slice.is_posting_buffer_valid() {
                    SKIP_LIST_BUFFER_SIZE
                } else {
                    flush_count
                };
                if decode_count == 0 {
                    return false;
                }
                let Some(docs) = state.decode_u32(slice, decode_count) else {
                    return false;
                };
                let mut out = Loaded {
                    docs,
                    ..Loaded::default()
                };
                if has_tf {
                    let Some(ttf) = state.decode_u32(slice, decode_count) else {
                        return false;
                    };
                    out.ttf = ttf;
                }
                if has_block_max {
                    let Some(bm) = state.decode_u32(slice, decode_count) else {
                        return false;
                    };
                    let Some(pct) = state.decode_u16(slice, decode_count) else {
                        return false;
                    };
                    out.block_max_tf = bm;
                    out.block_max_pct = pct;
                }
                let Some(offsets) = state.decode_u32(slice, decode_count) else {
                    return false;
                };
                out.offsets = offsets;
                out
            }
        };
        let doc_num = loaded.docs.len();
        assert!(doc_num <= SKIP_LIST_BUFFER_SIZE, "skip list buffer overflow");
        assert!(
            loaded.ttf.is_empty() || loaded.ttf.len() == doc_num,
            "SKipList decode error, doc_num = {doc_num} ttf_num = {}",
            loaded.ttf.len()
        );
        assert!(
            loaded.block_max_tf.is_empty() || loaded.block_max_tf.len() == doc_num,
            "SKipList decode error, doc_num = {doc_num} block_max_tf_num = {}",
            loaded.block_max_tf.len()
        );
        assert!(
            loaded.block_max_pct.is_empty() || loaded.block_max_pct.len() == doc_num,
            "SKipList decode error, doc_num = {doc_num} block_max_tf_percentage_num = {}",
            loaded.block_max_pct.len()
        );
        assert!(
            loaded.offsets.len() == doc_num,
            "SKipList decode error, doc_num = {doc_num} offset_num = {}",
            loaded.offsets.len()
        );
        for i in 0..doc_num {
            self.doc_id_buffer[i] = loaded.docs[i];
            self.offset_buffer[i] = loaded.offsets[i];
            if has_tf {
                self.ttf_buffer[i] = loaded.ttf[i];
            }
            if has_block_max {
                self.block_max_tf_buffer[i] = loaded.block_max_tf[i];
                self.block_max_tf_percentage_buffer[i] = loaded.block_max_pct[i];
            }
        }
        self.num_in_buffer = doc_num;
        self.current_cursor = 0;
        true
    }

    /// `SkipListReader::SkipTo`.
    pub fn skip_to(&mut self, query_doc_id: u32) -> Option<SkipToResult> {
        let mut current_doc_id = self.current_doc_id;
        let mut current_offset = self.current_offset;
        let mut current_ttf = self.current_ttf;
        let mut current_cursor = self.current_cursor;
        let mut skipped_item_count = self.skipped_item_count;
        let mut num_in_buffer = self.num_in_buffer;
        loop {
            let local_prev_doc_id = current_doc_id;
            let local_prev_offset = current_offset;
            let local_prev_ttf = current_ttf;
            if current_cursor >= num_in_buffer {
                if !self.load_buffer() {
                    skipped_item_count += 1;
                    break;
                }
                current_cursor = self.current_cursor;
                num_in_buffer = self.num_in_buffer;
            }
            current_doc_id = current_doc_id.wrapping_add(self.doc_id_buffer[current_cursor]);
            current_offset = current_offset.wrapping_add(self.offset_buffer[current_cursor]);
            if self.format.has_tf_list {
                current_ttf = current_ttf.wrapping_add(self.ttf_buffer[current_cursor]);
            }
            skipped_item_count += 1;
            if current_doc_id >= query_doc_id {
                self.skipped_item_count = skipped_item_count;
                self.current_doc_id = current_doc_id;
                self.current_offset = current_offset;
                self.current_ttf = current_ttf;
                if self.format.has_block_max {
                    self.current_block_max_tf = self.block_max_tf_buffer[current_cursor];
                    self.current_block_max_tf_percentage =
                        self.block_max_tf_percentage_buffer[current_cursor];
                }
                self.prev_doc_id = local_prev_doc_id;
                self.prev_offset = local_prev_offset;
                self.prev_ttf = local_prev_ttf;
                let delta = self.offset_buffer[current_cursor];
                self.current_cursor = current_cursor + 1;
                return Some(SkipToResult {
                    doc_id: current_doc_id,
                    prev_doc_id: local_prev_doc_id,
                    offset: local_prev_offset,
                    delta,
                });
            }
            current_cursor += 1;
        }
        self.skipped_item_count = skipped_item_count;
        self.current_doc_id = current_doc_id;
        self.current_offset = current_offset;
        self.current_ttf = current_ttf;
        self.current_cursor = current_cursor;
        None
    }

    pub fn get_last_value_in_buffer(&self) -> u32 {
        let mut last = self.current_offset;
        for cursor in self.current_cursor..self.num_in_buffer {
            last = last.wrapping_add(self.offset_buffer[cursor]);
        }
        last
    }

    pub fn get_last_key_in_buffer(&self) -> u32 {
        let mut last = self.current_doc_id;
        for cursor in self.current_cursor..self.num_in_buffer {
            last = last.wrapping_add(self.doc_id_buffer[cursor]);
        }
        last
    }

    pub fn get_skipped_item_count(&self) -> i32 {
        std::cmp::max(self.skipped_item_count - 1, 0)
    }

    pub fn get_current_ttf(&self) -> u32 {
        self.current_ttf
    }

    pub fn get_prev_ttf(&self) -> u32 {
        self.prev_ttf
    }

    pub fn get_prev_key(&self) -> u32 {
        self.prev_doc_id
    }

    pub fn get_current_key(&self) -> u32 {
        self.current_doc_id
    }

    pub fn get_block_max_info(&self) -> (u32, u16) {
        (
            self.current_block_max_tf,
            self.current_block_max_tf_percentage,
        )
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    struct Rng(u64);

    impl Rng {
        fn new(seed: u64) -> Self {
            Self(seed)
        }

        fn next(&mut self) -> u32 {
            self.0 = self
                .0
                .wrapping_mul(6364136223846793005)
                .wrapping_add(1442695040888963407);
            (self.0 >> 33) as u32
        }
    }

    // -----------------------------------------------------------------------
    // PostingBufferTest (posting_buffer_ut.cpp)
    // -----------------------------------------------------------------------

    fn check_posting_buffer(
        values: &[u32],
        row_num: usize,
        col_num: usize,
        existing: Option<&mut PostingBuffer>,
        expected_capacity: Option<u8>,
    ) {
        let mut owned;
        let buffer = match existing {
            Some(buffer) => buffer,
            None => {
                owned = PostingBuffer::new();
                owned.init(&u32_fields(row_num));
                &mut owned
            }
        };
        assert_eq!(expected_capacity.unwrap_or(0), buffer.capacity());
        assert_eq!(0, buffer.size());
        assert_eq!(row_num, buffer.row_count());
        for c in 0..col_num {
            for r in 0..row_num {
                assert!(buffer.push_back_u32(r, values[c + r * col_num]));
            }
            buffer.end_push_back();
            assert_eq!((c + 1) as u8, buffer.size());
            let want = expected_capacity.unwrap_or_else(|| PostingBuffer::allocate_plan(c as u8));
            assert_eq!(want, buffer.capacity());
        }
        assert_eq!(row_num, buffer.row_count());
        assert_eq!(col_num as u8, buffer.size());
        for r in 0..row_num {
            let row = buffer.row_u32(r);
            for c in 0..col_num {
                assert_eq!(values[c + r * col_num], row[c]);
            }
        }
    }

    #[test]
    fn posting_buffer_test1() {
        let mut buffer = PostingBuffer::new();
        buffer.init(&u32_fields(1));
        assert_eq!(0, buffer.capacity());
        assert_eq!(0, buffer.size());
        assert!(buffer.push_back_u32(0, 100));
        buffer.end_push_back();
        assert_eq!(2, buffer.capacity());
        assert_eq!(1, buffer.size());
    }

    #[test]
    fn posting_buffer_test2() {
        let values = [100, 101, 102, 200, 201, 202];
        check_posting_buffer(&values, 2, 3, None, None);
    }

    #[test]
    fn posting_buffer_test3() {
        let mut rng = Rng::new(7);
        let mut values = [0u32; 128 * 8];
        for c in 0..128 {
            for r in 0..8 {
                values[c + r * 128] = rng.next();
            }
        }
        let mut buffer = PostingBuffer::new();
        buffer.init(&u32_fields(8));
        check_posting_buffer(&values, 8, 128, Some(&mut buffer), None);
        buffer.clear();
        check_posting_buffer(&values, 8, 128, Some(&mut buffer), Some(128));
    }

    #[test]
    fn posting_buffer_test4() {
        {
            let mut buffer = PostingBuffer::new();
            buffer.init(&u32_fields(2));
            assert!(buffer.push_back_u32(0, 100));
            buffer.end_push_back();
            assert!(buffer.push_back_u32(0, 200));
        }
        {
            let mut buffer = PostingBuffer::new();
            buffer.init(&u32_fields(1));
            for col in 0..128u32 {
                assert!(buffer.push_back_u32(0, col));
                buffer.end_push_back();
            }
            assert!(!buffer.push_back_u32(0, 128));
        }
    }

    #[test]
    fn posting_buffer_test5() {
        let mut buffer = PostingBuffer::new();
        buffer.init(&u32_fields(2));
        assert!(buffer.push_back_u32(0, 100));
        assert_eq!(0, buffer.size());
        assert!(buffer.push_back_u32(1, 200));
        buffer.end_push_back();
        assert_eq!(1, buffer.size());
    }

    #[test]
    fn posting_buffer_test6() {
        let mut buffer = PostingBuffer::new();
        buffer.init(&u32_fields(3));
        assert_eq!(0, buffer.capacity());
        assert_eq!(0, buffer.size());
        assert!(buffer.push_back_u32(2, 100));
        assert_eq!(2, buffer.capacity());
        assert_eq!(0, buffer.size());
        assert!(buffer.push_back_u32(0, 100));
        assert_eq!(2, buffer.capacity());
        assert_eq!(0, buffer.size());
        assert!(buffer.push_back_u32(1, 100));
        buffer.end_push_back();
        assert_eq!(2, buffer.capacity());
        assert_eq!(1, buffer.size());
    }

    #[test]
    fn posting_buffer_test7() {
        let mut buffer = PostingBuffer::new();
        buffer.init(&u32_fields(1));
        buffer.push_back_u32(0, 100);
        buffer.end_push_back();
        assert_eq!(1, buffer.size());
        assert_eq!(2, buffer.capacity());
        buffer.clear();
        assert_eq!(0, buffer.size());
        assert_eq!(2, buffer.capacity());
    }

    #[test]
    fn posting_buffer_test8() {
        let mut fields = PostingFields::new();
        for (i, ty) in [
            FieldType::U32,
            FieldType::U32,
            FieldType::U16,
            FieldType::U16,
        ]
        .into_iter()
        .enumerate()
        {
            fields.add(PostingField {
                location: i,
                offset: i * 2,
                ty,
            });
        }
        let mut buffer = PostingBuffer::new();
        buffer.init(&fields);
        assert_eq!(0, buffer.capacity());
        assert_eq!(0, buffer.size());
        for i in 0..128u32 {
            assert!(buffer.push_back_u32(0, i));
            assert!(buffer.push_back_u32(1, i));
            assert!(buffer.push_back_u16(2, i as u16));
            assert!(buffer.push_back_u16(3, i as u16));
            buffer.end_push_back();
        }
        assert_eq!(128, buffer.capacity());
        assert_eq!(128, buffer.size());
        for i in 0..128usize {
            assert_eq!(i as u32, buffer.row_u32(0)[i]);
            assert_eq!(i as u32, buffer.row_u32(1)[i]);
            assert_eq!(i as u16, buffer.row_u16(2)[i]);
            assert_eq!(i as u16, buffer.row_u16(3)[i]);
        }
    }

    // -----------------------------------------------------------------------
    // PostingByteSliceTest (posting_byte_slice_ut.cpp)
    // -----------------------------------------------------------------------

    /// `NO_TERM_FREQUENCY`: doc id (`u32`) and doc payload (`u16`).
    fn doc_payload_fields() -> PostingFields {
        let mut fields = PostingFields::new();
        fields.add(PostingField {
            location: 0,
            offset: 0,
            ty: FieldType::U32,
        });
        fields.add(PostingField {
            location: 1,
            offset: 4,
            ty: FieldType::U16,
        });
        fields
    }

    #[test]
    fn posting_byte_slice_test1() {
        let mut slice = PostingByteSlice::new();
        slice.init(&doc_payload_fields());
        slice.push_back_u32(0, 1);
        slice.push_back_u16(1, 2);
        slice.end_push_back();
        assert!(slice.flush_info.is_valid_posting_buffer());
        slice.push_back_u32(0, 2);
        slice.push_back_u16(1, 3);
        slice.end_push_back();

        let mut reader = ReaderState::default();
        reader.open();
        assert_eq!(reader.decode_u32(&slice, MAX_DOC_PER_RECORD), Some(vec![1, 2]));
        assert_eq!(reader.decode_u16(&slice, MAX_DOC_PER_RECORD), Some(vec![2, 3]));
    }

    #[test]
    fn posting_byte_slice_test2() {
        let mut slice = PostingByteSlice::new();
        slice.init(&doc_payload_fields());
        slice.push_back_u32(0, 1);
        slice.push_back_u16(1, 2);
        slice.end_push_back();
        slice.flush();
        assert!(!slice.flush_info.is_valid_posting_buffer());
        assert_eq!(1, slice.flush_info.get_flush_count());
        assert_eq!(0, slice.buffer_size());
        assert_eq!(0, slice.flush());
    }

    #[test]
    fn posting_byte_slice_test3() {
        let mut slice = PostingByteSlice::new();
        slice.init(&doc_payload_fields());
        let decode_len = 5;
        let count = 11;
        for i in 0..count {
            slice.push_back_u32(0, i);
            slice.push_back_u16(1, (i * 2) as u16);
            slice.end_push_back();
            if slice.need_flush(decode_len) {
                slice.flush();
            }
        }
        let snapshot = slice.snapshot();
        assert_eq!(snapshot.total_count(), slice.total_count());
        assert!(Arc::ptr_eq(&snapshot.bytes, &slice.bytes));

        let mut reader = ReaderState::default();
        reader.open();
        let mut buffer = Vec::new();
        let mut payload = Vec::new();
        for _ in 0..3 {
            buffer.extend(reader.decode_u32(&slice, decode_len).unwrap());
            payload.extend(reader.decode_u16(&slice, decode_len).unwrap());
        }
        for i in 0..count {
            assert_eq!(i, buffer[i as usize]);
            assert_eq!(i * 2, u32::from(payload[i as usize]));
        }
    }

    // -----------------------------------------------------------------------
    // PostingByteSliceReaderTest (posting_byte_slice_reader_ut.cpp)
    // -----------------------------------------------------------------------

    #[test]
    fn posting_byte_slice_reader_test1() {
        let fields = u32_fields(1);

        // empty posting buffer
        {
            let mut slice = PostingByteSlice::new();
            slice.init(&fields);
            let mut reader = ReaderState::default();
            reader.open();
            assert!(reader.decode_u32(&slice, MAX_DOC_PER_RECORD).is_none());
        }
        // only has short buffer
        {
            let mut slice = PostingByteSlice::new();
            slice.init(&fields);
            slice.push_back_u32(0, 1);
            slice.end_push_back();
            let mut reader = ReaderState::default();
            reader.open();
            assert!(reader.decode_u32(&slice, 0).is_none());
        }
        // only has flushed byte slice
        {
            let mut slice = PostingByteSlice::new();
            slice.init(&fields);
            slice.push_back_u32(0, 1);
            slice.end_push_back();
            slice.flush();
            let mut reader = ReaderState::default();
            reader.open();
            assert!(reader.decode_u32(&slice, 0).is_none());
        }
        // has both
        {
            let mut slice = PostingByteSlice::new();
            slice.init(&fields);
            slice.push_back_u32(0, 1);
            slice.end_push_back();
            slice.flush();
            slice.push_back_u32(0, 1);
            slice.end_push_back();
            let mut reader = ReaderState::default();
            reader.open();
            assert!(reader.decode_u32(&slice, 0).is_none());
        }
    }

    fn create_reader(doc_count: u32, flush_count: usize) -> (PostingByteSlice, ReaderState) {
        let mut slice = PostingByteSlice::new();
        slice.init(&doc_payload_fields());
        for i in 0..doc_count {
            slice.push_back_u32(0, i);
            slice.push_back_u16(1, (i * 2) as u16);
            slice.end_push_back();
            if slice.need_flush(flush_count) {
                slice.flush();
            }
        }
        let mut reader = ReaderState::default();
        reader.open();
        (slice, reader)
    }

    #[test]
    fn posting_byte_slice_reader_test2() {
        let flush_size = 5usize;
        let (slice, mut reader) = create_reader(33, flush_size);

        for block in 0..6usize {
            let doc = reader.decode_u32(&slice, MAX_DOC_PER_RECORD).unwrap();
            assert_eq!(flush_size, doc.len());
            assert_eq!((block * flush_size + 4) as u32, doc[4]);
            let payload = reader.decode_u16(&slice, MAX_DOC_PER_RECORD).unwrap();
            assert_eq!(flush_size, payload.len());
            assert_eq!(((block * flush_size + 4) * 2) as u16, payload[4]);
        }
        let doc = reader.decode_u32(&slice, MAX_DOC_PER_RECORD).unwrap();
        assert_eq!(3, doc.len());
        assert_eq!(32, doc[2]);
        let payload = reader.decode_u16(&slice, MAX_DOC_PER_RECORD).unwrap();
        assert_eq!(3, payload.len());
        assert_eq!(64, payload[2]);
    }

    fn check_decode(doc_count: usize, flush_count: usize) {
        let (slice, mut reader) = create_reader(doc_count as u32, flush_count);
        let doc_id: Vec<u32> = (0..doc_count as u32).collect();
        let payload: Vec<u16> = (0..doc_count).map(|i| (i * 2) as u16).collect();
        let mut doc_buffer = Vec::with_capacity(doc_count * 2);
        let mut payload_buffer = Vec::with_capacity(doc_count * 2);

        let mut i = 0;
        while i < doc_count / flush_count {
            let docs = reader.decode_u32(&slice, MAX_DOC_PER_RECORD).unwrap();
            assert_eq!(docs.len(), flush_count);
            doc_buffer.extend(docs);
            let payloads = reader.decode_u16(&slice, MAX_DOC_PER_RECORD).unwrap();
            assert_eq!(payloads.len(), flush_count);
            payload_buffer.extend(payloads);
            i += 1;
        }
        if !doc_count.is_multiple_of(flush_count) {
            let docs = reader.decode_u32(&slice, MAX_DOC_PER_RECORD).unwrap();
            assert_eq!(docs.len(), doc_count % flush_count);
            doc_buffer.extend(docs);
            let payloads = reader.decode_u16(&slice, MAX_DOC_PER_RECORD).unwrap();
            assert_eq!(payloads.len(), doc_count % flush_count);
            payload_buffer.extend(payloads);
        }
        assert!(reader.decode_u32(&slice, MAX_DOC_PER_RECORD).is_none());
        assert!(reader.decode_u16(&slice, MAX_DOC_PER_RECORD).is_none());

        assert_eq!(doc_buffer, doc_id);
        assert_eq!(payload_buffer, payload);
    }

    #[test]
    fn posting_byte_slice_reader_test3() {
        check_decode(10, 5);
        check_decode(513, 128);
    }

    // -----------------------------------------------------------------------
    // SkipListWriterTest (skiplist_writer_ut.cpp)
    // -----------------------------------------------------------------------

    #[test]
    fn skiplist_writer_test1() {
        let format = SkipListFormat {
            fields: u32_fields(1),
            has_tf_list: false,
            has_block_max: false,
        };
        let mut writer = SkipListWriter::new();
        writer.init(&format);
        let delta = 10u32;
        let mut expected = Vec::new();
        let mut now = 0u32;
        for _ in 0..SKIP_LIST_BUFFER_SIZE + 2 {
            writer.add_item_1(delta);
            now += delta;
            expected.push(now);
        }
        assert!(!writer.bytes().is_empty());

        let mut reader = ReaderState::default();
        reader.open();
        let first = reader.decode_u32(writer.slice(), SKIP_LIST_BUFFER_SIZE).unwrap();
        let mut buffer = first;
        buffer.extend(reader.decode_u32(writer.slice(), 2).unwrap());
        assert_eq!(buffer, expected);
    }

    #[test]
    fn skiplist_writer_test2() {
        let format = SkipListFormat {
            fields: u32_fields(2),
            has_tf_list: false,
            has_block_max: false,
        };
        let mut writer = SkipListWriter::new();
        writer.init(&format);
        let delta = 10u32;

        let mut expected_keys = Vec::new();
        let mut expected_values = Vec::new();
        let mut now_key = 0u32;
        let mut now_value = 0u32;
        writer.add_item_2(now_key, now_value);
        expected_keys.push(now_key);
        expected_values.push(now_value);
        for _ in 0..SKIP_LIST_BUFFER_SIZE + 2 {
            now_key += delta;
            now_value += delta - 1;
            writer.add_item_2(now_key, now_value);
            expected_keys.push(now_key);
            expected_values.push(now_value);
        }

        let mut reader = ReaderState::default();
        reader.open();
        let keys1 = reader.decode_u32(writer.slice(), SKIP_LIST_BUFFER_SIZE).unwrap();
        let values1 = reader.decode_u32(writer.slice(), SKIP_LIST_BUFFER_SIZE).unwrap();
        let keys2 = reader.decode_u32(writer.slice(), 3).unwrap();
        let values2 = reader.decode_u32(writer.slice(), 3).unwrap();
        let mut read_key = 0u32;
        let mut read_values = Vec::new();
        read_values.extend(values1);
        read_values.extend(values2);
        for (i, (delta_key, value)) in keys1.iter().chain(keys2.iter()).zip(&read_values).enumerate() {
            read_key += delta_key;
            assert_eq!(read_key, expected_keys[i]);
            assert_eq!(*value, expected_values[i]);
        }
    }

    #[test]
    fn skiplist_writer_test3() {
        let format = SkipListFormat {
            fields: u32_fields(3),
            has_tf_list: false,
            has_block_max: false,
        };
        let mut writer = SkipListWriter::new();
        writer.init(&format);
        let delta = 10u32;

        let mut expected_keys = Vec::new();
        let mut expected_values1 = Vec::new();
        let mut expected_values2 = Vec::new();
        let mut now_key = 0u32;
        let mut now_value1 = 0u32;
        let mut now_value2 = 0u32;
        writer.add_item_3(now_key, now_value1, now_value2);
        expected_keys.push(now_key);
        expected_values1.push(now_value1);
        expected_values2.push(now_value2);
        for _ in 0..SKIP_LIST_BUFFER_SIZE + 2 {
            now_key += delta;
            now_value1 += delta - 1;
            now_value2 += delta - 2;
            writer.add_item_3(now_key, now_value1, now_value2);
            expected_keys.push(now_key);
            expected_values1.push(now_value1);
            expected_values2.push(now_value2);
        }

        let mut reader = ReaderState::default();
        reader.open();
        let keys1 = reader.decode_u32(writer.slice(), SKIP_LIST_BUFFER_SIZE).unwrap();
        let vals1_1 = reader.decode_u32(writer.slice(), SKIP_LIST_BUFFER_SIZE).unwrap();
        let vals2_1 = reader.decode_u32(writer.slice(), SKIP_LIST_BUFFER_SIZE).unwrap();
        let keys2 = reader.decode_u32(writer.slice(), 3).unwrap();
        let vals1_2 = reader.decode_u32(writer.slice(), 3).unwrap();
        let vals2_2 = reader.decode_u32(writer.slice(), 3).unwrap();

        let mut read_key = 0u32;
        let mut read_value1 = 0u32;
        let mut i = 0usize;
        for (delta_key, (dv1, value2)) in keys1
            .iter()
            .chain(keys2.iter())
            .zip(vals1_1.iter().chain(vals1_2.iter()).zip(vals2_1.iter().chain(vals2_2.iter())))
        {
            read_key += delta_key;
            read_value1 += dv1;
            assert_eq!(read_key, expected_keys[i]);
            assert_eq!(read_value1, expected_values1[i]);
            assert_eq!(*value2, expected_values2[i]);
            i += 1;
        }
        assert_eq!(expected_keys.len(), i);
    }

    // -----------------------------------------------------------------------
    // SkiplistReaderTest (skiplist_reader_ut.cpp)
    // -----------------------------------------------------------------------

    fn init_skiplist_writer(
        doc_num: usize,
    ) -> (SkipListWriter, Vec<u32>, Vec<u32>, Vec<u32>, Vec<u32>) {
        let mut fields = PostingFields::new();
        for i in 0..3 {
            fields.add(PostingField {
                location: i,
                offset: i * 4,
                ty: FieldType::U32,
            });
        }
        let format = SkipListFormat {
            fields,
            has_tf_list: true,
            has_block_max: false,
        };
        let mut writer = SkipListWriter::new();
        writer.init(&format);

        let mut doc_ids = Vec::new();
        let mut tfs = Vec::new();
        let mut deltas = Vec::new();
        let mut offsets = Vec::new();
        let mut now_offset = 0u32;
        for i in 0..doc_num {
            let doc_id = i as u32;
            let tf = (i * 10) as u32;
            let delta = (i * 100) as u32;
            doc_ids.push(doc_id);
            tfs.push(tf);
            deltas.push(delta);
            offsets.push(now_offset);
            now_offset += delta;
            writer.add_item_3(doc_id, tf, delta);
        }
        writer.flush();
        (writer, doc_ids, tfs, deltas, offsets)
    }

    fn check_skips(reader: &mut SkipListReader, queries: &[u32], doc_ids: &[u32], deltas: &[u32], offsets: &[u32]) {
        for &query in queries {
            let result = reader.skip_to(query).expect("skip_to");
            assert_eq!(result.doc_id, doc_ids[query as usize]);
            assert_eq!(result.offset, offsets[query as usize]);
            assert_eq!(result.delta, deltas[query as usize]);
        }
    }

    #[test]
    fn skiplist_reader_byte_slice() {
        let (writer, doc_ids, _tfs, deltas, offsets) =
            init_skiplist_writer(SKIP_LIST_BUFFER_SIZE + 2);
        let format = SkipListFormat::for_doc(true, false);
        let total = writer.bytes().len();
        let queries: Vec<u32> = (0..doc_ids.len() as u32).collect();
        let step_queries: Vec<u32> = (0..doc_ids.len() as u32).step_by(3).collect();
        let mut varying = Vec::new();
        let mut q = 0u32;
        while (q as usize) < doc_ids.len() {
            varying.push(q);
            q += q % 7 + 1;
        }

        for queries in [&queries, &step_queries, &varying] {
            let mut reader = SkipListReader::new(&format);
            reader.load_byte_slice(writer.bytes(), 0, total);
            check_skips(&mut reader, queries, &doc_ids, &deltas, &offsets);
        }
    }

    #[test]
    fn skiplist_reader_posting_byte_slice() {
        let (writer, doc_ids, _tfs, deltas, offsets) =
            init_skiplist_writer(SKIP_LIST_BUFFER_SIZE + 2);
        let format = SkipListFormat::for_doc(true, false);
        let queries: Vec<u32> = (0..doc_ids.len() as u32).collect();
        let step_queries: Vec<u32> = (0..doc_ids.len() as u32).step_by(3).collect();
        let mut varying = Vec::new();
        let mut q = 0u32;
        while (q as usize) < doc_ids.len() {
            varying.push(q);
            q += q % 7 + 1;
        }

        for queries in [&queries, &step_queries, &varying] {
            let mut reader = SkipListReader::new(&format);
            reader.load_posting_byte_slice(writer.slice());
            check_skips(&mut reader, queries, &doc_ids, &deltas, &offsets);
        }
    }
}
