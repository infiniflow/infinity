//! Scalar/`EXISTS` subqueries, CTEs and non-literal `LIMIT`/`OFFSET`.
//!
//! These constructs were parsed but either evaluated as NULL (subqueries),
//! never bound (CTEs) or silently dropped (expression limits). The tests below
//! exercise the real SQL surface end to end through [`Engine::execute`].

use crate::{Engine, QueryResult};
use tempfile::tempdir;

fn engine() -> (tempfile::TempDir, Engine) {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE t (a INT);").unwrap();
    e.execute("INSERT INTO t VALUES (1), (2), (3);").unwrap();
    (dir, e)
}

fn rows(e: &Engine, sql: &str) -> Vec<Vec<String>> {
    match e
        .execute(sql)
        .unwrap()
        .into_iter()
        .next()
        .unwrap_or_else(|| panic!("no result for {sql}"))
    {
        QueryResult::Rows { rows, .. } => rows,
        other => panic!("expected rows for {sql}, got {other:?}"),
    }
}

fn string_rows(values: &[&str]) -> Vec<Vec<String>> {
    values.iter().map(|v| vec![v.to_string()]).collect()
}

// ---------------------------------------------------------------------------
// Scalar subqueries
// ---------------------------------------------------------------------------

#[test]
fn scalar_subquery_returns_its_only_value() {
    let (_d, e) = engine();
    assert_eq!(rows(&e, "SELECT (SELECT 1);"), string_rows(&["1"]));
    assert_eq!(
        rows(&e, "SELECT (SELECT a FROM t WHERE a = 2);"),
        string_rows(&["2"])
    );
    // A subquery used as a comparison operand resolves before the filter runs.
    assert_eq!(
        rows(&e, "SELECT a FROM t WHERE a = (SELECT 2);"),
        string_rows(&["2"])
    );
}

#[test]
fn scalar_subquery_is_null_when_empty() {
    let (_d, e) = engine();
    assert_eq!(
        rows(&e, "SELECT (SELECT a FROM t WHERE a = 99);"),
        string_rows(&["null"])
    );
}

#[test]
fn scalar_subquery_rejects_multiple_rows_or_columns() {
    let (_d, e) = engine();
    let err = e
        .execute("SELECT (SELECT a FROM t);")
        .unwrap_err()
        .to_string();
    assert!(err.contains("more than one row"), "unexpected error: {err}");

    let err = e
        .execute("SELECT (SELECT a, a FROM t WHERE a = 1);")
        .unwrap_err()
        .to_string();
    assert!(err.contains("one column"), "unexpected error: {err}");
}

// ---------------------------------------------------------------------------
// EXISTS / NOT EXISTS
// ---------------------------------------------------------------------------

#[test]
fn exists_filters_on_subquery_cardinality() {
    let (_d, e) = engine();
    assert_eq!(
        rows(&e, "SELECT a FROM t WHERE EXISTS (SELECT 1) ORDER BY a;"),
        string_rows(&["1", "2", "3"])
    );
    assert!(rows(&e, "SELECT a FROM t WHERE NOT EXISTS (SELECT 1);").is_empty());

    // A correlated-looking literal filter decides the subquery's emptiness.
    assert_eq!(
        rows(
            &e,
            "SELECT a FROM t WHERE EXISTS (SELECT 1 FROM t WHERE a = 2) ORDER BY a;"
        ),
        string_rows(&["1", "2", "3"])
    );
    assert!(rows(
        &e,
        "SELECT a FROM t WHERE EXISTS (SELECT 1 FROM t WHERE a = 99);"
    )
    .is_empty());
    assert_eq!(
        rows(
            &e,
            "SELECT a FROM t WHERE NOT EXISTS (SELECT 1 FROM t WHERE a = 99) ORDER BY a;"
        ),
        string_rows(&["1", "2", "3"])
    );
}

// ---------------------------------------------------------------------------
// Common table expressions
// ---------------------------------------------------------------------------

#[test]
fn single_cte_binds_as_a_table() {
    let (_d, e) = engine();
    assert_eq!(
        rows(
            &e,
            "WITH x AS (SELECT a FROM t WHERE a >= 2) SELECT * FROM x ORDER BY a;"
        ),
        string_rows(&["2", "3"])
    );
    // Projection/filtering against the CTE's columns also works.
    assert_eq!(
        rows(
            &e,
            "WITH x AS (SELECT a FROM t) SELECT a FROM x WHERE a > 2;"
        ),
        string_rows(&["3"])
    );
}

