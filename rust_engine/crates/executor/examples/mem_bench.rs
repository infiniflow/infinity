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

//! Throwaway driver: what bounding the block-map cache costs *index* work.
//!
//!   mem_bench <dir> <rows> <dim> <budget_mb> <iters> build
//!   mem_bench <dir> <rows> <dim> <budget_mb> <iters> measure
//!   mem_bench <dir> <rows> <dim> <budget_mb> <iters> concurrent <threads>
//!
//! `build` creates the tables, loads them through `COPY`, flushes the blocks and
//! creates the HNSW, SPANN and FULLTEXT indexes (timing each), so every later
//! run only measures queries. Every run opens the engine with
//! `buffer_manager_size = budget_mb`, so the only thing that differs between
//! runs is the budget.

use common::Result;
use std::io::Write;
use std::path::Path;
use std::sync::atomic::{AtomicUsize, Ordering};
use std::sync::{Arc, Mutex};
use std::time::Instant;

use executor::Engine;

fn rss_kb() -> u64 {
    if let Ok(text) = std::fs::read_to_string("/proc/self/status") {
        for line in text.lines() {
            if let Some(rest) = line.strip_prefix("VmRSS:") {
                if let Some(kb) = rest.split_whitespace().next() {
                    return kb.parse().unwrap_or(0);
                }
            }
        }
    }
    let out = std::process::Command::new("ps")
        .args(["-o", "rss=", "-p", &std::process::id().to_string()])
        .output();
    out.ok()
        .and_then(|o| String::from_utf8(o.stdout).ok())
        .and_then(|s| s.trim().parse::<u64>().ok())
        .unwrap_or(0)
}

/// The runtime this example drives the engine on.
///
/// The engine's API is async; an example is a synchronous `main`, so it owns a
/// runtime of its own and `block_on`s - the shape a binary entry point uses
/// (a runtime created here is not a pool worker anyone else waits on).
fn rt() -> &'static tokio::runtime::Runtime {
    static RT: std::sync::OnceLock<tokio::runtime::Runtime> = std::sync::OnceLock::new();
    RT.get_or_init(|| {
        tokio::runtime::Builder::new_multi_thread()
            .enable_all()
            .build()
            .expect("build the example runtime")
    })
}

/// Runs one statement to completion.
fn run(e: &Engine, sql: &str) -> Result<Vec<executor::QueryResult>> {
    rt().block_on(e.execute(sql))
}

fn rows_of(e: &Engine, sql: &str) -> usize {
    let out = run(e, sql).unwrap_or_else(|err| panic!("{sql}: {err:?}"));
    out.iter()
        .filter_map(|r| match r {
            executor::QueryResult::Rows { rows, .. } => Some(rows.len()),
            _ => None,
        })
        .max()
        .unwrap_or(0)
}

fn vec_text(i: usize, dim: usize) -> String {
    let body: Vec<String> = (0..dim)
        .map(|d| {
            let v = ((i * 31 + d * 17) % 1000) as f32 / 1000.0;
            format!("{v:.3}")
        })
        .collect();
    format!("[{}]", body.join(","))
}

fn timed(label: &str, f: impl FnOnce()) {
    let t0 = Instant::now();
    f();
    println!("  {label:<10} {:>8.2} s", t0.elapsed().as_secs_f64());
}

fn build(e: &Engine, dir: &Path, rows: usize, dim: usize) {
    run(
        e,
        &format!("CREATE TABLE bench (id INT, v EMBEDDING(FLOAT, {dim}), body VARCHAR);"),
    )
    .ok();
    let path = dir.join("bench.csv");
    {
        let mut w = std::io::BufWriter::new(std::fs::File::create(&path).unwrap());
        for i in 0..rows {
            writeln!(
                w,
                "{i},\"{}\",\"alpha w{} beta gamma\"",
                vec_text(i, dim),
                i % 1000
            )
            .unwrap();
        }
        w.flush().unwrap();
    }
    timed("copy", || {
        run(
            e,
            &format!(
                "COPY bench FROM '{}' WITH (DELIMITER ',', FORMAT CSV);",
                path.display()
            ),
        )
        .expect("COPY");
    });
    timed("flush", || {
        run(e, "FLUSH;").expect("FLUSH");
    });
    timed("hnsw", || {
        run(
            e,
            "CREATE INDEX bench_hnsw ON bench (v) USING HNSW \
             WITH (metric='l2', encode='plain', m=16, ef_construction=200);",
        )
        .expect("CREATE INDEX hnsw");
    });
    timed("fulltext", || {
        run(e, "CREATE INDEX bench_ft ON bench (body) USING FULLTEXT;")
            .expect("CREATE INDEX fulltext");
    });

    // The same vectors in their own table: one vector index per column, and the
    // SPANN tree is built by reading them through the block cache (`VectorColumn`),
    // which is the other half of this change.
    run(
        e,
        &format!("CREATE TABLE bench_sp (id INT, v EMBEDDING(FLOAT, {dim}));"),
    )
    .ok();
    let sp_path = dir.join("spann.csv");
    {
        let mut w = std::io::BufWriter::new(std::fs::File::create(&sp_path).unwrap());
        for i in 0..rows {
            writeln!(w, "{i},\"{}\"", vec_text(i, dim)).unwrap();
        }
        w.flush().unwrap();
    }
    timed("sp copy", || {
        run(
            e,
            &format!(
                "COPY bench_sp FROM '{}' WITH (DELIMITER ',', FORMAT CSV);",
                sp_path.display()
            ),
        )
        .expect("COPY bench_sp");
    });
    timed("sp flush", || {
        run(e, "FLUSH;").expect("FLUSH");
    });
    timed("spann", || {
        run(
            e,
            "CREATE INDEX bench_sp_i ON bench_sp (v) USING SPANN \
             WITH (min_rows=1, metric='l2', max_partition_size=256, num_centroids=64);",
        )
        .expect("CREATE INDEX spann");
    });
}

