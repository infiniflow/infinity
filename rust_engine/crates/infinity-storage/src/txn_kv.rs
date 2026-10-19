//! The transactional KV substrate behind the C++ `KVStore` / `KVInstance`
//! (`src/storage/catalog/kv_store.cppm`).
//!
//! The reference wraps a RocksDB `TransactionDB`:
//!
//! * `KVStore::Put` / `Delete` / `Merge` are autocommit writes whose per-write
//!   `disableWAL` flag decides whether the write survives a reopen (a clean
//!   `Uninit` does **not** flush the memtable, so only WAL-backed writes and
//!   explicit `Flush` calls are durable);
//! * `KVStore::GetInstance` begins a transaction with `set_snapshot = true`,
//!   so writes buffer until `Commit`, write conflict is reported at `Put` time
//!   (`Resource busy`), and iterators opened on an instance keep that
//!   instance's snapshot view;
//! * `Merge` runs [`String2UInt64AddOperator`]: both operands are parsed as
//!   `u64` and the sum is stored in decimal (`"1"` + `"2"` = `"3"`);
//! * `CreateBackup` / `RestoreFromBackup` copy the whole database.
//!
//! This module reproduces those observable semantics with an in-memory
//! multi-version map plus a snapshot file and an append-only WAL. The storage
//! format is private to the module; only the semantics above are contractual.
//!
//! The ports of `src/unit_test/storage/new_catalog/kv_store_ut.cpp` live in
//! the test module at the bottom of this file.

use std::collections::{BTreeMap, HashMap};
use std::fs::{File, OpenOptions};
use std::io::{Read, Write};
use std::path::{Path, PathBuf};
use std::sync::{Arc, Mutex};

use infinity_common::{Error, Result};

const SNAPSHOT_FILE: &str = "txn_kv.snapshot";
const WAL_FILE: &str = "txn_kv.wal";
const SNAPSHOT_MAGIC: &[u8; 8] = b"IFKVSNAP";
const WAL_MAGIC: &[u8; 8] = b"IFKVWAL\0";

fn io_err(path: &Path, source: std::io::Error) -> Error {
    Error::io(path, source)
}

/// One committed value version. `None` is a tombstone.
type VersionedValue = (u64, Option<Vec<u8>>);

struct Inner {
    /// Latest committed value per key.
    data: BTreeMap<Vec<u8>, Vec<u8>>,
    /// Per-key versions, ascending. The last entry is the latest commit.
    history: HashMap<Vec<u8>, Vec<VersionedValue>>,
    /// Commit version of the latest write.
    version: u64,
    /// Next transaction id.
    next_txn: u64,
    /// Write lock per key: the transaction id that buffered a write.
    locks: HashMap<Vec<u8>, u64>,
}

impl Inner {
    fn new() -> Self {
        Self {
            data: BTreeMap::new(),
            history: HashMap::new(),
            version: 0,
            next_txn: 1,
            locks: HashMap::new(),
        }
    }

    fn latest_version(&self, key: &[u8]) -> u64 {
        self.history
            .get(key)
            .and_then(|versions| versions.last())
            .map(|(version, _)| *version)
            .unwrap_or(0)
    }

    fn value_at(&self, key: &[u8], snapshot: u64) -> Option<&[u8]> {
        let versions = self.history.get(key)?;
        // Versions are appended in commit order, so the last entry at or
        // below the snapshot is the visible one.
        let (_version, value) = versions
            .iter()
            .rev()
            .find(|(version, _)| *version <= snapshot)?;
        value.as_deref()
    }

    fn apply(&mut self, writes: BTreeMap<Vec<u8>, Option<Vec<u8>>>) {
        if writes.is_empty() {
            return;
        }
        self.version += 1;
        let version = self.version;
        for (key, value) in writes {
            match &value {
                Some(bytes) => {
                    self.data.insert(key.clone(), bytes.clone());
                }
                None => {
                    self.data.remove(&key);
                }
            }
            self.history.entry(key).or_default().push((version, value));
        }
    }

    fn release_locks(&mut self, txn: u64) {
        self.locks.retain(|_, owner| *owner != txn);
    }
}

