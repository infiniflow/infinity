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

//! `MATCH TEXT` with an empty field list and a `default_field` option.
//!
//! The Python benchmark issues
//! `table.output(["_row_id", "_score"]).match_text("", q, 10, {"default_field": "body"})`
//! and the reference answers every query. The port used to return nothing for
//! an empty field list: the column-less branch only tried to read `field:term`
//! pairs out of the query text, and a plain keyword has none, so it gave up
//! before the option was ever consulted.

use executor::{Engine, QueryResult};

async fn rows(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
    let rs = engine
        .execute(sql)
        .await
        .unwrap_or_else(|e| panic!("{sql}\n{e}"));
    for r in rs {
        if let QueryResult::Rows { rows, .. } = r {
            return rows
                .iter()
                .map(|r| r.iter().map(|v| v.to_slt_string()).collect())
                .collect();
        }
    }
    Vec::new()
}

async fn engine_with_docs() -> (tempfile::TempDir, Engine) {
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE t (id INTEGER, body VARCHAR);")
        .await
        .unwrap();
    e.execute("CREATE INDEX t_ft ON t (body) USING FULLTEXT;")
        .await
        .unwrap();
    e.execute(
        "INSERT INTO t VALUES (1, 'harmful chemical compound'), (2, 'harmless water'), \
         (3, 'another chemical reaction');",
    )
    .await
    .unwrap();
    (dir, e)
}

#[tokio::test]
async fn an_empty_field_list_falls_back_to_default_field() {
    let (_dir, e) = engine_with_docs().await;
    let hits = rows(
        &e,
        "SELECT id FROM t SEARCH MATCH TEXT ('', 'chemical', 'topn=10;default_field=body');",
    )
    .await;
    assert_eq!(
        hits.len(),
        2,
        "expected both documents mentioning chemical: {hits:?}"
    );
}

#[tokio::test]
async fn an_explicit_field_still_wins_over_default_field() {
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    // Two text columns, so an explicit field and the default one can disagree.
    e.execute("CREATE TABLE t (id INTEGER, body VARCHAR, title VARCHAR);")
        .await
        .unwrap();
    e.execute("CREATE INDEX t_ft ON t (body) USING FULLTEXT;")
        .await
        .unwrap();
    e.execute("CREATE INDEX t_title_ft ON t (title) USING FULLTEXT;")
        .await
        .unwrap();
    e.execute(
        "INSERT INTO t VALUES (1, 'harmful chemical compound', 'water'), \
         (2, 'harmless water', 'chemical'), (3, 'another chemical reaction', 'air');",
    )
    .await
    .unwrap();

    // `body` holds the term twice, `title` once: naming `title` must not fall
    // back to the default field and return the body matches too.
    let hits = rows(
        &e,
        "SELECT id FROM t SEARCH MATCH TEXT ('title', 'chemical', 'topn=10;default_field=body');",
    )
    .await;
    assert_eq!(hits.len(), 1, "explicit field must win: {hits:?}");
    assert_eq!(hits[0][0], "2");
}

#[tokio::test]
async fn a_field_qualified_query_still_works_without_a_field_list() {
    let (_dir, e) = engine_with_docs().await;
    let hits = rows(
        &e,
        "SELECT id FROM t SEARCH MATCH TEXT ('', 'body:chemical', 'topn=10');",
    )
    .await;
    assert_eq!(hits.len(), 2, "field-qualified query: {hits:?}");
}

#[tokio::test]
async fn an_empty_field_list_without_default_field_finds_nothing() {
    let (_dir, e) = engine_with_docs().await;
    // No field to search and no `field:term` in the query: there is nothing to
    // scan, so an empty result is the correct answer (the reference agrees).
    let hits = rows(
        &e,
        "SELECT id FROM t SEARCH MATCH TEXT ('', 'chemical', 'topn=10');",
    )
    .await;
    assert!(hits.is_empty(), "expected no hits, got {hits:?}");
}

#[tokio::test]
async fn a_default_field_that_is_not_a_column_is_ignored() {
    let (_dir, e) = engine_with_docs().await;
    let hits = rows(
        &e,
        "SELECT id FROM t SEARCH MATCH TEXT ('', 'chemical', 'topn=10;default_field=nope');",
    )
    .await;
    assert!(
        hits.is_empty(),
        "a non-column default_field must not match: {hits:?}"
    );
}

/// The fallback re-enters the statement with the field filled in. The scan
/// gate admits two full-text scans, so if that re-entry took its own slot two
/// concurrent statements would each wait for the one the other holds - a
/// deadlock that only shows up under load, which is why it is a case here and
/// not a comment.
#[tokio::test(flavor = "multi_thread")]
async fn concurrent_default_field_searches_do_not_deadlock() {
    let (_dir, e) = engine_with_docs().await;
    let engine = std::sync::Arc::new(e);

    let mut handles = Vec::new();
    for _ in 0..8 {
        let engine = std::sync::Arc::clone(&engine);
        handles.push(tokio::spawn(async move {
            for _ in 0..20 {
                let hits = rows(
                    &engine,
                    "SELECT id FROM t SEARCH MATCH TEXT ('', 'chemical', \
                     'topn=10;default_field=body');",
                )
                .await;
                assert_eq!(hits.len(), 2, "expected both chemical documents: {hits:?}");
            }
        }));
    }
    for h in handles {
        h.await.expect("a concurrent default_field search panicked");
    }
}
