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
//! The two data-visibility cases (`test_create_snapshot_insert_data`,
//! `test_create_snapshot_delete_data`) use [`crate::EngineTxn`]: an append or a
//! row-id delete is staged in a live transaction, and a snapshot taken by
//! another transaction must contain only committed state.

use crate::{Engine, QueryResult};
use ::common::Error;
use parser::Literal;
use std::sync::atomic::{AtomicBool, AtomicUsize, Ordering};
use tempfile::tempdir;

async fn rows(e: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in e.execute(sql).await.unwrap() {
        if let QueryResult::Rows { .. } = result {
            return result.rows_text();
        }
    }
    Vec::new()
}

async fn count(e: &Engine, table: &str) -> usize {
    rows(e, &format!("SELECT count(*) FROM {table};")).await[0][0]
        .parse()
        .unwrap()
}

async fn databases(e: &Engine) -> Vec<String> {
    rows(e, "SHOW DATABASES;")
        .await
        .into_iter()
        .map(|r| r[0].clone())
        .collect()
}

/// Column name and rendered type of every column in `table`.
async fn columns(e: &Engine, table: &str) -> Vec<(String, String)> {
    rows(e, &format!("SHOW TABLE {table} COLUMNS;"))
        .await
        .into_iter()
        .map(|r| (r[0].clone(), r[1].clone()))
        .collect()
}

/// The fixture tables all declare `(col1 INT, col2 VARCHAR)`.
async fn assert_table_schema(e: &Engine, table: &str) {
    assert_eq!(
        columns(e, table).await,
        vec![
            ("col1".to_string(), "Integer".to_string()),
            ("col2".to_string(), "Varchar".to_string()),
        ],
        "schema of {table}"
    );
}

async fn assert_restored_table(e: &Engine, table: &str, expected_rows: usize) {
    assert!(e.table_exists(table), "{table} was not restored");
    assert_eq!(count(e, table).await, expected_rows, "row count of {table}");
    assert_table_schema(e, table).await;
}

/// `MakeInputBlock(v1, v2, row_cnt)`: `blocks` appends of `per_block` rows
/// carrying the same values.
async fn append_repeated(e: &Engine, table: &str, value: i64, blocks: usize, per_block: usize) {
    for _ in 0..blocks {
        let batch: Vec<Vec<Literal>> = (0..per_block)
            .map(|_| {
                vec![
                    Literal::Int(value),
                    Literal::Str("abcdefghijklmnopqrstuvwxyz".to_string()),
                ]
            })
            .collect();
        e.insert_literals(table, None, batch).await.unwrap();
    }
}

/// C++ `DatabaseSnapshotTest::SetupDatabase`: two databases with two tables
/// each, a secondary and a fulltext index, ten 5000-row appends per table, one
/// snapshot per database, then the databases are dropped.
async fn setup_database_snapshots(e: &Engine) {
    for i in 0..2 {
        let db = format!("db_{i}");
        let tb1 = format!("{db}_tb_1");
        let tb2 = format!("{db}_tb_2");
        e.execute(&format!("CREATE DATABASE {db};")).await.unwrap();
        e.execute(&format!(
            "CREATE TABLE {db}.{tb1} (col1 INT, col2 VARCHAR);"
        ))
        .await
        .unwrap();
        e.execute(&format!(
            "CREATE TABLE {db}.{tb2} (col1 INT, col2 VARCHAR);"
        ))
        .await
        .unwrap();
        e.execute(&format!("CREATE INDEX idx1 ON {db}.{tb1}(col1);"))
            .await
            .unwrap();
        e.execute(&format!(
            "CREATE INDEX idx2 ON {db}.{tb1}(col2) USING fulltext;"
        ))
        .await
        .unwrap();
        for j in 0..10 {
            append_repeated(e, &format!("{db}.{tb1}"), j, 1, 5000).await;
            append_repeated(e, &format!("{db}.{tb2}"), j, 1, 5000).await;
        }
        e.execute(&format!("CREATE SNAPSHOT snapshot_{i} ON DATABASE {db};"))
            .await
            .unwrap();
        assert!(e
            .execute(&format!("SHOW SNAPSHOT snapshot_{i};"))
            .await
            .is_ok());
        e.execute(&format!("DROP DATABASE {db};")).await.unwrap();
    }
}

