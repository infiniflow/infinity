//! The `MetaStore` trait and its pure-Rust `fjall` implementation (plan §3 D2).
//!
//! Backs catalog/meta, ID counters, object manifests and tombstones — the subset
//! of RocksDB usage that Infinity relies on. `rust-rocksdb` can later provide an
//! alternative impl behind the same trait.

use std::path::Path;
use std::sync::Mutex;

use fjall::{Database, Keyspace, KeyspaceCreateOptions, PersistMode};
use infinity_common::{Error, Result};

fn fe(e: fjall::Error) -> Error {
    Error::Storage(e.to_string())
}

/// Stored values larger than this are LZ4-compressed before they reach the
/// LSM tree. Table rows are tab-separated text with thousands of floats, which
/// compresses 3-4x; the codec is transparent to every caller.
const COMPRESS_MIN: usize = 512;
/// A compressed value starts with this pair; all legacy values are UTF-8 text,
/// so they never collide with it.
const LZ4_MAGIC: [u8; 2] = [0xC1, 0x4C];

fn encode_value(value: &[u8]) -> Vec<u8> {
    if value.len() < COMPRESS_MIN {
        return value.to_vec();
    }
    let packed = lz4_flex::compress_prepend_size(value);
    if packed.len() + LZ4_MAGIC.len() >= value.len() {
        return value.to_vec();
    }
    let mut out = Vec::with_capacity(LZ4_MAGIC.len() + packed.len());
    out.extend_from_slice(&LZ4_MAGIC);
    out.extend_from_slice(&packed);
    out
}

fn decode_value(value: &[u8]) -> Vec<u8> {
    if value.len() > LZ4_MAGIC.len() && value[0] == LZ4_MAGIC[0] && value[1] == LZ4_MAGIC[1] {
        if let Ok(raw) = lz4_flex::decompress_size_prepended(&value[LZ4_MAGIC.len()..]) {
            return raw;
        }
    }
    value.to_vec()
}

/// A transactional key-value store for metadata.
///
/// The write surface mirrors the reference's `KVInstance`
/// (`src/storage/catalog/kv_store.cppm:42-98`): a single commit may carry
/// both puts and deletes ([`MetaStore::atomic_batch`]), and `Merge` runs the
/// numeric `String2UInt64AddOperator` ([`MetaStore::merge`]).
pub trait MetaStore: Send + Sync {
    fn get(&self, key: &[u8]) -> Result<Option<Vec<u8>>>;
    fn put(&self, key: &[u8], value: &[u8]) -> Result<()>;
    fn delete(&self, key: &[u8]) -> Result<()>;
    /// C++: `KVInstance::Put` / `KVInstance::Delete` + `Commit` — one
    /// all-or-nothing commit of puts (`Some(value)`) and deletes (`None`).
    ///
    /// This is the single primitive a DDL uses: a catalog mutation that
    /// removes N old entries and adds M new ones commits once, so a crash or
    /// an error can never leave half a DDL behind.
    ///
    /// The default applies the operations one by one; an implementation that
    /// can batch (the LSM and RocksDB backends) must override it.
    fn atomic_batch(&self, items: &[(Vec<u8>, Option<Vec<u8>>)]) -> Result<()> {
        for (key, value) in items {
            match value {
                Some(value) => self.put(key, value)?,
                None => self.delete(key)?,
            }
        }
        Ok(())
    }
    /// Atomically apply all writes (single all-or-nothing batch).
    fn atomic_write(&self, items: &[(Vec<u8>, Vec<u8>)]) -> Result<()> {
        let batch: Vec<(Vec<u8>, Option<Vec<u8>>)> = items
            .iter()
            .map(|(key, value)| (key.clone(), Some(value.clone())))
            .collect();
        self.atomic_batch(&batch)
    }
    /// C++: `KVStore::Merge` with `String2UInt64AddOperator` — the stored
    /// value and `delta` are read as `u64` and the sum is stored in decimal
    /// (`"1"` merged with `2` is `"3"`). A missing key merges against `0`.
    ///
    /// Returns the merged value. The read-modify-write is atomic within the
    /// store; like the reference, it is not coordinated with another process
    /// opening the same catalog directory.
    fn merge(&self, key: &[u8], delta: u64) -> Result<u64> {
        let merged = merge_delta(self.get(key)?.as_deref(), delta)?;
        self.put(key, merged.to_string().as_bytes())?;
        Ok(merged)
    }
    /// All `(key, value)` pairs whose key starts with `prefix`, in key order.
    fn scan_prefix(&self, prefix: &[u8]) -> Result<Vec<(Vec<u8>, Vec<u8>)>>;
    /// All `(key, value)` pairs with `start <= key < end`, in key order.
    ///
    /// The block mirror reads one block's rows at a time instead of loading a
    /// whole table: the reference streams its rows through the buffer manager
    /// (`NewTxn::Import` -> `AppendInBlock`), so a table of any size is
    /// mirrored with O(block) memory. A backend that cannot bound the scan
    /// falls back to the prefix walk below.
    fn scan_range(&self, start: &[u8], end: &[u8]) -> Result<Vec<(Vec<u8>, Vec<u8>)>> {
        Ok(self
            .scan_prefix(start)?
            .into_iter()
            .take_while(|(key, _)| key.as_slice() < end)
            .collect())
    }
    /// Number of keys with `prefix`. Counting (`SELECT count(*)`, `SHOW TABLE`)
    /// must not read the row values, which can be tens of kilobytes each.
    fn count_prefix(&self, prefix: &[u8]) -> Result<usize> {
        Ok(self.scan_prefix(prefix)?.len())
    }
    /// Flush to disk (durability barrier).
    fn flush(&self) -> Result<()>;
}

