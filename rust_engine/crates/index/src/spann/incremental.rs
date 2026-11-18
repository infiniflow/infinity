//! The incremental index: a mutable partition tree over [`SpannMutStore`].
//!
//! This is the replacement for the `index_delta_mode = rebuild` path. The v2
//! path made every dump rebuild a whole segment's tree; here a write touches
//! exactly one partition:
//!
//! * **insert** — descend to a leaf, encode against its (frozen) centroid,
//!   append one member row and one row→partition row, atomic batch;
//! * **delete** — look the row up in the mapping, remove one member row;
//! * **split** — re-cluster one leaf's ≤ `max_partition_size` rows into two
//!   new leaves and re-point the parent, one atomic batch;
//! * **merge** — the inverse, for two under-filled siblings.
//!
//! The cost of a write is therefore `O(depth)` metadata reads plus one small
//! batch, and the cost of a split is that one leaf's rows — never the segment.
//!
//! **Centroid freeze** is what makes appends O(1) (`partition_metadata.go:324`:
//! "calculated once when the partition is created and never changes, even if
//! additional vectors are added later"). A code is relative to its leaf's
//! centroid, so updating the centroid on every insert would invalidate every
//! existing member. Drift is corrected by the split, which is the operation
//! that recomputes centroids anyway.

use std::collections::{BTreeMap, HashMap, HashSet};
use std::sync::atomic::{AtomicU32, AtomicUsize, Ordering};
use std::sync::{Arc, Mutex, RwLock};

use crate::vector::{l2_sq, Metric};

use super::kmeans::balanced_kmeans_2;
use super::kv::{MetaValue, PartitionState};
use super::mut_store::{KvOp, SpannMutStore};
use super::partition::{metric_score, Member};
use super::quantize::{self, Rotation};
use super::search::{self, Beam, SearchSet};
use super::{Encode, Options, PartitionKey, SpannError, ROOT_KEY};

/// Fetches a row's raw vector. Splits and merges need it; inserts and deletes
/// do not (a delete finds its leaf through the row mapping).
pub type FetchVector<'a> = &'a dyn Fn(u64) -> Option<Vec<f32>>;

/// Leaves scored on one thread below this; above it the probe is sliced across
/// threads.
const PROBE_PARALLEL_MIN_LEAVES: usize = 64;

/// Slices a probe is cut into. Fixed, and merged in order, so the returned pool
/// does not depend on the core count.
const PROBE_CHUNKS: usize = 8;

/// The tree and its store.
pub struct IncrementalIndex<S: SpannMutStore> {
    store: S,
    opts: Options,
    rotation: Rotation,
    /// Live rows, maintained by `insert`/`delete` so the executor's coverage
    /// check does not have to walk every partition on each query (the walk is
    /// `O(partitions)`, and `all_covered` runs per k-NN).
    rows: AtomicUsize,
    /// A branch's children, as the descent needs them: `(key, radius, centroid)`
    /// per child.
    ///
    /// `find_leaf` runs **once per row**, and reading the root's children from
    /// the store per row was 64 metadata rows of `dims × 4` bytes - 384 KB at
    /// 1536d, per row. Branches are one per `max_partition_size / 2` rows and
    /// change only on split/merge, which is where this is invalidated.
    branches: RwLock<HashMap<PartitionKey, Arc<Branch>>>,
    /// Leaves a write touched since the last [`IncrementalIndex::maintain`], so
    /// the fixup pass starts where the writes happened instead of walking every
    /// partition of the tree.
    dirty: Mutex<Vec<PartitionKey>>,
    /// Where `maintain`'s background sweep resumes: the reference's fixup
    /// worker visits partitions in a rolling order rather than rescanning the
    /// whole tree per batch (`fixup_worker.go`, `pacer.go`).
    sweep: AtomicU32,
}

/// One branch's children, resolved once.
struct Branch {
    children: Vec<(PartitionKey, f32, Vec<f32>)>,
}

/// What one [`IncrementalIndex::maintain`] pass did, for `SHOW INDEX` and the
/// pacer.
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub struct MaintainReport {
    pub splits: usize,
    pub merges: usize,
    pub emptied: usize,
}

/// A snapshot of the tree's shape and row distribution.
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub struct TreeStats {
    pub partitions: usize,
    pub leaves: usize,
    pub rows: usize,
    pub height: u32,
    pub max_leaf: usize,
    pub min_leaf: usize,
}

impl<S: SpannMutStore> IncrementalIndex<S> {
    /// Opens the tree, creating an empty root leaf if the store has none.
    pub fn open(store: S, opts: Options) -> Result<Self, SpannError> {
        opts.validate()?;
        let dims = store.dims();
        let rotation = Rotation::new(dims, opts.seed);
        let index = Self {
            store,
            opts,
            rotation,
            rows: AtomicUsize::new(0),
            branches: RwLock::new(HashMap::new()),
            dirty: Mutex::new(Vec::new()),
            sweep: AtomicU32::new(0),
        };
        if index.store.get_meta(ROOT_KEY)?.is_none() {
            let mut root = MetaValue::new(1, ROOT_KEY, vec![0.0; dims]);
            root.radius = 0.0;
            index.store.commit(&[
                KvOp::PutMeta {
                    key: ROOT_KEY,
                    meta: root,
                },
                KvOp::SetCounter(index.store.counter()?.max(1)),
            ])?;
        }
        // The persisted counter first: `stats()` walks every partition's
        // member rows, which on a 100k-row tree is seconds of cold RocksDB
        // reads on the first query after a restart. The walk is only the
        // migration path for a store written before the counter existed.
        let rows = match index.store.stat("rows")? {
            Some(rows) => rows as usize,
            None => {
                let rows = index.stats()?.rows;
                index.store.commit(&[KvOp::SetStat {
                    name: "rows".to_string(),
                    value: rows as u64,
                }])?;
                rows
            }
        };
        index.rows.store(rows, std::sync::atomic::Ordering::Relaxed);
        Ok(index)
    }