/// C++ `src/unit_test/storage/txn/database_snapshot_ut.cpp:163`
/// (`test_restore_database_rollback_basic`).
#[tokio::test]
async fn database_snapshot_restore_rollback_basic() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup_database_snapshots(&e).await;

    e.execute("RESTORE DATABASE SNAPSHOT snapshot_0;")
        .await
        .unwrap();
    assert_restored_table(&e, "db_0.db_0_tb_1", 50_000).await;
    assert_restored_table(&e, "db_0.db_0_tb_2", 50_000).await;
    // Rows keep their values through the snapshot round trip.
    let sample = rows(
        &e,
        "SELECT col1, col2 FROM db_0.db_0_tb_1 WHERE col1 = 9 LIMIT 1;",
    )
    .await;
    assert_eq!(sample, vec![vec!["9", "abcdefghijklmnopqrstuvwxyz"]]);

    e.execute("DROP DATABASE db_0;").await.unwrap();
    assert!(!e.table_exists("db_0.db_0_tb_1"));

    // `RestoreDatabaseSnapshot` inside a transaction, then `Rollback`: the
    // restore must not become visible.
    let pending = e.begin_restore_database_snapshot("snapshot_0").unwrap();
    pending.rollback();
    assert!(!e.table_exists("db_0.db_0_tb_1"));
    assert!(!databases(&e).await.iter().any(|d| d == "db_0"));
}

/// C++ `src/unit_test/storage/txn/database_snapshot_ut.cpp:247`
/// (`test_restore_database_create_database_multithreaded`).
#[tokio::test]
async fn database_snapshot_restore_create_database_multithreaded() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup_database_snapshots(&e).await;

    // Two tasks released together, not two threads with a `block_on`: both
    // sides are statements, and the race this pins is the commit order, which
    // the multi-thread runtime interleaves on its own.
    let barrier = std::sync::Arc::new(tokio::sync::Barrier::new(2));
    let restore_ok = AtomicBool::new(false);
    let create_ok = AtomicBool::new(false);
    let restore = {
        let (e, barrier, ok) = (&e, std::sync::Arc::clone(&barrier), &restore_ok);
        async move {
            barrier.wait().await;
            let pending = e.begin_restore_database_snapshot("snapshot_0").unwrap();
            ok.store(pending.commit().await.is_ok(), Ordering::SeqCst);
        }
    };
    let create = {
        let (e, barrier, ok) = (&e, std::sync::Arc::clone(&barrier), &create_ok);
        async move {
            barrier.wait().await;
            ok.store(
                e.execute("CREATE DATABASE db_0;").await.is_ok(),
                Ordering::SeqCst,
            );
        }
    };
    tokio::join!(restore, create);

    // `ConflictType::kError`: whichever request commits second must fail.
    assert!(
        restore_ok.load(Ordering::SeqCst) || create_ok.load(Ordering::SeqCst),
        "neither the restore nor the create committed"
    );
    if restore_ok.load(Ordering::SeqCst) {
        assert_restored_table(&e, "db_0.db_0_tb_1", 50_000).await;
    } else {
        assert!(create_ok.load(Ordering::SeqCst));
        assert!(!e.table_exists("db_0.db_0_tb_1"));
    }
}

/// C++ `src/unit_test/storage/txn/database_snapshot_ut.cpp:323`
/// (`test_create_snapshot_same_name_multithreaded`).
#[tokio::test]
async fn database_snapshot_create_same_name_multithreaded() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup_database_snapshots(&e).await;

    e.execute("RESTORE DATABASE SNAPSHOT snapshot_0;")
        .await
        .unwrap();
    assert_restored_table(&e, "db_0.db_0_tb_1", 50_000).await;

    // Tasks behind a tokio barrier: both statements await, and a thread would
    // park on each of them (AGENTS.md §7.1).
    let barrier = std::sync::Arc::new(tokio::sync::Barrier::new(2));
    let succeeded = AtomicUsize::new(0);
    let race = |barrier: std::sync::Arc<tokio::sync::Barrier>| {
        let (e, ok) = (&e, &succeeded);
        async move {
            barrier.wait().await;
            if e.execute("CREATE SNAPSHOT conflict_snapshot ON DATABASE db_0;")
                .await
                .is_ok()
            {
                ok.fetch_add(1, Ordering::SeqCst);
            }
        }
    };
    tokio::join!(race(barrier.clone()), race(barrier));
    assert!(succeeded.load(Ordering::SeqCst) >= 1);
    assert!(e
        .snapshot_names()
        .unwrap()
        .iter()
        .any(|n| n == "conflict_snapshot"));

    assert!(e.execute("SHOW SNAPSHOTS;").await.is_ok());
    e.execute("DROP SNAPSHOT conflict_snapshot;").await.unwrap();
    assert!(!e
        .snapshot_names()
        .unwrap()
        .iter()
        .any(|n| n == "conflict_snapshot"));
}

