//! HNSW neighbour lists in the reference's compact layout.
//!
//! The reference keeps the graph in `GraphStoreInner` as one flat buffer:
//! layer 0 is a fixed-stride slab of `2 * Mmax` `i32` slots per vertex
//! (`VertexL0` = `layer_n_`, `layers_p_`, `neighbor_n_`, then the neighbour
//! array), and the upper layers are one contiguous block per vertex that only
//! exists when the vertex actually reaches that layer (`layers_p_` points at
//! it). `graph_store.cppm:31-46,172-219`.
//!
//! This module reproduces that shape in Rust: `u32` slot ids, `2M` slots for
//! layer 0, `M` slots per upper layer, allocated per vertex only for the layers
//! the vertex appears in. The straightforward `Vec<Vec<Vec<usize>>>` it
//! replaces paid a `Vec` header plus one heap allocation per vertex *per
//! layer*, used `usize` ids, and — worse — had to size every layer to `n`
//! during `add_batch`, which allocated empty upper-layer lists for vertices
//! that never reach those layers.
//!
//! Slots are `AtomicU32` because graph construction is multi-threaded: threads
//! link disjoint vertices concurrently (the reference takes a per-vertex
//! `shared_mutex`, `hnsw_alg.cppm` `WithLock`), so a read of a vertex another
//! thread is rewriting must not be a data race. The length is published after
//! the slots (Release/Acquire), so a reader that sees `len = k` also sees
//! `slots[0..k]`.

use std::sync::atomic::{AtomicU32, Ordering};

/// `layer_n_`, the layer pointer and `neighbor_n_` of a level-0 record, plus
/// the 4 trailing bytes of alignment padding (`VERTEX_L0_SIZE`).
const VERTEX_L0_SIZE: usize = 24;
/// `neighbor_n_` of an upper-layer record (`VERTEX_LX_SIZE`).
const VERTEX_LX_SIZE: usize = 4;

/// A vertex has no upper-layer block.
const NO_BLOCK: u32 = u32::MAX;

/// Neighbour list of one vertex on one layer.
///
/// Yields `u32` vertex ids; the underlying storage is shared with the builder,
/// so it is read on demand rather than copied.
#[derive(Clone, Copy)]
pub struct Neighbors<'a> {
    slots: Slots<'a>,
}

#[derive(Clone, Copy)]
enum Slots<'a> {
    /// The builder's own slabs, written concurrently during construction.
    Shared(&'a [AtomicU32]),
    /// A sealed chunk's region: ids read straight out of the file mapping.
    Mapped(&'a [u32]),
}

impl<'a> Neighbors<'a> {
    /// An empty list, for vertices the caller knows are absent.
    #[inline]
    pub fn empty() -> Self {
        Self {
            slots: Slots::Shared(&[]),
        }
    }

    #[inline]
    fn shared(slots: &'a [AtomicU32]) -> Self {
        Self {
            slots: Slots::Shared(slots),
        }
    }

    #[inline]
    fn mapped(slots: &'a [u32]) -> Self {
        Self {
            slots: Slots::Mapped(slots),
        }
    }

    #[inline]
    pub fn len(&self) -> usize {
        match self.slots {
            Slots::Shared(s) => s.len(),
            Slots::Mapped(s) => s.len(),
        }
    }

    #[inline]
    pub fn is_empty(&self) -> bool {
        self.len() == 0
    }

    #[inline]
    pub fn get(&self, index: usize) -> Option<u32> {
        match self.slots {
            Slots::Shared(s) => s.get(index).map(|s| s.load(Ordering::Relaxed)),
            Slots::Mapped(s) => s.get(index).copied(),
        }
    }

    #[inline]
    pub fn iter(&self) -> impl Iterator<Item = u32> + 'a {
        // Exactly one arm is non-empty; chaining them keeps the iterator a single
        // concrete type without an `Either` helper.
        let shared: &'a [AtomicU32] = match self.slots {
            Slots::Shared(s) => s,
            Slots::Mapped(_) => &[],
        };
        let mapped: &'a [u32] = match self.slots {
            Slots::Mapped(s) => s,
            Slots::Shared(_) => &[],
        };
        shared
            .iter()
            .map(|s| s.load(Ordering::Relaxed))
            .chain(mapped.iter().copied())
    }

    pub fn to_vec(&self) -> Vec<u32> {
        self.iter().collect()
    }
}

impl std::fmt::Debug for Neighbors<'_> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_list().entries(self.iter()).finish()
    }
}

