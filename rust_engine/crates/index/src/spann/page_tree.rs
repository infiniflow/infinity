// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//! A **page-backed** view of a published tree: the write path of a delta
//! checkpoint (`SPANN_DESIGN.md` §5.4).
//!
//! A delta checkpoint routes new rows into a tree that already exists on disk
//! and rewrites only the pages that changed. Materialising the whole directory
//! to do that costs one centroid per partition - 380 MB at 62k partitions of
//! 1536d - on every checkpoint, for a batch of rows that touches a few leaves.
//!
//! This module is the reference implementations' shape instead: Cockroach's
//! `vecstore` reads one partition at a time and SPFresh's `ExtraDynamicSearcher`
//! reads one posting at a time. A descent reads the pages it visits, an insert
//! touches one leaf, and a split touches the leaf plus the children it creates.
//!
//! ## Memory, and why it is bounded
//!
//! Two byte budgets, both enforced on the write path:
//!
//! * `budget` bounds the cached pages. Eviction is least-recently-used and never
//!   drops a dirty page, so re-visiting a hot internal node does not cost a page
//!   parse per row.
//! * `max_dirty` bounds the pages that are about to be written. A dirty page
//!   cannot be evicted - its bytes *are* the delta - so [`PageTree::insert`]
//!   returns [`PageTreeError::Full`] once it is spent and the caller keeps the
//!   remaining rows for the next checkpoint. That is Cockroach's per-partition
//!   transaction reduced to bytes: the work is bounded by what is written, not
//!   by how many rows arrived.
//!
//! ## What is not here
//!
//! Search. A query walks the mapped chunk (`ChunkChain::search`); this tree
//! exists between "rows were appended" and "the delta chunk is written".

use std::collections::{BTreeSet, HashMap, HashSet};

use crate::vector::l2_sq;

use super::incr::{kmeans_k, FANOUT, MAX_DEPTH};
use super::kmeans::balanced_kmeans_2;
use super::partition::Partition;
use super::quantize::{self, Rotation};
use super::{Member, Options, PartitionKey, SpannError, SpannIndex, ROOT_KEY};

/// The directory half of one page: what routing needs and nothing else.
///
/// Deliberately without the slot column - loading 128 codes per visited page
/// would defeat the module. Slots are read on demand, by [`PageSource::slots`],
/// when a leaf is split or exported.
#[derive(Clone, Debug)]
pub struct PageMeta {
    pub key: u64,
    /// `1` for a leaf, `1 + max(child level)` above it - the chunk's numbering.
    pub level: u32,
    pub centroid: Vec<f32>,
    pub radius: f32,
    pub children: Vec<u64>,
    /// Rows the page already holds on disk (its published slot column).
    pub count: usize,
}

/// Where a [`PageTree`] reads from: one immutable chunk chain.
///
/// The implementation lives in `storage` (it wraps `ChunkChain`); `index` has no
/// dependency on the file format, hence the trait.
pub trait PageSource {
    fn page(&self, key: u64) -> Option<PageMeta>;
    /// The full slot column of `key`: the rows a split has to get back.
    fn slots(&self, key: u64) -> Option<Vec<Member>>;
}

/// Why an insert could not be taken.
#[derive(Debug)]
pub enum PageTreeError {
    /// No root page: the chain is empty or unreadable.
    MissingRoot,
    /// The dirty budget is spent; the row was not inserted.
    Full,
    /// A page the tree has to move is unreadable.
    Corrupt(String),
    Spann(SpannError),
}

impl From<SpannError> for PageTreeError {
    fn from(error: SpannError) -> Self {
        Self::Spann(error)
    }
}

#[derive(Debug)]
struct Page {
    level: u32,
    centroid: Vec<f32>,
    /// `rot.apply(centroid)`, computed once per load and dropped with the page.
    rot: std::sync::OnceLock<Vec<f32>>,
    radius: f32,
    children: Vec<u64>,
    /// The slot column, once loaded: published rows first, then new ones.
    members: Vec<Member>,
    /// Published rows not loaded into `members` yet.
    published: usize,
    bytes: usize,
}

impl Page {
    fn from_meta(meta: PageMeta, per_member: usize) -> Self {
        let published = if meta.children.is_empty() {
            meta.count
        } else {
            0
        };
        let mut page = Self {
            level: meta.level,
            centroid: meta.centroid,
            rot: std::sync::OnceLock::new(),
            radius: meta.radius,
            children: meta.children,
            members: Vec::new(),
            published,
            bytes: 0,
        };
        page.recount(per_member);
        page
    }

    fn new_leaf(centroid: Vec<f32>) -> Self {
        Self {
            level: 1,
            centroid,
            rot: std::sync::OnceLock::new(),
            radius: 0.0,
            children: Vec::new(),
            members: Vec::new(),
            published: 0,
            bytes: 0,
        }
    }

    fn len(&self) -> usize {
        self.published + self.members.len()
    }

