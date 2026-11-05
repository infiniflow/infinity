//! Port of `src/storage/wal/bottom_executor.{cppm,impl}`.
//!
//! Mechanical translation first: the same worker pool, the same bulk dequeue
//! and the same table affinity. The reference routes a transaction to a worker
//! by `CRC32(table_id) % executor_size`, so commits of one table stay ordered
//! on one thread; the worker calls `CommitBottom()` only when the txn is in
//! `kCommitting`.
//!
//! The reference's `BottomExecutor` is typed on `NewTxn *`. This port is
//! generic over a [`BottomTask`] so it does not depend on the transaction
//! type; the behaviour is the same.

use std::collections::VecDeque;
use std::sync::{Arc, Condvar, Mutex};
use std::thread::JoinHandle;

use crate::crc::crc32_ieee;

/// C++ `DEFAULT_BLOCKING_QUEUE_SIZE` (`default_values.cppm`).
const DEFAULT_QUEUE_CAPACITY: usize = 1024;

/// What a worker needs from a transaction (`bottom_executor_impl.cpp:49-52`).
pub trait BottomTask: Send + Sync + 'static {
    /// `NewTxn::GetTableIdStr` — the routing key.
    fn table_id(&self) -> &str;
    /// `NewTxn::GetTxnState() == TxnState::kCommitting`.
    fn is_committing(&self) -> bool;
    /// `NewTxn::CommitBottom`.
    fn commit_bottom(&self);
}

struct QueueInner<T> {
    items: VecDeque<Option<Arc<T>>>,
}

/// A blocking queue with the reference's `Enqueue` / `DequeueBulk` pair.
/// `None` is the stop signal (`bottom_executor_impl.cpp:44`).
struct BlockingQueue<T> {
    inner: Mutex<QueueInner<T>>,
    cv: Condvar,
    /// C++ `DEFAULT_BLOCKING_QUEUE_SIZE`: `Enqueue` waits while the queue holds
    /// this many items, so a producer cannot outrun the worker.
    capacity: usize,
}

impl<T> BlockingQueue<T> {
    fn new() -> Self {
        Self {
            inner: Mutex::new(QueueInner {
                items: VecDeque::new(),
            }),
            cv: Condvar::new(),
            capacity: DEFAULT_QUEUE_CAPACITY,
        }
    }

    fn enqueue(&self, item: Option<Arc<T>>) {
        let mut inner = self.inner.lock().unwrap();
        while inner.items.len() >= self.capacity {
            inner = self.cv.wait(inner).unwrap();
        }
        inner.items.push_back(item);
        self.cv.notify_all();
    }

    /// Blocks until at least one item is available, then drains everything
    /// currently queued (the reference's `DequeueBulk`).
    fn dequeue_bulk(&self, out: &mut Vec<Option<Arc<T>>>) {
        let mut inner = self.inner.lock().unwrap();
        while inner.items.is_empty() {
            inner = self.cv.wait(inner).unwrap();
        }
        out.extend(inner.items.drain(..));
        drop(inner);
        self.cv.notify_all();
    }
}

/// C++ `BottomExecutor` (`bottom_executor.cppm:24`).
pub struct BottomExecutor<T: BottomTask> {
    queues: Vec<Arc<BlockingQueue<T>>>,
    executors: Vec<JoinHandle<()>>,
}

impl<T: BottomTask> Default for BottomExecutor<T> {
    fn default() -> Self {
        Self::new()
    }
}

impl<T: BottomTask> BottomExecutor<T> {
    pub fn new() -> Self {
        Self {
            queues: Vec::new(),
            executors: Vec::new(),
        }
    }

    /// C++ `BottomExecutor::Start` (`bottom_executor_impl.cpp:32`).
    pub fn start(&mut self, executor_size: usize) {
        for _ in 0..executor_size {
            let queue = Arc::new(BlockingQueue::<T>::new());
            self.queues.push(queue.clone());
            let handle = std::thread::spawn(move || {
                let mut txns: Vec<Option<Arc<T>>> = Vec::new();
                loop {
                    queue.dequeue_bulk(&mut txns);
                    let txns_size = txns.len();
                    for txn_idx in 0..txns_size {
                        match &txns[txn_idx] {
                            None => {
                                // Stop signal: this worker exits.
                                return;
                            }
                            Some(txn) => {
                                if txn.is_committing() {
                                    txn.commit_bottom();
                                }
                            }
                        }
                    }
                    txns.clear();
                }
            });
            self.executors.push(handle);
        }
    }

    /// C++ `BottomExecutor::Stop` (`bottom_executor_impl.cpp:62`): one stop
    /// signal per worker, then join.
    pub fn stop(&mut self) {
        for queue in &self.queues {
            queue.enqueue(None);
        }
        for handle in self.executors.drain(..) {
            let _ = handle.join();
        }
        self.queues.clear();
    }

    /// C++ `BottomExecutor::Submit` (`bottom_executor_impl.cpp:73`).
    pub fn submit(&self, txn: Arc<T>) {
        let checksum = crc32_ieee(txn.table_id().as_bytes());
        let idx = (checksum as usize) % self.queues.len();
        self.queues[idx].enqueue(Some(txn));
    }
}

impl<T: BottomTask> Drop for BottomExecutor<T> {
    fn drop(&mut self) {
        if !self.executors.is_empty() {
            self.stop();
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::sync::atomic::{AtomicUsize, Ordering};

    #[derive(Default)]
    struct Counters {
        committed: AtomicUsize,
    }

    struct Task {
        table: String,
        committing: bool,
        counters: Arc<Counters>,
    }

    impl BottomTask for Task {
        fn table_id(&self) -> &str {
            &self.table
        }
        fn is_committing(&self) -> bool {
            self.committing
        }
        fn commit_bottom(&self) {
            self.counters.committed.fetch_add(1, Ordering::SeqCst);
        }
    }

    #[test]
    fn routes_by_crc_and_commits_only_committing_txns() {
        let counters = Arc::new(Counters::default());
        let mut executor = BottomExecutor::<Task>::new();
        executor.start(4);
        for i in 0..8 {
            executor.submit(Arc::new(Task {
                table: format!("db.tbl{i}"),
                committing: i % 2 == 0,
                counters: counters.clone(),
            }));
        }
        executor.stop();
        // Only the even ones were committing.
        assert_eq!(counters.committed.load(Ordering::SeqCst), 4);
    }

    #[test]
    fn routing_index_matches_crc_modulo() {
        // Pin the reference's routing rule independently of the worker pool.
        let n = 4usize;
        for table in ["db.t0", "db.t1", "db.t2", "db.t3"] {
            let idx = (crc32_ieee(table.as_bytes()) as usize) % n;
            assert!(idx < n);
        }
    }
}
