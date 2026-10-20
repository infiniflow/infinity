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
//!
//! A [`FileWorker`] gives such a buffer its data and spill paths
//! (`DataFileWorker` / `VarFileWorker`). The pool reproduces the manager's
//! spill accounting: `temp_set_`, `PickForCleanup`/`RemoveClean` and the
//! `RequestSpace` budget — the surface `buffer_manager_ut.cpp` drives.

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
    /// Picked for cleanup (`PickForCleanup`); `RemoveClean` deletes the file.
    Clean,
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
    /// `kToMmap`: the payload is still resident but the next unload frees it
    /// and switches to the mapped form.
    ToMmap,
    /// `kMmap`: reads come from the mapped file, not from the budget.
    Mmap,
}

/// C++ `FileWorkerType`, as reported by `BufferObjectInfo`.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum FileWorkerType {
    Invalid,
    /// `DataFileWorker`: a fixed-size payload.
    DataFile,
    /// `VarFileWorker`: a payload that grows on append.
    VarFile,
}

/// C++ `FileWorker`: the payload file of a [`BufferObj`].
///
/// The reference has one worker subclass per payload kind; the port keeps the
/// two shapes the unit tests exercise: `DataFileWorker` (a fixed-size byte
/// buffer) and `VarFileWorker` (a `VarBuffer` that grows on append). A worker
/// owns the *data* path (`data_dir/file_dir/file_name`) and the *spill* path
/// (`temp_dir/file_dir/file_name`); `Save`/`MoveFile` move the spill copy to
/// the data path.
#[derive(Clone, Debug, PartialEq, Eq)]
pub struct FileWorker {
    data_dir: PathBuf,
    temp_dir: PathBuf,
    file_dir: PathBuf,
    file_name: String,
    buffer_size: usize,
    variable: bool,
}

impl FileWorker {
    /// C++ `DataFileWorker(data_dir, temp_dir, file_dir, file_name, size)`.
    pub fn data(
        data_dir: impl Into<PathBuf>,
        temp_dir: impl Into<PathBuf>,
        file_dir: impl Into<PathBuf>,
        file_name: impl Into<String>,
        buffer_size: usize,
    ) -> Self {
        Self {
            data_dir: data_dir.into(),
            temp_dir: temp_dir.into(),
            file_dir: file_dir.into(),
            file_name: file_name.into(),
            buffer_size,
            variable: false,
        }
    }

    /// C++ `VarFileWorker`: the payload starts empty and grows through
    /// [`BufferHandle::var_append`].
    pub fn var(
        data_dir: impl Into<PathBuf>,
        temp_dir: impl Into<PathBuf>,
        file_dir: impl Into<PathBuf>,
        file_name: impl Into<String>,
        buffer_size: usize,
    ) -> Self {
        Self {
            data_dir: data_dir.into(),
            temp_dir: temp_dir.into(),
            file_dir: file_dir.into(),
            file_name: file_name.into(),
            buffer_size,
            variable: true,
        }
    }

    /// C++ `FileWorker::GetFilePath`: the data path, also the key of
    /// `BufferManager::buffer_map_`.
    pub fn file_path(&self) -> PathBuf {
        self.data_dir.join(&self.file_dir).join(&self.file_name)
    }

    /// C++ `FileWorker::ChooseFileDir(true)`: the spill path.
    pub fn spill_path(&self) -> PathBuf {
        self.temp_dir.join(&self.file_dir).join(&self.file_name)
    }

    /// C++ `VarFileWorker::GetBufferSize` / `DataFileWorker::GetMemoryCost`.
    pub fn buffer_size(&self) -> usize {
        self.buffer_size
    }

    pub fn is_variable(&self) -> bool {
        self.variable
    }

    /// C++ `FileWorker::Type`.
    pub fn worker_type(&self) -> FileWorkerType {
        if self.variable {
            FileWorkerType::VarFile
        } else {
            FileWorkerType::DataFile
        }
    }
}

/// The shared budget of a [`BufferPool`].
struct PoolState {
    budget: usize,
    used: usize,
    /// C++ `BufferManager::lru_caches_`: the GC queues are sharded by
    /// `buffer id % lru_count`, and `RequestSpace` sweeps them round-robin so
    /// one hot shard cannot pin the whole budget.
    gc_shards: Vec<VecDeque<Weak<BufferObj>>>,
    /// C++ `BufferManager::round_robin_`: the shard the next sweep starts at.
    round_robin: usize,
    /// C++ `BufferManager::temp_set_`: payloads spilled to the temp dir and
    /// not yet moved to their data path.
    temps: Vec<Weak<BufferObj>>,
    /// C++ `BufferManager::clean_temp_set_`: spill files scheduled for
    /// deletion by [`BufferPool::remove_clean`].
    clean_temps: Vec<Weak<BufferObj>>,
    /// C++ `BufferManager::clean_list_`: buffers picked for cleanup whose data
    /// file `RemoveClean` deletes.
    clean: Vec<Weak<BufferObj>>,
    /// C++ `BufferManager::total_request_count_` / `cache_miss_count_`.
    total_request_count: u64,
    cache_miss_count: u64,
}

/// C++ `DEFAULT_BUFFER_MANAGER_LRU_COUNT` (`common/default_values.cppm:200`).
pub const DEFAULT_LRU_COUNT: usize = 7;

static NEXT_BUFFER_ID: AtomicU64 = AtomicU64::new(0);

/// An owned byte buffer with the reference's `BufferObj` lifecycle.
///
/// C++: `BufferObj` (`src/storage/buffer/buffer_obj.cppm`). The payload is
/// allocated on `Load` and charged to the pool's budget; `rc()` counts the
/// live handles. Dropping the last handle marks the buffer `Unloaded` and
/// queues it for GC (the reference's `gc_list_`); the payload is released by
/// [`BufferObj::free`], which the pool runs under memory pressure.
pub struct BufferObj {
    /// Object id (C++ `BufferObj::id`), for diagnostics.
    id: u64,
    state: Arc<Mutex<PoolState>>,
    data: Mutex<Option<Vec<u8>>>,
    status: AtomicU8,
    kind: AtomicU8,
    rc: AtomicUsize,
    /// C++ `obj_rc_`: owner references (`AddObjRc`/`PickForCleanup`).
    obj_rc: AtomicUsize,
    /// Spill/persist target of an anonymous buffer. File workers carry their
    /// own data/spill paths.
    path: Option<PathBuf>,
    /// C++ `file_worker_`; behind a lock because
    /// [`BufferObj::update_file_worker_info`] rewrites its size on restart.
    worker: Mutex<Option<FileWorker>>,
    /// Whether `path` is an auto-created temp file this object must delete.
    owns_path: bool,
    /// Charged bytes: the payload size (`GetMemoryCost`).
    bytes: AtomicUsize,
    /// C++ `DataFileWorker::data_size_`: bytes `Save` writes.
    data_size: AtomicUsize,
    /// Assigned by the pool so the GC queue can hold a weak reference.
    self_ref: OnceLock<Weak<BufferObj>>,
}

const KIND_EPHEMERAL: u8 = 0;
const KIND_PERSISTENT: u8 = 1;
const KIND_TEMP: u8 = 2;
const KIND_TO_MMAP: u8 = 3;
const KIND_MMAP: u8 = 4;

impl BufferObj {
    fn new(
        state: Arc<Mutex<PoolState>>,
        bytes: usize,
        kind: BufferType,
        path: Option<PathBuf>,
        worker: Option<FileWorker>,
        status: BufferStatus,
    ) -> Self {
        let id = NEXT_BUFFER_ID.fetch_add(1, Ordering::Relaxed);
        let (path, owns_path) = match (path, &worker, kind) {
            (Some(path), _, _) => (Some(path), false),
            (None, Some(_), _) => (None, false),
            (None, None, BufferType::Ephemeral | BufferType::Temp) => (
                Some(std::env::temp_dir().join(format!("infinity_buffer_{id}.bin"))),
                true,
            ),
            (None, None, BufferType::Persistent | BufferType::ToMmap | BufferType::Mmap) => {
                (None, false)
            }
        };
        Self {
            id,
            state,
            data: Mutex::new(None),
            status: AtomicU8::new(status as u8),
            kind: AtomicU8::new(match kind {
                BufferType::Ephemeral => KIND_EPHEMERAL,
                BufferType::Persistent => KIND_PERSISTENT,
                BufferType::Temp => KIND_TEMP,
                BufferType::ToMmap => KIND_TO_MMAP,
                BufferType::Mmap => KIND_MMAP,
            }),
            rc: AtomicUsize::new(0),
            obj_rc: AtomicUsize::new(0),
            path,
            worker: Mutex::new(worker),
            owns_path,
            bytes: AtomicUsize::new(bytes),
            data_size: AtomicUsize::new(bytes),
            self_ref: OnceLock::new(),
        }
    }

    /// C++ `BufferObj::id`.
    pub fn id(&self) -> u64 {
        self.id
    }

    pub fn status(&self) -> BufferStatus {
        match self.status.load(Ordering::Acquire) {
            0 => BufferStatus::New,
            1 => BufferStatus::Loaded,
            2 => BufferStatus::Unloaded,
            3 => BufferStatus::Freed,
            _ => BufferStatus::Clean,
        }
    }

