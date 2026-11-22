// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
use crate::bg_task::{BGTaskProcessor, BgTask};
use crate::{Engine, QueryResult};
use parser::Literal;
use std::sync::atomic::{AtomicUsize, Ordering};
use std::sync::Arc;
use tempfile::tempdir;
async fn rows(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in engine.execute(sql).await.unwrap() {
        if let QueryResult::Rows { .. } = result {
            return result.rows_text();
        }
    }
    Vec::new()
}
/// One 8192-row block: `col1 = 1`, `col2 = abcdefghijklmnopqrstuvwxyz`
/// (`MakeInputBlock`).
async fn append_block(engine: &Engine, table: &str) {
    let row = [
        Literal::Int(1),
        Literal::Str("abcdefghijklmnopqrstuvwxyz".into()),
    ];
    engine
        .insert_literals(table, None, (0..8192).map(|_| row.to_vec()).collect())
        .await
        .unwrap();
}
/// C++: `cleanup_task_ut.cpp:57` (`test_delete_db_simple`).
///
/// After `DROP TABLE` + checkpoint + cleanup the dropped table's data files are
/// gone and every file of the surviving table is still there
/// (`BaseTest::CheckFilePaths`).
#[tokio::test]
async fn cleanup_removes_the_dropped_table_files() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .await
        .unwrap();
    e.execute("CREATE TABLE tb2 (col1 INT, col2 VARCHAR);")
        .await
        .unwrap();
    append_block(&e, "default_db.tb1").await;
    append_block(&e, "default_db.tb2").await;
    e.flush().await.unwrap();
    let exist_paths = e.table_files("default_db.tb1");
    let delete_paths = e.table_files("default_db.tb2");
    assert!(!exist_paths.is_empty(), "tb1 has no data files");
    assert!(!delete_paths.is_empty(), "tb2 has no data files");
    e.execute("DROP TABLE default_db.tb2;").await.unwrap();
    e.flush().await.unwrap();
    e.cleanup().await.unwrap();
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
#[tokio::test]
async fn background_cleanup_task_removes_dropped_files() {
    let dir = tempdir().unwrap();
    let engine = Arc::new(
        Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap(),
    );
    engine
        .execute("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .await
        .unwrap();
    engine
        .execute("CREATE TABLE tb2 (col1 INT, col2 VARCHAR);")
        .await
        .unwrap();
    append_block(&engine, "default_db.tb1").await;
    append_block(&engine, "default_db.tb2").await;
    engine.flush().await.unwrap();
    let exist_paths = engine.table_files("default_db.tb1");
    let delete_paths = engine.table_files("default_db.tb2");
    assert!(!exist_paths.is_empty(), "tb1 has no data files");
    assert!(!delete_paths.is_empty(), "tb2 has no data files");
    engine.execute("DROP TABLE default_db.tb2;").await.unwrap();
    engine.flush().await.unwrap();
    let processor = Arc::new(BGTaskProcessor::new(1));
    processor.start().await;
    engine.attach_bg_task_processor(Arc::clone(&processor));
    assert!(engine.submit_cleanup(), "cleanup was not queued");
    engine.drain_background_tasks().await;
    for path in &delete_paths {
        assert!(!path.exists(), "dropped table file survived: {path:?}");
    }
    for path in &exist_paths {
        assert!(path.exists(), "surviving table file was removed: {path:?}");
    }
    processor.stop().await;
}
/// `start_background_tasks` attaches a processor (so the memory-index quota
/// starts proposing dumps instead of dumping inline) and spawns the periodic
/// trigger thread. The intervals are zero here so no trigger fires and the test
/// does not depend on timing.
#[tokio::test]
async fn start_background_tasks_sets_processor() {
    let dir = tempdir().unwrap();
    let engine = Arc::new(
        Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap(),
    );
    assert!(!engine.submit_cleanup());
    engine
        .start_background_tasks(
            std::time::Duration::ZERO,
            std::time::Duration::ZERO,
            std::time::Duration::ZERO,
            std::time::Duration::ZERO,
        )
        .await;
    // `submit_cleanup` is the marker here, not `submit_optimize_indexes`: an
    // optimize submission now refuses to queue a pass when no index has anything
    // to fold (its cheap gate), so an empty catalog legitimately answers `false`
    // whether or not a processor is attached.
    assert!(engine.submit_cleanup(), "no processor attached");
    engine.drain_background_tasks().await;
    drop(engine);
}
/// C++: `compact_segments_task_ut.cpp:73` (`bg_compact`).
///
/// Eleven imports form eleven segments; compaction merges them (`segment_count`
/// drops from 11 to 1) without losing a row.
#[tokio::test]
async fn compact_reduces_the_segment_count() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .await
        .unwrap();
    for _ in 0..11 {
        append_block(&e, "default_db.tb1").await;
    }
    assert_eq!(e.row_count("default_db.tb1").unwrap(), 11 * 8192);
    let before = e.segment_count("default_db.tb1");
    assert_eq!(before, 11);
    e.compact_all_segments("default_db.tb1").await.unwrap();
    let after = e.segment_count("default_db.tb1");
    assert!(after < before);
    assert_eq!(after, 1);
    assert_eq!(e.row_count("default_db.tb1").unwrap(), 11 * 8192);
    let count: usize = rows(&e, "SELECT count(*) FROM default_db.tb1;").await[0][0]
        .parse()
        .unwrap();
    assert_eq!(count, 11 * 8192);
}
/// `{col1 INT, col2 VARCHAR}` plus a secondary index on `col1`
/// (`DumpMemIndexTaskTest` fixture).
async fn secondary_fixture(engine: &Engine) {
    engine.execute("CREATE DATABASE db1;").await.unwrap();
    engine
        .execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .await
        .unwrap();
    engine
        .execute("CREATE INDEX idx1 ON db1.tb1 (col1);")
        .await
        .unwrap();
}
struct CountTask(Arc<AtomicUsize>);
impl BgTask for CountTask {
    fn name(&self) -> &str {
        "CountTask"
    }
    fn run(self: Box<Self>) -> std::pin::Pin<Box<dyn std::future::Future<Output = ()> + Send>> {
        Box::pin(async move {
            self.0.fetch_add(1, Ordering::SeqCst);
        })
    }
}
/// C++: `background_process_ut.cpp:37` (`BGProcessTest.test1`).
///
/// Start the processor, run a submitted task to completion (the reference's
/// stop task is queued behind it), and stop.
#[tokio::test]
async fn background_process_start_stop() {
    let processor = Arc::new(BGTaskProcessor::new(2));
    processor.start().await;
    let counter = Arc::new(AtomicUsize::new(0));
    processor.submit(Box::new(CountTask(Arc::clone(&counter))));
    processor.drain().await;
    assert_eq!(counter.load(Ordering::SeqCst), 1);
    processor.stop().await;
    assert_eq!(processor.running_task_count(), 0);
}
/// A task that finishes only after a delay, so a test can tell "waited for it"
/// from "returned before it ran".
struct SlowTask(Arc<AtomicUsize>);
impl BgTask for SlowTask {
    fn name(&self) -> &str {
        "SlowTask"
    }
    fn run(self: Box<Self>) -> std::pin::Pin<Box<dyn std::future::Future<Output = ()> + Send>> {
        Box::pin(async move {
            tokio::time::sleep(std::time::Duration::from_millis(200)).await;
            self.0.fetch_add(1, Ordering::SeqCst);
        })
    }
}

