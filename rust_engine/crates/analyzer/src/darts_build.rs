//! Darts-clone's dictionary builder: DAWG -> double-array, the code the
//! reference runs when rag/huqie.trie is missing
//! (rag_analyzer_impl.cpp:752-782 over third_party/darts/darts.h).
//!
//! This is a faithful port, including libstdc++'s std::sort (introsort with a
//! 16-element insertion-sort tail and a heapsort fallback): the trie is a pure
//! function of the dictionary, and the reference's build reproduces the shipped
//! huqie.trie byte for byte. Anything less than the same ordering changes the
//! layout, because the DAWG deduplicates nodes in insertion order.
//!
//! Only what the RAG analyzer needs is here: build + save. Reading a built trie
//! lives in analyzer::DartsTrie.

/// DoubleArrayBuilder::BLOCK_SIZE.
const BLOCK_SIZE: usize = 256;
/// DoubleArrayBuilder::NUM_EXTRA_BLOCKS.
const NUM_EXTRA_BLOCKS: usize = 16;
/// DoubleArrayBuilder::NUM_EXTRAS.
const NUM_EXTRAS: usize = BLOCK_SIZE * NUM_EXTRA_BLOCKS;
/// DoubleArrayBuilder::UPPER_MASK.
const UPPER_MASK: u32 = 0xFF << 21;
/// DoubleArrayBuilder::LOWER_MASK.
const LOWER_MASK: u32 = 0xFF;

// ---------------------------------------------------------------------------
// Succinct bit vector (darts.h:691-777)
// ---------------------------------------------------------------------------

#[derive(Default)]
struct BitVector {
    units: Vec<u32>,
    ranks: Vec<u32>,
    num_ones: u64,
    size: usize,
}

impl BitVector {
    fn get(&self, id: usize) -> bool {
        (self.units[id / 32] >> (id % 32)) & 1 == 1
    }

    /// BitVector::rank - the number of set bits in [0, id].
    fn rank(&self, id: usize) -> u32 {
        let unit_id = id / 32;
        let mask = u32::MAX >> (32 - (id % 32) - 1);
        self.ranks[unit_id] + (self.units[unit_id] & mask).count_ones()
    }

    fn set(&mut self, id: usize, bit: bool) {
        if bit {
            self.units[id / 32] |= 1 << (id % 32);
        } else {
            self.units[id / 32] &= !(1 << (id % 32));
        }
    }

    fn append(&mut self) {
        if self.size % 32 == 0 {
            self.units.push(0);
        }
        self.size += 1;
    }

    fn len(&self) -> usize {
        self.size
    }

    fn build(&mut self) {
        self.ranks = vec![0; self.units.len()];
        self.num_ones = 0;
        for i in 0..self.units.len() {
            self.ranks[i] = self.num_ones as u32;
            self.num_ones += u64::from(self.units[i].count_ones());
        }
    }
}
// ---------------------------------------------------------------------------
// DAWG (darts.h:826-1197)
// ---------------------------------------------------------------------------

#[derive(Clone, Copy, Default)]
struct DawgNode {
    child: u32,
    sibling: u32,
    label: u8,
    is_state: bool,
    has_sibling: bool,
}

impl DawgNode {
    fn unit(&self) -> u32 {
        if self.label == 0 {
            (self.child << 1) | u32::from(self.has_sibling)
        } else {
            (self.child << 2) | if self.is_state { 2 } else { 0 } | u32::from(self.has_sibling)
        }
    }
}

#[derive(Clone, Copy, Default)]
struct DawgUnit(u32);

impl DawgUnit {
    fn child(&self) -> u32 {
        self.0 >> 2
    }
    fn has_sibling(&self) -> bool {
        self.0 & 1 == 1
    }
    fn value(&self) -> i32 {
        (self.0 >> 1) as i32
    }
    fn is_state(&self) -> bool {
        self.0 & 2 == 2
    }
}

