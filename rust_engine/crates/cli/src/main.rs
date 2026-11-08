//! `infinity` — the engine's single binary.
//!
//! The reference's surface is reproduced as-is: with no subcommand,
//! `infinity [-f <config>] [-m]` starts the Thrift, HTTP and PostgreSQL front
//! ends on one engine (C++ `src/bin/infinity_main.cpp`). Everything below is the rewrite's
//! extension — drivers used for byte-alignment and benchmarks — and is not part
//! of the reference CLI.
//!
//! Extensions (`infinity <command> <dir> [args...]`):
//!   serve            Thrift + HTTP front ends on <dir>
//!   query            run SQL against <dir>
//!   create-table, list-tables, put, get, recover, dump-catalog, smoke
//!   import-csv, bench, knn-bench, fulltext-bench, conformance

mod config;
mod fulltext_bench;

use std::path::{Path, PathBuf};
use std::sync::Arc;
use std::time::Instant;

use catalog::Catalog;
use executor::{Engine, QueryResult};
use storage::{MetaStore, RocksMetaStore};
use txn::TxnManager;

fn catalog_dir(dir: &Path) -> PathBuf {
    dir.join("catalog")
}
fn wal_path(dir: &Path) -> PathBuf {
    dir.join("wal").join("wal.log")
}

fn open_catalog(dir: &Path) -> common::Result<Catalog> {
    Catalog::open(catalog_dir(dir))
}

fn open_txn(dir: &Path) -> common::Result<(TxnManager, Arc<dyn MetaStore>)> {
    // One RocksDB, like the engine: the catalog and every other record share it.
    let store: Arc<dyn MetaStore> = Arc::new(RocksMetaStore::open(catalog_dir(dir))?);
    std::fs::create_dir_all(dir.join("wal")).map_err(|e| common::Error::Io {
        path: dir.join("wal"),
        source: e,
    })?;
    let m = TxnManager::open(wal_path(dir), store.clone())?;
    Ok((m, store))
}

/// The rewrite's extensions. Anything else on the command line is the
/// reference's own surface (`[-f <config>] [-m]`) and starts the servers.
const SUBCOMMANDS: &[&str] = &[
    "serve",
    "query",
    "create-table",
    "list-tables",
    "put",
    "get",
    "recover",
    "dump-catalog",
    "smoke",
    "import-csv",
    "bench",
    "knn-bench",
    "fulltext-bench",
    "conformance",
];

fn usage_text() -> String {
    format!(
        "usage: infinity [-f <config>] [-m]\n       infinity <{}> <dir> [args...]",
        SUBCOMMANDS.join("|")
    )
}

