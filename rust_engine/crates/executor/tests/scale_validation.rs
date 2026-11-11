//! Large-scale checks for the vector index: multi-vector recall, LSG
//! correctness, resident memory, and search/insert concurrency.
//!
//! These build tens of thousands of rows, so they are `#[ignore]`d and run on
//! demand:
//!
//! ```text
//! cargo test -p executor --release --test scale_validation -- --ignored --nocapture
//! ```
//!
//! Every test reports its numbers; the assertions pin the property each one is
//! named after, so a regression is a failure rather than a shifted printout.

use std::sync::Arc;

use executor::{Engine, QueryResult};

fn rows(rs: &[QueryResult]) -> Vec<Vec<String>> {
    match &rs[0] {
        QueryResult::Rows { .. } => rs[0].rows_text(),
        other => panic!("expected rows, got {other:?}"),
    }
}

fn texts(e: &Engine, sql: &str) -> Vec<Vec<String>> {
    rows(&e.execute(sql).unwrap())
}

/// Deterministic xorshift64*: the suite has to be reproducible without adding a
/// `rand` dependency to the workspace.
struct Rng(u64);

impl Rng {
    fn next(&mut self) -> u64 {
        let mut x = self.0;
        x ^= x >> 12;
        x ^= x << 25;
        x ^= x >> 27;
        self.0 = x;
        x.wrapping_mul(0x2545_F491_4F6C_DD1D)
    }

    fn unit(&mut self) -> f32 {
        (self.next() >> 40) as f32 / (1u64 << 24) as f32
    }

    fn vector(&mut self, dim: usize) -> Vec<f32> {
        (0..dim).map(|_| self.unit() * 2.0 - 1.0).collect()
    }
}

/// A literal the parser reads back exactly (`f32`'s shortest round-trip form).
fn lit(v: &[f32]) -> String {
    let parts: Vec<String> = v.iter().map(|x| format!("{x}")).collect();
    format!("[{}]", parts.join(","))
}

/// A **flat** query literal for a multi-token query: the query side is one array
/// of `dim`-sized tokens back to back, not a nested one.
fn flat_lit(tokens: &[Vec<f32>]) -> String {
    let parts: Vec<String> = tokens.iter().flatten().map(|x| format!("{x}")).collect();
    format!("[{}]", parts.join(","))
}

/// Resident set size in KiB, from `statm` (4 KiB pages on the platforms this
/// suite runs on).
fn rss_kb() -> u64 {
    std::fs::read_to_string("/proc/self/statm")
        .ok()
        .and_then(|s| {
            s.split_whitespace()
                .nth(1)
                .and_then(|x| x.parse::<u64>().ok())
        })
        .unwrap_or(0)
        * 4
}

/// Recall of `got` against `want`, as a fraction.
fn recall(got: &[Vec<String>], want: &[Vec<String>]) -> f32 {
    if want.is_empty() {
        return 1.0;
    }
    let hit = got.iter().filter(|row| want.contains(row)).count();
    hit as f32 / want.len() as f32
}

/// Multi-vector at scale: one vertex per token, per-token beam, exact MaxSim
/// re-scoring. The index has to reproduce the exhaustive scan's top-10 for
/// self-queries, which is what proves tokens are indexed (a graph holding one
/// vertex per *row* would rank by a single token and fall apart here).
#[test]
#[ignore = "large-scale: run with --ignored"]
fn multivector_search_keeps_recall_at_scale() {
    let (n, tokens, dim) = (8000usize, 4usize, 32usize);
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    for t in ["mvi", "mvf"] {
        e.execute(&format!(
            "CREATE TABLE {t} (id INT, v MULTIVECTOR(FLOAT,{dim}));"
        ))
        .unwrap();
    }
    let mut rng = Rng(0x5EED_1234);
    let data: Vec<Vec<Vec<f32>>> = (0..n)
        .map(|_| (0..tokens).map(|_| rng.vector(dim)).collect())
        .collect();
    for lo in (0..n).step_by(500) {
        let hi = (lo + 500).min(n);
        let body: Vec<String> = (lo..hi)
            .map(|i| {
                let cell: Vec<String> = data[i].iter().map(|t| lit(t)).collect();
                format!("({i}, [{}])", cell.join(","))
            })
            .collect();
        for t in ["mvi", "mvf"] {
            e.execute(&format!("INSERT INTO {t} VALUES {};", body.join(",")))
                .unwrap();
        }
    }
    e.execute("CREATE INDEX mvi_i ON mvi (v) USING HNSW WITH (metric='l2', M=16);")
        .unwrap();
    e.flush().unwrap();

    let mut total = 0.0f32;
    let mut queries = 0;
    for q in 0..20 {
        let src = (q * 397 + 11) % n;
        let query = flat_lit(&data[src]);
        let sql = |t: &str| {
            format!("SELECT id FROM {t} SEARCH MATCH VECTOR (v, {query}, 'float', 'l2', 10);")
        };
        let indexed = texts(&e, &sql("mvi"));
        let exact = texts(&e, &sql("mvf"));
        assert_eq!(exact.len(), 10, "the exhaustive scan returns a full top-10");
        let self_id = src.to_string();
        assert_eq!(exact[0][0], self_id, "the row itself wins its own query");
        assert_eq!(
            indexed[0][0], self_id,
            "the index ranks the row's own tokens first"
        );
        total += recall(&indexed, &exact);
        queries += 1;
    }
    let recall = total / queries as f32;
    println!(
        "multivector: n={n} tokens={tokens} dim={dim} recall@10 over {queries} self-queries = {recall:.3}"
    );
    assert!(recall >= 0.9, "recall@10 {recall:.3} < 0.9");
}