/// A `RocksDB TransactionDB` stand-in with the reference's durability rules.
pub struct KVStore {
    path: PathBuf,
    inner: Arc<Mutex<Inner>>,
}

impl KVStore {
    /// `KVStore::Init`.
    pub fn open(path: impl AsRef<Path>) -> Result<Self> {
        let path = path.as_ref().to_path_buf();
        std::fs::create_dir_all(&path).map_err(|e| io_err(&path, e))?;
        let mut inner = Inner::new();
        load_snapshot(&path, &mut inner)?;
        replay_wal(&path, &mut inner)?;
        Ok(Self {
            path,
            inner: Arc::new(Mutex::new(inner)),
        })
    }

    pub fn path(&self) -> &Path {
        &self.path
    }

    /// `KVStore::Get` — reads the latest committed value.
    pub fn get(&self, key: &str) -> Result<Vec<u8>> {
        let inner = self.inner.lock().unwrap();
        inner
            .data
            .get(key.as_bytes())
            .cloned()
            .ok_or_else(|| Error::NotFound(format!("Key not found: {key}")))
    }

    /// `KVStore::Put` with `disable_wal = true` (the reference's default).
    pub fn put(&self, key: &str, value: &str) -> Result<()> {
        self.put_with_options(key, value, true)
    }

    /// `KVStore::Put(key, value, disable_wal)`.
    ///
    /// With WAL enabled the write is appended and synced to the WAL
    /// immediately, so it survives a reopen; with WAL disabled it only lives
    /// in the memtable and an `Uninit` without `Flush` loses it.
    pub fn put_with_options(&self, key: &str, value: &str, disable_wal: bool) -> Result<()> {
        let mut inner = self.inner.lock().unwrap();
        write_direct(&self.path, &mut inner, key.as_bytes(), Some(value.as_bytes()), disable_wal)
    }

    /// `KVStore::Delete` with `disable_wal = true`.
    pub fn delete(&self, key: &str) -> Result<()> {
        self.delete_with_options(key, true)
    }

    pub fn delete_with_options(&self, key: &str, disable_wal: bool) -> Result<()> {
        let mut inner = self.inner.lock().unwrap();
        write_direct(&self.path, &mut inner, key.as_bytes(), None, disable_wal)
    }

    /// `KVStore::Merge` with `disable_wal = true`.
    pub fn merge(&self, key: &str, delta: &str) -> Result<()> {
        self.merge_with_options(key, delta, true)
    }

    /// `KVStore::Merge`: numeric merge (`String2UInt64AddOperator`).
    pub fn merge_with_options(&self, key: &str, delta: &str, disable_wal: bool) -> Result<()> {
        let mut inner = self.inner.lock().unwrap();
        let base = inner
            .data
            .get(key.as_bytes())
            .map(|value| parse_u64(value))
            .transpose()?
            .unwrap_or(0);
        let operand = parse_u64(delta.as_bytes())?;
        let merged = base
            .checked_add(operand)
            .ok_or_else(|| Error::Storage("String2UInt64AddOperator overflow".into()))?;
        write_direct(
            &self.path,
            &mut inner,
            key.as_bytes(),
            Some(merged.to_string().as_bytes()),
            disable_wal,
        )
    }

    /// `KVStore::GetInstance` — a snapshot-isolated transaction.
    pub fn get_instance(&self) -> KVInstance {
        let mut inner = self.inner.lock().unwrap();
        let id = inner.next_txn;
        inner.next_txn += 1;
        let snapshot = inner.version;
        KVInstance {
            inner: Arc::clone(&self.inner),
            id,
            snapshot,
            writes: BTreeMap::new(),
            closed: false,
        }
    }

    /// `KVStore::Flush` — persists everything written so far (including
    /// WAL-disabled writes) and empties the WAL.
    pub fn flush(&self) -> Result<()> {
        let inner = self.inner.lock().unwrap();
        write_snapshot(&self.path, &inner)?;
        // Truncate the WAL: the snapshot supersedes it.
        let wal = self.path.join(WAL_FILE);
        let file = File::create(&wal).map_err(|e| io_err(&wal, e))?;
        file.sync_all().map_err(|e| io_err(&wal, e))?;
        Ok(())
    }

