//! Local A/B harness for the result path: same workload on the pre- and
//! post-`DataBlock` trees, in process, after a checkpoint so the rows are read
//! back out of sealed blocks (the case the columnar reader exists for).
//!
//! Only `execute_blocking` and `rows.len()` are used, so this compiles against
//! both trees.

use std::time::{Duration, Instant};

use executor::{Engine, QueryResult};

fn rows_of(result: &[QueryResult]) -> usize {
    match result.first() {
        Some(QueryResult::Rows { rows, .. }) => rows.len(),
        _ => 0,
    }
}

fn best(e: &Engine, sql: &str, runs: usize) -> (Duration, usize) {
    let mut best = Duration::from_secs(3600);
    let mut rows = 0;
    for _ in 0..runs {
        let t0 = Instant::now();
        let out = e.execute_blocking(sql).expect("execute");
        best = best.min(t0.elapsed());
        rows = rows_of(&out);
    }
    (best, rows)
}

fn main() {
    let dir = std::env::args()
        .nth(1)
        .unwrap_or_else(|| "/tmp/result-bench".into());
    let n: usize = std::env::args()
        .nth(2)
        .and_then(|s| s.parse().ok())
        .unwrap_or(40000);
    let _ = std::fs::remove_dir_all(&dir);
    let e = Engine::open(&dir).expect("open");
    let _ = e.execute_blocking("DROP TABLE IF EXISTS t;");
    let _ = e.execute_blocking("CREATE TABLE t (id INT, name VARCHAR);");
    let mut i = 0;
    while i < n {
        let batch = (n - i).min(100);
        let vals: Vec<String> = (0..batch)
            .map(|j| format!("({}, 'v{}')", i + j, i + j))
            .collect();
        e.execute_blocking(&format!("INSERT INTO t VALUES {};", vals.join(", ")))
            .expect("insert");
        i += batch;
    }
    let t0 = Instant::now();
    executor::block_on(e.flush()).expect("flush");
    println!("checkpoint: {:?}", t0.elapsed());

    for sql in [
        "SELECT * FROM t;",
        "SELECT id FROM t;",
        "SELECT name FROM t;",
        "SELECT * FROM t WHERE id = 39999;",
        "SELECT * FROM t ORDER BY id;",
        "SELECT count(*) FROM t;",
    ] {
        let (elapsed, rows) = best(&e, sql, 5);
        println!("{elapsed:>12?}  {rows:>6} rows  {sql}");
    }
    let _ = std::fs::remove_dir_all(&dir);
}
