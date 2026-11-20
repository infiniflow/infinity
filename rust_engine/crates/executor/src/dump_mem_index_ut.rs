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

//! Ports of `src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp`
//! (`TestTxnDumpMemIndex`).
//!
//! The reference drives `kDumpMemIndex` transactions concurrently with DDL and
//! reads `SegmentIndexMeta` / `ChunkIndexMeta`; a `DumpMemIndex` transaction
//! seals the memory index into the C++-format chunks (a checkpoint does not),
//! `EngineTxn::dump_index` is a staged `DumpMemIndex` transaction, and
//! `secondary_tail` / `secondary_chunks` read the same state the reference
//! inspects.
//!
//! A staged dump validates the table/index metadata it started from when it
//! commits, so a `DROP`/`RENAME`/`ALTER` that commits first makes the dump
//! fail. `Import` allocates a fresh segment, so `dump_and_import` observes
//! segment ids `{0,1}` like `NewTxn::Import`.

use crate::{Engine, QueryResult};
use parser::Literal;
use tempfile::tempdir;

async fn rows(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in engine.execute(sql).await.unwrap() {
        if let QueryResult::Rows { .. } = result {
            return result.rows_text();
        }
    }
    Vec::new()
}

/// `TableDef::Make(... {col1 INT, col3 EMBEDDING(FLOAT,4), col4 SPARSE})` plus
/// `IndexSecondary` on `col1` (`index1`).
async fn fixture(engine: &Engine) {
    engine.execute("CREATE DATABASE db1;").await.unwrap();
    engine
        .execute(
            "CREATE TABLE db1.tb1 (col1 INT, col3 EMBEDDING(FLOAT,4), \
             col4 SPARSE(FLOAT,10));",
        )
        .await
        .unwrap();
    engine
        .execute("CREATE INDEX index1 ON db1.tb1 (col1);")
        .await
        .unwrap();
}

/// One 8192-row `input_block`: `col1` alternates 1/2, `col3` is a constant
/// 4-dim vector and `col4` a constant sparse vector.
fn block() -> Vec<Vec<Literal>> {
    (0..8192)
        .map(|i| {
            vec![
                Literal::Int(if i % 2 == 0 { 1 } else { 2 }),
                Literal::Vector(vec![1.0, 2.0, 3.0, 4.0]),
                Literal::Array(vec!["1:1.0".to_string(), "2:2.0".to_string()]),
            ]
        })
        .collect()
}

async fn append_block(engine: &Engine) {
    engine
        .insert_literals("db1.tb1", None, block())
        .await
        .unwrap();
}

/// One `DumpMemIndex` transaction for `index1` — the reference's dump. A
/// checkpoint does not seal the memory tail, so every "dump" in this file goes
/// through the dump transaction, exactly like `NewTxn::DumpMemIndex`.
async fn dump(engine: &Engine) {
    engine
        .dump_index_transaction("db1.tb1", "index1")
        .await
        .unwrap();
}

/// `NewTxn::Import` for one 8192-row block: always opens a new segment.
async fn import_block(engine: &Engine) {
    engine
        .import_literals("db1.tb1", None, block())
        .await
        .unwrap();
}

async fn databases(engine: &Engine) -> Vec<String> {
    rows(engine, "SHOW DATABASES;")
        .await
        .into_iter()
        .map(|r| r[0].clone())
        .collect()
}

/// Whether `index` is still registered on `table` (`GetTableIndexMeta`).
fn has_index(engine: &Engine, table: &str, index: &str) -> bool {
    let table = engine.table_key(table);
    engine
        .storage
        .catalog
        .list_indexes()
        .unwrap()
        .iter()
        .any(|im| engine.table_key(&im.table) == table && im.name == index)
}

/// `check_index0`: the dump emptied the memory tail and sealed one chunk.
fn assert_dumped(engine: &Engine) {
    let state = engine.secondary_index_state("db1.tb1", "col1").unwrap();
    assert_eq!(state.tail, None, "the dump emptied the memory tail");
    assert_eq!(state.chunks, vec![(0, 8192)], "one sealed chunk");
}

