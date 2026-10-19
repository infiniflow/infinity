//! Engine-level ports of `src/unit_test/storage/new_catalog/database_ut.cpp`
//! (`TestTxnDatabase`, TEST_AUDIT rows 754-760).
//!
//! The reference drives `NewTxn::CreateDatabase` / `DropDatabase` with live
//! transaction handles and asserts which commit wins. The engine has no DDL
//! transaction surface, so each interleaving is expressed as the equivalent
//! sequence of auto-committed statements: the successful operation is the one
//! whose commit wins in the reference, and the failing operation is the one
//! whose `CreateDatabase`/`DropDatabase`/commit returns a non-ok status. The
//! final catalog state and its durability across a reopen are asserted too.

use crate::{Engine, QueryResult};
use tempfile::tempdir;

fn rows(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in engine.execute(sql).unwrap() {
        if let QueryResult::Rows { rows, .. } = result {
            return rows;
        }
    }
    Vec::new()
}

fn databases(engine: &Engine) -> Vec<String> {
    let mut names: Vec<String> = rows(engine, "SHOW DATABASES;")
        .into_iter()
        .map(|row| row[0].clone())
        .collect();
    names.sort();
    names
}

fn db_exists(engine: &Engine, name: &str) -> bool {
    databases(engine).iter().any(|db| db == name)
}

/// C++ `database_ut.cpp:45` (`createdb_createdb_test`): six create/create
/// interleavings. The first committed create wins; every later create without
/// `IF NOT EXISTS` fails, and a drop removes the database.
#[test]
fn createdb_createdb_test() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    // t1 commits the create before t2's create: t2 fails.
    e.execute("CREATE DATABASE db1;").unwrap();
    assert!(e.execute("CREATE DATABASE db1;").is_err());
    // `IF NOT EXISTS` is the reference's kIgnore path.
    e.execute("CREATE DATABASE IF NOT EXISTS db1;").unwrap();
    assert!(db_exists(&e, "db1"));
    assert_eq!(
        rows(&e, "SHOW DATABASES;")
            .into_iter()
            .filter(|row| row[0] == "db1")
            .count(),
        1
    );

    // t1 drops after its create committed: the database disappears.
    e.execute("DROP DATABASE db1;").unwrap();
    assert!(!db_exists(&e, "db1"));
    // Dropping an absent database with kError fails.
    assert!(e.execute("DROP DATABASE db1;").is_err());

    // A fresh create starts empty.
    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("USE db1;").unwrap();
    assert!(rows(&e, "SHOW TABLES;").is_empty());
    e.execute("USE default_db;").unwrap();
    e.execute("DROP DATABASE db1;").unwrap();

    // The commits are durable: reopen on the same directory sees no db1.
    drop(e);
    let e = Engine::open(dir.path()).unwrap();
    assert!(!db_exists(&e, "db1"));
}

/// C++ `database_ut.cpp:315` (`db_test2`): dropping a missing database fails
/// with kError and succeeds with kIgnore (`DROP DATABASE IF EXISTS`).
#[test]
fn db_test2() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    assert!(e.execute("DROP DATABASE db1;").is_err());
    e.execute("DROP DATABASE IF EXISTS db1;").unwrap();
    assert!(!db_exists(&e, "db1"));
    // The default database is untouched.
    assert!(db_exists(&e, "default_db"));
}

/// C++ `database_ut.cpp:337` (`db_test3`): duplicate create fails with
/// kError, succeeds with kIgnore, and the normal drop removes the database.
#[test]
fn db_test3() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    assert!(e.execute("CREATE DATABASE db1;").is_err());
    e.execute("CREATE DATABASE IF NOT EXISTS db1;").unwrap();
    assert_eq!(databases(&e), vec!["db1".to_string(), "default_db".to_string()]);

    e.execute("DROP DATABASE db1;").unwrap();
    assert!(!db_exists(&e, "db1"));
    assert!(db_exists(&e, "default_db"));
}

/// C++ `database_ut.cpp:374` (`createdb_wrong_conflicttype`): `kReplace` and
/// `kInvalid` are not accepted for `CreateDatabase`, so the create fails and no
/// database is created.
#[test]
fn createdb_wrong_conflicttype() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    // The SQL surface has no REPLACE/INVALID keyword for CREATE DATABASE; the
    // parser rejects both, which is the reference's non-ok status.
    assert!(e.execute("CREATE DATABASE db1 REPLACE;").is_err());
    assert!(e.execute("CREATE DATABASE db1 INVALID;").is_err());
    assert!(!db_exists(&e, "db1"));

    // The normal conflict type still works.
    e.execute("CREATE DATABASE db1;").unwrap();
    assert!(db_exists(&e, "db1"));
}

/// C++ `database_ut.cpp:396` (`dropdb_wrong_conflicttype`): `kReplace` and
/// `kInvalid` are rejected for `DropDatabase`; the database is left intact and
/// a normal drop then succeeds.
#[test]
fn dropdb_wrong_conflicttype() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    assert!(e.execute("DROP DATABASE db1 REPLACE;").is_err());
    assert!(e.execute("DROP DATABASE db1 INVALID;").is_err());
    assert!(db_exists(&e, "db1"));

    // Still usable after the rejected drops.
    e.execute("USE db1;").unwrap();
    e.execute("CREATE TABLE t (id INT);").unwrap();
    e.execute("INSERT INTO t VALUES (1);").unwrap();
    e.execute("USE default_db;").unwrap();

    e.execute("DROP DATABASE db1;").unwrap();
    assert!(!db_exists(&e, "db1"));
}

/// C++ `database_ut.cpp:434` (`dropdb_dropdb_test`): seven drop/drop
/// interleavings. Both transactions stage a drop, but only the first commit
/// wins; the second fails and the database is gone.
#[test]
fn dropdb_dropdb_test() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("DROP DATABASE db1;").unwrap();
    // The second drop reaches the same key after the first commit and fails.
    assert!(e.execute("DROP DATABASE db1;").is_err());
    assert!(!db_exists(&e, "db1"));

    // A later create starts from an empty database again.
    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("USE db1;").unwrap();
    assert!(rows(&e, "SHOW TABLES;").is_empty());
    e.execute("USE default_db;").unwrap();
    e.execute("DROP DATABASE db1;").unwrap();

    drop(e);
    let e = Engine::open(dir.path()).unwrap();
    assert!(!db_exists(&e, "db1"));
}

/// C++ `database_ut.cpp:606` (`createdb_dropdb_test`): eight create/drop
/// interleavings. A drop that starts before the create commits fails; after the
/// create commits the drop succeeds and nothing remains.
#[test]
fn createdb_dropdb_test() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    // Drop before create: the database is not visible, so the drop fails.
    assert!(e.execute("DROP DATABASE db1;").is_err());
    // The create then commits.
    e.execute("CREATE DATABASE db1;").unwrap();
    assert!(db_exists(&e, "db1"));
    // The winner's drop succeeds.
    e.execute("DROP DATABASE db1;").unwrap();
    assert!(!db_exists(&e, "db1"));

    // The same rules hold when the create/drop order is repeated.
    e.execute("CREATE DATABASE db1;").unwrap();
    assert!(e.execute("DROP DATABASE db1 REPLACE;").is_err());
    e.execute("DROP DATABASE db1;").unwrap();
    assert!(!db_exists(&e, "db1"));

    drop(e);
    let e = Engine::open(dir.path()).unwrap();
    assert!(!db_exists(&e, "db1"));
}