/// Compact, append-only neighbour store for a whole HNSW graph.
pub(crate) struct GraphStore {
    /// `M`: neighbour limit for layers ≥ 1.
    m: usize,
    /// `Mmax0 = 2 * M`: neighbour limit for layer 0.
    m0: usize,
    /// `layer_n_` per vertex: how many layers above 0 the vertex appears in.
    levels: Vec<u32>,
    /// Layer 0: `m0` slots per vertex, `n * m0` entries.
    l0: Vec<AtomicU32>,
    /// Layer 0 neighbour counts.
    l0_len: Vec<AtomicU32>,
    /// Layers ≥ 1: `m` slots per (vertex, layer) block, blocks back to back.
    up: Vec<AtomicU32>,
    /// Neighbour count per upper-layer block.
    up_len: Vec<AtomicU32>,
    /// First block ordinal of each vertex (`NO_BLOCK` when level == 0).
    up_start: Vec<u32>,
    /// Number of upper-layer blocks handed out so far.
    up_blocks: usize,
    /// Set when this graph **is** a sealed chunk's `GraphStoreInner` region:
    /// then every list reads out of the file mapping and the slabs above stay
    /// empty. The reference's `GraphStoreInner` is a pointer into the buffer
    /// manager's chunk, so its graph costs no heap either; decoding the region
    /// into `l0`/`up` was the last full copy a loaded index made.
    mapped: Option<MappedGraph>,
}

/// A graph read out of a mapped chunk.
///
/// Everything is derived from the region on demand — the level of a vertex, the
/// offset of its upper-layer block, the count and the slots — so only the header
/// is materialised here.
#[derive(Clone)]
struct MappedGraph {
    src: crate::vector::ByteSource,
    /// Offset of the `layer_sum` header inside `src`.
    base: usize,
    n: usize,
    /// Level-0 record stride (`VERTEX_L0_SIZE + 4 * Mmax0`).
    l0: usize,
    /// Upper-layer record stride (`VERTEX_LX_SIZE + 4 * Mmax`).
    lx: usize,
    layer_sum: usize,
}

/// The 20-byte header of a level-0 record: level at +0, `layers_p_` at +8,
/// `neighbor_n_` at +16, the slots from +20 (the record's trailing 4 bytes are
/// alignment padding, which is why the slots do not start at `VERTEX_L0_SIZE`).
const L0_SLOTS: usize = 20;
/// The 4-byte `neighbor_n_` that opens an upper-layer record.
const LX_SLOTS: usize = 4;

/// Reinterprets mapped bytes as `u32` ids.
#[allow(unsafe_code)]
fn u32s_at(bytes: &[u8]) -> Option<&[u32]> {
    if !bytes.len().is_multiple_of(4) {
        return None;
    }
    // SAFETY: `u32` has four-byte alignment and accepts every bit pattern; the
    // region's start and every slot offset are validated to be 4-byte aligned
    // before a mapping is accepted, and a published chunk is never written, so
    // no `&u32` here can observe a torn value.
    let (head, mid, tail) = unsafe { bytes.align_to::<u32>() };
    (head.is_empty() && tail.is_empty()).then_some(mid)
}

impl MappedGraph {
    fn bytes(&self) -> &[u8] {
        self.src.as_ref().as_ref()
    }

    /// Offset of `node`'s level-0 record.
    fn l0_record(&self, node: usize) -> usize {
        self.base + 8 + node * self.l0
    }

    /// Offset of the first upper-layer record.
    fn upper_base(&self) -> usize {
        self.base + 8 + self.n * self.l0
    }

    fn i32_at(&self, at: usize) -> Option<i32> {
        self.bytes()
            .get(at..at + 4)
            .map(|b| i32::from_le_bytes(b.try_into().unwrap()))
    }

    fn u64_at(&self, at: usize) -> Option<u64> {
        self.bytes()
            .get(at..at + 8)
            .map(|b| u64::from_le_bytes(b.try_into().unwrap()))
    }

    /// `mmax0`, the slot count a level-0 record was written with.
    fn slots_l0(&self) -> usize {
        (self.l0 - VERTEX_L0_SIZE) / 4
    }

    /// `mmax`, the slot count an upper-layer record was written with.
    fn slots_up(&self) -> usize {
        (self.lx - VERTEX_LX_SIZE) / 4
    }