/// C++ `src/unit_test/storage/txn/database_snapshot_ut.cpp:411`
/// (`test_sql_parser`).
#[tokio::test]
async fn database_snapshot_sql_parser() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup_database_snapshots(&e).await;

    e.execute("RESTORE DATABASE SNAPSHOT snapshot_0;")
        .await
        .unwrap();
    assert_restored_table(&e, "db_0.db_0_tb_1", 50_000).await;
    assert_restored_table(&e, "db_0.db_0_tb_2", 50_000).await;
}

/// C++ `SystemSnapshotTest::SetupDatabase`: two databases with two tables
/// each, a secondary and a fulltext index, ten 5000-row appends per table, one
/// system snapshot, then the databases are dropped.
async fn setup_system_snapshot(e: &Engine) {
    for i in 0..2 {
        let db = format!("db{i}");
        let tb1 = format!("{db}_tb1");
        let tb2 = format!("{db}_tb2");
        e.execute(&format!("CREATE DATABASE {db};")).await.unwrap();
        e.execute(&format!(
            "CREATE TABLE {db}.{tb1} (col1 INT, col2 VARCHAR);"
        ))
        .await
        .unwrap();
        e.execute(&format!(
            "CREATE TABLE {db}.{tb2} (col1 INT, col2 VARCHAR);"
        ))
        .await
        .unwrap();
        e.execute(&format!("CREATE INDEX idx1 ON {db}.{tb1}(col1);"))
            .await
            .unwrap();
        e.execute(&format!(
            "CREATE INDEX idx2 ON {db}.{tb2}(col2) USING fulltext;"
        ))
        .await
        .unwrap();
        for j in 0..10 {
            append_repeated(e, &format!("{db}.{tb1}"), j, 1, 5000).await;
            append_repeated(e, &format!("{db}.{tb2}"), j, 1, 5000).await;
        }
    }
    e.execute("CREATE SNAPSHOT system_snapshot ON SYSTEM;")
        .await
        .unwrap();
    assert!(e.execute("SHOW SNAPSHOT system_snapshot;").await.is_ok());
    for db in ["db0", "db1"] {
        e.execute(&format!("DROP DATABASE {db};")).await.unwrap();
    }
}

/// C++ `src/unit_test/storage/txn/system_snapshot_ut.cpp:188`
/// (`test_restore_system_rollback_basic`).
#[tokio::test]
async fn system_snapshot_restore_rollback_basic() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup_system_snapshot(&e).await;

    for db in ["db0", "db1"] {
        assert!(!databases(&e).await.iter().any(|d| d == db));
        assert!(!e.table_exists(&format!("{db}.{db}_tb1")));
    }

    e.execute("RESTORE SYSTEM SNAPSHOT system_snapshot;")
        .await
        .unwrap();
    for db in ["db0", "db1"] {
        assert_restored_table(&e, &format!("{db}.{db}_tb1"), 50_000).await;
        assert_restored_table(&e, &format!("{db}.{db}_tb2"), 50_000).await;
    }
    let listed = databases(&e).await;
    assert!(listed.iter().any(|d| d == "db0"));
    assert!(listed.iter().any(|d| d == "db1"));
    assert!(e.execute("SHOW DATABASES;").await.is_ok());
    // `select count(*) from db0.<first table>` in the reference fixture.
    assert_eq!(count(&e, "db0.db0_tb1").await, 50_000);

    for db in ["db0", "db1"] {
        e.execute(&format!("DROP DATABASE {db};")).await.unwrap();
    }

    let pending = e.begin_restore_system_snapshot("system_snapshot").unwrap();
    pending.rollback();
    for db in ["db0", "db1"] {
        assert!(!databases(&e).await.iter().any(|d| d == db));
        assert!(!e.table_exists(&format!("{db}.{db}_tb1")));
    }
}

