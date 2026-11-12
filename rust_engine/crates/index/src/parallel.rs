//! Bounded fan-out on **tokio**: one task per item, a `Semaphore` for the bound.
//!
//! Every parallel site in the engine is "N independent items, same function":
//! a checkpoint's tables, a rebuild's tables, a scan's blocks, a segment's term
//! batches, an HNSW build's buckets. This module is the one primitive they
//! share. It creates no threads and owns no pool: the bound is the caller's
//! `workers`, enforced by permits, and a fan-out inside a fan-out just takes
//! permits from its own gate instead of starving a second scheduler.
//!
//! The item handed to [`for_each_blocking`] / [`map_blocking`] must already be
//! **one blocking unit** - the caller has split its work until what is left is
//! either a pure-CPU leaf (a block decode, a posting encode, a bucket of graph
//! construction) or a single blocking call (one table's file writes). The item
//! then runs on the blocking pool, and only that item does. A mixed workload is
//! never moved across a thread boundary as a whole.

use std::future::Future;
use std::sync::Arc;

use tokio::runtime::Handle;
use tokio::sync::Semaphore;
use tokio::task::JoinHandle;

/// The process-wide runtime a fan-out spawns on when the calling thread has
/// none entered (the checkpoint trigger thread, a `block_on` adapter, a CLI
/// helper). The engine installs its own runtime here at startup.
static FAN_OUT_RUNTIME: std::sync::OnceLock<Handle> = std::sync::OnceLock::new();

/// Records the runtime every fan-out falls back to.
///
/// Called once by the engine's scheduler when its runtime starts. A fan-out on
/// a thread with an entered runtime uses that one; a thread without one (the
/// periodic checkpoint trigger, a synchronous `Engine::execute_blocking`) uses
/// this, so the work still runs in parallel instead of collapsing to the
/// calling thread.
pub fn install_runtime(handle: Handle) {
    let _ = FAN_OUT_RUNTIME.set(handle);
}

/// The runtime to spawn on: the caller's when it is multi-threaded, else the
/// installed one, else a lazily created fallback (see [`fallback_runtime`]).
fn spawn_runtime() -> Option<Handle> {
    if let Ok(handle) = Handle::try_current() {
        if handle.runtime_flavor() == tokio::runtime::RuntimeFlavor::MultiThread {
            return Some(handle);
        }
    }
    FAN_OUT_RUNTIME
        .get()
        .cloned()
        .or_else(|| Some(fallback_runtime().handle().clone()))
}

/// The runtime for callers that have none: a benchmark, an example, a unit
/// test of this crate, `cargo run -p cli -- conformance` without the server.
///
/// It is created once and lives for the process, so the cost is paid once and
/// a fan-out there is still real parallelism instead of a silent serial pass.
fn fallback_runtime() -> &'static tokio::runtime::Runtime {
    static FALLBACK: std::sync::OnceLock<tokio::runtime::Runtime> = std::sync::OnceLock::new();
    FALLBACK.get_or_init(|| {
        tokio::runtime::Builder::new_multi_thread()
            .worker_threads(
                std::thread::available_parallelism()
                    .map(|n| n.get())
                    .unwrap_or(1),
            )
            .thread_name("fan-out")
            .enable_all()
            .build()
            .expect("build the fan-out runtime")
    })
}

/// Whether this thread is **already** running a fan-out body, or is about to
/// park waiting for one.
///
/// Two shapes count, and both must avoid a second *blocking* layer:
///
/// * a blocking-pool thread: tokio gives it the runtime handle but no task id
///   (an id exists only while a task is being polled), so "a handle and no
///   task" is exactly "inside `spawn_blocking`";
/// * a thread with **no runtime context at all** - the synchronous adapters
///   (`scheduler::block_on`, `Engine::execute_blocking`), the CLI, a unit
///   test. `drive_on` parks such a thread until the fan-out finishes, and it
///   may well be a blocking-pool thread that reached the sync adapter from
///   inside a `spawn_blocking` body. Handing its items to `spawn_blocking`
///   as well starves the pool that the parked thread occupies.
///
/// A nested fan-out is real (a checkpoint's per-table body rebuilds an HNSW
/// chunk, which builds its buckets in parallel; a SELECT dispatched to the
/// blocking pool decodes its blocks in parallel). It must not spawn a second
/// layer of *blocking* bodies: the outer layer's threads would sit waiting for
/// permits that only their own children can release, and with
/// `tables x buckets` above the pool's thread cap that is a deadlock. The
/// nested call therefore uses plain tasks on the runtime - whose workers are
/// free, because the outer layer is parked on blocking threads.
fn inside_a_blocking_body() -> bool {
    match Handle::try_current() {
        Ok(_) => tokio::task::try_id().is_none(),
        Err(_) => true,
    }
}

