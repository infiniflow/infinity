//! Timestamp-array MVCC (`BlockVersion`), mirroring Infinity's
//! `src/storage/catalog/meta/block_version.cppm`.
//!
//! Rows are never mutated in place: an update is *delete + append*. Visibility
//! is decided by comparing per-row delete timestamps and per-append create
//! timestamps against the reader's `begin_ts` (snapshot isolation by timestamp).
//! This deliberately does **not** use KV snapshots (plan 附录 A.2).

use infinity_common::{Error, Result, TxnTimeStamp};

use crate::cpp_format::BlockVersionFile;

/// One append batch: `row_count` rows are visible in total at `create_ts`.
///
/// C++: `CreateField` (`block_version.cppm:27`); `row_count_` is the
/// **cumulative** row count, not the batch size.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct CreateField {
    pub create_ts: TxnTimeStamp,
    pub row_count: i64,
}

/// MVCC state of a single block.
///
/// C++: `BlockVersion` (`block_version.cppm:44`). `deleted` is pre-sized to the
/// block capacity, so `delete(offset, ts)` addresses a slot directly, exactly
/// like the reference's `deleted_[offset]`.
#[derive(Debug, Clone, Default)]
pub struct BlockVersion {
    created: Vec<CreateField>,
    /// Per-slot delete timestamp; `0` means "not deleted".
    deleted: Vec<TxnTimeStamp>,
    latest_change_ts: TxnTimeStamp,
}

impl BlockVersion {
    /// C++: `explicit BlockVersion(size_t capacity)`.
    pub fn new(capacity: usize) -> Self {
        Self {
            created: Vec::new(),
            deleted: vec![0; capacity],
            latest_change_ts: 0,
        }
    }

    pub fn capacity(&self) -> usize {
        self.deleted.len()
    }

    /// C++: `BlockVersion::Append(commit_ts, row_count)` — `row_count` is the
    /// cumulative number of rows after this batch.
    pub fn append(&mut self, commit_ts: TxnTimeStamp, row_count: i64) {
        self.created.push(CreateField {
            create_ts: commit_ts,
            row_count,
        });
        self.latest_change_ts = commit_ts;
    }

    /// C++: `BlockVersion::CommitAppend` — rewrites the batch first appended at
    /// `save_ts` once its real commit timestamp is known.
    pub fn commit_append(&mut self, save_ts: TxnTimeStamp, commit_ts: TxnTimeStamp) {
        for field in &mut self.created {
            if field.create_ts == save_ts {
                field.create_ts = commit_ts;
                break;
            }
        }
        self.latest_change_ts = commit_ts;
    }

    /// C++: `BlockVersion::Delete` — deleting a slot twice is a write-write
    /// conflict (`Status::TxnWWConflict`).
    pub fn delete(&mut self, offset: usize, commit_ts: TxnTimeStamp) -> Result<()> {
        let Some(slot) = self.deleted.get_mut(offset) else {
            return Err(Error::Storage(format!(
                "delete offset {offset} out of range"
            )));
        };
        if *slot != 0 {
            return Err(Error::TxnConflict(format!(
                "Delete twice at offset: {offset}, commit_ts: {commit_ts}, old_ts: {slot}"
            )));
        }
        *slot = commit_ts;
        self.latest_change_ts = commit_ts;
        Ok(())
    }

    /// C++: `BlockVersion::RollbackDelete`.
    pub fn rollback_delete(&mut self, offset: usize) {
        if let Some(slot) = self.deleted.get_mut(offset) {
            *slot = 0;
        }
    }

    /// C++: `BlockVersion::CheckDelete` — `deleted[offset] != 0 && <= check_ts`;
    /// an out-of-range offset is visible.
    pub fn check_delete(&self, offset: usize, check_ts: TxnTimeStamp) -> bool {
        match self.deleted.get(offset) {
            Some(&d) => d != 0 && d <= check_ts,
            None => false,
        }
    }

