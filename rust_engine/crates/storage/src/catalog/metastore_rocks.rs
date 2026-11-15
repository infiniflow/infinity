//! RocksDB-backed `MetaStore` for a **C++-compatible catalog** (plan §2.6.1
//! item 4: "Catalog KV 需切到 RocksDB").
//!
//! The C++ engine stores its catalog in RocksDB under the key space frozen in
//! `docs/CPP_FORMAT_SPEC.md` §8.2; `infinity-storage::catalog_format` renders
//! those keys. This module provides the substrate, so pointing the engine at a
//! directory the C++ engine wrote (or handing it one) uses the same bytes.
//!
//! This is the only substrate: the catalog and the row store both live in a
//! RocksDB, the same one the reference writes.

use std::path::Path;
use std::sync::Mutex;

use ::common::{Error, Result};
use rocksdb::{Options, WriteBatch, WriteOptions, DB};

use crate::metastore::MetaStore;
use crate::persistence::{PM_OBJECT_PREFIX, PM_OBJECT_STAT_PREFIX};

fn re(e: rocksdb::Error) -> Error {
    Error::Storage(format!("rocksdb: {e}"))
}

/// The reference writes its catalog keys with `disableWAL = true`
/// (`kv_store_impl.cpp:311,441`); only the object bookkeeping keys keep the log
/// on, and then flush it with `FlushWAL(false)` — a userspace flush, never an
/// `fsync` (`persistence_manager_impl.cpp:141`, `obj_stat_accessor_impl.cpp:160`).
fn keeps_wal(key: &[u8]) -> bool {
    key.starts_with(PM_OBJECT_PREFIX.as_bytes())
        || key.starts_with(PM_OBJECT_STAT_PREFIX.as_bytes())
}

fn write_options(wal: bool) -> WriteOptions {
    let mut opts = WriteOptions::default();
    opts.disable_wal(!wal);
    opts
}

/// A `MetaStore` over a RocksDB directory (one default column family, matching
/// the C++ engine's `catalog` directory).
pub struct RocksMetaStore {
    db: DB,
    /// Serialises the read-modify-write of [`MetaStore::merge`].
    merge_lock: Mutex<()>,
}

impl RocksMetaStore {
    /// Opens (creating if needed) a RocksDB database at `path`.
    ///
    /// `create_if_missing` mirrors the C++ open path. Like the reference's
    /// `KVStore`, the handle keeps a WAL only for the object bookkeeping keys —
    /// everything else is written with `disable_wal` and becomes durable at the
    /// checkpoint's `Flush()` (see `keeps_wal`).
    pub fn open(path: impl AsRef<Path>) -> Result<Self> {
        let path = path.as_ref();
        // `create_if_missing` creates the database's own files, not its parent
        // directory: a data root that was just emptied (the restart harness
        // removes `catalog/` between cases) made `DB::open` fail with
        // "No such file or directory" and the server exit at startup.
        if let Some(parent) = path.parent() {
            std::fs::create_dir_all(parent).map_err(|e| Error::Io {
                path: parent.to_path_buf(),
                source: e,
            })?;
        }
        let mut opts = Options::default();
        opts.create_if_missing(true);
        let db = DB::open(&opts, path).map_err(re)?;
        Ok(Self {
            db,
            merge_lock: Mutex::new(()),
        })
    }
}

impl MetaStore for RocksMetaStore {
    fn get(&self, key: &[u8]) -> Result<Option<Vec<u8>>> {
        self.db.get(key).map_err(re)
    }

    fn put(&self, key: &[u8], value: &[u8]) -> Result<()> {
        let wal = keeps_wal(key);
        self.db
            .put_opt(key, value, &write_options(wal))
            .map_err(re)?;
        if wal {
            self.db.flush_wal(false).map_err(re)?;
        }
        Ok(())
    }

    fn delete(&self, key: &[u8]) -> Result<()> {
        let wal = keeps_wal(key);
        self.db.delete_opt(key, &write_options(wal)).map_err(re)?;
        if wal {
            self.db.flush_wal(false).map_err(re)?;
        }
        Ok(())
    }

    fn atomic_batch(&self, items: &[(Vec<u8>, Option<Vec<u8>>)]) -> Result<()> {
        let mut batch = WriteBatch::default();
        for (key, value) in items {
            match value {
                Some(value) => batch.put(key, value),
                None => batch.delete(key),
            }
        }
        let wal = items.iter().any(|(key, _)| keeps_wal(key));
        self.db.write_opt(batch, &write_options(wal)).map_err(re)?;
        if wal {
            self.db.flush_wal(false).map_err(re)?;
        }
        Ok(())
    }

