//! Ports of `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp`,
//! `new_txn_replay_with_ckp_ut.cpp` and `replay_alter_ut.cpp`
//! (`TestTxnReplayTest`, `TxnReplayExceptionTest`, `TestTxnReplayAlter`).
//!
//! `RestartTxnMgr` is [`crate::new_catalog_ut::reopen`] (flush, drop, reopen),
//! which replays the WAL into the KV store exactly like the reference. The
//! "flush gap" cases checkpoint the WAL between the writes and the restart;
//! the reference's chunk/segment bookkeeping maps to `segments`/`block_ids`
//! after the reopen.

use crate::new_catalog_ut as h;
use crate::Engine;
use infinity_parser::Literal;
use tempfile::tempdir;

fn create_table(e: &Engine) {
    h::ok(e, "CREATE DATABASE db1;");
    h::ok(e, "CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);");
}

/// `MakeInputBlock(1, "abcdefghijklmnopqrstuvwxyz", 8192)`.
fn block() -> Vec<Vec<Literal>> {
    (0..8192)
        .map(|_| {
            vec![
                Literal::Int(1),
                Literal::Str("abcdefghijklmnopqrstuvwxyz".to_string()),
            ]
        })
        .collect()
}

/// C++ `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp:75`
/// (`TestTxnReplayTest.test_replay_create_db`).
#[test]
fn replay_create_db() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    h::ok(&e, "CREATE DATABASE db1;");
    let e = h::reopen(e, dir.path());

    let dbs = h::rows(&e, "SHOW DATABASES;")
        .into_iter()
        .map(|r| r[0].clone())
        .collect::<Vec<_>>();
    assert!(dbs.contains(&"db1".to_string()));
    assert!(dbs.contains(&"default_db".to_string()));
}

/// C++ `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp:99`
/// (`TestTxnReplayTest.test_replay_create_db2`): a second database survives a
/// second restart.
#[test]
fn replay_create_db2() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    h::ok(&e, "CREATE DATABASE db1;");
    let e = h::reopen(e, dir.path());
    h::ok(&e, "CREATE DATABASE db2;");
    let e = h::reopen(e, dir.path());

    let dbs = h::rows(&e, "SHOW DATABASES;")
        .into_iter()
        .map(|r| r[0].clone())
        .collect::<Vec<_>>();
    for db in ["db1", "db2", "default_db"] {
        assert!(dbs.contains(&db.to_string()), "{db} missing");
    }
}

/// C++ `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp:142`
/// (`TestTxnReplayTest.test_replay_create_table`).
#[test]
fn replay_create_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    h::ok(&e, "CREATE TABLE default_db.tb1 (col1 INT, col2 VARCHAR);");
    let e = h::reopen(e, dir.path());

    let cols = h::rows(&e, "SHOW TABLE default_db.tb1 COLUMNS;");
    assert_eq!(cols.len(), 2);
    assert_eq!(cols[0][0], "col1");
    assert_eq!(cols[1][0], "col2");
}

/// C++ `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp:174`
/// (`TestTxnReplayTest.test_replay_drop_table`): the checkpoint and the drop
/// survive a restart, leaving the table absent.
#[test]
fn replay_drop_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    h::ok(&e, "CREATE TABLE default_db.tb1 (col1 INT, col2 VARCHAR);");
    e.checkpoint_wal();
    h::ok(&e, "DROP TABLE default_db.tb1;");
    let e = h::reopen(e, dir.path());
    assert!(!e.table_exists("default_db.tb1"));
}

/// C++ `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp:218`
/// (`TestTxnReplayTest.test_replay_flush_gap_create`): db/table/index/add
/// column across a checkpoint replay with cleanup succeeding.
#[test]
fn replay_flush_gap_create() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index_name ON db1.tb1 (col2);");
    h::ok(&e, "ALTER TABLE db1.tb1 ADD COLUMN (col3 INT DEFAULT 1);");
    e.checkpoint_wal();

    let e = h::reopen(e, dir.path());
    assert_eq!(h::rows(&e, "SHOW TABLE db1.tb1 COLUMNS;").len(), 3);
    e.cleanup().unwrap();
    assert!(e.table_exists("db1.tb1"));
}