#[derive(Default)]
struct DawgBuilder {
    nodes: Vec<DawgNode>,
    units: Vec<DawgUnit>,
    labels: Vec<u8>,
    is_intersections: BitVector,
    table: Vec<u32>,
    node_stack: Vec<u32>,
    recycle_bin: Vec<u32>,
    num_states: usize,
}

/// DawgBuilder::hash - the mixer the unit/node tables use.
fn dawg_hash(mut key: u32) -> u32 {
    key = key.wrapping_neg().wrapping_add(key << 15);
    key ^= key >> 12;
    key = key.wrapping_add(key << 2);
    key ^= key >> 4;
    key = key.wrapping_mul(2057);
    key ^ (key >> 16)
}

impl DawgBuilder {
    fn new() -> Self {
        let mut b = Self {
            table: vec![0; 1 << 10],
            ..Self::default()
        };
        b.append_node();
        b.append_unit();
        b.num_states = 1;
        b.nodes[0].label = 0xFF;
        b.node_stack.push(0);
        b
    }

    fn root(&self) -> u32 {
        0
    }

    fn child(&self, id: u32) -> u32 {
        self.units[id as usize].child()
    }

    fn sibling(&self, id: u32) -> u32 {
        if self.units[id as usize].has_sibling() {
            id + 1
        } else {
            0
        }
    }

    fn value(&self, id: u32) -> i32 {
        self.units[id as usize].value()
    }

    fn is_leaf(&self, id: u32) -> bool {
        self.label(id) == 0
    }

    fn label(&self, id: u32) -> u8 {
        self.labels[id as usize]
    }

    fn is_intersection(&self, id: u32) -> bool {
        self.is_intersections.get(id as usize)
    }

    fn intersection_id(&self, id: u32) -> u32 {
        self.is_intersections.rank(id as usize) - 1
    }

    fn num_intersections(&self) -> usize {
        self.is_intersections.num_ones as usize
    }

    fn size(&self) -> usize {
        self.units.len()
    }

    fn finish(&mut self) {
        self.flush(0);
        self.units[0] = DawgUnit(self.nodes[0].unit());
        self.labels[0] = self.nodes[0].label;
        self.nodes.clear();
        self.table.clear();
        self.node_stack.clear();
        self.recycle_bin.clear();
        self.is_intersections.build();
    }

    fn insert(&mut self, key: &[u8], value: i32) {
        let length = key.len();
        assert!(value >= 0, "failed to insert key: negative value");
        assert!(length > 0, "failed to insert key: zero-length key");

        let mut id = 0u32;
        let mut key_pos = 0usize;
        while key_pos <= length {
            let child_id = self.nodes[id as usize].child;
            if child_id == 0 {
                break;
            }
            // key[key_pos] past the end is the C string terminator.
            let key_label = if key_pos < length { key[key_pos] } else { 0 };
            assert!(
                !(key_pos < length && key_label == 0),
                "failed to insert key: invalid null character"
            );
            let unit_label = self.nodes[child_id as usize].label;
            assert!(
                key_label >= unit_label,
                "failed to insert key: wrong key order"
            );
            if key_label > unit_label {
                self.nodes[child_id as usize].has_sibling = true;
                self.flush(child_id);
                break;
            }
            id = child_id;
            key_pos += 1;
        }
        if key_pos > length {
            return;
        }

        while key_pos <= length {
            let key_label = if key_pos < length { key[key_pos] } else { 0 };
            let child_id = self.append_node();
            if self.nodes[id as usize].child == 0 {
                self.nodes[child_id as usize].is_state = true;
            }
            self.nodes[child_id as usize].sibling = self.nodes[id as usize].child;
            self.nodes[child_id as usize].label = key_label;
            self.nodes[id as usize].child = child_id;
            self.node_stack.push(child_id);
            id = child_id;
            key_pos += 1;
        }
        self.nodes[id as usize].child = value as u32;
    }

