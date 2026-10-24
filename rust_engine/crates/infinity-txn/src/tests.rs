//! Tests for the commit pipeline, the typed WAL and the transaction manager.

use super::*;
use infinity_storage::FjallMetaStore;
use tempfile::tempdir;

fn manager(dir: &std::path::Path) -> TxnManager {
    let store = Arc::new(FjallMetaStore::open(dir.join("kv"), "default").unwrap());
    TxnManager::open(dir.join("wal.log"), store).unwrap()
}

fn row(table: &str, seq: u64) -> Vec<u8> {
    row_key(table, seq)
}

#[test]
fn commit_applies_to_kv() {
    let dir = tempdir().unwrap();
    {
        let store: Arc<dyn MetaStore> =
            Arc::new(FjallMetaStore::open(dir.path().join("kv"), "default").unwrap());
        let m = TxnManager::open(dir.path().join("wal.log"), store.clone()).unwrap();
        let mut t = m.begin();
        t.put(b"a".to_vec(), b"1".to_vec());
        t.put(b"b".to_vec(), b"2".to_vec());
        let ts = m.commit(t).unwrap();
        assert!(ts >= 1);
        // commit is visible in the KV store immediately
        assert_eq!(store.get(b"a").unwrap(), Some(b"1".to_vec()));
        assert_eq!(store.get(b"b").unwrap(), Some(b"2".to_vec()));
    }
    // reopen after the handle is released
    let m2 = manager(dir.path());
    assert_eq!(m2.recover(0).unwrap(), 1); // idempotent re-apply
}

#[test]
fn recovery_replays_wal_after_reopen() {
    let dir = tempdir().unwrap();
    {
        let m = manager(dir.path());
        for i in 0..3u64 {
            let mut t = m.begin();
            t.put(format!("k{i}").into_bytes(), format!("v{i}").into_bytes());
            m.commit(t).unwrap();
        }
    }
    // fresh manager + fresh store: KV empty, WAL has 3 -> recover
    let store = Arc::new(FjallMetaStore::open(dir.path().join("kv2"), "default").unwrap());
    let m = TxnManager::open(dir.path().join("wal.log"), store.clone()).unwrap();
    assert_eq!(m.recover(0).unwrap(), 3);
    assert_eq!(store.get(b"k0").unwrap(), Some(b"v0".to_vec()));
    assert_eq!(store.get(b"k2").unwrap(), Some(b"v2".to_vec()));
    // idempotent
    assert_eq!(m.recover(0).unwrap(), 3);
}

#[test]
fn concurrent_commits_are_serialized_and_durable() {
    use std::collections::BTreeSet;
    let dir = tempdir().unwrap();
    const THREADS: u64 = 4;
    const PER: u64 = 25;
    {
        let store: Arc<dyn MetaStore> =
            Arc::new(FjallMetaStore::open(dir.path().join("kv"), "default").unwrap());
        let mgr = Arc::new(TxnManager::open(dir.path().join("wal.log"), store.clone()).unwrap());
        let handles: Vec<_> = (0..THREADS)
            .map(|t| {
                let mgr = mgr.clone();
                std::thread::spawn(move || {
                    for j in 0..PER {
                        let mut tx = mgr.begin();
                        tx.put(
                            format!("k{t}_{j}").into_bytes(),
                            format!("v{t}_{j}").into_bytes(),
                        );
                        mgr.commit(tx).unwrap();
                    }
                })
            })
            .collect();
        for h in handles {
            h.join().unwrap();
        }
        mgr.flush().unwrap();
    }
    // all rows are durable in KV
    let store: Arc<dyn MetaStore> =
        Arc::new(FjallMetaStore::open(dir.path().join("kv"), "default").unwrap());
    assert_eq!(
        store.scan_prefix(b"k").unwrap().len(),
        (THREADS * PER) as usize
    );

    // WAL has one record per commit, with unique commit timestamps
    let recs = WalManager::read_all(dir.path().join("wal.log")).unwrap();
    assert_eq!(recs.len(), (THREADS * PER) as usize);
    let ts: BTreeSet<u64> = recs.iter().map(|r| r.commit_ts).collect();
    assert_eq!(
        ts.len(),
        (THREADS * PER) as usize,
        "commit timestamps must be unique"
    );
}