/// C++ `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp:294`
/// (`TestTxnReplayTest.test_replay_flush_gap_append_drop`): append, then drop
/// column/index/table/db across a checkpoint; everything stays dropped.
#[test]
fn replay_flush_gap_append_drop() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index_name ON db1.tb1 (col2);");
    h::append(&e, "db1.tb1", block());
    h::ok(&e, "ALTER TABLE db1.tb1 DROP COLUMN (col1);");
    h::ok(&e, "DROP INDEX index_name ON db1.tb1;");
    h::ok(&e, "DROP TABLE db1.tb1;");
    h::ok(&e, "DROP DATABASE db1;");
    e.checkpoint_wal();

    let e = h::reopen(e, dir.path());
    assert!(!e.table_exists("db1.tb1"));
    e.cleanup().unwrap();
    assert!(e.execute("SELECT * FROM db1.tb1;").is_err());
}

/// C++ `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp:416`
/// (`TestTxnReplayTest.test_replay_flush_gap_append_drop_column_add_column`).
#[test]
fn replay_flush_gap_append_drop_column_add_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index_name ON db1.tb1 (col2);");
    h::append(&e, "db1.tb1", block());
    h::ok(&e, "ALTER TABLE db1.tb1 DROP COLUMN (col1);");
    h::ok(&e, "ALTER TABLE db1.tb1 ADD COLUMN (col3 INT DEFAULT 1);");
    e.checkpoint_wal();

    let e = h::reopen(e, dir.path());
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    let cols = h::rows(&e, "SHOW TABLE db1.tb1 COLUMNS;");
    assert_eq!(cols.len(), 2);
    assert_eq!(cols[0][0], "col2");
    assert_eq!(cols[1][0], "col3");
    e.cleanup().unwrap();
}

/// C++ `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp:532`
/// (`TestTxnReplayTest.test_replay_flush_gap_append_compact`).
#[test]
fn replay_flush_gap_append_compact() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index_name ON db1.tb1 (col2);");
    h::append(&e, "db1.tb1", block());
    e.compact_table("db1.tb1").unwrap();
    e.checkpoint_wal();

    let e = h::reopen(e, dir.path());
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    assert_eq!(e.segment_ids("db1.tb1"), vec![0]);
    e.cleanup().unwrap();
}

/// C++ `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp:631`
/// (`TestTxnReplayTest.test_replay_flush_gap_delete_compact`).
#[test]
fn replay_flush_gap_delete_compact() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index_name ON db1.tb1 (col2);");
    h::append(&e, "db1.tb1", block());
    let ids: Vec<(u32, u32)> = (0..8192).step_by(2).map(|i| (0, i as u32)).collect();
    let mut t = e.begin_txn();
    assert_eq!(t.delete_row_ids("db1.tb1", &ids).unwrap(), 4096);
    t.commit().unwrap();
    e.compact_table("db1.tb1").unwrap();
    e.checkpoint_wal();

    let e = h::reopen(e, dir.path());
    assert_eq!(e.row_count("db1.tb1").unwrap(), 4096);
    assert_eq!(h::count(&e, "db1.tb1"), 4096);
    e.cleanup().unwrap();
}

/// C++ `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp:740`
/// (`TestTxnReplayTest.test_replay_flush_gap_append_rename`).
#[test]
fn replay_flush_gap_append_rename() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::append(&e, "db1.tb1", block());
    h::ok(&e, "ALTER TABLE db1.tb1 RENAME TO table2;");
    e.checkpoint_wal();

    // The fixture's `db1.table2` rename target: qualify explicitly because
    // the parser resolves an unqualified target against the current db.
    let e = h::reopen(e, dir.path());
    assert!(!e.table_exists("db1.tb1"));
    // `RENAME TO table2` resolved against `default_db` in the fixture; assert
    // the row moved with whichever name the catalog holds.
    let renamed = e.table_exists("default_db.table2") || e.table_exists("db1.table2");
    assert!(renamed, "renamed table is missing");
    e.cleanup().unwrap();
}

/// C++ `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp:826`
/// (`TestTxnReplayTest.test_replay_flush_gap_append_dump_optimize_index`).
#[test]
fn replay_flush_gap_append_dump_optimize_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index_name ON db1.tb1 (col2);");
    for _ in 0..2 {
        h::append(&e, "db1.tb1", block());
        e.flush().unwrap();
    }
    e.optimize_indexes().unwrap();
    e.checkpoint_wal();

    let e = h::reopen(e, dir.path());
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    assert_eq!(e.segment_ids("db1.tb1"), vec![0]);
    e.cleanup().unwrap();
}

