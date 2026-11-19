//! A **chain of chunks** that together form one tree (`SPANN_DESIGN.md` §5.4,
//! §6.5 without a manifest object: the catalog's chunk records *are* the
//! manifest).
//!
//! A checkpoint no longer rebuilds a segment's tree. It rewrites the partitions
//! that changed - a leaf that took rows, a node whose children changed, the
//! nodes a split created - and publishes them as one more chunk:
//!
//! ```text
//! chunk_7.idx  (newest)  leaf 41 (rewritten), leaf 42/43 (split), node 40
//! chunk_3.idx            the tree as of the last full build
//! ```
//!
//! Resolution is **newest wins per key**: a node key names one partition, and
//! the first layer that mentions it is its current page. The walk therefore
//! starts at the root of the newest layer that holds it and resolves every child
//! through the same table, which is what lets a delta chunk carry dangling
//! edges - its untouched children (and its ancestors) stay in older chunks.
//!
//! Cost of resolution: one binary search over `keys`, plus the page read itself.
//! For 390k partitions the table is ~3 MB, built once at open.

use std::collections::{BTreeSet, HashMap, HashSet};
use std::sync::Arc;

use index::spann::search::{self, SearchSet};
use index::spann::{Member, Options, PartitionKey, SpannError, ROOT_KEY};
use index::vector::Metric;

use super::{FileIndex, TreeMeta};

/// One row of the merged directory: the newest copy of a partition.
#[derive(Clone, Debug, PartialEq)]
pub struct ChainMeta {
    pub key: PartitionKey,
    pub level: u32,
    pub members: usize,
    pub radius: f32,
    pub children: Vec<PartitionKey>,
    pub centroid: Vec<f32>,
}

impl From<TreeMeta> for ChainMeta {
    fn from(meta: TreeMeta) -> Self {
        Self {
            key: meta.key,
            level: meta.level,
            members: meta.members,
            radius: meta.radius,
            children: meta.children,
            centroid: meta.centroid,
        }
    }
}

impl ChainMeta {
    /// The page of `key` inside one layer, as the chain reports it.
    fn read(chunk: &FileIndex, key: PartitionKey) -> Option<Self> {
        let meta = chunk.meta(key)?;
        Some(Self {
            key: meta.key,
            level: meta.level,
            members: meta.count as usize,
            radius: meta.radius,
            children: chunk.children(key),
            centroid: chunk.centroid(key)?,
        })
    }
}

/// What a chain covers, as the coverage check needs it (§9.7).
#[derive(Clone, Debug, Default, PartialEq)]
pub struct Coverage {
    /// Rows reachable from the root.
    pub rows: usize,
    /// Row ids held by more than one reachable leaf: a bug, never a state.
    pub duplicates: Vec<u64>,
    /// Rows the caller asked about that no reachable leaf holds. Filled by
    /// [`ChunkChain::covers`]; empty for a plain [`ChunkChain::coverage`].
    pub missing: Vec<u64>,
    /// Child edges naming a key no layer holds: the chain is incomplete, so the
    /// subtree behind the edge is invisible.
    pub dangling: Vec<PartitionKey>,
}

impl Coverage {
    pub fn is_complete(&self) -> bool {
        self.duplicates.is_empty() && self.missing.is_empty() && self.dangling.is_empty()
    }
}

/// Layers that together answer as one tree.
#[derive(Debug)]
pub struct ChunkChain {
    /// **Newest first.** A key is served by the first layer that names it.
    layers: Vec<Arc<FileIndex>>,
    /// `(key, layer)`, ascending by key. The entry for a duplicate key is the
    /// newest layer, because the layers were walked newest-first while building.
    owners: Vec<(PartitionKey, u32)>,
    root: PartitionKey,
    beam: usize,
}

impl ChunkChain {
    /// Opens the chain. `paths` must be **newest first** - the order the catalog
    /// hands the chunks back in, newest chunk id first.
    pub fn open(paths: &[std::path::PathBuf], opts: &Options) -> Result<Self, SpannError> {
        if paths.is_empty() {
            return Err(SpannError::Corrupt(
                "a chain needs at least one chunk".to_string(),
            ));
        }
        let mut layers = Vec::with_capacity(paths.len());
        for path in paths {
            layers.push(Arc::new(FileIndex::open(path, opts)?));
        }
        Ok(Self::from_layers(layers, opts))
    }

