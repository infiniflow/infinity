//! A bounded LRU cache of decoded column blocks — the counterpart of the
//! reference's `BufferManager` (`src/storage/buffer/buffer_manager*.cppm`).
//!
//! Reads go through [`BufferManager::get`]; a miss loads the block from disk
//! and [`BufferManager::put`] evicts the least-recently-used blocks until the
//! configured byte budget holds. The executor wires `[buffer]
//! buffer_manager_size` to it, so RSS is bounded by the budget instead of the
//! total size of the tables read (Gate C/E).
//!
//! [`BufferObj`] / [`BufferHandle`] / [`BufferPool`] mirror the reference's
//! owned-buffer classes (`buffer_obj.cppm`, `buffer_handle.cppm`): a pool
//! accounts every loaded payload against its limit, and a handle pins the
//! payload until it is dropped. The executor's block cache stores decoded
//! blocks directly in [`BufferManager`], so it does not allocate these objects
//! on the scan path; they exist for the index/file workers that need owned
//! buffers with the reference's lifecycle.

use std::collections::{HashMap, VecDeque};
use std::path::PathBuf;
use std::sync::atomic::{AtomicU64, AtomicU8, AtomicUsize, Ordering};
use std::sync::{Arc, Mutex, MutexGuard, OnceLock, Weak};

use infinity_common::{Error, Result};

use crate::cpp_format::{BlockId, RawBlockColumn};

/// C++ `BufferStatus` (`src/storage/buffer/buffer_obj.cppm`).
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum BufferStatus {
    New,
    Loaded,
    /// The last handle was dropped; the payload stays charged to the pool
    /// until the next GC pass frees it (the reference's GC queue).
    Unloaded,
    Freed,
}

/// C++ `BufferType`: whether a payload outlives its last handle.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum BufferType {
    /// Backed by a spill file once its last handle is dropped.
    Ephemeral,
    /// Stays in memory until [`BufferObj::free`] is called.
    Persistent,
    /// Spilled to a temporary file, to be moved to its final path on save.
    Temp,
}

/// The shared budget of a [`BufferPool`].
struct PoolState {
    budget: usize,
    used: usize,
    /// C++ `LRUCache::gc_list_`: buffers whose last handle was dropped, in
    /// unload order. Weak so the pool does not keep them alive.
    gc_queue: VecDeque<Weak<BufferObj>>,
}

static NEXT_BUFFER_ID: AtomicU64 = AtomicU64::new(0);

/// An owned byte buffer with the reference's `BufferObj` lifecycle.
///
/// C++: `BufferObj` (`src/storage/buffer/buffer_obj.cppm`). The payload is
/// allocated on `Load` and charged to the pool's budget; `rc()` counts the
/// live handles. Dropping the last handle marks the buffer `Unloaded` and
/// queues it for GC (the reference's `gc_list_`); the payload is released by
/// [`BufferObj::free`], which the pool runs under memory pressure.
pub struct BufferObj {
    state: Arc<Mutex<PoolState>>,
    data: Mutex<Option<Vec<u8>>>,
    status: AtomicU8,
    kind: AtomicU8,
    rc: AtomicUsize,
    /// Spill/persist target. Ephemeral buffers get a temporary file so their
    /// contents survive the last handle (the reference spills to its temp dir).
    path: Option<PathBuf>,
    /// Whether `path` is an auto-created temp file this object must delete.
    owns_path: bool,
    bytes: usize,
    /// Assigned by the pool so the GC queue can hold a weak reference.
    self_ref: OnceLock<Weak<BufferObj>>,
}

const KIND_EPHEMERAL: u8 = 0;
const KIND_PERSISTENT: u8 = 1;
const KIND_TEMP: u8 = 2;

impl BufferObj {
    fn new(
        state: Arc<Mutex<PoolState>>,
        bytes: usize,
        kind: BufferType,
        path: Option<PathBuf>,
    ) -> Self {
        let (path, owns_path) = match (path, kind) {
            (Some(path), _) => (Some(path), false),
            (None, BufferType::Ephemeral | BufferType::Temp) => {
                let id = NEXT_BUFFER_ID.fetch_add(1, Ordering::Relaxed);
                (
                    Some(std::env::temp_dir().join(format!("infinity_buffer_{id}.bin"))),
                    true,
                )
            }
            (None, BufferType::Persistent) => (None, false),
        };
        Self {
            state,
            data: Mutex::new(None),
            status: AtomicU8::new(BufferStatus::New as u8),
            kind: AtomicU8::new(match kind {
                BufferType::Ephemeral => KIND_EPHEMERAL,
                BufferType::Persistent => KIND_PERSISTENT,
                BufferType::Temp => KIND_TEMP,
            }),
            rc: AtomicUsize::new(0),
            path,
            owns_path,
            bytes,
            self_ref: OnceLock::new(),
        }
    }

