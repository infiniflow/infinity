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

use std::sync::OnceLock;

type Job = Box<dyn FnOnce() + Send + 'static>;

pub struct TaskScheduler {
    runtime: tokio::runtime::Runtime,
}

impl TaskScheduler {
    fn start() -> Self {
        // `TaskScheduler::Init`: `min(hardware_concurrency, CPULimit)` workers.
        let workers = std::thread::available_parallelism()
            .map(|n| n.get())
            .unwrap_or(1);
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
        let runtime = builder.build().expect("build the task scheduler runtime");
        // The fan-out primitive (`index::parallel`) spawns its tasks on the
        // calling thread's runtime when there is one. The checkpoint trigger
        // thread and the synchronous `block_on` adapters have none, so they use
        // this as the fallback instead of collapsing to a serial pass.
        index::parallel::install_runtime(runtime.handle().clone());
        Self { runtime }
    }

    /// `ScheduleTask`: runs `job` on the blocking pool.
    ///
    /// The one caller is the per-table bottom drain (`Engine::submit_bottom`):
    /// it pops that table's queued commits and runs each one's
    /// `BottomTask::commit_bottom`, which is a **synchronous** trait method that
    /// takes `tokio` gates with `block_on` and then writes rows, index entries
    /// and block files. There is no future to await inside it, and it must not
    /// run on a runtime worker (the `block_on` on a gate would park a worker
    /// that the gate's holder may need). A blocking thread is the right home
    /// for exactly one table's drain, and the drain returns the moment the
    /// queue is empty, so an idle table holds no thread.
    pub fn submit(&self, job: Job) {
        self.runtime.spawn_blocking(job);
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

    /// Runs `job` where the caller can block, and returns its value.
    ///
    /// This is the fragment's sink task: it pulls from the tasklets' exchange
    /// queues in order, so it *is* blocking work. It used to be a pool job the
    /// caller waited for, which spent a second blocking thread per statement -
    /// the caller was already on one, waiting. Running it here keeps the
    /// thread count at one (plan 6.12.4 step 3).
    ///
    /// The three contexts:
    /// * outside a runtime (a unit test, a `cli` tool) - just call it;
    /// * on a multi-thread worker - `block_in_place` hands the worker's core to
    ///   a replacement so this worker's other tasks keep being polled while the
    ///   sink drains;
    /// * on a `current_thread` runtime - there is no replacement worker and
    ///   `block_in_place` would panic, so the sink runs inline. Calling
    ///   `Runtime::block_on` here would panic anyway when `run` is itself
    ///   called from inside another runtime's task.
    pub fn run<F, T>(&self, job: F) -> T
    where
        F: FnOnce() -> T + Send + 'static,
        T: Send + 'static,
    {
        let on_worker = tokio::runtime::Handle::try_current()
            .map(|handle| handle.runtime_flavor() == tokio::runtime::RuntimeFlavor::MultiThread)
            .unwrap_or(false);
        if on_worker {
            tokio::task::block_in_place(job)
        } else {
            job()
        }
    }

    /// Drives `future` to completion **on this runtime**, entering its context.
    ///
    /// For a caller that owns the process's main thread and has a long-lived
    /// future to run (a protocol front end's accept loop). Unlike [`block_on`],
    /// the future is polled inside the runtime, so tokio APIs that require a
    /// reactor - `TcpListener::from_std`, timers, `spawn` - work. This blocks
    /// the calling thread for the life of the process, which is exactly what
    /// `infinity` wants on its main thread.
    pub fn block_on_runtime<F: std::future::Future>(&self, future: F) -> F::Output {
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
                crate::engine::statements::with_pool_session(session, || {
                    crate::engine::state::with_pool_read_ts(read_ts, f)
                })
            })
            .await
            .expect("the blocking pool dropped a job without running it")
    }
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
pub fn block_on<F: std::future::Future>(future: F) -> F::Output {
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

/// The process-wide pool.
pub fn scheduler() -> &'static TaskScheduler {
    static SCHEDULER: OnceLock<TaskScheduler> = OnceLock::new();
    SCHEDULER.get_or_init(TaskScheduler::start)
}

#[cfg(test)]
mod tests {
    use super::*;

    /// Outside a runtime `run` is a plain call: there is no worker to protect,
    /// so the sink does not pay for a blocking thread just to come back.
    #[test]
    fn run_outside_a_runtime_calls_inline() {
        let here = std::thread::current().id();
        let there = scheduler().run(|| std::thread::current().id());
        assert_eq!(here, there, "no runtime, nothing to hand the core to");
    }

    /// On a multi-thread runtime `run` uses `block_in_place`: the sink runs on
    /// the caller's worker instead of being handed to a blocking thread
    /// (plan 6.12.4 step 3).
    #[test]
    fn run_on_a_worker_stays_on_the_callers_thread() {
        let runtime = tokio::runtime::Builder::new_multi_thread()
            .worker_threads(2)
            .enable_all()
            .build()
            .unwrap();
        let (caller, ran_on) = runtime.block_on(async {
            let caller = std::thread::current().id();
            let ran_on = scheduler().run(|| std::thread::current().id());
            (caller, ran_on)
        });
        assert_eq!(
            caller, ran_on,
            "block_in_place hands the core away but keeps the thread"
        );
    }

    /// The pool is process-wide, so fragments from different statements share
    /// it instead of paying for threads of their own.
    #[test]
    fn the_pool_is_resident() {
        let first = scheduler() as *const TaskScheduler;
        let second = scheduler() as *const TaskScheduler;
        assert_eq!(first, second);
    }

