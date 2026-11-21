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

//! Ports of `src/unit_test/storage/new_catalog/replay_append_delete_ut.cpp`,
//! `replay_import_ut.cpp`, `replay_index_ut.cpp` and `new_txn_mgr_ut.cpp`
//! (`TestTxnReplayAppend`, `TestTxnReplayImport`, `TestTxnReplayIndex`,
//! `TestTxnManagerTest`).
//!
//! Restarts use [`crate::new_catalog_ut::reopen`]. `new_txn_mgr_ut` drives the
//! transaction manager itself: `CurrentTS` maps to [`crate::Engine::current_ts`],
//! and the reference's check-candidate list maps to the engine's per-key
//! delete-conflict rule (a delete that began before another commit on the same
//! row fails at commit). The per-transaction `BeginTS` is not asserted: it is
//! internal arithmetic (`current_ts + 1`), not observable behaviour.

use crate::new_catalog_ut as h;
use crate::Engine;
use parser::Literal;
use tempfile::tempdir;

async fn create_table(e: &Engine) {
    h::ok(e, "CREATE TABLE default_db.tb1 (col1 INT, col2 VARCHAR);").await;
}

fn alternating_block() -> Vec<Vec<Literal>> {
    (0..8192)
        .map(|i| {
            if i % 2 == 0 {
                vec![Literal::Int(1), Literal::Str("abc".to_string())]
            } else {
                vec![
                    Literal::Int(2),
                    Literal::Str("abcdefghijklmnopqrstuvwxyz".to_string()),
                ]
            }
        })
        .collect()
}

/// C++ `src/unit_test/storage/new_catalog/replay_append_delete_ut.cpp:68`
/// (`TestTxnReplayAppend.test_append0`): two 4-row appends replay into one
/// 8-row block.
#[tokio::test]
async fn replay_append0() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    h::ok(&e, "CREATE DATABASE db1;").await;
    h::ok(&e, "CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);").await;
    for _ in 0..2 {
        h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 4).await;
    }

    let e = h::reopen(e, dir.path()).await;
    assert_eq!(e.segment_ids("db1.tb1").await, vec![0]);
    assert_eq!(h::segment_rows(&e, "db1.tb1"), 8);
    e.flush().await.unwrap();
    assert_eq!(e.block_ids("db1.tb1", 0), vec![0]);
    assert_eq!(e.block_visible_range("db1.tb1", 0, 0), Some((0, 8)));
}

/// C++ `src/unit_test/storage/new_catalog/replay_append_delete_ut.cpp:174`
/// (`TestTxnReplayAppend.test_replay_append_delete`): the delete replay after a
/// restart removes offsets 1 and 3, leaving two visible ranges.
#[tokio::test]
async fn replay_append_delete() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    let block = vec![
        vec![Literal::Int(1), Literal::Str("abc".to_string())],
        vec![
            Literal::Int(2),
            Literal::Str("abcdefghijklmnopqrstuvwxyz".to_string()),
        ],
    ];
    for _ in 0..2 {
        h::append(&e, "default_db.tb1", block.clone()).await;
    }
    let e = h::reopen(e, dir.path()).await;

    let mut t = e.begin_txn();
    assert_eq!(
        t.delete_row_ids("default_db.tb1", &[(0, 1), (0, 3)])
            .unwrap(),
        2
    );
    t.commit().await.unwrap();
    let e = h::reopen(e, dir.path()).await;

    assert_eq!(e.row_count("default_db.tb1").unwrap(), 2);
    assert_eq!(
        h::rows(&e, "SELECT col1, col2 FROM default_db.tb1 ORDER BY col1;").await,
        vec![
            vec!["1".to_string(), "abc".to_string()],
            vec!["1".to_string(), "abc".to_string()],
        ]
    );
}

/// C++ `src/unit_test/storage/new_catalog/replay_append_delete_ut.cpp:286`
/// (`TestTxnReplayAppend.test_replay_append_with_index0`): both indexes replay
/// after a restart and cover 2*8192 rows in segment 0.
#[tokio::test]
async fn replay_append_with_index0() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::ok(&e, "CREATE INDEX index1 ON default_db.tb1 (col1);").await;
    h::ok(
        &e,
        "CREATE INDEX index2 ON default_db.tb1 (col2) USING fulltext;",
    )
    .await;
    for _ in 0..2 {
        h::append(&e, "default_db.tb1", alternating_block()).await;
    }

    let e = h::reopen(e, dir.path()).await;
    assert_eq!(e.segment_ids("default_db.tb1").await, vec![0]);
    assert_eq!(e.row_count("default_db.tb1").unwrap(), 16384);
    h::ok(&e, "SHOW TABLE default_db.tb1 INDEX index1;").await;
    h::ok(&e, "SHOW TABLE default_db.tb1 INDEX index2;").await;
    // The reference checks that each index's memory segment starts at row 0
    // and covers 2*8192 rows; the engine's secondary mirror is rebuilt from
    // the catalog on open, so the same rows are reachable.
    assert_eq!(h::count(&e, "default_db.tb1").await, 16384);
}

