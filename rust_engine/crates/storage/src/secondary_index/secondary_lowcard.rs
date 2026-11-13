//! Low-cardinality secondary-index chunk: unique keys plus one bitmap of
//! offsets per key (`SecondaryIndexDataLowCardinalityT::SaveIndexInner`,
//! `secondary_index_data_impl.cpp:298`).
//!
//! ```text
//! u32 unique_key_count
//! u64 keys[unique_key_count]        (the reference's OrderedKeyType)
//! per key: i32 bitmap_size, then Bitmap::WriteAdv's bytes
//! ```
//!
//! `secondary_index_data_impl.cpp:321` sizes each write with
//! `Bitmap::GetSizeInBytes()` and then fills it with `Bitmap::WriteAdv`
//! (`roaring_bitmap.cppm:283`, `:298`). For `Bitmap = RoaringBitmap<false>`
//! that is:
//!
//! ```text
//! u32 count_        the row universe of the chunk (max offset + 1)
//! i32 size          bytes of the portable bitmap that follow
//! portable bitmap   roaring::Roaring::write (CRoaring portable format)
//! ```
//!
//! `count_` is the value the bitmap was constructed with: `InsertData` builds
//! `Bitmap bitmap(max_offset + 1)` over the whole chunk
//! (`secondary_index_data_impl.cpp:433-444`), so every bitmap in one chunk
//! carries the same `count_`. `GetSizeInBytes()` also calls `RunOptimize()`
//! first, so the portable image is CRoaring's post-run-optimisation encoding;
//! the portable payload is written and read by the `roaring` crate, whose
//! format is the [RoaringFormatSpec] CRoaring implements.

use crate::mapped::MappedFile;
use memmap2::Mmap;
use roaring::RoaringBitmap;
use std::fs::File;
use std::path::Path;

use crate::format::FormatError;
use crate::secondary_segment::{Key, KeyKind};

/// `Bitmap::WriteAdv` (`roaring_bitmap.cppm:298`): `u32 count_`, `i32 size`,
/// then the CRoaring portable image (`roaring::Roaring::write`).
///
/// `GetSizeInBytes` run-optimises first, so the image is the post-`RunOptimize`
/// encoding; `roaring::RoaringBitmap::optimize()` is that same step, and
/// `serialize_into` writes the [RoaringFormatSpec] image CRoaring reads.
///
/// [RoaringFormatSpec]: https://github.com/RoaringBitmap/RoaringFormatSpec
fn encode_bitmap(offsets: &[u32]) -> Vec<u8> {
    let mut bitmap: RoaringBitmap = offsets.iter().copied().collect();
    bitmap.optimize();
    let mut out = Vec::with_capacity(bitmap.serialized_size());
    bitmap
        .serialize_into(&mut out)
        .expect("writing a bitmap into a Vec cannot fail");
    out
}

/// The offsets of a portable bitmap (both CRoaring cookies, every container).
fn decode_bitmap(buf: &[u8]) -> Result<Vec<u32>, FormatError> {
    let bitmap = RoaringBitmap::deserialize_from(buf)
        .map_err(|e| FormatError::Io(format!("roaring bitmap: {e}")))?;
    Ok(bitmap.iter().collect())
}

/// A low-cardinality chunk key.
///
/// The reference's key type is `ConvertToOrderedType<RawValueType>`, which is
/// one of the fixed-width integers/floats, `bool`, or `JsonTermT` (a term
/// string). `Bytes` is that term.
// `Key` holds `f32`/`f64`, so equality is `PartialEq` (a NaN key never equals
// itself); ordering goes through [`LowCardKey::compare`], which is total.
#[derive(Debug, Clone, PartialEq)]
pub enum LowCardKey {
    Fixed(Key),
    /// `JsonTermT`: `u32 len` + the term's UTF-8 bytes in the chunk.
    Bytes(Vec<u8>),
}

impl From<u64> for LowCardKey {
    fn from(value: u64) -> Self {
        Self::Fixed(Key::U64(value))
    }
}

impl From<Key> for LowCardKey {
    fn from(key: Key) -> Self {
        Self::Fixed(key)
    }
}

impl From<&LowCardKey> for LowCardKey {
    fn from(key: &LowCardKey) -> Self {
        key.clone()
    }
}

impl LowCardKey {
    pub fn fixed(key: Key) -> Self {
        Self::Fixed(key)
    }

    pub fn bytes(text: impl Into<Vec<u8>>) -> Self {
        Self::Bytes(text.into())
    }

