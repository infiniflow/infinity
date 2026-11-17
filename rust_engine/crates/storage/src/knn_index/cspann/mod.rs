//! The cspann chunk format (`docs/CSPANN_DESIGN.md` §6).
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
//! 0    128      header
//! 32   ..       section table: 6 × { offset u64, len u64 }
//! 128  ..       Section A  partition metadata, 32 B per partition
//!      ..       Section B  centroids, dims × f32 per partition
//!      ..       Section C  child keys, u32 per child
//!      ..       Section D  slot pages, one 4 KB-aligned column group per partition
//!      ..       Section E  full vectors (reserved for `encode = none`)
//!      ..       Section F  row ids, u64 per covered row
//! end  -32      trailer: 8 B magic + 6 × u32 CRC32C (one per section)
//! ```
//!
//! Column groups inside a slot page (§6.2's columnar layout):
//!
//! ```text
//! row_ids  : u64 × n
//! norm     : f32 × n     ‖o − c‖
//! raw_norm : f32 × n     ‖o‖²   (ip / cosine)
//! sum      : f32 × n     popcount of the code
//! error    : f32 × n     ⟨u, û⟩  (the error bound in §8.3)
//! codes    : u8 × n × ceil(dims/8)
//! ```
//!
//! `row_id` is `u64` and not the design's `u32`: row ids are engine-wide and a
//! chunk is not guaranteed to start at 0, so 4 bytes saved per row is not worth
//! a silent truncation.

use index::cspann::{
    partition::{metric_score, Member, Partition},
    quantize::{self, code_bytes, Code, Rotation},
    search::{Beam, SearchSet},
    CspannError, CspannIndex, Encode, Options, PartitionKey, ROOT_KEY,
};
use index::vector::{l2_sq, Metric};