/// `check_index`: the second block is the memory tail, starting at row 8192.
fn assert_second_block_in_tail(engine: &Engine) {
    let state = engine.secondary_index_state("db1.tb1", "col1").unwrap();
    assert_eq!(
        state.tail.map(|t| (t.begin_row_id, t.row_count)),
        Some((8192, 8192)),
        "the second block is the memory tail"
    );
}

/// C++: `dump_mem_index_ut.cpp:1151` (`dump_and_add_column`).
///
/// `ADD COLUMN` interleaved with the dump: in both orders the dumped chunk and
/// the memory tail keep their row ids, and the new column appears.
#[tokio::test]
async fn dump_and_add_column() {
    // dump then add column
    {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        fixture(&e).await;
        append_block(&e).await;
        dump(&e).await;
        assert_dumped(&e);
        append_block(&e).await;
        assert_second_block_in_tail(&e);

        e.execute("ALTER TABLE db1.tb1 ADD COLUMN (col5 VARCHAR DEFAULT 'default');")
            .await
            .unwrap();
        assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
        assert!(e.execute("SELECT col5 FROM db1.tb1 LIMIT 1;").await.is_ok());
        e.execute("DROP DATABASE db1;").await.unwrap();
    }

    // add column then dump
    {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        fixture(&e).await;
        append_block(&e).await;
        e.execute("ALTER TABLE db1.tb1 ADD COLUMN (col5 VARCHAR DEFAULT 'default');")
            .await
            .unwrap();
        append_block(&e).await;
        dump(&e).await;
        // Both blocks were in the memory tail, so one chunk covers them.
        assert_eq!(
            e.secondary_index_state("db1.tb1", "col1").unwrap().chunks,
            vec![(0, 16384)]
        );
        assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
        e.execute("DROP DATABASE db1;").await.unwrap();
    }
}

/// C++: `dump_mem_index_ut.cpp:1799` (`dump_and_drop_column`).
///
/// The embedding column is dropped (the index is on `col1`), then another block
/// is appended without it; the dumped chunk and the new tail cover the rows.
#[tokio::test]
async fn dump_and_drop_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    fixture(&e).await;
    append_block(&e).await;
    dump(&e).await;
    assert_dumped(&e);
    append_block(&e).await;
    assert_second_block_in_tail(&e);

    e.execute("ALTER TABLE db1.tb1 DROP COLUMN (col3);")
        .await
        .unwrap();
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    assert_eq!(
        rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col1 IN (1, 2);").await[0][0],
        "16384"
    );
    assert!(e
        .execute("SELECT col3 FROM db1.tb1 LIMIT 1;")
        .await
        .is_err());

    // `append_a_block_after_drop_column`: two columns, no embedding.
    let block: Vec<Vec<Literal>> = (0..8192)
        .map(|i| {
            vec![
                Literal::Int(if i % 2 == 0 { 1 } else { 2 }),
                Literal::Array(vec!["1:1.0".to_string()]),
            ]
        })
        .collect();
    e.insert_literals("db1.tb1", None, block).await.unwrap();
    assert_eq!(e.row_count("db1.tb1").unwrap(), 24576);
    e.execute("DROP DATABASE db1;").await.unwrap();
}

/// C++: `dump_mem_index_ut.cpp:2984` (`dump_and_create_index`).
///
/// `CREATE INDEX` with `ConflictType::kError` on the existing name fails; the
/// dump's chunk and the following memory tail are untouched.
#[tokio::test]
async fn dump_and_create_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    fixture(&e).await;
    append_block(&e).await;
    dump(&e).await;
    assert_dumped(&e);

    // Duplicate create without IF NOT EXISTS is `ConflictType::kError`.
    assert!(e
        .execute("CREATE INDEX index1 ON db1.tb1 (col1);")
        .await
        .is_err());

    append_block(&e).await;
    assert_second_block_in_tail(&e);
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    e.execute("DROP DATABASE db1;").await.unwrap();
}

