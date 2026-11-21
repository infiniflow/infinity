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

//! Scalar/`EXISTS` subqueries, CTEs and non-literal `LIMIT`/`OFFSET`.
//!
//! These constructs were parsed but either evaluated as NULL (subqueries),
//! never bound (CTEs) or silently dropped (expression limits). The tests below
//! exercise the real SQL surface end to end through [`Engine::execute`].

use crate::{Engine, QueryResult};
use tempfile::tempdir;

async fn engine() -> (tempfile::TempDir, Engine) {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE t (a INT);").await.unwrap();
    e.execute("INSERT INTO t VALUES (1), (2), (3);")
        .await
        .unwrap();
    (dir, e)
}

async fn rows(e: &Engine, sql: &str) -> Vec<Vec<String>> {
    let r = e
        .execute(sql)
        .await
        .unwrap()
        .into_iter()
        .next()
        .unwrap_or_else(|| panic!("no result for {sql}"));
    match &r {
        QueryResult::Rows { .. } => r.rows_text(),
        other => panic!("expected rows for {sql}, got {other:?}"),
    }
}

fn string_rows(values: &[&str]) -> Vec<Vec<String>> {
    values.iter().map(|v| vec![v.to_string()]).collect()
}

// ---------------------------------------------------------------------------
// Scalar subqueries
// ---------------------------------------------------------------------------

#[tokio::test]
async fn scalar_subquery_returns_its_only_value() {
    let (_d, e) = engine().await;
    assert_eq!(rows(&e, "SELECT (SELECT 1);").await, string_rows(&["1"]));
    assert_eq!(
        rows(&e, "SELECT (SELECT a FROM t WHERE a = 2);").await,
        string_rows(&["2"])
    );
    // A subquery used as a comparison operand resolves before the filter runs.
    assert_eq!(
        rows(&e, "SELECT a FROM t WHERE a = (SELECT 2);").await,
        string_rows(&["2"])
    );
}

#[tokio::test]
async fn scalar_subquery_is_null_when_empty() {
    let (_d, e) = engine().await;
    assert_eq!(
        rows(&e, "SELECT (SELECT a FROM t WHERE a = 99);").await,
        string_rows(&["null"])
    );
}

#[tokio::test]
async fn scalar_subquery_rejects_multiple_rows_or_columns() {
    let (_d, e) = engine().await;
    let err = e
        .execute("SELECT (SELECT a FROM t);")
        .await
        .unwrap_err()
        .to_string();
    assert!(err.contains("more than one row"), "unexpected error: {err}");

    let err = e
        .execute("SELECT (SELECT a, a FROM t WHERE a = 1);")
        .await
        .unwrap_err()
        .to_string();
    assert!(err.contains("one column"), "unexpected error: {err}");
}

// ---------------------------------------------------------------------------
// EXISTS / NOT EXISTS
// ---------------------------------------------------------------------------

#[tokio::test]
async fn exists_filters_on_subquery_cardinality() {
    let (_d, e) = engine().await;
    assert_eq!(
        rows(&e, "SELECT a FROM t WHERE EXISTS (SELECT 1) ORDER BY a;").await,
        string_rows(&["1", "2", "3"])
    );
    assert!(rows(&e, "SELECT a FROM t WHERE NOT EXISTS (SELECT 1);")
        .await
        .is_empty());

    // A correlated-looking literal filter decides the subquery's emptiness.
    assert_eq!(
        rows(
            &e,
            "SELECT a FROM t WHERE EXISTS (SELECT 1 FROM t WHERE a = 2) ORDER BY a;"
        )
        .await,
        string_rows(&["1", "2", "3"])
    );
    assert!(rows(
        &e,
        "SELECT a FROM t WHERE EXISTS (SELECT 1 FROM t WHERE a = 99);"
    )
    .await
    .is_empty());
    assert_eq!(
        rows(
            &e,
            "SELECT a FROM t WHERE NOT EXISTS (SELECT 1 FROM t WHERE a = 99) ORDER BY a;"
        )
        .await,
        string_rows(&["1", "2", "3"])
    );
}

// ---------------------------------------------------------------------------
// Common table expressions
// ---------------------------------------------------------------------------

#[tokio::test]
async fn single_cte_binds_as_a_table() {
    let (_d, e) = engine().await;
    assert_eq!(
        rows(
            &e,
            "WITH x AS (SELECT a FROM t WHERE a >= 2) SELECT * FROM x ORDER BY a;"
        )
        .await,
        string_rows(&["2", "3"])
    );
    // Projection/filtering against the CTE's columns also works.
    assert_eq!(
        rows(
            &e,
            "WITH x AS (SELECT a FROM t) SELECT a FROM x WHERE a > 2;"
        )
        .await,
        string_rows(&["3"])
    );
}

