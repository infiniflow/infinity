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
use std::sync::atomic::{AtomicUsize, Ordering as AtomicOrdering};
use std::sync::{Arc, Mutex, RwLock};

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
///
/// Keys are **order-preserving encodings** of the indexed value, so the trait's
/// `range` is a real ordered range: byte order is value order. Both the
/// in-memory tail and the sealed chunks agree on the encoding
/// (`storage::secondary_segment::Key::ordered_bytes`).
pub trait Index {
    /// Records that `row_id` has `key`.
    fn insert(&self, key: &[u8], row_id: RowID);
    /// Row ids for an exact key (empty if absent).
    fn lookup(&self, key: &[u8]) -> Vec<RowID>;
    /// Entries with `lo <= key <= hi` (`None` = unbounded), in key order.
    ///
    /// Inclusive on both ends, like the reference's `lower_bound`/`upper_bound`
    /// pair over a chunk's key array.
    fn range(&self, lo: Option<&[u8]>, hi: Option<&[u8]>) -> Vec<(Vec<u8>, Vec<RowID>)>;
    /// Number of distinct keys.
    fn key_count(&self) -> usize;
}

/// An ordered multimap of row ids, one entry per `(key, row)` pair.
///
/// The reference's memory tail is `RcuMultiMap<KeyType, u32>`
/// (`src/storage/common/rcu_multimap.cppm`): a `std::multimap` ordered by the
/// column's `OrderedKeyType` — `INTEGER` keeps the value, `VARCHAR` is hashed to
/// `u64`, `DATE` is the day count — with readers that never take the writer's
/// lock. Three properties matter, and the `HashMap<Vec<u8>, Vec<RowID>>` this
/// replaces had none of them:
///
/// * **Order.** Keys are order-preserving encodings of the value
///   (`storage::secondary_segment::Key::ordered_bytes`), so byte order *is*
///   value order and a range is two `BTreeMap::range` calls. With the cell's
///   display text as the key, `"10"` sorted before `"9"` and a numeric range
///   could only be answered by decoding every key in the map.
/// * **Reads never block writes.** The same RCU shape the reference has: a
///   *published* snapshot (`read_map_`) that readers take an `Arc` of, plus a
///   *pending* map (`dirty_map_`) that writers append to under one short lock.
///   The pending map is folded into the snapshot once it has grown to the size
///   of the snapshot, so the copy is amortised `O(1)` per insert.
/// * **No lost rows.** The reference's `Get`/`range` stop at the first map that
///   has the key, so a row written after the last swap is invisible to a key
///   that the published map already holds (`read_map_` has `{k: a}`, `dirty_map_`
///   has `{k: a, b}`, and `b` is dropped). Here both sides are always merged.
#[derive(Debug)]
pub struct SecondaryIndex {
    /// RCU `read_map_`: the published snapshot. Readers clone the `Arc` under a
    /// read lock held for one pointer copy, then work lock-free.
    published: RwLock<Arc<Map>>,
    /// RCU `dirty_map_`: everything written since the last fold.
    pending: Mutex<Map>,
    /// Row entries in both maps (C++ `GetRowCount`), maintained on insert.
    rows: AtomicUsize,
    /// Approximate heap bytes, maintained by the mutators.
    ///
    /// The unified memindex quota is checked on every row, so this used to walk
    /// the whole map per check: with three 100k-key indexes a 5k-row `UPDATE`
    /// spent 50% of its time summing keys it already knew.
    bytes: AtomicUsize,
    /// Distinct keys in `published`, so the fold threshold is read without a
    /// lock.
    published_keys: AtomicUsize,
}

/// `BTreeMap<encoded key, row ids>`: the reference's `InnerMultiMap`.
type Map = BTreeMap<Vec<u8>, Vec<RowID>>;

/// The pending map is folded into the snapshot once it holds at least this
/// many keys, or as many as the snapshot already has (whichever is larger).
/// The snapshot doubles each time, which makes the `O(n)` copy amortised `O(1)`
/// per insert - the reference's `miss_time_ >= dirty_map_->size()` heuristic
/// with a bound that also fires on a write-only workload, where no read ever
/// records a miss.
const FOLD_MIN_KEYS: usize = 1024;