    pub fn status(&self) -> BufferStatus {
        match self.status.load(Ordering::Acquire) {
            0 => BufferStatus::New,
            1 => BufferStatus::Loaded,
            2 => BufferStatus::Unloaded,
            _ => BufferStatus::Freed,
        }
    }

    pub fn kind(&self) -> BufferType {
        match self.kind.load(Ordering::Acquire) {
            KIND_EPHEMERAL => BufferType::Ephemeral,
            KIND_PERSISTENT => BufferType::Persistent,
            _ => BufferType::Temp,
        }
    }

    /// C++ `BufferObj::rc()`: live handles.
    pub fn rc(&self) -> usize {
        self.rc.load(Ordering::Acquire)
    }

    pub fn size(&self) -> usize {
        self.bytes
    }

    pub fn path(&self) -> Option<PathBuf> {
        self.path.clone()
    }

    /// C++ `BufferObj::Load`: allocates (or reads back) the payload and pins
    /// it. Under memory pressure the pool first frees GC-queued buffers; if
    /// none can be freed the request fails with `Storage`.
    pub fn load(&self) -> Result<BufferHandle<'_>> {
        let mut data = self.data.lock().unwrap();
        if data.is_none() {
            // A buffer being loaded must not be its own GC victim. The
            // reference removes an UNLOADED buffer from the queue before the
            // memory request; a Freed buffer may also carry a stale entry
            // when it was freed directly, so drop any entry unconditionally.
            self.remove_from_gc_queue();
            if self.status() == BufferStatus::Freed && self.kind() == BufferType::Ephemeral {
                return Err(Error::Storage("Invalid status".into()));
            }
            self.reserve(self.bytes)?;
            let payload = match &self.path {
                Some(p) if p.exists() => std::fs::read(p).map_err(|e| Error::io(p, e))?,
                _ => vec![0u8; self.bytes],
            };
            *data = Some(payload);
        }
        self.status
            .store(BufferStatus::Loaded as u8, Ordering::Release);
        drop(data);
        self.rc.fetch_add(1, Ordering::AcqRel);
        Ok(BufferHandle { obj: self })
    }

    /// C++ `BufferObj::Save`: writes an ephemeral payload back and moves the
    /// buffer to the persistent state. A `Temp` buffer is moved in place.
    pub fn save(&self) -> Result<()> {
        let mut became_unloaded = false;
        {
            let mut data = self.data.lock().unwrap();
            match self.kind() {
                BufferType::Persistent => {
                    // A file-backed persistent payload is written in place.
                    if let Some(payload) = data.as_ref() {
                        self.write_payload(payload)?;
                    }
                    return Ok(());
                }
                BufferType::Temp => {
                    self.kind.store(KIND_PERSISTENT, Ordering::Release);
                    return Ok(());
                }
                BufferType::Ephemeral => {
                    if data.is_none() {
                        self.reserve(self.bytes)?;
                        *data = Some(vec![0u8; self.bytes]);
                        became_unloaded = self.status() == BufferStatus::New;
                    }
                    if let Some(payload) = data.as_ref() {
                        self.write_payload(payload)?;
                    }
                    self.kind.store(KIND_PERSISTENT, Ordering::Release);
                }
            }
        }
        if became_unloaded {
            self.status
                .store(BufferStatus::Unloaded as u8, Ordering::Release);
            self.push_gc_queue();
        }
        Ok(())
    }

    /// C++ `BufferObj::Free`: drops the payload and returns its budget. Only
    /// an `Unloaded` buffer is freeable; an ephemeral payload is spilled to
    /// its temp file and the buffer becomes `Temp` first.
    pub fn free(&self) -> Result<()> {
        self.remove_from_gc_queue();
        let mut data = self.data.lock().unwrap();
        if self.status() == BufferStatus::Freed {
            return Ok(());
        }
        if self.status() != BufferStatus::Unloaded {
            return Err(Error::Storage(format!(
                "attempt to free {:?} buffer object",
                self.status()
            )));
        }
        match self.kind() {
            BufferType::Persistent | BufferType::Temp => {}
            BufferType::Ephemeral => {
                if let Some(payload) = data.as_ref() {
                    self.write_payload(payload)?;
                }
                self.kind.store(KIND_TEMP, Ordering::Release);
            }
        }
        let freed = data.take().is_some();
        drop(data);
        if freed {
            let mut state = self.state.lock().unwrap();
            state.used = state.used.saturating_sub(self.bytes);
        }
        self.status
            .store(BufferStatus::Freed as u8, Ordering::Release);
        Ok(())
    }

    /// C++ `BufferObj::CheckState`: the status/reference-count invariants.
    pub fn check_state(&self) -> Result<()> {
        let invalid = |status: BufferStatus| {
            Err(Error::Storage(format!(
                "Invalid status: {status:?}, rc={}",
                self.rc()
            )))
        };
        match self.status() {
            BufferStatus::Loaded => {
                if self.rc() == 0 {
                    return invalid(BufferStatus::Loaded);
                }
            }
            BufferStatus::Unloaded | BufferStatus::Freed => {
                if self.rc() > 0 {
                    return invalid(self.status());
                }
            }
            BufferStatus::New => {
                if self.kind() != BufferType::Ephemeral || self.rc() > 0 {
                    return invalid(BufferStatus::New);
                }
            }
        }
        Ok(())
    }

    fn write_payload(&self, payload: &[u8]) -> Result<()> {
        if let Some(path) = self.path.as_ref() {
            std::fs::write(path, payload).map_err(|e| Error::io(path, e))?;
        }
        Ok(())
    }

    /// C++ `BufferManager::RequestSpace` + `LRUCache::RequestSpace`: charge
    /// `needed` bytes, freeing GC-queued buffers until the budget holds.
    fn reserve(&self, needed: usize) -> Result<()> {
        loop {
            let victim = {
                let mut state = self.state.lock().unwrap();
                if state.used + needed <= state.budget {
                    state.used += needed;
                    return Ok(());
                }
                state.gc_queue.pop_front()
            };
            let Some(victim) = victim else {
                return Err(Error::Storage("buffer manager out of memory".into()));
            };
            if let Some(victim) = victim.upgrade() {
                // A concurrent reload makes `free` refuse; the reference
                // skips such a buffer as well.
                let _ = victim.free();
            }
        }
    }

    fn push_gc_queue(&self) {
        let Some(me) = self.self_ref.get() else {
            return;
        };
        let mut state = self.state.lock().unwrap();
        state.gc_queue.push_back(me.clone());
    }

    fn remove_from_gc_queue(&self) {
        let Some(me) = self.self_ref.get() else {
            return;
        };
        let mut state = self.state.lock().unwrap();
        state.gc_queue.retain(|weak| !Weak::ptr_eq(weak, me));
    }

    /// C++ `BufferObj::UnloadInner`: the last handle leaves the payload
    /// resident but queues the buffer for GC.
    fn release_handle(&self) {
        let rc = self.rc.fetch_sub(1, Ordering::AcqRel).saturating_sub(1);
        if rc > 0 {
            return;
        }
        self.status
            .store(BufferStatus::Unloaded as u8, Ordering::Release);
        self.push_gc_queue();
    }
}

