// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//! Row bitmaps: the reference's `Bitmask` / `Bitmap`
//! (`src/storage/common/roaring_bitmap.cppm`).
//!
//! The reference expresses "which rows may this statement see" as a CRoaring
//! bitmap, never as a hash set. The distinction is not cosmetic: a `DELETE FROM
//! t WHERE id < 980` marks a *run*, which a roaring container stores in a few
//! dozen bytes where a `HashSet<u64>` needs ~15 KB and one hash lookup per row
//! tested. Every scan then tests membership once per candidate row, so the
//! representation is on the hot path of every indexed query.
//!
//! Two shapes live here, matching the two universes the engine has:
//!
//! * [`Bitmask`] — a `u32` universe with the reference's **all-true fast
//!   path**. This is what `CommonQueryFilter::filter_result_` holds, one per
//!   segment (`common_query_filter.cppm:53`), keyed by segment offset. The
//!   `all_true` flag is what makes "nobody deleted anything and the statement
//!   has no predicate" cost zero bytes.
//! * [`RowBitmap`] — a table-wide `u64` universe over **row ids**
//!   (`segment << 32 | offset`). `roaring::RoaringTreemap` is a
//!   `BTreeMap<u32, RoaringBitmap>`, and a row id's high 32 bits are exactly
//!   that partition key, so one row id is one lookup in the partition its
//!   segment already selects and the runs a segment deletes stay compressed.
//!
//! `RowBitmap` has no `all_true` flag: its callers always mean "these row ids
//! are in the set", and the empty set is the common case, which is already
//! zero-cost.

use std::sync::Arc;

use roaring::{RoaringBitmap, RoaringTreemap};

/// The reference's `Bitmask`, i.e. `RoaringBitmap<true>`
/// (`roaring_bitmap.cppm:27`): a `u32` universe that remembers whether it is
/// still all-true without materialising the bits.
///
/// `count` is the universe (`RoaringBitmap::count_`), so [`Self::is_true`]
/// answers `false` for an index at or past it exactly as the reference does
/// (`roaring_bitmap.cppm:88-95`).
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Bitmask {
    count: u32,
    all_true: bool,
    values: RoaringBitmap,
}

impl Bitmask {
    /// C++ `Bitmask(count)`: every bit in `0..count` is set.
    pub fn all_true(count: u32) -> Self {
        Self {
            count,
            all_true: true,
            values: RoaringBitmap::new(),
        }
    }

    /// C++ `Bitmap(count)`: nothing is set.
    pub fn all_false(count: u32) -> Self {
        Self {
            count,
            all_true: false,
            values: RoaringBitmap::new(),
        }
    }

    /// C++ `RoaringBitmap::count()`: the universe, not the cardinality.
    pub fn count(&self) -> u32 {
        self.count
    }

    /// C++ `RoaringBitmap::IsAllTrue()`.
    pub fn is_all_true(&self) -> bool {
        self.all_true
    }

    /// C++ `RoaringBitmap::IsTrue(row)`.
    ///
    /// A row outside the universe answers `false`: the reference logs and
    /// returns `false` rather than including it (`roaring_bitmap.cppm:88-95`).
    pub fn is_true(&self, row: u32) -> bool {
        row < self.count && (self.all_true || self.values.contains(row))
    }

    /// C++ `RoaringBitmap::SetTrue(row)`. Out-of-universe writes are dropped;
    /// the reference treats them as an unrecoverable error, and a bitmap that
    /// silently grew past its universe would make [`Self::count_true`] lie.
    pub fn set_true(&mut self, row: u32) {
        if row >= self.count || self.all_true {
            return;
        }
        self.values.insert(row);
        if self.values.len() == u64::from(self.count) {
            self.all_true = true;
            self.values.clear();
        }
    }

    /// C++ `RoaringBitmap::SetFalse(row)`.
    pub fn set_false(&mut self, row: u32) {
        if row >= self.count {
            return;
        }
        // `TryRemoveAllTrueFlag`: the all-true shortcut has to become real bits
        // before one can be cleared.
        if self.all_true {
            self.values.insert_range(0..self.count);
            self.all_true = false;
        }
        self.values.remove(row);
    }

    /// C++ `RoaringBitmap::CountTrue()`.
    pub fn count_true(&self) -> u32 {
        if self.all_true {
            self.count
        } else {
            self.values.len() as u32
        }
    }

