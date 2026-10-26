//! Ports of `src/unit_test/storage/bg_task/*_ut.cpp`.
//!
//! Rows ported here:
//! - 285 `background_process_ut.cpp` `BGProcessTest.test1` — start/stop the
//!   `BGTaskProcessor` (`crate::bg_task`).
//! - 288 `dump_index_task_2_ut.cpp`
//!   `DISABLED_SLOW_row_cnt_exceed_memory_quota` — the quota proposes a
//!   background `DumpMemIndexTask`; two appends coalesce into one 16384-row
//!   chunk, the next append produces a second 8192-row chunk.
//! - 289 `dump_index_task_ut.cpp`
//!   `DISABLED_SLOW_row_cnt_exceed_memindex_capacity` — the row-capacity
//!   trigger dumps each 8192-row append into its own chunk.
//!
//! The reference disables 288/289 (`DISABLED_SLOW_`), so their Rust ports keep
//! `#[ignore]`; they still run under `--ignored`.

use std::sync::atomic::{AtomicUsize, Ordering};
use std::sync::Arc;

use crate::bg_task::{BGTaskProcessor, BgTask};
use crate::{Engine, QueryResult};
use infinity_parser::Literal;
use tempfile::tempdir;

fn rows(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in engine.execute(sql).unwrap() {
        if let QueryResult::Rows { rows, .. } = result {
            return rows;
        }
    }
    Vec::new()
}

/// One 8192-row block: `col1 = 1`, `col2 = abcdefghijklmnopqrstuvwxyz`
/// (`MakeInputBlock`).
fn append_block(engine: &Engine, table: &str) {
    let row = [
        Literal::Int(1),
        Literal::Str("abcdefghijklmnopqrstuvwxyz".into()),
    ];
    engine
        .insert_literals(table, None, (0..8192).map(|_| row.to_vec()).collect())
        .unwrap();
}

/// C++: `cleanup_task_ut.cpp:57` (`test_delete_db_simple`).
///
/// After `DROP TABLE` + checkpoint + cleanup the dropped table's data files are
/// gone and every file of the surviving table is still there
/// (`BaseTest::CheckFilePaths`).
#[test]
fn cleanup_removes_the_dropped_table_files() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    e.execute("CREATE TABLE tb2 (col1 INT, col2 VARCHAR);")
        .unwrap();
    append_block(&e, "default_db.tb1");
    append_block(&e, "default_db.tb2");
    e.flush().unwrap();

    let exist_paths = e.table_files("default_db.tb1");
    let delete_paths = e.table_files("default_db.tb2");
    assert!(!exist_paths.is_empty(), "tb1 has no data files");
    assert!(!delete_paths.is_empty(), "tb2 has no data files");

    e.execute("DROP TABLE default_db.tb2;").unwrap();
    e.flush().unwrap();
    // `TransactionType::kCleanup` (the reference waits for its bg task).
    e.cleanup().unwrap();

    for path in &delete_paths {
        assert!(!path.exists(), "dropped table file survived: {path:?}");
    }
    for path in &exist_paths {
        assert!(path.exists(), "surviving table file was removed: {path:?}");
    }
    assert!(e.table_exists("tb1"));
    assert!(!e.table_exists("tb2"));
}