    /// Live rows the tree holds. Maintained incrementally, so this is O(1).
    pub fn row_count(&self) -> usize {
        self.rows.load(std::sync::atomic::Ordering::Relaxed)
    }

    pub fn store(&self) -> &S {
        &self.store
    }

    pub fn opts(&self) -> &Options {
        &self.opts
    }

    pub fn rotation(&self) -> &Rotation {
        &self.rotation
    }

    pub fn dims(&self) -> usize {
        self.store.dims()
    }

    pub fn height(&self) -> Result<u32, SpannError> {
        Ok(self
            .store
            .get_meta(ROOT_KEY)?
            .map(|meta| meta.level)
            .unwrap_or(0))
    }

    // ---------------------------------------------------------------- search

    /// Approximate k-NN. Scores are the code estimates; the caller re-scores
    /// the candidates exactly, as `SPANN_DESIGN.md` §8.6 requires.
    pub fn search(
        &self,
        q: &[f32],
        k: usize,
        beam: Option<usize>,
    ) -> Result<Vec<(u64, f32)>, SpannError> {
        self.search_masked(q, k, beam, None)
    }

    /// [`Self::search`] with the statement's filter bitmap, tested per slot.
    pub fn search_filtered(
        &self,
        q: &[f32],
        k: usize,
        beam: Option<usize>,
        allowed: &HashSet<u64>,
    ) -> Result<Vec<(u64, f32)>, SpannError> {
        self.search_masked(q, k, beam, Some(allowed))
    }

    fn search_masked(
        &self,
        q: &[f32],
        k: usize,
        beam: Option<usize>,
        allowed: Option<&HashSet<u64>>,
    ) -> Result<Vec<(u64, f32)>, SpannError> {
        if k == 0 || q.len() != self.dims() {
            return Ok(Vec::new());
        }
        let Some(root) = self.store.get_meta(ROOT_KEY)? else {
            return Ok(Vec::new());
        };
        let width = beam.unwrap_or(self.opts.base_beam).max(1);
        let mut level = root.level;
        let mut current: Vec<(f32, PartitionKey)> = vec![(0.0, ROOT_KEY)];
        // Leaves met above the bottom level: a split by row count can leave a
        // shallow branch, and dropping those rows is the bug `mixed_depth`
        // guards (see `mod.rs`'s `search_masked`).
        let mut shallow: Vec<(f32, PartitionKey)> = Vec::new();
        loop {
            let mut next = Beam::new(search::level_width(width, level.saturating_sub(1)));
            let mut descended = false;
            for (bound, key) in &current {
                let Some(meta) = self.store.get_meta(*key)? else {
                    continue;
                };
                if meta.is_leaf() {
                    shallow.push((*bound, *key));
                    continue;
                }
                descended = true;
                let children = self.store.member_ids(*key)?;
                let keys: Vec<PartitionKey> = children
                    .iter()
                    .map(|child| *child as PartitionKey)
                    .collect();
                let metas = self.store.get_metas(&keys)?;
                for (child, child_meta) in keys.iter().zip(metas.iter()) {
                    if let Some(child_meta) = child_meta {
                        next.push(meta_lower_bound(child_meta, q, self.opts.metric), *child);
                    }
                }
            }
            if !descended {
                break;
            }
            current = next.into_items();
            if current.is_empty() {
                break;
            }
            level = level.saturating_sub(1).max(2);
        }
        current.extend(shallow);
        current.sort_by(|a, b| a.0.total_cmp(&b.0).then(a.1.cmp(&b.1)));

        // The leaves are independent reads and the scoring is per-member, so the
        // probe runs in slices. At 2M rows the default width is 512 leaves of
        // 128 members each, and one thread scored them at ~0.2 ms per leaf -
        // which was the whole 1.1 s a query cost. A slice prunes against its own
        // k-th best rather than the merged one, so it can only examine more, never
        // score fewer.
        let leaves: Vec<(f32, PartitionKey)> = current
            .into_iter()
            .filter(
                |(_, key)| matches!(self.store.get_meta(*key), Ok(Some(meta)) if meta.is_leaf()),
            )
            .collect();
        let score = |range: std::ops::Range<usize>| -> Result<SearchSet, SpannError> {
            let mut set = SearchSet::with_default_capacity(k);
            for (bound, key) in &leaves[range] {
                let Some(meta) = self.store.get_meta(*key)? else {
                    continue;
                };
                if self.opts.metric.is_l2() {
                    if let Some(kth) = set.kth_upper() {
                        if !search::l2_can_beat(*bound, kth) {
                            break;
                        }
                    }
                }
                let rot_centroid = self.rotation.apply(&meta.centroid);
                let query = quantize::make_query(&self.rotation, &rot_centroid, q);
                for member in self.store.get_members(*key)? {
                    if let Some(allowed) = allowed {
                        if !allowed.contains(&member.id) {
                            continue;
                        }
                    }
                    let squared = quantize::distance_estimate(&query, &member.code, self.dims());
                    let err = quantize::error_bound(&query, &member.code);
                    let (score, score_err) = metric_score(
                        self.opts.metric,
                        squared,
                        err,
                        query.raw_norm,
                        member.code.raw_norm,
                    );
                    set.add(score, score_err, member.id);
                }
            }
            Ok(set)
        };
        let parts = if leaves.len() < PROBE_PARALLEL_MIN_LEAVES {
            vec![score(0..leaves.len())]
        } else {
            let per = leaves.len().div_ceil(PROBE_CHUNKS).max(1);
            std::thread::scope(|scope| {
                let handles: Vec<_> = (0..leaves.len())
                    .step_by(per)
                    .map(|from| {
                        let to = (from + per).min(leaves.len());
                        let score = &score;
                        scope.spawn(move || score(from..to))
                    })
                    .collect();
                handles
                    .into_iter()
                    .map(|handle| match handle.join() {
                        Ok(result) => result,
                        Err(payload) => std::panic::resume_unwind(payload),
                    })
                    .collect::<Vec<_>>()
            })
        };
        // Merged in slice order, so the pool is the same on every run. The
        // scores are final here (each carries its own error correction), so the
        // merge only orders them.
        let mut set = SearchSet::with_default_capacity(k);
        for part in parts {
            for (id, score) in part?.into_sorted() {
                set.add(score, 0.0, id);
            }
        }
        Ok(set.into_sorted())
    }