    /// C++ `RoaringBitmap::CountFalse()`.
    pub fn count_false(&self) -> u32 {
        self.count - self.count_true()
    }

    /// C++ `RoaringBitmap::SetAllFalse()`.
    pub fn set_all_false(&mut self) {
        self.all_true = false;
        self.values.clear();
    }

    /// C++ `RoaringBitmap::MergeAnd(other)` — the reference's `&=`.
    pub fn merge_and(&mut self, other: &Bitmask) {
        if other.all_true {
            return;
        }
        if self.all_true {
            *self = other.clone();
            return;
        }
        self.values &= &other.values;
    }

    /// C++ `RoaringBitmap::MergeOr(other)` — the reference's `|=`.
    pub fn merge_or(&mut self, other: &Bitmask) {
        if other.all_true {
            self.set_all_true();
            return;
        }
        if self.all_true {
            return;
        }
        self.values |= &other.values;
        if self.values.len() == u64::from(self.count) {
            self.all_true = true;
            self.values.clear();
        }
    }

    /// C++ `RoaringBitmap::SetAllTrue()`.
    pub fn set_all_true(&mut self) {
        self.all_true = true;
        self.values.clear();
    }

    /// C++ `RoaringBitmap::RunOptimize()`: convert containers to the cheaper
    /// of array/bitmap/run encoding. Called once when a filter is finished
    /// being built, exactly where the reference calls it
    /// (`common_query_filter_impl.cpp:238`).
    pub fn run_optimize(&mut self) {
        if !self.all_true {
            self.values.optimize();
        }
    }

    /// The set bits, ascending. The reference iterates with
    /// `RoaringBitmapApplyFunc`; the all-true case would be a range, which no
    /// caller needs, so it is spelled out rather than silently yielding
    /// nothing.
    pub fn iter(&self) -> Box<dyn Iterator<Item = u32> + '_> {
        if self.all_true {
            Box::new(0..self.count)
        } else {
            Box::new(self.values.iter())
        }
    }

    /// Whether every bit is false.
    pub fn is_empty(&self) -> bool {
        self.count_true() == 0
    }
}

impl Default for Bitmask {
    fn default() -> Self {
        Self::all_false(0)
    }
}

/// A set of row ids (`segment << 32 | offset`).
///
/// This is the port's counterpart to the reference's `Bitmask` for the cases
/// whose universe is the whole table rather than one segment: the delete
/// journal, a statement's admitted rows, an index's members. `RoaringTreemap`
/// partitions on the high 32 bits, which is the segment id, so a segment's
/// tombstones stay in that segment's container and never widen the others.
///
/// The type is immutable by convention once shared: callers hold
/// `Arc<RowBitmap>` and build a new one to change it, which is what makes the
/// per-statement snapshot cheap (one refcount) instead of a set copy.
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct RowBitmap {
    rows: RoaringTreemap,
}

impl RowBitmap {
    pub fn new() -> Self {
        Self::default()
    }

    /// A set built from `rows`. (Named rather than the `FromIterator` impl so a
    /// `RowBitmap` can be produced where only the inherent form reads
    /// naturally.)
    pub fn of<I: IntoIterator<Item = u64>>(rows: I) -> Self {
        Self {
            rows: rows.into_iter().collect(),
        }
    }

    pub fn insert(&mut self, row: u64) -> bool {
        self.rows.insert(row)
    }

    pub fn remove(&mut self, row: u64) -> bool {
        self.rows.remove(row)
    }

    pub fn contains(&self, row: u64) -> bool {
        self.rows.contains(row)
    }

    pub fn extend<I: IntoIterator<Item = u64>>(&mut self, rows: I) {
        self.rows.extend(rows);
    }

    /// How many row ids the set holds.
    pub fn len(&self) -> u64 {
        self.rows.len()
    }

    pub fn is_empty(&self) -> bool {
        self.rows.is_empty()
    }

    pub fn clear(&mut self) {
        self.rows.clear();
    }