    fn compare(&self, other: &LowCardKey) -> std::cmp::Ordering {
        match (self, other) {
            (LowCardKey::Bytes(a), LowCardKey::Bytes(b)) => a.cmp(b),
            (LowCardKey::Fixed(a), LowCardKey::Fixed(b)) => a.order(b),
            // A caller that mixes the two is comparing across key domains; keep
            // the order total so a binary search still terminates.
            (LowCardKey::Fixed(_), LowCardKey::Bytes(_)) => std::cmp::Ordering::Less,
            (LowCardKey::Bytes(_), LowCardKey::Fixed(_)) => std::cmp::Ordering::Greater,
        }
    }
}

/// `sizeof(OrderedKeyType)` bytes of `key`, or the `u32 len` + bytes of a term.
fn push_lowcard_key(out: &mut Vec<u8>, key: &LowCardKey) -> Result<(), FormatError> {
    match key {
        LowCardKey::Bytes(bytes) => {
            out.extend_from_slice(&(bytes.len() as u32).to_le_bytes());
            out.extend_from_slice(bytes);
        }
        LowCardKey::Fixed(Key::Bool(flag)) => out.push(u8::from(*flag)),
        LowCardKey::Fixed(Key::I8(value)) => out.push(*value as u8),
        LowCardKey::Fixed(Key::I16(value)) => out.extend_from_slice(&value.to_le_bytes()),
        LowCardKey::Fixed(Key::I32(value)) => out.extend_from_slice(&value.to_le_bytes()),
        LowCardKey::Fixed(Key::I64(value)) => out.extend_from_slice(&value.to_le_bytes()),
        LowCardKey::Fixed(Key::F32(value)) => out.extend_from_slice(&value.to_le_bytes()),
        LowCardKey::Fixed(Key::F64(value)) => out.extend_from_slice(&value.to_le_bytes()),
        LowCardKey::Fixed(Key::U64(value)) => out.extend_from_slice(&value.to_le_bytes()),
    }
    Ok(())
}

/// Writes one low-cardinality chunk for `(key, offsets)` pairs sorted by key.
///
/// The key array is `unique_key_count_` followed by the keys in the
/// reference's own layout (`SaveIndexInner`, `secondary_index_data_impl.cpp:
/// 298-328`): fixed-width keys concatenated, or `u32 len` + bytes for a JSON
/// term. Every key's posting list follows as `i32 bitmap_size` + a portable
/// roaring bitmap, all sized against the same `max_offset + 1` universe.
pub fn write_lowcard_segment_typed(
    path: &Path,
    entries: &[(LowCardKey, Vec<u32>)],
) -> Result<(), FormatError> {
    let mut out = Vec::new();
    out.extend_from_slice(&(entries.len() as u32).to_le_bytes());
    for (key, _) in entries {
        push_lowcard_key(&mut out, key)?;
    }
    // `InsertData` sizes every bitmap with the chunk-wide row universe
    // (`max_offset + 1`), so all `count_` prefixes in a chunk are equal.
    let max_offset = entries
        .iter()
        .flat_map(|(_, offsets)| offsets.iter().copied())
        .max()
        .unwrap_or(0);
    let count = max_offset.wrapping_add(1);
    for (_, offsets) in entries {
        let mut sorted = offsets.clone();
        sorted.sort_unstable();
        sorted.dedup();
        let bitmap = encode_bitmap(&sorted);
        // `bitmap_size = GetSizeInBytes() = sizeof(count_) + sizeof(i32) + portable`.
        let bitmap_size = 4 + 4 + bitmap.len();
        out.extend_from_slice(&(bitmap_size as i32).to_le_bytes());
        out.extend_from_slice(&count.to_le_bytes());
        out.extend_from_slice(&(bitmap.len() as i32).to_le_bytes());
        out.extend_from_slice(&bitmap);
    }
    if let Some(parent) = path.parent() {
        std::fs::create_dir_all(parent)
            .map_err(|e| FormatError::Io(format!("{}: {e}", parent.display())))?;
    }
    std::fs::write(path, out).map_err(|e| FormatError::Io(format!("{}: {e}", path.display())))
}

/// Writes one low-cardinality chunk whose keys are `u64` (the hashed VARCHAR
/// key, or the port's order-mapped integer key). Kept as the short form of
/// [`write_lowcard_segment_typed`] for callers that already hold `u64`s.
pub fn write_lowcard_segment(path: &Path, entries: &[(u64, Vec<u32>)]) -> Result<(), FormatError> {
    let entries: Vec<(LowCardKey, Vec<u32>)> = entries
        .iter()
        .map(|(key, offsets)| (LowCardKey::Fixed(Key::U64(*key)), offsets.clone()))
        .collect();
    write_lowcard_segment_typed(path, &entries)
}

/// A low-cardinality chunk mapped read-only.
///
/// The chunk carries no key-kind header, so the reader is told the kind (from
/// the column's declared type, or `U64` for the port's hashed/order-mapped
/// chunks).
pub struct LowCardReader {
    map: MappedFile,
    count: usize,
    kind: KeyKind,
    /// Start byte of each key plus one past the last, for a variable-length
    /// (`Bytes`) key array; empty for a fixed-width one.
    key_offsets: Vec<u32>,
}

