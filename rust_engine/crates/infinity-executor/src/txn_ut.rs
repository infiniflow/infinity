//! Engine-level ports of the reference's snapshot transaction tests
//! (`src/unit_test/storage/txn/{database,system,table}_snapshot_ut.cpp`).
//!
//! The reference fixtures create snapshots through `NewTxn`
//! (`TransactionType::kCreate*Snapshot`) and restore them inside `kRestore*`
//! transactions, committing with `CommitTxn` and discarding with
//! `NewTxn::Rollback`. The Rust engine exposes the same operations through
//! [`Engine::execute`] and the snapshot methods; a restore that must be rolled
//! back is staged with `begin_restore_*` and discarded through
//! [`crate::PendingRestore::rollback`], mirroring `NewTxn::Rollback`.
//!
//! Two reference cases are not ported because they depend on an uncommitted
//! append/delete being invisible to a snapshot created by another live
//! transaction (`test_create_snapshot_insert_data`,
//! `test_create_snapshot_delete_data`). The Rust engine has no multi-statement
//! transactions: every statement commits before the next one starts, so that
//! isolation cannot be constructed here.

use crate::{Engine, QueryResult};
use infinity_parser::Literal;
use std::sync::atomic::{AtomicBool, AtomicUsize, Ordering};
use std::sync::Barrier;
use tempfile::tempdir;

fn rows(e: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in e.execute(sql).unwrap() {
        if let QueryResult::Rows { rows, .. } = result {
            return rows;
        }
    }
    Vec::new()
}

fn count(e: &Engine, table: &str) -> usize {
    rows(e, &format!("SELECT count(*) FROM {table};"))[0][0]
        .parse()
        .unwrap()
}

fn databases(e: &Engine) -> Vec<String> {
    rows(e, "SHOW DATABASES;")
        .into_iter()
        .map(|r| r[0].clone())
        .collect()
}

/// Column name and rendered type of every column in `table`.
fn columns(e: &Engine, table: &str) -> Vec<(String, String)> {
    rows(e, &format!("SHOW TABLE {table} COLUMNS;"))
        .into_iter()
        .map(|r| (r[0].clone(), r[1].clone()))
        .collect()
}

/// The fixture tables all declare `(col1 INT, col2 VARCHAR)`.
fn assert_table_schema(e: &Engine, table: &str) {
    assert_eq!(
        columns(e, table),
        vec![
            ("col1".to_string(), "Integer".to_string()),
            ("col2".to_string(), "Varchar".to_string()),
        ],
        "schema of {table}"
    );
}

fn assert_restored_table(e: &Engine, table: &str, expected_rows: usize) {
    assert!(e.table_exists(table), "{table} was not restored");
    assert_eq!(count(e, table), expected_rows, "row count of {table}");
    assert_table_schema(e, table);
}

/// `MakeInputBlock(v1, v2, row_cnt)`: `blocks` appends of `per_block` rows
/// carrying the same values.
fn append_repeated(e: &Engine, table: &str, value: i64, blocks: usize, per_block: usize) {
    for _ in 0..blocks {
        let batch: Vec<Vec<Literal>> = (0..per_block)
            .map(|_| {
                vec![
                    Literal::Int(value),
                    Literal::Str("abcdefghijklmnopqrstuvwxyz".to_string()),
                ]
            })
            .collect();
        e.insert_literals(table, None, batch).unwrap();
    }
}

/// C++ `DatabaseSnapshotTest::SetupDatabase`: two databases with two tables
/// each, a secondary and a fulltext index, ten 5000-row appends per table, one
/// snapshot per database, then the databases are dropped.
fn setup_database_snapshots(e: &Engine) {
    for i in 0..2 {
        let db = format!("db_{i}");
        let tb1 = format!("{db}_tb_1");
        let tb2 = format!("{db}_tb_2");
        e.execute(&format!("CREATE DATABASE {db};")).unwrap();
        e.execute(&format!(
            "CREATE TABLE {db}.{tb1} (col1 INT, col2 VARCHAR);"
        ))
        .unwrap();
        e.execute(&format!(
            "CREATE TABLE {db}.{tb2} (col1 INT, col2 VARCHAR);"
        ))
        .unwrap();
        e.execute(&format!("CREATE INDEX idx1 ON {db}.{tb1}(col1);"))
            .unwrap();
        e.execute(&format!(
            "CREATE INDEX idx2 ON {db}.{tb1}(col2) USING fulltext;"
        ))
        .unwrap();
        for j in 0..10 {
            append_repeated(e, &format!("{db}.{tb1}"), j, 1, 5000);
            append_repeated(e, &format!("{db}.{tb2}"), j, 1, 5000);
        }
        e.execute(&format!("CREATE SNAPSHOT snapshot_{i} ON DATABASE {db};"))
            .unwrap();
        assert!(e.execute(&format!("SHOW SNAPSHOT snapshot_{i};")).is_ok());
        e.execute(&format!("DROP DATABASE {db};")).unwrap();
    }
}

