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

//! Engine-level port of `src/unit_test/storage/new_catalog/cleanup_ut.cpp`
//! (TEST_AUDIT rows 721-732).
//!
//! The reference runs twelve interleavings of a cleanup transaction
//! (`a`/`b`/`c`) with another transaction (`A`/`B`/`C`) and asserts that the
//! committed operation's visible state is unaffected by cleanup. The Rust
//! engine has no cleanup txn handle; `Engine::cleanup` is a synchronous
//! flush + garbage collection, so the port runs each of the twelve plans with
//! `cleanup()` at the `a`/`b`/`c` positions and the operation at `B`, then
//! asserts the same post-state. `cleanup()` must never change visible data.
//!
//! Segment-id mapping: the reference's `PreTest` uses two `Import` calls,
//! which allocate segments 0 and 1; the Rust `INSERT` keeps both one-row
//! batches in segment 0 (only `COPY` opens a new segment). Every reference
//! segment id is therefore one higher than the Rust id: `{2}` (after the
//! setup compaction) maps to `{1}`, `{3}` to `{2}`, `{4}` to `{3}`. All values
//! below are exact for the Rust engine.

use crate::{Engine, QueryResult};
use parser::Literal;
use tempfile::tempdir;

/// The twelve plans from `TestTxnCleanup::plans_`.
const PLANS: [&str; 12] = [
    "abAcBC", "abABcC", "abABCc", "aAbcBC", "aAbBcC", "aAbBCc", "aABbcC", "aABbCc", "aABCbc",
    "ABabcC", "ABabCc", "ABaCbc",
];

const TEXT: &str = "abcdefghijklmnopqrstuvwxyz";

async fn rows(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in engine.execute(sql).await.unwrap() {
        if let QueryResult::Rows { .. } = result {
            return result.rows_text();
        }
    }
    Vec::new()
}

async fn count(engine: &Engine, table: &str) -> usize {
    rows(engine, &format!("SELECT count(*) FROM {table};")).await[0][0]
        .parse()
        .unwrap()
}

async fn columns(engine: &Engine, table: &str) -> Vec<String> {
    rows(engine, &format!("SHOW TABLE {table} COLUMNS;"))
        .await
        .into_iter()
        .map(|row| row[0].clone())
        .collect()
}

async fn insert_one(engine: &Engine, table: &str, value: i64) {
    engine
        .insert_literals(
            table,
            None,
            vec![vec![Literal::Int(value), Literal::Str(TEXT.into())]],
        )
        .await
        .unwrap();
}

/// `TestTxnCleanup::PreTest`: db1/tb1, a secondary index, two one-row imports,
/// a merge and a checkpoint/cleanup.
async fn pre_test(engine: &Engine) {
    engine.execute("CREATE DATABASE db1;").await.unwrap();
    engine
        .execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .await
        .unwrap();
    engine
        .execute("CREATE INDEX index1 ON db1.tb1(col1);")
        .await
        .unwrap();
    insert_one(engine, "db1.tb1", 1).await;
    insert_one(engine, "db1.tb1", 1).await;
    engine.compact_all_segments("db1.tb1").await.unwrap();
    engine.cleanup().await.unwrap();
}

/// Runs every plan: `cleanup()` at `a`/`b`/`c`, `op` at `B`, then the check
/// and the reference's `PostTest`.
async fn run_plans<P, O, C>(pre: P, op: O, check: C)
where
    P: AsyncFn(&Engine),
    O: AsyncFn(&Engine),
    C: AsyncFn(&Engine),
{
    for plan in PLANS {
        let dir = tempdir().unwrap();
        let e = Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        pre(&e).await;
        let mut ran = false;
        for c in plan.chars() {
            match c {
                'B' => {
                    op(&e).await;
                    ran = true;
                }
                'a'..='c' => e.cleanup().await.unwrap(),
                // `A` (begin) and `C` (commit) have no visible state.
                _ => {}
            }
        }
        if !ran {
            op(&e).await;
        }
        e.cleanup().await.unwrap();
        check(&e).await;

        // `TestTxnCleanup::PostTest`.
        e.execute("DROP DATABASE IF EXISTS db1;").await.unwrap();
        e.cleanup().await.unwrap();
        assert!(!e.table_exists("db1.tb1"));
    }
}

/// C++ `cleanup_ut.cpp:289` (`cleanup_with_drop_db`).
#[tokio::test]
async fn cleanup_with_drop_db() {
    run_plans(
        pre_test,
        async |e| {
            e.execute("DROP DATABASE db1;").await.unwrap();
        },
        async |e| {
            assert!(!e.table_exists("db1.tb1"));
            assert!(!rows(e, "SHOW DATABASES;")
                .await
                .iter()
                .any(|r| r[0] == "db1"));
        },
    )
    .await;
}