    /// C++: `BlockVersion::GetDeleteTS` — `size` slots starting at `offset`.
    pub fn get_delete_ts(&self, offset: usize, size: usize) -> Vec<TxnTimeStamp> {
        (offset..offset + size)
            .map(|i| self.deleted.get(i).copied().unwrap_or(0))
            .collect()
    }

    /// C++: `BlockVersion::GetCreateTS` — the create timestamp of each of
    /// `size` rows starting at `offset`.
    pub fn get_create_ts(&self, offset: usize, size: usize) -> Vec<TxnTimeStamp> {
        (offset..offset + size)
            .map(|i| self.row_create_ts(i))
            .collect()
    }

    /// Rows ever appended (the last batch's cumulative count).
    pub fn total_rows(&self) -> usize {
        self.created
            .last()
            .map(|f| f.row_count.max(0) as usize)
            .unwrap_or(0)
    }

    /// C++: `BlockVersion::GetRowCount(TxnTimeStamp)` — rows created at or
    /// before `begin_ts`.
    pub fn row_count(&self, begin_ts: TxnTimeStamp) -> usize {
        self.created
            .iter()
            .rev()
            .find(|f| f.create_ts <= begin_ts)
            .map(|f| f.row_count.max(0) as usize)
            .unwrap_or(0)
    }

    /// C++: `BlockVersion::GetCommitRowCount(commit_ts)` — the
    /// `(offset, count)` of the append batch committed exactly at
    /// `commit_ts`, so a reader outside the append txn can still see it:
    /// `rows_visible_at(begin_ts) + count`.
    ///
    /// Returns `(0, 0)` when no batch was committed at `commit_ts`, and for
    /// `MAX_TIMESTAMP` (the reference's "no commit ts" sentinel).
    pub fn commit_row_count(&self, commit_ts: TxnTimeStamp) -> (u16, i32) {
        if commit_ts == TxnTimeStamp::MAX {
            return (0, 0);
        }
        // `lower_bound` on `create_ts >= commit_ts`.
        let index = self
            .created
            .partition_point(|f| f.create_ts < commit_ts);
        let Some(field) = self.created.get(index) else {
            return (0, 0);
        };
        if field.create_ts != commit_ts {
            return (0, 0);
        }
        let total_cnt = field.row_count;
        let offset = if index == 0 {
            0
        } else {
            self.created[index - 1].row_count
        };
        (offset as u16, (total_cnt - offset) as i32)
    }

    /// The create timestamp of the row at `offset` (0 if out of range).
    pub fn row_create_ts(&self, offset: usize) -> TxnTimeStamp {
        let offset = offset as i64;
        self.created
            .iter()
            .find(|f| offset < f.row_count)
            .map(|f| f.create_ts)
            .unwrap_or(0)
    }

    /// Rows visible to snapshot `ts`: created `<= ts` and not deleted `<= ts`.
    pub fn visible_row_count(&self, ts: TxnTimeStamp) -> usize {
        (0..self.total_rows())
            .filter(|&o| self.row_create_ts(o) <= ts && !self.check_delete(o, ts))
            .count()
    }

    /// C++: `BlockVersion::latest_change_ts`.
    pub fn latest_change_ts(&self) -> TxnTimeStamp {
        self.latest_change_ts
    }

