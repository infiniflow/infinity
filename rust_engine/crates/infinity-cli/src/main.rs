//! `infinity` CLI — a thin driver over the storage kernel, used for smoke tests
//! and manual verification of the rewrite.
//!
//! Usage:
//!   infinity create-table <dir> <name> <columns>
//!   infinity list-tables <dir>
//!   infinity put <dir> <key> <value>
//!   infinity get <dir> <key>
//!   infinity recover <dir>
//!   infinity smoke <dir>          # create table + commit kv + reopen + verify

use std::path::{Path, PathBuf};
use std::sync::Arc;
use std::time::Instant;

use infinity_catalog::Catalog;
use infinity_executor::{Engine, QueryResult};
use infinity_storage::{FjallMetaStore, MetaStore};
use infinity_txn::TxnManager;

fn catalog_dir(dir: &Path) -> PathBuf {
    dir.join("catalog")
}
fn kv_dir(dir: &Path) -> PathBuf {
    dir.join("persistence")
}
fn wal_path(dir: &Path) -> PathBuf {
    dir.join("wal").join("wal.log")
}

fn open_catalog(dir: &Path) -> infinity_common::Result<Catalog> {
    Catalog::open(catalog_dir(dir))
}

fn open_txn(dir: &Path) -> infinity_common::Result<(TxnManager, Arc<dyn MetaStore>)> {
    let store: Arc<dyn MetaStore> = Arc::new(FjallMetaStore::open(kv_dir(dir), "default")?);
    std::fs::create_dir_all(dir.join("wal")).map_err(|e| infinity_common::Error::Io {
        path: dir.join("wal"),
        source: e,
    })?;
    let m = TxnManager::open(wal_path(dir), store.clone())?;
    Ok((m, store))
}

fn usage() -> ! {
    eprintln!(
        "usage: infinity <create-table|list-tables|put|get|recover|query|bench|serve|import-csv|conformance|smoke> <dir> [args...]"
    );
    std::process::exit(2);
}

/// Reads `key = "value"` out of a TOML text without pulling in a parser.
fn toml_value(text: &str, key: &str) -> Option<String> {
    text.lines()
        .map(str::trim)
        .find(|l| l.starts_with(key) && l[key.len()..].trim_start().starts_with('='))
        .and_then(|l| l.split_once('='))
        .map(|(_, v)| v.trim().trim_matches('"').to_string())
}

