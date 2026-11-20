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

//! Local A/B harness for the result path: same workload on the pre- and
//! post-`DataBlock` trees, in process, after a checkpoint so the rows are read
//! back out of sealed blocks (the case the columnar reader exists for).
//!
//! Only `execute_blocking` and `rows.len()` are used, so this compiles against
//! both trees.

use std::time::{Duration, Instant};

use executor::{Engine, QueryResult};

fn rows_of(result: &[QueryResult]) -> usize {
    match result.first() {
        Some(QueryResult::Rows { rows, .. }) => rows.len(),
        _ => 0,
    }
}

async fn best(e: &Engine, sql: &str, runs: usize) -> (Duration, usize) {
    let mut best = Duration::from_secs(3600);
    let mut rows = 0;
    for _ in 0..runs {
        let t0 = Instant::now();
        let out = e.execute(sql).await.expect("execute");
        best = best.min(t0.elapsed());
        rows = rows_of(&out);
    }
    (best, rows)
}

fn main() {
    executor::scheduler().block_on_runtime(run());
}

async fn run() {
    let dir = std::env::args()
        .nth(1)
        .unwrap_or_else(|| "/tmp/result-bench".into());
    let n: usize = std::env::args()
        .nth(2)
        .and_then(|s| s.parse().ok())
        .unwrap_or(40000);
    let _ = std::fs::remove_dir_all(&dir);
    let e = Engine::open_async(&dir, Default::default())
        .await
        .expect("open");
    let _ = e.execute("DROP TABLE IF EXISTS t;").await;
    let _ = e.execute("CREATE TABLE t (id INT, name VARCHAR);").await;
    let mut i = 0;
    while i < n {
        let batch = (n - i).min(100);
        let vals: Vec<String> = (0..batch)
            .map(|j| format!("({}, 'v{}')", i + j, i + j))
            .collect();
        e.execute(&format!("INSERT INTO t VALUES {};", vals.join(", ")))
            .await
            .expect("insert");
        i += batch;
    }
    let t0 = Instant::now();
    e.flush().await.expect("flush");
    println!("checkpoint: {:?}", t0.elapsed());

    for sql in [
        "SELECT * FROM t;",
        "SELECT id FROM t;",
        "SELECT name FROM t;",
        "SELECT * FROM t WHERE id = 39999;",
        "SELECT * FROM t ORDER BY id;",
        "SELECT count(*) FROM t;",
    ] {
        let (elapsed, rows) = best(&e, sql, 5).await;
        println!("{elapsed:>12?}  {rows:>6} rows  {sql}");
    }
    let _ = std::fs::remove_dir_all(&dir);
}
