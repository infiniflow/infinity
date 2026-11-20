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

//! Tests for the commit pipeline, the typed WAL and the transaction manager.

use super::*;
use storage::RocksMetaStore;
use tempfile::tempdir;

async fn manager(dir: &std::path::Path) -> TxnManager {
    let store = Arc::new(RocksMetaStore::open(dir.join("kv")).unwrap());
    TxnManager::open(dir.join("wal.log"), store).await.unwrap()
}

fn row(table: &str, seq: u64) -> Vec<u8> {
    row_key(table, seq)
}

#[tokio::test]
async fn commit_applies_to_kv() {
    let dir = tempdir().unwrap();
    {
        let store: Arc<dyn MetaStore> =
            Arc::new(RocksMetaStore::open(dir.path().join("kv")).unwrap());
        let m = TxnManager::open(dir.path().join("wal.log"), store.clone())
            .await
            .unwrap();
        let mut t = m.begin();
        t.put(b"a".to_vec(), b"1".to_vec());
        t.put(b"b".to_vec(), b"2".to_vec());
        let ts = (m.commit(t)).await.unwrap();
        assert!(ts >= 1);
        // commit is visible in the KV store immediately
        assert_eq!(store.get(b"a").unwrap(), Some(b"1".to_vec()));
        assert_eq!(store.get(b"b").unwrap(), Some(b"2".to_vec()));
    }
    // reopen after the handle is released
    let m2 = manager(dir.path()).await;
    assert_eq!(m2.recover(0).await.unwrap(), 1); // idempotent re-apply
}

#[tokio::test]
async fn recovery_replays_wal_after_reopen() {
    let dir = tempdir().unwrap();
    {
        let m = manager(dir.path()).await;
        for i in 0..3u64 {
            let mut t = m.begin();
            t.put(format!("k{i}").into_bytes(), format!("v{i}").into_bytes());
            (m.commit(t)).await.unwrap();
        }
    }
    // fresh manager + fresh store: KV empty, WAL has 3 -> recover
    let store = Arc::new(RocksMetaStore::open(dir.path().join("kv2")).unwrap());
    let m = TxnManager::open(dir.path().join("wal.log"), store.clone())
        .await
        .unwrap();
    assert_eq!(m.recover(0).await.unwrap(), 3);
    assert_eq!(store.get(b"k0").unwrap(), Some(b"v0".to_vec()));
    assert_eq!(store.get(b"k2").unwrap(), Some(b"v2".to_vec()));
    // idempotent
    assert_eq!(m.recover(0).await.unwrap(), 3);
}

#[tokio::test]
async fn concurrent_commits_are_serialized_and_durable() {
    use std::collections::BTreeSet;
    let dir = tempdir().unwrap();
    const THREADS: u64 = 4;
    const PER: u64 = 25;
    {
        let store: Arc<dyn MetaStore> =
            Arc::new(RocksMetaStore::open(dir.path().join("kv")).unwrap());
        let mgr = Arc::new(
            TxnManager::open(dir.path().join("wal.log"), store.clone())
                .await
                .unwrap(),
        );
        let handles: Vec<_> = (0..THREADS)
            .map(|t| {
                let mgr = mgr.clone();
                tokio::spawn(async move {
                    for j in 0..PER {
                        let mut tx = mgr.begin();
                        tx.put(
                            format!("k{t}_{j}").into_bytes(),
                            format!("v{t}_{j}").into_bytes(),
                        );
                        mgr.commit(tx).await.unwrap();
                    }
                })
            })
            .collect();
        for h in handles {
            h.await.unwrap();
        }
        mgr.flush().unwrap();
    }
    // all rows are durable in KV
    let store: Arc<dyn MetaStore> = Arc::new(RocksMetaStore::open(dir.path().join("kv")).unwrap());
    assert_eq!(
        store.scan_prefix(b"k").unwrap().len(),
        (THREADS * PER) as usize
    );

    // WAL has one record per commit, with unique commit timestamps
    let recs = WalManager::read_all(dir.path().join("wal.log"))
        .await
        .unwrap();
    assert_eq!(recs.len(), (THREADS * PER) as usize);
    let ts: BTreeSet<u64> = recs.iter().map(|r| r.commit_ts).collect();
    assert_eq!(
        ts.len(),
        (THREADS * PER) as usize,
        "commit timestamps must be unique"
    );
}

