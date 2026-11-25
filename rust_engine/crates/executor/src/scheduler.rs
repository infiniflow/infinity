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

//! C++ `TaskScheduler` (`task_scheduler_impl.cpp:60-140`), built on a tokio
//! runtime: a resident pool that runs every fragment's tasklets and the sink
//! task that merges them.
//!
//! The reference keeps `hardware_concurrency()` worker threads for the life of
//! the storage, one task queue each, and schedules a fragment's tasks onto
//! them; a statement does not create threads of its own. This module keeps the
//! same shape with tokio: one multi-threaded runtime per process, sized like the
//! reference, and:
//!
//! * a tasklet waits on its exchange queue for the sink to drain it, so it is
//!   an **async task** on the runtime, not a blocking-pool job - a blocked
//!   tasklet costs a future, not a thread (plan 6.12.4 step 3);
//! * the sink drains those queues in order, so it runs in place on the
//!   statement's thread (`block_in_place`), which is what `QueryContext`
//!   waiting for the fragment looks like here.

use std::sync::atomic::{AtomicUsize, Ordering};
use std::sync::OnceLock;

#[cfg(target_os = "linux")]
fn parse_cpu_list(path: &str) -> Option<Vec<usize>> {
    let text = std::fs::read_to_string(path).ok()?;
    let mut cpus = Vec::new();
    for part in text.trim().split(',') {
        let part = part.trim();
        if part.is_empty() {
            continue;
        }
        if let Some((start, end)) = part.split_once('-') {
            let s: usize = start.trim().parse().ok()?;
            let e: usize = end.trim().parse().ok()?;
            if s <= e {
                cpus.extend(s..=e);
            }
        } else {
            cpus.push(part.parse().ok()?);
        }
    }
    (!cpus.is_empty()).then_some(cpus)
}

#[cfg(not(target_os = "linux"))]
fn parse_cpu_list(_path: &str) -> Option<Vec<usize>> {
    None
}

type Job = Box<dyn FnOnce() + Send + 'static>;

pub struct TaskScheduler {
    runtime: tokio::runtime::Runtime,
}

impl TaskScheduler {
    fn start() -> Self {
        let total_cpus = std::thread::available_parallelism()
            .map(|n| n.get())
            .unwrap_or(1);
        let pin_enabled = std::env::var("INFINITY_PIN_PCORES")
            .map(|v| v.trim() != "0")
            .unwrap_or(true);
        let p_cores = if pin_enabled {
            parse_cpu_list("/sys/devices/cpu_core/cpus").filter(|cpus| cpus.len() < total_cpus)
        } else {
            None
        };
        // `TaskScheduler::Init`: `min(hardware_concurrency, CPULimit)` workers.
        // On hybrid P/E-core CPUs, default worker count to the P-core count so
        // async query workers stay on P-cores instead of spilling to E-cores.
        let workers = std::env::var("INFINITY_WORKER_THREADS")
            .ok()
            .and_then(|v| v.trim().parse::<usize>().ok())
            .filter(|n| *n > 0)
            .unwrap_or_else(|| p_cores.as_ref().map_or(total_cpus, Vec::len));
        // The blocking pool is left at tokio's default. It is not a concurrency
        // limit for the engine: tasklets are async tasks now, so the pool only
        // holds work that genuinely blocks. A cap of `cores + 4` made the pool
        // the bottleneck for work that was already bounded by the cores.
        //
        // What still legitimately blocks is small and bounded: the bottom
        // drain per table, and the fragment sink that pulls from every tasklet
        // in order.
        //
        // `INFINITY_BLOCKING_THREADS` remains as an escape hatch for a box with
        // a tight thread budget; it is not set by default.
        //
        // The worker **stack** is the server's: the three front ends (Thrift,
        // HTTP, PG) used to each build a runtime with an 8 MiB stack, because
        // `dispatch_arm` is a ~1500-line match compiled into one async state
        // machine and a statement's `execute` future nests inside it - the
        // default 2 MiB overflowed (measured). They now share this runtime, so
        // the size moves here; a worker must be able to host any of the three.
        let mut builder = tokio::runtime::Builder::new_multi_thread();
        builder
            .worker_threads(workers)
            .thread_stack_size(8 * 1024 * 1024)
            // The three protocol front ends now run **on this runtime**
            // (`TaskScheduler::block_on_runtime`), so it needs the reactor and
            // the timer: `TcpListener::from_std`, `tokio::net` I/O and
            // `tokio::time::sleep` all require them.
            .enable_all()
            .thread_name("task-scheduler");
        if let Some(limit) = std::env::var("INFINITY_BLOCKING_THREADS")
            .ok()
            .and_then(|v| v.trim().parse::<usize>().ok())
            .filter(|n| *n > 0)
        {
            builder.max_blocking_threads(limit);
        }
        if let Some(cpus) = p_cores {
            // `builder.build()` spawns the `workers` runtime worker threads
            // up front before returning; blocking pool threads are spawned
            // lazily afterwards, so pinning only the first `workers` threads
            // keeps `spawn_blocking` / background work free to use all cores.
            let worker_idx = AtomicUsize::new(0);
            builder.on_thread_start(move || {
                if worker_idx.fetch_add(1, Ordering::Relaxed) < workers {
                    storage::pin_current_thread_to_cpus(&cpus);
                }
            });
        }
        let runtime = builder.build().expect("build the task scheduler runtime");
        // This is the process's **only** runtime. The fan-out primitives spawn
        // their tasks on it and never build a pool of their own, so a caller
        // outside the engine brings its own runtime and runs the call inside
        // it (`AGENTS.md` §7.5).
        Self { runtime }
    }