    /// [`Self::open`] over already-mapped chunks.
    pub fn from_layers(layers: Vec<Arc<FileIndex>>, opts: &Options) -> Self {
        debug_assert!(!layers.is_empty(), "callers check before opening");
        let mut owners: Vec<(PartitionKey, u32)> = Vec::new();
        let mut seen: HashSet<PartitionKey> = HashSet::new();
        for (layer, chunk) in layers.iter().enumerate() {
            // Keys, not `tree_meta`: the owner table needs the key set and
            // nothing else, and `tree_meta` copies a `dims`-sized centroid per
            // node - at 1536d that is the difference between 4 B and 6 KB per
            // partition, per layer, per open (a checkpoint opens the chain twice
            // and reloads it once).
            for key in chunk.keys() {
                if seen.insert(key) {
                    owners.push((key, layer as u32));
                }
            }
        }
        owners.sort_by_key(|(key, _)| *key);
        // The root is the smallest key either way (keys are allocated in
        // creation order), but taking it from the table rather than assuming 0
        // keeps a re-keyed chunk readable.
        let root = owners
            .first()
            .map(|(key, _)| *key)
            .filter(|key| *key == ROOT_KEY)
            .or_else(|| {
                // A chunk built by an older writer numbers by DFS position, where
                // the root is still the smallest key.
                owners.first().map(|(key, _)| *key)
            })
            .unwrap_or(ROOT_KEY);
        Self {
            layers,
            owners,
            root,
            beam: opts.base_beam,
        }
    }

    /// Number of chunks in the chain, newest first.
    pub fn layers(&self) -> usize {
        self.layers.len()
    }

    /// The bytes this chain holds in the heap: the owner table plus each
    /// layer's header/rotation. The slot pages are page cache and are not part
    /// of it (`buffer::mapped::mapped_bytes` reports those).
    pub fn heap_bytes(&self) -> usize {
        std::mem::size_of::<Self>()
            + self.owners.capacity() * std::mem::size_of::<(PartitionKey, u32)>()
            + self
                .layers
                .iter()
                .map(|layer| layer.heap_bytes())
                .sum::<usize>()
    }

    /// Drops the resident pages of every layer, returning the bytes released.
    ///
    /// A chain is the only long-lived owner of a segment's chunk mappings, so
    /// this is the call that brings an idle index's RSS back down without
    /// closing it: the pages come back from the file on the next query.
    pub fn release_pages(&self) -> usize {
        self.layers.iter().map(|layer| layer.release_pages()).sum()
    }

    /// The segment every layer was written for.
    pub fn segment(&self) -> u32 {
        self.layers
            .first()
            .map(|layer| layer.segment())
            .unwrap_or(0)
    }

    /// The chain with `layer` as its **newest** chunk.
    ///
    /// What a checkpoint needs: it already holds the chain that is answering, so
    /// the delta must not be assembled by re-opening the older layers from their
    /// paths - each open maps and validates a file, and the base chunk is the
    /// same bytes it validated on the previous checkpoint. The owner table is
    /// rebuilt from the new layer's keys plus the ones it does not name, which
    /// is four bytes per partition.
    pub fn with_newest(&self, layer: Arc<FileIndex>) -> Self {
        let mut owners: Vec<(PartitionKey, u32)> =
            Vec::with_capacity(self.owners.len() + layer.partitions());
        let mut newest: HashSet<PartitionKey> = HashSet::with_capacity(layer.partitions());
        for key in layer.keys() {
            if newest.insert(key) {
                owners.push((key, 0));
            }
        }
        for (key, old) in &self.owners {
            if !newest.contains(key) {
                // The old layers shift down by one.
                owners.push((*key, old + 1));
            }
        }
        owners.sort_by_key(|(key, _)| *key);
        let mut layers = Vec::with_capacity(self.layers.len() + 1);
        layers.push(layer);
        layers.extend(self.layers.iter().cloned());
        let root = owners
            .first()
            .map(|(key, _)| *key)
            .filter(|key| *key == ROOT_KEY)
            .or_else(|| owners.first().map(|(key, _)| *key))
            .unwrap_or(ROOT_KEY);
        Self {
            layers,
            owners,
            root,
            beam: self.beam,
        }
    }

