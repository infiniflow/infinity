//! Memory-index tracer.
//!
//! Port of the C++ `MemIndexTracer` (`src/storage/tracer/memindex_tracer.*`):
//! index memory usage is tracked against a quota; when the quota is exceeded
//! one non-EMVB index (or every EMVB index) is proposed for a dump. The actual
//! dump is asynchronous — `trigger_dump` hands a task to the subclass, and
//! `dump_done` releases the proposal once the dump completed.

use std::collections::HashMap;
use std::sync::Mutex;

/// C++ `MemIndexDetail`: one in-memory index visible to the tracer.
#[derive(Clone, Debug, Default, PartialEq, Eq)]
pub struct MemIndexDetail {
    pub mem_index_id: u64,
    pub db_name: String,
    pub table_name: String,
    pub index_name: String,
    pub segment_id: u32,
    pub begin_row_id: u64,
    pub mem_used: usize,
    pub row_count: usize,
    pub is_emvb_index: bool,
}

/// C++ `DumpMemIndexTask`.
#[derive(Clone, Debug, Default, PartialEq, Eq)]
pub struct DumpMemIndexTask {
    pub mem_index_id: u64,
    pub db_name: String,
    pub table_name: String,
    pub index_name: String,
    pub segment_id: u32,
    pub begin_row_id: u64,
}

/// Mutable tracer state, protected by the tracer's mutex.
#[derive(Clone, Debug, Default)]
pub struct TracerState {
    pub cur_index_memory: usize,
    /// Proposed dumps keyed by `MemIndexDetail::mem_index_id`.
    pub proposed_dump: HashMap<u64, usize>,
    pub proposed_dump_size: usize,
}

/// C++ `MemIndexTracer`. Implementors supply the catalog view and the dump
/// sink; the quota logic is provided here.
pub trait MemIndexTracer: Send + Sync {
    fn index_memory_limit(&self) -> usize;
    fn state(&self) -> &Mutex<TracerState>;
    fn all_mem_indexes(&self) -> Vec<MemIndexDetail>;
    fn trigger_dump(&self, task: DumpMemIndexTask);

    /// C++ `IncreaseMemoryUsage`.
    fn increase_memory_usage(&self, mem_inc: usize) {
        if mem_inc == 0 || self.index_memory_limit() == 0 {
            return;
        }
        let need_trigger = {
            let mut state = self.state().lock().unwrap();
            state.cur_index_memory += mem_inc;
            let limit = self.index_memory_limit();
            state.cur_index_memory > limit + state.proposed_dump_size
        };
        if need_trigger {
            self.try_trigger_dump();
        }
    }

    /// C++ `DecreaseMemUsed`.
    fn decrease_mem_used(&self, mem_dec: usize) {
        decrease_mem_used(self.state(), mem_dec);
    }

    /// C++ `DumpDone`: release a proposal.
    fn dump_done(&self, mem_index_id: u64) {
        dump_done(self.state(), mem_index_id);
    }

    /// C++ `MakeDumpTask`: every EMVB index plus at most one non-EMVB index.
    fn make_dump_task(&self) -> Vec<DumpMemIndexTask> {
        let details = self.all_mem_indexes();
        make_dump_task(self.state(), details)
    }

    /// C++ `TryTriggerDump`.
    fn try_trigger_dump(&self) -> bool {
        let tasks = self.make_dump_task();
        if tasks.is_empty() {
            return false;
        }
        for task in tasks {
            self.trigger_dump(task);
        }
        true
    }
}

/// Free-function form of `DecreaseMemUsed`, usable by dump threads that only
/// hold the shared state.
pub fn decrease_mem_used(state: &Mutex<TracerState>, mem_dec: usize) {
    let mut state = state.lock().unwrap();
    if state.cur_index_memory >= mem_dec {
        state.cur_index_memory -= mem_dec;
    } else {
        state.cur_index_memory = 0;
    }
}

/// Free-function form of `DumpDone`.
pub fn dump_done(state: &Mutex<TracerState>, mem_index_id: u64) {
    let mut state = state.lock().unwrap();
    let Some(dump_size) = state.proposed_dump.remove(&mem_index_id) else {
        return;
    };
    if state.proposed_dump_size >= dump_size {
        state.proposed_dump_size -= dump_size;
        if state.proposed_dump.is_empty() {
            state.proposed_dump_size = 0;
        }
    } else {
        state.proposed_dump_size = 0;
    }
}