/// C++: `dump_mem_index_ut.cpp:4566` (`dump_and_append`).
///
/// A dump between two appends seals only the first block; a dump after both
/// appends seals one 16384-row chunk.
#[tokio::test]
async fn dump_and_append() {
    {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        fixture(&e).await;
        append_block(&e).await;
        dump(&e).await;
        assert_dumped(&e);
        append_block(&e).await;
        assert_second_block_in_tail(&e);
        assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    }
    {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        fixture(&e).await;
        append_block(&e).await;
        append_block(&e).await;
        dump(&e).await;
        assert_eq!(
            e.secondary_index_state("db1.tb1", "col1").unwrap().chunks,
            vec![(0, 16384)]
        );
        assert_eq!(
            e.secondary_index_state("db1.tb1", "col1")
                .and_then(|s| s.tail),
            None
        );
    }
}

/// C++: `dump_mem_index_ut.cpp:5172` (`dump_and_delete`).
///
/// Every odd row is deleted after the dump; the chunk keeps its 8192 rows (the
/// reference does not rewrite chunks on delete) and only the even rows remain
/// visible.
#[tokio::test]
async fn dump_and_delete() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    fixture(&e).await;
    append_block(&e).await;
    dump(&e).await;
    assert_dumped(&e);

    // Every odd row id is the `col1 = 2` half of the block, so the predicate
    // names the same set through the product path.
    e.execute("DELETE FROM db1.tb1 WHERE col1 = 2;")
        .await
        .unwrap();
    assert_eq!(e.row_count("db1.tb1").unwrap(), 4096);

    // `check_index0`: the dump's memory tail stays empty.
    assert_eq!(
        e.secondary_index_state("db1.tb1", "col1")
            .and_then(|s| s.tail),
        None
    );
    assert_eq!(
        e.secondary_index_state("db1.tb1", "col1").unwrap().chunks,
        vec![(0, 8192)]
    );

    // `check_data`: the surviving rows are 0,2,4,... and every one has col1=1.
    assert_eq!(e.row_count("db1.tb1").unwrap(), 4096);
    let col1: Vec<Vec<String>> = rows(&e, "SELECT col1 FROM db1.tb1;").await;
    assert_eq!(col1.len(), 4096);
    assert!(col1.iter().all(|r| r[0] == "1"));
    e.execute("DROP DATABASE db1;").await.unwrap();
}

/// C++: `dump_mem_index_ut.cpp:5767` (`DISABLED_SLOW_dump_and_dump`).
///
/// Repeated dump/append rounds: each dump seals the tail and the next append
/// starts at the following row id.
#[tokio::test]
#[ignore = "slow: the reference repeats the dump/append round many times"]
async fn dump_and_dump() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    fixture(&e).await;

    append_block(&e).await;
    dump(&e).await;
    assert_dumped(&e);

    append_block(&e).await;
    dump(&e).await;
    assert_eq!(
        e.secondary_index_state("db1.tb1", "col1").unwrap().chunks,
        vec![(0, 8192), (0, 8192)]
    );
    assert_eq!(
        e.secondary_index_state("db1.tb1", "col1")
            .and_then(|s| s.tail),
        None
    );

    append_block(&e).await;
    // The third block starts where the second one ended.
    assert_eq!(
        e.secondary_index_state("db1.tb1", "col1")
            .and_then(|s| s.tail)
            .map(|t| (t.begin_row_id, t.row_count)),
        Some((16384, 8192))
    );
    assert_eq!(e.row_count("db1.tb1").unwrap(), 24576);
    e.execute("DROP DATABASE db1;").await.unwrap();
}