    /// `KVStore::CreateBackup` — flushes and copies the database files.
    ///
    /// Returns the backed-up file names (the reference's `BackupInfo` list).
    pub fn create_backup(&self, backup_path: impl AsRef<Path>) -> Result<Vec<String>> {
        self.flush()?;
        let backup_path = backup_path.as_ref();
        std::fs::create_dir_all(backup_path).map_err(|e| io_err(backup_path, e))?;
        let mut files = Vec::new();
        for name in [SNAPSHOT_FILE, WAL_FILE] {
            let source = self.path.join(name);
            if !source.exists() {
                continue;
            }
            let target = backup_path.join(name);
            std::fs::copy(&source, &target).map_err(|e| io_err(&target, e))?;
            files.push(name.to_string());
        }
        Ok(files)
    }

    /// `KVStore::RestoreFromBackup`.
    pub fn restore_from_backup(
        backup_path: impl AsRef<Path>,
        db_path: impl AsRef<Path>,
    ) -> Result<()> {
        let backup_path = backup_path.as_ref();
        let db_path = db_path.as_ref();
        if db_path.exists() {
            std::fs::remove_dir_all(db_path).map_err(|e| io_err(db_path, e))?;
        }
        std::fs::create_dir_all(db_path).map_err(|e| io_err(db_path, e))?;
        for name in [SNAPSHOT_FILE, WAL_FILE] {
            let source = backup_path.join(name);
            if !source.exists() {
                continue;
            }
            let target = db_path.join(name);
            std::fs::copy(&source, &target).map_err(|e| io_err(&target, e))?;
        }
        Ok(())
    }

    /// `KVStore::Destroy` — drops the store's directory.
    pub fn destroy(path: impl AsRef<Path>) -> Result<()> {
        let path = path.as_ref();
        match std::fs::remove_dir_all(path) {
            Ok(()) => Ok(()),
            Err(error) if error.kind() == std::io::ErrorKind::NotFound => Ok(()),
            Err(error) => Err(io_err(path, error)),
        }
    }
}

fn parse_u64(value: &[u8]) -> Result<u64> {
    std::str::from_utf8(value)
        .ok()
        .and_then(|text| text.trim().parse::<u64>().ok())
        .ok_or_else(|| Error::Storage("merge operand is not a u64".into()))
}

fn write_direct(
    path: &Path,
    inner: &mut Inner,
    key: &[u8],
    value: Option<&[u8]>,
    disable_wal: bool,
) -> Result<()> {
    let mut writes = BTreeMap::new();
    writes.insert(key.to_vec(), value.map(|bytes| bytes.to_vec()));
    inner.apply(writes);
    if !disable_wal {
        append_wal(path, key, value)?;
    }
    Ok(())
}

fn append_wal(path: &Path, key: &[u8], value: Option<&[u8]>) -> Result<()> {
    let wal = path.join(WAL_FILE);
    let mut file = OpenOptions::new()
        .create(true)
        .append(true)
        .open(&wal)
        .map_err(|e| io_err(&wal, e))?;
    if file.metadata().map(|m| m.len()).unwrap_or(0) == 0 {
        file.write_all(WAL_MAGIC).map_err(|e| io_err(&wal, e))?;
    }
    let mut record = Vec::with_capacity(1 + 4 + key.len() + 4 + value.map_or(0, |v| v.len()));
    record.push(if value.is_some() { 1 } else { 2 });
    record.extend_from_slice(&(key.len() as u32).to_le_bytes());
    record.extend_from_slice(key);
    let value = value.unwrap_or(&[]);
    record.extend_from_slice(&(value.len() as u32).to_le_bytes());
    record.extend_from_slice(value);
    file.write_all(&record).map_err(|e| io_err(&wal, e))?;
    // `FlushWAL(false)` in the reference: the record must be on disk.
    file.sync_data().map_err(|e| io_err(&wal, e))?;
    Ok(())
}

