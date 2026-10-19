//! Low-cardinality secondary-index chunk: unique keys plus one bitmap of
//! offsets per key (`SecondaryIndexDataLowCardinalityT::SaveIndexInner`,
//! `secondary_index_data_impl.cpp:298`).
//!
//! ```text
//! u32 unique_key_count
//! u64 keys[unique_key_count]        (the reference's OrderedKeyType)
//! per key: i32 bitmap_size, then the portable Roaring bytes
//! ```
//!
//! `RoaringBitmap::WriteAdv` writes `u32 count`, `i32 size`, then
//! `roaring::Roaring::write` (`roaring_bitmap.cppm:298`). The portable layout
//! used here is the no-run form CRoaring reads:
//!
//! ```text
//! u32 cookie = 12346, u32 container_count
//! per container: u16 key, u16 (cardinality - 1)
//! per container: u32 offset (from the start of the container data)
//! containers: u16[card] for card <= 4096, else 8192 bytes of bitset
//! ```

use memmap2::Mmap;
use std::fs::File;
use std::path::Path;

use crate::cpp_format::FormatError;

const SERIAL_COOKIE_NO_RUNCONTAINER: u32 = 12346;
const ARRAY_MAX_CARD: usize = 4096;

/// Encodes `values` (ascending, distinct) as a portable Roaring bitmap.
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
    let mut out = Vec::new();
    out.extend_from_slice(&SERIAL_COOKIE_NO_RUNCONTAINER.to_le_bytes());
    out.extend_from_slice(&(containers.len() as u32).to_le_bytes());
    for (key, list) in &containers {
        out.extend_from_slice(&key.to_le_bytes());
        out.extend_from_slice(&((list.len() as u16).wrapping_sub(1)).to_le_bytes());
    }
    // Container data starts after the offset table.
    let header = 8 + containers.len() * 4 + containers.len() * 4;
    let mut data = Vec::new();
    let mut offsets = Vec::with_capacity(containers.len());
    for (_, list) in &containers {
        offsets.push((header + data.len()) as u32);
        if list.len() <= ARRAY_MAX_CARD {
            for v in list {
                data.extend_from_slice(&v.to_le_bytes());
            }
        } else {
            let mut bits = vec![0u64; 1024];
            for v in list {
                bits[*v as usize / 64] |= 1u64 << (*v as usize % 64);
            }
            for w in bits {
                data.extend_from_slice(&w.to_le_bytes());
            }
        }
    }
    for o in offsets {
        out.extend_from_slice(&o.to_le_bytes());
    }
    out.extend_from_slice(&data);
    out
}

/// Decodes the portable bitmap written by [`encode_roaring`].
fn decode_roaring(buf: &[u8]) -> Result<Vec<u32>, FormatError> {
    let at = |o: usize| -> Result<u32, FormatError> {
        if o + 4 > buf.len() {
            return Err(FormatError::Truncated {
                need: o + 4,
                got: buf.len(),
            });
        }
        Ok(u32::from_le_bytes(buf[o..o + 4].try_into().unwrap()))
    };
    let cookie = at(0)?;
    if cookie != SERIAL_COOKIE_NO_RUNCONTAINER {
        return Err(FormatError::BadMagic(cookie as u64));
    }
    let n = at(4)? as usize;
    let mut out = Vec::new();
    for i in 0..n {
        let hdr = 8 + i * 4;
        if hdr + 4 > buf.len() {
            return Err(FormatError::Truncated {
                need: hdr + 4,
                got: buf.len(),
            });
        }
        let key = u16::from_le_bytes(buf[hdr..hdr + 2].try_into().unwrap()) as u32;
        let card = u16::from_le_bytes(buf[hdr + 2..hdr + 4].try_into().unwrap()) as usize + 1;
        let off = at(8 + n * 4 + i * 4)? as usize;
        if card <= ARRAY_MAX_CARD {
            if off + card * 2 > buf.len() {
                return Err(FormatError::Truncated {
                    need: off + card * 2,
                    got: buf.len(),
                });
            }
            for j in 0..card {
                let low = u16::from_le_bytes(buf[off + j * 2..off + j * 2 + 2].try_into().unwrap())
                    as u32;
                out.push((key << 16) | low);
            }
        } else {
            if off + 8192 > buf.len() {
                return Err(FormatError::Truncated {
                    need: off + 8192,
                    got: buf.len(),
                });
            }
            for w in 0..1024 {
                let word =
                    u64::from_le_bytes(buf[off + w * 8..off + w * 8 + 8].try_into().unwrap());
                for b in 0..64 {
                    if word & (1u64 << b) != 0 {
                        out.push((key << 16) | ((w * 64 + b) as u32));
                    }
                }
            }
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
    for (_, offsets) in entries {
        let mut sorted = offsets.clone();
        sorted.sort_unstable();
        sorted.dedup();
        let bitmap = encode_roaring(&sorted);
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
    map: Mmap,
    count: usize,
}

impl LowCardReader {
    #[allow(unsafe_code)]
    pub fn open(path: &Path) -> Result<Self, FormatError> {
        let file =
            File::open(path).map_err(|e| FormatError::Io(format!("{}: {e}", path.display())))?;
        // SAFETY: read-only mapping of an immutable chunk file.
        let map = unsafe { Mmap::map(&file) }.map_err(|e| FormatError::Io(e.to_string()))?;
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
            let size = i32::from_le_bytes(self.map[at..at + 4].try_into().unwrap()) as usize;
            at += 4 + size;
        }
        let size = i32::from_le_bytes(self.map[at..at + 4].try_into().unwrap()) as usize;
        at += 4;
        decode_roaring(&self.map[at..at + size]).unwrap_or_default()
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
