//! `infinity-index`: the index layer (plan §2.6 / M3).
//!
//! Seeded with a **secondary index**: a byte-keyed ordered multimap with equality
//! and range lookup, plus a persist/load round-trip. Keys are opaque byte strings
//! (the engine encodes typed values to bytes), matching Infinity's byte-key
//! design. Indexes are pluggable behind the [`Index`] trait.

#![forbid(unsafe_code)]

pub mod fulltext;
pub mod vector;

use std::collections::BTreeMap;
use std::fs;
use std::io::Write;
use std::path::Path;

use infinity_common::{Error, Result};

const MAGIC: u32 = 0x5345_4331; // "SEC1"
pub type RowID = u64;

/// A generic secondary index over byte keys.
pub trait Index {
    /// Records that `row_id` has `key`.
    fn insert(&mut self, key: &[u8], row_id: RowID);
    /// Row ids for an exact key (empty if absent).
    fn lookup(&self, key: &[u8]) -> &[RowID];
    /// Entries with `lo <= key < hi` (`None` = unbounded), in key order.
    fn range(&self, lo: Option<&[u8]>, hi: Option<&[u8]>) -> Vec<(Vec<u8>, Vec<RowID>)>;
    /// Number of distinct keys.
    fn key_count(&self) -> usize;
}

/// A byte-keyed ordered multimap.
#[derive(Debug, Default, Clone)]
pub struct SecondaryIndex {
    map: BTreeMap<Vec<u8>, Vec<RowID>>,
}

impl SecondaryIndex {
    pub fn new() -> Self {
        Self::default()
    }

    /// Persists to `path` (length-prefixed entries). Durable via `sync_all`.
    pub fn flush_to(&self, path: impl AsRef<Path>) -> Result<()> {
        let path = path.as_ref();
        let mut buf = Vec::new();
        buf.extend_from_slice(&MAGIC.to_le_bytes());
        buf.extend_from_slice(&(self.map.len() as u64).to_le_bytes());
        for (k, ids) in &self.map {
            buf.extend_from_slice(&(k.len() as u32).to_le_bytes());
            buf.extend_from_slice(k);
            buf.extend_from_slice(&(ids.len() as u32).to_le_bytes());
            for id in ids {
                buf.extend_from_slice(&id.to_le_bytes());
            }
        }
        let mut f = fs::File::create(path).map_err(|e| Error::io(path, e))?;
        f.write_all(&buf).map_err(|e| Error::io(path, e))?;
        f.sync_all().map_err(|e| Error::io(path, e))?;
        Ok(())
    }

    /// Loads a previously flushed index.
    pub fn load(path: impl AsRef<Path>) -> Result<Self> {
        let path = path.as_ref();
        let buf = fs::read(path).map_err(|e| Error::io(path, e))?;
        let bad = || Error::Invalid("corrupt secondary index".into());
        let mut p = 0usize;
        let rd_u32 = |b: &[u8], p: &mut usize| -> Result<u32> {
            if *p + 4 > b.len() {
                return Err(Error::Invalid("truncated index".into()));
            }
            let v = u32::from_le_bytes(b[*p..*p + 4].try_into().unwrap());
            *p += 4;
            Ok(v)
        };
        let rd_u64 = |b: &[u8], p: &mut usize| -> Result<u64> {
            if *p + 8 > b.len() {
                return Err(Error::Invalid("truncated index".into()));
            }
            let v = u64::from_le_bytes(b[*p..*p + 8].try_into().unwrap());
            *p += 8;
            Ok(v)
        };
        if rd_u32(&buf, &mut p)? != MAGIC {
            return Err(bad());
        }
        let n = rd_u64(&buf, &mut p)?;
        let mut map = BTreeMap::new();
        for _ in 0..n {
            let klen = rd_u32(&buf, &mut p)? as usize;
            if p + klen > buf.len() {
                return Err(bad());
            }
            let k = buf[p..p + klen].to_vec();
            p += klen;
            let m = rd_u32(&buf, &mut p)? as usize;
            let mut ids = Vec::with_capacity(m);
            for _ in 0..m {
                ids.push(rd_u64(&buf, &mut p)?);
            }
            map.insert(k, ids);
        }
        Ok(Self { map })
    }
}

impl Index for SecondaryIndex {
    fn insert(&mut self, key: &[u8], row_id: RowID) {
        self.map.entry(key.to_vec()).or_default().push(row_id);
    }

    fn lookup(&self, key: &[u8]) -> &[RowID] {
        self.map.get(key).map(|v| v.as_slice()).unwrap_or(&[])
    }

    fn range(&self, lo: Option<&[u8]>, hi: Option<&[u8]>) -> Vec<(Vec<u8>, Vec<RowID>)> {
        use std::ops::Bound::{Excluded, Included, Unbounded};
        let lower = lo.map(|k| Included(k.to_vec())).unwrap_or(Unbounded);
        let upper = hi.map(|k| Excluded(k.to_vec())).unwrap_or(Unbounded);
        self.map
            .range((lower, upper))
            .map(|(k, v)| (k.clone(), v.clone()))
            .collect()
    }

    fn key_count(&self) -> usize {
        self.map.len()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::tempdir;

    #[test]
    fn insert_lookup() {
        let mut idx = SecondaryIndex::new();
        idx.insert(b"a", 1);
        idx.insert(b"a", 2);
        idx.insert(b"b", 3);
        assert_eq!(idx.lookup(b"a"), &[1, 2]);
        assert_eq!(idx.lookup(b"b"), &[3]);
        assert_eq!(idx.lookup(b"z"), &[] as &[RowID]);
        assert_eq!(idx.key_count(), 2);
    }

    #[test]
    fn range_scan() {
        let mut idx = SecondaryIndex::new();
        for (k, id) in [(b"a", 1u64), (b"b", 2), (b"c", 3), (b"d", 4)] {
            idx.insert(k, id);
        }
        let got: Vec<Vec<u8>> = idx
            .range(Some(b"b"), Some(b"d"))
            .into_iter()
            .map(|(k, _)| k)
            .collect();
        assert_eq!(got, vec![b"b".to_vec(), b"c".to_vec()]);
        let all = idx.range(None, None);
        assert_eq!(all.len(), 4);
    }

    #[test]
    fn persist_and_load() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("chunk_0.idx");
        let mut idx = SecondaryIndex::new();
        idx.insert(b"x", 10);
        idx.insert(b"x", 11);
        idx.insert(b"y", 12);
        idx.flush_to(&path).unwrap();

        let loaded = SecondaryIndex::load(&path).unwrap();
        assert_eq!(loaded.lookup(b"x"), &[10, 11]);
        assert_eq!(loaded.lookup(b"y"), &[12]);
        assert_eq!(loaded.key_count(), 2);
    }
}