fn write_snapshot(path: &Path, inner: &Inner) -> Result<()> {
    let snapshot = path.join(SNAPSHOT_FILE);
    let mut file = File::create(&snapshot).map_err(|e| io_err(&snapshot, e))?;
    let mut buf = Vec::new();
    buf.extend_from_slice(SNAPSHOT_MAGIC);
    buf.extend_from_slice(&inner.version.to_le_bytes());
    buf.extend_from_slice(&(inner.data.len() as u64).to_le_bytes());
    for (key, value) in &inner.data {
        buf.extend_from_slice(&(key.len() as u32).to_le_bytes());
        buf.extend_from_slice(key);
        buf.extend_from_slice(&(value.len() as u32).to_le_bytes());
        buf.extend_from_slice(value);
    }
    file.write_all(&buf).map_err(|e| io_err(&snapshot, e))?;
    file.sync_all().map_err(|e| io_err(&snapshot, e))?;
    Ok(())
}

fn load_snapshot(path: &Path, inner: &mut Inner) -> Result<()> {
    let snapshot = path.join(SNAPSHOT_FILE);
    if !snapshot.exists() {
        return Ok(());
    }
    let mut buf = Vec::new();
    File::open(&snapshot)
        .map_err(|e| io_err(&snapshot, e))?
        .read_to_end(&mut buf)
        .map_err(|e| io_err(&snapshot, e))?;
    let mut cursor = Cursor::new(&buf);
    let mut magic = [0u8; 8];
    cursor.take_exact(&mut magic, &snapshot)?;
    if &magic != SNAPSHOT_MAGIC {
        return Err(Error::Storage(format!(
            "corrupt KV snapshot at {}",
            snapshot.display()
        )));
    }
    inner.version = cursor.u64(&snapshot)?;
    let count = cursor.u64(&snapshot)? as usize;
    for _ in 0..count {
        let key = cursor.bytes(&snapshot)?;
        let value = cursor.bytes(&snapshot)?;
        inner.history.insert(key.clone(), vec![(inner.version, Some(value.clone()))]);
        inner.data.insert(key, value);
    }
    Ok(())
}

fn replay_wal(path: &Path, inner: &mut Inner) -> Result<()> {
    let wal = path.join(WAL_FILE);
    if !wal.exists() {
        return Ok(());
    }
    let mut buf = Vec::new();
    File::open(&wal)
        .map_err(|e| io_err(&wal, e))?
        .read_to_end(&mut buf)
        .map_err(|e| io_err(&wal, e))?;
    if buf.is_empty() {
        return Ok(());
    }
    let mut cursor = Cursor::new(&buf);
    let mut magic = [0u8; 8];
    cursor.take_exact(&mut magic, &wal)?;
    if &magic != WAL_MAGIC {
        return Err(Error::Storage(format!(
            "corrupt KV WAL at {}",
            wal.display()
        )));
    }
    while cursor.pos < buf.len() {
        let op = cursor.u8(&wal)?;
        let key = cursor.bytes(&wal)?;
        let value = cursor.bytes(&wal)?;
        let value = match op {
            1 => Some(value),
            2 => None,
            other => {
                return Err(Error::Storage(format!(
                    "corrupt KV WAL record op {other} at {}",
                    wal.display()
                )))
            }
        };
        let mut writes = BTreeMap::new();
        writes.insert(key, value);
        inner.apply(writes);
    }
    Ok(())
}

struct Cursor<'a> {
    buf: &'a [u8],
    pos: usize,
}

impl<'a> Cursor<'a> {
    fn new(buf: &'a [u8]) -> Self {
        Self { buf, pos: 0 }
    }

    fn take_exact(&mut self, out: &mut [u8], path: &Path) -> Result<()> {
        if self.pos + out.len() > self.buf.len() {
            return Err(Error::Storage(format!(
                "truncated KV file at {}",
                path.display()
            )));
        }
        out.copy_from_slice(&self.buf[self.pos..self.pos + out.len()]);
        self.pos += out.len();
        Ok(())
    }

    fn u8(&mut self, path: &Path) -> Result<u8> {
        let mut out = [0u8; 1];
        self.take_exact(&mut out, path)?;
        Ok(out[0])
    }

