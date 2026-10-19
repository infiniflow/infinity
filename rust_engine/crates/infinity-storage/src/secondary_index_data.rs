//! In-memory secondary-index chunk builders.
//!
//! Port of the two `SecondaryIndexDataBase` implementations in
//! `src/storage/secondary_index/secondary_index_data_impl.cpp`:
//!
//! * [`HighCardinalityIndex`] — the reference's `SecondaryIndexDataHighCardinalityT`:
//!   a sorted `(key, segment offset)` array plus the PGM saved by
//!   [`crate::secondary_segment::write_secondary_segment`]. The base class
//!   reports no unique-key view (`GetUniqueKeyCount` returns 0,
//!   `GetUniqueKeysPtr` returns nullptr).
//! * [`LowCardinalityIndex`] — `SecondaryIndexDataLowCardinalityT`: one sorted
//!   unique-key array plus one Roaring bitmap of offsets per key, saved by
//!   [`crate::secondary_lowcard::write_lowcard_segment`].
//!
//! Keys are the reference's `OrderedKeyType` widened to `u64`; the executor
//! already stores every column's ordered key as a `u64` (integers sign-flipped,
//! floating point bit-ordered, VARCHAR hashed), so the layout matches
//! [`crate::secondary_segment`]'s fixed-width key array.

use std::collections::{BTreeMap, BTreeSet};
use std::path::Path;

use crate::cpp_format::FormatError;
use crate::secondary_lowcard::{write_lowcard_segment, LowCardReader};
use crate::secondary_segment::{write_secondary_segment, SecondarySegmentReader};

/// Order-preserving map of a signed integer into `u64`: `i64::MIN` -> 0.
pub fn ordered_i64(value: i64) -> u64 {
    (value as u64) ^ (1u64 << 63)
}

/// Order-preserving map of a finite float into `u64`.
pub fn ordered_f64(value: f64) -> u64 {
    let bits = value.to_bits();
    if bits & (1u64 << 63) != 0 {
        !bits
    } else {
        bits ^ (1u64 << 63)
    }
}

/// C++ `SecondaryIndexDataHighCardinalityT`.
#[derive(Debug, Default)]
pub struct HighCardinalityIndex {
    entries: Vec<(u64, u32)>,
}

impl HighCardinalityIndex {
    pub fn new() -> Self {
        Self::default()
    }

    /// `InsertData`: the reference receives a `multimap` already ordered by key.
    pub fn insert_data(&mut self, entries: impl IntoIterator<Item = (u64, u32)>) {
        self.entries = entries.into_iter().collect();
        self.entries.sort_unstable_by_key(|(key, _)| *key);
    }

    pub fn row_count(&self) -> usize {
        self.entries.len()
    }

    pub fn entries(&self) -> &[(u64, u32)] {
        &self.entries
    }

    /// `SecondaryIndexDataBase::GetUniqueKeyCount` — the high-cardinality
    /// variant keeps no unique-key view.
    pub fn unique_key_count(&self) -> u32 {
        0
    }

    /// `SecondaryIndexDataBase::GetUniqueKeysPtr` — always null here.
    pub fn unique_keys(&self) -> Option<&[u64]> {
        None
    }

    /// `SaveIndexInner`.
    pub fn save(&self, path: &Path) -> Result<(), FormatError> {
        write_secondary_segment(path, &self.entries)
    }

    /// `ReadIndexInner` into a fresh mapped chunk.
    pub fn load(path: &Path) -> Result<HighCardinalityReader, FormatError> {
        SecondarySegmentReader::open(path).map(|reader| HighCardinalityReader { reader })
    }
}

/// A high-cardinality chunk reloaded from disk.
pub struct HighCardinalityReader {
    reader: SecondarySegmentReader,
}

impl HighCardinalityReader {
    pub fn row_count(&self) -> usize {
        self.reader.len()
    }

    pub fn unique_key_count(&self) -> u32 {
        0
    }

    pub fn lookup(&self, key: u64) -> Vec<u32> {
        self.reader.lookup(key)
    }
}

/// C++ `SecondaryIndexDataLowCardinalityT`.
#[derive(Debug, Default)]
pub struct LowCardinalityIndex {
    bitmaps: BTreeMap<u64, BTreeSet<u32>>,
}

impl LowCardinalityIndex {
    pub fn new() -> Self {
        Self::default()
    }

    /// `InsertData`: group the rows' segment offsets by key.
    pub fn insert_data(&mut self, entries: impl IntoIterator<Item = (u64, u32)>) {
        for (key, offset) in entries {
            self.bitmaps.entry(key).or_default().insert(offset);
        }
    }

    /// `GetUniqueKeyCount`.
    pub fn unique_key_count(&self) -> u32 {
        self.bitmaps.len() as u32
    }

    /// `GetUniqueKeysPtr`: ascending unique keys, `None` for an empty index.
    pub fn unique_keys(&self) -> Option<Vec<u64>> {
        if self.bitmaps.is_empty() {
            None
        } else {
            Some(self.bitmaps.keys().copied().collect())
        }
    }