    fn is_leaf(&self) -> bool {
        self.children.is_empty()
    }

    fn rot_centroid(&self, rot: &Rotation) -> &[f32] {
        self.rot.get_or_init(|| rot.apply(&self.centroid))
    }

    fn recount(&mut self, per_member: usize) {
        self.bytes = std::mem::size_of::<Self>()
            + self.centroid.len() * 4
            + self.rot.get().map(|rot| rot.len() * 4).unwrap_or(0)
            + self.children.len() * 8
            + self.members.len() * per_member;
    }
}

/// A tree whose pages live in a chunk and are loaded on demand.
pub struct PageTree<'a> {
    dim: usize,
    opts: Options,
    /// `encode.record_bytes(dim)`: the per-member slot cost, and the unit the
    /// page and dirty budgets are measured in.
    per_member: usize,
    rot: Rotation,
    src: &'a dyn PageSource,
    root: u64,
    next_key: u64,
    pages: HashMap<u64, Page>,
    /// LRU stamps; kept beside the pages so `to_spann_delta` can move the slot
    /// columns out without dropping the ordering.
    stamps: HashMap<u64, u64>,
    /// Pages whose content differs from the chunk: the delta's key set.
    dirty: BTreeSet<u64>,
    /// `key -> bytes of its dirty page`, so the dirty budget can be adjusted
    /// when a page grows instead of counted twice.
    dirty_page_bytes: HashMap<u64, usize>,
    /// Pages a merge retired: absent from the tree, so not written.
    removed: HashSet<u64>,
    /// `child -> parent`, filled by every descent and split.
    parents: HashMap<u64, u64>,
    /// Bytes of everything cached, and the budget for it.
    resident: usize,
    budget: usize,
    dirty_bytes: usize,
    max_dirty: usize,
    clock: u64,
    fixups: Vec<u64>,
    queued: HashSet<u64>,
    evictions: u64,
    splits: u64,
    merges: u64,
}

impl<'a> PageTree<'a> {
    /// Opens the tree `chain` describes for writing.
    ///
    /// `next_key` must be above every published key (the caller takes it from
    /// the chain) so a new node never collides with an existing page.
    #[allow(clippy::too_many_arguments)]
    pub fn new(
        dim: usize,
        opts: Options,
        rot: Rotation,
        src: &'a dyn PageSource,
        root: u64,
        next_key: u64,
        budget: usize,
        max_dirty: usize,
    ) -> Result<Self, PageTreeError> {
        let per_member = opts.encode.record_bytes(dim);
        let mut tree = Self {
            dim,
            opts,
            per_member,
            rot,
            src,
            root,
            next_key,
            pages: HashMap::new(),
            stamps: HashMap::new(),
            dirty: BTreeSet::new(),
            dirty_page_bytes: HashMap::new(),
            removed: HashSet::new(),
            parents: HashMap::new(),
            resident: 0,
            budget,
            dirty_bytes: 0,
            max_dirty,
            clock: 0,
            fixups: Vec::new(),
            queued: HashSet::new(),
            evictions: 0,
            splits: 0,
            merges: 0,
        };
        if tree.load(root).is_none() {
            return Err(PageTreeError::MissingRoot);
        }
        Ok(tree)
    }

    pub fn dirty_keys(&self) -> Vec<u64> {
        self.dirty.iter().copied().collect()
    }

    /// Levels in the tree: the root's level ( when the root is unreadable).
    pub fn height(&self) -> u32 {
        self.pages
            .get(&self.root)
            .map(|page| page.level)
            .unwrap_or(0)
    }

    pub fn is_dirty(&self) -> bool {
        !self.dirty.is_empty()
    }

    pub fn splits(&self) -> u64 {
        self.splits
    }

    pub fn merges(&self) -> u64 {
        self.merges
    }

    /// Cached bytes (clean and dirty) and how many pages the budget evicted.
    pub fn resident_bytes(&self) -> usize {
        self.resident
    }

    pub fn evictions(&self) -> u64 {
        self.evictions
    }

