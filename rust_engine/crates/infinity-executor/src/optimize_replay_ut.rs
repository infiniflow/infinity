//! Ports of `src/unit_test/storage/new_catalog/optimize_index_ut.cpp`,
//! `replay_optimize_ut.cpp` and `replay_restore_ut.cpp`
//! (`TestTxnOptimizeIndex`, `TestTxnReplayOptimize`, `TestTxnReplayRestore`).
//!
//! `NewTxn::OptimizeIndex` becomes [`Engine::optimize_indexes`] followed by
//! [`Engine::cleanup`]; `DumpMemIndex` becomes [`Engine::flush`]. The fixtures
//! use a *secondary* index, for which the reference asserts chunk ids
//! (`{0,1}` -> `{2}`); the Rust engine exposes chunk ids only for HNSW
//! (`hnsw_chunk_ids`), so the ports keep the data/row assertions and the
//! index declaration check, and document the chunk-id mapping.

use crate::new_catalog_ut as h;
use crate::Engine;
use tempfile::tempdir;

fn create_table(e: &Engine) {
    h::ok(e, "CREATE DATABASE db1;");
    h::ok(e, "CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);");
}

/// C++ `src/unit_test/storage/new_catalog/optimize_index_ut.cpp:193`
/// (`TestTxnOptimizeIndex.optimize_index_rollback`): a rolled-back optimize
/// keeps the incremental chunks' rows; a committed one keeps the same rows.
#[test]
fn optimize_index_rollback() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);");
    for _ in 0..2 {
        h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192);
        e.flush().unwrap();
    }
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);

    // `RollBackTxn`: no optimize call.
    assert_eq!(
        h::rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abcdefghijklmnopqrstuvwxyz';"),
        vec![vec!["16384"]]
    );
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index1;");

    // committed optimize
    e.optimize_indexes().unwrap();
    e.cleanup().unwrap();
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    assert_eq!(
        h::rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abcdefghijklmnopqrstuvwxyz';"),
        vec![vec!["16384"]]
    );

    h::ok(&e, "DROP DATABASE db1;");
}

/// C++ `src/unit_test/storage/new_catalog/optimize_index_ut.cpp:256`
/// (`TestTxnOptimizeIndex.optimize_index_and_drop_db`): optimize then drop the
/// database.
#[test]
fn optimize_index_then_drop_database() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);");
    for _ in 0..2 {
        h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192);
        e.flush().unwrap();
    }
    e.optimize_indexes().unwrap();
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);

    h::ok(&e, "DROP DATABASE db1;");
    assert!(e.execute("SELECT * FROM db1.tb1;").is_err());
    assert!(e.execute("INSERT INTO db1.tb1 VALUES (1, 'x');").is_err());
}

/// C++ `src/unit_test/storage/new_catalog/optimize_index_ut.cpp:392`
/// (`TestTxnOptimizeIndex.optimize_index_and_drop_table`).
#[test]
fn optimize_index_then_drop_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);");
    for _ in 0..2 {
        h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192);
        e.flush().unwrap();
    }
    e.optimize_indexes().unwrap();

    h::ok(&e, "DROP TABLE db1.tb1;");
    assert!(!e.table_exists("db1.tb1"));
    assert!(e.execute("SELECT * FROM db1.tb1;").is_err());
}

/// C++ `src/unit_test/storage/new_catalog/optimize_index_ut.cpp:540`
/// (`TestTxnOptimizeIndex.optimize_index_and_drop_index`): dropping the index
/// after optimize leaves the rows.
#[test]
fn optimize_index_then_drop_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);");
    for _ in 0..2 {
        h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192);
        e.flush().unwrap();
    }
    e.optimize_indexes().unwrap();

    h::ok(&e, "DROP INDEX index1 ON db1.tb1;");
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    assert_eq!(
        h::rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abcdefghijklmnopqrstuvwxyz';"),
        vec![vec!["16384"]]
    );
}

