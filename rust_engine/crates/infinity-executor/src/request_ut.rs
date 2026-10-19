//! Engine-level ports of the reference's `new_request` DDL tests
//! (`src/unit_test/storage/new_request/ddl_request_ut.cpp`).
//!
//! Each reference case issues a DDL statement twice through a fresh
//! `QueryContext` and asserts that the second attempt fails. The Rust engine
//! exposes the same behavior through [`Engine::execute`], so every test keeps
//! the reference's success/failure sequence and drops only the reference's
//! per-statement `QueryContext` bookkeeping.

use crate::Engine;
use tempfile::tempdir;

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:38`
/// (`test_create_db`).
#[test]
fn create_database_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    assert!(e.execute("CREATE DATABASE db1;").is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:54`
/// (`test_drop_db`).
#[test]
fn drop_database_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("DROP DATABASE db1;").unwrap();
    assert!(e.execute("DROP DATABASE db1;").is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:77`
/// (`test_create_table`).
#[test]
fn create_table_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE TABLE t1(c1 INT, c2 VARCHAR);").unwrap();
    assert!(e.execute("CREATE TABLE t1(c1 INT, c2 VARCHAR);").is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:93`
/// (`test_drop_table`).
#[test]
fn drop_table_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE TABLE t1(c1 INT, c2 VARCHAR);").unwrap();
    e.execute("DROP TABLE t1;").unwrap();
    assert!(!e.table_exists("t1"));
    assert!(e.execute("DROP TABLE t1;").is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:116`
/// (`test_create_index`).
#[test]
fn create_index_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE TABLE t1(c1 INT, c2 VARCHAR);").unwrap();
    e.execute("CREATE INDEX idx1 ON t1 (c1);").unwrap();
    assert!(e.execute("CREATE INDEX idx1 ON t1 (c1);").is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:139`
/// (`test_drop_index`).
#[test]
fn drop_index_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE TABLE t1(c1 INT, c2 VARCHAR);").unwrap();
    e.execute("CREATE INDEX idx1 ON t1 (c1);").unwrap();
    e.execute("DROP INDEX idx1 ON t1;").unwrap();
    assert!(e.execute("DROP INDEX idx1 ON t1;").is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:169`
/// (`test_add_column`).
#[test]
fn add_column_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE TABLE t1(c1 INT, c2 VARCHAR);").unwrap();
    e.execute("ALTER TABLE t1 ADD COLUMN (c3 INT DEFAULT 0);")
        .unwrap();
    assert!(e
        .execute("ALTER TABLE t1 ADD COLUMN (c3 INT DEFAULT 0);")
        .is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:192`
/// (`test_drop_column`).
#[test]
fn drop_column_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE TABLE t1(c1 INT, c2 VARCHAR);").unwrap();
    e.execute("ALTER TABLE t1 DROP COLUMN (c1);").unwrap();
    assert!(e.execute("ALTER TABLE t1 DROP COLUMN (c1);").is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:215`
/// (`test_use_schema`).
#[test]
fn use_requires_existing_database() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("USE db1;").unwrap();
    assert!(e.execute("USE db2;").is_err());
}
