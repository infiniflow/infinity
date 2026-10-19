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
use infinity_common::Result;
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

fn count(engine: &Engine, table: &str) -> usize {
    let table = if table.contains('.') {
        table.to_string()
    } else {
        format!("default_db.{table}")
    };
    let sql = format!("SELECT count(*) FROM {table};");
    rows(engine, &sql)[0][0].parse().unwrap()
}

/// Sum of the rows the engine attributes to the table's segments.
fn segment_rows(engine: &Engine, table: &str) -> usize {
    engine
        .segments(table)
        .iter()
        .map(|(_, rows)| *rows as usize)
        .sum()
}

fn insert_rows(engine: &Engine, table: &str, rows: Vec<Vec<Literal>>) -> Result<()> {
    engine.insert_literals(table, None, rows).map(|_| ())
}

/// C++: `src/unit_test/storage/new_catalog/database_ut.cpp:45`
/// (`createdb_createdb_test`), `:434` (`dropdb_dropdb_test`).
#[test]
fn create_and_drop_database_conflicts() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    // ConflictType::kError: a second create without IF NOT EXISTS fails.
    assert!(e.execute("CREATE DATABASE db1;").is_err());
    e.execute("CREATE DATABASE IF NOT EXISTS db1;").unwrap();
    e.execute("USE db1;").unwrap();
    e.execute("CREATE TABLE t (id INT);").unwrap();
    e.execute("INSERT INTO t VALUES (1);").unwrap();
    e.execute("USE default_db;").unwrap();

    e.execute("DROP DATABASE db1;").unwrap();
    // The database and its tables are gone; a new create starts empty.
    assert!(e.execute("INSERT INTO db1.t VALUES (2);").is_err());
    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("USE db1;").unwrap();
    assert!(!e.table_exists("t"));
}

/// C++: `src/unit_test/storage/new_catalog/table_ut.cpp:49` (`table_test1`),
/// `:108` (`table_test2`), `:1395` (`createtable_droptable_test`).
#[test]
fn create_and_drop_table_conflicts() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE TABLE t1 (id INT, name VARCHAR);")
        .unwrap();
    assert!(e.execute("CREATE TABLE t1 (id INT);").is_err());
    e.execute("CREATE TABLE t2 (id INT);").unwrap();
    assert!(e.table_exists("t1"));
    assert!(e.table_exists("t2"));

    e.execute("DROP TABLE t1;").unwrap();
    assert!(!e.table_exists("t1"));
    assert!(e.execute("INSERT INTO t1 VALUES (1);").is_err());
    assert!(e.table_exists("t2"));

    // Dropping a missing table is only an error without IF EXISTS.
    assert!(e.execute("DROP TABLE t1;").is_err());
    e.execute("DROP TABLE IF EXISTS t1;").unwrap();
}

/// C++: `src/unit_test/storage/new_catalog/append_ut.cpp:118` (`test_append0`)
/// and `:188` (`test_append1`): two committed appends are both visible and the
/// segment metadata accounts for every row.
#[test]
fn appends_accumulate_across_commits() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE ap (id INT, name VARCHAR);")
        .unwrap();

    for block in 0..2 {
        let mut batch = Vec::new();
        for i in 0..4 {
            let id = block * 4 + i;
            batch.push(vec![Literal::Int(id), Literal::Str(format!("name_{id}"))]);
        }
        insert_rows(&e, "default_db.ap", batch).unwrap();
    }

    assert_eq!(e.row_count("ap").unwrap(), 8);
    assert_eq!(segment_rows(&e, "ap"), 8);
    let all = rows(&e, "SELECT id, name FROM ap ORDER BY id;");
    assert_eq!(all.len(), 8);
    assert_eq!(all[0], vec!["0", "name_0"]);
    assert_eq!(all[7], vec!["7", "name_7"]);
}

/// C++: `src/unit_test/storage/new_catalog/append_ut.cpp:335` (`test_append2`):
/// large appends keep every row and the per-segment counts cover the table.
#[test]
fn large_appends_span_blocks() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE big (id INT, name VARCHAR);")
        .unwrap();

    for batch in 0..2 {
        let rows = (0..8192)
            .map(|i| {
                let id = batch * 8192 + i;
                vec![Literal::Int(id as i64), Literal::Str(format!("v{id}"))]
            })
            .collect();
        insert_rows(&e, "default_db.big", rows).unwrap();
    }

    assert_eq!(e.row_count("big").unwrap(), 16384);
    assert_eq!(segment_rows(&e, "big"), 16384);
    assert_eq!(count(&e, "big"), 16384);
    let boundary = rows(&e, "SELECT name FROM big WHERE id = 8191;");
    assert_eq!(boundary, vec![vec!["v8191".to_string()]]);
    let boundary = rows(&e, "SELECT name FROM big WHERE id = 16383;");
    assert_eq!(boundary, vec![vec!["v16383".to_string()]]);
}

