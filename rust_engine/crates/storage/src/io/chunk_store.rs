//! Where an index chunk's bytes live.
//!
//! The reference never opens a chunk path with the filesystem API: every chunk
//! write goes through `PersistenceManager::Persist(path, tmp, try_compose)` and
//! every read through `PersistenceManager::GetObjCache(path)` +
//! `VirtualStore::MmapFile(pm->GetObjPath(obj_key))`
//! (`memory_indexer_impl.cpp:679`, `disk_segment_reader_impl.cpp:59`). The
//! logical path (`db_1/tbl_2/idx_3/seg_0/ft_…`) is only a key there - the bytes
//! live in `workspace/<obj_key>`, alone or as one aligned part of a composed
//! object - so on an object-storage profile there may be no file at the logical
//! path at all.
//!
//! This module is that seam for the port. Two backends:
//!
//! * [`LocalChunkStore`] - the direct-disk profile: `write_synced` (temp file +
//!   fsync + rename + fsync of the directory), `MappedFile::open`,
//!   `remove_file`. This is what every profile used before the seam existed.
//! * [`PersistentChunkStore`] - a profile with `EngineOptions::object_dir`: the
//!   bytes are published through the [`PersistenceManager`] (one object per
//!   chunk file, `try_compose = false`, as the reference's full-text writer
//!   does), and a read that has no local copy resolves the object part and maps
//!   the range ([`MappedFile::open_range`]).
//!
//! Deviation from the reference, deliberate: the persistent backend keeps the
//! local file as a cache (it is the directory the port enumerates and the
//! directory `PersistenceManager::restore_missing_files` rebuilds from), while
//! C++ moves the temp file into the object and leaves the logical path absent.
//! Keeping it means "a chunk exists" and "a chunk is durable" stay two separate
//! questions, which is what lets the same code run with and without a
//! persistence manager.
//!
//! The store is a process-wide slot, like the reference's
//! `InfinityContext::instance().persistence_manager()`: the engine installs one
//! when it opens and every chunk reader/writer picks it up. Without an
//! installed store the local backend is used, so a test that never opens an
//! object directory behaves exactly as before.

use std::collections::HashMap;
use std::path::{Path, PathBuf};
use std::sync::{Arc, Mutex, OnceLock, RwLock};

use crate::mapped::MappedFile;
use crate::persistence::PersistenceManager;

/// Distinguishes the spool files of two writes of the same path.
static SPOOL_SEQ: std::sync::atomic::AtomicU64 = std::sync::atomic::AtomicU64::new(0);

/// One chunk's byte storage.
pub trait ChunkStore: Send + Sync + std::fmt::Debug {
    /// Publishes `bytes` at `path`, durably.
    ///
    /// This is the reference's `pm->Persist(path, tmp)` with `try_compose = true`
    /// (`FileWorker::WriteToFile`): many small files share one bounded object.
    fn write(&self, path: &Path, bytes: &[u8]) -> std::io::Result<()>;

    /// [`Self::write`] with `try_compose = false`: the file gets an object of its
    /// own.
    ///
    /// The reference's full-text writer and merger pass `false`
    /// (`memory_indexer_impl.cpp:679`, `column_index_merger_impl.cpp:162`), so a
    /// chunk file is exactly one object and a reader maps it whole. Off a
    /// persistence profile this is [`Self::write`].
    fn write_dedicated(&self, path: &Path, bytes: &[u8]) -> std::io::Result<()> {
        self.write(path, bytes)
    }

    /// [`Self::write`] / [`Self::write_dedicated`] for bytes **produced while
    /// they are written**, returning how many there were.
    ///
    /// The rest of this trait takes the whole value, which is right for a block
    /// and wrong for a spann chunk: one segment's tree is tens of MB, and
    /// `knn_index::spann::writer::write_to` streams it so the peak is a buffer,
    /// not the chunk. `dedicated` means what [`Self::write_dedicated`] means.
    ///
    /// The default buffers, so a backend that cannot stream still publishes.
    fn write_streaming(
        &self,
        path: &Path,
        dedicated: bool,
        produce: &mut dyn FnMut(&mut dyn std::io::Write) -> std::io::Result<()>,
    ) -> std::io::Result<u64> {
        let mut buf: Vec<u8> = Vec::new();
        produce(&mut buf)?;
        let len = buf.len() as u64;
        if dedicated {
            self.write_dedicated(path, &buf)?;
        } else {
            self.write(path, &buf)?;
        }
        Ok(len)
    }

    /// Reads the whole value at `path`.
    fn read(&self, path: &Path) -> std::io::Result<Vec<u8>>;