impl Drop for BufferObj {
    fn drop(&mut self) {
        if self.owns_path {
            if let Some(path) = self.path.as_ref() {
                let _ = std::fs::remove_file(path);
            }
        }
    }
}

/// A pinned [`BufferObj`]. Dropping it decrements the object's reference count.
///
/// C++: `BufferHandle` (`src/storage/buffer/buffer_handle.cppm`).
pub struct BufferHandle<'a> {
    obj: &'a BufferObj,
}

impl<'a> BufferHandle<'a> {
    /// C++ `GetData()`.
    pub fn data(&self) -> MutexGuard<'_, Option<Vec<u8>>> {
        self.obj.data.lock().unwrap()
    }

    /// C++ `GetDataMut()`. Taking a mutable pointer makes the buffer
    /// ephemeral again (the reference's `GetMutPointer`), so unflushed writes
    /// are spilled by the GC rather than silently kept persistent.
    pub fn data_mut(&self) -> MutexGuard<'_, Option<Vec<u8>>> {
        self.obj.kind.store(KIND_EPHEMERAL, Ordering::Release);
        self.obj.data.lock().unwrap()
    }

    pub fn obj(&self) -> &'a BufferObj {
        self.obj
    }
}

impl Drop for BufferHandle<'_> {
    fn drop(&mut self) {
        self.obj.release_handle();
    }
}