impl LowCardReader {
    pub fn open(path: &Path) -> Result<Self, FormatError> {
        Self::open_with_kind(path, KeyKind::U64)
    }

    #[allow(unsafe_code)]
    pub fn open_with_kind(path: &Path, kind: KeyKind) -> Result<Self, FormatError> {
        let file =
            File::open(path).map_err(|e| FormatError::Io(format!("{}: {e}", path.display())))?;
        // SAFETY: read-only mapping of an immutable chunk file.
        let map = MappedFile::from_mmap(
            unsafe { Mmap::map(&file) }.map_err(|e| FormatError::Io(e.to_string()))?,
        );
        if map.len() < 4 {
            return Err(FormatError::Truncated {
                need: 4,
                got: map.len(),
            });
        }
        let count = u32::from_le_bytes(map[0..4].try_into().unwrap()) as usize;
        let mut key_offsets = Vec::new();
        if kind == KeyKind::Bytes {
            // A term array is `[u32 len][bytes]…`, so the only way to index it
            // is a prefix pass; it runs once, at open.
            key_offsets.reserve(count + 1);
            key_offsets.push(4u32);
            let mut at = 4usize;
            for _ in 0..count {
                if at + 4 > map.len() {
                    return Err(FormatError::Truncated {
                        need: at + 4,
                        got: map.len(),
                    });
                }
                let len = u32::from_le_bytes(map[at..at + 4].try_into().unwrap()) as usize;
                at += 4 + len;
                if at > map.len() {
                    return Err(FormatError::Truncated {
                        need: at,
                        got: map.len(),
                    });
                }
                key_offsets.push(at as u32);
            }
        }
        Ok(Self {
            map,
            count,
            kind,
            key_offsets,
        })
    }

    pub fn len(&self) -> usize {
        self.count
    }

    pub fn is_empty(&self) -> bool {
        self.count == 0
    }

    /// The kind this chunk was opened with.
    pub fn kind(&self) -> KeyKind {
        self.kind
    }

    /// Bytes taken by the key array, after the four-byte count.
    fn keys_len(&self) -> usize {
        match self.kind {
            KeyKind::Bytes => self
                .key_offsets
                .last()
                .map(|end| *end as usize - 4)
                .unwrap_or(0),
            kind => self.count * kind.key_width(),
        }
    }

    fn fixed_key(&self, i: usize) -> Key {
        let at = 4 + i * self.kind.key_width();
        match self.kind {
            KeyKind::U64 => Key::U64(u64::from_le_bytes(self.map[at..at + 8].try_into().unwrap())),
            KeyKind::I64 => Key::I64(i64::from_le_bytes(self.map[at..at + 8].try_into().unwrap())),
            KeyKind::F64 => Key::F64(f64::from_bits(u64::from_le_bytes(
                self.map[at..at + 8].try_into().unwrap(),
            ))),
            KeyKind::I32 => Key::I32(i32::from_le_bytes(self.map[at..at + 4].try_into().unwrap())),
            KeyKind::F32 => Key::F32(f32::from_bits(u32::from_le_bytes(
                self.map[at..at + 4].try_into().unwrap(),
            ))),
            KeyKind::I16 => Key::I16(i16::from_le_bytes(self.map[at..at + 2].try_into().unwrap())),
            KeyKind::I8 => Key::I8(self.map[at] as i8),
            KeyKind::Bool => Key::Bool(self.map[at] != 0),
            KeyKind::Bytes => Key::U64(0),
        }
    }

    fn bytes_key(&self, i: usize) -> &[u8] {
        let start = self.key_offsets[i] as usize + 4;
        let end = self.key_offsets[i + 1] as usize;
        &self.map[start..end]
    }

    /// The stored key at `i`.
    pub fn key_at(&self, i: usize) -> Option<LowCardKey> {
        if i >= self.count {
            return None;
        }
        Some(match self.kind {
            KeyKind::Bytes => LowCardKey::Bytes(self.bytes_key(i).to_vec()),
            _ => LowCardKey::Fixed(self.fixed_key(i)),
        })
    }

    fn compare_at(&self, i: usize, key: &LowCardKey) -> std::cmp::Ordering {
        match (self.kind, key) {
            (KeyKind::Bytes, LowCardKey::Bytes(want)) => self.bytes_key(i).cmp(want.as_slice()),
            // Cross-domain probe: keep the order total so the search ends.
            (KeyKind::Bytes, LowCardKey::Fixed(_)) => std::cmp::Ordering::Greater,
            (_, LowCardKey::Bytes(_)) => std::cmp::Ordering::Less,
            (_, LowCardKey::Fixed(want)) => self.fixed_key(i).order(want),
        }
    }

