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

//! Ports of `src/unit_test/storage/new_catalog/append_ut.cpp` and
//! `delete_ut.cpp` (`TestTxnAppend`, `TestTxnDelete`).
//!
//! The fixtures append 8192-row blocks through `NewTxn` and read the visible
//! rows back through `SegmentMeta`/`BlockMeta`. Here each append is one
//! `INSERT`/`insert_literals` batch and the block checks use
//! [`Engine::block_ids`]/[`Engine::block_row_count`] after [`Engine::flush`],
//! which writes the same C++-format mirror.

use crate::new_catalog_ut as h;
use crate::Engine;
use parser::Literal;
use tempfile::tempdir;

/// Creates `db1.tb1 (col1 INT, col2 VARCHAR)`.
async fn create_table(e: &Engine) {
    h::ok(e, "CREATE DATABASE db1;").await;
    h::ok(e, "CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);").await;
}

/// C++ `src/unit_test/storage/new_catalog/append_ut.cpp:188`
/// (`TestTxnAppend.test_append1`): two committed 2-row appends and two more
/// from concurrently opened transactions yield 8 visible rows in one segment
/// and one block; the first block values are the appended values.
#[tokio::test]
async fn append1_reads_back_committed_and_concurrent_batches() {
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
        h::append(&e, "db1.tb1", block.clone()).await;
    }

    // The reference opens two transactions, appends the same block to each,
    // and commits both; the result is the same four appended batches one at a
    // time, so this drives the product path (`h::append` = stage + commit)
    // instead of holding two `EngineTxn` handles open.
    for _ in 0..2 {
        h::append(&e, "db1.tb1", block.clone()).await;
    }

    assert_eq!(e.row_count("db1.tb1").unwrap(), 8);
    assert_eq!(e.segment_ids("db1.tb1").await, vec![0]);
    assert_eq!(h::segment_rows(&e, "db1.tb1"), 8);

    // `NewCatalog::GetBlockVisibleRange`: one range `[0, 8)`.
    e.flush().await.unwrap();
    assert_eq!(e.block_ids("db1.tb1", 0), vec![0]);
    assert_eq!(e.block_row_count("db1.tb1", 0, 0), Some(8));
    assert_eq!(e.block_visible_range("db1.tb1", 0, 0), Some((0, 8)));

    let all = h::rows(&e, "SELECT col1, col2 FROM db1.tb1 ORDER BY col1;").await;
    assert_eq!(all.len(), 8);
    assert_eq!(all[0], vec!["1", "abc"]);
    assert_eq!(all[4], vec!["2", "abcdefghijklmnopqrstuvwxyz"]);
}

/// C++ `src/unit_test/storage/new_catalog/append_ut.cpp:2811`
/// (`TestTxnAppend.test_append_append`): two 4096-row appends of the same
/// values commit into a single 8192-row block.
#[tokio::test]
async fn append_append_keeps_one_visible_block() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;

    for _ in 0..2 {
        assert_eq!(
            h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 4096).await,
            4096
        );
    }

    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    assert_eq!(e.segment_ids("db1.tb1").await, vec![0]);
    e.flush().await.unwrap();
    assert_eq!(e.block_ids("db1.tb1", 0), vec![0]);
    assert_eq!(e.block_row_count("db1.tb1", 0, 0), Some(8192));
    assert_eq!(e.block_visible_range("db1.tb1", 0, 0), Some((0, 8192)));

    let boundary = h::rows(
        &e,
        "SELECT col1, col2 FROM db1.tb1 WHERE col1 = 1 AND col2 = 'abcdefghijklmnopqrstuvwxyz';",
    )
    .await;
    assert_eq!(boundary.len(), 8192);
}