/// C++: `src/unit_test/storage/new_catalog/append_ut.cpp:511`
/// (`test_append_mismatch`): a block whose column *type* differs from the
/// table's is rejected (`DataTypeMismatch`). The reference's other half, a
/// block with a different column *count*, has no SQL surface: `INSERT` fills
/// omitted trailing columns with defaults, so that shape is not expressible at
/// this layer.
#[test]
fn append_type_mismatch_is_rejected() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE mm (id INT, name VARCHAR);")
        .unwrap();

    assert!(e
        .execute("INSERT INTO mm VALUES ('not_an_int', 'a');")
        .is_err());
    assert_eq!(e.row_count("mm").unwrap(), 0);
    // The matching shape still works.
    e.execute("INSERT INTO mm VALUES (1, 'a');").unwrap();
    assert_eq!(e.row_count("mm").unwrap(), 1);
}

/// C++: `src/unit_test/storage/new_catalog/append_ut.cpp` (`test_append_drop_db`,
/// `test_append_drop_table`): append after the table or database was dropped
/// fails instead of recreating it.
#[test]
fn append_after_drop_is_rejected() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE TABLE gone (id INT);").unwrap();
    e.execute("DROP TABLE gone;").unwrap();
    assert!(e.execute("INSERT INTO gone VALUES (1);").is_err());
    assert!(!e.table_exists("gone"));

    e.execute("CREATE DATABASE db2;").unwrap();
    e.execute("USE db2;").unwrap();
    e.execute("CREATE TABLE t (id INT);").unwrap();
    e.execute("USE default_db;").unwrap();
    e.execute("DROP DATABASE db2;").unwrap();
    assert!(e.execute("INSERT INTO db2.t VALUES (1);").is_err());
}

/// C++: `src/unit_test/storage/new_catalog/append_ut.cpp`
/// (`test_append_add_column`, `test_append_drop_column`): rows appended before
/// an ADD COLUMN read back NULL in the new column, and a DROP COLUMN removes it
/// from both old and new rows.
#[test]
fn append_add_column_fills_old_rows_with_null() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE ac (id INT, name VARCHAR);")
        .unwrap();
    e.execute("INSERT INTO ac VALUES (1, 'a'), (2, 'b');")
        .unwrap();

    e.execute("ALTER TABLE ac ADD COLUMN (extra INT);").unwrap();
    e.execute("INSERT INTO ac VALUES (3, 'c', 30);").unwrap();

    assert_eq!(e.row_count("ac").unwrap(), 3);
    let all = rows(&e, "SELECT id, name, extra FROM ac ORDER BY id;");
    assert_eq!(
        all,
        vec![
            vec!["1", "a", "null"],
            vec!["2", "b", "null"],
            vec!["3", "c", "30"],
        ]
    );

    e.execute("ALTER TABLE ac DROP COLUMN (extra);").unwrap();
    let all = rows(&e, "SELECT id, name FROM ac ORDER BY id;");
    assert_eq!(all, vec![vec!["1", "a"], vec!["2", "b"], vec!["3", "c"],]);
}

/// C++: `src/unit_test/storage/new_catalog/append_ut.cpp` (`test_append_rename`).
#[test]
fn append_uses_the_renamed_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE old_name (id INT);").unwrap();
    e.execute("INSERT INTO old_name VALUES (1);").unwrap();

    e.execute("ALTER TABLE old_name RENAME TO new_name;")
        .unwrap();
    assert!(!e.table_exists("old_name"));
    assert!(e.table_exists("new_name"));
    e.execute("INSERT INTO new_name VALUES (2);").unwrap();

    assert_eq!(e.row_count("new_name").unwrap(), 2);
    assert!(e.execute("INSERT INTO old_name VALUES (3);").is_err());
}

/// C++: `src/unit_test/storage/new_catalog/append_ut.cpp`
/// (`test_append_and_compact`): compaction keeps every appended row.
#[test]
fn append_then_compact_preserves_rows() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE acp (id INT, name VARCHAR);")
        .unwrap();
    let batch = (0..2000)
        .map(|i| vec![Literal::Int(i), Literal::Str(format!("v{i}"))])
        .collect();
    insert_rows(&e, "default_db.acp", batch).unwrap();

    e.compact_table("default_db.acp").unwrap();
    assert_eq!(e.row_count("acp").unwrap(), 2000);
    assert_eq!(count(&e, "acp"), 2000);
    let last = rows(&e, "SELECT name FROM acp WHERE id = 1999;");
    assert_eq!(last, vec![vec!["v1999".to_string()]]);
}

