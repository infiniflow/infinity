//! C++-compatible on-disk layouts (RUST_REWRITE_PLAN.md §2.6.1, task #68).
//!
//! The byte-level facts implemented here are frozen in
//! `rust_engine/docs/CPP_FORMAT_SPEC.md` (§1, §8.2, §8.3) with `file:line`
//! citations into the C++ tree. Nothing in this module invents a layout: every
//! field below is copied from that spec so a file written by this crate is
//! byte-identical to one written by the C++ engine.
//!
//! Layouts:
//!   * **data file** (`.col`, data blocks, snapshots): `u64 magic` +
//!     `u64 buffer_size` + `buffer_size` body bytes + `u64 checksum`.
//!   * **block `version` file**: raw, *no* magic/checksum —
//!     `u16 create_size` + `create_size × (u64 create_ts, i64 cumulative_row_count)`
//!     + `u16 capacity` + `capacity × u64 delete_ts`.
//!   * **column body inside `.col`**: `[data_region (data_cap_size bytes)]
//!     [null_bitmap ((capacity+7)/8 bytes)]` with **bit = 1 meaning non-NULL**.
//!   * **outline file** (`col_{id}_out`): the raw concatenation of the chunks
//!     referenced by variable-length slots; no header at all.

use infinity_common::TxnTimeStamp;

/// Data-file magic (`magic` in `src/storage/buffer/file_worker/data_file_worker_impl.cpp`).
pub const DATA_FILE_MAGIC: u64 = 0x00dd_3344;

/// Rows per block (`DEFAULT_BLOCK_CAPACITY` in `src/storage/definition/default_values.cppm`).
pub const DEFAULT_BLOCK_CAPACITY: usize = 8192;

/// Size of the C++ `size_t` on the 64-bit target the format is defined for.
const SIZE_OF_SIZE_T: usize = 8;

/// Errors from decoding an on-disk structure.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum FormatError {
    /// The buffer is shorter than the structure requires.
    Truncated { need: usize, got: usize },
    /// The leading magic did not match [`DATA_FILE_MAGIC`].
    BadMagic(u64),
}

impl std::fmt::Display for FormatError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            FormatError::Truncated { need, got } => {
                write!(f, "truncated: need {need} bytes, got {got}")
            }
            FormatError::BadMagic(m) => write!(f, "bad data-file magic: {m:#x}"),
        }
    }
}

impl std::error::Error for FormatError {}

// ---------------------------------------------------------------------------
// §1 — generic data file (`DataFileWorker`)
// ---------------------------------------------------------------------------

/// Frames `body` as a C++ data file: `magic` + `buffer_size` + body + `checksum`.
///
/// The C++ writer sets the data size to exactly `buffer_size` (no padding), and
/// the checksum field is written as `0`.
pub fn encode_data_file(body: &[u8]) -> Vec<u8> {
    let mut out = Vec::with_capacity(8 + SIZE_OF_SIZE_T + body.len() + 8);
    out.extend_from_slice(&DATA_FILE_MAGIC.to_le_bytes());
    out.extend_from_slice(&(body.len() as u64).to_le_bytes());
    out.extend_from_slice(body);
    out.extend_from_slice(&0u64.to_le_bytes());
    out
}

/// Returns the body of a data file written by [`encode_data_file`] (or by C++).
pub fn decode_data_file(buf: &[u8]) -> Result<Vec<u8>, FormatError> {
    let header = 8 + SIZE_OF_SIZE_T;
    if buf.len() < header + 8 {
        return Err(FormatError::Truncated {
            need: header + 8,
            got: buf.len(),
        });
    }
    let magic = u64::from_le_bytes(buf[0..8].try_into().unwrap());
    if magic != DATA_FILE_MAGIC {
        return Err(FormatError::BadMagic(magic));
    }
    let buffer_size = u64::from_le_bytes(buf[8..header].try_into().unwrap()) as usize;
    if buf.len() < header + buffer_size + 8 {
        return Err(FormatError::Truncated {
            need: header + buffer_size + 8,
            got: buf.len(),
        });
    }
    Ok(buf[header..header + buffer_size].to_vec())
}

// ---------------------------------------------------------------------------
// §8.2 — per-block `version` file (`BlockVersion`)
// ---------------------------------------------------------------------------

/// One append batch in the `version` file: the commit timestamp plus the
/// **cumulative** row count after that append.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct VersionCreate {
    pub create_ts: TxnTimeStamp,
    pub cumulative_row_count: i64,
}

/// The serialized form of a block's `version` file.
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct BlockVersionFile {
    pub created: Vec<VersionCreate>,
    /// One entry per block slot (length = block capacity); `0` = live row.
    pub deleted: Vec<TxnTimeStamp>,
}

impl BlockVersionFile {
    /// Builds the file from `(create_ts, batch_rows)` pairs and the capacity the
    /// block was created with (the C++ `deleted_` array is always that long).
    pub fn new(batches: &[(TxnTimeStamp, u32)], capacity: usize) -> Self {
        let mut created = Vec::with_capacity(batches.len());
        let mut cumulative = 0i64;
        for (ts, rows) in batches {
            cumulative += i64::from(*rows);
            created.push(VersionCreate {
                create_ts: *ts,
                cumulative_row_count: cumulative,
            });
        }
        Self {
            created,
            deleted: vec![0; capacity],
        }
    }

    /// Marks `offset` deleted at `ts` (the block capacity already sized `deleted`).
    pub fn delete(&mut self, offset: usize, ts: TxnTimeStamp) {
        if offset < self.deleted.len() {
            self.deleted[offset] = ts;
        }
    }

    /// Serializes exactly as C++ `BlockVersion::SaveToFile` does.
    pub fn encode(&self) -> Vec<u8> {
        let mut out = Vec::with_capacity(2 + self.created.len() * 16 + 2 + self.deleted.len() * 8);
        // `BlockOffset create_size` (u16 LE).
        out.extend_from_slice(&(self.created.len() as u16).to_le_bytes());
        for c in &self.created {
            out.extend_from_slice(&c.create_ts.to_le_bytes()); // TxnTimeStamp = u64
            out.extend_from_slice(&c.cumulative_row_count.to_le_bytes()); // i64
        }
        // `BlockOffset capacity` (u16 LE) then one u64 per slot.
        out.extend_from_slice(&(self.deleted.len() as u16).to_le_bytes());
        for ts in &self.deleted {
            out.extend_from_slice(&ts.to_le_bytes());
        }
        out
    }

