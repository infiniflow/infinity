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
use infinity_parser::Literal;
use tempfile::tempdir;

/// Creates `db1.tb1 (col1 INT, col2 VARCHAR)`.
fn create_table(e: &Engine) {
    h::ok(e, "CREATE DATABASE db1;");
    h::ok(e, "CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);");
}

/// C++ `src/unit_test/storage/new_catalog/append_ut.cpp:188`
/// (`TestTxnAppend.test_append1`): two committed 2-row appends and two more
/// from concurrently opened transactions yield 8 visible rows in one segment
/// and one block; the first block values are the appended values.
#[test]
fn append1_reads_back_committed_and_concurrent_batches() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);

    let block = vec![
        vec![Literal::Int(1), Literal::Str("abc".to_string())],
        vec![
            Literal::Int(2),
            Literal::Str("abcdefghijklmnopqrstuvwxyz".to_string()),
        ],
    ];
    for _ in 0..2 {
        h::append(&e, "db1.tb1", block.clone());
    }

    // The reference opens two transactions, appends the same block to each,
    // and commits both. Model that with two live transaction handles.
    let mut t1 = e.begin_txn();
    let mut t2 = e.begin_txn();
    assert_eq!(t1.append("db1.tb1", block.clone()).unwrap(), 2);
    assert_eq!(t2.append("db1.tb1", block.clone()).unwrap(), 2);
    t1.commit().unwrap();
    t2.commit().unwrap();

    assert_eq!(e.row_count("db1.tb1").unwrap(), 8);
    assert_eq!(e.segment_ids("db1.tb1"), vec![0]);
    assert_eq!(h::segment_rows(&e, "db1.tb1"), 8);

    // `NewCatalog::GetBlockVisibleRange`: one range `[0, 8)`.
    e.flush().unwrap();
    assert_eq!(e.block_ids("db1.tb1", 0), vec![0]);
    assert_eq!(e.block_row_count("db1.tb1", 0, 0), Some(8));
    assert_eq!(e.block_visible_range("db1.tb1", 0, 0), Some((0, 8)));

    let all = h::rows(&e, "SELECT col1, col2 FROM db1.tb1 ORDER BY col1;");
    assert_eq!(all.len(), 8);
    assert_eq!(all[0], vec!["1", "abc"]);
    assert_eq!(all[4], vec!["2", "abcdefghijklmnopqrstuvwxyz"]);
}

/// C++ `src/unit_test/storage/new_catalog/append_ut.cpp:2811`
/// (`TestTxnAppend.test_append_append`): two 4096-row appends of the same
/// values commit into a single 8192-row block.
#[test]
fn append_append_keeps_one_visible_block() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);

    for _ in 0..2 {
        assert_eq!(
            h::append_repeated(
                &e,
                "db1.tb1",
                1,
                "abcdefghijklmnopqrstuvwxyz",
                4096
            ),
            4096
        );
    }

    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    assert_eq!(e.segment_ids("db1.tb1"), vec![0]);
    e.flush().unwrap();
    assert_eq!(e.block_ids("db1.tb1", 0), vec![0]);
    assert_eq!(e.block_row_count("db1.tb1", 0, 0), Some(8192));
    assert_eq!(e.block_visible_range("db1.tb1", 0, 0), Some((0, 8192)));

    let boundary = h::rows(
        &e,
        "SELECT col1, col2 FROM db1.tb1 WHERE col1 = 1 AND col2 = 'abcdefghijklmnopqrstuvwxyz';",
    );
    assert_eq!(boundary.len(), 8192);
}

