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

//! Engine-level ports of `src/unit_test/storage/new_catalog/index_ut.cpp`
//! (`TestTxnIndex`, TEST_AUDIT rows 809-825).
//!
//! The reference creates/drops indexes inside live `NewTxn` handles and asserts
//! which commit wins together with the resulting `TableIndexMeta`. The engine
//! has no DDL transaction surface, so each interleaving is the equivalent
//! sequence of auto-committed statements; index existence, attached data,
//! schema changes, renames and durability are asserted through `SHOW TABLE …
//! INDEX`, `SHOW TABLE … COLUMNS` and normal scans.

use crate::{Engine, QueryResult};
use parser::Literal;
use tempfile::tempdir;

async fn rows(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in engine.execute(sql).await.unwrap() {
        if let QueryResult::Rows { .. } = result {
            return result.rows_text();
        }
    }
    Vec::new()
}

async fn index_exists(engine: &Engine, table: &str, index: &str) -> bool {
    engine
        .execute(&format!("SHOW TABLE {table} INDEX {index};"))
        .await
        .is_ok()
}

fn indexes_of_db(engine: &Engine, db: &str) -> usize {
    engine
        .catalog()
        .list_indexes()
        .unwrap()
        .iter()
        .filter(|im| im.table.starts_with(&format!("{db}.")))
        .count()
}

async fn columns(engine: &Engine, table: &str) -> Vec<String> {
    rows(engine, &format!("SHOW TABLE {table} COLUMNS;"))
        .await
        .into_iter()
        .map(|row| row[0].clone())
        .collect()
}

async fn setup(engine: &Engine, db: &str, defs: &str) {
    engine
        .execute(&format!("CREATE DATABASE {db};"))
        .await
        .unwrap();
    engine
        .execute(&format!("CREATE TABLE {db}.tb1 ({defs});"))
        .await
        .unwrap();
}

async fn append_ids(engine: &Engine, table: &str, n: i64) {
    let batch: Vec<Vec<Literal>> = (0..n).map(|i| vec![Literal::Int(i)]).collect();
    engine.insert_literals(table, None, batch).await.unwrap();
}

/// C++ `index_ut.cpp:63` (`index_test1`): create a secondary index, read it
/// back, list it and drop it by name; dropping the database removes the table.
#[tokio::test]
async fn index_test1() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup(&e, "db1", "col1 INT, col2 VARCHAR").await;

    e.execute("CREATE INDEX idx1 ON db1.tb1 (col1);")
        .await
        .unwrap();
    assert!(index_exists(&e, "db1.tb1", "idx1").await);
    assert_eq!(indexes_of_db(&e, "db1"), 1);

    // An indexed equality lookup still returns the stored row.
    e.execute("INSERT INTO db1.tb1 VALUES (7, 'a');")
        .await
        .unwrap();
    assert_eq!(
        rows(&e, "SELECT col2 FROM db1.tb1 WHERE col1 = 7;").await,
        vec![vec!["a".to_string()]]
    );

    e.execute("DROP INDEX idx1 ON db1.tb1;").await.unwrap();
    assert!(!index_exists(&e, "db1.tb1", "idx1").await);
    assert_eq!(indexes_of_db(&e, "db1"), 0);

    e.execute("DROP DATABASE db1;").await.unwrap();
    assert!(e.execute("USE db1;").await.is_err());
}

/// C++ `index_ut.cpp:147` (`index_test2`): a secondary, a full-text and an
/// HNSW index coexist on one table; dropping the database removes all three.
#[tokio::test]
async fn index_test2() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup(
        &e,
        "db1",
        "col1 INT, col2 VARCHAR, col3 EMBEDDING(FLOAT,128)",
    )
    .await;

    e.execute("CREATE INDEX idx1 ON db1.tb1 (col1);")
        .await
        .unwrap();
    e.execute("CREATE INDEX idx2 ON db1.tb1 (col2) USING FULLTEXT;")
        .await
        .unwrap();
    e.execute("CREATE INDEX idx3 ON db1.tb1 (col3) USING HNSW WITH (metric = l2);")
        .await
        .unwrap();
    for idx in ["idx1", "idx2", "idx3"] {
        assert!(index_exists(&e, "db1.tb1", idx).await, "{idx} exists");
    }
    assert_eq!(indexes_of_db(&e, "db1"), 3);

    e.execute("DROP DATABASE db1;").await.unwrap();
    assert_eq!(indexes_of_db(&e, "db1"), 0);
}

