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

//! Engine-level ports of the reference's `new_catalog` transaction tests
//! (`src/unit_test/storage/new_catalog/*.cpp`).
//!
//! The reference fixtures drive `NewTxn`/`NewCatalog` directly and inspect
//! `SegmentMeta`/`BlockMeta` visibility. The Rust engine exposes the same
//! behavior through [`Engine::execute`] plus [`Engine::row_count`] and
//! [`Engine::segments`], so each test keeps the reference's assertions and
//! maps its metadata checks to those APIs. Transaction-conflict cases that the
//! reference expresses with two live `NewTxn` handles are covered separately by
//! the executor's `delete_conflicts_follow_the_reference` test.
use crate::{Engine, QueryResult};
use ::common::Result;
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
async fn count(engine: &Engine, table: &str) -> usize {
    let table = if table.contains('.') {
        table.to_string()
    } else {
        format!("default_db.{table}")
    };
    let sql = format!("SELECT count(*) FROM {table};");
    rows(engine, &sql).await[0][0].parse().unwrap()
}
/// Sum of the rows the engine attributes to the table's segments.
fn segment_rows(engine: &Engine, table: &str) -> usize {
    engine
        .segments(table)
        .iter()
        .map(|(_, rows)| *rows as usize)
        .sum()
}
async fn insert_rows(engine: &Engine, table: &str, rows: Vec<Vec<Literal>>) -> Result<()> {
    engine.insert_literals(table, None, rows).await.map(|_| ())
}
/// C++: `src/unit_test/storage/new_catalog/database_ut.cpp:45`
/// (`createdb_createdb_test`), `:434` (`dropdb_dropdb_test`).
#[tokio::test]
async fn create_and_drop_database_conflicts() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE DATABASE db1;").await.unwrap();
    assert!(e.execute("CREATE DATABASE db1;").await.is_err());
    e.execute("CREATE DATABASE IF NOT EXISTS db1;")
        .await
        .unwrap();
    e.execute("USE db1;").await.unwrap();
    e.execute("CREATE TABLE t (id INT);").await.unwrap();
    e.execute("INSERT INTO t VALUES (1);").await.unwrap();
    e.execute("USE default_db;").await.unwrap();
    e.execute("DROP DATABASE db1;").await.unwrap();
    assert!(e.execute("INSERT INTO db1.t VALUES (2);").await.is_err());
    e.execute("CREATE DATABASE db1;").await.unwrap();
    e.execute("USE db1;").await.unwrap();
    assert!(!e.table_exists("t"));
}
/// C++: `src/unit_test/storage/new_catalog/table_ut.cpp:49` (`table_test1`),
/// `:108` (`table_test2`), `:1395` (`createtable_droptable_test`).
#[tokio::test]
async fn create_and_drop_table_conflicts() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE t1 (id INT, name VARCHAR);")
        .await
        .unwrap();
    assert!(e.execute("CREATE TABLE t1 (id INT);").await.is_err());
    e.execute("CREATE TABLE t2 (id INT);").await.unwrap();
    assert!(e.table_exists("t1"));
    assert!(e.table_exists("t2"));
    e.execute("DROP TABLE t1;").await.unwrap();
    assert!(!e.table_exists("t1"));
    assert!(e.execute("INSERT INTO t1 VALUES (1);").await.is_err());
    assert!(e.table_exists("t2"));
    assert!(e.execute("DROP TABLE t1;").await.is_err());
    e.execute("DROP TABLE IF EXISTS t1;").await.unwrap();
}
/// C++: `src/unit_test/storage/new_catalog/append_ut.cpp:118` (`test_append0`)
/// and `:188` (`test_append1`): two committed appends are both visible and the
/// segment metadata accounts for every row.
#[tokio::test]
async fn appends_accumulate_across_commits() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE ap (id INT, name VARCHAR);")
        .await
        .unwrap();
    for block in 0..2 {
        let mut batch = Vec::new();
        for i in 0..4 {
            let id = block * 4 + i;
            batch.push(vec![Literal::Int(id), Literal::Str(format!("name_{id}"))]);
        }
        insert_rows(&e, "default_db.ap", batch).await.unwrap();
    }
    assert_eq!(e.row_count("ap").unwrap(), 8);
    assert_eq!(segment_rows(&e, "ap"), 8);
    let all = rows(&e, "SELECT id, name FROM ap ORDER BY id;").await;
    assert_eq!(all.len(), 8);
    assert_eq!(all[0], vec!["0", "name_0"]);
    assert_eq!(all[7], vec!["7", "name_7"]);
}
/// C++: `src/unit_test/storage/new_catalog/append_ut.cpp:335` (`test_append2`):
/// large appends keep every row and the per-segment counts cover the table.
#[tokio::test]
async fn large_appends_span_blocks() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE big (id INT, name VARCHAR);")
        .await
        .unwrap();
    for batch in 0..2 {
        let rows = (0..8192)
            .map(|i| {
                let id = batch * 8192 + i;
                vec![Literal::Int(id as i64), Literal::Str(format!("v{id}"))]
            })
            .collect();
        insert_rows(&e, "default_db.big", rows).await.unwrap();
    }
    assert_eq!(e.row_count("big").unwrap(), 16384);
    assert_eq!(segment_rows(&e, "big"), 16384);
    assert_eq!(count(&e, "big").await, 16384);
    let boundary = rows(&e, "SELECT name FROM big WHERE id = 8191;").await;
    assert_eq!(boundary, vec![vec!["v8191".to_string()]]);
    let boundary = rows(&e, "SELECT name FROM big WHERE id = 16383;").await;
    assert_eq!(boundary, vec![vec!["v16383".to_string()]]);
}
/// C++: `src/unit_test/storage/new_catalog/append_ut.cpp:511`
/// (`test_append_mismatch`): a block whose column *type* differs from the
/// table's is rejected (`DataTypeMismatch`). The reference's other half, a
/// block with a different column *count*, has no SQL surface: `INSERT` fills
/// omitted trailing columns with defaults, so that shape is not expressible at
/// this layer.
#[tokio::test]
async fn append_type_mismatch_is_rejected() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE mm (id INT, name VARCHAR);")
        .await
        .unwrap();
    assert!(e
        .execute("INSERT INTO mm VALUES ('not_an_int', 'a');")
        .await
        .is_err());
    assert_eq!(e.row_count("mm").unwrap(), 0);
    e.execute("INSERT INTO mm VALUES (1, 'a');").await.unwrap();
    assert_eq!(e.row_count("mm").unwrap(), 1);
}
/// C++: `src/unit_test/storage/new_catalog/append_ut.cpp` (`test_append_drop_db`,
/// `test_append_drop_table`): append after the table or database was dropped
/// fails instead of recreating it.
#[tokio::test]
async fn append_after_drop_is_rejected() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE gone (id INT);").await.unwrap();
    e.execute("DROP TABLE gone;").await.unwrap();
    assert!(e.execute("INSERT INTO gone VALUES (1);").await.is_err());
    assert!(!e.table_exists("gone"));
    e.execute("CREATE DATABASE db2;").await.unwrap();
    e.execute("USE db2;").await.unwrap();
    e.execute("CREATE TABLE t (id INT);").await.unwrap();
    e.execute("USE default_db;").await.unwrap();
    e.execute("DROP DATABASE db2;").await.unwrap();
    assert!(e.execute("INSERT INTO db2.t VALUES (1);").await.is_err());
}
/// C++: `src/unit_test/storage/new_catalog/append_ut.cpp`
/// (`test_append_add_column`, `test_append_drop_column`): rows appended before
/// an ADD COLUMN read back NULL in the new column, and a DROP COLUMN removes it
/// from both old and new rows.
#[tokio::test]
async fn append_add_column_fills_old_rows_with_null() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE ac (id INT, name VARCHAR);")
        .await
        .unwrap();
    e.execute("INSERT INTO ac VALUES (1, 'a'), (2, 'b');")
        .await
        .unwrap();
    e.execute("ALTER TABLE ac ADD COLUMN (extra INT);")
        .await
        .unwrap();
    e.execute("INSERT INTO ac VALUES (3, 'c', 30);")
        .await
        .unwrap();
    assert_eq!(e.row_count("ac").unwrap(), 3);
    let all = rows(&e, "SELECT id, name, extra FROM ac ORDER BY id;").await;
    assert_eq!(
        all,
        vec![
            vec!["1", "a", "null"],
            vec!["2", "b", "null"],
            vec!["3", "c", "30"],
        ]
    );
    e.execute("ALTER TABLE ac DROP COLUMN (extra);")
        .await
        .unwrap();
    let all = rows(&e, "SELECT id, name FROM ac ORDER BY id;").await;
    assert_eq!(all, vec![vec!["1", "a"], vec!["2", "b"], vec!["3", "c"],]);
}
/// C++: `src/unit_test/storage/new_catalog/append_ut.cpp` (`test_append_rename`).
#[tokio::test]
async fn append_uses_the_renamed_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE old_name (id INT);").await.unwrap();
    e.execute("INSERT INTO old_name VALUES (1);").await.unwrap();
    e.execute("ALTER TABLE old_name RENAME TO new_name;")
        .await
        .unwrap();
    assert!(!e.table_exists("old_name"));
    assert!(e.table_exists("new_name"));
    e.execute("INSERT INTO new_name VALUES (2);").await.unwrap();
    assert_eq!(e.row_count("new_name").unwrap(), 2);
    assert!(e.execute("INSERT INTO old_name VALUES (3);").await.is_err());
}
/// C++: `src/unit_test/storage/new_catalog/append_ut.cpp`
/// (`test_append_and_compact`): compaction keeps every appended row.
#[tokio::test]
async fn append_then_compact_preserves_rows() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE acp (id INT, name VARCHAR);")
        .await
        .unwrap();
    let batch = (0..2000)
        .map(|i| vec![Literal::Int(i), Literal::Str(format!("v{i}"))])
        .collect();
    insert_rows(&e, "default_db.acp", batch).await.unwrap();
    e.compact_all_segments("default_db.acp").await.unwrap();
    assert_eq!(e.row_count("acp").unwrap(), 2000);
    assert_eq!(count(&e, "acp").await, 2000);
    let last = rows(&e, "SELECT name FROM acp WHERE id = 1999;").await;
    assert_eq!(last, vec![vec!["v1999".to_string()]]);
}
/// C++: `src/unit_test/storage/new_catalog/append_ut.cpp`
/// (`test_append_and_create_index`, `test_append_and_drop_index`): the index
/// sees rows appended after it was built, and dropping it leaves the data
/// readable.
#[tokio::test]
async fn append_around_secondary_index_lifecycle() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE aix (id INT, name VARCHAR);")
        .await
        .unwrap();
    e.execute("INSERT INTO aix VALUES (1, 'a'), (2, 'b');")
        .await
        .unwrap();
    e.execute("CREATE INDEX aix_name ON aix (name);")
        .await
        .unwrap();
    e.execute("INSERT INTO aix VALUES (3, 'c');").await.unwrap();
    let hit = rows(&e, "SELECT id FROM aix WHERE name = 'c';").await;
    assert_eq!(hit, vec![vec!["3".to_string()]]);
    e.execute("DROP INDEX aix_name ON aix;").await.unwrap();
    let hit = rows(&e, "SELECT id FROM aix WHERE name = 'c';").await;
    assert_eq!(hit, vec![vec!["3".to_string()]]);
    assert_eq!(e.row_count("aix").unwrap(), 3);
}
/// C++: `src/unit_test/storage/new_catalog/delete_ut.cpp:63` (`test_delete`):
/// deleted rows disappear from scans and the count, and a delete-all empties
/// the table.
#[tokio::test]
async fn delete_removes_rows_from_scans() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE del (id INT, name VARCHAR);")
        .await
        .unwrap();
    e.execute("INSERT INTO del VALUES (1,'a'), (2,'b'), (3,'c'), (4,'d');")
        .await
        .unwrap();
    let deleted = e.execute("DELETE FROM del WHERE id = 2;").await.unwrap();
    assert!(matches!(deleted[0], QueryResult::Deleted { count: 1 }));
    assert_eq!(e.row_count("del").unwrap(), 3);
    assert_eq!(
        rows(&e, "SELECT id FROM del ORDER BY id;").await,
        vec![
            vec!["1".to_string()],
            vec!["3".to_string()],
            vec!["4".to_string()]
        ]
    );
    e.execute("DELETE FROM del;").await.unwrap();
    assert_eq!(e.row_count("del").unwrap(), 0);
    assert!(rows(&e, "SELECT id FROM del;").await.is_empty());
}
/// C++: `src/unit_test/storage/new_catalog/delete_ut.cpp:210`
/// (`test_delete_multiple_blocks`): a delete whose predicate only matches the
/// second batch leaves the first batch intact.
#[tokio::test]
async fn delete_across_multiple_blocks() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE dmb (id INT);").await.unwrap();
    for batch in 0..2 {
        let rows = (0..8192)
            .map(|i| vec![Literal::Int(batch * 8192 + i)])
            .collect();
        insert_rows(&e, "default_db.dmb", rows).await.unwrap();
    }
    assert_eq!(e.row_count("dmb").unwrap(), 16384);
    e.execute("DELETE FROM dmb WHERE id >= 8192;")
        .await
        .unwrap();
    assert_eq!(e.row_count("dmb").unwrap(), 8192);
    assert_eq!(count(&e, "dmb").await, 8192);
    let boundary = rows(&e, "SELECT id FROM dmb WHERE id = 8191;").await;
    assert_eq!(boundary, vec![vec!["8191".to_string()]]);
    assert!(rows(&e, "SELECT id FROM dmb WHERE id = 8192;")
        .await
        .is_empty());
}
/// C++: `src/unit_test/storage/new_catalog/delete_ut.cpp`
/// (`test_delete_and_append`, `test_delete_and_compact`): appending after a
/// delete adds live rows, and compaction preserves the tombstones.
#[tokio::test]
async fn delete_then_append_and_compact() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE dac (id INT);").await.unwrap();
    let batch = (0..100).map(|i| vec![Literal::Int(i)]).collect();
    insert_rows(&e, "default_db.dac", batch).await.unwrap();
    e.execute("DELETE FROM dac WHERE id < 10;").await.unwrap();
    assert_eq!(e.row_count("dac").unwrap(), 90);
    let extra = (100..105).map(|i| vec![Literal::Int(i)]).collect();
    insert_rows(&e, "default_db.dac", extra).await.unwrap();
    assert_eq!(e.row_count("dac").unwrap(), 95);
    e.compact_all_segments("default_db.dac").await.unwrap();
    assert_eq!(e.row_count("dac").unwrap(), 95);
    assert_eq!(count(&e, "dac").await, 95);
    assert!(rows(&e, "SELECT id FROM dac WHERE id = 9;")
        .await
        .is_empty());
}
/// C++: `src/unit_test/storage/new_catalog/update_ut.cpp:348` (`test_update`):
/// an update makes the new values visible and keeps the row count unchanged.
#[tokio::test]
async fn update_replaces_visible_values() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE upd (id INT, name VARCHAR);")
        .await
        .unwrap();
    e.execute("INSERT INTO upd VALUES (1,'a'), (2,'b'), (3,'c'), (4,'d');")
        .await
        .unwrap();
    let updated = e
        .execute("UPDATE upd SET name = 'zzz', id = 100 WHERE id = 2;")
        .await
        .unwrap();
    assert!(matches!(updated[0], QueryResult::Updated { count: 1 }));
    assert_eq!(e.row_count("upd").unwrap(), 4);
    assert_eq!(
        rows(&e, "SELECT name FROM upd WHERE id = 100;").await,
        vec![vec!["zzz".to_string()]]
    );
    assert!(rows(&e, "SELECT name FROM upd WHERE id = 2;")
        .await
        .is_empty());
    e.execute("UPDATE upd SET name = 'y' WHERE id >= 3;")
        .await
        .unwrap();
    assert_eq!(
        rows(&e, "SELECT id, name FROM upd ORDER BY id;").await,
        vec![
            vec!["1", "a"],
            vec!["3", "y"],
            vec!["4", "y"],
            vec!["100", "y"],
        ]
    );
}
/// C++: `src/unit_test/storage/new_catalog/update_ut.cpp:1676`
/// (`test_update_and_delete_conflicts`), `:1835`
/// (`test_update_and_delete_no_conflicts`): update and delete compose; a row
/// deleted first is not updated, and a row updated first can then be deleted.
#[tokio::test]
async fn update_and_delete_compose() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE ud (id INT, name VARCHAR);")
        .await
        .unwrap();
    e.execute("INSERT INTO ud VALUES (1,'a'), (2,'b'), (3,'c');")
        .await
        .unwrap();
    e.execute("UPDATE ud SET name = 'A' WHERE id = 1;")
        .await
        .unwrap();
    e.execute("DELETE FROM ud WHERE id = 1;").await.unwrap();
    assert_eq!(e.row_count("ud").unwrap(), 2);
    let again = e
        .execute("UPDATE ud SET name = 'B' WHERE id = 1;")
        .await
        .unwrap();
    assert!(matches!(again[0], QueryResult::Updated { count: 0 }));
    e.execute("DELETE FROM ud WHERE id = 2;").await.unwrap();
    let noop = e
        .execute("UPDATE ud SET name = 'C' WHERE id = 2;")
        .await
        .unwrap();
    assert!(matches!(noop[0], QueryResult::Updated { count: 0 }));
    assert_eq!(
        rows(&e, "SELECT id, name FROM ud;").await,
        vec![vec!["3", "c"]]
    );
}
/// C++: `src/unit_test/storage/new_catalog/compact_ut.cpp:1448`
/// (`compact_and_compact`): repeated compaction is a no-op on the visible data
/// and keeps the segment accounting consistent.
#[tokio::test]
async fn compaction_is_idempotent() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE cc (id INT, name VARCHAR);")
        .await
        .unwrap();
    let batch = (0..5000)
        .map(|i| vec![Literal::Int(i), Literal::Str(format!("v{i}"))])
        .collect();
    insert_rows(&e, "default_db.cc", batch).await.unwrap();
    e.compact_all_segments("default_db.cc").await.unwrap();
    assert_eq!(e.row_count("cc").unwrap(), 5000);
    e.compact_all_segments("default_db.cc").await.unwrap();
    assert_eq!(e.row_count("cc").unwrap(), 5000);
    assert_eq!(segment_rows(&e, "cc"), 5000);
    assert_eq!(count(&e, "cc").await, 5000);
    assert_eq!(
        rows(&e, "SELECT name FROM cc WHERE id = 4999;").await,
        vec![vec!["v4999".to_string()]]
    );
}
/// C++: `src/unit_test/storage/new_catalog/compact_ut.cpp:749`
/// (`compact_and_add_columns`), `:602` (`compact_and_drop_table`): schema
/// changes after compaction see the compacted rows, and dropping a compacted
/// table removes it.
#[tokio::test]
async fn compact_then_alter_and_drop_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE cad (id INT);").await.unwrap();
    let batch = (0..32).map(|i| vec![Literal::Int(i)]).collect();
    insert_rows(&e, "default_db.cad", batch).await.unwrap();
    e.compact_all_segments("default_db.cad").await.unwrap();
    e.execute("ALTER TABLE cad ADD COLUMN (name VARCHAR);")
        .await
        .unwrap();
    let all = rows(&e, "SELECT id, name FROM cad ORDER BY id;").await;
    assert_eq!(all.len(), 32);
    assert_eq!(all[0], vec!["0", "null"]);
    assert_eq!(all[31], vec!["31", "null"]);
    e.execute("DROP TABLE cad;").await.unwrap();
    assert!(!e.table_exists("cad"));
    assert!(e.execute("INSERT INTO cad VALUES (1);").await.is_err());
}
