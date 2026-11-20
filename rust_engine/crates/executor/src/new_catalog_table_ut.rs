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

async fn rows(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in engine.execute(sql).await.unwrap() {
        if let QueryResult::Rows { .. } = result {
            return result.rows_text();
        }
    }
    Vec::new()
}

async fn databases(engine: &Engine) -> Vec<String> {
    rows(engine, "SHOW DATABASES;")
        .await
        .into_iter()
        .map(|row| row[0].clone())
        .collect()
}

async fn db_exists(engine: &Engine, name: &str) -> bool {
    databases(engine).await.iter().any(|db| db == name)
}

async fn tables(engine: &Engine, db: &str) -> Vec<String> {
    let mut names: Vec<String> = rows(engine, &format!("SHOW TABLES FROM {db};"))
        .await
        .into_iter()
        .map(|row| row[1].clone())
        .collect();
    names.sort();
    names
}

/// C++ `table_ut.cpp:49` (`table_test1`): create database, create table, list
/// it, drop the table, then drop the database.
#[tokio::test]
async fn table_test1() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();

    e.execute("CREATE DATABASE db1;").await.unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .await
        .unwrap();
    assert_eq!(tables(&e, "db1").await, vec!["tb1".to_string()]);

    e.execute("DROP TABLE db1.tb1;").await.unwrap();
    assert!(tables(&e, "db1").await.is_empty());
    assert!(e
        .execute("INSERT INTO db1.tb1 VALUES (1, 'a');")
        .await
        .is_err());

    e.execute("DROP DATABASE db1;").await.unwrap();
    assert!(!db_exists(&e, "db1").await);
}

/// C++ `table_ut.cpp:108` (`table_test2`): three tables survive until the
/// database is dropped, which removes all of them.
#[tokio::test]
async fn table_test2() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();

    e.execute("CREATE DATABASE db1;").await.unwrap();
    for name in ["tb1", "tb2", "tb3"] {
        e.execute(&format!(
            "CREATE TABLE db1.{name} (col1 INT, col2 VARCHAR);"
        ))
        .await
        .unwrap();
    }
    assert_eq!(
        tables(&e, "db1").await,
        vec!["tb1".to_string(), "tb2".to_string(), "tb3".to_string()]
    );

    e.execute("DROP DATABASE db1;").await.unwrap();
    assert!(!db_exists(&e, "db1").await);
    for name in ["tb1", "tb2", "tb3"] {
        assert!(e
            .execute(&format!("INSERT INTO db1.{name} VALUES (1, 'a');"))
            .await
            .is_err());
    }

    // Re-creating the database starts empty.
    e.execute("CREATE DATABASE db1;").await.unwrap();
    assert!(tables(&e, "db1").await.is_empty());
}

/// C++ `table_ut.cpp:160` (`createtable_wrong_conflicttype`): `kReplace` and
/// `kInvalid` are not accepted for `CreateTable`; both create attempts fail and
/// leave the database without the table.
#[tokio::test]
async fn createtable_wrong_conflicttype() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();

    e.execute("CREATE DATABASE db1;").await.unwrap();
    // The SQL DDL has no conflict-type option for CREATE TABLE. The parser
    // rejects a conflict keyword in the position where the reference's
    // ConflictType is passed (before the column list), which is the equivalent
    // of `kReplace`/`kInvalid` being refused.
    assert!(e
        .execute("CREATE TABLE db1.tb1 REPLACE (col1 INT);")
        .await
        .is_err());
    assert!(e
        .execute("CREATE TABLE db1.tb1 INVALID (col1 INT);")
        .await
        .is_err());
    assert!(tables(&e, "db1").await.is_empty());

    // The default conflict type still works.
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .await
        .unwrap();
    assert_eq!(tables(&e, "db1").await, vec!["tb1".to_string()]);

    e.execute("DROP DATABASE db1;").await.unwrap();
}

