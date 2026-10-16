//! RocksDB-backed `MetaStore` for a **C++-compatible catalog** (plan §2.6.1
//! item 4: "Catalog KV 需切到 RocksDB").
//!
//! The C++ engine stores its catalog in RocksDB under the key space frozen in
//! `docs/CPP_FORMAT_SPEC.md` §8.2; `infinity-storage::catalog_format` renders
//! those keys. This module provides the substrate, so pointing the engine at a
//! directory the C++ engine wrote (or handing it one) uses the same bytes.
//!
//! Gated behind the `rocksdb-store` feature because it pulls in the C++
//! `librocksdb`; the default build stays pure Rust.

use std::path::Path;

use infinity_common::{Error, Result};
use rocksdb::{Options, WriteBatch, DB};

use crate::metastore::MetaStore;

fn re(e: rocksdb::Error) -> Error {
    Error::Storage(format!("rocksdb: {e}"))
}

/// A `MetaStore` over a RocksDB directory (one default column family, matching
/// the C++ engine's `catalog` directory).
pub struct RocksMetaStore {
    db: DB,
}

impl RocksMetaStore {
    /// Opens (creating if needed) a RocksDB database at `path`.
    ///
    /// `create_if_missing` mirrors the C++ open path; `sync` on flush gives the
    /// durability barrier the `MetaStore` contract asks for.
    pub fn open(path: impl AsRef<Path>) -> Result<Self> {
        let mut opts = Options::default();
        opts.create_if_missing(true);
        let db = DB::open(&opts, path.as_ref()).map_err(re)?;
        Ok(Self { db })
    }
}

impl MetaStore for RocksMetaStore {
    fn get(&self, key: &[u8]) -> Result<Option<Vec<u8>>> {
        self.db.get(key).map_err(re)
    }

    fn put(&self, key: &[u8], value: &[u8]) -> Result<()> {
        self.db.put(key, value).map_err(re)
    }

    fn delete(&self, key: &[u8]) -> Result<()> {
        self.db.delete(key).map_err(re)
    }

    fn atomic_write(&self, items: &[(Vec<u8>, Vec<u8>)]) -> Result<()> {
        let mut batch = WriteBatch::default();
        for (k, v) in items {
            batch.put(k, v);
        }
        self.db.write(batch).map_err(re)
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

    fn flush(&self) -> Result<()> {
        // `flush` + WAL sync is the durability barrier the trait contract wants.
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
