//! Rust port of `benchmark/local_infinity/fulltext/fulltext_benchmark.cpp`.
//!
//! The reference drives `Infinity::LocalInit`/`LocalConnect` and times four
//! modes against the same `{"id","title","text"}` JSONL corpus:
//!
//! * `insert` — create the full-text index, then `Insert` in batches of 500;
//! * `import` — create the index, then one `IMPORT` of the whole file;
//! * `merge`  — insert, then `Optimize` (index chunk merge);
//! * `query`  — insert, optimize, then the reference's five `MATCH TEXT`
//!   queries (`topn=10`) projected to `ROW_ID(), SCORE()`.
//!
//! The port keeps the same phases and the same corpus so the wall-clock lines
//! are directly comparable; the engine calls go through the public
//! `Engine`/`insert_literals` API the Thrift endpoint uses, which is the
//! closest Rust analogue of the reference's in-process `Insert`.
//!
//! One deliberate difference: the reference times `CreateIndex` together with
//! its `Flush`, while the port times the DDL alone and reproduces the
//! checkpoint only under `FTBENCH_FLUSH=1` (see FULLTEXT_BENCH.md §4.11).

use std::path::Path;
use std::time::Instant;

use infinity_executor::{Engine, QueryResult};
use infinity_parser::Literal;

const TABLE: &str = "ft_dbpedia_benchmark";
/// `insert_literals`/`import_literals` take the catalog name (db-qualified),
/// while the SQL statements can use the bare one.
const QUALIFIED: &str = "default_db.ft_dbpedia_benchmark";
const INDEX: &str = "ft_dbpedia_index";

/// The five queries `BenchmarkQuery` runs (two of them phrase queries).
const QUERIES: [&str; 7] = [
    "harmful \"social custom\"",
    "social custom \"harmful chemical\"",
    "\"annual American awards\"",
    "harmful chemical",
    "\"one of\"",
    // Multi-term (10+ OR terms): every term is a WAND cursor in the same pass,
    // which is where the per-posting cursor machinery shows up.
    "harmful chemical social custom annual American awards one of the first world war",
    "national football league high school state university population family city game season team player",
];

/// Reads the JSONL corpus every value of which is a string.
pub fn read_jsonl_rows(path: &str) -> infinity_common::Result<Vec<[String; 3]>> {
    let text = std::fs::read_to_string(path).map_err(|e| infinity_common::Error::io(path, e))?;
    let mut rows = Vec::new();
    for line in text.lines() {
        if line.trim().is_empty() {
            continue;
        }
        let kv = parse_json_object_strings(line).ok_or_else(|| {
            infinity_common::Error::Invalid(format!(
                "bad jsonl line: {}",
                &line[..line.len().min(100)]
            ))
        })?;
        let field = |name: &str| {
            kv.iter()
                .find(|(k, _)| k == name)
                .map(|(_, v)| v.clone())
                .unwrap_or_default()
        };
        rows.push([field("id"), field("title"), field("text")]);
    }
    Ok(rows)
}

/// Parses `{"k": "v", ...}`; `None` for anything but a flat string map.
fn parse_json_object_strings(line: &str) -> Option<Vec<(String, String)>> {
    let s = line.as_bytes();
    let mut i = 0usize;
    let skip_ws = |i: &mut usize| {
        while *i < s.len() && s[*i].is_ascii_whitespace() {
            *i += 1;
        }
    };
    skip_ws(&mut i);
    if s.get(i) != Some(&b'{') {
        return None;
    }
    i += 1;
    let mut out = Vec::new();
    loop {
        skip_ws(&mut i);
        match *s.get(i)? {
            b'}' => return Some(out),
            b',' => {
                i += 1;
                continue;
            }
            b'"' => {}
            _ => return None,
        }
        let (key, next) = json_string_at(line, i)?;
        i = next;
        skip_ws(&mut i);
        if *s.get(i)? != b':' {
            return None;
        }
        i += 1;
        skip_ws(&mut i);
        if *s.get(i)? != b'"' {
            return None;
        }
        let (value, next) = json_string_at(line, i)?;
        i = next;
        out.push((key, value));
    }
}