#[tokio::test]
async fn truncated_wal_recovers_exactly_the_valid_prefix() {
    let dir = tempdir().unwrap();
    let wal = dir.path().join("wal.log");
    {
        let store: Arc<dyn MetaStore> =
            Arc::new(RocksMetaStore::open(dir.path().join("kv")).unwrap());
        let m = TxnManager::open(&wal, store).await.unwrap();
        for i in 0..10u64 {
            let mut t = m.begin();
            t.put(row("default_db.t", i), format!("v{i}").into_bytes());
            (m.commit(t)).await.unwrap();
        }
    }
    let full = std::fs::read(&wal).unwrap();
    // simulate a crash that truncates the WAL at many different offsets
    for cut in (0..full.len()).step_by(41) {
        let d2 = tempdir().unwrap();
        // Checkpoint frames and records share `wal.log`, so the truncated
        // prefix is written to the same name the manager opens.
        let wal2 = d2.path().join("wal.log");
        std::fs::write(&wal2, &full[..cut]).unwrap();
        let store: Arc<dyn MetaStore> =
            Arc::new(RocksMetaStore::open(d2.path().join("kv")).unwrap());
        let m = TxnManager::open(d2.path().join("wal.log"), store.clone())
            .await
            .unwrap();
        let mut appends = Vec::new();
        let mut deletes = Vec::new();
        m.recover_into(0, 0, &mut appends, &mut deletes)
            .await
            .unwrap(); // must not panic
        let expected = WalManager::read_all(&wal2).await.unwrap().len();
        let got = appends.iter().map(|a| a.rows.len()).sum::<usize>();
        assert_eq!(
            got, expected,
            "cut={cut}: recovered {got} rows, expected {expected}"
        );
    }
}

#[tokio::test]
async fn empty_txn_rejected() {
    let dir = tempdir().unwrap();
    let m = manager(dir.path()).await;
    assert!((m.commit(m.begin())).await.is_err());
}

#[tokio::test]
async fn concurrent_deletes_conflict() {
    let dir = tempdir().unwrap();
    let m = manager(dir.path()).await;
    let mut t1 = m.begin();
    t1.delete(b"row:1".to_vec());
    let mut t2 = m.begin();
    t2.delete(b"row:1".to_vec());
    (m.commit(t1)).await.unwrap();
    assert!(matches!((m.commit(t2)).await, Err(Error::TxnConflict(_))));

    // A transaction that starts after the winner is not stale.
    let mut t3 = m.begin();
    t3.delete(b"row:1".to_vec());
    (m.commit(t3)).await.unwrap();

    // Disjoint keys never conflict.
    let mut t4 = m.begin();
    t4.delete(b"row:2".to_vec());
    let mut t5 = m.begin();
    t5.delete(b"row:3".to_vec());
    (m.commit(t5)).await.unwrap();
    (m.commit(t4)).await.unwrap();
}