    fn flush(&mut self, id: u32) {
        while *self.node_stack.last().expect("node stack") != id {
            let node_id = self.node_stack.pop().expect("node stack");
            if self.num_states >= self.table.len() - (self.table.len() >> 2) {
                self.expand_table();
            }
            let mut num_siblings = 0u32;
            let mut i = node_id;
            while i != 0 {
                num_siblings += 1;
                i = self.nodes[i as usize].sibling;
            }

            let mut hash_id = 0u32;
            let match_id = self.find_node(node_id, &mut hash_id);
            let match_id = if match_id != 0 {
                self.is_intersections.set(match_id as usize, true);
                match_id
            } else {
                let mut unit_id = 0u32;
                for _ in 0..num_siblings {
                    unit_id = self.append_unit();
                }
                let mut i = node_id;
                while i != 0 {
                    self.units[unit_id as usize] = DawgUnit(self.nodes[i as usize].unit());
                    self.labels[unit_id as usize] = self.nodes[i as usize].label;
                    unit_id -= 1;
                    i = self.nodes[i as usize].sibling;
                }
                let match_id = unit_id + 1;
                self.table[hash_id as usize] = match_id;
                self.num_states += 1;
                match_id
            };

            let mut i = node_id;
            while i != 0 {
                let next = self.nodes[i as usize].sibling;
                self.recycle_bin.push(i);
                i = next;
            }

            let parent = *self.node_stack.last().expect("node stack");
            self.nodes[parent as usize].child = match_id;
        }
        self.node_stack.pop();
    }

    fn expand_table(&mut self) {
        let table_size = self.table.len() << 1;
        self.table.clear();
        self.table = vec![0; table_size];
        for i in 1..self.units.len() {
            let id = i as u32;
            if self.labels[id as usize] == 0 || self.units[id as usize].is_state() {
                let mut hash_id = 0u32;
                self.find_unit(id, &mut hash_id);
                self.table[hash_id as usize] = id;
            }
        }
    }

    /// DawgBuilder::find_unit always returns 0: it only reports the free slot.
    fn find_unit(&self, id: u32, hash_id: &mut u32) -> u32 {
        *hash_id = self.hash_unit(id) % self.table.len() as u32;
        loop {
            if self.table[*hash_id as usize] == 0 {
                break;
            }
            *hash_id = (*hash_id + 1) % self.table.len() as u32;
        }
        0
    }

    fn find_node(&self, node_id: u32, hash_id: &mut u32) -> u32 {
        *hash_id = self.hash_node(node_id) % self.table.len() as u32;
        loop {
            let unit_id = self.table[*hash_id as usize];
            if unit_id == 0 {
                break;
            }
            if self.are_equal(node_id, unit_id) {
                return unit_id;
            }
            *hash_id = (*hash_id + 1) % self.table.len() as u32;
        }
        0
    }

    fn are_equal(&self, node_id: u32, unit_id: u32) -> bool {
        let mut unit_id = unit_id;
        let mut i = self.nodes[node_id as usize].sibling;
        while i != 0 {
            if !self.units[unit_id as usize].has_sibling() {
                return false;
            }
            unit_id += 1;
            i = self.nodes[i as usize].sibling;
        }
        if self.units[unit_id as usize].has_sibling() {
            return false;
        }
        let mut i = node_id;
        while i != 0 {
            if self.nodes[i as usize].unit() != self.units[unit_id as usize].0
                || self.nodes[i as usize].label != self.labels[unit_id as usize]
            {
                return false;
            }
            i = self.nodes[i as usize].sibling;
            unit_id -= 1;
        }
        true
    }