    /// `GetOffsetsForKeyPtr`: the bitmap's offsets for `key`.
    pub fn offsets_for_key(&self, key: u64) -> Option<&BTreeSet<u32>> {
        self.bitmaps.get(&key)
    }

    /// `SaveIndexInner`.
    pub fn save(&self, path: &Path) -> Result<(), FormatError> {
        let entries: Vec<(u64, Vec<u32>)> = self
            .bitmaps
            .iter()
            .map(|(key, offsets)| (*key, offsets.iter().copied().collect()))
            .collect();
        write_lowcard_segment(path, &entries)
    }

    /// `ReadIndexInner` from a mapped chunk.
    pub fn load(path: &Path) -> Result<LowCardinalityReader, FormatError> {
        LowCardReader::open(path).map(|reader| LowCardinalityReader { reader })
    }
}

/// A low-cardinality chunk reloaded from disk.
pub struct LowCardinalityReader {
    reader: LowCardReader,
}

impl LowCardinalityReader {
    pub fn unique_key_count(&self) -> u32 {
        self.reader.len() as u32
    }

    pub fn offsets_for_key(&self, key: u64) -> Vec<u32> {
        self.reader.lookup(key)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    /// `CreateLowCardinalityData<T>`: `row_count` rows whose key cycles over
    /// `unique_values` distinct values; the segment offset is the row index.
    fn cyclic_i64(row_count: u32, unique_values: u32) -> Vec<(u64, u32)> {
        (0..row_count)
            .map(|i| (ordered_i64(i64::from(i % unique_values)), i))
            .collect()
    }

    fn offsets(bitmap: &BTreeSet<u32>) -> Vec<u32> {
        bitmap.iter().copied().collect()
    }

    /// C++: `LowCardinalitySecondaryIndexTest.TestIntegerLowCardinality`.
    #[test]
    fn integer_low_cardinality() {
        const ROWS: u32 = 100;
        const UNIQUE: u32 = 5;
        let data = cyclic_i64(ROWS, UNIQUE);
        let mut index = LowCardinalityIndex::new();
        index.insert_data(data.clone());
        assert_eq!(index.unique_key_count(), UNIQUE);

        let keys = index.unique_keys().unwrap();
        assert_eq!(keys.len(), UNIQUE as usize);
        for pair in keys.windows(2) {
            assert!(pair[0] < pair[1], "unique keys are not ascending");
        }
        for i in 0..UNIQUE {
            let key = ordered_i64(i64::from(i));
            let bitmap = index.offsets_for_key(key).expect("bitmap");
            let expected: Vec<u32> = data
                .iter()
                .filter(|(k, _)| *k == key)
                .map(|(_, off)| *off)
                .collect();
            assert_eq!(offsets(bitmap), expected);
        }
    }

    /// C++: `LowCardinalitySecondaryIndexTest.TestFloatLowCardinality`.
    #[test]
    fn float_low_cardinality() {
        const ROWS: u32 = 80;
        let unique_keys = [1.5f64, 2.5, 3.5, 4.5];
        let data: Vec<(u64, u32)> = (0..ROWS)
            .map(|i| (ordered_f64(unique_keys[(i % 4) as usize]), i))
            .collect();
        let mut index = LowCardinalityIndex::new();
        index.insert_data(data.clone());
        assert_eq!(index.unique_key_count(), 4);
        for key in unique_keys {
            let encoded = ordered_f64(key);
            let bitmap = index.offsets_for_key(encoded).expect("bitmap");
            let mut actual = offsets(bitmap);
            let mut expected: Vec<u32> = data
                .iter()
                .filter(|(k, _)| *k == encoded)
                .map(|(_, off)| *off)
                .collect();
            actual.sort_unstable();
            expected.sort_unstable();
            assert_eq!(actual, expected);
        }
    }

    /// C++: `LowCardinalitySecondaryIndexTest.TestEmptyIndex`.
    #[test]
    fn empty_index() {
        let mut index = LowCardinalityIndex::new();
        index.insert_data(std::iter::empty());
        assert_eq!(index.unique_key_count(), 0);
        assert!(index.unique_keys().is_none());
    }

    /// C++: `LowCardinalitySecondaryIndexTest.TestSingleValueCardinality`.
    #[test]
    fn single_value_cardinality() {
        const ROWS: u32 = 50;
        let data: Vec<(u64, u32)> = (0..ROWS).map(|i| (ordered_i64(0), i)).collect();
        let mut index = LowCardinalityIndex::new();
        index.insert_data(data);
        assert_eq!(index.unique_key_count(), 1);
        let bitmap = index.offsets_for_key(ordered_i64(0)).expect("bitmap");
        assert_eq!(bitmap.len(), ROWS as usize);
    }

    /// C++: `LowCardinalitySecondaryIndexTest.TestComparisonWithHighCardinality`.
    #[test]
    fn comparison_with_high_cardinality() {
        const ROWS: u32 = 100;
        const UNIQUE: u32 = 5;
        let data = cyclic_i64(ROWS, UNIQUE);
        let mut high = HighCardinalityIndex::new();
        high.insert_data(data.clone());
        let mut low = LowCardinalityIndex::new();
        low.insert_data(data);

        assert_eq!(high.unique_key_count(), 0);
        assert!(high.unique_keys().is_none());
        assert_eq!(low.unique_key_count(), UNIQUE);
        assert!(low.unique_keys().is_some());
        for i in 0..UNIQUE {
            let bitmap = low.offsets_for_key(ordered_i64(i64::from(i))).unwrap();
            assert_eq!(bitmap.len(), (ROWS / UNIQUE) as usize);
        }
    }

    /// C++: `LowCardinalitySecondaryIndexTest.TestSaveLoadLowCardinality`.
    #[test]
    fn save_load_low_cardinality() {
        const ROWS: u32 = 1000;
        const UNIQUE: u32 = 5;
        let data = cyclic_i64(ROWS, UNIQUE);
        let mut index = LowCardinalityIndex::new();
        index.insert_data(data.clone());
        let key_count = index.unique_key_count();

        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("test001.idx");
        index.save(&path).unwrap();
        let loaded = LowCardinalityIndex::load(&path).unwrap();
        assert_eq!(key_count, loaded.unique_key_count());
        assert_eq!(loaded.unique_key_count(), UNIQUE);
        for i in 0..UNIQUE {
            let key = ordered_i64(i64::from(i));
            let expected: Vec<u32> = data
                .iter()
                .filter(|(k, _)| *k == key)
                .map(|(_, off)| *off)
                .collect();
            assert_eq!(loaded.offsets_for_key(key), expected);
        }
    }

    /// C++: `LowCardinalitySecondaryIndexTest.TestAllDataType`.
    #[test]
    fn all_data_types() {
        const ROWS: u32 = 500;
        const UNIQUE: u32 = 10;
        // kBigInt.
        let mut index = LowCardinalityIndex::new();
        index.insert_data(cyclic_i64(ROWS, UNIQUE));
        assert_eq!(index.unique_key_count(), 10);
        // kDouble.
        let mut index = LowCardinalityIndex::new();
        index.insert_data((0..ROWS).map(|i| (ordered_f64(f64::from(i % UNIQUE)), i)));
        assert_eq!(index.unique_key_count(), 10);
        // kDate / kTime (i32) and kDateTime / kTimestamp (i64).
        for _ in 0..4 {
            let mut index = LowCardinalityIndex::new();
            index.insert_data(cyclic_i64(ROWS, UNIQUE));
            assert_eq!(index.unique_key_count(), 10);
        }
    }

    /// C++: `HighCardinalitySecondaryIndexTest.TestSaveLoadHighCardinality`.
    #[test]
    fn save_load_high_cardinality() {
        const ROWS: u32 = 10;
        const UNIQUE: u32 = 5;
        let data = cyclic_i64(ROWS, UNIQUE);
        let mut index = HighCardinalityIndex::new();
        index.insert_data(data.clone());
        let key_count = index.unique_key_count();
        assert_eq!(key_count, 0);
        assert_eq!(index.row_count(), ROWS as usize);

        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("test001.idx");
        index.save(&path).unwrap();
        let loaded = HighCardinalityIndex::load(&path).unwrap();
        // `EXPECT_EQ(key_count, index2->GetUniqueKeyCount())`.
        assert_eq!(key_count, loaded.unique_key_count());
        assert_eq!(loaded.unique_key_count(), 0);
        assert_eq!(loaded.row_count(), ROWS as usize);
        // Every key/offset pair survives the save/load round trip in order.
        let mut expected = data.clone();
        expected.sort_unstable_by_key(|(key, _)| *key);
        for i in 0..UNIQUE {
            let key = ordered_i64(i64::from(i));
            let want: Vec<u32> = expected
                .iter()
                .filter(|(k, _)| *k == key)
                .map(|(_, off)| *off)
                .collect();
            let got = loaded.lookup(key);
            assert_eq!(got, want, "key {i}");
            assert!(got.windows(2).all(|w| w[0] < w[1]), "offsets ascend");
        }
        assert_eq!(loaded.lookup(ordered_i64(1)), vec![1, 6]);
    }

    /// C++: `HighCardinalitySecondaryIndexTest.TestTxn` — the index half: the
    /// reference creates the table/index through a transaction (covered by the
    /// catalog tests) and then only exercises `InsertData`.
    #[test]
    fn txn_insert_data() {
        const ROWS: u32 = 10;
        const UNIQUE: u32 = 5;
        let mut index = HighCardinalityIndex::new();
        index.insert_data(cyclic_i64(ROWS, UNIQUE));
        assert_eq!(index.row_count(), ROWS as usize);
        assert_eq!(index.unique_key_count(), 0);
    }
}
