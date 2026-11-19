//! Incremental index: one tree whose scope is the **whole index**, not one
//! segment (`SPANN_DESIGN.md` §0.1, `ver = 3`).
//!
//! The bulk builder in [`super::build`] is a top-down recursion: it needs every
//! vector in memory at once and produces a tree that never changes afterwards.
//! Here the shape is the opposite — rows arrive one at a time, each one is
//! routed to a leaf, and a leaf only splits when it overflows. The two share
//! the quantizer, the scoring and the descent rules, so their answers are
//! comparable; they differ in *when* centroids are decided.
//!
//! ## Why this satisfies the three constraints
//!
//! * **Small memory.** A member is `(row_id, Code)`: 208 B at 1536d, versus
//!   6 KB for the raw vector. Nothing keeps `f32` rows except the un-split root,
//!   which is capped at `max_partition_size` rows (~768 KB).
//! * **Cheap merge.** Splitting reads **one** leaf (`max_partition_size` codes,
//!   ~54 KB), k-means it in two, and writes two leaves. There is no "rebuild the
//!   whole tree" step anywhere — that is what made the segment-scoped `ver = 2`
//!   chunk cost ~39 s per 8.4 M rows.
//! * **Fast query.** One descent, and the path encoding below makes a subtree a
//!   contiguous key range, so a parent's whole child set is one `range` read
//!   when this lands on disk.
//!
//! ## Keys
//!
//! A key is a **stable, dense `u32`-range identifier** handed out in creation
//! order: the root is 0, and every new node takes `next_key`. Two properties are
//! load-bearing:
//!
//! * **a split never renumbers an existing key** - the property a DFS counter
//!   lacks, and what lets a dirty leaf be republished as one page without
//!   touching the partitions around it;
//! * keys only ever grow, so `parts` stays sorted by key (`push`) and
//!   `binary_search` locates any key.
//!
//! The earlier scheme packed the *path* into the key (`parent << 7·d | slot`),
//! which made a subtree a contiguous interval. A chunk whose keys are paths
//! cannot fit its keys in the format's `u32`, and the interval arithmetic is not
//! what the incremental dump needs: a dirty set is a **set**, resolved per key
//! through the chunk chain (`storage::knn_index::spann::ChunkChain`), not a
//! range.

use crate::vector::l2_sq;

use super::partition::{metric_score, Partition};
use super::quantize::{self, Code, QueryCode, Rotation};
use super::search::SearchSet;
use super::source::VectorSource;
use super::{Encode, Member, Options, PartitionKey, SpannError, SpannIndex};

/// Children an interior node may keep. A sideways split at `MAX_DEPTH` is the
/// only producer of extra children, and every child costs one centroid distance
/// per descent, so the width is capped rather than the depth alone.
pub const FANOUT: usize = 64;
/// Upper bound on fixup steps in one drain, so a commit cannot hang.
const MAX_FIXUP_STEPS: usize = 1 << 14;

/// Rows an import draws to seed the routing levels (§0.0.5). Strided over the
/// source, so it is one sequential read: 8192 × 6 KB ≈ 48 MB at 1536d, dropped
/// as soon as the space exists.
pub const BOOTSTRAP_SAMPLE_ROWS: usize = 8 * 1024;
/// Sample rows a bootstrap child needs before it earns a centroid. 32 is the
/// point where a cluster mean means something; below it the "centroid" is one
/// or two rows and the child would be a leaf nobody routes to.
const BOOTSTRAP_ROWS_PER_CHILD: usize = 32;

/// Maximum tree depth (root = depth 0). At the cap a leaf splits **sideways**
/// (a new sibling under the parent), so the size limit keeps applying.
pub const MAX_DEPTH: u32 = 9;
/// The root's key. Every other key is larger, because keys are allocated in
/// creation order.
pub const ROOT_KEY: u64 = 0;

/// One node of the incremental tree.
///
/// A leaf keeps its rows in `members` (codes against `centroid`) plus `pending`
/// (the same code layout, but not yet folded into the size accounting, so a
/// flush can hand them to the writer without touching `members`). An interior
/// node keeps its children's keys in `children` and no members.
#[derive(Clone, Debug)]
pub struct IncPartition {
    /// Stable identifier, allocated in creation order (see the module docs).
    pub key: u64,
    /// 0 for the root.
    pub depth: u32,
    /// The node whose `children` names this key. `None` only for the root.
    ///
    /// Kept on the node rather than searched for: a split/merge has to mark the
    /// parent dirty (its child list is part of its page), and a linear scan per
    /// fixup is `O(partitions)`.
    pub parent: Option<u64>,
    /// Mean of the members, raw space. Frozen once the node stops being a
    /// mutable root (invariant I1).
    pub centroid: Vec<f32>,
    /// `rot.apply(centroid)`; the codes and the query side are both relative
    /// to this, so it is cached rather than recomputed per insert.
    pub rot_centroid: Vec<f32>,
    /// `max ‖x − c‖`, for the radius lower bound (§8.3).
    pub radius: f32,
    /// Child keys, empty for a leaf.
    pub children: Vec<u64>,
    /// Rows (leaf) — codes only.
    pub members: Vec<Member>,
    /// Rows inserted since the last fold; same encoding as `members`.
    pub pending: Vec<Member>,
    /// Rows whose codes are **already on disk** (in the chunk this tree was
    /// loaded from) and are not held here.
    ///
    /// This is what makes a live tree over a published chunk cheap: the
    /// directory (centroid, radius, children, counts) stays in heap and the
    /// slot pages stay in the mapping. A split of such a leaf reads its page
    /// back through [`SlotSource`] and re-encodes the rows into two new leaves,
    /// at which point its count returns to zero.
    pub published: usize,
    /// Only the un-split root: it keeps the rows **themselves** and is scanned
    /// exactly, which is what Cockroach's root partition does (the one partition
    /// the store encodes with `UnQuantizer`) and what spfresh's full-precision
    /// posting records do. Two properties follow, and both are why the root is
    /// special:
    ///
    /// * an insert appends and stops - nothing already in the root is re-encoded,
    ///   so the write path is `O(dim)` rather than `O(rows · dim)`;
    /// * the first row's centroid does not have to be honest, because no code
    ///   ranks anything until the root splits (k-means then puts a real centroid
    ///   on each child).
    ///
    /// At most `max_partition_size` rows, ≤ 768 KB at 1536d.
    mutable: bool,
    /// The mutable root's rows, `dim` floats each, in [`Self::raw_ids`] order.
    raw: Option<Vec<f32>>,
    raw_ids: Vec<u64>,
    dim: usize,
}

impl IncPartition {
    /// An empty leaf whose centroid is `centroid`.
    fn new_leaf(
        key: u64,
        depth: u32,
        parent: Option<u64>,
        centroid: Vec<f32>,
        rot: &Rotation,
        dim: usize,
        mutable: bool,
    ) -> Self {
        let rot_centroid = rot.apply(&centroid);
        Self {
            key,
            depth,
            parent,
            centroid,
            rot_centroid,
            radius: 0.0,
            children: Vec::new(),
            members: Vec::new(),
            pending: Vec::new(),
            published: 0,
            mutable,
            raw: mutable.then(|| Vec::with_capacity(dim * 4)),
            raw_ids: Vec::new(),
            dim,
        }
    }

    pub fn is_leaf(&self) -> bool {
        self.children.is_empty()
    }

    pub fn len(&self) -> usize {
        match self.raw {
            Some(_) => self.raw_ids.len(),
            None => self.published + self.members.len() + self.pending.len(),
        }
    }

    /// Rows this node holds **here** (not the ones already published).
    pub fn local_len(&self) -> usize {
        match self.raw {
            Some(_) => self.raw_ids.len(),
            None => self.members.len() + self.pending.len(),
        }
    }

    pub fn is_empty(&self) -> bool {
        self.len() == 0
    }

    /// Folds `pending` into `members`. The codes are already relative to this
    /// node's centroid, so this is a `Vec` splice, not a re-encode.
    pub fn fold_pending(&mut self) {
        if self.pending.is_empty() {
            return;
        }
        self.members.append(&mut self.pending);
    }

    fn push_member(&mut self, id: u64, code: Code) {
        self.pending.push(Member { id, code });
    }

    /// Appends a row to the un-split root. Nothing is encoded here: the root is
    /// scanned exactly (`scan_leaf`) and its codes are produced once, when it
    /// splits or is published.
    fn push_raw(&mut self, id: u64, vector: &[f32]) {
        let raw = self
            .raw
            .as_mut()
            .expect("only the un-split root keeps its rows");
        debug_assert_eq!(raw.len(), self.raw_ids.len() * self.dim);
        self.raw_ids.push(id);
        raw.extend_from_slice(vector);
    }
}

/// Supplies a leaf's **original** rows to a split (spfresh's
/// `SPFreshVectorFetcher`, `SPANN_DESIGN.md` §2.2).
///
/// A leaf keeps only 1-bit codes. Without this callback a split can only
/// re-derive its rows by dequantizing them, and the code it writes then
/// describes the reconstruction instead of the row.
///
/// [`crate::spann::source::row_source`] turns the engine's existing
/// `VectorSource` (the mapped blocks the build already reads, or a mapped
/// scratch file) into one of these; the caller builds it once, before the
/// inserts, so a split that lands mid-batch still has every row it needs.
pub type RowSource<'a> = &'a dyn Fn(u64) -> Option<Vec<f32>>;

/// Supplies a **published leaf's slots** by key: the codes and row ids already
/// stored in the chunk the live tree was loaded from.
///
/// The counterpart of [`RowSource`] for a tree that keeps only its directory in
/// heap. `storage::knn_index::spann::ChunkChain` answers it from the mapped slot
/// page, so a split reads one page (~27 KB at 128 rows × 212 B), not the tree.
pub type SlotSource<'a> = &'a dyn Fn(u64) -> Option<Vec<Member>>;

/// One node of a published directory, as [`IncIndex::from_durable`] takes it.
///
/// The field names are the chunk's (`Storage`'s `TreeMeta`): a **level** counted
/// upward from the leaf and a member **count**, because a published leaf's rows
/// stay in the chunk.
#[derive(Clone, Debug, PartialEq)]
pub struct DurableNode {
    pub key: PartitionKey,
    /// `1` = leaf, `root.level == height` (§6.1).
    pub level: u32,
    /// Rows the node covers (a leaf's slot count).
    pub members: usize,
    pub radius: f32,
    pub children: Vec<PartitionKey>,
    pub centroid: Vec<f32>,
}

/// The incremental index.
///
/// `parts` is sorted by [`IncPartition::key`], and keys are allocated in
/// creation order, so `find` is a binary search and a new node is a `push`.
pub struct IncIndex {
    opts: Options,
    dim: usize,
    rot: Rotation,
    parts: Vec<IncPartition>,
    rows: usize,
    /// Next key to hand out. Allocated nodes are always larger than every
    /// existing one, which is what keeps `parts` sorted without a re-sort.
    next_key: u64,
    splits: u64,
    merges: u64,
    /// Nodes whose **page** no longer matches what is on disk: a leaf that took
    /// rows, a node whose children changed, a node that did not exist yet. This
    /// is the set `to_spann_dirty` publishes and nothing else.
    dirty: std::collections::HashSet<u64>,
    /// Nodes whose size crossed a threshold, by key. Drained by
    /// [`IncIndex::run_fixups`], never on the insert path: both references queue
    /// this work (sprefresh's `SplitAsync` pool, Cockroach's fixup processor)
    /// because an inline k-means turns one insert into a millisecond tail. The
    /// drain re-checks the size before acting, as theirs does.
    fixups: Vec<u64>,
    /// Keys already in `fixups`, so a hot leaf is not enqueued on every insert.
    queued: std::collections::HashSet<u64>,
}

impl IncIndex {
    pub fn new(dim: usize, opts: Options) -> Result<Self, SpannError> {
        opts.validate()?;
        if dim == 0 {
            return Err(SpannError::DimensionMismatch {
                expected: 1,
                actual: 0,
            });
        }
        Ok(Self {
            rot: Rotation::new(dim, opts.seed),
            opts,
            dim,
            parts: Vec::new(),
            rows: 0,
            next_key: ROOT_KEY + 1,
            splits: 0,
            merges: 0,
            dirty: std::collections::HashSet::new(),
            fixups: Vec::new(),
            queued: std::collections::HashSet::new(),
        })
    }