/// LSG at scale: the local scaling factor is actually applied, and recall does
/// not fall below the plain graph's.
///
/// `alpha` is the exponent of `d / (avg_i * avg_j)^alpha`. `alpha = 0` collapses
/// the formula to the plain distance, so an index built with `alpha = 0` and one
/// built with `alpha = 1.2` must answer at least one query differently — that is
/// the only direct evidence the factor reaches the distance function rather than
/// being parsed and dropped.
#[test]
#[ignore = "large-scale: run with --ignored"]
fn lsg_scaling_is_applied_and_keeps_recall() {
    let (n, dim) = (20000usize, 64usize);
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    for t in ["lgex", "lgs0", "lgs1"] {
        e.execute(&format!(
            "CREATE TABLE {t} (id INT, v EMBEDDING(FLOAT,{dim}));"
        ))
        .unwrap();
    }
    let mut rng = Rng(0xA5A5_0F0F);
    let data: Vec<Vec<f32>> = (0..n).map(|_| rng.vector(dim)).collect();
    for lo in (0..n).step_by(1000) {
        let hi = (lo + 1000).min(n);
        let body: Vec<String> = (lo..hi)
            .map(|i| format!("({i}, {})", lit(&data[i])))
            .collect();
        for t in ["lgex", "lgs0", "lgs1"] {
            e.execute(&format!("INSERT INTO {t} VALUES {};", body.join(",")))
                .unwrap();
        }
    }
    // `sample_ratio` is 1% in the reference; 5% of 20k rows is enough samples for
    // `ls_k = 10` neighbours to be meaningful.
    for (t, alpha) in [("lgs0", "0.0"), ("lgs1", "1.2")] {
        e.execute(&format!(
            "CREATE INDEX {t}_i ON {t} (v) USING HNSW WITH (metric='l2', M=16, \
             build_type='lsg', lsg_config='sample_ratio=0.05,ls_k=10,alpha={alpha}');"
        ))
        .unwrap();
    }
    e.flush().unwrap();

    let mut r0 = 0.0f32;
    let mut r1 = 0.0f32;
    let mut differing = 0;
    let mut queries = 0;
    for q in 0..20 {
        let src = (q * 977 + 3) % n;
        let sql = |t: &str| {
            format!(
                "SELECT id FROM {t} SEARCH MATCH VECTOR (v, {}, 'float', 'l2', 10);",
                lit(&data[src])
            )
        };
        let exact = texts(&e, &sql("lgex"));
        let zero = texts(&e, &sql("lgs0"));
        let scaled = texts(&e, &sql("lgs1"));
        assert_eq!(exact[0][0], src.to_string());
        assert_eq!(
            zero[0][0],
            src.to_string(),
            "alpha=0 still finds its own row"
        );
        assert_eq!(scaled[0][0], src.to_string(), "alpha=1.2 still finds it");
        if zero != scaled {
            differing += 1;
        }
        r0 += recall(&zero, &exact);
        r1 += recall(&scaled, &exact);
        queries += 1;
    }
    let (r0, r1) = (r0 / queries as f32, r1 / queries as f32);
    println!(
        "lsg: n={n} dim={dim} recall@10 alpha=0 {r0:.3}, alpha=1.2 {r1:.3}, \
         queries where alpha changed the answer: {differing}/{queries}"
    );
    assert!(r0 >= 0.9, "alpha=0 recall@10 {r0:.3} < 0.9");
    assert!(r1 >= 0.9, "alpha=1.2 recall@10 {r1:.3} < 0.9");
    assert!(
        differing > 0,
        "the scaling exponent never changed a result, so it is not reaching the distance"
    );
}