    /// Inserts one row: one descent, one leaf page, one queued split.
    ///
    /// [`PageTreeError::Full`] means the dirty budget is spent and the row was
    /// **not** inserted; the caller keeps it for the next checkpoint.
    pub fn insert(&mut self, row_id: u64, vector: &[f32]) -> Result<(), PageTreeError> {
        if vector.len() != self.dim {
            return Err(PageTreeError::Spann(SpannError::DimensionMismatch {
                expected: self.dim,
                actual: vector.len(),
            }));
        }
        // Checked **before** the page is touched, and with one member's worth of
        // slack: the caller puts a refused row back in the tail, so a row this
        // method accepted must not also be refused later in the same call.
        if self.dirty_bytes + self.per_member + 128 > self.max_dirty {
            return Err(PageTreeError::Full);
        }
        let leaf = self.leaf_for(vector)?;
        let (rot_centroid, centroid) = {
            let page = self
                .pages
                .get(&leaf)
                .ok_or_else(|| PageTreeError::Corrupt(format!("leaf {leaf} vanished")))?;
            (page.rot_centroid(&self.rot).to_vec(), page.centroid.clone())
        };
        let code = quantize::encode(&self.rot, &rot_centroid, vector);
        let radius = l2_sq(vector, &centroid).sqrt();
        let page = self
            .pages
            .get_mut(&leaf)
            .ok_or_else(|| PageTreeError::Corrupt(format!("leaf {leaf} vanished")))?;
        if radius > page.radius {
            page.radius = radius;
        }
        page.members.push(Member { id: row_id, code });
        page.recount(self.per_member);
        self.touch(leaf);
        if let Err(error) = self.mark_dirty(leaf) {
            // The row was appended before the accounting; take it back, or the
            // caller would re-insert a row that is already in the tree.
            if let Some(page) = self.pages.get_mut(&leaf) {
                page.members.pop();
                page.recount(self.per_member);
            }
            return Err(error);
        }
        if self.page_len(leaf) > self.opts.max_partition_size {
            self.queue_fixup(leaf);
        }
        Ok(())
    }

    /// Drains up to `budget` queued splits/merges. Called at a batch boundary,
    /// never per row.
    pub fn run_fixups(
        &mut self,
        source: Option<&dyn Fn(u64) -> Option<Vec<f32>>>,
        budget: usize,
    ) -> usize {
        let cap = budget.max(1);
        let mut steps = 0usize;
        while let Some(key) = self.fixups.pop() {
            if steps >= cap {
                self.fixups.push(key);
                break;
            }
            self.queued.remove(&key);
            steps += 1;
            let Some(len) = self.load(key).map(|page| page.len()) else {
                continue;
            };
            if len > self.opts.max_partition_size {
                let _ = self.split_or_sideways(key, source);
            } else if len < self.opts.min_partition_size {
                let _ = self.merge(key, source);
            }
        }
        if steps < cap {
            self.merge_underfull(source);
        }
        steps
    }

    /// The delta chunk: one partition per dirty page.
    ///
    /// The slot columns are **moved** into the chunk, so the peak is one delta
    /// and not two.
    pub fn to_spann_delta(&mut self) -> Option<SpannIndex> {
        if self.dirty.is_empty() {
            return None;
        }
        let height = self
            .pages
            .get(&self.root)
            .map(|page| page.level)
            .unwrap_or(1);
        let keys: Vec<u64> = self.dirty.iter().copied().collect();
        let mut partitions = Vec::with_capacity(keys.len());
        for key in keys {
            if self.removed.contains(&key) {
                continue;
            }
            // A delta page **shadows** the published one (the chain resolves a
            // key to its newest layer), so it has to carry the whole leaf: the
            // published rows plus the ones this checkpoint added. A page with
            // only the new rows would hide the rest.
            if self.ensure_slots(key).is_err() {
                return None;
            }
            let Some(page) = self.pages.get_mut(&key) else {
                continue;
            };
            let members = std::mem::take(&mut page.members);
            page.published = 0;
            page.recount(self.per_member);
            partitions.push(Partition::from_decoded(
                key as PartitionKey,
                page.level,
                self.dim,
                page.centroid.clone(),
                page.radius,
                page.rot_centroid(&self.rot).to_vec(),
                members,
                page.children
                    .iter()
                    .map(|child| *child as PartitionKey)
                    .collect(),
            ));
        }
        if partitions.is_empty() {
            return None;
        }
        Some(SpannIndex::from_parts(
            self.opts.clone(),
            self.dim,
            self.rot.clone(),
            partitions,
            height,
            ROOT_KEY as PartitionKey,
        ))
    }

    // ---------------------------------------------------------------- pages

    fn page_len(&self, key: u64) -> usize {
        self.pages.get(&key).map(|page| page.len()).unwrap_or(0)
    }

    fn load(&mut self, key: u64) -> Option<&mut Page> {
        if !self.pages.contains_key(&key) {
            let meta = self.src.page(key)?;
            let page = Page::from_meta(meta, self.per_member);
            self.resident += page.bytes;
            self.pages.insert(key, page);
            self.evict_to_budget();
        }
        self.touch(key);
        self.pages.get_mut(&key)
    }

    fn touch(&mut self, key: u64) {
        self.clock += 1;
        self.stamps.insert(key, self.clock);
    }

    /// Drops clean pages, least recently used first, until the budget holds.
    ///
    /// Dirty pages are never candidates - their bytes are the output - so an
    /// all-dirty cache can exceed `budget`; `max_dirty` is what bounds that side.
    fn evict_to_budget(&mut self) {
        while self.resident > self.budget {
            let victim = self
                .stamps
                .iter()
                .filter(|(key, _)| !self.dirty.contains(key) && !self.queued.contains(key))
                .min_by_key(|(_, clock)| **clock)
                .map(|(key, _)| *key);
            let Some(victim) = victim else {
                break;
            };
            self.stamps.remove(&victim);
            if let Some(page) = self.pages.remove(&victim) {
                self.resident = self.resident.saturating_sub(page.bytes);
                self.evictions += 1;
            }
        }
    }