/// The commit path writes a typed `WalCmd::Append` (not a KV batch), and
/// recovery reproduces exactly the rows the KV held before the restart.
#[tokio::test]
async fn append_is_a_typed_command_and_replays_the_same_rows() {
    let dir = tempdir().unwrap();
    let table = "default_db.t";
    let expected: Vec<(Vec<u8>, Vec<u8>)> = (0..5u64)
        .map(|i| (row(table, i), format!("{i}\trow-{i}").into_bytes()))
        .collect();
    {
        let store: Arc<dyn MetaStore> =
            Arc::new(RocksMetaStore::open(dir.path().join("kv")).unwrap());
        let m = TxnManager::open(dir.path().join("wal.log"), store.clone())
            .await
            .unwrap();
        let mut t = m.begin();
        for (key, value) in &expected {
            t.put(key.clone(), value.clone());
        }
        (m.commit(t)).await.unwrap();
        assert_eq!(store.scan_prefix(b"row:").unwrap(), expected);
    }

    // The record is a typed Append with one coalesced row range.
    let mut iter = WalEntryIterator::make(dir.path().join("wal.log"), false)
        .await
        .unwrap();
    let entry = iter.next().expect("one record");
    assert!(!iter.has_next());
    assert_eq!(entry.cmds.len(), 1);
    match &entry.cmds[0] {
        WalCmd::Append {
            db_name,
            table_name,
            row_ranges,
            block,
            ..
        } => {
            assert_eq!(db_name, "default_db");
            assert_eq!(table_name, "t");
            assert_eq!(row_ranges, &[(WalRowId::new(0, 0), 5)]);
            assert_eq!(block.len(), 1);
            assert_eq!(block[0].values.varchar(3).as_deref(), Some("3\trow-3"));
        }
        other => panic!("expected an Append command, got {other:?}"),
    }

    // Recovery hands the rows back for the engine to append to the segment
    // (`AppendInBlock`); the log itself no longer carries `row:` keys.
    let store = Arc::new(RocksMetaStore::open(dir.path().join("kv2")).unwrap());
    let m = TxnManager::open(dir.path().join("wal.log"), store.clone())
        .await
        .unwrap();
    let mut appends = Vec::new();
    let mut deletes = Vec::new();
    m.recover_into(0, 0, &mut appends, &mut deletes)
        .await
        .unwrap();
    assert!(deletes.is_empty());
    let rows: Vec<(u64, String)> = appends.iter().flat_map(|a| a.rows.clone()).collect();
    assert_eq!(rows.len(), expected.len());
    for ((seq, text), (key, value)) in rows.iter().zip(expected.iter()) {
        assert_eq!(&row("default_db.t", *seq), key);
        assert_eq!(text.as_bytes(), value.as_slice());
    }
}

/// A delete commits as a typed `WalCmd::Delete` and replays the tombstones.
#[tokio::test]
async fn delete_is_a_typed_command_and_replays_the_tombstones() {
    let dir = tempdir().unwrap();
    let table = "default_db.t";
    {
        let store: Arc<dyn MetaStore> =
            Arc::new(RocksMetaStore::open(dir.path().join("kv")).unwrap());
        let m = TxnManager::open(dir.path().join("wal.log"), store.clone())
            .await
            .unwrap();
        let mut t = m.begin();
        t.delete(del_key(table, 3));
        t.delete(del_key(table, 4));
        (m.commit(t)).await.unwrap();
        assert_eq!(store.scan_prefix(b"del:").unwrap().len(), 2);
    }
    let mut iter = WalEntryIterator::make(dir.path().join("wal.log"), false)
        .await
        .unwrap();
    let entry = iter.next().expect("one record");
    match &entry.cmds[0] {
        WalCmd::Delete {
            db_name,
            table_name,
            row_ids,
            ..
        } => {
            assert_eq!(db_name, "default_db");
            assert_eq!(table_name, "t");
            assert_eq!(row_ids, &[WalRowId::new(0, 3), WalRowId::new(0, 4)]);
        }
        other => panic!("expected a Delete command, got {other:?}"),
    }

    // The tombstones come back as ids for the block's `version`
    // (`DeleteInBlock`); the log holds no `del:` key of its own.
    let store = Arc::new(RocksMetaStore::open(dir.path().join("kv2")).unwrap());
    let m = TxnManager::open(dir.path().join("wal.log"), store.clone())
        .await
        .unwrap();
    let mut appends = Vec::new();
    let mut deletes = Vec::new();
    m.recover_into(0, 0, &mut appends, &mut deletes)
        .await
        .unwrap();
    assert!(appends.is_empty());
    assert_eq!(deletes.len(), 1);
    assert_eq!(deletes[0].table, table);
    assert_eq!(deletes[0].row_ids, vec![3, 4]);
}