    fn hash_unit(&self, id: u32) -> u32 {
        let mut hash_value = 0u32;
        let mut id = id;
        while id != 0 {
            let unit = self.units[id as usize].0;
            let label = self.labels[id as usize];
            hash_value ^= dawg_hash((u32::from(label) << 24) ^ unit);
            if !self.units[id as usize].has_sibling() {
                break;
            }
            id += 1;
        }
        hash_value
    }

    fn hash_node(&self, id: u32) -> u32 {
        let mut hash_value = 0u32;
        let mut id = id;
        while id != 0 {
            let unit = self.nodes[id as usize].unit();
            let label = self.nodes[id as usize].label;
            hash_value ^= dawg_hash((u32::from(label) << 24) ^ unit);
            id = self.nodes[id as usize].sibling;
        }
        hash_value
    }

    fn append_unit(&mut self) -> u32 {
        self.is_intersections.append();
        self.units.push(DawgUnit(0));
        self.labels.push(0);
        (self.is_intersections.len() - 1) as u32
    }

    fn append_node(&mut self) -> u32 {
        if let Some(id) = self.recycle_bin.pop() {
            self.nodes[id as usize] = DawgNode::default();
            id
        } else {
            self.nodes.push(DawgNode::default());
            (self.nodes.len() - 1) as u32
        }
    }
}
// ---------------------------------------------------------------------------
// Double-array builder (darts.h:1208-1690)
// ---------------------------------------------------------------------------

#[derive(Clone, Copy, Default)]
struct BuilderUnit(u32);

impl BuilderUnit {
    fn set_has_leaf(&mut self, has_leaf: bool) {
        if has_leaf {
            self.0 |= 1 << 8;
        } else {
            self.0 &= !(1 << 8);
        }
    }

    fn set_value(&mut self, value: i32) {
        self.0 = value as u32 | (1 << 31);
    }

    fn set_label(&mut self, label: u8) {
        self.0 = (self.0 & !0xFF) | u32::from(label);
    }

    fn set_offset(&mut self, offset: u32) {
        assert!(offset < 1 << 29, "failed to modify unit: too large offset");
        self.0 &= (1 << 31) | (1 << 8) | 0xFF;
        if offset < 1 << 21 {
            self.0 |= offset << 10;
        } else {
            self.0 |= (offset << 2) | (1 << 9);
        }
    }
}

#[derive(Clone, Copy, Default)]
struct ExtraUnit {
    prev: u32,
    next: u32,
    is_fixed: bool,
    is_used: bool,
}

struct DoubleArrayBuilder {
    units: Vec<BuilderUnit>,
    extras: Vec<ExtraUnit>,
    labels: Vec<u8>,
    table: Vec<u32>,
    extras_head: u32,
}

impl DoubleArrayBuilder {
    fn new() -> Self {
        Self {
            units: Vec::new(),
            extras: vec![ExtraUnit::default(); NUM_EXTRAS],
            labels: Vec::new(),
            table: Vec::new(),
            extras_head: 0,
        }
    }

    fn num_blocks(&self) -> usize {
        self.units.len() / BLOCK_SIZE
    }

    fn extra(&self, id: u32) -> &ExtraUnit {
        &self.extras[id as usize % NUM_EXTRAS]
    }

    fn extra_mut(&mut self, id: u32) -> &mut ExtraUnit {
        &mut self.extras[id as usize % NUM_EXTRAS]
    }

    fn build(&mut self, dawg: &DawgBuilder) {
        let mut num_units = 1usize;
        while num_units < dawg.size() {
            num_units <<= 1;
        }
        self.units.reserve(num_units);
        self.table = vec![0; dawg.num_intersections()];

        self.reserve_id(0);
        self.extra_mut(0).is_used = true;
        self.units[0].set_offset(1);
        self.units[0].set_label(0);

        if dawg.child(dawg.root()) != 0 {
            self.build_from_dawg(dawg, dawg.root(), 0);
        }
        self.fix_all_blocks();
    }