    /// C++: `BlockVersion::SaveToFile(checkpoint_ts, file_handle)` — the
    /// checkpoint-pruned `version` file.
    ///
    /// Only appends committed at or before `checkpoint_ts` are written; the
    /// rest are trimmed because the WAL replay after that checkpoint will
    /// re-append them. Delete timestamps above the checkpoint are written as
    /// `0` for the same reason. The byte layout is exactly
    /// [`BlockVersion::spill_to_file`] (and the reference's): a `u16` create
    /// count, that many `(u64 create_ts, i64 cumulative_row_count)` pairs, a
    /// `u16` capacity and one `u64` delete timestamp per slot.
    ///
    /// Returns the bytes and the reference's return value: `false` when
    /// anything was pruned (`is_modified`), i.e. the file changed relative to
    /// a checkpoint-free dump.
    pub fn save_to_file(&self, checkpoint_ts: TxnTimeStamp) -> (Vec<u8>, bool) {
        let mut create_size = self.created.len();
        let mut is_modified = false;
        while create_size > 0 && self.created[create_size - 1].create_ts > checkpoint_ts {
            create_size -= 1;
            is_modified = true;
        }
        let created = self.created[..create_size]
            .iter()
            .map(|f| crate::cpp_format::VersionCreate {
                create_ts: f.create_ts,
                cumulative_row_count: f.row_count,
            })
            .collect();
        let deleted: Vec<TxnTimeStamp> = self
            .deleted
            .iter()
            .map(|&ts| {
                if ts <= checkpoint_ts {
                    ts
                } else {
                    is_modified = true;
                    0
                }
            })
            .collect();
        (
            BlockVersionFile { created, deleted }.encode(),
            !is_modified,
        )
    }

    /// C++: `BlockVersion::SpillToFile` — the serialized `version` file.
    pub fn spill_to_file(&self) -> Vec<u8> {
        let created = self
            .created
            .iter()
            .map(|f| crate::cpp_format::VersionCreate {
                create_ts: f.create_ts,
                cumulative_row_count: f.row_count,
            })
            .collect();
        BlockVersionFile {
            created,
            deleted: self.deleted.clone(),
        }
        .encode()
    }

    /// C++: `BlockVersion::LoadFromFile`.
    pub fn load_from_file(bytes: &[u8]) -> Result<Self> {
        let file = BlockVersionFile::decode(bytes)
            .map_err(|e| Error::Storage(format!("block version file: {e:?}")))?;
        let created = file
            .created
            .iter()
            .map(|c| CreateField {
                create_ts: c.create_ts,
                row_count: c.cumulative_row_count,
            })
            .collect();
        Ok(Self {
            created,
            latest_change_ts: file.deleted.iter().copied().max().unwrap_or(0),
            deleted: file.deleted,
        })
    }

