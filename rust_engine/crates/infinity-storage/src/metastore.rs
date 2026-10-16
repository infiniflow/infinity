//! The `MetaStore` trait and its pure-Rust `fjall` implementation (plan §3 D2).
//!
//! Backs catalog/meta, ID counters, object manifests and tombstones — the subset
//! of RocksDB usage that Infinity relies on. `rust-rocksdb` can later provide an
//! alternative impl behind the same trait.

use std::path::Path;

use fjall::{Database, Keyspace, KeyspaceCreateOptions, PersistMode};
use infinity_common::{Error, Result};

fn fe(e: fjall::Error) -> Error {
    Error::Storage(e.to_string())
}

/// A transactional key-value store for metadata.
pub trait MetaStore: Send + Sync {
    fn get(&self, key: &[u8]) -> Result<Option<Vec<u8>>>;
    fn put(&self, key: &[u8], value: &[u8]) -> Result<()>;
    fn delete(&self, key: &[u8]) -> Result<()>;
    /// Atomically apply all writes (single all-or-nothing batch).
    fn atomic_write(&self, items: &[(Vec<u8>, Vec<u8>)]) -> Result<()>;
    /// All `(key, value)` pairs whose key starts with `prefix`, in key order.
    fn scan_prefix(&self, prefix: &[u8]) -> Result<Vec<(Vec<u8>, Vec<u8>)>>;
    /// Flush to disk (durability barrier).
    fn flush(&self) -> Result<()>;
}

/// `fjall`-backed `MetaStore`.
pub struct FjallMetaStore {
    db: Database,
    ks: Keyspace,
}

impl FjallMetaStore {
    /// Opens (creating if needed) a metastore under `path` in keyspace `name`.
    pub fn open(path: impl AsRef<Path>, name: &str) -> Result<Self> {
        let db = Database::builder(path.as_ref()).open().map_err(fe)?;
        let ks = db
            .keyspace(name, KeyspaceCreateOptions::default)
            .map_err(fe)?;
        Ok(Self { db, ks })
    }
}

impl MetaStore for FjallMetaStore {
    fn get(&self, key: &[u8]) -> Result<Option<Vec<u8>>> {
        Ok(self.ks.get(key).map_err(fe)?.map(|v| v.to_vec()))
    }

    fn put(&self, key: &[u8], value: &[u8]) -> Result<()> {
        self.ks.insert(key, value).map_err(fe)
    }

    fn delete(&self, key: &[u8]) -> Result<()> {
        self.ks.remove(key).map_err(fe)
    }

    fn atomic_write(&self, items: &[(Vec<u8>, Vec<u8>)]) -> Result<()> {
        let mut batch = self.db.batch();
        for (k, v) in items {
            batch.insert(&self.ks, k.as_slice(), v.as_slice());
        }
        batch.commit().map_err(fe)
    }

    fn scan_prefix(&self, prefix: &[u8]) -> Result<Vec<(Vec<u8>, Vec<u8>)>> {
        let mut out = Vec::new();
        for kv in self.ks.prefix(prefix) {
            let (k, v) = kv.into_inner().map_err(fe)?;
            out.push((k.to_vec(), v.to_vec()));
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