struct PanickingTask;
impl BgTask for PanickingTask {
    fn name(&self) -> &str {
        "PanickingTask"
    }
    fn run(self: Box<Self>) -> std::pin::Pin<Box<dyn std::future::Future<Output = ()> + Send>> {
        Box::pin(async move {
            panic!("task failure");
        })
    }
}
/// A task that unwinds must not leave the running-task counter raised: the
/// counter is what `drain()` and `stop()` wait on, so a skipped decrement hangs
/// the shutdown of the process (the `PlaidIndex` `expect` in a background dump
/// is the reachable case).
#[tokio::test]
async fn a_panicking_task_does_not_wedge_drain() {
    let processor = Arc::new(BGTaskProcessor::new(1));
    processor.start().await;
    assert!(processor.submit(Box::new(PanickingTask)));
    // A task, not a thread: `drain` is awaited now, and parking a thread on it
    // is exactly what the panic-safety fix removed. `timeout` fails the test if
    // the counter is left raised.
    let draining = Arc::clone(&processor);
    tokio::time::timeout(
        std::time::Duration::from_secs(60),
        tokio::spawn(async move { draining.drain().await }),
    )
    .await
    .expect("drain() wedged on a panicking task")
    .expect("the drain task panicked");
    assert_eq!(processor.running_task_count(), 0);
    let counter = Arc::new(AtomicUsize::new(0));
    assert!(processor.submit(Box::new(CountTask(Arc::clone(&counter)))));
    processor.drain().await;
    assert_eq!(counter.load(Ordering::SeqCst), 1);
    processor.stop().await;
}
/// `Engine::close` is the wait that `Drop` cannot perform: it stops the
/// processor after the queued tasks have run, so a caller that needs the
/// background work finished has an explicit way to ask for it.
///
/// Dropping the engine instead closes the queue without waiting - the split
/// `BGTaskProcessor::drop` documents.
#[tokio::test]
async fn close_waits_for_the_queued_tasks() {
    let dir = tempdir().unwrap();
    let engine = Arc::new(
        Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap(),
    );
    engine.set_memindex_quota(1024);
    secondary_fixture(&engine).await;
    let processor = Arc::new(BGTaskProcessor::new(1));
    processor.start().await;
    engine.attach_bg_task_processor(Arc::clone(&processor));

    let counter = Arc::new(AtomicUsize::new(0));
    assert!(processor.submit(Box::new(SlowTask(Arc::clone(&counter)))));
    engine.close().await.unwrap();
    assert_eq!(
        counter.load(Ordering::SeqCst),
        1,
        "close() returned before the queued task ran"
    );
    assert_eq!(processor.running_task_count(), 0);
}