/// C++ `cleanup_ut.cpp:318` (`cleanup_and_drop_table`).
#[tokio::test]
async fn cleanup_and_drop_table() {
    run_plans(
        pre_test,
        async |e| {
            e.execute("DROP TABLE db1.tb1;").await.unwrap();
        },
        async |e| {
            assert!(!e.table_exists("db1.tb1"));
        },
    )
    .await;
}

/// C++ `cleanup_ut.cpp:348` (`cleanup_and_add_columns`).
#[tokio::test]
async fn cleanup_and_add_columns() {
    run_plans(
        pre_test,
        async |e| {
            e.execute("ALTER TABLE db1.tb1 ADD COLUMN (col3 VARCHAR DEFAULT '');")
                .await
                .unwrap();
        },
        async |e| {
            assert_eq!(e.segment_ids("db1.tb1").await, vec![1]);
            let cols = columns(e, "db1.tb1").await;
            assert_eq!(cols, vec!["col1", "col2", "col3"]);
            let meta = e
                .catalog()
                .get_table_by_name("db1.tb1")
                .unwrap()
                .expect("tb1");
            assert_eq!(meta.ids, vec![0, 1, 2]);
            // Old rows read back the empty-string default.
            for row in rows(e, "SELECT col3 FROM db1.tb1;").await {
                assert_eq!(row, vec![String::new()]);
            }
        },
    )
    .await;
}

/// C++ `cleanup_ut.cpp:401` (`cleanup_and_drop_columns`).
#[tokio::test]
async fn cleanup_and_drop_columns() {
    run_plans(
        pre_test,
        async |e| {
            e.execute("ALTER TABLE db1.tb1 DROP COLUMN (col2);")
                .await
                .unwrap();
        },
        async |e| {
            assert_eq!(e.segment_ids("db1.tb1").await, vec![1]);
            assert_eq!(columns(e, "db1.tb1").await, vec!["col1"]);
            let meta = e
                .catalog()
                .get_table_by_name("db1.tb1")
                .unwrap()
                .expect("tb1");
            assert!(!meta.ids.contains(&1));
            assert_eq!(count(e, "db1.tb1").await, 2);
        },
    )
    .await;
}

/// C++ `cleanup_ut.cpp:446` (`cleanup_and_rename_table`).
#[tokio::test]
async fn cleanup_and_rename_table() {
    run_plans(
        pre_test,
        async |e| {
            e.execute("ALTER TABLE db1.tb1 RENAME TO db1.tb2;")
                .await
                .unwrap();
        },
        async |e| {
            assert!(!e.table_exists("db1.tb1"));
            assert_eq!(e.segment_ids("db1.tb2").await, vec![1]);
            assert_eq!(count(e, "db1.tb2").await, 2);
        },
    )
    .await;
}

/// C++ `cleanup_ut.cpp:484` (`cleanup_and_compact`).
#[tokio::test]
async fn cleanup_and_compact() {
    run_plans(
        pre_test,
        async |e| {
            e.compact_all_segments("db1.tb1").await.unwrap();
        },
        async |e| {
            assert_eq!(e.segment_ids("db1.tb1").await, vec![2]);
            assert_eq!(count(e, "db1.tb1").await, 2);
            assert_eq!(e.segments("db1.tb1"), vec![(2, 2)]);
        },
    )
    .await;
}

/// C++ `cleanup_ut.cpp:521` (`cleanup_and_drop_index`).
#[tokio::test]
async fn cleanup_and_drop_index() {
    run_plans(
        pre_test,
        async |e| {
            e.execute("DROP INDEX index1 ON db1.tb1;").await.unwrap();
        },
        async |e| {
            assert_eq!(e.segment_ids("db1.tb1").await, vec![1]);
            assert!(e.execute("SHOW TABLE db1.tb1 INDEX index1;").await.is_err());
        },
    )
    .await;
}

/// C++ `cleanup_ut.cpp:563` (`cleanup_and_optimize_index`).
///
/// The fixture's pre-test appends one row and dumps the index twice, then
/// optimizes. The secondary index has no Rust chunk bookkeeping (`GetChunkIDs1`
/// / `ChunkIndexMetaInfo`), so the port asserts the optimized table's segment,
/// row count and index visibility instead.
#[tokio::test]
async fn cleanup_and_optimize_index() {
    run_plans(
        async |e| {
            e.execute("CREATE DATABASE db1;").await.unwrap();
            e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
                .await
                .unwrap();
            e.execute("CREATE INDEX index1 ON db1.tb1(col1);")
                .await
                .unwrap();
            for _ in 0..2 {
                insert_one(e, "db1.tb1", 1).await;
                // `DumpMemIndex`.
                e.flush().await.unwrap();
            }
        },
        async |e| {
            e.optimize_indexes_async().await.unwrap();
        },
        async |e| {
            assert_eq!(e.segment_ids("db1.tb1").await, vec![0]);
            assert_eq!(count(e, "db1.tb1").await, 2);
            // Both rows still match the indexed predicate.
            assert_eq!(
                rows(e, "SELECT count(*) FROM db1.tb1 WHERE col1 = 1;").await,
                vec![vec!["2".to_string()]]
            );
            e.execute("SHOW TABLE db1.tb1 INDEX index1;").await.unwrap();
        },
    )
    .await;
}

