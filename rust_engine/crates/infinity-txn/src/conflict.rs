// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

//! The reference's conflict matrix (`NewTxn::CheckConflictTxnStore`).
//!
//! The reference does not compare key sets: every transaction carries a typed
//! store (`AppendTxnStore`, `DeleteTxnStore`, …) and each type's overload of
//! `CheckConflictTxnStore` decides, against every transaction that took an
//! earlier commit ts, whether the pair conflicts and whether re-running the
//! statement could help (`retry_query`). The rules live in
//! `src/storage/new_txn/new_txn_impl.cpp:2922-4560`; [`TxnOp`] is the port's
//! equivalent of "the type plus the fields its rules read", and
//! [`conflict_between`] is the matrix itself.

/// A row id in the reference's shape: `segment << 32 | offset`.
pub type RowKey = u64;

/// What a write transaction does. [`TxnOp::Other`] is a statement the executor
/// did not classify; the transaction manager keeps the key-set check for those.
#[derive(Clone, Debug, Default, PartialEq, Eq)]
pub enum TxnOp {
    #[default]
    Other,
    Append {
        table: String,
    },
    Import {
        table: String,
    },
    Delete {
        table: String,
        rows: Vec<RowKey>,
    },
    Update {
        table: String,
        rows: Vec<RowKey>,
        segments: Vec<u32>,
    },
    Compact {
        table: String,
        segments: Vec<u32>,
    },
    OptimizeIndex {
        table: String,
        index: String,
        segments: Vec<u32>,
    },
    DumpMemIndex {
        table: String,
        index: String,
        segments: Vec<u32>,
    },
    CreateIndex {
        table: String,
        index: String,
        columns: Vec<String>,
    },
    DropIndex {
        table: String,
        index: String,
    },
    AddColumn {
        table: String,
    },
    DropColumn {
        table: String,
        columns: Vec<String>,
    },
    CreateTable {
        db: String,
        table: String,
    },
    DropTable {
        db: String,
        table: String,
    },
    RenameTable {
        db: String,
        old: String,
        new: String,
    },
    CreateDatabase {
        db: String,
    },
    DropDatabase {
        db: String,
    },
    Snapshot {
        name: String,
        /// The table a table snapshot covers (`CreateTableSnapshotTxnStore`),
        /// which is what the appends and updates on it conflict with.
        table: Option<String>,
    },
    Cleanup,
    Checkpoint,
}

impl TxnOp {
    /// The table the operation names, when it names one (`db.table`).
    pub fn table(&self) -> Option<&str> {
        match self {
            Self::Append { table }
            | Self::Import { table }
            | Self::Delete { table, .. }
            | Self::Update { table, .. }
            | Self::Compact { table, .. }
            | Self::OptimizeIndex { table, .. }
            | Self::DumpMemIndex { table, .. }
            | Self::CreateIndex { table, .. }
            | Self::DropIndex { table, .. }
            | Self::AddColumn { table }
            | Self::DropColumn { table, .. } => Some(table),
            Self::CreateTable { table, .. } | Self::DropTable { table, .. } => Some(table),
            // A rename is judged by the name it moves away from, which is the
            // one an older transaction could have used.
            Self::RenameTable { old, .. } => Some(old),
            _ => None,
        }
    }

    fn table_eq(&self, other: &str) -> bool {
        self.table().is_some_and(|table| table == other)
    }

    fn same_table(&self, other: &Self) -> bool {
        match (self.table(), other.table()) {
            (Some(a), Some(b)) => a == b,
            _ => false,
        }
    }

    fn in_db(&self, db: &str) -> bool {
        self.table()
            .is_some_and(|table| table.split('.').next() == Some(db))
    }
}

/// Whether the pair conflicts, and whether the statement can be retried.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Conflict {
    /// `kTxnConflict`: re-running the statement against a fresh snapshot may
    /// succeed.
    Retry,
    /// `kTxnConflictNoRetry`: the object the statement targets is gone or its
    /// shape changed, so a retry would fail the same way.
    NoRetry,
}

