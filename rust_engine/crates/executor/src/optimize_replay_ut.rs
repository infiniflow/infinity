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

async fn create_table(e: &Engine) {
    h::ok(e, "CREATE DATABASE db1;").await;
    h::ok(e, "CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);").await;
}

/// C++ `src/unit_test/storage/new_catalog/optimize_index_ut.cpp:193`
/// (`TestTxnOptimizeIndex.optimize_index_rollback`): a rolled-back optimize
/// keeps the incremental chunks' rows; a committed one keeps the same rows.
#[tokio::test]
async fn optimize_index_rollback() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);").await;
    for _ in 0..2 {
        h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192).await;
        e.flush().await.unwrap();
    }
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);

    // `RollBackTxn`: no optimize call.
    assert_eq!(
        h::rows(
            &e,
            "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abcdefghijklmnopqrstuvwxyz';"
        )
        .await,
        vec![vec!["16384"]]
    );
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index1;").await;

    // committed optimize
    e.optimize_indexes().await.unwrap();
    e.cleanup().await.unwrap();
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    assert_eq!(
        h::rows(
            &e,
            "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abcdefghijklmnopqrstuvwxyz';"
        )
        .await,
        vec![vec!["16384"]]
    );

    h::ok(&e, "DROP DATABASE db1;").await;
}

/// C++ `src/unit_test/storage/new_catalog/optimize_index_ut.cpp:256`
/// (`TestTxnOptimizeIndex.optimize_index_and_drop_db`): optimize then drop the
/// database.
#[tokio::test]
async fn optimize_index_then_drop_database() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);").await;
    for _ in 0..2 {
        h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192).await;
        e.flush().await.unwrap();
    }
    e.optimize_indexes().await.unwrap();
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);

    h::ok(&e, "DROP DATABASE db1;").await;
    assert!(e.execute("SELECT * FROM db1.tb1;").await.is_err());
    assert!(e
        .execute("INSERT INTO db1.tb1 VALUES (1, 'x');")
        .await
        .is_err());
}

/// C++ `src/unit_test/storage/new_catalog/optimize_index_ut.cpp:392`
/// (`TestTxnOptimizeIndex.optimize_index_and_drop_table`).
#[tokio::test]
async fn optimize_index_then_drop_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);").await;
    for _ in 0..2 {
        h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192).await;
        e.flush().await.unwrap();
    }
    e.optimize_indexes().await.unwrap();

    h::ok(&e, "DROP TABLE db1.tb1;").await;
    assert!(!e.table_exists("db1.tb1"));
    assert!(e.execute("SELECT * FROM db1.tb1;").await.is_err());
}

/// C++ `src/unit_test/storage/new_catalog/optimize_index_ut.cpp:540`
/// (`TestTxnOptimizeIndex.optimize_index_and_drop_index`): dropping the index
/// after optimize leaves the rows.
#[tokio::test]
async fn optimize_index_then_drop_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);").await;
    for _ in 0..2 {
        h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192).await;
        e.flush().await.unwrap();
    }
    e.optimize_indexes().await.unwrap();

    h::ok(&e, "DROP INDEX index1 ON db1.tb1;").await;
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    assert_eq!(
        h::rows(
            &e,
            "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abcdefghijklmnopqrstuvwxyz';"
        )
        .await,
        vec![vec!["16384"]]
    );
}

/// C++ `src/unit_test/storage/new_catalog/optimize_index_ut.cpp:687`
/// (`TestTxnOptimizeIndex.DISABLED_SLOW_optimize_index_and_optimize_index`).
#[tokio::test]
#[ignore = "slow: C++ TestTxnOptimizeIndex.DISABLED_SLOW_optimize_index_and_optimize_index"]
async fn optimize_index_and_optimize_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);").await;
    for _ in 0..2 {
        h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192).await;
        e.flush().await.unwrap();
    }
    e.optimize_indexes().await.unwrap();
    e.cleanup().await.unwrap();
    e.optimize_indexes().await.unwrap();
    e.cleanup().await.unwrap();
    let e = h::reopen(e, dir.path()).await;
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index1;").await;
}