    /// Parses a `version` file. Note there is no magic or checksum to validate.
    pub fn decode(buf: &[u8]) -> Result<Self, FormatError> {
        let mut pos = 0usize;
        let take = |pos: &mut usize, n: usize| -> Result<usize, FormatError> {
            if buf.len() < *pos + n {
                return Err(FormatError::Truncated {
                    need: *pos + n,
                    got: buf.len(),
                });
            }
            let at = *pos;
            *pos += n;
            Ok(at)
        };

        let at = take(&mut pos, 2)?;
        let create_size = u16::from_le_bytes(buf[at..at + 2].try_into().unwrap()) as usize;
        let mut created = Vec::with_capacity(create_size);
        for _ in 0..create_size {
            let at = take(&mut pos, 8)?;
            let create_ts = u64::from_le_bytes(buf[at..at + 8].try_into().unwrap());
            let at = take(&mut pos, 8)?;
            let cumulative_row_count = i64::from_le_bytes(buf[at..at + 8].try_into().unwrap());
            created.push(VersionCreate {
                create_ts,
                cumulative_row_count,
            });
        }
        let at = take(&mut pos, 2)?;
        let capacity = u16::from_le_bytes(buf[at..at + 2].try_into().unwrap()) as usize;
        let mut deleted = Vec::with_capacity(capacity);
        for _ in 0..capacity {
            let at = take(&mut pos, 8)?;
            deleted.push(u64::from_le_bytes(buf[at..at + 8].try_into().unwrap()));
        }
        Ok(Self { created, deleted })
    }
}

// ---------------------------------------------------------------------------
// §8.3 — column body inside `.col` (`[data_region][null_bitmap]`)
// ---------------------------------------------------------------------------

/// A column's body as stored inside `{column_id}.col`.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct ColumnBody {
    /// The fixed-width data region (`data_cap_size` bytes; shorter when fewer
    /// rows are present — the C++ buffer is capacity-sized but only the written
    /// prefix is meaningful here).
    pub data: Vec<u8>,
    /// `(capacity + 7) / 8` bytes; **bit = 1 means the row is non-NULL**.
    pub null_bitmap: Vec<u8>,
}

impl ColumnBody {
    /// Number of bytes the *capacity-sized* data region occupies.
    pub fn data_capacity_bytes(capacity: usize, slot_size: usize) -> usize {
        capacity * slot_size
    }

    /// Length of the null bitmap for `capacity` rows.
    pub fn null_bitmap_len(capacity: usize) -> usize {
        capacity.div_ceil(8)
    }

    /// Builds a body, padding the data region to `capacity * slot_size` and
    /// setting the null bitmap (bit = 1 for rows flagged `true` in `valid`).
    ///
    /// `data` must be exactly `rows * slot_size` bytes.
    pub fn build(
        capacity: usize,
        slot_size: usize,
        data: &[u8],
        valid: &[bool],
    ) -> Result<Self, FormatError> {
        let cap_bytes = Self::data_capacity_bytes(capacity, slot_size);
        if cap_bytes < data.len() {
            return Err(FormatError::Truncated {
                need: data.len(),
                got: cap_bytes,
            });
        }
        let mut region = vec![0u8; cap_bytes];
        region[..data.len()].copy_from_slice(data);

        let mut null_bitmap = vec![0u8; Self::null_bitmap_len(capacity)];
        for (row, ok) in valid.iter().enumerate() {
            if *ok {
                null_bitmap[row / 8] |= 1 << (row % 8);
            }
        }
        Ok(Self {
            data: region,
            null_bitmap,
        })
    }

    /// Serializes as `[data_region][null_bitmap]`.
    pub fn encode(&self) -> Vec<u8> {
        let mut out = Vec::with_capacity(self.data.len() + self.null_bitmap.len());
        out.extend_from_slice(&self.data);
        out.extend_from_slice(&self.null_bitmap);
        out
    }

    /// Is row `row` non-NULL?
    pub fn is_valid(&self, row: usize) -> bool {
        self.null_bitmap
            .get(row / 8)
            .is_some_and(|b| b & (1 << (row % 8)) != 0)
    }
}

// ---------------------------------------------------------------------------
// §8.1 — path helpers
// ---------------------------------------------------------------------------

/// `db_{db_id}/tbl_{table_id}/seg_{segment_id}/blk_{block_id}` (decimal ids, no
/// zero padding).
pub fn block_dir(db_id: u64, table_id: u64, segment_id: u32, block_id: u16) -> String {
    format!("db_{db_id}/tbl_{table_id}/seg_{segment_id}/blk_{block_id}")
}

/// `{column_id}.col` inside a block directory.
pub fn column_file(column_id: u64) -> String {
    format!("{column_id}.col")
}

/// `col_{column_id}_out` (outline for variable-length columns).
pub fn column_outline_file(column_id: u64) -> String {
    format!("col_{column_id}_out")
}

/// The block's `version` file name (written last).
pub const VERSION_FILE: &str = "version";

/// `db_{db_id}/tbl_{table_id}/idx_{index_id}/seg_{segment_id}`.
pub fn segment_index_dir(db_id: u64, table_id: u64, index_id: u64, segment_id: u32) -> String {
    format!("db_{db_id}/tbl_{table_id}/idx_{index_id}/seg_{segment_id}")
}

/// `chunk_{chunk_id}.idx` inside a segment-index directory.
pub fn chunk_index_file(chunk_id: u64) -> String {
    format!("chunk_{chunk_id}.idx")
}

/// A full-text index chunk base name: `ft_{rowid:016x}` (16 lowercase hex
/// digits), suffixed by the row count once chunks are merged.
pub fn fulltext_base_name(row_id: u64, merged_row_count: Option<u64>) -> String {
    match merged_row_count {
        Some(n) => format!("ft_{row_id:016x}_{n:x}"),
        None => format!("ft_{row_id:016x}"),
    }
}

/// The three full-text index files for a chunk base name.
pub fn fulltext_files(base_name: &str) -> [String; 3] {
    [
        format!("{base_name}.pos"),
        format!("{base_name}.dic"),
        format!("{base_name}.len"),
    ]
}