    /// Maps the value at `path` read-only.
    fn open(&self, path: &Path) -> std::io::Result<MappedFile>;

    /// Removes the value at `path`; a missing one is not an error.
    ///
    /// A real failure is returned instead of being swallowed: the caller that
    /// reclaims a deprecated chunk clears the deprecation marker only when the
    /// bytes are gone, and a marker cleared over a surviving object would leak
    /// the chunk for good.
    fn remove(&self, path: &Path) -> std::io::Result<()>;

    /// Removes a whole directory tree and every value recorded below it.
    ///
    /// A dropped table, a dropped index or a cleaned-up segment deletes a
    /// directory; on a persistence profile the parts of those files have to be
    /// forgotten too, or the objects keep the bytes alive with nothing naming
    /// them.
    fn remove_tree(&self, dir: &Path);

    /// The number of bytes at `path`, if it exists.
    fn length(&self, path: &Path) -> Option<u64>;

    /// The directory listing, for chunk discovery.
    ///
    /// A backend that only has objects (no local cache) returns nothing here;
    /// the caller then relies on the catalog, which is where the reference
    /// keeps its chunk list anyway (`TableIndexMeta` → `SegmentIndexMeta` →
    /// `ChunkIndexMetaInfo`).
    fn list(&self, dir: &Path, prefix: &str) -> Vec<String>;

    /// Whether this is the direct-disk backend.
    fn is_local(&self) -> bool {
        false
    }
}

/// The direct-disk backend: every logical path is a real file.
#[derive(Debug, Default)]
pub struct LocalChunkStore;

impl ChunkStore for LocalChunkStore {
    fn write(&self, path: &Path, bytes: &[u8]) -> std::io::Result<()> {
        // The raw primitive: `format::write_synced` is the store-aware wrapper,
        // so calling it here would come straight back to this function.
        crate::format::write_synced_direct(path, bytes)
    }

    fn read(&self, path: &Path) -> std::io::Result<Vec<u8>> {
        std::fs::read(path)
    }

    fn open(&self, path: &Path) -> std::io::Result<MappedFile> {
        MappedFile::open_path(path)
    }

    fn remove(&self, path: &Path) -> std::io::Result<()> {
        remove_local_file(path)
    }

    fn remove_tree(&self, dir: &Path) {
        let _ = std::fs::remove_dir_all(dir);
    }

    fn length(&self, path: &Path) -> Option<u64> {
        std::fs::metadata(path).ok().map(|meta| meta.len())
    }

    fn list(&self, dir: &Path, prefix: &str) -> Vec<String> {
        let Ok(entries) = std::fs::read_dir(dir) else {
            return Vec::new();
        };
        let mut names: Vec<String> = entries
            .flatten()
            .map(|entry| entry.file_name().to_string_lossy().to_string())
            .filter(|name| name.starts_with(prefix))
            .collect();
        names.sort();
        names
    }

    fn is_local(&self) -> bool {
        true
    }

    /// Off a persistence profile the streamed bytes *are* the file, so they go
    /// straight to the temp+rename publisher instead of being buffered first.
    fn write_streaming(
        &self,
        path: &Path,
        _dedicated: bool,
        produce: &mut dyn FnMut(&mut dyn std::io::Write) -> std::io::Result<()>,
    ) -> std::io::Result<u64> {
        crate::format::write_synced_stream(path, produce)
    }
}

/// The persistence backend: every chunk file is also published as an object.
pub struct PersistentChunkStore {
    manager: Arc<PersistenceManager>,
    /// The root the persistence keys are relative to (the engine directory).
    root: PathBuf,
    /// One lock per logical path, held across the cache write **and** the
    /// publication.
    ///
    /// Two writers of one path must not interleave: the publish reads the bytes
    /// it was handed and records the object, so a write that lands between
    /// another's cache replacement and its publication would leave the cache
    /// holding one value and the object the other.
    path_locks: Mutex<HashMap<PathBuf, Arc<Mutex<()>>>>,
}

impl PersistentChunkStore {
    pub fn new(manager: Arc<PersistenceManager>, root: impl AsRef<Path>) -> Self {
        Self {
            manager,
            root: root.as_ref().to_path_buf(),
            path_locks: Mutex::new(HashMap::new()),
        }
    }

    /// The lock serialising the writers of `path`.
    fn path_lock(&self, path: &Path) -> Arc<Mutex<()>> {
        let mut locks = self
            .path_locks
            .lock()
            .unwrap_or_else(|poisoned| poisoned.into_inner());
        Arc::clone(locks.entry(path.to_path_buf()).or_default())
    }