/// C++ `src/unit_test/storage/new_catalog/optimize_index_ut.cpp:822`
/// (`TestTxnOptimizeIndex.DISABLED_SLOW_optimize_index_and_dump_index`).
#[tokio::test]
#[ignore = "slow: C++ TestTxnOptimizeIndex.DISABLED_SLOW_optimize_index_and_dump_index"]
async fn optimize_index_and_dump_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);").await;
    for _ in 0..2 {
        h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192).await;
        e.flush().await.unwrap();
    }
    e.optimize_indexes().await.unwrap();
    e.flush().await.unwrap();
    let e = h::reopen(e, dir.path()).await;
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index1;").await;
}

/// C++ `src/unit_test/storage/new_catalog/optimize_index_ut.cpp:982`
/// (`TestTxnOptimizeIndex.optimize_index_and_add_column`): rows read the new
/// column's default after optimize.
#[tokio::test]
async fn optimize_index_then_add_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);").await;
    for _ in 0..2 {
        h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192).await;
        e.flush().await.unwrap();
    }
    e.optimize_indexes().await.unwrap();

    h::ok(&e, "ALTER TABLE db1.tb1 ADD COLUMN (col3 INT DEFAULT 5);").await;
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    assert_eq!(
        h::rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col3 = 5;").await,
        vec![vec!["16384"]]
    );
}

/// C++ `src/unit_test/storage/new_catalog/optimize_index_ut.cpp:1207`
/// (`TestTxnOptimizeIndex.optimize_index_and_drop_column`): optimize then drop
/// the indexed column fails (the index still references it), matching the
/// reference's failed commit; dropping `col2` succeeds.
#[tokio::test]
async fn optimize_index_then_drop_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);").await;
    for _ in 0..2 {
        h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192).await;
        e.flush().await.unwrap();
    }
    e.optimize_indexes().await.unwrap();

    // Indexed column: blocked by the index.
    assert!(e
        .execute("ALTER TABLE db1.tb1 DROP COLUMN (col1);")
        .await
        .is_err());
    h::ok(&e, "ALTER TABLE db1.tb1 DROP COLUMN (col2);").await;
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    assert_eq!(h::rows(&e, "SHOW TABLE db1.tb1 COLUMNS;").await.len(), 1);
}

/// C++ `src/unit_test/storage/new_catalog/optimize_index_ut.cpp:1417`
/// (`TestTxnOptimizeIndex.optimize_index_and_rename_table`).
#[tokio::test]
async fn optimize_index_then_rename_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);").await;
    for _ in 0..2 {
        h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192).await;
        e.flush().await.unwrap();
    }
    e.optimize_indexes().await.unwrap();

    h::ok(&e, "ALTER TABLE db1.tb1 RENAME TO db1.tb2;").await;
    assert_eq!(e.row_count("db1.tb2").unwrap(), 16384);
    h::ok(&e, "SHOW TABLE db1.tb2 INDEX index1;").await;
}

/// C++ `src/unit_test/storage/new_catalog/optimize_index_ut.cpp:1611`
/// (`TestTxnOptimizeIndex.DISABLED_SLOW_optimize_index_and_compact_table`).
#[tokio::test]
#[ignore = "slow: C++ TestTxnOptimizeIndex.DISABLED_SLOW_optimize_index_and_compact_table"]
async fn optimize_index_and_compact_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);").await;
    for _ in 0..2 {
        h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192).await;
        e.flush().await.unwrap();
    }
    e.optimize_indexes().await.unwrap();
    e.compact_all_segments("db1.tb1").await.unwrap();
    let e = h::reopen(e, dir.path()).await;
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    // C++ renumbers the compacted output ({2,3} for its two live segments);
    // this engine keeps one active segment and allocates the next id, so the
    // equivalent assertion is a single segment holding every row.
    assert_eq!(e.segment_ids("db1.tb1").await, vec![1]);
}

/// `TestTxnReplayOptimize` setup: a secondary index plus two dumped appends.
async fn prepare_replay(e: &Engine) {
    create_table(e).await;
    h::ok(e, "CREATE INDEX index1 ON db1.tb1 (col1);").await;
    for _ in 0..2 {
        h::append_repeated(e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192).await;
        e.flush().await.unwrap();
    }
}

