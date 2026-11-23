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

//! `minimum_should_match` and `rank_features` alongside the query-string grammar.
//!
//! RAGFlow's retrieval spells a share out on every Chinese query - the default
//! `minimum_should_match=30%` - and its query text is always a boolean
//! expression (`(a OR b)^5 OR ("c d")^0.7`). The reference answers both options on
//! the parsed tree (`MinimumShouldMatchIterator` over the root's clauses, and a
//! `rank_features` bonus added to each score), so rejecting them turned every such
//! search into an error. A share that no clause set can satisfy is the reference's
//! null iterator: it matches nothing.

use executor::{Engine, QueryResult};

async fn engine_with_rows() -> (tempfile::TempDir, Engine) {
    let dir = tempfile::tempdir().unwrap();
    let engine = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    engine
        .execute("CREATE TABLE t (id INTEGER, doc VARCHAR);")
        .await
        .unwrap();
    engine
        .execute("CREATE INDEX t_ft ON t (doc) USING FULLTEXT;")
        .await
        .unwrap();
    engine
        .execute("INSERT INTO t VALUES (1, 'apple pie'), (2, 'banana bread'), (3, 'cherry tart');")
        .await
        .unwrap();
    (dir, engine)
}

/// One row per clause level: `apple`, `apple banana`, `apple banana cherry`.
async fn engine_with_nested_rows() -> (tempfile::TempDir, Engine) {
    let dir = tempfile::tempdir().unwrap();
    let engine = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    engine
        .execute("CREATE TABLE m (id INTEGER, doc VARCHAR);")
        .await
        .unwrap();
    engine
        .execute("CREATE INDEX m_ft ON m (doc) USING FULLTEXT;")
        .await
        .unwrap();
    engine
        .execute(
            "INSERT INTO m VALUES (1, 'apple'), (2, 'apple banana'), (3, 'apple banana cherry');",
        )
        .await
        .unwrap();
    (dir, engine)
}

fn ids(result: &[QueryResult]) -> Vec<i64> {
    match &result[0] {
        QueryResult::Rows { rows, .. } => rows
            .iter()
            .map(|r| r[0].to_slt_string().parse::<i64>().expect("id"))
            .collect(),
        other => panic!("expected rows, got {other:?}"),
    }
}

fn sorted_ids(result: &[QueryResult]) -> Vec<i64> {
    let mut ids = ids(result);
    ids.sort_unstable();
    ids
}

#[tokio::test]
async fn zero_minimum_should_match_is_accepted_with_query_syntax() {
    let (_dir, engine) = engine_with_rows().await;
    let result = engine
        .execute(
            "SELECT id FROM t SEARCH MATCH TEXT ('doc', '(apple OR banana)', 'minimum_should_match=0%');",
        )
        .await
        .expect("a zero share must not be rejected on a parsed query");
    assert_eq!(
        sorted_ids(&result),
        vec![1, 2],
        "the OR expression should match both rows"
    );
}