/// The same cleanup, but as the reference runs it: submitted to the background
/// processor and waited for with `drain` instead of called inline. This is what
/// the periodic trigger does in production
/// (`CleanupPeriodicTrigger::Trigger` → `BGTaskProcessor::Submit`).
#[test]
fn background_cleanup_task_removes_dropped_files() {
    let dir = tempdir().unwrap();
    let engine = Arc::new(Engine::open(dir.path()).unwrap());
    engine
        .execute("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    engine
        .execute("CREATE TABLE tb2 (col1 INT, col2 VARCHAR);")
        .unwrap();
    append_block(&engine, "default_db.tb1");
    append_block(&engine, "default_db.tb2");
    engine.flush().unwrap();

    let exist_paths = engine.table_files("default_db.tb1");
    let delete_paths = engine.table_files("default_db.tb2");
    assert!(!exist_paths.is_empty(), "tb1 has no data files");
    assert!(!delete_paths.is_empty(), "tb2 has no data files");

    engine.execute("DROP TABLE default_db.tb2;").unwrap();
    engine.flush().unwrap();

    let processor = Arc::new(BGTaskProcessor::new(1));
    processor.start();
    engine.attach_bg_task_processor(Arc::clone(&processor));
    assert!(engine.submit_cleanup(), "cleanup was not queued");
    engine.drain_background_tasks();

    for path in &delete_paths {
        assert!(!path.exists(), "dropped table file survived: {path:?}");
    }
    for path in &exist_paths {
        assert!(path.exists(), "surviving table file was removed: {path:?}");
    }
    processor.stop();
}

/// `start_background_tasks` attaches a processor (so the memory-index quota
/// starts proposing dumps instead of dumping inline) and spawns the periodic
/// trigger thread. The intervals are zero here so no trigger fires and the test
/// does not depend on timing.
#[test]
fn start_background_tasks_sets_processor() {
    let dir = tempdir().unwrap();
    let engine = Arc::new(Engine::open(dir.path()).unwrap());
    // No processor by default: `Engine::open` must stay deterministic.
    assert!(!engine.submit_cleanup());

    engine.start_background_tasks(
        std::time::Duration::ZERO,
        std::time::Duration::ZERO,
        std::time::Duration::ZERO,
        std::time::Duration::ZERO,
    );
    assert!(engine.submit_optimize_indexes(), "no processor attached");
    engine.drain_background_tasks();

    // Dropping the engine stops the processor (`BGTaskProcessor::drop`) and the
    // trigger thread, which only holds a `Weak` and exits on its next tick.
    drop(engine);
}

/// C++: `compact_segments_task_ut.cpp:73` (`bg_compact`).
///
/// Eleven imports form eleven segments; compaction merges them (`segment_count`
/// drops from 11 to 1) without losing a row.
#[test]
fn compact_reduces_the_segment_count() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();

    // `segment_sizes = {1, 10}`: one segment holding one block, then ten more.
    for _ in 0..11 {
        append_block(&e, "default_db.tb1");
    }
    assert_eq!(e.row_count("default_db.tb1").unwrap(), 11 * 8192);
    // The reference asserts `segment_ids.size() < 11` after the background
    // compaction; here the eleven write batches report as eleven segments
    // before it and a single merged segment after.
    let before = e.segment_count("default_db.tb1");
    assert_eq!(before, 11);

    e.compact_all_segments("default_db.tb1").unwrap();

    let after = e.segment_count("default_db.tb1");
    assert!(after < before);
    assert_eq!(after, 1);
    assert_eq!(e.row_count("default_db.tb1").unwrap(), 11 * 8192);
    let count: usize = rows(&e, "SELECT count(*) FROM default_db.tb1;")[0][0]
        .parse()
        .unwrap();
    assert_eq!(count, 11 * 8192);
}

/// `{col1 INT, col2 VARCHAR}` plus a secondary index on `col1`
/// (`DumpMemIndexTaskTest` fixture).
fn secondary_fixture(engine: &Engine) {
    engine.execute("CREATE DATABASE db1;").unwrap();
    engine
        .execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    engine
        .execute("CREATE INDEX idx1 ON db1.tb1 (col1);")
        .unwrap();
}

struct CountTask(Arc<AtomicUsize>);

impl BgTask for CountTask {
    fn name(&self) -> &str {
        "CountTask"
    }

    fn run(self: Box<Self>) {
        self.0.fetch_add(1, Ordering::SeqCst);
    }
}

/// C++: `background_process_ut.cpp:37` (`BGProcessTest.test1`).
///
/// Start the processor, run a submitted task to completion (the reference's
/// stop task is queued behind it), and stop.
#[test]
fn background_process_start_stop() {
    let processor = Arc::new(BGTaskProcessor::new(2));
    processor.start();

    let counter = Arc::new(AtomicUsize::new(0));
    processor.submit(Box::new(CountTask(Arc::clone(&counter))));
    processor.drain();
    assert_eq!(counter.load(Ordering::SeqCst), 1);

    processor.stop();
    assert_eq!(processor.running_task_count(), 0);
}

struct PanickingTask;

impl BgTask for PanickingTask {
    fn name(&self) -> &str {
        "PanickingTask"
    }

    fn run(self: Box<Self>) {
        panic!("task failure");
    }
}