    /// `count` slots at `at`, read out of the mapping.
    ///
    /// `from_cpp_region_mapped` only publishes a mapping whose base, strides and
    /// record degrees all check out, so the failure an unaligned read would cause
    /// - `u32s_at` refusing the slice and the list silently coming back empty -
    ///   cannot happen here. What is left is a count that runs past the region,
    ///   which the same validation rejects.
    fn slots_at(&self, at: usize, count: usize) -> Neighbors<'_> {
        debug_assert!(
            at.is_multiple_of(4),
            "a published mapping has word-aligned slots"
        );
        match at
            .checked_add(count.saturating_mul(4))
            .and_then(|end| self.bytes().get(at..end))
            .and_then(u32s_at)
        {
            Some(slots) => Neighbors::mapped(slots),
            None => Neighbors::empty(),
        }
    }

    fn neighbors(&self, layer: usize, node: usize) -> Neighbors<'_> {
        if node >= self.n {
            return Neighbors::empty();
        }
        let record = self.l0_record(node);
        let level = self.i32_at(record).filter(|l| *l >= 0).unwrap_or(0) as usize;
        if layer == 0 {
            // `neighbor_n_` sits at +16; the slots start at +20.
            let count = self.i32_at(record + 16).unwrap_or(0);
            let count = if count <= 0 { 0 } else { count as usize };
            return self.slots_at(record + L0_SLOTS, count.min(self.slots_l0()));
        }
        if layer > level {
            return Neighbors::empty();
        }
        // `layers_p_` is a byte offset into the upper region, block per layer.
        let block = self
            .u64_at(record + 8)
            .unwrap_or(0)
            .try_into()
            .ok()
            .and_then(|at: usize| at.checked_add((layer - 1) * self.lx))
            .and_then(|at| at.checked_add(self.upper_base()));
        let Some(block) = block else {
            return Neighbors::empty();
        };
        let count = self.i32_at(block).unwrap_or(0);
        let count = if count <= 0 { 0 } else { count as usize };
        self.slots_at(block + LX_SLOTS, count.min(self.slots_up()))
    }

    /// The region as it sits in the file, for a rewrite that re-encodes it.
    fn region(&self) -> &[u8] {
        let len = 8 + self.n * self.l0 + self.layer_sum * self.lx;
        self.bytes()
            .get(self.base..self.base + len)
            .unwrap_or_default()
    }
}

impl GraphStore {
    pub(crate) fn new(m: usize) -> Self {
        Self {
            m,
            m0: m * 2,
            levels: Vec::new(),
            l0: Vec::new(),
            l0_len: Vec::new(),
            up: Vec::new(),
            up_len: Vec::new(),
            up_start: Vec::new(),
            up_blocks: 0,
            mapped: None,
        }
    }

    pub fn len(&self) -> usize {
        match &self.mapped {
            Some(mapped) => mapped.n,
            None => self.levels.len(),
        }
    }

    pub(crate) fn max_degree(&self, layer: usize) -> usize {
        if layer == 0 {
            self.m0
        } else {
            self.m
        }
    }

    pub(crate) fn level_of(&self, node: usize) -> usize {
        if let Some(mapped) = &self.mapped {
            return mapped
                .i32_at(mapped.l0_record(node))
                .filter(|level| *level >= 0)
                .unwrap_or(0) as usize;
        }
        self.levels.get(node).copied().unwrap_or(0) as usize
    }

    /// Whether the graph reads out of a mapping instead of heap slabs.
    pub(crate) fn is_mapped(&self) -> bool {
        self.mapped.is_some()
    }

