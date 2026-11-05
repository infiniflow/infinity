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
use std::sync::{Arc, Mutex};
use std::time::{SystemTime, UNIX_EPOCH};

use ::common::{Error, Result};

use crate::MetaStore;

/// Alignment of a part inside a composed object (`PersistenceManager::ObjAlignment`).
pub const OBJ_ALIGNMENT: usize = 8;

/// The address used for an empty file (`ObjAddr::KeyEmpty`).
pub const KEY_EMPTY: &str = "KEY_EMPTY";
/// Catalog prefix of the path → object mapping (`KeyEncode::PMObjectKey`).
pub const PM_OBJECT_PREFIX: &str = "pm|object|";
/// Catalog prefix of the per-object statistics (`KeyEncode::PMObjectStatKey`).
pub const PM_OBJECT_STAT_PREFIX: &str = "pm|object_stat|";
/// `DEFAULT_PERSISTENCE_OBJECT_SIZE_LIMIT` (`default_values.cppm:218`).
pub const DEFAULT_OBJECT_SIZE_LIMIT: usize = 128 * 1024 * 1024;

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
    /// C++ `ObjAddr::Serialize` (`persistence_manager_impl.cpp:41`).
    pub fn serialize(&self) -> String {
        format!(
            "{{\"obj_key\":\"{}\",\"part_offset\":{},\"part_size\":{}}}",
            self.obj_key, self.part_offset, self.part_size
        )
    }

    /// C++ `ObjAddr::Deserialize` (`persistence_manager_impl.cpp:48`).
    pub fn deserialize(text: &str) -> Option<Self> {
        let field = |name: &str| -> Option<&str> {
            let key = format!("\"{name}\":");
            let start = text.find(&key)? + key.len();
            let rest = &text[start..];
            let end = rest.find([',', '}'])?;
            Some(rest[..end].trim())
        };
        Some(Self {
            obj_key: field("obj_key")?.trim_matches('"').to_string(),
            part_offset: field("part_offset")?.parse().ok()?,
            part_size: field("part_size")?.parse().ok()?,
        })
    }

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
    /// C++ `ObjStat::ToString`: the value stored under `pm|object_stat|`.
    /// `ref_count` is in memory only and is deliberately absent.
    pub fn to_json(&self) -> String {
        let ranges: Vec<String> = self
            .deleted_ranges
            .iter()
            .map(|r| format!("{{\"start\":{},\"end\":{}}}", r.start, r.end))
            .collect();
        format!(
            "{{\"deleted_ranges\":[{}],\"obj_size\":{},\"parts\":{}}}",
            ranges.join(","),
            self.obj_size,
            self.parts
        )
    }

    /// Parses a `pm|object_stat|` value.
    pub fn from_json(text: &str) -> Option<Self> {
        let field = |name: &str| -> Option<usize> {
            let key = format!("\"{name}\":");
            let start = text.find(&key)? + key.len();
            let rest = &text[start..];
            let end = rest.find([',', '}'])?;
            rest[..end].trim().parse().ok()
        };
        let mut stat = Self::new(field("obj_size")?, field("parts")?, 0);
        let mut rest = text;
        while let Some(start) = rest.find("{\"start\":") {
            let tail = &rest[start + 9..];
            let s: usize = tail.split(',').next()?.trim().parse().ok()?;
            let e_start = tail.find("\"end\":")? + 6;
            let e: usize = tail[e_start..].split('}').next()?.trim().parse().ok()?;
            stat.deleted_ranges.insert(Range { start: s, end: e });
            rest = &tail[e_start..];
        }
        Some(stat)
    }

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
    /// The catalog the `pm|object|` / `pm|object_stat|` records live in.
    /// `None` keeps the mapping in memory only (the unit tests and a
    /// directory that is not shared with the reference).
    store: Option<Arc<dyn MetaStore>>,
    workspace: PathBuf,
    /// Data-dir prefix (`RemovePrefix` strips it from a path).
    local_data_dir: PathBuf,
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
        let local_data_dir = data_dir.as_ref().to_path_buf();
        let manager = Self {
            store: None,
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

    /// Writes `pm|object|<rel>`; the in-memory map stays authoritative.
    fn record_file(&self, rel: &str, addr: &ObjAddr) {
        if let Some(store) = self.store.as_ref() {
            let key = format!("{PM_OBJECT_PREFIX}{rel}");
            let _ = store.put(key.as_bytes(), addr.serialize().as_bytes());
        }
    }

    fn forget_file(&self, rel: &str) {
        if let Some(store) = self.store.as_ref() {
            let key = format!("{PM_OBJECT_PREFIX}{rel}");
            let _ = store.delete(key.as_bytes());
        }
    }

    /// Writes `pm|object_stat|<key>`.
    fn record_stat(&self, key: &str, stat: &ObjStat) {
        if let Some(store) = self.store.as_ref() {
            if key == KEY_EMPTY {
                return;
            }
            let key = format!("{PM_OBJECT_STAT_PREFIX}{key}");
            let _ = store.put(key.as_bytes(), stat.to_json().as_bytes());
        }
    }

    fn forget_stat(&self, key: &str) {
        if let Some(store) = self.store.as_ref() {
            let key = format!("{PM_OBJECT_STAT_PREFIX}{key}");
            let _ = store.delete(key.as_bytes());
        }
    }

    /// A manager whose mapping lives in `store` (the catalog), as the
    /// reference keeps it. The records are loaded on construction, so a
    /// directory the other engine persisted opens with its objects known.
    pub fn with_catalog(
        store: Arc<dyn MetaStore>,
        workspace: impl AsRef<Path>,
        data_dir: impl AsRef<Path>,
        object_size_limit: usize,
    ) -> Result<Self> {
        let mut manager = Self::new(workspace, data_dir, object_size_limit)?;
        manager.store = Some(store);
        manager.load_records()?;
        Ok(manager)
    }

    fn load_records(&self) -> Result<()> {
        let Some(store) = self.store.as_ref() else {
            return Ok(());
        };
        let mut inner = self.inner.lock().unwrap();
        for (key, value) in store.scan_prefix(PM_OBJECT_PREFIX.as_bytes())? {
            let rel = String::from_utf8_lossy(&key[PM_OBJECT_PREFIX.len()..]).to_string();
            if let Some(addr) = ObjAddr::deserialize(&String::from_utf8_lossy(&value)) {
                inner.files.insert(rel, addr);
            }
        }
        for (key, value) in store.scan_prefix(PM_OBJECT_STAT_PREFIX.as_bytes())? {
            let obj_key = String::from_utf8_lossy(&key[PM_OBJECT_STAT_PREFIX.len()..]).to_string();
            if let Some(stat) = ObjStat::from_json(&String::from_utf8_lossy(&value)) {
                inner.objects.insert(obj_key, stat);
            }
        }
        Ok(())
    }

    /// C++ `RemovePrefix`: strip the data-dir prefix, keep relative paths, and
    /// return an empty string for an absolute path outside the data dir.
    fn remove_prefix(&self, path: &Path) -> String {
        if let Ok(rest) = path.strip_prefix(&self.local_data_dir) {
            return rest.to_string_lossy().replace('\\', "/");
        }
        if path.is_relative() {
            return path.to_string_lossy().replace('\\', "/");
        }
        String::new()
    }

    /// C++ `Persist`: store `tmp_file_path` under the key `file_path`.
    #[must_use]
    pub fn persist(
        &self,
        file_path: &Path,
        tmp_file_path: impl AsRef<Path>,
    ) -> Result<PersistWriteResult> {
        let tmp_file_path = tmp_file_path.as_ref();
        let local_path = self.remove_prefix(file_path);
        if local_path.is_empty() {
            return Err(Error::Unrecoverable(format!(
                "Failed to find local path of {}",
                file_path.display()
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
            self.record_file(&local_path, &obj_addr);
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
            let stat = ObjStat::new(src_size, 1, 0);
            self.record_stat(&obj_key, &stat);
            inner.objects.insert(obj_key.clone(), stat);
            self.record_file(&local_path, &obj_addr);
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
            self.record_stat(&obj_addr.obj_key, &stat);
            inner.objects.insert(obj_addr.obj_key.clone(), stat);
            self.record_file(&local_path, &obj_addr);
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
    #[must_use]
    pub fn current_obj_finalize(&self) -> PersistWriteResult {
        let mut result = PersistWriteResult::default();
        let mut inner = self.inner.lock().unwrap();
        self.finalize_locked(&mut inner, &mut result.persist_keys);
        result
    }

    /// C++ `GetObjCache`: look up the address and take a reference.
    #[must_use]
    pub fn get_obj_cache(&self, file_path: &Path) -> PersistReadResult {
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
    #[must_use]
    pub fn put_obj_cache(&self, file_path: &Path) -> Result<PersistWriteResult> {
        let local_path = self.remove_prefix(file_path);
        if local_path.is_empty() {
            return Err(Error::Unrecoverable(format!(
                "Failed to find file path of {}",
                file_path.display()
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
    #[must_use]
    pub fn cleanup(&self, file_path: &Path) -> Result<PersistWriteResult> {
        let local_path = self.remove_prefix(file_path);
        if local_path.is_empty() {
            return Err(Error::Unrecoverable(format!(
                "Failed to find local path of {}",
                file_path.display()
            )));
        }
        let mut result = PersistWriteResult::default();
        let mut inner = self.inner.lock().unwrap();
        let Some(obj_addr) = inner.files.remove(&local_path) else {
            return Ok(result);
        };
        self.forget_file(&local_path);
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
    #[must_use]
    pub fn get_file_size(&self, file_path: &Path) -> Result<usize> {
        let local_path = self.remove_prefix(file_path);
        if local_path.is_empty() {
            return Err(Error::Unrecoverable(format!(
                "Failed to find local path of {}",
                file_path.display()
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
        let current_size = dst.metadata().map_err(|e| Error::io(&dst_path, e))?.len() as usize;
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
        let stat = ObjStat::new(
            inner.current_object_size,
            inner.current_object_parts,
            inner.current_object_ref_count,
        );
        self.record_stat(&inner.current_object_key, &stat);
        inner.objects.insert(inner.current_object_key.clone(), stat);
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
                    inner
                        .objects
                        .get(&obj_addr.obj_key)
                        .cloned()
                        .ok_or_else(|| {
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
            self.forget_stat(&obj_addr.obj_key);
        } else {
            self.record_stat(&obj_addr.obj_key, &stat);
            inner.objects.insert(obj_addr.obj_key.clone(), stat);
        }
        Ok(())
    }

    /// The port's open-time step: materialise every `data/` file the records
    /// describe but the directory does not hold. The reference reads through
    /// `GetObjCache` instead; for a directory both engines share, restoring the
    /// files is what makes the port's block readers work.
    pub fn restore_missing_files(&self) -> Result<usize> {
        let mut restored = 0usize;
        let files: Vec<(String, ObjAddr)> = {
            let inner = self.inner.lock().unwrap();
            inner
                .files
                .iter()
                .map(|(k, v)| (k.clone(), v.clone()))
                .collect()
        };
        for (rel, addr) in files {
            let target = self.local_data_dir.join(&rel);
            if target.exists() {
                continue;
            }
            if let Some(parent) = target.parent() {
                fs::create_dir_all(parent).map_err(|e| Error::io(parent, e))?;
            }
            if addr.obj_key == KEY_EMPTY || addr.part_size == 0 {
                fs::write(&target, b"").map_err(|e| Error::io(&target, e))?;
                restored += 1;
                continue;
            }
            let object = self.get_obj_path(&addr.obj_key);
            let blob = match fs::read(&object) {
                Ok(blob) => blob,
                Err(_) => continue, // no object to restore from: leave it missing
            };
            let start = addr.part_offset;
            let end = start + addr.part_size;
            if end > blob.len() {
                return Err(Error::Storage(format!(
                    "object {} is shorter than its record for '{rel}'",
                    addr.obj_key
                )));
            }
            fs::write(&target, &blob[start..end]).map_err(|e| Error::io(&target, e))?;
            restored += 1;
        }
        Ok(restored)
    }

    /// The port's checkpoint step: publish every file under the data directory
    /// (`try_compose = true`, in path order) and seal the current object.
    pub fn persist_directory(&self) -> Result<usize> {
        let root = self.local_data_dir.clone();
        let mut files: Vec<PathBuf> = Vec::new();
        let mut stack = vec![root];
        while let Some(dir) = stack.pop() {
            let Ok(entries) = fs::read_dir(&dir) else {
                continue;
            };
            for entry in entries.flatten() {
                let path = entry.path();
                if path.is_dir() {
                    stack.push(path);
                } else {
                    files.push(path);
                }
            }
        }
        files.sort();
        // `persist` consumes its `tmp_file_path` (the reference's spool file),
        // while the port's `data/` is also its local cache: spool a copy and
        // persist that, so the checkpoint does not delete the files it just
        // wrote.
        let spool = self.workspace.join(".spool");
        fs::create_dir_all(&spool).map_err(|e| Error::io(&spool, e))?;
        let mut written = 0usize;
        for path in &files {
            let key = path.to_string_lossy().to_string();
            let tmp = spool.join(unique_key());
            fs::copy(path, &tmp).map_err(|e| Error::io(&tmp, e))?;
            let result = self.persist(Path::new(&key), &tmp)?;
            self.apply_write_result(&result)?;
            written += 1;
        }
        let result = self.current_obj_finalize();
        self.apply_write_result(&result)?;
        Ok(written)
    }
}

impl PersistenceManager {
    /// C++ `PersistResultHandler::HandleWriteResult`: local uploads are already
    /// in place for a local workspace; the objects the cleanup dropped are
    /// removed.
    pub fn apply_write_result(&self, result: &PersistWriteResult) -> Result<()> {
        for key in result
            .drop_keys
            .iter()
            .chain(result.drop_from_remote_keys.iter())
        {
            let path = self.get_obj_path(key);
            match fs::remove_file(&path) {
                Ok(()) => {}
                Err(e) if e.kind() == std::io::ErrorKind::NotFound => {}
                Err(e) => return Err(Error::io(&path, e)),
            }
        }
        Ok(())
    }

    /// C++ `PersistResultHandler::HandleReadResult`: a local object is always
    /// cached, so the address is returned unchanged.
    pub fn resolve_read(&self, result: PersistReadResult) -> ObjAddr {
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
        fn check_obj_data(&self, local_file_path: &Path, data: &str) {
            let result = self.pm.get_obj_cache(local_file_path);
            let obj_addr = self.pm.resolve_read(result);
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
            self.pm.apply_write_result(&res).unwrap();
        }
    }

    #[test]
    fn persist_file_basic() {
        let fixture = Fixture::new();
        let persist_str = "Persistence Manager Test";
        let file_path = fixture.write_file("persist_file", persist_str);

        let result = fixture
            .pm
            .persist(Path::new(&file_path), Path::new(&file_path))
            .expect("persist");
        fixture.pm.apply_write_result(&result).unwrap();
        let obj_addr = &result.obj_addr;
        assert!(obj_addr.valid());
        assert_eq!(obj_addr.part_size, persist_str.len());

        let result2 = fixture.pm.current_obj_finalize();
        fixture.pm.apply_write_result(&result2).unwrap();

        fixture.check_obj_data(Path::new(&file_path), persist_str);
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
                .persist(Path::new(&file_path), Path::new(&file_path))
                .expect("persist");
            fixture.pm.apply_write_result(&result).unwrap();
            assert!(result.obj_addr.valid());
            assert_eq!(result.obj_addr.part_size, persist_strs[i].len());
            obj_addrs.push(result.obj_addr);
        }
        assert_eq!(file_paths.len(), persist_strs.len());
        assert_eq!(file_paths.len(), obj_addrs.len());

        let result = fixture.pm.current_obj_finalize();
        fixture.pm.apply_write_result(&result).unwrap();

        for (path, data) in file_paths.iter().zip(persist_strs.iter()) {
            fixture.check_obj_data(Path::new(path), data);
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
                    let result = pm
                        .persist(Path::new(file_path), Path::new(file_path))
                        .expect("persist");
                    pm.apply_write_result(&result).unwrap();
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
        fixture.pm.apply_write_result(&result).unwrap();

        for (path, data) in file_paths.iter().zip(persist_strs.iter()) {
            fixture.check_obj_data(Path::new(path), data);
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
                .persist(Path::new(&file_path), Path::new(&file_path))
                .expect("persist");
            fixture.pm.apply_write_result(&result).unwrap();
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
        fixture.pm.apply_write_result(&result).unwrap();

        for (path, data) in file_paths.iter().zip(persist_strs.iter()) {
            fixture.check_obj_data(Path::new(path), data);
        }
        for obj_path in &obj_paths {
            assert!(Path::new(obj_path).exists());
        }

        // The reference shuffles the paths; any order is valid because cleanup
        // only depends on the recorded ranges.
        file_paths.rotate_left(3);
        for file_path in &file_paths {
            let result = fixture.pm.cleanup(Path::new(file_path)).expect("cleanup");
            fixture.pm.apply_write_result(&result).unwrap();
        }
        for obj_path in &obj_paths {
            assert!(!Path::new(obj_path).exists(), "{obj_path} still exists");
        }
    }
    // -----------------------------------------------------------------------
    // The records and the compose rules, against the values the reference
    // wrote (measured on a 100-row table with `storage_type = "local"`).
    // -----------------------------------------------------------------------

    #[test]
    fn obj_addr_json_matches_the_reference() {
        let addr = ObjAddr {
            obj_key: "01a167be-0114-7d50-b77c-82b3a6d8948e".to_string(),
            part_offset: 65560,
            part_size: 33816,
        };
        let text = addr.serialize();
        assert_eq!(
            text,
            "{\"obj_key\":\"01a167be-0114-7d50-b77c-82b3a6d8948e\",\"part_offset\":65560,\"part_size\":33816}"
        );
        assert_eq!(ObjAddr::deserialize(&text), Some(addr));
    }

    #[test]
    fn obj_stat_json_matches_the_reference() {
        let stat = ObjStat::new(231496, 3, 0);
        assert_eq!(
            stat.to_json(),
            "{\"deleted_ranges\":[],\"obj_size\":231496,\"parts\":3}"
        );
        let parsed = ObjStat::from_json(&stat.to_json()).unwrap();
        assert_eq!((parsed.obj_size, parsed.parts), (231496, 3));
        assert!(parsed.deleted_ranges.is_empty());
    }

    #[test]
    fn empty_file_maps_to_key_empty() {
        let fixture = Fixture::new();
        let path = fixture.write_file("empty", "");
        let result = fixture
            .pm
            .persist(Path::new(&path), Path::new(&path))
            .expect("persist");
        assert_eq!(result.obj_addr.obj_key, KEY_EMPTY);
        assert_eq!(result.obj_addr.part_size, 0);
        assert_eq!(result.persist_keys, vec![KEY_EMPTY.to_string()]);
    }

    #[test]
    fn a_file_at_the_limit_gets_a_dedicated_object() {
        let fixture = Fixture::new();
        let data = "x".repeat(OBJ_SIZE_LIMIT);
        let path = fixture.write_file("big", &data);
        let result = fixture
            .pm
            .persist(Path::new(&path), Path::new(&path))
            .expect("persist");
        assert_ne!(result.obj_addr.obj_key, KEY_EMPTY);
        assert_eq!(result.obj_addr.part_offset, 0);
        assert_eq!(result.obj_addr.part_size, OBJ_SIZE_LIMIT);
        // The object holds exactly that file (no alignment padding, no footer).
        let object = fixture.pm.get_obj_path(&result.obj_addr.obj_key);
        assert_eq!(
            fs::metadata(&object).unwrap().len() as usize,
            OBJ_SIZE_LIMIT
        );
    }

    #[test]
    fn a_composed_object_ends_with_the_footer() {
        let fixture = Fixture::new();
        let first = fixture.write_file("a", "first part");
        let second = fixture.write_file("b", "second");
        fixture
            .pm
            .persist(Path::new(&first), Path::new(&first))
            .unwrap();
        let result = fixture
            .pm
            .persist(Path::new(&second), Path::new(&second))
            .unwrap();
        let object = fixture.pm.get_obj_path(&result.obj_addr.obj_key);
        let sealed = fixture.pm.current_obj_finalize();
        fixture.pm.apply_write_result(&sealed).unwrap();
        let bytes = fs::read(&object).unwrap();
        // Two parts -> a `u32 = 1` footer after the aligned payload.
        assert_eq!(&bytes[bytes.len() - 4..], &1u32.to_le_bytes());
        // The footer starts at `current_object_size_`, the end of the last
        // part, not at the next aligned offset (`CurrentObjFinalizeNoLock`).
        assert_eq!(
            bytes.len(),
            (result.obj_addr.part_offset + result.obj_addr.part_size) as usize + 4
        );
    }

    #[test]
    fn a_full_object_is_sealed_and_a_second_one_starts() {
        let fixture = Fixture::new();
        let chunk = "y".repeat(70);
        let first = fixture.write_file("c1", &chunk);
        let second = fixture.write_file("c2", &chunk);
        let a = fixture
            .pm
            .persist(Path::new(&first), Path::new(&first))
            .unwrap()
            .obj_addr;
        // 70 + 70 > 128: the second file does not fit in the current object.
        let b = fixture
            .pm
            .persist(Path::new(&second), Path::new(&second))
            .unwrap()
            .obj_addr;
        assert_ne!(a.obj_key, b.obj_key);
        assert_eq!(b.part_offset, 0);
        let stat = fixture.pm.inner.lock().unwrap();
        assert_eq!(stat.objects.get(&a.obj_key).map(|s| s.parts), Some(1));
    }

    #[test]
    fn catalog_records_round_trip() {
        let tmp = tempdir().unwrap();
        let workspace = tmp.path().join("persistence");
        let data_dir = tmp.path().join("data");
        fs::create_dir_all(data_dir.join("db_1/tbl_0/seg_0/blk_0")).unwrap();
        let catalog = tmp.path().join("catalog");
        fs::create_dir_all(&catalog).unwrap();
        let store = Arc::new(crate::RocksMetaStore::open(&catalog).unwrap());

        let file = data_dir.join("db_1/tbl_0/seg_0/blk_0/version");
        fs::write(&file, b"payload").unwrap();
        // `persist` consumes the spool file, as the reference does.
        let spool = tmp.path().join("spool");
        fs::write(&spool, b"payload").unwrap();

        {
            let pm = PersistenceManager::with_catalog(
                store.clone() as Arc<dyn MetaStore>,
                &workspace,
                &data_dir,
                OBJ_SIZE_LIMIT,
            )
            .unwrap();
            let result = pm.persist(&file, &spool).unwrap();
            assert!(!result.obj_addr.obj_key.is_empty());
        }

        // A second manager sees the record and can rebuild the data file.
        let pm = PersistenceManager::with_catalog(
            store.clone() as Arc<dyn MetaStore>,
            &workspace,
            &data_dir,
            OBJ_SIZE_LIMIT,
        )
        .unwrap();
        assert_eq!(pm.get_file_size(&file).unwrap(), 7);
        fs::remove_file(&file).unwrap();
        assert_eq!(pm.restore_missing_files().unwrap(), 1);
        assert_eq!(fs::read(&file).unwrap(), b"payload");
    }
}
