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

use std::collections::HashMap;
use std::path::PathBuf;
use std::sync::atomic::{AtomicU64, AtomicU8, AtomicUsize, Ordering};
use std::sync::{Arc, Mutex, MutexGuard};

use infinity_common::{Error, Result};

use crate::cpp_format::{BlockId, RawBlockColumn};

/// C++ `BufferStatus` (`src/storage/buffer/buffer_obj.cppm`).
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum BufferStatus {
    New,
    Loaded,
    Freed,
}

/// C++ `BufferType`: whether a payload outlives its last handle.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum BufferType {
    /// Backed by a spill file once its last handle is dropped.
    Ephemeral,
    /// Stays in memory until [`BufferObj::free`] is called.
    Persistent,
    /// Dropped together with its last handle.
    Transient,
}

/// The shared budget of a [`BufferPool`].
struct PoolState {
    budget: usize,
    used: usize,
}

static NEXT_BUFFER_ID: AtomicU64 = AtomicU64::new(0);

/// An owned byte buffer with the reference's `BufferObj` lifecycle.
///
/// C++: `BufferObj` (`src/storage/buffer/buffer_obj.cppm`). The payload is
/// allocated on `Load` and charged to the pool's budget; `rc()` counts the
/// live handles.
pub struct BufferObj {
    state: Arc<Mutex<PoolState>>,
    data: Mutex<Option<Vec<u8>>>,
    status: AtomicU8,
    kind: BufferType,
    rc: AtomicUsize,
    /// Spill/persist target. Ephemeral buffers get a temporary file so their
    /// contents survive the last handle (the reference spills to its temp dir).
    path: Option<PathBuf>,
    bytes: usize,
}

impl BufferObj {
    fn new(
        state: Arc<Mutex<PoolState>>,
        bytes: usize,
        kind: BufferType,
        path: Option<PathBuf>,
    ) -> Self {
        let path = path.or_else(|| match kind {
            BufferType::Ephemeral => {
                let id = NEXT_BUFFER_ID.fetch_add(1, Ordering::Relaxed);
                Some(std::env::temp_dir().join(format!("infinity_buffer_{id}.bin")))
            }
            _ => None,
        });
        Self {
            state,
            data: Mutex::new(None),
            status: AtomicU8::new(BufferStatus::New as u8),
            kind,
            rc: AtomicUsize::new(0),
            path,
            bytes,
        }
    }

    pub fn status(&self) -> BufferStatus {
        match self.status.load(Ordering::Acquire) {
            0 => BufferStatus::New,
            1 => BufferStatus::Loaded,
            _ => BufferStatus::Freed,
        }
    }

    pub fn kind(&self) -> BufferType {
        self.kind
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
    /// it. Fails with `Storage` when the pool's budget would be exceeded.
    pub fn load(&self) -> Result<BufferHandle<'_>> {
        let mut data = self.data.lock().unwrap();
        if data.is_none() {
            let mut state = self.state.lock().unwrap();
            if state.used + self.bytes > state.budget {
                return Err(Error::Storage("buffer manager out of memory".into()));
            }
            let payload = match &self.path {
                Some(p) if p.exists() => std::fs::read(p).map_err(|e| Error::io(p, e))?,
                _ => vec![0u8; self.bytes],
            };
            *data = Some(payload);
            state.used += self.bytes;
            self.status
                .store(BufferStatus::Loaded as u8, Ordering::Release);
        }
        drop(data);
        self.rc.fetch_add(1, Ordering::AcqRel);
        Ok(BufferHandle { obj: self })
    }

    /// C++ `BufferObj::Save`: writes a persistent (or spilled) payload back.
    pub fn save(&self) -> Result<()> {
        let Some(path) = self.path.clone() else {
            return Ok(());
        };
        let data = self.data.lock().unwrap();
        let Some(payload) = data.as_ref() else {
            return Ok(());
        };
        std::fs::write(&path, payload).map_err(|e| Error::io(path, e))
    }

    /// C++ `BufferObj::Free`: drops the payload and returns its budget.
    pub fn free(&self) {
        let mut data = self.data.lock().unwrap();
        if data.take().is_some() {
            let mut state = self.state.lock().unwrap();
            state.used = state.used.saturating_sub(self.bytes);
        }
        self.status
            .store(BufferStatus::Freed as u8, Ordering::Release);
    }

    fn release_handle(&self) {
        let rc = self.rc.fetch_sub(1, Ordering::AcqRel).saturating_sub(1);
        if rc > 0 {
            return;
        }
        match self.kind {
            BufferType::Persistent => {}
            BufferType::Ephemeral => {
                // Spill before releasing so a later Load sees the writes
                // (the reference flushes ephemeral buffers to its temp dir).
                let _ = self.save();
                self.free();
            }
            BufferType::Transient => self.free(),
        }
    }
}

impl Drop for BufferObj {
    fn drop(&mut self) {
        if self.kind == BufferType::Ephemeral {
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

    /// C++ `GetDataMut()`.
    pub fn data_mut(&self) -> MutexGuard<'_, Option<Vec<u8>>> {
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
            state: Arc::new(Mutex::new(PoolState { budget, used: 0 })),
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
        Arc::new(BufferObj::new(self.state.clone(), bytes, kind, None))
    }

    /// C++ `BufferManager::AllocateBufferObject` for a file-backed buffer.
    pub fn allocate_persistent(
        &self,
        path: impl Into<PathBuf>,
        bytes: usize,
        kind: BufferType,
    ) -> Arc<BufferObj> {
        Arc::new(BufferObj::new(
            self.state.clone(),
            bytes,
            kind,
            Some(path.into()),
        ))
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
        assert_eq!(pool.used_bytes(), 0);

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
        b1.free();
        b2.free();
        b3.free();
        assert_eq!(pool.used_bytes(), 0);
    }

    /// C++ `BufferType::kPersistent`: the payload stays loaded (and charged)
    /// after the last handle is dropped, until `Free`.
    #[test]
    fn persistent_buffers_outlive_their_handles() {
        let pool = BufferPool::new(1024);
        let b = pool.allocate(512, BufferType::Persistent);
        {
            let h = b.load().unwrap();
            h.data_mut().as_mut().unwrap()[0] = 7;
        }
        assert_eq!(b.rc(), 0);
        assert_eq!(b.status(), BufferStatus::Loaded);
        assert_eq!(pool.used_bytes(), 512);
        {
            let h = b.load().unwrap();
            assert_eq!(h.data().as_ref().unwrap()[0], 7);
        }
        b.free();
        assert_eq!(b.status(), BufferStatus::Freed);
        assert_eq!(pool.used_bytes(), 0);
    }
}