    /// Runs a fragment's tasklet as an **async task**.
    ///
    /// A tasklet waits on its exchange queue for the sink to drain it, and only
    /// computes in bursts. Making it a blocking job pinned a thread for the
    /// whole fragment, so `min(cores, blocks)` tasklets claimed every blocking
    /// thread and a second statement queued behind the first.
    ///
    /// The tasklet body stays on this runtime - no `spawn_blocking` inside it.
    /// A step that profiling shows to be CPU-bound is moved onto its own
    /// primitive there and then, not wrapped around the loop.
    pub fn spawn_tasklet<F, Fut>(&self, job: F)
    where
        F: FnOnce() -> Fut + Send + 'static,
        Fut: std::future::Future<Output = ()> + Send + 'static,
    {
        self.runtime.spawn(job());
    }

    /// Runs a long-lived **async** task on the runtime - the periodic trigger
    /// loop and the background task processor.
    ///
    /// Both are loops whose bodies are `.await`s (a checkpoint, a dump) with a
    /// pure enqueue in between, so between ticks they cost a future. Neither
    /// needs a thread of its own, and neither may hold one: a parked thread per
    /// background loop is what this port is removing.
    pub fn spawn_task<F>(&self, job: F) -> tokio::task::JoinHandle<()>
    where
        F: std::future::Future<Output = ()> + Send + 'static,
    {
        self.runtime.spawn(job)
    }

    /// A long-lived **blocking** consumer that owns a queue for the life of
    /// the process - the row allocator.
    ///
    /// It must not be a task: its callers reach it through `block_on` from a
    /// synchronous commit callback (see `Engine::allocator`), and parking a
    /// runtime worker to wait for something that needs that same worker would
    /// deadlock. A dedicated blocking thread makes the reply independent of the
    /// runtime.
    pub fn spawn_worker(&self, job: Job) {
        self.runtime.spawn_blocking(job);
    }

