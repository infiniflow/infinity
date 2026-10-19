//! Local-storage persistence manager.
//!
//! Port of the C++ `PersistenceManager` (`src/storage/persistence/`): data files
//! are packed into aligned objects of a bounded size, the local-path → object
//! mapping is kept in a KV store, and deleting a file records its range in the
//! object stat so an object is dropped once every part has been cleaned up.
//!
//! The reference has two backends: a local one (`VirtualStore` over the local
//! disk) and a remote object store. This port keeps the local profile; the
//! `PersistResultHandler` is the hook the reference uses to upload finalized
//! objects and to delete dropped ones.

use std::collections::{BTreeSet, HashMap};
use std::fs;
use std::io::Write;
use std::path::{Path, PathBuf};
use std::sync::atomic::{AtomicU64, Ordering};
use std::sync::Mutex;
use std::time::{SystemTime, UNIX_EPOCH};

use infinity_common::{Error, Result};

/// Alignment of a part inside a composed object (`PersistenceManager::ObjAlignment`).
pub const OBJ_ALIGNMENT: usize = 8;

/// The address used for an empty file (`ObjAddr::KeyEmpty`).
pub const KEY_EMPTY: &str = "KEY_EMPTY";

fn align_up(value: usize) -> usize {
    (value + OBJ_ALIGNMENT - 1) & !(OBJ_ALIGNMENT - 1)
}

/// C++ `ObjAddr`: where a persisted part lives.
#[derive(Clone, Debug, Default, PartialEq, Eq)]
pub struct ObjAddr {
    pub obj_key: String,
    pub part_offset: usize,
    pub part_size: usize,
}

impl ObjAddr {
    pub fn valid(&self) -> bool {
        !self.obj_key.is_empty()
    }
}

/// C++ `PersistWriteResult`.
#[derive(Clone, Debug, Default)]
pub struct PersistWriteResult {
    pub obj_addr: ObjAddr,
    /// Objects that should be persisted to durable storage.
    pub persist_keys: Vec<String>,
    /// Objects that should be removed from the local disk.
    pub drop_keys: Vec<String>,
    /// Objects whose every part is gone; remove them from local and remote.
    pub drop_from_remote_keys: Vec<String>,
}

/// C++ `PersistReadResult`.
#[derive(Clone, Debug, Default)]
pub struct PersistReadResult {
    pub obj_addr: ObjAddr,
    pub drop_keys: Vec<String>,
    pub drop_from_remote_keys: Vec<String>,
}

/// A half-open byte range inside an object (`obj_status.cppm`).
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Range {
    pub start: usize,
    pub end: usize,
}

impl Range {
    fn cover(&self, other: &Range) -> bool {
        self.start <= other.start && other.end <= self.end
    }

    fn intersect(&self, other: &Range) -> bool {
        self.start < other.end && other.start < self.end
    }
}

impl PartialOrd for Range {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        Some(self.cmp(other))
    }
}

impl Ord for Range {
    /// The reference's `std::set<Range>` compares `start_` only.
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        self.start.cmp(&other.start)
    }
}

/// C++ `ObjStat`: per-object bookkeeping.
#[derive(Clone, Debug, Default)]
pub struct ObjStat {
    /// Object size excluding a compose footer.
    pub obj_size: usize,
    /// Number of parts appended to the object.
    pub parts: usize,
    /// Number of users (readers and writers) of the object.
    pub ref_count: usize,
    /// Ranges already cleaned up, merged when adjacent.
    pub deleted_ranges: BTreeSet<Range>,
}

impl ObjStat {
    fn new(obj_size: usize, parts: usize, ref_count: usize) -> Self {
        Self {
            obj_size,
            parts,
            ref_count,
            deleted_ranges: BTreeSet::new(),
        }
    }
}

struct Inner {
    /// Local path (relative to the data dir) -> object address.
    files: HashMap<String, ObjAddr>,
    objects: HashMap<String, ObjStat>,
    current_object_key: String,
    current_object_size: usize,
    current_object_parts: usize,
    current_object_ref_count: usize,
}