    /// First index whose key is `>= key`.
    fn lower_bound(&self, key: &LowCardKey) -> usize {
        let (mut lo, mut hi) = (0usize, self.count);
        while lo < hi {
            let mid = (lo + hi) / 2;
            if self.compare_at(mid, key) == std::cmp::Ordering::Less {
                lo = mid + 1;
            } else {
                hi = mid;
            }
        }
        lo
    }

    /// First index whose key is `> key`.
    fn upper_bound(&self, key: &LowCardKey) -> usize {
        let (mut lo, mut hi) = (0usize, self.count);
        while lo < hi {
            let mid = (lo + hi) / 2;
            if self.compare_at(mid, key) == std::cmp::Ordering::Greater {
                hi = mid;
            } else {
                lo = mid + 1;
            }
        }
        lo
    }

    /// The ascending unique keys (`GetUniqueKeysPtr`) of a `u64` chunk; `None`
    /// when empty or for any other kind.
    pub fn unique_keys(&self) -> Option<Vec<u64>> {
        if self.count == 0 || self.kind != KeyKind::U64 {
            return None;
        }
        Some(
            (0..self.count)
                .map(|i| match self.fixed_key(i) {
                    Key::U64(value) => value,
                    _ => 0,
                })
                .collect(),
        )
    }

    /// The ascending unique keys, whatever the kind.
    pub fn unique_keys_typed(&self) -> Option<Vec<LowCardKey>> {
        if self.count == 0 {
            None
        } else {
            Some((0..self.count).filter_map(|i| self.key_at(i)).collect())
        }
    }

    /// All offsets stored for `key`. Accepts a `u64` (the port's order-mapped
    /// key), a [`Key`], or a [`LowCardKey`] by value or reference.
    pub fn lookup(&self, key: impl Into<LowCardKey>) -> Vec<u32> {
        let key = key.into();
        let lo = self.lower_bound(&key);
        if lo >= self.count || self.compare_at(lo, &key) != std::cmp::Ordering::Equal {
            return Vec::new();
        }
        self.bitmap_at(lo)
    }

    /// All offsets whose key lies in the inclusive range `[lo, hi]`.
    pub fn range(&self, lo: impl Into<LowCardKey>, hi: impl Into<LowCardKey>) -> Vec<u32> {
        let (lo, hi) = (lo.into(), hi.into());
        if self.count == 0 || lo.compare(&hi) == std::cmp::Ordering::Greater {
            return Vec::new();
        }
        let begin = self.lower_bound(&lo);
        let stop = self.upper_bound(&hi);
        let mut out = Vec::new();
        for i in begin..stop {
            out.extend(self.bitmap_at(i));
        }
        out
    }