/// Decodes the JSON string starting at byte `i` (the opening quote).
fn json_string_at(line: &str, start: usize) -> Option<(String, usize)> {
    let s = line.as_bytes();
    if s.get(start) != Some(&b'"') {
        return None;
    }
    let mut i = start + 1;
    let mut out = String::new();
    loop {
        match *s.get(i)? {
            b'"' => return Some((out, i + 1)),
            b'\\' => {
                i += 1;
                match *s.get(i)? {
                    b'"' => out.push('"'),
                    b'\\' => out.push('\\'),
                    b'/' => out.push('/'),
                    b'n' => out.push('\n'),
                    b'r' => out.push('\r'),
                    b't' => out.push('\t'),
                    b'b' => out.push('\u{8}'),
                    b'f' => out.push('\u{c}'),
                    b'u' => {
                        let hex = std::str::from_utf8(s.get(i + 1..i + 5)?).ok()?;
                        let code = u32::from_str_radix(hex, 16).ok()?;
                        out.push(char::from_u32(code)?);
                        i += 4;
                    }
                    _ => return None,
                }
                i += 1;
            }
            _ => {
                let ch = line[i..].chars().next()?;
                out.push(ch);
                i += ch.len_utf8();
            }
        }
    }
}

/// Runs the reference's five queries (`FTBENCH_QUERY_REPEAT` samples each and
/// the best is reported; `FTBENCH_EXTRA_QUERY` adds one).
fn run_queries(e: &Engine) -> infinity_common::Result<()> {
    let repeat: usize = std::env::var("FTBENCH_QUERY_REPEAT")
        .ok()
        .and_then(|v| v.parse().ok())
        .unwrap_or(1)
        .max(1);
    let extra = std::env::var("FTBENCH_EXTRA_QUERY").ok();
    // The reference's QueryBenchOptions: topn, and the block_max switch that
    // selects naive / sort / heap (its `--topn` / `--algo`).
    let topn = std::env::var("FTBENCH_TOPN").unwrap_or_else(|_| "10".into());
    let block_max = std::env::var("FTBENCH_BLOCK_MAX").ok();
    let options = match block_max.as_deref() {
        Some(algo) if !algo.is_empty() => format!("topn={topn};block_max={algo}"),
        _ => format!("topn={topn}"),
    };
    let queries: Vec<&str> = QUERIES.iter().copied().chain(extra.as_deref()).collect();
    for query in queries {
        let sql = format!(
            "SELECT ROW_ID(), SCORE() FROM {TABLE} SEARCH MATCH TEXT ('text', '{query}', '{options}');"
        );
        let mut best = std::time::Duration::MAX;
        let mut first = std::time::Duration::ZERO;
        let mut hits = 0;
        for i in 0..repeat {
            let t = Instant::now();
            let rs = e.execute(&sql)?;
            let elapsed = t.elapsed();
            if i == 0 {
                first = elapsed;
            }
            best = best.min(elapsed);
            hits = match rs.first() {
                Some(QueryResult::Rows { rows, .. }) => rows.len(),
                _ => 0,
            };
        }
        println!("Query text: {query}, time cost: {best:?} (first: {first:?}, rows: {hits})");
    }
    Ok(())
}