    fn u32(&mut self, path: &Path) -> Result<u32> {
        let mut out = [0u8; 4];
        self.take_exact(&mut out, path)?;
        Ok(u32::from_le_bytes(out))
    }

    fn u64(&mut self, path: &Path) -> Result<u64> {
        let mut out = [0u8; 8];
        self.take_exact(&mut out, path)?;
        Ok(u64::from_le_bytes(out))
    }

    fn bytes(&mut self, path: &Path) -> Result<Vec<u8>> {
        let len = self.u32(path)? as usize;
        let mut out = vec![0u8; len];
        self.take_exact(&mut out, path)?;
        Ok(out)
    }
}

/// A snapshot-isolated transaction (`KVInstance`).
///
/// Writes buffer locally until [`KVInstance::commit`]. A write conflicts when
/// another live instance already buffered the same key, or when the key
/// changed after this instance's snapshot; both report `Resource busy`, like
/// RocksDB's pessimistic `TransactionDB`.
pub struct KVInstance {
    inner: Arc<Mutex<Inner>>,
    id: u64,
    snapshot: u64,
    writes: BTreeMap<Vec<u8>, Option<Vec<u8>>>,
    closed: bool,
}

impl KVInstance {
    fn ensure_open(&self) -> Result<()> {
        if self.closed {
            return Err(Error::Txn("transaction is already finished".into()));
        }
        Ok(())
    }

    fn check_write(&self, inner: &Inner, key: &[u8]) -> Result<()> {
        if let Some(owner) = inner.locks.get(key) {
            if *owner != self.id {
                return Err(Error::Storage(format!(
                    "Resource busy: key '{}' is locked by another transaction",
                    String::from_utf8_lossy(key)
                )));
            }
        }
        if inner.latest_version(key) > self.snapshot {
            return Err(Error::Storage(format!(
                "Resource busy: key '{}' changed after this transaction's snapshot",
                String::from_utf8_lossy(key)
            )));
        }
        Ok(())
    }

    /// `KVInstance::Put`.
    pub fn put(&mut self, key: &str, value: &str) -> Result<()> {
        self.ensure_open()?;
        let mut inner = self.inner.lock().unwrap();
        self.check_write(&inner, key.as_bytes())?;
        inner.locks.insert(key.as_bytes().to_vec(), self.id);
        self.writes
            .insert(key.as_bytes().to_vec(), Some(value.as_bytes().to_vec()));
        Ok(())
    }

    /// `KVInstance::Delete`.
    pub fn delete(&mut self, key: &str) -> Result<()> {
        self.ensure_open()?;
        let mut inner = self.inner.lock().unwrap();
        self.check_write(&inner, key.as_bytes())?;
        inner.locks.insert(key.as_bytes().to_vec(), self.id);
        self.writes.insert(key.as_bytes().to_vec(), None);
        Ok(())
    }

    /// `KVInstance::Get` — own writes first, then the snapshot view.
    pub fn get(&self, key: &str) -> Result<Vec<u8>> {
        self.ensure_open()?;
        let inner = self.inner.lock().unwrap();
        if let Some(value) = self.writes.get(key.as_bytes()) {
            return value
                .clone()
                .ok_or_else(|| Error::NotFound(format!("Key not found: {key}")));
        }
        inner
            .value_at(key.as_bytes(), self.snapshot)
            .map(|value| value.to_vec())
            .ok_or_else(|| Error::NotFound(format!("Key not found: {key}")))
    }

    /// `KVInstance::GetIterator` — the snapshot view plus this instance's own
    /// buffered writes at the time the iterator is opened.
    pub fn iterator(&self) -> Result<KVIterator> {
        self.iterator_with_bounds(None, None)
    }

