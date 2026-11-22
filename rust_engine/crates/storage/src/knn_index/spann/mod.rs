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

//! The spann chunk format (`docs/SPANN_DESIGN.md` §6).
//!
//! One chunk = one immutable tree. The header is self-describing on purpose:
//! the HNSW chunk (`knn_format.rs`) has no magic, no version and no checksum and
//! leans on the catalog JSON for the dimension, which is why a stale or truncated
//! file can only be detected by decoding it. Here every section carries a
//! CRC32C, so a corrupt page fails at open with a named section instead of
//! returning wrong rows.
//!
//! Layout (all integers little-endian):
//!
//! ```text
//! 0    160      header
//! 40   ..       section table: 6 × { offset u64, len u64 }
//! 160  ..       Section A  partition metadata, 32 B per partition
//!      ..       Section B  centroids, dims × f32 per partition
//!      ..       Section C  child keys, u32 per child
//!      ..       Section D  slot pages, one 4 KB-aligned column group per partition
//!      ..       Section E  full vectors (reserved for `encode = none`)
//!      ..       Section F  row offsets, u32 per covered row
//! end  -32      trailer: 8 B magic + 6 × u32 CRC32C (one per section)
//! ```
//!
//! Column groups inside a slot page (§6.2's columnar layout):
//!
//! ```text
//! row_off  : u32 × n     the segment's own offset
//! norm     : f32 × n     ‖o − c‖
//! raw_norm : f32 × n     ‖o‖²   (ip / cosine)
//! sum      : f32 × n     popcount of the code
//! error    : f32 × n     ⟨u, û⟩  (the error bound in §8.3)
//! codes    : u8 × n × ceil(dims/8)
//! ```
//!
//! `row_off` is `u32` because **the chunk is one segment's tree** (§3 I6): the
//! segment lives in the header and the engine-wide id is reconstructed as
//! `segment << 32 | row_off`. Storing the engine-wide id here would cost four
//! bytes per row and go stale the moment a compaction renumbered the segment.

use common::RowMask;
use index::spann::{
    partition::{metric_score, Member, Partition},
    quantize::{self, Code, Rotation},
    search::{self, SearchSet},
    Encode, Options, PartitionKey, SpannError, SpannIndex, ROOT_KEY,
};
use index::vector::{l2_sq, Metric};

mod chain;
mod writer;
pub use chain::{ChainMeta, ChunkChain, Coverage};
pub use writer::{write, write_dirty, write_dirty_to, write_keys, write_range, write_to};

/// The chunk's signature: the name, a NUL pad (the field is eight bytes wide so
/// the offsets that follow are the ones `docs/SPANN_FORMAT.md` tabulates) and the
/// format version. A chunk whose signature is anything else is rejected rather
/// than guessed at - the pre-rename `CSPANN\0\1` files included.
pub const MAGIC: [u8; 8] = *b"SPANN\x00\x00\x01";
const TRAILER_MAGIC: [u8; 8] = *b"SPEND\x00\x00\x01";
/// Bytes of a chunk header: the fixed fields, the segment id, and the section
/// table at 40 so the id sits on the 8-byte alignment `SPANN_FORMAT.md`
/// tabulates.
pub const HEADER_LEN: usize = 160;
/// The only version this build writes and reads.
///
/// A chunk from another version is refused rather than misread: a spann chunk is
/// derived data (`index_delta_mode = rebuild`), so the answer is to rebuild it,
/// not to guess at its layout.
pub const VERSION: u16 = 1;
/// Where the section table starts inside the header: `magic(8) ver(2)
/// header_len(2) dims(4) metric(1) encode(1) rot_algo(1) pad(1) seed(8)
/// partitions(4) segment(4) pad(4)`, then 6 × `{offset u64, len u64}`.
pub const SECTION_TABLE_AT: usize = 40;
/// Bytes per row id: the slot's leading column and one entry of section F.
/// A row id is a segment-local offset, so the segment's 32 bits live once, in
/// the header (§6.2).
pub const ROW_ID_BYTES: usize = 4;
/// Bytes per partition in section A.
pub const META_ENTRY: usize = 32;
pub const SECTION_COUNT: usize = 6;
const TRAILER_LEN: usize = 8 + 4 * SECTION_COUNT;
/// Slot pages are aligned to a page so a future `mmap` reader can map one
/// partition at a time (§6.4).
pub const SLOT_ALIGN: usize = 4096;

/// Section index, in the order they are written.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Section {
    Meta = 0,
    Centroids = 1,
    Children = 2,
    Slots = 3,
    Vectors = 4,
    RowIds = 5,
}

const SECTION_NAMES: [&str; SECTION_COUNT] = [
    "meta",
    "centroids",
    "children",
    "slots",
    "vectors",
    "row_ids",
];

/// The parsed header (§6.1).
#[derive(Clone, Debug, PartialEq)]
pub struct ChunkHeader {
    pub dims: usize,
    pub metric: Metric,
    pub encode: Encode,
    pub rot_algo: u8,
    pub seed: u64,
    pub partitions: usize,
    /// The segment this tree was built over. Slots and section F hold the
    /// segment's **own** offsets; `segment << 32 | offset` is what the engine
    /// sees (`docs/SPANN_DESIGN.md` §6.2).
    pub segment: u32,
    /// The DFS key of this chunk's first partition: 0 for a chunk that holds the
    /// whole tree, `lo` for a subtree chunk covering `[lo, lo + partitions)`.
    ///
    /// Keys inside a chunk stay **local** (0..partitions), so the reader is
    /// unchanged; the base is what lets a segment map a global key onto
    /// `(chunk, local)` once a manifest hands out ranges.
    pub first_key: u32,
}

impl ChunkHeader {
    /// The high half of every row id this chunk names.
    pub fn row_base(&self) -> u64 {
        u64::from(self.segment) << 32
    }
}

/// One partition of a chunk's directory, as [`FileIndex::tree_meta`] reports it.
#[derive(Clone, Debug, PartialEq)]
pub struct TreeMeta {
    /// DFS key, local to the chunk.
    pub key: PartitionKey,
    pub level: u32,
    pub members: usize,
    pub radius: f32,
    pub children: Vec<u32>,
    pub centroid: Vec<f32>,
}

/// Per-partition metadata (§6.1 section A).
#[derive(Clone, Copy, Debug, PartialEq)]
pub struct PartitionMeta {
    pub key: PartitionKey,
    pub level: u32,
    pub count: u32,
    pub radius: f32,
    pub centroid_off: u32,
    pub slots_off: u32,
    pub children_off: u32,
    pub children_len: u32,
}

/// A borrowed view of one partition's slot page.
#[derive(Clone, Copy, Debug)]
pub struct SlotView<'a> {
    row_ids: &'a [u8],
    norm: &'a [u8],
    raw_norm: &'a [u8],
    sum: &'a [u8],
    error: &'a [u8],
    codes: &'a [u8],
    count: usize,
    code_bytes: usize,
    /// `segment << 32`: the high half the page does not store (§6.2).
    row_base: u64,
}

impl SlotView<'_> {
    pub fn len(&self) -> usize {
        self.count
    }

    pub fn is_empty(&self) -> bool {
        self.count == 0
    }

    pub fn row_id(&self, i: usize) -> Option<u64> {
        if i >= self.count {
            return None;
        }
        let at = i * 4;
        let off = u32::from_le_bytes(self.row_ids.get(at..at + 4)?.try_into().ok()?);
        Some(self.row_base | u64::from(off))
    }

    fn f32_at(column: &[u8], i: usize) -> f32 {
        let at = i * 4;
        column
            .get(at..at + 4)
            .and_then(|b| <[u8; 4]>::try_from(b).ok())
            .map(f32::from_le_bytes)
            .unwrap_or(0.0)
    }

    /// The raw columns of member `i`: `(norm, raw_norm, sum, error, bits)`.
    ///
    /// The mapped query path scores from these directly — building a `Code` per
    /// member would allocate once per row per query (`quantize::*_parts`).
    pub fn parts(&self, i: usize) -> Option<(f32, f32, f32, f32, &'_ [u8])> {
        if i >= self.count {
            return None;
        }
        let at = i * self.code_bytes;
        Some((
            Self::f32_at(self.norm, i),
            Self::f32_at(self.raw_norm, i),
            Self::f32_at(self.sum, i),
            Self::f32_at(self.error, i),
            self.codes.get(at..at + self.code_bytes)?,
        ))
    }

    /// Decodes member `i` into the algorithm layer's `Code`.
    pub fn code(&self, i: usize) -> Option<Code> {
        if i >= self.count {
            return None;
        }
        let at = i * self.code_bytes;
        Some(Code {
            norm: Self::f32_at(self.norm, i),
            raw_norm: Self::f32_at(self.raw_norm, i),
            sum: Self::f32_at(self.sum, i),
            error: Self::f32_at(self.error, i),
            bits: self.codes.get(at..at + self.code_bytes)?.to_vec(),
        })
    }
}

