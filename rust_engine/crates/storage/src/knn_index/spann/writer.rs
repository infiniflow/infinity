//! Chunk serialisation (`docs/SPANN_DESIGN.md` §6).
//!
//! Two entry points over one implementation:
//!
//! * [`write`] returns the bytes, which is what tests and small dumps want;
//! * [`write_to`] streams the same bytes into a file.
//!
//! The streaming form exists because the build is out-of-core everywhere else:
//! keeping all six sections alive (section D alone is ~208 B per row at 1536d)
//! and then copying them into one `Vec<u8>` peaks at roughly twice the chunk,
//! which at 50M rows is 20 GB of avoidable allocation. Sizes are deterministic,
//! so the section table is filled before the body is written and each section's
//! CRC is folded in as its bytes go out.

use std::io::Write;

use index::spann::quantize::code_bytes;
use index::spann::{Encode, Partition, PartitionKey, SpannIndex};
use index::vector::Metric;

use super::{
    Crc32c, Section, SpannError, HEADER_LEN, MAGIC, META_ENTRY, SECTION_COUNT, SECTION_TABLE_AT,
    SLOT_ALIGN, TRAILER_LEN, TRAILER_MAGIC, VERSION,
};

/// Rounds `value` up to `align`.
pub(crate) const fn align_up(value: usize, align: usize) -> usize {
    value.div_ceil(align) * align
}

/// The chunk for `index`, in memory.
///
/// `segment` is the segment the tree was built over: the slots store the
/// segment's own offsets (§6.2), so every member id must be inside it.
pub fn write(index: &SpannIndex, segment: u32) -> Result<Vec<u8>, SpannError> {
    let mut out: Vec<u8> = Vec::new();
    sections(index, segment, &mut out)?;
    Ok(out)
}

/// The chunk for the half-open partition range `[lo, hi)` of `index`.
///
/// A subtree chunk is the unit a manifest hands out: its keys are renumbered to
/// `0..(hi - lo)`, `header.first_key` names `lo`, and every child edge must stay
/// inside the range - a subtree is closed under children by construction, so an
/// edge that leaves it is a caller bug, not something to paper over.
pub fn write_range(
    index: &SpannIndex,
    segment: u32,
    lo: usize,
    hi: usize,
) -> Result<Vec<u8>, SpannError> {
    let all = index.partitions();
    if lo >= hi || hi > all.len() {
        return Err(SpannError::InvalidOption(
            "range",
            format!("partition range {lo}..{hi} of {}", all.len()),
        ));
    }
    let keys: Vec<PartitionKey> = all[lo..hi].iter().map(|part| part.key).collect();
    write_keys(index, segment, &keys)
}

/// The chunk for an explicit set of partition keys.
///
/// This is the form an incremental dump needs: the keys of a re-keyed tree are
/// **stable identifiers**, not positions, so the dirty set is a set (a leaf plus
/// its ancestors), not a numeric range. Section A is written sorted by key and
/// rows are read back through it (`FileIndex::slot_of`), so a chunk can hold any
/// closed set of partitions - children included - without renumbering anything.
pub fn write_keys(
    index: &SpannIndex,
    segment: u32,
    keys: &[PartitionKey],
) -> Result<Vec<u8>, SpannError> {
    let mut out: Vec<u8> = Vec::new();
    sections_keys(index, segment, keys, true, &mut out)?;
    Ok(out)
}

/// The chunk for a **dirty set** (§5.4): the partitions a checkpoint rewrote,
/// with edges to partitions it did not touch left **dangling**.
///
/// A delta chunk is not a tree on its own - a leaf's parent may live in the
/// chunk that is already published - so it is only readable through
/// [`super::ChunkChain`], which resolves a key against the newest chunk that
/// names it. Every other invariant is `write_keys`': keys stay as the tree
/// allocated them, and the byte layout is the same, so a full-set write through
/// this entry point is byte-identical to [`write_keys`].
pub fn write_dirty(
    index: &SpannIndex,
    segment: u32,
    keys: &[PartitionKey],
) -> Result<Vec<u8>, SpannError> {
    let mut out: Vec<u8> = Vec::new();
    sections_keys(index, segment, keys, false, &mut out)?;
    Ok(out)
}

