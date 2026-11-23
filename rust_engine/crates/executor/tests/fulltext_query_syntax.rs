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

//! `minimum_should_match` alongside the query-string grammar.
//!
//! RAGFlow's retrieval spells out the default on every English query -
//! `minimum_should_match=0%` - and its query text is always a boolean
//! expression (`(a OR b)^5 OR ("c d")^0.7`). A zero share asks for nothing the
//! tree path cannot answer ("any clause may match" is what its boolean
//! structure computes), so it must be accepted; a positive share counts matched
//! clauses, which only the WAND path implements, and stays an error instead of
//! being silently dropped.

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

fn ids(result: &[QueryResult]) -> Vec<i64> {
    match &result[0] {
        QueryResult::Rows { rows, .. } => rows
            .iter()
            .map(|r| r[0].to_slt_string().parse::<i64>().expect("id"))
            .collect(),
        other => panic!("expected rows, got {other:?}"),
    }
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
    let mut got = ids(&result);
    got.sort();
    assert_eq!(got, vec![1, 2], "the OR expression should match both rows");
}

#[tokio::test]
async fn positive_minimum_should_match_is_still_rejected() {
    let (_dir, engine) = engine_with_rows().await;
    let err = engine
        .execute(
            "SELECT id FROM t SEARCH MATCH TEXT ('doc', '(apple OR banana OR cherry)', 'minimum_should_match=50%');",
        )
        .await
        .expect_err("the tree path cannot count matched clauses");
    assert!(
        err.to_string().contains("minimum_should_match"),
        "error should name the option: {err}"
    );
}