/// A submission made after `stop()` is reported instead of silently dropped,
/// so a caller that set a pending-work flag can clear it.
#[tokio::test]
async fn submit_after_stop_is_rejected() {
    let processor = Arc::new(BGTaskProcessor::new(1));
    processor.start().await;
    let counter = Arc::new(AtomicUsize::new(0));
    assert!(processor.submit(Box::new(CountTask(Arc::clone(&counter)))));
    processor.drain().await;
    assert_eq!(counter.load(Ordering::SeqCst), 1);
    processor.stop().await;
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
#[tokio::test]
#[ignore = "slow: C++ DumpMemIndexTaskTest2.DISABLED_SLOW_row_cnt_exceed_memory_quota"]
async fn dump_index_task_row_cnt_exceed_memory_quota() {
    let dir = tempdir().unwrap();
    let engine = Arc::new(
        Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap(),
    );
    engine.set_memindex_quota(1024);
    secondary_fixture(&engine).await;
    let processor = Arc::new(BGTaskProcessor::new(1));
    engine.attach_bg_task_processor(Arc::clone(&processor));
    append_block(&engine, "db1.tb1").await;
    append_block(&engine, "db1.tb1").await;
    processor.start().await;
    engine.drain_background_tasks().await;
    assert_eq!(
        engine.secondary_chunk_meta("db1.tb1", "col1"),
        vec![(0, 8192 * 2)]
    );
    append_block(&engine, "db1.tb1").await;
    engine.drain_background_tasks().await;
    assert_eq!(
        engine.secondary_chunk_meta("db1.tb1", "col1"),
        vec![(0, 8192 * 2), (8192 * 2, 8192)]
    );
    engine.execute("DROP DATABASE db1;").await.unwrap();
    processor.stop().await;
}
/// C++: `dump_index_task_ut.cpp:63`
/// (`DumpMemIndexTaskTest.DISABLED_SLOW_row_cnt_exceed_memindex_capacity`).
///
/// `mem_index_capacity = 8192`: each append reaches the row bound and is
/// dumped at the end of its batch, so the second chunk starts at row 8192.
#[tokio::test]
#[ignore = "slow: C++ DumpMemIndexTaskTest.DISABLED_SLOW_row_cnt_exceed_memindex_capacity"]
async fn dump_index_task_row_cnt_exceed_memindex_capacity() {
    let dir = tempdir().unwrap();
    let engine = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    engine.set_memindex_capacity(8192);
    secondary_fixture(&engine).await;
    append_block(&engine, "db1.tb1").await;
    assert_eq!(
        engine.secondary_chunk_meta("db1.tb1", "col1"),
        vec![(0, 8192)]
    );
    append_block(&engine, "db1.tb1").await;
    assert_eq!(
        engine.secondary_chunk_meta("db1.tb1", "col1"),
        vec![(0, 8192), (8192, 8192)]
    );
    engine.execute("DROP DATABASE db1;").await.unwrap();
}