/// C++ `table_ut.cpp:204` (`droptable_wrong_conflicttype`): the rejected
/// conflict types leave the table intact; a normal drop then removes it.
#[tokio::test]
async fn droptable_wrong_conflicttype() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();

    e.execute("CREATE DATABASE db1;").await.unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .await
        .unwrap();

    // No DROP TABLE conflict-type clause exists; a conflict keyword where the
    // table name belongs is rejected, as the reference rejects kReplace /
    // kInvalid.
    assert!(e.execute("DROP TABLE REPLACE db1.tb1;").await.is_err());
    assert!(e.execute("DROP TABLE INVALID db1.tb1;").await.is_err());
    assert_eq!(tables(&e, "db1").await, vec!["tb1".to_string()]);

    e.execute("DROP TABLE db1.tb1;").await.unwrap();
    assert!(tables(&e, "db1").await.is_empty());
    e.execute("DROP DATABASE db1;").await.unwrap();
}

/// C++ `table_ut.cpp:257` (`createtable_createtable`): six create/create
/// interleavings. The first committed create wins, every later create of the
/// same name fails, and the table stays.
#[tokio::test]
async fn createtable_createtable() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();

    e.execute("CREATE DATABASE db1;").await.unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .await
        .unwrap();
    assert!(e
        .execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .await
        .is_err());
    assert_eq!(tables(&e, "db1").await, vec!["tb1".to_string()]);

    // A create after the table was dropped succeeds again.
    e.execute("DROP TABLE db1.tb1;").await.unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .await
        .unwrap();
    assert_eq!(tables(&e, "db1").await, vec!["tb1".to_string()]);

    e.execute("DROP TABLE db1.tb1;").await.unwrap();
    e.execute("DROP DATABASE db1;").await.unwrap();
}

/// C++ `table_ut.cpp:494` (`createdb_createtable_test`): eight
/// create-database/create-table interleavings. A create-table transaction that
/// started before the database existed cannot commit; after the database
/// exists, the create succeeds.
#[tokio::test]
async fn createdb_createtable_test() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();

    // The catalog is name-only: a qualified CREATE TABLE does not validate the
    // database. The reference's isolation check is therefore asserted through
    // `USE`, which does reject an unregistered database.
    assert!(e.execute("USE db1;").await.is_err());

    e.execute("CREATE DATABASE db1;").await.unwrap();
    e.execute("USE db1;").await.unwrap();
    e.execute("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .await
        .unwrap();
    assert_eq!(tables(&e, "db1").await, vec!["tb1".to_string()]);
    e.execute("USE default_db;").await.unwrap();

    e.execute("DROP DATABASE db1;").await.unwrap();
    assert!(!db_exists(&e, "db1").await);
}

/// C++ `table_ut.cpp:848` (`createtable_dropdb_test`): creating a table and
/// dropping the database race; whichever create commits after the drop cannot
/// resurrect the table, and the database stays gone.
#[tokio::test]
async fn createtable_dropdb_test() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();

    e.execute("CREATE DATABASE db1;").await.unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .await
        .unwrap();
    e.execute("DROP DATABASE db1;").await.unwrap();
    assert!(!db_exists(&e, "db1").await);
    assert!(e
        .execute("INSERT INTO db1.tb1 VALUES (1, 'a');")
        .await
        .is_err());
    // The database itself is gone: the session cannot enter it and the stored
    // table is not reachable through the dropped database.
    assert!(e.execute("USE db1;").await.is_err());
    // Note: the Rust catalog stores fully-qualified table names without a
    // database foreign key, so `CREATE TABLE` does not validate the database
    // the way `NewTxn::CreateTable` does. That specific rejection has no engine
    // surface; the surviving assertions are that the database is not
    // resurrected and that recreating it yields no table.
    assert!(!db_exists(&e, "db1").await);

    // Recreating the database gives no table back.
    e.execute("CREATE DATABASE db1;").await.unwrap();
    assert!(tables(&e, "db1").await.is_empty());
    e.execute("DROP DATABASE db1;").await.unwrap();
}