/// C++ `src/unit_test/storage/txn/system_snapshot_ut.cpp:292`
/// (`test_restore_system_create_database_multithreaded`).
#[tokio::test]
async fn system_snapshot_restore_create_database_multithreaded() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup_system_snapshot(&e).await;

    // Two tasks released together, for the same reason as the database-snapshot
    // test above.
    let barrier = std::sync::Arc::new(tokio::sync::Barrier::new(2));
    let restore_ok = AtomicBool::new(false);
    let create_ok = AtomicBool::new(false);
    let restore = {
        let (e, barrier, ok) = (&e, std::sync::Arc::clone(&barrier), &restore_ok);
        async move {
            barrier.wait().await;
            let pending = e.begin_restore_system_snapshot("system_snapshot").unwrap();
            ok.store(pending.commit().await.is_ok(), Ordering::SeqCst);
        }
    };
    let create = {
        let (e, barrier, ok) = (&e, std::sync::Arc::clone(&barrier), &create_ok);
        async move {
            barrier.wait().await;
            ok.store(
                e.execute("CREATE DATABASE db0;").await.is_ok(),
                Ordering::SeqCst,
            );
        }
    };
    tokio::join!(restore, create);

    // `ConflictType::kError`: whichever request commits second must fail.
    assert!(
        restore_ok.load(Ordering::SeqCst) || create_ok.load(Ordering::SeqCst),
        "neither the restore nor the create committed"
    );
    if restore_ok.load(Ordering::SeqCst) {
        assert_restored_table(&e, "db0.db0_tb1", 50_000).await;
        assert_restored_table(&e, "db1.db1_tb1", 50_000).await;
    } else {
        assert!(create_ok.load(Ordering::SeqCst));
        assert!(!e.table_exists("db0.db0_tb1"));
    }
}

/// C++ `src/unit_test/storage/txn/system_snapshot_ut.cpp:366`
/// (`test_sql_parser`). The fixture has already dropped `db0`/`db1`, so the
/// system snapshot taken here is empty and its restore is a no-op, as in the
/// reference.
#[tokio::test]
async fn system_snapshot_sql_parser() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup_system_snapshot(&e).await;

    e.execute("CREATE SNAPSHOT sql_snapshot ON SYSTEM;")
        .await
        .unwrap();
    assert!(e
        .snapshot_names()
        .unwrap()
        .iter()
        .any(|n| n == "sql_snapshot"));
    e.execute("RESTORE SYSTEM SNAPSHOT sql_snapshot;")
        .await
        .unwrap();
}

/// C++ `TableSnapshotTest::SetupTestTable`: `default_db.tb1` with a secondary
/// and a fulltext index, twenty 5000-row appends, one table snapshot, then the
/// table is dropped and the WAL checkpointed.
async fn setup_table_snapshot(e: &Engine) {
    e.execute("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .await
        .unwrap();
    e.execute("CREATE INDEX idx1 ON tb1(col1);").await.unwrap();
    e.execute("CREATE INDEX idx2 ON tb1(col2) USING fulltext;")
        .await
        .unwrap();
    for j in 0..20 {
        append_repeated(e, "default_db.tb1", j, 1, 5000).await;
    }
    e.execute("CREATE SNAPSHOT tb1_snapshot ON TABLE tb1;")
        .await
        .unwrap();
    e.execute("DROP TABLE tb1;").await.unwrap();
    e.checkpoint_wal().await;
    assert!(e.execute("SHOW SNAPSHOT tb1_snapshot;").await.is_ok());
}

/// C++ `src/unit_test/storage/txn/table_snapshot_ut.cpp:156`
/// (`test_restore_table_rollback_basic`).
#[tokio::test]
async fn table_snapshot_restore_rollback_basic() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup_table_snapshot(&e).await;

    let pending = e.begin_restore_table_snapshot("tb1_snapshot").unwrap();
    pending.commit().await.unwrap();
    assert_restored_table(&e, "default_db.tb1", 100_000).await;
    let sample = rows(&e, "SELECT col1, col2 FROM tb1 WHERE col1 = 19 LIMIT 1;").await;
    assert_eq!(sample, vec![vec!["19", "abcdefghijklmnopqrstuvwxyz"]]);

    e.execute("DROP TABLE tb1;").await.unwrap();

    // `RestoreTableSnapshot` inside a transaction, then `Rollback`.
    let pending = e.begin_restore_table_snapshot("tb1_snapshot").unwrap();
    pending.rollback();
    assert!(!e.table_exists("default_db.tb1"));
}