/// Catalog records keep their exact value in a `KvBatch` and replay verbatim.
#[tokio::test]
async fn typed_commands_replace_the_catalog_kv_batch() {
    // A DDL that prepared typed WAL commands emits them instead of a `KvBatch`
    // for the same catalog records (`build_commands`).
    let dir = tempdir().unwrap();
    {
        let store: Arc<dyn MetaStore> =
            Arc::new(RocksMetaStore::open(dir.path().join("kv")).unwrap());
        let m = TxnManager::open(dir.path().join("wal.log"), store)
            .await
            .unwrap();
        let mut t = m.begin();
        t.put(b"tbl:0000000000000001".to_vec(), b"catalog-record".to_vec());
        t.set_typed_commands(vec![WalCmd::Checkpoint { max_commit_ts: 7 }]);
        (m.commit(t)).await.unwrap();
    }
    let mut iter = WalEntryIterator::make(dir.path().join("wal.log"), false)
        .await
        .unwrap();
    let entry = iter.next().expect("one record");
    assert!(
        matches!(
            entry.cmds.as_slice(),
            [WalCmd::Checkpoint { max_commit_ts: 7 }]
        ),
        "expected the typed command to replace the KvBatch, got {:?}",
        entry.cmds
    );
}

#[tokio::test]
async fn catalog_writes_are_carried_as_a_kv_batch() {
    let dir = tempdir().unwrap();
    let key = b"tbl:0000000000000001".to_vec();
    let value = b"catalog-record".to_vec();
    {
        let store: Arc<dyn MetaStore> =
            Arc::new(RocksMetaStore::open(dir.path().join("kv")).unwrap());
        let m = TxnManager::open(dir.path().join("wal.log"), store.clone())
            .await
            .unwrap();
        let mut t = m.begin();
        t.put(key.clone(), value.clone());
        (m.commit(t)).await.unwrap();
    }
    let mut iter = WalEntryIterator::make(dir.path().join("wal.log"), false)
        .await
        .unwrap();
    let entry = iter.next().expect("one record");
    match &entry.cmds[0] {
        WalCmd::KvBatch { kind, writes } => {
            assert_eq!(*kind, KvBatchKind::Catalog);
            assert_eq!(writes, &[(key.clone(), value.clone())]);
        }
        other => panic!("expected a KvBatch command, got {other:?}"),
    }
    let store = Arc::new(RocksMetaStore::open(dir.path().join("kv2")).unwrap());
    let m = TxnManager::open(dir.path().join("wal.log"), store.clone())
        .await
        .unwrap();
    m.recover(0).await.unwrap();
    assert_eq!(store.get(&key).unwrap(), Some(value));
}

#[tokio::test]
async fn rollback_frees_the_slot_and_the_next_txn_does_not_conflict() {
    let dir = tempdir().unwrap();
    let m = manager(dir.path()).await;
    let mut t1 = m.begin();
    t1.delete(b"row:1".to_vec());
    assert_eq!(m.active_txn_count(), 1);
    m.rollback(t1).unwrap();
    assert_eq!(m.active_txn_count(), 0);
    assert_eq!(m.total_rollbacked_txn_count(), 1);

    // The rolled-back transaction never committed, so the same delete succeeds.
    let mut t2 = m.begin();
    t2.delete(b"row:1".to_vec());
    (m.commit(t2)).await.unwrap();
    assert_eq!(m.total_committed_txn_count(), 1);

    // Nothing reached the WAL: only the committed record is there.
    let recs = WalManager::read_all(dir.path().join("wal.log"))
        .await
        .unwrap();
    assert_eq!(recs.len(), 1);
}

#[tokio::test]
async fn overlapping_write_ranges_conflict() {
    let dir = tempdir().unwrap();
    let m = manager(dir.path()).await;
    let mut t1 = m.begin();
    let mut t2 = m.begin();
    t1.put(b"k".to_vec(), b"a".to_vec());
    t2.put(b"k".to_vec(), b"b".to_vec());
    (m.commit(t1)).await.unwrap();
    assert!(matches!((m.commit(t2)).await, Err(Error::TxnConflict(_))));
}