/// `infinity --config=<file>` — the C++ server's invocation, used by
/// `python/restart_test`. The data root is the parent of `persistence_dir`.
fn serve_from_config(path: &str) -> infinity_common::Result<()> {
    let text = std::fs::read_to_string(path).map_err(|e| infinity_common::Error::Io {
        path: PathBuf::from(path),
        source: e,
    })?;
    let persistence =
        toml_value(&text, "persistence_dir").unwrap_or_else(|| "/var/infinity/persistence".into());
    let root = Path::new(&persistence)
        .parent()
        .map(Path::to_path_buf)
        .unwrap_or_else(|| PathBuf::from("/var/infinity"));
    for sub in ["catalog", "persistence", "wal", "log", "tmp"] {
        std::fs::create_dir_all(root.join(sub)).map_err(|e| infinity_common::Error::Io {
            path: root.join(sub),
            source: e,
        })?;
    }
    let addr = std::env::var("INFINITY_BIND").unwrap_or_else(|_| "0.0.0.0:23817".into());
    // `storage_type = "minio"`: the C++-format data files live in the bucket and
    // are downloaded before recovery, then kept in sync at every checkpoint.
    let storage_type = toml_value(&text, "storage_type").unwrap_or_default();
    let object_store = if storage_type.eq_ignore_ascii_case("minio") {
        let endpoint = toml_value(&text, "url").unwrap_or_default();
        let bucket = toml_value(&text, "bucket_name").unwrap_or_default();
        let https = toml_value(&text, "enable_https").unwrap_or_default();
        if https.eq_ignore_ascii_case("true") {
            eprintln!("object store: enable_https is not supported; keeping data local");
            None
        } else if endpoint.is_empty() || bucket.is_empty() {
            eprintln!("object store: url/bucket_name missing; keeping data local");
            None
        } else {
            let client = infinity_storage::s3::S3Client::new(infinity_storage::s3::S3Config {
                endpoint,
                bucket,
                access_key: toml_value(&text, "access_key").unwrap_or_default(),
                secret_key: toml_value(&text, "secret_key").unwrap_or_default(),
            });
            match client.ensure_bucket() {
                Ok(()) => Some(Arc::new(client)),
                Err(e) => {
                    eprintln!("object store: {e}; keeping data local");
                    None
                }
            }
        }
    } else {
        None
    };
    if let Some(client) = &object_store {
        match infinity_storage::s3::restore_tree(client, &root.join("data"), "data") {
            Ok(n) => println!("object store: restored {n} file(s) from s3"),
            Err(e) => eprintln!("object store: restore failed: {e}"),
        }
    }
    let engine = Arc::new(Engine::open(&root)?);
    if let Some(client) = object_store {
        engine.set_object_store(client);
    }
    // `[buffer] memindex_memory_quota` — a memory index is dumped into a C++
    // chunk once it reaches this size.
    engine.set_memindex_quota(parse_size_bytes(
        &toml_value(&text, "memindex_memory_quota").unwrap_or_default(),
    ));
    // The reference checkpoints periodically (`[wal] checkpoint_interval`), which
    // is what dumps the memory blocks into the on-disk C++ layout.
    let secs = parse_interval_secs(&toml_value(&text, "checkpoint_interval").unwrap_or_default());
    if secs > 0 {
        let weak = Arc::downgrade(&engine);
        std::thread::spawn(move || loop {
            std::thread::sleep(std::time::Duration::from_secs(secs));
            match weak.upgrade() {
                Some(e) => {
                    // Cheap first: bound the WAL even if the full checkpoint
                    // (fsyncing a large LSM) takes a while.
                    e.checkpoint_wal();
                    let _ = e.flush();
                }
                None => break,
            }
        });
    }
    // `[storage] optimize_interval` — the background optimizer appends a merged
    // index chunk once the memory dumps have left more than one behind.
    let opt = parse_interval_secs(&toml_value(&text, "optimize_interval").unwrap_or_default());
    if opt > 0 {
        let weak = Arc::downgrade(&engine);
        std::thread::spawn(move || loop {
            std::thread::sleep(std::time::Duration::from_secs(opt));
            match weak.upgrade() {
                Some(e) => {
                    let _ = e.optimize_indexes();
                }
                None => break,
            }
        });
    }
    let server = infinity_server::Server::bind_with_engine(&addr, engine)?;
    println!("serving on {}", server.local_addr()?);
    server.run();
}

/// Parses a duration like `1s`, `500ms`, `1.5s` or `0s` into whole seconds.
fn parse_interval_secs(text: &str) -> u64 {
    let t = text.trim();
    let (num, scale) = if let Some(v) = t.strip_suffix("ms") {
        (v, 0.001)
    } else if let Some(v) = t.strip_suffix('s') {
        (v, 1.0)
    } else if let Some(v) = t.strip_suffix('m') {
        (v, 60.0)
    } else {
        (t, 1.0)
    };
    let value: f64 = num.trim().parse().unwrap_or(0.0);
    (value * scale).round() as u64
}

/// Parses a size like `1KB`, `512MB`, `8GB` or a bare byte count.
fn parse_size_bytes(text: &str) -> u64 {
    let t = text.trim().to_ascii_uppercase();
    let (num, scale) = if let Some(v) = t.strip_suffix("KB") {
        (v, 1024.0)
    } else if let Some(v) = t.strip_suffix("MB") {
        (v, 1024.0 * 1024.0)
    } else if let Some(v) = t.strip_suffix("GB") {
        (v, 1024.0 * 1024.0 * 1024.0)
    } else if let Some(v) = t.strip_suffix('B') {
        (v, 1.0)
    } else {
        (t.as_str(), 1.0)
    };
    let value: f64 = num.trim().parse().unwrap_or(0.0);
    (value * scale) as u64
}