/// Pack local files into bounded objects and track their deletion.
pub struct PersistenceManager {
    workspace: PathBuf,
    /// Data-dir prefix with a trailing separator, as in the reference.
    local_data_dir: String,
    object_size_limit: usize,
    inner: Mutex<Inner>,
}

impl PersistenceManager {
    /// C++ `PersistenceManager::PersistenceManager` (local storage).
    pub fn new(
        workspace: impl AsRef<Path>,
        data_dir: impl AsRef<Path>,
        object_size_limit: usize,
    ) -> Result<Self> {
        let workspace = workspace.as_ref().to_path_buf();
        fs::create_dir_all(&workspace).map_err(|e| Error::io(&workspace, e))?;
        let mut local_data_dir = data_dir.as_ref().to_string_lossy().into_owned();
        if !local_data_dir.ends_with('/') {
            local_data_dir.push('/');
        }
        let manager = Self {
            workspace,
            local_data_dir,
            object_size_limit,
            inner: Mutex::new(Inner {
                files: HashMap::new(),
                objects: HashMap::new(),
                current_object_key: unique_key(),
                current_object_size: 0,
                current_object_parts: 0,
                current_object_ref_count: 0,
            }),
        };
        // The empty-object address points at a real (empty) file.
        let empty_path = manager.get_obj_path(KEY_EMPTY);
        fs::write(&empty_path, b"").map_err(|e| Error::io(&empty_path, e))?;
        Ok(manager)
    }

    /// C++ `GetObjPath`.
    pub fn get_obj_path(&self, obj_key: &str) -> PathBuf {
        self.workspace.join(obj_key)
    }

    /// C++ `RemovePrefix`: strip the data-dir prefix, keep relative paths, and
    /// return an empty string for an absolute path outside the data dir.
    fn remove_prefix(&self, path: &str) -> String {
        if let Some(rest) = path.strip_prefix(&self.local_data_dir) {
            return rest.to_string();
        }
        if !path.starts_with('/') {
            return path.to_string();
        }
        String::new()
    }

    /// C++ `Persist`: store `tmp_file_path` under the key `file_path`.
    pub fn persist(
        &self,
        file_path: &str,
        tmp_file_path: impl AsRef<Path>,
    ) -> Result<PersistWriteResult> {
        let tmp_file_path = tmp_file_path.as_ref();
        let local_path = self.remove_prefix(file_path);
        if local_path.is_empty() {
            return Err(Error::Unrecoverable(format!(
                "Failed to find local path of {local_path}"
            )));
        }
        let src_size = fs::metadata(tmp_file_path)
            .map_err(|e| Error::io(tmp_file_path, e))?
            .len() as usize;

        let mut result = PersistWriteResult::default();
        let mut inner = self.inner.lock().unwrap();
        let old = inner.files.get(&local_path).cloned();

        if src_size == 0 {
            fs::remove_file(tmp_file_path).map_err(|e| Error::io(tmp_file_path, e))?;
            let obj_addr = ObjAddr {
                obj_key: KEY_EMPTY.to_string(),
                part_offset: 0,
                part_size: 0,
            };
            inner.files.insert(local_path, obj_addr.clone());
            result.persist_keys.push(KEY_EMPTY.to_string());
            result.obj_addr = obj_addr;
        } else if src_size >= self.object_size_limit {
            let obj_key = unique_key();
            let dst_path = self.get_obj_path(&obj_key);
            fs::rename(tmp_file_path, &dst_path).map_err(|e| Error::io(&dst_path, e))?;
            let obj_addr = ObjAddr {
                obj_key: obj_key.clone(),
                part_offset: 0,
                part_size: src_size,
            };
            inner
                .objects
                .insert(obj_key.clone(), ObjStat::new(src_size, 1, 0));
            inner.files.insert(local_path, obj_addr.clone());
            result.persist_keys.push(obj_key);
            result.obj_addr = obj_addr;
        } else {
            if src_size >= self.current_room(&inner) {
                self.finalize_locked(&mut inner, &mut result.persist_keys);
            }
            inner.current_object_size = align_up(inner.current_object_size);
            let obj_addr = ObjAddr {
                obj_key: inner.current_object_key.clone(),
                part_offset: inner.current_object_size,
                part_size: src_size,
            };
            self.append_locked(&mut inner, tmp_file_path, src_size)?;
            fs::remove_file(tmp_file_path).map_err(|e| Error::io(tmp_file_path, e))?;
            let stat = ObjStat::new(
                inner.current_object_size,
                inner.current_object_parts,
                inner.current_object_ref_count,
            );
            inner.objects.insert(obj_addr.obj_key.clone(), stat);
            inner.files.insert(local_path, obj_addr.clone());
            result.obj_addr = obj_addr;
        }

        // The old object of this local path is no longer reachable.
        if let Some(old_addr) = old {
            self.cleanup_locked(
                &mut inner,
                &old_addr,
                &mut result.persist_keys,
                &mut result.drop_from_remote_keys,
                false,
            )?;
        }
        Ok(result)
    }

