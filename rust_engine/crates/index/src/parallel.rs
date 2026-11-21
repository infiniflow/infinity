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

//! Bounded fan-out on **tokio**: one task per item, a `Semaphore` for the bound.
//!
//! Every parallel site in the engine is "N independent items, same function":
//! a checkpoint's tables, a rebuild's tables, a scan's blocks, a segment's term
//! batches, an HNSW build's buckets. This module is the one primitive they
//! share, and it is **async all the way down**: every entry point returns a
//! future, a waiting item yields its task, and nothing here parks a thread.
//!
//! There are exactly three shapes, and which one a call site uses is a
//! statement about its body:
//!
//! * [`for_each_async`] / [`map_async`] - the body is `async` and does its own
//!   waiting. This is the default.
//! * [`for_each_cpu_bound`] / [`map_cpu_bound`] - the body is a **pure-CPU
//!   leaf** (a block decode, a posting encode, a bucket of graph
//!   construction). It runs on the blocking pool, one `spawn_blocking` per
//!   item, because a multi-second leaf on a runtime worker stops that worker
//!   from polling every other task.
//! * a plain `spawn_blocking` in the caller - a single blocking call (one
//!   table's file writes, an `fsync`) that is not a fan-out.
//!
//! The distinction matters because the blocking pool is finite and shared: a
//! leaf that blocks on a lock, on I/O, or on another fan-out must not be put
//! there, and a pure-CPU leaf must be, or it occupies a worker instead.

use std::future::Future;
use std::sync::Arc;

use tokio::sync::Semaphore;
use tokio::task::JoinHandle;

/// Runs the **async** body `f` over `items` with at most `workers` in flight.
///
/// Every item is attempted even after one fails - a checkpoint must not leave
/// a table half-written - and the first error is returned once all are done.
pub async fn for_each_async<T, F, Fut>(items: Vec<T>, workers: usize, f: F) -> common::Result<()>
where
    T: Send + 'static,
    F: Fn(T) -> Fut + Send + Sync + 'static,
    Fut: Future<Output = common::Result<()>> + Send + 'static,
{
    map_async(items, workers, f).await.map(|_| ())
}

/// [`for_each_async`] that collects the values, **in input order**.
///
/// One tokio task per item; a `Semaphore` permit decides how many run at once.
/// A body that finishes early does not reorder the results, because the tasks
/// are joined in submission order.
pub async fn map_async<T, U, F, Fut>(items: Vec<T>, workers: usize, f: F) -> common::Result<Vec<U>>
where
    T: Send + 'static,
    U: Send + 'static,
    F: Fn(T) -> Fut + Send + Sync + 'static,
    Fut: Future<Output = common::Result<U>> + Send + 'static,
{
    if items.is_empty() {
        return Ok(Vec::new());
    }
    let workers = workers.clamp(1, items.len());
    let f = Arc::new(f);
    let gate = Arc::new(Semaphore::new(workers));
    let mut tasks: Vec<JoinHandle<common::Result<U>>> = Vec::with_capacity(items.len());
    for item in items {
        let f = Arc::clone(&f);
        let gate = Arc::clone(&gate);
        tasks.push(tokio::spawn(async move {
            let _permit = gate.acquire_owned().await.expect("the gate stays open");
            f(item).await
        }));
    }
    join_in_order(tasks).await
}

/// Runs the **pure-CPU** body `f` over `items`, one `spawn_blocking` per item.
///
/// Every item is attempted even after one fails, and the first error is
/// returned once all are done.
pub async fn for_each_cpu_bound<T, F>(items: Vec<T>, workers: usize, f: F) -> common::Result<()>
where
    T: Send + 'static,
    F: Fn(T) -> common::Result<()> + Send + Sync + 'static,
{
    map_cpu_bound(items, workers, f).await.map(|_| ())
}