fn make_dump_task(
    state: &Mutex<TracerState>,
    details: Vec<MemIndexDetail>,
) -> Vec<DumpMemIndexTask> {
    let mut tasks = Vec::new();
    for detail in details {
        {
            let mut state = state.lock().unwrap();
            if state.proposed_dump.contains_key(&detail.mem_index_id) {
                continue;
            }
            state
                .proposed_dump
                .insert(detail.mem_index_id, detail.mem_used);
            state.proposed_dump_size += detail.mem_used;
        }
        let is_emvb = detail.is_emvb_index;
        tasks.push(DumpMemIndexTask {
            mem_index_id: detail.mem_index_id,
            db_name: detail.db_name,
            table_name: detail.table_name,
            index_name: detail.index_name,
            segment_id: detail.segment_id,
            begin_row_id: detail.begin_row_id,
        });
        if !is_emvb {
            break;
        }
    }
    tasks
}

/// C++ `DummyIndexInMem`: the bookkeeping-only index the reference's tracer
/// test uses. `append` accounts the memory against the tracer.
#[derive(Clone, Debug, Default)]
pub struct DummyIndexInMem {
    pub mem_index_id: u64,
    pub db_name: String,
    pub table_name: String,
    pub index_name: String,
    pub segment_id: u32,
    pub row_cnt: usize,
    pub mem_used: usize,
}

impl DummyIndexInMem {
    pub fn new(
        mem_index_id: u64,
        db_name: impl Into<String>,
        table_name: impl Into<String>,
        index_name: impl Into<String>,
        segment_id: u32,
    ) -> Self {
        Self {
            mem_index_id,
            db_name: db_name.into(),
            table_name: table_name.into(),
            index_name: index_name.into(),
            segment_id,
            row_cnt: 0,
            mem_used: 0,
        }
    }

    /// C++ `DummyIndexInMem::Append`. Callers must not hold a catalog lock.
    pub fn append(&mut self, row_cnt: usize, mem_used: usize, tracer: &dyn MemIndexTracer) {
        self.row_cnt += row_cnt;
        self.mem_used += mem_used;
        tracer.increase_memory_usage(mem_used);
    }

    /// C++ `DummyIndexInMem::Dump` is a no-op.
    pub fn dump(&self) {}