/// A large sealed index loads without copying its vectors.
///
/// Structural, not statistical: the plain store must borrow the mapping
/// (`vectors_are_mapped`), its heap size must be zero, and the segment's
/// accounted bytes must be the graph's. RSS is printed for the reader but not
/// asserted, because mapping a page in is the OS's decision, not the engine's.
#[test]
#[ignore = "large-scale: run with --ignored"]
fn a_large_index_loads_without_copying_its_vectors() {
    let (n, dim) = (100_000usize, 128usize);
    let dir = tempfile::tempdir().unwrap();
    {
        let e = Engine::open(dir.path()).unwrap();
        e.execute(&format!(
            "CREATE TABLE big (id INT, v EMBEDDING(FLOAT,{dim}));"
        ))
        .unwrap();
        let mut rng = Rng(0x1234_5678_9ABC);
        for lo in (0..n).step_by(2000) {
            let hi = (lo + 2000).min(n);
            let body: Vec<String> = (lo..hi)
                .map(|i| format!("({i}, {})", lit(&rng.vector(dim))))
                .collect();
            e.execute(&format!("INSERT INTO big VALUES {};", body.join(",")))
                .unwrap();
        }
        e.execute("CREATE INDEX big_i ON big (v) USING HNSW WITH (metric='l2', M=16);")
            .unwrap();
        e.flush().unwrap();
    }
    let e = Engine::open(dir.path()).unwrap();
    let before = rss_kb();
    let loaded = e.load_vector_index("big", "big_i").unwrap();
    let after = rss_kb();
    let index = loaded.index.index();
    let payload = n * dim * 4;
    println!(
        "memory: n={n} dim={dim} payload={} MiB, mapped={}, vector heap={} B, \
         segment accounted={} MiB, rss delta={} MiB",
        payload / (1 << 20),
        index.vectors_are_mapped(),
        index.store_mem_bytes(),
        loaded.bytes / (1 << 20),
        (after.saturating_sub(before)) / 1024,
    );
    assert_eq!(index.len(), n);
    assert!(
        index.vectors_are_mapped(),
        "the payload must be borrowed from the mapping"
    );
    assert_eq!(index.store_mem_bytes(), 0, "no vector heap");
    // The graph is what is left: 24 + 4*2M bytes per vertex plus the label and
    // level bookkeeping, ~16 MiB here. A copied payload alone would be 48 MiB.
    assert!(
        loaded.bytes < 40 * (1 << 20),
        "segment accounts {} MiB, so the payload was copied",
        loaded.bytes / (1 << 20)
    );
    // And it still answers.
    let query = loaded.index.search_index(
        &(0..dim).map(|j| j as f32 * 0.01).collect::<Vec<f32>>(),
        5,
        index::hnsw_handler::KnnSearchOption { ef: None },
    );
    assert_eq!(query.len(), 5);
}