impl Default for SecondaryIndex {
    fn default() -> Self {
        Self {
            published: RwLock::new(Arc::new(Map::new())),
            pending: Mutex::new(Map::new()),
            rows: AtomicUsize::new(0),
            bytes: AtomicUsize::new(0),
            published_keys: AtomicUsize::new(0),
        }
    }
}

impl Clone for SecondaryIndex {
    fn clone(&self) -> Self {
        // `pending` first, like every reader: `fold` takes it before it
        // republishes, so the two views cannot disagree.
        let pending = self.pending.lock().unwrap().clone();
        let published = self.snapshot();
        Self {
            published: RwLock::new(published),
            rows: AtomicUsize::new(self.rows.load(AtomicOrdering::Relaxed)),
            bytes: AtomicUsize::new(self.bytes.load(AtomicOrdering::Relaxed)),
            published_keys: AtomicUsize::new(self.published_keys.load(AtomicOrdering::Relaxed)),
            pending: Mutex::new(pending),
        }
    }
}

/// A frozen memory tail: the entries a dump has taken and is writing into a
/// chunk.
///
/// `take` leaves the index empty, so between the dump reading the tail and the
/// chunk it writes becoming visible a reader would see neither copy. The dump
/// keeps the taken entries here, and readers merge them, until the chunk is
/// registered (`Engine::seal_secondary_index`).
pub struct SealedTail(Map);

impl SealedTail {
    pub fn is_empty(&self) -> bool {
        self.0.is_empty()
    }

    /// The row ids `key` maps to, exactly like an unfrozen tail.
    pub fn lookup(&self, key: &[u8]) -> Vec<RowID> {
        self.0.get(key).cloned().unwrap_or_default()
    }

    /// Every entry whose key lies in the inclusive range `[lo, hi]`.
    pub fn range(&self, lo: Option<&[u8]>, hi: Option<&[u8]>) -> Vec<(Vec<u8>, Vec<RowID>)> {
        use std::ops::Bound;
        let start = lo.map_or(Bound::Unbounded, |k| Bound::Included(k.to_vec()));
        let end = hi.map_or(Bound::Unbounded, |k| Bound::Included(k.to_vec()));
        self.0
            .range((start, end))
            .map(|(key, ids)| (key.clone(), ids.clone()))
            .collect()
    }

    /// Every entry in ascending key order: what the chunk writer consumes.
    pub fn entries(&self) -> Vec<(Vec<u8>, Vec<RowID>)> {
        self.0
            .iter()
            .map(|(key, ids)| (key.clone(), ids.clone()))
            .collect()
    }
}

impl SecondaryIndex {
    pub fn new() -> Self {
        Self::default()
    }

    /// The published snapshot. The `RwLock` is held for one `Arc` clone and
    /// released immediately, so a scan iterates it without blocking writers.
    fn snapshot(&self) -> Arc<Map> {
        Arc::clone(&self.published.read().unwrap())
    }

    /// Approximate heap bytes held by the map, for the unified memindex budget.
    pub fn approx_bytes(&self) -> usize {
        self.bytes.load(AtomicOrdering::Relaxed)
    }

    /// Folds `pending` into the published snapshot (the reference's
    /// `CheckSwapInLock`, minus the copy-of-dirty: readers merge both sides, so
    /// the pending map can start out empty afterwards).
    ///
    /// Lock order is `published` -> `pending`; `insert` only ever takes
    /// `pending`, so the two cannot deadlock.
    fn fold(&self) {
        let mut pending = self.pending.lock().unwrap();
        if pending.is_empty() {
            return;
        }
        let mut merged = (*self.snapshot()).clone();
        for (key, ids) in pending.iter() {
            match merged.get_mut(key) {
                Some(existing) => existing.extend_from_slice(ids),
                None => {
                    merged.insert(key.clone(), ids.clone());
                }
            }
        }
        let mut published = self.published.write().unwrap();
        *published = Arc::new(merged);
        self.published_keys
            .store(published.len(), AtomicOrdering::Relaxed);
        pending.clear();
    }

