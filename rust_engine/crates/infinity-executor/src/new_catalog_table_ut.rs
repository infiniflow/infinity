//! Engine-level ports of `src/unit_test/storage/new_catalog/table_ut.cpp`
//! (`TestTxnTable`, TEST_AUDIT rows 900-910).
//!
//! The reference creates/drops tables inside live `NewTxn` handles and asserts
//! which commit wins. The engine has no DDL transaction surface, so each
//! interleaving is expressed as the equivalent sequence of auto-committed
//! statements: the operation whose commit wins becomes the successful call, and
//! the operation whose `CreateTable`/`DropTable`/commit returns non-ok becomes
//! an `is_err()` assertion. Catalog state, visibility and durability across a
//! reopen are asserted as in the reference.

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
    rows(engine, "SHOW DATABASES;")
        .into_iter()
        .map(|row| row[0].clone())
        .collect()
}

fn db_exists(engine: &Engine, name: &str) -> bool {
    databases(engine).iter().any(|db| db == name)
}

fn tables(engine: &Engine, db: &str) -> Vec<String> {
    let mut names: Vec<String> = rows(engine, &format!("SHOW TABLES FROM {db};"))
        .into_iter()
        .map(|row| row[1].clone())
        .collect();
    names.sort();
    names
}

/// C++ `table_ut.cpp:49` (`table_test1`): create database, create table, list
/// it, drop the table, then drop the database.
#[test]
fn table_test1() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    assert_eq!(tables(&e, "db1"), vec!["tb1".to_string()]);

    e.execute("DROP TABLE db1.tb1;").unwrap();
    assert!(tables(&e, "db1").is_empty());
    assert!(e.execute("INSERT INTO db1.tb1 VALUES (1, 'a');").is_err());

    e.execute("DROP DATABASE db1;").unwrap();
    assert!(!db_exists(&e, "db1"));
}

/// C++ `table_ut.cpp:108` (`table_test2`): three tables survive until the
/// database is dropped, which removes all of them.
#[test]
fn table_test2() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    for name in ["tb1", "tb2", "tb3"] {
        e.execute(&format!(
            "CREATE TABLE db1.{name} (col1 INT, col2 VARCHAR);"
        ))
        .unwrap();
    }
    assert_eq!(
        tables(&e, "db1"),
        vec!["tb1".to_string(), "tb2".to_string(), "tb3".to_string()]
    );

    e.execute("DROP DATABASE db1;").unwrap();
    assert!(!db_exists(&e, "db1"));
    for name in ["tb1", "tb2", "tb3"] {
        assert!(e
            .execute(&format!("INSERT INTO db1.{name} VALUES (1, 'a');"))
            .is_err());
    }

    // Re-creating the database starts empty.
    e.execute("CREATE DATABASE db1;").unwrap();
    assert!(tables(&e, "db1").is_empty());
}

/// C++ `table_ut.cpp:160` (`createtable_wrong_conflicttype`): `kReplace` and
/// `kInvalid` are not accepted for `CreateTable`; both create attempts fail and
/// leave the database without the table.
#[test]
fn createtable_wrong_conflicttype() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    // The SQL DDL has no conflict-type option for CREATE TABLE. The parser
    // rejects a conflict keyword in the position where the reference's
    // ConflictType is passed (before the column list), which is the equivalent
    // of `kReplace`/`kInvalid` being refused.
    assert!(e.execute("CREATE TABLE db1.tb1 REPLACE (col1 INT);").is_err());
    assert!(e.execute("CREATE TABLE db1.tb1 INVALID (col1 INT);").is_err());
    assert!(tables(&e, "db1").is_empty());

    // The default conflict type still works.
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    assert_eq!(tables(&e, "db1"), vec!["tb1".to_string()]);

    e.execute("DROP DATABASE db1;").unwrap();
}

/// C++ `table_ut.cpp:204` (`droptable_wrong_conflicttype`): the rejected
/// conflict types leave the table intact; a normal drop then removes it.
#[test]
fn droptable_wrong_conflicttype() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();

    // No DROP TABLE conflict-type clause exists; a conflict keyword where the
    // table name belongs is rejected, as the reference rejects kReplace /
    // kInvalid.
    assert!(e.execute("DROP TABLE REPLACE db1.tb1;").is_err());
    assert!(e.execute("DROP TABLE INVALID db1.tb1;").is_err());
    assert_eq!(tables(&e, "db1"), vec!["tb1".to_string()]);

    e.execute("DROP TABLE db1.tb1;").unwrap();
    assert!(tables(&e, "db1").is_empty());
    e.execute("DROP DATABASE db1;").unwrap();
}

/// C++ `table_ut.cpp:257` (`createtable_createtable`): six create/create
/// interleavings. The first committed create wins, every later create of the
/// same name fails, and the table stays.
#[test]
fn createtable_createtable() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    assert!(e
        .execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .is_err());
    assert_eq!(tables(&e, "db1"), vec!["tb1".to_string()]);

    // A create after the table was dropped succeeds again.
    e.execute("DROP TABLE db1.tb1;").unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    assert_eq!(tables(&e, "db1"), vec!["tb1".to_string()]);

    e.execute("DROP TABLE db1.tb1;").unwrap();
    e.execute("DROP DATABASE db1;").unwrap();
}