/// A parsed, borrowed chunk.
#[derive(Debug)]
pub struct Chunk<'a> {
    bytes: &'a [u8],
    header: ChunkHeader,
    sections: [(usize, usize); SECTION_COUNT],
}

/// The verified header and section table of a chunk.
///
/// The file-backed reader (§6.4) keeps one of these next to its mapping: a query
/// must not re-parse (or re-checksum) the file, so `Layout::open` validates
/// everything **once** and `Chunk::from_layout` hands out a zero-copy view.
#[derive(Clone, Debug)]
pub struct Layout {
    pub header: ChunkHeader,
    pub sections: [(usize, usize); SECTION_COUNT],
}

impl Layout {
    /// Reads magic, version, header fields, the section table, and verifies one
    /// CRC32C per section.
    pub fn open(bytes: &[u8]) -> Result<Self, SpannError> {
        if bytes.len() < HEADER_LEN + TRAILER_LEN {
            return Err(SpannError::Corrupt("file shorter than header".to_string()));
        }
        if bytes[0..8] != MAGIC {
            return Err(SpannError::Corrupt("bad magic".to_string()));
        }
        let version = u16::from_le_bytes([bytes[8], bytes[9]]);
        if version != VERSION {
            // Not corruption: the bytes are a chunk of another format version,
            // so the segment has to be rebuilt from the column. The caller can
            // tell the two apart and schedule that rebuild instead of treating
            // a readable index as damaged.
            return Err(SpannError::NeedsRebuild {
                found: version,
                expected: VERSION,
            });
        }
        let header_len = u16::from_le_bytes([bytes[10], bytes[11]]) as usize;
        if header_len != HEADER_LEN {
            return Err(SpannError::Corrupt(format!(
                "header length {header_len}, expected {HEADER_LEN}"
            )));
        }
        let dims = u32::from_le_bytes(bytes[12..16].try_into().unwrap()) as usize;
        if dims == 0 {
            return Err(SpannError::Corrupt("zero dimension".to_string()));
        }
        let metric = match bytes[16] {
            0 => Metric::L2,
            1 => Metric::Ip,
            2 => Metric::Cos,
            other => return Err(SpannError::Corrupt(format!("bad metric {other}"))),
        };
        let encode = match bytes[17] {
            0 => Encode::None,
            1 => Encode::Rabitq,
            other => return Err(SpannError::Corrupt(format!("bad encode {other}"))),
        };
        let rot_algo = bytes[18];
        let seed = u64::from_le_bytes(bytes[20..28].try_into().unwrap());
        let partitions = u32::from_le_bytes(bytes[28..32].try_into().unwrap()) as usize;
        if partitions == 0 {
            return Err(SpannError::Corrupt("no partitions".to_string()));
        }
        let segment = u32::from_le_bytes(bytes[32..36].try_into().unwrap());
        let first_key = u32::from_le_bytes(bytes[136..140].try_into().unwrap());
        if (first_key as usize).checked_add(partitions).is_none()
            || first_key as usize + partitions > u32::MAX as usize
        {
            return Err(SpannError::Corrupt(format!(
                "subtree base {first_key} + {partitions} partitions overflows the key space"
            )));
        }

        // Section table, then bounds and CRCs.
        let tail = bytes.len() - TRAILER_LEN;
        if bytes[tail..tail + 8] != TRAILER_MAGIC {
            return Err(SpannError::Corrupt("missing trailer".to_string()));
        }
        let mut sections = [(0usize, 0usize); SECTION_COUNT];
        for section in 0..SECTION_COUNT {
            let at = SECTION_TABLE_AT + section * 16;
            let offset = u64::from_le_bytes(bytes[at..at + 8].try_into().unwrap()) as usize;
            let len = u64::from_le_bytes(bytes[at + 8..at + 16].try_into().unwrap()) as usize;
            let end = offset
                .checked_add(len)
                .ok_or_else(|| SpannError::Corrupt("section overflow".to_string()))?;
            if offset < HEADER_LEN || end > tail {
                return Err(SpannError::Corrupt(format!(
                    "section {} out of bounds ({offset}..{end}, file {tail})",
                    SECTION_NAMES[section]
                )));
            }
            let want = u32::from_le_bytes(
                bytes[tail + 8 + section * 4..tail + 12 + section * 4]
                    .try_into()
                    .unwrap(),
            );
            let got = crc32c(&bytes[offset..end]);
            if want != got {
                return Err(SpannError::Corrupt(format!(
                    "section {} checksum {got:#x} != {want:#x}",
                    SECTION_NAMES[section]
                )));
            }
            sections[section] = (offset, len);
        }

        let meta_len = sections[Section::Meta as usize].1;
        if meta_len != partitions * META_ENTRY {
            return Err(SpannError::Corrupt(format!(
                "meta section {meta_len} B for {partitions} partitions"
            )));
        }
        Ok(Layout {
            header: ChunkHeader {
                dims,
                metric,
                encode,
                rot_algo,
                seed,
                partitions,
                segment,
                first_key,
            },
            sections,
        })
    }
}

impl<'a> Chunk<'a> {
    /// Parses and verifies `bytes` (the open-time path).
    pub fn parse(bytes: &'a [u8]) -> Result<Self, SpannError> {
        Ok(Self::from_layout(bytes, Layout::open(bytes)?))
    }

    /// A zero-copy view over an already-verified layout — what a mapped chunk
    /// hands a query.
    pub fn from_layout(bytes: &'a [u8], layout: Layout) -> Self {
        Self {
            bytes,
            header: layout.header,
            sections: layout.sections,
        }
    }

    pub fn header(&self) -> &ChunkHeader {
        &self.header
    }

    pub fn code_bytes(&self) -> usize {
        match self.header.encode {
            Encode::None => self.header.dims * 4,
            Encode::Rabitq => index::spann::quantize::code_bytes(self.header.dims),
        }
    }

