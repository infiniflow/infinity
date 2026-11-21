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

//! Multi-statement inputs and the `_row_id` system column.
//!
//! The result cache used to key on the whole input string, so a second
//! `SELECT` in the same call was answered with the first one's rows.

use executor::{Engine, QueryResult};

fn rows(rs: &[QueryResult]) -> Vec<Vec<String>> {
    match &rs[0] {
        QueryResult::Rows { .. } => rs[0].rows_text(),
        other => panic!("expected rows, got {other:?}"),
    }
}

#[tokio::test]
async fn every_statement_in_one_call_uses_its_own_result() {
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    let rs = e
        .execute("SELECT 1 AS a; SELECT 2 AS b; SELECT 3 AS c;")
        .await
        .unwrap();
    assert_eq!(rs.len(), 3);
    for (i, r) in rs.iter().enumerate() {
        assert_eq!(rows(std::slice::from_ref(r))[0][0], (i + 1).to_string());
    }
}

#[tokio::test]
async fn the_row_id_system_column_is_selectable() {
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE t (c1 INTEGER);").await.unwrap();
    e.execute("INSERT INTO t VALUES (10), (20), (30);")
        .await
        .unwrap();

    let rs = e
        .execute("SELECT c1, _row_id FROM t ORDER BY c1;")
        .await
        .unwrap();
    let cols = match &rs[0] {
        QueryResult::Rows { columns, .. } => columns,
        other => panic!("expected rows, got {other:?}"),
    };
    assert_eq!(cols, &vec!["c1".to_string(), "_row_id".to_string()]);
    let got: Vec<(String, String)> = rows(&rs)
        .iter()
        .map(|r| (r[0].clone(), r[1].clone()))
        .collect();
    assert_eq!(
        got,
        vec![
            ("10".into(), "0".into()),
            ("20".into(), "1".into()),
            ("30".into(), "2".into()),
        ]
    );
}
