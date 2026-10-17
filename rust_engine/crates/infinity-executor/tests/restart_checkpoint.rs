//! Restart / checkpoint ("compaction") invariants (plan §6 Gate C).
//!
//! A checkpoint truncates the WAL once every mutation is durable in the KV
//! base. Recovery must then still reconstruct the full state, and mutations
//! made after recovery must themselves survive a further restart.

use infinity_executor::{Engine, QueryResult};
use infinity_storage::WalManager;

fn count(e: &Engine) -> i64 {
    let rs = e.execute("SELECT COUNT(*) FROM t;").unwrap();
    match &rs[0] {
        QueryResult::Rows { rows, .. } => rows[0][0].parse().unwrap(),
        other => panic!("expected rows, got {other:?}"),
    }
}

fn open(dir: &std::path::Path) -> Engine {
    Engine::open(dir).unwrap()
}

#[test]
fn state_survives_restart_after_wal_truncation() {
    let dir = tempfile::tempdir().unwrap();
    {
        let e = open(dir.path());
        e.execute("CREATE TABLE t (id INTEGER);").unwrap();
        e.execute("INSERT INTO t VALUES (1), (2);").unwrap();
        e.flush().unwrap();
    }
    // checkpoint: drop every WAL record (all durable in the KV base already)
    WalManager::truncate_up_to_ts(dir.path().join("wal.log"), u64::MAX).unwrap();

    // restart 1: the KV base alone reconstructs the state
    {
        let e = open(dir.path());
        assert_eq!(count(&e), 2, "state lost after WAL truncation");
        // a mutation after recovery ...
        e.execute("INSERT INTO t VALUES (3);").unwrap();
        e.flush().unwrap();
    }
    // ... survives another restart
    {
        let e = open(dir.path());
        assert_eq!(count(&e), 3, "post-recovery mutation lost");
    }
}

#[test]
fn compaction_survives_wal_replay() {
    // Compaction renumbers rows. The renumbering must be WAL-visible: replay
    // used to re-create the old positions and resurrect rows deleted before
    // the compact.
    let dir = tempfile::tempdir().unwrap();
    {
        let e = open(dir.path());
        e.execute("CREATE TABLE t (id INTEGER);").unwrap();
        e.execute("INSERT INTO t VALUES (1),(2),(3),(4),(5),(6),(7),(8),(9),(10);")
            .unwrap();
        e.execute("DELETE FROM t WHERE id = 2;").unwrap();
        e.execute("DELETE FROM t WHERE id = 7;").unwrap();
        e.execute("COMPACT TABLE t;").unwrap();
        e.flush().unwrap();
        assert_eq!(count(&e), 8);
    }
    let e = open(dir.path());
    assert_eq!(count(&e), 8, "replay resurrected rows compacted away");
}

#[test]
fn deleting_the_wal_file_after_checkpoint_keeps_state() {
    let dir = tempfile::tempdir().unwrap();
    {
        let e = open(dir.path());
        e.execute("CREATE TABLE t (id INTEGER);").unwrap();
        e.execute("INSERT INTO t VALUES (7);").unwrap();
        e.flush().unwrap();
    }
    std::fs::remove_file(dir.path().join("wal.log")).unwrap();
    let e = open(dir.path());
    assert_eq!(count(&e), 1, "state lost when the compacted WAL is gone");
}
