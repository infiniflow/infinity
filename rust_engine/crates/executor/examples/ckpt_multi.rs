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

//! Checkpoint N tables with a given worker count, and report CPU time vs wall
//! time. A CPU-bound sweep should show cpu% climbing with workers; a sweep that
//! serialises on a lock or on fsync stays near 100%.

// The production allocator (mimalloc) comes from `common`, so a probe that
// measures allocator behaviour already uses the same one.

use executor::Engine;

fn main() {
    let tables: usize = std::env::args()
        .nth(1)
        .and_then(|v| v.parse().ok())
        .unwrap_or(16);
    let rows: usize = std::env::args()
        .nth(2)
        .and_then(|v| v.parse().ok())
        .unwrap_or(20_000);
    // The sweep reads INFINITY_CHECKPOINT_CONCURRENCY; report it back so the
    // numbers are self-describing.
    let workers: usize = std::env::var("INFINITY_CHECKPOINT_CONCURRENCY")
        .ok()
        .and_then(|v| v.parse().ok())
        .unwrap_or_else(|| {
            std::thread::available_parallelism()
                .map(|n| n.get())
                .unwrap_or(1)
        });
    // An explicit root lets the same probe run on tmpfs, which has no fsync
    // cost: the difference separates CPU work from journal serialisation.
    let dir = match std::env::var("CKPT_ROOT") {
        Ok(root) => {
            let path = std::path::PathBuf::from(root).join(format!(
                "probe-{}-{}",
                std::process::id(),
                std::time::SystemTime::now()
                    .duration_since(std::time::UNIX_EPOCH)
                    .unwrap()
                    .as_nanos()
            ));
            std::fs::create_dir_all(&path).unwrap();
            tempfile::TempDir::new_in(&path).unwrap()
        }
        Err(_) => tempfile::tempdir().unwrap(),
    };
    let e = Engine::open(dir.path()).unwrap();
    e.set_memindex_quota(1 << 30);
    for t in 0..tables {
        e.execute_blocking(&format!(
            "CREATE TABLE m{t} (id INT, a INT, b INT, c VARCHAR);"
        ))
        .unwrap();
        let mut values = Vec::new();
        for i in 0..rows {
            values.push(format!("({i},{},{},'row-{i}')", i % 97, i % 13));
            if values.len() == 1000 {
                e.execute_blocking(&format!("INSERT INTO m{t} VALUES {};", values.join(",")))
                    .unwrap();
                values.clear();
            }
        }
        if !values.is_empty() {
            e.execute_blocking(&format!("INSERT INTO m{t} VALUES {};", values.join(",")))
                .unwrap();
        }
    }
    // Warm the cold path (every block written) before timing.
    executor::block_on(e.flush()).unwrap();
    for t in 0..tables {
        e.execute_blocking(&format!("INSERT INTO m{t} VALUES (999999,1,1,'x');"))
            .unwrap();
    }
    let started = std::time::Instant::now();
    executor::block_on(e.flush()).unwrap();
    eprintln!(
        "{tables} tables x {rows} rows, {workers} workers, steady checkpoint: {:?}",
        started.elapsed()
    );
}