    /// Wraps a sealed chunk's `GraphStoreInner` region without copying it.
    ///
    /// `base` is the region's offset inside `src`. The layout is validated the
    /// same way [`Self::from_cpp_region`] validates it, but nothing is stored:
    /// levels, block offsets and counts are read where they are.
    pub fn from_cpp_region_mapped(
        src: crate::vector::ByteSource,
        base: usize,
        n: usize,
        m: usize,
    ) -> Result<Self, String> {
        let m0 = m.checked_mul(2).ok_or("graph size overflow")?;
        let l0 = VERTEX_L0_SIZE + 4 * m0;
        let lx = VERTEX_LX_SIZE + 4 * m;
        let bytes = src.as_ref().as_ref();
        let need = 8usize
            .checked_add(n.checked_mul(l0).ok_or("graph size overflow")?)
            .ok_or("graph size overflow")?;
        let end = base.checked_add(need).ok_or("graph size overflow")?;
        if bytes.len() < end {
            return Err(format!("graph region truncated: {} < {end}", bytes.len()));
        }
        let layer_sum = u64::from_le_bytes(bytes[base..base + 8].try_into().unwrap()) as usize;
        let upper_len = layer_sum.checked_mul(lx).ok_or("graph size overflow")?;
        let upper_end = end.checked_add(upper_len).ok_or("graph size overflow")?;
        if bytes.len() < upper_end {
            return Err("upper-layer region truncated".to_string());
        }
        // A mapped graph is read as `u32` slices, which is only sound when the
        // region starts on a four-byte boundary. The chunk layout does not
        // promise that: the graph region begins right after `n * record_size`
        // vector bytes, and a quantized record is not always a word wide
        // (`LvqStore` packs two dimensions per byte, so a 100-dimension column
        // stores 50 bytes). C++ reads such a file through pointers that tolerate
        // the misalignment, so the bytes are legal; a `u32` slice is not, and
        // `u32s_at` would refuse every slot read and silently empty the graph.
        // Copy the (comparatively small) graph region instead - the same decode
        // the unaligned writer would have forced.
        if base % 4 != 0 || l0 % 4 != 0 || lx % 4 != 0 {
            return Self::from_cpp_region(&bytes[base..upper_end], n, m0, m);
        }
        // Validate every vertex record, exactly as `from_cpp_region` does. The
        // mapped readers below treat a field that is missing, negative or out of
        // range as "level 0, no neighbours" (`level_of`, `neighbors`), so a
        // malformed record would not fail the load - it would quietly drop a
        // vertex out of the graph, and a query would answer short instead of
        // reporting a broken chunk.
        let upper_base = base + 8 + n * l0;
        let mut level_total = 0usize;
        for node in 0..n {
            let record = base + 8 + node * l0;
            let level = i32::from_le_bytes(bytes[record..record + 4].try_into().unwrap());
            if level < 0 {
                return Err(format!("vertex {node} has negative layer count"));
            }
            let level = level as usize;
            level_total += level;
            let count = i32::from_le_bytes(bytes[record + 16..record + 20].try_into().unwrap());
            if count < 0 || count as usize > m0 {
                return Err(format!("vertex {node} has invalid degree {count}"));
            }
            if level == 0 {
                continue;
            }
            let at = u64::from_le_bytes(bytes[record + 8..record + 16].try_into().unwrap());
            let at = usize::try_from(at)
                .map_err(|_| format!("vertex {node} has an out-of-range upper-layer offset"))?;
            // `layers_p_` is a byte offset into the upper region at block
            // granularity (`up_start * lx`), so a record that names anything
            // else cannot be walked.
            if at % lx != 0 {
                return Err(format!(
                    "vertex {node} has an unaligned upper-layer offset {at}"
                ));
            }
            let span = at
                .checked_add(level.checked_mul(lx).ok_or("graph size overflow")?)
                .ok_or("graph size overflow")?;
            if span > upper_len {
                return Err(format!(
                    "vertex {node}'s upper layers run past the graph region"
                ));
            }
            for layer in 0..level {
                let block = upper_base + at + layer * lx;
                let count = i32::from_le_bytes(bytes[block..block + 4].try_into().unwrap());
                if count < 0 || count as usize > m {
                    return Err(format!("vertex {node} has invalid degree {count}"));
                }
            }
        }
        if level_total != layer_sum {
            return Err(format!(
                "layer_sum {layer_sum} does not match the vertex records ({level_total})"
            ));
        }
        let mut graph = Self::new(m);
        graph.mapped = Some(MappedGraph {
            src,
            base,
            n,
            l0,
            lx,
            layer_sum,
        });
        Ok(graph)
    }

    /// Appends `count` vertices, taking their levels from `new_levels`.
    ///
    /// All the storage a batch needs is reserved here, before any thread starts
    /// writing: the builder only ever fills slots.
    pub(crate) fn push_levels(&mut self, new_levels: &[u32]) {
        assert!(
            self.mapped.is_none(),
            "a mapped graph is a published chunk and is read-only"
        );
        if new_levels.is_empty() {
            return;
        }
        let n = self.levels.len() + new_levels.len();
        self.levels.extend_from_slice(new_levels);
        self.l0
            .resize_with(n * self.m0, || AtomicU32::new(u32::MAX));
        self.l0_len.resize_with(n, || AtomicU32::new(0));
        self.up_start.reserve(new_levels.len());
        for &level in new_levels {
            if level == 0 {
                self.up_start.push(NO_BLOCK);
            } else {
                self.up_start.push(self.up_blocks as u32);
                self.up_blocks += level as usize;
            }
        }
        self.up
            .resize_with(self.up_blocks * self.m, || AtomicU32::new(u32::MAX));
        self.up_len
            .resize_with(self.up_blocks, || AtomicU32::new(0));
    }