#[tokio::test]
async fn minimum_should_match_counts_matched_clauses() {
    let (_dir, engine) = engine_with_nested_rows().await;
    let search = |option: &str| {
        format!(
            "SELECT id FROM m SEARCH MATCH TEXT ('doc', '(apple OR banana OR cherry)', 'topn=10;{option}');"
        )
    };

    // All three clauses: only the row carrying every term.
    let all = engine
        .execute(&search("minimum_should_match=100%"))
        .await
        .expect("a full share is answerable on a parsed query");
    assert_eq!(sorted_ids(&all), vec![3]);

    // 67% of three leaves rounds down to two clauses.
    let two = engine
        .execute(&search("minimum_should_match=67%"))
        .await
        .expect("a partial share is answerable on a parsed query");
    assert_eq!(sorted_ids(&two), vec![2, 3]);

    // A bare count is taken literally.
    let two_count = engine
        .execute(&search("minimum_should_match=2"))
        .await
        .expect("a clause count is answerable on a parsed query");
    assert_eq!(sorted_ids(&two_count), vec![2, 3]);

    // `0` and `1` ask for nothing beyond the query's own boolean structure.
    let any = engine
        .execute(&search("minimum_should_match=1"))
        .await
        .expect("a one-clause share keeps the plain OR");
    assert_eq!(sorted_ids(&any), vec![1, 2, 3]);

    // A share above the root's clause count can never be satisfied, which is the
    // reference's null iterator. `(apple AND banana) OR (cherry AND apple)` has
    // two root clauses but four leaves, so a full share asks for four of two.
    let impossible = engine
        .execute(
            "SELECT id FROM m SEARCH MATCH TEXT \
             ('doc', '(apple AND banana) OR (cherry AND apple)', 'topn=10;minimum_should_match=100%');",
        )
        .await
        .expect("an unsatisfiable share answers with no rows");
    assert_eq!(ids(&impossible), Vec::<i64>::new());
}

#[tokio::test]
async fn rank_features_score_a_grammar_query() {
    let dir = tempfile::tempdir().unwrap();
    let engine = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    engine
        .execute("CREATE TABLE rg (id INTEGER, doc VARCHAR, feat VARCHAR);")
        .await
        .unwrap();
    engine
        .execute(
            "INSERT INTO rg VALUES (1, 'apple', '[{\"Tag1\":1}]'), (2, 'apple', '[{\"Tag1\":2}]');",
        )
        .await
        .unwrap();
    engine
        .execute("CREATE INDEX rg_doc ON rg (doc) USING FULLTEXT;")
        .await
        .unwrap();
    engine
        .execute("CREATE INDEX rg_feat ON rg (feat) USING FULLTEXT WITH (analyzer='rankfeatures');")
        .await
        .unwrap();

    let score_of = |result: &[QueryResult], id: &str| -> f64 {
        match &result[0] {
            QueryResult::Rows { rows, .. } => rows
                .iter()
                .find(|r| r[0].to_slt_string() == id)
                .unwrap_or_else(|| panic!("id {id} missing: {rows:?}"))[1]
                .to_slt_string()
                .parse()
                .unwrap(),
            other => panic!("expected rows, got {other:?}"),
        }
    };

    let base = engine
        .execute(
            "SELECT id, SCORE() FROM rg SEARCH MATCH TEXT ('doc','(apple OR banana)','topn=10');",
        )
        .await
        .unwrap();
    let ranked = engine
        .execute(
            "SELECT id, SCORE() FROM rg SEARCH MATCH TEXT \
             ('doc','(apple OR banana)','topn=10;rank_features=feat^Tag1^1');",
        )
        .await
        .expect("rank_features must be answerable on a parsed query");

    // Both rows carry the same `doc`, so the BM25 half is identical and the
    // difference is exactly the feature weight.
    assert!((score_of(&ranked, "1") - score_of(&base, "1") - 1.0).abs() < 1e-4);
    assert!((score_of(&ranked, "2") - score_of(&base, "2") - 2.0).abs() < 1e-4);

    // A cold handle has to answer both questions the analyzer decides: it must
    // accept the `rankfeatures` column (the maps are written only by `CREATE
    // INDEX`, so a restarted process reads them from the catalog now), and an
    // index it rebuilds from the persisted rows must keep the payload.
    drop(engine);
    let reopened = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    let cold = reopened
        .execute(
            "SELECT id, SCORE() FROM rg SEARCH MATCH TEXT \
             ('doc','(apple OR banana)','topn=10;rank_features=feat^Tag1^1');",
        )
        .await
        .expect("a cold handle must accept a rankfeatures column");
    assert!((score_of(&cold, "1") - score_of(&ranked, "1")).abs() < 1e-4);
    assert!((score_of(&cold, "2") - score_of(&ranked, "2")).abs() < 1e-4);
}