    /// C++: `BlockVersion::RestoreFromSnapshot(commit_ts)` — all appends
    /// collapse into one batch of the same total size, and every deleted slot
    /// moves to `commit_ts`; used when a block is restored from a table
    /// snapshot, where the whole block became visible at the snapshot's
    /// commit timestamp.
    pub fn restore_from_snapshot(&mut self, commit_ts: TxnTimeStamp) {
        let row_count = self.created.last().map(|f| f.row_count).unwrap_or(0);
        self.created.clear();
        self.created.push(CreateField {
            create_ts: commit_ts,
            row_count,
        });
        for ts in &mut self.deleted {
            if *ts != 0 {
                *ts = commit_ts;
            }
        }
        self.latest_change_ts = commit_ts;
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    /// `src/unit_test/storage/meta/entry/block_version_ut.cpp::SaveAndLoad`.
    #[test]
    fn save_and_load_roundtrip() {
        let mut v = BlockVersion::new(8192);
        v.append(10, 3);
        v.append(20, 9);
        v.delete(2, 30).unwrap();
        v.delete(5, 40).unwrap();
        let bytes = v.spill_to_file();
        let loaded = BlockVersion::load_from_file(&bytes).unwrap();
        assert_eq!(loaded.created, v.created);
        assert_eq!(loaded.deleted, v.deleted);
        assert_eq!(loaded.capacity(), 8192);
    }

    /// `SaveAndLoad2`: the file keeps the capacity, so a reopen sees the same
    /// slot array even before the second append is applied.
    #[test]
    fn reopen_preserves_the_slot_array() {
        let mut first = BlockVersion::new(8192);
        first.append(10, 3);
        first.delete(2, 30).unwrap();
        first.delete(5, 40).unwrap();
        let bytes = first.spill_to_file();

        let mut reopened = BlockVersion::load_from_file(&bytes).unwrap();
        assert_eq!(reopened.row_count(10), 3);
        assert!(reopened.check_delete(2, 30));
        assert!(!reopened.check_delete(2, 29));
        reopened.append(20, 9);
        assert_eq!(reopened.total_rows(), 9);
        // Row 2 is deleted at 30, row 5 not before 40 → 8 of 9 visible at 30.
        assert_eq!(reopened.visible_row_count(30), 8);
    }

    /// `delete_test`: a double delete is rejected.
    #[test]
    fn delete_twice_is_rejected() {
        let mut v = BlockVersion::new(8192);
        v.delete(2, 30).unwrap();
        assert!(v.delete(2, 30).is_err());
    }

    /// `check_delete_test`.
    #[test]
    fn check_delete_compares_timestamps() {
        let mut v = BlockVersion::new(8192);
        v.delete(2, 30).unwrap();
        assert!(v.check_delete(2, 30));
        assert!(v.check_delete(2, 40));
        assert!(!v.check_delete(2, 29));
        assert!(!v.check_delete(3, 30));
        assert!(!v.check_delete(8193, 30));
    }

    /// `get_delete_ts_test`.
    #[test]
    fn get_delete_ts_returns_the_slot_range() {
        let mut v = BlockVersion::new(8192);
        v.delete(2, 30).unwrap();
        v.delete(5, 40).unwrap();
        assert_eq!(v.get_delete_ts(2, 4), vec![30, 0, 0, 40]);
    }

    #[test]
    fn append_then_visibility() {
        let mut v = BlockVersion::new(8);
        v.append(10, 3);
        assert_eq!(v.total_rows(), 3);
        assert_eq!(v.row_count(9), 0);
        assert_eq!(v.row_count(10), 3);
        assert_eq!(v.visible_row_count(9), 0);
        assert_eq!(v.visible_row_count(10), 3);
    }

    #[test]
    fn delete_respects_snapshot() {
        let mut v = BlockVersion::new(8);
        v.append(10, 3);
        v.delete(1, 20).unwrap();
        // A reader before the delete still sees all three rows.
        assert_eq!(v.visible_row_count(15), 3);
        // A reader after it sees two.
        assert_eq!(v.visible_row_count(20), 2);
    }

    #[test]
    fn rollback_delete_restores_row() {
        let mut v = BlockVersion::new(8);
        v.append(10, 2);
        v.delete(0, 20).unwrap();
        v.rollback_delete(0);
        assert_eq!(v.visible_row_count(20), 2);
    }

    #[test]
    fn update_is_delete_plus_append() {
        let mut v = BlockVersion::new(8);
        v.append(10, 2); // rows 0,1
        v.delete(1, 20).unwrap(); // delete row 1
        v.append(20, 3); // append the replacement as row 2
        assert_eq!(v.total_rows(), 3);
        assert_eq!(v.visible_row_count(20), 2); // rows 0 and 2
        assert_eq!(v.row_create_ts(2), 20);
        assert_eq!(v.row_create_ts(0), 10);
    }

    #[test]
    fn commit_append_rewrites_the_pending_timestamp() {
        let mut v = BlockVersion::new(4);
        v.append(7, 1);
        v.commit_append(7, 11);
        assert_eq!(v.row_create_ts(0), 11);
        assert_eq!(v.latest_change_ts(), 11);
    }

    /// The fixture every checkpoint test starts from: three appends and two
    /// deletes, one of which is below and one above the checkpoint.
    fn checkpoint_fixture() -> BlockVersion {
        let mut v = BlockVersion::new(8);
        v.append(10, 3);
        v.append(20, 9);
        v.append(30, 12);
        v.delete(2, 30).unwrap();
        v.delete(5, 40).unwrap();
        v
    }

    /// C++ `BlockVersion::SaveToFile(checkpoint_ts, file_handle)`: the appends
    /// above the checkpoint are trimmed and the future deletes are zeroed,
    /// in the reference's exact byte layout.
    #[test]
    fn save_to_file_prunes_above_the_checkpoint() {
        let v = checkpoint_fixture();
        let (bytes, unchanged) = v.save_to_file(25);
        assert!(!unchanged, "the file was pruned");

        // u16 create count, 2 × (u64 ts, i64 rows), u16 capacity, 8 × u64.
        assert_eq!(bytes.len(), 2 + 2 * 16 + 2 + 8 * 8);
        let mut expected = Vec::new();
        expected.extend_from_slice(&2u16.to_le_bytes());
        for (ts, rows) in [(10u64, 3i64), (20, 9)] {
            expected.extend_from_slice(&ts.to_le_bytes());
            expected.extend_from_slice(&rows.to_le_bytes());
        }
        expected.extend_from_slice(&8u16.to_le_bytes());
        expected.extend_from_slice(&[0u8; 8 * 8]);
        assert_eq!(bytes, expected);

        let pruned = BlockVersion::load_from_file(&bytes).unwrap();
        assert_eq!(pruned.created.len(), 2);
        assert_eq!(pruned.capacity(), 8);
        assert_eq!(pruned.total_rows(), 9);
        // Delete timestamps above the checkpoint are dropped.
        assert!(!pruned.check_delete(2, 1000));
        assert!(!pruned.check_delete(5, 1000));
    }

    /// A checkpoint that covers every append and delete leaves the file
    /// identical to the spill form and reports "not modified".
    #[test]
    fn save_to_file_at_the_checkpoint_equals_the_spill_form() {
        let v = checkpoint_fixture();
        let (bytes, unchanged) = v.save_to_file(40);
        assert!(unchanged);
        assert_eq!(bytes, v.spill_to_file());

        let (bytes, unchanged) = v.save_to_file(0);
        assert!(!unchanged);
        let pruned = BlockVersion::load_from_file(&bytes).unwrap();
        assert!(pruned.created.is_empty());
        assert_eq!(pruned.total_rows(), 0);
        assert_eq!(pruned.capacity(), 8);
    }

    /// C++ `BlockVersion::GetCommitRowCount`.
    #[test]
    fn commit_row_count_returns_the_batch_range() {
        let v = checkpoint_fixture();
        assert_eq!(v.commit_row_count(10), (0, 3));
        assert_eq!(v.commit_row_count(20), (3, 6));
        assert_eq!(v.commit_row_count(30), (9, 3));
        // Committed rows a reader at `begin_ts` cannot see yet.
        assert_eq!(v.row_count(25), 9);
        assert_eq!(v.row_count(25) + v.commit_row_count(30).1 as usize, 12);
        // No batch at that timestamp, and the "no commit ts" sentinel.
        assert_eq!(v.commit_row_count(15), (0, 0));
        assert_eq!(v.commit_row_count(TxnTimeStamp::MAX), (0, 0));
    }

    /// C++ `BlockVersion::RestoreFromSnapshot(commit_ts)`.
    #[test]
    fn restore_from_snapshot_collapses_appends() {
        let mut v = checkpoint_fixture();
        v.restore_from_snapshot(100);
        assert_eq!(v.created.len(), 1);
        assert_eq!(v.row_count(99), 0);
        assert_eq!(v.row_count(100), 12);
        assert_eq!(v.get_delete_ts(0, 8), vec![0, 0, 100, 0, 0, 100, 0, 0]);
        assert_eq!(v.latest_change_ts(), 100);
        assert_eq!(v.visible_row_count(100), 10);
    }

    /// An empty block restores to a single zero-row batch.
    #[test]
    fn restore_from_snapshot_of_an_empty_block() {
        let mut v = BlockVersion::new(4);
        v.restore_from_snapshot(7);
        assert_eq!(v.total_rows(), 0);
        assert_eq!(v.row_count(7), 0);
        assert_eq!(v.latest_change_ts(), 7);
    }
}
