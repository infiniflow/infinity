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
    dir.join("kv")
}
fn wal_path(dir: &Path) -> PathBuf {
    dir.join("wal.log")
}

fn open_catalog(dir: &Path) -> infinity_common::Result<Catalog> {
    Catalog::open(catalog_dir(dir))
}

fn open_txn(dir: &Path) -> infinity_common::Result<(TxnManager, Arc<dyn MetaStore>)> {
    let store: Arc<dyn MetaStore> = Arc::new(FjallMetaStore::open(kv_dir(dir), "default")?);
    let m = TxnManager::open(wal_path(dir), store.clone())?;
    Ok((m, store))
}

fn usage() -> ! {
    eprintln!(
        "usage: infinity <create-table|list-tables|put|get|recover|query|bench|import-csv|conformance|smoke> <dir> [args...]"
    );
    std::process::exit(2);
}

fn main() {
    let args: Vec<String> = std::env::args().skip(1).collect();
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
    if let Some(root) = Path::new(slt_dir).parent() {
        let data = root.join("data");
        if data.is_dir() {
            std::env::set_var("INFINITY_DATA_DIR", data);
        }
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
                .map(|s| s.trim().to_string())
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
                        }
                    }
                    _ => {}
                }
            } else if head.starts_with("query ") {
                queries += 1;
                match e.execute(&sql) {
                    Ok(rs) => {
                        let mut rows: Vec<String> = Vec::new();
                        if let Some(QueryResult::Rows { rows: r, .. }) = rs.into_iter().next() {
                            for row in r {
                                rows.push(row.join(" "));
                            }
                        }
                        let mut got = rows;
                        got.sort();
                        let mut exp = expected;
                        exp.sort();
                        if got == exp {
                            query_pass += 1;
                        } else {
                            this_file_fails += 1;
                            fail_reasons
                                .entry(format!("MISMATCH ({})", first_word(&sql)))
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
    println!("-- top query failure reasons --");
    println!("-- top files by failed records --");
    let mut franked: Vec<_> = file_fail.iter().collect();
    franked.sort_by(|a, b| b.1.cmp(a.1));
    for (file, n) in franked.iter().take(15) {
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

fn first_word(sql: &str) -> String {
    sql.split_whitespace()
        .take(3)
        .collect::<Vec<_>>()
        .join(" ")
        .to_uppercase()
}