/// Allocates [`BufferObj`]s under one memory limit.
///
/// C++: the allocation/accounting half of `BufferManager`.
pub struct BufferPool {
    state: Arc<Mutex<PoolState>>,
}

impl BufferPool {
    pub fn new(budget: usize) -> Self {
        Self {
            state: Arc::new(Mutex::new(PoolState {
                budget,
                used: 0,
                gc_queue: VecDeque::new(),
            })),
        }
    }

    pub fn budget(&self) -> usize {
        self.state.lock().unwrap().budget
    }

    pub fn used_bytes(&self) -> usize {
        self.state.lock().unwrap().used
    }

    /// C++ `BufferManager::AllocateBufferObject` for an anonymous buffer.
    pub fn allocate(&self, bytes: usize, kind: BufferType) -> Arc<BufferObj> {
        self.register(BufferObj::new(self.state.clone(), bytes, kind, None))
    }

    /// C++ `BufferManager::AllocateBufferObject` for a file-backed buffer.
    pub fn allocate_persistent(
        &self,
        path: impl Into<PathBuf>,
        bytes: usize,
        kind: BufferType,
    ) -> Arc<BufferObj> {
        self.register(BufferObj::new(
            self.state.clone(),
            bytes,
            kind,
            Some(path.into()),
        ))
    }

    fn register(&self, obj: BufferObj) -> Arc<BufferObj> {
        let obj = Arc::new(obj);
        let _ = obj.self_ref.set(Arc::downgrade(&obj));
        obj
    }
}

/// Bounded LRU cache keyed by [`BlockId`].
pub struct BufferManager<V> {
    budget: usize,
    used: usize,
    tick: u64,
    entries: HashMap<BlockId, (Arc<V>, usize, u64)>,
}

impl<V> BufferManager<V> {
    pub fn new(budget: usize) -> Self {
        Self {
            budget,
            used: 0,
            tick: 0,
            entries: HashMap::new(),
        }
    }

    pub fn set_budget(&mut self, budget: usize) {
        self.budget = budget;
        self.evict();
    }

    pub fn budget(&self) -> usize {
        self.budget
    }

    pub fn used_bytes(&self) -> usize {
        self.used
    }

    /// Looks a block up and marks it most-recently used.
    pub fn get(&mut self, key: &BlockId) -> Option<Arc<V>> {
        self.tick += 1;
        let tick = self.tick;
        let entry = self.entries.get_mut(key)?;
        entry.2 = tick;
        Some(entry.0.clone())
    }

    /// Inserts a block, evicting LRU entries until it fits.
    pub fn put(&mut self, key: BlockId, value: Arc<V>, bytes: usize) {
        self.tick += 1;
        if let Some((_, old, _)) = self.entries.remove(&key) {
            self.used = self.used.saturating_sub(old);
        }
        let bytes = bytes.min(self.budget);
        self.entries.insert(key, (value, bytes, self.tick));
        self.used += bytes;
        self.evict();
    }

    pub fn invalidate(&mut self, key: &BlockId) {
        if let Some((_, bytes, _)) = self.entries.remove(key) {
            self.used = self.used.saturating_sub(bytes);
        }
    }

    pub fn clear(&mut self) {
        self.entries.clear();
        self.used = 0;
    }

    fn evict(&mut self) {
        while self.used > self.budget && !self.entries.is_empty() {
            let Some(key) = self
                .entries
                .iter()
                .min_by_key(|(_, (_, _, last))| *last)
                .map(|(k, _)| *k)
            else {
                break;
            };
            self.invalidate(&key);
        }
    }
}

/// Byte size of a decoded block, for the budget.
pub fn block_bytes(columns: &[RawBlockColumn]) -> usize {
    columns.iter().map(|c| c.body.len() + c.outline.len()).sum()
}

#[cfg(test)]
mod tests {
    use super::*;

    fn key(block: u16) -> BlockId {
        BlockId {
            db_id: 1,
            table_id: 2,
            segment_id: 0,
            block_id: block,
        }
    }

    #[test]
    fn evicts_least_recently_used() {
        let mut cache: BufferManager<Vec<u8>> = BufferManager::new(100);
        cache.put(key(0), Arc::new(vec![0]), 40);
        cache.put(key(1), Arc::new(vec![1]), 40);
        assert!(cache.get(&key(0)).is_some());
        // Adding a third 40-byte block exceeds 100; block 1 is the LRU.
        cache.put(key(2), Arc::new(vec![2]), 40);
        assert_eq!(cache.used_bytes(), 80);
        assert!(cache.get(&key(0)).is_some());
        assert!(cache.get(&key(1)).is_none());
        assert!(cache.get(&key(2)).is_some());
    }