/// `NewTxn::CheckConflictTxnStore` for the pair `(this, previous)`.
///
/// **Directional**, like the reference: only the transaction that commits
/// *later* runs the check, and only its own overload decides. A rule that
/// exists on one side alone therefore does not fire when the other side
/// commits second - `Append` knows about `DropColumn` (`retry=false`), while
/// `DropColumn` has no overload at all.
pub fn conflict_between(this: &TxnOp, previous: &TxnOp) -> Option<Conflict> {
    rule(this, previous)
}

fn intersects(a: &[u64], b: &[u64]) -> bool {
    a.iter().any(|id| b.contains(id))
}

fn segments_intersect(a: &[u32], b: &[u32]) -> bool {
    a.iter().any(|id| b.contains(id))
}

#[allow(clippy::too_many_lines)]
fn rule(this: &TxnOp, previous: &TxnOp) -> Option<Conflict> {
    use Conflict::{NoRetry, Retry};
    match this {
        // `new_txn_impl.cpp:3347-3424`: appends conflict with schema and index
        // changes on the same table (table level - the row range is not even
        // allocated yet), and with a snapshot of it.
        TxnOp::Append { table } => match previous {
            TxnOp::CreateIndex { table: t, .. } | TxnOp::DropIndex { table: t, .. } if t == table => {
                Some(Retry)
            }
            TxnOp::AddColumn { table: t } | TxnOp::DropColumn { table: t, .. } if t == table => {
                Some(NoRetry)
            }
            TxnOp::DropTable { table: t, .. } if t == table => Some(NoRetry),
            TxnOp::RenameTable { old, .. } if old == table => Some(NoRetry),
            TxnOp::DropDatabase { db } if this.in_db(db) => Some(NoRetry),
            TxnOp::Snapshot { table: Some(t), .. } if t == table => Some(Retry),
            _ => None,
        },
        // `new_txn_impl.cpp:3426-3495`: the same, without the rename rule.
        TxnOp::Import { table } => match previous {
            TxnOp::CreateIndex { table: t, .. } | TxnOp::DropIndex { table: t, .. } if t == table => {
                Some(Retry)
            }
            TxnOp::AddColumn { table: t } | TxnOp::DropColumn { table: t, .. } if t == table => {
                Some(NoRetry)
            }
            TxnOp::DropTable { table: t, .. } if t == table => Some(NoRetry),
            TxnOp::DropDatabase { db } if this.in_db(db) => Some(NoRetry),
            TxnOp::Snapshot { table: Some(t), .. } if t == table => Some(Retry),
            _ => None,
        },
        // `new_txn_impl.cpp:4135-4203`: row level against other deletes and
        // updates, table level against a compaction.
        TxnOp::Delete { rows, .. } => match previous {
            TxnOp::Compact { .. } if this.same_table(previous) => Some(Retry),
            TxnOp::Delete { rows: theirs, .. } if this.same_table(previous) => {
                intersects(rows, theirs).then_some(Retry)
            }
            TxnOp::Update { rows: theirs, .. } if this.same_table(previous) => {
                intersects(rows, theirs).then_some(Retry)
            }
            TxnOp::DropTable { .. } if this.same_table(previous) => Some(NoRetry),
            TxnOp::DropDatabase { db } if this.in_db(db) => Some(NoRetry),
            TxnOp::Snapshot { table: Some(t), .. } if Some(t.as_str()) == this.table() => {
                Some(Retry)
            }
            _ => None,
        },
        // `new_txn_impl.cpp:4269-4375`: delete-vs-update is row level,
        // update-vs-update is table level, dump is segment level.
        TxnOp::Update { rows, segments, .. } => match previous {
            TxnOp::Compact { .. }
            | TxnOp::CreateIndex { .. }
            | TxnOp::DropIndex { .. }
            | TxnOp::AddColumn { .. }
            | TxnOp::DropColumn { .. }
                if this.same_table(previous) =>
            {
                Some(Retry)
            }
            TxnOp::DumpMemIndex {
                segments: theirs, ..
            } if this.same_table(previous) => segments_intersect(segments, theirs).then_some(Retry),
            // Update-vs-update is table level, not row level.
            TxnOp::Update { .. } if this.same_table(previous) => Some(Retry),
            TxnOp::Delete { rows: theirs, .. } if this.same_table(previous) => {
                intersects(rows, theirs).then_some(Retry)
            }
            TxnOp::DropTable { .. } if this.same_table(previous) => Some(NoRetry),
            TxnOp::DropDatabase { db } if this.in_db(db) => Some(NoRetry),
            TxnOp::Snapshot { table: Some(t), .. } if Some(t.as_str()) == this.table() => {
                Some(Retry)
            }
            _ => None,
        },
        // `new_txn_impl.cpp:3666-3767`.
        TxnOp::Compact { segments, .. } => match previous {
            TxnOp::Compact { .. } if this.same_table(previous) => Some(NoRetry),
            TxnOp::CreateIndex { .. }
            | TxnOp::DropIndex { .. }
            | TxnOp::AddColumn { .. }
            | TxnOp::DropColumn { .. }
            | TxnOp::Delete { .. }
            | TxnOp::Update { .. }
                if this.same_table(previous) =>
            {
                Some(Retry)
            }
            TxnOp::OptimizeIndex {
                segments: theirs, ..
            } if this.same_table(previous) => segments_intersect(segments, theirs).then_some(NoRetry),
            TxnOp::DropTable { .. } if this.same_table(previous) => Some(NoRetry),
            TxnOp::DropDatabase { db } if this.in_db(db) => Some(NoRetry),
            TxnOp::Snapshot { table: Some(t), .. } if Some(t.as_str()) == this.table() => {
                Some(Retry)
            }
            _ => None,
        },
        // `new_txn_impl.cpp:4089-4133`: the reference treats two dumps as
        // unrecoverable; here it is a no-retry conflict with the same meaning.
        TxnOp::DumpMemIndex { index, .. } => match previous {
            TxnOp::DumpMemIndex { .. } => Some(NoRetry),
            TxnOp::DropIndex { index: i, .. } if i == index => Some(NoRetry),
            TxnOp::DropTable { .. } if this.same_table(previous) => Some(NoRetry),
            TxnOp::DropDatabase { db } if this.in_db(db) => Some(NoRetry),
            _ => None,
        },
        // `new_txn_impl.cpp:3949-4087`.
        TxnOp::OptimizeIndex { index, segments, .. } => match previous {
            TxnOp::OptimizeIndex { .. } => Some(Retry),
            TxnOp::Append { .. } | TxnOp::Update { .. } if this.same_table(previous) => Some(Retry),
            TxnOp::Compact {
                segments: theirs, ..
            } if this.same_table(previous) => segments_intersect(segments, theirs).then_some(Retry),
            TxnOp::DumpMemIndex { index: i, .. } if i == index => Some(Retry),
            TxnOp::DropIndex { index: i, .. } if i == index => Some(NoRetry),
            TxnOp::DropTable { .. } if this.same_table(previous) => Some(NoRetry),
            TxnOp::DropDatabase { db } if this.in_db(db) => Some(NoRetry),
            TxnOp::Snapshot { table: Some(t), .. } if Some(t.as_str()) == this.table() => {
                Some(Retry)
            }
            _ => None,
        },
        // `new_txn_impl.cpp:3769-3857`.
        TxnOp::CreateIndex {
            table,
            index,
            columns,
        } => match previous {
            TxnOp::CreateIndex {
                table: t, index: i, ..
            } if t == table && i == index => Some(NoRetry),
            TxnOp::Append { .. }
            | TxnOp::Import { .. }
            | TxnOp::Compact { .. }
            | TxnOp::Update { .. }
                if this.same_table(previous) =>
            {
                Some(Retry)
            }
            TxnOp::DropColumn {
                table: t,
                columns: theirs,
            } if t == table => columns
                .iter()
                .any(|c| theirs.contains(c))
                .then_some(NoRetry),
            TxnOp::DropTable { .. } if this.same_table(previous) => Some(NoRetry),
            TxnOp::DropDatabase { db } if this.in_db(db) => Some(NoRetry),
            TxnOp::Snapshot { table: Some(t), .. } if t == table => Some(Retry),
            _ => None,
        },
        // `new_txn_impl.cpp:3859-3907`.
        TxnOp::DropIndex { index, .. } => match previous {
            TxnOp::DropIndex { index: i, .. } if i == index => Some(NoRetry),
            TxnOp::DropTable { .. } if this.same_table(previous) => Some(NoRetry),
            TxnOp::DropDatabase { db } if this.in_db(db) => Some(NoRetry),
            TxnOp::Snapshot { table: Some(t), .. } if Some(t.as_str()) == this.table() => {
                Some(Retry)
            }
            _ => None,
        },
        // `new_txn_impl.cpp:3909-3947`.
        TxnOp::DropTable { .. } => match previous {
            TxnOp::DropTable { .. } | TxnOp::DropDatabase { .. }
                if self::conflict_between_drop(this, previous) =>
            {
                Some(NoRetry)
            }
            TxnOp::Snapshot { table: Some(t), .. } if Some(t.as_str()) == this.table() => {
                Some(Retry)
            }
            _ => None,
        },
        // `new_txn_impl.cpp:4205-4267`.
        TxnOp::RenameTable { old, new, .. } => match previous {
            TxnOp::CreateTable { table, .. } if table == new => Some(NoRetry),
            TxnOp::RenameTable { old: o, .. } if o == old => Some(NoRetry),
            TxnOp::DropTable { .. } if this.same_table(previous) => Some(NoRetry),
            TxnOp::DropDatabase { db } if this.in_db(db) => Some(NoRetry),
            TxnOp::Snapshot { table: Some(t), .. } if t == old || t == new => Some(Retry),
            _ => None,
        },
        // `new_txn_impl.cpp:3215-3243`.
        TxnOp::CreateTable { .. } => match previous {
            TxnOp::RenameTable { new: n, .. } if Some(n.as_str()) == this.table() => Some(NoRetry),
            TxnOp::DropDatabase { db } if this.in_db(db) => Some(NoRetry),
            TxnOp::Snapshot { table: Some(t), .. } if Some(t.as_str()) == this.table() => {
                Some(Retry)
            }
            _ => None,
        },
        // `new_txn_impl.cpp:3160-3186`.
        TxnOp::DropDatabase { db } => match previous {
            TxnOp::DropDatabase { db: d } if d == db => Some(NoRetry),
            TxnOp::Snapshot { .. } => None,
            _ => None,
        },
        // `new_txn_impl.cpp:4377-4534`.
        TxnOp::Snapshot { name, .. } => match previous {
            TxnOp::Snapshot { name: n, .. } if n == name => Some(NoRetry),
            TxnOp::Cleanup => Some(Retry),
            _ => None,
        },
        // The reference has no overload of its own for a schema change: the
        // appends, updates, compactions and index creations on the table see it
        // through their rules. Two `ALTER TABLE`s serialise on the catalog.
        TxnOp::AddColumn { .. } | TxnOp::DropColumn { .. } => None,
        TxnOp::CreateDatabase { .. } | TxnOp::Cleanup | TxnOp::Checkpoint | TxnOp::Other => None,
    }
}