/// C++ `cleanup_ut.cpp:654` (`cleanup_and_append`).
#[tokio::test]
async fn cleanup_and_append() {
    run_plans(
        pre_test,
        async |e| {
            insert_one(e, "db1.tb1", 1).await;
        },
        async |e| {
            assert_eq!(e.segment_ids("db1.tb1").await, vec![1, 2]);
            assert_eq!(e.segments("db1.tb1"), vec![(1, 2), (2, 1)]);
            assert_eq!(e.row_count("db1.tb1").unwrap(), 3);
            // The block mirror writes each segment into its own directory, so
            // after a checkpoint the three live rows sit in the two segments
            // that hold them.
            e.flush().await.unwrap();
            assert_eq!(e.block_ids("db1.tb1", 1), vec![0]);
            assert_eq!(e.block_row_count("db1.tb1", 1, 0), Some(2));
            assert_eq!(e.block_ids("db1.tb1", 2), vec![0]);
            assert_eq!(e.block_row_count("db1.tb1", 2, 0), Some(1));
        },
    )
    .await;
}

/// C++ `cleanup_ut.cpp:716` (`cleanup_and_delete`).
#[tokio::test]
async fn cleanup_and_delete() {
    run_plans(
        pre_test,
        async |e| {
            // `Delete(RowID(2, 0))`: the first row of the merged segment.
            let mut t = e.begin_txn();
            assert_eq!(t.delete_row_ids("db1.tb1", &[(1, 0)]).unwrap(), 1);
            t.commit().await.unwrap();
            e.compact_all_segments("db1.tb1").await.unwrap();
        },
        async |e| {
            assert_eq!(e.segment_ids("db1.tb1").await, vec![2]);
            assert_eq!(e.segments("db1.tb1"), vec![(2, 1)]);
            assert_eq!(e.row_count("db1.tb1").unwrap(), 1);
            e.flush().await.unwrap();
            assert_eq!(e.block_ids("db1.tb1", 2), vec![0]);
            assert_eq!(e.block_row_count("db1.tb1", 2, 0), Some(1));
        },
    )
    .await;
}

/// C++ `cleanup_ut.cpp:772` (`cleanup_and_update`).
///
/// `Update(RowID(2, 0))` rewrites one row; the port models it as a row-id
/// delete followed by an append of the new value, which is exactly what the
/// reference's update does internally (the new row lands in a new segment).
#[tokio::test]
async fn cleanup_and_update() {
    run_plans(
        pre_test,
        async |e| {
            let mut t = e.begin_txn();
            assert_eq!(t.delete_row_ids("db1.tb1", &[(1, 0)]).unwrap(), 1);
            t.commit().await.unwrap();
            insert_one(e, "db1.tb1", 2).await;
            e.compact_all_segments("db1.tb1").await.unwrap();
        },
        async |e| {
            assert_eq!(e.segment_ids("db1.tb1").await, vec![3]);
            assert_eq!(e.segments("db1.tb1"), vec![(3, 2)]);
            assert_eq!(e.row_count("db1.tb1").unwrap(), 2);
            assert_eq!(
                rows(e, "SELECT count(*) FROM db1.tb1 WHERE col1 = 1;").await,
                vec![vec!["1".to_string()]]
            );
            assert_eq!(
                rows(e, "SELECT count(*) FROM db1.tb1 WHERE col1 = 2;").await,
                vec![vec!["1".to_string()]]
            );
            e.flush().await.unwrap();
            assert_eq!(e.block_ids("db1.tb1", 3), vec![0]);
            assert_eq!(e.block_row_count("db1.tb1", 3, 0), Some(2));
        },
    )
    .await;
}

/// C++ `cleanup_ut.cpp:849` (`cleanup_and_dump_index`).
///
/// The fixture dumps the index for segment 1 and then reads the mem-index and
/// chunk metadata. The Rust engine has no per-segment mem-index for a
/// secondary index; `flush()` performs the dump and the visible segment/row
/// and index state is asserted exactly.
#[tokio::test]
async fn cleanup_and_dump_index() {
    run_plans(
        pre_test,
        async |e| {
            e.flush().await.unwrap();
        },
        async |e| {
            assert_eq!(e.segment_ids("db1.tb1").await, vec![1]);
            assert_eq!(count(e, "db1.tb1").await, 2);
            e.execute("SHOW TABLE db1.tb1 INDEX index1;").await.unwrap();
            // The dump keeps every indexed row queryable.
            assert_eq!(
                rows(e, "SELECT count(*) FROM db1.tb1 WHERE col1 = 1;").await,
                vec![vec!["2".to_string()]]
            );
        },
    )
    .await;
}