/// C++ `src/unit_test/storage/txn/database_snapshot_ut.cpp:163`
/// (`test_restore_database_rollback_basic`).
#[test]
fn database_snapshot_restore_rollback_basic() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    setup_database_snapshots(&e);

    e.execute("RESTORE DATABASE SNAPSHOT snapshot_0;").unwrap();
    assert_restored_table(&e, "db_0.db_0_tb_1", 50_000);
    assert_restored_table(&e, "db_0.db_0_tb_2", 50_000);
    // Rows keep their values through the snapshot round trip.
    let sample = rows(
        &e,
        "SELECT col1, col2 FROM db_0.db_0_tb_1 WHERE col1 = 9 LIMIT 1;",
    );
    assert_eq!(sample, vec![vec!["9", "abcdefghijklmnopqrstuvwxyz"]]);

    e.execute("DROP DATABASE db_0;").unwrap();
    assert!(!e.table_exists("db_0.db_0_tb_1"));

    // `RestoreDatabaseSnapshot` inside a transaction, then `Rollback`: the
    // restore must not become visible.
    let pending = e.begin_restore_database_snapshot("snapshot_0").unwrap();
    pending.rollback();
    assert!(!e.table_exists("db_0.db_0_tb_1"));
    assert!(!databases(&e).iter().any(|d| d == "db_0"));
}

/// C++ `src/unit_test/storage/txn/database_snapshot_ut.cpp:247`
/// (`test_restore_database_create_database_multithreaded`).
#[test]
fn database_snapshot_restore_create_database_multithreaded() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    setup_database_snapshots(&e);

    let barrier = Barrier::new(2);
    let restore_ok = AtomicBool::new(false);
    let create_ok = AtomicBool::new(false);
    std::thread::scope(|s| {
        s.spawn(|| {
            barrier.wait();
            let pending = e.begin_restore_database_snapshot("snapshot_0").unwrap();
            restore_ok.store(pending.commit().is_ok(), Ordering::SeqCst);
        });
        s.spawn(|| {
            barrier.wait();
            create_ok.store(e.execute("CREATE DATABASE db_0;").is_ok(), Ordering::SeqCst);
        });
    });

    // `ConflictType::kError`: whichever request commits second must fail.
    assert!(
        restore_ok.load(Ordering::SeqCst) || create_ok.load(Ordering::SeqCst),
        "neither the restore nor the create committed"
    );
    if restore_ok.load(Ordering::SeqCst) {
        assert_restored_table(&e, "db_0.db_0_tb_1", 50_000);
    } else {
        assert!(create_ok.load(Ordering::SeqCst));
        assert!(!e.table_exists("db_0.db_0_tb_1"));
    }
}

/// C++ `src/unit_test/storage/txn/database_snapshot_ut.cpp:323`
/// (`test_create_snapshot_same_name_multithreaded`).
#[test]
fn database_snapshot_create_same_name_multithreaded() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    setup_database_snapshots(&e);

    e.execute("RESTORE DATABASE SNAPSHOT snapshot_0;").unwrap();
    assert_restored_table(&e, "db_0.db_0_tb_1", 50_000);

    let barrier = Barrier::new(2);
    let succeeded = AtomicUsize::new(0);
    std::thread::scope(|s| {
        for _ in 0..2 {
            s.spawn(|| {
                barrier.wait();
                if e.execute("CREATE SNAPSHOT conflict_snapshot ON DATABASE db_0;")
                    .is_ok()
                {
                    succeeded.fetch_add(1, Ordering::SeqCst);
                }
            });
        }
    });
    assert!(succeeded.load(Ordering::SeqCst) >= 1);
    assert!(e
        .snapshot_names()
        .unwrap()
        .iter()
        .any(|n| n == "conflict_snapshot"));

    assert!(e.execute("SHOW SNAPSHOTS;").is_ok());
    e.execute("DROP SNAPSHOT conflict_snapshot;").unwrap();
    assert!(!e
        .snapshot_names()
        .unwrap()
        .iter()
        .any(|n| n == "conflict_snapshot"));
}

