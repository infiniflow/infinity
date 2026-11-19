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

//! Times HNSW graph construction alone (the part `bench_index_types.py`)
//! measures as `INDEX hnsw build`), serially and bucket-parallel.
//!
//! `cargo run --release -p infinity-index --example hnsw_build_bench -- [rows] [dims] [threads]`
//!
//! The bucket build fans out on **a runtime the caller provides** - the engine
//! installs its own scheduler runtime, and a standalone binary has to bring one
//! (the index crate never builds a pool of its own). Without it the buckets
//! would run one after another on this thread.

use std::time::Instant;

use index::vector::{HnswEncoding, HnswIndex, Metric};

fn dataset(rows: usize, dims: usize) -> Vec<Vec<f32>> {
    // The same generator shape as `tools/bench_index_types.py` row(): a small
    // integer lattice, which is what the C++ golden was measured on.
    (0..rows)
        .map(|i| {
            let mut v = vec![0f32; dims];
            v[0] = (i % 97) as f32 / 97.0;
            v[1] = (i % 13) as f32 / 13.0;
            for (d, slot) in v.iter_mut().enumerate().skip(2) {
                *slot = 0.25 + d as f32 * 0.1;
            }
            v
        })
        .collect()
}

/// The runtime the bucket build spawns its tasks on.
///
/// It stands in for the engine's `task-scheduler`: same multi-thread geometry,
/// and the buckets join it the way a statement's fan-out joins the engine's.
fn runtime(workers: usize) -> tokio::runtime::Runtime {
    tokio::runtime::Builder::new_multi_thread()
        .worker_threads(workers.max(1))
        .enable_all()
        .thread_name("hnsw-bench")
        .build()
        .expect("build the bench runtime")
}

fn main() {
    let mut args = std::env::args().skip(1);
    let rows: usize = args.next().and_then(|s| s.parse().ok()).unwrap_or(20_000);
    let dims: usize = args.next().and_then(|s| s.parse().ok()).unwrap_or(4);
    let threads: usize = args.next().and_then(|s| s.parse().ok()).unwrap_or(1);
    let data = dataset(rows, dims);
    let rt = runtime(threads);

    let mut index = HnswIndex::with_encoding(dims, 16, 200, 64, Metric::L2, HnswEncoding::Plain);
    let t = Instant::now();
    // The build runs inside the runtime, so its buckets spawn onto it and this
    // thread parks while they run - the same shape a statement has, where the
    // build sits on the blocking pool and the buckets join the scheduler.
    let added = rt.block_on(async { index.add_batch(&data, threads) });
    let build = t.elapsed();
    println!(
        "rows {rows} dims {dims} threads {threads} built {added} in {:.4}s  levels {}",
        build.as_secs_f64(),
        index.max_level() + 1
    );

    // Query latency and recall against the exact answer for the same corpus.
    let t = Instant::now();
    let mut hits = 0usize;
    let probes = 200.min(rows);
    for (i, q) in data.iter().take(probes).enumerate() {
        let got = index.search_with_ef(q, 10, 200);
        if got.first().is_some_and(|(id, _)| *id == i) {
            hits += 1;
        }
    }
    println!(
        "{probes} queries ef=200 in {:.4}s  top1 self-recall {:.3}",
        t.elapsed().as_secs_f64(),
        hits as f64 / probes as f64
    );
}