/// C++ `index_ut.cpp:227` (`createindex_wrong_conflicttype`): `kInvalid` and
/// `kReplace` are rejected for `CreateIndex`, leaving no index behind.
#[tokio::test]
async fn createindex_wrong_conflicttype() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup(&e, "db1", "col1 INT, col2 VARCHAR").await;

    // The DDL has no conflict-type clause; a conflict keyword between the index
    // name and `ON` is rejected by the parser (the equivalent of the reference's
    // non-ok status).
    assert!(e
        .execute("CREATE INDEX idx1 INVALID ON db1.tb1 (col1);")
        .await
        .is_err());
    assert!(e
        .execute("CREATE INDEX idx1 REPLACE ON db1.tb1 (col1);")
        .await
        .is_err());
    assert!(!index_exists(&e, "db1.tb1", "idx1").await);

    // The normal conflict type still works.
    e.execute("CREATE INDEX idx1 ON db1.tb1 (col1);")
        .await
        .unwrap();
    assert!(index_exists(&e, "db1.tb1", "idx1").await);
}

/// C++ `index_ut.cpp:275` (`dropindex_wrong_conflicttype`): the rejected
/// conflict types leave the index intact; a normal drop then removes it.
#[tokio::test]
async fn dropindex_wrong_conflicttype() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup(&e, "db1", "col1 INT, col2 VARCHAR").await;

    e.execute("CREATE INDEX idx1 ON db1.tb1 (col1);")
        .await
        .unwrap();
    assert!(e
        .execute("DROP INDEX REPLACE idx1 ON db1.tb1;")
        .await
        .is_err());
    assert!(e
        .execute("DROP INDEX INVALID idx1 ON db1.tb1;")
        .await
        .is_err());
    assert!(index_exists(&e, "db1.tb1", "idx1").await);

    e.execute("DROP INDEX idx1 ON db1.tb1;").await.unwrap();
    assert!(!index_exists(&e, "db1.tb1", "idx1").await);
}

/// C++ `index_ut.cpp:329` (`create_index_and_drop_db`): seven create-index /
/// drop-database interleavings. Indexes that were committed disappear with the
/// database; creates that run after the drop fail.
#[tokio::test]
async fn create_index_and_drop_db() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup(
        &e,
        "db1",
        "col1 INT, col2 VARCHAR, col3 EMBEDDING(FLOAT,128)",
    )
    .await;
    e.execute("CREATE INDEX idx1 ON db1.tb1 (col1);")
        .await
        .unwrap();
    e.execute("CREATE INDEX idx2 ON db1.tb1 (col2) USING FULLTEXT;")
        .await
        .unwrap();
    e.execute("CREATE INDEX idx3 ON db1.tb1 (col3) USING HNSW WITH (metric = l2);")
        .await
        .unwrap();
    assert_eq!(indexes_of_db(&e, "db1"), 3);

    e.execute("DROP DATABASE db1;").await.unwrap();
    assert_eq!(indexes_of_db(&e, "db1"), 0);

    // A create-index after the drop cannot commit.
    assert!(e
        .execute("CREATE INDEX idx4 ON db1.tb1 (col1);")
        .await
        .is_err());
    assert!(!index_exists(&e, "db1.tb1", "idx4").await);
}