    /// A live tree over a **published directory**: the shape the next insert
    /// descends and the leaves it appends to.
    ///
    /// What is *not* read here is the rows: a published leaf's codes stay in the
    /// chunk and are fetched by key when a split needs them
    /// (`IncPartition::published`, [`SlotSource`]). So loading a 390k-partition
    /// tree costs the directory (~40 B per node plus two centroids), not
    /// 208 B per row.
    ///
    /// Keys are kept **as published**, which is what makes the first delta chunk
    /// addressable next to the base one; new nodes take keys above the highest
    /// loaded key.
    pub fn from_durable(
        dim: usize,
        opts: Options,
        mut nodes: Vec<DurableNode>,
    ) -> Result<Self, SpannError> {
        let mut index = Self::new(dim, opts)?;
        if nodes.is_empty() {
            return Ok(index);
        }
        for node in &nodes {
            if node.centroid.len() != dim {
                return Err(SpannError::DimensionMismatch {
                    expected: dim,
                    actual: node.centroid.len(),
                });
            }
        }
        nodes.sort_unstable_by_key(|node| node.key);
        // The shape is read from the **child edges**, not from the `level`
        // column. A split converts the overflowing leaf into a branch *in place*
        // (so `ROOT_KEY` never moves), which raises the level of every node above
        // it - and a delta chunk that rewrote that leaf leaves the root's page,
        // and its now stale level, in the chunk underneath it. Deriving "the root
        // is the highest level" then finds two roots. Depth from the root, by
        // walk, is what the tree actually is.
        let root_key = ROOT_KEY as PartitionKey;
        if nodes.iter().all(|node| node.key != root_key) {
            return Err(SpannError::Corrupt(
                "a directory without its root".to_string(),
            ));
        }
        let at_of: std::collections::HashMap<PartitionKey, usize> = nodes
            .iter()
            .enumerate()
            .map(|(at, node)| (node.key, at))
            .collect();
        // Child → parent, one pass instead of a scan per node. An edge to a key
        // no node names is left out: that is a dangling edge (a delta chunk
        // whose parent page it did not rewrite), which `ChunkChain::coverage`
        // reports and the reader resolves through the older layer.
        let mut parents: std::collections::HashMap<PartitionKey, PartitionKey> =
            std::collections::HashMap::new();
        for node in &nodes {
            for child in &node.children {
                if at_of.contains_key(child) {
                    parents.insert(*child, node.key);
                }
            }
        }
        // Depth from the root. A node nobody reaches is a page an older layer
        // still holds but the current parent no longer names (a retired sibling):
        // it keeps its rows' keys resolvable and starts a descent that never
        // arrives, so keeping it costs one directory entry.
        let root_at = at_of[&root_key];
        let mut depths = vec![u32::MAX; nodes.len()];
        depths[root_at] = 0;
        let mut frontier: Vec<PartitionKey> = vec![root_key];
        while let Some(key) = frontier.pop() {
            let depth = depths[at_of[&key]];
            for child in &nodes[at_of[&key]].children {
                if let Some(at) = at_of.get(child) {
                    if depths[*at] > depth + 1 {
                        depths[*at] = depth + 1;
                        frontier.push(*child);
                    }
                }
            }
        }
        let mut rows = 0usize;
        let mut parts = Vec::with_capacity(nodes.len());
        for (at, node) in nodes.iter().enumerate() {
            let reachable = depths[at] != u32::MAX;
            // An unreachable page sits where its parent put it, one level under
            // it, so a split that later reaches it starts from the right depth.
            let depth = if reachable {
                depths[at]
            } else {
                parents
                    .get(&node.key)
                    .and_then(|parent| at_of.get(parent))
                    .map(|parent| depths[*parent].saturating_add(1))
                    .filter(|depth| *depth != u32::MAX)
                    .unwrap_or(1)
            };
            let parent = parents.get(&node.key).map(|key| u64::from(*key));
            let mut part = IncPartition::new_leaf(
                u64::from(node.key),
                depth,
                parent,
                node.centroid.clone(),
                &index.rot,
                dim,
                false,
            );
            part.radius = node.radius;
            part.children = node.children.iter().map(|key| u64::from(*key)).collect();
            part.published = if node.children.is_empty() {
                node.members
            } else {
                0
            };
            // Only a reachable leaf's rows are the tree's: a retired leaf's page
            // still holds the rows that moved into the leaves that replaced it.
            if reachable && node.children.is_empty() {
                rows += part.published;
            }
            parts.push(part);
        }
        index.next_key = parts
            .last()
            .map(|part| part.key + 1)
            .unwrap_or(ROOT_KEY + 1);
        index.parts = parts;
        index.rows = rows;
        Ok(index)
    }

    /// A key no node uses yet. Always larger than every existing key.
    fn alloc_key(&mut self) -> u64 {
        let key = self.next_key;
        self.next_key += 1;
        debug_assert!(
            key <= u64::from(u32::MAX),
            "a chunk's key column is u32; {} partitions is past it",
            key
        );
        key
    }

    /// Keys whose page differs from what is on disk - the publish set (§5.4).
    pub fn dirty_nodes(&self) -> Vec<u64> {
        let mut keys: Vec<u64> = self.dirty.iter().copied().collect();
        keys.sort_unstable();
        keys
    }

    /// Whether anything was written since the tree was created or published.
    pub fn is_dirty(&self) -> bool {
        !self.dirty.is_empty()
    }

    /// Rows the tree accounts for but has not published yet.
    pub fn pending_rows(&self) -> usize {
        self.parts
            .iter()
            .filter(|part| part.is_leaf())
            .map(|part| part.local_len())
            .sum()
    }

    /// Rows whose codes are already in a chunk.
    pub fn published_rows(&self) -> usize {
        self.parts.iter().map(|part| part.published).sum()
    }

    pub fn dim(&self) -> usize {
        self.dim
    }

    pub fn len(&self) -> usize {
        self.rows
    }

    pub fn is_empty(&self) -> bool {
        self.rows == 0
    }

    pub fn parts(&self) -> &[IncPartition] {
        &self.parts
    }

    /// Number of splits performed, for the write-amplification metric.
    pub fn splits(&self) -> u64 {
        self.splits
    }

    /// Number of merges performed, the other half of that metric.
    pub fn merges(&self) -> u64 {
        self.merges
    }

    /// Fixups queued and not yet drained. A caller that commits a batch drains
    /// when this is non-zero.
    pub fn pending_fixups(&self) -> usize {
        self.fixups.len()
    }

    pub fn height(&self) -> u32 {
        self.parts.iter().map(|p| p.depth + 1).max().unwrap_or(0)
    }

    /// The same tree as a bulk [`SpannIndex`], which is the form the chunk
    /// writer publishes (`storage::knn_index::spann::write_to`).
    ///
    /// **Keys are kept**, not renumbered to positions: a published key is the
    /// node's stable identifier, and a delta chunk is only addressable next to
    /// the chunk it supersedes if both name the node the same way.
    ///
    /// **Levels** are counted bottom-up (`1` = leaf, `root.level == height`,
    /// §6.1). `depth` cannot stand in for `level`: this tree grows one leaf at a
    /// time, so a leaf can sit above `MAX_DEPTH`'s complement of the root, and
    /// the format's reader treats `level <= 1` as "this is a leaf".
    ///
    /// `None` when there is nothing to publish, or when a leaf's rows are only
    /// on disk and no [`SlotSource`] was supplied: silently publishing a short
    /// partition would lose rows.
    pub fn to_spann(&self) -> Option<SpannIndex> {
        let keys: Vec<u64> = self.parts.iter().map(|part| part.key).collect();
        self.to_spann_keys(&keys, None)
    }

    /// [`Self::to_spann`] over an explicit key set - the publish path of a
    /// **dirty set** (§5.4).
    ///
    /// Each key becomes one partition of the chunk, with its own children and
    /// members; an edge to a key outside `keys` is left dangling, which is what
    /// the chain reader resolves against the older chunks
    /// (`storage::knn_index::spann::write_dirty`).
    pub fn to_spann_keys(
        &self,
        keys: &[u64],
        slots: Option<SlotSource<'_>>,
    ) -> Option<SpannIndex> {
        if keys.is_empty() || self.parts.is_empty() {
            return None;
        }
        let levels = self.levels();
        let height = levels.first().copied().unwrap_or(1).max(1);
        let mut partitions: Vec<Partition> = Vec::with_capacity(keys.len());
        for key in keys {
            let at = self.find(*key)?;
            let part = &self.parts[at];
            let members = self.members_of(part, slots)?;
            partitions.push(Partition::from_decoded(
                *key as PartitionKey,
                levels[at],
                self.dim,
                part.centroid.clone(),
                part.radius,
                part.rot_centroid.clone(),
                members,
                part.children.iter().map(|child| *child as PartitionKey).collect(),
            ));
        }
        Some(SpannIndex::from_parts(
            self.opts.clone(),
            self.dim,
            self.rot.clone(),
            partitions,
            height,
            super::ROOT_KEY,
        ))
    }

    /// What changed since the tree was loaded or last published (§5.4).
    pub fn to_spann_dirty(&self, slots: Option<SlotSource<'_>>) -> Option<SpannIndex> {
        let keys = self.dirty_nodes();
        self.to_spann_keys(&keys, slots)
    }

    /// `1` for a leaf, `1 + max(child level)` above it - the format's numbering.
    fn levels(&self) -> Vec<u32> {
        let mut levels = vec![1u32; self.parts.len()];
        // Keys are allocated before their children's, so a reverse walk visits
        // every node after the node that owns it.
        for at in (0..self.parts.len()).rev() {
            if let Some(owner_key) = self.parts[at].parent {
                if let Some(owner) = self.find(owner_key) {
                    levels[owner] = levels[owner].max(levels[at] + 1);
                }
            }
        }
        levels
    }

    /// The members a partition's page holds: a leaf's rows (published base +
    /// local) or a branch's child centroids, encoded against its own centroid.
    fn members_of(&self, part: &IncPartition, slots: Option<SlotSource<'_>>) -> Option<Vec<Member>> {
        if let Some(raw) = part.raw.as_ref() {
            // The un-split root never encoded its rows; do it here, once, so the
            // chunk carries the same codes a split would have produced.
            return Some(
                part.raw_ids
                    .iter()
                    .zip(raw.chunks_exact(self.dim))
                    .map(|(id, vector)| Member {
                        id: *id,
                        code: quantize::encode(&self.rot, &part.rot_centroid, vector),
                    })
                    .collect(),
            );
        }
        let mut out: Vec<Member> = Vec::with_capacity(part.len());
        if part.published > 0 {
            let base = slots?(part.key)?;
            if base.len() != part.published {
                // The page this tree was loaded from says a different number of
                // rows: the caller is publishing against a chain that moved.
                return None;
            }
            out.extend(base);
        }
        if part.children.is_empty() {
            out.extend(part.members.iter().cloned());
            out.extend(part.pending.iter().cloned());
        } else {
            for child in &part.children {
                let centroid = &self.parts[self.find(*child)?].centroid;
                out.push(Member {
                    id: *child,
                    code: quantize::encode(&self.rot, &part.rot_centroid, centroid),
                });
            }
        }
        Some(out)
    }

    /// [`Self::to_spann`] for a tree nobody needs afterwards: the codes and
    /// centroids are **moved** into the chunk's partitions, so a build peaks at
    /// one copy of the index rather than two.
    ///
    /// A tree loaded from a chunk has its base rows on disk rather than here, so
    /// it cannot be published this way (there is nothing to move); it publishes
    /// through [`Self::to_spann_dirty`] with its [`SlotSource`] instead.
    pub fn into_spann(self) -> Option<SpannIndex> {
        if self.parts.iter().any(|part| part.published > 0) {
            return self.to_spann();
        }
        let IncIndex {
            opts,
            dim,
            rot,
            parts,
            ..
        } = self;
        if parts.is_empty() {
            return None;
        }
        let levels = Self::levels_for(&parts);
        let height = levels.first().copied().unwrap_or(1).max(1);

        let mut partitions: Vec<Partition> = Vec::with_capacity(parts.len());
        for (at, part) in parts.into_iter().enumerate() {
            let IncPartition {
                key,
                centroid,
                rot_centroid,
                radius,
                children,
                mut members,
                mut pending,
                raw,
                raw_ids,
                ..
            } = part;
            // Child keys stay the keys they are: a chunk's edges name stable
            // identifiers, and the reader resolves them by key.
            let members: Vec<Member> = match raw {
                // The un-split root never encoded its rows; do it once here.
                Some(raw) => raw_ids
                    .iter()
                    .zip(raw.chunks_exact(dim))
                    .map(|(id, vector)| Member {
                        id: *id,
                        code: quantize::encode(&rot, &rot_centroid, vector),
                    })
                    .collect(),
                None => {
                    members.append(&mut pending);
                    members
                }
            };
            partitions.push(Partition::from_decoded(
                key as PartitionKey,
                levels[at],
                dim,
                centroid,
                radius,
                rot_centroid,
                members,
                children.iter().map(|child| *child as PartitionKey).collect(),
            ));
        }
        Some(SpannIndex::from_parts(
            opts,
            dim,
            rot,
            partitions,
            height,
            super::ROOT_KEY,
        ))
    }

    /// [`Self::levels`] over a node vector that is being consumed.
    fn levels_for(parts: &[IncPartition]) -> Vec<u32> {
        let mut levels = vec![1u32; parts.len()];
        for at in (0..parts.len()).rev() {
            if let Some(owner_key) = parts[at].parent {
                if let Ok(owner) = parts.binary_search_by_key(&owner_key, |part| part.key) {
                    levels[owner] = levels[owner].max(levels[at] + 1);
                }
            }
        }
        levels
    }

    /// Bytes held by codes, centroids and the mutable root's raw buffer.
    ///
    /// A loaded tree holds no codes for its published leaves, so this is the
    /// directory plus the rows written since the load - the number that decides
    /// whether a live tree fits next to its chunks.
    pub fn mem_usage(&self) -> usize {
        let per_member = self.opts.encode.record_bytes(self.dim) + 24;
        let mut bytes = 0;
        for p in &self.parts {
            bytes += p.local_len() * per_member;
            bytes += p.centroid.len() * 4 + p.rot_centroid.len() * 4;
            bytes += p.children.len() * 8;
            bytes += p.raw.as_ref().map(|r| r.len() * 4).unwrap_or(0);
        }
        bytes
    }

