//! The engine's diagnostics accessors: the values `SHOW VARIABLE`,
//! `SHOW BUFFERS`, `SHOW QUERIES`, `SHOW TRANSACTIONS`, … and the HTTP
//! `/instance/*`, `/variables/*` and `/admin/*` routes read.
//!
//! Each test creates the state it asserts on and asks the accessor whether it
//! can see it, so a fabricated constant (the audit's complaint) fails.

use std::sync::{Arc, Condvar, Mutex};
use std::time::{Duration, Instant};

use crate::bg_task::{BGTaskProcessor, BgTask};
use crate::{Engine, QueryResult, Scope};
use parser::Literal;
use tempfile::tempdir;

/// The value behind `SHOW VARIABLE <name>` at global scope.
fn global(engine: &Engine, name: &str) -> String {
    match engine.show_variable(name, Scope::Global).unwrap() {
        QueryResult::Rows { rows, .. } => rows[0][0].to_slt_string(),
        other => panic!("SHOW VARIABLE {name} returned {other:?}"),
    }
}

fn session(engine: &Engine, name: &str) -> String {
    match engine.show_variable(name, Scope::Session).unwrap() {
        QueryResult::Rows { rows, .. } => rows[0][0].to_slt_string(),
        other => panic!("SHOW SESSION VARIABLE {name} returned {other:?}"),
    }
}

fn number(engine: &Engine, name: &str) -> u64 {
    global(engine, name)
        .parse()
        .unwrap_or_else(|e| panic!("{name} is not a number: {e}"))
}

/// A background task that blocks until the test releases it, so
/// `bg_task_count` has something real to count.
struct BlockingTask {
    gate: Arc<(Mutex<bool>, Condvar)>,
}

impl BgTask for BlockingTask {
    fn name(&self) -> &str {
        "BlockingTask"
    }

    fn run(self: Box<Self>) {
        let (lock, cv) = &*self.gate;
        let mut released = lock.lock().unwrap();
        while !*released {
            released = cv.wait(released).unwrap();
        }
    }
}

fn wait_until(mut cond: impl FnMut() -> bool, what: &str) {
    let deadline = Instant::now() + Duration::from_secs(5);
    while Instant::now() < deadline {
        if cond() {
            return;
        }
        std::thread::sleep(Duration::from_millis(5));
    }
    panic!("timed out waiting for {what}");
}

/// The audit's item 29: every global variable `SHOW VARIABLES` serves must
/// report engine state. The test builds that state — a buffered block, a live
/// transaction, a rollback, a queued background task — and reads it back
/// through the same accessor `SHOW VARIABLE` uses.
#[test]
fn global_variables_report_real_counts() {
    let dir = tempdir().unwrap();
    let engine = Arc::new(Engine::open(dir.path()).unwrap());
    engine.execute("CREATE TABLE gv (id INT);").unwrap();
    engine.execute("INSERT INTO gv VALUES (1), (2);").unwrap();
    // A checkpoint writes the C++ block mirror; reading it back is what puts
    // blocks in the engine's buffer cache (`BlockIndex::get_block`).
    engine.flush().unwrap();
    engine.read_cpp_blocks("gv").unwrap();

    // Reading the block mirror populates the engine's block cache, so the
    // buffer counters must see the blocks the test itself created.
    let buffered = number(&engine, "buffer_object_count");
    assert!(
        buffered >= 1,
        "reading the block mirror must buffer at least one block"
    );
    // Every cached block is owned by the cache alone right after the read.
    assert!(number(&engine, "unused_buffer_object") <= buffered);
    // The cache saw the requests the read issued.
    let misses = global(&engine, "memory_cache_miss");
    let (miss, requests) = misses.split_once('/').expect("misses/requests");
    assert!(requests.parse::<u64>().unwrap() >= buffered);
    assert!(miss.parse::<u64>().unwrap() >= 1);

    // A live transaction is counted while it exists and its rollback is
    // counted after it ends.
    assert_eq!(number(&engine, "active_txn_count"), 0);
    let txn = engine.begin_txn();
    assert_eq!(number(&engine, "active_txn_count"), 1);
    txn.rollback();
    assert_eq!(number(&engine, "active_txn_count"), 0);
    assert_eq!(number(&engine, "total_rollback_count"), 1);

    // No processor attached: no task can be running. Attached but idle: still
    // nothing running. One blocked task: exactly one.
    assert_eq!(number(&engine, "bg_task_count"), 0);
    let processor = Arc::new(BGTaskProcessor::new(1));
    processor.start();
    engine.attach_bg_task_processor(processor.clone());
    assert_eq!(number(&engine, "bg_task_count"), 0);
    let gate = Arc::new((Mutex::new(false), Condvar::new()));
    assert!(processor.submit(Box::new(BlockingTask { gate: gate.clone() })));
    wait_until(
        || processor.running_task_count() == 1,
        "the background task to start",
    );
    assert_eq!(number(&engine, "bg_task_count"), 1);
    assert_eq!(number(&engine, "running_bg_task"), 1);
    {
        let (lock, cv) = &*gate;
        *lock.lock().unwrap() = true;
        cv.notify_all();
    }
    processor.drain();
    assert_eq!(number(&engine, "bg_task_count"), 0);

    // The process's own counters, read from /proc.
    assert!(
        number(&engine, "system_memory_usage") > 0,
        "a running process has a nonzero RSS"
    );
    assert!(
        number(&engine, "open_file_count") > 2,
        "stdin/stdout/stderr are always open"
    );
    // The first cpu_usage call has no previous sample; later calls measure the
    // delta since it, so they can only be non-negative.
    assert_eq!(global(&engine, "cpu_usage"), "0");
    assert!(global(&engine, "cpu_usage").parse::<f64>().unwrap() >= 0.0);

    // Session scope reports this engine's own counters, not constants.
    engine.execute("SELECT 1;").unwrap();
    assert!(session(&engine, "query_count").parse::<u64>().unwrap() >= 1);
    assert_eq!(session(&engine, "total_rollback_count"), "1");
}

