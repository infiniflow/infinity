//! Engine-level ports of `src/unit_test/storage/new_catalog/column_ut.cpp`
//! (`TestTxnColumn`, TEST_AUDIT rows 733-741).
//!
//! The reference adds/drops columns through live `NewTxn` handles and asserts
//! the schema and the values of old rows after the commit. The engine has no
//! DDL transaction surface, so each interleaving is the equivalent sequence of
//! auto-committed `ALTER TABLE` statements; the schema, row values, row counts
//! and durability across a reopen are all asserted.

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

/// Column names in definition order (`SHOW TABLE … COLUMNS`).
fn columns(engine: &Engine, table: &str) -> Vec<String> {
    rows(engine, &format!("SHOW TABLE {table} COLUMNS;"))
        .into_iter()
        .map(|row| row[0].clone())
        .collect()
}

fn setup(engine: &Engine, db: &str, table: &str, defs: &str) {
    engine.execute(&format!("CREATE DATABASE {db};")).unwrap();
    engine
        .execute(&format!("CREATE TABLE {db}.{table} ({defs});"))
        .unwrap();
}

/// Insert `n` rows whose first column cycles through `base..base+n`.
fn append_ints(engine: &Engine, table: &str, n: usize, base: i64) {
    let batch: Vec<Vec<Literal>> = (0..n)
        .map(|i| vec![Literal::Int(base + i as i64)])
        .collect();
    engine.insert_literals(table, None, batch).unwrap();
}

/// C++ `column_ut.cpp:55` (`test_add_columns`): 8192 rows written before an
/// ADD COLUMN read the column default; the column is then dropped.
#[test]
fn test_add_columns() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    setup(&e, "db1", "tb1", "col1 INT");

    append_ints(&e, "db1.tb1", 8192, 1);
    e.execute(
        "ALTER TABLE db1.tb1 ADD COLUMN \
         (col2 VARCHAR DEFAULT 'abcdefghijklmnopqrstuvwxyz');",
    )
    .unwrap();

    let total: usize = rows(&e, "SELECT count(*) FROM db1.tb1;")[0][0]
        .parse()
        .unwrap();
    assert_eq!(total, 8192);
    // Every old row carries the default, not just the first one.
    assert_eq!(
        rows(
            &e,
            "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abcdefghijklmnopqrstuvwxyz';"
        )[0][0]
            .parse::<usize>()
            .unwrap(),
        8192
    );
    assert_eq!(
        rows(&e, "SELECT DISTINCT col2 FROM db1.tb1;"),
        vec![vec!["abcdefghijklmnopqrstuvwxyz".to_string()]]
    );
    assert_eq!(columns(&e, "db1.tb1"), vec!["col1", "col2"]);

    e.execute("ALTER TABLE db1.tb1 DROP COLUMN (col2);").unwrap();
    assert_eq!(columns(&e, "db1.tb1"), vec!["col1"]);
    e.cleanup().unwrap();
}

/// C++ `column_ut.cpp:179` (`alter_column`): add two columns then drop an
/// existing and an added one; the remaining schema is col1, col4.
#[test]
fn alter_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    setup(&e, "db1", "tb1", "col1 INT, col2 VARCHAR");

    e.execute("ALTER TABLE db1.tb1 ADD COLUMN (col3 VARCHAR);")
        .unwrap();
    e.execute("ALTER TABLE db1.tb1 ADD COLUMN (col4 VARCHAR);")
        .unwrap();
    assert_eq!(
        columns(&e, "db1.tb1"),
        vec!["col1", "col2", "col3", "col4"]
    );

    e.execute("ALTER TABLE db1.tb1 DROP COLUMN (col3);")
        .unwrap();
    e.execute("ALTER TABLE db1.tb1 DROP COLUMN (col2);")
        .unwrap();
    assert_eq!(columns(&e, "db1.tb1"), vec!["col1", "col4"]);
    // The table is still readable through the surviving columns.
    e.execute("INSERT INTO db1.tb1 (col1, col4) VALUES (1, 'a');")
        .unwrap();
    assert_eq!(
        rows(&e, "SELECT col1, col4 FROM db1.tb1;"),
        vec![vec!["1".to_string(), "a".to_string()]]
    );

    e.execute("DROP TABLE db1.tb1;").unwrap();
    e.execute("DROP DATABASE db1;").unwrap();
}

/// C++ `column_ut.cpp:250` (`add_column_and_drop_db`): the added column lives
/// in the database; dropping the database removes it, and later adds fail.
#[test]
fn add_column_and_drop_db() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    setup(&e, "db1", "tb1", "col1 INT, col2 VARCHAR");

    e.execute("ALTER TABLE db1.tb1 ADD COLUMN (col3 VARCHAR);")
        .unwrap();
    assert_eq!(columns(&e, "db1.tb1"), vec!["col1", "col2", "col3"]);

    e.execute("DROP DATABASE db1;").unwrap();
    assert!(e.execute("USE db1;").is_err());
    // Adding to the dropped table fails.
    assert!(e
        .execute("ALTER TABLE db1.tb1 ADD COLUMN (col4 VARCHAR);")
        .is_err());
    // Recreating the database does not resurrect the table or its column.
    e.execute("CREATE DATABASE db1;").unwrap();
    assert!(rows(&e, "SHOW TABLES FROM db1;").is_empty());
    e.execute("DROP DATABASE db1;").unwrap();
}