    /// Inserts one row: descend, append, queue. `O(depth · FANOUT · dim)` for the
    /// descent plus `O(dim)` to append - no re-encode, no k-means, no partition
    /// rewrite.
    ///
    /// That is the shape all three reference implementations use. spfresh appends
    /// one self-describing record to a posting and stops
    /// (`ExtraDynamicSearcher.h:1495-1514`), and its split goes to
    /// `SplitAsync`'s pool (`:816-838`). Cockroach C-SPANN writes one KV put into
    /// the leaf's key range and queues `fixups.AddSplit`
    /// (`vecstore/store_txn.go:250-260`, `cspann/fixup_processor.go:308-324`).
    /// Neither re-encodes the partition it appends to; turbopuffer goes further
    /// and makes the append an object-store WAL entry, with the index built
    /// behind it.
    ///
    /// So the queue is [`Self::fixups`] and the caller drains it with
    /// [`Self::run_fixups`]. The engine's commit/flush loop is the natural drain
    /// point, the way turbopuffer's group commit (at most one WAL entry a second)
    /// is theirs. A caller that never drains is still *correct*, it just leaves
    /// the tree coarse: the leaf grows past `max_partition_size` and the split
    /// happens in the next drain, over more rows.
    pub fn insert(&mut self, row_id: u64, vector: &[f32]) -> Result<(), SpannError> {
        if vector.len() != self.dim {
            return Err(SpannError::DimensionMismatch {
                expected: self.dim,
                actual: vector.len(),
            });
        }
        if self.parts.is_empty() {
            let mut part =
                IncPartition::new_leaf(ROOT_KEY, 0, None, vector.to_vec(), &self.rot, self.dim, true);
            part.push_raw(row_id, vector);
            self.parts.push(part);
            self.dirty.insert(ROOT_KEY);
            self.rows = 1;
            return Ok(());
        }

        let idx = self.leaf_for(vector);
        let key = self.parts[idx].key;
        if self.parts[idx].mutable {
            // The un-split root: append and walk away. It is not ranked by codes
            // (`scan_leaf` measures its rows exactly), so its centroid does not
            // have to follow the data and no existing row is re-encoded.
            let part = &mut self.parts[idx];
            part.push_raw(row_id, vector);
            part.radius = part.radius.max(l2_sq(vector, &part.centroid).sqrt());
            self.rows += 1;
        } else {
            let rot_centroid = self.parts[idx].rot_centroid.clone();
            let code = quantize::encode(&self.rot, &rot_centroid, vector);
            let radius = l2_sq(vector, &self.parts[idx].centroid).sqrt();
            let part = &mut self.parts[idx];
            if radius > part.radius {
                part.radius = radius;
            }
            part.push_member(row_id, code);
            self.rows += 1;
        }
        // One page changed: this leaf's slot column takes the new row (or, for
        // the mutable root, the node exists for the first time).
        self.dirty.insert(key);
        if self.parts[idx].len() > self.opts.max_partition_size {
            self.queue_fixup(key);
        }
        Ok(())
    }

    /// Drains the fixup queue: splits that have been earned, then merges for
    /// leaves that have fallen under `min_partition_size`.
    ///
    /// Not on the insert path, by design - see [`Self::insert`]. The caller runs
    /// this at a batch boundary (commit, flush, or the end of a load).
    pub fn run_fixups(&mut self, source: Option<RowSource<'_>>) {
        self.run_fixups_from(source, None);
    }

    /// [`Self::run_fixups`] for a tree whose leaves are **published**: a split
    /// has to read the leaf's slot page to get its rows back, and `slots`
    /// supplies it (`storage::knn_index::spann::ChunkChain`).
    pub fn run_fixups_from(&mut self, source: Option<RowSource<'_>>, slots: Option<SlotSource<'_>>) {
        // A split queues nothing new and a merge neither, so one pass per queued
        // key is the whole job; the bound only stops a pathological input from
        // hanging a commit.
        let mut steps = 0usize;
        let mut rows_before: usize = self
            .parts
            .iter()
            .filter(|p| p.is_leaf())
            .map(|p| p.len())
            .sum();
        while let Some(key) = self.fixups.pop() {
            steps += 1;
            if steps > MAX_FIXUP_STEPS {
                break;
            }
            self.queued.remove(&key);
            let Some(idx) = self.find(key) else {
                // Merged away by an earlier job in this pass.
                continue;
            };
            let len = self.parts[idx].len();
            let _ = if len > self.opts.max_partition_size {
                self.split_or_sideways(idx, source, slots)
            } else if len < self.opts.min_partition_size {
                self.merge(idx, source, slots)
            } else {
                Ok(())
            };
            let now: usize = self
                .parts
                .iter()
                .filter(|p| p.is_leaf())
                .map(|p| p.len())
                .sum();
            debug_assert_eq!(now, rows_before, "fixup on key {key} lost a row");
            rows_before = now;
        }
        self.merge_underfull(source, slots);
    }

    fn queue_fixup(&mut self, key: u64) {
        if self.queued.insert(key) {
            self.fixups.push(key);
        }
    }

    /// Builds the routing levels of an empty index from a sample (§0.0.5).
    ///
    /// Why sample at all: a tree that starts as one partition decides its first
    /// cuts from whichever rows happened to arrive first, and the import cannot
    /// hold the codes of the whole table to do better (R1). A strided sample is
    /// read **sequentially** off the mapped column - no row-id table, no second
    /// pass - and gives every row a global centroid space to descend before the
    /// first row is written. That is spfresh's shape: `SelectHead`
    /// (`SPANNIndex.cpp:652`) runs a BKT k-means over a sample of the data and
    /// hands the selected heads to `BuildIndex`, which then only *assigns*.
    ///
    /// The budget is `BOOTSTRAP_SAMPLE_ROWS` rows whatever the table size, and a
    /// child needs `BOOTSTRAP_ROWS_PER_CHILD` sample rows to earn a centroid:
    /// with 8k rows that is ~256 leaves of upper structure, and the rest of the
    /// tree grows by splits (which is monotone, so it cannot cost quality).
    pub fn bootstrap(
        src: &dyn VectorSource,
        dim: usize,
        opts: Options,
    ) -> Result<Self, SpannError> {
        let sample = draw_sample(src, BOOTSTRAP_SAMPLE_ROWS);
        Self::bootstrap_from_sample(dim, opts, src.len(), sample)
    }

    /// [`Self::bootstrap`] over sample rows the caller already holds.
    pub fn bootstrap_from_sample(
        dim: usize,
        opts: Options,
        est_rows: usize,
        sample: Vec<Vec<f32>>,
    ) -> Result<Self, SpannError> {
        let mut index = Self::new(dim, opts.clone())?;
        for row in &sample {
            if row.len() != dim {
                return Err(SpannError::DimensionMismatch {
                    expected: dim,
                    actual: row.len(),
                });
            }
        }
        if sample.len() < BOOTSTRAP_ROWS_PER_CHILD * 2 || est_rows <= opts.max_partition_size {
            // Too small to be worth a space: the root becomes a normal mutable
            // leaf on the first insert, and splits shape it from there.
            return Ok(index);
        }
        let want = est_rows.div_ceil(opts.max_partition_size).max(2);
        index.bootstrap_subtree(ROOT_KEY, 0, &sample, want);
        Ok(index)
    }

    /// Appends one node (and its subtree) in DFS pre-order. Returns the key it
    /// created, which is always `key`.
    fn bootstrap_subtree(&mut self, key: u64, depth: u32, rows: &[Vec<f32>], want: usize) -> u64 {
        let centroid = mean_of(rows, self.dim);
        let radius = rows
            .iter()
            .map(|row| l2_sq(row, &centroid).sqrt())
            .fold(0.0f32, f32::max);
        let pos = self.parts.len();
        let mut part = IncPartition::new_leaf(key, depth, None, centroid, &self.rot, self.dim, false);
        part.radius = radius;
        self.parts.push(part);

        if depth + 1 >= MAX_DEPTH {
            return key;
        }
        // Children worth having, bounded by the sample: a child with fewer than
        // `BOOTSTRAP_ROWS_PER_CHILD` sample rows has a meaningless centroid, and
        // `want` is what the estimated row count asks for. Take as many as both
        // allow, up to the fan-out - a binary chain would make the sample 7
        // levels deep and leave a beam of 8 covering 8 of its leaves.
        let viable = rows.len() / BOOTSTRAP_ROWS_PER_CHILD;
        if want <= 1 || viable < 2 {
            return key;
        }
        let k = want.min(FANOUT).min(viable).clamp(2, FANOUT);

        let refs: Vec<&[f32]> = rows.iter().map(|row| row.as_slice()).collect();
        let (_, assign) = kmeans_k(
            &refs,
            self.dim,
            k,
            self.opts.kmeans_iterations,
            self.opts.seed ^ key.wrapping_mul(0x9E37_79B9_7F4A_7C15),
        );
        let mut groups: Vec<Vec<Vec<f32>>> = vec![Vec::new(); k];
        for (i, slot) in assign.iter().enumerate() {
            groups[*slot as usize].push(rows[i].clone());
        }
        groups.retain(|group| group.len() >= BOOTSTRAP_ROWS_PER_CHILD);
        if groups.len() < 2 {
            return key;
        }

        let per_child = (want / groups.len()).max(1);
        let mut children = Vec::with_capacity(groups.len());
        for group in groups.iter() {
            let child = self.alloc_key();
            self.bootstrap_subtree(child, depth + 1, group, per_child);
            children.push(child);
            let at = self.find(child).expect("just appended");
            self.parts[at].parent = Some(key);
        }
        self.parts[pos].children = children;
        // The upper structure was never on disk; publishing it is the caller's
        // choice (`to_spann_keys` over every key), so it is not marked dirty.
        key
    }

    /// Splits a leaf that is still over its limit, sideways when it has no depth
    /// left. The second look is Cockroach's (`fixup_split.go:121-131`): rows may
    /// have been merged away between the queue and the drain.
    fn split_or_sideways(
        &mut self,
        idx: usize,
        source: Option<RowSource<'_>>,
        slots: Option<SlotSource<'_>>,
    ) -> Result<(), SpannError> {
        let part = &self.parts[idx];
        if !part.is_leaf() || part.len() < 2 {
            return Ok(());
        }
        let abandon = (self.opts.max_partition_size + self.opts.min_partition_size) / 2;
        if part.len() <= abandon {
            return Ok(());
        }
        if part.depth + 1 < MAX_DEPTH {
            self.split(idx, source, slots)
        } else {
            self.split_sideways(idx, source, slots)
        }
    }

    /// Folds a leaf that fell under `min_partition_size` into its nearest siblings.
    ///
    /// The shape is the reference's, not "move the whole leaf into one target":
    /// every row goes to its own nearest sibling that still has room. Cockroach
    /// re-inserts each vector at the merged partition's level
    /// (`fixup_merge.go:120-135`), and spfresh assigns each vector to the nearest
    /// head with room (`ExtraDynamicSearcher.h:1693`). Moving the leaf wholesale
    /// would let a handful of far rows inflate the target's radius and would fill
    /// one sibling while its neighbours stay empty.
    ///
    /// Only *leaf* siblings are candidates: an interior node holds its rows in
    /// its children, so appending members to one parks them where no descent,
    /// scan or publish ever looks (that was the lost-row bug).
    ///
    /// The plan is computed before anything moves (every row's target, with the
    /// projected counts budgeted), because an append cannot be undone: spfresh
    /// documents the same constraint, "assign first, migrate second".
    ///
    /// Quality gate (I9c): the nearest candidate must be *adjacent*,
    /// `d(c_s, c_t) <= r_s + r_t` - the two covering balls touch. A source whose
    /// ball touches no sibling is the only centroid serving that region, so it is
    /// not retired: its rows would move from `α·r_s` to `α(d + r_s)` error with
    /// nothing gaining that ground back. Rows are re-encoded against `c_t` here
    /// (RaBitQ codes are centroid-relative), so unlike spfresh - which
    /// concatenates postings verbatim - a merge *can* degrade a code; adjacency
    /// plus the underfull trigger keeps that bounded.
    fn merge(
        &mut self,
        idx: usize,
        source: Option<RowSource<'_>>,
        slots: Option<SlotSource<'_>>,
    ) -> Result<(), SpannError> {
        let (key, len, src_centroid, src_radius) = {
            let part = &self.parts[idx];
            (part.key, part.len(), part.centroid.clone(), part.radius)
        };
        if len == 0 || len >= self.opts.min_partition_size {
            return Ok(());
        }
        let Some(parent) = self.parts.iter().position(|p| p.children.contains(&key)) else {
            // The root, or a leaf an earlier merge already detached.
            return Ok(());
        };

        // Candidates: leaf siblings with room.
        let mut candidates: Vec<(usize, f32, usize)> = Vec::new();
        for child in &self.parts[parent].children {
            if *child == key {
                continue;
            }
            let Some(at) = self.find(*child) else {
                continue;
            };
            let part = &self.parts[at];
            if !part.is_leaf() {
                continue;
            }
            let room = self.opts.max_partition_size.saturating_sub(part.len());
            if room == 0 {
                continue;
            }
            let d = l2_sq(&src_centroid, &part.centroid).sqrt();
            candidates.push((at, d, room));
        }
        if candidates.is_empty() || candidates.iter().map(|c| c.2).sum::<usize>() < len {
            // Every sibling is full: the next sweep tries again.
            return Ok(());
        }
        candidates.sort_by(|a, b| a.1.total_cmp(&b.1));
        let (nearest, nearest_d, _) = candidates[0];
        if nearest_d > src_radius + self.parts[nearest].radius {
            return Ok(());
        }

        let (vectors, ids) = self.gather_rows(idx, source, slots);
        debug_assert_eq!(
            vectors.len(),
            len,
            "a merge has to supply every row of {key}"
        );

        // Plan before moving: `room` shrinks as rows are placed, so the second
        // half of a leaf cannot all pile into one sibling.
        let mut room: Vec<usize> = candidates.iter().map(|c| c.2).collect();
        let mut plan: Vec<usize> = Vec::with_capacity(vectors.len());
        for vector in &vectors {
            let mut choice = usize::MAX;
            let mut best = f32::INFINITY;
            for (slot, (at, _, _)) in candidates.iter().enumerate() {
                if room[slot] == 0 {
                    continue;
                }
                let d = l2_sq(vector, &self.parts[*at].centroid);
                if d < best {
                    best = d;
                    choice = slot;
                }
            }
            if choice == usize::MAX {
                return Ok(());
            }
            room[choice] -= 1;
            plan.push(choice);
        }

        let leaf_rows = |parts: &[IncPartition]| -> usize {
            parts.iter().filter(|p| p.is_leaf()).map(|p| p.len()).sum()
        };
        let before = leaf_rows(&self.parts);

        for (i, slot) in plan.iter().enumerate() {
            let at = candidates[*slot].0;
            let vector = &vectors[i];
            let code = quantize::encode(&self.rot, &self.parts[at].rot_centroid, vector);
            let radius = l2_sq(vector, &self.parts[at].centroid).sqrt();
            let part = &mut self.parts[at];
            part.push_member(ids[i], code);
            if radius > part.radius {
                part.radius = radius;
            }
            let target = part.key;
            self.dirty.insert(target);
        }
        for (at, _, _) in &candidates {
            self.parts[*at].fold_pending();
        }

        // Detach the emptied leaf. The parent keeps its centroid (I1), so
        // dropping a child invalidates nothing; a parent left with one child is
        // legal (any level width is) and is collapsed by an `optimize` pass, not
        // here.
        let parent_key = self.parts[parent].key;
        self.parts[parent].children.retain(|child| *child != key);
        self.parts.remove(idx);
        self.queued.remove(&key);
        // The leaf's page disappears from the tree (it is no longer reachable, so
        // the chain stops counting its rows) and the parent's child list changed.
        self.dirty.remove(&key);
        self.dirty.insert(parent_key);
        self.merges += 1;
        debug_assert_eq!(
            leaf_rows(&self.parts),
            before,
            "merging {key} into its siblings lost a row"
        );
        Ok(())
    }

