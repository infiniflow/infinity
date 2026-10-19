//! Integer bitmaps with the subset of the CRoaring API the reference tests
//! exercise (`roaring::Roaring` and `roaring::Roaring64Map`).
//!
//! The reference links CRoaring for compression and set algebra. The Rust
//! rewrite intentionally carries no third-party bitmap dependency: a sorted
//! set of integers already gives the canonical representation (`run_optimize`
//! is therefore a no-op) and supports the same set operations with logarithmic
//! membership.

use std::collections::BTreeSet;
use std::ops::{BitAnd, BitOr, BitOrAssign, BitXor};

/// C++: `roaring::Roaring` (32-bit values).
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct Bitmap {
    values: BTreeSet<u32>,
}

impl Bitmap {
    /// C++: `Roaring()`.
    pub fn new() -> Self {
        Self::default()
    }

    /// C++: `Roaring(count, values)` / `Roaring::bitmapOfList`.
    pub fn from_values(values: impl IntoIterator<Item = u32>) -> Self {
        Self {
            values: values.into_iter().collect(),
        }
    }

    /// C++: `Roaring::add`.
    pub fn add(&mut self, value: u32) -> bool {
        self.values.insert(value)
    }

    /// C++: `Roaring::addRange(min, max)` — adds `[min, max)`.
    pub fn add_range(&mut self, min: u32, max: u32) {
        if min < max {
            self.values.extend(min..max);
        }
    }

    /// C++: `Roaring::remove`.
    pub fn remove(&mut self, value: u32) -> bool {
        self.values.remove(&value)
    }

    /// C++: `Roaring::contains`.
    pub fn contains(&self, value: u32) -> bool {
        self.values.contains(&value)
    }

    /// C++: `Roaring::cardinality`.
    pub fn cardinality(&self) -> u64 {
        self.values.len() as u64
    }

    /// C++: `Roaring::isEmpty`.
    pub fn is_empty(&self) -> bool {
        self.values.is_empty()
    }

    /// C++: `Roaring::rank` — the number of values less than or equal to the
    /// given value.
    pub fn rank(&self, value: u32) -> u64 {
        self.values.range(..=value).count() as u64
    }

    /// C++: `operator|`.
    pub fn or(&self, other: &Bitmap) -> Bitmap {
        Bitmap {
            values: self.values.union(&other.values).copied().collect(),
        }
    }

    /// C++: `operator&`.
    pub fn and(&self, other: &Bitmap) -> Bitmap {
        Bitmap {
            values: self.values.intersection(&other.values).copied().collect(),
        }
    }

    /// C++: `operator^`.
    pub fn xor(&self, other: &Bitmap) -> Bitmap {
        Bitmap {
            values: self.values.symmetric_difference(&other.values).copied().collect(),
        }
    }

    /// C++: `Roaring::runOptimize` — the sorted set is already canonical, so
    /// there is no representation change.
    pub fn run_optimize(&mut self) {}

    /// C++: `Roaring::setCopyOnWrite` — copies are always deep here.
    pub fn set_copy_on_write(&mut self, _enabled: bool) {}

    /// C++: `Roaring::getSizeInBytes` for our wire format.
    pub fn serialized_len(&self) -> usize {
        // 4-byte cardinality + 4 bytes per value.
        4 + 4 * self.values.len()
    }

    /// C++: `Roaring::write` — cardinality followed by the sorted values,
    /// little-endian.
    pub fn write(&self) -> Vec<u8> {
        let mut bytes = Vec::with_capacity(self.serialized_len());
        bytes.extend_from_slice(&(self.values.len() as u32).to_le_bytes());
        for value in &self.values {
            bytes.extend_from_slice(&value.to_le_bytes());
        }
        bytes
    }

    /// C++: `Roaring::readSafe` — rejects truncated, oversized, unsorted or
    /// duplicated input instead of reading past the buffer.
    pub fn read(bytes: &[u8]) -> Option<Self> {
        let count = u32::from_le_bytes(bytes.get(..4)?.try_into().ok()?) as usize;
        if bytes.len() != 4usize.checked_add(count.checked_mul(4)?)? {
            return None;
        }
        let mut values = BTreeSet::new();
        let mut previous = None;
        for chunk in bytes[4..].chunks_exact(4) {
            let value = u32::from_le_bytes(chunk.try_into().ok()?);
            if previous.is_some_and(|last| value <= last) {
                return None;
            }
            previous = Some(value);
            values.insert(value);
        }
        Some(Self { values })
    }

