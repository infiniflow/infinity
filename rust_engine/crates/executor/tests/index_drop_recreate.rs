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

//! `DROP INDEX` → recreate invariants (plan §6 Gate G / INVARIANT_MATRIX §3).
//!
//! Recreating an index after `DROP INDEX` must leave lookups correct (the
//! rebuilt index answers the same rows as before the drop).

use executor::{Engine, QueryResult};

async fn ids(engine: &Engine, sql: &str) -> Vec<i64> {
    let rs = engine.execute(sql).await.expect("select");
    match &rs[0] {
        QueryResult::Rows { rows, .. } => rows
            .iter()
            .map(|r| r[0].to_slt_string().parse().unwrap())
            .collect(),
        other => panic!("expected rows, got {other:?}"),
    }
}

#[tokio::test]
async fn lookups_stay_correct_across_drop_and_recreate() {
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE t (id INTEGER, name VARCHAR);")
        .await
        .unwrap();
    e.execute("CREATE INDEX idx ON t (name);").await.unwrap();
    e.execute("INSERT INTO t VALUES (1, 'a'), (2, 'b'), (3, 'a');")
        .await
        .unwrap();

    let q = "SELECT id FROM t WHERE name = 'a' ORDER BY id;";
    assert_eq!(ids(&e, q).await, vec![1, 3], "before drop");

    e.execute("DROP INDEX idx;").await.unwrap();
    assert_eq!(
        ids(&e, q).await,
        vec![1, 3],
        "after drop (falls back to a scan)"
    );

    // recreate: the rebuilt index must answer the same rows
    e.execute("CREATE INDEX idx ON t (name);").await.unwrap();
    assert_eq!(ids(&e, q).await, vec![1, 3], "after recreate");

    // and new rows are visible through the rebuilt index
    e.execute("INSERT INTO t VALUES (4, 'a');").await.unwrap();
    assert_eq!(
        ids(&e, q).await,
        vec![1, 3, 4],
        "after insert into the rebuilt index"
    );
}