    pub fn root(&self) -> PartitionKey {
        self.root
    }

    pub fn dims(&self) -> usize {
        self.layers.first().map(|layer| layer.dims()).unwrap_or(0)
    }

    pub fn metric(&self) -> Metric {
        self.layers
            .first()
            .map(|layer| layer.metric())
            .unwrap_or(Metric::L2)
    }

    pub fn height(&self) -> u32 {
        self.meta(self.root).map(|meta| meta.level).unwrap_or(0)
    }

    /// Partitions in the merged directory, counting shadowed copies once.
    pub fn partitions(&self) -> usize {
        self.owners.len()
    }

    /// The highest key any layer names. New nodes take keys above it, which is
    /// why a delta never collides with a published page.
    pub fn max_key(&self) -> u64 {
        self.owners
            .last()
            .map(|(key, _)| u64::from(*key))
            .unwrap_or(0)
    }

    fn owner(&self, key: PartitionKey) -> Option<usize> {
        self.owners
            .binary_search_by_key(&key, |(owner, _)| *owner)
            .ok()
            .map(|at| self.owners[at].1 as usize)
    }

    /// The current page of `key`: the newest layer that names it.
    pub fn meta(&self, key: PartitionKey) -> Option<ChainMeta> {
        let layer = self.owner(key)?;
        ChainMeta::read(&self.layers[layer], key)
    }

    pub fn centroid(&self, key: PartitionKey) -> Option<Vec<f32>> {
        let layer = self.owner(key)?;
        self.layers[layer].centroid(key)
    }

    pub fn children(&self, key: PartitionKey) -> Vec<PartitionKey> {
        self.meta(key).map(|meta| meta.children).unwrap_or_default()
    }

    /// A published leaf's rows: `(key) -> members`, the page a split reads back.
    pub fn slot_members(&self, key: PartitionKey) -> Option<Vec<Member>> {
        let layer = self.owner(key)?;
        self.layers[layer].slot_members(key)
    }

    /// The merged directory, ascending by key - what a live tree loads.
    pub fn tree_meta(&self) -> Vec<ChainMeta> {
        self.owners
            .iter()
            .filter_map(|(key, layer)| ChainMeta::read(&self.layers[*layer as usize], *key))
            .collect()
    }

    /// Rows the **reachable** leaves hold (§6.2 section F's role, per chain).
    ///
    /// Reachability, not "every layer's leaves": a leaf a merge retired is still
    /// named by an older chunk, but the parent's current page no longer points
    /// at it, so its rows moved and must not be counted twice.
    pub fn row_ids(&self) -> Vec<u64> {
        let mut ids = Vec::new();
        self.walk_leaves(|key, layer, _meta| {
            if let Some(rows) = self.layers[layer].slot_row_ids(key) {
                ids.extend(rows);
            }
        });
        ids
    }

    /// Rows the directory says are covered - no page read, so this is what a
    /// `SHOW INDEX` or a query's coverage test can afford.
    pub fn len(&self) -> usize {
        let mut rows = 0;
        self.walk_leaves(|_key, _layer, meta| rows += meta.members);
        rows
    }

    pub fn is_empty(&self) -> bool {
        self.owners.is_empty()
    }

    /// Visits every reachable leaf once: `(key, layer, meta)`.
    fn walk_leaves(&self, mut visit: impl FnMut(PartitionKey, usize, ChainMeta)) {
        let mut stack = vec![self.root];
        let mut seen: BTreeSet<PartitionKey> = BTreeSet::new();
        while let Some(key) = stack.pop() {
            if !seen.insert(key) {
                continue;
            }
            let Some(layer) = self.owner(key) else {
                continue;
            };
            let Some(meta) = self.meta(key) else {
                continue;
            };
            if meta.level <= 1 {
                visit(key, layer, meta);
            } else {
                stack.extend(meta.children.iter().copied());
            }
        }
    }