#[tokio::test]
async fn chained_ctes_can_reference_earlier_bindings() {
    let (_d, e) = engine().await;
    assert_eq!(
        rows(
            &e,
            "WITH x AS (SELECT a FROM t), y AS (SELECT a FROM x WHERE a > 1)
             SELECT a FROM y ORDER BY a;"
        )
        .await,
        string_rows(&["2", "3"])
    );
    // A later CTE may reference an earlier one even when the outer query uses
    // the earlier binding too.
    assert_eq!(
        rows(
            &e,
            "WITH x AS (SELECT a FROM t WHERE a > 1), y AS (SELECT a FROM x)
             SELECT a FROM y ORDER BY a;"
        )
        .await,
        string_rows(&["2", "3"])
    );
}

#[tokio::test]
async fn cte_alias_list_renames_output_columns() {
    let (_d, e) = engine().await;
    assert_eq!(
        rows(
            &e,
            "WITH x (b) AS (SELECT a FROM t) SELECT b FROM x WHERE b = 1;"
        )
        .await,
        string_rows(&["1"])
    );
    assert_eq!(
        rows(
            &e,
            "WITH x (b) AS (SELECT a FROM t) SELECT * FROM x ORDER BY b;"
        )
        .await,
        string_rows(&["1", "2", "3"])
    );

    let err = e
        .execute("WITH x (b, c) AS (SELECT a FROM t) SELECT * FROM x;")
        .await
        .unwrap_err()
        .to_string();
    assert!(
        err.contains("column(s) available"),
        "unexpected error: {err}"
    );
}

#[tokio::test]
async fn delete_and_update_filters_accept_subqueries() {
    use crate::QueryResult;

    let (_d, e) = engine().await;
    assert_eq!(
        e.execute("UPDATE t SET a = 10 WHERE a = (SELECT 1);")
            .await
            .unwrap()
            .into_iter()
            .next(),
        Some(QueryResult::Updated { count: 1 })
    );
    assert_eq!(
        rows(&e, "SELECT a FROM t ORDER BY a;").await,
        string_rows(&["2", "3", "10"])
    );

    assert_eq!(
        e.execute("DELETE FROM t WHERE EXISTS (SELECT 1 FROM t WHERE a = 10);")
            .await
            .unwrap()
            .into_iter()
            .next(),
        Some(QueryResult::Deleted { count: 3 })
    );
    assert!(rows(&e, "SELECT a FROM t;").await.is_empty());
}

#[tokio::test]
async fn cte_bindings_are_visible_inside_subqueries() {
    let (_d, e) = engine().await;
    assert_eq!(
        rows(&e, "WITH x (b) AS (SELECT 2) SELECT (SELECT b FROM x);").await,
        string_rows(&["2"])
    );
    assert_eq!(
        rows(
            &e,
            "WITH x AS (SELECT a FROM t WHERE a = 2)
             SELECT a FROM t WHERE EXISTS (SELECT * FROM x) ORDER BY a;"
        )
        .await,
        string_rows(&["1", "2", "3"])
    );
    assert!(rows(
        &e,
        "WITH x AS (SELECT a FROM t WHERE a = 99)
         SELECT a FROM t WHERE EXISTS (SELECT * FROM x);"
    )
    .await
    .is_empty());
}

// ---------------------------------------------------------------------------
// Non-literal LIMIT / OFFSET
// ---------------------------------------------------------------------------

#[tokio::test]
async fn constant_limit_and_offset_expressions_apply() {
    let (_d, e) = engine().await;
    assert_eq!(
        rows(&e, "SELECT a FROM t ORDER BY a LIMIT 1+1;").await,
        string_rows(&["1", "2"])
    );
    assert_eq!(
        rows(&e, "SELECT a FROM t ORDER BY a LIMIT 2 OFFSET 0+1;").await,
        string_rows(&["2", "3"])
    );
    // The count fast path must still honour a computed limit.
    assert_eq!(
        rows(&e, "SELECT count(*) FROM t LIMIT 1+1;").await,
        string_rows(&["3"])
    );
}

#[tokio::test]
async fn non_constant_limit_is_an_explicit_error() {
    let (_d, e) = engine().await;
    let err = e
        .execute("SELECT a FROM t LIMIT (SELECT 1);")
        .await
        .unwrap_err()
        .to_string();
    assert!(
        err.contains("constant expression"),
        "unexpected error: {err}"
    );

    let err = e
        .execute("SELECT a FROM t LIMIT 0-1;")
        .await
        .unwrap_err()
        .to_string();
    assert!(
        err.contains("non-negative integer"),
        "unexpected error: {err}"
    );
}
