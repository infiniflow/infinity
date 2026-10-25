//! The `MetaStore` trait — the KV surface the catalog and the row store sit on.
//!
//! The one implementation is RocksDB ([`crate::metastore_rocks::RocksMetaStore`]),
//! the substrate the C++ engine uses, so a directory either engine wrote reads
//! back byte-for-byte. The trait keeps that decision out of the callers.
//!
//! It backs catalog/meta, ID counters, row records and tombstones — the subset
//! of RocksDB usage that Infinity relies on.

use infinity_common::{Error, Result};

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
    /// mirrored with O(block) memory.
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

#[cfg(test)]
mod tests {
    use crate::metastore_rocks::RocksMetaStore;
    use crate::MetaStore;
    use tempfile::tempdir;

    fn open() -> (tempfile::TempDir, RocksMetaStore) {
        let dir = tempdir().unwrap();
        let s = RocksMetaStore::open(dir.path()).unwrap();
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
            let s = RocksMetaStore::open(dir.path()).unwrap();
            s.put(b"persist", b"yes").unwrap();
            s.flush().unwrap();
        }
        let s = RocksMetaStore::open(dir.path()).unwrap();
        assert_eq!(s.get(b"persist").unwrap(), Some(b"yes".to_vec()));
    }
}
