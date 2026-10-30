//! Times HNSW graph construction alone (the part `bench_index_types.py`)
//! measures as `INDEX hnsw build`), serially and bucket-parallel.
//!
//! `cargo run --release -p infinity-index --example hnsw_build_bench -- [rows] [dims] [threads]`

use std::time::Instant;

use infinity_index::vector::{HnswEncoding, HnswIndex, Metric};

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

fn main() {
    let mut args = std::env::args().skip(1);
    let rows: usize = args.next().and_then(|s| s.parse().ok()).unwrap_or(20_000);
    let dims: usize = args.next().and_then(|s| s.parse().ok()).unwrap_or(4);
    let threads: usize = args.next().and_then(|s| s.parse().ok()).unwrap_or(1);
    let data = dataset(rows, dims);

    let mut index = HnswIndex::with_encoding(dims, 16, 200, 64, Metric::L2, HnswEncoding::Plain);
    let t = Instant::now();
    let added = index.add_batch(&data, threads);
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
