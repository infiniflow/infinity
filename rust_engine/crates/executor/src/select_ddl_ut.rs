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

//! `INSERT … SELECT`, `CREATE TABLE … AS SELECT`, `DESCRIBE` and view DDL.
//!
//! Both `INSERT … SELECT` and the view statements are cases where the
//! reference's own implementation is incomplete, so the port matches the
//! failure instead of answering a query the reference cannot run: an
//! `INSERT … SELECT` that reads a column dies in the expression evaluator,
//! and every view statement dies in the DDL dispatch.

use crate::{Engine, QueryResult};
use tempfile::tempdir;

async fn engine() -> (tempfile::TempDir, Engine) {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    (dir, e)
}

async fn result(e: &Engine, sql: &str) -> QueryResult {
    e.execute(sql).await.unwrap().into_iter().next().unwrap()
}

/// Rows as display text, for assertions.
fn texts(rows: &crate::Batch) -> Vec<Vec<String>> {
    rows.text_rows()
}

async fn select_rows(e: &Engine, sql: &str) -> Vec<Vec<String>> {
    let r = result(e, sql).await;
    match &r {
        QueryResult::Rows { .. } => r.rows_text(),
        other => panic!("expected rows, got {other:?}"),
    }
}

async fn row_strings(e: &Engine, sql: &str) -> Vec<Vec<String>> {
    select_rows(e, sql).await
}

async fn seed_source(e: &Engine) {
    e.execute("CREATE TABLE src (a INT, b VARCHAR);")
        .await
        .unwrap();
    e.execute("INSERT INTO src VALUES (1,'x'), (2,'y'), (3,'z');")
        .await
        .unwrap();
}

/// The reference's `PhysicalInsert` evaluates each projected expression
/// through `ExpressionState::CreateState`, which has no case for a column
/// reference: any `INSERT … SELECT` that reads one dies there
/// (`expression_state_impl.cpp:68`). A select list of constants survives, and
/// is the only shape the port runs.
#[tokio::test]
async fn insert_select_is_refused_when_the_select_list_reads_a_column() {
    let (_d, e) = engine().await;
    seed_source(&e).await;
    e.execute("CREATE TABLE dst (id INT, name VARCHAR);")
        .await
        .unwrap();

    for sql in [
        "INSERT INTO dst SELECT a, b FROM src;",
        "INSERT INTO dst SELECT * FROM src;",
        "INSERT INTO dst SELECT a + 1, b FROM src;",
    ] {
        let err = e.execute(sql).await.unwrap_err();
        assert_eq!(
            err.to_string(),
            "Unknown expression type: Column. expression name: a",
            "{sql}"
        );
    }
    // The explicit list does not change which column is reported first.
    let err = e
        .execute("INSERT INTO dst (name, id) SELECT b, a FROM src;")
        .await
        .unwrap_err();
    assert_eq!(
        err.to_string(),
        "Unknown expression type: Column. expression name: b"
    );
    // The reference fails before it appends, so nothing landed.
    assert_eq!(select_rows(&e, "SELECT id, name FROM dst;").await.len(), 0);
}

#[tokio::test]
async fn insert_select_of_constants_inserts_one_row_per_source_row() {
    let (_d, e) = engine().await;
    seed_source(&e).await;
    e.execute("CREATE TABLE dst (id INT, name VARCHAR);")
        .await
        .unwrap();

    assert_eq!(
        e.execute("INSERT INTO dst SELECT 7, 'z';").await.unwrap(),
        vec![QueryResult::Inserted { count: 1 }]
    );
    // A constant list over a source still yields one row per source row.
    assert_eq!(
        e.execute("INSERT INTO dst SELECT 8, 'w' FROM src;")
            .await
            .unwrap(),
        vec![QueryResult::Inserted { count: 3 }]
    );
    assert_eq!(
        row_strings(&e, "SELECT id, name FROM dst ORDER BY id;").await,
        vec![
            vec!["7".to_string(), "z".to_string()],
            vec!["8".to_string(), "w".to_string()],
            vec!["8".to_string(), "w".to_string()],
            vec!["8".to_string(), "w".to_string()],
        ]
    );
}

