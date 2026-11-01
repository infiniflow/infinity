//! `infinity-index`: the index layer (plan §2.6 / M3).
//!
//! Seeded with a **secondary index**: a byte-keyed ordered multimap with equality
//! and range lookup, plus a persist/load round-trip. Keys are opaque byte strings
//! (the engine encodes typed values to bytes), matching Infinity's byte-key
//! design. Indexes are pluggable behind the [`Index`] trait.

#![forbid(unsafe_code)]
// The distance kernels use portable SIMD (`std::simd`, still nightly-only) to
// mirror the reference's `F32L2*` / `F32IP*` / `F32Cos*` intrinsics; see
// `simd.rs` for why that is `std::simd` and not `std::arch`.
#![feature(portable_simd)]

pub mod bp;
pub mod emvb;
pub mod fulltext;
pub mod hnsw_handler;
pub mod ivf;
pub mod ivf_def;
pub mod knn_flat;
pub mod linscan;
pub mod lsg;
pub mod lvq;
pub mod merge_knn;
pub mod multivector;
pub mod parallel_build;
pub mod plaid;
pub mod rabitq;
pub mod simd;
pub mod smve;
pub mod sparse;
pub mod vector;

#[cfg(test)]
mod quantizer_perf;

use std::collections::BTreeMap;
use std::fs;
use std::io::Write;
use std::path::Path;

/// Writes an index file and fsyncs it before returning.
///
/// The reference syncs every file its index workers finish
/// (`memory_indexer_impl.cpp:535-536,762-763`, and `FileWriter::Sync` for the
/// KNN writers): a chunk a catalog record names must be on disk on its own, not
/// only in the page cache. No directory fsync — the reference does not fsync
/// directories either.
pub(crate) fn write_synced(path: &Path, bytes: &[u8]) -> Result<()> {
    if let Some(dir) = path.parent() {
        fs::create_dir_all(dir).map_err(|e| Error::Invalid(format!("{}: {e}", dir.display())))?;
    }
    let mut file = fs::File::create(path).map_err(|e| Error::io(path, e))?;
    file.write_all(bytes).map_err(|e| Error::io(path, e))?;
    file.sync_all().map_err(|e| Error::io(path, e))?;
    Ok(())
}

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
    /// Approximate heap bytes, maintained by the mutators.
    ///
    /// The unified memindex quota is checked on every row, so this used to walk
    /// the whole map per check: with three 100k-key indexes a 5k-row `UPDATE`
    /// spent 50% of its time summing keys it already knew.
    bytes: usize,
}

impl SecondaryIndex {
    pub fn new() -> Self {
        Self::default()
    }

    /// Approximate heap bytes held by the map, for the unified memindex budget.
    pub fn approx_bytes(&self) -> usize {
        self.bytes
    }

    /// Removes and returns every entry, leaving the index empty. Used when a
    /// memory tail is dumped into a sealed segment.
    pub fn take(&mut self) -> BTreeMap<Vec<u8>, Vec<RowID>> {
        self.bytes = 0;
        std::mem::take(&mut self.map)
    }

    /// Number of row entries across all keys (C++ `GetRowCount`).
    pub fn row_count(&self) -> usize {
        self.map.values().map(Vec::len).sum()
    }

    /// Lowest row id recorded, in `segment << 32 | offset` form (C++
    /// `GetBeginRowID`).
    pub fn begin_row_id(&self) -> Option<RowID> {
        self.map.values().flatten().min().copied()
    }

    /// Visits every `(key, row ids)` pair in ascending **byte** order, without
    /// cloning, so a caller can apply its own comparison.
    ///
    /// This order is not the order of the values the keys were built from: the
    /// memory tail is keyed by `Literal::to_slt_string().as_bytes()`
    /// (`infinity-executor`'s `index_lookup`), so `b"10"` sorts before
    /// `b"9"`. A numeric range scan must decode and compare each key it visits
    /// — it must **not** stop early or slice the walk at a bound, or it will
    /// silently drop rows. [`Index::range`] is byte-order-only for the same
    /// reason.
    pub fn for_each_in_order(&self, mut f: impl FnMut(&[u8], &[RowID])) {
        for (key, ids) in &self.map {
            f(key, ids);
        }
    }

    /// Lowest row id and row count of `segment` (C++ `SegmentIndexMeta::
    /// GetMemIndex()` reads one segment's memory tail).
    pub fn segment_begin_and_count(&self, segment: u32) -> Option<(RowID, usize)> {
        let segment = segment as u64;
        let mut begin: Option<RowID> = None;
        let mut count = 0usize;
        for id in self.map.values().flatten() {
            if (id >> 32) == segment {
                begin = Some(begin.map_or(*id, |b| b.min(*id)));
                count += 1;
            }
        }
        begin.map(|b| (b, count))
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
        let mut bytes = 0usize;
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
            bytes += k.len() + 16 + 24 + ids.len() * std::mem::size_of::<RowID>();
            map.insert(k, ids);
        }
        Ok(Self { map, bytes })
    }
}

impl Index for SecondaryIndex {
    fn insert(&mut self, key: &[u8], row_id: RowID) {
        match self.map.get_mut(key) {
            Some(ids) => {
                ids.push(row_id);
                self.bytes += std::mem::size_of::<RowID>();
            }
            None => {
                // The key, the row-id vector and its first element.
                self.bytes += key.len() + 16 + 24 + std::mem::size_of::<RowID>();
                self.map.insert(key.to_vec(), vec![row_id]);
            }
        }
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

    /// `for_each_in_order` walks in byte order and reports every row id of a
    /// key exactly once.
    #[test]
    fn for_each_in_order_walks_keys_and_row_ids_once() {
        let mut idx = SecondaryIndex::new();
        // Inserted out of order; the walk is by key.
        for (k, id) in [("c", 3u64), ("a", 1), ("b", 2), ("b", 4)] {
            idx.insert(k.as_bytes(), id);
        }
        let mut seen: Vec<(String, Vec<RowID>)> = Vec::new();
        idx.for_each_in_order(|key, ids| {
            seen.push((String::from_utf8(key.to_vec()).unwrap(), ids.to_vec()))
        });
        assert_eq!(
            seen,
            vec![
                ("a".to_string(), vec![1]),
                ("b".to_string(), vec![2, 4]),
                ("c".to_string(), vec![3]),
            ]
        );
        // Every row id appears exactly once across the whole walk.
        let mut ids: Vec<RowID> = seen.iter().flat_map(|(_, ids)| ids.clone()).collect();
        ids.sort_unstable();
        assert_eq!(ids, vec![1, 2, 3, 4]);
        assert_eq!(idx.row_count(), 4);
    }

    /// The walk is **byte** order, not the numeric order of the values the
    /// keys were encoded from: `"10"` precedes `"9"`. A range scan must decode
    /// and compare each key rather than slice the walk.
    #[test]
    fn for_each_in_order_is_byte_order_not_numeric_order() {
        let mut idx = SecondaryIndex::new();
        idx.insert(b"9", 9);
        idx.insert(b"10", 10);
        let mut keys: Vec<String> = Vec::new();
        idx.for_each_in_order(|key, _| keys.push(String::from_utf8_lossy(key).into_owned()));
        assert_eq!(keys, vec!["10".to_string(), "9".to_string()]);
    }
}