#[test]
fn truncated_wal_recovers_exactly_the_valid_prefix() {
    let dir = tempdir().unwrap();
    let wal = dir.path().join("wal.log");
    {
        let store: Arc<dyn MetaStore> =
            Arc::new(FjallMetaStore::open(dir.path().join("kv"), "default").unwrap());
        let m = TxnManager::open(&wal, store).unwrap();
        for i in 0..10u64 {
            let mut t = m.begin();
            t.put(row("default_db.t", i), format!("v{i}").into_bytes());
            m.commit(t).unwrap();
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
            Arc::new(FjallMetaStore::open(d2.path().join("kv"), "default").unwrap());
        let m = TxnManager::open(d2.path().join("wal.log"), store.clone()).unwrap();
        m.recover(0).unwrap(); // must not panic
        let expected = WalManager::read_all(&wal2).unwrap().len();
        let got = store.scan_prefix(b"row:").unwrap().len();
        assert_eq!(
            got, expected,
            "cut={cut}: recovered {got} rows, expected {expected}"
        );
    }
}

#[test]
fn empty_txn_rejected() {
    let dir = tempdir().unwrap();
    let m = manager(dir.path());
    assert!(m.commit(m.begin()).is_err());
}

#[test]
fn concurrent_deletes_conflict() {
    let dir = tempdir().unwrap();
    let m = manager(dir.path());
    let mut t1 = m.begin();
    t1.delete(b"row:1".to_vec());
    let mut t2 = m.begin();
    t2.delete(b"row:1".to_vec());
    m.commit(t1).unwrap();
    assert!(matches!(m.commit(t2), Err(Error::TxnConflict(_))));

    // A transaction that starts after the winner is not stale.
    let mut t3 = m.begin();
    t3.delete(b"row:1".to_vec());
    m.commit(t3).unwrap();

    // Disjoint keys never conflict.
    let mut t4 = m.begin();
    t4.delete(b"row:2".to_vec());
    let mut t5 = m.begin();
    t5.delete(b"row:3".to_vec());
    m.commit(t5).unwrap();
    m.commit(t4).unwrap();
}

/// The commit path writes a typed `WalCmd::Append` (not a KV batch), and
/// recovery reproduces exactly the rows the KV held before the restart.
#[test]
fn append_is_a_typed_command_and_replays_the_same_rows() {
    let dir = tempdir().unwrap();
    let table = "default_db.t";
    let expected: Vec<(Vec<u8>, Vec<u8>)> = (0..5u64)
        .map(|i| (row(table, i), format!("{i}\trow-{i}").into_bytes()))
        .collect();
    {
        let store: Arc<dyn MetaStore> =
            Arc::new(FjallMetaStore::open(dir.path().join("kv"), "default").unwrap());
        let m = TxnManager::open(dir.path().join("wal.log"), store.clone()).unwrap();
        let mut t = m.begin();
        for (key, value) in &expected {
            t.put(key.clone(), value.clone());
        }
        m.commit(t).unwrap();
        assert_eq!(store.scan_prefix(b"row:").unwrap(), expected);
    }

    // The record is a typed Append with one coalesced row range.
    let mut iter = WalEntryIterator::make(dir.path().join("wal.log"), false).unwrap();
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

    // Recovery into an empty KV reproduces the rows byte for byte.
    let store = Arc::new(FjallMetaStore::open(dir.path().join("kv2"), "default").unwrap());
    let m = TxnManager::open(dir.path().join("wal.log"), store.clone()).unwrap();
    m.recover(0).unwrap();
    assert_eq!(store.scan_prefix(b"row:").unwrap(), expected);
}

/// A delete commits as a typed `WalCmd::Delete` and replays the tombstones.
#[test]
fn delete_is_a_typed_command_and_replays_the_tombstones() {
    let dir = tempdir().unwrap();
    let table = "default_db.t";
    {
        let store: Arc<dyn MetaStore> =
            Arc::new(FjallMetaStore::open(dir.path().join("kv"), "default").unwrap());
        let m = TxnManager::open(dir.path().join("wal.log"), store.clone()).unwrap();
        let mut t = m.begin();
        t.delete(del_key(table, 3));
        t.delete(del_key(table, 4));
        m.commit(t).unwrap();
        assert_eq!(store.scan_prefix(b"del:").unwrap().len(), 2);
    }
    let mut iter = WalEntryIterator::make(dir.path().join("wal.log"), false).unwrap();
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

    let store = Arc::new(FjallMetaStore::open(dir.path().join("kv2"), "default").unwrap());
    let m = TxnManager::open(dir.path().join("wal.log"), store.clone()).unwrap();
    m.recover(0).unwrap();
    assert_eq!(store.get(&del_key(table, 3)).unwrap(), Some(b"1".to_vec()));
    assert_eq!(store.get(&del_key(table, 4)).unwrap(), Some(b"1".to_vec()));
}

/// Catalog records keep their exact value in a `KvBatch` and replay verbatim.
#[test]
fn typed_commands_replace_the_catalog_kv_batch() {
    // A DDL that prepared typed WAL commands emits them instead of a `KvBatch`
    // for the same catalog records (`build_commands`).
    let dir = tempdir().unwrap();
    {
        let store: Arc<dyn MetaStore> =
            Arc::new(FjallMetaStore::open(dir.path().join("kv"), "default").unwrap());
        let m = TxnManager::open(dir.path().join("wal.log"), store).unwrap();
        let mut t = m.begin();
        t.put(b"tbl:0000000000000001".to_vec(), b"catalog-record".to_vec());
        t.set_typed_commands(vec![WalCmd::Checkpoint { max_commit_ts: 7 }]);
        m.commit(t).unwrap();
    }
    let mut iter = WalEntryIterator::make(dir.path().join("wal.log"), false).unwrap();
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

#[test]
fn catalog_writes_are_carried_as_a_kv_batch() {
    let dir = tempdir().unwrap();
    let key = b"tbl:0000000000000001".to_vec();
    let value = b"catalog-record".to_vec();
    {
        let store: Arc<dyn MetaStore> =
            Arc::new(FjallMetaStore::open(dir.path().join("kv"), "default").unwrap());
        let m = TxnManager::open(dir.path().join("wal.log"), store.clone()).unwrap();
        let mut t = m.begin();
        t.put(key.clone(), value.clone());
        m.commit(t).unwrap();
    }
    let mut iter = WalEntryIterator::make(dir.path().join("wal.log"), false).unwrap();
    let entry = iter.next().expect("one record");
    match &entry.cmds[0] {
        WalCmd::KvBatch { kind, writes } => {
            assert_eq!(*kind, KvBatchKind::Catalog);
            assert_eq!(writes, &[(key.clone(), value.clone())]);
        }
        other => panic!("expected a KvBatch command, got {other:?}"),
    }
    let store = Arc::new(FjallMetaStore::open(dir.path().join("kv2"), "default").unwrap());
    let m = TxnManager::open(dir.path().join("wal.log"), store.clone()).unwrap();
    m.recover(0).unwrap();
    assert_eq!(store.get(&key).unwrap(), Some(value));
}

#[test]
fn rollback_frees_the_slot_and_the_next_txn_does_not_conflict() {
    let dir = tempdir().unwrap();
    let m = manager(dir.path());
    let mut t1 = m.begin();
    t1.delete(b"row:1".to_vec());
    assert_eq!(m.active_txn_count(), 1);
    m.rollback(t1).unwrap();
    assert_eq!(m.active_txn_count(), 0);
    assert_eq!(m.total_rollbacked_txn_count(), 1);

    // The rolled-back transaction never committed, so the same delete succeeds.
    let mut t2 = m.begin();
    t2.delete(b"row:1".to_vec());
    m.commit(t2).unwrap();
    assert_eq!(m.total_committed_txn_count(), 1);

    // Nothing reached the WAL: only the committed record is there.
    let recs = WalManager::read_all(dir.path().join("wal.log")).unwrap();
    assert_eq!(recs.len(), 1);
}

#[test]
fn overlapping_write_ranges_conflict() {
    let dir = tempdir().unwrap();
    let m = manager(dir.path());
    let mut t1 = m.begin();
    let mut t2 = m.begin();
    t1.put(b"k".to_vec(), b"a".to_vec());
    t2.put(b"k".to_vec(), b"b".to_vec());
    m.commit(t1).unwrap();
    assert!(matches!(m.commit(t2), Err(Error::TxnConflict(_))));
}

#[test]
fn read_sets_and_ranges_conflict() {
    let dir = tempdir().unwrap();
    let m = manager(dir.path());
    let mut writer = m.begin();
    writer.put(row("default_db.t", 0), b"a".to_vec());

    // A read-only transaction is never validated: the reference hands it
    // `current_ts + 1` and returns before `CheckConflict1`
    // (`NewTxn::Commit`, `new_txn_impl.cpp:2035-2057`).
    let mut reader = m.begin();
    reader.read(row("default_db.t", 0));
    m.commit(writer).unwrap();
    assert_eq!(m.commit(reader).unwrap(), m.current_ts() + 1);

    // A *write* transaction that read the key does conflict.
    let mut read_writer = m.begin();
    read_writer.read(row("default_db.t", 0));
    read_writer.put(b"unrelated".to_vec(), b"x".to_vec());
    let mut writer2 = m.begin();
    writer2.put(row("default_db.t", 0), b"b".to_vec());
    m.commit(writer2).unwrap();
    assert!(matches!(m.commit(read_writer), Err(Error::TxnConflict(_))));

    // A range read sees any newer write under the prefix.
    let mut ranged = m.begin();
    ranged.read_prefix(b"row:default_db.t:");
    ranged.put(b"unrelated2".to_vec(), b"y".to_vec());
    let mut writer3 = m.begin();
    writer3.put(row("default_db.t", 1), b"b".to_vec());
    m.commit(writer3).unwrap();
    assert!(matches!(m.commit(ranged), Err(Error::TxnConflict(_))));

    // A read from before the writes, committed without overlap, is fine.
    let mut clean = m.begin();
    clean.read(b"untouched".to_vec());
    clean.put(b"other".to_vec(), b"c".to_vec());
    m.commit(clean).unwrap();
}

#[test]
fn active_txn_count_tracks_begin_commit_and_drop() {
    let dir = tempdir().unwrap();
    let m = manager(dir.path());
    assert_eq!(m.active_txn_count(), 0);

    let mut t = m.begin_with_text("INSERT INTO t VALUES (1)");
    assert_eq!(m.active_txn_count(), 1);
    assert_eq!(m.oldest_alive_ts(), t.begin_ts);
    let info = m.get_txn_info_by_id(t.id).expect("registered");
    assert_eq!(info.txn_text, "INSERT INTO t VALUES (1)");
    assert_eq!(m.get_txn_info_array().len(), 1);

    t.put(b"k".to_vec(), b"v".to_vec());
    m.commit(t).unwrap();
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
#[test]
fn restart_rebuilds_the_conflict_state_from_the_wal() {
    let dir = tempdir().unwrap();
    {
        let m = manager(dir.path());
        let mut t = m.begin();
        t.put(row("default_db.t", 0), b"a".to_vec());
        t.delete(del_key("default_db.t", 7));
        m.commit(t).unwrap();
    }
    let m = manager(dir.path());
    let seen = m.write_ts.lock().unwrap();
    // `TxnManager::open` spends the recovery transaction's timestamp first
    // (`prepare_commit_ts_ = start_ts + 2`, the reference's
    // `BeginRecoveryTxn`), so the first commit here is 4, not 2.
    assert_eq!(seen.get(&row("default_db.t", 0)), Some(&4));
    assert_eq!(seen.get(&del_key("default_db.t", 7)), Some(&4));
}

#[test]
fn checkpoint_ts_skips_already_applied_records() {
    let dir = tempdir().unwrap();
    {
        let m = manager(dir.path());
        for i in 0..3u64 {
            let mut t = m.begin();
            t.put(row("default_db.t", i), format!("v{i}").into_bytes());
            m.commit(t).unwrap();
        }
    }
    let store = Arc::new(FjallMetaStore::open(dir.path().join("kv2"), "default").unwrap());
    let m = TxnManager::open(dir.path().join("wal.log"), store.clone()).unwrap();
    // The three commits took timestamps 4, 6 and 8 (`open` spends the
    // recovery transaction's ts first): replaying past the first one leaves
    // the other two.
    assert_eq!(m.recover(4).unwrap(), 2);
    assert_eq!(store.scan_prefix(b"row:").unwrap().len(), 2);
}
