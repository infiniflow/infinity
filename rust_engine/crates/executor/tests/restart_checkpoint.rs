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