    // ---------------------------------------------------------------- writes

    /// Inserts `rows`, then splits any leaf that overflowed.
    ///
    /// `fetch` supplies the raw vectors a split needs; it is only called when a
    /// leaf actually overflowed, so the common batch never pays for it.
    pub fn insert(&self, rows: &[(u64, &[f32])], fetch: FetchVector<'_>) -> Result<(), SpannError> {
        if rows.is_empty() {
            return Ok(());
        }
        let dims = self.dims();
        // Descend first, then read each leaf's metadata **once**: a leaf's
        // centroid is `dims × 4` bytes (6 KB at 1536d) and a batch of 500 rows
        // lands in a handful of leaves, so the old per-row read was the batch's
        // largest fixed cost.
        let mut targets: BTreeMap<PartitionKey, Vec<usize>> = BTreeMap::new();
        let mut added = 0usize;
        for (at, (row, vector)) in rows.iter().enumerate() {
            if vector.len() != dims {
                return Err(SpannError::DimensionMismatch {
                    expected: dims,
                    actual: vector.len(),
                });
            }
            if self.store.row_partition(*row)?.is_none() {
                added += 1;
            }
            let leaf = self.find_leaf(vector)?;
            targets.entry(leaf).or_default().push(at);
        }
        let mut groups: BTreeMap<PartitionKey, Vec<Member>> = BTreeMap::new();
        let mut ops = Vec::with_capacity(rows.len() * 2 + 1);
        for (leaf, at) in &targets {
            let Some(meta) = self.store.get_meta(*leaf)? else {
                continue;
            };
            // The old row keeps its mapping only until this batch overwrites
            // it; an UPDATE's insert half lands on the same key.
            let rot_centroid = self.rotation.apply(&meta.centroid);
            for at in at {
                let (row, vector) = rows[*at];
                let code = quantize::encode(&self.rotation, &rot_centroid, vector);
                let member = Member { id: row, code };
                ops.push(KvOp::PutMember {
                    partition: *leaf,
                    member: member.clone(),
                });
                ops.push(KvOp::PutRowMap {
                    row,
                    partition: *leaf,
                });
                groups.entry(*leaf).or_default().push(member);
            }
        }
        let next_rows = self.rows.load(std::sync::atomic::Ordering::Relaxed) + added;
        ops.push(KvOp::SetStat {
            name: "rows".to_string(),
            value: next_rows as u64,
        });
        self.store.commit(&ops)?;
        self.rows
            .store(next_rows, std::sync::atomic::Ordering::Relaxed);
        // The fixup pass starts at the leaves this batch wrote.
        self.mark_dirty(groups.keys().copied());

        // A split repeats: one leaf can take more than one batch (a `COPY`
        // lands 600 rows in the root before the first split runs), so the loop
        // drains every overflowing leaf, including the ones a split just made.
        for leaf in groups.keys() {
            let mut pending = vec![*leaf];
            while let Some(key) = pending.pop() {
                if self.store.member_count(key)? > self.opts.max_partition_size {
                    if let Some((left, right)) = self.split(key, fetch)? {
                        pending.push(left);
                        pending.push(right);
                    }
                }
            }
        }
        Ok(())
    }

    /// Removes `row` from its leaf. `false` when the mapping holds no such row
    /// (already deleted, or written before this index existed).
    pub fn delete(&self, row: u64) -> Result<bool, SpannError> {
        let Some(leaf) = self.store.row_partition(row)? else {
            return Ok(false);
        };
        if !self.store.member_ids(leaf)?.contains(&row) {
            // The mapping is stale (a split moved the row and its batch lost
            // the race): fall back to clearing it so the next delete is clean.
            self.store.commit(&[KvOp::DeleteRowMap { row }])?;
            return Ok(false);
        }
        self.store.commit(&[
            KvOp::DeleteMember {
                partition: leaf,
                child: row,
            },
            KvOp::DeleteRowMap { row },
            KvOp::SetStat {
                name: "rows".to_string(),
                value: self
                    .rows
                    .load(std::sync::atomic::Ordering::Relaxed)
                    .saturating_sub(1) as u64,
            },
        ])?;
        self.rows.fetch_sub(1, std::sync::atomic::Ordering::Relaxed);
        Ok(true)
    }

