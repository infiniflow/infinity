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

use std::collections::HashMap;
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

use common::{Error, Result};

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

/// A byte-keyed multimap.
///
/// The map is unordered: an insert hashes the key instead of walking a
/// B-tree's nodes (at 100k keys a `BTreeMap` insert spent its time in
/// `memcmp` over ~17 levels), and every reader either looks a key up or scans
/// all of them. The chunk dump sorts what it writes, so the files stay
/// deterministic.
#[derive(Debug, Default, Clone)]
pub struct SecondaryIndex {
    map: HashMap<Vec<u8>, Vec<RowID>>,
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
    pub fn take(&mut self) -> HashMap<Vec<u8>, Vec<RowID>> {
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

    /// Visits every `(key, row ids)` pair, without cloning, so a caller can
    /// apply its own comparison. The order is unspecified.
    ///
    /// A range scan must decode and compare each key it visits - it must
    /// **not** stop early or slice the walk at a bound: the memory tail is
    /// keyed by the cell's stored text (`Literal::to_slt_string().as_bytes()`),
    /// so `b"10"` sorts before `b"9"` and numeric order is not byte order.
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
        let mut map = HashMap::new();
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

impl SecondaryIndex {
    /// Drops one row from every posting list, and the keys that become empty.
    ///
    /// `DeleteInBlock`'s index half: an updated or deleted row leaves the value
    /// it used to have. The caller knows the row ids, not the old values, so
    /// this walks the map - the same shape as the full-text tail's
    /// `index_delete_row`.
    pub fn delete_row(&mut self, row_id: RowID) {
        let mut emptied: Vec<Vec<u8>> = Vec::new();
        for (key, ids) in self.map.iter_mut() {
            let before = ids.len();
            ids.retain(|id| *id != row_id);
            if ids.len() != before {
                self.bytes = self
                    .bytes
                    .saturating_sub((before - ids.len()) * std::mem::size_of::<RowID>());
                if ids.is_empty() {
                    emptied.push(key.clone());
                }
            }
        }
        for key in emptied {
            self.map.remove(&key);
            self.bytes = self.bytes.saturating_sub(key.len() + 16 + 24);
        }
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
        let mut out: Vec<(Vec<u8>, Vec<RowID>)> = self
            .map
            .iter()
            .filter(|(key, _)| {
                lo.map(|lo| key.as_slice() >= lo).unwrap_or(true)
                    && hi.map(|hi| key.as_slice() < hi).unwrap_or(true)
            })
            .map(|(key, ids)| (key.clone(), ids.clone()))
            .collect();
        out.sort_by(|a, b| a.0.cmp(&b.0));
        out
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

    /// The walk reports every key once, with all of its row ids (the order is
    /// unspecified: the tail is a hash map).
    #[test]
    fn for_each_in_order_walks_keys_and_row_ids_once() {
        let mut idx = SecondaryIndex::new();
        for (k, id) in [("c", 3u64), ("a", 1), ("b", 2), ("b", 4)] {
            idx.insert(k.as_bytes(), id);
        }
        let mut seen: Vec<(String, Vec<RowID>)> = Vec::new();
        idx.for_each_in_order(|key, ids| {
            seen.push((String::from_utf8(key.to_vec()).unwrap(), ids.to_vec()))
        });
        seen.sort_by(|a, b| a.0.cmp(&b.0));
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

    /// `range` is **byte** order, not the numeric order of the values the keys
    /// were encoded from: `"10"` precedes `"9"`, so a scan that slices at a
    /// numeric bound would drop rows. It sorts its result by key for that
    /// reason, whatever order the tail holds.
    #[test]
    fn range_is_byte_order_not_numeric_order() {
        let mut idx = SecondaryIndex::new();
        idx.insert(b"9", 9);
        idx.insert(b"10", 10);
        idx.insert(b"11", 11);
        let keys: Vec<String> = idx
            .range(None, None)
            .into_iter()
            .map(|(key, _)| String::from_utf8_lossy(&key).into_owned())
            .collect();
        assert_eq!(
            keys,
            vec!["10".to_string(), "11".to_string(), "9".to_string()]
        );
        // A bound is compared byte-wise: ["10", "9") keeps both numeric strings
        // below "9" in byte order, i.e. everything from "10" up to "8…".
        let keys: Vec<String> = idx
            .range(Some(b"10"), Some(b"9"))
            .into_iter()
            .map(|(key, _)| String::from_utf8_lossy(&key).into_owned())
            .collect();
        assert_eq!(keys, vec!["10".to_string(), "11".to_string()]);
    }
}