    /// Merges every leaf that a delete or a split left under the limit.
    fn merge_underfull(&mut self, source: Option<RowSource<'_>>, slots: Option<SlotSource<'_>>) {
        let thin: Vec<u64> = self
            .parts
            .iter()
            .filter(|p| p.is_leaf() && !p.is_empty() && p.len() < self.opts.min_partition_size)
            .map(|p| p.key)
            .collect();
        for key in thin {
            if let Some(idx) = self.find(key) {
                let _ = self.merge(idx, source, slots);
            }
        }
    }

    /// Cuts one leaf in two.
    ///
    /// A full leaf holds only 1-bit codes, so the split needs its rows back.
    /// `source` supplies them; a row it cannot answer for falls back to its
    /// dequantized code, and that fallback is **not** a small error: the
    /// re-encoded code describes `dequantize(code)`, which sits
    /// `≈ 0.75·‖x − c‖` away from the row it stands for. Measured at 128d that
    /// is 3.2–4.5 against a leaf radius of 5–6.6 — half a radius per row — so a
    /// re-encoded leaf answers its own rows' self-queries with that distance and
    /// loses every neighbour that is closer than it. Keeping the raw rows in
    /// DRAM is the alternative and costs 6 KB per row at 1536d, which is the
    /// constraint the codes exist to avoid, so the row source is the cheap side
    /// of the trade.
    fn split(
        &mut self,
        idx: usize,
        source: Option<RowSource<'_>>,
        slots: Option<SlotSource<'_>>,
    ) -> Result<(), SpannError> {
        let part = &self.parts[idx];
        let depth = part.depth;
        let key = part.key;
        let (vectors, ids) = self.gather_rows(idx, source, slots);
        if vectors.len() < 2 {
            return Ok(());
        }

        let refs: Vec<&[f32]> = vectors.iter().map(|v| v.as_slice()).collect();
        // A leaf that just crossed `max` (the common case: 129 rows) is cut in
        // two balanced halves, exactly like Cockroach's bisection; a leaf with
        // many times `max` in it (a bulk import streaming into one partition, or
        // the root after bootstrap) is cut into as many groups as the target
        // fill implies. The two-way case must be balanced, which is why it does
        // not go through the generic k-way path.
        let target = self.opts.max_partition_size;
        let groups_wanted = if vectors.len() <= 2 * target {
            2
        } else {
            vectors.len().div_ceil((target / 2).max(1)).clamp(2, FANOUT)
        };
        let seed = self.opts.seed ^ key.wrapping_mul(0x9E37_79B9_7F4A_7C15);
        let groups: Vec<Vec<(u64, &[f32])>> = if groups_wanted == 2 {
            // Two-way cuts are the common case (a leaf crosses `max` by a row or
            // two) and have to be *balanced*: Cockroach bisects with
            // `BalancedKMeans` for exactly this reason. An unbalanced 2-way cut
            // hands one child fewer rows than `min_partition_size`, the merge
            // pass folds it straight back into its sibling, and the leaf refills
            // and splits again - measured at 1344 merges per 200k pure inserts,
            // one per two splits. A balanced cut makes that zero.
            let split = super::kmeans::balanced_kmeans_2(
                &refs,
                self.dim,
                self.opts.kmeans_iterations,
                seed,
            );
            let mut groups: Vec<Vec<(u64, &[f32])>> = vec![Vec::new(), Vec::new()];
            for (i, &second) in split.second.iter().enumerate() {
                groups[usize::from(second)].push((ids[i], refs[i]));
            }
            groups
        } else {
            let (centroids, assign) = kmeans_k(
                &refs,
                self.dim,
                groups_wanted,
                self.opts.kmeans_iterations,
                seed,
            );
            let mut groups: Vec<Vec<(u64, &[f32])>> = vec![Vec::new(); centroids.len()];
            for (i, slot) in assign.iter().enumerate() {
                groups[*slot as usize].push((ids[i], refs[i]));
            }
            groups
        };
        // A degenerate cut (everything on one side) is refused: splitting would
        // produce empty leaves and the next insert would split again forever.
        let non_empty = groups.iter().filter(|g| !g.is_empty()).count();
        if non_empty < 2 {
            return Ok(());
        }

        // The parent keeps its centroid (I1) and becomes an interior node. Its
        // rows move into the children, so nothing of the old page survives.
        let parent = &mut self.parts[idx];
        parent.members.clear();
        parent.pending.clear();
        parent.published = 0;
        parent.raw = None;
        parent.raw_ids.clear();
        parent.mutable = false;
        parent.children.clear();

        let mut leaves = Vec::with_capacity(non_empty);
        for group in groups.iter().filter(|group| !group.is_empty()) {
            let child = self.alloc_key();
            let leaf = self.make_leaf(child, depth + 1, Some(key), group);
            leaves.push((child, leaf));
        }
        for (child, leaf) in leaves {
            self.parts[idx].children.push(child);
            self.parts.push(leaf);
            self.dirty.insert(child);
        }
        // The node itself changed level and children, and its slots are gone.
        self.dirty.insert(key);
        debug_assert_eq!(self.parts[idx].children.len(), non_empty);
        self.splits += 1;
        Ok(())
    }

    /// The rows of `idx`, from `raw` while the leaf still keeps them, else from
    /// the row source, else by dequantizing the codes - the last one is the
    /// lossy fallback, see [`Self::split`].
    ///
    /// A leaf loaded from a chunk has neither `raw` nor `members`: its codes come
    /// from `slots(key)`, the published page. That is the read half of the
    /// read-modify-write the dirty-leaf dump performs.
    fn gather_rows(
        &self,
        idx: usize,
        source: Option<RowSource<'_>>,
        slots: Option<SlotSource<'_>>,
    ) -> (Vec<Vec<f32>>, Vec<u64>) {
        let part = &self.parts[idx];
        let mut vectors: Vec<Vec<f32>> = Vec::with_capacity(part.len());
        let mut ids: Vec<u64> = Vec::with_capacity(part.len());
        if let Some(raw) = part.raw.as_ref() {
            for (i, chunk) in raw.chunks_exact(self.dim).enumerate() {
                vectors.push(chunk.to_vec());
                ids.push(part.raw_ids.get(i).copied().unwrap_or(0));
            }
            return (vectors, ids);
        }
        let base: Vec<Member> = if part.published > 0 {
            match slots.and_then(|page| page(part.key)) {
                Some(members) if members.len() == part.published => members,
                // Without the page (or with a page that moved) the leaf cannot be
                // split honestly: reporting a short row list makes the caller
                // refuse the split (`vectors.len() < 2`), which leaves the leaf
                // overfull rather than silently dropping rows.
                _ => return (Vec::new(), Vec::new()),
            }
        } else {
            Vec::new()
        };
        for m in base
            .iter()
            .chain(part.members.iter())
            .chain(part.pending.iter())
        {
            let row = match source.and_then(|fetch| fetch(m.id)) {
                Some(v) if v.len() == self.dim => v,
                _ => dequantize_member(&self.rot, &part.rot_centroid, m),
            };
            vectors.push(row);
            ids.push(m.id);
        }
        (vectors, ids)
    }

    /// Splits a leaf that has run out of depth **sideways**: the two halves
    /// become children of the leaf's parent, at the overflowing leaf's own depth,
    /// so no level gets deeper.
    ///
    /// A depth cap cannot be the rule that stops a leaf from splitting. Inserts
    /// follow the nearest centroid, so one crowded region keeps feeding the same
    /// leaf; once the tree is at `MAX_DEPTH` the size limit silently stops
    /// applying, and the query cost of that leaf grows without bound. Measured
    /// on 50k isotropic rows x 128d with `max_partition_size = 128` and
    /// `MAX_DEPTH = 9`: the tree hit the cap and the largest leaf reached **476
    /// rows**, 3.7x the limit, which every query in that region scans.
    ///
    /// Nothing else in the tree needs to know: the sibling is an extra child of
    /// the parent at the leaf's own depth, so `find`, the descent, the beam and
    /// the chunk writer already handle a node whose children are at mixed depths
    /// - that is the same `mixed_depth` shape the tree has from the first insert.
    ///
    /// Gives up, leaving the leaf overfull, when the parent already has
    /// [`FANOUT`] children or when k-means cannot cut the rows in two: both mean
    /// the second half has nowhere to go.
    fn split_sideways(
        &mut self,
        idx: usize,
        source: Option<RowSource<'_>>,
        slots: Option<SlotSource<'_>>,
    ) -> Result<(), SpannError> {
        let (key, depth) = {
            let part = &self.parts[idx];
            (part.key, part.depth)
        };
        let Some(parent_key) = self.parts[idx].parent else {
            return Ok(());
        };
        let Some(parent_pos) = self.find(parent_key) else {
            return Ok(());
        };
        if self.parts[parent_pos].children.len() >= FANOUT {
            return Ok(());
        }

        let (vectors, ids) = self.gather_rows(idx, source, slots);
        if vectors.len() < 2 {
            return Ok(());
        }
        let refs: Vec<&[f32]> = vectors.iter().map(|v| v.as_slice()).collect();
        let (_, assign) = kmeans_k(
            &refs,
            self.dim,
            2,
            self.opts.kmeans_iterations,
            self.opts.seed ^ key.wrapping_mul(0x9E37_79B9_7F4A_7C15),
        );
        let mut halves: [Vec<(u64, &[f32])>; 2] = [Vec::new(), Vec::new()];
        for (i, slot) in assign.iter().enumerate() {
            halves[*slot as usize].push((ids[i], vectors[i].as_slice()));
        }
        if halves[0].is_empty() || halves[1].is_empty() {
            return Ok(());
        }

        let sibling_key = self.alloc_key();
        let kept = self.make_leaf(key, depth, Some(parent_key), &halves[0]);
        let sibling = self.make_leaf(sibling_key, depth, Some(parent_key), &halves[1]);
        self.parts[parent_pos].children.push(sibling_key);
        // `kept` goes in before the push so the index stays valid, and the
        // sibling is appended: its key is the largest in the tree.
        self.parts[idx] = kept;
        self.parts.push(sibling);
        self.dirty.insert(key);
        self.dirty.insert(sibling_key);
        self.dirty.insert(parent_key);
        self.splits += 1;
        Ok(())
    }

    /// Builds a frozen leaf from `(id, vector)` pairs.
    fn make_leaf(
        &self,
        key: u64,
        depth: u32,
        parent: Option<u64>,
        items: &[(u64, &[f32])],
    ) -> IncPartition {
        let mut centroid = vec![0.0f32; self.dim];
        for (_, v) in items {
            for (o, x) in centroid.iter_mut().zip(*v) {
                *o += *x;
            }
        }
        let inv = 1.0 / items.len() as f32;
        for o in &mut centroid {
            *o *= inv;
        }
        let mut part =
            IncPartition::new_leaf(key, depth, parent, centroid, &self.rot, self.dim, false);
        let mut radius = 0.0f32;
        for (id, v) in items {
            radius = radius.max(l2_sq(v, &part.centroid).sqrt());
            let code = quantize::encode(&self.rot, &part.rot_centroid, v);
            part.push_member(*id, code);
        }
        part.fold_pending();
        part.radius = radius;
        part
    }

    /// Index in `parts` of `key`.
    fn find(&self, key: u64) -> Option<usize> {
        self.parts.binary_search_by_key(&key, |p| p.key).ok()
    }

    /// Descends to the leaf that owns `vector`, choosing the nearest child
    /// centroid at every level.
    fn leaf_for(&self, vector: &[f32]) -> usize {
        let mut idx = 0usize;
        loop {
            let part = &self.parts[idx];
            if part.children.is_empty() {
                return idx;
            }
            let mut best = usize::MAX;
            let mut best_d = f32::INFINITY;
            for &ck in &part.children {
                let Some(ci) = self.find(ck) else {
                    continue;
                };
                let d = l2_sq(vector, &self.parts[ci].centroid);
                if d < best_d {
                    best_d = d;
                    best = ci;
                }
            }
            if best == usize::MAX {
                return idx;
            }
            idx = best;
        }
    }

    /// Beam search over the tree, then a code scan of every visited leaf.
    ///
    /// Leaves at *different* depths are scanned as they are reached, so a tree
    /// that is still shallow on one side and deep on the other answers
    /// correctly (the bulk builder's `mixed_depth` test has the same rule).
    pub fn search(&self, query: &[f32], k: usize, beam: usize) -> Vec<(u64, f32)> {
        self.search_masked(query, k, beam, None)
    }