    fn mark_dirty(&mut self, key: u64) -> Result<(), PageTreeError> {
        let bytes = self.pages.get(&key).map(|page| page.bytes).unwrap_or(0);
        let previous = self.dirty_page_bytes.insert(key, bytes).unwrap_or(0);
        self.dirty_bytes = self.dirty_bytes + bytes - previous.min(bytes);
        self.dirty.insert(key);
        if self.dirty_bytes > self.max_dirty {
            return Err(PageTreeError::Full);
        }
        Ok(())
    }

    fn mark_removed(&mut self, key: u64) {
        self.dirty.remove(&key);
        if let Some(bytes) = self.dirty_page_bytes.remove(&key) {
            self.dirty_bytes = self.dirty_bytes.saturating_sub(bytes);
        }
        self.removed.insert(key);
        if let Some(page) = self.pages.remove(&key) {
            self.resident = self.resident.saturating_sub(page.bytes);
        }
        self.stamps.remove(&key);
    }

    // ------------------------------------------------------------- routing

    fn leaf_for(&mut self, vector: &[f32]) -> Result<u64, PageTreeError> {
        let mut key = self.root;
        loop {
            let children = {
                let page = self
                    .load(key)
                    .ok_or_else(|| PageTreeError::Corrupt(format!("page {key} vanished")))?;
                page.children.clone()
            };
            if children.is_empty() {
                return Ok(key);
            }
            let mut best = None::<(u64, f32)>;
            for child in children {
                let d = match self.load(child) {
                    Some(page) => l2_sq(vector, &page.centroid),
                    None => continue,
                };
                self.parents.insert(child, key);
                if best.is_none_or(|(_, bd)| d < bd) {
                    best = Some((child, d));
                }
            }
            let Some((child, _)) = best else {
                return Err(PageTreeError::Corrupt(format!(
                    "branch {key} has no readable child"
                )));
            };
            key = child;
        }
    }

    fn queue_fixup(&mut self, key: u64) {
        if self.queued.insert(key) {
            self.fixups.push(key);
        }
    }

    // --------------------------------------------------------------- fixups

    fn split_or_sideways(
        &mut self,
        key: u64,
        source: Option<&dyn Fn(u64) -> Option<Vec<f32>>>,
    ) -> Result<(), PageTreeError> {
        let (len, is_leaf) = {
            let page = self
                .load(key)
                .ok_or_else(|| PageTreeError::Corrupt(format!("page {key} vanished")))?;
            (page.len(), page.is_leaf())
        };
        if !is_leaf || len < 2 {
            return Ok(());
        }
        let abandon = (self.opts.max_partition_size + self.opts.min_partition_size) / 2;
        if len <= abandon {
            return Ok(());
        }
        // The cap is the tree's **height** (the root's level, counted from the
        // leaf), not the leaf's own level - a leaf is always 1, so testing that
        // would let the tree grow without bound.
        let height = self.height();
        if height < MAX_DEPTH {
            self.split(key, source)
        } else {
            self.split_sideways(key, source)
        }
    }

    /// The rows of a leaf: the chunk's published column plus the local inserts,
    /// each as a raw vector (`source`) or its dequantized code (the lossy
    /// fallback - see `IncIndex::split`).
    fn gather_rows(
        &mut self,
        key: u64,
        source: Option<&dyn Fn(u64) -> Option<Vec<f32>>>,
    ) -> Result<(Vec<Vec<f32>>, Vec<u64>), PageTreeError> {
        self.ensure_slots(key)?;
        let page = self
            .pages
            .get(&key)
            .ok_or_else(|| PageTreeError::Corrupt(format!("page {key} vanished")))?;
        let rot_centroid = page.rot_centroid(&self.rot).to_vec();
        let mut vectors = Vec::with_capacity(page.members.len());
        let mut ids = Vec::with_capacity(page.members.len());
        for member in &page.members {
            let vector = match source.and_then(|fetch| fetch(member.id)) {
                Some(vector) if vector.len() == self.dim => vector,
                _ => quantize::dequantize(&self.rot, &rot_centroid, &member.code),
            };
            vectors.push(vector);
            ids.push(member.id);
        }
        Ok((vectors, ids))
    }