    /// `pm|object|<rel>` key of a logical path.
    ///
    /// Files below the data dir use the reference's rule (`RemovePrefix`, so the
    /// key is `db_1/tbl_2/...`). Files the port keeps beside it - the full-text
    /// chunk cache - have no data-dir-relative form, so their key is the path
    /// relative to the engine root, which lives in the same `pm|object|`
    /// namespace under another prefix. Either way a C++ engine reading the
    /// catalog simply never looks such a key up.
    fn key(&self, path: &Path) -> Option<String> {
        if let Some(key) = self.manager.key_of(path) {
            return Some(key);
        }
        let rel = path.strip_prefix(&self.root).ok()?;
        Some(rel.to_string_lossy().replace('\\', "/"))
    }

    /// The object holding `path`, if the manager knows one.
    fn resolve(&self, path: &Path) -> Option<(PathBuf, usize, usize)> {
        let key = self.key(path)?;
        let addr = self.manager.lookup_key(&key)?;
        if addr.part_size == 0 || addr.obj_key == crate::persistence::KEY_EMPTY {
            return None;
        }
        Some((
            self.manager.get_obj_path(&addr.obj_key),
            addr.part_offset,
            addr.part_size,
        ))
    }

    /// A spool file inside the object workspace.
    ///
    /// `persist_keyed` **renames** its input into place (a dedicated object) or
    /// appends and unlinks it (a composed part), so the source must live on the
    /// object's filesystem - a rename across filesystems fails with `EXDEV`.
    /// The name carries the target's own name so a leftover spool is
    /// identifiable, and a counter so two writers of one path never share one.
    fn spool_path(&self, path: &Path) -> std::io::Result<std::path::PathBuf> {
        let spool = self.manager.workspace().join(".spool");
        std::fs::create_dir_all(&spool)?;
        let seq = SPOOL_SEQ.fetch_add(1, std::sync::atomic::Ordering::Relaxed);
        Ok(spool.join(format!(
            "{}-{seq}.pmtmp",
            path.file_name().unwrap_or_default().to_string_lossy()
        )))
    }

    /// Records `key` as having no bytes, as `persist` does for a zero-length
    /// file: the reference stores `ObjAddr::KeyEmpty` rather than an object.
    fn forget(&self, key: &str) -> std::io::Result<()> {
        if let Ok(result) = self.manager.cleanup_key(key) {
            let _ = self.manager.apply_write_result(&result);
        }
        Ok(())
    }

    /// Publishes `bytes` into an object, alone or as a part of the current one.
    fn publish(&self, path: &Path, bytes: &[u8], try_compose: bool) -> std::io::Result<()> {
        let Some(key) = self.key(path) else {
            // Outside the engine root: nothing to key the object by, so the
            // local file is all there is.
            return Ok(());
        };
        if bytes.is_empty() {
            // An empty file never owns bytes, so it has no object: the
            // reference records `ObjAddr::KeyEmpty` instead of writing one.
            // Dropping the old record keeps a stale object from being read.
            return self.forget(&key);
        }
        let tmp = self.spool_path(path)?;
        crate::format::write_synced_direct(&tmp, bytes)?;
        let result = self
            .manager
            .persist_keyed(&key, &tmp, try_compose)
            .map_err(|error| std::io::Error::other(error.to_string()))?;
        self.manager
            .apply_write_result(&result)
            .map_err(|error| std::io::Error::other(error.to_string()))?;
        Ok(())
    }
}

impl ChunkStore for PersistentChunkStore {
    fn write(&self, path: &Path, bytes: &[u8]) -> std::io::Result<()> {
        // The local file is the cache the port enumerates; the object is what
        // survives a crash or a wiped cache. Both are written before the call
        // returns, so "published" and "durable" never disagree - and the lock
        // covers both halves, so two writers of one path cannot leave the cache
        // and the object holding different values.
        let lock = self.path_lock(path);
        let _guard = lock.lock().unwrap_or_else(|poisoned| poisoned.into_inner());
        crate::format::write_synced_direct(path, bytes)?;
        self.publish(path, bytes, true)
    }

    fn write_dedicated(&self, path: &Path, bytes: &[u8]) -> std::io::Result<()> {
        let lock = self.path_lock(path);
        let _guard = lock.lock().unwrap_or_else(|poisoned| poisoned.into_inner());
        crate::format::write_synced_direct(path, bytes)?;
        self.publish(path, bytes, false)
    }