    fn build_from_dawg(&mut self, dawg: &DawgBuilder, dawg_id: u32, dic_id: u32) {
        let dawg_child_id = dawg.child(dawg_id);
        if dawg.is_intersection(dawg_child_id) {
            let intersection_id = dawg.intersection_id(dawg_child_id);
            let mut offset = self.table[intersection_id as usize];
            if offset != 0 {
                offset ^= dic_id;
                if offset & UPPER_MASK == 0 || offset & LOWER_MASK == 0 {
                    if dawg.is_leaf(dawg_child_id) {
                        self.units[dic_id as usize].set_has_leaf(true);
                    }
                    self.units[dic_id as usize].set_offset(offset);
                    return;
                }
            }
        }

        let offset = self.arrange_from_dawg(dawg, dawg_id, dic_id);
        if dawg.is_intersection(dawg_child_id) {
            let intersection_id = dawg.intersection_id(dawg_child_id);
            self.table[intersection_id as usize] = offset;
        }

        let mut child = dawg_child_id;
        loop {
            let child_label = dawg.label(child);
            let dic_child_id = offset ^ u32::from(child_label);
            if child_label != 0 {
                self.build_from_dawg(dawg, child, dic_child_id);
            }
            child = dawg.sibling(child);
            if child == 0 {
                break;
            }
        }
    }

    fn arrange_from_dawg(&mut self, dawg: &DawgBuilder, dawg_id: u32, dic_id: u32) -> u32 {
        self.labels.clear();
        let mut child = dawg.child(dawg_id);
        while child != 0 {
            self.labels.push(dawg.label(child));
            child = dawg.sibling(child);
        }

        let offset = self.find_valid_offset(dic_id);
        self.units[dic_id as usize].set_offset(dic_id ^ offset);

        let mut child = dawg.child(dawg_id);
        for i in 0..self.labels.len() {
            let label = self.labels[i];
            let dic_child_id = offset ^ u32::from(label);
            self.reserve_id(dic_child_id);
            if dawg.is_leaf(child) {
                self.units[dic_id as usize].set_has_leaf(true);
                self.units[dic_child_id as usize].set_value(dawg.value(child));
            } else {
                self.units[dic_child_id as usize].set_label(label);
            }
            child = dawg.sibling(child);
        }
        self.extra_mut(offset).is_used = true;
        offset
    }

    fn find_valid_offset(&self, id: u32) -> u32 {
        if self.extras_head as usize >= self.units.len() {
            return self.units.len() as u32 | (id & LOWER_MASK);
        }
        let mut unfixed_id = self.extras_head;
        loop {
            let offset = unfixed_id ^ u32::from(self.labels[0]);
            if self.is_valid_offset(id, offset) {
                return offset;
            }
            unfixed_id = self.extra(unfixed_id).next;
            if unfixed_id == self.extras_head {
                break;
            }
        }
        self.units.len() as u32 | (id & LOWER_MASK)
    }

    fn is_valid_offset(&self, id: u32, offset: u32) -> bool {
        if self.extra(offset).is_used {
            return false;
        }
        let rel_offset = id ^ offset;
        if (rel_offset & LOWER_MASK) != 0 && (rel_offset & UPPER_MASK) != 0 {
            return false;
        }
        for i in 1..self.labels.len() {
            if self.extra(offset ^ u32::from(self.labels[i])).is_fixed {
                return false;
            }
        }
        true
    }

    fn reserve_id(&mut self, id: u32) {
        if id as usize >= self.units.len() {
            self.expand_units();
        }
        if id == self.extras_head {
            self.extras_head = self.extra(id).next;
            if self.extras_head == id {
                self.extras_head = self.units.len() as u32;
            }
        }
        let (prev, next) = {
            let e = self.extra(id);
            (e.prev, e.next)
        };
        self.extra_mut(prev).next = next;
        self.extra_mut(next).prev = prev;
        self.extra_mut(id).is_fixed = true;
    }