pub const MAGIC: [u8; 8] = *b"CSPANN\x00\x01";
const TRAILER_MAGIC: [u8; 8] = *b"CSPEND\x00\x01";
pub const HEADER_LEN: usize = 128;
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
        let at = i * 8;
        Some(u64::from_le_bytes(
            self.row_ids.get(at..at + 8)?.try_into().ok()?,
        ))
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
    pub fn open(bytes: &[u8]) -> Result<Self, CspannError> {
        if bytes.len() < HEADER_LEN + TRAILER_LEN {
            return Err(CspannError::Corrupt("file shorter than header".to_string()));
        }
        if bytes[0..8] != MAGIC {
            return Err(CspannError::Corrupt("bad magic".to_string()));
        }
        let version = u16::from_le_bytes([bytes[8], bytes[9]]);
        if version != 1 {
            return Err(CspannError::Corrupt(format!(
                "unsupported version {version}"
            )));
        }
        let header_len = u16::from_le_bytes([bytes[10], bytes[11]]) as usize;
        if header_len != HEADER_LEN {
            return Err(CspannError::Corrupt(format!(
                "header length {header_len}, expected {HEADER_LEN}"
            )));
        }
        let dims = u32::from_le_bytes(bytes[12..16].try_into().unwrap()) as usize;
        if dims == 0 {
            return Err(CspannError::Corrupt("zero dimension".to_string()));
        }
        let metric = match bytes[16] {
            0 => Metric::L2,
            1 => Metric::Ip,
            2 => Metric::Cos,
            other => return Err(CspannError::Corrupt(format!("bad metric {other}"))),
        };
        let encode = match bytes[17] {
            0 => Encode::None,
            1 => Encode::Rabitq,
            other => return Err(CspannError::Corrupt(format!("bad encode {other}"))),
        };
        let rot_algo = bytes[18];
        let seed = u64::from_le_bytes(bytes[20..28].try_into().unwrap());
        let partitions = u32::from_le_bytes(bytes[28..32].try_into().unwrap()) as usize;
        if partitions == 0 {
            return Err(CspannError::Corrupt("no partitions".to_string()));
        }

        // Section table, then bounds and CRCs.
        let tail = bytes.len() - TRAILER_LEN;
        if bytes[tail..tail + 8] != TRAILER_MAGIC {
            return Err(CspannError::Corrupt("missing trailer".to_string()));
        }
        let mut sections = [(0usize, 0usize); SECTION_COUNT];
        for section in 0..SECTION_COUNT {
            let at = 32 + section * 16;
            let offset = u64::from_le_bytes(bytes[at..at + 8].try_into().unwrap()) as usize;
            let len = u64::from_le_bytes(bytes[at + 8..at + 16].try_into().unwrap()) as usize;
            let end = offset
                .checked_add(len)
                .ok_or_else(|| CspannError::Corrupt("section overflow".to_string()))?;
            if offset < HEADER_LEN || end > tail {
                return Err(CspannError::Corrupt(format!(
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
                return Err(CspannError::Corrupt(format!(
                    "section {} checksum {got:#x} != {want:#x}",
                    SECTION_NAMES[section]
                )));
            }
            sections[section] = (offset, len);
        }

        let meta_len = sections[Section::Meta as usize].1;
        if meta_len != partitions * META_ENTRY {
            return Err(CspannError::Corrupt(format!(
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
            },
            sections,
        })
    }
}

impl<'a> Chunk<'a> {
    /// Parses and verifies `bytes` (the open-time path).
    pub fn parse(bytes: &'a [u8]) -> Result<Self, CspannError> {
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
            Encode::Rabitq => index::cspann::quantize::code_bytes(self.header.dims),
        }
    }

    fn section(&self, section: Section) -> &'a [u8] {
        let (offset, len) = self.sections[section as usize];
        &self.bytes[offset..offset + len]
    }

    pub fn meta(&self, key: PartitionKey) -> Option<PartitionMeta> {
        if key as usize >= self.header.partitions {
            return None;
        }
        let at = key as usize * META_ENTRY;
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
        let block_len = count * (24 + code_bytes);
        let start = meta.slots_off as usize;
        let slots = self.section(Section::Slots).get(start..start + block_len)?;
        let mut at = 0usize;
        let mut take = |bytes: usize| {
            let out = &slots[at..at + bytes];
            at += bytes;
            out
        };
        let row_ids = take(count * 8);
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
        })
    }

    pub fn row_ids(&self) -> Vec<u64> {
        self.section(Section::RowIds)
            .as_chunks::<8>()
            .0
            .iter()
            .map(|b| u64::from_le_bytes(*b))
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
    pub fn to_index(&self, opts: Options) -> Result<CspannIndex, CspannError> {
        if opts.metric != self.header.metric {
            return Err(CspannError::Corrupt(format!(
                "metric mismatch: catalog {:?}, chunk {:?}",
                opts.metric, self.header.metric
            )));
        }
        if opts.encode != self.header.encode {
            return Err(CspannError::Corrupt(format!(
                "encode mismatch: catalog {:?}, chunk {:?}",
                opts.encode, self.header.encode
            )));
        }
        if opts.seed != self.header.seed {
            return Err(CspannError::Corrupt(format!(
                "seed mismatch: catalog {}, chunk {}",
                opts.seed, self.header.seed
            )));
        }
        let rotation = Rotation::new(self.header.dims, self.header.seed);
        let mut partitions = Vec::with_capacity(self.header.partitions);
        for key in 0..self.header.partitions as PartitionKey {
            let meta = self
                .meta(key)
                .ok_or_else(|| CspannError::Corrupt(format!("missing meta {key}")))?;
            let centroid = self
                .centroid(key)
                .ok_or_else(|| CspannError::Corrupt(format!("missing centroid {key}")))?;
            let rot_centroid = rotation.apply(&centroid);
            let view = self
                .slots(key)
                .ok_or_else(|| CspannError::Corrupt(format!("missing slots {key}")))?;
            let mut members = Vec::with_capacity(view.len());
            for i in 0..view.len() {
                let id = view
                    .row_id(i)
                    .ok_or_else(|| CspannError::Corrupt(format!("slot {i} of {key}")))?;
                let code = view
                    .code(i)
                    .ok_or_else(|| CspannError::Corrupt(format!("code {i} of {key}")))?;
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
        Ok(CspannIndex::from_parts(
            opts,
            self.header.dims,
            rotation,
            partitions,
            height,
            ROOT_KEY,
        ))
    }
}

/// Serialises a built index into the chunk format.
pub fn write(index: &CspannIndex) -> Result<Vec<u8>, CspannError> {
    let opts = index.opts();
    if opts.encode != Encode::Rabitq {
        // Section E is reserved; v1 only ships the production encoding.
        return Err(CspannError::InvalidOption(
            "encode",
            "chunk format v1 only stores rabitq codes".to_string(),
        ));
    }
    let dims = index.dim();
    let parts = index.partitions();
    let code_bytes = code_bytes(dims);

    let mut meta = Vec::with_capacity(parts.len() * META_ENTRY);
    let mut centroids = Vec::with_capacity(parts.len() * dims * 4);
    let mut children = Vec::new();
    let mut slots: Vec<u8> = Vec::new();
    for part in parts {
        while !slots.len().is_multiple_of(SLOT_ALIGN) {
            slots.push(0);
        }
        let centroids_off = centroids.len() as u32;
        for x in &part.centroid {
            centroids.extend_from_slice(&x.to_le_bytes());
        }
        let children_off = children.len() / 4;
        for child in &part.children {
            children.extend_from_slice(&child.to_le_bytes());
        }
        let slots_off = slots.len() as u32;

        let count = part.members.len();
        for member in &part.members {
            slots.extend_from_slice(&member.id.to_le_bytes());
        }
        for member in &part.members {
            slots.extend_from_slice(&member.code.norm.to_le_bytes());
        }
        for member in &part.members {
            slots.extend_from_slice(&member.code.raw_norm.to_le_bytes());
        }
        for member in &part.members {
            slots.extend_from_slice(&member.code.sum.to_le_bytes());
        }
        for member in &part.members {
            slots.extend_from_slice(&member.code.error.to_le_bytes());
        }
        for member in &part.members {
            debug_assert_eq!(member.code.bits.len(), code_bytes);
            slots.extend_from_slice(&member.code.bits);
        }

        meta.extend_from_slice(&part.key.to_le_bytes());
        meta.extend_from_slice(&(part.level as u16).to_le_bytes());
        meta.extend_from_slice(&(count as u16).to_le_bytes());
        meta.extend_from_slice(&part.radius.to_le_bytes());
        meta.extend_from_slice(&centroids_off.to_le_bytes());
        meta.extend_from_slice(&slots_off.to_le_bytes());
        meta.extend_from_slice(&(children_off as u32).to_le_bytes());
        meta.extend_from_slice(&(part.children.len() as u16).to_le_bytes());
        meta.extend_from_slice(&0u16.to_le_bytes());
        meta.extend_from_slice(&0u32.to_le_bytes());
    }

    let mut row_ids = Vec::new();
    for id in index.row_ids() {
        row_ids.extend_from_slice(&id.to_le_bytes());
    }

    let sections: [&[u8]; SECTION_COUNT] = [&meta, &centroids, &children, &slots, &[], &row_ids];

    // Layout: header, then each section 8-byte aligned (slot pages to a page).
    let mut out: Vec<u8> = vec![0; HEADER_LEN];
    let mut table = [(0u64, 0u64); SECTION_COUNT];
    for (i, bytes) in sections.iter().enumerate() {
        let align = if i == Section::Slots as usize {
            SLOT_ALIGN
        } else {
            8
        };
        while !out.len().is_multiple_of(align) {
            out.push(0);
        }
        table[i] = (out.len() as u64, bytes.len() as u64);
        out.extend_from_slice(bytes);
    }

    let mut trailer = Vec::with_capacity(TRAILER_LEN);
    trailer.extend_from_slice(&TRAILER_MAGIC);
    for (i, bytes) in sections.iter().enumerate() {
        trailer.extend_from_slice(&crc32c(bytes).to_le_bytes());
        let _ = i;
    }
    out.extend_from_slice(&trailer);

    out[0..8].copy_from_slice(&MAGIC);
    out[8..10].copy_from_slice(&1u16.to_le_bytes());
    out[10..12].copy_from_slice(&(HEADER_LEN as u16).to_le_bytes());
    out[12..16].copy_from_slice(&(dims as u32).to_le_bytes());
    out[16] = match opts.metric {
        Metric::L2 => 0,
        Metric::Ip => 1,
        Metric::Cos => 2,
    };
    out[17] = match opts.encode {
        Encode::None => 0,
        Encode::Rabitq => 1,
    };
    out[18] = 1; // rot_algo: 1 = block-diagonal RHT
    out[19] = 0;
    out[20..28].copy_from_slice(&opts.seed.to_le_bytes());
    out[28..32].copy_from_slice(&(parts.len() as u32).to_le_bytes());
    for (i, (offset, len)) in table.iter().enumerate() {
        let at = 32 + i * 16;
        out[at..at + 8].copy_from_slice(&offset.to_le_bytes());
        out[at + 8..at + 16].copy_from_slice(&len.to_le_bytes());
    }
    Ok(out)
}

/// CRC-32C (Castagnoli), the polynomial §6.1 names. Bitwise on purpose: the
/// chunk hashes run once per file, not per query.
pub fn crc32c(data: &[u8]) -> u32 {
    let mut crc = !0u32;
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
    !crc
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
    pub fn open(path: impl AsRef<std::path::Path>, opts: &Options) -> Result<Self, CspannError> {
        let map = crate::mapped::MappedFile::open_path(path)
            .map_err(|e| CspannError::Corrupt(format!("map: {e}")))?;
        let layout = Layout::open(&map)?;
        if layout.header.metric != opts.metric
            || layout.header.encode != opts.encode
            || layout.header.seed != opts.seed
        {
            return Err(CspannError::Corrupt(format!(
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

    /// A view over the mapping; the layout was verified at open, so this is free.
    fn chunk(&self) -> Chunk<'_> {
        Chunk::from_layout(&self.map, self.layout.clone())
    }

    pub fn dims(&self) -> usize {
        self.layout.header.dims
    }

    pub fn len(&self) -> usize {
        self.layout.sections[Section::RowIds as usize].1 / 8
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
    /// (`docs/CSPANN_DESIGN.md` §9.7).
    pub fn row_ids(&self) -> Vec<u64> {
        self.chunk().row_ids()
    }

    pub fn partitions(&self) -> usize {
        self.layout.header.partitions
    }

    pub fn metric(&self) -> Metric {
        self.layout.header.metric
    }

    /// Same descent as `CspannIndex::search`, but every partition is read from
    /// the mapping (`§8.4`: beam per level, radius bound, exact rescoring by the
    /// caller).
    pub fn search(&self, q: &[f32], k: usize, beam: Option<usize>) -> Vec<(u64, f32)> {
        let dims = self.dims();
        if k == 0 || q.len() != dims {
            return Vec::new();
        }
        let chunk = self.chunk();
        let metric = self.layout.header.metric;
        let width = beam.unwrap_or(self.beam).max(1);

        let mut current: Vec<PartitionKey> = vec![ROOT_KEY];
        let mut level = chunk.height();
        while level > 1 {
            let mut next = Beam::new(width);
            for key in &current {
                for child in chunk.children(*key) {
                    let Some(centroid) = chunk.centroid(child) else {
                        continue;
                    };
                    let radius = chunk.meta(child).map(|m| m.radius).unwrap_or(0.0);
                    let distance = l2_sq(q, &centroid).max(0.0).sqrt();
                    let lower = if metric.is_l2() {
                        (distance - radius).max(0.0)
                    } else {
                        distance
                    };
                    next.push(lower, child);
                }
            }
            current = next.into_keys();
            if current.is_empty() {
                return Vec::new();
            }
            level -= 1;
        }

        let mut set = SearchSet::with_default_capacity(k);
        for key in &current {
            let Some(slots) = chunk.slots(*key) else {
                continue;
            };
            let Some(centroid) = chunk.centroid(*key) else {
                continue;
            };
            let rot_centroid = self.rotation.apply(&centroid);
            let query = quantize::make_query(&self.rotation, &rot_centroid, q);
            for i in 0..slots.len() {
                let (Some(row), Some((norm, raw_norm, sum, error, bits))) =
                    (slots.row_id(i), slots.parts(i))
                else {
                    continue;
                };
                // Scored from the raw columns: no per-member allocation, which is
                // what made the first mapped version 3.4× slower than heap.
                let squared =
                    quantize::distance_estimate_parts(&query, norm, sum, error, bits, dims);
                let err = quantize::error_bound_parts(&query, norm, error);
                let (score, bound) = metric_score(metric, squared, err, query.raw_norm, raw_norm);
                set.add(score, bound, row);
            }
        }
        set.into_sorted()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use index::cspann::CspannIndex;

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

    #[test]
    fn crc32c_matches_the_reference_vector() {
        assert_eq!(crc32c(b"123456789"), 0xE306_9283);
    }

    #[test]
    fn round_trip_preserves_the_tree_and_the_answers() {
        let data = rows(300, 16);
        let index = CspannIndex::build(data.clone(), opts()).unwrap();
        let bytes = write(&index).unwrap();
        let chunk = Chunk::parse(&bytes).unwrap();

        assert_eq!(chunk.header().dims, 16);
        assert_eq!(chunk.header().metric, Metric::L2);
        assert_eq!(chunk.header().encode, Encode::Rabitq);
        assert_eq!(chunk.header().seed, index.opts().seed);
        assert_eq!(chunk.header().partitions, index.partitions().len());
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
        let index = CspannIndex::build(data.clone(), opts()).unwrap();
        let bytes = write(&index).unwrap();
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

    #[test]
    fn slot_pages_are_page_aligned_and_complete() {
        let index = CspannIndex::build(rows(200, 8), opts()).unwrap();
        let bytes = write(&index).unwrap();
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
        let index = CspannIndex::build(rows(120, 8), opts()).unwrap();
        let bytes = write(&index).unwrap();
        let (offset, len) = Chunk::parse(&bytes).unwrap().sections[Section::Centroids as usize];
        assert!(len > 0);
        let mut corrupt = bytes.clone();
        corrupt[offset + 3] ^= 0x40;
        let err = Chunk::parse(&corrupt).unwrap_err();
        assert!(
            matches!(&err, CspannError::Corrupt(msg) if msg.contains("checksum")),
            "{err}"
        );
    }

    #[test]
    fn truncated_or_foreign_files_are_rejected() {
        let index = CspannIndex::build(rows(120, 8), opts()).unwrap();
        let bytes = write(&index).unwrap();

        let err = Chunk::parse(&bytes[..HEADER_LEN]).unwrap_err();
        assert!(matches!(err, CspannError::Corrupt(_)));

        let mut wrong_magic = bytes.clone();
        wrong_magic[0] = b'X';
        assert!(matches!(
            Chunk::parse(&wrong_magic).unwrap_err(),
            CspannError::Corrupt(_)
        ));

        // A valid header that claims a section beyond EOF.
        let mut bad_table = bytes.clone();
        bad_table[32..40].copy_from_slice(&(u64::MAX / 2).to_le_bytes());
        assert!(matches!(
            Chunk::parse(&bad_table).unwrap_err(),
            CspannError::Corrupt(_)
        ));
    }

    #[test]
    fn catalog_mismatch_is_rejected() {
        let index = CspannIndex::build(rows(50, 8), opts()).unwrap();
        let bytes = write(&index).unwrap();
        let chunk = Chunk::parse(&bytes).unwrap();

        let mut other = opts();
        other.seed ^= 1;
        assert!(matches!(
            chunk.to_index(other).unwrap_err(),
            CspannError::Corrupt(_)
        ));

        let mut cos = opts();
        cos.metric = Metric::Cos;
        assert!(matches!(
            chunk.to_index(cos).unwrap_err(),
            CspannError::Corrupt(_)
        ));
    }

    #[test]
    fn encode_none_is_not_persisted_yet() {
        let index = CspannIndex::build(
            rows(20, 8),
            Options {
                encode: Encode::None,
                ..opts()
            },
        )
        .unwrap();
        assert!(matches!(
            write(&index).unwrap_err(),
            CspannError::InvalidOption("encode", _)
        ));
    }
}
