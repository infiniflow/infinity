//! `INSERT … SELECT`, `CREATE TABLE … AS SELECT`, `DESCRIBE` and view DDL.
//!
//! These statements were previously parsed but rejected by the executor; the
//! tests below cover the happy paths plus the arity/name errors the SQL surface
//! must report.

use crate::{Engine, QueryResult};
use tempfile::tempdir;

fn engine() -> (tempfile::TempDir, Engine) {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    (dir, e)
}

fn result(e: &Engine, sql: &str) -> QueryResult {
    e.execute(sql).unwrap().into_iter().next().unwrap()
}

fn select_rows(e: &Engine, sql: &str) -> Vec<Vec<String>> {
    match result(e, sql) {
        QueryResult::Rows { rows, .. } => rows,
        other => panic!("expected rows, got {other:?}"),
    }
}

fn row_strings(e: &Engine, sql: &str) -> Vec<Vec<String>> {
    select_rows(e, sql)
}

fn seed_source(e: &Engine) {
    e.execute("CREATE TABLE src (a INT, b VARCHAR);").unwrap();
    e.execute("INSERT INTO src VALUES (1,'x'), (2,'y'), (3,'z');")
        .unwrap();
}

#[test]
fn insert_select_appends_rows_and_honors_the_target_column_list() {
    let (_d, e) = engine();
    seed_source(&e);
    e.execute("CREATE TABLE dst (id INT, name VARCHAR);")
        .unwrap();

    assert_eq!(
        e.execute("INSERT INTO dst SELECT a, b FROM src;").unwrap(),
        vec![QueryResult::Inserted { count: 3 }]
    );
    assert_eq!(
        row_strings(&e, "SELECT id, name FROM dst ORDER BY id;"),
        vec![
            vec!["1".to_string(), "x".to_string()],
            vec!["2".to_string(), "y".to_string()],
            vec!["3".to_string(), "z".to_string()],
        ]
    );

    // The explicit list reorders the source columns.
    assert_eq!(
        e.execute("INSERT INTO dst (name, id) SELECT b, a FROM src WHERE a > 2;")
            .unwrap(),
        vec![QueryResult::Inserted { count: 1 }]
    );
    assert_eq!(
        row_strings(&e, "SELECT id, name FROM dst ORDER BY id;"),
        vec![
            vec!["1".to_string(), "x".to_string()],
            vec!["2".to_string(), "y".to_string()],
            vec!["3".to_string(), "z".to_string()],
            vec!["3".to_string(), "z".to_string()],
        ]
    );
}

#[test]
fn insert_select_preserves_nulls_and_casts_to_the_target_type() {
    let (_d, e) = engine();
    e.execute("CREATE TABLE source (i INT, f DOUBLE, s VARCHAR, n INT);")
        .unwrap();
    e.execute("INSERT INTO source VALUES (1, 1.5, 'a', NULL), (2, 2.25, 'b', 9);")
        .unwrap();
    e.execute("CREATE TABLE target (i BIGINT, f FLOAT, s VARCHAR, n INT);")
        .unwrap();

    assert_eq!(
        e.execute("INSERT INTO target SELECT i, f, s, n FROM source;")
            .unwrap(),
        vec![QueryResult::Inserted { count: 2 }]
    );
    assert_eq!(
        row_strings(&e, "SELECT i, f, s, n FROM target ORDER BY i;"),
        vec![
            vec![
                "1".to_string(),
                "1.500000".to_string(),
                "a".to_string(),
                "null".to_string(),
            ],
            vec![
                "2".to_string(),
                "2.250000".to_string(),
                "b".to_string(),
                "9".to_string(),
            ],
        ]
    );
}

#[test]
fn insert_select_reports_arity_and_name_errors() {
    let (_d, e) = engine();
    seed_source(&e);
    e.execute("CREATE TABLE dst (id INT, name VARCHAR);")
        .unwrap();

    let err = e.execute("INSERT INTO dst SELECT a FROM src;").unwrap_err();
    assert!(
        err.to_string()
            .contains("INSERT has 2 target column(s) but SELECT returns 1 column(s)"),
        "{err}"
    );

    let err = e
        .execute("INSERT INTO dst (nope) SELECT a FROM src;")
        .unwrap_err();
    assert!(
        err.to_string().contains("column 'nope' does not exist"),
        "{err}"
    );

    let err = e
        .execute("INSERT INTO missing SELECT a FROM src;")
        .unwrap_err();
    assert!(err.to_string().contains("table"), "{err}");
}