    fn section(&self, section: Section) -> &'a [u8] {
        let (offset, len) = self.sections[section as usize];
        &self.bytes[offset..offset + len]
    }

    /// The section-A slot a key lives in.
    ///
    /// Keys are **looked up**, not assumed to be slots: today's chunks write
    /// DFS positions, so the fast path hits, but a chunk whose keys are stable
    /// identifiers (`parent << 6 | slot`) sorts section A by key and resolves
    /// with a binary search. That lookup is what lets a split insert a child
    /// without renumbering every partition after it - the obstacle that makes a
    /// positional key model rewrite the whole tree for one dirty partition.
    fn slot_of(&self, key: PartitionKey) -> Option<usize> {
        let section = self.section(Section::Meta);
        let count = self.header.partitions;
        let read = |slot: usize| -> PartitionKey {
            let at = slot * META_ENTRY;
            u32::from_le_bytes(section[at..at + 4].try_into().unwrap())
        };
        if (key as usize) < count && read(key as usize) == key {
            return Some(key as usize);
        }
        let mut lo = 0usize;
        let mut hi = count;
        while lo < hi {
            let mid = (lo + hi) / 2;
            match read(mid).cmp(&key) {
                std::cmp::Ordering::Less => lo = mid + 1,
                std::cmp::Ordering::Greater => hi = mid,
                std::cmp::Ordering::Equal => return Some(mid),
            }
        }
        None
    }

    /// The key stored in section A at `slot`: the inverse of [`Self::slot_of`],
    /// for walking the directory positionally.
    pub fn key_at(&self, slot: usize) -> Option<PartitionKey> {
        if slot >= self.header.partitions {
            return None;
        }
        let at = slot * META_ENTRY;
        let section = self.section(Section::Meta);
        Some(u32::from_le_bytes(section[at..at + 4].try_into().unwrap()))
    }

    pub fn meta(&self, key: PartitionKey) -> Option<PartitionMeta> {
        let at = self.slot_of(key)? * META_ENTRY;
        let section = self.section(Section::Meta);
        let word = |at: usize| u32::from_le_bytes(section[at..at + 4].try_into().unwrap());
        Some(PartitionMeta {
            key: word(at),
            level: u32::from(u16::from_le_bytes([section[at + 4], section[at + 5]])),
            count: u32::from(u16::from_le_bytes([section[at + 6], section[at + 7]])),
            radius: f32::from_le_bytes(section[at + 8..at + 12].try_into().unwrap()),
            centroid_off: word(at + 12),
            slots_off: word(at + 16),
            children_off: word(at + 20),
            children_len: u32::from(u16::from_le_bytes([section[at + 24], section[at + 25]])),
        })
    }

    /// Decoded on demand: the crate forbids `unsafe`, so the columns are copied
    /// instead of reinterpreted. The load path calls this once per partition;
    /// the mmap reader (§6.4) will need a `safe` zero-copy variant.
    pub fn centroid(&self, key: PartitionKey) -> Option<Vec<f32>> {
        let meta = self.meta(key)?;
        let at = meta.centroid_off as usize;
        let len = self.header.dims * 4;
        let raw = self.section(Section::Centroids).get(at..at + len)?;
        Some(
            raw.as_chunks::<4>()
                .0
                .iter()
                .map(|b| f32::from_le_bytes(*b))
                .collect(),
        )
    }

    pub fn children(&self, key: PartitionKey) -> Vec<u32> {
        let Some(meta) = self.meta(key) else {
            return Vec::new();
        };
        let at = meta.children_off as usize * 4;
        let len = meta.children_len as usize * 4;
        let Some(raw) = self.section(Section::Children).get(at..at + len) else {
            return Vec::new();
        };
        raw.as_chunks::<4>()
            .0
            .iter()
            .map(|b| u32::from_le_bytes(*b))
            .collect()
    }

    pub fn slots(&self, key: PartitionKey) -> Option<SlotView<'a>> {
        let meta = self.meta(key)?;
        let count = meta.count as usize;
        let code_bytes = self.code_bytes();
        // One slot: row_off u32 + norm/raw_norm/sum/error f32 + code.
        let block_len = count * (20 + code_bytes);
        let start = meta.slots_off as usize;
        let slots = self.section(Section::Slots).get(start..start + block_len)?;
        let mut at = 0usize;
        let mut take = |bytes: usize| {
            let out = &slots[at..at + bytes];
            at += bytes;
            out
        };
        let row_ids = take(count * 4);
        let norm = take(count * 4);
        let raw_norm = take(count * 4);
        let sum = take(count * 4);
        let error = take(count * 4);
        let codes = take(count * code_bytes);
        Some(SlotView {
            row_ids,
            norm,
            raw_norm,
            sum,
            error,
            codes,
            count,
            code_bytes,
            // A branch's word is a child key, not a row (§6.2), so only a leaf
            // gets the segment prefix.
            row_base: if meta.level <= 1 {
                self.header.row_base()
            } else {
                0
            },
        })
    }

    pub fn row_ids(&self) -> Vec<u64> {
        let base = self.header.row_base();
        self.section(Section::RowIds)
            .as_chunks::<4>()
            .0
            .iter()
            .map(|b| base | u64::from(u32::from_le_bytes(*b)))
            .collect()
    }

    pub fn height(&self) -> u32 {
        (0..self.header.partitions)
            .filter_map(|key| self.meta(key as PartitionKey))
            .map(|meta| meta.level)
            .max()
            .unwrap_or(1)
    }

    /// Rebuilds the searchable index. The caller passes the options it read from
    /// the catalog; `metric`, `encode` and `seed` must match the header, because
    /// they change what the codes mean.
    pub fn to_index(&self, opts: Options) -> Result<SpannIndex, SpannError> {
        if opts.metric != self.header.metric {
            return Err(SpannError::Corrupt(format!(
                "metric mismatch: catalog {:?}, chunk {:?}",
                opts.metric, self.header.metric
            )));
        }
        if opts.encode != self.header.encode {
            return Err(SpannError::Corrupt(format!(
                "encode mismatch: catalog {:?}, chunk {:?}",
                opts.encode, self.header.encode
            )));
        }
        if opts.seed != self.header.seed {
            return Err(SpannError::Corrupt(format!(
                "seed mismatch: catalog {}, chunk {}",
                opts.seed, self.header.seed
            )));
        }
        let rotation = Rotation::new(self.header.dims, self.header.seed);
        let mut partitions = Vec::with_capacity(self.header.partitions);
        for key in 0..self.header.partitions as PartitionKey {
            let meta = self
                .meta(key)
                .ok_or_else(|| SpannError::Corrupt(format!("missing meta {key}")))?;
            let centroid = self
                .centroid(key)
                .ok_or_else(|| SpannError::Corrupt(format!("missing centroid {key}")))?;
            let rot_centroid = rotation.apply(&centroid);
            let view = self
                .slots(key)
                .ok_or_else(|| SpannError::Corrupt(format!("missing slots {key}")))?;
            let mut members = Vec::with_capacity(view.len());
            for i in 0..view.len() {
                let id = view
                    .row_id(i)
                    .ok_or_else(|| SpannError::Corrupt(format!("slot {i} of {key}")))?;
                let code = view
                    .code(i)
                    .ok_or_else(|| SpannError::Corrupt(format!("code {i} of {key}")))?;
                members.push(Member { id, code });
            }
            let children: Vec<PartitionKey> = self.children(key);
            partitions.push(Partition::from_decoded(
                key,
                meta.level,
                self.header.dims,
                centroid,
                meta.radius,
                rot_centroid,
                members,
                children,
            ));
        }
        let height = self.height();
        Ok(SpannIndex::from_parts(
            opts,
            self.header.dims,
            rotation,
            partitions,
            height,
            ROOT_KEY,
        ))
    }
}

/// CRC-32C (Castagnoli), the polynomial §6.1 names. Bitwise on purpose: the
/// chunk hashes run once per file, not per query.
pub fn crc32c(data: &[u8]) -> u32 {
    let mut crc = Crc32c::new();
    crc.update(data);
    crc.finish()
}

/// Incremental CRC-32C, so a section can be hashed while it is being written
/// instead of being buffered first (`writer::write_to`).
#[derive(Clone, Copy, Debug)]
pub struct Crc32c(u32);

impl Crc32c {
    pub fn new() -> Self {
        Self(!0)
    }

    pub fn update(&mut self, data: &[u8]) {
        let mut crc = self.0;
        for byte in data {
            crc ^= u32::from(*byte);
            for _ in 0..8 {
                crc = if crc & 1 != 0 {
                    (crc >> 1) ^ 0x82F6_3B78
                } else {
                    crc >> 1
                };
            }
        }
        self.0 = crc;
    }

    pub fn finish(self) -> u32 {
        !self.0
    }
}

impl Default for Crc32c {
    fn default() -> Self {
        Self::new()
    }
}