fn main() {
    let args: Vec<String> = std::env::args().skip(1).collect();
    if let Some(cfg) = args.iter().find_map(|a| a.strip_prefix("--config=")) {
        if let Err(e) = serve_from_config(cfg) {
            eprintln!("error: {e}");
            std::process::exit(1);
        }
        return;
    }
    if args.len() < 2 {
        usage();
    }
    if let Err(e) = run(&args) {
        eprintln!("error: {e}");
        std::process::exit(1);
    }
}

fn run(args: &[String]) -> infinity_common::Result<()> {
    let cmd = args[0].as_str();
    let dir = PathBuf::from(&args[1]);
    match cmd {
        "create-table" => {
            let name = args.get(2).cloned().unwrap_or_else(|| "t".into());
            let cols_csv = args.get(3).cloned().unwrap_or_else(|| "c0".into());
            let columns: Vec<String> = cols_csv
                .split(',')
                .map(|s| s.trim().to_string())
                .filter(|s| !s.is_empty())
                .collect();
            let c = open_catalog(&dir)?;
            let t = c.create_table(&name, &columns)?;
            c.flush()?;
            println!(
                "created table id={} name={} columns={:?}",
                t.id, t.name, t.columns
            );
            Ok(())
        }
        "list-tables" => {
            let c = open_catalog(&dir)?;
            for t in c.list_tables()? {
                println!("{} {} {:?}", t.id, t.name, t.columns);
            }
            Ok(())
        }
        "put" => {
            let k = args.get(2).cloned().unwrap_or_else(|| "k".into());
            let v = args.get(3).cloned().unwrap_or_else(|| "v".into());
            let (m, _) = open_txn(&dir)?;
            let mut t = m.begin();
            t.put(k.clone().into_bytes(), v.clone().into_bytes());
            let ts = m.commit(t)?;
            m.flush()?;
            println!("committed {k}={v} at ts={ts}");
            Ok(())
        }
        "get" => {
            let k = args.get(2).cloned().unwrap_or_else(|| "k".into());
            let (_, store) = open_txn(&dir)?;
            match store.get(k.as_bytes())? {
                Some(v) => println!("{k}={}", String::from_utf8_lossy(&v)),
                None => println!("{k}=(none)"),
            }
            Ok(())
        }
        "recover" => {
            let (m, store) = open_txn(&dir)?;
            let n = m.recover(0)?;
            println!("replayed {n} WAL record(s)");
            for (k, v) in store.scan_prefix(b"")? {
                println!(
                    "{}={}",
                    String::from_utf8_lossy(&k),
                    String::from_utf8_lossy(&v)
                );
            }
            Ok(())
        }
        "query" => {
            let sql = args[2..].join(" ");
            let e = Engine::open(&dir)?;
            for r in e.execute(&sql)? {
                print_result(&r);
            }
            e.flush()?;
            Ok(())
        }
        "serve" => {
            // `infinity serve <dir> [host:port]` — Thrift/Thrift-over-TCP frontend.
            let addr = args
                .get(2)
                .cloned()
                .unwrap_or_else(|| "0.0.0.0:23817".into());
            let engine = std::sync::Arc::new(infinity_executor::Engine::open(&dir)?);
            let server = infinity_server::Server::bind_with_engine(&addr, engine)?;
            println!("serving on {}", server.local_addr()?);
            server.run();
        }
        "bench" => {
            let n: usize = args.get(2).and_then(|s| s.parse().ok()).unwrap_or(2000);
            let e = Engine::open(&dir)?;
            let _ = e.execute("CREATE TABLE t (id INT, name VARCHAR);");
            let t0 = Instant::now();
            let mut i = 0;
            while i < n {
                let batch = (n - i).min(100); // batched durable commits
                let vals: Vec<String> = (0..batch)
                    .map(|j| {
                        let id = i + j;
                        format!("({id}, 'v{id}')")
                    })
                    .collect();
                e.execute(&format!("INSERT INTO t VALUES {};", vals.join(", ")))?;
                i += batch;
            }
            let ins = t0.elapsed();
            let t1 = Instant::now();
            let r = e.execute("SELECT * FROM t;")?;
            let sel = t1.elapsed();
            e.flush()?;
            let rows = match &r[0] {
                QueryResult::Rows { rows, .. } => rows.len(),
                _ => 0,
            };
            println!(
                "insert: {n} rows in {:?} ({:.0} rows/s)",
                ins,
                n as f64 / ins.as_secs_f64()
            );
            println!("select: {rows} rows in {:?}", sel);
            // Machine-readable lines consumed by
            // `tools/ci_tools/check_benchmark_result.py` (Gate E).
            println!("Total cost : {:.6} s", ins.as_secs_f64());
            println!("Total cost : {:.6} s", sel.as_secs_f64());
            Ok(())
        }
        "import-csv" => {
            let table = args.get(2).cloned().unwrap_or_else(|| "t".into());
            let file = args.get(3).cloned().unwrap_or_default();
            if file.is_empty() {
                return Err(infinity_common::Error::Invalid(
                    "import-csv needs a file path".into(),
                ));
            }
            let text =
                std::fs::read_to_string(&file).map_err(|e| infinity_common::Error::io(&file, e))?;
            let mut lines = text.lines().filter(|l| !l.trim().is_empty());
            let header: Vec<String> = lines
                .next()
                .ok_or_else(|| infinity_common::Error::Invalid("empty csv".into()))?
                .split(',')
                .map(|s| s.trim().to_string())
                .collect();
            let e = Engine::open(&dir)?;
            // create the table if it does not exist (all columns VARCHAR)
            let cols: Vec<String> = header.iter().map(|c| format!("{c} VARCHAR")).collect();
            let _ = e.execute(&format!("CREATE TABLE {table} ({});", cols.join(", ")));

            let mut total = 0usize;
            let mut batch: Vec<String> = Vec::new();
            let flush = |e: &Engine, batch: &mut Vec<String>| -> infinity_common::Result<usize> {
                if batch.is_empty() {
                    return Ok(0);
                }
                let sql = format!("INSERT INTO {table} VALUES {};", batch.join(", "));
                let n = batch.len();
                e.execute(&sql)?;
                batch.clear();
                Ok(n)
            };
            for line in lines {
                let vals: Vec<String> = line
                    .split(',')
                    .map(|v| format!("'{}'", v.trim().replace('\'', "''")))
                    .collect();
                batch.push(format!("({})", vals.join(", ")));
                if batch.len() >= 100 {
                    total += flush(&e, &mut batch)?;
                }
            }
            total += flush(&e, &mut batch)?;
            e.flush()?;
            println!("imported {total} row(s) into {table} from {file}");
            Ok(())
        }
        "conformance" => {
            let slt_dir = dir.to_string_lossy().to_string();
            run_conformance(&slt_dir)
        }
        "smoke" => run_smoke(&dir),
        _ => usage(),
    }
}