/// `NewTxnManager::GetCheckCandidateTxns`: the candidate test is on the KV
/// commit timestamp, not on the commit ts, and the pair the transaction froze
/// at begin (`last_kv_commit_ts` / `last_commit_ts`) decides the boundary.
#[tokio::test]
async fn a_snapshot_that_saw_a_commit_does_not_conflict_with_it() {
    let dir = tempdir().unwrap();
    let m = manager(dir.path()).await;
    let update = |rows: Vec<u64>| TxnOp::Update {
        table: "default_db.t".into(),
        rows,
        segments: vec![0],
    };

    // A commits an update of row 1. Its KV batch becomes visible one step
    // after the commit (`CommitKVInstance`), which is what the manager records.
    let mut a = m.begin();
    a.set_op(update(vec![1]));
    a.put(row("default_db.t", 1), b"a".to_vec());
    let a_ts = (m.commit(a)).await.unwrap();
    assert_eq!(*m.kv_clock.lock().unwrap(), (a_ts + 1, a_ts));

    // B begins after it: its `begin_ts` lands exactly on A's KV commit ts
    // (case 2 of the reference). Update-vs-update is table level, so the only
    // thing that can keep B out of a conflict is A being visible to it.
    let mut b = m.begin();
    assert_eq!(b.begin_ts, a_ts + 1, "begin_ts == A's kv commit ts");
    assert_eq!(
        (b.last_system_kv_commit_ts, b.last_system_commit_ts),
        (a_ts + 1, a_ts)
    );
    b.set_op(update(vec![2]));
    b.put(row("default_db.t", 2), b"b".to_vec());
    (m.commit(b))
        .await
        .expect("A is visible to B, so the pair does not conflict");

    // C began before A committed: same rule, but A's batch is newer than C's
    // snapshot, so C is rejected.
    let mut c = m.begin();
    let mut d = m.begin();
    d.set_op(update(vec![3]));
    d.put(row("default_db.t", 3), b"d".to_vec());
    (m.commit(d)).await.unwrap();
    c.set_op(update(vec![4]));
    c.put(row("default_db.t", 4), b"c".to_vec());
    assert!(matches!((m.commit(c)).await, Err(Error::TxnConflict(_))));
}

#[tokio::test]
async fn read_sets_and_ranges_conflict() {
    let dir = tempdir().unwrap();
    let m = manager(dir.path()).await;
    let mut writer = m.begin();
    writer.put(row("default_db.t", 0), b"a".to_vec());

    // A read-only transaction is never validated: the reference hands it
    // `current_ts + 1` and returns before `CheckConflict1`
    // (`NewTxn::Commit`, `new_txn_impl.cpp:2035-2057`).
    let mut reader = m.begin();
    reader.read(row("default_db.t", 0));
    (m.commit(writer)).await.unwrap();
    assert_eq!((m.commit(reader)).await.unwrap(), m.current_ts() + 1);

    // A *write* transaction that read the key does conflict.
    let mut read_writer = m.begin();
    read_writer.read(row("default_db.t", 0));
    read_writer.put(b"unrelated".to_vec(), b"x".to_vec());
    let mut writer2 = m.begin();
    writer2.put(row("default_db.t", 0), b"b".to_vec());
    (m.commit(writer2)).await.unwrap();
    assert!(matches!(
        (m.commit(read_writer)).await,
        Err(Error::TxnConflict(_))
    ));

    // A range read sees any newer write under the prefix.
    let mut ranged = m.begin();
    ranged.read_prefix(b"row:default_db.t:");
    ranged.put(b"unrelated2".to_vec(), b"y".to_vec());
    let mut writer3 = m.begin();
    writer3.put(row("default_db.t", 1), b"b".to_vec());
    (m.commit(writer3)).await.unwrap();
    assert!(matches!(
        (m.commit(ranged)).await,
        Err(Error::TxnConflict(_))
    ));

    // A read from before the writes, committed without overlap, is fine.
    let mut clean = m.begin();
    clean.read(b"untouched".to_vec());
    clean.put(b"other".to_vec(), b"c".to_vec());
    (m.commit(clean)).await.unwrap();
}