/// [`write_dirty`] streamed into `path`; returns its length in bytes.
pub fn write_dirty_to(
    index: &SpannIndex,
    path: impl AsRef<std::path::Path>,
    segment: u32,
    keys: &[PartitionKey],
) -> Result<u64, SpannError> {
    let mut failure: Option<SpannError> = None;
    let mut produce = |sink: &mut dyn std::io::Write| -> std::io::Result<()> {
        let mut out = sink;
        match sections_keys(index, segment, keys, false, &mut out) {
            Ok(()) => Ok(()),
            Err(e) => {
                failure = Some(e);
                Err(std::io::Error::other("chunk section failed"))
            }
        }
    };
    let len = crate::chunk_store::write_streaming_file(path, true, &mut produce)
        .map_err(|e| SpannError::Corrupt(format!("write chunk: {e}")))?;
    if let Some(e) = failure {
        return Err(e);
    }
    Ok(len)
}

/// The chunk for `index`, streamed into `path`; returns its length in bytes.
///
/// The stream goes through [`crate::chunk_store`], i.e. through the
/// `PersistenceManager` on a persistence profile: a chunk a catalog record
/// names must be an object, not just a file in the cache. `dedicated` is the
/// reference's `try_compose = false` - a chunk is one object of its own, so a
/// reader maps the chunk rather than a part of a shared object.
pub fn write_to(
    index: &SpannIndex,
    path: impl AsRef<std::path::Path>,
    segment: u32,
) -> Result<u64, SpannError> {
    let mut failure: Option<SpannError> = None;
    let mut produce = |sink: &mut dyn std::io::Write| -> std::io::Result<()> {
        // `&mut dyn Write` is itself `Write` (and sized), which is what the
        // section builder is generic over: re-borrowing costs nothing and keeps
        // every helper in this file off a `?Sized` bound.
        let mut out = sink;
        match sections(index, segment, &mut out) {
            Ok(()) => Ok(()),
            Err(e) => {
                // The section builder is fallible (`encode = none`, an id from
                // another segment); hand its error back rather than wrapping it
                // as an IO failure the caller cannot read.
                failure = Some(e);
                Err(std::io::Error::other("chunk section failed"))
            }
        }
    };
    let len = crate::chunk_store::write_streaming_file(path, true, &mut produce)
        .map_err(|e| SpannError::Corrupt(format!("write chunk: {e}")))?;
    if let Some(e) = failure {
        return Err(e);
    }
    Ok(len)
}

fn put<W: Write>(
    out: &mut W,
    crc: &mut Crc32c,
    pos: &mut usize,
    bytes: &[u8],
) -> Result<(), SpannError> {
    out.write_all(bytes)
        .map_err(|e| SpannError::Corrupt(format!("write chunk: {e}")))?;
    crc.update(bytes);
    *pos += bytes.len();
    Ok(())
}

/// Emits the alignment padding that puts the next section at `target`.
///
/// The padding sits *between* sections, so it belongs to no checksum - but the
/// reader addresses every section by the table, which means the bytes have to be
/// there or every later section reads shifted.
fn pad_to<W: Write>(out: &mut W, pos: &mut usize, target: usize) -> Result<(), SpannError> {
    const ZEROS: [u8; 64] = [0; 64];
    while *pos < target {
        let take = (target - *pos).min(ZEROS.len());
        out.write_all(&ZEROS[..take])
            .map_err(|e| SpannError::Corrupt(format!("write padding: {e}")))?;
        *pos += take;
    }
    Ok(())
}

/// The segment-local half of `id`, or an error naming the mismatch.
///
/// A chunk is one segment's tree, so an id from another segment means the
/// caller built it over the wrong rows - writing the low half alone would
/// silently alias them onto this segment's rows.
fn segment_offset(id: u64, segment: u32) -> Result<u32, SpannError> {
    if (id >> 32) as u32 != segment {
        return Err(SpannError::InvalidOption(
            "segment",
            format!("row id {id:#x} is not in segment {segment}"),
        ));
    }
    Ok(id as u32)
}