    /// Every entry, both maps merged, in ascending key order. Used by the
    /// flush/take/statistics paths, which have to materialise anyway.
    fn merged(&self) -> Map {
        // `pending` first: with it held, `fold` cannot republish between the two
        // reads (see `Index::lookup`).
        let pending = self.pending.lock().unwrap();
        let mut out = (*self.snapshot()).clone();
        for (key, ids) in pending.iter() {
            match out.get_mut(key) {
                Some(existing) => existing.extend_from_slice(ids),
                None => {
                    out.insert(key.clone(), ids.clone());
                }
            }
        }
        out
    }

    /// Removes and returns every entry, leaving the index empty. Used when a
    /// memory tail is dumped into a sealed segment (the reference's
    /// `PopMemIndex`).
    pub fn take(&self) -> Map {
        let mut pending = self.pending.lock().unwrap();
        let published =
            std::mem::replace(&mut *self.published.write().unwrap(), Arc::new(Map::new()));
        let mut merged = Arc::try_unwrap(published).unwrap_or_else(|arc| (*arc).clone());
        for (key, ids) in std::mem::take(&mut *pending) {
            match merged.get_mut(&key) {
                Some(existing) => existing.extend_from_slice(&ids),
                None => {
                    merged.insert(key, ids);
                }
            }
        }
        self.rows.store(0, AtomicOrdering::Relaxed);
        self.bytes.store(0, AtomicOrdering::Relaxed);
        self.published_keys.store(0, AtomicOrdering::Relaxed);
        merged
    }

    /// Freezes the tail for a dump: removes the entries and hands them back in a
    /// form readers can still query until the chunk that holds them is
    /// registered.
    pub fn seal(&self) -> SealedTail {
        SealedTail(self.take())
    }

    /// Whether `row_id` is already recorded under `key`.
    ///
    /// The DDL catch-up (`Engine::secondary_catch_up`) appends the rows a
    /// concurrent writer committed after the index build had read the table. A
    /// row that arrived *after* the tail was registered is already here, and
    /// [`Self::insert`] is not idempotent, so the row's first key decides.
    ///
    /// `pending` before the snapshot, like every other reader: with it held,
    /// `fold` cannot move the entry between the two views.
    pub fn contains(&self, key: &[u8], row_id: RowID) -> bool {
        let pending = self.pending.lock().unwrap();
        if pending.get(key).is_some_and(|ids| ids.contains(&row_id)) {
            return true;
        }
        self.snapshot()
            .get(key)
            .is_some_and(|ids| ids.contains(&row_id))
    }

    /// Number of row entries across all keys (C++ `GetRowCount`) - an atomic
    /// load, not a walk: the quota check reads it once per row.
    pub fn row_count(&self) -> usize {
        self.rows.load(AtomicOrdering::Relaxed)
    }

    /// Lowest row id recorded, in `segment << 32 | offset` form (C++
    /// `GetBeginRowID`).
    pub fn begin_row_id(&self) -> Option<RowID> {
        let pending = self.pending.lock().unwrap();
        let snapshot = self.snapshot();
        snapshot
            .values()
            .chain(pending.values())
            .flatten()
            .min()
            .copied()
    }

    /// Visits every `(key, row ids)` pair in ascending key order, without
    /// cloning the maps, so a caller can apply its own comparison.
    ///
    /// Ascending **encoded** order, i.e. value order: a range scan slices at its
    /// bounds instead of decoding every key. The two maps are merged on the fly,
    /// so a key that exists in both is reported once with both posting lists.
    pub fn for_each_in_order(&self, mut f: impl FnMut(&[u8], &[RowID])) {
        let pending = self.pending.lock().unwrap();
        let snapshot = self.snapshot();
        let mut pending_iter = pending.iter().peekable();
        for (key, ids) in snapshot.iter() {
            while let Some((pending_key, _)) = pending_iter.peek() {
                if pending_key.as_slice() >= key.as_slice() {
                    break;
                }
                let (pending_key, pending_ids) = pending_iter.next().unwrap();
                f(pending_key, pending_ids);
            }
            match pending_iter.peek() {
                Some((pending_key, pending_ids)) if pending_key.as_slice() == key.as_slice() => {
                    let (_, pending_ids) = pending_iter.next().unwrap();
                    let mut merged = ids.clone();
                    merged.extend_from_slice(pending_ids);
                    f(key, &merged);
                }
                _ => f(key, ids),
            }
        }
        for (key, ids) in pending_iter {
            f(key, ids);
        }
    }

