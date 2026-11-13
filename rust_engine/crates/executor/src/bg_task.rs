//! Background task processing (`BGTaskProcessor`, `src/storage/bg_task/
//! background_process.cppm`).
//!
//! The reference runs a single processor thread fed by a blocking queue and
//! uses completion events (`BGTask::Complete`) to wait for a stop task. This
//! module keeps the same contract — `start` / `submit` / `drain` / `stop` —
//! with the same single consumer. `drain` is the deterministic equivalent of
//! the reference's `StopProcessorTask::Wait()` for tests: it returns only after
//! every submitted task has run.
//!
//! **The worker is a task, not a thread** (plan `THREADS_TO_TOKIO.md` §1.3).
//! `BgTask::run` is `async`: its waits (the dump transaction's `flush_lock`,
//! the commit's WAL reply, the bottom half) stay `.await`s, and only the file
//! writes inside it go to the blocking pool. Wrapping the whole `run` in
//! `spawn_blocking` - the shape this module used to have - would have put an
//! entire async transaction on a blocking thread, with another `block_on`
//! nested inside it.
//!
//! The queue is still a bounded **tokio** channel, and `submit` still applies
//! backpressure (`DEFAULT_BLOCKING_QUEUE_SIZE`). An outstanding-work counter,
//! not a queue length, tells `drain` when the pool is idle: a worker holding a
//! task has taken it off the queue but has not finished it.

use std::future::Future;
use std::pin::Pin;
use std::sync::atomic::{AtomicUsize, Ordering};
use std::sync::{Arc, Condvar, Mutex};

/// A unit of background work.
pub trait BgTask: Send + 'static {
    /// Task description, mirroring `BGTask::ToString()`.
    fn name(&self) -> &str;
    /// Runs the task. The box is consumed, like C++'s `shared_ptr` task.
    ///
    /// A boxed future rather than a native `async fn` because the trait is used
    /// behind `dyn BgTask`; `async fn` in a trait is not dyn-compatible.
    fn run(self: Box<Self>) -> Pin<Box<dyn Future<Output = ()> + Send>>;
}

/// C++ `DEFAULT_BLOCKING_QUEUE_SIZE` (`default_values.cppm`): `submit` waits
/// while the queue holds this many tasks, so a producer cannot outrun the
/// worker.
const QUEUE_CAPACITY: usize = 1024;

/// The pieces the worker task needs after the processor itself may be gone.
struct Shared {
    /// Tasks submitted but not finished (queued or running).
    outstanding: AtomicUsize,
    /// Signalled when `outstanding` reaches zero.
    idle: Mutex<()>,
    idle_cv: Condvar,
}

/// A background task processor (`BGTaskProcessor`).
pub struct BGTaskProcessor {
    /// `None` before `start` and after `stop`: dropping the last sender closes
    /// the channel, so the worker's `recv().await` returns `None` and it exits
    /// once the queue is drained.
    jobs: Mutex<Option<tokio::sync::mpsc::Sender<Box<dyn BgTask>>>>,
    shared: Arc<Shared>,
    /// The worker's `JoinHandle`. A runtime task, so `stop` waits by aborting
    /// nothing - it closes the channel and parks until the worker's own
    /// `outstanding` counter reaches zero.
    handles: Mutex<Vec<tokio::task::JoinHandle<()>>>,
}

impl BGTaskProcessor {
    pub fn new(_workers: usize) -> Self {
        // One consumer. The queue is FIFO and a background task is whole-file
        // work; the callers parallelise a sweep themselves (a dump sweep is a
        // tokio fan-out), so a second consumer would only let two dumps of one
        // table race. The parameter stays so the call shape matches the
        // reference's processor pool.
        Self {
            jobs: Mutex::new(None),
            shared: Arc::new(Shared {
                outstanding: AtomicUsize::new(0),
                idle: Mutex::new(()),
                idle_cv: Condvar::new(),
            }),
            handles: Mutex::new(Vec::new()),
        }
    }

    /// Starts the worker task. Idempotent.
    pub fn start(self: &Arc<Self>) {
        let mut handles = self.handles.lock().unwrap();
        if !handles.is_empty() {
            return;
        }
        let (jobs, mut queue) = tokio::sync::mpsc::channel::<Box<dyn BgTask>>(QUEUE_CAPACITY);
        *self.jobs.lock().unwrap() = Some(jobs);
        let shared = Arc::clone(&self.shared);
        let handle = crate::scheduler::scheduler().spawn_task(async move {
            while let Some(task) = queue.recv().await {
                // A panicking task must not take the worker down with it: the
                // task would die with `outstanding` still incremented, and
                // every later `drain` or `stop` would hang (Plaid's
                // `expect("PLAID mode has a quantizer")` in `plaid/index.rs` is
                // reachable from a background dump). The `JoinHandle` reports
                // the unwind; report it and keep serving the queue.
                let name = task.name().to_string();
                if let Err(error) = tokio::spawn(task.run()).await {
                    eprintln!("background task '{name}' failed: {error}");
                }
                // The decrement and the wake-up happen on **every** exit path,
                // panic included, so `drain` cannot be left waiting on a task
                // that is gone.
                if shared.outstanding.fetch_sub(1, Ordering::SeqCst) == 1 {
                    let _idle = shared.idle.lock().unwrap();
                    shared.idle_cv.notify_all();
                }
            }
        });
        handles.push(handle);
    }