    /// The rows in `[start, end)`, ascending.
    pub fn range(&self, start: u64, end: u64) -> impl Iterator<Item = u64> + '_ {
        self.rows
            .iter()
            .skip_while(move |row| *row < start)
            .take_while(move |row| *row < end)
    }

    /// How many rows the two sets share (`RoaringBitmap::intersection_len`).
    pub fn intersection_len(&self, other: &RowBitmap) -> u64 {
        self.rows.intersection_len(&other.rows)
    }

    /// The rows, ascending.
    pub fn iter(&self) -> roaring::treemap::Iter<'_> {
        self.rows.iter()
    }

    /// C++ `RunOptimize()`.
    pub fn run_optimize(&mut self) {
        self.rows.optimize();
    }

    /// Bytes this set occupies, for the memory accounting the memindex quota
    /// reads.
    ///
    /// `RoaringTreemap` has no `serialized_size`, so the containers are summed
    /// directly: a partition entry is a `u32` key plus the bitmap's serialized
    /// image, and the tree's own nodes are `BTreeMap` overhead the caller does
    /// not need exact.
    pub fn mem_usage(&self) -> usize {
        self.rows
            .bitmaps()
            .map(|(_, bitmap)| bitmap.serialized_size() + std::mem::size_of::<u32>())
            .sum()
    }

    /// Every row id in the set, sorted ascending. Callers that need a `Vec`
    /// ask for it once instead of collecting the iterator themselves.
    pub fn to_vec(&self) -> Vec<u64> {
        self.rows.iter().collect()
    }
}

impl FromIterator<u64> for RowBitmap {
    fn from_iter<I: IntoIterator<Item = u64>>(rows: I) -> Self {
        Self {
            rows: rows.into_iter().collect(),
        }
    }
}

/// A shared, immutable row set. The engine's read path hands one of these out
/// per statement instead of copying a set, which is what the reference does
/// when it moves a finished `Bitmask` into `filter_result_`.
pub type SharedRows = Arc<RowBitmap>;

/// The rows of one statement may see: the reference's `CommonQueryFilter`
/// (`src/storage/secondary_index/common_query_filter.cppm`).
///
/// The reference keeps `filter_result_: std::map<SegmentID, Bitmask>` and folds
/// the deleted rows into the *same* bitmaps as the `WHERE` result
/// (`common_query_filter_impl.cpp:234-241`), so an index probe receives one
/// filter rather than a predicate mask and a tombstone set it has to apply
/// separately. This type is that single filter, and its two halves exist
/// because folding eagerly needs the reference's per-segment `count_`:
///
/// * `admit` — the rows the statement's predicate admits (`None` = the
///   predicate admits everything, the reference's `AlwaysTrue` for the filter
///   half);
/// * `exclude` — the rows deleted as of the statement's snapshot (`None` = the
///   table has no deletes, the reference's `!has_delete`).
///
/// The reference writes `admit & !exclude` into one bitmap because it has each
/// segment's row count to start from. This port does not carry that count on
/// the read path - deriving it is the full-table walk `live_count` exists to
/// avoid - so the two halves are tested separately. The second test only
/// happens when the table has deletes, and both are container lookups.
///
/// `contains` is then exactly `CommonQueryFilter`'s `Bitmask::IsTrue` after the
/// fold: the reference's `filter_result_` is what this answers.
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct RowMask {
    admit: Option<RowBitmap>,
    exclude: Option<RowBitmap>,
}

impl RowMask {
    /// C++ `CommonQueryFilter` with no filter and no delete: every row is
    /// admitted, nothing is materialised, and `contains` is a constant.
    pub fn all_true() -> Self {
        Self {
            admit: None,
            exclude: None,
        }
    }

    /// A mask that admits exactly `rows` and excludes nothing.
    pub fn from_rows(rows: RowBitmap) -> Self {
        Self {
            admit: Some(rows),
            exclude: None,
        }
    }

    /// Nothing is admitted.
    pub fn none() -> Self {
        Self {
            admit: Some(RowBitmap::new()),
            exclude: None,
        }
    }

    /// The statement's tombstone snapshot, which the index probe has to honour
    /// whether or not a predicate was pushed down.
    pub fn with_excluded(mut self, excluded: Option<RowBitmap>) -> Self {
        self.exclude = excluded.filter(|rows| !rows.is_empty());
        self
    }

    /// C++ `CommonQueryFilter::AlwaysTrue`: no predicate *and* no delete. This
    /// is the common case, and the one that has to stay free.
    pub fn is_all_true(&self) -> bool {
        self.admit.is_none() && self.exclude.is_none()
    }