    #[test]
    fn shrinking_the_budget_evicts() {
        let mut cache: BufferManager<Vec<u8>> = BufferManager::new(100);
        for b in 0..4 {
            cache.put(key(b), Arc::new(vec![b as u8]), 25);
        }
        assert_eq!(cache.used_bytes(), 100);
        cache.set_budget(50);
        assert_eq!(cache.used_bytes(), 50);
    }

    /// Port of `buffer/buffer_handle_ut.cpp`: three 512-byte buffers under a
    /// 1024-byte limit; the third `Load` fails, and an ephemeral payload
    /// survives its last handle through the spill file.
    #[test]
    fn buffer_handles_follow_the_reference_lifecycle() {
        let pool = BufferPool::new(1024);
        let b1 = pool.allocate(512, BufferType::Ephemeral);
        let b2 = pool.allocate(512, BufferType::Ephemeral);
        let b3 = pool.allocate(512, BufferType::Ephemeral);

        {
            let _h1 = b1.load().unwrap();
            assert_eq!(b1.rc(), 1);
            let _h2 = b2.load().unwrap();
            assert!(b3.load().is_err(), "the pool is out of memory");
            assert_eq!(b3.rc(), 0);
            assert_eq!(b3.status(), BufferStatus::New);
            assert_eq!(b3.kind(), BufferType::Ephemeral);
        }
        assert_eq!(b1.rc(), 0);
        assert_eq!(b2.rc(), 0);
        // C++ `UnloadInner` only queues the buffers for GC; the payloads stay
        // charged until a later load evicts them.
        assert_eq!(b1.status(), BufferStatus::Unloaded);
        assert_eq!(b2.status(), BufferStatus::Unloaded);
        assert_eq!(pool.used_bytes(), 1024);

        // Two handles on one object share the payload and the budget.
        {
            let _h1 = b1.load().unwrap();
            let _h2 = b1.load().unwrap();
            assert_eq!(b1.rc(), 2);
        }

        // 512 bytes == 128 u32 values.
        {
            let h = b1.load().unwrap();
            let mut guard = h.data_mut();
            let data = guard.as_mut().unwrap();
            for i in 0..128usize {
                data[i * 4..i * 4 + 4].copy_from_slice(&(i as u32).to_le_bytes());
            }
        }
        // Force the other buffers through the pool and read b1 back.
        {
            let _h2 = b2.load().unwrap();
            let _h3 = b3.load().unwrap();
        }
        {
            let h = b1.load().unwrap();
            let guard = h.data();
            let data = guard.as_ref().unwrap();
            for i in 0..128usize {
                let v = u32::from_le_bytes(data[i * 4..i * 4 + 4].try_into().unwrap());
                assert_eq!(v, i as u32);
            }
        }
        {
            let h = b1.load().unwrap();
            {
                let mut guard = h.data_mut();
                let data = guard.as_mut().unwrap();
                for i in 0..128usize {
                    let v = u32::from_le_bytes(data[i * 4..i * 4 + 4].try_into().unwrap());
                    assert_eq!(v, i as u32);
                    data[i * 4..i * 4 + 4].copy_from_slice(&(2 * i as u32).to_le_bytes());
                }
            }
            h.obj().save().unwrap();
        }
        {
            let h = b1.load().unwrap();
            let guard = h.data();
            let data = guard.as_ref().unwrap();
            for i in 0..128usize {
                let v = u32::from_le_bytes(data[i * 4..i * 4 + 4].try_into().unwrap());
                assert_eq!(v, 2 * i as u32);
            }
        }
        b1.free().unwrap();
        b2.free().unwrap();
        b3.free().unwrap();
        assert_eq!(pool.used_bytes(), 0);
    }

    /// C++ `BufferType::kPersistent`: the payload stays resident (and
    /// charged) after the last handle is dropped — the buffer is `Unloaded`
    /// in the reference, not `Loaded` — until `Free`.
    #[test]
    fn persistent_buffers_outlive_their_handles() {
        let pool = BufferPool::new(1024);
        let b = pool.allocate(512, BufferType::Persistent);
        {
            let h = b.load().unwrap();
            h.data_mut().as_mut().unwrap()[0] = 7;
        }
        assert_eq!(b.rc(), 0);
        assert_eq!(b.status(), BufferStatus::Unloaded);
        assert_eq!(pool.used_bytes(), 512);
        {
            let h = b.load().unwrap();
            assert_eq!(h.data().as_ref().unwrap()[0], 7);
        }
        b.free().unwrap();
        assert_eq!(b.status(), BufferStatus::Freed);
        assert_eq!(pool.used_bytes(), 0);
    }