/// Runs the **async** body `f` over `items` with at most `workers` in flight.
///
/// This is the shape for a unit whose own work is async: a table's checkpoint
/// waits for its bottom gate (`.await`) and only its file writes go to the
/// blocking pool **inside the body**. Nothing here moves a mixed workload
/// across a thread boundary.
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
    let mut out = Vec::with_capacity(tasks.len());
    let mut failure: Option<common::Error> = None;
    // Every handle is awaited even after a failure: a dropped one would cancel
    // its item, leaving that table (or segment) half-written.
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

/// Runs the blocking unit `f` over `items` with at most `workers` in flight.
///
/// Every item is attempted even after one fails - a checkpoint must not leave
/// a table half-written - and the first error is returned once all are done.
pub fn for_each_blocking<T, F>(items: Vec<T>, workers: usize, f: F) -> common::Result<()>
where
    T: Send + 'static,
    F: Fn(T) -> common::Result<()> + Send + Sync + 'static,
{
    map_blocking(items, workers, f).map(|_| ())
}

/// [`for_each_blocking`] that collects the values, **in input order**.
///
/// One tokio task per item; a permit decides how many are in flight; the body
/// runs on the blocking pool while the permit is held. Tasks are joined in
/// submission order, so the result order is the input order.
pub fn map_blocking<T, U, F>(items: Vec<T>, workers: usize, f: F) -> common::Result<Vec<U>>
where
    T: Send + 'static,
    U: Send + 'static,
    F: Fn(T) -> common::Result<U> + Send + Sync + 'static,
{
    if items.is_empty() {
        return Ok(Vec::new());
    }
    let workers = workers.clamp(1, items.len());
    // No runtime anywhere (a standalone test, an example): there is nothing to
    // spawn on, so the items run here in order. A `current_thread` caller is
    // treated the same way only when nothing was installed - spawning onto that
    // runtime would deadlock, because the single thread is the one waiting.
    let Some(runtime) = spawn_runtime() else {
        // Same contract serially: every item runs, the first error is what the
        // caller gets. Stopping at the first failure would leave the tables
        // after it unwritten.
        let mut results = Vec::with_capacity(items.len());
        let mut failure: Option<common::Error> = None;
        for item in items {
            match f(item) {
                Ok(value) => results.push(value),
                Err(e) => {
                    if failure.is_none() {
                        failure = Some(e);
                    }
                }
            }
        }
        return match failure {
            Some(e) => Err(e),
            None => Ok(results),
        };
    };
    // Dispatch shapes, in order of preference:
    // * on a runtime worker - a task per item, blocking body on the pool;
    // * inside another fan-out's body - a task per item, body on the runtime
    //   (see `inside_a_blocking_body`: a second blocking layer can deadlock);
    // * nowhere - this thread, one item after another.
    let nested = inside_a_blocking_body();
    let f = Arc::new(f);
    let count = items.len();
    let tasks_runtime = runtime.clone();
    let (results, failure): (Vec<Option<U>>, Option<common::Error>) =
        drive_on(&runtime, async move {
            let mut results: Vec<Option<U>> = (0..count).map(|_| None).collect();
            let mut failure: Option<common::Error> = None;
            let gate = Arc::new(Semaphore::new(workers));
            let mut tasks: Vec<JoinHandle<common::Result<(usize, U)>>> =
                Vec::with_capacity(items.len());
            for (index, item) in items.into_iter().enumerate() {
                let f = Arc::clone(&f);
                let gate = Arc::clone(&gate);
                tasks.push(tasks_runtime.spawn(async move {
                    let permit = gate.acquire_owned().await.expect("the gate stays open");
                    let value = if nested {
                        // No second blocking layer: the runtime worker runs the
                        // body, and the permit keeps the bound.
                        let _permit = permit;
                        f(item)?
                    } else {
                        tokio::task::spawn_blocking(move || {
                            // Held for exactly as long as the body runs, so the
                            // gate bounds bodies, not queued entries.
                            let _permit = permit;
                            f(item)
                        })
                        .await
                        .map_err(|e| {
                            common::Error::Invalid(format!("a fan-out body was lost: {e}"))
                        })??
                    };
                    Ok((index, value))
                }));
            }
            // Every handle is awaited even after a failure: a dropped one would
            // cancel its item, leaving that table (or segment) half-written.
            for task in tasks {
                match task.await {
                    Ok(Ok((index, value))) => results[index] = Some(value),
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
            (results, failure)
        });
    match failure {
        Some(e) => Err(e),
        None => Ok(results
            .into_iter()
            .map(|value| value.expect("every item succeeded, so every slot is filled"))
            .collect()),
    }
}

/// Runs `future` to completion on `runtime`'s behalf from a synchronous
/// caller.
///
/// On a worker of a multi-threaded runtime the core is handed to a replacement
/// (`block_in_place`) and that runtime drives the future, which is what keeps
/// the spawned tasks progressing. Any other caller (the checkpoint trigger
/// thread, a `block_on` adapter, the CLI) has no context of its own: it enters
/// `runtime` so spawned tasks land there, then parks until they finish.
fn drive_on<F: Future>(runtime: &Handle, future: F) -> F::Output {
    if let Ok(current) = Handle::try_current() {
        if current.runtime_flavor() == tokio::runtime::RuntimeFlavor::MultiThread {
            return tokio::task::block_in_place(|| current.block_on(future));
        }
    }
    let _entered = runtime.enter();
    park_block_on(future)
}

/// Runs `future` to completion from a synchronous caller.
///
/// On a multi-threaded runtime worker this is `block_in_place` + `Handle::
/// block_on`: the worker hands its core to a replacement first, so the tasks
/// the fan-out spawns keep being polled while this thread waits. Off the
/// runtime (a blocking-pool thread, a unit test, the `cli`) it is a plain
/// park/unpark loop - `futures_executor::block_on` cannot nest and the engine
/// nests by construction, while "poll, and if pending, sleep until woken" has
/// no such restriction.
pub fn block_on<F: Future>(future: F) -> F::Output {
    use tokio::runtime::{Handle, RuntimeFlavor};

    if let Ok(handle) = Handle::try_current() {
        if handle.runtime_flavor() == RuntimeFlavor::MultiThread {
            // On a worker this hands the core to a replacement so the spawned
            // tasks keep being polled; on a blocking-pool thread it is a plain
            // call and the park below is what waits.
            return tokio::task::block_in_place(|| handle.block_on(future));
        }
    }
    park_block_on(future)
}

/// Polls `future` on this thread until it completes.
fn park_block_on<F: Future>(future: F) -> F::Output {
    use std::sync::atomic::{AtomicBool, Ordering};
    use std::task::{Context, Poll, Wake, Waker};

    struct ThreadWaker {
        thread: std::thread::Thread,
        woken: AtomicBool,
    }
    impl Wake for ThreadWaker {
        fn wake(self: std::sync::Arc<Self>) {
            self.woken.store(true, Ordering::SeqCst);
            self.thread.unpark();
        }
        fn wake_by_ref(self: &std::sync::Arc<Self>) {
            self.woken.store(true, Ordering::SeqCst);
            self.thread.unpark();
        }
    }

    let waker = std::sync::Arc::new(ThreadWaker {
        thread: std::thread::current(),
        woken: AtomicBool::new(false),
    });
    let waker = Waker::from(waker);
    let mut context = Context::from_waker(&waker);
    let mut future = std::pin::pin!(future);
    loop {
        match future.as_mut().poll(&mut context) {
            Poll::Ready(value) => return value,
            Poll::Pending => std::thread::park(),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::sync::atomic::{AtomicUsize, Ordering};

    /// Every item runs, results come back in input order.
    #[test]
    fn map_keeps_input_order() {
        let items: Vec<usize> = (0..32).collect();
        let out = map_blocking(items.clone(), 4, |item| Ok(item * 10)).unwrap();
        assert_eq!(out, items.iter().map(|item| item * 10).collect::<Vec<_>>());
    }

    /// The bound is the caller's, not the machine's: `workers` permits, so at
    /// most that many bodies are in flight.
    #[test]
    fn the_bound_is_the_callers() {
        let items: Vec<usize> = (0..16).collect();
        let live = Arc::new(AtomicUsize::new(0));
        let peak = Arc::new(AtomicUsize::new(0));
        let (l, p) = (Arc::clone(&live), Arc::clone(&peak));
        for_each_blocking(items, 3, move |_| {
            let now = l.fetch_add(1, Ordering::SeqCst) + 1;
            p.fetch_max(now, Ordering::SeqCst);
            std::thread::sleep(std::time::Duration::from_millis(4));
            l.fetch_sub(1, Ordering::SeqCst);
            Ok(())
        })
        .unwrap();
        // Without a runtime the pass is serial, which is within the bound too.
        assert!(peak.load(Ordering::SeqCst) <= 3);
    }

    /// A failure reaches the caller, and every item still ran (a checkpoint
    /// must not leave the tables after the failing one unwritten).
    #[test]
    fn a_failure_is_reported_after_every_item_ran() {
        let items: Vec<usize> = (0..16).collect();
        let attempted = Arc::new(AtomicUsize::new(0));
        let seen = Arc::clone(&attempted);
        let result = for_each_blocking(items, 4, move |item| {
            seen.fetch_add(1, Ordering::SeqCst);
            if item == 5 {
                return Err(common::Error::Invalid("boom".into()));
            }
            Ok(())
        });
        assert!(result.is_err(), "the failure must reach the caller");
        assert_eq!(attempted.load(Ordering::SeqCst), 16);
    }

    /// On a runtime the bodies really are tasks: they run, bounded, while the
    /// caller waits, and `block_on` hands the worker's core away.
    #[test]
    fn a_runtime_runs_the_items_on_tasks() {
        let runtime = tokio::runtime::Builder::new_multi_thread()
            .worker_threads(1)
            .enable_all()
            .build()
            .unwrap();
        let out = runtime.block_on(async {
            let items: Vec<u64> = (0..8).collect();
            map_blocking(items, 3, |item| Ok(item * 2)).unwrap()
        });
        assert_eq!(out, (0..8u64).map(|i| i * 2).collect::<Vec<_>>());
    }
}