    /// Deletes a batch of rows in one commit.
    pub fn delete_batch(&self, rows: &[u64]) -> Result<usize, SpannError> {
        if rows.is_empty() {
            return Ok(0);
        }
        let mut ops = Vec::with_capacity(rows.len() * 2);
        let mut removed = 0usize;
        for row in rows {
            let Some(leaf) = self.store.row_partition(*row)? else {
                continue;
            };
            if !self.store.member_ids(leaf)?.contains(row) {
                ops.push(KvOp::DeleteRowMap { row: *row });
                continue;
            }
            ops.push(KvOp::DeleteMember {
                partition: leaf,
                child: *row,
            });
            ops.push(KvOp::DeleteRowMap { row: *row });
            removed += 1;
        }
        if removed > 0 {
            ops.push(KvOp::SetStat {
                name: "rows".to_string(),
                value: self
                    .rows
                    .load(std::sync::atomic::Ordering::Relaxed)
                    .saturating_sub(removed) as u64,
            });
        }
        self.store.commit(&ops)?;
        if removed > 0 {
            self.rows
                .fetch_sub(removed, std::sync::atomic::Ordering::Relaxed);
        }
        Ok(removed)
    }

    // -------------------------------------------------------------- fixups

    /// Splits overflowing leaves, merges under-filled siblings and drops empty
    /// leaves. Bounded by `budget` so a pacer can spread it over time, exactly
    /// like the reference's fixup worker (`fixup_worker.go`).
    pub fn maintain(
        &self,
        fetch: FetchVector<'_>,
        budget: usize,
    ) -> Result<MaintainReport, SpannError> {
        let mut report = MaintainReport::default();
        if budget == 0 {
            return Ok(report);
        }
        let mut keys = self.store.partition_keys()?;
        keys.sort_unstable();
        // Overflowing leaves a split creates must be revisited in the same
        // pass: a 600-row root splits into 300/300, which each still overflow.
        //
        // Where the pass *starts* is what made a flush cost `O(tree)` before: it
        // examined every partition of the tree on every batch, so an index with
        // 32k partitions paid 64k metadata reads to write 500 rows. Now it starts
        // at the leaves the batch wrote and continues from a rolling cursor, so
        // the whole tree is still swept - just over many batches, which is what
        // the reference's pacer does (`pacer.go`).
        let dirty: Vec<PartitionKey> = {
            let mut dirty = self.dirty.lock().unwrap();
            std::mem::take(&mut *dirty)
        };
        let start = if keys.is_empty() {
            0
        } else {
            self.sweep.load(Ordering::Relaxed) as usize % keys.len()
        };
        let window = budget.saturating_mul(4).max(1);
        let mut queue: Vec<PartitionKey> = dirty;
        if !keys.is_empty() {
            queue.extend(
                keys.iter()
                    .cycle()
                    .skip(start)
                    .take(window.min(keys.len()))
                    .copied(),
            );
            let next = (start + window) % keys.len();
            self.sweep.store(next as u32, Ordering::Relaxed);
        }
        let mut spent = 0usize;
        while let Some(key) = queue.pop() {
            if spent >= budget {
                break;
            }
            let Some(meta) = self.store.get_meta(key)? else {
                continue;
            };
            if !meta.is_leaf() {
                continue;
            }
            let count = self.store.member_count(key)?;
            if key != ROOT_KEY && count == 0 {
                self.drop_leaf(key)?;
                report.emptied += 1;
                spent += 1;
                continue;
            }
            if count > self.opts.max_partition_size {
                if let Some((left, right)) = self.split(key, fetch)? {
                    queue.push(left);
                    queue.push(right);
                    report.splits += 1;
                    spent += 1;
                }
                continue;
            }
            if key != ROOT_KEY && count * 2 < self.opts.min_partition_size {
                if let Some(sibling) = self.underfilled_sibling(key, count)? {
                    if let Some((merged, _)) = self.merge(key, sibling, fetch)? {
                        queue.push(merged);
                        report.merges += 1;
                        spent += 1;
                    }
                }
            }
        }
        Ok(report)
    }

    /// The tree's shape, for `SHOW INDEX` and the tests' invariants.
    pub fn stats(&self) -> Result<TreeStats, SpannError> {
        let mut stats = TreeStats {
            height: self.height()?,
            ..TreeStats::default()
        };
        let mut max_leaf = 0usize;
        let mut min_leaf = usize::MAX;
        for key in self.store.partition_keys()? {
            let Some(meta) = self.store.get_meta(key)? else {
                continue;
            };
            stats.partitions += 1;
            if meta.is_leaf() {
                stats.leaves += 1;
                let count = self.store.member_count(key)?;
                stats.rows += count;
                max_leaf = max_leaf.max(count);
                min_leaf = min_leaf.min(count);
            }
        }
        stats.max_leaf = max_leaf;
        stats.min_leaf = if min_leaf == usize::MAX { 0 } else { min_leaf };
        Ok(stats)
    }