    /// Whether `row` is admitted.
    pub fn contains(&self, row: u64) -> bool {
        if self.exclude.as_ref().is_some_and(|set| set.contains(row)) {
            return false;
        }
        self.admit.as_ref().is_none_or(|set| set.contains(row))
    }

    /// The first admitted row `>= row`.
    ///
    /// This is the seek the full-text cursors use (`DocFilter::next_ge`), and
    /// the reference gets it from `RoaringBitmap::NextSetBit`. An excluded row
    /// has to be hopped over, so the walk retries past it - a tombstone is
    /// bounded work, and the alternative (a materialised complement) needs the
    /// universe this type deliberately does not carry.
    pub fn next_ge(&self, row: u64) -> Option<u64> {
        let mut at = match &self.admit {
            Some(admit) => admit.range(row, u64::MAX).next()?,
            None => row,
        };
        while self.exclude.as_ref().is_some_and(|set| set.contains(at)) {
            at = match &self.admit {
                Some(admit) => admit.range(at + 1, u64::MAX).next()?,
                None => at.checked_add(1)?,
            };
        }
        Some(at)
    }

    /// How many rows are admitted, when that is known without a universe.
    pub fn len(&self) -> Option<u64> {
        match (&self.admit, &self.exclude) {
            // `all_true`, or "everything minus deletes": the count is the
            // table's, which this type does not hold.
            (None, _) => None,
            (Some(admit), None) => Some(admit.len()),
            (Some(admit), Some(exclude)) => {
                Some(admit.len().saturating_sub(admit.intersection_len(exclude)))
            }
        }
    }

    pub fn is_empty(&self) -> bool {
        self.len() == Some(0)
    }

    /// The admitted rows, ascending. `all_true` cannot be enumerated without a
    /// universe, so it yields nothing - callers that iterate a mask have
    /// already excluded the all-true case.
    pub fn iter(&self) -> Box<dyn Iterator<Item = u64> + '_> {
        match (&self.admit, &self.exclude) {
            (Some(admit), Some(exclude)) => {
                Box::new(admit.iter().filter(|row| !exclude.contains(*row)))
            }
            (Some(admit), None) => Box::new(admit.iter()),
            (None, _) => Box::new(std::iter::empty()),
        }
    }

    /// The predicate half, for a caller that needs the row ids themselves.
    pub fn rows(&self) -> Option<&RowBitmap> {
        self.admit.as_ref()
    }

    /// C++ `RoaringBitmap::RunOptimize()`.
    pub fn run_optimize(&mut self) {
        for set in [self.admit.as_mut(), self.exclude.as_mut()]
            .into_iter()
            .flatten()
        {
            set.run_optimize();
        }
    }
}

#[cfg(test)]
mod row_mask_tests {
    use super::*;

    #[test]
    fn the_all_true_mask_admits_everything_without_storing_anything() {
        let mask = RowMask::all_true();
        assert!(mask.is_all_true());
        assert!(mask.contains(0));
        assert!(mask.contains(u64::MAX));
        assert_eq!(mask.next_ge(12345), Some(12345));
        assert_eq!(mask.len(), None);
    }

    #[test]
    fn an_explicit_mask_seeks_like_the_references_next_set_bit() {
        let mask = RowMask::from_rows([3u64, 9, 40].into_iter().collect());
        assert!(mask.contains(9));
        assert!(!mask.contains(10));
        assert_eq!(mask.next_ge(0), Some(3));
        assert_eq!(mask.next_ge(3), Some(3));
        assert_eq!(mask.next_ge(4), Some(9));
        assert_eq!(mask.next_ge(41), None);
        assert_eq!(mask.len(), Some(3));
    }

    #[test]
    fn row_ids_of_several_segments_share_one_mask() {
        // The reference's `map<SegmentID, Bitmask>` is the treemap's own
        // partition map: segment 2 and segment 0 coexist without either
        // widening the other.
        let mut rows = RowBitmap::new();
        rows.insert(5);
        rows.insert((2u64 << 32) | 1);
        let mask = RowMask::from_rows(rows);
        assert!(mask.contains(5));
        assert!(mask.contains((2u64 << 32) | 1));
        assert!(!mask.contains(6));
        assert_eq!(mask.next_ge(1u64 << 32), Some((2u64 << 32) | 1));
    }