    /// The streamed form writes the bytes **once**: into the local cache (the
    /// file `list` enumerates and a restart re-maps), then copies that file into
    /// a spool for `persist_keyed`, which consumes what it is given. One pass
    /// over the producer, one file copy - and never the chunk in memory, which
    /// is what `write_to` streams for.
    fn write_streaming(
        &self,
        path: &Path,
        dedicated: bool,
        produce: &mut dyn FnMut(&mut dyn std::io::Write) -> std::io::Result<()>,
    ) -> std::io::Result<u64> {
        let lock = self.path_lock(path);
        let _guard = lock.lock().unwrap_or_else(|poisoned| poisoned.into_inner());
        let len = crate::format::write_synced_stream(path, produce)?;
        let Some(key) = self.key(path) else {
            // Outside the engine root: nothing to key the object by, so the
            // local file is all there is - the buffered path does the same.
            return Ok(len);
        };
        if len == 0 {
            return self.forget(&key).map(|()| 0);
        }
        let tmp = self.spool_path(path)?;
        std::fs::copy(path, &tmp)?;
        let result = self
            .manager
            .persist_keyed(&key, &tmp, !dedicated)
            .map_err(|error| std::io::Error::other(error.to_string()))?;
        self.manager
            .apply_write_result(&result)
            .map_err(|error| std::io::Error::other(error.to_string()))?;
        Ok(len)
    }

    fn read(&self, path: &Path) -> std::io::Result<Vec<u8>> {
        if let Ok(bytes) = std::fs::read(path) {
            return Ok(bytes);
        }
        let Some((object, offset, size)) = self.resolve(path) else {
            return Err(std::io::Error::new(
                std::io::ErrorKind::NotFound,
                format!("no file and no object for {}", path.display()),
            ));
        };
        let blob = std::fs::read(&object)?;
        let end = offset
            .checked_add(size)
            .filter(|end| *end <= blob.len())
            .ok_or_else(|| {
                std::io::Error::new(
                    std::io::ErrorKind::InvalidData,
                    format!(
                        "object {} is shorter than its record for {}",
                        object.display(),
                        path.display()
                    ),
                )
            })?;
        Ok(blob[offset..end].to_vec())
    }

    fn open(&self, path: &Path) -> std::io::Result<MappedFile> {
        // Raw primitive again (`MappedFile::open` is the store-aware facade).
        match MappedFile::open_path(path) {
            Ok(map) => Ok(map),
            Err(error) if error.kind() == std::io::ErrorKind::NotFound => {
                let Some((object, offset, size)) = self.resolve(path) else {
                    return Err(error);
                };
                MappedFile::open_range(object, offset, size)
            }
            Err(error) => Err(error),
        }
    }

    fn remove(&self, path: &Path) -> std::io::Result<()> {
        // Both halves have to succeed before the caller may forget the chunk:
        // the local file is the cache, the object is the durable copy, and a
        // failure in either leaves bytes behind that a later cleanup still has
        // to find.
        remove_local_file(path)?;
        if let Some(key) = self.key(path) {
            let result = self
                .manager
                .cleanup_key(&key)
                .map_err(|error| std::io::Error::other(error.to_string()))?;
            self.manager
                .apply_write_result(&result)
                .map_err(|error| std::io::Error::other(error.to_string()))?;
        }
        Ok(())
    }

    fn remove_tree(&self, dir: &Path) {
        if let Some(prefix) = self.key(dir) {
            let prefix = format!("{prefix}/");
            let keys: Vec<String> = self.manager.keys_under(&prefix).into_iter().collect();
            for key in keys {
                if let Ok(result) = self.manager.cleanup_key(&key) {
                    let _ = self.manager.apply_write_result(&result);
                }
            }
        }
        let _ = std::fs::remove_dir_all(dir);
    }

    fn length(&self, path: &Path) -> Option<u64> {
        if let Ok(meta) = std::fs::metadata(path) {
            return Some(meta.len());
        }
        self.resolve(path).map(|(_, _, size)| size as u64)
    }

    fn list(&self, dir: &Path, prefix: &str) -> Vec<String> {
        LocalChunkStore.list(dir, prefix)
    }
}

impl std::fmt::Debug for PersistentChunkStore {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("PersistentChunkStore")
            .field("root", &self.root)
            .finish_non_exhaustive()
    }
}

/// Removes a local file; a missing one is the caller's success case.
///
/// Every backend bottoms out here, so "the file was already gone" means the
/// same thing on both profiles and a real `EPERM`/`EBUSY` still reaches the
/// caller that decides whether a deprecation marker may be cleared.
fn remove_local_file(path: &Path) -> std::io::Result<()> {
    match std::fs::remove_file(path) {
        Ok(()) => Ok(()),
        Err(error) if error.kind() == std::io::ErrorKind::NotFound => Ok(()),
        Err(error) => Err(error),
    }
}