    /// Every row id the tree holds, ascending. The conformance tests use this
    /// where the old coverage check used `row_ids()`.
    pub fn row_ids(&self) -> Result<Vec<u64>, SpannError> {
        let mut ids = Vec::new();
        for key in self.store.partition_keys()? {
            let Some(meta) = self.store.get_meta(key)? else {
                continue;
            };
            if meta.is_leaf() {
                ids.extend(self.store.member_ids(key)?);
            }
        }
        ids.sort_unstable();
        Ok(ids)
    }

    // ------------------------------------------------------------- internals

    /// Greedy descent to the leaf a new vector belongs in.
    ///
    /// A branch's children come from [`Self::branch`], which caches them: the
    /// descent runs once per row, and re-reading the path from the store per row
    /// was the whole cost of an incremental insert at 1536d.
    fn find_leaf(&self, vector: &[f32]) -> Result<PartitionKey, SpannError> {
        let mut key = ROOT_KEY;
        loop {
            let Some(branch) = self.branch(key)? else {
                // A leaf, a missing partition, or a branch with no children: the
                // row belongs here.
                return Ok(key);
            };
            let mut best: Option<(f32, PartitionKey)> = None;
            for (child, radius, centroid) in &branch.children {
                let bound = lower_bound(centroid, *radius, vector, self.opts.metric);
                if best.map(|(b, _)| bound < b).unwrap_or(true) {
                    best = Some((bound, *child));
                }
            }
            match best {
                Some((_, next)) => key = next,
                None => return Ok(key),
            }
        }
    }

    /// The children of `key`, cached.
    ///
    /// `None` means "not a branch": a leaf, a partition that is gone, or one
    /// whose children are all gone. Every caller treats those the same way -
    /// the descent stops, the search treats the key as a leaf.
    fn branch(&self, key: PartitionKey) -> Result<Option<Arc<Branch>>, SpannError> {
        if let Some(hit) = self.branches.read().unwrap().get(&key) {
            return Ok(Some(Arc::clone(hit)));
        }
        let Some(meta) = self.store.get_meta(key)? else {
            return Ok(None);
        };
        if meta.is_leaf() {
            return Ok(None);
        }
        let children = self.store.member_ids(key)?;
        if children.is_empty() {
            return Ok(None);
        }
        let keys: Vec<PartitionKey> = children.iter().map(|c| *c as PartitionKey).collect();
        let metas = self.store.get_metas(&keys)?;
        let entries: Vec<(PartitionKey, f32, Vec<f32>)> = keys
            .iter()
            .zip(metas.iter())
            .filter_map(|(child, meta)| {
                meta.as_ref()
                    .map(|meta| (*child, meta.radius, meta.centroid.clone()))
            })
            .collect();
        let branch = Arc::new(Branch { children: entries });
        self.branches
            .write()
            .unwrap()
            .insert(key, Arc::clone(&branch));
        Ok(Some(branch))
    }

    /// Drops cached branches. Called wherever a child's metadata changes: a
    /// split rewrites the parent's member rows, a merge deletes them, and a new
    /// level adds children the cache has never seen.
    fn forget_branches(&self, keys: impl IntoIterator<Item = PartitionKey>) {
        let mut cache = self.branches.write().unwrap();
        for key in keys {
            cache.remove(&key);
        }
    }

    /// Records the leaves a write touched, for the next [`Self::maintain`].
    fn mark_dirty(&self, keys: impl IntoIterator<Item = PartitionKey>) {
        let mut dirty = self.dirty.lock().unwrap();
        for key in keys {
            if !dirty.contains(&key) {
                dirty.push(key);
            }
        }
    }