    /// C++: `BufferObjTest.test1` (`buffer_obj_ut.cpp`) — the full
    /// New/Loaded/Unloaded/Freed × Ephemeral/Temp/Persistent state machine.
    /// Two 1024-byte buffers under a 1024-byte limit force the GC to free the
    /// unloaded buffer on every alternating load.
    #[test]
    fn buffer_obj_state_machine() {
        let pool = BufferPool::new(1024);
        let buf1 = pool.allocate(1024, BufferType::Ephemeral);
        let buf2 = pool.allocate(1024, BufferType::Ephemeral);

        // kNew, kEphemeral
        assert_eq!(buf1.status(), BufferStatus::New);
        assert_eq!(buf1.kind(), BufferType::Ephemeral);
        assert_eq!(buf1.rc(), 0);
        buf1.check_state().unwrap();

        {
            let _h1 = buf1.load().unwrap();
            // kNew, kEphemeral -> kLoaded, kEphemeral
            assert_eq!(buf1.status(), BufferStatus::Loaded);
            buf1.check_state().unwrap();
        }
        // kLoaded, kEphemeral -> kUnloaded, kEphemeral
        assert_eq!(buf1.status(), BufferStatus::Unloaded);
        buf1.check_state().unwrap();

        {
            let _h1 = buf1.load().unwrap();
            assert_eq!(buf1.status(), BufferStatus::Loaded);
            buf1.check_state().unwrap();
        }
        {
            let _h2 = buf2.load().unwrap();
        }
        // kUnloaded, kEphemeral -> kFreed, kEphemeral (the GC spilled it)
        assert_eq!(buf1.status(), BufferStatus::Freed);
        buf1.check_state().unwrap();

        {
            let h1 = buf1.load().unwrap();
            let _data = h1.data_mut();
            // kFreed, kEphemeral -> kLoaded, kEphemeral
            assert_eq!(buf1.status(), BufferStatus::Loaded);
            buf1.check_state().unwrap();
        }
        {
            let _h2 = buf2.load().unwrap();
        }

        // kTemp
        {
            let _h1 = buf1.load().unwrap();
            // kFreed, kEphemeral -> kLoaded, kTemp
            assert_eq!(buf1.status(), BufferStatus::Loaded);
            assert_eq!(buf1.kind(), BufferType::Temp);
            buf1.check_state().unwrap();
        }
        // kLoaded, kTemp -> kUnloaded, kTemp
        assert_eq!(buf1.status(), BufferStatus::Unloaded);
        buf1.check_state().unwrap();

        {
            let _h1 = buf1.load().unwrap();
            // kUnloaded, kTemp -> kLoaded, kTemp
            assert_eq!(buf1.status(), BufferStatus::Loaded);
            assert_eq!(buf1.kind(), BufferType::Temp);
            buf1.check_state().unwrap();
        }
        {
            let _h2 = buf2.load().unwrap();
        }
        // kUnloaded, kTemp -> kFreed, kTemp
        assert_eq!(buf1.status(), BufferStatus::Freed);
        buf1.check_state().unwrap();

        // kPersistent
        buf1.save().unwrap();
        // kFreed, kTemp -> kFreed, kPersistent
        assert_eq!(buf1.kind(), BufferType::Persistent);
        assert_eq!(buf1.status(), BufferStatus::Freed);

        {
            let _h1 = buf1.load().unwrap();
            // kFreed, kPersistent -> kLoaded, kPersistent
            assert_eq!(buf1.status(), BufferStatus::Loaded);
            buf1.check_state().unwrap();
        }
        // kLoaded, kPersistent -> kUnloaded, kPersistent
        assert_eq!(buf1.status(), BufferStatus::Unloaded);
        buf1.check_state().unwrap();

        {
            let _h1 = buf1.load().unwrap();
            assert_eq!(buf1.status(), BufferStatus::Loaded);
            buf1.check_state().unwrap();
        }
        {
            let _h2 = buf2.load().unwrap();
        }
        // kUnloaded, kPersistent -> kFreed, kPersistent
        assert_eq!(buf1.status(), BufferStatus::Freed);
        buf1.check_state().unwrap();

        // kEphemeral
        {
            let h1 = buf1.load().unwrap();
            let _data = h1.data_mut();
            // kFreed, kPersistent -> kLoaded, kEphemeral
            assert_eq!(buf1.status(), BufferStatus::Loaded);
            assert_eq!(buf1.kind(), BufferType::Ephemeral);
            buf1.check_state().unwrap();
        }
        buf1.save().unwrap();
        // kUnloadedModified, kEphemeral -> kUnloaded, kPersistent
        assert_eq!(buf1.status(), BufferStatus::Unloaded);
        assert_eq!(buf1.kind(), BufferType::Persistent);
        buf1.check_state().unwrap();

        {
            let h1 = buf1.load().unwrap();
            {
                // The guard pins the payload; release it before `save`.
                let _data = h1.data_mut();
            }
            // kUnloaded, kPersistent -> kLoaded, kEphemeral
            assert_eq!(buf1.status(), BufferStatus::Loaded);
            assert_eq!(buf1.kind(), BufferType::Ephemeral);
            buf1.check_state().unwrap();

            buf1.save().unwrap();
            // kLoaded, kEphemeral -> kLoaded, kPersistent
            assert_eq!(buf1.status(), BufferStatus::Loaded);
            assert_eq!(buf1.kind(), BufferType::Persistent);
            buf1.check_state().unwrap();
        }

        {
            let h1 = buf1.load().unwrap();
            let _data = h1.data_mut();
        }
        {
            let _h1 = buf1.load().unwrap();
            buf1.save().unwrap();
            // kLoaded, kEphemeral -> kLoaded, kPersistent
            assert_eq!(buf1.status(), BufferStatus::Loaded);
            assert_eq!(buf1.kind(), BufferType::Persistent);
            buf1.check_state().unwrap();
        }

        {
            let h1 = buf1.load().unwrap();
            let _data = h1.data_mut();
        }
        {
            let _h2 = buf2.load().unwrap();
        }
        {
            let _h1 = buf1.load().unwrap();
            buf1.save().unwrap();
            // kLoaded, kPersistent -> kLoaded, kPersistent
            assert_eq!(buf1.status(), BufferStatus::Loaded);
            assert_eq!(buf1.kind(), BufferType::Persistent);
            buf1.check_state().unwrap();
        }

        {
            let h1 = buf1.load().unwrap();
            let _data = h1.data_mut();
        }
        {
            let _h2 = buf2.load().unwrap();
        }
        {
            let _h1 = buf1.load().unwrap();
        }
        buf1.save().unwrap();
        // kUnloaded, kPersistent -> kUnloaded, kPersistent
        assert_eq!(buf1.status(), BufferStatus::Unloaded);
        assert_eq!(buf1.kind(), BufferType::Persistent);
        buf1.check_state().unwrap();
    }

