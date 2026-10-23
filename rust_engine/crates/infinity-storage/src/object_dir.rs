//! The reference's object directory: one file per `pm|object|` record.
//!
//! The C++ engine's persistence layer (`PersistenceManager`,
//! `src/storage/persistence/persistence_manager_impl.cpp`) keeps data files
//! out of `data/` and in a "workspace" of **objects**:
//!
//! * a file is appended to an object (8-byte aligned) or moved there whole;
//! * `pm|object|<path>` in the catalog maps a data-dir-relative path to
//!   `{"obj_key": …, "part_offset": …, "part_size": …}`, with the special key
//!   `KEY_EMPTY` for a zero-length file;
//! * `pm|object_stat|<obj_key>` records `{"deleted_ranges":[],"obj_size":…,
//!   "parts":…}`;
//! * the object files sit under `persistence_dir` (local mode) or in the bucket
//!   (MinIO mode) under their `obj_key` name.
//!
//! Reading a block then resolves `pm|object|` and reads the object at
//! `[part_offset, part_offset + part_size)`. This module reads and writes that
//! layout, so a directory one engine packed can be unpacked by the other:
//! [`unpack`] materialises the `data/` tree the port's block readers expect,
//! [`pack`] publishes a checkpoint's files the way the reference publishes them.

use std::path::{Path, PathBuf};

use infinity_common::Result;

use crate::MetaStore;

/// Catalog prefix of the path → object mapping (`KeyEncode::PMObjectKey`).
pub const PM_OBJECT_PREFIX: &str = "pm|object|";
/// Catalog prefix of the per-object statistics (`KeyEncode::PMObjectStatKey`).
pub const PM_OBJECT_STAT_PREFIX: &str = "pm|object_stat|";
/// `ObjAddr::KeyEmpty`: the path exists but holds no bytes.
pub const KEY_EMPTY: &str = "KEY_EMPTY";
/// `PersistenceManager::ObjAlignment`: every part starts 8-byte aligned.
const ALIGN: usize = 8;

/// One `ObjAddr` as the catalog stores it.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct ObjAddr {
    pub obj_key: String,
    pub part_offset: u64,
    pub part_size: u64,
}

impl ObjAddr {
    /// `ObjAddr::Serialize` (`persistence_manager_impl.cpp:41`).
    pub fn to_json(&self) -> String {
        format!(
            "{{\"obj_key\":\"{}\",\"part_offset\":{},\"part_size\":{}}}",
            self.obj_key, self.part_offset, self.part_size
        )
    }

    /// Parses the three fields; the reference writes them in this order.
    pub fn from_json(text: &str) -> Option<Self> {
        let field = |name: &str| -> Option<&str> {
            let key = format!("\"{name}\":");
            let start = text.find(&key)? + key.len();
            let rest = &text[start..];
            let end = rest.find([',', '}'])?;
            Some(rest[..end].trim())
        };
        let obj_key = field("obj_key")?.trim_matches('"').to_string();
        let part_offset = field("part_offset")?.parse().ok()?;
        let part_size = field("part_size")?.parse().ok()?;
        Some(Self {
            obj_key,
            part_offset,
            part_size,
        })
    }
}

/// The files under `data_dir`, as `(relative path, absolute path)`, sorted so a
/// pack is reproducible.
fn data_files(data_dir: &Path) -> Result<Vec<(String, PathBuf)>> {
    let mut out = Vec::new();
    let mut stack = vec![data_dir.to_path_buf()];
    while let Some(dir) = stack.pop() {
        let Ok(entries) = std::fs::read_dir(&dir) else {
            continue;
        };
        for entry in entries.flatten() {
            let path = entry.path();
            if path.is_dir() {
                stack.push(path);
            } else if let Ok(rel) = path.strip_prefix(data_dir) {
                out.push((rel.to_string_lossy().replace('\\', "/"), path.clone()));
            }
        }
    }
    out.sort();
    Ok(out)
}

/// Materialises `<data_dir>/…` from the catalog's `pm|object|` records and the
/// object files in `obj_dir`.
///
/// A file that already exists locally is left alone: the reference keeps the
/// object copy as the durable one, but a directory that still has its data
/// files must not be rewritten from an older object.
pub fn unpack(store: &dyn MetaStore, data_dir: &Path, obj_dir: &Path) -> Result<usize> {
    let mut restored = 0usize;
    for (key, value) in store.scan_prefix(PM_OBJECT_PREFIX.as_bytes())? {
        let rel = String::from_utf8_lossy(&key[PM_OBJECT_PREFIX.len()..]).to_string();
        let target = data_dir.join(&rel);
        if target.exists() {
            continue;
        }
        let Some(addr) = ObjAddr::from_json(&String::from_utf8_lossy(&value)) else {
            continue;
        };
        if let Some(parent) = target.parent() {
            std::fs::create_dir_all(parent).map_err(|e| infinity_common::Error::Io {
                path: parent.to_path_buf(),
                source: e,
            })?;
        }
        if addr.obj_key == KEY_EMPTY || addr.part_size == 0 {
            std::fs::write(&target, b"").map_err(|e| infinity_common::Error::Io {
                path: target.clone(),
                source: e,
            })?;
            restored += 1;
            continue;
        }
        let object = obj_dir.join(&addr.obj_key);
        let blob = match std::fs::read(&object) {
            Ok(blob) => blob,
            Err(_) => continue, // no object to restore from: leave it missing
        };
        let start = addr.part_offset as usize;
        let end = start.saturating_add(addr.part_size as usize);
        if end > blob.len() {
            continue;
        }
        std::fs::write(&target, &blob[start..end]).map_err(|e| infinity_common::Error::Io {
            path: target.clone(),
            source: e,
        })?;
        restored += 1;
    }
    Ok(restored)
}