/// C++ `src/unit_test/storage/new_catalog/optimize_index_ut.cpp:687`
/// (`TestTxnOptimizeIndex.DISABLED_SLOW_optimize_index_and_optimize_index`).
#[test]
#[ignore = "C++ DISABLED_SLOW_optimize_index_and_optimize_index"]
fn disabled_slow_optimize_index_and_optimize_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);");
    for _ in 0..2 {
        h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192);
        e.flush().unwrap();
    }
    e.optimize_indexes().unwrap();
    e.cleanup().unwrap();
    e.optimize_indexes().unwrap();
    e.cleanup().unwrap();
    let e = h::reopen(e, dir.path());
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index1;");
}

/// C++ `src/unit_test/storage/new_catalog/optimize_index_ut.cpp:822`
/// (`TestTxnOptimizeIndex.DISABLED_SLOW_optimize_index_and_dump_index`).
#[test]
#[ignore = "C++ DISABLED_SLOW_optimize_index_and_dump_index"]
fn disabled_slow_optimize_index_and_dump_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);");
    for _ in 0..2 {
        h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192);
        e.flush().unwrap();
    }
    e.optimize_indexes().unwrap();
    e.flush().unwrap();
    let e = h::reopen(e, dir.path());
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index1;");
}

/// C++ `src/unit_test/storage/new_catalog/optimize_index_ut.cpp:982`
/// (`TestTxnOptimizeIndex.optimize_index_and_add_column`): rows read the new
/// column's default after optimize.
#[test]
fn optimize_index_then_add_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);");
    for _ in 0..2 {
        h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192);
        e.flush().unwrap();
    }
    e.optimize_indexes().unwrap();

    h::ok(&e, "ALTER TABLE db1.tb1 ADD COLUMN (col3 INT DEFAULT 5);");
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    assert_eq!(
        h::rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col3 = 5;"),
        vec![vec!["16384"]]
    );
}

/// C++ `src/unit_test/storage/new_catalog/optimize_index_ut.cpp:1207`
/// (`TestTxnOptimizeIndex.optimize_index_and_drop_column`): optimize then drop
/// the indexed column fails (the index still references it), matching the
/// reference's failed commit; dropping `col2` succeeds.
#[test]
fn optimize_index_then_drop_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);");
    for _ in 0..2 {
        h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192);
        e.flush().unwrap();
    }
    e.optimize_indexes().unwrap();

    // Indexed column: blocked by the index.
    assert!(e
        .execute("ALTER TABLE db1.tb1 DROP COLUMN (col1);")
        .is_err());
    h::ok(&e, "ALTER TABLE db1.tb1 DROP COLUMN (col2);");
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    assert_eq!(h::rows(&e, "SHOW TABLE db1.tb1 COLUMNS;").len(), 1);
}

/// C++ `src/unit_test/storage/new_catalog/optimize_index_ut.cpp:1417`
/// (`TestTxnOptimizeIndex.optimize_index_and_rename_table`).
#[test]
fn optimize_index_then_rename_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);");
    for _ in 0..2 {
        h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192);
        e.flush().unwrap();
    }
    e.optimize_indexes().unwrap();

    h::ok(&e, "ALTER TABLE db1.tb1 RENAME TO db1.tb2;");
    assert_eq!(e.row_count("db1.tb2").unwrap(), 16384);
    h::ok(&e, "SHOW TABLE db1.tb2 INDEX index1;");
}

/// C++ `src/unit_test/storage/new_catalog/optimize_index_ut.cpp:1611`
/// (`TestTxnOptimizeIndex.DISABLED_SLOW_optimize_index_and_compact_table`).
#[test]
#[ignore = "C++ DISABLED_SLOW_optimize_index_and_compact_table"]
fn disabled_slow_optimize_index_and_compact_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);");
    for _ in 0..2 {
        h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192);
        e.flush().unwrap();
    }
    e.optimize_indexes().unwrap();
    e.compact_table("db1.tb1").unwrap();
    let e = h::reopen(e, dir.path());
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    assert_eq!(e.segment_ids("db1.tb1"), vec![0]);
}

/// `TestTxnReplayOptimize` setup: a secondary index plus two dumped appends.
fn prepare_replay(e: &Engine) {
    create_table(e);
    h::ok(e, "CREATE INDEX index1 ON db1.tb1 (col1);");
    for _ in 0..2 {
        h::append_repeated(e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192);
        e.flush().unwrap();
    }
}