fn print_result(r: &QueryResult) {
    match r {
        QueryResult::CreatedTable { id, name } => println!("CREATE TABLE {name} (id={id})"),
        QueryResult::CreatedDatabase { name } => println!("CREATE DATABASE {name}"),
        QueryResult::DroppedDatabase { name } => println!("DROP DATABASE {name}"),
        QueryResult::CreatedIndex {
            name,
            table,
            column,
        } => {
            println!("CREATE INDEX {name} ON {table}({column})")
        }
        QueryResult::DroppedTable { name, existed } => {
            println!("DROP TABLE {name} (existed={existed})")
        }
        QueryResult::DroppedIndex { name, existed } => {
            println!("DROP INDEX {name} (existed={existed})")
        }
        QueryResult::Inserted { count } => println!("INSERT {count} row(s)"),
        QueryResult::Deleted { count } => println!("DELETE {count} row(s)"),
        QueryResult::Updated { count } => println!("UPDATE {count} row(s)"),
        QueryResult::Rows { columns, rows } => {
            if !columns.is_empty() {
                println!("{}", columns.join(" | "));
            }
            for row in rows {
                println!("{}", row.join(" | "));
            }
            println!("({} row(s))", rows.len());
        }
    }
}

fn run_smoke(dir: &Path) -> infinity_common::Result<()> {
    // 1. create tables
    {
        let c = open_catalog(dir)?;
        c.create_table("my_table", &["id".to_string(), "name".to_string()])?;
        c.flush()?;
    }
    // 2. commit some kv through the txn pipeline
    {
        let (m, _) = open_txn(dir)?;
        for i in 0..3u64 {
            let mut t = m.begin();
            t.put(
                format!("row{i}").into_bytes(),
                format!("val{i}").into_bytes(),
            );
            m.commit(t)?;
        }
        m.flush()?;
    }
    // 3. "restart": reopen catalog and replay WAL into a *fresh* kv to prove recovery
    let c = open_catalog(dir)?;
    let tables = c.list_tables()?;
    println!("[smoke] tables after restart: {tables:?}");

    let fresh: Arc<dyn MetaStore> =
        Arc::new(FjallMetaStore::open(dir.join("kv_recovered"), "default")?);
    let m = TxnManager::open(wal_path(dir), fresh.clone())?;
    let n = m.recover(0)?;
    println!("[smoke] replayed {n} WAL record(s) into fresh store");
    let rows = fresh.scan_prefix(b"row")?;
    for (k, v) in &rows {
        println!(
            "[smoke]   {}={}",
            String::from_utf8_lossy(k),
            String::from_utf8_lossy(v)
        );
    }
    assert_eq!(tables.len(), 1, "expected 1 table");
    assert_eq!(rows.len(), 3, "expected 3 recovered rows");
    println!("[smoke] OK");
    Ok(())
}