    /// An iterator restricted to `lower <= key < upper` (RocksDB's
    /// `iterate_lower_bound` / `iterate_upper_bound`).
    pub fn iterator_with_bounds(
        &self,
        lower: Option<&str>,
        upper: Option<&str>,
    ) -> Result<KVIterator> {
        self.ensure_open()?;
        let inner = self.inner.lock().unwrap();
        let mut view: BTreeMap<Vec<u8>, Vec<u8>> = BTreeMap::new();
        for key in inner.history.keys() {
            if let Some(value) = inner.value_at(key, self.snapshot) {
                view.insert(key.clone(), value.to_vec());
            }
        }
        for (key, value) in &self.writes {
            match value {
                Some(bytes) => {
                    view.insert(key.clone(), bytes.clone());
                }
                None => {
                    view.remove(key);
                }
            }
        }
        let lower = lower.map(str::as_bytes);
        let upper = upper.map(str::as_bytes);
        let entries = view
            .into_iter()
            .filter(|(key, _)| {
                lower.is_none_or(|bound| key.as_slice() >= bound)
                    && upper.is_none_or(|bound| key.as_slice() < bound)
            })
            .collect();
        Ok(KVIterator { entries, pos: 0 })
    }

    /// `KVInstance::Commit`.
    pub fn commit(&mut self) -> Result<()> {
        self.ensure_open()?;
        let mut inner = self.inner.lock().unwrap();
        inner.release_locks(self.id);
        inner.apply(std::mem::take(&mut self.writes));
        self.closed = true;
        Ok(())
    }

    /// `KVInstance::Rollback`.
    pub fn rollback(&mut self) -> Result<()> {
        self.ensure_open()?;
        let mut inner = self.inner.lock().unwrap();
        inner.release_locks(self.id);
        self.writes.clear();
        self.closed = true;
        Ok(())
    }
}

impl Drop for KVInstance {
    fn drop(&mut self) {
        if self.closed {
            return;
        }
        if let Ok(mut inner) = self.inner.lock() {
            inner.release_locks(self.id);
        }
    }
}

/// `KVIterator` over a materialised snapshot view, in key order.
pub struct KVIterator {
    entries: Vec<(Vec<u8>, Vec<u8>)>,
    pos: usize,
}

impl KVIterator {
    /// `Seek`: an empty key seeks to the first entry.
    pub fn seek(&mut self, key: &str) {
        if key.is_empty() {
            self.pos = 0;
            return;
        }
        let key = key.as_bytes();
        self.pos = self
            .entries
            .partition_point(|(candidate, _)| candidate.as_slice() < key);
    }

    pub fn valid(&self) -> bool {
        self.pos < self.entries.len()
    }

    pub fn next(&mut self) {
        self.pos += 1;
    }

    pub fn key(&self) -> &[u8] {
        &self.entries[self.pos].0
    }

    pub fn value(&self) -> &[u8] {
        &self.entries[self.pos].1
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::tempdir;

    /// C++: `kv_store_ut.cpp:29` (`kv_store0`).
    ///
    /// Get-miss, Put/Get, numeric Merge (`"1"` + `"2"` = `"3"`), Delete and
    /// Destroy.
    #[test]
    fn kv_store0() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("rocksdb_transaction_example");
        let store = KVStore::open(&path).unwrap();

        assert!(store.get("key1").is_err());

        store.put("key1", "1").unwrap();
        assert_eq!(store.get("key1").unwrap(), b"1");

        store.merge("key1", "2").unwrap();
        assert_eq!(store.get("key1").unwrap(), b"3");

        store.delete("key1").unwrap();
        assert!(store.get("key1").is_err());

        drop(store);
        KVStore::destroy(&path).unwrap();
        assert!(!path.exists());
    }

    /// C++: `kv_store_ut.cpp:65` (`DISABLED_SLOW_kv_store1`).
    ///
    /// Multi-version transactions: uncommitted writes are invisible, a second
    /// writer of a key locked by another live transaction gets `Resource
    /// busy`, a write to a key changed after the snapshot conflicts, rejected
    /// writes are skipped at commit, and rollback discards.
    #[test]
    #[ignore = "SLOW: the reference disables this test"]
    fn kv_store1_multi_version_transactions() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("rocksdb_transaction_example");
        let store = KVStore::open(&path).unwrap();
        let mut instance1 = store.get_instance();

        assert!(instance1.get("abc").is_err());
        instance1.put("abc", "def").unwrap();

        let mut instance2 = store.get_instance();
        assert!(instance2.get("abc").is_err());
        instance2.put("xyz", "zzz").unwrap();
        assert!(instance2.put("abc", "def").is_err());