    /// Drives `future` to completion **on this runtime**, entering its context.
    ///
    /// For a caller that owns the process's main thread and has a long-lived
    /// future to run (a protocol front end's accept loop). Unlike [`block_on`],
    /// the future is polled inside the runtime, so tokio APIs that require a
    /// reactor - `TcpListener::from_std`, timers, `spawn` - work. This blocks
    /// the calling thread for the life of the process, which is exactly what
    /// `infinity` wants on its main thread.
    ///
    /// **Must not be called from inside the runtime.** `Runtime::block_on`
    /// panics with "Cannot start a runtime from within a runtime", so an
    /// `async fn` that ends by parking its caller here cannot itself be driven
    /// by `block_on_runtime`. The assertion below turns the tokio panic into a
    /// named failure that says which rule was broken.
    ///
    /// The legitimate callers are the process entry points - the CLI's `main`,
    /// the examples' and helper binaries' `main`, and a test thread that has
    /// built its own runtime - plus the external `sqllogictest::DB::run`
    /// boundary, which the CLI dispatches to the blocking pool.
    pub fn block_on_runtime<F: std::future::Future>(&self, future: F) -> F::Output {
        // Legal from this engine's **blocking pool**: those jobs block by
        // design, are drawn from tokio's own growable pool, and parking one
        // blocks nothing the runtime needs. `ON_A_BLOCKING_JOB` is set by
        // [`Self::spawn_blocking`] around every job it dispatches - it is the
        // only signal that separates one from a worker, because tokio gives a
        // blocking job a task id too (`try_id()` is `Some` on both).
        //
        // A worker parking here stalls the executor, and nesting a second
        // `block_on` inside one panics inside tokio.
        let on_a_blocking_job = ON_A_BLOCKING_JOB.with(|flag| flag.get());
        let on_a_runtime_task = tokio::runtime::Handle::try_current().is_ok() && !on_a_blocking_job;
        assert!(
            !on_a_runtime_task,
            "block_on_runtime parks the calling thread in the engine's runtime; \
             calling it from a task (or from another block_on_runtime) panics \
             inside tokio. Await the future instead, or keep the caller synchronous."
        );
        self.runtime.block_on(future)
    }

    /// Runs `f` on the **blocking pool** and awaits it.
    ///
    /// This is the dispatch for work that must not occupy a runtime worker: a
    /// pure-CPU step (a SELECT's planning and execution) or a genuinely blocking
    /// call (`fsync`, a RocksDB batch, an `mmap` fault).
    ///
    /// `spawn_blocking` runs the closure on the calling runtime's blocking pool,
    /// so the async worker is free to poll other tasks while it runs: the
    /// statement keeps its task, its future and its connection, and the thread
    /// it costs is drawn from the pool tokio grows for blocking work. That is
    /// the shape a request path needs - one thread per *executing* statement, no
    /// thread per *in-flight* one.
    ///
    /// The closure runs on a pool thread with no task context, so the
    /// statement's session is carried across explicitly (`with_pool_session`).
    pub async fn spawn_blocking<F, T>(&self, f: F) -> T
    where
        F: FnOnce() -> T + Send + 'static,
        T: Send + 'static,
    {
        if tokio::runtime::Handle::try_current().is_err() {
            // Outside a runtime (a unit test, the `cli`): no pool to hand the
            // work to and no worker to protect.
            return f();
        }
        let session = crate::engine::statements::active_session();
        // The read timestamp travels the same way: the task-local does not
        // exist on a pool thread, and `read_ts()` would fall back to `u64::MAX`
        // there - every row would look visible, deleted ones included.
        let read_ts = crate::engine::state::read_ts();
        self.runtime
            .spawn_blocking(move || {
                ON_A_BLOCKING_JOB.with(|flag| flag.set(true));
                let out = crate::engine::statements::with_pool_session(session, || {
                    crate::engine::state::with_pool_read_ts(read_ts, f)
                });
                ON_A_BLOCKING_JOB.with(|flag| flag.set(false));
                out
            })
            .await
            .expect("the blocking pool dropped a job without running it")
    }
}

thread_local! {
    /// Whether this thread is running one of this engine's `spawn_blocking`
    /// jobs (see `block_on_runtime`). Only valid inside a job: the flag is set
    /// and cleared by [`TaskScheduler::spawn_blocking`].
    static ON_A_BLOCKING_JOB: std::cell::Cell<bool> = const { std::cell::Cell::new(false) };
}

/// Sleeps for `duration` on the caller's runtime.
///
/// The CLI has no direct tokio dependency (it links the engine, not the
/// runtime), so its periodic page release asks for the wait through here.
pub async fn sleep(duration: std::time::Duration) {
    tokio::time::sleep(duration).await;
}