// ---------------------------------------------------------------------------
// §8.1/§8.3 — a block directory on disk
// ---------------------------------------------------------------------------

use std::path::{Path, PathBuf};

/// One column of a block: its `[data_region][null_bitmap]` body plus the raw
/// contents of the outline file (`col_{id}_out`, empty for fixed-width columns).
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct BlockColumn {
    pub column_id: u64,
    pub body: ColumnBody,
    pub outline: Vec<u8>,
}

/// Block identifiers, used to build the C++ directory path.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct BlockId {
    pub db_id: u64,
    pub table_id: u64,
    pub segment_id: u32,
    pub block_id: u16,
}

/// One column of a block as read back from disk: the raw `.col` body (i.e.
/// `[data_region][null_bitmap]`, undivided) and the outline contents.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct RawBlockColumn {
    pub column_id: u64,
    pub body: Vec<u8>,
    pub outline: Vec<u8>,
}

/// Writes one block directory in the C++ layout.
///
/// Files are created in the C++ order: every column's `{id}.col` (followed by
/// its `col_{id}_out` when present), and the `version` file **last**.
pub fn write_block(
    root: &Path,
    id: BlockId,
    columns: &[BlockColumn],
    version: &BlockVersionFile,
) -> std::io::Result<PathBuf> {
    let dir = root.join(block_dir(id.db_id, id.table_id, id.segment_id, id.block_id));
    std::fs::create_dir_all(&dir)?;
    for col in columns {
        std::fs::write(
            dir.join(column_file(col.column_id)),
            encode_data_file(&col.body.encode()),
        )?;
        if !col.outline.is_empty() {
            // The outline file is raw: no header, no trailer.
            std::fs::write(dir.join(column_outline_file(col.column_id)), &col.outline)?;
        }
    }
    std::fs::write(dir.join(VERSION_FILE), version.encode())?;
    Ok(dir)
}

/// Reads a block directory written by [`write_block`] (or by the C++ engine).
///
/// Returns the `version` file plus every `{id}.col` body, sorted by column id.
pub fn read_block(
    root: &Path,
    id: BlockId,
) -> std::io::Result<(BlockVersionFile, Vec<RawBlockColumn>)> {
    let dir = root.join(block_dir(id.db_id, id.table_id, id.segment_id, id.block_id));
    let version = BlockVersionFile::decode(&std::fs::read(dir.join(VERSION_FILE))?)
        .map_err(|e| std::io::Error::new(std::io::ErrorKind::InvalidData, e))?;

    let mut columns = Vec::new();
    for entry in std::fs::read_dir(&dir)? {
        let entry = entry?;
        let name = entry.file_name();
        let Some(name) = name.to_str() else { continue };
        let Some(stem) = name.strip_suffix(".col") else {
            continue;
        };
        let Ok(column_id) = stem.parse::<u64>() else {
            continue;
        };
        let body = decode_data_file(&std::fs::read(entry.path())?)
            .map_err(|e| std::io::Error::new(std::io::ErrorKind::InvalidData, e))?;
        let outline_path = dir.join(column_outline_file(column_id));
        let outline = if outline_path.exists() {
            std::fs::read(&outline_path)?
        } else {
            Vec::new()
        };
        columns.push(RawBlockColumn {
            column_id,
            body,
            outline,
        });
    }
    columns.sort_by_key(|c| c.column_id);
    Ok((version, columns))
}

/// Splits a raw `.col` body into its `[data_region][null_bitmap]` parts.
pub fn split_body(raw: &[u8], capacity: usize, slot_size: usize) -> ColumnBody {
    let cap_bytes = ColumnBody::data_capacity_bytes(capacity, slot_size);
    split_body_at(raw, cap_bytes)
}

/// Splits a raw `.col` body given the column's layout, which knows the data
/// region size for compact-bit (boolean) columns as well.
pub fn split_body_for_layout(raw: &[u8], capacity: usize, layout: ColumnLayout) -> ColumnBody {
    let cap_bytes = match layout {
        ColumnLayout::CompactBit => ColumnBody::null_bitmap_len(capacity),
        ColumnLayout::Fixed(width) | ColumnLayout::VarLen(width) => capacity * width,
    };
    split_body_at(raw, cap_bytes)
}

fn split_body_at(raw: &[u8], cap_bytes: usize) -> ColumnBody {
    let bitmap_len = raw.len().saturating_sub(cap_bytes);
    let data = raw.get(..cap_bytes).unwrap_or(raw).to_vec();
    let null_bitmap = raw
        .get(cap_bytes..cap_bytes + bitmap_len)
        .unwrap_or(&[])
        .to_vec();
    ColumnBody { data, null_bitmap }
}

// ---------------------------------------------------------------------------
// Date/time text -> wire/disk integers
// ---------------------------------------------------------------------------

/// Days from 1970-01-01 for a proleptic-Gregorian civil date (Hinnant's algorithm).
fn days_from_civil(y: i64, m: i64, d: i64) -> i64 {
    let y = if m <= 2 { y - 1 } else { y };
    let era = if y >= 0 { y } else { y - 399 } / 400;
    let yoe = y - era * 400;
    let mp = (m + 9) % 12;
    let doy = (153 * mp + 2) / 5 + d - 1;
    let doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    era * 146097 + doe - 719468
}

/// Parses `YYYY-MM-DD` (or `YYYY/MM/DD`) into days since the epoch — the
/// `ColumnDate`/`ColumnDateTime` integer form.
pub fn date_to_days(s: &str) -> Option<i32> {
    let mut it = s.trim().split(['-', '/']);
    let y: i64 = it.next()?.parse().ok()?;
    let m: i64 = it.next()?.parse().ok()?;
    let d: i64 = it.next()?.parse().ok()?;
    i32::try_from(days_from_civil(y, m, d)).ok()
}

/// Parses `HH:MM:SS` into seconds since midnight — the `ColumnTime` form.
pub fn time_to_secs(s: &str) -> Option<i32> {
    let mut it = s.trim().split(':');
    let h: i64 = it.next()?.parse().ok()?;
    let m: i64 = it.next().unwrap_or("0").parse().ok()?;
    let sec: i64 = it.next().unwrap_or("0").parse().ok()?;
    i32::try_from(h * 3600 + m * 60 + sec).ok()
}