/// C++ `src/unit_test/storage/txn/table_snapshot_ut.cpp:229`
/// (`test_restore_table_create_table_multithreaded`).
#[tokio::test]
async fn table_snapshot_restore_create_table_multithreaded() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup_table_snapshot(&e).await;

    let barrier = std::sync::Arc::new(tokio::sync::Barrier::new(2));
    let restore_ok = AtomicBool::new(false);
    let create_ok = AtomicBool::new(false);
    let restore = {
        let (e, barrier, ok) = (&e, std::sync::Arc::clone(&barrier), &restore_ok);
        async move {
            barrier.wait().await;
            ok.store(
                e.execute("RESTORE TABLE SNAPSHOT tb1_snapshot;")
                    .await
                    .is_ok(),
                Ordering::SeqCst,
            );
        }
    };
    let create = {
        let (e, barrier, ok) = (&e, std::sync::Arc::clone(&barrier), &create_ok);
        async move {
            barrier.wait().await;
            ok.store(
                e.execute("CREATE TABLE tb1(col1 int, col2 varchar);")
                    .await
                    .is_ok(),
                Ordering::SeqCst,
            );
        }
    };
    tokio::join!(restore, create);

    assert!(
        restore_ok.load(Ordering::SeqCst) || create_ok.load(Ordering::SeqCst),
        "neither the restore nor the create committed"
    );
    let n = count(&e, "default_db.tb1").await;
    if restore_ok.load(Ordering::SeqCst) {
        assert_eq!(n, 100_000, "restored rows");
        assert_table_schema(&e, "default_db.tb1").await;
    } else {
        assert_eq!(n, 0, "the empty table created by the racing request");
    }
}

/// C++ `src/unit_test/storage/txn/table_snapshot_ut.cpp:285`
/// (`test_create_snapshot_same_name_multithreaded`).
#[tokio::test]
async fn table_snapshot_create_same_name_multithreaded() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup_table_snapshot(&e).await;

    e.execute("CREATE TABLE tb1(col1 int, col2 varchar);")
        .await
        .unwrap();
    e.execute("INSERT INTO tb1 VALUES (1, 'abc'), (2, 'def'), (3, 'ghi');")
        .await
        .unwrap();

    let barrier = std::sync::Arc::new(tokio::sync::Barrier::new(2));
    let succeeded = AtomicUsize::new(0);
    let race = |barrier: std::sync::Arc<tokio::sync::Barrier>| {
        let (e, ok) = (&e, &succeeded);
        async move {
            barrier.wait().await;
            if e.execute("CREATE SNAPSHOT conflict_snapshot ON TABLE tb1;")
                .await
                .is_ok()
            {
                ok.fetch_add(1, Ordering::SeqCst);
            }
        }
    };
    tokio::join!(race(barrier.clone()), race(barrier));
    assert!(succeeded.load(Ordering::SeqCst) >= 1);
    assert!(e
        .snapshot_names()
        .unwrap()
        .iter()
        .any(|n| n == "conflict_snapshot"));
}