/// C++ `src/unit_test/storage/txn/database_snapshot_ut.cpp:411`
/// (`test_sql_parser`).
#[test]
fn database_snapshot_sql_parser() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    setup_database_snapshots(&e);

    e.execute("RESTORE DATABASE SNAPSHOT snapshot_0;").unwrap();
    assert_restored_table(&e, "db_0.db_0_tb_1", 50_000);
    assert_restored_table(&e, "db_0.db_0_tb_2", 50_000);
}

/// C++ `SystemSnapshotTest::SetupDatabase`: two databases with two tables
/// each, a secondary and a fulltext index, ten 5000-row appends per table, one
/// system snapshot, then the databases are dropped.
fn setup_system_snapshot(e: &Engine) {
    for i in 0..2 {
        let db = format!("db{i}");
        let tb1 = format!("{db}_tb1");
        let tb2 = format!("{db}_tb2");
        e.execute(&format!("CREATE DATABASE {db};")).unwrap();
        e.execute(&format!(
            "CREATE TABLE {db}.{tb1} (col1 INT, col2 VARCHAR);"
        ))
        .unwrap();
        e.execute(&format!(
            "CREATE TABLE {db}.{tb2} (col1 INT, col2 VARCHAR);"
        ))
        .unwrap();
        e.execute(&format!("CREATE INDEX idx1 ON {db}.{tb1}(col1);"))
            .unwrap();
        e.execute(&format!(
            "CREATE INDEX idx2 ON {db}.{tb2}(col2) USING fulltext;"
        ))
        .unwrap();
        for j in 0..10 {
            append_repeated(e, &format!("{db}.{tb1}"), j, 1, 5000);
            append_repeated(e, &format!("{db}.{tb2}"), j, 1, 5000);
        }
    }
    e.execute("CREATE SNAPSHOT system_snapshot ON SYSTEM;")
        .unwrap();
    assert!(e.execute("SHOW SNAPSHOT system_snapshot;").is_ok());
    for db in ["db0", "db1"] {
        e.execute(&format!("DROP DATABASE {db};")).unwrap();
    }
}

/// C++ `src/unit_test/storage/txn/system_snapshot_ut.cpp:188`
/// (`test_restore_system_rollback_basic`).
#[test]
fn system_snapshot_restore_rollback_basic() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    setup_system_snapshot(&e);

    for db in ["db0", "db1"] {
        assert!(!databases(&e).iter().any(|d| d == db));
        assert!(!e.table_exists(&format!("{db}.{db}_tb1")));
    }

    e.execute("RESTORE SYSTEM SNAPSHOT system_snapshot;")
        .unwrap();
    for db in ["db0", "db1"] {
        assert_restored_table(&e, &format!("{db}.{db}_tb1"), 50_000);
        assert_restored_table(&e, &format!("{db}.{db}_tb2"), 50_000);
    }
    let listed = databases(&e);
    assert!(listed.iter().any(|d| d == "db0"));
    assert!(listed.iter().any(|d| d == "db1"));
    assert!(e.execute("SHOW DATABASES;").is_ok());
    // `select count(*) from db0.<first table>` in the reference fixture.
    assert_eq!(count(&e, "db0.db0_tb1"), 50_000);

    for db in ["db0", "db1"] {
        e.execute(&format!("DROP DATABASE {db};")).unwrap();
    }

    let pending = e.begin_restore_system_snapshot("system_snapshot").unwrap();
    pending.rollback();
    for db in ["db0", "db1"] {
        assert!(!databases(&e).iter().any(|d| d == db));
        assert!(!e.table_exists(&format!("{db}.{db}_tb1")));
    }
}

