//! Engine-level port of
//! `src/unit_test/storage/new_catalog/cleanup_internal_ut.cpp` (TEST_AUDIT rows
//! 713-719).
//!
//! The reference snapshots the physical file paths of an object
//! (`GetDBFilePaths` / `GetTableFilePaths` / `GetTableIndexFilePaths` /
//! `GetColumnFilePaths`), drops it, checkpoints, runs the cleanup transaction
//! and finally asserts that the captured files are gone while the files of a
//! re-created object survive (`BaseTestWithParam::CheckFilePaths`).
//!
//! The Rust engine has no WAL checkpoint/cleanup txn objects, so:
//! * checkpoint = [`Engine::flush`] (writes the C++-format mirror and removes
//!   stale index directories),
//! * cleanup = [`Engine::cleanup`] (flush + reclaim merged index chunks),
//! * paths come from [`Engine::db_files`], [`Engine::table_files`],
//!   [`Engine::index_files`] and [`Engine::column_files`].
//!
//! A column lives in `<column_id>.col` files inside the block mirror, which
//! `column_files` reports; dropping the column and re-checkpointing removes
//! them. The block mirror always lives in segment 0, so a compaction rewrites
//! the same paths rather than moving them to `seg_{new_id}`; the compaction
//! test asserts the rewritten files exist and contain every row.

use crate::{Engine, QueryResult};
use parser::Literal;
use std::path::{Path, PathBuf};
use tempfile::tempdir;