/// C++: `String2UInt64AddOperator` (`catalog/kv_store.cpp:91`) — both merge
/// operands are parsed as `u64`.
fn parse_u64(value: &[u8]) -> Result<u64> {
    std::str::from_utf8(value)
        .ok()
        .and_then(|text| text.trim().parse::<u64>().ok())
        .ok_or_else(|| Error::Storage("merge operand is not a u64".into()))
}

/// The merge operator itself: `current` (or `0`) plus `delta`.
pub(crate) fn merge_delta(current: Option<&[u8]>, delta: u64) -> Result<u64> {
    let base = match current {
        Some(value) => parse_u64(value)?,
        None => 0,
    };
    base.checked_add(delta)
        .ok_or_else(|| Error::Storage("String2UInt64AddOperator overflow".into()))
}

/// The substrate a store directory holds.
///
/// The two substrates are told apart by what they leave on disk: fjall writes
/// a `keyspaces/` directory and `<id>.jnl` journals, RocksDB writes `CURRENT`
/// and a `MANIFEST-*`. The engine looks at the directory it was pointed at and
/// picks the matching substrate, so one binary can open a directory either
/// engine wrote.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum StoreBackend {
    Fjall,
    RocksDb,
}

impl StoreBackend {
    /// Detects the substrate of an existing store directory.
    ///
    /// `None` when the directory does not exist or carries no marker (an empty
    /// directory, or one only holding leftovers).
    ///
    /// RocksDB is looked for first: it is the substrate the reference uses, so
    /// a directory that holds one is opened as a RocksDB, and fjall is the
    /// fallback for a directory that does not.
    pub fn detect(path: &Path) -> Option<Self> {
        if !path.is_dir() {
            return None;
        }
        let has_manifest = std::fs::read_dir(path)
            .ok()?
            .flatten()
            .any(|e| e.file_name().to_string_lossy().starts_with("MANIFEST-"));
        if has_manifest || path.join("CURRENT").is_file() {
            return Some(StoreBackend::RocksDb);
        }
        if path.join("keyspaces").is_dir() {
            return Some(StoreBackend::Fjall);
        }
        None
    }

    pub fn name(self) -> &'static str {
        match self {
            StoreBackend::Fjall => "fjall",
            StoreBackend::RocksDb => "rocksdb",
        }
    }

    /// Parses the name of a substrate (`fjall`, `rocksdb`).
    pub fn parse(text: &str) -> Option<Self> {
        match text.trim().to_ascii_lowercase().as_str() {
            "fjall" => Some(StoreBackend::Fjall),
            "rocksdb" | "rocks" => Some(StoreBackend::RocksDb),
            _ => None,
        }
    }
}

/// `fjall`-backed `MetaStore`.
pub struct FjallMetaStore {
    db: Database,
    ks: Keyspace,
    /// Serialises the read-modify-write of [`MetaStore::merge`], so two
    /// concurrent merges of one counter cannot lose an increment.
    merge_lock: Mutex<()>,
}

impl FjallMetaStore {
    /// Opens (creating if needed) a metastore under `path` in keyspace `name`.
    pub fn open(path: impl AsRef<Path>, name: &str) -> Result<Self> {
        let db = Database::builder(path.as_ref()).open().map_err(fe)?;
        let ks = db
            .keyspace(name, KeyspaceCreateOptions::default)
            .map_err(fe)?;
        Ok(Self {
            db,
            ks,
            merge_lock: Mutex::new(()),
        })
    }
}

impl MetaStore for FjallMetaStore {
    fn get(&self, key: &[u8]) -> Result<Option<Vec<u8>>> {
        Ok(self.ks.get(key).map_err(fe)?.map(|v| decode_value(&v)))
    }

    fn put(&self, key: &[u8], value: &[u8]) -> Result<()> {
        self.ks.insert(key, encode_value(value)).map_err(fe)
    }