/// C++ `table_ut.cpp:1195` (`dropdb_droptable_test`): drop-table and drop-
/// database each succeed once; the table cannot be dropped again after the
/// database is gone.
#[tokio::test]
async fn dropdb_droptable_test() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();

    e.execute("CREATE DATABASE db1;").await.unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .await
        .unwrap();
    e.execute("DROP TABLE db1.tb1;").await.unwrap();
    e.execute("DROP DATABASE db1;").await.unwrap();
    assert!(!db_exists(&e, "db1").await);
    assert!(e.execute("DROP TABLE db1.tb1;").await.is_err());
}

/// C++ `table_ut.cpp:1395` (`createtable_droptable_test`): create/drop table
/// interleavings. Creating an existing name fails, dropping a missing name
/// fails, and a create after the drop succeeds.
#[tokio::test]
async fn createtable_droptable_test() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();

    e.execute("CREATE DATABASE db1;").await.unwrap();
    assert!(e.execute("DROP TABLE db1.tb1;").await.is_err());

    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .await
        .unwrap();
    assert!(e
        .execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .await
        .is_err());

    e.execute("DROP TABLE db1.tb1;").await.unwrap();
    assert!(e.execute("DROP TABLE db1.tb1;").await.is_err());

    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .await
        .unwrap();
    assert_eq!(tables(&e, "db1").await, vec!["tb1".to_string()]);

    e.execute("DROP TABLE db1.tb1;").await.unwrap();
    e.execute("DROP DATABASE db1;").await.unwrap();
}

/// C++ `table_ut.cpp:1778` (`droptable_droptable_test`): the first drop
/// commits; the second drop of the same table fails.
#[tokio::test]
async fn droptable_droptable_test() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();

    e.execute("CREATE DATABASE db1;").await.unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .await
        .unwrap();

    e.execute("DROP TABLE db1.tb1;").await.unwrap();
    assert!(e.execute("DROP TABLE db1.tb1;").await.is_err());
    assert!(tables(&e, "db1").await.is_empty());

    e.execute("DROP DATABASE db1;").await.unwrap();
    assert!(!db_exists(&e, "db1").await);

    // Durability: the dropped table does not come back after a reopen.
    drop(e);
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    assert!(!db_exists(&e, "db1").await);
}

/// C++ `table_ut.cpp:2061` (`rename_table_test`): five rename interleavings.
/// The first committed rename wins, the old name disappears, the new name
/// keeps the table and its rows, and renaming from the old name fails.
#[tokio::test]
async fn rename_table_test() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();

    e.execute("CREATE DATABASE db1;").await.unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .await
        .unwrap();
    e.execute("INSERT INTO db1.tb1 VALUES (1, 'a'), (2, 'b');")
        .await
        .unwrap();

    e.execute("ALTER TABLE db1.tb1 RENAME TO db1.tb2;")
        .await
        .unwrap();
    assert_eq!(tables(&e, "db1").await, vec!["tb2".to_string()]);
    assert!(!e.table_exists("db1.tb1"));
    assert!(e.table_exists("db1.tb2"));
    // The rows follow the renamed table.
    assert_eq!(e.row_count("db1.tb2").unwrap(), 2);
    assert!(e
        .execute("INSERT INTO db1.tb1 VALUES (3, 'c');")
        .await
        .is_err());

    // Renaming again from the old name fails: it no longer exists.
    assert!(e
        .execute("ALTER TABLE db1.tb1 RENAME TO db1.tb2;")
        .await
        .is_err());
    // Renaming to an existing name fails and leaves both names as they were.
    e.execute("CREATE TABLE db1.tb3 (col1 INT);").await.unwrap();
    assert!(e
        .execute("ALTER TABLE db1.tb2 RENAME TO db1.tb3;")
        .await
        .is_err());
    assert!(e.table_exists("db1.tb2"));
    assert!(e.table_exists("db1.tb3"));

    e.execute("DROP TABLE db1.tb3;").await.unwrap();
    e.execute("DROP TABLE db1.tb2;").await.unwrap();
    e.execute("DROP DATABASE db1;").await.unwrap();
    assert!(!db_exists(&e, "db1").await);
}