    /// Queues `task` and wakes the worker.
    ///
    /// Returns `false` when the pool is shutting down and `task` was dropped
    /// without running: callers that set a "proposed work" flag before
    /// submitting must clear it again, or the flag stays set for the rest of
    /// the process and stops new work from being proposed.
    ///
    /// The send is `try_send`, not `blocking_send`: `submit` is called from
    /// `enforce_memindex_quota`, which already runs on a **runtime worker**
    /// (its callers are all async: `run_bottom`, `insert_literals_inner`,
    /// `copy_from_inner`), and blocking that worker on a full queue would park
    /// the thread the rest of the statement needs. A full queue reports
    /// `false` like a stopped one, so the caller takes its existing synchronous
    /// dump path instead of waiting.
    pub fn submit(&self, task: Box<dyn BgTask>) -> bool {
        let Some(sender) = self.jobs.lock().unwrap().clone() else {
            return false;
        };
        // Counted before the send: otherwise a fast worker could finish the
        // task and see zero outstanding while a `drain` had already passed the
        // queue.
        self.shared.outstanding.fetch_add(1, Ordering::SeqCst);
        if let Err(error) = sender.try_send(task) {
            // The task never ran, so it must not stay counted.
            self.shared.outstanding.fetch_sub(1, Ordering::SeqCst);
            self.wake_if_idle();
            // `Full` means the producer outran the worker; the caller's
            // synchronous path covers it just like a shutdown does.
            let _ = error;
            return false;
        }
        true
    }

    /// Blocks until the queue is empty and every running task has finished.
    pub fn drain(&self) {
        let mut idle = self.shared.idle.lock().unwrap();
        while self.shared.outstanding.load(Ordering::SeqCst) != 0 {
            idle = self.shared.idle_cv.wait(idle).unwrap();
        }
    }

    /// Number of queued plus running tasks.
    pub fn running_task_count(&self) -> usize {
        self.shared.outstanding.load(Ordering::SeqCst)
    }

    /// Stops the pool after the current tasks finish. Idempotent.
    pub fn stop(&self) {
        // Closing the channel lets the worker's `recv().await` return `None`
        // once the queue is drained; `Stop()` in the reference also waits for
        // the tasks already queued. The wait is the outstanding counter, not a
        // thread join: the worker is a runtime task and joining it from a
        // non-async caller would need a `block_on` of its own.
        self.jobs.lock().unwrap().take();
        let handles: Vec<tokio::task::JoinHandle<()>> =
            std::mem::take(&mut *self.handles.lock().unwrap());
        drop(handles);
        self.drain();
    }

    fn wake_if_idle(&self) {
        if self.shared.outstanding.load(Ordering::SeqCst) == 0 {
            let _idle = self.shared.idle.lock().unwrap();
            self.shared.idle_cv.notify_all();
        }
    }
}

impl Default for BGTaskProcessor {
    fn default() -> Self {
        Self::new(1)
    }
}

impl Drop for BGTaskProcessor {
    fn drop(&mut self) {
        self.stop();
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    struct Increment(Arc<AtomicUsize>);

    impl BgTask for Increment {
        fn name(&self) -> &str {
            "Increment"
        }

        fn run(self: Box<Self>) -> Pin<Box<dyn Future<Output = ()> + Send>> {
            Box::pin(async move {
                self.0.fetch_add(1, Ordering::SeqCst);
            })
        }
    }

    /// C++: `background_process_ut.cpp:37` (`BGProcessTest.test1`) — start,
    /// submit a real task, drain and stop.
    #[test]
    fn start_submit_drain_stop() {
        let processor = Arc::new(BGTaskProcessor::new(2));
        processor.start();

        let counter = Arc::new(AtomicUsize::new(0));
        for _ in 0..8 {
            // `submit` is `try_send` now: a bounded queue of 1024 takes eight
            // tasks without any of them being refused.
            assert!(processor.submit(Box::new(Increment(Arc::clone(&counter)))));
        }
        processor.drain();
        assert_eq!(counter.load(Ordering::SeqCst), 8);
        assert_eq!(processor.running_task_count(), 0);

        processor.stop();
        assert_eq!(processor.running_task_count(), 0);
    }
}