/// C++: `dump_mem_index_ut.cpp:67` (`dump_and_drop_db`).
///
/// The dump either commits before `DROP DATABASE` (success) or is still
/// staged when the drop commits (commit fails). A dump that starts after the
/// drop cannot find the table/index.
#[tokio::test]
async fn dump_and_drop_db() {
    // dump commits, then drop db
    {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        fixture(&e).await;
        append_block(&e).await;

        dump(&e).await;
        assert_dumped(&e);
        assert_eq!(e.segment_ids("db1.tb1").await, vec![0]);

        e.execute("DROP DATABASE db1;").await.unwrap();
        assert!(!databases(&e).await.iter().any(|d| d == "db1"));
        assert!(!e.table_exists("db1.tb1"));
    }

    // dump staged, drop db commits first -> dump commit fails
    {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        fixture(&e).await;
        append_block(&e).await;

        let mut txn = e.begin_txn();
        txn.dump_index("db1.tb1", "index1").unwrap();
        e.execute("DROP DATABASE db1;").await.unwrap();
        assert!(
            txn.commit().await.is_err(),
            "dump committed after the db drop"
        );
        assert!(!databases(&e).await.iter().any(|d| d == "db1"));
        assert!(!e.table_exists("db1.tb1"));
    }

    // drop db committed before DumpMemIndex -> it reports the table missing
    {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        fixture(&e).await;
        append_block(&e).await;
        e.execute("DROP DATABASE db1;").await.unwrap();

        assert!(
            e.dump_index_transaction("db1.tb1", "index1").await.is_err(),
            "the table is already gone"
        );
    }
}

/// C++: `dump_mem_index_ut.cpp:596` (`dump_and_drop_table`).
#[tokio::test]
async fn dump_and_drop_table() {
    // dump commits, then drop table
    {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        fixture(&e).await;
        append_block(&e).await;

        dump(&e).await;
        assert_dumped(&e);

        e.execute("DROP TABLE db1.tb1;").await.unwrap();
        assert!(!e.table_exists("db1.tb1"));
        assert!(!has_index(&e, "db1.tb1", "index1"));
        assert!(databases(&e).await.iter().any(|d| d == "db1"));
    }

    // dump staged, drop table commits first -> dump commit fails
    {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        fixture(&e).await;
        append_block(&e).await;

        let mut txn = e.begin_txn();
        txn.dump_index("db1.tb1", "index1").unwrap();
        e.execute("DROP TABLE db1.tb1;").await.unwrap();
        assert!(
            txn.commit().await.is_err(),
            "dump committed after the table drop"
        );
        assert!(!e.table_exists("db1.tb1"));
        assert!(!has_index(&e, "db1.tb1", "index1"));
    }

    // drop table committed before DumpMemIndex
    {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        fixture(&e).await;
        append_block(&e).await;
        e.execute("DROP TABLE db1.tb1;").await.unwrap();

        assert!(
            e.dump_index_transaction("db1.tb1", "index1").await.is_err(),
            "the table is already gone"
        );
    }
}