    /// Reachability, duplicate rows and dangling edges - the three ways a delta
    /// can be wrong.
    /// Child edges that land on no layer - the half of [`Self::coverage`] that
    /// needs the directory only.
    ///
    /// `coverage` also reads every reachable leaf's slot page, because it counts
    /// rows and looks for duplicates: O(rows) page misses, which a checkpoint
    /// cannot afford on its own publish path. This is O(partitions) and catches
    /// the failure a truncated delta produces (an edge into a page no chunk
    /// holds), which is the one the publisher has to refuse.
    pub fn dangling(&self) -> Vec<PartitionKey> {
        let mut out = Vec::new();
        for (key, _) in &self.owners {
            let Some(meta) = self.meta(*key) else {
                continue;
            };
            for child in meta.children {
                if self.owner(child).is_none() {
                    out.push(child);
                }
            }
        }
        out.sort_unstable();
        out.dedup();
        out
    }

    pub fn coverage(&self) -> Coverage {
        let mut out = Coverage::default();
        let mut seen: HashMap<u64, PartitionKey> = HashMap::new();
        self.walk_leaves(|key, layer, _meta| {
            let Some(rows) = self.layers[layer].slot_row_ids(key) else {
                return;
            };
            for id in rows {
                out.rows += 1;
                if let Some(other) = seen.insert(id, key) {
                    if other != key {
                        out.duplicates.push(id);
                    }
                }
            }
        });
        out.duplicates.sort_unstable();
        out.duplicates.dedup();
        // Every edge, from every node the chain can reach or name, must land on
        // some layer: an edge to a missing key is what a truncated delta looks
        // like.
        for (key, _) in &self.owners {
            let Some(meta) = self.meta(*key) else {
                continue;
            };
            for child in meta.children {
                if self.owner(child).is_none() {
                    out.dangling.push(child);
                }
            }
        }
        out.dangling.sort_unstable();
        out.dangling.dedup();
        out
    }

    /// [`Self::coverage`] plus "which live rows the tree does **not** hold".
    ///
    /// The caller's `live` is the segment's row set; a delta that dropped a leaf
    /// page shows up here rather than as a silently smaller index.
    pub fn covers(&self, live: &HashSet<u64>) -> Coverage {
        let mut out = self.coverage();
        let mut held: HashSet<u64> = HashSet::new();
        self.walk_leaves(|key, layer, _meta| {
            if let Some(rows) = self.layers[layer].slot_row_ids(key) {
                held.extend(rows);
            }
        });
        out.missing = live.difference(&held).copied().collect();
        out.missing.sort_unstable();
        out
    }

    /// Approximate k-NN over the whole chain. Same best-first walk as
    /// [`FileIndex::search`], with every key resolved through [`Self::meta`].
    pub fn search(&self, q: &[f32], k: usize, beam: Option<usize>) -> Vec<(u64, f32)> {
        self.search_masked(q, k, beam, None)
    }

    pub fn search_filtered(
        &self,
        q: &[f32],
        k: usize,
        beam: Option<usize>,
        allowed: &HashSet<u64>,
    ) -> Vec<(u64, f32)> {
        self.search_masked(q, k, beam, Some(allowed))
    }