    /// C++ `CurrentObjFinalize`.
    pub fn current_obj_finalize(&self) -> PersistWriteResult {
        let mut result = PersistWriteResult::default();
        let mut inner = self.inner.lock().unwrap();
        self.finalize_locked(&mut inner, &mut result.persist_keys);
        result
    }

    /// C++ `GetObjCache`: look up the address and take a reference.
    pub fn get_obj_cache(&self, file_path: &str) -> PersistReadResult {
        let local_path = self.remove_prefix(file_path);
        let mut result = PersistReadResult::default();
        if local_path.is_empty() {
            return result;
        }
        let mut inner = self.inner.lock().unwrap();
        let Some(obj_addr) = inner.files.get(&local_path).cloned() else {
            return result;
        };
        result.obj_addr = obj_addr.clone();
        if obj_addr.part_size == 0 {
            // An empty file never owns a reference.
            debug_assert_eq!(obj_addr.obj_key, KEY_EMPTY);
        } else if obj_addr.obj_key == inner.current_object_key {
            inner.current_object_ref_count += 1;
        } else if let Some(stat) = inner.objects.get_mut(&obj_addr.obj_key) {
            stat.ref_count += 1;
        }
        result
    }

    /// C++ `PutObjCache`: release the reference taken by [`Self::get_obj_cache`].
    pub fn put_obj_cache(&self, file_path: &str) -> Result<PersistWriteResult> {
        let local_path = self.remove_prefix(file_path);
        if local_path.is_empty() {
            return Err(Error::Unrecoverable(format!(
                "Failed to find file path of {file_path}"
            )));
        }
        let mut inner = self.inner.lock().unwrap();
        let Some(obj_addr) = inner.files.get(&local_path).cloned() else {
            return Err(Error::Unrecoverable(format!(
                "Failed to find file_path: {local_path} stored object"
            )));
        };
        if obj_addr.part_size == 0 {
            return Ok(PersistWriteResult::default());
        }
        if obj_addr.obj_key == inner.current_object_key {
            if inner.current_object_ref_count == 0 {
                return Err(Error::Unrecoverable(format!(
                    "PutObjCache current object {} ref count is {}",
                    obj_addr.obj_key, inner.current_object_ref_count
                )));
            }
            inner.current_object_ref_count -= 1;
        } else if let Some(stat) = inner.objects.get_mut(&obj_addr.obj_key) {
            if stat.ref_count == 0 {
                return Err(Error::Unrecoverable(format!(
                    "Release object {} ref count is {}",
                    obj_addr.obj_key, stat.ref_count
                )));
            }
            stat.ref_count -= 1;
        }
        Ok(PersistWriteResult::default())
    }

    /// C++ `Cleanup`: forget a local path and record the part as deleted.
    pub fn cleanup(&self, file_path: &str) -> Result<PersistWriteResult> {
        let local_path = self.remove_prefix(file_path);
        if local_path.is_empty() {
            return Err(Error::Unrecoverable(format!(
                "Failed to find local path of {local_path}"
            )));
        }
        let mut result = PersistWriteResult::default();
        let mut inner = self.inner.lock().unwrap();
        let Some(obj_addr) = inner.files.remove(&local_path) else {
            return Ok(result);
        };
        self.cleanup_locked(
            &mut inner,
            &obj_addr,
            &mut result.persist_keys,
            &mut result.drop_from_remote_keys,
            true,
        )?;
        Ok(result)
    }