    /// C++: iteration over the stored values in ascending order.
    pub fn iter(&self) -> impl Iterator<Item = u32> + '_ {
        self.values.iter().copied()
    }

    /// C++: `Roaring::toUint32Array`.
    pub fn to_vec(&self) -> Vec<u32> {
        self.values.iter().copied().collect()
    }

    /// C++: `Roaring::const_iterator::equalorlarger` — the first value `>= v`.
    pub fn equal_or_larger(&self, value: u32) -> Option<u32> {
        self.values.range(value..).next().copied()
    }
}

impl BitOr<&Bitmap> for &Bitmap {
    type Output = Bitmap;

    fn bitor(self, rhs: &Bitmap) -> Bitmap {
        self.or(rhs)
    }
}

impl BitOrAssign<&Bitmap> for Bitmap {
    fn bitor_assign(&mut self, rhs: &Bitmap) {
        self.values.extend(rhs.values.iter().copied());
    }
}

impl BitAnd<&Bitmap> for &Bitmap {
    type Output = Bitmap;

    fn bitand(self, rhs: &Bitmap) -> Bitmap {
        self.and(rhs)
    }
}

impl BitXor<&Bitmap> for &Bitmap {
    type Output = Bitmap;

    fn bitxor(self, rhs: &Bitmap) -> Bitmap {
        self.xor(rhs)
    }
}

/// C++: `roaring::Roaring64Map`.
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct Bitmap64 {
    values: BTreeSet<u64>,
}

impl Bitmap64 {
    /// C++: `Roaring64Map()`.
    pub fn new() -> Self {
        Self::default()
    }

    /// C++: `Roaring64Map::add`.
    pub fn add(&mut self, value: u64) -> bool {
        self.values.insert(value)
    }

    /// C++: `Roaring64Map::addRange(min, max)` — adds `[min, max)`.
    pub fn add_range(&mut self, min: u64, max: u64) {
        if min < max {
            self.values.extend(min..max);
        }
    }

    /// C++: `Roaring64Map::contains`.
    pub fn contains(&self, value: u64) -> bool {
        self.values.contains(&value)
    }

    /// C++: `Roaring64Map::cardinality`.
    pub fn cardinality(&self) -> u64 {
        self.values.len() as u64
    }

    /// C++: `Roaring64Map::isEmpty`.
    pub fn is_empty(&self) -> bool {
        self.values.is_empty()
    }
}

#[cfg(test)]
mod third_party_reference_suite {
    //! Ports of `src/unit_test/third_party/test_croaring_ut.cpp`
    //! (`CRoaringTest`).

    use super::*;

    /// C++: `CRoaringTest.test_size` — the reference only prints these values;
    /// the assertions state the printed facts.
    #[test]
    fn roaring_size_and_optimize() {
        // Block 1: add a range.
        let mut ranged = Bitmap::new();
        ranged.add_range(0, 8192);
        assert_eq!(ranged.cardinality(), 8192);
        assert!(ranged.serialized_len() > 0);

        // Block 2: add one by one, then optimize.
        let mut one_by_one = Bitmap::new();
        for value in 0..8192 {
            one_by_one.add(value);
        }
        assert_eq!(one_by_one.cardinality(), 8192);
        assert!(one_by_one.serialized_len() > 0);
        one_by_one.run_optimize();
        assert_eq!(one_by_one.cardinality(), 8192);
        assert_eq!(one_by_one.serialized_len(), ranged.serialized_len());
        assert_eq!(one_by_one, ranged);

        // Block 3: add one by one, then add the same range, then optimize.
        let mut mixed = Bitmap::new();
        for value in 0..8192 {
            mixed.add(value);
        }
        assert_eq!(mixed.cardinality(), 8192);
        mixed.add_range(0, 8192);
        assert_eq!(mixed.cardinality(), 8192);
        mixed.run_optimize();
        assert_eq!(mixed.cardinality(), 8192);
        assert_eq!(mixed, ranged);
        assert_eq!(Bitmap::read(&mixed.write()), Some(mixed));
    }

