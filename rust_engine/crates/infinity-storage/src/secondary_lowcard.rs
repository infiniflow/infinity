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
//! the writer below reproduces that byte for byte.

use std::path::Path;

use crate::cpp_format::FormatError;

const SERIAL_COOKIE_NO_RUNCONTAINER: u32 = 12346;
const SERIAL_COOKIE: u32 = 12347;
const NO_OFFSET_THRESHOLD: usize = 4;
const ARRAY_MAX_CARD: usize = 4096;
const BITSET_SIZE: usize = 8192;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
enum ContainerKind {
    Array,
    Bitset,
    Run,
}

/// The maximal runs of consecutive values in an ascending container.
fn container_runs(values: &[u16]) -> Vec<(u16, u16)> {
    let mut runs = Vec::new();
    let mut start = values[0];
    let mut prev = values[0];
    for &v in &values[1..] {
        if v == prev + 1 {
            prev = v;
        } else {
            runs.push((start, prev - start));
            start = v;
            prev = v;
        }
    }
    runs.push((start, prev - start));
    runs
}

/// Encodes `values` (ascending, distinct) exactly as `RunOptimize()` followed
/// by `roaring::Roaring::write` does (`ra_portable_serialize`,
/// `convert_run_optimize` in CRoaring 4.7).
fn encode_roaring(values: &[u32]) -> Vec<u8> {
    // Group by the high 16 bits.
    let mut containers: Vec<(u16, Vec<u16>)> = Vec::new();
    for &v in values {
        let key = (v >> 16) as u16;
        let low = (v & 0xffff) as u16;
        match containers.last_mut() {
            Some((k, list)) if *k == key => list.push(low),
            _ => containers.push((key, vec![low])),
        }
    }

    // Pick a representation per container: run containers win when strictly
    // smaller than the array/bitset container CRoaring currently holds.
    let mut kinds = Vec::with_capacity(containers.len());
    let mut blobs: Vec<Vec<u8>> = Vec::with_capacity(containers.len());
    for (_, list) in &containers {
        let card = list.len();
        let runs = container_runs(list);
        let size_as_run = 2 + runs.len() * 4;
        let size_current = if card <= ARRAY_MAX_CARD {
            card * 2
        } else {
            BITSET_SIZE
        };
        if size_as_run < size_current {
            let mut blob = Vec::with_capacity(size_as_run);
            blob.extend_from_slice(&(runs.len() as u16).to_le_bytes());
            for (start, length) in &runs {
                blob.extend_from_slice(&start.to_le_bytes());
                blob.extend_from_slice(&length.to_le_bytes());
            }
            kinds.push(ContainerKind::Run);
            blobs.push(blob);
        } else if card <= ARRAY_MAX_CARD {
            let mut blob = Vec::with_capacity(card * 2);
            for v in list {
                blob.extend_from_slice(&v.to_le_bytes());
            }
            kinds.push(ContainerKind::Array);
            blobs.push(blob);
        } else {
            let mut words = [0u64; 1024];
            for v in list {
                words[*v as usize / 64] |= 1u64 << (*v as usize % 64);
            }
            let mut blob = Vec::with_capacity(BITSET_SIZE);
            for word in words {
                blob.extend_from_slice(&word.to_le_bytes());
            }
            kinds.push(ContainerKind::Bitset);
            blobs.push(blob);
        }
    }

    let n = containers.len();
    let has_run = kinds.contains(&ContainerKind::Run);
    let mut out = Vec::new();
    if has_run {
        let cookie = SERIAL_COOKIE | ((n as u32 - 1) << 16);
        out.extend_from_slice(&cookie.to_le_bytes());
        let run_bitmap_len = n.div_ceil(8);
        let mut run_bitmap = vec![0u8; run_bitmap_len];
        for (i, kind) in kinds.iter().enumerate() {
            if *kind == ContainerKind::Run {
                run_bitmap[i / 8] |= 1 << (i % 8);
            }
        }
        out.extend_from_slice(&run_bitmap);
    } else {
        out.extend_from_slice(&SERIAL_COOKIE_NO_RUNCONTAINER.to_le_bytes());
        out.extend_from_slice(&(n as u32).to_le_bytes());
    }

    for (key, list) in &containers {
        out.extend_from_slice(&key.to_le_bytes());
        out.extend_from_slice(&((list.len() as u16).wrapping_sub(1)).to_le_bytes());
    }

    let header = out.len();
    let offsets_len = if !has_run || n >= NO_OFFSET_THRESHOLD {
        n * 4
    } else {
        0
    };
    let mut at = (header + offsets_len) as u32;
    let mut offsets = Vec::with_capacity(n);
    for blob in &blobs {
        offsets.push(at);
        at += blob.len() as u32;
    }
    if offsets_len > 0 {
        for o in offsets {
            out.extend_from_slice(&o.to_le_bytes());
        }
    }
    for blob in &blobs {
        out.extend_from_slice(blob);
    }
    out
}