#[tokio::test]
async fn active_txn_count_tracks_begin_commit_and_drop() {
    let dir = tempdir().unwrap();
    let m = manager(dir.path()).await;
    assert_eq!(m.active_txn_count(), 0);

    let mut t = m.begin_with_text("INSERT INTO t VALUES (1)");
    assert_eq!(m.active_txn_count(), 1);
    assert_eq!(m.oldest_alive_ts(), t.begin_ts);
    let info = m.get_txn_info_by_id(t.id).expect("registered");
    assert_eq!(info.txn_text, "INSERT INTO t VALUES (1)");
    assert_eq!(m.get_txn_info_array().len(), 1);

    t.put(b"k".to_vec(), b"v".to_vec());
    (m.commit(t)).await.unwrap();
    assert_eq!(m.active_txn_count(), 0);
    assert_eq!(m.oldest_alive_ts(), 0);
    assert!(m.get_txn_info_by_id(1).is_none());

    // Dropping an uncommitted transaction releases its slot too.
    let abandoned = m.begin();
    assert_eq!(m.active_txn_count(), 1);
    drop(abandoned);
    assert_eq!(m.active_txn_count(), 0);
}

/// The conflict map is rebuilt from the WAL on open (nothing is persisted
/// separately); every key a record wrote is present with its commit ts.
#[tokio::test]
async fn restart_rebuilds_the_conflict_state_from_the_wal() {
    let dir = tempdir().unwrap();
    {
        let m = manager(dir.path()).await;
        let mut t = m.begin();
        t.put(row("default_db.t", 0), b"a".to_vec());
        t.delete(del_key("default_db.t", 7));
        (m.commit(t)).await.unwrap();
    }
    let m = manager(dir.path()).await;
    let seen = m.write_ts.lock().unwrap();
    // `TxnManager::open` spends the recovery transaction's timestamp first
    // (`prepare_commit_ts_ = start_ts + 2`, the reference's
    // `BeginRecoveryTxn`), so the first commit here is 4, not 2.
    assert_eq!(seen.get(&row("default_db.t", 0)), Some(&4));
    assert_eq!(seen.get(&del_key("default_db.t", 7)), Some(&4));
}

#[tokio::test]
async fn checkpoint_ts_skips_already_applied_records() {
    let dir = tempdir().unwrap();
    {
        let m = manager(dir.path()).await;
        for i in 0..3u64 {
            let mut t = m.begin();
            t.put(row("default_db.t", i), format!("v{i}").into_bytes());
            (m.commit(t)).await.unwrap();
        }
    }
    let store = Arc::new(RocksMetaStore::open(dir.path().join("kv2")).unwrap());
    let m = TxnManager::open(dir.path().join("wal.log"), store.clone())
        .await
        .unwrap();
    // The three commits took timestamps 4, 6 and 8 (`open` spends the
    // recovery transaction's ts first): replaying past the first one leaves
    // the other two.
    assert_eq!(m.recover(4).await.unwrap(), 2);
    let mut appends = Vec::new();
    let mut deletes = Vec::new();
    assert_eq!(
        m.recover_into(4, 4, &mut appends, &mut deletes)
            .await
            .unwrap(),
        2
    );
    let rows: usize = appends.iter().map(|a| a.rows.len()).sum();
    assert_eq!(rows, 2);
}

/// A checkpoint frame is the replay floor: every record at or below it is
/// skipped because the checkpoint's files hold it. A frame written by a
/// WAL-only rotation therefore may not name the clock - the commits since the
/// last flush are in the RocksDB memtable (catalog writes carry no log) and in
/// the block buffer, so naming them hides the entries that recreate them.
///
/// Measured on `test_alter_simple`: the periodic trigger appended such a frame
/// between the DDLs and the flush that follows it, the harness killed the
/// server in that window, and the restart found an empty catalog with a frame
/// claiming ts 17.
#[tokio::test]
async fn a_wal_only_checkpoint_claims_only_the_flushed_commits() {
    let dir = tempdir().unwrap();
    let m = manager(dir.path()).await;
    let mut t = m.begin();
    t.put(row("default_db.t", 0), b"v".to_vec());
    let committed = (m.commit(t)).await.unwrap();
    assert_eq!(m.synced_ts(), 0, "no store flush has happened yet");
    (m.append_checkpoint_flushed()).await.unwrap();
    let floor = m.applied_ts().await.unwrap();
    assert!(
        floor <= committed,
        "the frame names ts {floor}, which hides the commit at {committed} from the replay"
    );
    // Once the store is flushed the frame may claim it.
    m.flush().unwrap();
    (m.append_checkpoint_flushed()).await.unwrap();
    assert!(m.applied_ts().await.unwrap() > committed);
}

