//! One-process large-scale full-text workload: insert, dump, merge, query,
//! delete, merge, reopen.
//!
//! ```text
//! cargo run --example fulltext_scale -j 4 -- 3000000 [/tmp/ft-scale]
//! ```
//!
//! It drives the engine directly (no server, no ports), so it can run beside a
//! gate that already owns the fixed Thrift port. Invariants are the table's
//! own content: `uniq{i}` appears in exactly one document, `bucket{b}` covers
//! exactly the rows with `i % 7 == b`.
//!
//! Env: `FT_SCALE_PHRASE=0` skips the phrase query (it materialises every
//! matching document's positions - see docs/FULLTEXT_BENCH.md §13).

use std::collections::BTreeSet;
use std::time::Instant;

use executor::Engine;

const BATCH: usize = 2000;

fn body(i: usize) -> String {
    format!(
        "alpha beta bucket{} gamma delta uniq{i} alpha epsilon zeta eta theta uniq{i} iota kappa",
        i % 7
    )
}

fn rss_mb() -> f64 {
    let status = std::fs::read_to_string("/proc/self/status").unwrap_or_default();
    for line in status.lines() {
        if let Some(rest) = line.strip_prefix("VmRSS:") {
            let kb: f64 = rest
                .trim()
                .trim_end_matches(" kB")
                .trim()
                .parse()
                .unwrap_or(0.0);
            return kb / 1024.0;
        }
    }
    0.0
}

fn dir_mb(path: &std::path::Path) -> u64 {
    let mut total = 0;
    let Ok(entries) = std::fs::read_dir(path) else {
        return 0;
    };
    for entry in entries.flatten() {
        let path = entry.path();
        match entry.file_type() {
            Ok(kind) if kind.is_dir() => total += dir_mb(&path),
            Ok(_) => total += entry.metadata().map(|meta| meta.len()).unwrap_or(0),
            Err(_) => {}
        }
    }
    total / 1_000_000
}

fn ids(e: &Engine, sql: &str) -> Vec<i64> {
    use executor::QueryResult;
    for result in e.execute_blocking(sql).unwrap() {
        if let QueryResult::Rows { .. } = result {
            return result
                .rows_text()
                .iter()
                .filter_map(|row| row.first().and_then(|value| value.parse().ok()))
                .collect();
        }
    }
    Vec::new()
}

fn count(e: &Engine, sql: &str) -> i64 {
    ids(e, sql).first().copied().unwrap_or(-1)
}

fn timed<T>(label: &str, run: impl FnOnce() -> T) -> T {
    let started = Instant::now();
    let out = run();
    println!("  {label}: {:.1} ms", started.elapsed().as_secs_f64() * 1000.0);
    out
}