    fn search_masked(
        &self,
        q: &[f32],
        k: usize,
        beam: Option<usize>,
        allowed: Option<&HashSet<u64>>,
    ) -> Vec<(u64, f32)> {
        let dims = self.dims();
        if k == 0 || q.len() != dims || self.owners.is_empty() {
            return Vec::new();
        }
        let metric = self.metric();
        let budget = beam.unwrap_or(self.beam).max(1);
        let mut set = SearchSet::with_default_capacity(k);
        let mut queue: std::collections::BinaryHeap<search::Candidate> =
            std::collections::BinaryHeap::new();
        queue.push(search::Candidate {
            bound: 0.0,
            lower: 0.0,
            key: u64::from(self.root),
        });
        let mut scanned = 0usize;
        while let Some(node) = queue.pop() {
            if metric.is_l2() {
                if let Some(kth) = set.kth_upper() {
                    if !search::l2_can_beat(node.lower, kth) {
                        continue;
                    }
                }
            }
            let key = node.key as PartitionKey;
            let Some(meta) = self.meta(key) else {
                continue;
            };
            if meta.level <= 1 {
                if scanned >= budget {
                    break;
                }
                scanned += 1;
                if let Some(layer) = self.owner(key) {
                    self.layers[layer].scan_leaf_into(key, q, allowed, &mut set);
                }
                continue;
            }
            for child in meta.children {
                let Some(centroid) = self.centroid(child) else {
                    continue;
                };
                let radius = self.meta(child).map(|meta| meta.radius).unwrap_or(0.0);
                let distance = index::vector::l2_sq(q, &centroid).max(0.0).sqrt();
                queue.push(search::Candidate {
                    bound: distance,
                    lower: search::radius_lower(distance, radius, metric),
                    key: u64::from(child),
                });
            }
        }
        set.into_sorted()
    }
}
impl index::spann::page_tree::PageSource for ChunkChain {
    fn page(&self, key: u64) -> Option<index::spann::page_tree::PageMeta> {
        let key = PartitionKey::try_from(key).ok()?;
        let meta = self.meta(key)?;
        Some(index::spann::page_tree::PageMeta {
            key: u64::from(meta.key),
            level: meta.level,
            centroid: meta.centroid,
            radius: meta.radius,
            children: meta.children.into_iter().map(u64::from).collect(),
            count: meta.members,
        })
    }

    fn slots(&self, key: u64) -> Option<Vec<Member>> {
        self.slot_members(PartitionKey::try_from(key).ok()?)
    }
}

#[cfg(test)]
mod tests {
    use super::super::{write_dirty, write_to};
    use super::*;
    use index::spann::source::{row_source, Memory};
    use index::spann::{Encode, SpannIndex};

    fn noise(i: usize, d: usize) -> f32 {
        let mut x = (i as u64).wrapping_mul(0x9E37_79B9_7F4A_7C15)
            ^ (d as u64).wrapping_mul(0xC2B2_AE3D_27D4_EB4F);
        x ^= x >> 30;
        x = x.wrapping_mul(0xBF58_476D_1CE4_E5B9);
        x ^= x >> 27;
        ((x >> 40) as f32 / 16_777_216.0) * 2.0 - 1.0
    }

    fn rows(range: std::ops::Range<u64>, dim: usize) -> Vec<(u64, Vec<f32>)> {
        range
            .map(|id| {
                let v: Vec<f32> = (0..dim).map(|d| noise(id as usize, d)).collect();
                (id, v)
            })
            .collect()
    }

    fn opts() -> Options {
        Options {
            max_partition_size: 32,
            min_partition_size: 8,
            encode: Encode::Rabitq,
            kmeans_iterations: 4,
            ..Options::default()
        }
    }