/// `SHOW VARIABLES` lists every name in the reference's table, and every value
/// is the one the single-name accessor returns.
#[test]
fn variables_listing_covers_the_name_table() {
    let dir = tempdir().unwrap();
    let engine = Engine::open(dir.path()).unwrap();
    let listed = engine.variables(Scope::Global);
    // `cache_result_capacity` is absent while the result cache is off, which
    // is the reference's default (`DEFAULT_RESULT_CACHE = "off"`) and its
    // `ExecuteShowGlobalVariables` behaviour.
    let expected = crate::variables::ALL_GLOBAL_NAMES
        .iter()
        .filter(|name| !name.eq_ignore_ascii_case("cache_result_capacity"))
        .collect::<Vec<_>>();
    assert_eq!(listed.len(), expected.len());
    assert!(
        !listed
            .iter()
            .any(|(name, _)| name.eq_ignore_ascii_case("cache_result_capacity")),
        "the capacity row belongs to an enabled cache"
    );
    // The names are the reference's table, in its order, and every value is
    // the one the single-name accessor returns. The live counters (open fds,
    // RSS, cache hits, transactions) are skipped: sampling them twice cannot
    // return the same number.
    const VOLATILE: [&str; 9] = [
        "active_txn_count",
        "buffer_object_count",
        "cpu_usage",
        "disk_cache_miss",
        "memory_cache_miss",
        "open_file_count",
        "system_memory_usage",
        "unused_buffer_object",
        "query_count",
    ];
    for (listed_name, (name, value)) in expected.iter().zip(listed.iter()) {
        assert_eq!(**listed_name, *name);
        assert!(!value.is_empty(), "{name} must have a value");
        if !VOLATILE.contains(&name.as_str()) {
            assert_eq!(*value, global(&engine, name), "{name}");
        }
    }
    let sessions = engine.variables(Scope::Session);
    assert_eq!(sessions.len(), crate::variables::ALL_SESSION_NAMES.len());
}

/// `SET VARIABLE` follows `PhysicalCommand`'s rules: session variables are
/// read-only, `profile` and `profile_record_capacity` are settable, every other
/// global variable is read-only, and an unknown name is an error.
#[test]
fn set_variables_follows_the_reference_rules() {
    let dir = tempdir().unwrap();
    let engine = Engine::open(dir.path()).unwrap();

    engine
        .set_variables(Scope::Global, &[("profile".into(), "true".into())])
        .unwrap();
    assert_eq!(global(&engine, "profile"), "true");
    engine
        .set_variables(
            Scope::Global,
            &[("profile_record_capacity".into(), "2".into())],
        )
        .unwrap();
    assert_eq!(global(&engine, "profile_record_capacity"), "2");

    let error = engine
        .set_variables(Scope::Global, &[("cpu_usage".into(), "1".into())])
        .unwrap_err();
    assert_eq!(error.status_code(), 3076);
    assert!(error
        .to_string()
        .ends_with("Global variable: cpu_usage is read-only"));

    let error = engine
        .set_variables(Scope::Global, &[("no_such_var".into(), "1".into())])
        .unwrap_err();
    assert_eq!(error.status_code(), 3076);
    assert!(error
        .to_string()
        .ends_with("unknown global variable no_such_var"));

    let error = engine
        .set_variables(Scope::Session, &[("query_count".into(), "1".into())])
        .unwrap_err();
    assert_eq!(error.status_code(), 3076);
    assert!(error
        .to_string()
        .ends_with("Session variable: query_count is read-only"));
}