    fn delete(&self, key: &[u8]) -> Result<()> {
        self.ks.remove(key).map_err(fe)
    }

    fn atomic_batch(&self, items: &[(Vec<u8>, Option<Vec<u8>>)]) -> Result<()> {
        if items.is_empty() {
            return Ok(());
        }
        let mut batch = self.db.batch();
        for (key, value) in items {
            match value {
                Some(value) => {
                    let stored = encode_value(value);
                    batch.insert(&self.ks, key.as_slice(), stored.as_slice());
                }
                None => batch.remove(&self.ks, key.as_slice()),
            }
        }
        batch.commit().map_err(fe)
    }

    fn merge(&self, key: &[u8], delta: u64) -> Result<u64> {
        let _guard = self.merge_lock.lock().unwrap();
        let merged = merge_delta(self.get(key)?.as_deref(), delta)?;
        self.put(key, merged.to_string().as_bytes())?;
        Ok(merged)
    }

    fn count_prefix(&self, prefix: &[u8]) -> Result<usize> {
        let mut n = 0usize;
        for kv in self.ks.prefix(prefix) {
            kv.into_inner().map_err(fe)?;
            n += 1;
        }
        Ok(n)
    }

    fn scan_prefix(&self, prefix: &[u8]) -> Result<Vec<(Vec<u8>, Vec<u8>)>> {
        let mut out = Vec::new();
        for kv in self.ks.prefix(prefix) {
            let (k, v) = kv.into_inner().map_err(fe)?;
            out.push((k.to_vec(), decode_value(&v)));
        }
        Ok(out)
    }

    fn scan_range(&self, start: &[u8], end: &[u8]) -> Result<Vec<(Vec<u8>, Vec<u8>)>> {
        let mut out = Vec::new();
        for kv in self.ks.range(start.to_vec()..end.to_vec()) {
            let (k, v) = kv.into_inner().map_err(fe)?;
            out.push((k.to_vec(), decode_value(&v)));
        }
        Ok(out)
    }