/// Polls `future` to completion on this thread.
///
/// `futures_executor::block_on` cannot be nested (it refuses to run inside
/// another `block_on`), and the engine nests by construction: a `block_on` over
/// a statement can reach a helper that itself waits for a WAL reply, the row
/// allocator or a bottom half. A park/unpark loop has no such restriction - it
/// is just "poll, and if pending, sleep until woken" - so it works at any
/// depth, on a runtime worker or off it.
///
/// The waker is the current thread's own: a future that is ready to make
/// progress from another thread (`oneshot`, `Notify`) unparks it directly.
///
/// A bare `park` would spin here: `unpark` leaves a token that the **next**
/// `park` consumes, so a future that is woken while it is being polled leaves
/// one behind and the loop re-polls with nothing to wait for. The flag records
/// "woken since the last check" and is consumed before parking, so a stray
/// token costs one iteration instead of a hot loop.
pub fn block_on<F: std::future::Future>(future: F) -> F::Output {
    use std::sync::atomic::{AtomicBool, Ordering};
    use std::task::{Context, Poll, Wake, Waker};

    struct ThreadWaker {
        thread: std::thread::Thread,
        woken: AtomicBool,
    }
    impl Wake for ThreadWaker {
        fn wake(self: std::sync::Arc<Self>) {
            self.wake_by_ref();
        }
        fn wake_by_ref(self: &std::sync::Arc<Self>) {
            self.woken.store(true, Ordering::SeqCst);
            self.thread.unpark();
        }
    }

    let state = std::sync::Arc::new(ThreadWaker {
        thread: std::thread::current(),
        woken: AtomicBool::new(false),
    });
    let waker = Waker::from(std::sync::Arc::clone(&state));
    let mut context = Context::from_waker(&waker);
    let mut future = std::pin::pin!(future);
    loop {
        // Consume the wake that arrived since the last poll *before* polling,
        // so a wake recorded during the poll below is the one this iteration
        // waits on.
        state.woken.store(false, Ordering::SeqCst);
        match future.as_mut().poll(&mut context) {
            Poll::Ready(value) => return value,
            Poll::Pending => {
                while !state.woken.swap(false, Ordering::SeqCst) {
                    std::thread::park();
                }
            }
        }
    }
}

/// The process-wide pool.
pub fn scheduler() -> &'static TaskScheduler {
    static SCHEDULER: OnceLock<TaskScheduler> = OnceLock::new();
    SCHEDULER.get_or_init(TaskScheduler::start)
}

#[cfg(test)]
mod tests {
    use super::*;

    /// The pool is process-wide, so fragments from different statements share
    /// it instead of paying for threads of their own.
    #[test]
    fn the_pool_is_resident() {
        let first = scheduler() as *const TaskScheduler;
        let second = scheduler() as *const TaskScheduler;
        assert_eq!(first, second);
    }

    /// `block_on_runtime` must not be reachable from inside the runtime.
    ///
    /// This is the rule the CLI's `serve_from_config` depends on: its last
    /// statement parks the thread via `block_on_runtime`, so it must stay a
    /// plain `fn`. Turning it into an `async fn` driven by another
    /// `block_on_runtime` panicked inside tokio with "Cannot start a runtime
    /// from within a runtime" - a crash that only showed up when the server
    /// was actually started.
    #[test]
    #[should_panic(expected = "parks the calling thread in the engine's runtime")]
    fn block_on_runtime_is_rejected_from_inside_the_runtime() {
        scheduler().block_on_runtime(async {
            // Now inside the runtime: the same call again is the misuse.
            scheduler().block_on_runtime(async {});
        });
    }