    fn ensure_slots(&mut self, key: u64) -> Result<(), PageTreeError> {
        let published = self.pages.get(&key).map(|page| page.published).unwrap_or(0);
        if published == 0 {
            return Ok(());
        }
        let slots = self
            .src
            .slots(key)
            .ok_or_else(|| PageTreeError::Corrupt(format!("slot page {key} is unreadable")))?;
        if slots.len() != published {
            return Err(PageTreeError::Corrupt(format!(
                "slot page {key} holds {} rows, the directory says {published}",
                slots.len()
            )));
        }
        let page = self
            .pages
            .get_mut(&key)
            .ok_or_else(|| PageTreeError::Corrupt(format!("page {key} vanished")))?;
        let mut members = slots;
        members.append(&mut page.members);
        page.members = members;
        page.published = 0;
        page.recount(self.per_member);
        Ok(())
    }

    fn split(
        &mut self,
        key: u64,
        source: Option<&dyn Fn(u64) -> Option<Vec<f32>>>,
    ) -> Result<(), PageTreeError> {
        let (vectors, ids) = self.gather_rows(key, source)?;
        if vectors.len() < 2 {
            return Ok(());
        }
        let refs: Vec<&[f32]> = vectors.iter().map(|v| v.as_slice()).collect();
        let target = self.opts.max_partition_size;
        let groups_wanted = if vectors.len() <= 2 * target {
            2
        } else {
            vectors.len().div_ceil((target / 2).max(1)).clamp(2, FANOUT)
        };
        let seed = self.opts.seed ^ key.wrapping_mul(0x9E37_79B9_7F4A_7C15);
        let groups: Vec<Vec<(u64, &[f32])>> = if groups_wanted == 2 {
            // A two-way cut has to be balanced, or one child lands under
            // `min_partition_size` and the merge pass folds it straight back.
            let split = balanced_kmeans_2(&refs, self.dim, self.opts.kmeans_iterations, seed);
            let mut groups: Vec<Vec<(u64, &[f32])>> = vec![Vec::new(), Vec::new()];
            for (i, second) in split.second.iter().enumerate() {
                groups[usize::from(*second)].push((ids[i], refs[i]));
            }
            groups
        } else {
            let (_, assign) = kmeans_k(
                &refs,
                self.dim,
                groups_wanted,
                self.opts.kmeans_iterations,
                seed,
            );
            let mut groups: Vec<Vec<(u64, &[f32])>> = vec![Vec::new(); groups_wanted];
            for (i, slot) in assign.iter().enumerate() {
                groups[*slot as usize].push((ids[i], refs[i]));
            }
            groups
        };
        if groups.iter().filter(|group| !group.is_empty()).count() < 2 {
            return Ok(());
        }

        // The page becomes an interior node. Its centroid stays (invariant I1:
        // moving it would invalidate every ancestor's routing).
        if let Some(page) = self.pages.get_mut(&key) {
            page.members.clear();
            page.published = 0;
            page.children.clear();
            page.level = 2;
        }
        let mut created = Vec::new();
        for group in groups.iter().filter(|group| !group.is_empty()) {
            let child = self.alloc_key();
            let page = self.make_leaf(group);
            self.resident += page.bytes;
            self.pages.insert(child, page);
            self.parents.insert(child, key);
            created.push(child);
        }
        if let Some(page) = self.pages.get_mut(&key) {
            page.children = created.clone();
            page.recount(self.per_member);
        }
        self.mark_dirty(key)?;
        for child in created {
            self.mark_dirty(child)?;
        }
        self.splits += 1;
        self.evict_to_budget();
        Ok(())
    }