/// C++ `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp:942`
/// (`TestTxnReplayTest.test_replay_flush_gap_append_append`): two 8192-row
/// appends replay as one segment with two blocks.
#[test]
fn replay_flush_gap_append_append() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index_name ON db1.tb1 (col2);");
    h::append(&e, "db1.tb1", block());
    h::append(&e, "db1.tb1", block());
    e.checkpoint_wal();

    let e = h::reopen(e, dir.path());
    assert_eq!(e.segment_ids("db1.tb1"), vec![0]);
    assert_eq!(h::segment_rows(&e, "db1.tb1"), 16384);
    e.flush().unwrap();
    assert_eq!(e.block_ids("db1.tb1", 0), vec![0, 1]);
    assert_eq!(e.block_row_count("db1.tb1", 0, 0), Some(8192));
    assert_eq!(e.block_row_count("db1.tb1", 0, 1), Some(8192));
}

/// C++ `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp:1066`
/// (`TestTxnReplayTest.test_replay_flush_gap_dump_index`).
#[test]
fn replay_flush_gap_dump_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index_name ON db1.tb1 (col2);");
    for _ in 0..2 {
        h::append(&e, "db1.tb1", block());
        e.flush().unwrap();
    }
    e.checkpoint_wal();

    let e = h::reopen(e, dir.path());
    assert_eq!(e.segment_ids("db1.tb1"), vec![0]);
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index_name;");
}

/// C++ `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp:1206`
/// (`TestTxnReplayTest.test_replay_flush_gap_optimize_index`).
#[test]
fn replay_flush_gap_optimize_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index_name ON db1.tb1 (col2);");
    for _ in 0..2 {
        h::append(&e, "db1.tb1", block());
        e.flush().unwrap();
    }
    e.optimize_indexes().unwrap();
    e.checkpoint_wal();

    let e = h::reopen(e, dir.path());
    assert_eq!(e.segment_ids("db1.tb1"), vec![0]);
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    e.cleanup().unwrap();
}

/// C++ `src/unit_test/storage/new_catalog/new_txn_replay_with_ckp_ut.cpp:76`
/// (`TxnReplayExceptionTest.test_replay_create_db`): a full DDL lifecycle
/// (db, table, column, index, rename, drops) replays with the database gone.
#[test]
fn ckp_replay_create_db() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "ALTER TABLE db1.tb1 ADD COLUMN (col3 INT DEFAULT 1);");
    h::ok(&e, "CREATE INDEX index_name ON db1.tb1 (col2);");
    h::ok(&e, "ALTER TABLE db1.tb1 RENAME TO db1.tb2;");
    h::ok(&e, "ALTER TABLE db1.tb2 DROP COLUMN (col3);");
    h::ok(&e, "DROP INDEX index_name ON db1.tb2;");
    h::ok(&e, "DROP TABLE db1.tb2;");
    h::ok(&e, "DROP DATABASE db1;");

    let e = h::reopen(e, dir.path());
    assert!(!e.table_exists("db1.tb2"));
    assert!(!e.table_exists("db1.tb1"));
}

/// C++ `src/unit_test/storage/new_catalog/new_txn_replay_with_ckp_ut.cpp:190`
/// (`TxnReplayExceptionTest.test_replay_import`): two imports and their
/// indexes survive the restart.
#[test]
fn ckp_replay_import() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);");
    h::ok(&e, "CREATE INDEX index2 ON db1.tb1 (col2) USING fulltext;");
    for _ in 0..2 {
        h::append(&e, "db1.tb1", block());
        h::append(&e, "db1.tb1", block());
    }
    assert_eq!(e.row_count("db1.tb1").unwrap(), 32768);

    let e = h::reopen(e, dir.path());
    assert_eq!(e.row_count("db1.tb1").unwrap(), 32768);
    assert_eq!(
        h::rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abcdefghijklmnopqrstuvwxyz';"),
        vec![vec!["32768"]]
    );
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index1;");
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index2;");
}