/// Splits `YYYY-MM-DD HH:MM:SS` into `(epoch days, seconds of day)` — the
/// `ColumnDateTime`/`ColumnTimestamp` form.
pub fn datetime_to_pair(s: &str) -> Option<(i32, i32)> {
    let (d, t) = s.trim().split_once([' ', 'T'])?;
    Some((date_to_days(d)?, time_to_secs(t)?))
}

// ---------------------------------------------------------------------------
// Outline-backed (variable-length) value parsing/encoding
// ---------------------------------------------------------------------------

/// Strips exactly one outer `[ … ]` pair (no-op when absent).
pub fn strip_one_bracket(t: &str) -> &str {
    let t = t.trim();
    if t.starts_with('[') && t.ends_with(']') && t.len() >= 2 {
        &t[1..t.len() - 1]
    } else {
        t
    }
}

/// Splits array content into its top-level comma-separated segments, keeping
/// nested `[…]` groups intact (`[1,2],[3,4]` → `["[1,2]", "[3,4]"]`).
pub fn split_top_level(content: &str) -> Vec<String> {
    let mut out = Vec::new();
    let mut depth = 0i32;
    let mut cur = String::new();
    for ch in content.chars() {
        match ch {
            '[' => {
                depth += 1;
                cur.push(ch);
            }
            ']' => {
                depth -= 1;
                cur.push(ch);
            }
            ',' if depth == 0 => {
                let s = cur.trim();
                if !s.is_empty() {
                    out.push(s.to_string());
                }
                cur.clear();
            }
            _ => cur.push(ch),
        }
    }
    let s = cur.trim();
    if !s.is_empty() {
        out.push(s.to_string());
    }
    out
}

/// Parses a comma-separated list of numbers.
pub fn parse_flat(s: &str) -> Vec<f64> {
    s.split(',')
        .filter_map(|x| x.trim().parse::<f64>().ok())
        .collect()
}

/// Parses a tensor rendering into its vectors (`[[1,2],[3,4]]` → two).
pub fn parse_nested_f64(cell: &str) -> Vec<Vec<f64>> {
    let content = strip_one_bracket(cell);
    if content.trim().is_empty() {
        return Vec::new();
    }
    if !content.contains('[') {
        return vec![parse_flat(content)];
    }
    split_top_level(content)
        .iter()
        .map(|seg| parse_flat(strip_one_bracket(seg)))
        .collect()
}

/// Parses a `TENSORARRAY` rendering into tensors of vectors.
pub fn parse_tensorarray_f64(cell: &str) -> Vec<Vec<Vec<f64>>> {
    let content = strip_one_bracket(cell);
    if content.trim().is_empty() {
        return Vec::new();
    }
    split_top_level(content)
        .iter()
        .map(|tensor| {
            let tc = strip_one_bracket(tensor);
            if !tc.contains('[') {
                return vec![parse_flat(tc)];
            }
            split_top_level(tc)
                .iter()
                .map(|seg| parse_flat(strip_one_bracket(seg)))
                .collect()
        })
        .collect()
}

/// Packs one tensor body: each vector padded/truncated to `dim` elements.
pub fn pack_tensor_body(vectors: &[Vec<f64>], dim: usize, element_type: i32) -> Vec<u8> {
    let mut body = Vec::new();
    for v in vectors {
        for k in 0..dim {
            push_embedding_element(&mut body, element_type, v.get(k).copied().unwrap_or(0.0));
        }
    }
    body
}

/// The `(count, outline offset)` slot of a `Tensor`/`TensorArray`/`MultiVector`
/// /`Array` column: `u64 { count : 16; file_offset : 48 }`.
///
/// §9.5 flags the bit order as **VERIFY** (it follows the compiler's bit-field
/// allocation on x86_64: the low 16 bits hold the count).
pub fn varlen_count_offset_slot(count: u64, offset: u64) -> [u8; 8] {
    ((count & 0xffff) | (offset << 16)).to_le_bytes()
}

/// The `(nnz, outline offset)` slot of a `Sparse` column: `i64 nnz` + `i64 offset`.
pub fn sparse_slot(nnz: i64, offset: u64) -> [u8; 16] {
    let mut out = [0u8; 16];
    out[..8].copy_from_slice(&nnz.to_le_bytes());
    out[8..].copy_from_slice(&offset.to_le_bytes());
    out
}

/// How a column's values occupy the `.col` data region (spec §8.3).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ColumnLayout {
    /// Boolean/null: `(capacity + 7) / 8` packed bits (C++ `kCompactBit`).
    CompactBit,
    /// `capacity * n` bytes, one little-endian value per row.
    Fixed(usize),
    /// `capacity * n` bytes of slot structs; the payload lives in the outline
    /// file (C++ `kVarBuffer`).
    VarLen(usize),
}

/// Maps the SDK/wire `ColumnType` code to its `.col` layout. `None` for types
/// this module does not pack yet (embedding/tensor/… — fixed but dim-dependent).
pub fn column_layout(column_type: i32) -> Option<ColumnLayout> {
    Some(match column_type {
        0 => ColumnLayout::CompactBit,           // Boolean
        1 => ColumnLayout::Fixed(1),             // Int8
        2 => ColumnLayout::Fixed(2),             // Int16
        3 => ColumnLayout::Fixed(4),             // Int32
        4 => ColumnLayout::Fixed(8),             // Int64
        5 => ColumnLayout::Fixed(4),             // Float32
        6 => ColumnLayout::Fixed(8),             // Float64
        7 => ColumnLayout::Fixed(2),             // Float16
        8 => ColumnLayout::Fixed(2),             // BFloat16
        9 => ColumnLayout::VarLen(16),           // Varchar
        13 => ColumnLayout::VarLen(16),          // Sparse (i64 nnz + i64 offset)
        16 => ColumnLayout::Fixed(4),            // Date
        17 => ColumnLayout::Fixed(4),            // Time
        18 => ColumnLayout::Fixed(8),            // DateTime
        19 => ColumnLayout::Fixed(8),            // Timestamp
        20 => ColumnLayout::Fixed(8),            // Interval
        21 => ColumnLayout::VarLen(8),           // Array
        22 => ColumnLayout::VarLen(16),          // Json
        11 | 12 | 14 => ColumnLayout::VarLen(8), // Tensor / TensorArray / MultiVector
        _ => return None,
    })
}