    pub fn search_masked(
        &self,
        query: &[f32],
        k: usize,
        beam: usize,
        allowed: Option<&std::collections::HashSet<u64>>,
    ) -> Vec<(u64, f32)> {
        if self.parts.is_empty() || query.len() != self.dim {
            return Vec::new();
        }
        // Same walk as the published tree (`SpannIndex::search_masked`): best
        // first, ranked by centroid distance, `beam` = number of leaves scanned.
        // A per-level width loses to the tree's depth - `IncIndex` grows by
        // 2-way splits, so at 100k x 1536d it is 9 levels and the width reaches
        // the leaves with `beam / 2^height` of the space.
        let budget = beam.max(1);
        let mut out = SearchSet::with_default_capacity(k);
        let mut queue: std::collections::BinaryHeap<super::search::Candidate> =
            std::collections::BinaryHeap::new();
        queue.push(super::search::Candidate {
            bound: 0.0,
            lower: 0.0,
            key: 0,
        });
        let mut scanned = 0usize;
        while let Some(node) = queue.pop() {
            if self.opts.metric.is_l2() {
                if let Some(kth) = out.kth_upper() {
                    if !super::search::l2_can_beat(node.lower, kth) {
                        continue;
                    }
                }
            }
            let Some(idx) = self.find(node.key) else {
                continue;
            };
            if self.parts[idx].children.is_empty() {
                if scanned >= budget {
                    break;
                }
                self.scan_leaf(idx, query, &mut out, allowed);
                scanned += 1;
                continue;
            }
            for child in &self.parts[idx].children {
                if let Some(child_idx) = self.find(*child) {
                    let part = &self.parts[child_idx];
                    let distance = l2_sq(query, &part.centroid).max(0.0).sqrt();
                    queue.push(super::search::Candidate {
                        bound: distance,
                        lower: super::search::radius_lower(distance, part.radius, self.opts.metric),
                        key: *child,
                    });
                }
            }
        }
        out.into_sorted()
    }

    fn scan_leaf(
        &self,
        idx: usize,
        query: &[f32],
        out: &mut SearchSet,
        allowed: Option<&std::collections::HashSet<u64>>,
    ) {
        let part = &self.parts[idx];
        if let Some(raw) = part.raw.as_ref() {
            // The un-split root keeps the rows themselves, so it is measured
            // exactly - the reason its centroid never has to follow the data.
            // `Partition::search` does the same for a partition it has raw rows
            // for (`raw_member`), and Cockroach's root partition is the one the
            // store encodes with `UnQuantizer`.
            let query_norm = l2_sq(query, query);
            for (at, chunk) in raw.chunks_exact(self.dim).enumerate() {
                let Some(id) = part.raw_ids.get(at) else {
                    continue;
                };
                if let Some(allowed) = allowed {
                    if !allowed.contains(id) {
                        continue;
                    }
                }
                let squared = l2_sq(query, chunk);
                let (score, bound) = metric_score(
                    self.opts.metric,
                    squared,
                    0.0,
                    query_norm,
                    l2_sq(chunk, chunk),
                );
                out.add(score, bound, *id);
            }
            return;
        }
        let qc = quantize::make_query(&self.rot, &part.rot_centroid, query);
        for member in part.members.iter().chain(part.pending.iter()) {
            if let Some(allowed) = allowed {
                if !allowed.contains(&member.id) {
                    continue;
                }
            }
            let squared = quantize::distance_estimate(&qc, &member.code, self.dim);
            let err = quantize::error_bound(&qc, &member.code);
            let (score, bound) = metric_score(
                self.opts.metric,
                squared,
                err,
                qc.raw_norm,
                member.code.raw_norm,
            );
            out.add(score, bound, member.id);
        }
    }

    /// Every row id in the index, ascending. Used by the conformance tests and
    /// by the coverage check (`uncovered = live − covered`).
    pub fn row_ids(&self) -> Vec<u64> {
        let mut ids: Vec<u64> = self
            .parts
            .iter()
            .filter(|p| p.is_leaf())
            .flat_map(|p| match p.raw {
                Some(_) => p.raw_ids.clone(),
                None => p
                    .members
                    .iter()
                    .chain(p.pending.iter())
                    .map(|m| m.id)
                    .collect(),
            })
            .collect();
        ids.sort_unstable();
        ids
    }

    /// Leaves that hold pending rows — the flush unit (§0.1: "rewrite only the
    /// chunks that own a dirty leaf").
    pub fn dirty_leaves(&self) -> Vec<u64> {
        self.parts
            .iter()
            .filter(|p| p.is_leaf() && (p.mutable || !p.pending.is_empty()))
            .map(|p| p.key)
            .collect()
    }

    /// Folds every leaf's pending rows, making the tree quiescent.
    pub fn flush_pending(&mut self) {
        for part in &mut self.parts {
            part.fold_pending();
        }
    }

    /// Removes a row from whichever leaf holds it. `O(partitions)` today; the
    /// chunk store deletes via the segment bitmap instead (§9.8), so this exists
    /// for tests and for `optimize` passes.
    pub fn remove(&mut self, row_id: u64) -> bool {
        for part in &mut self.parts {
            if !part.is_leaf() {
                continue;
            }
            let before = part.len();
            if let Some(raw) = part.raw.take() {
                // The un-split root: rebuild the row buffer without this id.
                let dim = part.dim;
                let mut kept = Vec::with_capacity(raw.len());
                let mut kept_ids = Vec::with_capacity(part.raw_ids.len());
                for (at, id) in part.raw_ids.iter().enumerate() {
                    if *id == row_id {
                        continue;
                    }
                    kept.extend_from_slice(&raw[at * dim..(at + 1) * dim]);
                    kept_ids.push(*id);
                }
                part.raw = Some(kept);
                part.raw_ids = kept_ids;
            }
            part.members.retain(|m| m.id != row_id);
            part.pending.retain(|m| m.id != row_id);
            let removed = before - part.len();
            if removed > 0 {
                self.rows -= removed;
                let key = part.key;
                self.queue_fixup(key);
                return true;
            }
        }
        false
    }
}

/// SplitMix64. Deterministic, so replaying the same insert sequence produces
/// the same tree (the bulk builder has the same requirement, `build_is_deterministic`).
struct Rng(u64);

impl Rng {
    fn new(seed: u64) -> Self {
        Self(seed | 1)
    }

    fn next(&mut self) -> u64 {
        self.0 = self.0.wrapping_add(0x9E37_79B9_7F4A_7C15);
        let mut z = self.0;
        z = (z ^ (z >> 30)).wrapping_mul(0xBF58_476D_1CE4_E5B9);
        z = (z ^ (z >> 27)).wrapping_mul(0x94D0_49BB_1331_11EB);
        z ^ (z >> 31)
    }

    fn next_f64(&mut self) -> f64 {
        (self.next() >> 11) as f64 / (1u64 << 53) as f64
    }
}

/// Lloyd's k-means with k-means++ seeding; returns `(centroids, assignment)`.
///
/// Why not reuse [`super::kmeans::balanced_kmeans_2`]: a binary cut makes the
/// tree `log2(leaves)` deep, and a beam of 8 cannot cover `2^k` leaves, so
/// recall collapsed to ~0.55 on a 600-row test. Cutting into up to `FANOUT`
/// groups keeps the depth at 2–3 for the whole 8.4 M-row segment.
///
/// Empty clusters are repaired by stealing a point from the largest cluster, so
/// a split never yields an empty leaf (which would split again on the very next
/// insert, forever).
fn kmeans_k(
    points: &[&[f32]],
    dim: usize,
    k: usize,
    iterations: usize,
    seed: u64,
) -> (Vec<Vec<f32>>, Vec<u32>) {
    let n = points.len();
    let k = k.min(n).max(1);
    let mut rng = Rng::new(seed);

    // k-means++ seeding: first centre uniformly, the rest proportional to D².
    let mut centroids: Vec<Vec<f32>> = Vec::with_capacity(k);
    centroids.push(points[(rng.next() % n as u64) as usize].to_vec());
    let mut nearest = vec![f32::INFINITY; n];
    while centroids.len() < k {
        let last = centroids.last().expect("non-empty by construction");
        let mut total = 0.0f64;
        for (i, p) in points.iter().enumerate() {
            let d = l2_sq(p, last);
            if d < nearest[i] {
                nearest[i] = d;
            }
            total += nearest[i] as f64;
        }
        if total <= 0.0 {
            // Every point coincides with a centre already chosen.
            centroids.push(points[(rng.next() % n as u64) as usize].to_vec());
            continue;
        }
        let mut target = rng.next_f64() * total;
        let mut pick = n - 1;
        for (i, d) in nearest.iter().enumerate() {
            target -= *d as f64;
            if target <= 0.0 {
                pick = i;
                break;
            }
        }
        centroids.push(points[pick].to_vec());
    }

    let mut assign = vec![0u32; n];
    for _ in 0..iterations {
        let mut changed = false;
        for i in 0..n {
            let mut best = 0usize;
            let mut best_d = f32::INFINITY;
            for (c, centre) in centroids.iter().enumerate() {
                let d = l2_sq(points[i], centre);
                if d < best_d {
                    best_d = d;
                    best = c;
                }
            }
            if assign[i] as usize != best {
                assign[i] = best as u32;
                changed = true;
            }
        }
        if !changed {
            break;
        }
        let mut sums = vec![vec![0.0f32; dim]; k];
        let mut counts = vec![0usize; k];
        for (i, p) in points.iter().enumerate() {
            let c = assign[i] as usize;
            counts[c] += 1;
            for (o, x) in sums[c].iter_mut().zip(*p) {
                *o += *x;
            }
        }
        for c in 0..k {
            if counts[c] == 0 {
                let (mut big, mut big_n) = (0usize, 0usize);
                for (j, count) in counts.iter().enumerate() {
                    if *count > big_n {
                        big_n = *count;
                        big = j;
                    }
                }
                if big_n > 1 {
                    if let Some(i) = (0..n).find(|i| assign[*i] as usize == big) {
                        assign[i] = c as u32;
                        centroids[c] = points[i].to_vec();
                        counts[c] = 1;
                        counts[big] -= 1;
                    }
                }
                continue;
            }
            let inv = 1.0 / counts[c] as f32;
            for d in 0..dim {
                centroids[c][d] = sums[c][d] * inv;
            }
        }
    }
    (centroids, assign)
}

fn dequantize_member(rot: &Rotation, rot_centroid: &[f32], member: &Member) -> Vec<f32> {
    quantize::dequantize(rot, rot_centroid, &member.code)
}

/// A strided draw off the source: the builder's bootstrap reads the mapped
/// column sequentially, so the sample costs `max` rows of resident memory and no
/// row-id table (`dim` floats each, dropped after the space exists).
fn draw_sample(src: &dyn VectorSource, max: usize) -> Vec<Vec<f32>> {
    let n = src.len();
    if n == 0 || max == 0 {
        return Vec::new();
    }
    let take = n.min(max);
    let stride = n.div_ceil(take).max(1);
    let mut out = Vec::with_capacity(take);
    let mut at = 0usize;
    while at < n && out.len() < take {
        out.push(src.vector(at).to_vec());
        at += stride;
    }
    out
}

fn mean_of(rows: &[Vec<f32>], dim: usize) -> Vec<f32> {
    let mut centroid = vec![0.0f32; dim];
    if rows.is_empty() {
        return centroid;
    }
    for row in rows {
        for (o, x) in centroid.iter_mut().zip(row) {
            *o += *x;
        }
    }
    let inv = 1.0 / rows.len() as f32;
    for o in &mut centroid {
        *o *= inv;
    }
    centroid
}

/// The query side of a leaf's codes, exposed so the file-backed reader can use
/// the same construction as the in-memory path.
pub fn query_code(rot: &Rotation, rot_centroid: &[f32], query: &[f32]) -> QueryCode {
    quantize::make_query(rot, rot_centroid, query)
}

/// `Encode::None` keeps raw `f32` in the code slot; this is the width the
/// incremental tree reports for a member in that mode.
pub fn member_bytes(encode: Encode, dim: usize) -> usize {
    encode.record_bytes(dim) + std::mem::size_of::<u64>()
}

#[cfg(test)]
mod tests {
    use super::*;

    /// A delta chunk that converted a leaf into a branch does **not** rewrite
    /// the root's page - the root's children did not change, only its level did -
    /// so the chain it is read through has a stale `level` on the root and the
    /// new branch carrying the same number. Deriving "the tree has one root, the
    /// highest level" then finds two; the shape has to come from the child edges.
    ///
    /// This is the exact state the eight-checkpoint test reached: keys 1..3 are
    /// leaves of the root, key 4 was a leaf and is now a branch over 5 and 6, and
    /// the root's page says level 2 just like key 4.
    #[test]
    fn a_directory_whose_root_level_is_stale_still_loads() {
        let dim = 8;
        let opts = || Options {
            max_partition_size: 16,
            min_partition_size: 4,
            ..Options::default()
        };
        let nodes: Vec<DurableNode> = [
            (0u32, 2u32, vec![1u32, 2, 3, 4], vec![0.0f32; 8]),
            (1, 1, vec![], vec![1.0; 8]),
            (2, 1, vec![], vec![2.0; 8]),
            (3, 1, vec![], vec![3.0; 8]),
            (4, 2, vec![5u32, 6], vec![4.0; 8]),
            (5, 1, vec![], vec![5.0; 8]),
            (6, 1, vec![], vec![6.0; 8]),
        ]
        .into_iter()
        .map(|(key, level, children, centroid)| DurableNode {
            key,
            level,
            members: if children.is_empty() { 10 } else { children.len() },
            radius: 1.0,
            children,
            centroid,
        })
        .collect();

        let live = IncIndex::from_durable(dim, opts(), nodes).expect("one root, read from the edges");
        assert_eq!(live.len(), 50, "five reachable leaves of ten rows");
        assert_eq!(live.parts().len(), 7);
        let root = live.find(u64::from(ROOT_KEY as u32)).expect("key 0");
        assert_eq!(live.parts()[root].depth, 0);
        let branch = live.find(4).expect("the converted leaf");
        assert_eq!(
            live.parts()[branch].depth, 1,
            "the branch sits where its parent put it"
        );
        for key in [5u64, 6] {
            let at = live.find(key).expect("a new leaf");
            assert_eq!(live.parts()[at].depth, 2);
        }
    }