    pub fn kind(&self) -> BufferType {
        match self.kind.load(Ordering::Acquire) {
            KIND_EPHEMERAL => BufferType::Ephemeral,
            KIND_PERSISTENT => BufferType::Persistent,
            KIND_TEMP => BufferType::Temp,
            KIND_TO_MMAP => BufferType::ToMmap,
            _ => BufferType::Mmap,
        }
    }

    /// C++ `BufferObj::SetType` (only the tests and
    /// [`BufferPool::change_buffer_object_state`] use it).
    pub fn set_type(&self, kind: BufferType) {
        let kind = match kind {
            BufferType::Ephemeral => KIND_EPHEMERAL,
            BufferType::Persistent => KIND_PERSISTENT,
            BufferType::Temp => KIND_TEMP,
            BufferType::ToMmap => KIND_TO_MMAP,
            BufferType::Mmap => KIND_MMAP,
        };
        self.kind.store(kind, Ordering::Release);
    }

    /// C++ `BufferObj::SetStatus`.
    pub fn set_status(&self, status: BufferStatus) {
        self.status.store(status as u8, Ordering::Release);
    }

    /// C++ `FileWorker::Type` of this object's worker (`kInvalid` when it has
    /// none).
    pub fn worker_type(&self) -> FileWorkerType {
        self.worker_info()
            .map_or(FileWorkerType::Invalid, |w| w.worker_type())
    }

    /// The clone of the file worker this object was created with.
    fn worker_info(&self) -> Option<FileWorker> {
        self.worker.lock().unwrap().clone()
    }

    /// C++ `BufferObj::UpdateFileWorkerInfo`: on restart the caller hands in a
    /// fresh worker; a `VarFileWorker` keeps the size it grew to.
    pub fn update_file_worker_info(&self, worker: FileWorker) {
        let mut slot = self.worker.lock().unwrap();
        if let Some(current) = slot.as_mut() {
            if current.variable && worker.variable {
                current.buffer_size = worker.buffer_size;
                self.bytes.store(worker.buffer_size, Ordering::Release);
            }
        }
    }

    /// C++ `BufferObj::AddBufferSize`: charge `add_size` to the pool and
    /// report whether the request was satisfied. Only a var-file payload
    /// grows this way.
    pub fn add_buffer_size(&self, add_size: usize) -> Result<bool> {
        if self.worker_type() != FileWorkerType::VarFile {
            return Err(Error::Storage("Invalid file worker type".into()));
        }
        Ok(self.request_space(add_size))
    }

    /// C++ `BufferObj::rc()`: live handles.
    pub fn rc(&self) -> usize {
        self.rc.load(Ordering::Acquire)
    }

    /// C++ `BufferObj::GetBufferSize` / `GetMemoryCost`.
    pub fn size(&self) -> usize {
        self.bytes.load(Ordering::Acquire)
    }

    pub fn path(&self) -> Option<PathBuf> {
        self.data_path()
    }

    fn data_path(&self) -> Option<PathBuf> {
        self.worker_info()
            .map(|w| w.file_path())
            .or_else(|| self.path.clone())
    }

    fn spill_path(&self) -> Option<PathBuf> {
        self.worker_info()
            .map(|w| w.spill_path())
            .or_else(|| self.path.clone())
    }