    /// C++ `GetFileSize`.
    pub fn get_file_size(&self, file_path: &str) -> Result<usize> {
        let local_path = self.remove_prefix(file_path);
        if local_path.is_empty() {
            return Err(Error::Unrecoverable(format!(
                "Failed to find local path of {local_path}"
            )));
        }
        let inner = self.inner.lock().unwrap();
        inner
            .files
            .get(&local_path)
            .map(|addr| addr.part_size)
            .ok_or_else(|| Error::NotFound(format!("Can't find {local_path}")))
    }

    fn current_room(&self, inner: &Inner) -> usize {
        let limit = self.object_size_limit as i64;
        let size = inner.current_object_size as i64;
        limit.saturating_sub(size) as usize
    }

    /// C++ `CurrentObjAppendNoLock`: append one part, zero-padding the
    /// alignment gap.
    fn append_locked(
        &self,
        inner: &mut Inner,
        tmp_file_path: &Path,
        file_size: usize,
    ) -> Result<()> {
        let dst_path = self.get_obj_path(&inner.current_object_key);
        let mut dst = fs::OpenOptions::new()
            .create(true)
            .append(true)
            .open(&dst_path)
            .map_err(|e| Error::io(&dst_path, e))?;
        let current_size = dst
            .metadata()
            .map_err(|e| Error::io(&dst_path, e))?
            .len() as usize;
        if current_size < inner.current_object_size {
            dst.write_all(&vec![0u8; inner.current_object_size - current_size])
                .map_err(|e| Error::io(&dst_path, e))?;
        }
        let mut src = fs::File::open(tmp_file_path).map_err(|e| Error::io(tmp_file_path, e))?;
        std::io::copy(&mut src, &mut dst).map_err(|e| Error::io(&dst_path, e))?;
        inner.current_object_size += file_size;
        inner.current_object_parts += 1;
        if inner.current_object_size >= self.object_size_limit {
            return Err(Error::Unrecoverable(format!(
                "CurrentObjAppendNoLock object {} size {} exceeds limit {}",
                inner.current_object_key, inner.current_object_size, self.object_size_limit
            )));
        }
        Ok(())
    }

    /// C++ `CurrentObjFinalizeNoLock`.
    fn finalize_locked(&self, inner: &mut Inner, persist_keys: &mut Vec<String>) {
        if inner.current_object_size == 0 {
            return;
        }
        persist_keys.push(inner.current_object_key.clone());
        if inner.current_object_parts > 1 {
            // Compose footer, format version 1.
            let dst_path = self.get_obj_path(&inner.current_object_key);
            let mut out = fs::OpenOptions::new()
                .append(true)
                .open(&dst_path)
                .expect("failed to open composed object for the footer");
            out.write_all(&1u32.to_le_bytes())
                .expect("failed to write composed object footer");
        }
        inner.objects.insert(
            inner.current_object_key.clone(),
            ObjStat::new(
                inner.current_object_size,
                inner.current_object_parts,
                inner.current_object_ref_count,
            ),
        );
        inner.current_object_key = unique_key();
        inner.current_object_size = 0;
        inner.current_object_parts = 0;
        inner.current_object_ref_count = 0;
    }