/// Publishes every file under `data_dir` as one composed object under
/// `obj_dir`, and rewrites the catalog's `pm|object|` / `pm|object_stat|`
/// records to point at it. Returns the number of paths published.
///
/// A new object per call keeps the write path simple: the reference's
/// `PersistenceManager` packs a batch of small files into one 8 MB object, but
/// a reader only follows the records, so one object per checkpoint reads back
/// the same way.
pub fn pack(store: &dyn MetaStore, data_dir: &Path, obj_dir: &Path) -> Result<usize> {
    let files = data_files(data_dir)?;
    if files.is_empty() {
        return Ok(0);
    }
    std::fs::create_dir_all(obj_dir).map_err(|e| infinity_common::Error::Io {
        path: obj_dir.to_path_buf(),
        source: e,
    })?;

    let mut previous: Vec<String> = Vec::new();
    for (key, value) in store.scan_prefix(PM_OBJECT_PREFIX.as_bytes())? {
        if let Some(addr) = ObjAddr::from_json(&String::from_utf8_lossy(&value)) {
            if addr.obj_key != KEY_EMPTY && !previous.contains(&addr.obj_key) {
                previous.push(addr.obj_key);
            }
        }
        let _ = key;
    }

    let obj_key = new_object_key();
    let mut blob: Vec<u8> = Vec::new();
    let mut parts = 0usize;
    for (rel, path) in &files {
        let bytes = std::fs::read(path).map_err(|e| infinity_common::Error::Io {
            path: path.clone(),
            source: e,
        })?;
        if bytes.is_empty() {
            let addr = ObjAddr {
                obj_key: KEY_EMPTY.to_string(),
                part_offset: 0,
                part_size: 0,
            };
            store.put(
                format!("{PM_OBJECT_PREFIX}{rel}").as_bytes(),
                addr.to_json().as_bytes(),
            )?;
            parts += 1;
            continue;
        }
        while blob.len() % ALIGN != 0 {
            blob.push(0);
        }
        let addr = ObjAddr {
            obj_key: obj_key.clone(),
            part_offset: blob.len() as u64,
            part_size: bytes.len() as u64,
        };
        blob.extend_from_slice(&bytes);
        store.put(
            format!("{PM_OBJECT_PREFIX}{rel}").as_bytes(),
            addr.to_json().as_bytes(),
        )?;
        parts += 1;
    }

    let object = obj_dir.join(&obj_key);
    std::fs::write(&object, &blob).map_err(|e| infinity_common::Error::Io {
        path: object.clone(),
        source: e,
    })?;
    store.put(
        format!("{PM_OBJECT_STAT_PREFIX}{obj_key}").as_bytes(),
        format!(
            "{{\"deleted_ranges\":[],\"obj_size\":{},\"parts\":{}}}",
            blob.len(),
            parts
        )
        .as_bytes(),
    )?;

    // The object this pack replaces is unreachable now; the reference removes
    // it the same way (`PersistResultHandler::HandleWriteResult`).
    for key in previous {
        if key != obj_key {
            let stale = obj_dir.join(&key);
            let _ = std::fs::remove_file(&stale);
            let _ = store.delete(format!("{PM_OBJECT_STAT_PREFIX}{key}").as_bytes());
        }
    }
    Ok(parts)
}

/// A fresh object name. The reference uses a UUID (`PersistenceManager::ObjCreate`);
/// a reader only treats it as an opaque key, so a time-and-counter name is
/// equivalent and needs no dependency.
fn new_object_key() -> String {
    use std::sync::atomic::{AtomicU64, Ordering};
    static COUNTER: AtomicU64 = AtomicU64::new(0);
    let seq = COUNTER.fetch_add(1, Ordering::SeqCst);
    let nanos = std::time::SystemTime::now()
        .duration_since(std::time::UNIX_EPOCH)
        .map(|d| d.as_nanos())
        .unwrap_or(0);
    format!(
        "{:08x}-{:04x}-4{:03x}-8{:03x}-{:012x}",
        (nanos >> 32) as u32 & 0xffff_ffff,
        (nanos >> 16) as u16,
        (nanos as u16) & 0x0fff,
        (seq as u16) & 0x0fff,
        nanos & 0xffff_ffff_ffff
    )
}