    /// Splits `leaf` into two new leaves, returning their keys. `None` when the
    /// leaf fits or when a vector cannot be fetched (the split is then
    /// deferred, never performed on partial data).
    fn split(
        &self,
        leaf: PartitionKey,
        fetch: FetchVector<'_>,
    ) -> Result<Option<(PartitionKey, PartitionKey)>, SpannError> {
        let Some(meta) = self.store.get_meta(leaf)? else {
            return Ok(None);
        };
        let members = self.store.get_members(leaf)?;
        if members.len() <= self.opts.max_partition_size || members.len() < 4 {
            return Ok(None);
        }
        let mut ids = Vec::with_capacity(members.len());
        let mut vectors = Vec::with_capacity(members.len());
        for member in &members {
            match fetch(member.id) {
                Some(vector) if vector.len() == self.dims() => {
                    ids.push(member.id);
                    vectors.push(vector);
                }
                // A row whose vector cannot be read must not be silently
                // dropped: leave the leaf whole and try again on a later pass.
                _ => return Ok(None),
            }
        }
        let refs: Vec<&[f32]> = vectors.iter().map(|v| v.as_slice()).collect();
        let split = balanced_kmeans_2(
            &refs,
            self.dims(),
            self.opts.kmeans_iterations,
            self.opts.seed ^ u64::from(leaf),
        );
        let (mut left, mut right) = (Vec::new(), Vec::new());
        for (at, second) in split.second.iter().enumerate() {
            if *second {
                right.push(at);
            } else {
                left.push(at);
            }
        }
        if left.is_empty() || right.is_empty() {
            let mid = ids.len() / 2;
            left.clear();
            right.clear();
            left.extend(0..mid);
            right.extend(mid..ids.len());
        }

        let counter = self.store.counter()?.max(1);
        let (left_key, right_key) = (counter as PartitionKey, (counter + 1) as PartitionKey);
        let left_centroid = split.centroids[0].clone();
        let right_centroid = split.centroids[1].clone();
        let left_radius = radius_of(&vectors, &left, &left_centroid);
        let right_radius = radius_of(&vectors, &right, &right_centroid);

        let mut ops = Vec::with_capacity(ids.len() * 3 + 8);
        ops.push(KvOp::PutMeta {
            key: left_key,
            meta: MetaValue {
                level: 1,
                state: PartitionState::Ready,
                parent: if leaf == ROOT_KEY {
                    ROOT_KEY
                } else {
                    meta.parent
                },
                radius: left_radius,
                centroid: left_centroid.clone(),
                target1: 0,
                target2: 0,
                source: 0,
            },
        });
        ops.push(KvOp::PutMeta {
            key: right_key,
            meta: MetaValue {
                level: 1,
                state: PartitionState::Ready,
                parent: if leaf == ROOT_KEY {
                    ROOT_KEY
                } else {
                    meta.parent
                },
                radius: right_radius,
                centroid: right_centroid.clone(),
                target1: 0,
                target2: 0,
                source: 0,
            },
        });
        self.push_reencoded(&mut ops, left_key, &ids, &vectors, &left, &left_centroid);
        self.push_reencoded(&mut ops, right_key, &ids, &vectors, &right, &right_centroid);

        if leaf == ROOT_KEY {
            // The root was the leaf: it becomes the branch above its two
            // children, and its own member rows are the old rows, which the
            // two new leaves now own.
            for member in &members {
                ops.push(KvOp::DeleteMember {
                    partition: ROOT_KEY,
                    child: member.id,
                });
            }
            let mut root = meta.clone();
            root.level = 2;
            root.state = PartitionState::Ready;
            ops.push(KvOp::PutMeta {
                key: ROOT_KEY,
                meta: root,
            });
            self.push_child_row(&mut ops, ROOT_KEY, &meta.centroid, left_key, &left_centroid);
            self.push_child_row(
                &mut ops,
                ROOT_KEY,
                &meta.centroid,
                right_key,
                &right_centroid,
            );
        } else {
            let parent = meta.parent;
            let Some(parent_meta) = self.store.get_meta(parent)? else {
                return Ok(None);
            };
            ops.push(KvOp::DeletePartition { key: leaf });
            ops.push(KvOp::DeleteMember {
                partition: parent,
                child: u64::from(leaf),
            });
            self.push_child_row(
                &mut ops,
                parent,
                &parent_meta.centroid,
                left_key,
                &left_centroid,
            );
            self.push_child_row(
                &mut ops,
                parent,
                &parent_meta.centroid,
                right_key,
                &right_centroid,
            );
        }
        ops.push(KvOp::SetCounter(counter + 2));
        self.store.commit(&ops)?;
        // The branch above the split child just gained rows: its cached
        // children are stale.
        self.forget_branches([if leaf == ROOT_KEY {
            ROOT_KEY
        } else {
            meta.parent
        }]);
        Ok(Some((left_key, right_key)))
    }

    fn push_reencoded(
        &self,
        ops: &mut Vec<KvOp>,
        partition: PartitionKey,
        ids: &[u64],
        vectors: &[Vec<f32>],
        take: &[usize],
        centroid: &[f32],
    ) {
        let rot_centroid = self.rotation.apply(centroid);
        for at in take {
            let code = quantize::encode(&self.rotation, &rot_centroid, &vectors[*at]);
            ops.push(KvOp::PutMember {
                partition,
                member: Member { id: ids[*at], code },
            });
            ops.push(KvOp::PutRowMap {
                row: ids[*at],
                partition,
            });
        }
    }

    /// Writes a branch's row for a child: the member id is the child key and
    /// the code is the child's **centroid**, encoded against the branch's own
    /// centroid (`Partition::build`'s shape for a branch, `partition.rs:237`).
    fn push_child_row(
        &self,
        ops: &mut Vec<KvOp>,
        branch: PartitionKey,
        branch_centroid: &[f32],
        child: PartitionKey,
        child_centroid: &[f32],
    ) {
        let rot_centroid = self.rotation.apply(branch_centroid);
        let code = quantize::encode(&self.rotation, &rot_centroid, child_centroid);
        ops.push(KvOp::PutMember {
            partition: branch,
            member: Member {
                id: u64::from(child),
                code,
            },
        });
    }

    /// The sibling to merge `leaf` with, if one is small enough that the sum
    /// fits in a single leaf.
    fn underfilled_sibling(
        &self,
        leaf: PartitionKey,
        count: usize,
    ) -> Result<Option<PartitionKey>, SpannError> {
        let Some(meta) = self.store.get_meta(leaf)? else {
            return Ok(None);
        };
        let parent = meta.parent;
        let Some(parent_meta) = self.store.get_meta(parent)? else {
            return Ok(None);
        };
        if !parent_meta.is_leaf() {
            for child in self.store.member_ids(parent)? {
                let child = child as PartitionKey;
                if child == leaf {
                    continue;
                }
                let other = self.store.member_count(child)?;
                if other + count <= self.opts.max_partition_size {
                    return Ok(Some(child));
                }
            }
        }
        Ok(None)
    }