/// `infinity fulltext-bench <dir> <mode> <corpus.jsonl> [batch]`.
pub fn run(dir: &Path, mode: &str, corpus: &str, batch: usize) -> infinity_common::Result<()> {
    let e = Engine::open(dir)?;
    // `query-only` reuses a directory a previous run left behind: no DDL, no
    // index build. It exists so a profiler sees the query path alone.
    if mode == "query-only" {
        run_queries(&e)?;
        return Ok(());
    }
    let _ = e.execute(&format!("DROP TABLE IF EXISTS {TABLE};"));
    e.execute(&format!(
        "CREATE TABLE {TABLE} (id VARCHAR, title VARCHAR, text VARCHAR);"
    ))?;

    // C++ `BenchmarkCreateIndex` times `CreateIndex` + `Flush` together, but
    // the two `Flush`es checkpoint different storage stacks, so the checkpoint
    // is excluded from the comparison and only reproduced on demand
    // (`FTBENCH_FLUSH=1`), timed on its own. See FULLTEXT_BENCH.md §4.11.
    // `FTBENCH_ANALYZER` builds the index with another analyzer family
    // (`ngram-3`, `sparsegram-2-3`, …) so the same corpus exercises the
    // streaming term paths, not just `standard`.
    let analyzer = std::env::var("FTBENCH_ANALYZER").ok();
    let create_index_sql = match analyzer.as_deref() {
        Some(name) => format!(
            "CREATE INDEX {INDEX} ON {TABLE} (text) USING FULLTEXT WITH (analyzer='{name}');"
        ),
        None => format!("CREATE INDEX {INDEX} ON {TABLE} (text) USING FULLTEXT;"),
    };
    let t = Instant::now();
    e.execute(&create_index_sql)?;
    let create_index = t.elapsed();
    match checkpoint_cost(&e)? {
        Some(flush) => {
            println!("Create index cost: {create_index:?} (DDL; Flush {flush:?} not included)")
        }
        None => println!("Create index cost: {create_index:?} (DDL only, Flush excluded)"),
    }

    // C++ `ReadJsonl` slurps the whole file before the timed inserts.
    let t = Instant::now();
    let rows = read_jsonl_rows(corpus)?;
    let n = rows.len();
    println!("ReadJsonl {n} rows cost: {:?}", t.elapsed());
    if n == 0 {
        return Err(infinity_common::Error::Invalid(format!(
            "no rows in {corpus}"
        )));
    }

    let columns = ["id".to_string(), "title".to_string(), "text".to_string()];
    let to_literals = |rs: &[[String; 3]]| -> Vec<Vec<Literal>> {
        rs.iter()
            .map(|r| {
                vec![
                    Literal::Str(r[0].clone()),
                    Literal::Str(r[1].clone()),
                    Literal::Str(r[2].clone()),
                ]
            })
            .collect()
    };
    let insert_all = |e: &Engine| -> infinity_common::Result<()> {
        let t = Instant::now();
        for chunk in rows.chunks(batch.max(1)) {
            e.insert_literals(QUALIFIED, Some(&columns), to_literals(chunk))?;
        }
        println!("Insert data {n} rows cost: {:?}", t.elapsed());
        Ok(())
    };

    match mode {
        "insert" => insert_all(&e)?,
        "import" => {
            let t = Instant::now();
            e.import_literals(QUALIFIED, Some(&columns), to_literals(&rows))?;
            println!("Import data cost: {:?}", t.elapsed());
        }
        "merge" => {
            insert_all(&e)?;
            let t = Instant::now();
            e.optimize_indexes()?;
            println!("Merge index cost: {:?}", t.elapsed());
        }
        "query" => {
            insert_all(&e)?;
            let t = Instant::now();
            e.optimize_indexes()?;
            println!("Merge index cost: {:?}", t.elapsed());
            run_queries(&e)?;
        }
        other => {
            return Err(infinity_common::Error::Invalid(format!(
                "unknown fulltext-bench mode '{other}' (insert|import|merge|query)"
            )))
        }
    }
    let _ = checkpoint_cost(&e)?;
    Ok(())
}

/// Whether the benchmark runs the checkpoint (`Flush`) steps at all.
fn flush_enabled() -> bool {
    std::env::var("FTBENCH_FLUSH")
        .map(|v| v == "1" || v.eq_ignore_ascii_case("true"))
        .unwrap_or(false)
}

/// Runs `Engine::flush` (the reference's `Flush`) when `FTBENCH_FLUSH` asks for
/// it and returns what it cost.
fn checkpoint_cost(e: &Engine) -> infinity_common::Result<Option<std::time::Duration>> {
    if !flush_enabled() {
        return Ok(None);
    }
    let t = Instant::now();
    e.flush()?;
    Ok(Some(t.elapsed()))
}
