//! Splits a PLAID build into k-means training, quantizer training and indexing.
//!
//! `cargo run --release -p infinity-index --example plaid_build_bench -- [rows] [vecs_per_row]`

use std::time::Instant;

use infinity_index::plaid::centroids::kmeans_l2;
use infinity_index::plaid::{PlaidIndex, PlaidQuantizer};

fn main() {
    let mut args = std::env::args().skip(1);
    let rows: usize = args.next().and_then(|s| s.parse().ok()).unwrap_or(20_000);
    let per_row: usize = args.next().and_then(|s| s.parse().ok()).unwrap_or(4);
    let dims = 8usize;

    let mut state = 0x2545_F491_4F6C_DD1Du64;
    let mut next = || {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        ((state >> 40) as f32 / (1u64 << 24) as f32) - 0.5
    };
    let mut flat = Vec::with_capacity(rows * per_row * dims);
    let mut lens = Vec::with_capacity(rows);
    for i in 0..rows {
        let cx = (i % 97) as f32 / 97.0;
        let cy = (i % 13) as f32 / 13.0;
        for _ in 0..per_row {
            flat.extend_from_slice(&[
                cx + next() * 0.02,
                cy + next() * 0.02,
                0.25,
                0.75,
                next(),
                next(),
                next(),
                next(),
            ]);
        }
        lens.push(per_row as u32);
    }
    let total = rows * per_row;

    let k = PlaidIndex::compute_auto_n_centroids(total as u64);
    println!("rows {rows} embeddings {total} dims {dims} n_centroids {k}");

    let t = Instant::now();
    let centroids = kmeans_l2(dims, &flat, total, k as usize, 4).unwrap();
    println!("kmeans {:.4}s", t.elapsed().as_secs_f64());
    let _ = centroids;

    let mut index = PlaidIndex::new(0, dims as u32, 4, k, false).unwrap();
    let t = Instant::now();
    index.train(k, &flat, total, 4).unwrap();
    println!("train(kmeans+quantizer) {:.4}s", t.elapsed().as_secs_f64());

    let t = Instant::now();
    index.add_multiple_docs_embeddings(&flat, &lens).unwrap();
    println!("add {:.4}s", t.elapsed().as_secs_f64());

    let mut q = PlaidQuantizer::new(4, dims as u32).unwrap();
    let t = Instant::now();
    q.train(&flat, total).unwrap();
    println!("quantizer alone {:.4}s", t.elapsed().as_secs_f64());
}