    fn merge(&self, key: &[u8], delta: u64) -> Result<u64> {
        let _guard = self.merge_lock.lock().unwrap();
        let merged = crate::metastore::merge_delta(self.get(key)?.as_deref(), delta)?;
        self.put(key, merged.to_string().as_bytes())?;
        Ok(merged)
    }

    fn scan_prefix(&self, prefix: &[u8]) -> Result<Vec<(Vec<u8>, Vec<u8>)>> {
        // RocksDB iterates in key order, which is the order the trait promises.
        let mut out = Vec::new();
        let iter = self.db.iterator(rocksdb::IteratorMode::From(
            prefix,
            rocksdb::Direction::Forward,
        ));
        for item in iter {
            let (k, v) = item.map_err(re)?;
            if !k.starts_with(prefix) {
                break;
            }
            out.push((k.to_vec(), v.to_vec()));
        }
        Ok(out)
    }

    fn scan_range(&self, start: &[u8], end: &[u8]) -> Result<Vec<(Vec<u8>, Vec<u8>)>> {
        let mut out = Vec::new();
        let iter = self.db.iterator(rocksdb::IteratorMode::From(
            start,
            rocksdb::Direction::Forward,
        ));
        for item in iter {
            let (k, v) = item.map_err(re)?;
            if k.as_ref() >= end {
                break;
            }
            out.push((k.to_vec(), v.to_vec()));
        }
        Ok(out)
    }

    fn flush(&self) -> Result<()> {
        // `KVStore::Flush()` (`kv_store_impl.cpp:390`) is one `Flush()`: the
        // memtable becomes an SST (RocksDB fsyncs it), and nothing syncs a log
        // because the catalog writes never entered one. The checkpoint is the
        // durability boundary, so that is all this step has to do.
        self.db.flush().map_err(re)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::catalog_format;

    #[test]
    fn put_get_delete_and_prefix_scan() {
        let tmp = tempfile::tempdir().unwrap();
        let store = RocksMetaStore::open(tmp.path()).unwrap();

        // Use the C++ key spellings so the bytes match a C++-written catalog.
        let db_id = catalog_format::catalog_db_key("default_db", 1);
        store.put(db_id.as_bytes(), b"1").unwrap();
        assert_eq!(store.get(db_id.as_bytes()).unwrap(), Some(b"1".to_vec()));

        let table = catalog_format::catalog_table_key("1", "t", 2);
        store.put(table.as_bytes(), b"9").unwrap();
        let prefix = catalog_format::catalog_table_prefix("1", "t");
        let scanned = store.scan_prefix(prefix.as_bytes()).unwrap();
        assert_eq!(scanned.len(), 1);
        assert_eq!(scanned[0].0, table.as_bytes());

        store.delete(db_id.as_bytes()).unwrap();
        assert_eq!(store.get(db_id.as_bytes()).unwrap(), None);
    }

    #[test]
    fn atomic_write_is_all_or_nothing() {
        let tmp = tempfile::tempdir().unwrap();
        let store = RocksMetaStore::open(tmp.path()).unwrap();
        store
            .atomic_write(&[
                (b"a".to_vec(), b"1".to_vec()),
                (b"b".to_vec(), b"2".to_vec()),
            ])
            .unwrap();
        assert_eq!(store.get(b"a").unwrap(), Some(b"1".to_vec()));
        assert_eq!(store.get(b"b").unwrap(), Some(b"2".to_vec()));
    }

    #[test]
    fn scan_prefix_is_key_ordered() {
        let tmp = tempfile::tempdir().unwrap();
        let store = RocksMetaStore::open(tmp.path()).unwrap();
        for n in ["9", "10", "1"] {
            let key = catalog_format::catalog_segment_key("1", "2", n.parse().unwrap());
            store.put(key.as_bytes(), b"x").unwrap();
        }
        let prefix = catalog_format::catalog_segment_prefix("1", "2");
        let keys: Vec<String> = store
            .scan_prefix(prefix.as_bytes())
            .unwrap()
            .into_iter()
            .map(|(k, _)| String::from_utf8_lossy(&k).to_string())
            .collect();
        // Decimal ids sort lexicographically, exactly as the C++ engine's do.
        assert_eq!(
            keys,
            vec![
                "catalog|seg|1|2|1",
                "catalog|seg|1|2|10",
                "catalog|seg|1|2|9"
            ]
        );
    }

    #[test]
    fn survives_reopen_after_flush() {
        let tmp = tempfile::tempdir().unwrap();
        {
            let store = RocksMetaStore::open(tmp.path()).unwrap();
            store.put(b"k", b"v").unwrap();
            store.flush().unwrap();
        }
        let store = RocksMetaStore::open(tmp.path()).unwrap();
        assert_eq!(store.get(b"k").unwrap(), Some(b"v".to_vec()));
    }
}