/// C++ `src/unit_test/storage/txn/system_snapshot_ut.cpp:292`
/// (`test_restore_system_create_database_multithreaded`).
#[test]
fn system_snapshot_restore_create_database_multithreaded() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    setup_system_snapshot(&e);

    let barrier = Barrier::new(2);
    let restore_ok = AtomicBool::new(false);
    let create_ok = AtomicBool::new(false);
    std::thread::scope(|s| {
        s.spawn(|| {
            barrier.wait();
            let pending = e.begin_restore_system_snapshot("system_snapshot").unwrap();
            restore_ok.store(pending.commit().is_ok(), Ordering::SeqCst);
        });
        s.spawn(|| {
            barrier.wait();
            create_ok.store(e.execute("CREATE DATABASE db0;").is_ok(), Ordering::SeqCst);
        });
    });

    // `ConflictType::kError`: whichever request commits second must fail.
    assert!(
        restore_ok.load(Ordering::SeqCst) || create_ok.load(Ordering::SeqCst),
        "neither the restore nor the create committed"
    );
    if restore_ok.load(Ordering::SeqCst) {
        assert_restored_table(&e, "db0.db0_tb1", 50_000);
        assert_restored_table(&e, "db1.db1_tb1", 50_000);
    } else {
        assert!(create_ok.load(Ordering::SeqCst));
        assert!(!e.table_exists("db0.db0_tb1"));
    }
}

/// C++ `src/unit_test/storage/txn/system_snapshot_ut.cpp:366`
/// (`test_sql_parser`). The fixture has already dropped `db0`/`db1`, so the
/// system snapshot taken here is empty and its restore is a no-op, as in the
/// reference.
#[test]
fn system_snapshot_sql_parser() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    setup_system_snapshot(&e);

    e.execute("CREATE SNAPSHOT sql_snapshot ON SYSTEM;")
        .unwrap();
    assert!(e
        .snapshot_names()
        .unwrap()
        .iter()
        .any(|n| n == "sql_snapshot"));
    e.execute("RESTORE SYSTEM SNAPSHOT sql_snapshot;").unwrap();
}

/// C++ `TableSnapshotTest::SetupTestTable`: `default_db.tb1` with a secondary
/// and a fulltext index, twenty 5000-row appends, one table snapshot, then the
/// table is dropped and the WAL checkpointed.
fn setup_table_snapshot(e: &Engine) {
    e.execute("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    e.execute("CREATE INDEX idx1 ON tb1(col1);").unwrap();
    e.execute("CREATE INDEX idx2 ON tb1(col2) USING fulltext;")
        .unwrap();
    for j in 0..20 {
        append_repeated(e, "default_db.tb1", j, 1, 5000);
    }
    e.execute("CREATE SNAPSHOT tb1_snapshot ON TABLE tb1;")
        .unwrap();
    e.execute("DROP TABLE tb1;").unwrap();
    e.checkpoint_wal();
    assert!(e.execute("SHOW SNAPSHOT tb1_snapshot;").is_ok());
}

/// C++ `src/unit_test/storage/txn/table_snapshot_ut.cpp:156`
/// (`test_restore_table_rollback_basic`).
#[test]
fn table_snapshot_restore_rollback_basic() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    setup_table_snapshot(&e);

    let pending = e.begin_restore_table_snapshot("tb1_snapshot").unwrap();
    pending.commit().unwrap();
    assert_restored_table(&e, "default_db.tb1", 100_000);
    let sample = rows(&e, "SELECT col1, col2 FROM tb1 WHERE col1 = 19 LIMIT 1;");
    assert_eq!(sample, vec![vec!["19", "abcdefghijklmnopqrstuvwxyz"]]);

    e.execute("DROP TABLE tb1;").unwrap();

    // `RestoreTableSnapshot` inside a transaction, then `Rollback`.
    let pending = e.begin_restore_table_snapshot("tb1_snapshot").unwrap();
    pending.rollback();
    assert!(!e.table_exists("default_db.tb1"));
}