    /// Lowest row id and row count of `segment` (C++ `SegmentIndexMeta::
    /// GetMemIndex()` reads one segment's memory tail).
    pub fn segment_begin_and_count(&self, segment: u32) -> Option<(RowID, usize)> {
        let segment = segment as u64;
        let snapshot = self.snapshot();
        let pending = self.pending.lock().unwrap();
        let mut begin: Option<RowID> = None;
        let mut count = 0usize;
        for id in snapshot.values().chain(pending.values()).flatten() {
            if (id >> 32) == segment {
                begin = Some(begin.map_or(*id, |b| b.min(*id)));
                count += 1;
            }
        }
        begin.map(|b| (b, count))
    }

    /// Persists to `path` (length-prefixed entries, ascending key order).
    /// Durable via `sync_all`.
    pub fn flush_to(&self, path: impl AsRef<Path>) -> Result<()> {
        let path = path.as_ref();
        let map = self.merged();
        let mut buf = Vec::new();
        buf.extend_from_slice(&MAGIC.to_le_bytes());
        buf.extend_from_slice(&(map.len() as u64).to_le_bytes());
        for (k, ids) in map.iter() {
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
        let rows = map.values().map(Vec::len).sum();
        let keys = map.len();
        Ok(Self {
            published: RwLock::new(Arc::new(map)),
            pending: Mutex::new(Map::new()),
            rows: AtomicUsize::new(rows),
            bytes: AtomicUsize::new(bytes),
            published_keys: AtomicUsize::new(keys),
        })
    }
}

impl Index for SecondaryIndex {
    fn insert(&self, key: &[u8], row_id: RowID) {
        let mut pending = self.pending.lock().unwrap();
        let fresh_key = match pending.get_mut(key) {
            Some(ids) => {
                ids.push(row_id);
                self.bytes
                    .fetch_add(std::mem::size_of::<RowID>(), AtomicOrdering::Relaxed);
                false
            }
            None => {
                // The key, the row-id vector and its first element.
                self.bytes.fetch_add(
                    key.len() + 16 + 24 + std::mem::size_of::<RowID>(),
                    AtomicOrdering::Relaxed,
                );
                pending.insert(key.to_vec(), vec![row_id]);
                true
            }
        };
        self.rows.fetch_add(1, AtomicOrdering::Relaxed);
        let fold_at = self
            .published_keys
            .load(AtomicOrdering::Relaxed)
            .max(FOLD_MIN_KEYS);
        let should_fold = fresh_key && pending.len() >= fold_at;
        drop(pending);
        if should_fold {
            // Outside the pending lock: `fold` takes it again together with the
            // published one, and it must not run with a reader's snapshot held.
            self.fold();
        }
    }

    fn lookup(&self, key: &[u8]) -> Vec<RowID> {
        // Both sides, always: the published map is stale by however much is
        // pending, and a key can exist in both (see the type docs).
        //
        // `pending` is taken **before** the snapshot: `fold` holds it while it
        // republishes, so with this lock held the two views describe the same
        // instant. Reading the snapshot first lets a fold move the keys it
        // folded out of `pending` after the reader already read the old
        // snapshot - the row then exists in neither view.
        let pending = self.pending.lock().unwrap();
        let snapshot = self.snapshot();
        let mut out = snapshot.get(key).cloned().unwrap_or_default();
        if let Some(ids) = pending.get(key) {
            out.extend_from_slice(ids);
        }
        out
    }

