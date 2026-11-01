//! Times the stages of a PLAID query.
//!
//! `cargo run --release -p infinity-index --example plaid_query_bench -- [rows] [queries]`

use std::time::Instant;

use infinity_index::plaid::{Bitmask, PlaidIndex};

fn main() {
    let mut args = std::env::args().skip(1);
    let rows: usize = args.next().and_then(|s| s.parse().ok()).unwrap_or(20_000);
    let queries: usize = args.next().and_then(|s| s.parse().ok()).unwrap_or(200);
    let dims = 8usize;
    let per_row = 4usize;

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
    let mut index = PlaidIndex::new(0, dims as u32, 4, k, false).unwrap();
    index.train(k, &flat, total, 4).unwrap();
    index.add_multiple_docs_embeddings(&flat, &lens).unwrap();
    index.finalize_ivf();
    println!("built rows {rows} k {k}");

    let mut qs = Vec::new();
    for j in 0..per_row {
        qs.push([
            0.5 + j as f32 * 0.001,
            0.5,
            0.25,
            0.75,
            0.02,
            0.02,
            0.02,
            0.02,
        ]);
    }
    let query: Vec<f32> = qs.iter().flatten().copied().collect();
    let mask = Bitmask::new_all_true(index.doc_num() as usize);

    // Warm.
    for _ in 0..5 {
        let _ = index.search_with_bitmask(&query, per_row, 10, &mask, 8, -0.1, 1000, 100);
    }

    let t = Instant::now();
    let mut total_hits = 0;
    for _ in 0..queries {
        let r = index.search_with_bitmask(&query, per_row, 10, &mask, 8, -0.1, 1000, 100);
        total_hits += r.ids.len();
    }
    println!(
        "search {:.4} ms/query (hits {total_hits})",
        t.elapsed().as_secs_f64() * 1000.0 / queries as f64
    );

    let t = Instant::now();
    for _ in 0..queries {
        let _ = index.exact_score_batch(&query, per_row, &[0u32, 1, 2, 3, 4, 5, 6, 7]);
    }
    println!(
        "exact8 {:.4} ms",
        t.elapsed().as_secs_f64() * 1000.0 / queries as f64
    );
}