/// The clock only moves past a commit ts once that commit's bottom is done
/// (`NewTxnManager::CommitBottom` walks `bottom_txns_` in order), or a reader
/// would see rows before their block and index writes. A checkpoint spends a
/// commit ts of its own, so it has to take the same ordered path: jumping the
/// clock to it also lets the *next* checkpoint's frame claim a commit whose
/// records never reached an SST.
#[tokio::test]
async fn a_checkpoint_does_not_publish_an_in_flight_commit() {
    let dir = tempdir().unwrap();
    let m = manager(dir.path()).await;
    let mut t = m.begin();
    t.put(row("default_db.t", 0), b"v".to_vec());
    // The KV half is in the store; the bottom (the block and index writes) is
    // still open, as it is for every append until `BottomGuard` drops.
    let pending = (m.commit_deferred(t)).await.unwrap();
    assert!(m.current_ts() < pending);
    (m.append_checkpoint_flushed()).await.unwrap();
    assert!(
        m.current_ts() < pending,
        "the checkpoint published ts {pending} before its bottom ran"
    );
}

/// `WalManager::Flush` swaps the file inside its own loop when it reaches a
/// checkpoint entry (`wal_manager_impl.cpp:288-294`), so the swap and the frame
/// are one step on the writer thread. As two jobs a commit could be written
/// into the fresh log before the frame, which would leave a row frame ahead of
/// the checkpoint the directory is opened by.
#[tokio::test]
async fn a_checkpoint_frame_starts_its_fresh_log() {
    let dir = tempdir().unwrap();
    let store: Arc<dyn MetaStore> = Arc::new(RocksMetaStore::open(dir.path().join("kv")).unwrap());
    let wal = dir.path().join("wal.log");
    let m = Arc::new(TxnManager::open(&wal, store).await.unwrap());

    let commits = {
        let m = Arc::clone(&m);
        tokio::spawn(async move {
            for i in 0..200u64 {
                let mut t = m.begin();
                t.put(format!("k{i}").into_bytes(), b"v".to_vec());
                m.commit(t).await.unwrap();
            }
        })
    };
    let checkpoints = {
        let m = Arc::clone(&m);
        tokio::spawn(async move {
            for _ in 0..100 {
                let _ = m.append_checkpoint_flushed().await;
            }
        })
    };
    commits.await.unwrap();
    checkpoints.await.unwrap();
    m.append_checkpoint_flushed().await.unwrap();

    let is_checkpoint = |entry: &WalEntry| {
        entry
            .cmds
            .iter()
            .any(|cmd| matches!(cmd, WalCmd::Checkpoint { .. }))
    };
    let mut files = vec![wal.clone()];
    for (path, _) in sealed_segments(&wal).unwrap() {
        files.push(path);
    }
    let mut checked = 0;
    for path in files {
        let entries = WalManager::read_all(&path).await.unwrap();
        if entries.iter().any(is_checkpoint) {
            checked += 1;
            assert!(
                is_checkpoint(&entries[0]),
                "{}: a row frame was written before the checkpoint",
                path.display()
            );
        }
    }
    assert!(checked > 1, "the run must have rotated more than once");
}