    /// Decodes the posting list of key `i`.
    ///
    /// Walk to it: every earlier entry carries an `i32` size prefix.
    fn bitmap_at(&self, i: usize) -> Vec<u32> {
        let mut at = 4 + self.keys_len();
        for _ in 0..i {
            if at + 4 > self.map.len() {
                return Vec::new();
            }
            let size = i32::from_le_bytes(self.map[at..at + 4].try_into().unwrap()) as usize;
            at += 4 + size;
        }
        // `i32 bitmap_size`, then `WriteAdv`: `u32 count_`, `i32 size`, bitmap.
        if at + 12 > self.map.len() {
            return Vec::new();
        }
        let bitmap_size = i32::from_le_bytes(self.map[at..at + 4].try_into().unwrap()) as usize;
        let size = i32::from_le_bytes(self.map[at + 8..at + 12].try_into().unwrap()) as usize;
        let start = at + 12;
        let end = (start + size).min(at + 4 + bitmap_size).min(self.map.len());
        decode_bitmap(&self.map[start..end]).unwrap_or_default()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::secondary_index_data::{
        ordered_f64, ordered_i64, HighCardinalityIndex, LowCardinalityIndex,
    };

    /// `CreateLowCardinalityData<T>`: `row_count` rows whose key cycles over
    /// `unique_values` distinct values; the segment offset is the row index.
    /// The result is stable-sorted by key, matching `std::multimap` iteration.
    fn create_low_cardinality_data(
        row_count: u32,
        unique_values: u32,
        encode: impl Fn(u32) -> u64,
    ) -> Vec<(u64, u32)> {
        let mut data: Vec<(u64, u32)> = (0..row_count)
            .map(|i| (encode(i % unique_values), i))
            .collect();
        data.sort_by_key(|(key, _)| *key);
        data
    }

    fn bitmap_offsets(bitmap: &RoaringBitmap) -> Vec<u32> {
        bitmap.iter().collect()
    }

    fn expected_offsets(data: &[(u64, u32)], key: u64) -> Vec<u32> {
        data.iter()
            .filter(|(data_key, _)| *data_key == key)
            .map(|(_, offset)| *offset)
            .collect()
    }

    #[test]
    fn roaring_roundtrip_small_and_dense() {
        let small = vec![1, 5, 9, 70000];
        assert_eq!(decode_bitmap(&encode_bitmap(&small)).unwrap(), small);
        let dense: Vec<u32> = (0..10000).collect();
        assert_eq!(decode_bitmap(&encode_bitmap(&dense)).unwrap(), dense);
    }

    /// Bytes of `Bitmap::WriteAdv` for the offsets `{3, 7}` with
    /// `count_ = 8`, captured from CRoaring 4.7 (`GetSizeInBytes` +
    /// `roaring::Roaring::write`): `u32 count_`, `i32 size`, portable image.
    const CPP_WRITE_ADV_ARRAY: &[u8] = &[
        8, 0, 0, 0, 20, 0, 0, 0, 0x3a, 0x30, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0x10, 0, 0, 0, 3, 0, 7,
        0,
    ];

    /// `Bitmap::WriteAdv` for the offsets `0..10000`, which CRoaring stores as
    /// a single run container (`count_ = 10000`, portable size 15).
    const CPP_WRITE_ADV_RUN: &[u8] = &[
        0x10, 0x27, 0, 0, 15, 0, 0, 0, 0x3b, 0x30, 0, 0, 1, 0, 0, 0x0f, 0x27, 1, 0, 0, 0, 0x0f,
        0x27,
    ];

    /// The writer must emit `SaveIndexInner`'s exact framing and bytes.
    #[test]
    fn write_adv_framing_matches_cpp() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("framing.idx");

        write_lowcard_segment(&path, &[(42u64, vec![3, 7])]).unwrap();
        let mut expected = Vec::new();
        expected.extend_from_slice(&1u32.to_le_bytes());
        expected.extend_from_slice(&42u64.to_le_bytes());
        // bitmap_size = GetSizeInBytes() = 4 + 4 + portable, then WriteAdv.
        expected.extend_from_slice(&(28i32).to_le_bytes());
        expected.extend_from_slice(CPP_WRITE_ADV_ARRAY);
        assert_eq!(std::fs::read(&path).unwrap(), expected);

        write_lowcard_segment(&path, &[(7u64, (0..10000).collect())]).unwrap();
        let mut expected = Vec::new();
        expected.extend_from_slice(&1u32.to_le_bytes());
        expected.extend_from_slice(&7u64.to_le_bytes());
        expected.extend_from_slice(&(23i32).to_le_bytes());
        expected.extend_from_slice(CPP_WRITE_ADV_RUN);
        assert_eq!(std::fs::read(&path).unwrap(), expected);
    }

    /// A chunk written by the reference (`SaveIndexInner`) must decode.
    #[test]
    fn reads_cpp_shaped_chunk() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("cpp.idx");
        let key = 0x0123_4567_89ab_cdefu64;
        let mut chunk = Vec::new();
        chunk.extend_from_slice(&1u32.to_le_bytes());
        chunk.extend_from_slice(&key.to_le_bytes());
        chunk.extend_from_slice(&28i32.to_le_bytes());
        chunk.extend_from_slice(CPP_WRITE_ADV_ARRAY);
        std::fs::write(&path, &chunk).unwrap();

        let reader = LowCardReader::open(&path).unwrap();
        assert_eq!(reader.len(), 1);
        assert_eq!(reader.lookup(key), vec![3, 7]);
        assert!(reader.lookup(key + 1).is_empty());

        // A run-optimised image (a shape only CRoaring's `RunOptimize` emits)
        // must decode as well.
        let mut chunk = Vec::new();
        chunk.extend_from_slice(&1u32.to_le_bytes());
        chunk.extend_from_slice(&key.to_le_bytes());
        chunk.extend_from_slice(&23i32.to_le_bytes());
        chunk.extend_from_slice(CPP_WRITE_ADV_RUN);
        std::fs::write(&path, &chunk).unwrap();

