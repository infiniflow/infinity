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

//! The multi-vector memory tail is searched through the in-memory EMVB index
//! (`EMVBIndexInMem`), not by scanning every row.
//!
//! `INFINITY_EMVB_BUILD_THRESHOLD` is lowered to 1 so the build triggers on the
//! first tail document; this test binary is a separate process from the other
//! tests, so the `OnceLock` cannot be pre-initialized.

use executor::{Engine, QueryResult};

fn rows(rs: &[QueryResult]) -> Vec<Vec<String>> {
    match &rs[0] {
        QueryResult::Rows { .. } => rs[0].rows_text(),
        other => panic!("expected rows, got {other:?}"),
    }
}

#[test]
fn the_memory_tail_is_searched_through_the_in_memory_emvb_index() {
    std::env::set_var("INFINITY_EMVB_BUILD_THRESHOLD", "1");
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    // EMVB is a tensor-column index in the reference
    // (`IndexEMVB::ValidateColumnDataType` rejects a multi-vector column), so
    // the multi-vector grouping is expressed with `TENSOR(FLOAT,2)` rows.
    e.execute_blocking("CREATE TABLE mvp (id INT, v TENSOR(FLOAT,2));")
        .unwrap();
    e.execute_blocking(
        "CREATE INDEX mvp_i ON mvp (v) USING EMVB \
         WITH (pq_subspace_num = 2, pq_subspace_bits = 8);",
    )
    .unwrap();
    // Written after the index was built, so these rows sit in the memory tail.
    for i in 0..8 {
        e.execute_blocking(&format!("INSERT INTO mvp VALUES ({i}, [{i}.0, 0.0]);"))
            .unwrap();
    }
    // MaxSim with the single query token [7,0]: document i scores 7*i.
    let got = rows(
        &e.execute_blocking(
            "SELECT id FROM mvp SEARCH MATCH TENSOR \
             (v, [7.0, 0.0], 'float', 'maxsim', 'topn=3');",
        )
        .unwrap(),
    )
    .clone();
    assert_eq!(
        got,
        vec![
            vec!["7".to_string()],
            vec!["6".to_string()],
            vec!["5".to_string()]
        ]
    );
}