/// `SHOW TRANSACTIONS` / `SHOW TRANSACTION <id>` read the transaction manager's
/// registry.
#[test]
fn transaction_accessors_expose_the_live_transaction() {
    let dir = tempdir().unwrap();
    let engine = Engine::open(dir.path()).unwrap();
    assert!(engine.active_transactions().is_empty());

    let txn = engine.begin_txn();
    let active = engine.active_transactions();
    assert_eq!(active.len(), 1);
    let id = active[0].txn_id;
    assert_eq!(
        engine.transaction_detail(id).map(|t| t.txn_id),
        Some(id),
        "SHOW TRANSACTION <id> must find the live transaction"
    );
    assert!(engine.transaction_detail(id + 1000).is_none());
    txn.rollback();
    assert!(engine.active_transactions().is_empty());
}

/// `SHOW QUERIES` / `SHOW QUERY <id>` / `SHOW PROFILES` are backed by the real
/// registry: a returned statement leaves the running set and, with profiling
/// on, lands in the profile history.
#[test]
fn query_registry_lists_running_and_finished_statements() {
    let dir = tempdir().unwrap();
    let engine = Engine::open(dir.path()).unwrap();
    engine
        .set_variables(Scope::Global, &[("profile".into(), "true".into())])
        .unwrap();

    assert!(engine.running_queries().is_empty());
    engine.execute("SELECT 1;").unwrap();
    assert!(
        engine.running_queries().is_empty(),
        "a returned statement is no longer running"
    );

    let profiles = engine.profiles();
    assert_eq!(profiles.len(), 1);
    assert_eq!(profiles[0].query_kind, "SELECT");
    assert_eq!(profiles[0].query_text, "SELECT 1;");

    // The id of a finished statement still resolves.
    let row = engine
        .query_profile(profiles[0].query_id)
        .expect("SHOW QUERY <id> of a finished statement");
    assert_eq!(row.session_id, crate::diagnostics::SESSION_ID);
    assert_eq!(row.query_kind, "SELECT");

    // The history is bounded by `profile_record_capacity`.
    engine
        .set_variables(
            Scope::Global,
            &[("profile_record_capacity".into(), "2".into())],
        )
        .unwrap();
    for _ in 0..3 {
        engine.execute("SELECT 2;").unwrap();
    }
    assert_eq!(engine.profiles().len(), 2);
    assert_eq!(engine.profiles()[0].query_text, "SELECT 2;");
}

/// A statement that fails still deregisters: the error path must not leak a
/// registry row.
#[test]
fn failed_statements_leave_the_registry() {
    let dir = tempdir().unwrap();
    let engine = Engine::open(dir.path()).unwrap();
    assert!(engine.execute("SELECT * FROM missing_table;").is_err());
    assert!(engine.running_queries().is_empty());
}

/// `SHOW BUFFERS` / `SHOW OBJECT <name>`: the rows come from the block cache,
/// and the object the test touched is addressable by its path.
#[test]
fn buffer_objects_describe_the_block_cache() {
    let dir = tempdir().unwrap();
    let engine = Engine::open(dir.path()).unwrap();
    engine.execute("CREATE TABLE bo (id INT);").unwrap();
    engine.execute("INSERT INTO bo VALUES (7);").unwrap();
    engine.flush().unwrap();
    engine.read_cpp_blocks("bo").unwrap();

    let objects = engine.buffer_objects();
    assert_eq!(objects.len(), 1, "one block was written and read");
    let row = &objects[0];
    assert!(row.object_size > 0);
    let block_path = row.object_path.to_string_lossy().into_owned();
    assert!(
        block_path.contains("/seg_0/blk_0") && row.object_path.is_dir(),
        "the path is the block's C++-layout directory: {block_path}"
    );
    let path = row.object_path.to_string_lossy().into_owned();
    let by_name = engine
        .buffer_object(&path)
        .expect("SHOW OBJECT <name> for a buffered block");
    assert_eq!(by_name.object_size, row.object_size);
    assert!(engine.buffer_object("no/such/block").is_none());
}

/// `SHOW FILES`: every file under the engine directory with its size.
#[test]
fn all_files_walks_the_engine_directory() {
    let dir = tempdir().unwrap();
    let engine = Engine::open(dir.path()).unwrap();
    let files = engine.all_files();
    assert!(!files.is_empty(), "an open engine has files on disk");
    for (path, size) in &files {
        assert!(path.starts_with(dir.path()), "{}", path.display());
        assert_eq!(
            std::fs::metadata(path).map(|m| m.len()).unwrap_or(0),
            *size as u64,
            "{}",
            path.display()
        );
    }
    engine.execute("CREATE TABLE af (id INT);").unwrap();
    engine.execute("INSERT INTO af VALUES (1);").unwrap();
    engine.flush().unwrap();
    assert!(
        engine
            .all_files()
            .iter()
            .any(|(path, size)| path.to_string_lossy().contains("/blk_") && *size > 0),
        "the checkpoint writes the block mirror"
    );
}