/// The checkpoint swaps the active log and writes its frame as one step
/// (`wal_manager_impl.cpp:288-294`). As two steps a commit could land in the
/// fresh log before the frame, and the directory's first frame would stop
/// being the checkpoint.
///
/// Both run in one closure under the writer's lock, so the observable form of
/// that rule is: after a checkpoint the **fresh** log holds the frame alone,
/// and the commit it covers stayed in the sealed one.
#[tokio::test]
async fn a_checkpoint_swaps_and_frames_in_one_step() {
    let dir = tempdir().unwrap();
    let m = manager(dir.path()).await;
    let mut t = m.begin();
    t.put(b"a".to_vec(), b"1".to_vec());
    (m.commit(t)).await.unwrap();

    (m.append_checkpoint_flushed()).await.unwrap();

    // The frame is the fresh log's first record: nothing slipped in front of it.
    let fresh = WalManager::read_all(dir.path().join("wal.log"))
        .await
        .unwrap();
    assert!(
        matches!(
            fresh.first().map(|e| &e.cmds[..]),
            Some([WalCmd::Checkpoint { .. }])
        ),
        "the frame must be the fresh log's first record: {fresh:?}"
    );
    // The commit it covers stayed in the sealed file the swap renamed away.
    let sealed = std::fs::read_dir(dir.path())
        .unwrap()
        .flatten()
        .map(|e| e.path())
        .filter(|p| {
            p.file_name()
                .and_then(|n| n.to_str())
                .is_some_and(|n| n.starts_with("wal.log."))
        })
        .max()
        .expect("the swap sealed the old log");
    let old = WalManager::read_all(&sealed).await.unwrap();
    assert!(
        !old.is_empty(),
        "the record the checkpoint covers stayed in the sealed log"
    );
}

/// The reply carries the job's error, so a failing rotate/append reaches the
/// caller instead of being swallowed by the writer thread.
#[tokio::test]
async fn a_writer_job_error_reaches_the_caller() {
    let dir = tempdir().unwrap();
    let m = manager(dir.path()).await;
    let error = m
        .wal
        .run(async |_| -> Result<()> { Err(Error::Wal("rotate failed".into())) })
        .await
        .unwrap_err();
    assert!(
        error.to_string().contains("rotate failed"),
        "unexpected error: {error}"
    );
    // And the writer is still usable afterwards.
    m.flush().unwrap();
}

/// The replay hands the commands out in commit order, counts them, and
/// lets the handler **await** - which is the whole point of the async form (a
/// replayed restore awaits the DDL turn, an index publish awaits its write).
#[tokio::test]
async fn replay_commands_yields_every_command_in_order() {
    let dir = tempdir().unwrap();
    let m = manager(dir.path()).await;
    for i in 0..5u64 {
        let mut t = m.begin();
        t.put(row("default_db.t", i), format!("v{i}").into_bytes());
        m.commit(t).await.unwrap();
    }

    let mut seen: Vec<TxnTimeStamp> = Vec::new();
    let count = m
        .replay_commands(0, async |_cmd: &WalCmd, ts: TxnTimeStamp| {
            // A real handler awaits here (a restore takes the DDL turn); the
            // yield is what makes this the async form.
            tokio::task::yield_now().await;
            seen.push(ts);
            Ok(())
        })
        .await
        .unwrap();

    assert_eq!(count, seen.len(), "every command is counted exactly once");
    assert!(!seen.is_empty(), "the five commits produced commands");
    let mut ordered = seen.clone();
    ordered.sort_unstable();
    assert_eq!(seen, ordered, "commands arrive in commit order");
}

/// A handler's error stops the replay and is returned to the caller, exactly as
/// the synchronous form propagates it.
#[tokio::test]
async fn replay_commands_stops_on_the_first_error() {
    let dir = tempdir().unwrap();
    let m = manager(dir.path()).await;
    for i in 0..3u64 {
        let mut t = m.begin();
        t.put(row("default_db.t", i), format!("v{i}").into_bytes());
        m.commit(t).await.unwrap();
    }

    let mut calls = 0;
    let result = m
        .replay_commands(0, async |_cmd: &WalCmd, _ts: TxnTimeStamp| {
            calls += 1;
            Err(::common::Error::Invalid("stop here".into()))
        })
        .await;

    assert!(result.is_err(), "the handler's error is propagated");
    assert_eq!(calls, 1, "nothing runs after the first failure");
}