fn queries(rows: usize, dim: usize) -> (String, String, String) {
    let probe = vec_text(rows / 2, dim);
    (
        format!("SELECT id FROM bench SEARCH MATCH VECTOR (v, {probe}, 'float', 'l2', 10);"),
        format!("SELECT id FROM bench_sp SEARCH MATCH VECTOR (v, {probe}, 'float', 'l2', 10);"),
        format!(
            "SELECT id FROM bench SEARCH MATCH TEXT (body, 'w{}', 20);",
            (rows / 2) % 1000
        ),
    )
}

fn bench_one(e: &Engine, name: &str, sql: &str, iters: usize, expect_rows: usize) {
    let got = rows_of(e, sql);
    assert_eq!(got, expect_rows, "{name}: wrong row count");
    let mut lat = Vec::with_capacity(iters);
    for _ in 0..iters {
        let t0 = Instant::now();
        let _ = rows_of(e, sql);
        lat.push(t0.elapsed().as_secs_f64());
    }
    lat.sort_by(|a, b| a.partial_cmp(b).unwrap());
    let sum: f64 = lat.iter().sum();
    println!(
        "  {name:<12} n={iters:<3} p50 {:>8.2} ms  p95 {:>8.2} ms  qps {:>8.2}",
        lat[lat.len() / 2] * 1000.0,
        lat[(lat.len() as f64 * 0.95) as usize] * 1000.0,
        iters as f64 / sum
    );
}

/// `threads` clients in parallel, each looping over the three index queries:
/// the mix a server actually serves, plus the contention a shared cache and a
/// shared `block_maps` mutex introduce.
fn concurrent(e: Arc<Engine>, rows: usize, dim: usize, iters: usize, threads: usize) {
    let (_, spann, ft) = queries(rows, dim);
    let lat = Arc::new(Mutex::new(Vec::<f64>::new()));
    let done = Arc::new(AtomicUsize::new(0));
    let t0 = Instant::now();
    let mut handles = Vec::new();
    for t in 0..threads {
        let e = Arc::clone(&e);
        let lat = Arc::clone(&lat);
        let done = Arc::clone(&done);
        // A different probe per thread, so the cache sees several working sets
        // instead of one shared one.
        let probe = vec_text(rows / 2 + t * 37, dim);
        let knn =
            format!("SELECT id FROM bench SEARCH MATCH VECTOR (v, {probe}, 'float', 'l2', 10);");
        let spann = spann.clone();
        let ft = ft.clone();
        handles.push(std::thread::spawn(move || {
            let mut local = Vec::with_capacity(iters * 3);
            for i in 0..iters {
                let sql = match i % 3 {
                    0 => &knn,
                    1 => &spann,
                    _ => &ft,
                };
                let q = Instant::now();
                let _ = rows_of(&e, sql);
                local.push(q.elapsed().as_secs_f64());
                done.fetch_add(1, Ordering::Relaxed);
            }
            lat.lock().unwrap().extend(local);
        }));
    }
    for h in handles {
        h.join().unwrap();
    }
    let wall = t0.elapsed().as_secs_f64();
    let mut lat = lat.lock().unwrap().clone();
    lat.sort_by(|a, b| a.partial_cmp(b).unwrap());
    let total = done.load(Ordering::Relaxed);
    println!(
        "  concurrent   t={threads:<3} n={total:<5} qps {:>8.2}  p50 {:>7.2} ms  p99 {:>7.2} ms",
        total as f64 / wall,
        lat[lat.len() / 2] * 1000.0,
        lat[(lat.len() as f64 * 0.99) as usize] * 1000.0
    );
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let dir = std::path::PathBuf::from(&args[1]);
    let rows: usize = args[2].parse().unwrap();
    let dim: usize = args[3].parse().unwrap();
    let budget_mb: usize = args[4].parse().unwrap();
    let iters: usize = args[5].parse().unwrap();
    let mode = args.get(6).map(String::as_str).unwrap_or("measure");

    let e = Arc::new(
        rt().block_on(Engine::open_async(&dir, Default::default()))
            .expect("open"),
    );
    e.set_buffer_budget(budget_mb * 1024 * 1024);
    if mode == "build" {
        build(&e, &dir, rows, dim);
    }
    println!(
        "budget {budget_mb} MB, rss {} MB, blocks {} mapped {} MB",
        rss_kb() / 1024,
        e.buffer_objects().len(),
        e.buffer_used_bytes() / (1024 * 1024)
    );

    let (knn, spann, ft) = queries(rows, dim);
    if mode == "concurrent" {
        let threads: usize = args.get(7).and_then(|s| s.parse().ok()).unwrap_or(8);
        concurrent(Arc::clone(&e), rows, dim, iters, threads);
    } else {
        bench_one(&e, "knn_hnsw", &knn, iters, 10);
        bench_one(&e, "knn_spann", &spann, iters, 10);
        bench_one(&e, "fulltext", &ft, iters, 20);
    }
    println!(
        "  after:  rss {} MB, blocks {} mapped {} MB",
        rss_kb() / 1024,
        e.buffer_objects().len(),
        e.buffer_used_bytes() / (1024 * 1024)
    );
}