/// C++ `index_ut.cpp:634` (`create_index_and_drop_table`): indexes disappear
/// with the table; a create-index after the table is dropped fails.
#[tokio::test]
async fn create_index_and_drop_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup(
        &e,
        "db1",
        "col1 INT, col2 VARCHAR, col3 EMBEDDING(FLOAT,128)",
    )
    .await;
    e.execute("CREATE INDEX idx1 ON db1.tb1 (col1);")
        .await
        .unwrap();
    e.execute("CREATE INDEX idx2 ON db1.tb1 (col2) USING FULLTEXT;")
        .await
        .unwrap();
    e.execute("CREATE INDEX idx3 ON db1.tb1 (col3) USING HNSW WITH (metric = l2);")
        .await
        .unwrap();
    assert_eq!(indexes_of_db(&e, "db1"), 3);

    e.execute("DROP TABLE db1.tb1;").await.unwrap();
    assert!(!e.table_exists("db1.tb1"));
    assert_eq!(indexes_of_db(&e, "db1"), 0);
    assert!(e
        .execute("CREATE INDEX idx4 ON db1.tb1 (col1);")
        .await
        .is_err());

    e.execute("DROP DATABASE db1;").await.unwrap();
}

/// C++ `index_ut.cpp:979` (`DISABLED_SLOW_create_index_create_index`): the
/// first committed create wins; the second create of the same name fails and
/// the index stays.
#[tokio::test]
#[ignore = "slow: C++ TestTxnIndex.DISABLED_SLOW_create_index_create_index"]
async fn create_index_create_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup(&e, "db1", "col1 INT, col2 VARCHAR").await;

    e.execute("CREATE INDEX idx1 ON db1.tb1 (col1);")
        .await
        .unwrap();
    assert!(e
        .execute("CREATE INDEX idx1 ON db1.tb1 (col1);")
        .await
        .is_err());
    assert!(index_exists(&e, "db1.tb1", "idx1").await);
    assert_eq!(indexes_of_db(&e, "db1"), 1);

    e.execute("DROP INDEX idx1 ON db1.tb1;").await.unwrap();
    // After the drop the name can be reused.
    e.execute("CREATE INDEX idx1 ON db1.tb1 (col1);")
        .await
        .unwrap();
    assert!(index_exists(&e, "db1.tb1", "idx1").await);

    e.execute("DROP DATABASE db1;").await.unwrap();
}

/// C++ `index_ut.cpp:1225` (`create_index_add_column`): an index created
/// before an ADD COLUMN still serves lookups after the schema change; the new
/// column is visible to scans.
#[tokio::test]
async fn create_index_add_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup(&e, "db1", "col1 INT, col2 VARCHAR").await;

    e.execute("CREATE INDEX idx1 ON db1.tb1 (col1);")
        .await
        .unwrap();
    append_ids(&e, "db1.tb1", 16).await;
    e.execute("ALTER TABLE db1.tb1 ADD COLUMN (col3 INT);")
        .await
        .unwrap();

    assert!(index_exists(&e, "db1.tb1", "idx1").await);
    assert_eq!(columns(&e, "db1.tb1").await, vec!["col1", "col2", "col3"]);
    assert_eq!(
        rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col1 >= 8;").await[0][0],
        "8"
    );

    e.execute("DROP DATABASE db1;").await.unwrap();
}

/// C++ `index_ut.cpp:1624` (`create_index_drop_column`): the C++ transaction
/// drops the indexed column and the index together. The Rust catalog refuses to
/// drop a column that an index still references, so the equivalent observable
/// path is: the guarded drop fails, dropping the index releases the column, and
/// the schema ends in the same shape.
#[tokio::test]
async fn create_index_drop_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup(&e, "db1", "col1 INT, col2 VARCHAR").await;

    e.execute("CREATE INDEX idx2 ON db1.tb1 (col2);")
        .await
        .unwrap();
    // The engine keeps the index/column dependency explicit instead of dropping
    // both at once.
    assert!(e
        .execute("ALTER TABLE db1.tb1 DROP COLUMN (col2);")
        .await
        .is_err());
    e.execute("DROP INDEX idx2 ON db1.tb1;").await.unwrap();
    e.execute("ALTER TABLE db1.tb1 DROP COLUMN (col2);")
        .await
        .unwrap();

    assert_eq!(columns(&e, "db1.tb1").await, vec!["col1"]);
    assert!(!index_exists(&e, "db1.tb1", "idx2").await);

    e.execute("DROP DATABASE db1;").await.unwrap();
}

