//! Timestamp-array MVCC (`BlockVersion`), mirroring Infinity's
//! `src/storage/catalog/meta/block_version.cppm`.
//!
//! Rows are never mutated in place: an update is *delete + append*. Visibility
//! is decided by comparing per-row delete timestamps and per-append create
//! timestamps against the reader's `begin_ts` (snapshot isolation by timestamp).
//! This deliberately does **not** use KV snapshots (plan 附录 A.2).

use infinity_common::TxnTimeStamp;

/// One append batch: `row_count` rows became visible at `create_ts`.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct CreateField {
    pub create_ts: TxnTimeStamp,
    pub row_count: u32,
}

/// MVCC state of a single block.
#[derive(Debug, Clone, Default)]
pub struct BlockVersion {
    created: Vec<CreateField>,
    /// Per-row delete timestamp; `0` means "not deleted".
    deleted: Vec<TxnTimeStamp>,
}

impl BlockVersion {
    pub fn new() -> Self {
        Self::default()
    }

    /// Number of rows ever appended (including deleted ones).
    pub fn total_rows(&self) -> usize {
        self.deleted.len()
    }

    /// Records that `row_count` rows were appended and committed at `create_ts`.
    pub fn append(&mut self, create_ts: TxnTimeStamp, row_count: u32) {
        self.created.push(CreateField {
            create_ts,
            row_count,
        });
        self.deleted
            .resize(self.deleted.len() + row_count as usize, 0);
    }

    /// Marks a single row deleted at `commit_ts`. Idempotent guard: a row already
    /// deleted (by an earlier committed txn) must not be re-deleted.
    pub fn delete(&mut self, offset: usize, commit_ts: TxnTimeStamp) -> bool {
        if offset >= self.deleted.len() || self.deleted[offset] != 0 {
            return false;
        }
        self.deleted[offset] = commit_ts;
        true
    }

    /// Rolls back a delete that has not been committed.
    pub fn rollback_delete(&mut self, offset: usize) {
        if offset < self.deleted.len() {
            self.deleted[offset] = 0;
        }
    }

    /// Is `offset` deleted as of snapshot `ts`?
    pub fn check_delete(&self, offset: usize, ts: TxnTimeStamp) -> bool {
        match self.deleted.get(offset) {
            Some(&d) => d != 0 && d <= ts,
            None => false,
        }
    }

    /// The create timestamp of the row at `offset` (0 if out of range).
    pub fn row_create_ts(&self, offset: usize) -> TxnTimeStamp {
        let mut seen = 0usize;
        for f in &self.created {
            seen += f.row_count as usize;
            if offset < seen {
                return f.create_ts;
            }
        }
        0
    }

    /// Rows created at or before `ts` (ignoring deletes).
    pub fn row_count(&self, ts: TxnTimeStamp) -> usize {
        self.created
            .iter()
            .filter(|f| f.create_ts <= ts)
            .map(|f| f.row_count as usize)
            .sum()
    }

    /// Rows visible to snapshot `ts`: created `<= ts` and not deleted `<= ts`.
    pub fn visible_row_count(&self, ts: TxnTimeStamp) -> usize {
        (0..self.total_rows())
            .filter(|&o| self.row_create_ts(o) <= ts && !self.check_delete(o, ts))
            .count()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn append_then_visibility() {
        let mut v = BlockVersion::new();
        v.append(10, 3);
        assert_eq!(v.total_rows(), 3);
        assert_eq!(v.row_count(9), 0);
        assert_eq!(v.row_count(10), 3);
        assert_eq!(v.visible_row_count(9), 0);
        assert_eq!(v.visible_row_count(10), 3);
    }

    #[test]
    fn delete_respects_snapshot() {
        let mut v = BlockVersion::new();
        v.append(10, 3);
        assert!(v.delete(1, 20));
        // reader before the delete still sees 3
        assert_eq!(v.visible_row_count(15), 3);
        // reader after sees 2
        assert_eq!(v.visible_row_count(20), 2);
        // re-delete is rejected
        assert!(!v.delete(1, 25));
    }

    #[test]
    fn rollback_delete_restores_row() {
        let mut v = BlockVersion::new();
        v.append(10, 2);
        assert!(v.delete(0, 20));
        v.rollback_delete(0);
        assert_eq!(v.visible_row_count(20), 2);
    }

    #[test]
    fn update_is_delete_plus_append() {
        let mut v = BlockVersion::new();
        v.append(10, 2); // rows 0,1
        v.delete(1, 20); // delete row 1
        v.append(20, 1); // append replacement at row 2
        assert_eq!(v.total_rows(), 3);
        assert_eq!(v.visible_row_count(20), 2); // rows 0 and 2
        assert_eq!(v.row_create_ts(2), 20);
        assert_eq!(v.row_create_ts(0), 10);
    }
}