/// Decodes a portable bitmap, accepting both CRoaring cookies (with and
/// without run containers) and every container type.
fn decode_roaring(buf: &[u8]) -> Result<Vec<u32>, FormatError> {
    let read_u16 = |o: usize| -> Result<u16, FormatError> {
        if o + 2 > buf.len() {
            return Err(FormatError::Truncated {
                need: o + 2,
                got: buf.len(),
            });
        }
        Ok(u16::from_le_bytes(buf[o..o + 2].try_into().unwrap()))
    };
    let read_u32 = |o: usize| -> Result<u32, FormatError> {
        if o + 4 > buf.len() {
            return Err(FormatError::Truncated {
                need: o + 4,
                got: buf.len(),
            });
        }
        Ok(u32::from_le_bytes(buf[o..o + 4].try_into().unwrap()))
    };

    let cookie = read_u32(0)?;
    let (n, run_bitmap): (usize, Option<&[u8]>) = if cookie == SERIAL_COOKIE_NO_RUNCONTAINER {
        (read_u32(4)? as usize, None)
    } else if cookie & 0xffff == SERIAL_COOKIE {
        let n = ((cookie >> 16) & 0xffff) as usize + 1;
        let len = n.div_ceil(8);
        if 4 + len > buf.len() {
            return Err(FormatError::Truncated {
                need: 4 + len,
                got: buf.len(),
            });
        }
        (n, Some(&buf[4..4 + len]))
    } else {
        return Err(FormatError::BadMagic(cookie as u64));
    };

    let has_run = run_bitmap.is_some();
    let descs_at = if has_run { 4 + n.div_ceil(8) } else { 8 };
    let offsets_at = descs_at + n * 4;
    if offsets_at > buf.len() {
        return Err(FormatError::Truncated {
            need: offsets_at,
            got: buf.len(),
        });
    }
    let with_offsets = !has_run || n >= NO_OFFSET_THRESHOLD;
    let data_at = offsets_at + if with_offsets { n * 4 } else { 0 };
    if data_at > buf.len() {
        return Err(FormatError::Truncated {
            need: data_at,
            got: buf.len(),
        });
    }

    let mut out = Vec::new();
    let mut cursor = data_at;
    for i in 0..n {
        let desc = descs_at + i * 4;
        let key = read_u16(desc)? as u32;
        let card = read_u16(desc + 2)? as usize + 1;
        let is_run = run_bitmap.is_some_and(|bits| bits[i / 8] & (1 << (i % 8)) != 0);
        if with_offsets {
            let off = read_u32(offsets_at + i * 4)? as usize;
            if off < data_at || off > buf.len() {
                return Err(FormatError::Truncated {
                    need: off,
                    got: buf.len(),
                });
            }
            cursor = off;
        }

        if is_run {
            if cursor + 2 > buf.len() {
                return Err(FormatError::Truncated {
                    need: cursor + 2,
                    got: buf.len(),
                });
            }
            let n_runs = read_u16(cursor)? as usize;
            cursor += 2;
            for _ in 0..n_runs {
                let start = read_u16(cursor)? as u32;
                let length = read_u16(cursor + 2)? as u32;
                cursor += 4;
                for v in start..=start + length {
                    out.push((key << 16) | v);
                }
            }
        } else if card <= ARRAY_MAX_CARD {
            for j in 0..card {
                let low = read_u16(cursor + j * 2)? as u32;
                out.push((key << 16) | low);
            }
            cursor += card * 2;
        } else {
            if cursor + BITSET_SIZE > buf.len() {
                return Err(FormatError::Truncated {
                    need: cursor + BITSET_SIZE,
                    got: buf.len(),
                });
            }
            for w in 0..1024 {
                let word =
                    u64::from_le_bytes(buf[cursor + w * 8..cursor + w * 8 + 8].try_into().unwrap());
                for b in 0..64 {
                    if word & (1u64 << b) != 0 {
                        out.push((key << 16) | ((w * 64 + b) as u32));
                    }
                }
            }
            cursor += BITSET_SIZE;
        }
    }
    Ok(out)
}

