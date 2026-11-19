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

//! Secondary-index insert throughput, split by phase.

use executor::Engine;

fn main() {
    let rows: usize = std::env::args()
        .nth(1)
        .and_then(|v| v.parse().ok())
        .unwrap_or(200_000);
    let batch: usize = std::env::args()
        .nth(2)
        .and_then(|v| v.parse().ok())
        .unwrap_or(5000);
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.set_memindex_capacity(1 << 30);
    e.execute_blocking("CREATE TABLE sec (id INTEGER, a INTEGER, b VARCHAR)")
        .unwrap();
    // One high-cardinality int and one varchar, the shapes a secondary index
    // actually holds.
    // `SEC_NO_INDEX=1` measures the same workload without the indexes, which
    // is what the index work is judged against.
    match std::env::var("SEC_MODE").as_deref() {
        Ok("none") => {}
        Ok("int") => {
            e.execute_blocking("CREATE INDEX sec_a ON sec (a)").unwrap();
        }
        Ok("varchar") => {
            e.execute_blocking("CREATE INDEX sec_b ON sec (b)").unwrap();
        }
        _ => {
            e.execute_blocking("CREATE INDEX sec_a ON sec (a)").unwrap();
            e.execute_blocking("CREATE INDEX sec_b ON sec (b)").unwrap();
        }
    }
    let mut values = Vec::new();
    let started = std::time::Instant::now();
    for i in 0..rows {
        values.push(format!("({i}, {}, 'k{}')", i, i % 50_000));
        if values.len() == batch {
            e.execute_blocking(&format!("INSERT INTO sec VALUES {};", values.join(",")))
                .unwrap();
            values.clear();
        }
    }
    if !values.is_empty() {
        e.execute_blocking(&format!("INSERT INTO sec VALUES {};", values.join(",")))
            .unwrap();
    }
    eprintln!(
        "{rows} rows, batch {batch}, 2 secondary indexes: {:?} ({:.0} rows/s)",
        started.elapsed(),
        rows as f64 / started.elapsed().as_secs_f64()
    );
}
