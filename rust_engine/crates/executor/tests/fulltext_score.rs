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

//! Full-text scoring invariants (plan §6 Gate G / INVARIANT_MATRIX G5).
//!
//! The BM25 scorer is an approximation of the C++/iresearch one, so these tests
//! assert *monotonic invariants* rather than exact scores:
//!   * more occurrences of the query term (higher tf) score no lower;
//!   * a document with the same tf but more tokens (longer) scores no higher.

use executor::{Engine, QueryResult};

/// Runs a text search and returns `(id, score)` pairs keyed by the first column.
async fn scores(engine: &Engine, query: &str) -> Vec<(i64, f64)> {
    let sql = format!(
        "SELECT id, ROUND(SCORE(), 6) FROM t SEARCH MATCH TEXT ('doc', '{query}', 'topn=10');"
    );
    let rs = engine.execute(&sql).await.expect("select");
    let rows = match &rs[0] {
        QueryResult::Rows { rows, .. } => rows.clone(),
        other => panic!("expected rows, got {other:?}"),
    };
    rows.iter()
        .map(|r| {
            let id = r[0].to_slt_string().parse::<i64>().expect("id");
            let score = r[1].to_slt_string().parse::<f64>().expect("score");
            (id, score)
        })
        .collect()
}

fn score_of(pairs: &[(i64, f64)], id: i64) -> f64 {
    pairs
        .iter()
        .find(|(i, _)| *i == id)
        .map(|(_, s)| *s)
        .unwrap_or_else(|| panic!("id {id} not found in {pairs:?}"))
}

#[tokio::test]
async fn higher_term_frequency_scores_no_lower() {
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE t (id INTEGER, doc VARCHAR);")
        .await
        .unwrap();
    // MATCH TEXT needs a full-text index on the searched column.
    e.execute("CREATE INDEX t_ft ON t (doc) USING FULLTEXT;")
        .await
        .unwrap();
    e.execute("INSERT INTO t VALUES (1, 'apple apple'), (3, 'apple banana cherry date');")
        .await
        .unwrap();
    let s = scores(&e, "apple").await;
    // doc 1 has tf=2, doc 3 has tf=1 (and is longer): doc 1 must not score lower
    assert!(
        score_of(&s, 1) >= score_of(&s, 3),
        "tf=2 should not score below tf=1: {s:?}"
    );
}

#[tokio::test]
async fn shorter_document_scores_no_lower_at_equal_tf() {
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE t (id INTEGER, doc VARCHAR);")
        .await
        .unwrap();
    // MATCH TEXT needs a full-text index on the searched column.
    e.execute("CREATE INDEX t_ft ON t (doc) USING FULLTEXT;")
        .await
        .unwrap();
    // same tf (1) for the query term, different document lengths
    e.execute(
        "INSERT INTO t VALUES (2, 'apple banana'), (3, 'apple banana cherry date fig grape');",
    )
    .await
    .unwrap();
    let s = scores(&e, "apple").await;
    assert!(
        score_of(&s, 2) >= score_of(&s, 3),
        "shorter document should not score below a longer one at equal tf: {s:?}"
    );
}

#[tokio::test]
async fn non_matching_document_is_absent() {
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE t (id INTEGER, doc VARCHAR);")
        .await
        .unwrap();
    // MATCH TEXT needs a full-text index on the searched column.
    e.execute("CREATE INDEX t_ft ON t (doc) USING FULLTEXT;")
        .await
        .unwrap();
    e.execute("INSERT INTO t VALUES (1, 'apple'), (2, 'pear');")
        .await
        .unwrap();
    let s = scores(&e, "apple").await;
    assert_eq!(s.iter().map(|(i, _)| *i).collect::<Vec<_>>(), vec![1]);
}