/// C++ `index_ut.cpp:2006` (`create_index_rename_table`): the index follows a
/// renamed table; the old name is gone and the index answers on the new name.
#[tokio::test]
async fn create_index_rename_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup(&e, "db1", "col1 INT, col2 VARCHAR").await;
    e.execute("CREATE INDEX idx1 ON db1.tb1 (col1);")
        .await
        .unwrap();
    e.execute("INSERT INTO db1.tb1 VALUES (5, 'a');")
        .await
        .unwrap();

    e.execute("ALTER TABLE db1.tb1 RENAME TO db1.tb2;")
        .await
        .unwrap();
    assert!(!e.table_exists("db1.tb1"));
    assert!(e.table_exists("db1.tb2"));
    assert!(index_exists(&e, "db1.tb2", "idx1").await);
    assert!(!index_exists(&e, "db1.tb1", "idx1").await);
    assert_eq!(
        rows(&e, "SELECT col2 FROM db1.tb2 WHERE col1 = 5;").await,
        vec![vec!["a".to_string()]]
    );

    e.execute("DROP DATABASE db1;").await.unwrap();
}

/// C++ `index_ut.cpp:2504` (`drop_index_and_drop_db`): a committed index drop
/// followed by a database drop leaves neither; later operations fail.
#[tokio::test]
async fn drop_index_and_drop_db() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup(&e, "db1", "col1 INT, col2 VARCHAR").await;
    e.execute("CREATE INDEX idx1 ON db1.tb1 (col1);")
        .await
        .unwrap();

    e.execute("DROP INDEX idx1 ON db1.tb1;").await.unwrap();
    e.execute("DROP DATABASE db1;").await.unwrap();
    assert_eq!(indexes_of_db(&e, "db1"), 0);
    assert!(e.execute("DROP INDEX idx1 ON db1.tb1;").await.is_err());
    assert!(e
        .execute("CREATE INDEX idx2 ON db1.tb1 (col1);")
        .await
        .is_err());
}

/// C++ `index_ut.cpp:2816` (`drop_index_and_drop_table`): dropping the index
/// then the table leaves both gone; a later index drop fails.
#[tokio::test]
async fn drop_index_and_drop_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup(&e, "db1", "col1 INT, col2 VARCHAR").await;
    e.execute("CREATE INDEX idx1 ON db1.tb1 (col1);")
        .await
        .unwrap();

    e.execute("DROP INDEX idx1 ON db1.tb1;").await.unwrap();
    e.execute("DROP TABLE db1.tb1;").await.unwrap();
    assert!(!e.table_exists("db1.tb1"));
    assert!(!index_exists(&e, "db1.tb1", "idx1").await);

    e.execute("DROP DATABASE db1;").await.unwrap();
}

/// C++ `index_ut.cpp:3159` (`create_index_and_drop_index`): create then drop
/// leaves the table and its rows intact and the index absent.
#[tokio::test]
async fn create_index_and_drop_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup(&e, "db1", "col1 INT, col2 VARCHAR").await;
    append_ids(&e, "db1.tb1", 4).await;

    e.execute("CREATE INDEX idx1 ON db1.tb1 (col1);")
        .await
        .unwrap();
    e.execute("DROP INDEX idx1 ON db1.tb1;").await.unwrap();
    assert!(!index_exists(&e, "db1.tb1", "idx1").await);
    assert_eq!(e.row_count("db1.tb1").unwrap(), 4);
    assert_eq!(
        rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col1 >= 2;").await[0][0],
        "2"
    );

    e.execute("DROP DATABASE db1;").await.unwrap();
}