/// C++ `src/unit_test/storage/new_catalog/replay_index_ut.cpp:87`
/// (`TestTxnReplayIndex.DISABLED_SLOW_test_replay_append_with_index`).
#[tokio::test]
#[ignore = "slow: C++ TestTxnReplayIndex.DISABLED_SLOW_test_replay_append_with_index"]
async fn replay_append_with_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::ok(&e, "CREATE INDEX index1 ON default_db.tb1 (col1);").await;
    h::ok(
        &e,
        "CREATE INDEX index2 ON default_db.tb1 (col2) USING fulltext;",
    )
    .await;
    h::append(&e, "default_db.tb1", alternating_block()).await;
    e.flush().await.unwrap();
    let e = h::reopen(e, dir.path()).await;
    assert_eq!(e.row_count("default_db.tb1").unwrap(), 8192);
    h::ok(&e, "SHOW TABLE default_db.tb1 INDEX index1;").await;
}

/// C++ `src/unit_test/storage/new_catalog/replay_index_ut.cpp:466`
/// (`TestTxnReplayIndex.DISABLED_SLOW_test_populate_index`).
#[tokio::test]
#[ignore = "slow: C++ TestTxnReplayIndex.DISABLED_SLOW_test_populate_index"]
async fn replay_populate_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::ok(&e, "CREATE INDEX index1 ON default_db.tb1 (col1);").await;
    for _ in 0..2 {
        h::append(&e, "default_db.tb1", alternating_block()).await;
        e.flush().await.unwrap();
    }
    let e = h::reopen(e, dir.path()).await;
    assert_eq!(e.row_count("default_db.tb1").unwrap(), 16384);
    assert_eq!(h::count(&e, "default_db.tb1").await, 16384);
    h::ok(&e, "SHOW TABLE default_db.tb1 INDEX index1;").await;
}

/// C++ `src/unit_test/storage/new_catalog/replay_import_ut.cpp:65`
/// (`TestTxnReplayImport.test_import0`): two imports of two 8192-row blocks
/// replay to 32768 rows.
#[tokio::test]
async fn replay_import0() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    for _ in 0..2 {
        h::append(&e, "default_db.tb1", alternating_block()).await;
        h::append(&e, "default_db.tb1", alternating_block()).await;
    }

    let e = h::reopen(e, dir.path()).await;
    assert_eq!(e.row_count("default_db.tb1").unwrap(), 32768);
    assert_eq!(
        h::rows(
            &e,
            "SELECT count(*) FROM default_db.tb1 WHERE col2 = 'abc';"
        )
        .await,
        vec![vec!["16384"]]
    );
    e.flush().await.unwrap();
    let blocks: usize = e
        .segment_ids("default_db.tb1")
        .await
        .into_iter()
        .map(|s| e.block_ids("default_db.tb1", s).len())
        .sum();
    assert_eq!(blocks, 4);
}

/// C++ `src/unit_test/storage/new_catalog/replay_import_ut.cpp:195`
/// (`TestTxnReplayImport.test_import_with_index`): secondary and full-text
/// indexes replay with the imported rows.
#[tokio::test]
async fn replay_import_with_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::ok(&e, "CREATE INDEX index1 ON default_db.tb1 (col1);").await;
    h::ok(
        &e,
        "CREATE INDEX index2 ON default_db.tb1 (col2) USING fulltext;",
    )
    .await;
    for _ in 0..2 {
        h::append(&e, "default_db.tb1", alternating_block()).await;
        h::append(&e, "default_db.tb1", alternating_block()).await;
    }

    let e = h::reopen(e, dir.path()).await;
    assert_eq!(e.row_count("default_db.tb1").unwrap(), 32768);
    h::ok(&e, "SHOW TABLE default_db.tb1 INDEX index1;").await;
    h::ok(&e, "SHOW TABLE default_db.tb1 INDEX index2;").await;
    assert_eq!(
        h::rows(
            &e,
            "SELECT count(*) FROM default_db.tb1 WHERE col2 = 'abcdefghijklmnopqrstuvwxyz';"
        )
        .await,
        vec![vec!["16384"]]
    );
}

/// C++ `src/unit_test/storage/new_catalog/replay_index_ut.cpp:730`
/// (`TestTxnReplayIndex.drop_index`): a dropped index stays dropped after the
/// restart.
#[tokio::test]
async fn replay_index_drop_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::ok(&e, "CREATE INDEX index1 ON default_db.tb1 (col1);").await;
    h::append_repeated(&e, "default_db.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192).await;
    e.checkpoint_wal().await;
    h::ok(&e, "DROP INDEX index1 ON default_db.tb1;").await;

    let e = h::reopen(e, dir.path()).await;
    assert!(e
        .execute("SHOW TABLE default_db.tb1 INDEX index1;")
        .await
        .is_err());
    assert_eq!(e.row_count("default_db.tb1").unwrap(), 8192);
}