/// The `.col` layout of an embedding/vector column: `dimension * element_width`
/// inline bytes per row (`kStandard`, no outline — spec §8.3).
pub fn embedding_column_layout(dimension: usize, element_width: usize) -> ColumnLayout {
    ColumnLayout::Fixed(dimension * element_width)
}

/// IEEE-754 binary16 of an `f32` (round to nearest even).
pub fn f32_to_f16_bits(f: f32) -> u16 {
    let bits = f.to_bits();
    let sign = ((bits >> 16) & 0x8000) as u16;
    let exp = ((bits >> 23) & 0xff) as i32 - 127 + 15;
    let mant = bits & 0x007f_ffff;
    if exp <= 0 {
        return sign;
    }
    if exp >= 31 {
        return sign | 0x7c00;
    }
    let half = sign | ((exp as u16) << 10) | ((mant >> 13) as u16);
    let rest = mant & 0x1fff;
    if rest > 0x1000 || (rest == 0x1000 && half & 1 == 1) {
        half + 1
    } else {
        half
    }
}

/// Appends one embedding element, little-endian, for the SDK `ElementType` code.
pub fn push_embedding_element(out: &mut Vec<u8>, element_type: i32, value: f64) {
    match element_type {
        0 | 1 => out.push(value as u8),   // Bit / UInt8
        2 => out.push(value as i8 as u8), // Int8
        3 => out.extend_from_slice(&(value as i16).to_le_bytes()),
        4 => out.extend_from_slice(&(value as i32).to_le_bytes()),
        5 => out.extend_from_slice(&(value as i64).to_le_bytes()),
        7 => out.extend_from_slice(&value.to_le_bytes()),
        8 => out.extend_from_slice(&f32_to_f16_bits(value as f32).to_le_bytes()),
        9 => out.extend_from_slice(&((value as f32).to_bits() >> 16).to_le_bytes()),
        _ => out.extend_from_slice(&(value as f32).to_le_bytes()), // Float32
    }
}

/// Reads one embedding element, little-endian, for the SDK `ElementType` code.
pub fn read_embedding_element(bytes: &[u8], element_type: i32) -> Option<f64> {
    Some(match element_type {
        0 | 1 => f64::from(bytes[0]),
        2 => f64::from(bytes[0] as i8),
        3 => f64::from(i16::from_le_bytes(bytes.try_into().ok()?)),
        4 => f64::from(i32::from_le_bytes(bytes.try_into().ok()?)),
        5 => i64::from_le_bytes(bytes.try_into().ok()?) as f64,
        7 => f64::from_le_bytes(bytes.try_into().ok()?),
        8 => f64::from(f16_bits_to_f32(u16::from_le_bytes(bytes.try_into().ok()?))),
        9 => {
            let hi = u16::from_le_bytes(bytes.try_into().ok()?) as u32;
            f64::from(f32::from_bits(hi << 16))
        }
        _ => f64::from(f32::from_le_bytes(bytes.try_into().ok()?)),
    })
}

/// IEEE-754 binary16 bits back to `f32`.
fn f16_bits_to_f32(half: u16) -> f32 {
    let sign = u32::from(half & 0x8000) << 16;
    let exp = (half >> 10) & 0x1f;
    let mant = u32::from(half & 0x03ff);
    let bits = if exp == 0 {
        if mant == 0 {
            sign
        } else {
            // Subnormal: normalise into a f32 exponent.
            let mut e = -1i32;
            let mut m = mant;
            while m & 0x400 == 0 {
                m <<= 1;
                e -= 1;
            }
            sign | (((127 - 15 + e + 1) as u32) << 23) | ((m & 0x3ff) << 13)
        }
    } else if exp == 0x1f {
        sign | 0x7f80_0000 | (mant << 13)
    } else {
        sign | ((u32::from(exp) + 127 - 15) << 23) | (mant << 13)
    };
    f32::from_bits(bits)
}

/// Byte width of one embedding element (bit elements are handled by the caller,
/// which packs `dimension / 8` bytes per row).
pub fn element_width(element_type: i32) -> usize {
    match element_type {
        0 => 1, // Bit (packed bit-by-bit; see the caller)
        1 | 2 => 1,
        3 | 8 | 9 => 2, // Int16 / Float16 / BFloat16
        4 | 6 => 4,     // Int32 / Float32
        5 | 7 => 8,     // Int64 / Float64
        _ => 4,
    }
}

/// Packs a **scalar** column into its `.col` body plus outline contents.
///
/// `slots[i]` is the row's already-encoded value; `None` marks a NULL (the slot
/// is zeroed and the row's null bit cleared). Rules by layout:
///   * `CompactBit` — `Some(b"\x01")` (or `b"true"`/`b"1"`) sets the bit.
///   * `Fixed(n)` — `Some` must be exactly `n` bytes, little-endian.
///   * `VarLen(16)` for `Varchar`(9)/`Json`(22) — `Some` is the UTF-8 payload,
///     appended to the outline; the slot records `u64(24-bit length | 48-bit
///     outline offset)`.
///
/// Array / tensor / sparse columns carry an element count that the string
/// level does not know, so they are packed by the caller (see the wire layer's
/// `rows_to_column_fields_typed`); this function rejects them.
pub fn pack_column(
    column_type: i32,
    slots: &[Option<&[u8]>],
    capacity: usize,
) -> Result<(ColumnBody, Vec<u8>), FormatError> {
    let layout = column_layout(column_type).ok_or(FormatError::Truncated {
        need: 0,
        got: slots.len(),
    })?;
    pack_column_with_layout(column_type, layout, slots, capacity)
}