/// C++: `src/unit_test/storage/new_catalog/append_ut.cpp`
/// (`test_append_and_create_index`, `test_append_and_drop_index`): the index
/// sees rows appended after it was built, and dropping it leaves the data
/// readable.
#[test]
fn append_around_secondary_index_lifecycle() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE aix (id INT, name VARCHAR);")
        .unwrap();
    e.execute("INSERT INTO aix VALUES (1, 'a'), (2, 'b');")
        .unwrap();
    e.execute("CREATE INDEX aix_name ON aix (name);").unwrap();
    e.execute("INSERT INTO aix VALUES (3, 'c');").unwrap();

    let hit = rows(&e, "SELECT id FROM aix WHERE name = 'c';");
    assert_eq!(hit, vec![vec!["3".to_string()]]);

    e.execute("DROP INDEX aix_name ON aix;").unwrap();
    let hit = rows(&e, "SELECT id FROM aix WHERE name = 'c';");
    assert_eq!(hit, vec![vec!["3".to_string()]]);
    assert_eq!(e.row_count("aix").unwrap(), 3);
}

/// C++: `src/unit_test/storage/new_catalog/delete_ut.cpp:63` (`test_delete`):
/// deleted rows disappear from scans and the count, and a delete-all empties
/// the table.
#[test]
fn delete_removes_rows_from_scans() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE del (id INT, name VARCHAR);")
        .unwrap();
    e.execute("INSERT INTO del VALUES (1,'a'), (2,'b'), (3,'c'), (4,'d');")
        .unwrap();

    let deleted = e.execute("DELETE FROM del WHERE id = 2;").unwrap();
    assert!(matches!(deleted[0], QueryResult::Deleted { count: 1 }));
    assert_eq!(e.row_count("del").unwrap(), 3);
    assert_eq!(
        rows(&e, "SELECT id FROM del ORDER BY id;"),
        vec![
            vec!["1".to_string()],
            vec!["3".to_string()],
            vec!["4".to_string()]
        ]
    );

    e.execute("DELETE FROM del;").unwrap();
    assert_eq!(e.row_count("del").unwrap(), 0);
    assert!(rows(&e, "SELECT id FROM del;").is_empty());
}

/// C++: `src/unit_test/storage/new_catalog/delete_ut.cpp:210`
/// (`test_delete_multiple_blocks`): a delete whose predicate only matches the
/// second batch leaves the first batch intact.
#[test]
fn delete_across_multiple_blocks() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE dmb (id INT);").unwrap();
    for batch in 0..2 {
        let rows = (0..8192)
            .map(|i| vec![Literal::Int(batch * 8192 + i)])
            .collect();
        insert_rows(&e, "default_db.dmb", rows).unwrap();
    }
    assert_eq!(e.row_count("dmb").unwrap(), 16384);

    e.execute("DELETE FROM dmb WHERE id >= 8192;").unwrap();
    assert_eq!(e.row_count("dmb").unwrap(), 8192);
    assert_eq!(count(&e, "dmb"), 8192);
    let boundary = rows(&e, "SELECT id FROM dmb WHERE id = 8191;");
    assert_eq!(boundary, vec![vec!["8191".to_string()]]);
    assert!(rows(&e, "SELECT id FROM dmb WHERE id = 8192;").is_empty());
}

/// C++: `src/unit_test/storage/new_catalog/delete_ut.cpp`
/// (`test_delete_and_append`, `test_delete_and_compact`): appending after a
/// delete adds live rows, and compaction preserves the tombstones.
#[test]
fn delete_then_append_and_compact() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE dac (id INT);").unwrap();
    let batch = (0..100).map(|i| vec![Literal::Int(i)]).collect();
    insert_rows(&e, "default_db.dac", batch).unwrap();

    e.execute("DELETE FROM dac WHERE id < 10;").unwrap();
    assert_eq!(e.row_count("dac").unwrap(), 90);
    let extra = (100..105).map(|i| vec![Literal::Int(i)]).collect();
    insert_rows(&e, "default_db.dac", extra).unwrap();
    assert_eq!(e.row_count("dac").unwrap(), 95);

    e.compact_table("default_db.dac").unwrap();
    assert_eq!(e.row_count("dac").unwrap(), 95);
    assert_eq!(count(&e, "dac"), 95);
    assert!(rows(&e, "SELECT id FROM dac WHERE id = 9;").is_empty());
}