    /// Several jobs submitted at once all run, so a fragment's tasklets make
    /// progress in parallel.
    #[test]
    fn concurrent_jobs_all_finish() {
        let (tx, rx) = std::sync::mpsc::channel();
        for i in 0..4 {
            let tx = tx.clone();
            scheduler().submit(Box::new(move || {
                let _ = tx.send(i);
            }));
        }
        drop(tx);
        let mut seen: Vec<i32> = rx.iter().collect();
        seen.sort();
        assert_eq!(seen, vec![0, 1, 2, 3]);
    }

    /// On a `current_thread` runtime `run` calls the job inline: there is no
    /// replacement worker for `block_in_place`, and it must not panic from
    /// inside a handler task.
    #[test]
    fn run_on_a_current_thread_runtime_stays_inline() {
        let runtime = tokio::runtime::Builder::new_current_thread()
            .enable_all()
            .build()
            .unwrap();
        let here = std::thread::current().id();
        let there = runtime.block_on(async { scheduler().run(|| std::thread::current().id()) });
        assert_eq!(here, there);
    }

    /// `Engine::execute` is the async entry a wire handler awaits. It
    /// must work on a spawned worker task (where `block_in_place` hands the
    /// core away) and on a `current_thread` runtime (where the body goes to
    /// `spawn_blocking`) - both shapes drive the statement's tokio channel
    /// waits without panicking.
    #[test]
    fn execute_works_from_both_runtime_shapes() {
        use std::sync::Arc;

        let dir = tempfile::tempdir().unwrap();
        let engine = Arc::new(crate::Engine::open(dir.path()).unwrap());

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
        for result in engine.execute_blocking("SELECT id FROM t;").unwrap() {
            if let crate::QueryResult::Rows { rows, .. } = result {
                total += rows.len();
            }
        }
        assert_eq!(total, 2);
    }

    /// `execute_blocking` is a synchronous adapter: from a runtime task it
    /// parks the worker and deadlocks. The assertion is what makes that a
    /// named failure instead of a hang.
    #[test]
    #[should_panic(expected = "execute_blocking parks the calling thread")]
    fn execute_blocking_is_rejected_on_a_runtime_task() {
        use std::sync::Arc;

        let dir = tempfile::tempdir().unwrap();
        let engine = Arc::new(crate::Engine::open(dir.path()).unwrap());
        engine.execute_blocking("CREATE TABLE t (id INT);").unwrap();
        let runtime = tokio::runtime::Builder::new_multi_thread()
            .worker_threads(2)
            .enable_all()
            .build()
            .unwrap();
        runtime.block_on(async move {
            tokio::spawn(async move {
                let _ = engine.execute_blocking("SELECT count(*) FROM t;");
            })
            .await
            .unwrap();
        });
    }

    /// The shape the server needs: many statements in flight at once on a
    /// runtime with **two** worker threads.
    ///
    /// Each statement waits on the WAL writer, the row allocator and a bottom
    /// half - a synchronous wait would park a worker per statement, so 64
    /// concurrent requests on 2 workers would deadlock. Completing them all is
    /// what proves the waits are `.await`s and the in-flight cost is a future,
    /// not a thread.
    #[test]
    fn many_statements_in_flight_do_not_need_a_thread_each() {
        use std::sync::Arc;

        const REQUESTS: usize = 64;

        let dir = tempfile::tempdir().unwrap();
        let engine = Arc::new(crate::Engine::open(dir.path()).unwrap());
        engine
            .execute_blocking("CREATE TABLE t (id INT, name VARCHAR);")
            .unwrap();

        let runtime = tokio::runtime::Builder::new_multi_thread()
            .worker_threads(2)
            .thread_stack_size(8 * 1024 * 1024)
            .enable_all()
            .build()
            .unwrap();

        let handle = Arc::clone(&engine);
        runtime.block_on(async move {
            let mut tasks = Vec::with_capacity(REQUESTS);
            for i in 0..REQUESTS {
                let engine = Arc::clone(&handle);
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
        });

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
    #[test]
    fn insert_select_reads_its_source_on_a_worker() {
        use std::sync::Arc;

        const ROWS: usize = 3000;

        let dir = tempfile::tempdir().unwrap();
        let engine = Arc::new(crate::Engine::open(dir.path()).unwrap());
        engine
            .execute_blocking(
                "CREATE TABLE src (id INT, name VARCHAR);\n\
                 CREATE TABLE dst (id INT, name VARCHAR);",
            )
            .unwrap();
        let values = (0..ROWS)
            .map(|i| format!("({i}, 'v{i}')"))
            .collect::<Vec<_>>()
            .join(", ");
        engine
            .execute_blocking(&format!("INSERT INTO src VALUES {values};"))
            .unwrap();

        let runtime = tokio::runtime::Builder::new_multi_thread()
            .worker_threads(2)
            .thread_stack_size(8 * 1024 * 1024)
            .enable_all()
            .build()
            .unwrap();
        let worker = Arc::clone(&engine);
        runtime.block_on(async move {
            tokio::spawn(async move {
                worker
                    .execute("INSERT INTO dst SELECT 7, 'x' FROM src;")
                    .await
                    .unwrap();
            })
            .await
            .unwrap();
        });

        assert_eq!(engine.row_count("default_db.dst").unwrap(), ROWS);
    }
}