/// [`for_each_cpu_bound`] that collects the values, **in input order**.
///
/// The body must be a **pure-CPU leaf**: it may not take a lock, do I/O, or
/// await anything. It runs on the blocking pool, which is finite and shared
/// with every other blocking call in the process - a leaf that waits there
/// holds a thread that a waiting unit elsewhere needs, which is how a fan-out
/// underneath it starved.
pub async fn map_cpu_bound<T, U, F>(items: Vec<T>, workers: usize, f: F) -> common::Result<Vec<U>>
where
    T: Send + 'static,
    U: Send + 'static,
    F: Fn(T) -> common::Result<U> + Send + Sync + 'static,
{
    if items.is_empty() {
        return Ok(Vec::new());
    }
    let workers = workers.clamp(1, items.len());
    let f = Arc::new(f);
    let gate = Arc::new(Semaphore::new(workers));
    let mut tasks: Vec<JoinHandle<common::Result<U>>> = Vec::with_capacity(items.len());
    for item in items {
        let f = Arc::clone(&f);
        let gate = Arc::clone(&gate);
        tasks.push(tokio::spawn(async move {
            let permit = gate.acquire_owned().await.expect("the gate stays open");
            tokio::task::spawn_blocking(move || {
                // Held for exactly as long as the body runs, so the gate bounds
                // bodies, not queued entries.
                let _permit = permit;
                f(item)
            })
            .await
            .map_err(|e| common::Error::Invalid(format!("a fan-out body was lost: {e}")))?
        }));
    }
    join_in_order(tasks).await
}

/// Joins every task in submission order, so the results keep the input order.
///
/// A dropped handle would cancel its item and leave that table (or segment)
/// half-written, so every one is awaited even after a failure.
async fn join_in_order<U>(tasks: Vec<JoinHandle<common::Result<U>>>) -> common::Result<Vec<U>> {
    let mut out = Vec::with_capacity(tasks.len());
    let mut failure: Option<common::Error> = None;
    for task in tasks {
        match task.await {
            Ok(Ok(value)) => out.push(value),
            Ok(Err(e)) => {
                if failure.is_none() {
                    failure = Some(e);
                }
            }
            Err(e) => {
                if failure.is_none() {
                    failure = Some(common::Error::Invalid(format!(
                        "a fan-out task did not finish: {e}"
                    )));
                }
            }
        }
    }
    match failure {
        Some(e) => Err(e),
        None => Ok(out),
    }
}

/// The process's runtime, recorded for the threads that have none entered.
///
/// The engine installs its scheduler's runtime here when it starts, which is
/// the only runtime the process runs: a fan-out on a thread whose context is
/// empty (the checkpoint trigger thread, a synchronous `Engine::execute_blocking`,
/// a blocking-pool body) spawns there instead of on a pool of its own.
#[cfg(test)]
mod tests {
    use super::*;
    use std::sync::atomic::{AtomicUsize, Ordering};

    #[tokio::test(flavor = "multi_thread")]
    async fn map_keeps_input_order() {
        let items: Vec<usize> = (0..32).collect();
        let out = map_async(items.clone(), 4, |item| async move { Ok(item * 10) })
            .await
            .unwrap();
        assert_eq!(out, items.iter().map(|item| item * 10).collect::<Vec<_>>());
    }

    #[tokio::test(flavor = "multi_thread")]
    async fn the_bound_is_the_callers() {
        let items: Vec<usize> = (0..16).collect();
        let live = Arc::new(AtomicUsize::new(0));
        let peak = Arc::new(AtomicUsize::new(0));
        let (l, p) = (Arc::clone(&live), Arc::clone(&peak));
        map_cpu_bound(items, 3, move |_| {
            let now = l.fetch_add(1, Ordering::SeqCst) + 1;
            p.fetch_max(now, Ordering::SeqCst);
            std::thread::sleep(std::time::Duration::from_millis(4));
            l.fetch_sub(1, Ordering::SeqCst);
            Ok(())
        })
        .await
        .unwrap();
        // Both bounds matter: `<= 3` is the callers' limit, and `== 3` is what
        // makes it a *parallel* fan-out. A body that has to run alone would
        // also satisfy the first, and did: the serial-store HNSW build passed
        // this test on one core.
        assert_eq!(
            peak.load(Ordering::SeqCst),
            3,
            "the fan-out must actually reach the caller's bound"
        );
    }

    #[tokio::test(flavor = "multi_thread")]
    async fn a_failure_is_reported_after_every_item_ran() {
        let items: Vec<usize> = (0..16).collect();
        let attempted = Arc::new(AtomicUsize::new(0));
        let seen = Arc::clone(&attempted);
        let result = map_async(items, 4, move |item| {
            let seen = Arc::clone(&seen);
            async move {
                seen.fetch_add(1, Ordering::SeqCst);
                if item == 5 {
                    return Err(common::Error::Invalid("boom".into()));
                }
                Ok(item)
            }
        })
        .await;
        assert!(result.is_err(), "the failure must reach the caller");
        assert_eq!(attempted.load(Ordering::SeqCst), 16);
    }
}