    /// Appends one vertex with `level`.
    pub(crate) fn push_level(&mut self, level: usize) -> usize {
        assert!(
            self.mapped.is_none(),
            "a mapped graph is a published chunk and is read-only"
        );
        let node = self.levels.len();
        let level = level as u32;
        self.levels.push(level);
        self.l0
            .resize_with(self.levels.len() * self.m0, || AtomicU32::new(u32::MAX));
        self.l0_len.push(AtomicU32::new(0));
        if level == 0 {
            self.up_start.push(NO_BLOCK);
        } else {
            self.up_start.push(self.up_blocks as u32);
            self.up_blocks += level as usize;
            self.up
                .resize_with(self.up_blocks * self.m, || AtomicU32::new(u32::MAX));
            self.up_len
                .resize_with(self.up_blocks, || AtomicU32::new(0));
        }
        node
    }

    #[inline]
    fn block(&self, node: usize, layer: usize) -> Option<usize> {
        debug_assert!(layer >= 1);
        if layer > *self.levels.get(node)? as usize {
            return None;
        }
        let start = *self.up_start.get(node)?;
        if start == NO_BLOCK {
            return None;
        }
        let block = start as usize + (layer - 1);
        (block < self.up_blocks).then_some(block)
    }

    /// Neighbours of `node` on `layer`; empty when the vertex does not reach
    /// that layer or does not exist.
    #[inline]
    pub(crate) fn neighbors(&self, layer: usize, node: usize) -> Neighbors<'_> {
        if let Some(mapped) = &self.mapped {
            return mapped.neighbors(layer, node);
        }
        if layer == 0 {
            let Some(count) = self.l0_len.get(node) else {
                return Neighbors::empty();
            };
            let count = count.load(Ordering::Acquire) as usize;
            let start = node * self.m0;
            let end = (start + count).min(self.l0.len());
            return Neighbors::shared(&self.l0[start.min(end)..end]);
        }
        let Some(block) = self.block(node, layer) else {
            return Neighbors::empty();
        };
        let count = self.up_len[block].load(Ordering::Acquire) as usize;
        let start = block * self.m;
        let end = (start + count).min(self.up.len());
        Neighbors::shared(&self.up[start.min(end)..end])
    }

    /// Replaces the neighbour list of `node` on `layer`.
    pub(crate) fn set_neighbors(&self, layer: usize, node: usize, list: &[u32]) {
        assert!(
            self.mapped.is_none(),
            "a mapped graph is a published chunk and is read-only"
        );
        let (slots, count) = if layer == 0 {
            let start = node * self.m0;
            (
                &self.l0[start..(start + self.m0).min(self.l0.len())],
                &self.l0_len[node],
            )
        } else {
            let Some(block) = self.block(node, layer) else {
                return;
            };
            let start = block * self.m;
            (&self.up[start..start + self.m], &self.up_len[block])
        };
        let n = list.len().min(slots.len());
        for (slot, &value) in slots.iter().zip(list.iter()).take(n) {
            slot.store(value, Ordering::Relaxed);
        }
        count.store(n as u32, Ordering::Release);
    }

    /// `GraphStoreInner::AddNeighbor`: appends when there is room.
    /// Returns `false` when the list is already full.
    pub(crate) fn push_neighbor(&self, layer: usize, node: usize, neighbor: u32) -> bool {
        if self.mapped.is_some() {
            return false;
        }
        let (slots, count) = if layer == 0 {
            let start = node * self.m0;
            (
                &self.l0[start..(start + self.m0).min(self.l0.len())],
                &self.l0_len[node],
            )
        } else {
            let Some(block) = self.block(node, layer) else {
                return false;
            };
            let start = block * self.m;
            (&self.up[start..start + self.m], &self.up_len[block])
        };
        let len = count.load(Ordering::Acquire) as usize;
        if len >= slots.len() {
            return false;
        }
        slots[len].store(neighbor, Ordering::Relaxed);
        count.store((len + 1) as u32, Ordering::Release);
        true
    }

    /// Byte size the graph occupies in a chunk: the `layer_sum` header, one
    /// level-0 record per vertex and one upper-layer record per (vertex, layer).
    pub(crate) fn serialized_size(&self) -> usize {
        if let Some(mapped) = &self.mapped {
            return 8 + mapped.n * mapped.l0 + mapped.layer_sum * mapped.lx;
        }
        let layer_sum: usize = self.levels.iter().map(|&l| l as usize).sum();
        8 + self.len() * self.level0_stride() + layer_sum * self.levelx_stride()
    }

    /// Bytes held by the graph, for the memindex budget.
    pub(crate) fn mem_bytes(&self) -> usize {
        if self.mapped.is_some() {
            // The lists live in the page cache; only the mapping handle is ours.
            return 0;
        }
        self.levels.capacity() * 4
            + self.l0.capacity() * 4
            + self.l0_len.capacity() * 4
            + self.up.capacity() * 4
            + self.up_len.capacity() * 4
            + self.up_start.capacity() * 4
    }

    /// Builds the store from layer-major neighbour lists (`lists[layer][node]`).
    pub(crate) fn from_layers(m: usize, levels: &[u32], lists: &[Vec<Vec<u32>>]) -> Self {
        let mut graph = Self::new(m);
        graph.push_levels(levels);
        for (layer, nodes) in lists.iter().enumerate() {
            for (node, list) in nodes.iter().enumerate() {
                if node >= graph.len() || layer > graph.level_of(node) {
                    continue;
                }
                graph.set_neighbors(layer, node, list);
            }
        }
        graph
    }

    /// Byte size of one level-0 record (`VERTEX_L0_SIZE + 4 * Mmax0`).
    pub(crate) fn level0_stride(&self) -> usize {
        VERTEX_L0_SIZE + 4 * self.m0
    }

    /// Byte size of one upper-layer record (`VERTEX_LX_SIZE + 4 * Mmax`).
    pub(crate) fn levelx_stride(&self) -> usize {
        VERTEX_LX_SIZE + 4 * self.m
    }

    /// Encodes the reference's `GraphStoreInner` region: `u64 layer_sum`, the
    /// level-0 records, then the upper-layer records.
    ///
    /// `layers_p_` is written as a `u64` byte offset into the upper region —
    /// the in-memory pointer has no meaning in a file, which is what
    /// `graph_store.cppm` does when it saves a chunk.
    pub fn to_cpp_region(&self) -> Vec<u8> {
        if let Some(mapped) = &self.mapped {
            // Already in this layout: hand the region back as it is.
            return mapped.region().to_vec();
        }
        let n = self.len();
        let l0 = self.level0_stride();
        let lx = self.levelx_stride();
        let layer_sum: u64 = self.levels.iter().map(|&l| l as u64).sum();
        let mut out = Vec::with_capacity(8 + n * l0 + layer_sum as usize * lx);
        out.extend_from_slice(&layer_sum.to_le_bytes());

        let layers_p: Vec<u64> = self
            .up_start
            .iter()
            .take(n)
            .map(|&start| (start as usize * lx) as u64)
            .collect();
        for node in 0..n {
            let start = out.len();
            out.extend_from_slice(&(self.levels[node] as i32).to_le_bytes());
            out.resize(start + 16, 0);
            out[start + 8..start + 16].copy_from_slice(&layers_p[node].to_le_bytes());
            let nbrs = self.neighbors(0, node);
            out.extend_from_slice(&(nbrs.len() as i32).to_le_bytes());
            for k in 0..self.m0 {
                out.extend_from_slice(&nbrs.get(k).unwrap_or(0).to_le_bytes());
            }
            out.resize(start + l0, 0);
        }
        for node in 0..n {
            for layer in 1..=self.levels[node] as usize {
                let start = out.len();
                let nbrs = self.neighbors(layer, node);
                out.extend_from_slice(&(nbrs.len() as i32).to_le_bytes());
                for k in 0..self.m {
                    out.extend_from_slice(&nbrs.get(k).unwrap_or(0).to_le_bytes());
                }
                out.resize(start + lx, 0);
            }
        }
        out
    }

    /// Decodes a `GraphStoreInner` region written by [`Self::to_cpp_region`]
    /// (or by the reference).
    pub fn from_cpp_region(region: &[u8], n: usize, m0: usize, m: usize) -> Result<Self, String> {
        let l0 = VERTEX_L0_SIZE + 4 * m0;
        let lx = VERTEX_LX_SIZE + 4 * m;
        let need = 8usize
            .checked_add(n.checked_mul(l0).ok_or("graph size overflow")?)
            .ok_or("graph size overflow")?;
        if region.len() < need {
            return Err(format!("graph region truncated: {} < {need}", region.len()));
        }
        let layer_sum = u64::from_le_bytes(region[0..8].try_into().unwrap()) as usize;
        let mut graph = Self::new(m);
        let mut levels = Vec::with_capacity(n);
        for node in 0..n {
            let base = 8 + node * l0;
            let layer_count = i32::from_le_bytes(region[base..base + 4].try_into().unwrap());
            if layer_count < 0 {
                return Err(format!("vertex {node} has negative layer count"));
            }
            levels.push(layer_count as u32);
        }
        if levels.iter().map(|&l| l as usize).sum::<usize>() != layer_sum {
            return Err("layer_sum does not match the vertex records".to_string());
        }
        graph.push_levels(&levels);
        for node in 0..n {
            let base = 8 + node * l0;
            let count = i32::from_le_bytes(region[base + 16..base + 20].try_into().unwrap());
            if count < 0 || count as usize > m0 {
                return Err(format!("vertex {node} has invalid degree {count}"));
            }
            let start = base + 20;
            let list: Vec<u32> = (0..count as usize)
                .map(|k| {
                    u32::from_le_bytes(region[start + 4 * k..start + 4 * k + 4].try_into().unwrap())
                })
                .collect();
            graph.set_neighbors(0, node, &list);
        }
        let mut cursor = 8 + n * l0;
        for node in 0..n {
            for layer in 1..=graph.levels[node] as usize {
                if cursor + lx > region.len() {
                    return Err("upper-layer region truncated".to_string());
                }
                let count = i32::from_le_bytes(region[cursor..cursor + 4].try_into().unwrap());
                if count < 0 || count as usize > m {
                    return Err(format!("vertex {node} has invalid degree {count}"));
                }
                let start = cursor + 4;
                let list: Vec<u32> = (0..count as usize)
                    .map(|k| {
                        u32::from_le_bytes(
                            region[start + 4 * k..start + 4 * k + 4].try_into().unwrap(),
                        )
                    })
                    .collect();
                graph.set_neighbors(layer, node, &list);
                cursor += lx;
            }
        }
        Ok(graph)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn small_graph() -> GraphStore {
        let mut graph = GraphStore::new(2);
        // node 0: level 1, node 1: level 0, node 2: level 2
        graph.push_levels(&[1, 0, 2]);
        graph.set_neighbors(0, 0, &[1, 2]);
        graph.set_neighbors(1, 0, &[2]);
        graph.set_neighbors(0, 1, &[0]);
        graph.set_neighbors(0, 2, &[0, 1]);
        graph.set_neighbors(1, 2, &[0]);
        graph.set_neighbors(2, 2, &[1]);
        graph
    }

    #[test]
    fn neighbors_follow_the_level_of_the_vertex() {
        let graph = small_graph();
        assert_eq!(graph.neighbors(0, 2).to_vec(), vec![0, 1]);
        assert_eq!(graph.neighbors(1, 2).to_vec(), vec![0]);
        assert_eq!(graph.neighbors(2, 2).to_vec(), vec![1]);
        // Node 1 never reaches layer 1 and node 0 never reaches layer 2.
        assert_eq!(graph.neighbors(1, 1).len(), 0);
        assert_eq!(graph.neighbors(2, 0).len(), 0);
        assert_eq!(graph.neighbors(0, 9).len(), 0);
    }

    #[test]
    fn upper_layers_are_allocated_per_vertex_not_globally() {
        let mut graph = GraphStore::new(16);
        // 1000 vertices, only the first has an upper layer.
        let mut levels = vec![0u32; 1000];
        levels[0] = 3;
        graph.push_levels(&levels);
        // 1000 * 2M slots for layer 0, 3 * M slots for the single upper vertex.
        assert_eq!(graph.up.len(), 3 * 16);
        assert_eq!(graph.l0.len(), 1000 * 32);
        // Per-vertex overhead: 2M*4 (layer 0) + 4 (count) + 4 (start) + 4 (level).
        let per_vertex = 32 * 4 + 4 + 4 + 4;
        assert!(graph.mem_bytes() >= 1000 * per_vertex);
        assert!(graph.mem_bytes() < 1000 * per_vertex * 2);
    }

    #[test]
    fn region_round_trips_through_the_reference_layout() {
        let graph = small_graph();
        let region = graph.to_cpp_region();
        assert_eq!(&region[0..8], &3u64.to_le_bytes());
        let decoded = GraphStore::from_cpp_region(&region, 3, 4, 2).unwrap();
        assert_eq!(
            (0..3)
                .map(|node| decoded.level_of(node))
                .collect::<Vec<_>>(),
            (0..3).map(|node| graph.level_of(node)).collect::<Vec<_>>()
        );
        for node in 0..3 {
            for layer in 0..=graph.level_of(node) {
                assert_eq!(
                    decoded.neighbors(layer, node).to_vec(),
                    graph.neighbors(layer, node).to_vec(),
                    "node {node} layer {layer}"
                );
            }
        }
        assert_eq!(decoded.to_cpp_region(), region);
    }

    /// The region can be searched where it lies: a mapped graph answers exactly
    /// what the decoded one answers, and keeps no heap at all.
    #[test]
    fn a_region_can_be_walked_in_place() {
        let owned = small_graph();
        let region = owned.to_cpp_region();
        let src: crate::vector::ByteSource = std::sync::Arc::new(region.clone());
        let mapped = GraphStore::from_cpp_region_mapped(src, 0, 3, 2).unwrap();
        assert!(mapped.is_mapped());
        assert_eq!(mapped.len(), owned.len());
        assert_eq!(mapped.serialized_size(), region.len());
        assert_eq!(mapped.mem_bytes(), 0, "a mapped graph keeps no heap");
        for node in 0..3 {
            assert_eq!(mapped.level_of(node), owned.level_of(node));
            for layer in 0..=owned.level_of(node) {
                assert_eq!(
                    mapped.neighbors(layer, node).to_vec(),
                    owned.neighbors(layer, node).to_vec(),
                    "node {node} layer {layer}"
                );
            }
            // A layer the vertex does not reach stays empty.
            assert_eq!(mapped.neighbors(owned.level_of(node) + 1, node).len(), 0);
        }
        assert_eq!(mapped.neighbors(0, 9).len(), 0, "no vertex 9");
        // A rewrite hands the region back unchanged.
        assert_eq!(mapped.to_cpp_region(), region);
    }

    #[test]
    fn a_truncated_region_is_rejected() {
        let region = small_graph().to_cpp_region();
        let short = region[..region.len() - 4].to_vec();
        let src: crate::vector::ByteSource = std::sync::Arc::new(short);
        assert!(GraphStore::from_cpp_region_mapped(src, 0, 3, 2).is_err());
    }

    /// The chunk layout does not promise a word-aligned graph region - it starts
    /// right after `n * record_size` vector bytes, and a quantized record is not
    /// always four bytes wide - and a `u32` view of a misaligned region reads
    /// every slot as empty. Such a region is copied instead of mapped.
    #[test]
    fn a_misaligned_region_is_copied_rather_than_mapped() {
        let owned = small_graph();
        let region = owned.to_cpp_region();
        let mut shifted = vec![0u8];
        shifted.extend_from_slice(&region);
        let src: crate::vector::ByteSource = std::sync::Arc::new(shifted);
        let mapped = GraphStore::from_cpp_region_mapped(src, 1, 3, 2).unwrap();
        assert!(!mapped.is_mapped(), "an unaligned region cannot be mapped");
        for node in 0..3 {
            assert_eq!(mapped.level_of(node), owned.level_of(node));
            for layer in 0..=owned.level_of(node) {
                assert_eq!(
                    mapped.neighbors(layer, node).to_vec(),
                    owned.neighbors(layer, node).to_vec(),
                    "node {node} layer {layer}"
                );
            }
        }
    }

    /// A mapped graph reads its levels, degrees and block offsets on demand and
    /// quietly treats a bad field as "level 0, no neighbours", so a malformed
    /// record has to fail the load the way `from_cpp_region` fails it.
    #[test]
    fn malformed_vertex_records_are_rejected_when_mapped() {
        let region = small_graph().to_cpp_region();
        let load = |bytes: Vec<u8>| {
            let src: crate::vector::ByteSource = std::sync::Arc::new(bytes);
            GraphStore::from_cpp_region_mapped(src, 0, 3, 2)
        };
        // Node 0's record starts at 8: level at +0, `layers_p_` at +8,
        // `neighbor_n_` at +16.
        let mut negative = region.clone();
        negative[8..12].copy_from_slice(&(-1i32).to_le_bytes());
        assert!(load(negative).is_err(), "negative level");
        let mut degree = region.clone();
        degree[8 + 16..8 + 20].copy_from_slice(&9i32.to_le_bytes());
        assert!(load(degree).is_err(), "degree above Mmax0");
        let mut offset = region.clone();
        offset[8 + 8..8 + 16].copy_from_slice(&(4 * 12u64).to_le_bytes());
        assert!(load(offset).is_err(), "upper layers past the region");
        let mut sum = region;
        sum[0] = 9;
        assert!(load(sum).is_err(), "layer_sum mismatch");
    }

    #[test]
    fn region_rejects_inconsistent_layer_sum() {
        let mut region = small_graph().to_cpp_region();
        region[0] = 9;
        assert!(GraphStore::from_cpp_region(&region, 3, 4, 2).is_err());
        assert!(GraphStore::from_cpp_region(&region[..10], 3, 4, 2).is_err());
    }

    #[test]
    fn push_neighbor_stops_at_the_degree_limit() {
        let mut graph = GraphStore::new(1);
        graph.push_level(1);
        assert!(graph.push_neighbor(0, 0, 7));
        assert!(graph.push_neighbor(0, 0, 8));
        assert!(!graph.push_neighbor(0, 0, 9));
        assert_eq!(graph.neighbors(0, 0).to_vec(), vec![7, 8]);
        assert!(graph.push_neighbor(1, 0, 5));
        assert!(!graph.push_neighbor(1, 0, 6));
        assert_eq!(graph.neighbors(1, 0).len(), 1);
    }
}