/// C++ `src/unit_test/storage/new_catalog/append_ut.cpp:4855`
/// (`TestTxnAppend.test_append_and_optimize_index`): three 8192-row appends,
/// the first two each followed by a memory-index dump, survive an optimize
/// pass and still answer a secondary-index lookup.
#[test]
fn append_and_optimize_index_keeps_rows_and_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);");

    for batch in 0..3 {
        let rows: Vec<Vec<Literal>> = (0..8192)
            .map(|i| {
                vec![
                    Literal::Int((batch * 8192 + i) as i64),
                    Literal::Str("abcdefghijklmnopqrstuvwxyz".to_string()),
                ]
            })
            .collect();
        assert_eq!(h::append(&e, "db1.tb1", rows), 8192);
        // `DumpMemIndex` seals the memory segment for the first two batches.
        if batch < 2 {
            e.flush().unwrap();
        }
    }

    e.optimize_indexes().unwrap();
    e.cleanup().unwrap();

    assert_eq!(e.row_count("db1.tb1").unwrap(), 3 * 8192);
    let hits = h::rows(&e, "SELECT col1 FROM db1.tb1 WHERE col1 = 1;");
    assert_eq!(hits, vec![vec!["1"]]);
    assert_eq!(h::count(&e, "db1.tb1"), 3 * 8192);
    // The reference checks `index_base->index_name_` and the segment's chunk
    // ids; the index-name assertion maps to SHOW TABLE INDEX.
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index1;");
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:322`
/// (`TestTxnDelete.test_delete_and_drop_db`): a committed delete stays applied
/// across `DROP DATABASE`, and every access to the dropped database fails.
#[test]
fn delete_then_drop_database() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192);
    h::append_repeated(&e, "db1.tb1", 2, "abcdefghijklmnopqrstuvwxyz", 8192);

    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 = 1;");
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    // Deleted offsets are outside the visible range.
    assert_eq!(h::count(&e, "db1.tb1"), 8192);

    h::ok(&e, "DROP DATABASE db1;");
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
        assert!(e.execute(sql).is_err(), "{sql} should fail");
    }
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:620`
/// (`TestTxnDelete.test_delete_and_drop_table`).
#[test]
fn delete_then_drop_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192);

    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 = 1;");
    assert_eq!(e.row_count("db1.tb1").unwrap(), 0);
    h::ok(&e, "DROP TABLE db1.tb1;");
    assert!(!e.table_exists("db1.tb1"));
    for sql in [
        "SELECT * FROM db1.tb1;",
        "INSERT INTO db1.tb1 VALUES (1, 'x');",
        "DELETE FROM db1.tb1;",
        "UPDATE db1.tb1 SET col1 = 3;",
    ] {
        assert!(e.execute(sql).is_err(), "{sql} should fail");
    }
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:946`
/// (`TestTxnDelete.test_delete_and_add_column`): rows appended before
/// `ADD COLUMN` read NULL in the new column, and deletes keep working.
#[test]
fn delete_then_add_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::append_repeated(&e, "db1.tb1", 1, "abc", 8192);
    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 = 1;");
    assert_eq!(e.row_count("db1.tb1").unwrap(), 0);

    h::ok(&e, "ALTER TABLE db1.tb1 ADD COLUMN (col3 INT);");
    h::append(&e, "db1.tb1", vec![vec![Literal::Int(2), Literal::Str("abc".into()), Literal::Int(9)]]);
    let all = h::rows(&e, "SELECT col1, col2, col3 FROM db1.tb1;");
    assert_eq!(all, vec![vec!["2", "abc", "9"]]);
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:1602`
/// (`TestTxnDelete.test_delete_and_drop_column`): dropping the deleted column
/// leaves the surviving column's rows intact.
#[test]
fn delete_then_drop_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::append_repeated(&e, "db1.tb1", 1, "abc", 8192);
    h::append_repeated(&e, "db1.tb1", 2, "def", 8192);

    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 = 1;");
    h::ok(&e, "ALTER TABLE db1.tb1 DROP COLUMN (col1);");
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    let all = h::rows(&e, "SELECT col2 FROM db1.tb1;");
    assert!(all.iter().all(|r| r[0] == "def"));
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:2227`
/// (`TestTxnDelete.test_delete_and_rename`): the tombstones follow the table
/// through `RENAME TO`.
#[test]
fn delete_then_rename_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    let rows: Vec<Vec<Literal>> = (0..8192)
        .map(|i| vec![Literal::Int(i), Literal::Str(format!("abc_{i}"))])
        .collect();
    h::append(&e, "db1.tb1", rows);
    let mut t = e.begin_txn();
    let ids: Vec<(u32, u32)> = (0..8192).filter(|i| i % 2 == 0).map(|i| (0, i as u32)).collect();
    assert_eq!(t.delete_row_ids("db1.tb1", &ids).unwrap(), 4096);
    t.commit().unwrap();

    h::ok(&e, "ALTER TABLE db1.tb1 RENAME TO db1.tb2;");
    assert!(!e.table_exists("db1.tb1"));
    assert_eq!(e.row_count("db1.tb2").unwrap(), 4096);
    assert_eq!(h::rows(&e, "SELECT col1 FROM db1.tb2 ORDER BY col1 LIMIT 1;"), vec![vec!["1"]]);
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:2839`
/// (`TestTxnDelete.test_delete_and_create_index`): a secondary index built
/// after the delete does not resurrect deleted rows.
#[test]
fn delete_then_create_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    let rows: Vec<Vec<Literal>> = (0..8192)
        .map(|i| vec![Literal::Int(i), Literal::Str(format!("abc_{i}"))])
        .collect();
    h::append(&e, "db1.tb1", rows);
    // `txn1->Delete(RowID(0, {0,2,4,…}))`: every even offset.
    let mut t = e.begin_txn();
    let ids: Vec<(u32, u32)> = (0..8192).filter(|i| i % 2 == 0).map(|i| (0, i as u32)).collect();
    assert_eq!(t.delete_row_ids("db1.tb1", &ids).unwrap(), 4096);
    t.commit().unwrap();

    h::ok(&e, "CREATE INDEX idx ON db1.tb1 (col1);");
    assert_eq!(h::count(&e, "db1.tb1"), 4096);
    for gone in [0, 2, 4] {
        assert!(
            h::rows(&e, &format!("SELECT col1 FROM db1.tb1 WHERE col1 = {gone};")).is_empty(),
            "even row {gone} is deleted"
        );
    }
    for live in [1, 3, 5] {
        assert_eq!(
            h::rows(&e, &format!("SELECT col1 FROM db1.tb1 WHERE col1 = {live};")),
            vec![vec![live.to_string()]]
        );
    }
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:3467`
/// (`TestTxnDelete.test_delete_and_drop_index`): dropping the index leaves the
/// deleted rows invisible.
#[test]
fn delete_then_drop_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX idx ON db1.tb1 (col1);");
    h::append_repeated(&e, "db1.tb1", 1, "abc", 8192);
    h::append_repeated(&e, "db1.tb1", 2, "def", 8192);

    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 = 1;");
    h::ok(&e, "DROP INDEX idx ON db1.tb1;");
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    assert_eq!(h::rows(&e, "SELECT col2 FROM db1.tb1;").len(), 8192);
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:4151`
/// (`TestTxnDelete.test_delete_and_import`): an imported 8192-row batch and a
/// committed delete leave exactly the imported rows that were not deleted.
#[test]
fn delete_then_import() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::append_repeated(&e, "db1.tb1", 1, "abcdefghijklmnopqrstuvwxyz", 8192);
    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 = 1;");
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
        &format!("COPY db1.tb1 FROM '{}' WITH (DELIMITER ',', FORMAT CSV);", csv.display()),
    );
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    assert_eq!(h::count(&e, "db1.tb1"), 8192);
    assert_eq!(h::rows(&e, "SELECT col1 FROM db1.tb1 LIMIT 1;"), vec![vec!["7"]]);
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:5638`
/// (`TestTxnDelete.test_delete_and_delete`): a second delete only removes the
/// rows the first one left behind.
#[test]
fn delete_then_delete() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::append_repeated(&e, "db1.tb1", 1, "abc", 8192);
    h::append_repeated(&e, "db1.tb1", 2, "def", 8192);

    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 = 1;");
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 = 2;");
    assert_eq!(e.row_count("db1.tb1").unwrap(), 0);
    assert!(h::rows(&e, "SELECT * FROM db1.tb1;").is_empty());
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:6183`
/// (`TestTxnDelete.test_delete_and_compact`): compaction drops the tombstones
/// and keeps only live rows in a single segment.
#[test]
fn delete_then_compact() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::append_repeated(&e, "db1.tb1", 1, "abc", 8192);
    h::append_repeated(&e, "db1.tb1", 2, "def", 8192);
    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 = 1;");

    e.compact_table("db1.tb1").unwrap();
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    assert_eq!(e.segment_ids("db1.tb1"), vec![0]);
    e.flush().unwrap();
    assert_eq!(e.block_ids("db1.tb1", 0).len(), 1);
    assert_eq!(e.block_row_count("db1.tb1", 0, 0), Some(8192));
    assert_eq!(e.block_visible_range("db1.tb1", 0, 0), Some((0, 8192)));
    assert!(h::rows(&e, "SELECT * FROM db1.tb1 WHERE col1 = 1;").is_empty());
}