    /// C++ `CleanupNoLock`.
    fn cleanup_locked(
        &self,
        inner: &mut Inner,
        obj_addr: &ObjAddr,
        persist_keys: &mut Vec<String>,
        drop_from_remote_keys: &mut Vec<String>,
        check_ref_count: bool,
    ) -> Result<()> {
        let mut stat = match inner.objects.get(&obj_addr.obj_key).cloned() {
            Some(stat) => stat,
            None => {
                if obj_addr.obj_key == KEY_EMPTY {
                    debug_assert_eq!(obj_addr.part_size, 0);
                    return Ok(());
                }
                if obj_addr.obj_key == inner.current_object_key {
                    self.finalize_locked(inner, persist_keys);
                    inner.objects.get(&obj_addr.obj_key).cloned().ok_or_else(|| {
                        Error::Unrecoverable(format!(
                            "CleanupNoLock Failed to find object {}",
                            obj_addr.obj_key
                        ))
                    })?
                } else {
                    return Err(Error::Unrecoverable(format!(
                        "CleanupNoLock Failed to find object {}",
                        obj_addr.obj_key
                    )));
                }
            }
        };

        let mut range = Range {
            start: obj_addr.part_offset,
            end: align_up(obj_addr.part_offset + obj_addr.part_size),
        };

        if let Some(prev) = stat.deleted_ranges.range(..range).next_back().copied() {
            if prev.cover(&range) {
                // Cleanup may delete the same local file more than once.
                return Ok(());
            }
            if prev.intersect(&range) {
                return Err(Error::Unrecoverable(format!(
                    "ObjAddr {} range to delete [{}, {}) intersects with prev one [{}, {})",
                    obj_addr.obj_key, range.start, range.end, prev.start, prev.end
                )));
            }
            if range.start == prev.end {
                range.start = prev.start;
                stat.deleted_ranges.remove(&prev);
            }
        }

        if let Some(next) = stat.deleted_ranges.range(range..).next().copied() {
            if next.cover(&range) {
                return Ok(());
            }
            if next.intersect(&range) {
                return Err(Error::Unrecoverable(format!(
                    "ObjAddr {} range to delete [{}, {}) intersects with next one [{}, {})",
                    obj_addr.obj_key, range.start, range.end, next.start, next.end
                )));
            }
            if range.end == next.start {
                range.end = next.end;
                stat.deleted_ranges.remove(&next);
            }
        }

        stat.deleted_ranges.insert(range);
        let obj_size = align_up(stat.obj_size);
        if range.start == 0 && range.end == obj_size && obj_addr.obj_key != inner.current_object_key
        {
            if check_ref_count && stat.ref_count > 0 {
                return Err(Error::Unrecoverable(format!(
                    "CleanupNoLock object {} ref count is {}",
                    obj_addr.obj_key, stat.ref_count
                )));
            }
            drop_from_remote_keys.push(obj_addr.obj_key.clone());
            inner.objects.remove(&obj_addr.obj_key);
        } else {
            inner.objects.insert(obj_addr.obj_key.clone(), stat);
        }
        Ok(())
    }
}

/// C++ `PersistResultHandler`: applies the side effects of a persist/cleanup.
pub struct PersistResultHandler<'a> {
    pm: &'a PersistenceManager,
}

impl<'a> PersistResultHandler<'a> {
    pub fn new(pm: &'a PersistenceManager) -> Self {
        Self { pm }
    }

    /// C++ `HandleWriteResult`. In the local profile `persist_keys` are already
    /// on disk; dropped objects are removed from the local cache.
    pub fn handle_write_result(&self, result: &PersistWriteResult) -> Result<()> {
        for key in result
            .drop_keys
            .iter()
            .chain(result.drop_from_remote_keys.iter())
        {
            let path = self.pm.get_obj_path(key);
            match fs::remove_file(&path) {
                Ok(()) => {}
                Err(e) if e.kind() == std::io::ErrorKind::NotFound => {}
                Err(e) => return Err(Error::io(&path, e)),
            }
        }
        Ok(())
    }

    /// C++ `HandleReadResult`: local objects are always cached.
    pub fn handle_read_result(&self, result: PersistReadResult) -> ObjAddr {
        result.obj_addr
    }
}