    fn expand_units(&mut self) {
        let src_num_units = self.units.len();
        let src_num_blocks = self.num_blocks();
        let dest_num_units = src_num_units + BLOCK_SIZE;
        let dest_num_blocks = src_num_blocks + 1;

        if dest_num_blocks > NUM_EXTRA_BLOCKS {
            self.fix_block((src_num_blocks - NUM_EXTRA_BLOCKS) as u32);
        }
        self.units.resize(dest_num_units, BuilderUnit::default());
        if dest_num_blocks > NUM_EXTRA_BLOCKS {
            for id in src_num_units..dest_num_units {
                let e = self.extra_mut(id as u32);
                e.is_used = false;
                e.is_fixed = false;
            }
        }
        for i in src_num_units + 1..dest_num_units {
            self.extra_mut((i - 1) as u32).next = i as u32;
            self.extra_mut(i as u32).prev = (i - 1) as u32;
        }
        // The order matters: each step reads a field the previous one wrote,
        // exactly like darts.h:1639-1655. Getting it wrong leaves the free list
        // as a self-loop, and find_valid_offset then falls back to
        // units.size(), which shifts every offset in the array.
        let src = src_num_units as u32;
        let dest_last = (dest_num_units - 1) as u32;
        self.extra_mut(src).prev = dest_last;
        self.extra_mut(dest_last).next = src;
        let head_prev = self.extra(self.extras_head).prev;
        self.extra_mut(src).prev = head_prev;
        self.extra_mut(dest_last).next = self.extras_head;
        self.extra_mut(head_prev).next = src;
        self.extra_mut(self.extras_head).prev = dest_last;
    }

    fn fix_all_blocks(&mut self) {
        let begin = if self.num_blocks() > NUM_EXTRA_BLOCKS {
            self.num_blocks() - NUM_EXTRA_BLOCKS
        } else {
            0
        };
        let end = self.num_blocks();
        for block_id in begin..end {
            self.fix_block(block_id as u32);
        }
    }

    fn fix_block(&mut self, block_id: u32) {
        let begin = block_id * BLOCK_SIZE as u32;
        let end = begin + BLOCK_SIZE as u32;
        let mut unused_offset = 0u32;
        let mut offset = begin;
        while offset != end {
            if !self.extra(offset).is_used {
                unused_offset = offset;
                break;
            }
            offset += 1;
        }
        let mut id = begin;
        while id != end {
            if !self.extra(id).is_fixed {
                self.reserve_id(id);
                self.units[id as usize].set_label((id ^ unused_offset) as u8);
            }
            id += 1;
        }
    }

    /// The raw little-endian unit array DartsTrie::load reads back.
    fn into_bytes(self) -> Vec<u8> {
        let mut out = Vec::with_capacity(self.units.len() * 4);
        for unit in self.units {
            out.extend_from_slice(&unit.0.to_le_bytes());
        }
        out
    }
}
// ---------------------------------------------------------------------------
// libstdc++ std::sort (bits/stl_algo.h), so the key order matches
// ---------------------------------------------------------------------------

/// std::sort's _S_threshold.
const SORT_THRESHOLD: usize = 16;

/// std::__lg: floor(log2(n)).
fn lg(mut n: usize) -> usize {
    let mut k = 0;
    while n > 1 {
        n >>= 1;
        k += 1;
    }
    k
}

/// std::sort on (key, value) with the key compared as std::string does.
///
/// Copies are what the reference does (std::sort moves whole pairs), so the
/// element type only has to be Clone.
pub(crate) fn sort_by_key<K: Ord + Clone, V: Clone>(a: &mut [(K, V)]) {
    if a.len() < 2 {
        return;
    }
    let depth_limit = lg(a.len()) * 2;
    introsort_loop(a, 0, a.len(), depth_limit);
    final_insertion_sort(a, 0, a.len());
}

