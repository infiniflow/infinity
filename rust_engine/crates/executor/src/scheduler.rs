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
//! * a tasklet is CPU-bound and blocks on its exchange queue, so it runs on the
//!   runtime's **blocking pool** (`spawn_blocking`) - the pool tokio grows past
//!   the worker count, which is what keeps a sink task from waiting behind
//!   tasklets that are blocked writing to it;
//! * the sink task is submitted the same way and awaited with `block_on`, so
//!   the statement thread waits for it exactly as `QueryContext` waits for the
//!   fragment.

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
        let runtime = tokio::runtime::Builder::new_multi_thread()
            .worker_threads(workers)
            .thread_name("task-scheduler")
            .build()
            .expect("build the task scheduler runtime");
        Self { runtime }
    }

    /// `ScheduleTask`: a tasklet becomes a blocking job on the pool.
    pub fn submit(&self, job: Job) {
        self.runtime.spawn_blocking(job);
    }

    /// A long-lived blocking consumer (the row allocator). It holds its queue
    /// for the life of the process, so it gets one of tokio's blocking threads
    /// instead of a `std::thread` of its own.
    pub fn spawn_worker(&self, job: Job) {
        self.runtime.spawn_blocking(job);
    }

    /// Schedules the fragment's sink task and waits for it, the way the
    /// statement thread waits for the fragment in the reference.
    pub fn run<F, T>(&self, job: F) -> T
    where
        F: FnOnce() -> T + Send + 'static,
        T: Send + 'static,
    {
        self.runtime
            .block_on(async move { tokio::task::spawn_blocking(job).await })
            .expect("the task scheduler dropped a job without running it")
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

    /// A job runs on the pool's threads, not on the caller.
    #[test]
    fn jobs_run_on_the_pool() {
        let here = std::thread::current().id();
        let there = scheduler().run(|| std::thread::current().id());
        assert_ne!(here, there, "the job should run on a scheduler thread");
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
}