/// A unique object key. The reference uses a UUID; a process-local counter is
/// sufficient for the local profile and keeps the dependency set unchanged.
fn unique_key() -> String {
    static SEQ: AtomicU64 = AtomicU64::new(1);
    let nanos = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .map(|d| d.as_nanos())
        .unwrap_or(0);
    format!(
        "{:x}{:x}{:x}",
        std::process::id(),
        nanos,
        SEQ.fetch_add(1, Ordering::Relaxed)
    )
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::collections::HashSet;
    use std::io::{Read, Seek, SeekFrom};
    use tempfile::tempdir;

    const OBJ_SIZE_LIMIT: usize = 128;

    struct Fixture {
        _tmp: tempfile::TempDir,
        workspace: PathBuf,
        file_dir: PathBuf,
        pm: PersistenceManager,
    }

    impl Fixture {
        fn new() -> Self {
            let tmp = tempdir().unwrap();
            let workspace = tmp.path().join("persistence");
            let file_dir = tmp.path().join("persistence_src");
            fs::create_dir_all(&file_dir).unwrap();
            let pm = PersistenceManager::new(&workspace, &file_dir, OBJ_SIZE_LIMIT).unwrap();
            Self {
                _tmp: tmp,
                workspace,
                file_dir,
                pm,
            }
        }

        fn write_file(&self, name: &str, data: &str) -> String {
            let path = self.file_dir.join(name);
            fs::write(&path, data).unwrap();
            path.to_string_lossy().into_owned()
        }

        /// C++ `PersistenceManagerTest::CheckObjData`.
        fn check_obj_data(&self, local_file_path: &str, data: &str) {
            let handler = PersistResultHandler::new(&self.pm);
            let result = self.pm.get_obj_cache(local_file_path);
            let obj_addr = handler.handle_read_result(result);
            let obj_path = self.pm.get_obj_path(&obj_addr.obj_key);
            assert!(obj_path.exists());
            assert_eq!(obj_addr.part_size, data.len());

            let obj_file_size = fs::metadata(&obj_path).unwrap().len() as usize;
            assert!(
                obj_file_size <= OBJ_SIZE_LIMIT,
                "object {obj_file_size} exceeds {OBJ_SIZE_LIMIT}"
            );

            let mut file = fs::File::open(&obj_path).unwrap();
            file.seek(SeekFrom::Start(obj_addr.part_offset as u64))
                .unwrap();
            let mut buffer = vec![0u8; obj_addr.part_size];
            file.read_exact(&mut buffer).unwrap();
            assert_eq!(String::from_utf8(buffer).unwrap(), data);

            let res = self.pm.put_obj_cache(local_file_path).unwrap();
            handler.handle_write_result(&res).unwrap();
        }
    }

    #[test]
    fn persist_file_basic() {
        let fixture = Fixture::new();
        let persist_str = "Persistence Manager Test";
        let file_path = fixture.write_file("persist_file", persist_str);

        let result = fixture
            .pm
            .persist(&file_path, &file_path)
            .expect("persist");
        PersistResultHandler::new(&fixture.pm)
            .handle_write_result(&result)
            .unwrap();
        let obj_addr = &result.obj_addr;
        assert!(obj_addr.valid());
        assert_eq!(obj_addr.part_size, persist_str.len());

        let result2 = fixture.pm.current_obj_finalize();
        PersistResultHandler::new(&fixture.pm)
            .handle_write_result(&result2)
            .unwrap();

        fixture.check_obj_data(&file_path, persist_str);
    }

    #[test]
    fn persist_multi_file() {
        let fixture = Fixture::new();
        let mut file_paths = Vec::new();
        let mut persist_strs = Vec::new();
        let mut obj_addrs = Vec::new();

        for i in 0..10 {
            let persist_str = format!("Persistence Manager Test {i}");
            let file_path = fixture.write_file(&format!("persist_file{i}"), &persist_str);
            file_paths.push(file_path.clone());
            persist_strs.push(persist_str);

            let result = fixture
                .pm
                .persist(&file_path, &file_path)
                .expect("persist");
            PersistResultHandler::new(&fixture.pm)
                .handle_write_result(&result)
                .unwrap();
            assert!(result.obj_addr.valid());
            assert_eq!(result.obj_addr.part_size, persist_strs[i].len());
            obj_addrs.push(result.obj_addr);
        }
        assert_eq!(file_paths.len(), persist_strs.len());
        assert_eq!(file_paths.len(), obj_addrs.len());

        let result = fixture.pm.current_obj_finalize();
        PersistResultHandler::new(&fixture.pm)
            .handle_write_result(&result)
            .unwrap();

        for (path, data) in file_paths.iter().zip(persist_strs.iter()) {
            fixture.check_obj_data(path, data);
        }
    }

    #[test]
    fn persist_file_multi_thread() {
        let fixture = Fixture::new();
        let mut file_paths = Vec::new();
        let mut persist_strs = Vec::new();
        let obj_addrs = Mutex::new(HashMap::new());

        for i in 0..10 {
            let persist_str = format!("Persistence Manager Test {i}");
            let file_path = fixture.write_file(&format!("persist_file{i}"), &persist_str);
            file_paths.push(file_path);
            persist_strs.push(persist_str);
        }

        std::thread::scope(|scope| {
            for (file_path, persist_str) in file_paths.iter().zip(persist_strs.iter()) {
                let obj_addrs = &obj_addrs;
                let pm = &fixture.pm;
                scope.spawn(move || {
                    let result = pm.persist(file_path, file_path).expect("persist");
                    PersistResultHandler::new(pm)
                        .handle_write_result(&result)
                        .unwrap();
                    assert!(result.obj_addr.valid());
                    assert_eq!(result.obj_addr.part_size, persist_str.len());
                    obj_addrs
                        .lock()
                        .unwrap()
                        .insert(file_path.clone(), result.obj_addr);
                });
            }
        });

        assert_eq!(file_paths.len(), persist_strs.len());
        assert_eq!(file_paths.len(), obj_addrs.lock().unwrap().len());

        let result = fixture.pm.current_obj_finalize();
        PersistResultHandler::new(&fixture.pm)
            .handle_write_result(&result)
            .unwrap();

        for (path, data) in file_paths.iter().zip(persist_strs.iter()) {
            fixture.check_obj_data(path, data);
        }
    }

    #[test]
    fn cleanup_basic() {
        let fixture = Fixture::new();
        let mut file_paths = Vec::new();
        let mut persist_strs = Vec::new();
        let mut obj_addrs = Vec::new();
        let mut obj_paths = HashSet::new();

        for i in 0..10 {
            let persist_str = format!("Persistence Manager Test {i}");
            let file_path = fixture.write_file(&format!("persist_file{i}"), &persist_str);
            let result = fixture
                .pm
                .persist(&file_path, &file_path)
                .expect("persist");
            PersistResultHandler::new(&fixture.pm)
                .handle_write_result(&result)
                .unwrap();
            assert!(result.obj_addr.valid());
            assert_eq!(result.obj_addr.part_size, persist_str.len());
            file_paths.push(file_path);
            persist_strs.push(persist_str);
            obj_paths.insert(
                fixture
                    .workspace
                    .join(&result.obj_addr.obj_key)
                    .to_string_lossy()
                    .into_owned(),
            );
            obj_addrs.push(result.obj_addr);
        }
        assert_eq!(file_paths.len(), persist_strs.len());
        assert_eq!(file_paths.len(), obj_addrs.len());

        let result = fixture.pm.current_obj_finalize();
        PersistResultHandler::new(&fixture.pm)
            .handle_write_result(&result)
            .unwrap();

        for (path, data) in file_paths.iter().zip(persist_strs.iter()) {
            fixture.check_obj_data(path, data);
        }
        for obj_path in &obj_paths {
            assert!(Path::new(obj_path).exists());
        }

        // The reference shuffles the paths; any order is valid because cleanup
        // only depends on the recorded ranges.
        file_paths.rotate_left(3);
        for file_path in &file_paths {
            let result = fixture.pm.cleanup(file_path).expect("cleanup");
            PersistResultHandler::new(&fixture.pm)
                .handle_write_result(&result)
                .unwrap();
        }
        for obj_path in &obj_paths {
            assert!(!Path::new(obj_path).exists(), "{obj_path} still exists");
        }
    }
}