fn walk_slt(dir: &Path, out: &mut Vec<PathBuf>) {
    // A single `.slt` file is accepted directly, which makes it easy to focus
    // the conformance report on one file while iterating.
    if dir.is_file() {
        if dir.extension().and_then(|s| s.to_str()) == Some("slt") {
            out.push(dir.to_path_buf());
        }
        return;
    }
    if let Ok(entries) = std::fs::read_dir(dir) {
        let mut es: Vec<_> = entries.filter_map(|e| e.ok()).collect();
        es.sort_by_key(|e| e.path());
        for e in es {
            let p = e.path();
            if p.is_dir() {
                walk_slt(&p, out);
            } else if p.extension().and_then(|s| s.to_str()) == Some("slt") {
                out.push(p);
            }
        }
    }
}

fn split_records(text: &str) -> Vec<Vec<String>> {
    let mut records = Vec::new();
    let mut cur: Vec<String> = Vec::new();
    for line in text.lines() {
        // `#`-comments are ignored wherever they appear.
        if line.trim_start().starts_with('#') {
            continue;
        }
        if line.trim().is_empty() {
            if !cur.is_empty() {
                records.push(std::mem::take(&mut cur));
            }
        } else {
            cur.push(line.to_string());
        }
    }
    if !cur.is_empty() {
        records.push(cur);
    }
    records
}