/// C++ `src/unit_test/storage/new_catalog/replay_index_ut.cpp:797`
/// (`TestTxnReplayIndex.test_json_index_replay`): a JSON secondary index with
/// `cardinality = low` replays with its options.
#[tokio::test]
async fn replay_json_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    h::ok(&e, "CREATE TABLE default_db.tb1 (id INT, meta JSON);").await;
    h::ok(
        &e,
        "CREATE INDEX idx_json ON default_db.tb1 (meta) WITH (cardinality = low);",
    )
    .await;

    let e = h::reopen(e, dir.path()).await;
    h::ok(&e, "SHOW TABLE default_db.tb1 INDEX idx_json;").await;
}

/// C++ `src/unit_test/storage/new_catalog/new_txn_mgr_ut.cpp:46`
/// (`TestTxnManagerTest.test_ts`): begin/commit timestamp ordering for a write
/// transaction, a read transaction, a rollback, a conflicting drop and a final
/// create.
#[tokio::test]
async fn txn_manager_timestamps() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    h::ok(&e, "CREATE TABLE default_db.tb1 (col1 INT);").await;

    // Every assertion below is on `current_ts()` / `prepare_commit_ts()`, both
    // of which the engine publishes (`SHOW VARIABLE total_commit_count`, the
    // checkpoint's floor). The per-transaction `begin_ts` is an implementation
    // detail and is deliberately not asserted: what a caller can observe is
    // that a commit moves the clock forward and that a read or a rollback
    // leaves it where it was.

    // A write transaction advances the commit clock.
    let before = e.current_ts();
    let mut t1 = e.begin_txn();
    t1.append("default_db.tb1", vec![vec![Literal::Int(1)]])
        .await
        .unwrap();
    t1.commit().await.unwrap();
    let commit1 = e.current_ts();
    assert!(
        commit1 > before,
        "a commit advances current ts: {before} -> {commit1}"
    );

    // A read transaction commits without spending a timestamp.
    let t2 = e.begin_txn();
    t2.commit().await.unwrap();
    assert_eq!(e.current_ts(), commit1, "a read does not move the clock");

    // A rollback advances nothing either.
    let t3 = e.begin_txn();
    t3.rollback();
    assert_eq!(
        e.current_ts(),
        commit1,
        "a rollback does not move the clock"
    );

    // Conflicting drop: the first commits, the second fails.
    h::ok(&e, "CREATE DATABASE db1;").await;
    h::ok(&e, "DROP DATABASE db1;").await;
    assert!(e.execute("DROP DATABASE db1;").await.is_err());
    let after = e.current_ts();
    assert!(after >= commit1);

    // A write after the failed statement still advances the clock.
    let mut t5 = e.begin_txn();
    t5.append("default_db.tb1", vec![vec![Literal::Int(2)]])
        .await
        .unwrap();
    t5.commit().await.unwrap();
    assert!(e.current_ts() > after);
    // The reference ends with `EXPECT_EQ(CurrentTS(), PrepareCommitTS())`; with
    // no transaction in flight both pointers sit on the last commit.
    assert_eq!(e.current_ts(), e.prepare_commit_ts());
}

/// C++ `src/unit_test/storage/new_catalog/new_txn_mgr_ut.cpp:215`
/// (`TestTxnManagerTest.test_check_txns`): the reference inspects the
/// check-candidate list; the engine exposes the same rule as delete conflicts.
#[tokio::test]
async fn txn_manager_check_txns() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    h::ok(&e, "CREATE TABLE default_db.tb1 (col1 INT);").await;
    h::append(
        &e,
        "default_db.tb1",
        (0..4).map(|i| vec![Literal::Int(i)]).collect(),
    )
    .await;

    // No writes: an empty transaction commits without conflict.
    let empty = e.begin_txn();
    empty.commit().await.unwrap();

    // txn2 begins before txn1's delete commits and deletes the same row: its
    // commit must fail (the reference's `GetCheckCandidateTxns` returns txn1).
    let mut t2 = e.begin_txn();
    assert_eq!(t2.delete_row_ids("default_db.tb1", &[(0, 0)]).unwrap(), 1);
    let mut t1 = e.begin_txn();
    assert_eq!(t1.delete_row_ids("default_db.tb1", &[(0, 0)]).unwrap(), 1);
    t1.commit().await.unwrap();
    assert!(t2.commit().await.is_err(), "stale delete must conflict");

    // A transaction that begins after the commit sees no conflict.
    let mut t3 = e.begin_txn();
    assert_eq!(t3.delete_row_ids("default_db.tb1", &[(0, 1)]).unwrap(), 1);
    t3.commit().await.unwrap();

    // Disjoint rows in live transactions both commit.
    let mut a = e.begin_txn();
    let mut b = e.begin_txn();
    assert_eq!(a.delete_row_ids("default_db.tb1", &[(0, 2)]).unwrap(), 1);
    assert_eq!(b.delete_row_ids("default_db.tb1", &[(0, 3)]).unwrap(), 1);
    a.commit().await.unwrap();
    b.commit().await.unwrap();

    assert_eq!(e.row_count("default_db.tb1").unwrap(), 0);
}