/// Searches stay correct while rows are being inserted.
///
/// The memory segment grows under the searches' feet, so this exercises the
/// `rows`/graph pairing: no ghost row may be returned, every inserted row must
/// be found once the writers finish, and no query may panic.
#[test]
#[ignore = "large-scale: run with --ignored"]
fn concurrent_inserts_keep_searches_correct() {
    let (base, extra, dim) = (2000usize, 1000usize, 32usize);
    let dir = tempfile::tempdir().unwrap();
    let e = Arc::new(Engine::open(dir.path()).unwrap());
    e.execute(&format!(
        "CREATE TABLE cc (id INT, v EMBEDDING(FLOAT,{dim}));"
    ))
    .unwrap();
    let mut rng = Rng(0xC0FF_EE00);
    let data: Vec<Vec<f32>> = (0..base + extra).map(|_| rng.vector(dim)).collect();
    let body: Vec<String> = (0..base)
        .map(|i| format!("({i}, {})", lit(&data[i])))
        .collect();
    e.execute(&format!("INSERT INTO cc VALUES {};", body.join(",")))
        .unwrap();
    e.execute("CREATE INDEX cc_i ON cc (v) USING HNSW WITH (metric='l2', M=16);")
        .unwrap();
    e.flush().unwrap();

    let observed: Arc<std::sync::Mutex<Vec<usize>>> = Arc::new(std::sync::Mutex::new(Vec::new()));
    let writers: Vec<_> = (0..4)
        .map(|w| {
            let e = Arc::clone(&e);
            let rows: Vec<String> = (0..250)
                .map(|k| {
                    let i = base + w * 250 + k;
                    format!("({i}, {})", lit(&data[i]))
                })
                .collect();
            std::thread::spawn(move || {
                for chunk in rows.chunks(50) {
                    e.execute(&format!("INSERT INTO cc VALUES {};", chunk.join(",")))
                        .unwrap();
                }
            })
        })
        .collect();
    let readers: Vec<_> = (0..4)
        .map(|r| {
            let e = Arc::clone(&e);
            let observed = Arc::clone(&observed);
            let probe = lit(&data[r * 7 + 1]);
            std::thread::spawn(move || {
                for _ in 0..25 {
                    let sql = format!(
                        "SELECT id FROM cc SEARCH MATCH VECTOR (v, {probe}, 'float', 'l2', 5);"
                    );
                    let seen: Vec<usize> = rows(&e.execute(&sql).unwrap())
                        .into_iter()
                        .map(|row| row[0].parse().expect("id column"))
                        .collect();
                    assert!(seen.len() <= 5, "topn is a ceiling, not a target");
                    observed.lock().unwrap().extend(seen);
                }
            })
        })
        .collect();
    for w in writers {
        w.join().unwrap();
    }
    for r in readers {
        r.join().unwrap();
    }
    e.flush().unwrap();

    // No ghost row: every id a concurrent reader saw exists.
    for id in observed.lock().unwrap().iter() {
        assert!(*id < base + extra, "ghost row {id}");
    }
    // Every row is stored.
    let all = texts(&e, "SELECT id FROM cc;");
    assert_eq!(all.len(), base + extra);
    // A row inserted while the searches ran is found by its own vector.
    for probe in [base, base + 500, base + 999] {
        let got = texts(
            &e,
            &format!(
                "SELECT id FROM cc SEARCH MATCH VECTOR (v, {}, 'float', 'l2', 20);",
                lit(&data[probe])
            ),
        );
        assert!(
            got.iter().any(|row| row[0] == probe.to_string()),
            "row {probe} is missing from its own query: {got:?}"
        );
    }
    println!("concurrency: 4 readers x 25 queries against 4 writers inserting {extra} rows: ok");
}

/// The exhaustive scan must rank a *multi-token* query too: a `MULTIVECTOR`
/// column's own score is MaxSim over the query tokens, and the index path is
/// only meaningful if the scan it is compared against answers at all.
#[test]
fn the_exhaustive_scan_answers_a_multi_token_query() {
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE m2 (id INT, v MULTIVECTOR(FLOAT,4));")
        .unwrap();
    e.execute(
        "INSERT INTO m2 VALUES (1, [[1.0,0.0,0.0,0.0],[0.0,1.0,0.0,0.0]]), \
         (2, [[0.0,0.0,1.0,0.0],[0.0,0.0,0.0,1.0]]);",
    )
    .unwrap();
    let got = texts(
        &e,
        "SELECT id FROM m2 SEARCH MATCH VECTOR \
         (v, [1.0,0.0,0.0,0.0,0.0,0.0,1.0,0.0], 'float', 'l2', 2);",
    );
    assert_eq!(got.len(), 2, "{got:?}");
    assert_eq!(got[0], vec!["1".to_string()]);

    // The same shape the scale test uses: four tokens of 32 dimensions.
    let mut rng = Rng(7);
    let tokens: Vec<Vec<f32>> = (0..4).map(|_| rng.vector(32)).collect();
    e.execute("CREATE TABLE w4 (id INT, v MULTIVECTOR(FLOAT,32));")
        .unwrap();
    let cell: String = tokens.iter().map(|t| lit(t)).collect::<Vec<_>>().join(",");
    let other: String = [rng.vector(32), rng.vector(32)]
        .iter()
        .map(|t| lit(t))
        .collect::<Vec<_>>()
        .join(",");
    e.execute(&format!(
        "INSERT INTO w4 VALUES (0, [{cell}]), (1, [{other}]);"
    ))
    .unwrap();
    let got = texts(
        &e,
        &format!(
            "SELECT id FROM w4 SEARCH MATCH VECTOR (v, {}, 'float', 'l2', 2);",
            flat_lit(&tokens)
        ),
    );
    assert_eq!(got.len(), 2, "{got:?}");
    assert_eq!(got[0], vec!["0".to_string()]);
}
