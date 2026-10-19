//! Ports of `src/unit_test/storage/bg_task/*_ut.cpp`.
//!
//! The reference runs a `BGTaskProcessor` thread pool; the Rust engine has no
//! background scheduler, so each case runs the task explicitly through its
//! public entry point (`cleanup`, `compact_table`, `flush`).
//!
//! Blocked rows (reported, no test):
//! - `background_process_ut.cpp` `BGProcessTest.test1`: there is no
//!   `BGTaskProcessor` (no start/stop thread pool) in the Rust engine.
//! - `dump_index_task_ut.cpp` / `dump_index_task_2_ut.cpp`: the reference's
//!   `DumpMemIndexTask` polls the memory-index capacity/quota from the
//!   background, so two appends accumulate into one chunk before it fires.
//!   The Rust engine enforces the quota inside the insert loop and dumps a
//!   secondary index only at a checkpoint, so the chunk boundaries the
//!   reference asserts are not reproducible without adding a background
//!   scheduler.

use crate::{Engine, QueryResult};
use infinity_parser::Literal;
use tempfile::tempdir;

fn rows(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in engine.execute(sql).unwrap() {
        if let QueryResult::Rows { rows, .. } = result {
            return rows;
        }
    }
    Vec::new()
}

/// One 8192-row block: `col1 = 1`, `col2 = abcdefghijklmnopqrstuvwxyz`
/// (`MakeInputBlock`).
fn append_block(engine: &Engine, table: &str) {
    let row = [
        Literal::Int(1),
        Literal::Str("abcdefghijklmnopqrstuvwxyz".into()),
    ];
    engine
        .insert_literals(table, None, (0..8192).map(|_| row.to_vec()).collect())
        .unwrap();
}

/// C++: `cleanup_task_ut.cpp:57` (`test_delete_db_simple`).
///
/// After `DROP TABLE` + checkpoint + cleanup the dropped table's data files are
/// gone and every file of the surviving table is still there
/// (`BaseTest::CheckFilePaths`).
#[test]
fn cleanup_removes_the_dropped_table_files() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    e.execute("CREATE TABLE tb2 (col1 INT, col2 VARCHAR);")
        .unwrap();
    append_block(&e, "default_db.tb1");
    append_block(&e, "default_db.tb2");
    e.flush().unwrap();

    let exist_paths = e.table_files("default_db.tb1");
    let delete_paths = e.table_files("default_db.tb2");
    assert!(!exist_paths.is_empty(), "tb1 has no data files");
    assert!(!delete_paths.is_empty(), "tb2 has no data files");

    e.execute("DROP TABLE default_db.tb2;").unwrap();
    e.flush().unwrap();
    // `TransactionType::kCleanup` (the reference waits for its bg task).
    e.cleanup().unwrap();

    for path in &delete_paths {
        assert!(!path.exists(), "dropped table file survived: {path:?}");
    }
    for path in &exist_paths {
        assert!(path.exists(), "surviving table file was removed: {path:?}");
    }
    assert!(e.table_exists("tb1"));
    assert!(!e.table_exists("tb2"));
}

/// C++: `compact_segments_task_ut.cpp:73` (`bg_compact`).
///
/// Eleven imports form eleven segments; compaction merges them (`segment_count`
/// drops from 11 to 1) without losing a row.
#[test]
fn compact_reduces_the_segment_count() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();

    // `segment_sizes = {1, 10}`: one segment holding one block, then ten more.
    for _ in 0..11 {
        append_block(&e, "default_db.tb1");
    }
    assert_eq!(e.row_count("default_db.tb1").unwrap(), 11 * 8192);
    // The reference asserts `segment_ids.size() < 11` after the background
    // compaction; here the eleven write batches report as eleven segments
    // before it and a single merged segment after.
    let before = e.segment_count("default_db.tb1");
    assert_eq!(before, 11);

    e.compact_table("default_db.tb1").unwrap();

    let after = e.segment_count("default_db.tb1");
    assert!(after < before);
    assert_eq!(after, 1);
    assert_eq!(e.row_count("default_db.tb1").unwrap(), 11 * 8192);
    let count: usize = rows(&e, "SELECT count(*) FROM default_db.tb1;")[0][0]
        .parse()
        .unwrap();
    assert_eq!(count, 11 * 8192);
}