        let reader = LowCardReader::open(&path).unwrap();
        assert_eq!(reader.lookup(key), (0..10000).collect::<Vec<u32>>());
    }

    /// Every bitmap in a chunk carries the chunk-wide `max_offset + 1`.
    #[test]
    fn count_prefix_is_chunk_wide_row_universe() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("count.idx");
        write_lowcard_segment(&path, &[(1u64, vec![3, 7]), (5, vec![10, 11, 12])]).unwrap();
        let bytes = std::fs::read(&path).unwrap();
        let first = 4 + 2 * 8;
        let second =
            first + 4 + i32::from_le_bytes(bytes[first..first + 4].try_into().unwrap()) as usize;
        for at in [first, second] {
            let count = u32::from_le_bytes(bytes[at + 4..at + 8].try_into().unwrap());
            assert_eq!(count, 13, "count_ at {at}");
        }
        let reader = LowCardReader::open(&path).unwrap();
        assert_eq!(reader.lookup(5), vec![10, 11, 12]);
    }

    #[test]
    fn lowcard_segment_lookup() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_0.idx");
        let entries = vec![
            (1u64, vec![3u32, 7]),
            (5, vec![10, 11, 12]),
            (9, (0..5000).collect::<Vec<u32>>()),
        ];
        write_lowcard_segment(&path, &entries).unwrap();
        let reader = LowCardReader::open(&path).unwrap();
        assert_eq!(reader.len(), 3);
        assert_eq!(reader.lookup(1), vec![3, 7]);
        assert_eq!(reader.lookup(5), vec![10, 11, 12]);
        assert_eq!(reader.lookup(9).len(), 5000);
        assert!(reader.lookup(2).is_empty());
    }

    /// C++: `LowCardinalitySecondaryIndexTest.TestIntegerLowCardinality`.
    #[test]
    fn test_integer_low_cardinality() {
        const CHUNK_ROW_COUNT: u32 = 100;
        const UNIQUE_VALUES: u32 = 5;
        let test_data = create_low_cardinality_data(CHUNK_ROW_COUNT, UNIQUE_VALUES, |v| {
            ordered_i64(i64::from(v as i32))
        });

        let mut index = LowCardinalityIndex::new();
        index.insert_data(test_data.clone());
        assert_eq!(index.unique_key_count(), UNIQUE_VALUES);

        // `GetUniqueKeysPtr`: ascending unique keys.
        let unique_keys = index.unique_keys().expect("unique keys");
        assert_eq!(unique_keys.len(), UNIQUE_VALUES as usize);
        for pair in unique_keys.windows(2) {
            assert!(pair[0] < pair[1], "unique keys are not ascending");
        }

        // `GetOffsetsForKeyPtr`: one bitmap per key, and the saved chunk
        // preserves both the key array and every bitmap.
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("integer_low_cardinality.idx");
        index.save(&path).unwrap();
        let reader = LowCardReader::open(&path).unwrap();
        assert_eq!(reader.unique_keys(), Some(unique_keys.clone()));
        for i in 0..UNIQUE_VALUES {
            let key = ordered_i64(i64::from(i as i32));
            let bitmap = index.offsets_for_key(key).expect("bitmap");
            let expected = expected_offsets(&test_data, key);
            assert_eq!(bitmap.len() as usize, expected.len());
            assert_eq!(bitmap_offsets(bitmap), expected);
            assert_eq!(reader.lookup(key), expected);
        }
    }

    /// C++: `LowCardinalitySecondaryIndexTest.TestFloatLowCardinality`.
    #[test]
    fn test_float_low_cardinality() {
        const CHUNK_ROW_COUNT: u32 = 80;
        const UNIQUE_VALUES: u32 = 4;
        let unique_float_keys = [1.5f32, 2.5, 3.5, 4.5];
        let test_data = create_low_cardinality_data(CHUNK_ROW_COUNT, UNIQUE_VALUES, |v| {
            ordered_f64(f64::from(unique_float_keys[v as usize]))
        });

        let mut index = LowCardinalityIndex::new();
        index.insert_data(test_data.clone());
        assert_eq!(index.unique_key_count(), UNIQUE_VALUES);

        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("float_low_cardinality.idx");
        index.save(&path).unwrap();
        let reader = LowCardReader::open(&path).unwrap();
        for key in unique_float_keys {
            let encoded = ordered_f64(f64::from(key));
            let bitmap = index.offsets_for_key(encoded).expect("bitmap");
            let mut actual = bitmap_offsets(bitmap);
            let mut expected = expected_offsets(&test_data, encoded);
            actual.sort_unstable();
            expected.sort_unstable();
            assert_eq!(actual, expected);
            assert_eq!(reader.lookup(encoded), expected);
        }
    }

    /// C++: `LowCardinalitySecondaryIndexTest.TestEmptyIndex`.
    #[test]
    fn test_empty_low_cardinality_index() {
        // C++: `chunk_row_count = 0` with an empty multimap.
        let mut index = LowCardinalityIndex::new();
        index.insert_data(std::iter::empty());
        assert_eq!(index.unique_key_count(), 0);
        assert!(index.unique_keys().is_none());
    }

    /// C++: `LowCardinalitySecondaryIndexTest.TestSingleValueCardinality`.
    #[test]
    fn test_single_value_low_cardinality() {
        const CHUNK_ROW_COUNT: u32 = 50;
        let test_data =
            create_low_cardinality_data(CHUNK_ROW_COUNT, 1, |v| ordered_i64(i64::from(v as i16)));

        let mut index = LowCardinalityIndex::new();
        index.insert_data(test_data);
        assert_eq!(index.unique_key_count(), 1);

        let key = ordered_i64(0);
        let bitmap = index.offsets_for_key(key).expect("bitmap");
        assert_eq!(bitmap.len() as usize, CHUNK_ROW_COUNT as usize);
        assert_eq!(
            bitmap_offsets(bitmap),
            (0..CHUNK_ROW_COUNT).collect::<Vec<_>>()
        );
    }

    /// C++: `LowCardinalitySecondaryIndexTest.TestComparisonWithHighCardinality`.
    #[test]
    fn test_low_cardinality_vs_high_cardinality() {
        const CHUNK_ROW_COUNT: u32 = 100;
        const UNIQUE_VALUES: u32 = 5;
        let test_data = create_low_cardinality_data(CHUNK_ROW_COUNT, UNIQUE_VALUES, |v| {
            ordered_i64(i64::from(v as i32))
        });

        let mut high_card_index = HighCardinalityIndex::new();
        let mut low_card_index = LowCardinalityIndex::new();
        high_card_index.insert_data(test_data.clone());
        low_card_index.insert_data(test_data.clone());

        // High cardinality keeps no unique-key view (base-class defaults).
        assert_eq!(high_card_index.unique_key_count(), 0);
        assert!(high_card_index.unique_keys().is_none());

        // Low cardinality exposes the expected unique keys.
        assert_eq!(low_card_index.unique_key_count(), UNIQUE_VALUES);
        assert!(low_card_index.unique_keys().is_some());

        for i in 0..UNIQUE_VALUES {
            let key = ordered_i64(i64::from(i as i32));
            let bitmap = low_card_index.offsets_for_key(key).expect("bitmap");
            let expected_count = (CHUNK_ROW_COUNT / UNIQUE_VALUES) as usize;
            assert_eq!(bitmap.len() as usize, expected_count);
            assert_eq!(expected_offsets(&test_data, key).len(), expected_count);
        }
    }

    /// C++: `LowCardinalitySecondaryIndexTest.TestSaveLoadLowCardinality`.
    #[test]
    fn test_save_load_low_cardinality() {
        const CHUNK_ROW_COUNT: u32 = 1000;
        const UNIQUE_VALUES: u32 = 5;
        let test_data = create_low_cardinality_data(CHUNK_ROW_COUNT, UNIQUE_VALUES, |v| {
            ordered_i64(i64::from(v as i32))
        });

        let mut index = LowCardinalityIndex::new();
        index.insert_data(test_data.clone());
        let key_count = index.unique_key_count();

        // `SaveIndexInner` then `ReadIndexInner`.
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("test001.idx");
        index.save(&path).unwrap();
        let loaded = LowCardinalityIndex::load(&path).unwrap();
        assert_eq!(key_count, loaded.unique_key_count());
        assert_eq!(loaded.unique_key_count(), UNIQUE_VALUES);

        // The reloaded per-key bitmaps still contain exactly the original
        // segment offsets.
        for i in 0..UNIQUE_VALUES {
            let key = ordered_i64(i64::from(i as i32));
            assert_eq!(
                loaded.offsets_for_key(key),
                expected_offsets(&test_data, key)
            );
        }
    }

    /// C++: `LowCardinalitySecondaryIndexTest.TestAllDataType`.
    #[test]
    fn test_all_data_types_low_cardinality() {
        const CHUNK_ROW_COUNT: u32 = 500;
        const UNIQUE_VALUES: u32 = 10;
        // The reference maps BigIntT to i64, DoubleT to double, DateT/TimeT
        // to i32 and DateTimeT/TimestampT to i64.
        let cases: [(&str, Vec<(u64, u32)>); 6] = [
            (
                "kBigInt/BigIntT",
                create_low_cardinality_data(CHUNK_ROW_COUNT, UNIQUE_VALUES, |v| {
                    ordered_i64(i64::from(v))
                }),
            ),
            (
                "kDouble/DoubleT",
                create_low_cardinality_data(CHUNK_ROW_COUNT, UNIQUE_VALUES, |v| {
                    ordered_f64(f64::from(v))
                }),
            ),
            (
                "kDate/DateT",
                create_low_cardinality_data(CHUNK_ROW_COUNT, UNIQUE_VALUES, |v| {
                    ordered_i64(i64::from(v as i32))
                }),
            ),
            (
                "kTime/TimeT",
                create_low_cardinality_data(CHUNK_ROW_COUNT, UNIQUE_VALUES, |v| {
                    ordered_i64(i64::from(v as i32))
                }),
            ),
            (
                "kDateTime/DateTimeT",
                create_low_cardinality_data(CHUNK_ROW_COUNT, UNIQUE_VALUES, |v| {
                    ordered_i64(i64::from(v))
                }),
            ),
            (
                "kTimestamp/TimestampT",
                create_low_cardinality_data(CHUNK_ROW_COUNT, UNIQUE_VALUES, |v| {
                    ordered_i64(i64::from(v))
                }),
            ),
        ];

        for (data_type, test_data) in cases {
            let mut index = LowCardinalityIndex::new();
            index.insert_data(test_data);
            assert_eq!(
                index.unique_key_count(),
                UNIQUE_VALUES,
                "unexpected unique key count for {data_type}"
            );
            let keys = index.unique_keys().expect("unique keys");
            assert_eq!(keys.len(), UNIQUE_VALUES as usize, "{data_type}");
            for pair in keys.windows(2) {
                assert!(pair[0] < pair[1], "keys are not ascending for {data_type}");
            }
        }
    }

    /// A BOOL chunk's key array is one `bool` per unique value
    /// (`KeepOrderedSelf<BooleanT>`), not a widened `u64`.
    #[test]
    fn bool_keys_are_one_byte() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("bool.idx");
        let entries = vec![
            (LowCardKey::Fixed(Key::Bool(false)), vec![0u32, 2]),
            (LowCardKey::Fixed(Key::Bool(true)), vec![1u32]),
        ];
        write_lowcard_segment_typed(&path, &entries).unwrap();

        let bytes = std::fs::read(&path).unwrap();
        assert_eq!(&bytes[0..4], &2u32.to_le_bytes());
        // `unique_key_count_` then the keys themselves: `false`, `true`.
        assert_eq!(bytes[4], 0);
        assert_eq!(bytes[5], 1);

        let reader = LowCardReader::open_with_kind(&path, KeyKind::Bool).unwrap();
        assert_eq!(reader.kind(), KeyKind::Bool);
        assert_eq!(reader.lookup(Key::Bool(false)), vec![0, 2]);
        assert_eq!(reader.lookup(Key::Bool(true)), vec![1]);
        assert!(reader.lookup(Key::Bool(false)).contains(&2));
        assert_eq!(
            reader.unique_keys_typed().unwrap(),
            vec![
                LowCardKey::Fixed(Key::Bool(false)),
                LowCardKey::Fixed(Key::Bool(true)),
            ]
        );
    }

    /// A JSON chunk's keys are `u32 len` + term bytes, and their byte order is
    /// the term order - which is what makes a path prefix a range.
    #[test]
    fn json_term_keys_roundtrip() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("json.idx");
        let entries = vec![
            (LowCardKey::bytes("$.a:i:8000000000000001"), vec![0u32]),
            (
                LowCardKey::bytes("$.a[0]:i:8000000000000001"),
                vec![1u32, 2],
            ),
            (LowCardKey::bytes("$.b:s:x"), vec![3u32]),
        ];
        write_lowcard_segment_typed(&path, &entries).unwrap();

        let reader = LowCardReader::open_with_kind(&path, KeyKind::Bytes).unwrap();
        assert_eq!(
            reader.lookup(LowCardKey::bytes("$.a:i:8000000000000001")),
            vec![0]
        );
        assert_eq!(
            reader.lookup(LowCardKey::bytes("$.a[0]:i:8000000000000001")),
            vec![1, 2]
        );
        assert!(reader
            .lookup(LowCardKey::bytes("$.a:i:8000000000000002"))
            .is_empty());
        // `':'` sorts before `'['`, so `$.a:i:…` precedes `$.a[0]:…`; a prefix
        // range over `$.a` covers both and stops short of `$.b`.
        assert_eq!(
            reader.range(LowCardKey::bytes("$.a"), LowCardKey::bytes("$.a~")),
            vec![0, 1, 2]
        );
        assert_eq!(
            reader.range(LowCardKey::bytes("$.b"), LowCardKey::bytes("$.b~")),
            vec![3]
        );
    }

    /// A TINYINT chunk's key array is one byte per key, as the reference's
    /// `int8_t` key is - the width comes from the kind, not a constant.
    #[test]
    fn tinyint_low_cardinality_uses_one_byte_keys() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("i8.idx");
        let entries = vec![
            (LowCardKey::Fixed(Key::I8(-3)), vec![0u32, 1]),
            (LowCardKey::Fixed(Key::I8(7)), vec![2u32]),
        ];
        write_lowcard_segment_typed(&path, &entries).unwrap();
        let bytes = std::fs::read(&path).unwrap();
        assert_eq!(bytes[4] as i8, -3);
        assert_eq!(bytes[5] as i8, 7);

        let reader = LowCardReader::open_with_kind(&path, KeyKind::I8).unwrap();
        assert_eq!(reader.lookup(Key::I8(-3)), vec![0, 1]);
        assert_eq!(reader.lookup(Key::I8(7)), vec![2]);
        assert_eq!(reader.range(Key::I8(-10), Key::I8(0)), vec![0, 1]);
    }
}