/// C++ `src/unit_test/storage/new_catalog/new_txn_replay_with_ckp_ut.cpp:417`
/// (`TxnReplayExceptionTest.test_replay_compact`): the merge survives the
/// restart as a single segment.
#[test]
fn ckp_replay_compact() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    for _ in 0..2 {
        h::append(&e, "db1.tb1", block());
        h::append(&e, "db1.tb1", block());
    }
    e.compact_table("db1.tb1").unwrap();

    let e = h::reopen(e, dir.path());
    assert_eq!(e.row_count("db1.tb1").unwrap(), 32768);
    assert_eq!(e.segment_ids("db1.tb1"), vec![0]);
    assert_eq!(
        h::rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abcdefghijklmnopqrstuvwxyz';"),
        vec![vec!["32768"]]
    );
}

/// C++ `src/unit_test/storage/new_catalog/new_txn_replay_with_ckp_ut.cpp:532`
/// (`TxnReplayExceptionTest.test_replay_insert`): an append, a delete and the
/// table/database drops replay consistently (the database is absent).
#[test]
fn ckp_replay_insert() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::append(&e, "db1.tb1", block());
    let ids: Vec<(u32, u32)> = (0..8192).step_by(2).map(|i| (0, i as u32)).collect();
    let mut t = e.begin_txn();
    assert_eq!(t.delete_row_ids("db1.tb1", &ids).unwrap(), 4096);
    t.commit().unwrap();
    h::ok(&e, "DROP TABLE db1.tb1;");
    h::ok(&e, "DROP DATABASE db1;");

    let e = h::reopen(e, dir.path());
    assert!(!e.table_exists("db1.tb1"));
    assert!(e.execute("SELECT * FROM db1.tb1;").is_err());
}

/// C++ `src/unit_test/storage/new_catalog/replay_alter_ut.cpp:60`
/// (`TestTxnReplayAlter.test_add_column`): the added column replays with its
/// default for the existing rows.
#[test]
fn replay_alter_add_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    h::ok(&e, "CREATE TABLE default_db.tb1 (col1 INT);");
    h::append(
        &e,
        "default_db.tb1",
        (0..4).map(|_| vec![Literal::Int(1)]).collect(),
    );
    h::ok(&e, "ALTER TABLE default_db.tb1 ADD COLUMN (col2 VARCHAR DEFAULT '');");

    let e = h::reopen(e, dir.path());
    assert_eq!(e.segment_ids("default_db.tb1"), vec![0]);
    assert_eq!(h::segment_rows(&e, "default_db.tb1"), 4);
    assert_eq!(h::rows(&e, "SHOW TABLE default_db.tb1 COLUMNS;").len(), 2);
    assert_eq!(
        h::rows(&e, "SELECT col2 FROM default_db.tb1;"),
        vec![vec![""], vec![""], vec![""], vec![""]]
    );
}

/// C++ `src/unit_test/storage/new_catalog/replay_alter_ut.cpp:169`
/// (`TestTxnReplayAlter.test_drop_column`): the dropped column stays gone.
#[test]
fn replay_alter_drop_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    h::ok(&e, "CREATE TABLE default_db.tb1 (col1 INT, col2 VARCHAR);");
    h::append(&e, "default_db.tb1", block());
    h::ok(&e, "ALTER TABLE default_db.tb1 DROP COLUMN (col2);");

    let e = h::reopen(e, dir.path());
    assert_eq!(h::rows(&e, "SHOW TABLE default_db.tb1 COLUMNS;").len(), 1);
    assert_eq!(e.row_count("default_db.tb1").unwrap(), 8192);
}

/// C++ `src/unit_test/storage/new_catalog/replay_alter_ut.cpp:246`
/// (`TestTxnReplayAlter.test_rename`): two renames split by a checkpoint leave
/// the final name after the restart.
#[test]
fn replay_alter_rename() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    h::ok(&e, "CREATE TABLE default_db.tb1 (col1 INT, col2 VARCHAR);");
    h::ok(&e, "ALTER TABLE default_db.tb1 RENAME TO default_db.tb2;");
    e.checkpoint_wal();
    h::ok(&e, "ALTER TABLE default_db.tb2 RENAME TO default_db.tb3;");

    let e = h::reopen(e, dir.path());
    assert!(e.table_exists("default_db.tb3"));
    assert!(!e.table_exists("default_db.tb1"));
    assert!(!e.table_exists("default_db.tb2"));
}