/// C++ `src/unit_test/storage/txn/table_snapshot_ut.cpp:355`
/// (`test_show_snapshot_multithreaded`).
#[tokio::test]
async fn table_snapshot_show_multithreaded() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup_table_snapshot(&e).await;

    e.execute("CREATE TABLE tb1(col1 int, col2 varchar);")
        .await
        .unwrap();
    e.execute("INSERT INTO tb1 VALUES (1, 'abc'), (2, 'def'), (3, 'ghi');")
        .await
        .unwrap();

    let created = AtomicBool::new(false);
    let create = {
        let (e, ok) = (&e, &created);
        async move {
            ok.store(
                e.execute("CREATE SNAPSHOT conflict_snapshot ON TABLE tb1;")
                    .await
                    .is_ok(),
                Ordering::SeqCst,
            );
        }
    };
    let show = async {
        assert!(e.execute("SHOW SNAPSHOTS;").await.is_ok());
    };
    tokio::join!(create, show);
    assert!(created.load(Ordering::SeqCst));
    assert!(e.execute("SHOW SNAPSHOTS;").await.is_ok());
    assert!(e
        .snapshot_names()
        .unwrap()
        .iter()
        .any(|n| n == "conflict_snapshot"));

    e.execute("DROP SNAPSHOT conflict_snapshot;").await.unwrap();
    assert!(!e
        .snapshot_names()
        .unwrap()
        .iter()
        .any(|n| n == "conflict_snapshot"));
}

/// C++ `src/unit_test/storage/txn/table_snapshot_ut.cpp:424`
/// (`test_restore_table_same_snapshot_multithreaded`).
#[tokio::test]
async fn table_snapshot_restore_same_snapshot_multithreaded() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup_table_snapshot(&e).await;

    let barrier = std::sync::Arc::new(tokio::sync::Barrier::new(2));
    let succeeded = AtomicUsize::new(0);
    let restore = |barrier: std::sync::Arc<tokio::sync::Barrier>| {
        let (e, ok) = (&e, &succeeded);
        async move {
            barrier.wait().await;
            if e.execute("RESTORE TABLE SNAPSHOT tb1_snapshot;")
                .await
                .is_ok()
            {
                ok.fetch_add(1, Ordering::SeqCst);
            }
        }
    };
    tokio::join!(restore(barrier.clone()), restore(barrier));
    assert!(succeeded.load(Ordering::SeqCst) >= 1);
    assert_eq!(count(&e, "default_db.tb1").await, 100_000);
    assert_table_schema(&e, "default_db.tb1").await;
}

/// C++ `src/unit_test/storage/txn/table_snapshot_ut.cpp:480`
/// (`test_create_snapshot_delete_data`).
///
/// Two row-id deletes are staged in live transactions; the snapshot created by
/// `txn2` in between must contain the committed 100,000 rows, i.e. neither the
/// earlier nor the later uncommitted delete may leak into it.
#[tokio::test]
async fn table_snapshot_create_snapshot_delete_data() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup_table_snapshot(&e).await;

    e.execute("RESTORE TABLE SNAPSHOT tb1_snapshot;")
        .await
        .unwrap();
    assert_eq!(count(&e, "default_db.tb1").await, 100_000);

    // `txn1->Delete(RowID(0, 1000..3000))`, not committed yet.
    let first: Vec<(u32, u32)> = (1000..3000).map(|row| (0, row)).collect();
    let mut txn1 = e.begin_txn();
    assert_eq!(txn1.delete_row_ids("default_db.tb1", &first).unwrap(), 2000);

    // The reference checkpoints the WAL between the two operations.
    e.checkpoint_wal().await;

    // `txn2->CreateTableSnapshot("default_db", "tb1", "test_delete")` while
    // `txn1` is still uncommitted.
    let txn2 = e.begin_txn();
    txn2.create_table_snapshot("default_db", "tb1", "test_delete")
        .await
        .unwrap();

    // `txn3->Delete(RowID(0, 4000..5000))`, also uncommitted.
    let second: Vec<(u32, u32)> = (4000..5000).map(|row| (0, row)).collect();
    let mut txn3 = e.begin_txn();
    assert_eq!(
        txn3.delete_row_ids("default_db.tb1", &second).unwrap(),
        1000
    );

    // Uncommitted deletes are invisible to a scan and to the snapshot.
    assert_eq!(count(&e, "default_db.tb1").await, 100_000);

    txn1.commit().await.unwrap();
    txn2.commit().await.unwrap();
    txn3.commit().await.unwrap();

    // Committed deletes are now visible.
    assert_eq!(count(&e, "default_db.tb1").await, 97_000);

    e.execute("DROP TABLE tb1;").await.unwrap();
    e.execute("RESTORE TABLE SNAPSHOT test_delete;")
        .await
        .unwrap();

    // The restored snapshot holds the state before the uncommitted deletes.
    let restored = e.fetch_rows_with_ids("default_db.tb1").await.unwrap();
    assert_eq!(restored.len(), 100_000);
    let ids: std::collections::HashSet<usize> = restored.iter().map(|(id, _)| *id).collect();
    assert!(
        ids.contains(&1000),
        "row 1000 was deleted only after the snapshot"
    );
    assert!(
        ids.contains(&4000),
        "row 4000 was deleted only after the snapshot"
    );
}