fn sections<W: Write>(index: &SpannIndex, segment: u32, out: &mut W) -> Result<(), SpannError> {
    let keys: Vec<PartitionKey> = index.partitions().iter().map(|part| part.key).collect();
    sections_keys(index, segment, &keys, true, out)
}

/// `closed` is the difference between a standalone chunk and a delta: a chunk
/// that is read on its own must hold every child it names, while a delta's
/// untouched children live in the chunk it supersedes.
fn sections_keys<W: Write>(
    index: &SpannIndex,
    segment: u32,
    keys: &[PartitionKey],
    closed: bool,
    out: &mut W,
) -> Result<(), SpannError> {
    let opts = index.opts();
    if opts.encode != Encode::Rabitq {
        // Section E is reserved; the format only ships the production encoding.
        return Err(SpannError::InvalidOption(
            "encode",
            "a chunk stores rabitq codes only".to_string(),
        ));
    }
    let dims = index.dim();
    let all = index.partitions();
    if keys.is_empty() {
        return Err(SpannError::InvalidOption(
            "keys",
            "a chunk needs at least one partition".to_string(),
        ));
    }
    // Sorted, because section A's key column is what the reader binary searches.
    let mut sorted: Vec<PartitionKey> = keys.to_vec();
    sorted.sort_unstable();
    sorted.dedup();
    let mut parts: Vec<&Partition> = Vec::with_capacity(sorted.len());
    for key in &sorted {
        let at = all
            .binary_search_by_key(key, |part| part.key)
            .map_err(|_| {
                SpannError::InvalidOption("keys", format!("no partition with key {key}"))
            })?;
        parts.push(&all[at]);
    }
    let member_keys: std::collections::HashSet<PartitionKey> = sorted.iter().copied().collect();
    let whole = parts.len() == all.len();
    let row_ids: Vec<u64> = if whole {
        index.row_ids()
    } else {
        let mut ids: Vec<u64> = parts
            .iter()
            .flat_map(|part| part.members.iter().map(|member| member.id))
            .collect();
        ids.sort_unstable();
        ids
    };
    let code_bytes = code_bytes(dims);
    // One slot is row_off + norm + raw_norm + sum + error + code (§6.2).
    let slot_bytes = 4 + 4 * 4 + code_bytes;
    // Where each partition's page starts inside section D, and how many bytes of
    // it are payload. A page is padded *up to* the alignment so the next one
    // starts aligned, which means the last page carries no trailing padding -
    // the in-memory writer never emitted it, and the checksum covers the bytes.
    let mut page_offsets = Vec::with_capacity(parts.len());
    let mut page_data = Vec::with_capacity(parts.len());
    let mut running = 0usize;
    for part in &parts {
        page_offsets.push(running);
        let data = part.members.len() * slot_bytes;
        page_data.push(data);
        running += align_up(data, SLOT_ALIGN);
    }
    let slots_len = match (page_offsets.last(), page_data.last()) {
        (Some(off), Some(data)) => off + data,
        _ => 0,
    };

    let mut lens = [0u64; SECTION_COUNT];
    lens[Section::Meta as usize] = (parts.len() * META_ENTRY) as u64;
    lens[Section::Centroids as usize] = (parts.len() * dims * 4) as u64;
    lens[Section::Children as usize] =
        (parts.iter().map(|p| p.children.len()).sum::<usize>() * 4) as u64;
    lens[Section::Slots as usize] = slots_len as u64;
    lens[Section::Vectors as usize] = 0;
    lens[Section::RowIds as usize] = (row_ids.len() * 4) as u64;

    // Layout: header, then each section aligned (slot pages to a page).
    let mut table = [(0u64, 0u64); SECTION_COUNT];
    let mut at = HEADER_LEN;
    for (i, len) in lens.iter().enumerate() {
        at = align_up(
            at,
            if i == Section::Slots as usize {
                SLOT_ALIGN
            } else {
                8
            },
        );
        table[i] = (at as u64, *len);
        at += *len as usize;
    }

    let mut header = [0u8; HEADER_LEN];
    header[0..8].copy_from_slice(&MAGIC);
    header[8..10].copy_from_slice(&VERSION.to_le_bytes());
    header[10..12].copy_from_slice(&(HEADER_LEN as u16).to_le_bytes());
    header[12..16].copy_from_slice(&(dims as u32).to_le_bytes());
    header[16] = match opts.metric {
        Metric::L2 => 0,
        Metric::Ip => 1,
        Metric::Cos => 2,
    };
    header[17] = match opts.encode {
        Encode::None => 0,
        Encode::Rabitq => 1,
    };
    header[18] = 1; // rot_algo: 1 = block-diagonal RHT
    header[19] = 0;
    header[20..28].copy_from_slice(&opts.seed.to_le_bytes());
    header[28..32].copy_from_slice(&(parts.len() as u32).to_le_bytes());
    header[32..36].copy_from_slice(&segment.to_le_bytes());
    // Bytes 136..160 are reserved; 136..140 name the **subtree base**: the DFS
    // key of this chunk's first partition. A whole-tree chunk writes 0 (its keys
    // are 0..partitions), and a subtree chunk written by a manifest-aware dump
    // names its base so the segment can resolve `global key -> (chunk, local)`.
    header[136..140].copy_from_slice(&sorted[0].to_le_bytes());
    for (i, (offset, len)) in table.iter().enumerate() {
        let at = SECTION_TABLE_AT + i * 16;
        header[at..at + 8].copy_from_slice(&offset.to_le_bytes());
        header[at + 8..at + 16].copy_from_slice(&len.to_le_bytes());
    }
    out.write_all(&header)
        .map_err(|e| SpannError::Corrupt(format!("write header: {e}")))?;
    let mut pos = HEADER_LEN;

    let mut crcs = [0u32; SECTION_COUNT];
    let mut buf: Vec<u8> = Vec::new();

    pad_to(out, &mut pos, table[Section::Meta as usize].0 as usize)?;
    // A: metadata. The per-partition offsets into B/C/D are running counters, so
    // this section is written exactly as the in-memory form built it.
    let mut crc = Crc32c::new();
    let mut centroids_off = 0usize;
    let mut children_off = 0usize;
    let mut entry = [0u8; META_ENTRY];
    for (i, part) in parts.iter().enumerate() {
        let slots_off = page_offsets[i];
        entry[0..4].copy_from_slice(&part.key.to_le_bytes());
        entry[4..6].copy_from_slice(&(part.level as u16).to_le_bytes());
        entry[6..8].copy_from_slice(&(part.members.len() as u16).to_le_bytes());
        entry[8..12].copy_from_slice(&part.radius.to_le_bytes());
        entry[12..16].copy_from_slice(&(centroids_off as u32).to_le_bytes());
        entry[16..20].copy_from_slice(&(slots_off as u32).to_le_bytes());
        entry[20..24].copy_from_slice(&(children_off as u32).to_le_bytes());
        entry[24..26].copy_from_slice(&(part.children.len() as u16).to_le_bytes());
        put(out, &mut crc, &mut pos, &entry)?;
        centroids_off += part.centroid.len() * 4;
        children_off += part.children.len();
    }
    crcs[Section::Meta as usize] = crc.finish();

    pad_to(out, &mut pos, table[Section::Centroids as usize].0 as usize)?;
    // B: centroids.
    let mut crc = Crc32c::new();
    for part in &parts {
        buf.clear();
        buf.reserve(part.centroid.len() * 4);
        for x in &part.centroid {
            buf.extend_from_slice(&x.to_le_bytes());
        }
        put(out, &mut crc, &mut pos, &buf)?;
    }
    crcs[Section::Centroids as usize] = crc.finish();

    pad_to(out, &mut pos, table[Section::Children as usize].0 as usize)?;
    // C: child keys.
    let mut crc = Crc32c::new();
    for part in &parts {
        buf.clear();
        buf.reserve(part.children.len() * 4);
        for child in &part.children {
            // A standalone chunk is closed under children: an edge to a
            // partition outside the set would make the reader resolve a key the
            // chunk does not hold. A delta leaves those edges dangling on
            // purpose - its chain resolves them.
            if closed && !member_keys.contains(child) {
                return Err(SpannError::InvalidOption(
                    "keys",
                    format!("child key {child} is not in the chunk's key set"),
                ));
            }
            buf.extend_from_slice(&child.to_le_bytes());
        }
        put(out, &mut crc, &mut pos, &buf)?;
    }
    crcs[Section::Children as usize] = crc.finish();

    pad_to(out, &mut pos, table[Section::Slots as usize].0 as usize)?;
    // D: one aligned slot page per partition. This is the bulk of the file, so
    // it goes out group by group rather than being assembled first.
    let mut crc = Crc32c::new();
    for (i, part) in parts.iter().enumerate() {
        let mut n = 0usize;
        for member in &part.members {
            // A leaf's member is a row; a branch's "members" are its child keys
            // (`build_subtree` builds it from the two centroids), so only a leaf
            // carries a row offset here. Both fit in 4 bytes, which is why the
            // column is `u32` for every level (§6.2).
            let word = if part.is_leaf() {
                segment_offset(member.id, segment)?
            } else {
                u32::try_from(member.id).map_err(|_| {
                    SpannError::Corrupt(format!("child key {:#x} does not fit in u32", member.id))
                })?
            };
            buf.clear();
            buf.extend_from_slice(&word.to_le_bytes());
            put(out, &mut crc, &mut pos, &buf)?;
            n += 4;
        }
        for group in 0..4 {
            buf.clear();
            buf.reserve(part.members.len() * 4);
            for member in &part.members {
                let x = match group {
                    0 => member.code.norm,
                    1 => member.code.raw_norm,
                    2 => member.code.sum,
                    _ => member.code.error,
                };
                buf.extend_from_slice(&x.to_le_bytes());
            }
            put(out, &mut crc, &mut pos, &buf)?;
            n += part.members.len() * 4;
        }
        for member in &part.members {
            debug_assert_eq!(member.code.bits.len(), code_bytes);
            put(out, &mut crc, &mut pos, &member.code.bits)?;
            n += member.code.bits.len();
        }
        // Pad up to the alignment so the next page starts aligned; the final
        // page ends the section and carries no padding.
        let pad = if i + 1 < parts.len() {
            align_up(n, SLOT_ALIGN) - n
        } else {
            0
        };
        if pad > 0 {
            const ZEROS: [u8; SLOT_ALIGN] = [0; SLOT_ALIGN];
            let mut left = pad;
            while left > 0 {
                let take = left.min(SLOT_ALIGN);
                put(out, &mut crc, &mut pos, &ZEROS[..take])?;
                left -= take;
            }
        }
    }
    crcs[Section::Slots as usize] = crc.finish();

    // E: full vectors, reserved (empty for rabitq).
    crcs[Section::Vectors as usize] = Crc32c::new().finish();

    pad_to(out, &mut pos, table[Section::RowIds as usize].0 as usize)?;
    // F: row ids, in blocks so a 50M-row file does not need 400 MB at once.
    let mut crc = Crc32c::new();
    for chunk in row_ids.chunks(4096) {
        buf.clear();
        buf.reserve(chunk.len() * 4);
        for id in chunk {
            buf.extend_from_slice(&segment_offset(*id, segment)?.to_le_bytes());
        }
        put(out, &mut crc, &mut pos, &buf)?;
    }
    crcs[Section::RowIds as usize] = crc.finish();

    let mut trailer = [0u8; TRAILER_LEN];
    trailer[..8].copy_from_slice(&TRAILER_MAGIC);
    for (i, crc) in crcs.iter().enumerate() {
        let at = 8 + i * 4;
        trailer[at..at + 4].copy_from_slice(&crc.to_le_bytes());
    }
    out.write_all(&trailer)
        .map_err(|e| SpannError::Corrupt(format!("write trailer: {e}")))?;
    Ok(())
}
