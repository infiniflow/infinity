//! Background task processing (`BGTaskProcessor`, `src/storage/bg_task/
//! background_process.cppm`).
//!
//! The reference runs a single processor thread fed by a blocking queue and
//! uses completion events (`BGTask::Complete`) to wait for a stop task. This
//! module keeps the same contract — `start` / `submit` / `drain` / `stop` —
//! with a small worker pool. `drain` is the deterministic equivalent of the
//! reference's `StopProcessorTask::Wait()` for tests: it returns only after
//! every submitted task has run.

use std::collections::VecDeque;
use std::sync::{Arc, Condvar, Mutex};
use std::thread::JoinHandle;

/// A unit of background work.
pub trait BgTask: Send + 'static {
    /// Task description, mirroring `BGTask::ToString()`.
    fn name(&self) -> &str;
    /// Runs the task. The box is consumed, like C++'s `shared_ptr` task.
    fn run(self: Box<Self>);
}

struct State {
    queue: VecDeque<Box<dyn BgTask>>,
    running: usize,
    shutdown: bool,
}

struct Shared {
    state: Mutex<State>,
    ready: Condvar,
}

/// A background task processor (`BGTaskProcessor`).
pub struct BGTaskProcessor {
    workers: usize,
    shared: Arc<Shared>,
    handles: Mutex<Vec<JoinHandle<()>>>,
}

impl BGTaskProcessor {
    pub fn new(workers: usize) -> Self {
        Self {
            workers: workers.max(1),
            shared: Arc::new(Shared {
                state: Mutex::new(State {
                    queue: VecDeque::new(),
                    running: 0,
                    shutdown: false,
                }),
                ready: Condvar::new(),
            }),
            handles: Mutex::new(Vec::new()),
        }
    }

    /// Starts the worker pool. Idempotent.
    pub fn start(self: &Arc<Self>) {
        let mut handles = self.handles.lock().unwrap();
        if !handles.is_empty() {
            return;
        }
        for i in 0..self.workers {
            let shared = Arc::clone(&self.shared);
            let handle = std::thread::Builder::new()
                .name(format!("infinity-bg-{i}"))
                .spawn(move || worker(shared))
                .expect("failed to spawn background worker");
            handles.push(handle);
        }
    }

    /// Queues `task` and wakes a worker.
    ///
    /// Returns `false` when the pool is shutting down and `task` was dropped
    /// without running: callers that set a "proposed work" flag before
    /// submitting must clear it again, or the flag stays set for the rest of
    /// the process and stops new work from being proposed.
    pub fn submit(&self, task: Box<dyn BgTask>) -> bool {
        let mut state = self.shared.state.lock().unwrap();
        if state.shutdown {
            return false;
        }
        state.queue.push_back(task);
        self.shared.ready.notify_one();
        true
    }

    /// Blocks until the queue is empty and every running task has finished.
    pub fn drain(&self) {
        let mut state = self.shared.state.lock().unwrap();
        while !state.queue.is_empty() || state.running != 0 {
            state = self.shared.ready.wait(state).unwrap();
        }
    }

    /// Number of queued plus running tasks.
    pub fn running_task_count(&self) -> usize {
        let state = self.shared.state.lock().unwrap();
        state.queue.len() + state.running
    }

    /// Stops the pool after the current tasks finish. Idempotent.
    pub fn stop(&self) {
        {
            let mut state = self.shared.state.lock().unwrap();
            state.shutdown = true;
            self.shared.ready.notify_all();
        }
        let handles: Vec<JoinHandle<()>> = std::mem::take(&mut *self.handles.lock().unwrap());
        for handle in handles {
            let _ = handle.join();
        }
    }
}

impl Drop for BGTaskProcessor {
    fn drop(&mut self) {
        self.stop();
    }
}

/// Keeps the running-task counter truthful when a task unwinds.
///
/// `State::running` and `State::queue` are what [`BGTaskProcessor::drain`] and
/// [`BGTaskProcessor::running_task_count`] wait on, so a decrement that is
/// skipped by a panic leaves the counter above zero forever and makes `drain`
/// (and therefore `Stop`) block for the lifetime of the process. The guard
/// performs the decrement and the final wake-up on every exit path.
struct RunningGuard(Arc<Shared>);

impl Drop for RunningGuard {
    fn drop(&mut self) {
        let mut state = self.0.state.lock().unwrap();
        state.running -= 1;
        if state.queue.is_empty() && state.running == 0 {
            self.0.ready.notify_all();
        }
    }
}

/// The message carried by a caught panic payload.
fn panic_message(payload: &(dyn std::any::Any + Send)) -> &str {
    if let Some(message) = payload.downcast_ref::<&str>() {
        message
    } else if let Some(message) = payload.downcast_ref::<String>() {
        message.as_str()
    } else {
        "non-string panic payload"
    }
}

fn worker(shared: Arc<Shared>) {
    loop {
        let mut state = shared.state.lock().unwrap();
        while !state.shutdown && state.queue.is_empty() {
            state = shared.ready.wait(state).unwrap();
        }
        // Finish queued tasks even when shutting down (`Stop()` in the
        // reference waits for the stop task behind them).
        let Some(task) = state.queue.pop_front() else {
            if state.shutdown {
                return;
            }
            continue;
        };
        state.running += 1;
        drop(state);

        // A panicking task must not take the worker down with it: the thread
        // would die with `running` still incremented, and every later `drain`
        // or `stop` would hang (Plaid's `expect("PLAID mode has a quantizer")`
        // in `plaid/index.rs` is reachable from a background dump). Catch the
        // unwind, report it, and keep serving the queue.
        let name = task.name().to_string();
        let guard = RunningGuard(Arc::clone(&shared));
        if let Err(payload) = std::panic::catch_unwind(std::panic::AssertUnwindSafe(|| task.run()))
        {
            eprintln!(
                "background task '{name}' panicked: {}",
                panic_message(payload.as_ref())
            );
        }
        drop(guard);
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::sync::atomic::{AtomicUsize, Ordering};

    struct Increment(Arc<AtomicUsize>);

    impl BgTask for Increment {
        fn name(&self) -> &str {
            "Increment"
        }

        fn run(self: Box<Self>) {
            self.0.fetch_add(1, Ordering::SeqCst);
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
            processor.submit(Box::new(Increment(Arc::clone(&counter))));
        }
        processor.drain();
        assert_eq!(counter.load(Ordering::SeqCst), 8);
        assert_eq!(processor.running_task_count(), 0);

        processor.stop();
        assert_eq!(processor.running_task_count(), 0);
    }
}