fn usage() -> ! {
    eprintln!("{}", usage_text());
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

/// The reference's default invocation: `infinity [-f <config>] [-m]` starts the
/// Thrift and HTTP front ends on one engine. The data root is the parent of
/// `persistence_dir`; with no config the reference's built-in defaults apply.
fn serve_from_config(path: Option<&str>, maintenance: bool) -> common::Result<()> {
    let text = match path {
        Some(path) => std::fs::read_to_string(path).map_err(|e| common::Error::Io {
            path: PathBuf::from(path),
            source: e,
        })?,
        None => String::new(),
    };
    let persistence =
        toml_value(&text, "persistence_dir").unwrap_or_else(|| "/var/infinity/persistence".into());
    let root = Path::new(&persistence)
        .parent()
        .map(Path::to_path_buf)
        .unwrap_or_else(|| PathBuf::from("/var/infinity"));
    for sub in ["catalog", "persistence", "wal", "log", "tmp"] {
        std::fs::create_dir_all(root.join(sub)).map_err(|e| common::Error::Io {
            path: root.join(sub),
            source: e,
        })?;
    }
    let host = toml_value(&text, "server_address").unwrap_or_else(|| "0.0.0.0".into());
    let client_port = toml_value(&text, "client_port").unwrap_or_else(|| "23817".into());
    let http_port = toml_value(&text, "http_port").unwrap_or_else(|| "23820".into());
    let addr = std::env::var("INFINITY_BIND").unwrap_or_else(|_| format!("{host}:{client_port}"));
    let http_addr =
        std::env::var("INFINITY_HTTP_BIND").unwrap_or_else(|_| format!("{host}:{http_port}"));
    // `storage_type = "minio"`: the C++-format data files live in the bucket and
    // are downloaded before recovery, then kept in sync at every checkpoint.
    let storage_type = toml_value(&text, "storage_type").unwrap_or_default();
    let object_store = if storage_type.eq_ignore_ascii_case("minio") {
        let endpoint = toml_value(&text, "url").unwrap_or_default();
        let bucket = toml_value(&text, "bucket_name").unwrap_or_default();
        let https = toml_value(&text, "enable_https").unwrap_or_default();
        if endpoint.is_empty() || bucket.is_empty() {
            eprintln!("object store: url/bucket_name missing; keeping data local");
            None
        } else {
            let client = storage::s3::S3Client::new(storage::s3::S3Config {
                endpoint,
                bucket,
                access_key: toml_value(&text, "access_key").unwrap_or_default(),
                secret_key: toml_value(&text, "secret_key").unwrap_or_default(),
                https: https.eq_ignore_ascii_case("true"),
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
        // Every tree the checkpoints upload has to come back, not only the
        // C++-format `data/`: the catalog holds the tables, so restoring `data/`
        // alone opens an empty engine in which the block files have nothing to
        // belong to.
        for (dir, prefix) in [("data", "data"), ("catalog", "catalog")] {
            match storage::s3::restore_tree(client, &root.join(dir), prefix) {
                Ok(n) if n > 0 => println!("object store: restored {n} {prefix} file(s) from s3"),
                Ok(_) => {}
                Err(e) => eprintln!("object store: restoring {prefix} failed: {e}"),
            }
        }
    }
    // `[storage] persistence_format = "reference"`: publish the data files the
    // way the reference's `PersistenceManager` does (`pm|object|` records plus
    // one packed object under `persistence_dir`), and materialise them again on
    // open. That is the layout a bucket holds, so a directory written this way
    // is what the other engine reads from its object store.
    let object_dir = if toml_value(&text, "persistence_format")
        .unwrap_or_default()
        .eq_ignore_ascii_case("reference")
    {
        Some(root.join("persistence"))
    } else {
        None
    };
    // `[storage] snapshot_dir`: one directory per snapshot name
    // (`Config::SnapshotDir()`).
    let snapshot_dir = toml_value(&text, "snapshot_dir")
        .map(|v| v.trim().to_string())
        .filter(|v| !v.is_empty())
        .map(std::path::PathBuf::from);
    let options = executor::EngineOptions {
        object_dir,
        snapshot_dir,
        ..Default::default()
    };
    let engine = Arc::new(Engine::open_with_options(&root, options)?);
    if let Some(client) = object_store {
        engine.set_object_store(client);
    }
    // `[buffer] memindex_memory_quota` — a memory index is dumped into a C++
    // chunk once it reaches this size. The reference defaults to 4 GiB.
    let quota = toml_value(&text, "memindex_memory_quota").unwrap_or_default();
    let quota = if quota.trim().is_empty() {
        4 * 1024 * 1024 * 1024
    } else {
        parse_size_bytes(&quota)
    };
    engine.set_memindex_quota(quota);
    // `[storage] mem_index_capacity` — rows an in-memory index holds before it
    // is dumped into a sealed chunk (the reference defaults to 65536 rows).
    let capacity = toml_value(&text, "mem_index_capacity")
        .and_then(|v| v.trim().parse::<usize>().ok())
        .unwrap_or(64 * 1024);
    engine.set_memindex_capacity(capacity);
    // `[buffer] buffer_manager_size` — the column-block cache budget (the
    // reference defaults to 8 GiB).
    let buffer = toml_value(&text, "buffer_manager_size").unwrap_or_default();
    let buffer = if buffer.trim().is_empty() {
        8 * 1024 * 1024 * 1024
    } else {
        parse_size_bytes(&buffer) as usize
    };
    engine.set_buffer_manager_size(buffer);
    // The reference checkpoints periodically (`[wal] checkpoint_interval`), which
    // is what dumps the memory blocks into the on-disk C++ layout.
    let secs = parse_interval_secs(&toml_value(&text, "checkpoint_interval").unwrap_or_default());
    // `-m/--maintenance` (the reference's admin mode) leaves the periodic
    // triggers off: maintenance is about inspecting and repairing a stopped
    // node, not about letting cleanup/compact/checkpoint mutate it.
    if secs > 0 && !maintenance {
        // The checkpoint itself (WAL rotation + block flush) runs on the
        // engine's periodic trigger thread, next to cleanup/compact/optimize
        // (`PeriodicTriggerThread`); only the page release below stays here,
        // because `trim_allocator` is CLI-only (`infinity-executor` forbids
        // unsafe code).
        let weak = Arc::downgrade(&engine);
        std::thread::spawn(move || loop {
            std::thread::sleep(std::time::Duration::from_secs(secs));
            match weak.upgrade() {
                Some(_) => trim_allocator(),
                None => break,
            }
        });
    }
    // `[storage] cleanup_interval` / `compact_interval` / `optimize_interval`:
    // the reference's periodic triggers behind `PeriodicTriggerThread`
    // (`periodic_trigger_thread_impl.cpp:49-74`), plus the background processor
    // the memory-index quota dumps through (`BGMemIndexTracer::TriggerDump`).
    // The checkpoint trigger above stays on its own thread because
    // `trim_allocator` is CLI-only (`infinity-executor` forbids unsafe code):
    // folding the checkpoint into the scheduler would drop the page release
    // after each dump (Gate C).
    let cleanup = parse_interval_secs(&toml_value(&text, "cleanup_interval").unwrap_or_default());
    let compact = parse_interval_secs(&toml_value(&text, "compact_interval").unwrap_or_default());
    let optimize = parse_interval_secs(&toml_value(&text, "optimize_interval").unwrap_or_default());
    if !maintenance {
        engine.start_background_tasks(
            std::time::Duration::from_secs(secs),
            std::time::Duration::from_secs(cleanup),
            std::time::Duration::from_secs(compact),
            std::time::Duration::from_secs(optimize),
        );
    }
    // The reference starts both front ends on the same storage
    // (`infinity_main.cpp`: `http_server.Start(...)` next to `StartThriftServer()`).
    let http = server::http::HttpServer::bind(&http_addr, engine.clone())?;
    println!("http listening on {}", http.local_addr()?);
    std::thread::spawn(move || http.run());
    // The reference serves the PostgreSQL wire protocol too
    // (`pg_server.Run()` on `[network] postgres_port`). A port already taken by
    // a real PostgreSQL must not take the engine down, so a failed bind only
    // disables this front end.
    let pg_port = toml_value(&text, "postgres_port").unwrap_or_else(|| "5432".into());
    let pg_addr = std::env::var("INFINITY_PG_BIND").unwrap_or_else(|_| format!("{host}:{pg_port}"));
    match server::pg::PgServer::bind(&pg_addr, engine.clone()) {
        Ok(pg) => {
            println!("postgres listening on {}", pg.local_addr()?);
            std::thread::spawn(move || pg.run());
        }
        Err(e) => eprintln!("postgres front end disabled: {e}"),
    }
    let server = server::Server::bind_with_engine(&addr, engine)?;
    println!("serving on {}", server.local_addr()?);
    server.run();
}

/// Releases free heap pages back to the OS (glibc). A no-op elsewhere.
#[cfg(all(target_os = "linux", target_env = "gnu"))]
fn trim_allocator() {
    unsafe extern "C" {
        fn malloc_trim(pad: usize) -> i32;
    }
    // SAFETY: glibc's `malloc_trim` only returns already-free heap pages.
    unsafe {
        malloc_trim(0);
    }
}

#[cfg(not(all(target_os = "linux", target_env = "gnu")))]
fn trim_allocator() {}

/// `SELECT ROW_ID() FROM <table> SEARCH MATCH VECTOR (...)` — the statement
/// `knn_query_benchmark` issues (`topn_ = topk`, `ef` as an option).
fn knn_sql(table: &str, query: &[f32], topk: usize, ef: u64) -> String {
    let vec = query
        .iter()
        .map(|x| format!("{x:.6}"))
        .collect::<Vec<_>>()
        .join(",");
    format!(
        "SELECT ROW_ID() FROM {table} SEARCH MATCH VECTOR (col1, [{vec}], 'float', 'l2', {topk}) WITH (ef = {ef});"
    )
}

/// Reads an `fvecs` file: per record a `i32` dimension then that many `f32`.
fn read_fvecs(path: &str) -> common::Result<Vec<Vec<f32>>> {
    let bytes = std::fs::read(path).map_err(|e| common::Error::Io {
        path: PathBuf::from(path),
        source: e,
    })?;
    let mut out = Vec::new();
    let mut at = 0usize;
    while at + 4 <= bytes.len() {
        let dim = i32::from_le_bytes(bytes[at..at + 4].try_into().unwrap()) as usize;
        at += 4;
        if at + 4 * dim > bytes.len() {
            break;
        }
        let mut row = Vec::with_capacity(dim);
        for i in 0..dim {
            row.push(f32::from_le_bytes(
                bytes[at + 4 * i..at + 4 * i + 4].try_into().unwrap(),
            ));
        }
        at += 4 * dim;
        out.push(row);
    }
    Ok(out)
}

/// Reads an `ivecs` file (the SIFT ground truth): `i32` dimension then `i32`s.
fn read_ivecs(path: &str) -> common::Result<Vec<Vec<i32>>> {
    let bytes = std::fs::read(path).map_err(|e| common::Error::Io {
        path: PathBuf::from(path),
        source: e,
    })?;
    let mut out = Vec::new();
    let mut at = 0usize;
    while at + 4 <= bytes.len() {
        let dim = i32::from_le_bytes(bytes[at..at + 4].try_into().unwrap()) as usize;
        at += 4;
        if at + 4 * dim > bytes.len() {
            break;
        }
        let mut row = Vec::with_capacity(dim);
        for i in 0..dim {
            row.push(i32::from_le_bytes(
                bytes[at + 4 * i..at + 4 * i + 4].try_into().unwrap(),
            ));
        }
        at += 4 * dim;
        out.push(row);
    }
    Ok(out)
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
    // `infinity [-f <config>] [-m]` is the reference's surface: no subcommand,
    // the servers start. A known subcommand selects the rewrite's extension.
    match args.first().map(String::as_str) {
        Some(first) if SUBCOMMANDS.contains(&first) => {
            if let Err(e) = run(&args) {
                eprintln!("error: {e}");
                std::process::exit(1);
            }
        }
        Some(first) if !first.starts_with('-') => usage(),
        _ => {
            let (config, maintenance) = match parse_serve_args(&args) {
                Ok(v) => v,
                Err(e) => {
                    eprintln!("error: {e}");
                    usage();
                }
            };
            if let Err(e) = serve_from_config(config.as_deref(), maintenance) {
                eprintln!("error: {e}");
                std::process::exit(1);
            }
        }
    }
}

/// `-f/--config <path>`, `--config=<path>`, `-m/--maintenance`, `-h/--help` —
/// the options `infinity_main.cpp` declares through CLI11.
fn parse_serve_args(args: &[String]) -> Result<(Option<String>, bool), String> {
    let mut config = None;
    let mut maintenance = false;
    let mut i = 0;
    while i < args.len() {
        let a = args[i].as_str();
        match a {
            "-f" | "--config" => {
                i += 1;
                let path = args
                    .get(i)
                    .ok_or_else(|| format!("{a} needs a config file path"))?;
                config = Some(path.clone());
            }
            "-m" | "--maintenance" => maintenance = true,
            "-h" | "--help" => {
                println!("{}", usage_text());
                std::process::exit(0);
            }
            _ if a.starts_with("--config=") => {
                config = Some(a["--config=".len()..].to_string());
            }
            _ => return Err(format!("unrecognised argument: {a}")),
        }
        i += 1;
    }
    Ok((config, maintenance))
}

fn run(args: &[String]) -> common::Result<()> {
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
        "dump-catalog" => {
            // Raw `key<tab>hex` for every pair in the catalog store. The
            // reference's `SHOW CATALOG 'all'` renders a binary value through
            // the Postgres text protocol, which turns the `fast_rough_filter`
            // records into an empty string; the byte gate needs the actual
            // bytes, and this command is the one place that yields them.
            let store = storage::open_store(&catalog_dir(&dir))?;
            let filter = args.get(2).cloned();
            for (k, v) in store.scan_prefix(b"")? {
                let key = String::from_utf8_lossy(&k);
                if let Some(f) = &filter {
                    if !key.contains(f.as_str()) {
                        continue;
                    }
                }
                let mut hex = String::with_capacity(v.len() * 2);
                for b in &v {
                    use std::fmt::Write;
                    let _ = write!(hex, "{b:02x}");
                }
                println!("{key}\t{hex}");
            }
            Ok(())
        }
        "query" => {
            let sql = args[2..].join(" ");
            let e = Engine::open(&dir)?;
            for r in e.execute(&sql)? {
                print_result(&r);
            }
            let t_flush = Instant::now();
            e.flush()?;
            if std::env::var_os("INFINITY_PROFILE_IMPORT").is_some() {
                eprintln!("flush profile: {:?}", t_flush.elapsed());
            }
            Ok(())
        }
        "serve" => {
            // Extension: `infinity serve <dir> [host:port]` — the engine opened
            // with default options plus both front ends (what the old
            // `infinity-server` binary did).
            let addr = args
                .get(2)
                .cloned()
                .unwrap_or_else(|| "0.0.0.0:23817".into());
            let http_addr =
                std::env::var("INFINITY_HTTP_BIND").unwrap_or_else(|_| "0.0.0.0:23820".into());
            let options = executor::EngineOptions::default();
            let engine = std::sync::Arc::new(executor::Engine::open_with_options(&dir, options)?);
            let http = server::http::HttpServer::bind(&http_addr, engine.clone())?;
            println!("http listening on {}", http.local_addr()?);
            std::thread::spawn(move || http.run());

            // PostgreSQL wire front end, ported from the C++ server's
            // `src/network/pg_protocol_handler.cppm`. Same scope as upstream:
            // startup handshake plus the simple query protocol, so `psql` can
            // run SQL, but no extended query protocol, no SSL, no auth.
            //
            // Unlike upstream, a bind failure is not fatal: the Thrift and HTTP
            // front ends stay usable when 5432 is already taken by a local
            // PostgreSQL instance.
            let pg_addr =
                std::env::var("INFINITY_PG_BIND").unwrap_or_else(|_| "0.0.0.0:5432".into());
            match server::pg::PgServer::bind(&pg_addr, engine.clone()) {
                Ok(pg) => {
                    println!("pg listening on {}", pg.local_addr()?);
                    std::thread::spawn(move || pg.run());
                }
                Err(e) => println!("pg disabled, cannot bind {pg_addr}: {e:?}"),
            }

            let server = server::Server::bind_with_engine(&addr, engine)?;
            println!("serving on {}", server.local_addr()?);
            server.run();
        }
        "bench" => {
            let n: usize = args.get(2).and_then(|s| s.parse().ok()).unwrap_or(2000);
            let e = Engine::open(&dir)?;
            // Gate E runs the driver twice over the same directory and compares
            // the two runs, so each one must start from an empty table: without
            // the drop, the second run selects twice as many rows.
            let _ = e.execute("DROP TABLE IF EXISTS t;");
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
        // `infinity knn-bench <dir> <query.fvecs> <groundtruth.ivecs> <ef> [topk] [table]`:
        // the query half of the CI benchmark (`knn_query_benchmark`), run in
        // process so the numbers are the engine's and not the wire's. The table
        // defaults to `sift`; pass `gist` for the 960-dimension GIST table.
        "knn-bench" => {
            let query_path = args
                .get(2)
                .cloned()
                .unwrap_or_else(|| "sift_query.fvecs".into());
            let truth_path = args.get(3).cloned().unwrap_or_default();
            let ef: u64 = args.get(4).and_then(|s| s.parse().ok()).unwrap_or(200);
            let topk: usize = args.get(5).and_then(|s| s.parse().ok()).unwrap_or(100);
            let table = args.get(6).map(String::as_str).unwrap_or("sift");
            let queries = read_fvecs(&query_path)?;
            let truth = if truth_path.is_empty() {
                Vec::new()
            } else {
                read_ivecs(&truth_path)?
            };
            let e = Engine::open(&dir)?;
            // `knn_query_benchmark` runs two warm-up loops before it times.
            let warm = queries.iter().take(5);
            for q in warm {
                let _ = e.execute(&knn_sql(table, q, topk, ef))?;
            }
            let mut latencies = Vec::with_capacity(queries.len());
            let mut hits = 0usize;
            let mut total = 0usize;
            for (qi, q) in queries.iter().enumerate() {
                let t0 = Instant::now();
                let out = e.execute(&knn_sql(table, q, topk, ef))?;
                latencies.push(t0.elapsed().as_secs_f64());
                if let Some(want) = truth.get(qi) {
                    let mut got: Vec<u64> = Vec::new();
                    if let Some(QueryResult::Rows { rows, .. }) = out.first() {
                        for row in rows {
                            if let Some(v) = row.first() {
                                if let Ok(id) = v.to_slt_string().parse::<u64>() {
                                    got.push(id);
                                }
                            }
                        }
                    }
                    let want: std::collections::HashSet<u64> =
                        want.iter().map(|id| *id as u64).collect();
                    hits += got.iter().filter(|id| want.contains(id)).count();
                    total += want.len();
                }
            }
            let mut sorted = latencies.clone();
            sorted.sort_by(|a, b| a.partial_cmp(b).unwrap());
            let sum: f64 = latencies.iter().sum();
            let recall = if total > 0 {
                hits as f64 / total as f64
            } else {
                0.0
            };
            println!("knn-bench: {} queries, topk {topk}, ef {ef}", queries.len());
            println!("Total cost : {sum:.6} s");
            println!("qps: {:.2}", queries.len() as f64 / sum);
            println!("lat p50: {:.3} ms", sorted[sorted.len() / 2] * 1000.0);
            println!(
                "lat p95: {:.3} ms",
                sorted[(sorted.len() as f64 * 0.95) as usize] * 1000.0
            );
            println!("recall@{topk}: {recall:.4}");
            Ok(())
        }
        "import-csv" => {
            let table = args.get(2).cloned().unwrap_or_else(|| "t".into());
            let file = args.get(3).cloned().unwrap_or_default();
            if file.is_empty() {
                return Err(common::Error::Invalid(
                    "import-csv needs a file path".into(),
                ));
            }
            let text = std::fs::read_to_string(&file).map_err(|e| common::Error::io(&file, e))?;
            let mut lines = text.lines().filter(|l| !l.trim().is_empty());
            let header: Vec<String> = lines
                .next()
                .ok_or_else(|| common::Error::Invalid("empty csv".into()))?
                .split(',')
                .map(|s| s.trim().to_string())
                .collect();
            let e = Engine::open(&dir)?;
            // create the table if it does not exist (all columns VARCHAR)
            let cols: Vec<String> = header.iter().map(|c| format!("{c} VARCHAR")).collect();
            let _ = e.execute(&format!("CREATE TABLE {table} ({});", cols.join(", ")));

            let mut total = 0usize;
            let mut batch: Vec<String> = Vec::new();
            let flush = |e: &Engine, batch: &mut Vec<String>| -> common::Result<usize> {
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
        // `infinity fulltext-bench <dir> <mode> <corpus.jsonl> [batch]`: the
        // reference's `fulltext_benchmark` (insert/import/merge/query).
        "fulltext-bench" => {
            let mode = args.get(2).cloned().unwrap_or_else(|| "query".into());
            let corpus = args.get(3).cloned().unwrap_or_default();
            if corpus.is_empty() {
                return Err(common::Error::Invalid(
                    "fulltext-bench needs a corpus.jsonl path".into(),
                ));
            }
            let batch: usize = args.get(4).and_then(|s| s.parse().ok()).unwrap_or(500);
            fulltext_bench::run(&dir, &mode, &corpus, batch)
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
                let cells: Vec<String> = row.iter().map(|v| v.to_slt_string()).collect();
                println!("{}", cells.join(" | "));
            }
            println!("({} row(s))", rows.len());
        }
    }
}

fn run_smoke(dir: &Path) -> common::Result<()> {
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

    let fresh: Arc<dyn MetaStore> = Arc::new(RocksMetaStore::open(dir.join("kv_recovered"))?);
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

/// One connection (the default one) for the crate's runner.
struct OneConnection(Option<SltDb>);

impl sqllogictest::MakeConnection for OneConnection {
    type Conn = SltDb;
    type MakeFuture = std::future::Ready<Result<SltDb, common::Error>>;

    fn make(&mut self) -> Self::MakeFuture {
        std::future::ready(Ok(self
            .0
            .take()
            .expect("the suite opens exactly one connection")))
    }
}

/// The database side of the suite: the crate parses and validates, this runs
/// the SQL.
struct SltDb {
    engine: Engine,
}

impl sqllogictest::DB for SltDb {
    type Error = common::Error;
    type ColumnType = sqllogictest::DefaultColumnType;

    fn run(
        &mut self,
        sql: &str,
    ) -> Result<sqllogictest::DBOutput<Self::ColumnType>, common::Error> {
        let results = self.engine.execute(sql)?;
        let Some(last) = results.into_iter().last() else {
            return Ok(sqllogictest::DBOutput::StatementComplete(0));
        };
        Ok(match last {
            QueryResult::Rows { columns, rows } => sqllogictest::DBOutput::Rows {
                // The crate's default column validator does not compare kinds;
                // text is what the wire renders anyway.
                types: vec![sqllogictest::DefaultColumnType::Text; columns.len().max(1)],
                rows: rows
                    .iter()
                    .map(|row| {
                        row.iter()
                            // The reference's runner writes an empty value as
                            // "(empty)"; a blank line cannot express it in .slt.
                            .map(|v| {
                                let text = v.to_slt_string();
                                if text.is_empty() {
                                    "(empty)".to_string()
                                } else {
                                    text
                                }
                            })
                            .collect()
                    })
                    .collect(),
            },
            QueryResult::Inserted { count }
            | QueryResult::Deleted { count }
            | QueryResult::Updated { count } => {
                sqllogictest::DBOutput::StatementComplete(count as u64)
            }
            _ => sqllogictest::DBOutput::StatementComplete(0),
        })
    }

    fn engine_name(&self) -> &str {
        "infinity"
    }
}

/// Runs the reference's .slt suite against the Rust engine.
///
/// The parser, the record semantics (statement ok/error, query <types>
/// [rowsort], the directives) and the validator all come from
/// sqllogictest-rs, the crate the reference's own runner is built from, so
/// there is one implementation of the format. Each file gets a fresh engine in
/// its own temporary directory; any failure makes the command exit non-zero.
fn run_conformance(slt_dir: &str) -> common::Result<()> {
    // Fixture data (/var/infinity/test_data/...) resolves under test/data.
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
    if files.is_empty() {
        return Err(common::Error::Invalid(format!(
            "no .slt file under {slt_dir}"
        )));
    }
    let mut failures = Vec::new();
    for (index, file) in files.iter().enumerate() {
        let dir = std::env::temp_dir().join(format!("infconf-{}-{index}", std::process::id()));
        let _ = std::fs::remove_dir_all(&dir);
        let engine = match Engine::open(&dir) {
            Ok(engine) => engine,
            Err(e) => {
                failures.push(format!("{}: open failed: {e}", file.display()));
                continue;
            }
        };
        let mut runner = sqllogictest::Runner::new(OneConnection(Some(SltDb { engine })));
        if let Err(e) = runner.run_file(file) {
            failures.push(format!("{}: {e}", file.display()));
        }
        runner.shutdown();
        let _ = std::fs::remove_dir_all(&dir);
    }
    println!("{} file(s), {} failed", files.len(), failures.len());
    if failures.is_empty() {
        return Ok(());
    }
    for failure in &failures {
        eprintln!("{failure}");
    }
    Err(common::Error::Invalid(format!(
        "{} .slt file(s) failed",
        failures.len()
    )))
}