    fn range(&self, lo: Option<&[u8]>, hi: Option<&[u8]>) -> Vec<(Vec<u8>, Vec<RowID>)> {
        use std::ops::Bound;
        let start = match lo {
            Some(k) => Bound::Included(k.to_vec()),
            None => Bound::Unbounded,
        };
        // Inclusive on the upper end: the chunk readers' `range` is, and the
        // planner's bounds are closed intervals.
        let end = match hi {
            Some(k) => Bound::Included(k.to_vec()),
            None => Bound::Unbounded,
        };
        // Same lock order as `lookup`: `pending` first, so a fold cannot move
        // entries between the two views.
        let pending = self.pending.lock().unwrap();
        let snapshot = self.snapshot();
        let mut out: Map = snapshot
            .range((start.clone(), end.clone()))
            .map(|(key, ids)| (key.clone(), ids.clone()))
            .collect();
        for (key, ids) in pending.range((start, end)) {
            match out.get_mut(key) {
                Some(existing) => existing.extend_from_slice(ids),
                None => {
                    out.insert(key.clone(), ids.clone());
                }
            }
        }
        out.into_iter().collect()
    }

    fn key_count(&self) -> usize {
        let pending = self.pending.lock().unwrap();
        let snapshot = self.snapshot();
        let fresh = pending
            .keys()
            .filter(|key| !snapshot.contains_key(*key))
            .count();
        snapshot.len() + fresh
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::tempdir;

    #[test]
    fn insert_lookup() {
        let idx = SecondaryIndex::new();
        idx.insert(b"a", 1);
        idx.insert(b"a", 2);
        idx.insert(b"b", 3);
        assert_eq!(idx.lookup(b"a"), vec![1, 2]);
        assert_eq!(idx.lookup(b"b"), vec![3]);
        assert!(idx.lookup(b"z").is_empty());
        assert_eq!(idx.key_count(), 2);
        assert_eq!(idx.row_count(), 3);
    }

    #[test]
    fn range_scan() {
        let idx = SecondaryIndex::new();
        for (k, id) in [(b"a", 1u64), (b"b", 2), (b"c", 3), (b"d", 4)] {
            idx.insert(k, id);
        }
        let got: Vec<Vec<u8>> = idx
            .range(Some(b"b"), Some(b"c"))
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
        let idx = SecondaryIndex::new();
        idx.insert(b"x", 10);
        idx.insert(b"x", 11);
        idx.insert(b"y", 12);
        idx.flush_to(&path).unwrap();

        let loaded = SecondaryIndex::load(&path).unwrap();
        assert_eq!(loaded.lookup(b"x"), vec![10, 11]);
        assert_eq!(loaded.lookup(b"y"), vec![12]);
        assert_eq!(loaded.key_count(), 2);
    }

    /// The walk reports every key once, in ascending key order, with all of its
    /// row ids (C++ `RcuMultiMap::range` walks an ordered multimap).
    #[test]
    fn for_each_in_order_walks_keys_and_row_ids_once() {
        let idx = SecondaryIndex::new();
        for (k, id) in [("c", 3u64), ("a", 1), ("b", 2), ("b", 4)] {
            idx.insert(k.as_bytes(), id);
        }
        let mut seen: Vec<(String, Vec<RowID>)> = Vec::new();
        idx.for_each_in_order(|key, ids| {
            seen.push((String::from_utf8(key.to_vec()).unwrap(), ids.to_vec()))
        });
        // No sort: the walk itself is ordered.
        assert_eq!(
            seen,
            vec![
                ("a".to_string(), vec![1]),
                ("b".to_string(), vec![2, 4]),
                ("c".to_string(), vec![3]),
            ]
        );
        assert_eq!(idx.row_count(), 4);
    }

    /// Keys are order-preserving encodings, so a numeric range is a real
    /// `BTreeMap::range` and not a walk that decodes every key: `10 > 9` here,
    /// while their *display texts* sort the other way round.
    #[test]
    fn range_slices_order_preserving_keys() {
        let idx = SecondaryIndex::new();
        for value in [9u64, 10, 11] {
            idx.insert(&value.to_be_bytes(), value);
        }
        let got: Vec<u64> = idx
            .range(Some(&10u64.to_be_bytes()), Some(&11u64.to_be_bytes()))
            .into_iter()
            .map(|(key, _)| u64::from_be_bytes(key.try_into().unwrap()))
            .collect();
        assert_eq!(got, vec![10, 11], "numeric order, not text order");
        // A closed interval on both ends.
        let only_10 = idx.range(Some(&10u64.to_be_bytes()), Some(&10u64.to_be_bytes()));
        assert_eq!(only_10.len(), 1);
    }

    /// The tail is an `RwLock`ed map: readers do not exclude each other and a
    /// writer appending a key cannot lose a concurrent reader's rows.
    #[test]
    fn concurrent_insert_and_read() {
        use std::sync::Arc;
        let idx = Arc::new(SecondaryIndex::new());
        let mut handles = Vec::new();
        for t in 0..4u64 {
            let idx = Arc::clone(&idx);
            handles.push(std::thread::spawn(move || {
                for i in 0..500u64 {
                    idx.insert(&(t * 1000 + i).to_be_bytes(), t * 1000 + i);
                }
            }));
        }
        let reader = {
            let idx = Arc::clone(&idx);
            std::thread::spawn(move || {
                let mut seen = 0usize;
                for _ in 0..200 {
                    seen += idx
                        .range(None, None)
                        .iter()
                        .map(|(_, ids)| ids.len())
                        .sum::<usize>();
                }
                seen
            })
        };
        for handle in handles {
            handle.join().unwrap();
        }
        reader.join().unwrap();
        assert_eq!(idx.row_count(), 2000);
    }

    /// A row written after the last fold must not be hidden by a hit in the
    /// published snapshot - the reference drops it (see the type docs), and a
    /// secondary index that loses rows answers a query with a wrong result.
    #[test]
    fn fold_keeps_rows_written_after_the_snapshot() {
        let idx = SecondaryIndex::new();
        idx.insert(b"k", 1);
        idx.fold();
        idx.insert(b"k", 2);
        assert_eq!(idx.lookup(b"k"), vec![1, 2]);
        assert_eq!(
            idx.range(Some(b"k"), Some(b"k")),
            vec![(b"k".to_vec(), vec![1, 2])]
        );
        let mut seen: Vec<(Vec<u8>, Vec<RowID>)> = Vec::new();
        idx.for_each_in_order(|key, ids| seen.push((key.to_vec(), ids.to_vec())));
        assert_eq!(seen, vec![(b"k".to_vec(), vec![1, 2])]);
    }

    /// The pending map is folded once it is as large as the snapshot, and the
    /// fold is invisible to readers: every row stays reachable, and the
    /// snapshot actually grew (otherwise the copy is repeated forever).
    #[test]
    fn fold_fires_once_pending_matches_the_snapshot() {
        let idx = SecondaryIndex::new();
        for i in 0..(FOLD_MIN_KEYS * 2) as u64 {
            idx.insert(&i.to_be_bytes(), i);
        }
        assert_eq!(idx.row_count(), FOLD_MIN_KEYS * 2);
        assert_eq!(idx.range(None, None).len(), FOLD_MIN_KEYS * 2);
        assert_eq!(idx.key_count(), FOLD_MIN_KEYS * 2);
        assert!(
            idx.published_keys.load(AtomicOrdering::Relaxed) >= FOLD_MIN_KEYS,
            "the snapshot should hold the first fold's keys"
        );
        // A key from the snapshot and one from the pending map read alike.
        assert_eq!(idx.lookup(&0u64.to_be_bytes()), vec![0]);
        assert_eq!(
            idx.lookup(&((FOLD_MIN_KEYS * 2 - 1) as u64).to_be_bytes()),
            vec![(FOLD_MIN_KEYS * 2 - 1) as u64]
        );
    }

    /// `Clone` takes a snapshot; mutating the clone leaves the original alone.
    #[test]
    fn clone_is_independent() {
        let idx = SecondaryIndex::new();
        idx.insert(b"a", 1);
        let copy = idx.clone();
        copy.insert(b"a", 2);
        assert_eq!(idx.lookup(b"a"), vec![1]);
        assert_eq!(copy.lookup(b"a"), vec![1, 2]);
    }
}