/// Like [`pack_column`] but with an explicit layout, which embedding columns
/// need because their fixed width depends on the declared dimension.
pub fn pack_column_with_layout(
    column_type: i32,
    layout: ColumnLayout,
    slots: &[Option<&[u8]>],
    capacity: usize,
) -> Result<(ColumnBody, Vec<u8>), FormatError> {
    let err = || FormatError::Truncated {
        need: 0,
        got: slots.len(),
    };
    let mut valid = Vec::with_capacity(slots.len());

    match layout {
        ColumnLayout::CompactBit => {
            let mut bits = vec![0u8; ColumnBody::null_bitmap_len(capacity)];
            let mut null_bitmap = vec![0u8; ColumnBody::null_bitmap_len(capacity)];
            for (row, v) in slots.iter().enumerate() {
                let Some(v) = v else {
                    valid.push(false);
                    continue;
                };
                valid.push(true);
                null_bitmap[row / 8] |= 1 << (row % 8);
                let on: &[u8] = v;
                if on == b"1" || on.eq_ignore_ascii_case(b"true") {
                    bits[row / 8] |= 1 << (row % 8);
                }
            }
            Ok((
                ColumnBody {
                    data: bits,
                    null_bitmap,
                },
                Vec::new(),
            ))
        }
        ColumnLayout::Fixed(width) => {
            let mut data = Vec::with_capacity(slots.len() * width);
            for v in slots {
                valid.push(v.is_some());
                match v {
                    Some(bytes) if bytes.len() == width => data.extend_from_slice(bytes),
                    Some(_) => return Err(err()), // wrong slot encoding
                    None => data.extend_from_slice(&vec![0u8; width]),
                }
            }
            Ok((
                ColumnBody::build(capacity, width, &data, &valid)?,
                Vec::new(),
            ))
        }
        ColumnLayout::VarLen(16) if matches!(column_type, 9 | 22) => {
            // C++ `Varchar` (16 bytes): word 0 = `u64 { length_ : 24; prefix_[5] }`
            // (the 5 prefix bytes are the payload's first five bytes), word 1 =
            // `u64 file_offset` into the outline.
            let mut outline = Vec::new();
            let mut data = Vec::with_capacity(slots.len() * 16);
            for v in slots {
                valid.push(v.is_some());
                let Some(payload) = v else {
                    // A NULL slot is left all-zero; nothing is appended.
                    data.extend_from_slice(&[0u8; 16]);
                    continue;
                };
                let offset = outline.len() as u64;
                outline.extend_from_slice(payload);
                let len = payload.len() as u64 & 0x00ff_ffff;
                let mut prefix = [0u8; 5];
                for (i, b) in payload.iter().take(5).enumerate() {
                    prefix[i] = *b;
                }
                let mut word0 = len;
                for (i, b) in prefix.iter().enumerate() {
                    word0 |= (*b as u64) << (24 + 8 * i);
                }
                data.extend_from_slice(&word0.to_le_bytes());
                data.extend_from_slice(&offset.to_le_bytes());
            }
            Ok((ColumnBody::build(capacity, 16, &data, &valid)?, outline))
        }
        ColumnLayout::VarLen(width) if !matches!(column_type, 9 | 22) => {
            // Tensor / Sparse and friends: the caller has already built the
            // count/offset slots and the outline, so the slots are copied
            // verbatim and no outline is produced here.
            let mut data = Vec::with_capacity(slots.len() * width);
            for v in slots {
                valid.push(v.is_some());
                match v {
                    Some(bytes) if bytes.len() == width => data.extend_from_slice(bytes),
                    Some(_) => return Err(err()),
                    None => data.extend_from_slice(&vec![0u8; width]),
                }
            }
            Ok((
                ColumnBody::build(capacity, width, &data, &valid)?,
                Vec::new(),
            ))
        }
        _ => Err(err()),
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn data_file_header_and_trailer() {
        let body = vec![1u8, 2, 3, 4, 5];
        let file = encode_data_file(&body);
        // 8 (magic) + 8 (size_t buffer_size) + 5 (body) + 8 (checksum)
        assert_eq!(file.len(), 8 + 8 + 5 + 8);
        assert_eq!(&file[0..8], &DATA_FILE_MAGIC.to_le_bytes());
        assert_eq!(&file[8..16], &5u64.to_le_bytes());
        assert_eq!(&file[16..21], &body[..]);
        assert_eq!(&file[21..29], &0u64.to_le_bytes());
        assert_eq!(decode_data_file(&file).unwrap(), body);
    }

    #[test]
    fn data_file_rejects_bad_magic_and_truncation() {
        let mut file = encode_data_file(b"abc");
        file[0] ^= 0xff;
        assert!(matches!(
            decode_data_file(&file),
            Err(FormatError::BadMagic(_))
        ));
        assert!(matches!(
            decode_data_file(&file[..10]),
            Err(FormatError::Truncated { .. })
        ));
    }

    #[test]
    fn block_version_file_layout() {
        // Two appends (3 then 2 rows) into an 8-row block, row 1 deleted at 30.
        let mut v = BlockVersionFile::new(&[(10, 3), (20, 2)], 8);
        v.delete(1, 30);
        let bytes = v.encode();

        // u16 create_size
        assert_eq!(&bytes[0..2], &2u16.to_le_bytes());
        // (u64 create_ts, i64 cumulative_row_count) × 2
        assert_eq!(&bytes[2..10], &10u64.to_le_bytes());
        assert_eq!(&bytes[10..18], &3i64.to_le_bytes());
        assert_eq!(&bytes[18..26], &20u64.to_le_bytes());
        assert_eq!(&bytes[26..34], &5i64.to_le_bytes());
        // u16 capacity
        assert_eq!(&bytes[34..36], &8u16.to_le_bytes());
        // u64 delete_ts × capacity
        assert_eq!(bytes.len(), 36 + 8 * 8);
        assert_eq!(&bytes[36..44], &0u64.to_le_bytes());
        assert_eq!(&bytes[44..52], &30u64.to_le_bytes());

        assert_eq!(BlockVersionFile::decode(&bytes).unwrap(), v);
    }

    #[test]
    fn block_version_roundtrip_empty() {
        let v = BlockVersionFile::new(&[], DEFAULT_BLOCK_CAPACITY);
        let bytes = v.encode();
        assert_eq!(bytes.len(), 2 + 2 + 8 * DEFAULT_BLOCK_CAPACITY);
        assert_eq!(BlockVersionFile::decode(&bytes).unwrap(), v);
    }

    #[test]
    fn column_body_pads_data_and_sets_null_bits() {
        // capacity 4, slot 4 bytes, 3 rows with row 1 NULL.
        let data = [1u8, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0];
        let body = ColumnBody::build(4, 4, &data, &[true, false, true]).unwrap();
        assert_eq!(body.data.len(), 16); // padded to capacity * slot
        assert_eq!(ColumnBody::null_bitmap_len(4), 1);
        assert_eq!(body.null_bitmap, vec![0b0000_0101]);
        assert!(body.is_valid(0));
        assert!(!body.is_valid(1));
        assert!(body.is_valid(2));
        assert!(!body.is_valid(3));

        let encoded = body.encode();
        assert_eq!(encoded.len(), 16 + 1);
        // data region first, then the bitmap
        assert_eq!(&encoded[16..17], &[0b0000_0101]);
    }

    #[test]
    fn column_body_rejects_oversized_data() {
        let data = vec![0u8; 20];
        assert!(ColumnBody::build(4, 4, &data, &[true, true, true, true]).is_err());
    }

    #[test]
    fn path_layout_matches_cpp() {
        assert_eq!(block_dir(1, 2, 3, 4), "db_1/tbl_2/seg_3/blk_4");
        assert_eq!(column_file(0), "0.col");
        assert_eq!(column_file(12), "12.col");
        assert_eq!(column_outline_file(12), "col_12_out");
        assert_eq!(VERSION_FILE, "version");
        assert_eq!(segment_index_dir(1, 2, 5, 3), "db_1/tbl_2/idx_5/seg_3");
        assert_eq!(chunk_index_file(7), "chunk_7.idx");
        assert_eq!(fulltext_base_name(0x1f, None), "ft_000000000000001f");
        assert_eq!(
            fulltext_base_name(0x1f, Some(0x2a)),
            "ft_000000000000001f_2a"
        );
        assert_eq!(
            fulltext_files("ft_000000000000001f"),
            [
                "ft_000000000000001f.pos".to_string(),
                "ft_000000000000001f.dic".to_string(),
                "ft_000000000000001f.len".to_string()
            ]
        );
    }

    #[test]
    fn block_roundtrip_on_disk() {
        let tmp = tempfile::tempdir().unwrap();
        let id = BlockId {
            db_id: 1,
            table_id: 2,
            segment_id: 3,
            block_id: 4,
        };
        // A 4-byte-slot int column with 3 rows (row 1 NULL) and a varchar column
        // whose payload lives in the outline file.
        let ints: Vec<u8> = [7i32, 0, 9].iter().flat_map(|v| v.to_le_bytes()).collect();
        let int_body = ColumnBody::build(8192, 4, &ints, &[true, false, true]).unwrap();
        let varchar = ColumnBody {
            data: {
                // Two 16-byte slots: word0 = length (| prefix), word1 = offset.
                let mut d = Vec::new();
                for (payload, off) in [(b"hello".as_slice(), 0u64), (b"ab".as_slice(), 5u64)] {
                    let mut word0 = payload.len() as u64;
                    for (i, b) in payload.iter().take(5).enumerate() {
                        word0 |= (*b as u64) << (24 + 8 * i);
                    }
                    d.extend_from_slice(&word0.to_le_bytes());
                    d.extend_from_slice(&off.to_le_bytes());
                }
                d
            },
            null_bitmap: vec![0b0000_0011],
        };
        let columns = vec![
            BlockColumn {
                column_id: 0,
                body: int_body,
                outline: Vec::new(),
            },
            BlockColumn {
                column_id: 1,
                body: varchar,
                outline: b"helloab".to_vec(),
            },
        ];
        let mut version = BlockVersionFile::new(&[(10, 3)], DEFAULT_BLOCK_CAPACITY);
        version.delete(1, 20);

        let dir = write_block(tmp.path(), id, &columns, &version).unwrap();
        // `version` written last, and the outline exists only for the varchar col.
        assert!(dir.join("0.col").exists());
        assert!(dir.join("1.col").exists());
        assert!(dir.join("col_1_out").exists());
        assert!(!dir.join("col_0_out").exists());
        assert!(dir.join(VERSION_FILE).exists());

        let (got_version, got_columns) = read_block(tmp.path(), id).unwrap();
        assert_eq!(got_version, version);
        assert_eq!(got_columns.len(), 2);

        // The int column's body is `[data_region][null_bitmap]`.
        let split = split_body(&got_columns[0].body, 8192, 4);
        assert_eq!(&split.data[..12], &ints[..]);
        assert_eq!(split.null_bitmap[0], 0b0000_0101);
        assert!(got_columns[0].outline.is_empty());

        // The varchar column points into the raw outline file.
        assert_eq!(got_columns[1].outline, b"helloab");
        let w0 = u64::from_le_bytes(got_columns[1].body[0..8].try_into().unwrap());
        let off = u64::from_le_bytes(got_columns[1].body[8..16].try_into().unwrap());
        assert_eq!(w0 & 0x00ff_ffff, 5); // length
        assert_eq!(&w0.to_le_bytes()[3..8], b"hello"); // inlined prefix
        assert_eq!(off, 0); // outline offset of the first row
    }

    #[test]
    fn read_block_rejects_bad_version_file() {
        let tmp = tempfile::tempdir().unwrap();
        let id = BlockId {
            db_id: 0,
            table_id: 0,
            segment_id: 0,
            block_id: 0,
        };
        let dir = write_block(tmp.path(), id, &[], &BlockVersionFile::new(&[], 4)).unwrap();
        std::fs::write(dir.join(VERSION_FILE), b"\x01").unwrap();
        let err = read_block(tmp.path(), id).unwrap_err();
        assert_eq!(err.kind(), std::io::ErrorKind::InvalidData);
    }

    #[test]
    fn column_layout_widths_match_cpp() {
        assert_eq!(column_layout(0), Some(ColumnLayout::CompactBit)); // Boolean
        assert_eq!(column_layout(3), Some(ColumnLayout::Fixed(4))); // Integer
        assert_eq!(column_layout(4), Some(ColumnLayout::Fixed(8))); // BigInt
        assert_eq!(column_layout(7), Some(ColumnLayout::Fixed(2))); // Float16
        assert_eq!(column_layout(9), Some(ColumnLayout::VarLen(16))); // Varchar
        assert_eq!(column_layout(22), Some(ColumnLayout::VarLen(16))); // Json
        assert_eq!(column_layout(255), None);
    }

    #[test]
    fn pack_fixed_column_lays_values_and_nulls() {
        let a = 7i32.to_le_bytes();
        let c = 9i32.to_le_bytes();
        let (body, outline) = pack_column(3, &[Some(&a), None, Some(&c)], 4).unwrap();
        assert!(outline.is_empty());
        assert_eq!(body.data.len(), 16); // padded to capacity * 4
        assert_eq!(&body.data[0..4], &a);
        assert_eq!(&body.data[4..8], &[0, 0, 0, 0]); // NULL slot is zeroed
        assert_eq!(&body.data[8..12], &c);
        assert_eq!(body.null_bitmap[0], 0b0000_0101);
    }

    #[test]
    fn pack_fixed_column_rejects_wrong_slot_width() {
        // A 2-byte value in a 4-byte slot is a caller bug.
        let bytes = 1i16.to_le_bytes();
        assert!(pack_column(3, &[Some(&bytes)], 4).is_err());
    }

    #[test]
    fn pack_bit_column() {
        let (body, _) =
            pack_column(0, &[Some(b"true"), Some(b"false"), None, Some(b"1")], 4).unwrap();
        assert_eq!(body.data.len(), 1); // (capacity + 7) / 8
        assert_eq!(body.data[0], 0b0000_1001); // rows 0 and 3 are true
        assert_eq!(body.null_bitmap[0], 0b0000_1011); // row 2 is NULL
    }

    #[test]
    fn pack_varchar_column_offsets_into_outline() {
        let (body, outline) = pack_column(9, &[Some(b"hello"), None, Some(b"ab")], 4).unwrap();
        assert_eq!(outline, b"helloab");
        assert_eq!(body.data.len(), 64); // capacity * 16
        let word = |row: usize| -> (u64, u64) {
            let at = row * 16;
            (
                u64::from_le_bytes(body.data[at..at + 8].try_into().unwrap()),
                u64::from_le_bytes(body.data[at + 8..at + 16].try_into().unwrap()),
            )
        };
        // word 0 = 24-bit length | 5 prefix bytes; word 1 = outline offset.
        let (w0, off0) = word(0);
        assert_eq!((w0 & 0x00ff_ffff, off0), (5, 0)); // "hello"
        assert_eq!(&w0.to_le_bytes()[3..8], b"hello"); // prefix inlined
        assert_eq!(word(1), (0, 0)); // NULL slot is all zero
        let (w2, off2) = word(2);
        assert_eq!((w2 & 0x00ff_ffff, off2), (2, 5)); // "ab" @ 5
        assert_eq!(body.null_bitmap[0], 0b0000_0101);
    }

    #[test]
    fn pack_column_rejects_unsupported_types() {
        // Arrays need an element count the byte-level API does not have.
        assert!(pack_column(21, &[Some(b"")], 4).is_err());
        assert!(pack_column(13, &[Some(b"")], 4).is_err());
    }

    #[test]
    fn tensor_text_parsing() {
        assert_eq!(
            parse_nested_f64("[[1,2],[3,4]]"),
            vec![vec![1.0, 2.0], vec![3.0, 4.0]]
        );
        // A flat vector is one row.
        assert_eq!(parse_nested_f64("[1,2,3]"), vec![vec![1.0, 2.0, 3.0]]);
        assert!(parse_nested_f64("[]").is_empty());
        assert_eq!(
            parse_tensorarray_f64("[[[1,2],[3,4]],[[5,6]]]"),
            vec![vec![vec![1.0, 2.0], vec![3.0, 4.0]], vec![vec![5.0, 6.0]]]
        );
        assert_eq!(parse_tensorarray_f64("[[[9]]]"), vec![vec![vec![9.0]]]);
    }

    #[test]
    fn tensor_body_pads_to_dimension() {
        let body = pack_tensor_body(
            &[vec![1.0, 2.0], vec![3.0]],
            3,
            6, // Float32
        );
        assert_eq!(body.len(), 24); // 2 vectors * dim 3 * 4 bytes
        let vals: Vec<f32> = body
            .chunks_exact(4)
            .map(|c| f32::from_le_bytes(c.try_into().unwrap()))
            .collect();
        // The short vector is zero-padded to the declared dimension.
        assert_eq!(vals, vec![1.0, 2.0, 0.0, 3.0, 0.0, 0.0]);
    }

    #[test]
    fn varlen_slot_encodings() {
        // `u64 { count : 16; file_offset : 48 }`.
        let slot = varlen_count_offset_slot(3, 40);
        assert_eq!(u64::from_le_bytes(slot), 3 | (40 << 16));
        assert_eq!(slot.len(), 8);
        let max = varlen_count_offset_slot(0xffff, (1u64 << 48) - 1);
        assert_eq!(u64::from_le_bytes(max), u64::MAX);
        // Sparse: i64 nnz + i64 offset.
        let sparse = sparse_slot(5, 1234);
        assert_eq!(i64::from_le_bytes(sparse[..8].try_into().unwrap()), 5);
        assert_eq!(u64::from_le_bytes(sparse[8..].try_into().unwrap()), 1234);
    }

    #[test]
    fn embedding_element_roundtrip() {
        for (code, values) in [
            (1, vec![1.0, 255.0]),    // UInt8
            (3, vec![-5.0, 300.0]),   // Int16
            (4, vec![-5.0, 70000.0]), // Int32
            (6, vec![1.5, -2.25]),    // Float32
            (7, vec![1.5, -2.25]),    // Float64
            (8, vec![1.5, -2.25]),    // Float16
        ] {
            let mut bytes = Vec::new();
            for v in &values {
                push_embedding_element(&mut bytes, code, *v);
            }
            let width = element_width(code);
            assert_eq!(bytes.len(), values.len() * width, "code {code}");
            for (i, v) in values.iter().enumerate() {
                let got = read_embedding_element(&bytes[i * width..(i + 1) * width], code).unwrap();
                // int8/int16/int32 truncate to the element width; f16 rounds.
                let expect = match code {
                    1 => v.rem_euclid(256.0),
                    3 => (*v as i16) as f64,
                    4 => (*v as i32) as f64,
                    8 => (*v as f32) as f64,
                    _ => *v,
                };
                if code == 8 {
                    assert!(
                        (got - expect).abs() < 0.01,
                        "code {code} got {got} want {expect}"
                    );
                } else {
                    assert_eq!(got, expect, "code {code}");
                }
            }
        }
    }
}