/// C++: `dump_mem_index_ut.cpp:2436` (`dump_and_rename_table`).
///
/// A dump of the old name cannot commit once `RENAME` committed; a dump of
/// the renamed table still works.
#[tokio::test]
async fn dump_and_rename_table() {
    // dump commits, then rename
    {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        fixture(&e).await;
        append_block(&e).await;

        dump(&e).await;
        assert_dumped(&e);

        e.execute("ALTER TABLE db1.tb1 RENAME TO db1.tb2;")
            .await
            .unwrap();
        assert!(!e.table_exists("db1.tb1"));
        assert!(e.table_exists("db1.tb2"));
        assert!(has_index(&e, "db1.tb2", "index1"));
        assert_eq!(e.segment_ids("db1.tb2").await, vec![0]);
        assert_eq!(e.row_count("db1.tb2").unwrap(), 8192);
    }

    // dump staged, rename commits first -> dump commit fails
    {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        fixture(&e).await;
        append_block(&e).await;

        let mut txn = e.begin_txn();
        txn.dump_index("db1.tb1", "index1").unwrap();
        e.execute("ALTER TABLE db1.tb1 RENAME TO db1.tb2;")
            .await
            .unwrap();
        assert!(
            txn.commit().await.is_err(),
            "dump committed after the rename"
        );
        assert!(!e.table_exists("db1.tb1"));
        assert!(e.table_exists("db1.tb2"));
        assert_eq!(e.row_count("db1.tb2").unwrap(), 8192);
    }

    // rename committed before DumpMemIndex: the old name is gone.
    {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        fixture(&e).await;
        append_block(&e).await;
        e.execute("ALTER TABLE db1.tb1 RENAME TO db1.tb2;")
            .await
            .unwrap();

        // The old name is gone and the new one still dumps; two product calls
        // express the same thing without holding a transaction open.
        assert!(
            e.dump_index_transaction("db1.tb1", "index1").await.is_err(),
            "the old name is gone"
        );
        e.dump_index_transaction("db1.tb2", "index1").await.unwrap();
        assert_eq!(
            e.sorted_secondary_chunks("db1.tb2", "col1"),
            vec![(0, 8192)]
        );
        assert_eq!(
            e.secondary_index_state("db1.tb2", "col1")
                .and_then(|s| s.tail),
            None
        );
    }
}

/// C++: `dump_mem_index_ut.cpp:3267` (`dump_and_drop_index`).
#[tokio::test]
async fn dump_and_drop_index() {
    // dump commits, then drop index
    {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        fixture(&e).await;
        append_block(&e).await;

        dump(&e).await;
        assert_dumped(&e);

        e.execute("DROP INDEX index1 ON db1.tb1;").await.unwrap();
        assert!(!has_index(&e, "db1.tb1", "index1"));
        assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    }

    // dump staged, drop index commits first -> dump commit fails
    {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        fixture(&e).await;
        append_block(&e).await;

        let mut txn = e.begin_txn();
        txn.dump_index("db1.tb1", "index1").unwrap();
        e.execute("DROP INDEX index1 ON db1.tb1;").await.unwrap();
        assert!(
            txn.commit().await.is_err(),
            "dump committed after the index drop"
        );
        assert!(!has_index(&e, "db1.tb1", "index1"));
        assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    }

    // drop index committed before DumpMemIndex
    {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        fixture(&e).await;
        append_block(&e).await;
        e.execute("DROP INDEX index1 ON db1.tb1;").await.unwrap();

        assert!(
            e.dump_index_transaction("db1.tb1", "index1").await.is_err(),
            "the table is already gone"
        );
    }
}

/// C++: `dump_mem_index_ut.cpp:3815` (`dump_and_import`).
///
/// `Import` opens a new segment, so `GetSegmentIDs1()` is `{0,1}` and the
/// memory tail/chunks are per segment. Import is not DDL: a dump staged before
/// the import still commits.
#[tokio::test]
async fn dump_and_import() {
    // dump commits, append, then import: segment 1 appears.
    {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        fixture(&e).await;
        append_block(&e).await;

        dump(&e).await;
        assert_dumped(&e);

        append_block(&e).await;
        assert_second_block_in_tail(&e);

        import_block(&e).await;
        assert_eq!(e.segment_ids("db1.tb1").await, vec![0, 1]);
        assert_eq!(
            e.secondary_tail_in_segment("db1.tb1", "col1", 0),
            Some((8192, 8192))
        );
        assert_eq!(
            e.secondary_tail_in_segment("db1.tb1", "col1", 1),
            Some((1 << 32, 8192))
        );
        assert_eq!(
            e.sorted_secondary_chunks("db1.tb1", "col1"),
            vec![(0, 8192)]
        );
        assert_eq!(e.row_count("db1.tb1").unwrap(), 24576);
    }

    // dump staged, import commits first -> the dump still commits.
    {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        fixture(&e).await;
        append_block(&e).await;

        let mut txn = e.begin_txn();
        txn.dump_index("db1.tb1", "index1").unwrap();
        import_block(&e).await;
        txn.commit().await.unwrap();

        assert_eq!(e.segment_ids("db1.tb1").await, vec![0, 1]);
        assert_eq!(
            e.sorted_secondary_chunks("db1.tb1", "col1"),
            vec![(0, 8192), (1 << 32, 8192)]
        );
        assert_eq!(
            e.secondary_index_state("db1.tb1", "col1")
                .and_then(|s| s.tail),
            None
        );
        assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    }

    // import, then dump.
    {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        fixture(&e).await;
        append_block(&e).await;
        import_block(&e).await;
        dump(&e).await;

        assert_eq!(e.segment_ids("db1.tb1").await, vec![0, 1]);
        assert_eq!(
            e.sorted_secondary_chunks("db1.tb1", "col1"),
            vec![(0, 8192), (1 << 32, 8192)]
        );
        assert_eq!(
            e.secondary_index_state("db1.tb1", "col1")
                .and_then(|s| s.tail),
            None
        );
        assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    }
}