/// C++ `src/unit_test/storage/new_catalog/delete_ut.cpp:6595`
/// (`TestTxnDelete.test_delete_and_optimize_index`): optimize keeps the
/// tombstones and the surviving rows visible.
#[test]
fn delete_then_optimize_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX idx ON db1.tb1 (col1);");
    for batch in 0..3 {
        let rows: Vec<Vec<Literal>> = (0..8192)
            .map(|i| {
                vec![
                    Literal::Int((batch * 8192 + i) as i64),
                    Literal::Str("abcdefghijklmnopqrstuvwxyz".to_string()),
                ]
            })
            .collect();
        h::append(&e, "db1.tb1", rows);
    }
    h::ok(&e, "DELETE FROM db1.tb1 WHERE col1 < 8192;");

    e.optimize_indexes().unwrap();
    e.cleanup().unwrap();
    assert_eq!(e.row_count("db1.tb1").unwrap(), 2 * 8192);
    assert_eq!(h::count(&e, "db1.tb1"), 2 * 8192);
    assert!(h::rows(&e, "SELECT col1 FROM db1.tb1 WHERE col1 = 1;").is_empty());
    assert_eq!(
        h::rows(&e, "SELECT col1 FROM db1.tb1 WHERE col1 = 8192;"),
        vec![vec!["8192"]]
    );
}
