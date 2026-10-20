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
use tempfile::tempdir;

/// The value behind `SHOW VARIABLE <name>` at global scope.
fn global(engine: &Engine, name: &str) -> String {
    match engine.show_variable(name, Scope::Global).unwrap() {
        QueryResult::Rows { rows, .. } => rows[0][0].clone(),
        other => panic!("SHOW VARIABLE {name} returned {other:?}"),
    }
}

fn session(engine: &Engine, name: &str) -> String {
    match engine.show_variable(name, Scope::Session).unwrap() {
        QueryResult::Rows { rows, .. } => rows[0][0].clone(),
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