fn introsort_loop<K: Ord + Clone, V: Clone>(
    a: &mut [(K, V)],
    first: usize,
    mut last: usize,
    mut depth: usize,
) {
    while last - first > SORT_THRESHOLD {
        if depth == 0 {
            // __partial_sort(first, last, last) with middle == last is a plain
            // heapsort.
            heapsort(a, first, last - first);
            return;
        }
        depth -= 1;
        let cut = unguarded_partition_pivot(a, first, last);
        introsort_loop(a, cut, last, depth);
        last = cut;
    }
}

fn unguarded_partition_pivot<K: Ord + Clone, V: Clone>(
    a: &mut [(K, V)],
    first: usize,
    last: usize,
) -> usize {
    let mid = first + (last - first) / 2;
    move_median_to_first(a, first, first + 1, mid, last - 1);
    unguarded_partition(a, first + 1, last, first)
}

fn move_median_to_first<K: Ord + Clone, V: Clone>(
    a: &mut [(K, V)],
    result: usize,
    x: usize,
    y: usize,
    z: usize,
) {
    if a[x].0 < a[y].0 {
        if a[y].0 < a[z].0 {
            a.swap(result, y);
        } else if a[x].0 < a[z].0 {
            a.swap(result, z);
        } else {
            a.swap(result, x);
        }
    } else if a[x].0 < a[z].0 {
        a.swap(result, x);
    } else if a[y].0 < a[z].0 {
        a.swap(result, z);
    } else {
        a.swap(result, y);
    }
}

fn unguarded_partition<K: Ord + Clone, V: Clone>(
    a: &mut [(K, V)],
    mut first: usize,
    mut last: usize,
    pivot: usize,
) -> usize {
    loop {
        while a[first].0 < a[pivot].0 {
            first += 1;
        }
        last -= 1;
        while a[pivot].0 < a[last].0 {
            last -= 1;
        }
        if first >= last {
            return first;
        }
        a.swap(first, last);
        first += 1;
    }
}

fn final_insertion_sort<K: Ord + Clone, V: Clone>(a: &mut [(K, V)], first: usize, last: usize) {
    if last - first > SORT_THRESHOLD {
        insertion_sort(a, first, first + SORT_THRESHOLD);
        for i in first + SORT_THRESHOLD..last {
            unguarded_linear_insert(a, i);
        }
    } else {
        insertion_sort(a, first, last);
    }
}

fn insertion_sort<K: Ord + Clone, V: Clone>(a: &mut [(K, V)], first: usize, last: usize) {
    if first == last {
        return;
    }
    for i in first + 1..last {
        if a[i].0 < a[first].0 {
            let value = a[i].clone();
            let mut j = i;
            while j > first {
                a[j] = a[j - 1].clone();
                j -= 1;
            }
            a[first] = value;
        } else {
            unguarded_linear_insert(a, i);
        }
    }
}

fn unguarded_linear_insert<K: Ord + Clone, V: Clone>(a: &mut [(K, V)], last: usize) {
    let value = a[last].clone();
    let mut next = last - 1;
    let mut last = last;
    while value.0 < a[next].0 {
        a[last] = a[next].clone();
        last = next;
        if next == 0 {
            // std::sort never walks past the sentinel it left at first; in the
            // tail sorts the first element is already the minimum.
            a[last] = value;
            return;
        }
        next -= 1;
    }
    a[last] = value;
}

fn heapsort<K: Ord + Clone, V: Clone>(a: &mut [(K, V)], first: usize, len: usize) {
    if len < 2 {
        return;
    }
    let mut parent = (len - 2) / 2;
    loop {
        let value = a[first + parent].clone();
        adjust_heap(a, first, parent, len, value);
        if parent == 0 {
            break;
        }
        parent -= 1;
    }
    let mut end = len;
    while end > 1 {
        end -= 1;
        let value = a[first + end].clone();
        a[first + end] = a[first].clone();
        adjust_heap(a, first, 0, end, value);
    }
}