    /// Merges `leaf` and `sibling` into a new leaf and re-points the parent.
    fn merge(
        &self,
        leaf: PartitionKey,
        sibling: PartitionKey,
        fetch: FetchVector<'_>,
    ) -> Result<Option<(PartitionKey, PartitionKey)>, SpannError> {
        let (Some(meta), Some(sibling_meta)) =
            (self.store.get_meta(leaf)?, self.store.get_meta(sibling)?)
        else {
            return Ok(None);
        };
        if meta.parent != sibling_meta.parent || leaf == ROOT_KEY || sibling == ROOT_KEY {
            return Ok(None);
        }
        let mut ids = Vec::new();
        let mut vectors = Vec::new();
        for key in [leaf, sibling] {
            for member in self.store.get_members(key)? {
                match fetch(member.id) {
                    Some(vector) if vector.len() == self.dims() => {
                        ids.push(member.id);
                        vectors.push(vector);
                    }
                    _ => return Ok(None),
                }
            }
        }
        if ids.is_empty() || ids.len() > self.opts.max_partition_size {
            return Ok(None);
        }
        let centroid = mean_of(&vectors, self.dims());
        let radius = max_distance(&vectors, &centroid);
        let counter = self.store.counter()?.max(1);
        let new_key = counter as PartitionKey;
        let parent = meta.parent;
        let Some(parent_meta) = self.store.get_meta(parent)? else {
            return Ok(None);
        };

        let mut ops = Vec::with_capacity(ids.len() * 3 + 8);
        ops.push(KvOp::PutMeta {
            key: new_key,
            meta: MetaValue {
                level: 1,
                state: PartitionState::Ready,
                parent,
                radius,
                centroid: centroid.clone(),
                target1: 0,
                target2: 0,
                source: 0,
            },
        });
        let all: Vec<usize> = (0..ids.len()).collect();
        self.push_reencoded(&mut ops, new_key, &ids, &vectors, &all, &centroid);
        ops.push(KvOp::DeletePartition { key: leaf });
        ops.push(KvOp::DeletePartition { key: sibling });
        for old in [leaf, sibling] {
            ops.push(KvOp::DeleteMember {
                partition: parent,
                child: u64::from(old),
            });
        }
        self.push_child_row(&mut ops, parent, &parent_meta.centroid, new_key, &centroid);
        ops.push(KvOp::SetCounter(counter + 1));
        self.store.commit(&ops)?;
        self.forget_branches([parent]);
        Ok(Some((new_key, parent)))
    }

    /// Drops an empty leaf and its parent's row for it.
    fn drop_leaf(&self, leaf: PartitionKey) -> Result<(), SpannError> {
        let Some(meta) = self.store.get_meta(leaf)? else {
            return Ok(());
        };
        let mut ops = vec![KvOp::DeletePartition { key: leaf }];
        if meta.parent != ROOT_KEY {
            ops.push(KvOp::DeleteMember {
                partition: meta.parent,
                child: u64::from(leaf),
            });
        }
        self.store.commit(&ops)?;
        self.forget_branches([meta.parent]);
        Ok(())
    }
}

fn meta_lower_bound(meta: &MetaValue, q: &[f32], metric: Metric) -> f32 {
    lower_bound(&meta.centroid, meta.radius, q, metric)
}

/// Lower bound on the distance from `q` to anything inside a partition.
fn lower_bound(centroid: &[f32], radius: f32, q: &[f32], metric: Metric) -> f32 {
    let distance = l2_sq(q, centroid).max(0.0).sqrt();
    if metric.is_l2() {
        (distance - radius).max(0.0)
    } else {
        distance
    }
}

fn radius_of(vectors: &[Vec<f32>], take: &[usize], centroid: &[f32]) -> f32 {
    take.iter()
        .map(|at| l2_sq(&vectors[*at], centroid).max(0.0).sqrt())
        .fold(0.0f32, f32::max)
}

fn max_distance(vectors: &[Vec<f32>], centroid: &[f32]) -> f32 {
    vectors
        .iter()
        .map(|v| l2_sq(v, centroid).max(0.0).sqrt())
        .fold(0.0f32, f32::max)
}

fn mean_of(vectors: &[Vec<f32>], dims: usize) -> Vec<f32> {
    let mut out = vec![0.0f32; dims];
    if vectors.is_empty() {
        return out;
    }
    for vector in vectors {
        for (o, x) in out.iter_mut().zip(vector.iter()) {
            *o += *x;
        }
    }
    let n = vectors.len() as f32;
    for o in &mut out {
        *o /= n;
    }
    out
}

