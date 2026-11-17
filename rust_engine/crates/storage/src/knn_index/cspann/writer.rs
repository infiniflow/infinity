//! Chunk serialisation (`docs/CSPANN_DESIGN.md` §6).
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

use index::cspann::quantize::code_bytes;
use index::cspann::{CspannIndex, Encode};
use index::vector::Metric;

use super::{
    Crc32c, CspannError, Section, HEADER_LEN, MAGIC, META_ENTRY, SECTION_COUNT, SLOT_ALIGN,
    TRAILER_LEN, TRAILER_MAGIC,
};

/// Rounds `value` up to `align`.
pub(crate) const fn align_up(value: usize, align: usize) -> usize {
    value.div_ceil(align) * align
}

/// The chunk for `index`, in memory.
pub fn write(index: &CspannIndex) -> Result<Vec<u8>, CspannError> {
    let mut out: Vec<u8> = Vec::new();
    sections(index, &mut out)?;
    Ok(out)
}

/// The chunk for `index`, streamed into `path`; returns its length in bytes.
pub fn write_to(
    index: &CspannIndex,
    path: impl AsRef<std::path::Path>,
) -> Result<u64, CspannError> {
    let file = std::fs::File::create(path)
        .map_err(|e| CspannError::Corrupt(format!("create chunk: {e}")))?;
    let mut out = std::io::BufWriter::with_capacity(1 << 20, file);
    sections(index, &mut out)?;
    let file = out
        .into_inner()
        .map_err(|e| CspannError::Corrupt(format!("flush chunk: {e}")))?;
    file.metadata()
        .map(|m| m.len())
        .map_err(|e| CspannError::Corrupt(format!("stat chunk: {e}")))
}

fn put<W: Write>(
    out: &mut W,
    crc: &mut Crc32c,
    pos: &mut usize,
    bytes: &[u8],
) -> Result<(), CspannError> {
    out.write_all(bytes)
        .map_err(|e| CspannError::Corrupt(format!("write chunk: {e}")))?;
    crc.update(bytes);
    *pos += bytes.len();
    Ok(())
}

/// Emits the alignment padding that puts the next section at `target`.
///
/// The padding sits *between* sections, so it belongs to no checksum - but the
/// reader addresses every section by the table, which means the bytes have to be
/// there or every later section reads shifted.
fn pad_to<W: Write>(out: &mut W, pos: &mut usize, target: usize) -> Result<(), CspannError> {
    const ZEROS: [u8; 64] = [0; 64];
    while *pos < target {
        let take = (target - *pos).min(ZEROS.len());
        out.write_all(&ZEROS[..take])
            .map_err(|e| CspannError::Corrupt(format!("write padding: {e}")))?;
        *pos += take;
    }
    Ok(())
}

fn sections<W: Write>(index: &CspannIndex, out: &mut W) -> Result<(), CspannError> {
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
    let row_ids = index.row_ids();
    let code_bytes = code_bytes(dims);
    // One slot is id + norm + raw_norm + sum + error + code.
    let slot_bytes = 8 + 4 * 4 + code_bytes;
    // Where each partition's page starts inside section D, and how many bytes of
    // it are payload. A page is padded *up to* the alignment so the next one
    // starts aligned, which means the last page carries no trailing padding -
    // the in-memory writer never emitted it, and the checksum covers the bytes.
    let mut page_offsets = Vec::with_capacity(parts.len());
    let mut page_data = Vec::with_capacity(parts.len());
    let mut running = 0usize;
    for part in parts {
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
    lens[Section::RowIds as usize] = (row_ids.len() * 8) as u64;

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
    header[8..10].copy_from_slice(&1u16.to_le_bytes());
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
    for (i, (offset, len)) in table.iter().enumerate() {
        let at = 32 + i * 16;
        header[at..at + 8].copy_from_slice(&offset.to_le_bytes());
        header[at + 8..at + 16].copy_from_slice(&len.to_le_bytes());
    }
    out.write_all(&header)
        .map_err(|e| CspannError::Corrupt(format!("write header: {e}")))?;
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
    for part in parts {
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
    for part in parts {
        buf.clear();
        buf.reserve(part.children.len() * 4);
        for child in &part.children {
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
            buf.clear();
            buf.extend_from_slice(&member.id.to_le_bytes());
            put(out, &mut crc, &mut pos, &buf)?;
            n += 8;
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
        buf.reserve(chunk.len() * 8);
        for id in chunk {
            buf.extend_from_slice(&id.to_le_bytes());
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
        .map_err(|e| CspannError::Corrupt(format!("write trailer: {e}")))?;
    Ok(())
}