/// Reads only the header of the chunk at `path`.
///
/// [`FileIndex::open`] verifies every section's CRC, i.e. reads the whole file.
/// A publisher that only wants to know whether the chunk it already has was
/// built with the catalog's options must not pay that: this reads `HEADER_LEN`
/// bytes.
pub fn read_header(path: impl AsRef<std::path::Path>) -> Result<ChunkHeader, SpannError> {
    // Mapped through the chunk store, so a cache that has been wiped still
    // answers from the object; only the header page is touched, not the
    // sections, which is the whole point of not calling `Layout::open`.
    let map = crate::buffer::mapped::MappedFile::open(path)
        .map_err(|e| SpannError::Corrupt(format!("open chunk: {e}")))?;
    let bytes: &[u8] = &map;
    let head = bytes
        .get(..HEADER_LEN)
        .ok_or_else(|| SpannError::Corrupt("chunk shorter than its header".to_string()))?;
    if head[0..8] != MAGIC {
        return Err(SpannError::Corrupt("bad magic".to_string()));
    }
    let version = u16::from_le_bytes([head[8], head[9]]);
    if version != VERSION {
        return Err(SpannError::NeedsRebuild {
            found: version,
            expected: VERSION,
        });
    }
    let dims = u32::from_le_bytes(head[12..16].try_into().unwrap()) as usize;
    let metric = match head[16] {
        0 => Metric::L2,
        1 => Metric::Ip,
        2 => Metric::Cos,
        other => return Err(SpannError::Corrupt(format!("bad metric {other}"))),
    };
    let encode = match head[17] {
        0 => Encode::None,
        1 => Encode::Rabitq,
        other => return Err(SpannError::Corrupt(format!("bad encode {other}"))),
    };
    Ok(ChunkHeader {
        dims,
        metric,
        encode,
        rot_algo: head[18],
        seed: u64::from_le_bytes(head[20..28].try_into().unwrap()),
        partitions: u32::from_le_bytes(head[28..32].try_into().unwrap()) as usize,
        segment: u32::from_le_bytes(head[32..36].try_into().unwrap()),
        first_key: u32::from_le_bytes(head[136..140].try_into().unwrap()),
    })
}

/// A chunk served straight out of its file (§6.4's out-of-core path).
///
/// The mapping is read-only and a chunk file is immutable once published — a
/// dump writes a **new** `chunk_{n}.idx` rather than rewriting one in place — so
/// a query can address the slot pages directly instead of decoding the tree into
/// heap `Partition`s. What stays in memory is the header, the section table, the
/// rotation (regenerated from the seed) and one `QueryCode` per probed
/// partition; the slot bytes are page cache, which the OS can reclaim.
#[derive(Debug)]
pub struct FileIndex {
    map: crate::mapped::MappedFile,
    layout: Layout,
    rotation: Rotation,
    beam: usize,
}

impl FileIndex {
    pub fn open(path: impl AsRef<std::path::Path>, opts: &Options) -> Result<Self, SpannError> {
        // `MappedFile::open` is the store-aware facade: with a persistence
        // profile and no local copy it maps the object that holds the chunk
        // (`MappedFile::open_range`), instead of failing on a missing cache.
        let map = crate::mapped::MappedFile::open(path)
            .map_err(|e| SpannError::Corrupt(format!("map: {e}")))?;
        let layout = Layout::open(&map)?;
        if layout.header.metric != opts.metric
            || layout.header.encode != opts.encode
            || layout.header.seed != opts.seed
        {
            return Err(SpannError::Corrupt(format!(
                "catalog ({:?}/{:?}/{}) and chunk ({:?}/{:?}/{}) disagree",
                opts.metric,
                opts.encode,
                opts.seed,
                layout.header.metric,
                layout.header.encode,
                layout.header.seed
            )));
        }
        let rotation = Rotation::new(layout.header.dims, layout.header.seed);
        Ok(Self {
            map,
            layout,
            rotation,
            beam: opts.base_beam,
        })
    }

    /// Drops the resident pages of this chunk's mapping, returning how many
    /// bytes were handed back to the kernel.
    ///
    /// The chunk is immutable once published, so a dropped page is re-read from
    /// the file (or the object) on the next access; nothing is lost but time.
    /// This is what lets a long-lived engine keep every segment's chain addressable
    /// while its RSS tracks the working set instead of the index size.
    pub fn release_pages(&self) -> usize {
        self.map.release_pages()
    }

    /// The bytes this reader holds in the heap: the header, the section table,
    /// and the regenerated rotation. The mapped pages are **not** counted - they
    /// are page cache (`buffer::mapped::mapped_bytes` reports their extent).
    pub fn heap_bytes(&self) -> usize {
        // The rotated copy of the chunk is `dims` signs plus the FWHT block
        // lengths (`index::spann::quantize::Rotation`), i.e. `dims x 4` order -
        // not a `dims x dims` matrix.
        std::mem::size_of::<Self>() + self.layout.header.dims * 8
    }

