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
use infinity_parser::Literal;
use tempfile::tempdir;

/// The twelve plans from `TestTxnCleanup::plans_`.
const PLANS: [&str; 12] = [
    "abAcBC", "abABcC", "abABCc", "aAbcBC", "aAbBcC", "aAbBCc", "aABbcC", "aABbCc", "aABCbc",
    "ABabcC", "ABabCc", "ABaCbc",
];

const TEXT: &str = "abcdefghijklmnopqrstuvwxyz";

fn rows(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in engine.execute(sql).unwrap() {
        if let QueryResult::Rows { rows, .. } = result {
            return rows;
        }
    }
    Vec::new()
}

fn count(engine: &Engine, table: &str) -> usize {
    rows(engine, &format!("SELECT count(*) FROM {table};"))[0][0]
        .parse()
        .unwrap()
}

fn columns(engine: &Engine, table: &str) -> Vec<String> {
    rows(engine, &format!("SHOW TABLE {table} COLUMNS;"))
        .into_iter()
        .map(|row| row[0].clone())
        .collect()
}

fn insert_one(engine: &Engine, table: &str, value: i64) {
    engine
        .insert_literals(
            table,
            None,
            vec![vec![Literal::Int(value), Literal::Str(TEXT.into())]],
        )
        .unwrap();
}

/// `TestTxnCleanup::PreTest`: db1/tb1, a secondary index, two one-row imports,
/// a merge and a checkpoint/cleanup.
fn pre_test(engine: &Engine) {
    engine.execute("CREATE DATABASE db1;").unwrap();
    engine
        .execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    engine
        .execute("CREATE INDEX index1 ON db1.tb1(col1);")
        .unwrap();
    insert_one(engine, "db1.tb1", 1);
    insert_one(engine, "db1.tb1", 1);
    engine.compact_table("db1.tb1").unwrap();
    engine.cleanup().unwrap();
}

/// Runs every plan: `cleanup()` at `a`/`b`/`c`, `op` at `B`, then the check
/// and the reference's `PostTest`.
fn run_plans(pre: impl Fn(&Engine), op: impl Fn(&Engine), check: impl Fn(&Engine)) {
    for plan in PLANS {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        pre(&e);
        let mut ran = false;
        for c in plan.chars() {
            match c {
                'B' => {
                    op(&e);
                    ran = true;
                }
                'a' | 'b' | 'c' => e.cleanup().unwrap(),
                // `A` (begin) and `C` (commit) have no visible state.
                _ => {}
            }
        }
        if !ran {
            op(&e);
        }
        e.cleanup().unwrap();
        check(&e);

        // `TestTxnCleanup::PostTest`.
        e.execute("DROP DATABASE IF EXISTS db1;").unwrap();
        e.cleanup().unwrap();
        assert!(!e.table_exists("db1.tb1"));
    }
}

/// C++ `cleanup_ut.cpp:289` (`cleanup_with_drop_db`).
#[test]
fn cleanup_with_drop_db() {
    run_plans(
        pre_test,
        |e| {
            e.execute("DROP DATABASE db1;").unwrap();
        },
        |e| {
            assert!(!e.table_exists("db1.tb1"));
            assert!(!rows(e, "SHOW DATABASES;").iter().any(|r| r[0] == "db1"));
        },
    );
}

/// C++ `cleanup_ut.cpp:318` (`cleanup_and_drop_table`).
#[test]
fn cleanup_and_drop_table() {
    run_plans(
        pre_test,
        |e| {
            e.execute("DROP TABLE db1.tb1;").unwrap();
        },
        |e| {
            assert!(!e.table_exists("db1.tb1"));
        },
    );
}

/// C++ `cleanup_ut.cpp:348` (`cleanup_and_add_columns`).
#[test]
fn cleanup_and_add_columns() {
    run_plans(
        pre_test,
        |e| {
            e.execute("ALTER TABLE db1.tb1 ADD COLUMN (col3 VARCHAR DEFAULT '');")
                .unwrap();
        },
        |e| {
            assert_eq!(e.segment_ids("db1.tb1"), vec![1]);
            let cols = columns(e, "db1.tb1");
            assert_eq!(cols, vec!["col1", "col2", "col3"]);
            let meta = e
                .catalog()
                .get_table_by_name("db1.tb1")
                .unwrap()
                .expect("tb1");
            assert_eq!(meta.ids, vec![0, 1, 2]);
            // Old rows read back the empty-string default.
            for row in rows(e, "SELECT col3 FROM db1.tb1;") {
                assert_eq!(row, vec![String::new()]);
            }
        },
    );
}