    fn flush(&self) -> Result<()> {
        self.db.persist(PersistMode::SyncAll).map_err(fe)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::tempdir;

    fn open() -> (tempfile::TempDir, FjallMetaStore) {
        let dir = tempdir().unwrap();
        let s = FjallMetaStore::open(dir.path(), "catalog").unwrap();
        (dir, s)
    }

    #[test]
    fn the_substrate_of_a_directory_is_detected() {
        let dir = tempdir().unwrap();
        // Nothing there yet: no substrate to detect.
        assert_eq!(StoreBackend::detect(&dir.path().join("catalog")), None);

        // A fjall store: a `keyspaces` directory.
        let fjall = dir.path().join("fjall");
        FjallMetaStore::open(&fjall, "catalog").unwrap();
        assert_eq!(StoreBackend::detect(&fjall), Some(StoreBackend::Fjall));

        // A RocksDB store: `CURRENT` and a `MANIFEST-*`.
        let rocks = dir.path().join("rocks");
        std::fs::create_dir_all(&rocks).unwrap();
        std::fs::write(rocks.join("CURRENT"), b"MANIFEST-000001\n").unwrap();
        std::fs::write(rocks.join("MANIFEST-000001"), b"").unwrap();
        assert_eq!(StoreBackend::detect(&rocks), Some(StoreBackend::RocksDb));

        // RocksDB is looked for first: a directory that carries both markers
        // is read as a RocksDB, because that is what the reference writes.
        let both = dir.path().join("both");
        std::fs::create_dir_all(both.join("keyspaces")).unwrap();
        std::fs::write(both.join("CURRENT"), b"MANIFEST-000001\n").unwrap();
        assert_eq!(StoreBackend::detect(&both), Some(StoreBackend::RocksDb));
    }

    #[test]
    fn compressed_values_roundtrip() {
        let (_d, s) = open();
        // A row-shaped value: thousands of repeated float fields.
        let mut text = String::new();
        for i in 0..4000 {
            text.push_str(&format!("{:.6},", (i % 97) as f64 * 0.25));
        }
        s.put(b"row", text.as_bytes()).unwrap();
        assert_eq!(s.get(b"row").unwrap().unwrap(), text.as_bytes());
        s.atomic_write(&[(b"row2".to_vec(), text.as_bytes().to_vec())])
            .unwrap();
        assert_eq!(s.get(b"row2").unwrap().unwrap(), text.as_bytes());
        let scan = s.scan_prefix(b"row").unwrap();
        assert_eq!(scan.len(), 2);
        assert_eq!(scan[0].1, text.as_bytes());
    }

    #[test]
    fn put_get_delete() {
        let (_d, s) = open();
        assert_eq!(s.get(b"a").unwrap(), None);
        s.put(b"a", b"1").unwrap();
        assert_eq!(s.get(b"a").unwrap(), Some(b"1".to_vec()));
        s.delete(b"a").unwrap();
        assert_eq!(s.get(b"a").unwrap(), None);
    }

    #[test]
    fn atomic_write_is_all_or_nothing() {
        let (_d, s) = open();
        s.atomic_write(&[
            (b"k1".to_vec(), b"v1".to_vec()),
            (b"k2".to_vec(), b"v2".to_vec()),
        ])
        .unwrap();
        assert_eq!(s.get(b"k1").unwrap(), Some(b"v1".to_vec()));
        assert_eq!(s.get(b"k2").unwrap(), Some(b"v2".to_vec()));
    }

    /// C++: `KVInstance::Put` + `KVInstance::Delete` + `Commit` — one DDL
    /// commit carries the new entries *and* the removals.
    #[test]
    fn atomic_batch_carries_puts_and_deletes_in_one_commit() {
        let (_d, s) = open();
        s.put(b"tbl:1:old_a", b"x").unwrap();
        s.put(b"tbl:1:old_b", b"y").unwrap();
        s.put(b"tbl:1:keep", b"z").unwrap();

        s.atomic_batch(&[
            (b"tbl:1:old_a".to_vec(), None),
            (b"tbl:1:old_b".to_vec(), None),
            (b"tbl:1:new".to_vec(), Some(b"n".to_vec())),
            (b"tbl:1:keep".to_vec(), Some(b"z2".to_vec())),
        ])
        .unwrap();

        assert_eq!(s.get(b"tbl:1:old_a").unwrap(), None);
        assert_eq!(s.get(b"tbl:1:old_b").unwrap(), None);
        assert_eq!(s.get(b"tbl:1:new").unwrap(), Some(b"n".to_vec()));
        assert_eq!(s.get(b"tbl:1:keep").unwrap(), Some(b"z2".to_vec()));
        assert_eq!(s.count_prefix(b"tbl:1:").unwrap(), 2);
    }

    /// Deleting a key that does not exist is a no-op, not an error.
    #[test]
    fn atomic_batch_tolerates_missing_deletes() {
        let (_d, s) = open();
        s.atomic_batch(&[
            (b"absent".to_vec(), None),
            (b"k".to_vec(), Some(b"v".to_vec())),
        ])
        .unwrap();
        assert_eq!(s.get(b"k").unwrap(), Some(b"v".to_vec()));
    }

    #[test]
    fn atomic_batch_empty_is_a_noop() {
        let (_d, s) = open();
        s.atomic_batch(&[]).unwrap();
        assert!(s.scan_prefix(b"").unwrap().is_empty());
    }

    /// C++ `kv_store_ut.cpp:29`: `Put("1")` then `Merge("2")` reads back `"3"`
    /// (the numeric `String2UInt64AddOperator`).
    #[test]
    fn merge_accumulates_u64_values() {
        let (_d, s) = open();
        s.put(b"counter", b"1").unwrap();
        assert_eq!(s.merge(b"counter", 2).unwrap(), 3);
        assert_eq!(s.get(b"counter").unwrap(), Some(b"3".to_vec()));
        assert_eq!(s.merge(b"counter", 7).unwrap(), 10);
        assert_eq!(s.get(b"counter").unwrap(), Some(b"10".to_vec()));
        // A missing key merges against zero.
        assert_eq!(s.merge(b"fresh", 5).unwrap(), 5);
        assert_eq!(s.get(b"fresh").unwrap(), Some(b"5".to_vec()));
        // The operand must be numeric, exactly as the reference's operator.
        s.put(b"text", b"abc").unwrap();
        assert!(s.merge(b"text", 1).is_err());
    }

    #[test]
    fn scan_prefix_is_ordered() {
        let (_d, s) = open();
        s.put(b"tbl:2", b"b").unwrap();
        s.put(b"tbl:1", b"a").unwrap();
        s.put(b"other:1", b"x").unwrap();
        let got = s.scan_prefix(b"tbl:").unwrap();
        assert_eq!(
            got,
            vec![
                (b"tbl:1".to_vec(), b"a".to_vec()),
                (b"tbl:2".to_vec(), b"b".to_vec())
            ]
        );
    }

    #[test]
    fn survives_reopen_after_flush() {
        let dir = tempdir().unwrap();
        {
            let s = FjallMetaStore::open(dir.path(), "catalog").unwrap();
            s.put(b"persist", b"yes").unwrap();
            s.flush().unwrap();
        }
        let s = FjallMetaStore::open(dir.path(), "catalog").unwrap();
        assert_eq!(s.get(b"persist").unwrap(), Some(b"yes".to_vec()));
    }
}
