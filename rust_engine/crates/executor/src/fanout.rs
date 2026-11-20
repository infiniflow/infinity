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

//! The engine's entry points into [`index::parallel`].
//!
//! The fan-out primitive spawns its tasks on the caller's runtime, or on the
//! engine's when the calling thread has none entered. The engine's runtime is
//! [`crate::scheduler::scheduler`], created and installed by `Engine::open`
//! before any engine work runs, so a fan-out on a thread with no runtime of its
//! own (a synchronous `Engine::execute_blocking`, a blocking-pool body) still
//! lands on the engine's pool instead of collapsing to the caller's thread.
//!
//! `index::parallel` builds no runtime of its own: outside the engine, the
//! caller brings one.
//!
//! On a serving path the scheduler already exists, so this costs one atomic
//! load.

/// Runs the **async** body `f` over `items` with at most `workers` in flight.
///
/// The body's waits stay `.await`s; whatever it hands to the blocking pool it
/// does so itself, per leaf.
pub(crate) async fn for_each_async<T, F, Fut>(
    items: Vec<T>,
    workers: usize,
    f: F,
) -> common::Result<()>
where
    T: Send + 'static,
    F: Fn(T) -> Fut + Send + Sync + 'static,
    Fut: std::future::Future<Output = common::Result<()>> + Send + 'static,
{
    let _ = crate::scheduler::scheduler();
    index::parallel::for_each_async(items, workers, f).await
}

/// Whether the caller is a **runtime worker** (driving tasks), as opposed to a
/// blocking-pool thread or a plain thread.
///
/// A synchronous wait from a worker is what deadlocks: parking the worker
/// leaves the tasks it spawned without one to poll them, and if every worker
/// is parked the same way none is left. Callers that can also run inline (a
/// block decode) should skip the fan-out while this is true.
pub(crate) fn on_a_runtime_worker() -> bool {
    tokio::runtime::Handle::try_current()
        .map(|handle| {
            handle.runtime_flavor() == tokio::runtime::RuntimeFlavor::MultiThread
                && tokio::task::try_id().is_some()
        })
        .unwrap_or(false)
}

/// The engine's parallel map for items whose body is **pure CPU**.
///
/// The body must not wait on anything - no lock, no I/O, no `block_on`. It runs
/// on the CPU-bound dispatch, which is why such a leaf can never be what fills
/// the blocking pool a parked scan needs a thread from.
pub(crate) async fn map_cpu_bound<T, U, F>(
    items: Vec<T>,
    workers: usize,
    f: F,
) -> common::Result<Vec<U>>
where
    T: Send + 'static,
    U: Send + 'static,
    F: Fn(T) -> common::Result<U> + Send + Sync + 'static,
{
    let _ = crate::scheduler::scheduler();
    index::parallel::map_cpu_bound(items, workers, f).await
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::sync::Mutex;

    /// A fan-out builds **no** runtime of its own.
    ///
    /// It spawns on the calling thread's runtime (or, when the thread has none,
    /// on the engine's `task-scheduler`) - the deleted fallback used to build a
    /// separate `fan-out` pool, and a body carrying that name means a second
    /// runtime came back. The check is the absence of that pool, not the exact
    /// worker name: any runtime the caller brings is a legitimate home.
    #[tokio::test]
    async fn a_fan_out_never_builds_a_pool_of_its_own() {
        let dir = tempfile::tempdir().unwrap();
        let _engine = crate::Engine::open_async(dir.path(), Default::default())
            .await
            .unwrap();
        let seen: std::sync::Arc<Mutex<Vec<String>>> = Default::default();
        let record = std::sync::Arc::clone(&seen);
        map_cpu_bound((0..8).collect(), 4, move |item| {
            record.lock().unwrap().push(
                std::thread::current()
                    .name()
                    .unwrap_or("<unnamed>")
                    .to_string(),
            );
            Ok(item)
        })
        .await
        .unwrap();
        let names = seen.lock().unwrap().clone();
        assert_eq!(names.len(), 8, "every item runs");
        for name in &names {
            assert!(
                !name.starts_with("fan-out"),
                "a fan-out body ran on '{name}', i.e. a pool of its own: {names:?}"
            );
        }
    }
}