/// C++ `cleanup_ut.cpp:401` (`cleanup_and_drop_columns`).
#[test]
fn cleanup_and_drop_columns() {
    run_plans(
        pre_test,
        |e| {
            e.execute("ALTER TABLE db1.tb1 DROP COLUMN (col2);")
                .unwrap();
        },
        |e| {
            assert_eq!(e.segment_ids("db1.tb1"), vec![1]);
            assert_eq!(columns(e, "db1.tb1"), vec!["col1"]);
            let meta = e
                .catalog()
                .get_table_by_name("db1.tb1")
                .unwrap()
                .expect("tb1");
            assert!(!meta.ids.contains(&1));
            assert_eq!(count(e, "db1.tb1"), 2);
        },
    );
}

/// C++ `cleanup_ut.cpp:446` (`cleanup_and_rename_table`).
#[test]
fn cleanup_and_rename_table() {
    run_plans(
        pre_test,
        |e| {
            e.execute("ALTER TABLE db1.tb1 RENAME TO db1.tb2;").unwrap();
        },
        |e| {
            assert!(!e.table_exists("db1.tb1"));
            assert_eq!(e.segment_ids("db1.tb2"), vec![1]);
            assert_eq!(count(e, "db1.tb2"), 2);
        },
    );
}

/// C++ `cleanup_ut.cpp:484` (`cleanup_and_compact`).
#[test]
fn cleanup_and_compact() {
    run_plans(
        pre_test,
        |e| {
            e.compact_table("db1.tb1").unwrap();
        },
        |e| {
            assert_eq!(e.segment_ids("db1.tb1"), vec![2]);
            assert_eq!(count(e, "db1.tb1"), 2);
            assert_eq!(e.segments("db1.tb1"), vec![(2, 2)]);
        },
    );
}

/// C++ `cleanup_ut.cpp:521` (`cleanup_and_drop_index`).
#[test]
fn cleanup_and_drop_index() {
    run_plans(
        pre_test,
        |e| {
            e.execute("DROP INDEX index1 ON db1.tb1;").unwrap();
        },
        |e| {
            assert_eq!(e.segment_ids("db1.tb1"), vec![1]);
            assert!(e.execute("SHOW TABLE db1.tb1 INDEX index1;").is_err());
        },
    );
}

/// C++ `cleanup_ut.cpp:563` (`cleanup_and_optimize_index`).
///
/// The fixture's pre-test appends one row and dumps the index twice, then
/// optimizes. The secondary index has no Rust chunk bookkeeping (`GetChunkIDs1`
/// / `ChunkIndexMetaInfo`), so the port asserts the optimized table's segment,
/// row count and index visibility instead.
#[test]
fn cleanup_and_optimize_index() {
    run_plans(
        |e| {
            e.execute("CREATE DATABASE db1;").unwrap();
            e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
                .unwrap();
            e.execute("CREATE INDEX index1 ON db1.tb1(col1);").unwrap();
            for _ in 0..2 {
                insert_one(e, "db1.tb1", 1);
                // `DumpMemIndex`.
                e.flush().unwrap();
            }
        },
        |e| {
            e.optimize_indexes().unwrap();
        },
        |e| {
            assert_eq!(e.segment_ids("db1.tb1"), vec![0]);
            assert_eq!(count(e, "db1.tb1"), 2);
            // Both rows still match the indexed predicate.
            assert_eq!(
                rows(e, "SELECT count(*) FROM db1.tb1 WHERE col1 = 1;"),
                vec![vec!["2".to_string()]]
            );
            e.execute("SHOW TABLE db1.tb1 INDEX index1;").unwrap();
        },
    );
}