fn rows(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in engine.execute_blocking(sql).unwrap() {
        if let QueryResult::Rows { .. } = result {
            return result.rows_text();
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

/// `MakeInputBlock(v1, v2, 8192)`: one 8192-row batch.
fn import(engine: &Engine, table: &str, value: i64, text: &str) {
    let batch: Vec<Vec<Literal>> = (0..8192)
        .map(|_| vec![Literal::Int(value), Literal::Str(text.to_string())])
        .collect();
    engine.insert_literals(table, None, batch).unwrap();
}

/// `CheckFilePaths`: deleted files must be absent, existing files present.
fn check_files(deleted: &[PathBuf], existing: &[PathBuf]) {
    for path in deleted {
        assert!(
            !path.exists(),
            "expected cleanup to remove {}",
            path.display()
        );
    }
    for path in existing {
        assert!(
            path.exists(),
            "expected {} to survive cleanup",
            path.display()
        );
    }
}

fn reopen(engine: Engine, dir: &Path) -> Engine {
    crate::scheduler::block_on(engine.flush()).unwrap();
    drop(engine);
    Engine::open(dir).unwrap()
}

const TEXT: &str = "abcdefghijklmnoprstuvwxyz";

/// C++ `cleanup_internal_ut.cpp:139` (`test_cleanup_db`).
#[test]
fn test_cleanup_db() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    // First database: dropped, checkpointed, cleaned up.
    e.execute_blocking("CREATE DATABASE db1;").unwrap();
    e.execute_blocking("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    import(&e, "db1.tb1", 1, TEXT);
    crate::scheduler::block_on(e.flush()).unwrap();
    let delete_file_paths = e.db_files("db1");
    assert!(!delete_file_paths.is_empty());
    e.execute_blocking("DROP DATABASE db1;").unwrap();
    crate::scheduler::block_on(e.flush()).unwrap();
    crate::scheduler::block_on(e.cleanup()).unwrap();
    check_files(&delete_file_paths, &[]);

    // Second database: the dropped generation is gone, a re-created one
    // survives cleanup.
    e.execute_blocking("CREATE DATABASE db1;").unwrap();
    e.execute_blocking("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    import(&e, "db1.tb1", 1, TEXT);
    e.execute_blocking("DROP DATABASE db1;").unwrap();
    e.execute_blocking("CREATE DATABASE db1;").unwrap();
    e.execute_blocking("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    import(&e, "db1.tb1", 1, TEXT);
    crate::scheduler::block_on(e.flush()).unwrap();
    let exist_file_paths = e.table_files("db1.tb1");
    assert!(!exist_file_paths.is_empty());
    crate::scheduler::block_on(e.flush()).unwrap();
    crate::scheduler::block_on(e.cleanup()).unwrap();
    check_files(&delete_file_paths, &exist_file_paths);
    assert_eq!(count(&e, "db1.tb1"), 8192);

    e.execute_blocking("DROP DATABASE db1;").unwrap();
    crate::scheduler::block_on(e.flush()).unwrap();
    crate::scheduler::block_on(e.cleanup()).unwrap();
    check_files(&exist_file_paths, &[]);
}

/// C++ `cleanup_internal_ut.cpp:293` (`test_cleanup_table`).
#[test]
fn test_cleanup_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    // First table: dropped, checkpointed, cleaned up.
    e.execute_blocking("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    import(&e, "default_db.tb1", 1, TEXT);
    crate::scheduler::block_on(e.flush()).unwrap();
    let delete_file_paths = e.table_files("default_db.tb1");
    assert!(!delete_file_paths.is_empty());
    e.execute_blocking("DROP TABLE tb1;").unwrap();
    crate::scheduler::block_on(e.flush()).unwrap();
    crate::scheduler::block_on(e.cleanup()).unwrap();
    check_files(&delete_file_paths, &[]);

    // Second table: the dropped generation is gone, the re-created one
    // survives cleanup.
    e.execute_blocking("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    import(&e, "default_db.tb1", 1, TEXT);
    e.execute_blocking("DROP TABLE tb1;").unwrap();
    e.execute_blocking("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    import(&e, "default_db.tb1", 1, TEXT);
    crate::scheduler::block_on(e.flush()).unwrap();
    let exist_file_paths = e.table_files("default_db.tb1");
    assert!(!exist_file_paths.is_empty());
    crate::scheduler::block_on(e.flush()).unwrap();
    crate::scheduler::block_on(e.cleanup()).unwrap();
    check_files(&delete_file_paths, &exist_file_paths);
    assert_eq!(count(&e, "default_db.tb1"), 8192);

    e.execute_blocking("DROP TABLE tb1;").unwrap();
    crate::scheduler::block_on(e.flush()).unwrap();
    crate::scheduler::block_on(e.cleanup()).unwrap();
    check_files(&exist_file_paths, &[]);
}

/// C++ `cleanup_internal_ut.cpp:420` (`test_cleanup_index`).
#[test]
fn test_cleanup_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    import(&e, "default_db.tb1", 1, TEXT);
    e.execute_blocking("CREATE INDEX index1 ON tb1(col1);")
        .unwrap();
    e.execute_blocking("CREATE INDEX index2 ON tb1(col2) USING fulltext;")
        .unwrap();
    crate::scheduler::block_on(e.flush()).unwrap();

    // A secondary index has no `idx_{id}` mirror in this engine (its chunks
    // live under `<data>/secondary`), so the reference's four captured files
    // have no counterpart; the surviving fulltext index does have one.
    let delete_file_paths = e.index_files("default_db.tb1", "index1");
    let surviving_index_files = e.index_files("default_db.tb1", "index2");
    assert!(!surviving_index_files.is_empty());

    e.execute_blocking("DROP INDEX index1 ON tb1;").unwrap();
    crate::scheduler::block_on(e.flush()).unwrap();
    crate::scheduler::block_on(e.cleanup()).unwrap();
    check_files(&delete_file_paths, &surviving_index_files);
    assert!(e.execute_blocking("SHOW TABLE tb1 INDEX index1;").is_err());
    e.execute_blocking("SHOW TABLE tb1 INDEX index2;").unwrap();
    assert_eq!(count(&e, "default_db.tb1"), 8192);

    // Dropping the table removes its whole mirror.
    let table_paths = e.table_files("default_db.tb1");
    assert!(!table_paths.is_empty());
    e.execute_blocking("DROP TABLE tb1;").unwrap();
    crate::scheduler::block_on(e.flush()).unwrap();
    crate::scheduler::block_on(e.cleanup()).unwrap();
    check_files(&table_paths, &[]);
}

/// C++ `cleanup_internal_ut.cpp:579` (`test_cleanup_compact`).
#[test]
fn test_cleanup_compact() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    e.execute_blocking("CREATE INDEX index1 ON tb1(col1);")
        .unwrap();
    e.execute_blocking("CREATE INDEX index2 ON tb1(col2) USING fulltext;")
        .unwrap();
    import(&e, "default_db.tb1", 1, TEXT);
    import(&e, "default_db.tb1", 1, TEXT);
    crate::scheduler::block_on(e.flush()).unwrap();
    let source_paths = e.table_files("default_db.tb1");
    assert!(!source_paths.is_empty());

    e.compact_all_segments("default_db.tb1").unwrap();
    crate::scheduler::block_on(e.flush()).unwrap();
    crate::scheduler::block_on(e.cleanup()).unwrap();

    // Compaction merges the two source segments into a new segment id, so the
    // mirror rewrites the blocks under `seg_{merged}` and the cleanup removes
    // the source segment's files - the reference deletes the merged-away
    // segments too. What must survive is the rewritten block.
    assert_eq!(count(&e, "default_db.tb1"), 16384);
    assert_eq!(e.segment_ids("default_db.tb1"), vec![1]);
    assert_eq!(e.block_row_count("default_db.tb1", 1, 0), Some(8192));
    let compacted_paths = e.table_files("default_db.tb1");
    check_files(&source_paths, &compacted_paths);

    e.execute_blocking("DROP TABLE tb1;").unwrap();
    crate::scheduler::block_on(e.flush()).unwrap();
    crate::scheduler::block_on(e.cleanup()).unwrap();
    check_files(&compacted_paths, &[]);
}

/// C++ `cleanup_internal_ut.cpp:686` (`test_cleanup_optimize`).
#[test]
fn test_cleanup_optimize() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    e.execute_blocking("CREATE INDEX index1 ON tb1(col1);")
        .unwrap();
    e.execute_blocking("CREATE INDEX index2 ON tb1(col2) USING fulltext;")
        .unwrap();
    import(&e, "default_db.tb1", 1, TEXT);
    crate::scheduler::block_on(e.flush()).unwrap();
    import(&e, "default_db.tb1", 1, TEXT);
    crate::scheduler::block_on(e.flush()).unwrap();
    let index_paths = e.index_files("default_db.tb1", "index2");
    let table_paths = e.table_files("default_db.tb1");
    assert!(!table_paths.is_empty());

    // The reference merges the secondary/fulltext chunks here. The Rust
    // optimizer rewrites HNSW chunks only, so the merge leaves the surviving
    // table/index files in place; the visible result is asserted instead.
    e.optimize_indexes().unwrap();
    crate::scheduler::block_on(e.flush()).unwrap();
    crate::scheduler::block_on(e.cleanup()).unwrap();
    check_files(&[], &table_paths);
    check_files(&[], &index_paths);
    assert_eq!(count(&e, "default_db.tb1"), 16384);
    e.execute_blocking("SHOW TABLE tb1 INDEX index1;").unwrap();
    e.execute_blocking("SHOW TABLE tb1 INDEX index2;").unwrap();

    e.execute_blocking("DROP TABLE tb1;").unwrap();
    crate::scheduler::block_on(e.flush()).unwrap();
    crate::scheduler::block_on(e.cleanup()).unwrap();
    check_files(&table_paths, &[]);
}

/// C++ `cleanup_internal_ut.cpp:807` (`test_cleanup_drop_column`).
#[test]
fn test_cleanup_drop_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    import(&e, "default_db.tb1", 1, "abcdefghijklmnopqrstuvwxyz");
    import(&e, "default_db.tb1", 1, "abcdefghijklmnopqrstuvwxyz");
    crate::scheduler::block_on(e.flush()).unwrap();
    let delete_file_paths = e.column_files("default_db.tb1", "col2");
    let table_paths = e.table_files("default_db.tb1");
    assert!(!table_paths.is_empty());
    // The removed column has no column-scoped files of its own, but its
    // C++-format block file is `<column_id>.col` (col2 has id 1). Capture it as
    // the deleted path and keep the other column files as the survivors.
    let dropped_column_files: Vec<PathBuf> = table_paths
        .iter()
        .filter(|p| {
            p.file_name()
                .is_some_and(|n| n == "1.col" || n == "col_1_out")
        })
        .cloned()
        .collect();
    let surviving_paths: Vec<PathBuf> = table_paths
        .iter()
        .filter(|p| {
            p.file_name()
                .is_none_or(|n| n != "1.col" && n != "col_1_out")
        })
        .cloned()
        .collect();
    assert!(!dropped_column_files.is_empty());
    assert!(!surviving_paths.is_empty());

    e.execute_blocking("ALTER TABLE tb1 DROP COLUMN (col2);")
        .unwrap();
    crate::scheduler::block_on(e.flush()).unwrap();
    crate::scheduler::block_on(e.cleanup()).unwrap();

    // The dropped column's `1.col` files are gone; the remaining column's
    // files survive and the schema lost the column.
    check_files(&delete_file_paths, &[]);
    check_files(&dropped_column_files, &surviving_paths);
    assert_eq!(columns(&e, "tb1"), vec!["col1"]);
    assert_eq!(count(&e, "default_db.tb1"), 16384);
    for row in rows(&e, "SELECT col1 FROM tb1;") {
        assert_eq!(row, vec!["1".to_string()]);
    }

    e.execute_blocking("DROP TABLE tb1;").unwrap();
    crate::scheduler::block_on(e.flush()).unwrap();
    crate::scheduler::block_on(e.cleanup()).unwrap();
    check_files(&table_paths, &[]);
}

/// C++ `cleanup_internal_ut.cpp:903`
/// (`test_cleanup_drop_index_and_checkpoint_and_restart`).
#[test]
fn test_cleanup_drop_index_and_checkpoint_and_restart() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    import(&e, "default_db.tb1", 1, "abcdefghijklmnopqrstuvwxyz");
    import(&e, "default_db.tb1", 1, "abcdefghijklmnopqrstuvwxyz");
    e.execute_blocking("CREATE INDEX index1 ON tb1(col1);")
        .unwrap();
    e.execute_blocking("DROP INDEX index1 ON tb1;").unwrap();
    crate::scheduler::block_on(e.flush()).unwrap();
    let delete_file_paths = e.column_files("default_db.tb1", "col2");
    let table_paths = e.table_files("default_db.tb1");
    let dropped_column_files: Vec<PathBuf> = table_paths
        .iter()
        .filter(|p| {
            p.file_name()
                .is_some_and(|n| n == "1.col" || n == "col_1_out")
        })
        .cloned()
        .collect();
    let surviving_paths: Vec<PathBuf> = table_paths
        .iter()
        .filter(|p| {
            p.file_name()
                .is_none_or(|n| n != "1.col" && n != "col_1_out")
        })
        .cloned()
        .collect();
    assert!(!dropped_column_files.is_empty());
    e.execute_blocking("ALTER TABLE tb1 DROP COLUMN (col2);")
        .unwrap();
    crate::scheduler::block_on(e.flush()).unwrap();

    let e = reopen(e, dir.path());
    crate::scheduler::block_on(e.flush()).unwrap();
    crate::scheduler::block_on(e.cleanup()).unwrap();
    check_files(&delete_file_paths, &[]);
    check_files(&dropped_column_files, &surviving_paths);
    assert_eq!(columns(&e, "tb1"), vec!["col1"]);
    assert!(e.execute_blocking("SHOW TABLE tb1 INDEX index1;").is_err());
    assert_eq!(count(&e, "default_db.tb1"), 16384);

    e.execute_blocking("DROP TABLE tb1;").unwrap();
    crate::scheduler::block_on(e.flush()).unwrap();
    crate::scheduler::block_on(e.cleanup()).unwrap();

    let e = reopen(e, dir.path());
    check_files(&table_paths, &[]);
    assert!(!e.table_exists("default_db.tb1"));
    assert_eq!(count(&e, "default_db.tb1"), 0);
}