/// C++: `dump_mem_index_ut.cpp:6260`
/// (`DISABLED_SLOW_test_dump_index_and_optimize_index`).
#[tokio::test]
#[ignore = "slow: C++ TestTxnDumpMemIndex.DISABLED_SLOW_dump_and_dump"]
async fn dump_index_and_optimize_index() {
    async fn prepare(e: &Engine) {
        e.execute("CREATE DATABASE db1;").await.unwrap();
        e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
            .await
            .unwrap();
        e.execute("CREATE INDEX index1 ON db1.tb1 (col1);")
            .await
            .unwrap();
        for _ in 0..2 {
            let batch: Vec<Vec<Literal>> = (0..8192)
                .map(|_| {
                    vec![
                        Literal::Int(1),
                        Literal::Str("abcdefghijklmnopqrstuvwxyz".into()),
                    ]
                })
                .collect();
            e.insert_literals("db1.tb1", None, batch).await.unwrap();
            dump(e).await;
        }
        let batch: Vec<Vec<Literal>> = (0..8192)
            .map(|_| {
                vec![
                    Literal::Int(1),
                    Literal::Str("abcdefghijklmnopqrstuvwxyz".into()),
                ]
            })
            .collect();
        e.insert_literals("db1.tb1", None, batch).await.unwrap();
    }

    // dump commits, then optimize commits.
    {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        prepare(&e).await;
        let mut dump = e.begin_txn();
        dump.dump_index("db1.tb1", "index1").unwrap();
        dump.commit().await.unwrap();
        e.optimize_indexes().await.unwrap();
        e.execute("DROP DATABASE db1;").await.unwrap();
    }

    // dump staged, optimize commits first -> dump commit fails.
    {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        prepare(&e).await;
        let mut dump = e.begin_txn();
        dump.dump_index("db1.tb1", "index1").unwrap();
        e.optimize_indexes().await.unwrap();
        assert!(
            dump.commit().await.is_err(),
            "dump committed after the optimize"
        );
        e.execute("DROP DATABASE db1;").await.unwrap();
    }
}

/// C++: `dump_mem_index_ut.cpp:6622` (`test_dump_index_and_compact`).
///
/// A dump followed by a segment compaction keeps every row; the merged segment
/// is the only one left.
#[tokio::test]
async fn test_dump_index_and_compact() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    fixture(&e).await;
    append_block(&e).await;
    dump(&e).await;
    assert_dumped(&e);

    e.compact_all_segments("db1.tb1").await.unwrap();

    assert_eq!(e.segment_count("db1.tb1"), 1);
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    let col1: Vec<Vec<String>> = rows(&e, "SELECT col1 FROM db1.tb1;").await;
    assert_eq!(col1.iter().filter(|r| r[0] == "1").count(), 4096);
    assert_eq!(col1.iter().filter(|r| r[0] == "2").count(), 4096);
    e.execute("DROP DATABASE db1;").await.unwrap();
}
