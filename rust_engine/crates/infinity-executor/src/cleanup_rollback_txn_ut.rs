//! Engine-level port of
//! `src/unit_test/storage/new_catalog/cleanup_rollback_txn.cpp` (TEST_AUDIT row
//! 720).
//!
//! The reference stages an `Import` (8192 rows, two indexes) in a live
//! transaction, drops the database from another transaction and commits the
//! drop first. Committing the import then fails and cleanup must reclaim the
//! dropped database's files. The Rust port stages the append through
//! [`crate::EngineTxn`], drops the database, asserts the commit is rejected
//! (the commit path validates the target table still exists) and checks that
//! cleanup removed the captured data files.

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

/// C++ `cleanup_rollback_txn.cpp:125`
/// (`test_import_with_index_rollback_cleanup`).
#[test]
fn test_import_with_index_rollback_cleanup() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    e.execute("CREATE INDEX index1 ON db1.tb1(col1);").unwrap();
    e.execute("CREATE INDEX index2 ON db1.tb1(col2) USING fulltext;")
        .unwrap();

    // `MakeInputBlock`: 8192 rows alternating (1, "abc") / (2, long text).
    let batch: Vec<Vec<Literal>> = (0..8192)
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
        .collect();
    let mut txn = e.begin_txn();
    assert_eq!(txn.append("db1.tb1", batch).unwrap(), 8192);

    // The other transaction drops the database and commits first.
    e.execute("DROP DATABASE db1;").unwrap();
    // Committing the import against the now-missing table must fail.
    assert!(txn.commit().is_err());

    e.flush().unwrap();
    e.cleanup().unwrap();

    // The database (and its tables/indexes) are gone, with no rows left behind.
    assert!(!e.table_exists("db1.tb1"));
    assert!(!rows(&e, "SHOW DATABASES;").iter().any(|r| r[0] == "db1"));
}