    /// C++: `BufferObjTest.test_big_with_gc_and_cleanup` — 256 blocks of
    /// `DEFAULT_BLOCK_CAPACITY` bigints (64 KiB each) under a 512 KiB budget.
    /// The reference drives this through INSERT/Scan/DROP TABLE; the buffer
    /// layer sees the same pressure: every block is evicted, spilled and read
    /// back, the pool stays within its limit, and cleanup returns it all.
    #[test]
    fn big_blocks_are_reclaimed_under_gc_and_cleanup() {
        use crate::cpp_format::DEFAULT_BLOCK_CAPACITY;
        use crate::data_block::{BlockValue, Column, ColumnVectorType, DEFAULT_VECTOR_SIZE};
        use infinity_common::LogicalType;

        const INSERT_N: usize = 256;
        const IMPORT_SIZE: usize = 8192;
        let budget = 512 * 1024;
        let block_bytes = IMPORT_SIZE * std::mem::size_of::<i64>();
        assert_eq!(block_bytes, 65536);
        assert_eq!(IMPORT_SIZE, DEFAULT_BLOCK_CAPACITY);

        let dir = tempfile::tempdir().unwrap();
        let pool = BufferPool::new(budget);
        let blocks: Vec<_> = (0..INSERT_N)
            .map(|idx| {
                pool.allocate_persistent(
                    dir.path().join(format!("block_{idx}.bin")),
                    block_bytes,
                    BufferType::Persistent,
                )
            })
            .collect();

        // Insert: write `idx * 1000 + j` into every row of every block.
        for (idx, block) in blocks.iter().enumerate() {
            let handle = block.load().unwrap();
            {
                let mut guard = handle.data_mut();
                let data = guard.as_mut().unwrap();
                for j in 0..IMPORT_SIZE {
                    let value = (idx * 1000 + j) as i64;
                    data[j * 8..j * 8 + 8].copy_from_slice(&value.to_le_bytes());
                }
            }
            block.save().unwrap();
            // The budget bounds the resident payloads while GC runs.
            assert!(pool.used_bytes() <= budget);
        }
        assert_eq!(pool.used_bytes(), budget);

        // Scan: every block is still readable after eviction and spill.
        for (idx, block) in blocks.iter().enumerate() {
            let handle = block.load().unwrap();
            let guard = handle.data();
            let data = guard.as_ref().unwrap();
            let mut column = Column::new(LogicalType::BigInt).unwrap();
            column.initialize(ColumnVectorType::Flat, DEFAULT_VECTOR_SIZE);
            for j in 0..IMPORT_SIZE {
                let value = i64::from_le_bytes(data[j * 8..j * 8 + 8].try_into().unwrap());
                column.push_block_value(BlockValue::Int(value)).unwrap();
            }
            assert_eq!(column.size(), DEFAULT_BLOCK_CAPACITY);
            for j in 0..IMPORT_SIZE {
                assert_eq!(
                    column.get_value_by_index(j),
                    Some(BlockValue::Int((idx * 1000 + j) as i64))
                );
            }
            assert!(pool.used_bytes() <= budget);
        }

        // Cleanup: drop table + cleanup task.
        for block in &blocks {
            block.free().unwrap();
        }
        assert_eq!(pool.used_bytes(), 0);
        for block in &blocks {
            assert_eq!(block.rc(), 0);
            assert_eq!(block.status(), BufferStatus::Freed);
        }
    }

