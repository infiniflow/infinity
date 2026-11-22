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

//! Restart / checkpoint ("compaction") invariants (plan §6 Gate C).
//!
//! A checkpoint truncates the WAL once every mutation is durable in the KV
//! base. Recovery must then still reconstruct the full state, and mutations
//! made after recovery must themselves survive a further restart.

use std::sync::atomic::{AtomicBool, AtomicI64, Ordering};
use std::sync::Arc;

use executor::{Engine, QueryResult};
use storage::WalManager;

async fn count(e: &Engine) -> i64 {
    let rs = e.execute("SELECT COUNT(*) FROM t;").await.unwrap();
    match &rs[0] {
        QueryResult::Rows { rows, .. } => rows.row(0).unwrap()[0].to_slt_string().parse().unwrap(),
        other => panic!("expected rows, got {other:?}"),
    }
}

async fn open(dir: &std::path::Path) -> Engine {
    Engine::open_async(dir, executor::EngineOptions::default())
        .await
        .unwrap()
}

#[tokio::test]
async fn state_survives_restart_after_wal_truncation() {
    let dir = tempfile::tempdir().unwrap();
    {
        let e = open(dir.path()).await;
        e.execute("CREATE TABLE t (id INTEGER);").await.unwrap();
        e.execute("INSERT INTO t VALUES (1), (2);").await.unwrap();
        e.flush().await.unwrap();
    }
    // checkpoint: drop every WAL record (all durable in the KV base already)
    WalManager::truncate_up_to_ts(dir.path().join("wal").join("wal.log"), u64::MAX)
        .await
        .unwrap();

    // restart 1: the KV base alone reconstructs the state
    {
        let e = open(dir.path()).await;
        assert_eq!(count(&e).await, 2, "state lost after WAL truncation");
        // a mutation after recovery ...
        e.execute("INSERT INTO t VALUES (3);").await.unwrap();
        e.flush().await.unwrap();
    }
    // ... survives another restart
    {
        let e = open(dir.path()).await;
        assert_eq!(count(&e).await, 3, "post-recovery mutation lost");
    }
}

#[tokio::test]
async fn compaction_survives_wal_replay() {
    // Compaction renumbers rows. The renumbering must be WAL-visible: replay
    // used to re-create the old positions and resurrect rows deleted before
    // the compact.
    let dir = tempfile::tempdir().unwrap();
    {
        let e = open(dir.path()).await;
        e.execute("CREATE TABLE t (id INTEGER);").await.unwrap();
        e.execute("INSERT INTO t VALUES (1),(2),(3),(4),(5),(6),(7),(8),(9),(10);")
            .await
            .unwrap();
        e.execute("DELETE FROM t WHERE id = 2;").await.unwrap();
        e.execute("DELETE FROM t WHERE id = 7;").await.unwrap();
        e.execute("COMPACT TABLE t;").await.unwrap();
        e.flush().await.unwrap();
        assert_eq!(count(&e).await, 8);
    }
    let e = open(dir.path()).await;
    assert_eq!(count(&e).await, 8, "replay resurrected rows compacted away");
}

#[tokio::test]
async fn deleting_the_wal_file_after_checkpoint_keeps_state() {
    let dir = tempfile::tempdir().unwrap();
    {
        let e = open(dir.path()).await;
        e.execute("CREATE TABLE t (id INTEGER);").await.unwrap();
        e.execute("INSERT INTO t VALUES (7);").await.unwrap();
        e.flush().await.unwrap();
    }
    std::fs::remove_file(dir.path().join("wal").join("wal.log")).unwrap();
    let e = open(dir.path()).await;
    assert_eq!(
        count(&e).await,
        1,
        "state lost when the compacted WAL is gone"
    );
}

/// The `restart_test` shape: one directory, iteration after iteration. Each
/// round inserts 8192 rows per statement on one connection while a checkpoint
/// runs under it, then the process is "killed" (dropped without `close`), and
/// the next round must see every row a committed `INSERT` returned.
///
/// The two floors a reopen derives are not the same: the checkpoint frame
/// covers what the KV store holds (`synced`), the block mirror covers the rows
/// of a **bounded** range. A commit that lands while the mirror runs is in the
/// first and in no block, so its rows are only in the WAL - and a replay that
/// skips the segment because the frame covers it loses them. This is
/// `test_insert.py::test_insert_checkpoint`'s failure: `count(*)` one 8192-row
/// batch short after a restart (`163840 == 172032`).
#[tokio::test(flavor = "multi_thread")]
async fn a_crash_after_a_racing_checkpoint_keeps_every_committed_row() {
    let dir = tempfile::tempdir().unwrap();
    {
        let e = open(dir.path()).await;
        e.execute("CREATE TABLE t (c1 INTEGER);").await.unwrap();
    }
    let mut total = 0i64;
    for round in 0..6u64 {
        let e = Arc::new(open(dir.path()).await);
        assert_eq!(
            count(&e).await,
            total,
            "round {round}: rows lost across the restart"
        );
        let inserted = Arc::new(AtomicI64::new(0));
        {
            let stop = Arc::new(AtomicBool::new(false));
            let writer = {
                let e = Arc::clone(&e);
                let inserted = Arc::clone(&inserted);
                let stop = Arc::clone(&stop);
                tokio::spawn(async move {
                    let mut next = 0i64;
                    while !stop.load(Ordering::Relaxed) {
                        let values: Vec<String> =
                            (next..next + 8192).map(|i| format!("({i})")).collect();
                        if e.execute(&format!("INSERT INTO t VALUES {};", values.join(",")))
                            .await
                            .is_ok()
                        {
                            next += 8192;
                            inserted.fetch_add(8192, Ordering::Relaxed);
                        } else {
                            break;
                        }
                    }
                })
            };
            // Let a few batches land, then checkpoint underneath the writer -
            // `flush_data()` on the harness's second connection. The delay walks
            // so the checkpoint lands at a different point of a batch in every
            // round.
            tokio::time::sleep(std::time::Duration::from_millis(150 + 7 * round)).await;
            e.flush().await.unwrap();
            stop.store(true, Ordering::Relaxed);
            writer.await.unwrap();
        }
        let inserted = inserted.load(Ordering::Relaxed);
        assert_eq!(
            count(&e).await,
            total + inserted,
            "round {round}: rows lost by the checkpoint itself"
        );
        total += inserted;
        drop(e); // the kill: no checkpoint, no drain
    }
}