/// `SHOW MEMINDEX`: the in-memory part of each index the test created, with the
/// rows it holds.
#[test]
fn memory_indexes_report_the_in_memory_rows() {
    let dir = tempdir().unwrap();
    let engine = Engine::open(dir.path()).unwrap();
    engine
        .execute("CREATE TABLE mi (id INT, body VARCHAR, v EMBEDDING(2));")
        .unwrap();
    engine
        .execute("CREATE INDEX mi_ft ON mi (body) USING FULLTEXT;")
        .unwrap();
    engine
        .execute("INSERT INTO mi VALUES (1, 'hello world', [1.0, 0.0]);")
        .unwrap();

    let rows = engine.memory_indexes();
    let fulltext = rows
        .iter()
        .find(|r| r.index == "mi_ft")
        .expect("the full-text index keeps an in-memory segment");
    assert_eq!(fulltext.table, "default_db.mi");
    assert_eq!(fulltext.index_type, "FULLTEXT");
    assert_eq!(fulltext.row_count, 1);
    assert!(fulltext.mem_used > 0, "the segment holds one document");
}

/// `total_rollback_count` must move exactly once per rollback.
///
/// `TxnManager::rollback` removes the transaction's registry slot and then
/// drops the `Txn`, whose `Drop` removes the slot again — so a counter bumped in
/// both places would double count. `Txn::drop` only releases the slot; the
/// counter is bumped by `rollback` alone, as `NewTxnManager::RollBackTxn` does in
/// the reference (`~NewTxn` is empty, `new_txn_impl.cpp:128`).
#[test]
fn rollback_counter_moves_exactly_once_per_rollback() {
    let dir = tempdir().unwrap();
    let engine = Engine::open(dir.path()).unwrap();
    engine.execute("CREATE TABLE rb (id INT);").unwrap();

    let txn = engine.begin_txn();
    assert_eq!(number(&engine, "active_txn_count"), 1);
    txn.rollback();
    assert_eq!(number(&engine, "total_rollback_count"), 1);
    assert_eq!(number(&engine, "active_txn_count"), 0);

    // A commit is not a rollback, and the counter stays where it was.
    // `total_commit_count` is measured as a delta: the startup and the DDL are
    // write transactions too (the reference counts them), so the absolute value
    // already moved before this commit — the assertion is that this one commit
    // adds exactly one.
    let commits_before = number(&engine, "total_commit_count");
    let mut txn = engine.begin_txn();
    txn.append("rb", vec![vec![Literal::Int(1)]]).unwrap();
    txn.commit().unwrap();
    assert_eq!(number(&engine, "total_rollback_count"), 1);
    assert_eq!(number(&engine, "total_commit_count"), commits_before + 1);

    // Dropping an uncommitted transaction releases its slot without counting a
    // rollback (only an explicit rollback does).
    {
        let _txn = engine.begin_txn();
        assert_eq!(number(&engine, "active_txn_count"), 1);
    }
    assert_eq!(number(&engine, "active_txn_count"), 0);
    assert_eq!(number(&engine, "total_rollback_count"), 1);

    // A second rollback moves it again, by exactly one.
    engine.begin_txn().rollback();
    assert_eq!(number(&engine, "total_rollback_count"), 2);
}

/// `cache_result_capacity` must report the configured capacity, not the
/// internal "unbounded" sentinel. `result_cache_capacity()` returns
/// `usize::MAX` when nothing is configured, and that value used to be printed
/// verbatim (18446744073709551615) by `SHOW VARIABLES` and `GET /variables/*`.
#[test]
fn cache_result_capacity_reports_the_configuration_not_the_sentinel() {
    let dir = tempdir().unwrap();
    let engine = Engine::open(dir.path()).unwrap();
    assert_eq!(
        engine
            .global_variable_value(crate::variables::GlobalVariable::CacheResultCapacity)
            .unwrap(),
        "1024",
        "the reference's default capacity, never usize::MAX"
    );
    engine
        .execute("SET CONFIG cache_result_capacity 64;")
        .unwrap();
    assert_eq!(
        engine
            .global_variable_value(crate::variables::GlobalVariable::CacheResultCapacity)
            .unwrap(),
        "64",
        "a configured capacity is reported as configured"
    );
}