    /// Splits at the depth cap: the second half becomes a sibling under the
    /// parent, so no level gets deeper and the size limit keeps applying.
    fn split_sideways(
        &mut self,
        key: u64,
        source: Option<&dyn Fn(u64) -> Option<Vec<f32>>>,
    ) -> Result<(), PageTreeError> {
        let Some(parent) = self.parents.get(&key).copied() else {
            return Ok(());
        };
        let parent_children = match self.load(parent) {
            Some(page) => page.children.clone(),
            None => return Ok(()),
        };
        if parent_children.len() >= FANOUT {
            return Ok(());
        }
        let (vectors, ids) = self.gather_rows(key, source)?;
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
            halves[*slot as usize].push((ids[i], refs[i]));
        }
        if halves[0].is_empty() || halves[1].is_empty() {
            return Ok(());
        }
        let sibling_key = self.alloc_key();
        let kept = self.make_leaf(&halves[0]);
        let sibling = self.make_leaf(&halves[1]);
        self.pages.insert(key, kept);
        self.pages.insert(sibling_key, sibling);
        self.parents.insert(sibling_key, parent);
        if let Some(page) = self.pages.get_mut(&parent) {
            page.children.push(sibling_key);
            page.recount(self.per_member);
        }
        for changed in [key, sibling_key, parent] {
            self.mark_dirty(changed)?;
        }
        self.splits += 1;
        self.evict_to_budget();
        Ok(())
    }

    /// Folds an underfull leaf into its nearest siblings that still have room.
    fn merge(
        &mut self,
        key: u64,
        source: Option<&dyn Fn(u64) -> Option<Vec<f32>>>,
    ) -> Result<(), PageTreeError> {
        let (len, src_centroid, src_radius) = {
            let Some(page) = self.load(key) else {
                return Ok(());
            };
            (page.len(), page.centroid.clone(), page.radius)
        };
        if len == 0 || len >= self.opts.min_partition_size {
            return Ok(());
        }
        let Some(parent) = self.parents.get(&key).copied() else {
            return Ok(());
        };
        let siblings = match self.load(parent) {
            Some(page) => page.children.clone(),
            None => return Ok(()),
        };
        let mut candidates: Vec<(u64, f32, usize)> = Vec::new();
        for child in siblings {
            if child == key {
                continue;
            }
            let (is_leaf, child_len, d) = match self.load(child) {
                Some(page) => (
                    page.is_leaf(),
                    page.len(),
                    l2_sq(&src_centroid, &page.centroid).sqrt(),
                ),
                None => continue,
            };
            if !is_leaf {
                continue;
            }
            let room = self.opts.max_partition_size.saturating_sub(child_len);
            if room == 0 {
                continue;
            }
            candidates.push((child, d, room));
        }
        if candidates.is_empty() || candidates.iter().map(|c| c.2).sum::<usize>() < len {
            return Ok(());
        }
        candidates.sort_by(|a, b| a.1.total_cmp(&b.1));
        let nearest_radius = self
            .pages
            .get(&candidates[0].0)
            .map(|page| page.radius)
            .unwrap_or(0.0);
        if candidates[0].1 > src_radius + nearest_radius {
            return Ok(());
        }

        let (vectors, ids) = self.gather_rows(key, source)?;
        if vectors.len() != len {
            return Err(PageTreeError::Corrupt(format!(
                "merge of {key} gathered {} of {len} rows",
                vectors.len()
            )));
        }
        let mut room: Vec<usize> = candidates.iter().map(|c| c.2).collect();
        let mut plan: Vec<usize> = Vec::with_capacity(vectors.len());
        for vector in &vectors {
            let mut choice = usize::MAX;
            let mut best = f32::INFINITY;
            for (slot, (at, _, _)) in candidates.iter().enumerate() {
                if room[slot] == 0 {
                    continue;
                }
                let d = match self.pages.get(at) {
                    Some(page) => l2_sq(vector, &page.centroid),
                    None => continue,
                };
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

        for (i, slot) in plan.iter().enumerate() {
            let at = candidates[*slot].0;
            let (rot_centroid, centroid) = match self.pages.get(&at) {
                Some(page) => (page.rot_centroid(&self.rot).to_vec(), page.centroid.clone()),
                None => continue,
            };
            let code = quantize::encode(&self.rot, &rot_centroid, &vectors[i]);
            let radius = l2_sq(&vectors[i], &centroid).sqrt();
            if let Some(page) = self.pages.get_mut(&at) {
                page.members.push(Member { id: ids[i], code });
                if radius > page.radius {
                    page.radius = radius;
                }
                page.recount(self.per_member);
            }
            self.mark_dirty(at)?;
        }
        // The emptied leaf leaves the parent's child list. Its own page is not
        // written (a chunk cannot delete one); the reader resolves the new set.
        if let Some(page) = self.pages.get_mut(&parent) {
            page.children.retain(|child| *child != key);
            page.recount(self.per_member);
        }
        self.mark_dirty(parent)?;
        self.mark_removed(key);
        self.queued.remove(&key);
        self.parents.remove(&key);
        self.merges += 1;
        Ok(())
    }

    fn merge_underfull(&mut self, source: Option<&dyn Fn(u64) -> Option<Vec<f32>>>) {
        // Only the pages this checkpoint touched are candidates: an untouched
        // published leaf belongs to an older chunk, and walking the directory
        // here is exactly the cost this module exists to avoid.
        let thin: Vec<u64> = self
            .dirty
            .iter()
            .copied()
            .filter(|key| {
                self.pages.get(key).is_some_and(|page| {
                    page.is_leaf() && page.len() > 0 && page.len() < self.opts.min_partition_size
                })
            })
            .collect();
        for key in thin {
            if self.pages.contains_key(&key) {
                let _ = self.merge(key, source);
            }
        }
    }

    fn make_leaf(&self, items: &[(u64, &[f32])]) -> Page {
        let mut centroid = vec![0.0f32; self.dim];
        for (_, vector) in items {
            for (out, value) in centroid.iter_mut().zip(*vector) {
                *out += *value;
            }
        }
        let inv = 1.0 / items.len() as f32;
        for out in &mut centroid {
            *out *= inv;
        }
        let mut page = Page::new_leaf(centroid);
        let mut radius = 0.0f32;
        let rot_centroid = page.rot_centroid(&self.rot).to_vec();
        for (id, vector) in items {
            radius = radius.max(l2_sq(vector, &page.centroid).sqrt());
            let code = quantize::encode(&self.rot, &rot_centroid, vector);
            page.members.push(Member { id: *id, code });
        }
        page.radius = radius;
        page.recount(self.per_member);
        page
    }

    fn alloc_key(&mut self) -> u64 {
        let key = self.next_key;
        self.next_key += 1;
        debug_assert!(key <= u64::from(u32::MAX), "a chunk's key column is u32");
        key
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    struct MemPages {
        pages: HashMap<u64, PageMeta>,
        slots: HashMap<u64, Vec<Member>>,
    }

    impl PageSource for MemPages {
        fn page(&self, key: u64) -> Option<PageMeta> {
            self.pages.get(&key).cloned()
        }

        fn slots(&self, key: u64) -> Option<Vec<Member>> {
            self.slots.get(&key).cloned()
        }
    }

    const DIM: usize = 8;

    /// A root over `leaves` published leaves of `per_leaf` rows each, every
    /// leaf's centroid on its own axis so a vector routes to one of them.
    fn published_tree(leaves: u64, per_leaf: usize) -> MemPages {
        capped_tree(leaves, per_leaf, 2)
    }

    /// [`published_tree`] with the root at `root_level` - the cap a sideways
    /// split needs (`MAX_DEPTH`).
    ///
    /// The centroids are far apart on purpose (`100` on its own axis): the rows
    /// a test appends can then cluster **around** one leaf without a rival
    /// centroid becoming the nearer one.
    fn capped_tree(leaves: u64, per_leaf: usize, root_level: u32) -> MemPages {
        let opts = Options::default();
        let rot = Rotation::new(DIM, opts.seed);
        let mut pages = HashMap::new();
        let mut slots = HashMap::new();
        let mut children = Vec::new();
        for leaf in 0..leaves {
            let key = leaf + 1;
            children.push(key);
            let centroid: Vec<f32> = (0..DIM)
                .map(|d| if d as u64 == leaf { 100.0 } else { 0.0 })
                .collect();
            let rot_centroid = rot.apply(&centroid);
            let members: Vec<Member> = (0..per_leaf)
                .map(|i| Member {
                    id: leaf * per_leaf as u64 + i as u64,
                    code: quantize::encode(&rot, &rot_centroid, &centroid),
                })
                .collect();
            pages.insert(
                key,
                PageMeta {
                    key,
                    level: 1,
                    centroid,
                    radius: 1.0,
                    children: Vec::new(),
                    count: per_leaf,
                },
            );
            slots.insert(key, members);
        }
        pages.insert(
            0,
            PageMeta {
                key: 0,
                level: root_level,
                centroid: vec![0.0; DIM],
                radius: 1.0,
                children,
                count: 0,
            },
        );
        MemPages { pages, slots }
    }

    /// The budgets are not advice: the clean cache evicts pages it cannot hold,
    /// and the dirty side refuses rows once its bytes are spent. This is what
    /// keeps a checkpoint's heap a function of the batch, not of the segment.
    #[test]
    fn the_budgets_bound_the_cache_and_the_delta() {
        let opts = Options {
            max_partition_size: 16,
            min_partition_size: 4,
            ..Options::default()
        };
        let src = published_tree(64, 8);
        let rot = Rotation::new(DIM, opts.seed);
        let (budget, max_dirty) = (512usize, 4 * 1024usize);
        let mut tree = PageTree::new(DIM, opts, rot, &src, 0, 1000, budget, max_dirty).unwrap();

        let mut full_at = None;
        for i in 0..5_000u64 {
            // One row into every leaf in turn: the descent has to visit (and
            // therefore cache) all 64 pages, which a 512 B budget cannot hold.
            let leaf = i % 64;
            let vector = src.pages.get(&(leaf + 1)).unwrap().centroid.clone();
            match tree.insert(1_000_000 + i, &vector) {
                Ok(()) => {}
                Err(PageTreeError::Full) => {
                    full_at = Some(i);
                    break;
                }
                Err(other) => panic!("unexpected {other:?}"),
            }
        }

        assert!(tree.evictions() > 0, "a 512 B clean budget has to evict");
        assert!(
            tree.resident_bytes() <= budget + max_dirty,
            "cache holds {} B, budget {} + dirty {}",
            tree.resident_bytes(),
            budget,
            max_dirty
        );
        let full_at = full_at.expect("a 4 KiB dirty budget has to refuse rows");
        assert!(full_at > 0, "the tree took some rows before it filled up");
        // What was taken is still publishable: the refusal is a boundary, not a
        // failure of the rows already inside.
        assert!(tree.is_dirty());
        assert!(
            tree.to_spann_delta().is_some(),
            "the accepted rows are a delta"
        );
    }

    /// At the height cap a split cannot add a level: the second half becomes a
    /// **sibling** under the same parent, so `MAX_DEPTH` bounds the descent and
    /// the size limit keeps applying. (`IncIndex`'s version of this is
    /// `a_leaf_at_the_depth_cap_splits_sideways`; the page-backed writer has its
    /// own parent map, so it is a separate path.)
    #[test]
    fn a_split_at_the_height_cap_moves_the_second_half_sideways() {
        let opts = Options {
            max_partition_size: 16,
            min_partition_size: 4,
            kmeans_iterations: 8,
            ..Options::default()
        };
        let src = capped_tree(4, 8, MAX_DEPTH);
        let rot = Rotation::new(DIM, opts.seed);
        let mut tree = PageTree::new(DIM, opts.clone(), rot, &src, 0, 1000, 1 << 20, 1 << 20)
            .expect("the fixture opens");
        assert_eq!(tree.height(), MAX_DEPTH, "the fixture sits at the cap");

        // Twelve rows in two tight groups around leaf 1's centroid: 8 + 12 is
        // past `max_partition_size`, and the two groups give the split a real
        // cut to make.
        let mut appended = Vec::new();
        for i in 0..12u64 {
            let offset = if i % 2 == 0 { -6.0 } else { 6.0 };
            let mut vector = vec![0.0f32; DIM];
            vector[0] = 100.0 + offset;
            vector[1] = (i as f32) * 0.5;
            let id = 1_000 + i;
            tree.insert(id, &vector).expect("the row is taken");
            appended.push(id);
        }
        tree.run_fixups(None, usize::MAX);

        assert_eq!(tree.height(), MAX_DEPTH, "a sideways split adds no level");
        let dirty = tree.dirty_keys();
        assert_eq!(
            dirty.len(),
            3,
            "the leaf, its sibling and the parent are the write: {dirty:?}"
        );
        // The parent gained exactly one child, and the two leaves it now holds
        // spread the 20 rows instead of one leaf keeping all of them.
        let root_children = src.pages.get(&0).unwrap().children.len();
        let delta = tree.to_spann_delta().expect("the moved pages are a delta");
        assert_eq!(
            delta.partitions().len(),
            3,
            "one partition per dirty page, the parent included"
        );
        let leaves: Vec<&Partition> = delta.partitions().iter().filter(|p| p.is_leaf()).collect();
        assert_eq!(leaves.len(), 2, "the split leaf and its sibling");
        assert!(
            leaves
                .iter()
                .all(|p| p.members.len() <= opts.max_partition_size),
            "both halves stay under the size limit: {:?}",
            leaves.iter().map(|p| p.members.len()).collect::<Vec<_>>()
        );
        let mut ids: Vec<u64> = leaves
            .iter()
            .flat_map(|p| p.members.iter().map(|m| m.id))
            .collect();
        ids.sort_unstable();
        let mut want: Vec<u64> = (0..8).chain(appended.iter().copied()).collect();
        want.sort_unstable();
        assert_eq!(ids, want, "every row of the leaf, once");
        assert_eq!(
            root_children + 1,
            delta
                .partitions()
                .iter()
                .find(|p| !p.is_leaf())
                .map(|p| p.children.len())
                .expect("the parent is in the delta"),
            "the parent lists the old children plus the sibling"
        );
    }

    /// A parent already at `FANOUT` has no room for a sibling, so the sideways
    /// split is refused and the leaf keeps its rows: the alternative - growing a
    /// level - is what `MAX_DEPTH` exists to prevent.
    #[test]
    fn a_full_parent_refuses_the_sideways_split_and_keeps_the_rows() {
        let opts = Options {
            max_partition_size: 16,
            min_partition_size: 4,
            ..Options::default()
        };
        let src = capped_tree(FANOUT as u64, 8, MAX_DEPTH);
        let rot = Rotation::new(DIM, opts.seed);
        let mut tree = PageTree::new(DIM, opts.clone(), rot, &src, 0, 1000, 1 << 20, 1 << 20)
            .expect("the fixture opens");

        let mut appended = Vec::new();
        for i in 0..12u64 {
            let mut vector = vec![0.0f32; DIM];
            vector[0] = 100.0 + if i % 2 == 0 { -6.0 } else { 6.0 };
            vector[1] = (i as f32) * 0.5;
            tree.insert(5_000 + i, &vector).expect("the row is taken");
            appended.push(5_000 + i);
        }
        tree.run_fixups(None, usize::MAX);

        assert_eq!(tree.height(), MAX_DEPTH, "no level was added");
        let delta = tree.to_spann_delta().expect("the leaf is still a delta");
        let mut ids: Vec<u64> = delta
            .partitions()
            .iter()
            .filter(|p| p.is_leaf())
            .flat_map(|p| p.members.iter().map(|m| m.id))
            .collect();
        ids.sort_unstable();
        let mut want: Vec<u64> = (0..8).chain(appended.iter().copied()).collect();
        want.sort_unstable();
        assert_eq!(ids, want, "every row stayed, none was dropped");
    }
}
