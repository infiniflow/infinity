use std::time::Instant;

use infinity_index::vector::{HnswEncoding, HnswIndex, Metric};

fn dataset(rows: usize, dims: usize) -> Vec<Vec<f32>> {
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
    let rows = 20_000usize;
    let dims = 4usize;
    let data = dataset(rows, dims);
    for threads in [1usize, 8] {
        for ef in [10usize, 200] {
            let mut index =
                HnswIndex::with_encoding(dims, 16, 200, 64, Metric::L2, HnswEncoding::Plain);
            let t = Instant::now();
            index.add_batch(&data, threads);
            let build = t.elapsed().as_secs_f64();
            // The bench driver's query: k=10 with no `ef`, i.e. ef = k.
            let t = Instant::now();
            let mut hits = 0usize;
            let probes = 50;
            for (i, q) in data.iter().take(probes).enumerate() {
                let got = index.search_with_ef(q, 10, ef);
                if got.first().is_some_and(|(id, _)| *id == i) {
                    hits += 1;
                }
            }
            let q_ms = t.elapsed().as_secs_f64() * 1000.0 / probes as f64;
            println!(
                "threads {threads} ef {ef}: build {build:.4}s  query {q_ms:.3} ms  top1 {:.3}  links_l0_avg {:.1}",
                hits as f64 / probes as f64,
                (0..rows).map(|n| index.neighbors(0, n).len()).sum::<usize>() as f64 / rows as f64
            );
        }
    }
}