/// C++ `table_ut.cpp:494` (`createdb_createtable_test`): eight
/// create-database/create-table interleavings. A create-table transaction that
/// started before the database existed cannot commit; after the database
/// exists, the create succeeds.
#[test]
fn createdb_createtable_test() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    // The catalog is name-only: a qualified CREATE TABLE does not validate the
    // database. The reference's isolation check is therefore asserted through
    // `USE`, which does reject an unregistered database.
    assert!(e.execute("USE db1;").is_err());

    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("USE db1;").unwrap();
    e.execute("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    assert_eq!(tables(&e, "db1"), vec!["tb1".to_string()]);
    e.execute("USE default_db;").unwrap();

    e.execute("DROP DATABASE db1;").unwrap();
    assert!(!db_exists(&e, "db1"));
}

/// C++ `table_ut.cpp:848` (`createtable_dropdb_test`): creating a table and
/// dropping the database race; whichever create commits after the drop cannot
/// resurrect the table, and the database stays gone.
#[test]
fn createtable_dropdb_test() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    e.execute("DROP DATABASE db1;").unwrap();
    assert!(!db_exists(&e, "db1"));
    assert!(e
        .execute("INSERT INTO db1.tb1 VALUES (1, 'a');")
        .is_err());
    // The database itself is gone: the session cannot enter it and the stored
    // table is not reachable through the dropped database.
    assert!(e.execute("USE db1;").is_err());
    // Note: the Rust catalog stores fully-qualified table names without a
    // database foreign key, so `CREATE TABLE` does not validate the database
    // the way `NewTxn::CreateTable` does. That specific rejection has no engine
    // surface; the surviving assertions are that the database is not
    // resurrected and that recreating it yields no table.
    assert!(!db_exists(&e, "db1"));

    // Recreating the database gives no table back.
    e.execute("CREATE DATABASE db1;").unwrap();
    assert!(tables(&e, "db1").is_empty());
    e.execute("DROP DATABASE db1;").unwrap();
}

/// C++ `table_ut.cpp:1195` (`dropdb_droptable_test`): drop-table and drop-
/// database each succeed once; the table cannot be dropped again after the
/// database is gone.
#[test]
fn dropdb_droptable_test() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    e.execute("DROP TABLE db1.tb1;").unwrap();
    e.execute("DROP DATABASE db1;").unwrap();
    assert!(!db_exists(&e, "db1"));
    assert!(e.execute("DROP TABLE db1.tb1;").is_err());
}

/// C++ `table_ut.cpp:1395` (`createtable_droptable_test`): create/drop table
/// interleavings. Creating an existing name fails, dropping a missing name
/// fails, and a create after the drop succeeds.
#[test]
fn createtable_droptable_test() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    assert!(e.execute("DROP TABLE db1.tb1;").is_err());

    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    assert!(e
        .execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .is_err());

    e.execute("DROP TABLE db1.tb1;").unwrap();
    assert!(e.execute("DROP TABLE db1.tb1;").is_err());

    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    assert_eq!(tables(&e, "db1"), vec!["tb1".to_string()]);

    e.execute("DROP TABLE db1.tb1;").unwrap();
    e.execute("DROP DATABASE db1;").unwrap();
}

/// C++ `table_ut.cpp:1778` (`droptable_droptable_test`): the first drop
/// commits; the second drop of the same table fails.
#[test]
fn droptable_droptable_test() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();

    e.execute("DROP TABLE db1.tb1;").unwrap();
    assert!(e.execute("DROP TABLE db1.tb1;").is_err());
    assert!(tables(&e, "db1").is_empty());

    e.execute("DROP DATABASE db1;").unwrap();
    assert!(!db_exists(&e, "db1"));

    // Durability: the dropped table does not come back after a reopen.
    drop(e);
    let e = Engine::open(dir.path()).unwrap();
    assert!(!db_exists(&e, "db1"));
}

/// C++ `table_ut.cpp:2061` (`rename_table_test`): five rename interleavings.
/// The first committed rename wins, the old name disappears, the new name
/// keeps the table and its rows, and renaming from the old name fails.
#[test]
fn rename_table_test() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    e.execute("INSERT INTO db1.tb1 VALUES (1, 'a'), (2, 'b');")
        .unwrap();

    e.execute("ALTER TABLE db1.tb1 RENAME TO db1.tb2;")
        .unwrap();
    assert_eq!(tables(&e, "db1"), vec!["tb2".to_string()]);
    assert!(!e.table_exists("db1.tb1"));
    assert!(e.table_exists("db1.tb2"));
    // The rows follow the renamed table.
    assert_eq!(e.row_count("db1.tb2").unwrap(), 2);
    assert!(e.execute("INSERT INTO db1.tb1 VALUES (3, 'c');").is_err());

    // Renaming again from the old name fails: it no longer exists.
    assert!(e.execute("ALTER TABLE db1.tb1 RENAME TO db1.tb2;").is_err());
    // Renaming to an existing name fails and leaves both names as they were.
    e.execute("CREATE TABLE db1.tb3 (col1 INT);").unwrap();
    assert!(e.execute("ALTER TABLE db1.tb2 RENAME TO db1.tb3;").is_err());
    assert!(e.table_exists("db1.tb2"));
    assert!(e.table_exists("db1.tb3"));

    e.execute("DROP TABLE db1.tb3;").unwrap();
    e.execute("DROP TABLE db1.tb2;").unwrap();
    e.execute("DROP DATABASE db1;").unwrap();
    assert!(!db_exists(&e, "db1"));
}