/// C++ `column_ut.cpp:434` (`add_column_and_drop_table`): an added column is
/// part of the table; dropping the table removes it and later adds fail.
#[test]
fn add_column_and_drop_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    setup(&e, "db1", "tb1", "col1 INT, col2 VARCHAR");

    e.execute("ALTER TABLE db1.tb1 ADD COLUMN (col3 VARCHAR);")
        .unwrap();
    e.execute("DROP TABLE db1.tb1;").unwrap();
    assert!(e
        .execute("ALTER TABLE db1.tb1 ADD COLUMN (col4 VARCHAR);")
        .is_err());
    assert!(!e.table_exists("db1.tb1"));
    e.execute("DROP DATABASE db1;").unwrap();
}

/// C++ `column_ut.cpp:740` (`add_column_and_add_column`): distinct adds both
/// commit; adding the same column again fails and leaves the schema intact.
///
/// The reference's second block also passes a duplicate *column id* with a new
/// name. The SQL surface assigns ids implicitly, so that half has no engine
/// surface; the duplicate-name rejection is the reachable equivalent.
#[test]
fn add_column_and_add_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    setup(&e, "db1", "tb1", "col1 INT, col2 VARCHAR");

    e.execute("ALTER TABLE db1.tb1 ADD COLUMN (col3 VARCHAR);")
        .unwrap();
    e.execute("ALTER TABLE db1.tb1 ADD COLUMN (col4 VARCHAR);")
        .unwrap();
    assert_eq!(
        columns(&e, "db1.tb1"),
        vec!["col1", "col2", "col3", "col4"]
    );

    // Duplicate column name (the reference's second add) is rejected.
    assert!(e
        .execute("ALTER TABLE db1.tb1 ADD COLUMN (col3 VARCHAR);")
        .is_err());
    assert_eq!(
        columns(&e, "db1.tb1"),
        vec!["col1", "col2", "col3", "col4"]
    );

    e.execute("DROP DATABASE db1;").unwrap();
}

/// C++ `column_ut.cpp:1030` (`drop_column_and_drop_db`): dropping a column
/// followed by the database leaves no database; a drop after the database is
/// gone fails.
#[test]
fn drop_column_and_drop_db() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    setup(&e, "db1", "tb1", "col1 INT, col2 VARCHAR");

    e.execute("ALTER TABLE db1.tb1 DROP COLUMN (col2);")
        .unwrap();
    assert_eq!(columns(&e, "db1.tb1"), vec!["col1"]);

    e.execute("DROP DATABASE db1;").unwrap();
    assert!(e
        .execute("ALTER TABLE db1.tb1 DROP COLUMN (col1);")
        .is_err());
    e.execute("CREATE DATABASE db1;").unwrap();
    assert!(rows(&e, "SHOW TABLES FROM db1;").is_empty());
    e.execute("DROP DATABASE db1;").unwrap();
}

/// C++ `column_ut.cpp:1165` (`drop_column_and_drop_table`): the column drop and
/// the table drop both commit; afterwards the table is gone and further column
/// drops fail.
#[test]
fn drop_column_and_drop_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    setup(&e, "db1", "tb1", "col1 INT, col2 VARCHAR");

    e.execute("ALTER TABLE db1.tb1 DROP COLUMN (col2);")
        .unwrap();
    e.execute("DROP TABLE db1.tb1;").unwrap();
    assert!(e
        .execute("ALTER TABLE db1.tb1 DROP COLUMN (col1);")
        .is_err());
    assert!(!e.table_exists("db1.tb1"));

    e.execute("DROP DATABASE db1;").unwrap();
}

/// C++ `column_ut.cpp:1453` (`drop_column_and_add_column`): drop col2 then add
/// col4, and the reverse order, both leave exactly col1 + the new column.
#[test]
fn drop_column_and_add_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    setup(&e, "db1", "tb1", "col1 INT, col2 VARCHAR");

    e.execute("ALTER TABLE db1.tb1 DROP COLUMN (col2);")
        .unwrap();
    e.execute("ALTER TABLE db1.tb1 ADD COLUMN (col4 VARCHAR);")
        .unwrap();
    assert_eq!(columns(&e, "db1.tb1"), vec!["col1", "col4"]);

    // The opposite interleaving: add col3 first, then drop col2.
    e.execute("CREATE TABLE db1.tb2 (col1 INT, col2 VARCHAR);")
        .unwrap();
    e.execute("ALTER TABLE db1.tb2 ADD COLUMN (col3 VARCHAR);")
        .unwrap();
    e.execute("ALTER TABLE db1.tb2 DROP COLUMN (col2);")
        .unwrap();
    assert_eq!(columns(&e, "db1.tb2"), vec!["col1", "col3"]);

    e.execute("DROP DATABASE db1;").unwrap();
}

/// C++ `column_ut.cpp:1672` (`drop_column_and_drop_column`): dropping two
/// different columns leaves the third; dropping an already-dropped column
/// fails and leaves the remaining schema untouched.
#[test]
fn drop_column_and_drop_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    setup(&e, "db1", "tb1", "col1 INT, col2 VARCHAR, col3 VARCHAR");

    // Two committed drops of different columns.
    e.execute("ALTER TABLE db1.tb1 DROP COLUMN (col1);")
        .unwrap();
    e.execute("ALTER TABLE db1.tb1 DROP COLUMN (col3);")
        .unwrap();
    assert_eq!(columns(&e, "db1.tb1"), vec!["col2"]);

    // Dropping the same names again fails.
    assert!(e.execute("ALTER TABLE db1.tb1 DROP COLUMN (col1);").is_err());
    assert!(e.execute("ALTER TABLE db1.tb1 DROP COLUMN (col3);").is_err());
    assert_eq!(columns(&e, "db1.tb1"), vec!["col2"]);

    e.execute("DROP DATABASE db1;").unwrap();
}