/// C++ `src/unit_test/storage/txn/table_snapshot_ut.cpp:576`
/// (`test_create_snapshot_insert_data`).
///
/// A 900-row append staged by `txn1` must not appear in the snapshot `txn2`
/// creates before either commits.
#[tokio::test]
async fn table_snapshot_create_snapshot_insert_data() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup_table_snapshot(&e).await;

    e.execute("RESTORE TABLE SNAPSHOT tb1_snapshot;")
        .await
        .unwrap();
    assert_eq!(count(&e, "default_db.tb1").await, 100_000);

    // `MakeInputBlock(999, "abcdefghijklmnopqrstuvwxyz", 900)`.
    let rows: Vec<Vec<Literal>> = (0..900)
        .map(|_| {
            vec![
                Literal::Int(999),
                Literal::Str("abcdefghijklmnopqrstuvwxyz".to_string()),
            ]
        })
        .collect();
    let mut txn1 = e.begin_txn();
    assert_eq!(txn1.append("default_db.tb1", rows).await.unwrap(), 900);

    let txn2 = e.begin_txn();
    txn2.create_table_snapshot("default_db", "tb1", "test_insert")
        .await
        .unwrap();

    // The append is still staged: a scan and the snapshot see 100,000 rows.
    assert_eq!(count(&e, "default_db.tb1").await, 100_000);

    // The reference commits the snapshot transaction before the append, and an
    // append that reaches its commit after a snapshot of the same table is a
    // retryable conflict (`CheckConflictTxnStore`: `kAppend` vs
    // `kCreateTableSnapshot`, `new_txn_impl.cpp:3406-3413`) - the snapshot the
    // client asked for must not gain the rows that were in flight.
    txn2.commit().await.unwrap();
    assert!(matches!(txn1.commit().await, Err(Error::TxnConflict(_))));
    assert_eq!(count(&e, "default_db.tb1").await, 100_000);

    e.execute("DROP TABLE tb1;").await.unwrap();
    e.execute("RESTORE TABLE SNAPSHOT test_insert;")
        .await
        .unwrap();

    // The uncommitted append was excluded from the snapshot.
    assert_eq!(count(&e, "default_db.tb1").await, 100_000);
}

/// C++ `src/unit_test/storage/secondary_index/test_high_cardinality_ut.cpp:112`
/// (`HighCardinalitySecondaryIndexTest.TestTxn`): the reference creates the
/// table and a secondary index through transactions, commits them, then builds
/// a high-cardinality chunk "just for coverage".
#[tokio::test]
async fn secondary_index_high_cardinality_txn() {
    use storage::secondary_index_data::{ordered_i64, HighCardinalityIndex};

    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();

    // BeginTxn(kCreateTable) -> CreateTable -> CommitTxn.
    e.execute("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .await
        .unwrap();
    // BeginTxn(kCreateIndex) -> CreateIndex -> CommitTxn.
    e.execute("CREATE INDEX second_idx ON tb1(col1);")
        .await
        .unwrap();

    // Committed DDL is visible.
    assert!(e.table_exists("default_db.tb1"));
    e.execute("SHOW TABLE tb1 INDEX second_idx;").await.unwrap();

    // "Just for coverage": insert into a high-cardinality chunk.
    let mut index = HighCardinalityIndex::new();
    index.insert_data((0..10).map(|i| (ordered_i64(i % 5), i as u32)));
    assert_eq!(index.row_count(), 10);
    assert_eq!(index.unique_key_count(), 0);
}