    pub fn detail(&self) -> MemIndexDetail {
        MemIndexDetail {
            mem_index_id: self.mem_index_id,
            db_name: self.db_name.clone(),
            table_name: self.table_name.clone(),
            index_name: self.index_name.clone(),
            segment_id: self.segment_id,
            begin_row_id: self.segment_id as u64,
            mem_used: self.mem_used,
            row_count: self.row_cnt,
            is_emvb_index: false,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::sync::atomic::{AtomicU64, Ordering};
    use std::sync::mpsc::{channel, Receiver, Sender};
    use std::sync::Arc;
    use std::thread::{self, JoinHandle};
    use std::time::Duration;

    /// C++ `TestCatalog`.
    #[derive(Default)]
    struct TestCatalog {
        memindexes: Mutex<HashMap<String, DummyIndexInMem>>,
        next_id: AtomicU64,
    }

    impl TestCatalog {
        fn new() -> Self {
            Self::default()
        }

        fn get_mem_indexes(&self) -> Vec<MemIndexDetail> {
            let memindexes = self.memindexes.lock().unwrap();
            let mut details: Vec<MemIndexDetail> =
                memindexes.values().map(DummyIndexInMem::detail).collect();
            details.sort_by(|lhs, rhs| rhs.mem_used.cmp(&lhs.mem_used));
            details
        }

        fn get_mem_index(&self, index_name: &str) -> Option<DummyIndexInMem> {
            self.memindexes.lock().unwrap().get(index_name).cloned()
        }

        /// C++ `AppendMemIndex`: find-or-create, accumulate, then account the
        /// memory. The catalog lock is released before calling the tracer.
        fn append_mem_index(
            &self,
            index_name: &str,
            row_cnt: usize,
            mem_used: usize,
            tracer: &dyn MemIndexTracer,
        ) {
            {
                let mut memindexes = self.memindexes.lock().unwrap();
                let dummy = memindexes.entry(index_name.to_string()).or_insert_with(|| {
                    let id = self.next_id.fetch_add(1, Ordering::Relaxed);
                    DummyIndexInMem::new(id, "db1", "tbl1", index_name, 0)
                });
                dummy.row_cnt += row_cnt;
                dummy.mem_used += mem_used;
            }
            tracer.increase_memory_usage(mem_used);
        }

        fn dump_mem_index(&self, index_name: &str) {
            let memindex = self.memindexes.lock().unwrap().remove(index_name);
            if let Some(memindex) = memindex {
                memindex.dump();
            }
        }
    }

    /// C++ `TestMemIndexTracer`.
    struct TestTracer {
        limit: usize,
        state: Arc<Mutex<TracerState>>,
        catalog: Arc<TestCatalog>,
        tx: Mutex<Option<Sender<Option<DumpMemIndexTask>>>>,
        join: Mutex<Option<JoinHandle<()>>>,
    }

    impl TestTracer {
        fn new(limit: usize, catalog: Arc<TestCatalog>) -> Arc<Self> {
            let (tx, rx): (
                Sender<Option<DumpMemIndexTask>>,
                Receiver<Option<DumpMemIndexTask>>,
            ) = channel();
            let state = Arc::new(Mutex::new(TracerState::default()));
            let thread_state = state.clone();
            let thread_catalog = catalog.clone();
            let join = thread::spawn(move || {
                while let Ok(Some(task)) = rx.recv() {
                    let index = thread_catalog.get_mem_index(&task.index_name);
                    thread_catalog.dump_mem_index(&task.index_name);
                    if let Some(index) = index {
                        decrease_mem_used(&thread_state, index.mem_used);
                    }
                    dump_done(&thread_state, task.mem_index_id);
                }
            });
            Arc::new(Self {
                limit,
                state,
                catalog,
                tx: Mutex::new(Some(tx)),
                join: Mutex::new(Some(join)),
            })
        }
    }

    impl Drop for TestTracer {
        fn drop(&mut self) {
            if let Some(tx) = self.tx.lock().unwrap().take() {
                let _ = tx.send(None);
            }
            if let Some(join) = self.join.lock().unwrap().take() {
                let _ = join.join();
            }
        }
    }

    impl MemIndexTracer for TestTracer {
        fn index_memory_limit(&self) -> usize {
            self.limit
        }

        fn state(&self) -> &Mutex<TracerState> {
            &self.state
        }

        fn all_mem_indexes(&self) -> Vec<MemIndexDetail> {
            self.catalog.get_mem_indexes()
        }

        fn trigger_dump(&self, task: DumpMemIndexTask) {
            if let Some(tx) = self.tx.lock().unwrap().as_ref() {
                let _ = tx.send(Some(task));
            }
        }
    }

    #[test]
    fn test1() {
        let memory_limit = 50;
        let catalog = Arc::new(TestCatalog::new());
        let tracer = TestTracer::new(memory_limit, catalog.clone());

        catalog.append_mem_index("idx1", 10, 10, tracer.as_ref());
        catalog.append_mem_index("idx2", 30, 30, tracer.as_ref());
        catalog.append_mem_index("idx3", 20, 20, tracer.as_ref());

        thread::sleep(Duration::from_secs(1));
        // Check idx2 has been dumped.
        let details = catalog.get_mem_indexes();
        assert_eq!(details.len(), 2);
        for detail in &details {
            assert_ne!(detail.index_name, "idx2");
        }
    }

    #[test]
    fn test2() {
        let memory_limit = 50;
        let catalog = Arc::new(TestCatalog::new());
        let tracer = TestTracer::new(memory_limit, catalog.clone());

        let thread_n = 2;
        let index_n = 10u64;
        let max_append_mem = 10u64;
        let iterate_n = 1000;
        let mut threads = Vec::new();
        for id in 0..thread_n {
            let catalog = catalog.clone();
            let tracer = tracer.clone();
            threads.push(thread::spawn(move || {
                let mut seed = 0x9E37_79B9_7F4A_7C15u64 ^ id as u64;
                for _ in 0..iterate_n {
                    seed = seed
                        .wrapping_mul(6_364_136_223_846_793_005)
                        .wrapping_add(1_442_695_040_888_963_407);
                    let idx_i = (seed >> 33) % index_n;
                    let mem = (seed >> 20) % max_append_mem + 1;
                    let idx_name = format!("idx{idx_i}");
                    catalog.append_mem_index(&idx_name, mem as usize, mem as usize, tracer.as_ref());
                }
            }));
        }
        for thread in threads {
            thread.join().unwrap();
        }
    }
}