/// Writes one low-cardinality chunk for `(key, offsets)` pairs sorted by key.
pub fn write_lowcard_segment(path: &Path, entries: &[(u64, Vec<u32>)]) -> Result<(), FormatError> {
    let mut out = Vec::new();
    out.extend_from_slice(&(entries.len() as u32).to_le_bytes());
    for (key, _) in entries {
        out.extend_from_slice(&key.to_le_bytes());
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
        let bitmap = encode_roaring(&sorted);
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

/// A low-cardinality chunk mapped read-only, keyed by the ordered key.
pub struct LowCardReader {
    map: Vec<u8>,
    count: usize,
}

impl LowCardReader {
    pub fn open(path: &Path) -> Result<Self, FormatError> {
        let map = crate::cpp_format::read_index_file(path)?;
        if map.len() < 4 {
            return Err(FormatError::Truncated {
                need: 4,
                got: map.len(),
            });
        }
        let count = u32::from_le_bytes(map[0..4].try_into().unwrap()) as usize;
        Ok(Self { map, count })
    }

    pub fn len(&self) -> usize {
        self.count
    }

    pub fn is_empty(&self) -> bool {
        self.count == 0
    }

    fn key(&self, i: usize) -> u64 {
        let at = 4 + i * 8;
        u64::from_le_bytes(self.map[at..at + 8].try_into().unwrap())
    }

    /// The ascending unique keys (`GetUniqueKeysPtr`); `None` when empty, as
    /// the reference's `key_ptr_` stays null for an empty index.
    pub fn unique_keys(&self) -> Option<Vec<u64>> {
        if self.count == 0 {
            None
        } else {
            Some((0..self.count).map(|i| self.key(i)).collect())
        }
    }

    /// All offsets stored for `key`.
    pub fn lookup(&self, key: u64) -> Vec<u32> {
        let (mut lo, mut hi) = (0usize, self.count);
        while lo < hi {
            let mid = (lo + hi) / 2;
            if self.key(mid) < key {
                lo = mid + 1;
            } else {
                hi = mid;
            }
        }
        if lo >= self.count || self.key(lo) != key {
            return Vec::new();
        }
        // Walk to this key's bitmap: every earlier entry carries a size prefix.
        let mut at = 4 + self.count * 8;
        for _ in 0..lo {
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
        decode_roaring(&self.map[start..end]).unwrap_or_default()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::secondary_index_data::{
        ordered_f64, ordered_i64, HighCardinalityIndex, LowCardinalityIndex,
    };
    use std::collections::BTreeSet;

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

    fn bitmap_offsets(bitmap: &BTreeSet<u32>) -> Vec<u32> {
        bitmap.iter().copied().collect()
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
        assert_eq!(decode_roaring(&encode_roaring(&small)).unwrap(), small);
        let dense: Vec<u32> = (0..10000).collect();
        assert_eq!(decode_roaring(&encode_roaring(&dense)).unwrap(), dense);
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
            assert_eq!(bitmap.len(), expected.len());
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
        assert_eq!(bitmap.len(), CHUNK_ROW_COUNT as usize);
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
            assert_eq!(bitmap.len(), expected_count);
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
}