    #[test]
    fn a_tombstone_hides_a_row_from_both_halves() {
        let mask = RowMask::from_rows((0..10u64).collect())
            .with_excluded(Some([1u64, 2, 3].into_iter().collect()));
        assert!(mask.contains(0));
        assert!(!mask.contains(2));
        assert_eq!(mask.len(), Some(7));
        assert_eq!(mask.next_ge(1), Some(4));
    }

    #[test]
    fn a_delete_without_a_predicate_still_filters_the_probe() {
        // No predicate (the predicate half admits everything) but the table has
        // deletes: the mask is not `all_true`, and the tombstone still hides its
        // row. This is the case the port used to answer short, because the
        // probe had no filter to work with at all.
        let mask = RowMask::all_true().with_excluded(Some([5u64, 6].into_iter().collect()));
        assert!(!mask.is_all_true());
        assert!(mask.contains(4));
        assert!(!mask.contains(5));
        assert_eq!(mask.next_ge(0), Some(0));
        assert_eq!(mask.next_ge(5), Some(7));
        assert_eq!(mask.len(), None);
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn the_all_true_flag_survives_a_clear_and_a_merge() {
        let mut mask = Bitmask::all_true(10);
        assert!(mask.is_all_true());
        assert_eq!(mask.count_true(), 10);
        mask.set_false(3);
        assert!(!mask.is_all_true());
        assert_eq!(mask.count_true(), 9);
        assert!(!mask.is_true(3));
        // Clearing the last false bit brings the shortcut back.
        mask.set_true(3);
        assert!(mask.is_all_true());
        assert_eq!(mask.count_true(), 10);
    }

    #[test]
    fn a_bit_outside_the_universe_is_never_true() {
        let mut mask = Bitmask::all_true(4);
        assert!(!mask.is_true(4));
        assert!(!mask.is_true(u32::MAX));
        // And writing one does not grow the universe.
        mask.set_true(9);
        assert_eq!(mask.count(), 4);
        assert_eq!(mask.count_true(), 4);
    }

    #[test]
    fn merge_and_and_merge_or_match_the_reference() {
        let mut a = Bitmask::all_false(8);
        a.set_true(1);
        a.set_true(2);
        let mut b = Bitmask::all_false(8);
        b.set_true(2);
        b.set_true(3);

        let mut and = a.clone();
        and.merge_and(&b);
        assert_eq!(and.iter().collect::<Vec<_>>(), vec![2]);

        let mut or = a.clone();
        or.merge_or(&b);
        assert_eq!(or.iter().collect::<Vec<_>>(), vec![1, 2, 3]);

        // `all_true` is the identity for `&` and the annihilator for `|`.
        let mut with_true = Bitmask::all_true(8);
        with_true.merge_and(&a);
        assert_eq!(with_true, a);
        let mut or_true = a.clone();
        or_true.merge_or(&Bitmask::all_true(8));
        assert!(or_true.is_all_true());
    }

    #[test]
    fn a_run_of_row_ids_compresses_to_a_fraction_of_a_hash_set() {
        // 980 consecutive deletions: the shape `DELETE FROM t WHERE id < 980`
        // produces, and the reason the reference uses roaring here.
        let mut rows: RowBitmap = (0..980u64).collect();
        // `RunOptimize()`: the caller does this once the set is finished, which
        // is where the reference calls it too (`common_query_filter_impl.cpp:238`).
        rows.run_optimize();
        assert_eq!(rows.len(), 980);
        assert!(rows.contains(0));
        assert!(rows.contains(979));
        assert!(!rows.contains(980));
        assert!(
            rows.mem_usage() < 200,
            "a run container should hold this in tens of bytes, got {}",
            rows.mem_usage()
        );
    }

    #[test]
    fn row_ids_from_several_segments_never_widen_each_other() {
        let mut rows = RowBitmap::new();
        rows.extend((0..5u64).collect::<Vec<_>>());
        // Segment 3, offset 7.
        rows.insert((3u64 << 32) | 7);
        assert_eq!(rows.len(), 6);
        assert!(rows.contains((3u64 << 32) | 7));
        assert!(!rows.contains(3u64 << 32));
        assert_eq!(rows.to_vec(), vec![0, 1, 2, 3, 4, (3u64 << 32) | 7]);
    }
}