    /// `Engine::execute` is the async entry a wire handler awaits. It
    /// must work on a spawned worker task (where `block_in_place` hands the
    /// core away) and on a `current_thread` runtime (where the body goes to
    /// `spawn_blocking`) - both shapes drive the statement's tokio channel
    /// waits without panicking.
    // A plain `#[test]`: the point of this case is the two runtimes it builds
    // itself, and `open_blocking` asserts it is called off any runtime - which
    // a `#[tokio::test]` would violate before the test body ran.
    #[test]
    fn execute_works_from_both_runtime_shapes() {
        use std::sync::Arc;

        let dir = tempfile::tempdir().unwrap();
        // This test's whole point is the two runtime shapes it builds itself,
        // so the engine opens on one of them rather than on a third.
        let engine =
            Arc::new(crate::Engine::open_blocking(dir.path(), Default::default()).unwrap());

        let sql = "CREATE TABLE t (id INT); INSERT INTO t VALUES (7);";
        let worker = Arc::clone(&engine);
        let multi = tokio::runtime::Builder::new_multi_thread()
            .worker_threads(2)
            .enable_all()
            .build()
            .unwrap();
        multi.block_on(async move {
            tokio::spawn(async move { worker.execute(sql).await.unwrap() })
                .await
                .unwrap();
        });

        let current = tokio::runtime::Builder::new_current_thread()
            .enable_all()
            .build()
            .unwrap();
        current.block_on(async { engine.execute("INSERT INTO t VALUES (8);").await.unwrap() });

        let mut total = 0;
        for result in current.block_on(async { engine.execute("SELECT id FROM t;").await.unwrap() })
        {
            if let crate::QueryResult::Rows { rows, .. } = result {
                total += rows.len();
            }
        }
        assert_eq!(total, 2);
    }

    /// The shape the server needs: many statements in flight at once on a
    /// runtime with **two** worker threads.
    ///
    /// Each statement waits on the WAL writer, the row allocator and a bottom
    /// half - a synchronous wait would park a worker per statement, so 64
    /// concurrent requests on 2 workers would deadlock. Completing them all is
    /// what proves the waits are `.await`s and the in-flight cost is a future,
    /// not a thread.
    #[tokio::test(flavor = "multi_thread", worker_threads = 2)]
    async fn many_statements_in_flight_do_not_need_a_thread_each() {
        use std::sync::Arc;

        const REQUESTS: usize = 64;

        let dir = tempfile::tempdir().unwrap();
        let engine = Arc::new(
            crate::Engine::open_async(dir.path(), Default::default())
                .await
                .unwrap(),
        );
        engine
            .execute("CREATE TABLE t (id INT, name VARCHAR);")
            .await
            .unwrap();

        let mut tasks = Vec::with_capacity(REQUESTS);
        for i in 0..REQUESTS {
            let engine = Arc::clone(&engine);
            tasks.push(tokio::spawn(async move {
                let sql = format!("INSERT INTO t VALUES ({i}, 'v{i}');");
                engine.execute(&sql).await.unwrap();
                // A read on the same task keeps the scan path in the mix.
                engine.execute("SELECT count(*) FROM t;").await.unwrap();
            }));
        }
        for task in tasks {
            task.await.unwrap();
        }

        assert_eq!(engine.row_count("default_db.t").unwrap(), REQUESTS);
    }

    /// `INSERT … SELECT` reads its source through the synchronous SELECT path
    /// **on the statement's task**, and that path's exchange sink pulls its
    /// tasklets' queues with a blocking wait. On a worker the wait must not be
    /// `recv` (tokio panics: "Cannot block the current thread from within a
    /// runtime"), or every such statement aborts its connection.
    ///
    /// The source is several blocks wide so the projection is split into
    /// tasklets and the sink really runs; the projection itself is constant
    /// because the reference rejects a column reference in `INSERT … SELECT`.
    #[tokio::test(flavor = "multi_thread", worker_threads = 2)]
    async fn insert_select_reads_its_source_on_a_worker() {
        use std::sync::Arc;

        const ROWS: usize = 3000;

        let dir = tempfile::tempdir().unwrap();
        let engine = Arc::new(
            crate::Engine::open_async(dir.path(), Default::default())
                .await
                .unwrap(),
        );
        let values = (0..ROWS)
            .map(|i| format!("({i}, 'v{i}')"))
            .collect::<Vec<_>>()
            .join(", ");
        engine
            .execute(
                "CREATE TABLE src (id INT, name VARCHAR);\n\
                 CREATE TABLE dst (id INT, name VARCHAR);",
            )
            .await
            .unwrap();
        engine
            .execute(&format!("INSERT INTO src VALUES {values};"))
            .await
            .unwrap();

        let worker = Arc::clone(&engine);
        tokio::spawn(async move {
            worker
                .execute("INSERT INTO dst SELECT 7, 'x' FROM src;")
                .await
                .unwrap();
        })
        .await
        .unwrap();

        assert_eq!(engine.row_count("default_db.dst").unwrap(), ROWS);
    }
}