/// The process-wide slot's value: one backend per engine root, plus a default
/// for paths that belong to no installed root.
///
/// A process may open several engines (a test suite does, and so does an
/// embedder with two directories), each with its own persistence manager. A
/// single global backend would make the engine opened last serve every earlier
/// engine's reads, writes and cleanups - and those paths are not in its
/// workspace. Routing by root keeps each engine on its own manager while paths
/// outside every root (the direct-disk case, a unit test) keep using the
/// default, exactly as a one-engine process did before.
#[derive(Clone, Debug)]
struct RoutingStore {
    default: Arc<dyn ChunkStore>,
    /// Installed engine roots; the longest matching prefix wins.
    roots: Vec<(PathBuf, Arc<dyn ChunkStore>)>,
}

impl RoutingStore {
    fn local() -> Self {
        Self {
            default: Arc::new(LocalChunkStore),
            roots: Vec::new(),
        }
    }

    /// The backend that owns `path`.
    fn pick(&self, path: &Path) -> &Arc<dyn ChunkStore> {
        self.roots
            .iter()
            .filter(|(root, _)| path.starts_with(root))
            .max_by_key(|(root, _)| root.as_os_str().len())
            .map(|(_, store)| store)
            .unwrap_or(&self.default)
    }
}

impl ChunkStore for RoutingStore {
    fn write(&self, path: &Path, bytes: &[u8]) -> std::io::Result<()> {
        self.pick(path).write(path, bytes)
    }

    fn write_dedicated(&self, path: &Path, bytes: &[u8]) -> std::io::Result<()> {
        self.pick(path).write_dedicated(path, bytes)
    }

    fn write_streaming(
        &self,
        path: &Path,
        dedicated: bool,
        produce: &mut dyn FnMut(&mut dyn std::io::Write) -> std::io::Result<()>,
    ) -> std::io::Result<u64> {
        // Forwarded, not inherited: the default buffers, and buffering a chunk
        // behind a router is exactly what the streaming form avoids.
        self.pick(path).write_streaming(path, dedicated, produce)
    }

    fn read(&self, path: &Path) -> std::io::Result<Vec<u8>> {
        self.pick(path).read(path)
    }

    fn open(&self, path: &Path) -> std::io::Result<MappedFile> {
        self.pick(path).open(path)
    }

    fn remove(&self, path: &Path) -> std::io::Result<()> {
        self.pick(path).remove(path)
    }

    fn remove_tree(&self, dir: &Path) {
        self.pick(dir).remove_tree(dir);
    }

    fn length(&self, path: &Path) -> Option<u64> {
        self.pick(path).length(path)
    }

    fn list(&self, dir: &Path, prefix: &str) -> Vec<String> {
        self.pick(dir).list(dir, prefix)
    }

    fn is_local(&self) -> bool {
        self.roots.is_empty() && self.default.is_local()
    }
}

type Slot = RwLock<Arc<RoutingStore>>;

fn slot() -> &'static Slot {
    static SLOT: OnceLock<Slot> = OnceLock::new();
    SLOT.get_or_init(|| RwLock::new(Arc::new(RoutingStore::local())))
}

/// Rewrites the slot's registry.
///
/// The value is an `Arc`, so a reader keeps the snapshot it cloned: an install
/// while a write is in flight never swaps the backend under that write.
fn update(f: impl FnOnce(&mut RoutingStore)) {
    let mut guard = match slot().write() {
        Ok(guard) => guard,
        Err(poisoned) => poisoned.into_inner(),
    };
    let mut next = RoutingStore::clone(&guard);
    f(&mut next);
    *guard = Arc::new(next);
}

/// [`ChunkStore::read`] for a caller holding anything path-like.
pub fn read_file(path: impl AsRef<Path>) -> std::io::Result<Vec<u8>> {
    chunk_store().read(path.as_ref())
}

/// [`ChunkStore::write`] for a caller holding anything path-like.
pub fn write_file(path: impl AsRef<Path>, bytes: &[u8]) -> std::io::Result<()> {
    chunk_store().write(path.as_ref(), bytes)
}

/// [`ChunkStore::write_streaming`] for a caller holding anything path-like.
pub fn write_streaming_file(
    path: impl AsRef<Path>,
    dedicated: bool,
    produce: &mut dyn FnMut(&mut dyn std::io::Write) -> std::io::Result<()>,
) -> std::io::Result<u64> {
    chunk_store().write_streaming(path.as_ref(), dedicated, produce)
}