#[tokio::test]
async fn insert_select_reports_arity_and_name_errors() {
    let (_d, e) = engine().await;
    seed_source(&e).await;
    e.execute("CREATE TABLE dst (id INT, name VARCHAR);")
        .await
        .unwrap();

    let err = e.execute("INSERT INTO dst SELECT 1;").await.unwrap_err();
    assert!(
        err.to_string()
            .contains("INSERT has 2 target column(s) but SELECT returns 1 column(s)"),
        "{err}"
    );

    let err = e
        .execute("INSERT INTO dst (nope) SELECT 1, 'x';")
        .await
        .unwrap_err();
    assert!(
        err.to_string().contains("column 'nope' does not exist"),
        "{err}"
    );

    let err = e
        .execute("INSERT INTO missing SELECT 1, 'x';")
        .await
        .unwrap_err();
    assert!(err.to_string().contains("table"), "{err}");

    // The binder runs before the evaluator, so an unknown column keeps its
    // bind error rather than the unrecoverable one.
    let err = e
        .execute("INSERT INTO dst SELECT c FROM src;")
        .await
        .unwrap_err();
    assert!(
        !err.to_string().starts_with("Unknown expression type"),
        "{err}"
    );
}

#[tokio::test]
async fn create_table_as_select_is_rejected_like_the_reference() {
    let (_d, e) = engine().await;
    seed_source(&e).await;

    let err = e
        .execute("CREATE TABLE copy AS SELECT a, b FROM src;")
        .await
        .unwrap_err();
    assert_eq!(
        err.to_string(),
        "Try to define Table copy without any column."
    );
    assert_eq!(err.status_code(), 3048);
    // Nothing was created.
    assert!(e.execute("SELECT * FROM copy;").await.is_err());

    let err = e
        .execute("CREATE TABLE typed (x DOUBLE, y VARCHAR) AS SELECT a, b FROM src;")
        .await
        .unwrap_err();
    assert!(err.to_string().contains("unexpected AS"), "{err}");
    assert!(e.execute("SELECT * FROM typed;").await.is_err());
}

#[tokio::test]
async fn describe_returns_the_reference_columns_in_definition_order() {
    let (_d, e) = engine().await;
    e.execute("CREATE TABLE described (id INT NOT NULL DEFAULT 7 COMMENT 'pk', name VARCHAR);")
        .await
        .unwrap();

    let QueryResult::Rows { columns, rows } = result(&e, "DESCRIBE described;").await else {
        panic!("DESCRIBE must return rows");
    };
    assert_eq!(columns, vec!["name", "type", "default", "comment"]);
    assert_eq!(
        texts(&rows),
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
    assert_eq!(select_rows(&e, "DESC described;").await.len(), 2);
    assert!(e.execute("DESCRIBE missing;").await.is_err());
}

/// The reference has no view implementation: `PhysicalCreateView` is an empty
/// stub and the DDL dispatch has no case for the statement, so `CREATE VIEW`
/// dies in `UnrecoverableException("Unknown DDL type")`
/// (`query_context_impl.cpp:467`) and `DROP VIEW` at `:489`. Nothing is
/// registered, so a later `FROM v` reports the missing table.
#[tokio::test]
async fn views_are_rejected_like_the_reference() {
    let (_d, e) = engine().await;
    seed_source(&e).await;

    for sql in [
        "CREATE VIEW v AS SELECT a, b FROM src;",
        "CREATE VIEW v (x, y) AS SELECT a, b FROM src;",
        "DROP VIEW v;",
        "DROP VIEW IF EXISTS v;",
    ] {
        let err = e.execute(sql).await.unwrap_err();
        assert_eq!(err.to_string(), "Unknown DDL type", "{sql}");
    }
    assert!(e.execute("SELECT * FROM v;").await.is_err());
    // A table of that name is still creatable: the view never occupied it.
    e.execute("CREATE TABLE v (a INT);").await.unwrap();
}