/// A task that unwinds must not leave the running-task counter raised: the
/// counter is what `drain()` and `stop()` wait on, so a skipped decrement hangs
/// the shutdown of the process (the `PlaidIndex` `expect` in a background dump
/// is the reachable case).
#[test]
fn a_panicking_task_does_not_wedge_drain() {
    let processor = Arc::new(BGTaskProcessor::new(1));
    processor.start();
    assert!(processor.submit(Box::new(PanickingTask)));

    // Bound the wait: a wedged `drain()` would otherwise hang the test run
    // instead of failing it.
    let (tx, rx) = std::sync::mpsc::channel();
    let draining = Arc::clone(&processor);
    std::thread::spawn(move || {
        draining.drain();
        let _ = tx.send(());
    });
    rx.recv_timeout(std::time::Duration::from_secs(5))
        .expect("drain() wedged on a panicking task");
    assert_eq!(processor.running_task_count(), 0);

    // The worker survived the unwind and still serves the queue.
    let counter = Arc::new(AtomicUsize::new(0));
    assert!(processor.submit(Box::new(CountTask(Arc::clone(&counter)))));
    processor.drain();
    assert_eq!(counter.load(Ordering::SeqCst), 1);
    processor.stop();
}

/// A submission made after `stop()` is reported instead of silently dropped,
/// so a caller that set a pending-work flag can clear it.
#[test]
fn submit_after_stop_is_rejected() {
    let processor = Arc::new(BGTaskProcessor::new(1));
    processor.start();
    let counter = Arc::new(AtomicUsize::new(0));
    assert!(processor.submit(Box::new(CountTask(Arc::clone(&counter)))));
    processor.drain();
    assert_eq!(counter.load(Ordering::SeqCst), 1);

    processor.stop();
    assert!(!processor.submit(Box::new(CountTask(Arc::clone(&counter)))));
    assert_eq!(counter.load(Ordering::SeqCst), 1);
    assert_eq!(processor.running_task_count(), 0);
}

/// C++: `dump_index_task_2_ut.cpp:63`
/// (`DumpMemIndexTaskTest2.DISABLED_SLOW_row_cnt_exceed_memory_quota`).
///
/// The mem-index quota is 1K. The insert path proposes one background dump
/// while two 8192-row appends are in flight; the task runs afterwards and
/// dumps both blocks as a single 16384-row chunk. The next append proposes
/// another dump, which starts at row 16384.
#[test]
#[ignore = "SLOW: the reference disables this test"]
fn dump_index_task_row_cnt_exceed_memory_quota() {
    let dir = tempdir().unwrap();
    let engine = Arc::new(Engine::open(dir.path()).unwrap());
    engine.set_memindex_quota(1024);
    secondary_fixture(&engine);

    // Attach before the writes so the quota proposes the real task. The
    // processor is started after both appends, which is the reference's
    // proposed-dump coalescing (`proposed_dump_size_`).
    let processor = Arc::new(BGTaskProcessor::new(1));
    engine.attach_bg_task_processor(Arc::clone(&processor));

    append_block(&engine, "db1.tb1");
    append_block(&engine, "db1.tb1");

    processor.start();
    engine.drain_background_tasks();
    assert_eq!(
        engine.secondary_chunk_meta("db1.tb1", "col1"),
        vec![(0, 8192 * 2)]
    );

    append_block(&engine, "db1.tb1");
    engine.drain_background_tasks();
    assert_eq!(
        engine.secondary_chunk_meta("db1.tb1", "col1"),
        vec![(0, 8192 * 2), (8192 * 2, 8192)]
    );

    engine.execute("DROP DATABASE db1;").unwrap();
    processor.stop();
}

/// C++: `dump_index_task_ut.cpp:63`
/// (`DumpMemIndexTaskTest.DISABLED_SLOW_row_cnt_exceed_memindex_capacity`).
///
/// `mem_index_capacity = 8192`: each append reaches the row bound and is
/// dumped at the end of its batch, so the second chunk starts at row 8192.
#[test]
#[ignore = "SLOW: the reference disables this test"]
fn dump_index_task_row_cnt_exceed_memindex_capacity() {
    let dir = tempdir().unwrap();
    let engine = Engine::open(dir.path()).unwrap();
    engine.set_memindex_capacity(8192);
    secondary_fixture(&engine);

    append_block(&engine, "db1.tb1");
    assert_eq!(
        engine.secondary_chunk_meta("db1.tb1", "col1"),
        vec![(0, 8192)]
    );

    append_block(&engine, "db1.tb1");
    assert_eq!(
        engine.secondary_chunk_meta("db1.tb1", "col1"),
        vec![(0, 8192), (8192, 8192)]
    );

    engine.execute("DROP DATABASE db1;").unwrap();
}