    /// C++ `BufferObj::Load`: allocates (or reads back) the payload and pins
    /// it. Under memory pressure the pool first frees GC-queued buffers; if
    /// none can be freed the request fails with `Storage`.
    pub fn load(&self) -> Result<BufferHandle<'_>> {
        self.count_request();
        if self.kind() == BufferType::Mmap {
            return self.load_mmap();
        }
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
            let need = self.size();
            if !self.request_space(need) {
                // C++ leaves the failed request charged; the Rust pool keeps
                // `used` equal to the resident bytes, so roll it back.
                let mut state = self.state.lock().unwrap();
                state.used = state.used.saturating_sub(need);
                return Err(Error::Storage("Out of memory.".into()));
            }
            let payload = match (self.status(), self.kind()) {
                (BufferStatus::Freed, BufferType::Temp) => {
                    self.read_from(self.spill_path(), need)?
                }
                (BufferStatus::Freed, BufferType::Persistent) => {
                    self.read_from(self.data_path(), need)?
                }
                _ => vec![0u8; need],
            };
            self.bytes.store(payload.len().max(need), Ordering::Release);
            *data = Some(payload);
        }
        self.status
            .store(BufferStatus::Loaded as u8, Ordering::Release);
        drop(data);
        self.rc.fetch_add(1, Ordering::AcqRel);
        Ok(BufferHandle { obj: self })
    }

    /// C++ `BufferManager::AddRequestCount` / `AddCacheMissCount`: a `Freed`
    /// or `New` load misses; an `Mmap` payload is mapped, so it is never
    /// counted as a miss (the reference returns before its status switch).
    fn count_request(&self) {
        let miss = match self.status() {
            BufferStatus::Freed | BufferStatus::New => self.kind() != BufferType::Mmap,
            _ => false,
        };
        let mut state = self.state.lock().unwrap();
        state.total_request_count += 1;
        if miss {
            state.cache_miss_count += 1;
        }
    }

    /// C++ `BufferObj::LoadNoLock` for a `kMmap` payload: map the file (once)
    /// and pin it. The mapped bytes are *not* charged to the budget.
    fn load_mmap(&self) -> Result<BufferHandle<'_>> {
        let mut data = self.data.lock().unwrap();
        match self.status() {
            BufferStatus::Unloaded | BufferStatus::Loaded => {}
            BufferStatus::Freed | BufferStatus::New => {
                drop(data);
                self.mmap_()?;
                data = self.data.lock().unwrap();
            }
            status => {
                return Err(Error::Storage(format!(
                    "Invalid status: {status:?} for a mapped buffer"
                )))
            }
        }
        self.status
            .store(BufferStatus::Loaded as u8, Ordering::Release);
        drop(data);
        self.rc.fetch_add(1, Ordering::AcqRel);
        Ok(BufferHandle { obj: self })
    }

    /// C++ `BufferObj::ToMmap`: this buffer will read from its file instead of
    /// the budget. A loaded payload is marked `ToMmap` so the mapping happens
    /// when its last handle drops; an unloaded one is freed and mapped here.
    pub fn to_mmap(&self) -> Result<()> {
        if matches!(
            self.kind(),
            BufferType::Mmap | BufferType::ToMmap | BufferType::Ephemeral
        ) {
            return Ok(());
        }
        if self.kind() != BufferType::Persistent {
            return Err(Error::Storage(format!(
                "Invalid buffer type: {:?}",
                self.kind()
            )));
        }
        match self.status() {
            BufferStatus::Loaded => self.kind.store(KIND_TO_MMAP, Ordering::Release),
            BufferStatus::Unloaded => {
                self.remove_from_gc_queue();
                self.release_resident();
                self.status
                    .store(BufferStatus::Freed as u8, Ordering::Release);
                self.kind.store(KIND_MMAP, Ordering::Release);
            }
            BufferStatus::Freed => self.kind.store(KIND_MMAP, Ordering::Release),
            status => {
                return Err(Error::Storage(format!(
                    "Invalid status: {status:?} for ToMmap"
                )))
            }
        }
        Ok(())
    }

    /// C++ `FileWorker::Mmap`: map the data file into `data`. Idempotent.
    pub fn mmap_(&self) -> Result<()> {
        let mut data = self.data.lock().unwrap();
        if data.is_some() {
            return Ok(());
        }
        let path = self.data_path();
        let payload = match &path {
            Some(path) if path.exists() => std::fs::read(path).map_err(|e| Error::io(path, e))?,
            _ => vec![0u8; self.size()],
        };
        self.bytes.store(payload.len(), Ordering::Release);
        *data = Some(payload);
        self.kind.store(KIND_MMAP, Ordering::Release);
        Ok(())
    }

    /// C++ `FileWorker::Munmap`: drop the mapped view (the file stays).
    pub fn munmap_(&self) {
        let mut data = self.data.lock().unwrap();
        *data = None;
    }

    /// C++ `BufferObj::SaveSnapshot`: copy this payload into a table
    /// snapshot directory.
    ///
    /// A resident payload is written from memory; a `Temp` payload from its
    /// spill file; a `Persistent`/`Mmap` one from its data file. Returns
    /// whether the snapshot copy was made.
    pub fn save_snapshot(&self, dest: &std::path::Path) -> Result<bool> {
        let source = match self.status() {
            BufferStatus::Loaded | BufferStatus::Unloaded => {
                if self.kind() == BufferType::Mmap {
                    self.data_path()
                } else {
                    self.write_to(Some(dest.to_path_buf()), &self.resident()?)?;
                    return Ok(true);
                }
            }
            BufferStatus::Freed => match self.kind() {
                BufferType::Temp => self.spill_path(),
                BufferType::Mmap | BufferType::Persistent => self.data_path(),
                kind => {
                    return Err(Error::Storage(format!(
                        "Invalid buffer type for snapshot: {kind:?}"
                    )))
                }
            },
            status => return Err(Error::Storage(format!("Invalid buffer status: {status:?}"))),
        };
        let Some(source) = source else {
            return Err(Error::Storage("Snapshot source has no path".into()));
        };
        if let Some(parent) = dest.parent() {
            std::fs::create_dir_all(parent).map_err(|e| Error::io(parent, e))?;
        }
        std::fs::copy(&source, dest).map_err(|e| Error::io(dest, e))?;
        Ok(true)
    }

    /// The resident payload, or an error when it was freed.
    fn resident(&self) -> Result<Vec<u8>> {
        self.data
            .lock()
            .unwrap()
            .clone()
            .ok_or_else(|| Error::Storage("Buffer is not resident".into()))
    }

    /// C++ `BufferObj::Save`: writes an ephemeral payload to its data path and
    /// turns it persistent, or moves a spilled `Temp` payload. Returns the
    /// reference's `write` flag (`false` when only a file move happened).
    pub fn save(&self) -> Result<bool> {
        let mut write = false;
        let mut data = self.data.lock().unwrap();
        match self.kind() {
            BufferType::Persistent => {
                // A file-backed persistent payload is written in place.
                if let Some(payload) = data.as_ref() {
                    self.write_to(self.data_path(), payload)?;
                }
                return Ok(false);
            }
            BufferType::Temp => {
                drop(data);
                self.pool_move_temp();
                self.move_file()?;
                self.kind.store(KIND_PERSISTENT, Ordering::Release);
                return Ok(false);
            }
            BufferType::Ephemeral => match self.status() {
                BufferStatus::New => {
                    let need = self.size();
                    if !self.request_space(need) {
                        return Err(Error::Storage("Out of memory.".into()));
                    }
                    *data = Some(vec![0u8; need]);
                    self.push_gc_queue();
                    self.status
                        .store(BufferStatus::Unloaded as u8, Ordering::Release);
                    if let Some(payload) = data.as_ref() {
                        self.write_to(self.data_path(), payload)?;
                    }
                    self.kind.store(KIND_PERSISTENT, Ordering::Release);
                    write = true;
                }
                BufferStatus::Loaded | BufferStatus::Unloaded => {
                    if let Some(payload) = data.as_ref() {
                        self.write_to(self.data_path(), payload)?;
                    }
                    self.kind.store(KIND_PERSISTENT, Ordering::Release);
                    write = true;
                }
                BufferStatus::Freed => {
                    drop(data);
                    self.move_file()?;
                }
                BufferStatus::Clean => {
                    return Err(Error::Storage("Invalid buffer status: Clean".into()));
                }
            },
            // `Save` only touches ephemeral/temp payloads; a mapped buffer is
            // already backed by its file.
            BufferType::ToMmap | BufferType::Mmap => return Ok(false),
        }
        Ok(write)
    }

    /// C++ `BufferObj::Free`: drops the payload and returns its budget. Only
    /// an `Unloaded` buffer is freeable; an ephemeral payload is spilled to
    /// its temp file (and becomes `Temp`) first.
    pub fn free(&self) -> Result<()> {
        self.remove_from_gc_queue();
        let data = self.data.lock().unwrap();
        self.free_locked(data)
    }

    /// C++ `BufferObj::Free` with its `try_lock`: a buffer another thread is
    /// loading or cleaning cannot be freed now (`false`); the GC skips it.
    fn try_free(&self) -> bool {
        match self.data.try_lock() {
            Ok(data) => self.free_locked(data).is_ok(),
            Err(_) => false,
        }
    }

    fn free_locked(&self, mut data: MutexGuard<'_, Option<Vec<u8>>>) -> Result<()> {
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
                    self.write_to(self.spill_path(), payload)?;
                }
                self.kind.store(KIND_TEMP, Ordering::Release);
                self.pool_add_temp();
            }
            kind => return Err(Error::Storage(format!("Invalid buffer type: {kind:?}"))),
        }
        let freed = data.take().is_some();
        drop(data);
        if freed {
            self.release_budget();
        }
        self.status
            .store(BufferStatus::Freed as u8, Ordering::Release);
        Ok(())
    }

    /// Return this buffer's charged bytes to the pool.
    fn release_budget(&self) {
        let mut state = self.state.lock().unwrap();
        state.used = state.used.saturating_sub(self.size());
    }

    /// C++ `BufferObj::FreeInMemory` plus the pool accounting: drop the
    /// resident payload and return its budget. The mapped bytes of a `kMmap`
    /// buffer are never charged, so they are not returned here.
    fn release_resident(&self) {
        let freed = self.data.lock().unwrap().take().is_some();
        if freed && self.kind() != BufferType::Mmap {
            self.release_budget();
        }
    }

    /// C++ `CheckState`: the status/reference-count invariants.
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
            BufferStatus::Unloaded | BufferStatus::Freed | BufferStatus::Clean => {
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

    /// C++ `BufferObj::AddObjRc`.
    pub fn add_obj_rc(&self) {
        self.obj_rc.fetch_add(1, Ordering::AcqRel);
    }

    /// C++ `BufferObj::SubObjRc`.
    pub fn sub_obj_rc(&self) -> Result<()> {
        let prev = self.obj_rc.fetch_sub(1, Ordering::AcqRel);
        if prev == 0 {
            self.obj_rc.fetch_add(1, Ordering::AcqRel);
            return Err(Error::Storage("SubObjRc: obj_rc_ is 0".into()));
        }
        if prev > 1 {
            return Ok(());
        }
        self.status
            .store(BufferStatus::Clean as u8, Ordering::Release);
        self.pool_add_clean();
        Ok(())
    }

    /// C++ `BufferObj::PickForCleanup`: the last owner releases the buffer,
    /// freeing its memory and scheduling the data file for deletion.
    pub fn pick_for_cleanup(&self) -> Result<()> {
        let prev = self.obj_rc.fetch_sub(1, Ordering::AcqRel);
        if prev == 0 {
            self.obj_rc.fetch_add(1, Ordering::AcqRel);
            return Err(Error::Storage("PickForCleanup: obj_rc_ is 0".into()));
        }
        if prev > 1 {
            return Ok(());
        }
        // C++ returns early for `kMmap`: unmap, schedule the file for
        // deletion, and never touch the budget (the bytes were not charged).
        if self.kind() == BufferType::Mmap {
            self.munmap_();
            self.status
                .store(BufferStatus::Clean as u8, Ordering::Release);
            self.pool_add_clean();
            return Ok(());
        }
        match self.status() {
            BufferStatus::Clean => return Ok(()),
            BufferStatus::New | BufferStatus::Freed => {}
            BufferStatus::Unloaded => {
                let freed = self.data.lock().unwrap().take().is_some();
                if freed {
                    self.release_budget();
                }
            }
            BufferStatus::Loaded => {
                return Err(Error::Storage(
                    "PickForCleanup: buffer is still loaded".into(),
                ));
            }
        }
        self.status
            .store(BufferStatus::Clean as u8, Ordering::Release);
        self.pool_add_clean();
        if self.kind() == BufferType::Temp {
            self.pool_remove_temp();
        }
        Ok(())
    }

    /// C++ `BufferObj::CleanupFile`: `RemoveClean` deletes the data file.
    fn cleanup_file(&self) -> Result<()> {
        if self.status() != BufferStatus::Clean {
            return Err(Error::Storage("Invalid status".into()));
        }
        if let Some(path) = self.data_path() {
            match std::fs::remove_file(&path) {
                Ok(()) => {}
                Err(e) if e.kind() == std::io::ErrorKind::NotFound => {}
                Err(e) => return Err(Error::io(&path, e)),
            }
        }
        Ok(())
    }

    /// C++ `BufferObj::CleanupTempFile`: delete the spill file unless the
    /// payload is still waiting to be moved (`kTemp`).
    fn cleanup_temp_file(&self) {
        if self.kind() == BufferType::Temp {
            return;
        }
        if let Some(path) = self.spill_path() {
            if path.exists() {
                let _ = std::fs::remove_file(path);
            }
        }
    }

    /// C++ `DataFileWorker::SetDataSize`.
    pub fn set_data_size(&self, size: usize) {
        self.data_size.store(size, Ordering::Release);
    }

    fn read_from(&self, path: Option<PathBuf>, expected: usize) -> Result<Vec<u8>> {
        let Some(path) = path else {
            return Ok(vec![0u8; expected]);
        };
        let mut payload = std::fs::read(&path).map_err(|e| Error::io(&path, e))?;
        if payload.len() < expected {
            payload.resize(expected, 0);
        }
        Ok(payload)
    }

    fn write_to(&self, path: Option<PathBuf>, payload: &[u8]) -> Result<()> {
        let Some(path) = path else {
            return Ok(());
        };
        if let Some(parent) = path.parent() {
            std::fs::create_dir_all(parent).map_err(|e| Error::io(parent, e))?;
        }
        let len = if self
            .worker_info()
            .is_some_and(|worker| !worker.is_variable())
        {
            self.data_size.load(Ordering::Acquire).min(payload.len())
        } else {
            payload.len()
        };
        std::fs::write(&path, &payload[..len]).map_err(|e| Error::io(&path, e))
    }

    fn move_file(&self) -> Result<()> {
        let (Some(src), Some(dst)) = (self.spill_path(), self.data_path()) else {
            return Ok(());
        };
        if src == dst {
            return Ok(());
        }
        if !src.exists() {
            return Err(Error::Storage(format!("File not found: {}", src.display())));
        }
        if let Some(parent) = dst.parent() {
            std::fs::create_dir_all(parent).map_err(|e| Error::io(parent, e))?;
        }
        std::fs::rename(&src, &dst).map_err(|e| Error::io(&dst, e))
    }

    /// C++ `BufferManager::RequestSpace` + `LRUCache::RequestSpace`: charge
    /// `needed` bytes, freeing GC-queued buffers until the budget holds.
    ///
    /// Mirrors the reference's arithmetic, including charging the request even
    /// when it fails (the `varfile_test` accounting assertion relies on it).
    fn request_space(&self, needed: usize) -> bool {
        let free_space = {
            let mut state = self.state.lock().unwrap();
            let free_space = state.budget.wrapping_sub(state.used);
            if free_space >= needed {
                state.used += needed;
                return true;
            }
            free_space
        };
        let mut freed = 0;
        // C++ `BufferManager::RequestSpace` sweeps the LRU shards round-robin,
        // one full round at most: each shard frees from the front of its GC
        // queue until the request is satisfied. A buffer another thread is
        // loading or cleaning is skipped (`Free` returns false) and stays
        // queued for a later pass.
        let (mut index, shards) = {
            let state = self.state.lock().unwrap();
            (state.round_robin, state.gc_shards.len())
        };
        let mut visited = 0;
        while freed + free_space < needed && visited < shards {
            freed += self.drain_shard(index, needed - freed - free_space);
            index = (index + 1) % shards;
            visited += 1;
        }
        let mut state = self.state.lock().unwrap();
        state.round_robin = if shards == 0 { 0 } else { index };
        state.used = state.used.wrapping_add(needed);
        freed + free_space >= needed
    }

    /// C++ `LRUCache::RequestSpace` for one shard: free up to `want` bytes of
    /// queued buffers, leaving the ones that cannot be freed in place.
    fn drain_shard(&self, index: usize, want: usize) -> usize {
        let pass = {
            let state = self.state.lock().unwrap();
            state.gc_shards.get(index).map_or(0, VecDeque::len)
        };
        let mut freed = 0;
        for _ in 0..pass {
            if freed >= want {
                break;
            }
            let victim = {
                let mut state = self.state.lock().unwrap();
                state.gc_shards.get_mut(index).and_then(VecDeque::pop_front)
            };
            let Some(victim) = victim else { break };
            let Some(obj) = victim.upgrade() else {
                continue;
            };
            let size = obj.size();
            if obj.try_free() {
                freed += size;
            } else {
                let mut state = self.state.lock().unwrap();
                if let Some(shard) = state.gc_shards.get_mut(index) {
                    shard.push_back(victim);
                }
            }
        }
        freed
    }

    /// C++ `BufferManager::LRUIdx`: which shard this buffer's GC entry lives
    /// in.
    fn lru_index(&self) -> usize {
        let shards = self.state.lock().unwrap().gc_shards.len();
        if shards == 0 {
            0
        } else {
            self.id as usize % shards
        }
    }

    fn push_gc_queue(&self) {
        let Some(me) = self.self_ref.get() else {
            return;
        };
        let index = self.lru_index();
        let mut state = self.state.lock().unwrap();
        if let Some(shard) = state.gc_shards.get_mut(index) {
            shard.push_back(me.clone());
        }
    }

    fn remove_from_gc_queue(&self) {
        let Some(me) = self.self_ref.get() else {
            return;
        };
        let index = self.lru_index();
        let mut state = self.state.lock().unwrap();
        if let Some(shard) = state.gc_shards.get_mut(index) {
            shard.retain(|weak| !Weak::ptr_eq(weak, me));
        }
    }

    /// C++ `BufferManager::AddTemp`.
    fn pool_add_temp(&self) {
        let Some(me) = self.self_ref.get() else {
            return;
        };
        let mut state = self.state.lock().unwrap();
        if !state.temps.iter().any(|weak| Weak::ptr_eq(weak, me)) {
            state.temps.push(me.clone());
        }
        state.clean_temps.retain(|weak| !Weak::ptr_eq(weak, me));
    }

    /// C++ `BufferManager::RemoveTemp`: the spill file is relinquished; its
    /// deletion is deferred to `RemoveClean`.
    fn pool_remove_temp(&self) {
        let Some(me) = self.self_ref.get() else {
            return;
        };
        let mut state = self.state.lock().unwrap();
        state.temps.retain(|weak| !Weak::ptr_eq(weak, me));
        if !state.clean_temps.iter().any(|weak| Weak::ptr_eq(weak, me)) {
            state.clean_temps.push(me.clone());
        }
    }

    /// C++ `BufferManager::MoveTemp`: `Save` consumes the spill file.
    fn pool_move_temp(&self) {
        let Some(me) = self.self_ref.get() else {
            return;
        };
        let mut state = self.state.lock().unwrap();
        state.temps.retain(|weak| !Weak::ptr_eq(weak, me));
    }

    /// C++ `BufferManager::AddToCleanList`.
    fn pool_add_clean(&self) {
        let Some(me) = self.self_ref.get() else {
            return;
        };
        let mut state = self.state.lock().unwrap();
        if !state.clean.iter().any(|weak| Weak::ptr_eq(weak, me)) {
            state.clean.push(me.clone());
        }
    }

    /// C++ `BufferObj::UnloadInner`: the last handle leaves the payload
    /// resident but queues the buffer for GC. The two mmap kinds are special:
    /// `ToMmap` frees now and becomes a mapped buffer, `Mmap` only drops the
    /// mapped view.
    fn release_handle(&self) {
        let rc = self.rc.fetch_sub(1, Ordering::AcqRel).saturating_sub(1);
        if rc > 0 {
            return;
        }
        match self.kind() {
            BufferType::ToMmap => {
                self.remove_from_gc_queue();
                self.release_resident();
                self.status
                    .store(BufferStatus::Freed as u8, Ordering::Release);
                self.kind.store(KIND_MMAP, Ordering::Release);
            }
            BufferType::Mmap => {
                self.status
                    .store(BufferStatus::Unloaded as u8, Ordering::Release);
            }
            _ => {
                self.status
                    .store(BufferStatus::Unloaded as u8, Ordering::Release);
                self.push_gc_queue();
            }
        }
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

    /// C++ `GetDataMut()` / `BufferObj::GetMutPointer`. Taking a mutable
    /// pointer makes the buffer ephemeral again, so unflushed writes are
    /// spilled by the GC rather than silently kept persistent. A spilled
    /// payload is relinquished from the temp set; `RemoveClean` deletes the
    /// spill file once the buffer is saved again.
    ///
    /// A `kMmap` payload is first charged to the budget and copied out of the
    /// file (the reference's `RequestSpace` + `ReadFromFile(false)`); a
    /// failed request leaves the pool over its budget, where the reference
    /// aborts the process.
    pub fn data_mut(&self) -> MutexGuard<'_, Option<Vec<u8>>> {
        if self.obj.kind() == BufferType::Temp {
            self.obj.pool_remove_temp();
        } else if self.obj.kind() == BufferType::Mmap {
            // `RequestSpace` charges the payload even when the GC cannot free
            // enough for it, exactly like the reference (which aborts there).
            let need = self.obj.size();
            self.obj.request_space(need);
        }
        self.obj.kind.store(KIND_EPHEMERAL, Ordering::Release);
        self.obj.data.lock().unwrap()
    }

    /// C++ `VarBuffer::Append` for a `VarFileWorker` payload: appends `bytes`
    /// and charges the growth to the pool. Returns the reference's
    /// `free_success` — the data is appended even when the request fails.
    pub fn var_append(&self, bytes: &[u8]) -> bool {
        let mut data = self.obj.data.lock().unwrap();
        let payload = data
            .as_mut()
            .expect("var_append requires a loaded buffer payload");
        let success = self.obj.request_space(bytes.len());
        payload.extend_from_slice(bytes);
        self.obj.bytes.store(payload.len(), Ordering::Release);
        self.obj.kind.store(KIND_EPHEMERAL, Ordering::Release);
        success
    }

    /// C++ `VarBuffer::TotalSize`.
    pub fn var_total_size(&self) -> usize {
        let data = self.obj.data.lock().unwrap();
        data.as_ref().map_or(0, Vec::len)
    }

    /// C++ `VarBuffer::Get(offset, size)`.
    pub fn var_get(&self, offset: usize, size: usize) -> Result<Option<Vec<u8>>> {
        if size == 0 {
            return Ok(None);
        }
        let data = self.obj.data.lock().unwrap();
        let payload = data.as_ref().expect("var_get requires a loaded payload");
        if offset + size > payload.len() {
            return Err(Error::Invalid(format!(
                "offset {offset} and size {size} is out of range [0, {})",
                payload.len()
            )));
        }
        Ok(Some(payload[offset..offset + size].to_vec()))
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

/// C++ `BufferObjectInfo` (`buffer/buffer_obj.cppm:46`) — one row of
/// `SHOW BUFFERS` / the diagnostics dump.
#[derive(Clone, Debug, PartialEq, Eq)]
pub struct BufferObjectInfo {
    pub object_path: PathBuf,
    pub buffered_status: BufferStatus,
    pub buffered_type: BufferType,
    pub file_type: FileWorkerType,
    pub object_size: usize,
}

/// Allocates [`BufferObj`]s under one memory limit.
///
/// C++: the allocation/accounting half of `BufferManager`.
pub struct BufferPool {
    state: Arc<Mutex<PoolState>>,
    /// C++ `BufferManager::buffer_map_`: file-backed buffers keyed by their
    /// data path, so a second `GetBufferObject` for the same file returns the
    /// existing object instead of allocating a new one.
    objects: Mutex<HashMap<PathBuf, Arc<BufferObj>>>,
    /// C++ `BufferManager::lru_caches_.size()`.
    lru_count: usize,
}

impl BufferPool {
    pub fn new(budget: usize) -> Self {
        Self::with_lru_count(budget, DEFAULT_LRU_COUNT)
    }

    /// C++ `BufferManager(memory_limit, …, lru_count)`.
    pub fn with_lru_count(budget: usize, lru_count: usize) -> Self {
        let lru_count = lru_count.max(1);
        Self {
            state: Arc::new(Mutex::new(PoolState {
                budget,
                used: 0,
                gc_shards: (0..lru_count).map(|_| VecDeque::new()).collect(),
                round_robin: 0,
                temps: Vec::new(),
                clean_temps: Vec::new(),
                clean: Vec::new(),
                total_request_count: 0,
                cache_miss_count: 0,
            })),
            objects: Mutex::new(HashMap::new()),
            lru_count,
        }
    }

    pub fn budget(&self) -> usize {
        self.state.lock().unwrap().budget
    }

    /// C++ `BufferManager::memory_usage`.
    pub fn used_bytes(&self) -> usize {
        self.state.lock().unwrap().used
    }

    /// C++ `BufferManager::TempSetSize`.
    pub fn temp_set_size(&self) -> usize {
        self.state.lock().unwrap().temps.len()
    }

    /// C++ `BufferManager::BufferedObjectCount`.
    pub fn buffered_object_count(&self) -> usize {
        self.objects.lock().unwrap().len()
    }

    /// C++ `BufferManager::WaitingGCObjectCount`: one count per LRU shard.
    pub fn waiting_gc_object_count(&self) -> Vec<usize> {
        let state = self.state.lock().unwrap();
        state.gc_shards.iter().map(VecDeque::len).collect()
    }

    /// C++ `BufferManager::TotalRequestCount` / `CacheMissCount`.
    pub fn total_request_count(&self) -> u64 {
        self.state.lock().unwrap().total_request_count
    }

    pub fn cache_miss_count(&self) -> u64 {
        self.state.lock().unwrap().cache_miss_count
    }

    /// C++ `BufferManager::lru_caches_.size()`.
    pub fn lru_count(&self) -> usize {
        self.lru_count
    }

    /// C++ `BufferManager::GetBufferObject(const std::string &file_path)` —
    /// the object already tracking that path, if any.
    pub fn get_object_by_path(&self, path: impl AsRef<std::path::Path>) -> Option<Arc<BufferObj>> {
        self.objects.lock().unwrap().get(path.as_ref()).cloned()
    }

    /// C++ `BufferManager::ChangeBufferObjectState`: after a restart the file
    /// on disk is the truth, so the object becomes a freed persistent buffer.
    pub fn change_buffer_object_state(&self, path: impl AsRef<std::path::Path>) {
        if let Some(obj) = self.get_object_by_path(path) {
            obj.set_type(BufferType::Persistent);
            obj.set_status(BufferStatus::Freed);
        }
    }

    /// C++ `BufferManager::GetBufferObjectsInfo`.
    pub fn get_buffer_objects_info(&self) -> Vec<BufferObjectInfo> {
        let objects = self.objects.lock().unwrap();
        objects
            .iter()
            .map(|(path, obj)| BufferObjectInfo {
                object_path: path.clone(),
                buffered_status: obj.status(),
                buffered_type: obj.kind(),
                file_type: obj.worker_type(),
                object_size: obj.size(),
            })
            .collect()
    }

    /// C++ `BufferManager::RemoveBufferObjects`: drop the objects tracking
    /// `paths` from the dedupe map (their files stay), erroring on a path that
    /// is not tracked.
    pub fn remove_buffer_objects(&self, paths: &[PathBuf]) -> Result<()> {
        let mut objects = self.objects.lock().unwrap();
        for path in paths {
            if objects.remove(path).is_none() {
                return Err(Error::Storage(format!(
                    "BufferManager::RemoveBufferObjects: object {} not found.",
                    path.display()
                )));
            }
        }
        Ok(())
    }

    /// C++ `BufferManager::AllocateBufferObject` for an anonymous buffer.
    pub fn allocate(&self, bytes: usize, kind: BufferType) -> Arc<BufferObj> {
        self.register(BufferObj::new(
            self.state.clone(),
            bytes,
            kind,
            None,
            None,
            BufferStatus::New,
        ))
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
            None,
            BufferStatus::New,
        ))
    }

    /// C++ `BufferManager::AllocateBufferObject(FileWorker)`: an ephemeral
    /// buffer registered under its data path.
    pub fn allocate_file(&self, worker: FileWorker) -> Result<Arc<BufferObj>> {
        let key = worker.file_path();
        let mut objects = self.objects.lock().unwrap();
        if objects.contains_key(&key) {
            return Err(Error::Storage(format!(
                "BufferManager::Allocate: file {} already exists.",
                key.display()
            )));
        }
        let bytes = worker.buffer_size();
        let obj = self.register(BufferObj::new(
            self.state.clone(),
            bytes,
            BufferType::Ephemeral,
            None,
            Some(worker),
            BufferStatus::New,
        ));
        objects.insert(key, obj.clone());
        Ok(obj)
    }

    /// C++ `BufferManager::GetBufferObject(FileWorker)`: the buffer already
    /// tracking this file path, or a persistent one for an existing file.
    pub fn get_file_object(&self, worker: FileWorker) -> Arc<BufferObj> {
        let key = worker.file_path();
        let mut objects = self.objects.lock().unwrap();
        if let Some(obj) = objects.get(&key) {
            return obj.clone();
        }
        let bytes = worker.buffer_size();
        let obj = self.register(BufferObj::new(
            self.state.clone(),
            bytes,
            BufferType::Persistent,
            None,
            Some(worker),
            BufferStatus::Freed,
        ));
        objects.insert(key, obj.clone());
        obj
    }

    /// C++ `BufferManager::RemoveClean`: delete every picked buffer's data
    /// file, then every relinquished spill file, and drop them from the map.
    pub fn remove_clean(&self) -> Result<()> {
        let (clean, temps) = {
            let mut state = self.state.lock().unwrap();
            (
                std::mem::take(&mut state.clean),
                std::mem::take(&mut state.clean_temps),
            )
        };
        for weak in &clean {
            if let Some(obj) = weak.upgrade() {
                obj.cleanup_file()?;
            }
        }
        for weak in &temps {
            if let Some(obj) = weak.upgrade() {
                obj.cleanup_temp_file();
            }
        }
        let mut objects = self.objects.lock().unwrap();
        for weak in clean {
            if let Some(obj) = weak.upgrade() {
                if let Some(path) = obj.data_path() {
                    objects.remove(&path);
                }
            }
        }
        Ok(())
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

    /// C++ `BufferObj::ToMmap` on an unloaded persistent buffer: the payload
    /// is freed now (the budget drops), the object becomes `kMmap`, and reads
    /// afterwards come from the file without charging the pool.
    #[test]
    fn to_mmap_frees_the_payload_and_maps_the_file() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("blk.bin");
        let pool = BufferPool::new(1024);
        let obj = pool.allocate_persistent(&path, 128, BufferType::Persistent);
        {
            let h = obj.load().unwrap();
            h.data_mut().as_mut().unwrap().fill(7);
        }
        // `GetMutPointer` made it ephemeral; `Save` writes the payload and
        // makes it persistent again, which is what `ToMmap` requires.
        obj.save().unwrap();
        assert_eq!(obj.kind(), BufferType::Persistent);
        assert_eq!(obj.status(), BufferStatus::Unloaded);
        assert_eq!(pool.used_bytes(), 128);

        obj.to_mmap().unwrap();
        assert_eq!(obj.kind(), BufferType::Mmap);
        assert_eq!(obj.status(), BufferStatus::Freed);
        assert_eq!(pool.used_bytes(), 0);

        // A mapped load reads the file and is not charged to the budget.
        {
            let h = obj.load().unwrap();
            assert_eq!(h.data().as_ref().unwrap(), &vec![7u8; 128]);
            assert_eq!(obj.status(), BufferStatus::Loaded);
        }
        assert_eq!(obj.status(), BufferStatus::Unloaded);
        assert_eq!(pool.used_bytes(), 0);
        obj.check_state().unwrap();
    }

    /// C++ `kToMmap`: a buffer that is still loaded only switches to the
    /// mapped form when its last handle drops.
    #[test]
    fn to_mmap_on_a_loaded_buffer_switches_on_unload() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("blk.bin");
        let pool = BufferPool::new(1024);
        let obj = pool.allocate_persistent(&path, 64, BufferType::Persistent);
        let handle = obj.load().unwrap();
        obj.to_mmap().unwrap();
        assert_eq!(obj.kind(), BufferType::ToMmap);
        assert_eq!(obj.status(), BufferStatus::Loaded);
        assert_eq!(pool.used_bytes(), 64);

        drop(handle);
        assert_eq!(obj.kind(), BufferType::Mmap);
        assert_eq!(obj.status(), BufferStatus::Freed);
        assert_eq!(pool.used_bytes(), 0);
        // It is not queued for GC: a mapped buffer is freed on demand.
        assert_eq!(pool.waiting_gc_object_count().iter().sum::<usize>(), 0);
        obj.check_state().unwrap();
    }

    /// C++ `GetMutPointer` on a `kMmap` buffer: taking a mutable pointer
    /// charges the payload and turns it back into an ephemeral buffer.
    #[test]
    fn data_mut_on_a_mapped_buffer_charges_the_payload() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("blk.bin");
        let pool = BufferPool::new(1024);
        let obj = pool.allocate_persistent(&path, 96, BufferType::Persistent);
        {
            let h = obj.load().unwrap();
            h.data_mut().as_mut().unwrap()[0] = 3;
        }
        obj.save().unwrap();
        obj.to_mmap().unwrap();
        assert_eq!(pool.used_bytes(), 0);

        {
            let h = obj.load().unwrap();
            let mut data = h.data_mut();
            assert_eq!(data.as_ref().unwrap()[0], 3);
            data.as_mut().unwrap()[1] = 4;
            assert_eq!(obj.kind(), BufferType::Ephemeral);
            assert_eq!(pool.used_bytes(), 96);
        }
        obj.free().unwrap();
        assert_eq!(pool.used_bytes(), 0);
    }

    /// C++ `BufferObj::SaveSnapshot`: a resident payload is written from
    /// memory, a spilled `Temp` payload is copied from its spill file.
    #[test]
    fn save_snapshot_copies_from_memory_and_from_the_spill_file() {
        let dir = tempfile::tempdir().unwrap();
        let data_dir = dir.path().join("data");
        let temp_dir = dir.path().join("temp");
        std::fs::create_dir_all(&data_dir).unwrap();
        std::fs::create_dir_all(&temp_dir).unwrap();
        let pool = BufferPool::new(1024);

        let resident = pool.allocate(16, BufferType::Persistent);
        {
            let h = resident.load().unwrap();
            h.data_mut().as_mut().unwrap().fill(9);
        }
        let from_memory = dir.path().join("snapshot/a.bin");
        assert!(resident.save_snapshot(&from_memory).unwrap());
        assert_eq!(std::fs::read(&from_memory).unwrap(), vec![9u8; 16]);

        let spilled = pool
            .allocate_file(FileWorker::data(&data_dir, &temp_dir, "", "b", 16))
            .unwrap();
        {
            let h = spilled.load().unwrap();
            h.data_mut().as_mut().unwrap().fill(5);
            h.obj().set_data_size(16);
        }
        spilled.free().unwrap();
        assert_eq!(spilled.kind(), BufferType::Temp);
        let from_spill = dir.path().join("snapshot/b.bin");
        assert!(spilled.save_snapshot(&from_spill).unwrap());
        assert_eq!(std::fs::read(&from_spill).unwrap(), vec![5u8; 16]);
    }

    /// C++ `BufferManager::GetBufferObjectsInfo`, `GetBufferObject(path)`,
    /// `ChangeBufferObjectState` and `RemoveBufferObjects`.
    #[test]
    fn object_manager_reports_and_removes_objects() {
        let dir = tempfile::tempdir().unwrap();
        let data_dir = dir.path().join("data");
        let temp_dir = dir.path().join("temp");
        std::fs::create_dir_all(&data_dir).unwrap();
        std::fs::create_dir_all(&temp_dir).unwrap();
        let pool = BufferPool::new(4096);

        let worker = FileWorker::data(&data_dir, &temp_dir, "", "blk", 64);
        let data_path = worker.file_path();
        let obj = pool.allocate_file(worker).unwrap();
        obj.add_obj_rc();
        {
            let h = obj.load().unwrap();
            h.data_mut().as_mut().unwrap().fill(1);
            h.obj().set_data_size(64);
            h.obj().save().unwrap();
        }
        assert_eq!(pool.buffered_object_count(), 1);

        let infos = pool.get_buffer_objects_info();
        assert_eq!(infos.len(), 1);
        assert_eq!(infos[0].object_path, data_path);
        assert_eq!(infos[0].buffered_type, BufferType::Persistent);
        assert_eq!(infos[0].buffered_status, BufferStatus::Unloaded);
        assert_eq!(infos[0].file_type, FileWorkerType::DataFile);
        assert_eq!(infos[0].object_size, 64);

        assert_eq!(pool.get_object_by_path(&data_path).unwrap().id(), obj.id());
        assert!(pool.get_object_by_path(dir.path().join("nope")).is_none());

        // After a restart the file is the truth: persistent + freed.
        pool.change_buffer_object_state(&data_path);
        assert_eq!(obj.kind(), BufferType::Persistent);
        assert_eq!(obj.status(), BufferStatus::Freed);

        pool.remove_buffer_objects(std::slice::from_ref(&data_path))
            .unwrap();
        assert_eq!(pool.buffered_object_count(), 0);
        assert!(pool.get_object_by_path(&data_path).is_none());
        assert!(pool.remove_buffer_objects(&[data_path]).is_err());
    }

    /// C++ `BufferManager::AddRequestCount` / `AddCacheMissCount`: every load
    /// is a request, a `New`/`Freed` load is a miss, `Unloaded`/`Loaded` are
    /// hits.
    #[test]
    fn request_and_miss_counters() {
        let pool = BufferPool::new(1024);
        let obj = pool.allocate(32, BufferType::Persistent);
        {
            let _h = obj.load().unwrap();
        }
        assert_eq!(
            (pool.total_request_count(), pool.cache_miss_count()),
            (1, 1)
        );
        {
            // `Unloaded` → a hit.
            let _h = obj.load().unwrap();
        }
        assert_eq!(
            (pool.total_request_count(), pool.cache_miss_count()),
            (2, 1)
        );
        obj.free().unwrap();
        {
            // `Freed` → a miss again.
            let _h = obj.load().unwrap();
        }
        assert_eq!(
            (pool.total_request_count(), pool.cache_miss_count()),
            (3, 2)
        );
    }

    /// The GC queues are sharded (C++ `lru_caches_`), one bucket per shard.
    #[test]
    fn gc_queues_are_sharded() {
        let pool = BufferPool::with_lru_count(1 << 20, 4);
        assert_eq!(pool.lru_count(), 4);
        for _ in 0..8 {
            let obj = pool.allocate(8, BufferType::Persistent);
            {
                let _h = obj.load().unwrap();
            }
        }
        let waiting = pool.waiting_gc_object_count();
        assert_eq!(waiting.len(), 4);
        assert_eq!(waiting.iter().sum::<usize>(), 8);
    }

    /// C++ `BufferObj::AddBufferSize` / `UpdateFileWorkerInfo`: a var-file
    /// payload grows through the budget, and a restarting worker hands the
    /// grown size back.
    #[test]
    fn var_file_budget_and_worker_restart() {
        let dir = tempfile::tempdir().unwrap();
        let data_dir = dir.path().join("data");
        let temp_dir = dir.path().join("temp");
        std::fs::create_dir_all(&data_dir).unwrap();
        std::fs::create_dir_all(&temp_dir).unwrap();
        let pool = BufferPool::new(64);
        let worker = FileWorker::var(&data_dir, &temp_dir, "", "var", 0);
        let obj = pool.allocate_file(worker).unwrap();
        assert!(obj.add_buffer_size(16).unwrap());
        assert_eq!(pool.used_bytes(), 16);
        // Requesting more than the budget still fails, and is still charged
        // (the reference's `AddBufferSize` only warns).
        assert!(!obj.add_buffer_size(128).unwrap());
        assert_eq!(pool.used_bytes(), 144);

        let restarted = FileWorker::var(&data_dir, &temp_dir, "", "var", 144);
        obj.update_file_worker_info(restarted);
        assert_eq!(obj.size(), 144);
        // A data worker has no growing payload.
        let data = pool
            .allocate_file(FileWorker::data(&data_dir, &temp_dir, "", "d", 8))
            .unwrap();
        assert!(data.add_buffer_size(8).is_err());
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

    /// Number of files below `dir` (C++ `ListAllFile`).
    fn count_files(dir: &std::path::Path) -> usize {
        fn walk(dir: &std::path::Path, count: &mut usize) {
            let Ok(entries) = std::fs::read_dir(dir) else {
                return;
            };
            for entry in entries.flatten() {
                let path = entry.path();
                if path.is_dir() {
                    walk(&path, count);
                } else {
                    *count += 1;
                }
            }
        }
        let mut count = 0;
        walk(dir, &mut count);
        count
    }

    /// C++: `BufferManagerTest.cleanup_test`
    /// (`storage/buffer/buffer_manager_ut.cpp:99`). 100 ephemeral 100-byte
    /// files under a 2-block budget: loading spills all but the last two to
    /// the temp dir, `Save` moves them to the data dir, and the two cleanup
    /// rounds (`RemoveClean`, then `PickForCleanup` + `RemoveClean`) delete
    /// every spill and data file.
    #[test]
    fn cleanup_test() {
        const K: usize = 2;
        const FILE_SIZE: usize = 100;
        const BUFFER_SIZE: usize = K * FILE_SIZE;
        const FILE_NUM: usize = 100;
        const FILE_NUM1: usize = FILE_NUM / 2;
        const {
            assert!(FILE_NUM > K + FILE_NUM1);
        }

        let dir = tempfile::tempdir().unwrap();
        let data_dir = dir.path().join("data");
        let temp_dir = dir.path().join("temp");
        std::fs::create_dir_all(&data_dir).unwrap();
        std::fs::create_dir_all(&temp_dir).unwrap();
        let check = |data: usize, temp: usize| {
            assert_eq!(count_files(&data_dir), data, "data files");
            assert_eq!(count_files(&temp_dir), temp, "temp files");
        };

        let pool = BufferPool::new(BUFFER_SIZE);
        let mut buffers = Vec::new();
        for i in 0..FILE_NUM {
            let worker = FileWorker::data(&data_dir, &temp_dir, "", format!("file_{i}"), FILE_SIZE);
            let obj = pool.allocate_file(worker).unwrap();
            obj.add_obj_rc();
            buffers.push(obj);
            {
                let handle = buffers[i].load().unwrap();
                let mut guard = handle.data_mut();
                let payload = guard.as_mut().unwrap();
                for (j, byte) in payload.iter_mut().take(FILE_SIZE).enumerate() {
                    *byte = b'a' + ((i + j) % 26) as u8;
                }
            }
            buffers[i].set_data_size(FILE_SIZE);
        }
        // The budget holds two payloads; the other 98 were spilled.
        check(0, FILE_NUM - K);
        let mut write_n = 0;
        for obj in &buffers {
            if obj.save().unwrap() {
                write_n += 1;
            }
        }
        assert_eq!(write_n, K);
        check(FILE_NUM, 0);
        for (i, obj) in buffers.iter().enumerate() {
            let handle = obj.load().unwrap();
            let guard = handle.data();
            let payload = guard.as_ref().unwrap();
            for (j, byte) in payload.iter().take(FILE_SIZE).enumerate() {
                assert_eq!(*byte, b'a' + ((i + j) % 26) as u8, "file {i}");
            }
        }
        check(FILE_NUM, 0);
        for (i, obj) in buffers.iter().enumerate() {
            let handle = obj.load().unwrap();
            let mut guard = handle.data_mut();
            let payload = guard.as_mut().unwrap();
            for (j, byte) in payload.iter_mut().take(FILE_SIZE).enumerate() {
                *byte = b'a' + ((i + j) % 26) as u8;
            }
        }
        {
            let mut write_n = 0;
            for obj in &buffers[..FILE_NUM1] {
                if obj.save().unwrap() {
                    write_n += 1;
                }
            }
            assert!(write_n <= K);
            pool.remove_clean().unwrap();
            check(FILE_NUM, FILE_NUM - K - FILE_NUM1);
        }
        for (i, obj) in buffers.iter().enumerate().skip(FILE_NUM1) {
            {
                let handle = obj.load().unwrap();
                let mut guard = handle.data_mut();
                let payload = guard.as_mut().unwrap();
                for (j, byte) in payload.iter_mut().take(FILE_SIZE).enumerate() {
                    *byte = b'A' + ((i + j) % 26) as u8;
                }
            }
            obj.save().unwrap();
        }
        check(FILE_NUM, FILE_NUM - FILE_NUM1);
        pool.remove_clean().unwrap();
        check(FILE_NUM, 0);

        for obj in &buffers {
            obj.pick_for_cleanup().unwrap();
        }
        pool.remove_clean().unwrap();
        check(0, 0);
        assert_eq!(pool.buffered_object_count(), 0);
    }

    /// C++: `BufferManagerTest.varfile_test`
    /// (`storage/buffer/buffer_manager_ut.cpp:204`): three `VarFileWorker`
    /// payloads append under a 100-byte budget; the fourth append cannot get
    /// space (all handles are live) yet the reference still charges it, and
    /// the two appended buffers read back both 25-byte chunks.
    #[test]
    fn varfile_test() {
        const BUFFER_SIZE: usize = 100;
        const FILE_NUM: usize = 10;
        const DATA_SIZE: usize = 25;

        let dir = tempfile::tempdir().unwrap();
        let data_dir = dir.path().join("data");
        let temp_dir = dir.path().join("temp");
        std::fs::create_dir_all(&data_dir).unwrap();
        std::fs::create_dir_all(&temp_dir).unwrap();

        let pool = BufferPool::new(BUFFER_SIZE);
        let mut buffers = Vec::new();
        for i in 0..FILE_NUM {
            let worker = FileWorker::var(&data_dir, &temp_dir, "", format!("file_{i}"), 0);
            buffers.push(pool.allocate_file(worker).unwrap());
        }
        let data: Vec<u8> = (0..DATA_SIZE).map(|i| b'a' + (i % 26) as u8).collect();
        {
            let h1 = buffers[0].load().unwrap();
            h1.var_append(&data);
            let h2 = buffers[1].load().unwrap();
            h2.var_append(&data);
            let h3 = buffers[2].load().unwrap();
            assert!(h3.var_append(&data));
            assert_eq!(pool.used_bytes(), 3 * DATA_SIZE);
        }
        {
            let h1 = buffers[0].load().unwrap();
            assert!(h1.var_append(&data));
            let _h2 = buffers[1].load().unwrap();
            let h3 = buffers[2].load().unwrap();
            // Every handle is live, so the GC has no victim: the append is
            // still performed but the request fails and is charged anyway.
            assert!(!h3.var_append(&data));
            assert_eq!(pool.used_bytes(), 5 * DATA_SIZE);
        }
        {
            let h2 = buffers[1].load().unwrap();
            assert!(h2.var_append(&data));
        }
        for (i, buf) in buffers.iter().take(2).enumerate() {
            let handle = buf.load().unwrap();
            assert_eq!(
                handle.var_get(0, DATA_SIZE).unwrap().as_deref(),
                Some(data.as_slice()),
                "buffer {i} first chunk"
            );
            assert_eq!(
                handle.var_get(DATA_SIZE, DATA_SIZE).unwrap().as_deref(),
                Some(data.as_slice()),
                "buffer {i} second chunk"
            );
        }
    }

    /// A small thread-safe xorshift RNG; the reference uses `rand()`.
    struct Rng(AtomicU64);

    impl Rng {
        fn new(seed: u64) -> Self {
            Self(AtomicU64::new(seed.max(1)))
        }

        fn next_u32(&self) -> u32 {
            let mut current = self.0.load(Ordering::Relaxed);
            loop {
                let mut next = current;
                next ^= next << 13;
                next ^= next >> 7;
                next ^= next << 17;
                match self.0.compare_exchange_weak(
                    current,
                    next,
                    Ordering::Relaxed,
                    Ordering::Relaxed,
                ) {
                    Ok(_) => return (next >> 16) as u32,
                    Err(observed) => current = observed,
                }
            }
        }
    }

    const THREAD_N: usize = 2;
    const FILE_N: usize = 100;
    const AVG_FILE_SIZE: usize = 100;
    const MAX_FILE_SIZE: usize = AVG_FILE_SIZE + AVG_FILE_SIZE / 2;
    /// `*2` because `BufferManager::RequestSpace` may scan a buffer that is
    /// loading/cleaning.
    const PARALLEL_BUFFER_SIZE: usize = MAX_FILE_SIZE * THREAD_N * 2;
    const LOOP_N: usize = 10;
    const TEST_N: usize = 2;
    const VAR_FILE_STEP: usize = MAX_FILE_SIZE / LOOP_N / TEST_N;

    /// C++ `FileInfo` (`buffer_manager_ut.cpp:277`).
    struct FileInfo {
        file_id: usize,
        buffer_obj: Option<Arc<BufferObj>>,
        file_size: usize,
        visit_cnt: usize,
    }

    impl FileInfo {
        fn new(file_id: usize) -> Self {
            Self {
                file_id,
                buffer_obj: None,
                file_size: 0,
                visit_cnt: 0,
            }
        }
    }

    /// C++ `TestObj`.
    trait TestObj: Sync {
        fn init(
            &self,
            alloc_new: bool,
            file_id: usize,
            file_size: &mut usize,
            slot: &mut Option<Arc<BufferObj>>,
        );
        fn write(&self, visit_cnt: usize, file_size: &mut usize, obj: &Arc<BufferObj>);
        fn check(&self, visit_cnt: usize, file_size: usize, obj: &Arc<BufferObj>);
    }

    /// C++ `BufferManagerParallelTest::TestRoutine`.
    fn test_routine(
        files: &[Mutex<FileInfo>],
        test_i: usize,
        test_obj: &dyn TestObj,
        finished_n: &AtomicUsize,
        rng: &Rng,
    ) {
        let alloc_new = test_i == 0;
        let clean = test_i + 1 == TEST_N;
        while finished_n.load(Ordering::Acquire) < FILE_N {
            let mut op = (rng.next_u32() % 5) as usize;
            if op > 2 {
                op = 2; // read
            }
            let file_id = (rng.next_u32() as usize) % FILE_N;
            let mut file_info = files[file_id].lock().unwrap();
            let mut file_size = file_info.file_size;
            let mut buffer_obj = file_info.buffer_obj.take();
            if op < 2 {
                // write
                if file_info.visit_cnt == test_i * LOOP_N {
                    assert!(buffer_obj.is_none());
                    test_obj.init(
                        alloc_new,
                        file_info.file_id,
                        &mut file_size,
                        &mut buffer_obj,
                    );
                } else if file_info.visit_cnt == (test_i + 1) * LOOP_N {
                    if clean {
                        assert!(buffer_obj.is_none());
                    }
                    file_info.file_size = file_size;
                    file_info.buffer_obj = buffer_obj;
                    continue;
                }
                test_obj.write(
                    file_info.visit_cnt,
                    &mut file_size,
                    buffer_obj.as_ref().unwrap(),
                );
                if op == 1 {
                    buffer_obj.as_ref().unwrap().save().expect("save");
                }
                file_info.visit_cnt += 1;
                if file_info.visit_cnt == (test_i + 1) * LOOP_N {
                    if clean {
                        buffer_obj
                            .as_ref()
                            .unwrap()
                            .pick_for_cleanup()
                            .expect("pick for cleanup");
                        buffer_obj = None;
                    }
                    finished_n.fetch_add(1, Ordering::AcqRel);
                }
            } else {
                // read
                let visit_cnt = file_info.visit_cnt;
                if visit_cnt == test_i * LOOP_N || visit_cnt == (test_i + 1) * LOOP_N {
                    file_info.file_size = file_size;
                    file_info.buffer_obj = buffer_obj;
                    continue;
                }
                test_obj.check(visit_cnt, file_size, buffer_obj.as_ref().unwrap());
            }
            file_info.file_size = file_size;
            file_info.buffer_obj = buffer_obj;
        }
    }

    /// C++ `Test1Obj`: fixed-size `DataFileWorker` payloads.
    struct Test1Obj<'a> {
        pool: &'a BufferPool,
        data_dir: PathBuf,
        temp_dir: PathBuf,
        rng: &'a Rng,
    }

    impl TestObj for Test1Obj<'_> {
        fn init(
            &self,
            alloc_new: bool,
            file_id: usize,
            file_size: &mut usize,
            slot: &mut Option<Arc<BufferObj>>,
        ) {
            if alloc_new {
                *file_size = (self.rng.next_u32() as usize) % AVG_FILE_SIZE + AVG_FILE_SIZE / 2;
            }
            let worker = FileWorker::data(
                &self.data_dir,
                &self.temp_dir,
                "",
                format!("file_{file_id}"),
                *file_size,
            );
            if alloc_new {
                let obj = self.pool.allocate_file(worker).expect("allocate");
                obj.add_obj_rc();
                *slot = Some(obj);
            } else {
                *slot = Some(self.pool.get_file_object(worker));
            }
        }

        fn write(&self, visit_cnt: usize, file_size: &mut usize, obj: &Arc<BufferObj>) {
            let handle = obj.load().expect("load");
            {
                let mut guard = handle.data_mut();
                let payload = guard.as_mut().expect("payload");
                payload[..*file_size].fill(b'a' + (visit_cnt % 26) as u8);
            }
            obj.set_data_size(*file_size);
        }

        fn check(&self, visit_cnt: usize, file_size: usize, obj: &Arc<BufferObj>) {
            let handle = obj.load().expect("load");
            let guard = handle.data();
            let payload = guard.as_ref().expect("payload");
            for (i, byte) in payload.iter().take(file_size).enumerate() {
                assert_eq!(
                    *byte,
                    b'a' + ((visit_cnt - 1) % 26) as u8,
                    "byte {i} of a {file_size}-byte buffer"
                );
            }
        }
    }

    /// C++ `Test2Obj`: growing `VarFileWorker` payloads.
    struct Test2Obj<'a> {
        pool: &'a BufferPool,
        data_dir: PathBuf,
        temp_dir: PathBuf,
    }

    impl TestObj for Test2Obj<'_> {
        fn init(
            &self,
            alloc_new: bool,
            file_id: usize,
            file_size: &mut usize,
            slot: &mut Option<Arc<BufferObj>>,
        ) {
            let worker = FileWorker::var(
                &self.data_dir,
                &self.temp_dir,
                "",
                format!("file_{file_id}"),
                *file_size,
            );
            if alloc_new {
                let obj = self.pool.allocate_file(worker).expect("allocate");
                obj.add_obj_rc();
                *slot = Some(obj);
            } else {
                *slot = Some(self.pool.get_file_object(worker));
            }
        }

        fn write(&self, _visit_cnt: usize, file_size: &mut usize, obj: &Arc<BufferObj>) {
            let handle = obj.load().expect("load");
            let step = VAR_FILE_STEP;
            let mut data = vec![0u8; step];
            for (i, byte) in data.iter_mut().enumerate() {
                *byte = b'a' + ((i + *file_size) % 26) as u8;
            }
            // The reference passes no `free_success` pointer: the append is
            // unconditional.
            handle.var_append(&data);
            *file_size += step;
            assert_eq!(obj.size(), *file_size, "GetBufferSize");
        }

        fn check(&self, _visit_cnt: usize, file_size: usize, obj: &Arc<BufferObj>) {
            let handle = obj.load().expect("load");
            let step = VAR_FILE_STEP;
            let mut offset = 0;
            while offset < file_size {
                let chunk = handle
                    .var_get(offset, step)
                    .expect("get")
                    .expect("non-empty chunk");
                for (j, byte) in chunk.iter().enumerate() {
                    assert_eq!(
                        *byte,
                        b'a' + ((offset + j) % 26) as u8,
                        "offset {offset} byte {j}"
                    );
                }
                offset += step;
            }
        }
    }

    fn parallel_dirs() -> (tempfile::TempDir, PathBuf, PathBuf) {
        let dir = tempfile::tempdir().unwrap();
        let data_dir = dir.path().join("data");
        let temp_dir = dir.path().join("temp");
        std::fs::create_dir_all(&data_dir).unwrap();
        std::fs::create_dir_all(&temp_dir).unwrap();
        (dir, data_dir, temp_dir)
    }

    fn run_parallel(test_obj: &dyn TestObj, pool: &BufferPool) {
        let files: Vec<Mutex<FileInfo>> =
            (0..FILE_N).map(|i| Mutex::new(FileInfo::new(i))).collect();
        let rng = Rng::new(1);
        for test_i in 0..TEST_N {
            let finished_n = AtomicUsize::new(0);
            for file in &files {
                file.lock().unwrap().buffer_obj = None;
            }
            std::thread::scope(|scope| {
                for _ in 0..THREAD_N {
                    scope.spawn(|| test_routine(&files, test_i, test_obj, &finished_n, &rng));
                }
            });
        }
        assert_eq!(pool.used_bytes(), 0, "memory usage after the run");
        pool.remove_clean().unwrap();
    }

    /// C++: `BufferManagerParallelTest.parallel_test1`
    /// (`storage/buffer/buffer_manager_ut.cpp:420`): two threads alternate
    /// reads/writes/saves over 100 fixed-size buffers for two passes; the
    /// second pass reloads the buffers created by the first.
    #[test]
    fn parallel_test1() {
        let (_dir, data_dir, temp_dir) = parallel_dirs();
        let pool = BufferPool::new(PARALLEL_BUFFER_SIZE);
        let rng = Rng::new(1);
        let test_obj = Test1Obj {
            pool: &pool,
            data_dir,
            temp_dir,
            rng: &rng,
        };
        run_parallel(&test_obj, &pool);
    }

    /// C++: `BufferManagerParallelTest.parallel_test2`
    /// (`storage/buffer/buffer_manager_ut.cpp:507`): the same scenario over
    /// growing `VarFileWorker` buffers; every append's size is charged and
    /// `GetBufferSize` tracks the accumulated bytes.
    #[test]
    fn parallel_test2() {
        let (_dir, data_dir, temp_dir) = parallel_dirs();
        let pool = BufferPool::new(PARALLEL_BUFFER_SIZE);
        let test_obj = Test2Obj {
            pool: &pool,
            data_dir,
            temp_dir,
        };
        run_parallel(&test_obj, &pool);
    }
}