/// Runs the existing `.slt` suite against the Rust engine and prints a baseline.
fn run_conformance(slt_dir: &str) -> infinity_common::Result<()> {
    // Fixture data (`/var/infinity/test_data/…`) resolves under `test/data`.
    // Walk parent directories so a single-file run still finds the fixtures.
    let mut probe = Path::new(slt_dir).parent();
    while let Some(p) = probe {
        let data = p.join("data");
        if data.is_dir() {
            std::env::set_var("INFINITY_DATA_DIR", &data);
            break;
        }
        probe = p.parent();
    }
    let mut files = Vec::new();
    walk_slt(Path::new(slt_dir), &mut files);
    let (mut stmt_ok, mut stmt_ok_pass) = (0usize, 0usize);
    let (mut stmt_err, mut stmt_err_pass) = (0usize, 0usize);
    let (mut queries, mut query_pass) = (0usize, 0usize);
    let mut fail_reasons: std::collections::HashMap<String, Vec<String>> =
        std::collections::HashMap::new();
    let mut stmt_fail: std::collections::HashMap<String, Vec<String>> =
        std::collections::HashMap::new();
    let mut file_fail: std::collections::HashMap<String, usize> = std::collections::HashMap::new();
    let mut stmt_err_missing: std::collections::HashMap<String, Vec<String>> =
        std::collections::HashMap::new();
    for (idx, f) in files.iter().enumerate() {
        let d = std::env::temp_dir().join(format!("infconf-{}-{idx}", std::process::id()));
        let _ = std::fs::remove_dir_all(&d);
        let e = match Engine::open(&d) {
            Ok(e) => e,
            Err(_) => continue,
        };
        let fname = f
            .strip_prefix(slt_dir)
            .unwrap_or(f)
            .to_string_lossy()
            .trim_start_matches('/')
            .to_string();
        let mut this_file_fails = 0usize;
        let text = std::fs::read_to_string(f).unwrap_or_default();
        for rec in split_records(&text) {
            let head = rec[0].trim();
            if head.starts_with('#') {
                continue;
            }
            let sql: String = rec
                .iter()
                .take_while(|l| l.trim() != "----")
                .skip(1)
                .cloned()
                .collect::<Vec<_>>()
                .join("\n");
            let expected: Vec<String> = rec
                .iter()
                .skip_while(|l| l.trim() != "----")
                .skip(1)
                // Only trailing whitespace is insignificant; a leading space can
                // be part of a VARCHAR value, so it is preserved.
                .map(|s| s.trim_end().to_string())
                .collect();
            if let Some(kind) = head.strip_prefix("statement ") {
                let kind = kind.split_whitespace().next().unwrap_or("");
                let res = e.execute(&sql);
                let ok = res.is_ok();
                match kind {
                    "ok" => {
                        stmt_ok += 1;
                        if ok {
                            stmt_ok_pass += 1;
                        } else {
                            this_file_fails += 1;
                            stmt_fail
                                .entry(format!("{:?} ({})", res.unwrap_err(), first_word(&sql)))
                                .or_default()
                                .push(sql.clone());
                        }
                    }
                    "error" => {
                        stmt_err += 1;
                        if !ok {
                            stmt_err_pass += 1;
                        } else {
                            // the engine accepted a statement the suite expects to fail
                            stmt_err_missing
                                .entry(first_word(&sql))
                                .or_default()
                                .push(sql.clone());
                        }
                    }
                    _ => {}
                }
            } else if head.starts_with("query ") {
                queries += 1;
                // `query <types> [<sort-mode>] [<label>]` — default `nosort`.
                let rowsort = head.split_whitespace().nth(2) == Some("rowsort");
                match e.execute(&sql) {
                    Ok(rs) => {
                        let mut rows: Vec<String> = Vec::new();
                        if let Some(QueryResult::Rows { rows: r, .. }) = rs.into_iter().next() {
                            for row in r {
                                // normalize each column, then join — matches
                                // `default_validator` in sqllogictest-rs.
                                let cols: Vec<String> = row.iter().map(|c| norm(c)).collect();
                                rows.push(cols.join(" "));
                            }
                        }
                        let mut got = rows;
                        // sqllogictest writes `(empty)` for an empty value.
                        let mut exp: Vec<String> = expected
                            .iter()
                            .map(|s| {
                                if s == "(empty)" {
                                    String::new()
                                } else {
                                    norm(s)
                                }
                            })
                            .collect();
                        if rowsort {
                            got.sort();
                            exp.sort();
                        }
                        if got == exp {
                            query_pass += 1;
                        } else {
                            this_file_fails += 1;
                            fail_reasons
                                .entry(format!("MISMATCH ({})", mismatch_key(&sql)))
                                .or_default()
                                .push(format!("{sql}  ==> got {got:?} want {exp:?}"));
                        }
                    }
                    Err(err) => {
                        this_file_fails += 1;
                        fail_reasons
                            .entry(format!("{err} ({})", first_word(&sql)))
                            .or_default()
                            .push(sql.clone());
                    }
                }
            }
        }
        let _ = std::fs::remove_dir_all(&d);
        if this_file_fails > 0 {
            file_fail.insert(fname, this_file_fails);
        }
    }
    println!("== Rust conformance baseline (SQL Logic Test) ==");
    println!("files          : {}", files.len());
    println!("statement ok   : {stmt_ok_pass}/{stmt_ok}");
    println!("statement error: {stmt_err_pass}/{stmt_err}");
    println!("query          : {query_pass}/{queries}");
    let mut ranked: Vec<_> = fail_reasons.iter().collect();
    ranked.sort_by(|a, b| b.1.len().cmp(&a.1.len()));
    println!("-- top statement-ok failure reasons --");
    let mut sranked: Vec<_> = stmt_fail.iter().collect();
    sranked.sort_by(|a, b| b.1.len().cmp(&a.1.len()));
    for (reason, samples) in sranked.iter().take(15) {
        println!("{:>5}  {}", samples.len(), reason);
        for s in samples.iter().take(2) {
            println!("        {}", s.replace('\n', " "));
        }
    }
    println!("-- statements that should error but did not --");
    let mut eranked: Vec<_> = stmt_err_missing.iter().collect();
    eranked.sort_by(|a, b| b.1.len().cmp(&a.1.len()));
    for (word, samples) in eranked.iter().take(15) {
        println!("{:>5}  {}", samples.len(), word);
        for s in samples.iter().take(6) {
            println!("        {}", s.replace('\n', " "));
        }
    }
    println!("-- top query failure reasons --");
    println!("-- top files by failed records --");
    let mut franked: Vec<_> = file_fail.iter().collect();
    franked.sort_by(|a, b| b.1.cmp(a.1));
    for (file, n) in franked.iter().take(200) {
        println!("{n:>5}  {file}");
    }
    for (reason, samples) in ranked.iter().take(25) {
        println!("{:>5}  {}", samples.len(), reason);
        for s in samples.iter().take(3) {
            println!("        {}", s.replace('\n', " "));
        }
    }
    Ok(())
}

/// A failure key: the leading keywords plus (when present) the FROM table.
fn mismatch_key(sql: &str) -> String {
    let head = first_word(sql);
    let up = sql.to_uppercase();
    let table = up
        .find(" FROM ")
        .map(|i| sql[i + 6..].trim_start())
        .and_then(|rest| rest.split_whitespace().next())
        .unwrap_or("");
    if table.is_empty() {
        head
    } else {
        format!("{head} {}:{}", table, "")
            .trim_end_matches(':')
            .to_string()
    }
}

fn first_word(sql: &str) -> String {
    sql.split_whitespace()
        .take(3)
        .collect::<Vec<_>>()
        .join(" ")
        .to_uppercase()
}

/// The `sqllogictest-rs` `default_normalizer`: trim and collapse runs of ASCII
/// whitespace to a single space. This makes the comparison insensitive to the
/// indentation of multi-line results such as `EXPLAIN` output.
fn norm(s: &str) -> String {
    s.split_ascii_whitespace().collect::<Vec<_>>().join(" ")
}
