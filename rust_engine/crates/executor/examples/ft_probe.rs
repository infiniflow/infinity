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

//! Batch full-text insert throughput: is tokenizing a batch on the pool worth
//! it over the row-by-row path?

use executor::Engine;

fn main() {
    executor::scheduler().block_on_runtime(run());
}

async fn run() {
    let rows: usize = std::env::args()
        .nth(1)
        .and_then(|v| v.parse().ok())
        .unwrap_or(20_000);
    let batch: usize = std::env::args()
        .nth(2)
        .and_then(|v| v.parse().ok())
        .unwrap_or(1000);
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.set_memindex_capacity(1 << 30);
    e.execute("CREATE TABLE ft (id INTEGER, body VARCHAR)")
        .await
        .unwrap();
    e.execute("CREATE INDEX ft_idx ON ft (body) USING FULLTEXT")
        .await
        .unwrap();
    // A prose-like document: ~40 words, the shape the bench corpus uses.
    // A realistic corpus vocabulary (the fulltext bench uses 20k); a tiny
    // vocabulary would make the local dictionaries all hits and hide the cost
    // that matters.
    let vocab: usize = std::env::args()
        .nth(3)
        .and_then(|v| v.parse().ok())
        .unwrap_or(20_000);
    let words: Vec<String> = (0..vocab).map(|i| format!("word{i}")).collect();
    let mut values = Vec::new();
    let started = std::time::Instant::now();
    for i in 0..rows {
        let doc: Vec<&str> = (0..40)
            .map(|k| words[(i * 7 + k * 13) % words.len()].as_str())
            .collect();
        values.push(format!("({i}, '{}')", doc.join(" ")));
        if values.len() == batch {
            e.execute(&format!("INSERT INTO ft VALUES {};", values.join(",")))
                .await
                .unwrap();
            values.clear();
        }
    }
    if !values.is_empty() {
        e.execute(&format!("INSERT INTO ft VALUES {};", values.join(",")))
            .await
            .unwrap();
    }
    eprintln!(
        "{rows} docs, batch {batch}: {:?} ({:.0} docs/s)",
        started.elapsed(),
        rows as f64 / started.elapsed().as_secs_f64()
    );
}