/// C++ `cleanup_ut.cpp:654` (`cleanup_and_append`).
#[test]
fn cleanup_and_append() {
    run_plans(
        pre_test,
        |e| {
            insert_one(e, "db1.tb1", 1);
        },
        |e| {
            assert_eq!(e.segment_ids("db1.tb1"), vec![1, 2]);
            assert_eq!(e.segments("db1.tb1"), vec![(1, 2), (2, 1)]);
            assert_eq!(e.row_count("db1.tb1").unwrap(), 3);
            // The Rust block mirror stores every segment in segment 0; after a
            // checkpoint it holds the three live rows in one block.
            e.flush().unwrap();
            assert_eq!(e.block_ids("db1.tb1", 0), vec![0]);
            assert_eq!(e.block_row_count("db1.tb1", 0, 0), Some(3));
        },
    );
}

/// C++ `cleanup_ut.cpp:716` (`cleanup_and_delete`).
#[test]
fn cleanup_and_delete() {
    run_plans(
        pre_test,
        |e| {
            // `Delete(RowID(2, 0))`: the first row of the merged segment.
            let mut t = e.begin_txn();
            assert_eq!(t.delete_row_ids("db1.tb1", &[(1, 0)]).unwrap(), 1);
            t.commit().unwrap();
            e.compact_table("db1.tb1").unwrap();
        },
        |e| {
            assert_eq!(e.segment_ids("db1.tb1"), vec![2]);
            assert_eq!(e.segments("db1.tb1"), vec![(2, 1)]);
            assert_eq!(e.row_count("db1.tb1").unwrap(), 1);
            e.flush().unwrap();
            assert_eq!(e.block_ids("db1.tb1", 0), vec![0]);
            assert_eq!(e.block_row_count("db1.tb1", 0, 0), Some(1));
        },
    );
}

/// C++ `cleanup_ut.cpp:772` (`cleanup_and_update`).
///
/// `Update(RowID(2, 0))` rewrites one row; the port models it as a row-id
/// delete followed by an append of the new value, which is exactly what the
/// reference's update does internally (the new row lands in a new segment).
#[test]
fn cleanup_and_update() {
    run_plans(
        pre_test,
        |e| {
            let mut t = e.begin_txn();
            assert_eq!(t.delete_row_ids("db1.tb1", &[(1, 0)]).unwrap(), 1);
            t.commit().unwrap();
            insert_one(e, "db1.tb1", 2);
            e.compact_table("db1.tb1").unwrap();
        },
        |e| {
            assert_eq!(e.segment_ids("db1.tb1"), vec![3]);
            assert_eq!(e.segments("db1.tb1"), vec![(3, 2)]);
            assert_eq!(e.row_count("db1.tb1").unwrap(), 2);
            assert_eq!(
                rows(e, "SELECT count(*) FROM db1.tb1 WHERE col1 = 1;"),
                vec![vec!["1".to_string()]]
            );
            assert_eq!(
                rows(e, "SELECT count(*) FROM db1.tb1 WHERE col1 = 2;"),
                vec![vec!["1".to_string()]]
            );
            e.flush().unwrap();
            assert_eq!(e.block_row_count("db1.tb1", 0, 0), Some(2));
        },
    );
}

/// C++ `cleanup_ut.cpp:849` (`cleanup_and_dump_index`).
///
/// The fixture dumps the index for segment 1 and then reads the mem-index and
/// chunk metadata. The Rust engine has no per-segment mem-index for a
/// secondary index; `flush()` performs the dump and the visible segment/row
/// and index state is asserted exactly.
#[test]
fn cleanup_and_dump_index() {
    run_plans(
        pre_test,
        |e| {
            e.flush().unwrap();
        },
        |e| {
            assert_eq!(e.segment_ids("db1.tb1"), vec![1]);
            assert_eq!(count(e, "db1.tb1"), 2);
            e.execute("SHOW TABLE db1.tb1 INDEX index1;").unwrap();
            // The dump keeps every indexed row queryable.
            assert_eq!(
                rows(e, "SELECT count(*) FROM db1.tb1 WHERE col1 = 1;"),
                vec![vec!["2".to_string()]]
            );
        },
    );
}