        assert_eq!(instance2.get("xyz").unwrap(), b"zzz");

        let mut instance4 = store.get_instance();

        instance1.commit().unwrap();
        instance2.commit().unwrap();

        let error = instance4.put("abc", "xxx").unwrap_err();
        assert!(error.to_string().contains("Resource busy"), "{error}");
        instance4.rollback().unwrap();

        let mut instance3 = store.get_instance();
        assert_eq!(instance3.get("xyz").unwrap(), b"zzz");
        assert_eq!(instance3.get("abc").unwrap(), b"def");
        instance3.commit().unwrap();

        drop((instance1, instance2, instance3, instance4));
        drop(store);
        KVStore::destroy(&path).unwrap();
    }

    /// C++: `kv_store_ut.cpp:141` (`kv_store2`).
    ///
    /// A transaction commit does not write the WAL. Without `Flush` it is lost
    /// on reopen; with `Flush` it survives.
    #[test]
    fn kv_store2_disable_wal() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("rocksdb_transaction_example");

        {
            let store = KVStore::open(&path).unwrap();
            let mut instance = store.get_instance();
            assert!(instance.get("abc").is_err());
            instance.put("abc", "def").unwrap();
            instance.commit().unwrap();
        }
        {
            let store = KVStore::open(&path).unwrap();
            assert!(store.get("abc").is_err());
        }
        KVStore::destroy(&path).unwrap();

        {
            let store = KVStore::open(&path).unwrap();
            let mut instance = store.get_instance();
            assert!(instance.get("abc").is_err());
            instance.put("abc", "def").unwrap();
            instance.commit().unwrap();
            store.flush().unwrap();
        }
        {
            let store = KVStore::open(&path).unwrap();
            let mut instance = store.get_instance();
            assert_eq!(instance.get("abc").unwrap(), b"def");
            instance.commit().unwrap();
        }
        drop(KVStore::destroy(&path));
    }

    /// C++: `kv_store_ut.cpp:209` (`kv_store3`).
    ///
    /// Snapshot-isolated iterator: an iterator opened on an instance that was
    /// created before another instance committed keeps the old view.
    #[test]
    fn kv_store3_snapshot_isolated_iterator() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("rocksdb_transaction_example");
        let store = KVStore::open(&path).unwrap();

        {
            let mut instance1 = store.get_instance();
            assert!(instance1.get("abc").is_err());
            instance1.put("db|db1|10", "10").unwrap();
            instance1.commit().unwrap();

            let mut instance2 = store.get_instance();
            let mut instance1 = store.get_instance();

            instance1.delete("db|db1|10").unwrap();
            instance1.put("db|db1|20", "20").unwrap();
            instance1.commit().unwrap();

            let prefix = "db|db1|";
            let mut iter = instance2.iterator().unwrap();
            iter.seek(prefix);
            let mut seen = Vec::new();
            while iter.valid() && iter.key().starts_with(prefix.as_bytes()) {
                seen.push((
                    String::from_utf8(iter.key().to_vec()).unwrap(),
                    String::from_utf8(iter.value().to_vec()).unwrap(),
                ));
                iter.next();
            }
            assert_eq!(seen, vec![("db|db1|10".to_string(), "10".to_string())]);
            instance2.commit().unwrap();
        }

        drop(store);
        KVStore::destroy(&path).unwrap();
    }

    /// C++: `kv_store_ut.cpp:263` (`kv_store4`).
    ///
    /// `CreateBackup` / `Destroy` / `RestoreFromBackup` round-trips both the
    /// prefix-iterated pairs and a point read.
    #[test]
    fn kv_store4_backup_restore() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("rocksdb_transaction_example");
        let backup_path = dir.path().join("rocksdb_example_backup");

        {
            let store = KVStore::open(&path).unwrap();
            let mut instance = store.get_instance();
            instance.put("db|db1|10", "10").unwrap();
            instance.put("abc", "def").unwrap();
            instance.commit().unwrap();
            store.flush().unwrap();

            let files = store.create_backup(&backup_path).unwrap();
            assert!(!files.is_empty(), "backup contains no files");
        }
        KVStore::destroy(&path).unwrap();

        KVStore::restore_from_backup(&backup_path, &path).unwrap();
        let store = KVStore::open(&path).unwrap();
        {
            let mut instance = store.get_instance();
            let prefix = "db|db1|";
            let mut iter = instance.iterator().unwrap();
            iter.seek(prefix);
            let mut seen = Vec::new();
            while iter.valid() && iter.key().starts_with(prefix.as_bytes()) {
                seen.push((
                    String::from_utf8(iter.key().to_vec()).unwrap(),
                    String::from_utf8(iter.value().to_vec()).unwrap(),
                ));
                iter.next();
            }
            assert_eq!(seen, vec![("db|db1|10".to_string(), "10".to_string())]);
            assert_eq!(instance.get("abc").unwrap(), b"def");
            instance.commit().unwrap();
        }
        drop(store);
        KVStore::destroy(&path).unwrap();
    }

    /// C++: `kv_store_ut.cpp:329` (`kv_store5`).
    ///
    /// Raw transaction snapshot plus iterator lower/upper bounds: seeking
    /// `key` under `[key1, key4)` visits `key1`..`key3` only.
    #[test]
    fn kv_store5_transaction_snapshot_iterator_bounds() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("rocksdb_two_phases_transaction");

        {
            let store = KVStore::open(&path).unwrap();
            let mut txn1 = store.get_instance();
            for i in 1..=5 {
                txn1.put(&format!("key{i}"), &format!("value{i}")).unwrap();
            }
            txn1.commit().unwrap();

            let mut txn2 = store.get_instance();
            let mut iter = txn2.iterator_with_bounds(Some("key1"), Some("key4")).unwrap();
            iter.seek("key");
            let mut seen = Vec::new();
            while iter.valid() && iter.key().starts_with(b"key") {
                seen.push(String::from_utf8(iter.key().to_vec()).unwrap());
                iter.next();
            }
            assert_eq!(
                seen,
                vec!["key1".to_string(), "key2".to_string(), "key3".to_string()]
            );
            txn2.commit().unwrap();
        }

        KVStore::destroy(&path).unwrap();
    }

    /// C++: `kv_store_ut.cpp:504` (`wal`).
    ///
    /// A WAL-disabled direct put is lost on reopen without a flush, while a
    /// WAL-enabled put survives because it is flushed to the WAL.
    #[test]
    fn wal_disabled_put_is_lost_without_flush() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("rocksdb_wal");

        {
            let store = KVStore::open(&path).unwrap();
            store.put("foo1", "bar1").unwrap();
            store.put_with_options("foo2", "bar2", false).unwrap();
        }

        let store = KVStore::open(&path).unwrap();
        assert!(store.get("foo1").is_err());
        assert_eq!(store.get("foo2").unwrap(), b"bar2");
        drop(store);
        KVStore::destroy(&path).unwrap();
    }

    /// C++: `kv_store_ut.cpp:538` (`txn_conflict_same_key_snapshot_isolation`).
    ///
    /// Two transactions share snapshot T0; B commits first. A keeps reading
    /// the old snapshot, its write of the same key is rejected, and A's commit
    /// therefore leaves B's value in place.
    #[test]
    fn txn_conflict_same_key_snapshot_isolation() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("rocksdb_snapshot_isolation");
        let store = KVStore::open(&path).unwrap();

        let key = "seg|2|0|0|first_delete_ts";
        let mut txn_a = store.get_instance();
        let mut txn_b = store.get_instance();

        assert!(txn_a.get(key).is_err());

        txn_b.put(key, "14").unwrap();
        txn_b.commit().unwrap();

        assert!(txn_a.get(key).is_err());

        let error = txn_a.put(key, "16").unwrap_err();
        assert!(error.to_string().contains("Resource busy"), "{error}");
        txn_a.commit().unwrap();

        {
            let mut verify = store.get_instance();
            assert_eq!(verify.get(key).unwrap(), b"14");
            verify.commit().unwrap();
        }

        drop(store);
        KVStore::destroy(&path).unwrap();
    }
}
