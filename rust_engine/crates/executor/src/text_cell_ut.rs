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

//! A text column keeps its cell through the stored form.
//!
//! A stored row is text, and reading it back used to re-infer each cell's type.
//! `str::parse::<f64>` accepts `inf`, `infinity` and `nan` in **any** case
//! (`from_str` compares them with `eq_ignore_ascii_case`), so a stored `iNF`
//! came back as `inf` - the flake `test/sql/dql/sort_top/big_top_varchar.slt`
//! caught on a random fixture - while `007` came back as `7` and `1.50` as
//! `1.5`. A `JSON` cell had the same problem, and a `VARCHAR` cell that spells
//! `null` must still decode as NULL, because the stored form has no other way to
//! say it.

use crate::{Engine, QueryResult};
use tempfile::tempdir;

async fn rows(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in engine.execute(sql).await.unwrap() {
        if let QueryResult::Rows { .. } = result {
            return result.rows_text();
        }
    }
    Vec::new()
}

fn texts(rows: Vec<Vec<String>>) -> Vec<String> {
    rows.into_iter().map(|r| r.join(" ")).collect()
}

#[tokio::test]
async fn varchar_cell_is_not_retyped_on_the_way_back() {
    let dir = tempdir().unwrap();
    let engine = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    rows(&engine, "CREATE TABLE t (c1 VARCHAR, c2 INT);").await;
    for (i, cell) in ["iNF", "007", "1.50", "infinity", "nan", "true", "null"]
        .iter()
        .enumerate()
    {
        rows(
            &engine,
            &format!("INSERT INTO t VALUES ('{cell}', {});", i + 1),
        )
        .await;
    }

    // `null` is the spelling the stored form writes for a NULL cell, so that one
    // cell reads back as NULL (rendered `null`); the other six keep their bytes.
    assert_eq!(
        texts(rows(&engine, "SELECT c1 FROM t ORDER BY c2;").await),
        vec!["iNF", "007", "1.50", "infinity", "nan", "true", "null"]
    );
    assert_eq!(
        texts(rows(&engine, "SELECT count(*) FROM t WHERE c1 IS NULL;").await),
        vec!["1"]
    );
    assert_eq!(
        texts(rows(&engine, "SELECT c2 FROM t WHERE c1 = 'iNF';").await),
        vec!["1"]
    );
}

/// The read has to survive a block round trip, not just the in-memory append
/// buffer: `flush` seals the blocks and the second read comes off the files.
#[tokio::test]
async fn varchar_cell_survives_a_flush() {
    let dir = tempdir().unwrap();
    let engine = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    rows(&engine, "CREATE TABLE t (c1 VARCHAR, c2 INT);").await;
    rows(&engine, "INSERT INTO t VALUES ('iNF', 1);").await;
    rows(&engine, "INSERT INTO t VALUES ('inf', 2);").await;

    crate::block_on(engine.flush()).unwrap();

    assert_eq!(
        texts(rows(&engine, "SELECT c1, c2 FROM t ORDER BY c2;").await),
        vec!["iNF 1", "inf 2"]
    );
}