/// [`ChunkStore::open`] for a caller holding anything path-like.
pub fn open_file(path: impl AsRef<Path>) -> std::io::Result<MappedFile> {
    chunk_store().open(path.as_ref())
}

/// [`ChunkStore::remove`] for a caller holding anything path-like.
///
/// Deleting a data file has to go through the store as well: a direct
/// `std::fs::remove_file` keeps the `pm|object|` record and the object's bytes
/// alive, and `restore_missing_files` then brings the deleted file back. The
/// store's answer is passed on, so a chunk whose bytes are still there is not
/// reported as reclaimed.
pub fn remove_file(path: impl AsRef<Path>) -> std::io::Result<()> {
    chunk_store().remove(path.as_ref())
}

/// The store every chunk reader and writer uses.
pub fn chunk_store() -> Arc<dyn ChunkStore> {
    let registry: Arc<RoutingStore> = match slot().read() {
        Ok(guard) => Arc::clone(&guard),
        Err(poisoned) => Arc::clone(&poisoned.into_inner()),
    };
    registry
}

/// Installs the backend for this process (`InfinityContext`'s persistence
/// manager, which the reference looks up the same way).
///
/// This is the default for paths that belong to no installed root; an engine
/// installs its own with [`install_chunk_store_for`].
pub fn install_chunk_store(store: Arc<dyn ChunkStore>) {
    update(|registry| registry.default = store);
}

/// Installs `store` as the backend of every path below `root`.
///
/// Each engine owns its own persistence manager and its own directory, so a
/// later open must not take over an earlier one's reads, writes and cleanups:
/// the registry keeps one entry per root and routes by path.
pub fn install_chunk_store_for(root: impl AsRef<Path>, store: Arc<dyn ChunkStore>) {
    let root = root.as_ref().to_path_buf();
    update(|registry| {
        registry.default = Arc::clone(&store);
        registry.roots.retain(|(installed, _)| installed != &root);
        registry.roots.push((root, store));
    });
}

/// Forgets the backend of `root`, so nothing in the process still refers to it.
///
/// An engine calls this when it drops: the registry entry is what keeps its
/// store - and the persistence manager behind it - alive, and a manager that is
/// still referenced keeps the object directory (its catalog) from being opened
/// again in this process. The default is reset only when it is that very store;
/// another engine's backend is left in place.
pub fn uninstall_chunk_store_for(root: impl AsRef<Path>) {
    let root = root.as_ref().to_path_buf();
    update(|registry| {
        let mut released: Vec<Arc<dyn ChunkStore>> = Vec::new();
        registry.roots.retain(|(installed, store)| {
            if installed == &root {
                released.push(Arc::clone(store));
                false
            } else {
                true
            }
        });
        if released
            .iter()
            .any(|store| Arc::ptr_eq(store, &registry.default))
        {
            registry.default = Arc::new(LocalChunkStore);
        }
    });
}

/// Goes back to the direct-disk backend and forgets every installed root.
pub fn install_local_chunk_store() {
    update(|registry| *registry = RoutingStore::local());
}

/// Installs the direct-disk backend unless a persistence profile owns the slot.
///
/// The engine calls this on every open. One engine per process never notices;
/// a process that opens several (a test suite) must not have a plain open undo
/// the object profile another open installed, so the roots of other engines
/// stay registered and only the default is (re)installed as local.
pub fn install_local_chunk_store_unless_persistent() {
    update(|registry| {
        if registry.is_local() {
            registry.default = Arc::new(LocalChunkStore);
        }
    });
}