    /// C++: `CRoaringTest.test1` — 32-bit and 64-bit cardinalities.
    #[test]
    fn roaring_cardinality_32_and_64() {
        let mut r1 = Bitmap::new();
        for value in 100..1000 {
            r1.add(value);
        }
        assert_eq!(r1.cardinality(), 900);

        let mut r2 = Bitmap64::new();
        for value in 18_000_000_000_000_000_100u64..18_000_000_000_000_001_000u64 {
            r2.add(value);
        }
        assert_eq!(r2.cardinality(), 900);
    }

    /// C++: `CRoaringTest.test2` — membership, algebra, serialization,
    /// iteration and the `equalorlarger` iterator move.
    #[test]
    fn roaring_bitmap_operations() {
        let mut r1 = Bitmap::new();
        for value in 100..1000 {
            r1.add(value);
        }

        assert!(r1.contains(500));
        assert_eq!(r1.cardinality(), 900);

        let size_before = r1.serialized_len();
        assert!(size_before > 0);
        r1.run_optimize();
        r1.set_copy_on_write(true);
        assert_eq!(r1.serialized_len(), size_before);
        assert_eq!(r1.cardinality(), 900);

        // bitmapOf(5, 1, 2, 3, 5, 6) and bitmapOfList({1,2,3,5,6}).
        let r2 = Bitmap::from_values([1, 2, 3, 5, 6]);
        let r2i = Bitmap::from_values([1, 2, 3, 5, 6]);
        assert_eq!(r2i, r2);

        let r3 = Bitmap::from_values([2, 3, 4]);

        // toUint32Array followed by the array constructor round-trips.
        let card1 = r1.cardinality() as usize;
        let arr1 = r1.to_vec();
        assert_eq!(arr1.len(), card1);
        let r1f = Bitmap::from_values(arr1);
        assert_eq!(r1, r1f);

        // Copy and compare.
        let z = r3.clone();
        assert_eq!(r3, z);

        // Union two-by-two and fastunion.
        let mut r1_2_3 = &r1 | &r2;
        r1_2_3 |= &r3;
        let bigunion = r1.or(&r2).or(&r3);
        assert_eq!(r1_2_3, bigunion);
        // 900 values in r1, 5 in r2, and only the value 4 from r3 is new.
        assert_eq!(bigunion.cardinality(), 906);

        // Intersection: r2's values are all below r1's range.
        let i1_2 = &r1 & &r2;
        assert!(i1_2.is_empty());

        // Symmetric difference.
        let x1_2 = &r1 ^ &r2;
        assert_eq!(x1_2.cardinality(), 905);
        assert!(x1_2.contains(1));
        assert!(x1_2.contains(100));
        // The two sets are disjoint, so the symmetric difference is the union.
        assert_eq!(x1_2, r1.or(&r2));

        // write/readSafe round-trip.
        let bytes = r1.write();
        assert_eq!(bytes.len(), r1.serialized_len());
        let t = Bitmap::read(&bytes).unwrap();
        assert_eq!(r1, t);
        assert_eq!(Bitmap::read(&bytes[..bytes.len() - 1]), None);

        // Custom-function iteration and C++-style iteration both visit every
        // value.
        let counter: u64 = r1.iter().map(u64::from).sum();
        let expected: u64 = (100..1000u64).sum();
        assert_eq!(counter, expected);
        let mut iterated = 0;
        for _ in t.iter() {
            iterated += 1;
        }
        assert_eq!(iterated, t.cardinality());

        // rank: 100..=500 inclusive.
        assert_eq!(r1.rank(500), 401);
        assert_eq!(r1.rank(99), 0);
        assert_eq!(r1.rank(999), 900);

        // equalorlarger skips values.
        let rogue = Bitmap::from_values([2, 3, 4, 7, 8]);
        assert_eq!(rogue.equal_or_larger(4), Some(4));
        assert_eq!(rogue.equal_or_larger(5), Some(7));
        assert_eq!(rogue.equal_or_larger(9), None);
    }
}