#[test]
fn create_table_as_infers_names_and_types_and_copies_rows() {
    let (_d, e) = engine();
    seed_source(&e);

    assert!(matches!(
        result(&e, "CREATE TABLE copy AS SELECT a, b FROM src;"),
        QueryResult::CreatedTable { .. }
    ));
    assert_eq!(
        row_strings(&e, "SELECT * FROM copy ORDER BY a;"),
        vec![
            vec!["1".to_string(), "x".to_string()],
            vec!["2".to_string(), "y".to_string()],
            vec!["3".to_string(), "z".to_string()],
        ]
    );
    let columns = select_rows(&e, "SHOW TABLE copy COLUMNS;");
    assert_eq!(columns[0][0], "a");
    assert_eq!(columns[0][1], "Integer");
    assert_eq!(columns[1][0], "b");
    assert_eq!(columns[1][1], "Varchar");
}

#[test]
fn create_table_as_accepts_explicit_definitions_and_aliases() {
    let (_d, e) = engine();
    seed_source(&e);

    result(
        &e,
        "CREATE TABLE typed (x DOUBLE, y VARCHAR) AS SELECT a, b FROM src;",
    );
    assert_eq!(
        row_strings(&e, "SELECT * FROM typed ORDER BY x;"),
        vec![
            vec!["1.000000".to_string(), "x".to_string()],
            vec!["2.000000".to_string(), "y".to_string()],
            vec!["3.000000".to_string(), "z".to_string()],
        ]
    );
    let columns = select_rows(&e, "SHOW TABLE typed COLUMNS;");
    assert_eq!(columns[0][1], "Double");

    result(
        &e,
        "CREATE TABLE named AS SELECT a AS id, b AS label FROM src;",
    );
    assert_eq!(
        select_rows(&e, "SELECT * FROM named ORDER BY id;")[0],
        vec!["1".to_string(), "x".to_string()]
    );

    // A SELECT without a FROM derives both names and types from the literals.
    result(&e, "CREATE TABLE konst AS SELECT 1 AS one, 'x' AS letter;");
    assert_eq!(
        select_rows(&e, "SELECT * FROM konst;"),
        vec![vec!["1".to_string(), "x".to_string()]]
    );
    assert_eq!(select_rows(&e, "SHOW TABLE konst COLUMNS;")[0][1], "BigInt");
}

#[test]
fn create_table_as_keeps_declared_types_for_direct_columns_even_when_empty() {
    let (_d, e) = engine();
    e.execute("CREATE TABLE vecs (id INT, v EMBEDDING(FLOAT,2));")
        .unwrap();
    e.execute("INSERT INTO vecs VALUES (1, [0.1,0.2]);")
        .unwrap();
    result(&e, "CREATE TABLE vcopy AS SELECT * FROM vecs;");
    let columns = select_rows(&e, "SHOW TABLE vcopy COLUMNS;");
    assert_eq!(columns[0][1], "Integer");
    assert_eq!(columns[1][1], "Embedding(float,2)");

    // No rows still yields the source schema, not an all-VARCHAR guess.
    e.execute("CREATE TABLE empty_src (a INT, v EMBEDDING(FLOAT,2));")
        .unwrap();
    result(&e, "CREATE TABLE empty_copy AS SELECT * FROM empty_src;");
    let columns = select_rows(&e, "SHOW TABLE empty_copy COLUMNS;");
    assert_eq!(columns[0][1], "Integer");
    assert_eq!(columns[1][1], "Embedding(float,2)");
}

#[test]
fn create_table_as_reports_definition_arity_mismatch() {
    let (_d, e) = engine();
    seed_source(&e);
    let err = e
        .execute("CREATE TABLE bad (x INT) AS SELECT a, b FROM src;")
        .unwrap_err();
    assert!(
        err.to_string()
            .contains("1 column definition(s) but SELECT returns 2 column(s)"),
        "{err}"
    );
    // A failed CTAS must not leave the table behind.
    assert!(e.execute("SELECT * FROM bad;").is_err());
}

#[test]
fn describe_returns_the_reference_columns_in_definition_order() {
    let (_d, e) = engine();
    e.execute("CREATE TABLE described (id INT NOT NULL DEFAULT 7 COMMENT 'pk', name VARCHAR);")
        .unwrap();

    let QueryResult::Rows { columns, rows } = result(&e, "DESCRIBE described;") else {
        panic!("DESCRIBE must return rows");
    };
    assert_eq!(columns, vec!["name", "type", "default", "comment"]);
    assert_eq!(
        rows,
        vec![
            vec![
                "id".to_string(),
                "Integer".to_string(),
                "7".to_string(),
                "pk".to_string(),
            ],
            vec![
                "name".to_string(),
                "Varchar".to_string(),
                "Null".to_string(),
                "(empty)".to_string(),
            ],
        ]
    );
    // `DESC` is the accepted legacy spelling.
    assert_eq!(select_rows(&e, "DESC described;").len(), 2);
    assert!(e.execute("DESCRIBE missing;").is_err());
}