/// Removes a data directory and forgets every value recorded below it.
pub fn remove_tree(dir: impl AsRef<Path>) {
    chunk_store().remove_tree(dir.as_ref());
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::sync::Mutex;

    /// The slot is process-wide, so a test that installs a backend has to put
    /// the local one back - and tests must not overlap.
    static LOCK: Mutex<()> = Mutex::new(());

    fn manager(root: &Path) -> Arc<PersistenceManager> {
        let workspace = root.join("persistence");
        Arc::new(
            PersistenceManager::new(&workspace, root.join("data"), 1 << 20)
                .expect("persistence manager"),
        )
    }

    /// A persisted chunk is readable through the object even after the local
    /// copy is gone - that is what makes an object-storage profile work at all.
    #[test]
    fn a_persisted_chunk_is_readable_without_its_local_copy() {
        let _guard = LOCK.lock().unwrap_or_else(|poisoned| poisoned.into_inner());
        let root = tempfile::tempdir().unwrap();
        let root = root.path();
        let store = PersistentChunkStore::new(manager(root), root);
        let path = root.join("fulltext/db_tbl_idx/ft_0000000000000000.dic");
        std::fs::create_dir_all(path.parent().unwrap()).unwrap();
        store.write(&path, b"dictionary bytes").unwrap();
        // The object exists and the mapping names it.
        assert!(store
            .manager
            .lookup_key("fulltext/db_tbl_idx/ft_0000000000000000.dic")
            .is_some());

        std::fs::remove_file(&path).unwrap();
        assert_eq!(store.read(&path).unwrap(), b"dictionary bytes");
        let mapped = store.open(&path).unwrap();
        assert_eq!(&mapped[..], b"dictionary bytes");
    }

    /// The streamed form publishes exactly like the buffered one: one object of
    /// its own, readable after the cache is gone, byte for byte.
    ///
    /// This is the path a spann chunk takes (`writer::write_to`): the producer
    /// streams because the chunk is tens of MB, and the object has to come out
    /// the same anyway.
    #[test]
    fn a_streamed_write_is_published_and_readable_without_its_local_copy() {
        let _guard = LOCK.lock().unwrap_or_else(|poisoned| poisoned.into_inner());
        let root = tempfile::tempdir().unwrap();
        let root = root.path();
        let store = PersistentChunkStore::new(manager(root), root);
        let path = root.join("data/db_1/tbl_2/idx_3/seg_0/chunk_0.idx");
        std::fs::create_dir_all(path.parent().unwrap()).unwrap();
        let bytes: Vec<u8> = (0..4096u32).map(|i| (i % 251) as u8).collect();
        let len = store
            .write_streaming(&path, true, &mut |sink| {
                sink.write_all(&bytes[..2048])?;
                sink.write_all(&bytes[2048..])
            })
            .unwrap();
        assert_eq!(len as usize, bytes.len());
        // The cache holds it, and the object is a dedicated one (offset 0).
        assert_eq!(std::fs::read(&path).unwrap(), bytes);
        let addr = store
            .manager
            .lookup_key("db_1/tbl_2/idx_3/seg_0/chunk_0.idx")
            .expect("streamed write must publish an object");
        assert_eq!(addr.part_offset, 0);
        assert_eq!(addr.part_size, bytes.len());

        std::fs::remove_file(&path).unwrap();
        assert_eq!(store.read(&path).unwrap(), bytes);
    }

    /// A spann chunk written by `writer::write_to` is an object: the mapped
    /// reader answers from it after the cache is gone.
    ///
    /// The chunk writer used to `File::create` straight into the cache, so on a
    /// persistence profile the file a catalog record names was never published
    /// and a wiped cache lost the index.
    #[test]
    fn a_spann_chunk_written_through_the_store_survives_a_wiped_cache() {
        let _guard = LOCK.lock().unwrap_or_else(|poisoned| poisoned.into_inner());
        let root = tempfile::tempdir().unwrap();
        let root = root.path();
        install_chunk_store(Arc::new(PersistentChunkStore::new(manager(root), root)));
        let rows: Vec<(u64, Vec<f32>)> = (0..120u64)
            .map(|i| {
                (
                    i,
                    (0..8)
                        .map(|d| ((i * 7 + d * 3) % 23) as f32 - 11.0)
                        .collect(),
                )
            })
            .collect();
        let opts = index::spann::Options {
            max_partition_size: 32,
            ..Default::default()
        };
        let index = index::spann::SpannIndex::build(rows, opts.clone()).unwrap();
        let path = root.join("data/db_1/tbl_2/idx_3/seg_0/chunk_0.idx");
        std::fs::create_dir_all(path.parent().unwrap()).unwrap();
        let len = crate::knn_index::spann::write_to(&index, &path, 0).unwrap();
        assert_eq!(std::fs::metadata(&path).unwrap().len(), len);
        assert!(
            chunk_store().read(&path).is_ok(),
            "the chunk must be published as an object"
        );

        // Wipe the cache exactly as a fresh container would.
        std::fs::remove_file(&path).unwrap();
        let reopened = crate::knn_index::spann::FileIndex::open(&path, &opts).unwrap();
        assert_eq!(reopened.len(), index.len());
        let header = crate::knn_index::spann::read_header(&path).unwrap();
        assert_eq!(header.segment, 0);
        // The shared slot is process-wide; put the local backend back so this
        // test cannot redirect another one's writes.
        install_chunk_store(Arc::new(LocalChunkStore));
    }

    /// A file below `data/` uses the reference's key (`RemovePrefix`), so a
    /// directory the port persisted is the directory the reference expects.
    #[test]
    fn a_data_file_uses_the_reference_key() {
        let _guard = LOCK.lock().unwrap_or_else(|poisoned| poisoned.into_inner());
        let root = tempfile::tempdir().unwrap();
        let root = root.path();
        let store = PersistentChunkStore::new(manager(root), root);
        let path = root.join("data/db_1/tbl_2/idx_3/seg_0/ft_0000000000000000.len");
        std::fs::create_dir_all(path.parent().unwrap()).unwrap();
        store.write(&path, &8u32.to_le_bytes()).unwrap();
        assert!(store
            .manager
            .lookup_key("db_1/tbl_2/idx_3/seg_0/ft_0000000000000000.len")
            .is_some());
    }

    /// `write_dedicated` is the reference's `try_compose = false` (one object per
    /// chunk file); `write` is its default (parts of one bounded object).
    #[test]
    fn a_dedicated_write_gets_an_object_of_its_own() {
        let _guard = LOCK.lock().unwrap_or_else(|poisoned| poisoned.into_inner());
        let root = tempfile::tempdir().unwrap();
        let root = root.path();
        let store = PersistentChunkStore::new(manager(root), root);
        let dir = root.join("fulltext/db_tbl_idx");
        std::fs::create_dir_all(&dir).unwrap();
        store.write_dedicated(&dir.join("a.dic"), b"chunk").unwrap();
        store.write_dedicated(&dir.join("a.pos"), b"chunk").unwrap();
        let first = store
            .manager
            .lookup_key("fulltext/db_tbl_idx/a.dic")
            .unwrap();
        let second = store
            .manager
            .lookup_key("fulltext/db_tbl_idx/a.pos")
            .unwrap();
        assert_ne!(first.obj_key, second.obj_key, "one object per chunk file");
        assert_eq!(first.part_offset, 0);
        assert_eq!(second.part_offset, 0);

        // The composed path shares the current object.
        store.write(&dir.join("b.col"), b"block").unwrap();
        store.write(&dir.join("c.col"), b"block").unwrap();
        let b = store
            .manager
            .lookup_key("fulltext/db_tbl_idx/b.col")
            .unwrap();
        let c = store
            .manager
            .lookup_key("fulltext/db_tbl_idx/c.col")
            .unwrap();
        assert_eq!(b.obj_key, c.obj_key, "composed into the current object");
        assert!(c.part_offset >= b.part_offset + b.part_size);
    }

    /// Removing a tree forgets the parts, so the objects do not keep bytes
    /// alive that nothing names any more.
    #[test]
    fn removing_a_tree_forgets_its_parts() {
        let _guard = LOCK.lock().unwrap_or_else(|poisoned| poisoned.into_inner());
        let root = tempfile::tempdir().unwrap();
        let root = root.path();
        let store = PersistentChunkStore::new(manager(root), root);
        let dir = root.join("fulltext/db_tbl_idx");
        std::fs::create_dir_all(&dir).unwrap();
        for name in ["a.dic", "a.pos", "a.len"] {
            store.write(&dir.join(name), b"x").unwrap();
        }
        assert_eq!(store.manager.keys_under("fulltext/db_tbl_idx/").len(), 3);
        store.remove_tree(&dir);
        assert!(store.manager.keys_under("fulltext/db_tbl_idx/").is_empty());
        assert!(!dir.exists());
    }

    /// The installed store is what `format::write_synced` and
    /// `MappedFile::open` go through; the local backend keeps the old
    /// behaviour, and installing a persistent one routes both.
    #[test]
    fn the_installed_store_owns_writes_and_mappings() {
        let _guard = LOCK.lock().unwrap_or_else(|poisoned| poisoned.into_inner());
        let root = tempfile::tempdir().unwrap();
        let root = root.path();
        install_chunk_store(Arc::new(PersistentChunkStore::new(manager(root), root)));
        let path = root.join("data/db_1/tbl_2/idx_3/seg_0/blob");
        std::fs::create_dir_all(path.parent().unwrap()).unwrap();
        crate::format::write_synced(&path, b"through the store").unwrap();
        let mapped = MappedFile::open(&path).unwrap();
        assert_eq!(&mapped[..], b"through the store");
        install_local_chunk_store();
        // And the local backend forgets nothing it never recorded.
        crate::format::write_synced(&path, b"local").unwrap();
        assert_eq!(std::fs::read(&path).unwrap(), b"local");
    }
}