/// C++: `src/unit_test/storage/new_catalog/update_ut.cpp:348` (`test_update`):
/// an update makes the new values visible and keeps the row count unchanged.
#[test]
fn update_replaces_visible_values() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE upd (id INT, name VARCHAR);")
        .unwrap();
    e.execute("INSERT INTO upd VALUES (1,'a'), (2,'b'), (3,'c'), (4,'d');")
        .unwrap();

    let updated = e
        .execute("UPDATE upd SET name = 'zzz', id = 100 WHERE id = 2;")
        .unwrap();
    assert!(matches!(updated[0], QueryResult::Updated { count: 1 }));
    assert_eq!(e.row_count("upd").unwrap(), 4);
    assert_eq!(
        rows(&e, "SELECT name FROM upd WHERE id = 100;"),
        vec![vec!["zzz".to_string()]]
    );
    assert!(rows(&e, "SELECT name FROM upd WHERE id = 2;").is_empty());

    e.execute("UPDATE upd SET name = 'y' WHERE id >= 3;")
        .unwrap();
    assert_eq!(
        rows(&e, "SELECT id, name FROM upd ORDER BY id;"),
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
#[test]
fn update_and_delete_compose() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE ud (id INT, name VARCHAR);")
        .unwrap();
    e.execute("INSERT INTO ud VALUES (1,'a'), (2,'b'), (3,'c');")
        .unwrap();

    e.execute("UPDATE ud SET name = 'A' WHERE id = 1;").unwrap();
    e.execute("DELETE FROM ud WHERE id = 1;").unwrap();
    assert_eq!(e.row_count("ud").unwrap(), 2);
    // Nothing left to update at id 1.
    let again = e.execute("UPDATE ud SET name = 'B' WHERE id = 1;").unwrap();
    assert!(matches!(again[0], QueryResult::Updated { count: 0 }));

    e.execute("DELETE FROM ud WHERE id = 2;").unwrap();
    let noop = e.execute("UPDATE ud SET name = 'C' WHERE id = 2;").unwrap();
    assert!(matches!(noop[0], QueryResult::Updated { count: 0 }));
    assert_eq!(rows(&e, "SELECT id, name FROM ud;"), vec![vec!["3", "c"]]);
}

/// C++: `src/unit_test/storage/new_catalog/compact_ut.cpp:1448`
/// (`compact_and_compact`): repeated compaction is a no-op on the visible data
/// and keeps the segment accounting consistent.
#[test]
fn compaction_is_idempotent() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE cc (id INT, name VARCHAR);")
        .unwrap();
    let batch = (0..5000)
        .map(|i| vec![Literal::Int(i), Literal::Str(format!("v{i}"))])
        .collect();
    insert_rows(&e, "default_db.cc", batch).unwrap();

    e.compact_table("default_db.cc").unwrap();
    assert_eq!(e.row_count("cc").unwrap(), 5000);
    e.compact_table("default_db.cc").unwrap();
    assert_eq!(e.row_count("cc").unwrap(), 5000);
    assert_eq!(segment_rows(&e, "cc"), 5000);
    assert_eq!(count(&e, "cc"), 5000);
    assert_eq!(
        rows(&e, "SELECT name FROM cc WHERE id = 4999;"),
        vec![vec!["v4999".to_string()]]
    );
}

/// C++: `src/unit_test/storage/new_catalog/compact_ut.cpp:749`
/// (`compact_and_add_columns`), `:602` (`compact_and_drop_table`): schema
/// changes after compaction see the compacted rows, and dropping a compacted
/// table removes it.
#[test]
fn compact_then_alter_and_drop_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE cad (id INT);").unwrap();
    let batch = (0..32).map(|i| vec![Literal::Int(i)]).collect();
    insert_rows(&e, "default_db.cad", batch).unwrap();
    e.compact_table("default_db.cad").unwrap();

    e.execute("ALTER TABLE cad ADD COLUMN (name VARCHAR);")
        .unwrap();
    let all = rows(&e, "SELECT id, name FROM cad ORDER BY id;");
    assert_eq!(all.len(), 32);
    assert_eq!(all[0], vec!["0", "null"]);
    assert_eq!(all[31], vec!["31", "null"]);

    e.execute("DROP TABLE cad;").unwrap();
    assert!(!e.table_exists("cad"));
    assert!(e.execute("INSERT INTO cad VALUES (1);").is_err());
}