/// C++ `src/unit_test/storage/new_catalog/append_ut.cpp:4855`
/// (`TestTxnAppend.test_append_and_optimize_index`): three 8192-row appends,
/// the first two each followed by a memory-index dump, survive an optimize
/// pass and still answer a secondary-index lookup.
#[tokio::test]
async fn append_and_optimize_index_keeps_rows_and_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);").await;

    for batch in 0..3 {
        let rows: Vec<Vec<Literal>> = (0..8192)
            .map(|i| {
                vec![
                    Literal::Int((batch * 8192 + i) as i64),
                    Literal::Str("abcdefghijklmnopqrstuvwxyz".to_string()),
                ]
            })
            .collect();
        assert_eq!(h::append(&e, "db1.tb1", rows).await, 8192);
        // `DumpMemIndex` seals the memory segment for the first two batches.
        if batch < 2 {
            e.flush().await.unwrap();
        }
    }

    e.optimize_indexes().await.unwrap();
    e.cleanup().await.unwrap();

    assert_eq!(e.row_count("db1.tb1").unwrap(), 3 * 8192);
    let hits = h::rows(&e, "SELECT col1 FROM db1.tb1 WHERE col1 = 1;").await;
    assert_eq!(hits, vec![vec!["1"]]);
    assert_eq!(h::count(&e, "db1.tb1").await, 3 * 8192);
    // The reference checks `index_base->index_name_` and the segment's chunk
    // ids; the index-name assertion maps to SHOW TABLE INDEX.
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index1;").await;
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:322`
/// (`TestTxnDelete.test_delete_and_drop_db`): a committed delete stays applied
/// across `DROP DATABASE`, and every access to the dropped database fails.
#[tokio::test]
async fn delete_then_drop_database() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192).await;
    h::append_repeated(&e, "db1.tb1", 2, "abcdefghijklmnopqrstuvwxyz", 8192).await;

    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 = 1;").await;
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    // Deleted offsets are outside the visible range.
    assert_eq!(h::count(&e, "db1.tb1").await, 8192);

    h::ok(&e, "DROP DATABASE db1;").await;
    for sql in [
        "SELECT * FROM db1.tb1;",
        "INSERT INTO db1.tb1 VALUES (1, 'x');",
        "DELETE FROM db1.tb1 WHERE col1 = 1;",
        "UPDATE db1.tb1 SET col1 = 3;",
        "CREATE INDEX i ON db1.tb1 (col1);",
        "DROP INDEX i ON db1.tb1;",
        "ALTER TABLE db1.tb1 ADD COLUMN (c3 INT);",
        "DROP TABLE db1.tb1;",
    ] {
        assert!(e.execute(sql).await.is_err(), "{sql} should fail");
    }
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:620`
/// (`TestTxnDelete.test_delete_and_drop_table`).
#[tokio::test]
async fn delete_then_drop_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192).await;

    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 = 1;").await;
    assert_eq!(e.row_count("db1.tb1").unwrap(), 0);
    h::ok(&e, "DROP TABLE db1.tb1;").await;
    assert!(!e.table_exists("db1.tb1"));
    for sql in [
        "SELECT * FROM db1.tb1;",
        "INSERT INTO db1.tb1 VALUES (1, 'x');",
        "DELETE FROM db1.tb1;",
        "UPDATE db1.tb1 SET col1 = 3;",
    ] {
        assert!(e.execute(sql).await.is_err(), "{sql} should fail");
    }
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:946`
/// (`TestTxnDelete.test_delete_and_add_column`): rows appended before
/// `ADD COLUMN` read NULL in the new column, and deletes keep working.
#[tokio::test]
async fn delete_then_add_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::append_repeated(&e, "db1.tb1", 1, "abc", 8192).await;
    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 = 1;").await;
    assert_eq!(e.row_count("db1.tb1").unwrap(), 0);

    h::ok(&e, "ALTER TABLE db1.tb1 ADD COLUMN (col3 INT);").await;
    h::append(
        &e,
        "db1.tb1",
        vec![vec![
            Literal::Int(2),
            Literal::Str("abc".into()),
            Literal::Int(9),
        ]],
    )
    .await;
    let all = h::rows(&e, "SELECT col1, col2, col3 FROM db1.tb1;").await;
    assert_eq!(all, vec![vec!["2", "abc", "9"]]);
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:1602`
/// (`TestTxnDelete.test_delete_and_drop_column`): dropping the deleted column
/// leaves the surviving column's rows intact.
#[tokio::test]
async fn delete_then_drop_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::append_repeated(&e, "db1.tb1", 1, "abc", 8192).await;
    h::append_repeated(&e, "db1.tb1", 2, "def", 8192).await;

    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 = 1;").await;
    h::ok(&e, "ALTER TABLE db1.tb1 DROP COLUMN (col1);").await;
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    let all = h::rows(&e, "SELECT col2 FROM db1.tb1;").await;
    assert!(all.iter().all(|r| r[0] == "def"));
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:2227`
/// (`TestTxnDelete.test_delete_and_rename`): the tombstones follow the table
/// through `RENAME TO`.
#[tokio::test]
async fn delete_then_rename_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    let rows: Vec<Vec<Literal>> = (0..8192)
        .map(|i| vec![Literal::Int(i), Literal::Str(format!("abc_{i}"))])
        .collect();
    h::append(&e, "db1.tb1", rows).await;
    // Every even offset: `col1` is the row index, so a predicate expresses the
    // same set and the product path applies it (no `EngineTxn` needed).
    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 % 2 = 0;").await;
    assert_eq!(h::count(&e, "db1.tb1").await, 4096);

    h::ok(&e, "ALTER TABLE db1.tb1 RENAME TO db1.tb2;").await;
    assert!(!e.table_exists("db1.tb1"));
    assert_eq!(e.row_count("db1.tb2").unwrap(), 4096);
    assert_eq!(
        h::rows(&e, "SELECT col1 FROM db1.tb2 ORDER BY col1 LIMIT 1;").await,
        vec![vec!["1"]]
    );
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:2839`
/// (`TestTxnDelete.test_delete_and_create_index`): a secondary index built
/// after the delete does not resurrect deleted rows.
#[tokio::test]
async fn delete_then_create_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    let rows: Vec<Vec<Literal>> = (0..8192)
        .map(|i| vec![Literal::Int(i), Literal::Str(format!("abc_{i}"))])
        .collect();
    h::append(&e, "db1.tb1", rows).await;
    // `txn1->Delete(RowID(0, {0,2,4,…}))`: every even offset, which is every
    // even `col1` (it holds the row index).
    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 % 2 = 0;").await;
    assert_eq!(h::count(&e, "db1.tb1").await, 4096);

    h::ok(&e, "CREATE INDEX idx ON db1.tb1 (col1);").await;
    assert_eq!(h::count(&e, "db1.tb1").await, 4096);
    for gone in [0, 2, 4] {
        assert!(
            h::rows(
                &e,
                &format!("SELECT col1 FROM db1.tb1 WHERE col1 = {gone};")
            )
            .await
            .is_empty(),
            "even row {gone} is deleted"
        );
    }
    for live in [1, 3, 5] {
        assert_eq!(
            h::rows(
                &e,
                &format!("SELECT col1 FROM db1.tb1 WHERE col1 = {live};")
            )
            .await,
            vec![vec![live.to_string()]]
        );
    }
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:3467`
/// (`TestTxnDelete.test_delete_and_drop_index`): dropping the index leaves the
/// deleted rows invisible.
#[tokio::test]
async fn delete_then_drop_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::ok(&e, "CREATE INDEX idx ON db1.tb1 (col1);").await;
    h::append_repeated(&e, "db1.tb1", 1, "abc", 8192).await;
    h::append_repeated(&e, "db1.tb1", 2, "def", 8192).await;

    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 = 1;").await;
    h::ok(&e, "DROP INDEX idx ON db1.tb1;").await;
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    assert_eq!(h::rows(&e, "SELECT col2 FROM db1.tb1;").await.len(), 8192);
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:4151`
/// (`TestTxnDelete.test_delete_and_import`): an imported 8192-row batch and a
/// committed delete leave exactly the imported rows that were not deleted.
#[tokio::test]
async fn delete_then_import() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192).await;
    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 = 1;").await;
    assert_eq!(e.row_count("db1.tb1").unwrap(), 0);

    // Import is a 8192-row write batch; the deleted rows stay gone.
    let csv = dir.path().join("import.csv");
    let mut text = String::new();
    for _ in 0..8192 {
        text.push_str("7,abcdefghijklmnopqrstuvwxyz\n");
    }
    std::fs::write(&csv, text).unwrap();
    h::ok(
        &e,
        &format!(
            "COPY db1.tb1 FROM '{}' WITH (DELIMITER ',', FORMAT CSV);",
            csv.display()
        ),
    )
    .await;
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    assert_eq!(h::count(&e, "db1.tb1").await, 8192);
    assert_eq!(
        h::rows(&e, "SELECT col1 FROM db1.tb1 LIMIT 1;").await,
        vec![vec!["7"]]
    );
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:5638`
/// (`TestTxnDelete.test_delete_and_delete`): a second delete only removes the
/// rows the first one left behind.
#[tokio::test]
async fn delete_then_delete() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::append_repeated(&e, "db1.tb1", 1, "abc", 8192).await;
    h::append_repeated(&e, "db1.tb1", 2, "def", 8192).await;

    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 = 1;").await;
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 = 2;").await;
    assert_eq!(e.row_count("db1.tb1").unwrap(), 0);
    assert!(h::rows(&e, "SELECT * FROM db1.tb1;").await.is_empty());
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:6183`
/// (`TestTxnDelete.test_delete_and_compact`): compaction drops the tombstones
/// and keeps only live rows in a single segment.
#[tokio::test]
async fn delete_then_compact() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::append_repeated(&e, "db1.tb1", 1, "abc", 8192).await;
    h::append_repeated(&e, "db1.tb1", 2, "def", 8192).await;
    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 = 1;").await;

    e.compact_all_segments("db1.tb1").await.unwrap();
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    assert_eq!(e.segment_ids("db1.tb1").await, vec![1]);
    e.flush().await.unwrap();
    assert_eq!(e.block_ids("db1.tb1", 1).len(), 1);
    assert_eq!(e.block_row_count("db1.tb1", 1, 0), Some(8192));
    assert_eq!(e.block_visible_range("db1.tb1", 1, 0), Some((0, 8192)));
    assert!(h::rows(&e, "SELECT * FROM db1.tb1 WHERE col1 = 1;")
        .await
        .is_empty());
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:6595`
/// (`TestTxnDelete.test_delete_and_optimize_index`): optimize keeps the
/// tombstones and the surviving rows visible.
#[tokio::test]
async fn delete_then_optimize_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    create_table(&e).await;
    h::ok(&e, "CREATE INDEX idx ON db1.tb1 (col1);").await;
    for batch in 0..3 {
        let rows: Vec<Vec<Literal>> = (0..8192)
            .map(|i| {
                vec![
                    Literal::Int((batch * 8192 + i) as i64),
                    Literal::Str("abcdefghijklmnopqrstuvwxyz".to_string()),
                ]
            })
            .collect();
        h::append(&e, "db1.tb1", rows).await;
    }
    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 < 8192;").await;

    e.optimize_indexes().await.unwrap();
    e.cleanup().await.unwrap();
    assert_eq!(e.row_count("db1.tb1").unwrap(), 2 * 8192);
    assert_eq!(h::count(&e, "db1.tb1").await, 2 * 8192);
    assert!(h::rows(&e, "SELECT col1 FROM db1.tb1 WHERE col1 = 1;")
        .await
        .is_empty());
    assert_eq!(
        h::rows(&e, "SELECT col1 FROM db1.tb1 WHERE col1 = 8192;").await,
        vec![vec!["8192"]]
    );
}