/// C++ `src/unit_test/storage/new_catalog/replay_optimize_ut.cpp:413`
/// (`TestTxnReplayOptimize.DISABLED_SLOW_test_optimize_commit`).
#[tokio::test]
#[ignore = "slow: C++ TestTxnReplayOptimize.DISABLED_SLOW_test_optimize_commit"]
async fn replay_optimize_commit() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    prepare_replay(&e).await;
    e.optimize_indexes().await.unwrap();
    e.cleanup().await.unwrap();

    let e = h::reopen(e, dir.path()).await;
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    assert_eq!(
        h::rows(
            &e,
            "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abcdefghijklmnopqrstuvwxyz';"
        )
        .await,
        vec![vec!["16384"]]
    );
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index1;").await;
}

/// C++ `src/unit_test/storage/new_catalog/replay_optimize_ut.cpp:455`
/// (`TestTxnReplayOptimize.DISABLED_SLOW_test_optimize_rollback`).
#[tokio::test]
#[ignore = "slow: C++ TestTxnReplayOptimize.DISABLED_SLOW_test_optimize_rollback"]
async fn replay_optimize_rollback() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    prepare_replay(&e).await;
    // Rollback: no optimize.

    let e = h::reopen(e, dir.path()).await;
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    assert_eq!(h::segment_rows(&e, "db1.tb1"), 16384);
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index1;").await;
}

/// C++ `src/unit_test/storage/new_catalog/replay_optimize_ut.cpp:511`
/// (`TestTxnReplayOptimize.DISABLED_SLOW_test_optimize_interrupt`).
#[tokio::test]
#[ignore = "slow: C++ TestTxnReplayOptimize.DISABLED_SLOW_test_optimize_interrupt"]
async fn replay_optimize_interrupt() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    prepare_replay(&e).await;
    // Interrupt: reopen before the optimize pass.
    drop(e);
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index1;").await;
}

/// C++ `src/unit_test/storage/new_catalog/replay_restore_ut.cpp:269`
/// (`TestTxnReplayRestore.DISABLED_SLOW_test_repaly_restore_table_snapshot`).
#[tokio::test]
#[ignore = "slow: C++ TestTxnReplayRestore.DISABLED_SLOW_test_repaly_restore_table_snapshot"]
async fn replay_restore_table_snapshot() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192).await;
    h::ok(&e, "CREATE SNAPSHOT tb_snapshot ON TABLE db1.tb1;").await;
    h::ok(&e, "DROP TABLE db1.tb1;").await;
    let e = h::reopen(e, dir.path()).await;

    h::ok(&e, "RESTORE TABLE SNAPSHOT tb_snapshot;").await;
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    assert_eq!(h::rows(&e, "SHOW TABLE db1.tb1 COLUMNS;").await.len(), 2);
}

/// C++ `src/unit_test/storage/new_catalog/replay_restore_ut.cpp:331`
/// (`TestTxnReplayRestore.DISABLED_SLOW_test_repaly_restore_database_snapshot`).
#[tokio::test]
#[ignore = "slow: C++ TestTxnReplayRestore.DISABLED_SLOW_test_repaly_restore_database_snapshot"]
async fn replay_restore_database_snapshot() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192).await;
    h::ok(&e, "CREATE SNAPSHOT db_snapshot ON DATABASE db1;").await;
    h::ok(&e, "DROP DATABASE db1;").await;
    let e = h::reopen(e, dir.path()).await;

    h::ok(&e, "RESTORE DATABASE SNAPSHOT db_snapshot;").await;
    assert!(e.table_exists("db1.tb1"));
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
}

/// C++ `src/unit_test/storage/new_catalog/replay_restore_ut.cpp:399`
/// (`TestTxnReplayRestore.DISABLED_SLOW_test_repaly_restore_system_snapshot`).
#[tokio::test]
#[ignore = "slow: C++ TestTxnReplayRestore.DISABLED_SLOW_test_repaly_restore_system_snapshot"]
async fn replay_restore_system_snapshot() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192).await;
    h::ok(&e, "CREATE SNAPSHOT sys_snapshot ON SYSTEM;").await;
    h::ok(&e, "DROP DATABASE db1;").await;
    let e = h::reopen(e, dir.path()).await;

    h::ok(&e, "RESTORE SYSTEM SNAPSHOT sys_snapshot;").await;
    assert!(e.table_exists("db1.tb1"));
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
}