#[test]
fn chained_ctes_can_reference_earlier_bindings() {
    let (_d, e) = engine();
    assert_eq!(
        rows(
            &e,
            "WITH x AS (SELECT a FROM t), y AS (SELECT a FROM x WHERE a > 1)
             SELECT a FROM y ORDER BY a;"
        ),
        string_rows(&["2", "3"])
    );
    // A later CTE may reference an earlier one even when the outer query uses
    // the earlier binding too.
    assert_eq!(
        rows(
            &e,
            "WITH x AS (SELECT a FROM t WHERE a > 1), y AS (SELECT a FROM x)
             SELECT a FROM y ORDER BY a;"
        ),
        string_rows(&["2", "3"])
    );
}

#[test]
fn cte_alias_list_renames_output_columns() {
    let (_d, e) = engine();
    assert_eq!(
        rows(
            &e,
            "WITH x (b) AS (SELECT a FROM t) SELECT b FROM x WHERE b = 1;"
        ),
        string_rows(&["1"])
    );
    assert_eq!(
        rows(
            &e,
            "WITH x (b) AS (SELECT a FROM t) SELECT * FROM x ORDER BY b;"
        ),
        string_rows(&["1", "2", "3"])
    );

    let err = e
        .execute("WITH x (b, c) AS (SELECT a FROM t) SELECT * FROM x;")
        .unwrap_err()
        .to_string();
    assert!(
        err.contains("column(s) available"),
        "unexpected error: {err}"
    );
}

#[test]
fn delete_and_update_filters_accept_subqueries() {
    use crate::QueryResult;

    let (_d, e) = engine();
    assert_eq!(
        e.execute("UPDATE t SET a = 10 WHERE a = (SELECT 1);")
            .unwrap()
            .into_iter()
            .next(),
        Some(QueryResult::Updated { count: 1 })
    );
    assert_eq!(
        rows(&e, "SELECT a FROM t ORDER BY a;"),
        string_rows(&["2", "3", "10"])
    );

    assert_eq!(
        e.execute("DELETE FROM t WHERE EXISTS (SELECT 1 FROM t WHERE a = 10);")
            .unwrap()
            .into_iter()
            .next(),
        Some(QueryResult::Deleted { count: 3 })
    );
    assert!(rows(&e, "SELECT a FROM t;").is_empty());
}

#[test]
fn cte_bindings_are_visible_inside_subqueries() {
    let (_d, e) = engine();
    assert_eq!(
        rows(&e, "WITH x (b) AS (SELECT 2) SELECT (SELECT b FROM x);"),
        string_rows(&["2"])
    );
    assert_eq!(
        rows(
            &e,
            "WITH x AS (SELECT a FROM t WHERE a = 2)
             SELECT a FROM t WHERE EXISTS (SELECT * FROM x) ORDER BY a;"
        ),
        string_rows(&["1", "2", "3"])
    );
    assert!(rows(
        &e,
        "WITH x AS (SELECT a FROM t WHERE a = 99)
         SELECT a FROM t WHERE EXISTS (SELECT * FROM x);"
    )
    .is_empty());
}

// ---------------------------------------------------------------------------
// Non-literal LIMIT / OFFSET
// ---------------------------------------------------------------------------

#[test]
fn constant_limit_and_offset_expressions_apply() {
    let (_d, e) = engine();
    assert_eq!(
        rows(&e, "SELECT a FROM t ORDER BY a LIMIT 1+1;"),
        string_rows(&["1", "2"])
    );
    assert_eq!(
        rows(&e, "SELECT a FROM t ORDER BY a LIMIT 2 OFFSET 0+1;"),
        string_rows(&["2", "3"])
    );
    // The count fast path must still honour a computed limit.
    assert_eq!(
        rows(&e, "SELECT count(*) FROM t LIMIT 1+1;"),
        string_rows(&["3"])
    );
}

#[test]
fn non_constant_limit_is_an_explicit_error() {
    let (_d, e) = engine();
    let err = e
        .execute("SELECT a FROM t LIMIT (SELECT 1);")
        .unwrap_err()
        .to_string();
    assert!(
        err.contains("constant expression"),
        "unexpected error: {err}"
    );

    let err = e
        .execute("SELECT a FROM t LIMIT 0-1;")
        .unwrap_err()
        .to_string();
    assert!(
        err.contains("non-negative integer"),
        "unexpected error: {err}"
    );
}