fn main() {
    let mut args = std::env::args().skip(1);
    let rows: usize = args
        .next()
        .and_then(|v| v.parse().ok())
        .unwrap_or(300_000);
    let dir = match args.next() {
        Some(path) => std::path::PathBuf::from(path),
        None => tempfile::tempdir().unwrap().keep(),
    };
    let phrase = std::env::var("FT_SCALE_PHRASE")
        .map(|v| v != "0")
        .unwrap_or(true);
    // `filter_fulltext` is a predicate the planner evaluates per row (it is
    // O(rows), not O(matches)), so a probe costs milliseconds per 100k rows;
    // fewer probes keeps a large run's check phase bounded.
    let probes: usize = std::env::var("FT_SCALE_PROBES")
        .ok()
        .and_then(|v| v.parse().ok())
        .unwrap_or(5);
    std::fs::create_dir_all(&dir).unwrap();
    println!("{rows} rows under {}", dir.display());

    let e = Engine::open(&dir).unwrap();
    // A bounded tail: the memory index must spill instead of growing with the
    // corpus, which is the shape a 10M import has.
    e.set_memindex_quota(512 << 20);
    e.execute_blocking("CREATE TABLE t (id INT, title VARCHAR, body VARCHAR);")
        .unwrap();
    e.execute_blocking("CREATE INDEX t_ft ON t (body) USING FULLTEXT;")
        .unwrap();

    let started = Instant::now();
    let mut done = 0;
    while done < rows {
        let count = BATCH.min(rows - done);
        let values: Vec<String> = (done..done + count)
            .map(|i| format!("({i}, 'r{i}', '{}')", body(i)))
            .collect();
        e.execute_blocking(&format!("INSERT INTO t VALUES {};", values.join(",")))
            .unwrap();
        done += count;
        if done % (BATCH * 100) == 0 {
            println!(
                "  insert {done}/{rows} {:.0} rows/s, rss {:.0} MB",
                done as f64 / started.elapsed().as_secs_f64(),
                rss_mb()
            );
        }
    }
    println!(
        "insert {rows} rows: {:.1} s = {:.0} rows/s",
        started.elapsed().as_secs_f64(),
        rows as f64 / started.elapsed().as_secs_f64()
    );

    timed("flush", || executor::block_on(e.flush()).unwrap());
    timed("dump", || {
        executor::block_on(e.dump_index_transaction("t", "t_ft")).unwrap()
    });

    let check = |e: &Engine, label: &str, deleted: &BTreeSet<i64>| {
        for i in (0..rows).step_by((rows / probes).max(1)) {
            let want = if deleted.contains(&(i as i64)) { 0 } else { 1 };
            let found = timed(&format!("uniq{i}"), || {
                count(e, &format!("SELECT count(*) FROM t WHERE filter_fulltext('body', 'uniq{i}');"))
            });
            assert_eq!(found, want, "{label}: uniq{i} -> {found}, wanted {want}");
        }
        for bucket in [0, 3, 6] {
            let alive = (0..rows).filter(|i| i % 7 == bucket).count() as i64
                - deleted.iter().filter(|i| (*i % 7) as usize == bucket).count() as i64;
            let found = timed(&format!("bucket{bucket} count"), || {
                count(
                    e,
                    &format!("SELECT count(*) FROM t WHERE filter_fulltext('body', 'bucket{bucket}');"),
                )
            });
            assert_eq!(found, alive, "{label}: bucket{bucket} -> {found}, wanted {alive}");
        }
        let top = timed("alpha beta gamma top-10", || {
            ids(
                e,
                "SELECT id FROM t SEARCH MATCH TEXT (body, 'alpha beta gamma', 10);",
            )
        });
        assert_eq!(top.len(), 10, "{label}: three-term top-10 -> {}", top.len());
        if phrase {
            let hits = timed("phrase top-10", || {
                ids(
                    e,
                    "SELECT id FROM t SEARCH MATCH TEXT (body, '\"alpha beta\"', 10);",
                )
            });
            assert_eq!(hits.len(), 10, "{label}: phrase top-10 -> {}", hits.len());
        }
    };
    let none = BTreeSet::new();
    check(&e, "after dump", &none);

    timed("optimize", || {
        executor::block_on(e.optimize_fulltext_async("t")).unwrap()
    });
    check(&e, "after optimize", &none);

    let victims: Vec<i64> = (0..(rows / 100).max(1)).map(|i| i as i64).collect();
    timed("delete 1%", || {
        for chunk in victims.chunks(5000) {
            let list = chunk
                .iter()
                .map(|id| id.to_string())
                .collect::<Vec<_>>()
                .join(",");
            e.execute_blocking(&format!("DELETE FROM t WHERE id IN ({list});"))
                .unwrap();
        }
    });
    let deleted: BTreeSet<i64> = victims.iter().copied().collect();
    check(&e, "after delete", &deleted);

    timed("optimize after delete", || {
        executor::block_on(e.optimize_fulltext_async("t")).unwrap()
    });
    check(&e, "after merge", &deleted);

    println!(
        "rss {:.0} MB, data {} MB, fulltext {} MB",
        rss_mb(),
        dir_mb(&dir.join("data")),
        dir_mb(&dir.join("fulltext"))
    );

    drop(e);
    let started = Instant::now();
    let reopened = Engine::open(&dir).unwrap();
    println!(
        "reopen {:.1} s, rss {:.0} MB",
        started.elapsed().as_secs_f64(),
        rss_mb()
    );
    check(&reopened, "after restart", &deleted);
    println!("all invariants held");
}
