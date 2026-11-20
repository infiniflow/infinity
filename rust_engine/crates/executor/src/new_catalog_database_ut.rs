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

async fn rows(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in engine.execute(sql).await.unwrap() {
        if let QueryResult::Rows { .. } = result {
            return result.rows_text();
        }
    }
    Vec::new()
}

async fn databases(engine: &Engine) -> Vec<String> {
    let mut names: Vec<String> = rows(engine, "SHOW DATABASES;")
        .await
        .into_iter()
        .map(|row| row[0].clone())
        .collect();
    names.sort();
    names
}

async fn db_exists(engine: &Engine, name: &str) -> bool {
    databases(engine).await.iter().any(|db| db == name)
}

/// C++ `database_ut.cpp:45` (`createdb_createdb_test`): six create/create
/// interleavings. The first committed create wins; every later create without
/// `IF NOT EXISTS` fails, and a drop removes the database.
#[tokio::test]
async fn createdb_createdb_test() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();

    // t1 commits the create before t2's create: t2 fails.
    e.execute("CREATE DATABASE db1;").await.unwrap();
    assert!(e.execute("CREATE DATABASE db1;").await.is_err());
    // `IF NOT EXISTS` is the reference's kIgnore path.
    e.execute("CREATE DATABASE IF NOT EXISTS db1;")
        .await
        .unwrap();
    assert!(db_exists(&e, "db1").await);
    assert_eq!(
        rows(&e, "SHOW DATABASES;")
            .await
            .into_iter()
            .filter(|row| row[0] == "db1")
            .count(),
        1
    );

    // t1 drops after its create committed: the database disappears.
    e.execute("DROP DATABASE db1;").await.unwrap();
    assert!(!db_exists(&e, "db1").await);
    // Dropping an absent database with kError fails.
    assert!(e.execute("DROP DATABASE db1;").await.is_err());

    // A fresh create starts empty.
    e.execute("CREATE DATABASE db1;").await.unwrap();
    e.execute("USE db1;").await.unwrap();
    assert!(rows(&e, "SHOW TABLES;").await.is_empty());
    e.execute("USE default_db;").await.unwrap();
    e.execute("DROP DATABASE db1;").await.unwrap();

    // The commits are durable: reopen on the same directory sees no db1.
    drop(e);
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    assert!(!db_exists(&e, "db1").await);
}

/// C++ `database_ut.cpp:315` (`db_test2`): dropping a missing database fails
/// with kError and succeeds with kIgnore (`DROP DATABASE IF EXISTS`).
#[tokio::test]
async fn db_test2() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();

    assert!(e.execute("DROP DATABASE db1;").await.is_err());
    e.execute("DROP DATABASE IF EXISTS db1;").await.unwrap();
    assert!(!db_exists(&e, "db1").await);
    // The default database is untouched.
    assert!(db_exists(&e, "default_db").await);
}

/// C++ `database_ut.cpp:337` (`db_test3`): duplicate create fails with
/// kError, succeeds with kIgnore, and the normal drop removes the database.
#[tokio::test]
async fn db_test3() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();

    e.execute("CREATE DATABASE db1;").await.unwrap();
    assert!(e.execute("CREATE DATABASE db1;").await.is_err());
    e.execute("CREATE DATABASE IF NOT EXISTS db1;")
        .await
        .unwrap();
    assert_eq!(
        databases(&e).await,
        vec!["db1".to_string(), "default_db".to_string()]
    );

    e.execute("DROP DATABASE db1;").await.unwrap();
    assert!(!db_exists(&e, "db1").await);
    assert!(db_exists(&e, "default_db").await);
}

/// C++ `database_ut.cpp:374` (`createdb_wrong_conflicttype`): `kReplace` and
/// `kInvalid` are not accepted for `CreateDatabase`, so the create fails and no
/// database is created.
#[tokio::test]
async fn createdb_wrong_conflicttype() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();

    // The SQL surface has no REPLACE/INVALID keyword for CREATE DATABASE; the
    // parser rejects both, which is the reference's non-ok status.
    assert!(e.execute("CREATE DATABASE db1 REPLACE;").await.is_err());
    assert!(e.execute("CREATE DATABASE db1 INVALID;").await.is_err());
    assert!(!db_exists(&e, "db1").await);

    // The normal conflict type still works.
    e.execute("CREATE DATABASE db1;").await.unwrap();
    assert!(db_exists(&e, "db1").await);
}

/// C++ `database_ut.cpp:396` (`dropdb_wrong_conflicttype`): `kReplace` and
/// `kInvalid` are rejected for `DropDatabase`; the database is left intact and
/// a normal drop then succeeds.
#[tokio::test]
async fn dropdb_wrong_conflicttype() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();

    e.execute("CREATE DATABASE db1;").await.unwrap();
    assert!(e.execute("DROP DATABASE db1 REPLACE;").await.is_err());
    assert!(e.execute("DROP DATABASE db1 INVALID;").await.is_err());
    assert!(db_exists(&e, "db1").await);

    // Still usable after the rejected drops.
    e.execute("USE db1;").await.unwrap();
    e.execute("CREATE TABLE t (id INT);").await.unwrap();
    e.execute("INSERT INTO t VALUES (1);").await.unwrap();
    e.execute("USE default_db;").await.unwrap();

    e.execute("DROP DATABASE db1;").await.unwrap();
    assert!(!db_exists(&e, "db1").await);
}

/// C++ `database_ut.cpp:434` (`dropdb_dropdb_test`): seven drop/drop
/// interleavings. Both transactions stage a drop, but only the first commit
/// wins; the second fails and the database is gone.
#[tokio::test]
async fn dropdb_dropdb_test() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();

    e.execute("CREATE DATABASE db1;").await.unwrap();
    e.execute("DROP DATABASE db1;").await.unwrap();
    // The second drop reaches the same key after the first commit and fails.
    assert!(e.execute("DROP DATABASE db1;").await.is_err());
    assert!(!db_exists(&e, "db1").await);

    // A later create starts from an empty database again.
    e.execute("CREATE DATABASE db1;").await.unwrap();
    e.execute("USE db1;").await.unwrap();
    assert!(rows(&e, "SHOW TABLES;").await.is_empty());
    e.execute("USE default_db;").await.unwrap();
    e.execute("DROP DATABASE db1;").await.unwrap();

    drop(e);
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    assert!(!db_exists(&e, "db1").await);
}

/// C++ `database_ut.cpp:606` (`createdb_dropdb_test`): eight create/drop
/// interleavings. A drop that starts before the create commits fails; after the
/// create commits the drop succeeds and nothing remains.
#[tokio::test]
async fn createdb_dropdb_test() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();

    // Drop before create: the database is not visible, so the drop fails.
    assert!(e.execute("DROP DATABASE db1;").await.is_err());
    // The create then commits.
    e.execute("CREATE DATABASE db1;").await.unwrap();
    assert!(db_exists(&e, "db1").await);
    // The winner's drop succeeds.
    e.execute("DROP DATABASE db1;").await.unwrap();
    assert!(!db_exists(&e, "db1").await);

    // The same rules hold when the create/drop order is repeated.
    e.execute("CREATE DATABASE db1;").await.unwrap();
    assert!(e.execute("DROP DATABASE db1 REPLACE;").await.is_err());
    e.execute("DROP DATABASE db1;").await.unwrap();
    assert!(!db_exists(&e, "db1").await);

    drop(e);
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    assert!(!db_exists(&e, "db1").await);
}