    /// A page an older layer still names but the current parent no longer does -
    /// a leaf a merge retired - is kept (its key must stay resolvable) and does
    /// not become a root.
    #[test]
    fn a_retired_page_is_kept_and_is_not_a_root() {
        let dim = 4;
        let nodes: Vec<DurableNode> = [
            (0u32, 2u32, vec![1u32, 2], vec![0.0f32; 4]),
            (1, 1, vec![], vec![1.0; 4]),
            (2, 1, vec![], vec![2.0; 4]),
            // Key 3 was retired: no parent lists it any more.
            (3, 1, vec![], vec![3.0; 4]),
        ]
        .into_iter()
        .map(|(key, level, children, centroid)| DurableNode {
            key,
            level,
            members: 3,
            radius: 1.0,
            children,
            centroid,
        })
        .collect();

        let live = IncIndex::from_durable(dim, Options::default(), nodes).expect("loads");
        assert!(live.find(3).is_some(), "the retired page stays resolvable");
        assert_eq!(
            live.len(),
            6,
            "only the reachable leaves' rows are the tree's"
        );
    }

    /// Two clusters at ±2 with **signed** per-dimension jitter.
    ///
    /// The sign spread matters: 1-bit RaBitQ keeps only the sign of each
    /// component relative to the centroid, so an all-positive data set (the
    /// first version of this helper) gives every row nearly the same code and
    /// no split can separate them. Real embeddings are zero-mean; this mirrors
    /// that.
    fn points(n: usize, dim: usize) -> Vec<(u64, Vec<f32>)> {
        (0..n)
            .map(|i| {
                let mut v = Vec::with_capacity(dim);
                for d in 0..dim {
                    let centre = if i % 2 == 0 { -2.0 } else { 2.0 };
                    let jitter = (((i * 131 + d * 17) % 199) as f32 / 199.0 - 0.5) * 2.0;
                    v.push(centre + jitter);
                }
                (i as u64, v)
            })
            .collect()
    }

    /// Zero-mean rows: every dimension changes sign from row to row, which is
    /// what a 1-bit code can separate.
    ///
    /// [`points`] cannot answer a `k = 1` self-query: inside a cluster its
    /// `±2` centre plus `±1` jitter never crosses zero, so the residual is
    /// dominated by a direction every row shares, and `(i − j)·131 ≡ ±1
    /// (mod 199)` additionally puts exact near-duplicate rows 0.11 apart. Both
    /// make the true nearest neighbour of a row *not* the row itself.
    fn noise_points(n: usize, dim: usize) -> Vec<(u64, Vec<f32>)> {
        let mut state = 0x2545_F491_4F6C_DD1Du64;
        let mut next = move || {
            state = state
                .wrapping_mul(6364136223846793005)
                .wrapping_add(1442695040888963407);
            (state >> 40) as f32 / (1u64 << 24) as f32
        };
        (0..n)
            .map(|i| {
                // Two uniforms: zero mean, triangular, sign changes per row.
                let v: Vec<f32> = (0..dim).map(|_| next() + next() - 1.0).collect();
                (i as u64, v)
            })
            .collect()
    }

    /// A clustered fixture: `clusters` centres with `noise` jitter around them,
    /// which is what a SPANN tree is for (a leaf is only useful when its
    /// centroid says something about where its rows are).
    fn clustered_points(n: usize, dim: usize, clusters: usize, noise: f32) -> Vec<(u64, Vec<f32>)> {
        let mut state = 0x2545_F491_4F6C_DD1Du64;
        let mut next = move || {
            state = state
                .wrapping_mul(6364136223846793005)
                .wrapping_add(1442695040888963407);
            (state >> 40) as f32 / (1u64 << 24) as f32
        };
        let centres: Vec<Vec<f32>> = (0..clusters)
            .map(|_| (0..dim).map(|_| next() * 2.0 - 1.0).collect())
            .collect();
        (0..n)
            .map(|i| {
                let centre = &centres[i % clusters];
                let v: Vec<f32> = centre.iter().map(|x| x + (next() - 0.5) * noise).collect();
                (i as u64, v)
            })
            .collect()
    }

    /// Rows on a low-dimensional manifold: `intrinsic` orthonormal directions in
    /// a `dim`-dimensional space, plus a little noise off the manifold.
    ///
    /// This is the structure a real embedding has (a 1536d BERT vector does not
    /// fill its 1536 dimensions), and it is what makes an inverted tree worth
    /// having: the ambient dimension sets the code width, the *intrinsic* one
    /// sets how far apart the neighbours are. Isotropic noise at the ambient
    /// dimension has no neighbourhoods to find at 128d - every pair of rows is
    /// roughly as far apart as the leaf radius, so no code can rank them.
    fn manifold_points(n: usize, dim: usize, intrinsic: usize, noise: f32) -> Vec<(u64, Vec<f32>)> {
        let mut state = 0x2545_F491_4F6C_DD1Du64;
        let mut next = move || {
            state = state
                .wrapping_mul(6364136223846793005)
                .wrapping_add(1442695040888963407);
            (state >> 40) as f32 / (1u64 << 24) as f32
        };
        let basis: Vec<Vec<f32>> = (0..intrinsic)
            .map(|_| {
                let mut v: Vec<f32> = (0..dim).map(|_| next() * 2.0 - 1.0).collect();
                let norm = v.iter().map(|x| x * x).sum::<f32>().sqrt();
                for x in &mut v {
                    *x /= norm;
                }
                v
            })
            .collect();
        (0..n)
            .map(|i| {
                let mut v = vec![0.0f32; dim];
                for axis in &basis {
                    let c = next() * 2.0 - 1.0;
                    for (x, b) in v.iter_mut().zip(axis) {
                        *x += c * b;
                    }
                }
                for x in &mut v {
                    *x += (next() - 0.5) * noise;
                }
                (i as u64, v)
            })
            .collect()
    }

    /// Inserts `pts` through `idx`, keeping each insert's latency: the point of
    /// a realtime mode is the tail, not the total.
    fn timed_inserts(
        idx: &mut IncIndex,
        pts: &[(u64, Vec<f32>)],
        lookup: &dyn Fn(u64) -> Option<Vec<f32>>,
    ) -> (Vec<u32>, std::time::Duration) {
        let mut micros = Vec::with_capacity(pts.len());
        let mut fixups = std::time::Duration::ZERO;
        for (id, v) in pts {
            let at = std::time::Instant::now();
            idx.insert(*id, v).unwrap();
            micros.push(at.elapsed().as_micros().min(u32::MAX as u128) as u32);
            // The batch boundary: the drain is where the split cost lands, which
            // is why the insert percentiles must not include it.
            if idx.pending_fixups() > 0 {
                let at = std::time::Instant::now();
                idx.run_fixups(Some(lookup));
                fixups += at.elapsed();
            }
        }
        (micros, fixups)
    }

    /// p50, p95, p99, max of a latency sample.
    fn percentiles(mut micros: Vec<u32>) -> (u32, u32, u32, u32) {
        assert!(!micros.is_empty());
        micros.sort_unstable();
        let at = |q: f64| {
            let last = micros.len() - 1;
            micros[((last as f64) * q).round() as usize]
        };
        (at(0.5), at(0.95), at(0.99), *micros.last().unwrap())
    }

    fn opts() -> Options {
        Options {
            max_partition_size: 64,
            min_partition_size: 8,
            kmeans_iterations: 8,
            encode: Encode::Rabitq,
            ..Options::default()
        }
    }

    /// Builds an index with a row source, so a split re-encodes the rows and
    /// not their 1-bit reconstructions.
    fn build_with(
        pts: Vec<(u64, Vec<f32>)>,
        dim: usize,
        opts: Options,
    ) -> (Vec<(u64, Vec<f32>)>, IncIndex) {
        let by_id: std::collections::HashMap<u64, Vec<f32>> =
            pts.iter().map(|(id, v)| (*id, v.clone())).collect();
        let source = move |id: u64| by_id.get(&id).cloned();
        let mut idx = IncIndex::new(dim, opts).unwrap();
        for (id, v) in &pts {
            idx.insert(*id, v).unwrap();
            if idx.pending_fixups() > 0 {
                idx.run_fixups(Some(&source));
            }
        }
        idx.run_fixups(Some(&source));
        (pts, idx)
    }

    /// Keys are allocated in creation order and a split never renumbers one -
    /// the property the dirty-leaf dump depends on, because a delta chunk names
    /// the same nodes the base chunk does.
    #[test]
    fn keys_are_allocated_in_creation_order_and_never_renumbered() {
        let pts = points(400, 8);
        let mut idx = IncIndex::new(8, opts()).unwrap();
        for (id, v) in &pts {
            idx.insert(*id, v).unwrap();
        }
        idx.run_fixups(None);
        assert!(idx.splits() > 0, "the fixture has to split");

        let keys: Vec<u64> = idx.parts().iter().map(|part| part.key).collect();
        let mut sorted = keys.clone();
        sorted.sort_unstable();
        assert_eq!(keys, sorted, "a key above every existing one is a push");
        assert_eq!(keys[0], ROOT_KEY, "the root is allocated first");
        assert_eq!(keys, (0..keys.len() as u64).collect::<Vec<_>>());

        // Every child's key is larger than its parent's, and the parent's
        // `children` names exactly the nodes that point back at it.
        for part in idx.parts() {
            for child in &part.children {
                assert!(*child > part.key, "child {child} of {}", part.key);
                let at = idx.find(*child).expect("a child is in the tree");
                assert_eq!(idx.parts()[at].parent, Some(part.key));
            }
            if part.key != ROOT_KEY {
                let parent = idx
                    .find(part.parent.expect("a non-root has a parent"))
                    .expect("the parent is in the tree");
                assert!(idx.parts()[parent].children.contains(&part.key));
            }
        }

        // Splitting again leaves every existing key where it was.
        let before: Vec<u64> = idx.parts().iter().map(|part| part.key).collect();
        for i in 400..600u64 {
            let (_, v) = &pts[i as usize % pts.len()];
            idx.insert(i, v).unwrap();
        }
        idx.run_fixups(None);
        for key in before {
            assert!(idx.find(key).is_some(), "key {key} disappeared");
        }
    }

    #[test]
    fn every_inserted_row_is_reachable() {
        let pts = points(400, 8);
        let mut idx = IncIndex::new(8, opts()).unwrap();
        for (id, v) in &pts {
            idx.insert(*id, v).unwrap();
        }
        idx.run_fixups(None);
        let ids = idx.row_ids();
        assert_eq!(ids.len(), pts.len());
        assert_eq!(ids.first().copied(), Some(0));
        assert!(idx.splits() > 0, "400 rows at 16 per leaf must split");
    }

    /// Recall@10 by **distance**, not by id.
    ///
    /// `points()` holds exact near-duplicate pairs (`(i − j)·131 ≡ ±1 (mod 199)`
    /// puts two rows 0.11 apart), so the tenth and eleventh neighbour are often
    /// the same distance from the query: asking for the same *ids* scores a
    /// correct answer as a miss. The k-th true distance is the bar the returned
    /// rows have to clear, which is what an ANN benchmark means by recall.
    #[test]
    fn incremental_recall_matches_the_exact_top_k() {
        let (pts, idx) = build_with(points(600, 128), 128, opts());
        let mut hits = 0usize;
        let queries = 20;
        for qi in 0..queries {
            let q = &pts[qi * 7 % pts.len()].1;
            let got = idx.search(q, 10, 8);
            let mut all: Vec<f32> = pts.iter().map(|(_, v)| l2_sq(q, v)).collect();
            all.sort_by(f32::total_cmp);
            let bar = all[9] * 1.001 + 1e-3;
            hits += got
                .iter()
                .filter(|(id, _)| {
                    pts.iter()
                        .find(|(other, _)| other == id)
                        .is_some_and(|(_, v)| l2_sq(q, v) <= bar)
                })
                .count();
        }
        let recall = hits as f32 / (queries * 10) as f32;
        assert!(recall >= 0.9, "recall {recall} too low");
    }

    /// The chunk writer publishes the bulk tree, so the incremental one has to
    /// translate without losing a row: DFS-ordinal keys, levels numbered
    /// upward, pending rows folded into their leaf.
    #[test]
    fn to_spann_publishes_every_row() {
        let (pts, idx) = build_with(noise_points(300, 128), 128, opts());
        assert!(
            idx.splits() > 0,
            "the fixture has to split to be interesting"
        );
        let spann = idx.to_spann().expect("a built index publishes");
        assert_eq!(spann.len(), idx.len());
        assert_eq!(spann.partitions().len(), idx.parts().len());
        // The format's height is the root's level, which is not the deepest
        // node's depth: a leaf this tree never split can sit above the bottom.
        assert!(spann.height() <= idx.height());
        let mut ids = spann.row_ids();
        ids.sort_unstable();
        assert_eq!(ids, idx.row_ids(), "the same rows, no duplicates");

        // Keys are the DFS ordinals the format addresses, and a level counts up
        // from the leaf (the chunk's convention, not the incremental `depth`).
        for (at, part) in spann.partitions().iter().enumerate() {
            assert_eq!(part.key, at as PartitionKey, "keys are 0..n");
            if part.is_leaf() {
                assert_eq!(part.level, 1);
            } else {
                for child in &part.children {
                    // Strictly fewer levels to a leaf - not exactly one less: a
                    // leaf this tree never split can hang directly off a node
                    // two levels up (`mixed_depth`).
                    assert!(
                        spann.partitions()[*child as usize].level < part.level,
                        "a child is closer to the leaves than its parent"
                    );
                }
            }
        }
        assert_eq!(
            spann.partitions()[0].level,
            spann.height(),
            "root is the top"
        );

        // Every row still finds itself through the published tree.
        let mut hit = 0usize;
        for (id, v) in &pts {
            if spann.search(v, 1, Some(64)).first().map(|(g, _)| *g) == Some(*id) {
                hit += 1;
            }
        }
        assert!(hit >= 285, "published self-query {hit}/300");
    }