    /// C++: `BufferObjTest.test_hnsw_index_buffer_obj_shutdown` — the
    /// configured 32 MiB buffer-manager limit, index buffers loaded within it,
    /// and shutdown releasing every byte.
    ///
    /// The reference creates a table and an HNSW index through the catalog and
    /// transaction manager. The Rust buffer layer has no catalog or HNSW
    /// `MemIndex` wiring, so this port exercises the buffer-object lifecycle
    /// that the reference asserts (limit, load, unload, cleanup, rc == 0).
    #[test]
    fn hnsw_index_buffer_objects_are_released_on_shutdown() {
        let memory_limit = 8 * 4 * 128 * 8192;
        let pool = BufferPool::new(memory_limit);
        assert_eq!(pool.budget(), 8 * 4 * 128 * 8192);

        let index_buffers: Vec<_> = (0..2)
            .map(|_| pool.allocate(memory_limit / 2, BufferType::Persistent))
            .collect();
        for buffer in &index_buffers {
            let handle = buffer.load().unwrap();
            assert_eq!(buffer.status(), BufferStatus::Loaded);
            assert_eq!(buffer.rc(), 1);
            buffer.check_state().unwrap();
            drop(handle);
            assert_eq!(buffer.rc(), 0);
            assert_eq!(buffer.status(), BufferStatus::Unloaded);
            buffer.check_state().unwrap();
        }
        assert_eq!(pool.used_bytes(), memory_limit);

        // Shutdown: cleanup frees the index buffers.
        for buffer in &index_buffers {
            buffer.free().unwrap();
        }
        for buffer in &index_buffers {
            assert_eq!(buffer.rc(), 0);
            assert_eq!(buffer.status(), BufferStatus::Freed);
        }
        assert_eq!(pool.used_bytes(), 0);
    }
    /// C++: `BufferObjTest.DISABLED_SLOW_test_multiple_threads_read` — many
    /// readers load the same persistent buffer concurrently.
    #[test]
    #[ignore = "C++ DISABLED_SLOW_test_multiple_threads_read (run with --ignored)"]
    fn disabled_slow_test_multiple_threads_read() {
        let pool = BufferPool::new(64 * 1024);
        let b = pool.allocate(4096, BufferType::Persistent);
        {
            let h = b.load().unwrap();
            h.data_mut().as_mut().unwrap()[0] = 9;
        }
        let mut handles = Vec::new();
        for _ in 0..8 {
            let b = b.clone();
            handles.push(std::thread::spawn(move || {
                for _ in 0..100 {
                    let h = b.load().unwrap();
                    assert_eq!(h.data().as_ref().unwrap()[0], 9);
                }
            }));
        }
        for h in handles {
            h.join().unwrap();
        }
    }
}