    /// §5.2-b: rows are assigned into the **published** tree - the leaf a
    /// descent picks, not a fresh tree over the new rows - and §5.3/§5.4: a
    /// checkpoint writes only the pages it touched, and the chain still answers
    /// for every row with each row exactly once.
    #[test]
    fn a_delta_over_a_published_tree_keeps_every_row() {
        let dim = 64;
        let all = rows(0..400, dim);
        let base = SpannIndex::build(all[..300].to_vec(), opts()).unwrap();

        let dir = tempfile::tempdir().unwrap();
        let base_path = dir.path().join("chunk_0.idx");
        write_to(&base, &base_path, 0).unwrap();
        let opened = FileIndex::open(&base_path, &opts()).unwrap();

        // The write path's view: pages on demand, the directory never
        // materialised. The base chunk is the page source.
        let base_chain = ChunkChain::from_layers(vec![std::sync::Arc::new(opened)], &opts());
        let mut tree = index::spann::page_tree::PageTree::new(
            dim,
            opts(),
            index::spann::quantize::Rotation::new(dim, opts().seed),
            &base_chain,
            u64::from(base_chain.root()),
            base_chain.max_key() + 1,
            64 << 20,
            512 << 20,
        )
        .unwrap();

        // The split path reads a row's raw vector, so the source has to be
        // wired exactly as the engine wires it.
        let source = Memory::new(all.clone());
        let lookup = row_source(&source);
        for (id, v) in &all[300..] {
            tree.insert(*id, v).unwrap();
            if *id % 8 == 0 {
                tree.run_fixups(Some(&lookup), 4);
            }
        }
        tree.run_fixups(Some(&lookup), usize::MAX);

        let dirty_keys: Vec<PartitionKey> = tree
            .dirty_keys()
            .iter()
            .map(|key| *key as PartitionKey)
            .collect();
        assert!(!dirty_keys.is_empty(), "the 100 rows touched some leaves");

        let delta = tree.to_spann_delta().expect("a dirty set publishes");
        assert_eq!(delta.partitions().len(), dirty_keys.len());
        let delta_path = dir.path().join("chunk_1.idx");
        let bytes = write_dirty(&delta, 0, &dirty_keys).unwrap();
        std::fs::write(&delta_path, &bytes).unwrap();

        // Newest first: the delta shadows the pages it rewrote.
        let chain = ChunkChain::open(&[delta_path, base_path.clone()], &opts()).unwrap();
        assert_eq!(chain.layers(), 2);
        assert!(chain.partitions() > 0);

        let mut ids = chain.row_ids();
        ids.sort_unstable();
        assert_eq!(
            ids,
            (0..400).collect::<Vec<u64>>(),
            "the chain answers for every row exactly once"
        );
        let live_set: HashSet<u64> = (0..400).collect();
        let coverage = chain.covers(&live_set);
        assert!(
            coverage.is_complete(),
            "coverage {coverage:?} after a delta publish"
        );
        assert_eq!(coverage.rows, 400);

        // A row inserted a moment ago is answered by the chain, and the tree the
        // rows were assigned into is the one the chain reports.
        for id in [0u64, 150, 300, 399] {
            let q = &all[id as usize].1;
            let hits = chain.search(q, 5, Some(64));
            assert!(
                hits.iter().any(|(got, _)| *got == id),
                "row {id} missing from the chain answer: {hits:?}"
            );
        }
        assert_eq!(chain.height(), tree.height());
    }

    /// A delta that rewrites only an interior node keeps its children, and the
    /// coverage check reports a row the chain does not hold instead of letting a
    /// query answer a shorter list in silence - §5.4's last step.
    #[test]
    fn coverage_names_the_rows_the_chain_does_not_hold() {
        let dim = 32;
        let all = rows(0..200, dim);
        let base = SpannIndex::build(all, opts()).unwrap();
        let dir = tempfile::tempdir().unwrap();
        let base_path = dir.path().join("chunk_0.idx");
        write_to(&base, &base_path, 0).unwrap();
        let opened = FileIndex::open(&base_path, &opts()).unwrap();

        let meta = opened
            .tree_meta()
            .into_iter()
            .find(|meta| !meta.children.is_empty())
            .expect("a 200-row tree has a branch");
        let rot = index::spann::quantize::Rotation::new(dim, opts().seed);
        let branch = index::spann::partition::Partition::from_decoded(
            meta.key,
            meta.level,
            dim,
            meta.centroid.clone(),
            meta.radius,
            rot.apply(&meta.centroid),
            Vec::new(),
            meta.children.clone(),
        );
        let delta = SpannIndex::from_parts(opts(), dim, rot, vec![branch], meta.level, meta.key);
        let delta_path = dir.path().join("chunk_1.idx");
        std::fs::write(&delta_path, write_dirty(&delta, 0, &[meta.key]).unwrap()).unwrap();

        let chain = ChunkChain::open(&[delta_path, base_path], &opts()).unwrap();
        let mut live_set: HashSet<u64> = (0..200).collect();
        let coverage = chain.covers(&live_set);
        assert!(
            coverage.is_complete(),
            "a rewritten branch that keeps its children loses nothing: {coverage:?}"
        );

        // A row the segment has and the chain does not: exactly what a dropped
        // leaf page looks like from the outside.
        live_set.insert(9999);
        let coverage = chain.covers(&live_set);
        assert_eq!(coverage.missing, vec![9999]);
        assert!(!coverage.is_complete());
    }
}