fn conflict_between_drop(this: &TxnOp, previous: &TxnOp) -> bool {
    match (this, previous) {
        (
            TxnOp::DropTable { db: a, table: t },
            TxnOp::DropTable { db: b, table: u },
        ) => a == b && t == u,
        (TxnOp::DropTable { db, .. }, TxnOp::DropDatabase { db: other }) => db == other,
        (TxnOp::DropTable { .. }, _) => false,
        _ => false,
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn append(table: &str) -> TxnOp {
        TxnOp::Append {
            table: table.into(),
        }
    }

    fn delete(table: &str, rows: &[u64]) -> TxnOp {
        TxnOp::Delete {
            table: table.into(),
            rows: rows.to_vec(),
        }
    }

    /// `new_txn_impl.cpp:3347-3424`: an append is table level - the row range
    /// does not exist yet when the check runs.
    #[test]
    fn append_conflicts_with_schema_and_index_changes_only() {
        let add = TxnOp::AddColumn {
            table: "db.t".into(),
        };
        assert_eq!(conflict_between(&append("db.t"), &add), Some(Conflict::NoRetry));
        // Directional: a schema change has no overload of its own, so it does
        // not conflict when *it* commits second.
        assert_eq!(conflict_between(&add, &append("db.t")), None);

        let index = TxnOp::CreateIndex {
            table: "db.t".into(),
            index: "i".into(),
            columns: vec!["c".into()],
        };
        assert_eq!(conflict_between(&append("db.t"), &index), Some(Conflict::Retry));
        // `CreateIndex` has the mirrored rule of its own.
        assert_eq!(conflict_between(&index, &append("db.t")), Some(Conflict::Retry));

        // Another table's schema change is not this append's business.
        let other = TxnOp::AddColumn {
            table: "db.u".into(),
        };
        assert_eq!(conflict_between(&append("db.t"), &other), None);
        // Two appends never conflict: the allocator hands out disjoint ranges.
        assert_eq!(conflict_between(&append("db.t"), &append("db.t")), None);
    }

    /// `new_txn_impl.cpp:4147-4169`: two deletes are row level.
    #[test]
    fn deletes_conflict_only_on_overlapping_rows() {
        assert_eq!(
            conflict_between(&delete("db.t", &[1, 2]), &delete("db.t", &[2, 3])),
            Some(Conflict::Retry)
        );
        assert_eq!(
            conflict_between(&delete("db.t", &[1, 2]), &delete("db.t", &[3, 4])),
            None
        );
        let update = TxnOp::Update {
            table: "db.t".into(),
            rows: vec![2],
            segments: vec![0],
        };
        assert_eq!(
            conflict_between(&delete("db.t", &[2]), &update),
            Some(Conflict::Retry)
        );
        assert_eq!(
            conflict_between(&delete("db.t", &[9]), &update),
            None
        );
    }

    /// `new_txn_impl.cpp:4336-4341`: two updates conflict on the whole table,
    /// even when the rows are disjoint.
    #[test]
    fn updates_conflict_at_table_level() {
        let a = TxnOp::Update {
            table: "db.t".into(),
            rows: vec![1],
            segments: vec![0],
        };
        let b = TxnOp::Update {
            table: "db.t".into(),
            rows: vec![2],
            segments: vec![0],
        };
        assert_eq!(conflict_between(&a, &b), Some(Conflict::Retry));
    }

    /// `new_txn_impl.cpp:4313-4324`: an update meets a dump only when the
    /// segments overlap.
    #[test]
    fn update_and_dump_conflict_per_segment() {
        let update = TxnOp::Update {
            table: "db.t".into(),
            rows: vec![1],
            segments: vec![2],
        };
        let dump = |segments: &[u32]| TxnOp::DumpMemIndex {
            table: "db.t".into(),
            index: "i".into(),
            segments: segments.to_vec(),
        };
        assert_eq!(conflict_between(&update, &dump(&[2])), Some(Conflict::Retry));
        assert_eq!(conflict_between(&update, &dump(&[3])), None);
    }

    /// `new_txn_impl.cpp:3666-3767`: a compaction conflicts with writes on the
    /// table, and two compactions on one table never both run.
    #[test]
    fn compaction_is_exclusive_per_table() {
        let compact = TxnOp::Compact {
            table: "db.t".into(),
            segments: vec![0, 1],
        };
        assert_eq!(
            conflict_between(&compact, &compact),
            Some(Conflict::NoRetry)
        );
        assert_eq!(
            conflict_between(&compact, &delete("db.t", &[1])),
            Some(Conflict::Retry)
        );
        assert_eq!(
            conflict_between(&compact, &append("db.t")),
            None,
            "the reference has no append-vs-compaction rule"
        );
    }

    /// `new_txn_impl.cpp:4089-4098`: two dumps on one directory are
    /// unrecoverable in the reference, a no-retry conflict here.
    #[test]
    fn two_dumps_conflict() {
        let dump = TxnOp::DumpMemIndex {
            table: "db.t".into(),
            index: "i".into(),
            segments: vec![0],
        };
        assert_eq!(conflict_between(&dump, &dump), Some(Conflict::NoRetry));
    }

    /// `new_txn_impl.cpp:3909-3947`: dropping a table conflicts with another
    /// drop of it and with dropping its database.
    #[test]
    fn dropping_a_table_conflicts_with_its_database() {
        let drop_table = TxnOp::DropTable {
            db: "db".into(),
            table: "db.t".into(),
        };
        let drop_db = TxnOp::DropDatabase { db: "db".into() };
        assert_eq!(
            conflict_between(&drop_table, &drop_db),
            Some(Conflict::NoRetry)
        );
        assert_eq!(
            conflict_between(&append("db.t"), &drop_table),
            Some(Conflict::NoRetry)
        );
    }
}