#[test]
fn views_expand_from_their_stored_query_and_drop() {
    let (_d, e) = engine();
    seed_source(&e);

    result(
        &e,
        "CREATE VIEW v (x, y) AS SELECT a, b FROM src WHERE a >= 2;",
    );
    assert_eq!(
        row_strings(&e, "SELECT * FROM v ORDER BY x;"),
        vec![
            vec!["2".to_string(), "y".to_string()],
            vec!["3".to_string(), "z".to_string()],
        ]
    );
    assert_eq!(
        select_rows(&e, "SELECT y FROM v WHERE x = 3;"),
        vec![vec!["z".to_string()]]
    );

    // A view over a view expands through the derived-table path.
    result(&e, "CREATE VIEW v2 AS SELECT x FROM v WHERE y = 'z';");
    assert_eq!(
        select_rows(&e, "SELECT * FROM v2;"),
        vec![vec!["3".to_string()]]
    );

    e.execute("DROP VIEW v2;").unwrap();
    assert!(e.execute("SELECT * FROM v2;").is_err());
    let err = e.execute("DROP VIEW v2;").unwrap_err();
    assert!(err.to_string().contains("does not exist"), "{err}");
    e.execute("DROP VIEW IF EXISTS v2;").unwrap();
}

#[test]
fn views_survive_reopening_the_catalog() {
    let dir = tempdir().unwrap();
    {
        let e = Engine::open(dir.path()).unwrap();
        seed_source(&e);
        result(&e, "CREATE VIEW persisted AS SELECT a, b FROM src;");
    }
    let e = Engine::open(dir.path()).unwrap();
    assert_eq!(
        row_strings(&e, "SELECT * FROM persisted ORDER BY a;"),
        vec![
            vec!["1".to_string(), "x".to_string()],
            vec!["2".to_string(), "y".to_string()],
            vec!["3".to_string(), "z".to_string()],
        ]
    );
}

#[test]
fn views_keep_their_creation_database_scope() {
    let (_d, e) = engine();
    seed_source(&e);
    e.execute("CREATE DATABASE db1;").unwrap();
    // Created while `default_db` is current, so `src` resolves there even
    // though the view itself lives in `db1`.
    result(&e, "CREATE VIEW db1.v AS SELECT a, b FROM src;");
    e.execute("USE db1;").unwrap();
    assert_eq!(
        row_strings(&e, "SELECT * FROM v ORDER BY a;"),
        vec![
            vec!["1".to_string(), "x".to_string()],
            vec!["2".to_string(), "y".to_string()],
            vec!["3".to_string(), "z".to_string()],
        ]
    );
}

#[test]
fn view_created_in_another_database_resolves_tables_globally() {
    let (_d, e) = engine();
    seed_source(&e);
    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("USE db1;").unwrap();
    // `src` only exists in `default_db`; the view is created while `db1` is
    // current, so the table reference must be resolved against the catalog
    // rather than blindly qualified with the session database.
    result(&e, "CREATE VIEW v AS SELECT a, b FROM src;");
    assert_eq!(
        row_strings(&e, "SELECT * FROM v ORDER BY a;"),
        vec![
            vec!["1".to_string(), "x".to_string()],
            vec!["2".to_string(), "y".to_string()],
            vec!["3".to_string(), "z".to_string()],
        ]
    );
}

#[test]
fn view_ddl_reports_name_and_arity_errors() {
    let (_d, e) = engine();
    seed_source(&e);
    e.execute("CREATE TABLE taken (id INT);").unwrap();

    let err = e
        .execute("CREATE VIEW taken AS SELECT a FROM src;")
        .unwrap_err();
    assert!(
        err.to_string().contains("table 'default_db.taken'"),
        "{err}"
    );

    let err = e
        .execute("CREATE VIEW wrong (only_one) AS SELECT a, b FROM src;")
        .unwrap_err();
    assert!(
        err.to_string()
            .contains("1 column name(s) but SELECT returns 2 column(s)"),
        "{err}"
    );

    result(&e, "CREATE VIEW dup AS SELECT a FROM src;");
    let err = e
        .execute("CREATE VIEW dup AS SELECT a FROM src;")
        .unwrap_err();
    assert!(err.to_string().contains("view 'default_db.dup'"), "{err}");

    // A view is not a JOIN input yet; say so rather than leaking a missing-table
    // error.
    let err = e
        .execute("SELECT * FROM src JOIN dup ON src.a = dup.a;")
        .unwrap_err();
    assert!(
        err.to_string().contains("view") && err.to_string().contains("JOIN"),
        "{err}"
    );
}