/// C++ `src/unit_test/storage/txn/table_snapshot_ut.cpp:229`
/// (`test_restore_table_create_table_multithreaded`).
#[test]
fn table_snapshot_restore_create_table_multithreaded() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    setup_table_snapshot(&e);

    let barrier = Barrier::new(2);
    let restore_ok = AtomicBool::new(false);
    let create_ok = AtomicBool::new(false);
    std::thread::scope(|s| {
        s.spawn(|| {
            barrier.wait();
            restore_ok.store(
                e.execute("RESTORE TABLE SNAPSHOT tb1_snapshot;").is_ok(),
                Ordering::SeqCst,
            );
        });
        s.spawn(|| {
            barrier.wait();
            create_ok.store(
                e.execute("CREATE TABLE tb1(col1 int, col2 varchar);")
                    .is_ok(),
                Ordering::SeqCst,
            );
        });
    });

    assert!(
        restore_ok.load(Ordering::SeqCst) || create_ok.load(Ordering::SeqCst),
        "neither the restore nor the create committed"
    );
    let n = count(&e, "default_db.tb1");
    if restore_ok.load(Ordering::SeqCst) {
        assert_eq!(n, 100_000, "restored rows");
        assert_table_schema(&e, "default_db.tb1");
    } else {
        assert_eq!(n, 0, "the empty table created by the racing request");
    }
}

/// C++ `src/unit_test/storage/txn/table_snapshot_ut.cpp:285`
/// (`test_create_snapshot_same_name_multithreaded`).
#[test]
fn table_snapshot_create_same_name_multithreaded() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    setup_table_snapshot(&e);

    e.execute("CREATE TABLE tb1(col1 int, col2 varchar);")
        .unwrap();
    e.execute("INSERT INTO tb1 VALUES (1, 'abc'), (2, 'def'), (3, 'ghi');")
        .unwrap();

    let barrier = Barrier::new(2);
    let succeeded = AtomicUsize::new(0);
    std::thread::scope(|s| {
        for _ in 0..2 {
            s.spawn(|| {
                barrier.wait();
                if e.execute("CREATE SNAPSHOT conflict_snapshot ON TABLE tb1;")
                    .is_ok()
                {
                    succeeded.fetch_add(1, Ordering::SeqCst);
                }
            });
        }
    });
    assert!(succeeded.load(Ordering::SeqCst) >= 1);
    assert!(e
        .snapshot_names()
        .unwrap()
        .iter()
        .any(|n| n == "conflict_snapshot"));
}

/// C++ `src/unit_test/storage/txn/table_snapshot_ut.cpp:355`
/// (`test_show_snapshot_multithreaded`).
#[test]
fn table_snapshot_show_multithreaded() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    setup_table_snapshot(&e);

    e.execute("CREATE TABLE tb1(col1 int, col2 varchar);")
        .unwrap();
    e.execute("INSERT INTO tb1 VALUES (1, 'abc'), (2, 'def'), (3, 'ghi');")
        .unwrap();

    let created = AtomicBool::new(false);
    std::thread::scope(|s| {
        s.spawn(|| {
            created.store(
                e.execute("CREATE SNAPSHOT conflict_snapshot ON TABLE tb1;")
                    .is_ok(),
                Ordering::SeqCst,
            );
        });
        s.spawn(|| {
            assert!(e.execute("SHOW SNAPSHOTS;").is_ok());
        });
    });
    assert!(created.load(Ordering::SeqCst));
    assert!(e.execute("SHOW SNAPSHOTS;").is_ok());
    assert!(e
        .snapshot_names()
        .unwrap()
        .iter()
        .any(|n| n == "conflict_snapshot"));

    e.execute("DROP SNAPSHOT conflict_snapshot;").unwrap();
    assert!(!e
        .snapshot_names()
        .unwrap()
        .iter()
        .any(|n| n == "conflict_snapshot"));
}

/// C++ `src/unit_test/storage/txn/table_snapshot_ut.cpp:424`
/// (`test_restore_table_same_snapshot_multithreaded`).
#[test]
fn table_snapshot_restore_same_snapshot_multithreaded() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    setup_table_snapshot(&e);

    let barrier = Barrier::new(2);
    let succeeded = AtomicUsize::new(0);
    std::thread::scope(|s| {
        for _ in 0..2 {
            s.spawn(|| {
                barrier.wait();
                if e.execute("RESTORE TABLE SNAPSHOT tb1_snapshot;").is_ok() {
                    succeeded.fetch_add(1, Ordering::SeqCst);
                }
            });
        }
    });
    assert!(succeeded.load(Ordering::SeqCst) >= 1);
    assert_eq!(count(&e, "default_db.tb1"), 100_000);
    assert_table_schema(&e, "default_db.tb1");
}