    /// A view over the mapping; the layout was verified at open, so this is free.
    fn chunk(&self) -> Chunk<'_> {
        Chunk::from_layout(&self.map, self.layout.clone())
    }

    /// The chunk's partition directory, in DFS order: what an incremental dump
    /// needs in order to insert a row into this tree without re-reading the
    /// slots of every partition (`SPANN_DESIGN.md` §0.0.3's read-modify-write
    /// path).
    ///
    /// `members` is the count, not the rows: a caller that wants the rows asks
    /// for one partition at a time (`slots`), which is the unit the dump rewrites.
    pub fn tree_meta(&self) -> Vec<TreeMeta> {
        let chunk = self.chunk();
        (0..self.partitions())
            .filter_map(|slot| {
                // Slots, not keys: a re-keyed chunk's keys are identifiers, so the
                // directory has to be walked positionally and the key read out of
                // section A.
                let key = chunk.key_at(slot)?;
                let meta = chunk.meta(key)?;
                Some(TreeMeta {
                    key,
                    level: meta.level,
                    members: meta.count as usize,
                    radius: meta.radius,
                    children: chunk.children(key),
                    centroid: chunk.centroid(key)?,
                })
            })
            .collect()
    }

    /// The DFS key this chunk's first partition sits at: 0 for a whole-tree
    /// chunk, `lo` for a subtree chunk (`writer::write_range`).
    pub fn first_key(&self) -> u32 {
        self.layout.header.first_key
    }

    /// Whether this chunk's directory names `key`.
    ///
    /// What a chain asks before falling back to an older layer: a delta chunk
    /// holds only the partitions it rewrote, so "not here" is the normal case.
    pub fn contains_key(&self, key: PartitionKey) -> bool {
        self.chunk().meta(key).is_some()
    }

    /// The directory entry for `key`, if this chunk holds it.
    pub fn meta(&self, key: PartitionKey) -> Option<PartitionMeta> {
        self.chunk().meta(key)
    }

    /// The segment this chunk was built over (§6.2).
    pub fn segment(&self) -> u32 {
        self.layout.header.segment
    }

    /// The directory's keys, in slot order - four bytes each, no centroids.
    ///
    /// This is what a chain needs to build its owner table: which layer names
    /// which key. [`Self::tree_meta`] answers the same question but copies a
    /// `dims`-sized centroid per node, which at 1536d and 62k partitions is
    /// 380 MB per layer, per open - the cost the checkpoint kept paying.
    pub fn keys(&self) -> Vec<PartitionKey> {
        (0..self.partitions())
            .filter_map(|slot| self.chunk().key_at(slot))
            .collect()
    }

    /// The centroid of `key`, if this chunk holds it.
    pub fn centroid(&self, key: PartitionKey) -> Option<Vec<f32>> {
        self.chunk().centroid(key)
    }

    /// `key`'s child list, from this chunk's copy of the node.
    pub fn children(&self, key: PartitionKey) -> Vec<u32> {
        self.chunk().children(key)
    }

    /// `key`'s slot page decoded into members - a **published leaf's rows**,
    /// which is what a live tree reads back to split it (`SlotSource`).
    ///
    /// Decoded on demand (the crate forbids `unsafe`, so the columns are copied
    /// rather than reinterpreted): one page, ~27 KB at 128 rows × 212 B.
    pub fn slot_members(&self, key: PartitionKey) -> Option<Vec<index::spann::Member>> {
        let view = self.chunk().slots(key)?;
        let mut out = Vec::with_capacity(view.len());
        for i in 0..view.len() {
            out.push(index::spann::Member {
                id: view.row_id(i)?,
                code: view.code(i)?,
            });
        }
        Some(out)
    }

    /// `key`'s row ids only - the ids column, not the codes.
    ///
    /// §6.2 stores row ids and quantized codes in separate columns, so a coverage
    /// pass over 390k leaves touches the `4 B/row` column and leaves the
    /// `212 B/row` code pages alone.
    pub fn slot_row_ids(&self, key: PartitionKey) -> Option<Vec<u64>> {
        let view = self.chunk().slots(key)?;
        let mut out = Vec::with_capacity(view.len());
        for i in 0..view.len() {
            out.push(view.row_id(i)?);
        }
        Some(out)
    }

    /// Scores one leaf's page into `set`, the single-chunk scan the chain reuses
    /// (`ChunkChain::search` walks the chain but scans through the owner layer).
    /// Reads one leaf's slots into `set`, returning how many the mask admitted.
    ///
    /// The count is what a filtered walk charges against its beam: a leaf whose
    /// rows are all rejected costs nothing, so a selective filter cannot spend
    /// the budget on partitions that cannot contribute.
    pub(crate) fn scan_leaf_into(
        &self,
        key: PartitionKey,
        q: &[f32],
        allowed: Option<&RowMask>,
        set: &mut SearchSet,
    ) -> usize {
        let chunk = self.chunk();
        self.scan_leaf(
            &chunk,
            key,
            q,
            self.layout.header.metric,
            self.dims(),
            allowed,
            set,
        )
    }

    pub fn dims(&self) -> usize {
        self.layout.header.dims
    }

    pub fn len(&self) -> usize {
        self.layout.sections[Section::RowIds as usize].1 / ROW_ID_BYTES
    }

    pub fn is_empty(&self) -> bool {
        self.len() == 0
    }

    pub fn height(&self) -> u32 {
        self.chunk().height()
    }

    /// Every row id the chunk covers, ascending (section F). The coverage check
    /// asks which rows the tree does *not* hold, so a partly covered table
    /// scores its tail instead of the whole table
    /// (`docs/SPANN_DESIGN.md` §9.7).
    ///
    /// Authoritative only for a **standalone** chunk. A delta chunk
    /// (`writer::write_dirty`) writes the partitions it rewrote and nothing
    /// else, so its section F is a slice of pages, not the tree's coverage -
    /// ask [`ChunkChain::row_ids`] instead, which walks the reachable leaves.
    pub fn row_ids(&self) -> Vec<u64> {
        self.chunk().row_ids()
    }

    /// The metric the chunk was built with; a merged result set orders by it.
    pub fn metric(&self) -> Metric {
        self.layout.header.metric
    }

    pub fn partitions(&self) -> usize {
        self.layout.header.partitions
    }

    /// Same descent as `SpannIndex::search`, but every partition is read from
    /// the mapping (`§8.4`: beam per level, radius bound, exact rescoring by the
    /// caller).
    pub fn search(&self, q: &[f32], k: usize, beam: Option<usize>) -> Vec<(u64, f32)> {
        self.search_masked(q, k, beam, None)
    }

    /// [`Self::search`] with the statement's filter bitmap, tested per slot while
    /// the page is being read (`§12.5`): a filtered query must not have to widen
    /// its candidate pool to the size of the table.
    pub fn search_filtered(
        &self,
        q: &[f32],
        k: usize,
        beam: Option<usize>,
        allowed: &RowMask,
    ) -> Vec<(u64, f32)> {
        self.search_masked(q, k, beam, Some(allowed))
    }

    fn search_masked(
        &self,
        q: &[f32],
        k: usize,
        beam: Option<usize>,
        allowed: Option<&RowMask>,
    ) -> Vec<(u64, f32)> {
        let dims = self.dims();
        if k == 0 || q.len() != dims {
            return Vec::new();
        }
        let chunk = self.chunk();
        let metric = self.layout.header.metric;
        // `beam` is the leaf-scan budget, matching the heap path
        // (`search::Candidate`): a tree grown by 2-way splits is ~log2(rows/CAP)
        // levels deep, so a per-level width dilutes by 2^height by the time it
        // reaches the leaves.
        let budget = beam.unwrap_or(self.beam).max(1);

        let mut set = SearchSet::with_default_capacity(k);
        let mut queue: std::collections::BinaryHeap<search::Candidate> =
            std::collections::BinaryHeap::new();
        // The chunk's root is its smallest key: a whole-tree chunk that is
        // `ROOT_KEY`, a subtree chunk the root of the subtree it holds.
        queue.push(search::Candidate {
            bound: 0.0,
            lower: 0.0,
            key: self.first_key() as u64,
        });
        let mut scanned = 0usize;
        while let Some(node) = queue.pop() {
            // Skip, never break: the queue is ordered by centroid distance, so a
            // later node may still have a smaller radius-adjusted bound.
            if metric.is_l2() {
                if let Some(kth) = set.kth_upper() {
                    if !search::l2_can_beat(node.lower, kth) {
                        continue;
                    }
                }
            }
            let key = node.key as PartitionKey;
            let Some(meta) = chunk.meta(key) else {
                continue;
            };
            if meta.level <= 1 {
                // The budget is checked **before** the scan, so `beam = 1`
                // scans exactly one leaf. Only a leaf that admits at least one
                // row is charged: a leaf the mask rejects cannot contribute, and
                // charging it made a selective filter come back short.
                if allowed.is_some() && scanned >= budget {
                    break;
                }
                let admitted = self.scan_leaf(&chunk, key, q, metric, dims, allowed, &mut set);
                if admitted > 0 || allowed.is_none() {
                    scanned += 1;
                }
                continue;
            }
            for child in chunk.children(key) {
                let Some(centroid) = chunk.centroid(child) else {
                    continue;
                };
                let radius = chunk.meta(child).map(|m| m.radius).unwrap_or(0.0);
                let distance = l2_sq(q, &centroid).max(0.0).sqrt();
                queue.push(search::Candidate {
                    bound: distance,
                    lower: search::radius_lower(distance, radius, metric),
                    key: child as u64,
                });
            }
        }
        set.into_sorted()
    }

    /// The admitted rows this chunk holds, scored exactly.
    ///
    /// `SpannHandle::search_filtered` calls this once its widened beam still
    /// came back short: the beam is a work bound, not a correctness one, so a
    /// selective filter must fall back to the whole tree rather than answer
    /// fewer than `k` rows while admitted rows exist.
    pub fn search_admitted_exact(&self, q: &[f32], k: usize, allowed: &RowMask) -> Vec<(u64, f32)> {
        self.search_masked(q, k, Some(usize::MAX), Some(allowed))
    }

    /// Scores every slot of one leaf into `set`.
    #[allow(clippy::too_many_arguments)]
    fn scan_leaf(
        &self,
        chunk: &Chunk<'_>,
        key: PartitionKey,
        q: &[f32],
        metric: Metric,
        dims: usize,
        allowed: Option<&RowMask>,
        set: &mut SearchSet,
    ) -> usize {
        let (Some(slots), Some(centroid)) = (chunk.slots(key), chunk.centroid(key)) else {
            return 0;
        };
        let rot_centroid = self.rotation.apply(&centroid);
        let query = quantize::make_query(&self.rotation, &rot_centroid, q);
        let mut admitted = 0usize;
        for i in 0..slots.len() {
            let (Some(row), Some((norm, raw_norm, sum, error, bits))) =
                (slots.row_id(i), slots.parts(i))
            else {
                continue;
            };
            if let Some(allowed) = allowed {
                if !allowed.contains(row) {
                    continue;
                }
            }
            admitted += 1;
            // Scored from the raw columns: no per-member allocation, which is
            // what made the first mapped version 3.4× slower than heap.
            let squared = quantize::distance_estimate_parts(&query, norm, sum, error, bits, dims);
            let err = quantize::error_bound_parts(&query, norm, error);
            let (score, bound) = metric_score(metric, squared, err, query.raw_norm, raw_norm);
            set.add(score, bound, row);
        }
        admitted
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use common::RowBitmap;
    use index::spann::SpannIndex;

    /// A chunk of another format version is **not** corruption: the bytes are
    /// readable, the layout is the same, and the repair is a rebuild from the
    /// column. The errors are distinct so the checkpoint can defer that rebuild
    /// instead of treating a readable index as damaged.
    #[test]
    fn a_chunk_of_another_version_asks_for_a_rebuild() {
        let index = SpannIndex::build(rows(60, 8), opts()).unwrap();
        let dir = std::env::temp_dir().join(format!("spann_version_{}", std::process::id()));
        std::fs::create_dir_all(&dir).unwrap();
        let path = dir.join("old.idx");
        write_to(&index, &path, 0).unwrap();

        let mut bytes = std::fs::read(&path).unwrap();
        // The version is the u16 at offset 8 (`Layout::open`).
        let bumped = (VERSION + 1).to_le_bytes();
        bytes[8] = bumped[0];
        bytes[9] = bumped[1];
        std::fs::write(&path, &bytes).unwrap();

        match read_header(&path) {
            Err(SpannError::NeedsRebuild { found, expected }) => {
                assert_eq!(found, VERSION + 1, "the header names the version it found");
                assert_eq!(expected, VERSION, "and the one this build reads");
            }
            _ => panic!("a version bump must ask for a rebuild, not report corruption"),
        }
        match Layout::open(&bytes) {
            Err(SpannError::NeedsRebuild { found, .. }) => assert_eq!(found, VERSION + 1),
            _ => panic!("the full layout must report the same thing"),
        }
    }

    /// Giving an idle chunk's pages back does not close it: the mapping stays
    /// valid and the next read faults the same immutable bytes back in from the
    /// file. That is what lets a long-lived engine keep every chain addressable
    /// while its RSS tracks the working set.
    #[test]
    fn releasing_the_pages_keeps_the_chunk_readable() {
        let index = SpannIndex::build(rows(150, 8), opts()).unwrap();
        let dir = std::env::temp_dir().join(format!("spann_release_{}", std::process::id()));
        std::fs::create_dir_all(&dir).unwrap();
        let path = dir.join("release.idx");
        write_to(&index, &path, 0).unwrap();

        let opened = FileIndex::open(&path, &opts()).unwrap();
        let before = opened.len();
        let released = opened.release_pages();
        assert!(released > 0, "a mapped chunk has pages to give back");
        assert_eq!(opened.len(), before);
        // The directory still answers, and every partition keeps its members:
        // the read goes back to the file, not to a copy that was dropped.
        let meta = opened.tree_meta();
        assert_eq!(meta.len(), index.partitions().len());
        for (at, part) in index.partitions().iter().enumerate() {
            assert_eq!(meta[at].members, part.members.len());
            assert_eq!(meta[at].key, part.key);
        }
    }

    fn rows(n: usize, dim: usize) -> Vec<(u64, Vec<f32>)> {
        (0..n)
            .map(|i| {
                let v: Vec<f32> = (0..dim)
                    .map(|d| ((i * 31 + d * 17 + d * d) % 53) as f32 - 26.0)
                    .collect();
                (i as u64, v)
            })
            .collect()
    }

    fn opts() -> Options {
        Options {
            max_partition_size: 32,
            min_partition_size: 8,
            ..Options::default()
        }
    }

    /// A filtered probe may not spend its beam on leaves the mask rejects.
    ///
    /// 400 rows in ~13 leaves, `beam = 2`: with a mask that admits only the
    /// rows of the **last** few leaves, charging every probed leaf to the beam
    /// spent the budget on the first two rejected partitions and answered
    /// nothing. Only a leaf that admits a row may count.
    #[test]
    fn a_filtered_beam_skips_leaves_the_mask_rejects() {
        let data = rows(400, 8);
        let index = SpannIndex::build(data.clone(), opts()).unwrap();
        let bytes = write(&index, 0).unwrap();
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_0.idx");
        std::fs::write(&path, &bytes).unwrap();
        let file = FileIndex::open(&path, &opts()).unwrap();

        // Admit only the rows of the **last** leaves in the tree's own order,
        // so a best-first walk reaches them last: a `beam = 2` walk that charges
        // rejected leaves stops far short of them.
        let mut leaf_order: Vec<u64> = index
            .partitions()
            .iter()
            .filter(|p| p.level <= 1)
            .flat_map(|p| p.members.iter().map(|m| m.id))
            .collect();
        leaf_order.dedup();
        let admitted: RowMask = RowMask::from_rows(
            leaf_order
                .iter()
                .rev()
                .take(6)
                .copied()
                .collect::<RowBitmap>(),
        );
        assert!(!admitted.is_empty(), "the mask admits the tail leaves");

        for query in data.iter().take(8) {
            let filtered: Vec<u64> = index
                .search_filtered(&query.1, 4, Some(2), &admitted)
                .into_iter()
                .map(|(id, _)| id)
                .collect();
            assert!(
                !filtered.is_empty(),
                "a rejected leaf must not consume the beam (row {}): {filtered:?}",
                query.0
            );
            assert!(
                filtered.iter().all(|id| admitted.contains(*id)),
                "row {}: the probe returned a filtered-out row",
                query.0
            );
            // `beam = 2` is approximate, so the ids need not equal the
            // exhaustive answer - but the exhaustive fallback the handle uses
            // must be full.
            let exact: Vec<u64> = index
                .search_admitted_exact(&query.1, 4, &admitted)
                .into_iter()
                .map(|(id, _)| id)
                .collect();
            assert_eq!(exact.len(), 4, "row {}: the fallback is short", query.0);

            // The mapped form walks the same code.
            let mapped: Vec<u64> = file
                .search_filtered(&query.1, 4, Some(2), &admitted)
                .into_iter()
                .map(|(id, _)| id)
                .collect();
            assert_eq!(mapped, filtered, "row {}: chunk file disagrees", query.0);
        }
    }

    #[test]
    fn a_range_chunk_renumbers_its_keys_and_names_its_base() {
        let index = SpannIndex::build(rows(120, 8), opts()).unwrap();
        let n = index.partitions().len();
        let dir = std::env::temp_dir().join(format!("spann_range_{}", std::process::id()));
        std::fs::create_dir_all(&dir).unwrap();
        let header_of = |bytes: &[u8], name: &str| {
            let path = dir.join(name);
            std::fs::write(&path, bytes).unwrap();
            read_header(&path).unwrap()
        };

        // A whole range is byte-identical to the whole tree: renumbering by 0 and
        // a base of 0 must be a no-op.
        let whole = write_range(&index, 0, 0, n).unwrap();
        assert_eq!(
            whole,
            write(&index, 0).unwrap(),
            "range 0..n must be identical"
        );
        assert_eq!(header_of(&whole, "whole.idx").first_key, 0);

        // A leaf is a closed subtree of one partition: its keys renumber to 0 and
        // the header names where it sits in the segment's key space.
        let leaf = (0..n)
            .find(|at| index.partitions()[*at].children.is_empty())
            .expect("a tree has a leaf");
        let single = write_range(&index, 0, leaf, leaf + 1).unwrap();
        let header = header_of(&single, "leaf.idx");
        assert_eq!(header.partitions, 1);
        assert_eq!(header.first_key, leaf as u32);
        // ... and it opens as a one-partition tree of its own.
        let opened = FileIndex::open(dir.join("leaf.idx"), &opts()).expect("open leaf chunk");
        assert_eq!(opened.partitions(), 1);
        assert_eq!(opened.len(), index.partitions()[leaf].members.len());
    }

    #[test]
    fn tree_meta_round_trips_the_directory() {
        let index = SpannIndex::build(rows(150, 8), opts()).unwrap();
        let dir = std::env::temp_dir().join(format!("spann_meta_{}", std::process::id()));
        std::fs::create_dir_all(&dir).unwrap();
        let path = dir.join("meta.idx");
        write_to(&index, &path, 0).unwrap();
        let opened = FileIndex::open(&path, &opts()).unwrap();

        let meta = opened.tree_meta();
        assert_eq!(meta.len(), index.partitions().len());
        for (at, part) in index.partitions().iter().enumerate() {
            let got = &meta[at];
            assert_eq!(got.key, part.key);
            assert_eq!(got.level, part.level);
            assert_eq!(got.members, part.members.len());
            assert_eq!(got.radius.to_bits(), part.radius.to_bits());
            assert_eq!(got.children, part.children);
            assert_eq!(got.centroid.len(), part.centroid.len());
            for (a, b) in got.centroid.iter().zip(&part.centroid) {
                assert_eq!(a.to_bits(), b.to_bits());
            }
        }
    }

    #[test]
    fn a_keyed_chunk_keeps_its_keys_and_needs_no_renumbering() {
        let index = SpannIndex::build(rows(120, 8), opts()).unwrap();
        let all: Vec<PartitionKey> = index.partitions().iter().map(|part| part.key).collect();
        // Every key, dense: byte-identical to the whole-tree write.
        assert_eq!(
            write_keys(&index, 0, &all).unwrap(),
            write(&index, 0).unwrap()
        );

        // A single leaf, keyed by its own key: section A still names that key,
        // the header names it as the base, and it opens on its own.
        let leaf = (0..all.len())
            .find(|at| index.partitions()[*at].children.is_empty())
            .expect("a leaf");
        let key = all[leaf];
        let dir = std::env::temp_dir().join(format!("spann_keys_{}", std::process::id()));
        std::fs::create_dir_all(&dir).unwrap();
        let path = dir.join("leaf.idx");
        let bytes = write_keys(&index, 0, &[key]).unwrap();
        std::fs::write(&path, &bytes).unwrap();
        let chunk = FileIndex::open(&path, &opts()).unwrap();
        assert_eq!(chunk.partitions(), 1);
        assert_eq!(
            chunk.first_key(),
            key,
            "the key is kept, not renumbered to 0"
        );
        // The reader resolves it by key, not by position.
        assert_eq!(chunk.tree_meta()[0].key, key);

        // A set that cuts a child edge is refused.
        let parent = (0..all.len()).find(|at| !index.partitions()[*at].children.is_empty());
        if let Some(parent) = parent {
            assert!(write_keys(&index, 0, &[all[parent]]).is_err());
        }
    }

    #[test]
    fn crc32c_matches_the_reference_vector() {
        assert_eq!(crc32c(b"123456789"), 0xE306_9283);
    }

    /// The streaming writer and the in-memory one are the same bytes: a dump
    /// that goes straight to a file must be readable by everything that reads
    /// `write`'s output, including the CRC trailers.
    #[test]
    fn streaming_and_in_memory_writes_agree() {
        let index = SpannIndex::build(rows(300, 16), opts()).unwrap();
        let bytes = write(&index, 0).unwrap();
        let path = std::env::temp_dir().join(format!(
            "spann-writer-{}-{:?}.idx",
            std::process::id(),
            std::thread::current().id()
        ));
        let len = write_to(&index, &path, 0).unwrap();
        let streamed = std::fs::read(&path).unwrap();
        let opened = FileIndex::open(&path, &opts()).unwrap();
        let _ = std::fs::remove_file(&path);

        assert_eq!(len as usize, bytes.len());
        assert_eq!(streamed, bytes);
        assert_eq!(opened.len(), index.len());
        assert_eq!(opened.height(), index.height());
    }

    #[test]
    fn round_trip_preserves_the_tree_and_the_answers() {
        let data = rows(300, 16);
        let index = SpannIndex::build(data.clone(), opts()).unwrap();
        let bytes = write(&index, 0).unwrap();
        let chunk = Chunk::parse(&bytes).unwrap();

        assert_eq!(chunk.header().dims, 16);
        assert_eq!(chunk.header().metric, Metric::L2);
        assert_eq!(chunk.header().encode, Encode::Rabitq);
        assert_eq!(chunk.header().seed, index.opts().seed);
        assert_eq!(chunk.header().partitions, index.partitions().len());
        // A whole-tree chunk starts at key 0; a subtree chunk names its base so
        // the segment can map a global key onto `(chunk, local)`.
        assert_eq!(chunk.header().first_key, 0);
        assert_eq!(chunk.height(), index.height());
        assert_eq!(chunk.row_ids().len(), 300);

        let restored = chunk.to_index(opts()).unwrap();
        assert_eq!(restored.len(), index.len());
        assert_eq!(restored.height(), index.height());
        for (a, b) in index.partitions().iter().zip(restored.partitions()) {
            assert_eq!(a.key, b.key);
            assert_eq!(a.level, b.level);
            assert_eq!(a.centroid, b.centroid);
            assert!((a.radius - b.radius).abs() < 1e-6);
            assert_eq!(a.members.len(), b.members.len());
            for (x, y) in a.members.iter().zip(&b.members) {
                assert_eq!(x.id, y.id);
                assert_eq!(x.code.bits, y.code.bits);
                assert!((x.code.norm - y.code.norm).abs() < 1e-6);
            }
        }

        // Same query, same answers.
        for q in data.iter().take(5) {
            let a: Vec<u64> = index
                .search(&q.1, 10, None)
                .into_iter()
                .map(|(id, _)| id)
                .collect();
            let b: Vec<u64> = restored
                .search(&q.1, 10, None)
                .into_iter()
                .map(|(id, _)| id)
                .collect();
            assert_eq!(a, b);
        }
    }

    /// The mmap reader must answer exactly like the in-memory tree — same
    /// descent, same scores, same rows.
    #[test]
    fn file_index_matches_the_in_memory_tree() {
        let data = rows(400, 16);
        let index = SpannIndex::build(data.clone(), opts()).unwrap();
        let bytes = write(&index, 0).unwrap();
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_0.idx");
        std::fs::write(&path, &bytes).unwrap();

        let options = opts();
        let file = FileIndex::open(&path, &options).unwrap();
        assert_eq!(file.dims(), 16);
        assert_eq!(file.len(), 400);
        assert_eq!(file.height(), index.height());
        assert_eq!(file.metric(), Metric::L2);

        for q in data.iter().take(10) {
            let in_memory: Vec<u64> = index
                .search(&q.1, 10, None)
                .into_iter()
                .map(|(id, _)| id)
                .collect();
            let mapped: Vec<u64> = file
                .search(&q.1, 10, None)
                .into_iter()
                .map(|(id, _)| id)
                .collect();
            assert_eq!(in_memory, mapped, "row {}", q.0);
        }

        // A catalog whose seed/metric disagrees must be refused, never misread.
        let mut other = opts();
        other.seed ^= 0x55;
        assert!(FileIndex::open(&path, &other).is_err());
        let mut cos = opts();
        cos.metric = Metric::Cos;
        assert!(FileIndex::open(&path, &cos).is_err());
    }

    /// §17.2's conformance: the same operations must answer identically on the
    /// in-memory tree (`MemStore`) and on the mapped chunk (`MmapStore`). The
    /// single-shape case above is the happy path; this is the matrix - every
    /// metric, both encodings, a range of `k` (including `k > n`), a narrow and a
    /// wide beam, and the filtered probe.
    #[test]
    fn every_operation_matches_between_the_memory_tree_and_the_mapped_chunk() {
        let data = rows(300, 8);
        let dir = tempfile::tempdir().unwrap();
        for metric in [Metric::L2, Metric::Cos, Metric::Ip] {
            for encode in [Encode::Rabitq, Encode::None] {
                let mut options = opts();
                options.metric = metric;
                options.encode = encode;
                let index = SpannIndex::build(data.clone(), options.clone()).unwrap();
                let bytes = write(&index, 0);
                if encode == Encode::None {
                    // A chunk stores rabitq codes only, so a `none` index has
                    // no mapped counterpart - and writing one must be refused
                    // rather than silently re-encoded (the build path keeps
                    // working, which is what the `encode = none` tests cover).
                    assert!(
                        bytes.is_err(),
                        "a `none` index must not be serialised into a chunk"
                    );
                    continue;
                }
                let path = dir.path().join(format!("chunk_{metric:?}_{encode:?}.idx"));
                std::fs::write(&path, bytes.unwrap()).unwrap();
                let file = FileIndex::open(&path, &options).unwrap();
                assert_eq!(file.len(), index.len(), "{metric:?}/{encode:?}");
                assert_eq!(file.height(), index.height(), "{metric:?}/{encode:?}");

                let queries: Vec<&Vec<f32>> = data.iter().take(6).map(|row| &row.1).collect();
                for (k, beam) in [
                    (1usize, None),
                    (10, None),
                    (10, Some(1)),
                    (10, Some(64)),
                    // `k` above the row count: both paths have to return every
                    // row rather than a short or a padded answer.
                    (data.len(), Some(4)),
                ] {
                    for query in &queries {
                        let memory: Vec<(u64, f32)> = index.search(query, k, beam);
                        let mapped: Vec<(u64, f32)> = file.search(query, k, beam);
                        assert_eq!(
                            memory.iter().map(|(id, _)| *id).collect::<Vec<_>>(),
                            mapped.iter().map(|(id, _)| *id).collect::<Vec<_>>(),
                            "{metric:?}/{encode:?} k={k} beam={beam:?}"
                        );
                        for ((_, a), (_, b)) in memory.iter().zip(&mapped) {
                            assert!(
                                (a - b).abs() <= 1e-3 * a.abs().max(1.0),
                                "{metric:?}/{encode:?} k={k} beam={beam:?}: {a} vs {b}"
                            );
                        }
                    }
                }

                // The filtered probe is the path the statement takes whenever a
                // predicate was pushed down, so it has to agree too.
                let allowed: RowMask =
                    RowMask::from_rows((0..data.len() as u64).filter(|i| i % 3 == 0).collect());
                for query in &queries {
                    let memory: Vec<u64> = index
                        .search_filtered(query, 10, Some(8), &allowed)
                        .into_iter()
                        .map(|(id, _)| id)
                        .collect();
                    let mapped: Vec<u64> = file
                        .search_filtered(query, 10, Some(8), &allowed)
                        .into_iter()
                        .map(|(id, _)| id)
                        .collect();
                    assert_eq!(memory, mapped, "{metric:?}/{encode:?} filtered");
                    assert!(
                        mapped.iter().all(|id| allowed.contains(*id)),
                        "{metric:?}/{encode:?}: the probe returned a filtered-out row"
                    );
                }
            }
        }
    }

    /// The incremental tree publishes through the same chunk format: what the
    /// live index answered and what a restart reads back are the same rows.
    #[test]
    fn an_incremental_tree_round_trips_through_a_chunk() {
        use index::spann::incr::IncIndex;
        use index::spann::source::{row_source, Memory};

        // 128d, not the 8d the format tests use: a 1-bit code cannot rank rows
        // at 8 dimensions (the estimate noise is the inter-row distance), and
        // this test is about the publish/read-back path, not the quantizer.
        let dim = 128;
        let mut state = 0x9E37_79B9_7F4A_7C15u64;
        let mut next = move || {
            state = state
                .wrapping_mul(6364136223846793005)
                .wrapping_add(1442695040888963407);
            (state >> 40) as f32 / (1u64 << 24) as f32
        };
        let rows: Vec<(u64, Vec<f32>)> = (0..200u64)
            .map(|i| {
                let v: Vec<f32> = (0..dim).map(|_| next() + next() - 1.0).collect();
                (i, v)
            })
            .collect();

        let options = opts();
        let source = Memory::new(rows.clone());
        let lookup = row_source(&source);
        let mut inc = IncIndex::new(dim, options.clone()).unwrap();
        for (id, v) in &rows {
            inc.insert(*id, v).unwrap();
            if inc.pending_fixups() > 0 {
                inc.run_fixups(Some(&lookup));
            }
        }
        inc.run_fixups(Some(&lookup));
        assert!(inc.splits() > 0, "the fixture has to split");

        let published = inc.to_spann().expect("a built index publishes");
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_0.idx");
        write_to(&published, &path, 0).unwrap();
        let file = FileIndex::open(&path, &options).unwrap();

        // Every row survives the fixups. A merge that picks an interior node as
        // its target appends to that node's `members`/`pending`, where no
        // descent, scan or publish ever looks, so this is the assertion that
        // catches a row going missing without any counter moving.
        let mut want = inc.row_ids();
        want.sort_unstable();
        want.dedup();
        assert_eq!(
            want,
            (0..rows.len() as u64).collect::<Vec<_>>(),
            "the incremental tree keeps every row it was given"
        );
        assert_eq!(published.len(), inc.len(), "publishing keeps every row");
        assert_eq!(file.len(), inc.len());
        let mut got = file.row_ids();
        got.sort_unstable();
        assert_eq!(got, want, "the chunk covers the same rows");

        // The heap tree and its chunk agree (same descent, same scores).
        for q in rows.iter().take(10) {
            let heap: Vec<u64> = published
                .search(&q.1, 5, Some(64))
                .into_iter()
                .map(|(id, _)| id)
                .collect();
            let reopened: Vec<u64> = file
                .search(&q.1, 5, Some(64))
                .into_iter()
                .map(|(id, _)| id)
                .collect();
            assert_eq!(heap, reopened, "row {}", q.0);
        }

        // And every row still finds itself through the restart.
        let mut hit = 0usize;
        for (id, v) in &rows {
            let got = file.search(v, 1, Some(64)).first().map(|(g, _)| *g);
            if got != Some(*id) {
                let live = inc.search(v, 1, 64).first().map(|(g, _)| *g);
                eprintln!("miss id {id}: chunk got {got:?}, live got {live:?}");
            }
            if got == Some(*id) {
                hit += 1;
            }
        }
        assert_eq!(hit, rows.len(), "self-query after a restart: {hit}/200");
    }

    #[test]
    fn slot_pages_are_page_aligned_and_complete() {
        let index = SpannIndex::build(rows(200, 8), opts()).unwrap();
        let bytes = write(&index, 0).unwrap();
        let chunk = Chunk::parse(&bytes).unwrap();
        let slots_offset = chunk.sections[Section::Slots as usize].0;
        assert_eq!(slots_offset % SLOT_ALIGN, 0);
        for key in 0..chunk.header().partitions as PartitionKey {
            let meta = chunk.meta(key).unwrap();
            let view = chunk.slots(key).unwrap();
            assert_eq!(view.len(), meta.count as usize);
            assert!(view.row_id(view.len()).is_none());
            for i in 0..view.len() {
                assert_eq!(view.code(i).unwrap().bits.len(), 1, "8d -> 1 byte code");
            }
        }
    }

    #[test]
    fn a_flipped_byte_is_rejected_by_the_checksum() {
        let index = SpannIndex::build(rows(120, 8), opts()).unwrap();
        let bytes = write(&index, 0).unwrap();
        let (offset, len) = Chunk::parse(&bytes).unwrap().sections[Section::Centroids as usize];
        assert!(len > 0);
        let mut corrupt = bytes.clone();
        corrupt[offset + 3] ^= 0x40;
        let err = Chunk::parse(&corrupt).unwrap_err();
        assert!(
            matches!(&err, SpannError::Corrupt(msg) if msg.contains("checksum")),
            "{err}"
        );
    }

    #[test]
    fn truncated_or_foreign_files_are_rejected() {
        let index = SpannIndex::build(rows(120, 8), opts()).unwrap();
        let bytes = write(&index, 0).unwrap();

        let err = Chunk::parse(&bytes[..HEADER_LEN]).unwrap_err();
        assert!(matches!(err, SpannError::Corrupt(_)));

        let mut wrong_magic = bytes.clone();
        wrong_magic[0] = b'X';
        assert!(matches!(
            Chunk::parse(&wrong_magic).unwrap_err(),
            SpannError::Corrupt(_)
        ));

        // A valid header that claims a section beyond EOF.
        let mut bad_table = bytes.clone();
        bad_table[SECTION_TABLE_AT..SECTION_TABLE_AT + 8]
            .copy_from_slice(&(u64::MAX / 2).to_le_bytes());
        assert!(matches!(
            Chunk::parse(&bad_table).unwrap_err(),
            SpannError::Corrupt(_)
        ));
    }

    #[test]
    fn catalog_mismatch_is_rejected() {
        let index = SpannIndex::build(rows(50, 8), opts()).unwrap();
        let bytes = write(&index, 0).unwrap();
        let chunk = Chunk::parse(&bytes).unwrap();

        let mut other = opts();
        other.seed ^= 1;
        assert!(matches!(
            chunk.to_index(other).unwrap_err(),
            SpannError::Corrupt(_)
        ));

        let mut cos = opts();
        cos.metric = Metric::Cos;
        assert!(matches!(
            chunk.to_index(cos).unwrap_err(),
            SpannError::Corrupt(_)
        ));
    }

    #[test]
    fn encode_none_is_not_persisted_yet() {
        let index = SpannIndex::build(
            rows(20, 8),
            Options {
                encode: Encode::None,
                ..opts()
            },
        )
        .unwrap();
        assert!(matches!(
            write(&index, 0).unwrap_err(),
            SpannError::InvalidOption("encode", _)
        ));
    }
}