fn adjust_heap<K: Ord + Clone, V: Clone>(
    a: &mut [(K, V)],
    first: usize,
    hole: usize,
    len: usize,
    value: (K, V),
) {
    let top = hole;
    let mut hole = hole;
    let mut second_child = hole;
    while second_child < (len - 1) / 2 {
        second_child = 2 * (second_child + 1);
        if a[first + second_child].0 < a[first + second_child - 1].0 {
            second_child -= 1;
        }
        a[first + hole] = a[first + second_child].clone();
        hole = second_child;
    }
    if len % 2 == 0 && second_child == (len - 2) / 2 {
        second_child = 2 * (second_child + 1);
        a[first + hole] = a[first + second_child - 1].clone();
        hole = second_child - 1;
    }
    push_heap(a, first, hole, top, value);
}

fn push_heap<K: Ord + Clone, V: Clone>(
    a: &mut [(K, V)],
    first: usize,
    hole: usize,
    top: usize,
    value: (K, V),
) {
    let mut hole = hole;
    while hole > top {
        let parent = (hole - 1) / 2;
        if a[first + parent].0 < value.0 {
            a[first + hole] = a[first + parent].clone();
            hole = parent;
        } else {
            break;
        }
    }
    a[first + hole] = value;
}

// ---------------------------------------------------------------------------
// The entry point the analyzer calls
// ---------------------------------------------------------------------------

/// Builds a Darts-clone unit array from (key, value) pairs and returns the
/// bytes DartsTrie::load reads (little-endian u32 units).
///
/// The keys are sorted exactly the way the reference sorts them (std::sort on
/// the key alone), because the DAWG deduplicates nodes in insertion order and
/// that order decides the layout.
pub(crate) fn build_dictionary(mut entries: Vec<(Vec<u8>, i32)>) -> Vec<u8> {
    sort_by_key(&mut entries);

    let mut dawg = DawgBuilder::new();
    for (key, value) in &entries {
        dawg.insert(key, *value);
    }
    dawg.finish();

    let mut builder = DoubleArrayBuilder::new();
    builder.build(&dawg);
    builder.into_bytes()
}

#[cfg(test)]
mod tests {
    use super::*;

    /// The three words the C++ probe is run on, so the unit array can be
    /// compared against Darts::DoubleArray byte for byte.
    #[test]
    fn builds_the_reference_mini_dictionary() {
        // huqie.txt: "abc 3 n", "abd 5 v", "x 7 n"; pos-id.def: n, v.
        let mut entries: Vec<(Vec<u8>, i32)> = Vec::new();
        // C++ Encode: sign in bit 23, POS index in bits 24-31.
        let encode = |freq: i32, idx: i32| -> i32 {
            let mut encoded: u32 = if freq < 0 {
                ((-freq) as u32) | (1 << 23)
            } else {
                (freq as u32) & 0x7F_FFFF
            };
            encoded |= (idx as u32) << 24;
            encoded as i32
        };
        for (word, raw, pos) in [("abc", 3u32, 0i32), ("abd", 5, 1), ("x", 7, 0)] {
            let freq = (f64::from((raw as f32 / 1_000_000f32).ln()) + 0.5) as i32;
            entries.push((word.as_bytes().to_vec(), encode(freq, pos)));
            let reversed: String = word.chars().rev().collect();
            entries.push((format!("DD{reversed}").into_bytes(), encode(1, 0)));
        }
        let bytes = build_dictionary(entries);
        let units: Vec<u32> = bytes
            .as_chunks::<4>()
            .0
            .iter()
            .map(|c| u32::from_le_bytes(*c))
            .collect();
        assert_eq!(units.len(), 256);
        assert_eq!(units[0], 0x0001_1400);
        assert_eq!(units[1], 0x0001_1c44);
        assert_eq!(units[6], 0x8000_0001);
    }
}