    /// No split at all: no descent and exactly one leaf, so a miss can only
    /// come from the encoder or the estimator. This is the isolation test for
    /// "does a 1-bit code rank the row itself first" — `points()` cannot answer
    /// it (see `noise_points`).
    #[test]
    fn self_query_without_any_split() {
        let (pts, idx) = build_with(
            noise_points(200, 128),
            128,
            Options {
                max_partition_size: 4096,
                ..opts()
            },
        );
        assert_eq!(idx.splits(), 0);
        let mut hit = 0usize;
        for (id, v) in &pts {
            if idx.search(v, 1, 64).first().map(|(g, _)| *g) == Some(*id) {
                hit += 1;
            }
        }
        assert!(hit >= 190, "no-split self-query {hit}/200");
    }

    /// A row must find itself with a wide beam. This separates "the descent lost
    /// the leaf" from "1-bit codes cannot rank nearby rows" — the latter is
    /// expected at low dimension, the former is a bug.
    ///
    /// It also pins the split path: without a row source every leaf below the
    /// root holds re-encoded *reconstructions* and this drops to 116/300.
    #[test]
    fn wide_beam_finds_every_row_itself() {
        let (pts, idx) = build_with(noise_points(300, 128), 128, opts());
        let mut hit = 0usize;
        for (id, v) in &pts {
            if idx.search(v, 1, 64).first().map(|(g, _)| *g) == Some(*id) {
                hit += 1;
            }
        }
        assert!(hit >= 285, "self-query only {hit}/300");
    }

    /// The un-split root neither re-centroids nor re-encodes on insert.
    ///
    /// That is deliberate and it is what the references do: spfresh appends a
    /// full-precision record and stops, Cockroach's root partition is the one
    /// partition the store encodes with `UnQuantizer`. What the root gives up in
    /// centroid honesty it takes back by scanning its rows exactly, so the very
    /// first row's vector still ranks first.
    #[test]
    fn the_cold_start_root_is_scanned_exactly() {
        let mut idx = IncIndex::new(4, opts()).unwrap();
        idx.insert(1, &[0.0, 0.0, 0.0, 0.0]).unwrap();
        idx.insert(2, &[2.0, 2.0, 2.0, 2.0]).unwrap();
        {
            let root = &idx.parts()[0];
            assert!(root.mutable);
            assert_eq!(root.len(), 2, "the root's rows count as rows");
            assert!(
                root.centroid.iter().all(|x| *x == 0.0),
                "the second insert must not move the centroid"
            );
            assert!(
                root.members.is_empty() && root.pending.is_empty(),
                "the root must not have encoded anything yet"
            );
        }
        // Exact scan: each row is still its own nearest neighbour.
        for (id, v) in [(1u64, [0.0f32; 4]), (2, [2.0; 4])] {
            assert_eq!(
                idx.search(&v, 1, 8).first().map(|(got, _)| *got),
                Some(id),
                "row {id} must find itself"
            );
        }
        // And publishing encodes them once, on the way out.
        let spann = idx.to_spann().expect("publishes");
        assert_eq!(spann.len(), 2);
    }

    #[test]
    fn filtered_search_only_returns_allowed_rows() {
        let pts = points(300, 8);
        let mut idx = IncIndex::new(8, opts()).unwrap();
        for (id, v) in &pts {
            idx.insert(*id, v).unwrap();
        }
        idx.run_fixups(None);
        let allowed: std::collections::HashSet<u64> = (0..300u64).filter(|i| i % 5 == 0).collect();
        let q = &pts[3].1;
        let got = idx.search_masked(q, 10, 8, Some(&allowed));
        assert!(!got.is_empty());
        for (id, _) in got {
            assert!(allowed.contains(&id), "row {id} is not allowed");
        }
    }

    #[test]
    fn memory_per_row_stays_close_to_the_code_width() {
        let dim = 128;
        let pts = points(2000, dim);
        let mut idx = IncIndex::new(dim, opts()).unwrap();
        for (id, v) in &pts {
            idx.insert(*id, v).unwrap();
        }
        idx.run_fixups(None);
        let per_row = idx.mem_usage() / idx.len().max(1);
        // 128d rabitq: 16 B code + 16 B meta + id, plus centroids amortised.
        assert!(per_row < 200, "per-row {per_row} B is too much for {dim}d");
    }

    /// A leaf that has run out of depth must not keep growing: the size limit is
    /// what bounds a query's scan, and `MAX_DEPTH` alone used to lift it.
    ///
    /// The chain is built by hand because reaching the cap through `insert`
    /// takes the skew only tens of thousands of rows produce - that is where it
    /// showed up (a 476-row leaf under a 128-row limit, see the load test).
    #[test]
    fn a_leaf_at_the_depth_cap_splits_sideways() {
        let dim = 8;
        let opts = Options {
            max_partition_size: 8,
            min_partition_size: 2,
            kmeans_iterations: 4,
            encode: Encode::Rabitq,
            ..Options::default()
        };
        let pts = noise_points(400, dim);
        let source = crate::spann::source::Memory::new(pts.clone());
        let lookup = crate::spann::source::row_source(&source);

        let mut idx = IncIndex::new(dim, opts.clone()).unwrap();
        let rot = idx.rot.clone();
        // One child per level, all centred on the origin: every insert follows
        // the chain, which is what a hot region looks like.
        let mut parts = vec![IncPartition::new_leaf(
            ROOT_KEY,
            0,
            None,
            vec![0.0; dim],
            &rot,
            dim,
            false,
        )];
        let mut key = ROOT_KEY;
        for depth in 0..MAX_DEPTH - 1 {
            let child = idx.alloc_key();
            parts.last_mut().unwrap().children.push(child);
            parts.push(IncPartition::new_leaf(
                child,
                depth + 1,
                Some(key),
                vec![0.0; dim],
                &rot,
                dim,
                false,
            ));
            key = child;
        }
        {
            let bottom = parts.last_mut().unwrap();
            for (id, v) in pts.iter().take(8) {
                let code = quantize::encode(&rot, &bottom.rot_centroid, v);
                bottom.push_member(*id, code);
            }
            bottom.fold_pending();
        }
        idx.parts = parts;
        idx.rows = 8;
        assert_eq!(idx.height(), MAX_DEPTH, "the fixture sits at the cap");
        let before = idx.parts.len();

        // The ninth row overflows the bottom leaf, which has no depth left.
        idx.insert(8, &pts[8].1).unwrap();
        assert_eq!(idx.pending_fixups(), 1, "the overflow is queued, not done");
        idx.run_fixups(Some(&lookup));

        assert_eq!(idx.parts.len(), before + 1, "one sibling, no new level");
        assert_eq!(idx.height(), MAX_DEPTH, "the cap still holds");
        let parent = idx
            .parts
            .iter()
            .position(|p| p.depth == MAX_DEPTH - 2 && !p.is_leaf())
            .expect("the level above the bottom");
        assert_eq!(
            idx.parts[parent].children.len(),
            2,
            "the overflowing leaf got a sibling under its parent"
        );
        for child in &idx.parts[parent].children {
            let at = idx.find(*child).expect("child is in the table");
            assert_eq!(idx.parts[at].depth, MAX_DEPTH - 1);
            assert!(idx.parts[at].is_leaf());
            assert!(
                idx.parts[at].len() <= opts.max_partition_size,
                "leaf {} rows over the limit",
                idx.parts[at].len()
            );
        }
        let mut ids = idx.row_ids();
        ids.sort_unstable();
        assert_eq!(
            ids,
            (0..9).collect::<Vec<u64>>(),
            "no row lost or duplicated"
        );
        // The rows are still findable, and the tree still translates to a chunk.
        assert_eq!(
            idx.search(&pts[8].1, 1, 8).first().map(|(g, _)| *g),
            Some(8)
        );
        assert!(idx.to_spann().is_some());
    }

    /// Load test: 50,000 rows x 128d, eight clusters, built row by row through
    /// the split path. Ignored by default because a debug build takes minutes;
    /// run it with
    ///
    /// ```text
    /// cargo test -p index --release spann::incr::tests::large_scale -- --ignored --nocapture
    /// ```
    ///
    /// It reports the numbers the recall question turns on:
    ///
    /// * **self-query** - can a row find itself (the ghost-row bug scored 39%);
    /// * **candidate recall** - are the true 10 nearest inside the `k + extra`
    ///   the index returns, which is what the executor's exact rescoring
    ///   (`TableHeap`/`BlockVectorSource`) needs (design D1: the index ranks,
    ///   the engine rescores);
    /// * **top-10 recall** - the order the codes alone produce, which is capped
    ///   by 1-bit precision at this dimension and is *not* what the engine ships;
    /// * **bulk vs incremental** - the same rows and options through
    ///   `SpannIndex::build`, so a gap is the incremental path's fault and a tie
    ///   is the fixture's.
    #[test]
    #[ignore = "load test: run with --release --ignored --nocapture"]
    fn large_scale_recall_and_self_query() {
        use std::time::Instant;

        let n = 50_000usize;
        let dim = 128usize;
        let clusters = 8usize;
        let pts = clustered_points(n, dim, clusters, 0.6);
        let opts = Options {
            max_partition_size: 128,
            min_partition_size: 32,
            kmeans_iterations: 8,
            encode: Encode::Rabitq,
            ..Options::default()
        };

        let source = crate::spann::source::Memory::new(pts.clone());
        let lookup = crate::spann::source::row_source(&source);
        let mut idx = IncIndex::new(dim, opts.clone()).unwrap();
        let built = Instant::now();
        for (id, v) in &pts {
            idx.insert(*id, v).unwrap();
            if idx.pending_fixups() > 0 {
                idx.run_fixups(Some(&lookup));
            }
        }
        idx.run_fixups(Some(&lookup));
        let build = built.elapsed();
        let per_row = idx.mem_usage() / idx.len().max(1);
        let biggest = idx
            .parts()
            .iter()
            .filter(|p| p.is_leaf())
            .map(|p| p.len())
            .max()
            .unwrap_or(0);
        eprintln!(
            "built {n} x {dim}d in {build:?}: splits {} parts {} height {} leaves {} biggest leaf {biggest} mem {per_row} B/row",
            idx.splits(),
            idx.parts().len(),
            idx.height(),
            idx.parts().iter().filter(|p| p.is_leaf()).count()
        );

        // The beam is a budget per level, and at this leaf size one visited leaf
        // is ~70 rows: `beam 64` scans half the tree, which is why it is not the
        // number to quote. 16 leaves is ~3% of the tree, the design's share.
        let self_query = Instant::now();
        let sample = 10_000usize;
        let mut found = 0usize;
        for (id, v) in pts.iter().take(sample) {
            if idx.search(v, 1, 16).first().map(|(g, _)| *g) == Some(*id) {
                found += 1;
            }
        }
        eprintln!(
            "self-query {found}/{sample} ({:.1}%) at beam 16, {:.2} ms/query",
            100.0 * found as f32 / sample as f32,
            self_query.elapsed().as_secs_f64() * 1000.0 / sample as f64
        );

        let queries: Vec<&Vec<f32>> = (0..200).map(|q| &pts[q * 191 % n].1).collect();
        let exact: Vec<Vec<u64>> = queries
            .iter()
            .map(|q| {
                let mut all: Vec<(u64, f32)> =
                    pts.iter().map(|(id, v)| (*id, l2_sq(q, v))).collect();
                all.sort_by(|a, b| a.1.total_cmp(&b.1));
                all.into_iter().take(10).map(|(id, _)| id).collect()
            })
            .collect();

        let bulk = SpannIndex::build(pts.clone(), opts.clone()).unwrap();
        eprintln!(
            "bulk tree: parts {} height {}",
            bulk.partitions().len(),
            bulk.height()
        );
        for (name, beam) in [
            ("incremental", 8usize),
            ("incremental", 16),
            ("incremental", 32),
        ] {
            let (mut in_candidates, mut top10) = (0usize, 0usize);
            let budget = 100usize;
            let started = Instant::now();
            for (q, want) in queries.iter().zip(&exact) {
                let got: Vec<u64> = idx
                    .search(q, budget, beam)
                    .into_iter()
                    .map(|(id, _)| id)
                    .collect();
                in_candidates += want.iter().filter(|id| got.contains(id)).count();
                top10 += got.iter().take(10).filter(|id| want.contains(id)).count();
            }
            let asked = queries.len() as f32;
            eprintln!(
                "{name} beam {beam:2}: candidate recall {:.3}, top-10 recall {:.3}, {:.2} ms/query",
                in_candidates as f32 / (asked * 10.0),
                top10 as f32 / (asked * 10.0),
                started.elapsed().as_secs_f64() * 1000.0 / asked as f64
            );
        }
        for beam in [8usize, 32] {
            let (mut in_candidates, mut top10) = (0usize, 0usize);
            for (q, want) in queries.iter().zip(&exact) {
                let got: Vec<u64> = bulk
                    .search(q, 100, Some(beam))
                    .into_iter()
                    .map(|(id, _)| id)
                    .collect();
                in_candidates += want.iter().filter(|id| got.contains(id)).count();
                top10 += got.iter().take(10).filter(|id| want.contains(id)).count();
            }
            let asked = queries.len() as f32;
            eprintln!(
                "bulk        beam {beam:2}: candidate recall {:.3}, top-10 recall {:.3}",
                in_candidates as f32 / (asked * 10.0),
                top10 as f32 / (asked * 10.0)
            );
        }
        assert!(
            found as f32 / sample as f32 >= 0.99,
            "self-query {found}/{sample} at {dim}d"
        );
    }