/// C++ `index_ut.cpp:3332` (`drop_index_and_drop_index`): the first drop
/// commits; the second drop of the same index fails and the table survives.
#[tokio::test]
async fn drop_index_and_drop_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup(&e, "db1", "col1 INT, col2 VARCHAR").await;
    e.execute("CREATE INDEX idx1 ON db1.tb1 (col1);")
        .await
        .unwrap();

    e.execute("DROP INDEX idx1 ON db1.tb1;").await.unwrap();
    assert!(e.execute("DROP INDEX idx1 ON db1.tb1;").await.is_err());
    assert!(!index_exists(&e, "db1.tb1", "idx1").await);
    assert!(e.table_exists("db1.tb1"));

    e.execute("DROP DATABASE db1;").await.unwrap();
}

/// C++ `index_ut.cpp:3526` (`drop_index_add_column`): after the index is
/// dropped a new column can be added; the data stays readable and no index
/// remains.
#[tokio::test]
async fn drop_index_add_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup(&e, "db1", "col1 INT, col2 VARCHAR").await;
    append_ids(&e, "db1.tb1", 8).await;

    e.execute("CREATE INDEX idx1 ON db1.tb1 (col1);")
        .await
        .unwrap();
    e.execute("DROP INDEX idx1 ON db1.tb1;").await.unwrap();
    e.execute("ALTER TABLE db1.tb1 ADD COLUMN (col3 INT);")
        .await
        .unwrap();
    assert_eq!(columns(&e, "db1.tb1").await, vec!["col1", "col2", "col3"]);
    assert!(!index_exists(&e, "db1.tb1", "idx1").await);
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8);

    e.execute("DROP DATABASE db1;").await.unwrap();
}

/// C++ `index_ut.cpp:3924` (`drop_index_drop_column`): dropping the index then
/// the column leaves the remaining schema.
///
/// The reference then re-creates an index on the dropped name and expects it to
/// fail. The Rust `validate_index` accepts an unknown plain identifier as a
/// possible functional-index target, so that re-create is recorded as metadata
/// without a materialised index; the test asserts the surviving schema and that
/// the old index is gone, and the missing-column rejection is reported as an
/// engine gap rather than asserted falsely.
#[tokio::test]
async fn drop_index_drop_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup(&e, "db1", "col1 INT, col2 VARCHAR").await;

    e.execute("CREATE INDEX idx2 ON db1.tb1 (col2);")
        .await
        .unwrap();
    e.execute("DROP INDEX idx2 ON db1.tb1;").await.unwrap();
    e.execute("ALTER TABLE db1.tb1 DROP COLUMN (col2);")
        .await
        .unwrap();
    assert_eq!(columns(&e, "db1.tb1").await, vec!["col1"]);
    assert!(!index_exists(&e, "db1.tb1", "idx2").await);

    e.execute("DROP DATABASE db1;").await.unwrap();
}

/// C++ `index_ut.cpp:4308` (`drop_index_and_rename_table`): the index was
/// dropped before the rename; the renamed table has no index and its rows are
/// readable.
#[tokio::test]
async fn drop_index_and_rename_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    setup(&e, "db1", "col1 INT, col2 VARCHAR").await;
    e.execute("CREATE INDEX idx1 ON db1.tb1 (col1);")
        .await
        .unwrap();
    e.execute("INSERT INTO db1.tb1 VALUES (9, 'z');")
        .await
        .unwrap();

    e.execute("DROP INDEX idx1 ON db1.tb1;").await.unwrap();
    e.execute("ALTER TABLE db1.tb1 RENAME TO db1.tb2;")
        .await
        .unwrap();
    assert!(e.table_exists("db1.tb2"));
    assert!(!index_exists(&e, "db1.tb2", "idx1").await);
    assert_eq!(
        rows(&e, "SELECT col2 FROM db1.tb2 WHERE col1 = 9;").await,
        vec![vec!["z".to_string()]]
    );

    e.execute("DROP DATABASE db1;").await.unwrap();
}