/// `Encode` is part of an index's identity; a store built with another encoding
/// cannot be searched by this build.
pub fn check_encoding(store_encoding: Encode, opts: &Options) -> Result<(), SpannError> {
    if store_encoding != opts.encode {
        return Err(SpannError::InvalidOption(
            "encode",
            format!(
                "store was built with {:?}, options say {:?}",
                store_encoding, opts.encode
            ),
        ));
    }
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::spann::mut_store::MemKvStore;

    fn opts(max: usize) -> Options {
        Options {
            metric: Metric::L2,
            max_partition_size: max,
            min_partition_size: max / 4,
            fill_factor: 0.9,
            encode: Encode::Rabitq,
            kmeans_iterations: 6,
            seed: 12345,
            base_beam: 8,
            root_fanout: 8,
            min_rows: 0,
            adaptive_beam: false,
        }
    }

    /// Points on a line, so a split has an unambiguous answer.
    fn rows(n: usize, dims: usize) -> Vec<(u64, Vec<f32>)> {
        (0..n)
            .map(|i| {
                (
                    i as u64,
                    (0..dims).map(|d| (i as f32) * 0.5 + d as f32).collect(),
                )
            })
            .collect()
    }

    fn fetcher<'a>(rows: &'a [(u64, Vec<f32>)]) -> impl Fn(u64) -> Option<Vec<f32>> + 'a {
        move |row| {
            rows.iter()
                .find(|(id, _)| *id == row)
                .map(|(_, v)| v.clone())
        }
    }

    #[test]
    fn every_row_is_reachable_after_splits() {
        let data = rows(600, 4);
        let store = MemKvStore::new(4);
        let index = IncrementalIndex::open(store, opts(32)).unwrap();
        let fetch = fetcher(&data);
        let batch: Vec<(u64, &[f32])> = data.iter().map(|(id, v)| (*id, v.as_slice())).collect();
        index.insert(&batch, &fetch).unwrap();
        let report = index.maintain(&fetch, usize::MAX).unwrap();
        assert!(
            index.stats().unwrap().partitions > 1,
            "the insert must have split the 600-row root (maintain: {report:?})"
        );
        assert!(index.height().unwrap() >= 2, "the tree must have grown");

        let stats = index.stats().unwrap();
        assert_eq!(stats.rows, 600, "a split must not lose a row");
        assert!(stats.max_leaf <= 32, "no leaf may exceed the bound");
        assert_eq!(index.row_ids().unwrap().len(), 600);
    }

    #[test]
    fn search_finds_a_rows_own_leaf() {
        let data = rows(500, 4);
        let store = MemKvStore::new(4);
        let index = IncrementalIndex::open(store, opts(32)).unwrap();
        let fetch = fetcher(&data);
        let batch: Vec<(u64, &[f32])> = data.iter().map(|(id, v)| (*id, v.as_slice())).collect();
        index.insert(&batch, &fetch).unwrap();
        index.maintain(&fetch, usize::MAX).unwrap();

        for probe in [10usize, 250, 499] {
            let hits = index.search(&data[probe].1, 5, Some(64)).unwrap();
            assert!(
                hits.iter().any(|(id, _)| *id == probe as u64),
                "row {probe} not in {hits:?}"
            );
        }
    }

    #[test]
    fn delete_removes_the_row_and_its_mapping() {
        let data = rows(200, 4);
        let store = MemKvStore::new(4);
        let index = IncrementalIndex::open(store, opts(32)).unwrap();
        let fetch = fetcher(&data);
        let batch: Vec<(u64, &[f32])> = data.iter().map(|(id, v)| (*id, v.as_slice())).collect();
        index.insert(&batch, &fetch).unwrap();
        index.maintain(&fetch, usize::MAX).unwrap();

        assert!(index.delete(7).unwrap());
        assert_eq!(index.stats().unwrap().rows, 199);
        assert_eq!(index.store().row_partition(7).unwrap(), None);
        assert!(!index.delete(7).unwrap(), "a second delete is a no-op");
    }

    #[test]
    fn insert_is_o_one_leaf_not_o_the_tree() {
        // The regression guard for the old design: inserting one row must add
        // exactly one member, not rewrite every partition.
        let data = rows(400, 4);
        let store = MemKvStore::new(4);
        let index = IncrementalIndex::open(store, opts(32)).unwrap();
        let fetch = fetcher(&data);
        let batch: Vec<(u64, &[f32])> = data.iter().map(|(id, v)| (*id, v.as_slice())).collect();
        index.insert(&batch, &fetch).unwrap();
        index.maintain(&fetch, usize::MAX).unwrap();

        let before = index.store().stored_rows();
        let extra = [(9_999u64, vec![3.0f32, 3.0, 3.0, 3.0])];
        let extra_batch: Vec<(u64, &[f32])> =
            extra.iter().map(|(id, v)| (*id, v.as_slice())).collect();
        index.insert(&extra_batch, &fetch).unwrap();
        assert_eq!(
            index.store().stored_rows(),
            before + 1,
            "an insert must not rewrite the tree"
        );
    }

    #[test]
    fn merge_folds_two_small_siblings() {
        let data = rows(120, 4);
        let store = MemKvStore::new(4);
        let mut o = opts(32);
        o.min_partition_size = 16;
        let index = IncrementalIndex::open(store, o).unwrap();
        let fetch = fetcher(&data);
        let batch: Vec<(u64, &[f32])> = data.iter().map(|(id, v)| (*id, v.as_slice())).collect();
        index.insert(&batch, &fetch).unwrap();
        index.maintain(&fetch, usize::MAX).unwrap();
        let before = index.stats().unwrap().leaves;

        // Empty two leaves by hand, then let `maintain` fold them.
        for row in 0..40u64 {
            index.delete(row).unwrap();
        }
        let report = index.maintain(&fetch, usize::MAX).unwrap();
        let after = index.stats().unwrap();
        assert!(
            report.merges > 0 || report.emptied > 0,
            "under-filled leaves must be folded: {report:?}"
        );
        assert!(after.leaves <= before, "{after:?}");
        assert_eq!(after.rows, 80);
    }
}