    /// The same load, on rows that have neighbours: 100,000 x 128d on a 4-dim
    /// manifold. This is the shape of the data the engine actually indexes, and
    /// the test that says the tree + codes deliver - the isotropic fixture above
    /// measures the ambient-dimension ceiling, not the implementation.
    ///
    /// ```text
    /// cargo test -p index --release spann::incr::tests::large_scale_manifold -- --ignored --nocapture
    /// ```
    #[test]
    #[ignore = "load test: run with --release --ignored --nocapture"]
    fn large_scale_manifold_recall() {
        use std::time::Instant;

        let n = 100_000usize;
        let dim = 128usize;
        let pts = manifold_points(n, dim, 4, 0.02);
        let opts = Options {
            max_partition_size: 128,
            min_partition_size: 32,
            kmeans_iterations: 8,
            encode: Encode::Rabitq,
            ..Options::default()
        };
        let source = crate::spann::source::Memory::new(pts.clone());
        let lookup = crate::spann::source::row_source(&source);
        let mut idx = IncIndex::new(dim, opts).unwrap();
        let built = Instant::now();
        let (micros, fixup_time) = timed_inserts(&mut idx, &pts, &lookup);
        let build = built.elapsed();
        let (p50, p95, p99, worst) = percentiles(micros.clone());
        eprintln!(
            "insert latency: p50 {p50} us, p95 {p95} us, p99 {p99} us, max {worst} us; \
             {:.0} inserts/s; fixups {:.2} s total ({:.1}% of the build)",
            n as f64 / build.as_secs_f64(),
            fixup_time.as_secs_f64(),
            100.0 * fixup_time.as_secs_f64() / build.as_secs_f64()
        );
        eprintln!(
            "built {n} x {dim}d (4 intrinsic) in {:?}: splits {} parts {} height {} biggest leaf {} mem {} B/row",
            built.elapsed(),
            idx.splits(),
            idx.parts().len(),
            idx.height(),
            idx.parts()
                .iter()
                .filter(|p| p.is_leaf())
                .map(|p| p.len())
                .max()
                .unwrap_or(0),
            idx.mem_usage() / idx.len().max(1)
        );
        let publish = Instant::now();
        let spann = idx.to_spann().expect("publishes");
        eprintln!(
            "publish: to_spann {:?} for {} partitions / {} rows",
            publish.elapsed(),
            spann.partitions().len(),
            spann.len()
        );

        let sample = 10_000usize;
        let mut found = 0usize;
        for (id, v) in pts.iter().take(sample) {
            if idx.search(v, 1, 16).first().map(|(g, _)| *g) == Some(*id) {
                found += 1;
            }
        }
        eprintln!(
            "self-query {found}/{sample} ({:.1}%)",
            100.0 * found as f32 / sample as f32
        );

        let queries: Vec<&Vec<f32>> = (0..200).map(|q| &pts[q * 97 % n].1).collect();
        let exact: Vec<Vec<u64>> = queries
            .iter()
            .map(|q| {
                let mut all: Vec<(u64, f32)> =
                    pts.iter().map(|(id, v)| (*id, l2_sq(q, v))).collect();
                all.sort_by(|a, b| a.1.total_cmp(&b.1));
                all.into_iter().take(10).map(|(id, _)| id).collect()
            })
            .collect();
        let mut best_candidate = 0.0f32;
        for beam in [8usize, 16, 32] {
            let (mut in_candidates, mut top10) = (0usize, 0usize);
            let started = Instant::now();
            for (q, want) in queries.iter().zip(&exact) {
                let got: Vec<u64> = idx
                    .search(q, 100, beam)
                    .into_iter()
                    .map(|(id, _)| id)
                    .collect();
                in_candidates += want.iter().filter(|id| got.contains(id)).count();
                top10 += got.iter().take(10).filter(|id| want.contains(id)).count();
            }
            let asked = queries.len() as f32;
            let candidate = in_candidates as f32 / (asked * 10.0);
            best_candidate = best_candidate.max(candidate);
            eprintln!(
                "beam {beam:2}: candidate recall {candidate:.3}, top-10 recall {:.3}, {:.2} ms/query",
                top10 as f32 / (asked * 10.0),
                started.elapsed().as_secs_f64() * 1000.0 / asked as f64
            );
        }
        assert!(
            found as f32 / sample as f32 >= 0.99,
            "self-query {found}/{sample} on the manifold"
        );
        assert!(
            best_candidate >= 0.95,
            "candidate recall {best_candidate} on a 4-dim manifold"
        );
    }

    /// A hand-built tree: an interior root with one leaf per entry, so a merge
    /// can be driven one step at a time instead of waiting for one to be earned
    /// by size. Leaves are filled the way a real one is - centroid = mean of the
    /// rows, codes relative to it, radius = max distance.
    fn tree_of_leaves(dim: usize, opts: Options, leaves: &[Vec<(u64, Vec<f32>)>]) -> IncIndex {
        let mut idx = IncIndex::new(dim, opts).unwrap();
        let rot = idx.rot.clone();
        let mut root =
            IncPartition::new_leaf(ROOT_KEY, 0, None, vec![0.0; dim], &rot, dim, false);
        let mut total = 0usize;
        for rows in leaves {
            let key = idx.alloc_key();
            let mut centroid = vec![0.0f32; dim];
            for (_, v) in rows {
                for (o, x) in centroid.iter_mut().zip(v) {
                    *o += *x;
                }
            }
            for o in &mut centroid {
                *o /= rows.len() as f32;
            }
            let mut leaf =
                IncPartition::new_leaf(key, 1, Some(ROOT_KEY), centroid.clone(), &rot, dim, false);
            for (id, v) in rows {
                let code = quantize::encode(&rot, &leaf.rot_centroid, v);
                leaf.push_member(*id, code);
                leaf.radius = leaf.radius.max(l2_sq(v, &centroid).sqrt());
            }
            leaf.fold_pending();
            total += leaf.len();
            root.children.push(key);
            idx.parts.push(leaf);
        }
        idx.parts.insert(0, root);
        idx.rows = total;
        idx
    }

    /// `tree_of_leaves` allocates one key per entry, in order, from `ROOT_KEY+1`.
    fn leaf_key(slot: u64) -> u64 {
        ROOT_KEY + 1 + slot
    }

    fn cluster_at(centre: f32, n: usize, dim: usize, id0: u64) -> Vec<(u64, Vec<f32>)> {
        (0..n)
            .map(|i| {
                let v = (0..dim)
                    .map(|d| centre + (((i * 31 + d * 7) % 13) as f32 / 13.0 - 0.5) * 0.2)
                    .collect();
                (id0 + i as u64, v)
            })
            .collect()
    }

    #[test]
    fn a_bootstrapped_tree_absorbs_every_row_it_is_given() {
        let dim = 32;
        let pts = noise_points(1200, dim);
        let src = super::super::source::Memory::new(pts.clone());
        let opts = Options {
            kmeans_iterations: 4,
            ..opts()
        };
        let mut idx = IncIndex::bootstrap(&src, dim, opts).unwrap();
        assert!(
            idx.parts().len() > 2,
            "the sample has to shape the routing levels"
        );
        assert_eq!(idx.len(), 0, "a bootstrap holds no rows");
        assert!(
            idx.parts().iter().all(|p| !p.is_leaf() || p.is_empty()),
            "bootstrap stops at empty leaves"
        );

        let rows = super::super::source::row_source(&src);
        for (id, v) in &pts {
            idx.insert(*id, v).unwrap();
            if idx.pending_fixups() > 0 {
                idx.run_fixups(Some(&rows));
            }
        }
        idx.run_fixups(Some(&rows));

        assert_eq!(idx.len(), 1200);
        let ids = idx.row_ids();
        let unique: std::collections::HashSet<u64> = ids.iter().copied().collect();
        assert_eq!(unique.len(), 1200, "every row is stored exactly once");
        let hit = pts
            .iter()
            .filter(|(id, v)| idx.search(v, 1, 64).first().map(|(got, _)| *got) == Some(*id))
            .count();
        assert!(
            hit >= 1140,
            "self-query {hit}/1200 through a bootstrapped space"
        );
    }

    #[test]
    fn a_bootstrapped_space_does_not_outgrow_its_sample() {
        let dim = 8;
        let pts = noise_points(BOOTSTRAP_SAMPLE_ROWS * 3, dim);
        let src = super::super::source::Memory::new(pts);
        let idx = IncIndex::bootstrap(&src, dim, opts()).unwrap();
        let leaves = idx.parts().iter().filter(|p| p.is_leaf()).count();
        assert!(leaves >= 2, "the sample has to produce a space");
        assert!(
            leaves <= BOOTSTRAP_SAMPLE_ROWS / BOOTSTRAP_ROWS_PER_CHILD,
            "{leaves} leaves from a {BOOTSTRAP_SAMPLE_ROWS}-row sample"
        );
        assert!(
            idx.height() <= 3,
            "the bootstrap only fixes the upper levels (height {}); splits grow the rest",
            idx.height()
        );
    }

    #[test]
    fn a_tree_publishes_through_into_spann_without_copying() {
        let dim = 32;
        let pts = noise_points(600, dim);
        let rows = super::super::source::Memory::new(pts.clone());
        let mut idx = IncIndex::new(dim, opts()).unwrap();
        let lookup = super::super::source::row_source(&rows);
        for (id, v) in &pts {
            idx.insert(*id, v).unwrap();
            if idx.pending_fixups() > 0 {
                idx.run_fixups(Some(&lookup));
            }
        }
        idx.run_fixups(Some(&lookup));

        let published = idx.into_spann().expect("a non-empty tree publishes");

        assert_eq!(published.len(), 600);
        assert_eq!(published.row_ids().len(), 600);
        assert!(
            published.height() >= 2,
            "600 rows over 64-row partitions needs a real tree"
        );
    }

    #[test]
    fn a_table_smaller_than_one_partition_needs_no_bootstrap() {
        let dim = 8;
        let pts = noise_points(10, dim);
        let src = super::super::source::Memory::new(pts);
        let idx = IncIndex::bootstrap(&src, dim, opts()).unwrap();
        assert!(
            idx.parts().is_empty(),
            "one partition's worth of rows needs no space"
        );
    }

    #[test]
    fn a_merge_moves_every_row_into_a_leaf_sibling() {
        let dim = 8;
        let src = cluster_at(0.0, 4, dim, 0);
        let dst = cluster_at(0.1, 10, dim, 100);
        let mut idx = tree_of_leaves(dim, opts(), &[src.clone(), dst.clone()]);
        let source = {
            let by_id: std::collections::HashMap<u64, Vec<f32>> = src
                .iter()
                .chain(dst.iter())
                .map(|(id, v)| (*id, v.clone()))
                .collect();
            move |id: u64| by_id.get(&id).cloned()
        };
        let key = leaf_key(0);
        let at = idx.find(key).unwrap();
        let rows_before = idx.row_ids().len();
        let parent_children = idx.parts[0].children.len();

        idx.merge(at, Some(&source), None).unwrap();

        assert_eq!(idx.merges(), 1, "the merge has to happen");
        assert_eq!(idx.parts[0].children.len(), parent_children - 1);
        assert!(idx.find(key).is_none(), "the source leaf is detached");
        let dst_at = idx.find(leaf_key(1)).unwrap();
        assert_eq!(idx.parts[dst_at].len(), 14, "4 rows landed in the sibling");
        let ids = idx.row_ids();
        assert_eq!(ids.len(), rows_before, "no row may disappear");
        for (id, _) in src.iter().chain(dst.iter()) {
            assert!(ids.contains(id), "row {id} survived the merge");
        }
        assert_eq!(
            idx.len(),
            rows_before,
            "the row counter is not touched by a merge"
        );
    }

    #[test]
    fn a_merge_is_refused_when_every_sibling_is_full() {
        let dim = 8;
        let src = cluster_at(0.0, 4, dim, 0);
        let dst = cluster_at(0.05, opts().max_partition_size, dim, 100);
        let mut idx = tree_of_leaves(dim, opts(), &[src, dst]);
        let at = idx.find(leaf_key(0)).unwrap();
        let before = idx.row_ids().len();

        idx.merge(at, None, None).unwrap();

        assert_eq!(idx.merges(), 0, "a full sibling must not be filled further");
        assert_eq!(idx.row_ids().len(), before);
        assert!(idx.find(leaf_key(0)).is_some());
    }

    #[test]
    fn a_merge_is_refused_when_the_nearest_sibling_is_too_far() {
        let dim = 8;
        // Two tight balls 20 apart: `d + r_s > 2 * r_t`, so the rows would land
        // far outside the target's ball and their codes would degrade.
        let src = cluster_at(0.0, 4, dim, 0);
        let dst = cluster_at(20.0, 10, dim, 100);
        let mut idx = tree_of_leaves(dim, opts(), &[src, dst]);
        let at = idx.find(leaf_key(0)).unwrap();

        idx.merge(at, None, None).unwrap();

        assert_eq!(idx.merges(), 0, "the I9c radius gate has to refuse this");
        assert!(idx.find(leaf_key(0)).is_some());
    }

    #[test]
    fn a_merge_splits_the_rows_between_the_siblings_that_are_near_them() {
        let dim = 8;
        // One left heap and two right heaps: the left rows belong to the left
        // sibling, the rest to whichever right sibling is closer. A wholesale
        // merge would put all four into one.
        let src: Vec<(u64, Vec<f32>)> = cluster_at(0.0, 2, dim, 0)
            .into_iter()
            .chain(cluster_at(10.0, 2, dim, 2))
            .collect();
        let left = cluster_at(0.05, 10, dim, 100);
        let right = cluster_at(10.05, 10, dim, 200);
        let mut idx = tree_of_leaves(dim, opts(), &[src, left, right]);
        let at = idx.find(leaf_key(0)).unwrap();

        idx.merge(at, None, None).unwrap();

        assert_eq!(idx.merges(), 1);
        let left_at = idx.find(leaf_key(1)).unwrap();
        let right_at = idx.find(leaf_key(2)).unwrap();
        assert_eq!(
            (idx.parts[left_at].len(), idx.parts[right_at].len()),
            (12, 12),
            "each heap's rows follow their own centroid"
        );
    }
}