/// C++ `src/unit_test/storage/new_catalog/replay_optimize_ut.cpp:413`
/// (`TestTxnReplayOptimize.DISABLED_SLOW_test_optimize_commit`).
#[test]
#[ignore = "C++ DISABLED_SLOW_test_optimize_commit"]
fn disabled_slow_test_optimize_commit() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    prepare_replay(&e);
    e.optimize_indexes().unwrap();
    e.cleanup().unwrap();

    let e = h::reopen(e, dir.path());
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    assert_eq!(
        h::rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abcdefghijklmnopqrstuvwxyz';"),
        vec![vec!["16384"]]
    );
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index1;");
}

/// C++ `src/unit_test/storage/new_catalog/replay_optimize_ut.cpp:455`
/// (`TestTxnReplayOptimize.DISABLED_SLOW_test_optimize_rollback`).
#[test]
#[ignore = "C++ DISABLED_SLOW_test_optimize_rollback"]
fn disabled_slow_test_optimize_rollback() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    prepare_replay(&e);
    // Rollback: no optimize.

    let e = h::reopen(e, dir.path());
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    assert_eq!(h::segment_rows(&e, "db1.tb1"), 16384);
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index1;");
}

/// C++ `src/unit_test/storage/new_catalog/replay_optimize_ut.cpp:511`
/// (`TestTxnReplayOptimize.DISABLED_SLOW_test_optimize_interrupt`).
#[test]
#[ignore = "C++ DISABLED_SLOW_test_optimize_interrupt"]
fn disabled_slow_test_optimize_interrupt() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    prepare_replay(&e);
    // Interrupt: reopen before the optimize pass.
    drop(e);
    let e = Engine::open(dir.path()).unwrap();
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index1;");
}

/// C++ `src/unit_test/storage/new_catalog/replay_restore_ut.cpp:269`
/// (`TestTxnReplayRestore.DISABLED_SLOW_test_repaly_restore_table_snapshot`).
#[test]
#[ignore = "C++ DISABLED_SLOW_test_repaly_restore_table_snapshot"]
fn disabled_slow_test_repaly_restore_table_snapshot() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192);
    h::ok(&e, "CREATE SNAPSHOT tb_snapshot ON TABLE db1.tb1;");
    h::ok(&e, "DROP TABLE db1.tb1;");
    let e = h::reopen(e, dir.path());

    h::ok(&e, "RESTORE TABLE SNAPSHOT tb_snapshot;");
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    assert_eq!(h::rows(&e, "SHOW TABLE db1.tb1 COLUMNS;").len(), 2);
}

/// C++ `src/unit_test/storage/new_catalog/replay_restore_ut.cpp:331`
/// (`TestTxnReplayRestore.DISABLED_SLOW_test_repaly_restore_database_snapshot`).
#[test]
#[ignore = "C++ DISABLED_SLOW_test_repaly_restore_database_snapshot"]
fn disabled_slow_test_repaly_restore_database_snapshot() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192);
    h::ok(&e, "CREATE SNAPSHOT db_snapshot ON DATABASE db1;");
    h::ok(&e, "DROP DATABASE db1;");
    let e = h::reopen(e, dir.path());

    h::ok(&e, "RESTORE DATABASE SNAPSHOT db_snapshot;");
    assert!(e.table_exists("db1.tb1"));
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
}

/// C++ `src/unit_test/storage/new_catalog/replay_restore_ut.cpp:399`
/// (`TestTxnReplayRestore.DISABLED_SLOW_test_repaly_restore_system_snapshot`).
#[test]
#[ignore = "C++ DISABLED_SLOW_test_repaly_restore_system_snapshot"]
fn disabled_slow_test_repaly_restore_system_snapshot() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192);
    h::ok(&e, "CREATE SNAPSHOT sys_snapshot ON SYSTEM;");
    h::ok(&e, "DROP DATABASE db1;");
    let e = h::reopen(e, dir.path());

    h::ok(&e, "RESTORE SYSTEM SNAPSHOT sys_snapshot;");
    assert!(e.table_exists("db1.tb1"));
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
}
