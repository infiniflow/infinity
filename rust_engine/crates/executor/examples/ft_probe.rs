//! Batch full-text insert throughput: is tokenizing a batch on the pool worth
//! it over the row-by-row path?

use executor::Engine;

fn main() {
    let rows: usize = std::env::args()
        .nth(1)
        .and_then(|v| v.parse().ok())
        .unwrap_or(20_000);
    let batch: usize = std::env::args()
        .nth(2)
        .and_then(|v| v.parse().ok())
        .unwrap_or(1000);
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.set_memindex_capacity(1 << 30);
    e.execute_blocking("CREATE TABLE ft (id INTEGER, body VARCHAR)")
        .unwrap();
    e.execute_blocking("CREATE INDEX ft_idx ON ft (body) USING FULLTEXT")
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
            e.execute_blocking(&format!("INSERT INTO ft VALUES {};", values.join(",")))
                .unwrap();
            values.clear();
        }
    }
    if !values.is_empty() {
        e.execute_blocking(&format!("INSERT INTO ft VALUES {};", values.join(",")))
            .unwrap();
    }
    eprintln!(
        "{rows} docs, batch {batch}: {:?} ({:.0} docs/s)",
        started.elapsed(),
        rows as f64 / started.elapsed().as_secs_f64()
    );
}
