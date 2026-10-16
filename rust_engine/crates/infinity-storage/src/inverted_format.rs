//! C++-compatible on-disk layouts for the **full-text inverted index**
//! (RUST_REWRITE_PLAN.md §2.6.1, task #71).
//!
//! Frozen in `rust_engine/docs/CPP_FORMAT_SPEC.md` §9.3 from `file:line`
//! citations into `src/storage/invertedindex/**`. The three files are:
//!
//!   * `{base_name}.dic` — the term dictionary: a run of [`TermMeta`] records
//!     followed by the FST mapping `term -> u64 TermMeta offset`. The FST's
//!     position is recovered from the `u64` at `len - 12`.
//!   * `{base_name}.pos` — postings, one VInt-prefixed skip-list/list pair per
//!     term (see `column_index_iterator_impl.cpp:86-116`).
//!   * `{base_name}.len` — a bare `u32[row_cnt]` of per-document column lengths.
//!
//! `base_name` is `ft_{rowid:016x}`, or `ft_{rowid:016x}_{row_count:x}` once
//! chunks have been merged (see [`crate::cpp_format::fulltext_base_name`]).
//!
//! None of the three files carries a magic number, a version or a checksum.

use crate::cpp_format::FormatError;

/// `optionflag_t` bits that change how a [`TermMeta`] is encoded. The flag word
/// itself lives in the catalog's IndexBase JSON, **not** in the index files
/// (`index_defines.cppm:13-38`; `index_full_text.cppm:42,73`).
#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
pub struct TermMetaFlags {
    /// `of_term_frequency` — a `total_tf` field follows `doc_freq`.
    pub has_term_frequency: bool,
    /// `of_term_payload` — a raw `u32` payload follows the frequency fields.
    pub has_term_payload: bool,
}

/// One term's dictionary entry (`term_meta.cppm:44-49`), encoded by
/// `TermMetaDumper::Dump` (`term_meta_impl.cpp:92-104`).
#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
pub struct TermMeta {
    /// Document frequency (number of documents containing the term).
    pub doc_freq: u32,
    /// Sum of term frequencies across the documents.
    pub total_tf: u32,
    /// Raw term payload (`of_term_payload` only).
    pub payload: u32,
    /// Offset of this term's doc-list slice inside `.pos`.
    pub doc_start: u64,
    /// Offset of this term's position-list slice inside `.pos`.
    pub pos_start: u64,
    /// End offset of this term's slice inside `.pos`.
    pub pos_end: u64,
}

// ---------------------------------------------------------------------------
// LEB128 (C++ `WriteVInt`/`WriteVLong`, `vbyte_compressor.cppm:60-127`)
// ---------------------------------------------------------------------------

/// Appends `value` as an unsigned LEB128 varint (7 bits per byte, low bits
/// first, high bit = continuation).
pub fn write_vint(out: &mut Vec<u8>, mut value: u64) {
    loop {
        let byte = (value & 0x7f) as u8;
        value >>= 7;
        if value == 0 {
            out.push(byte);
            return;
        }
        out.push(byte | 0x80);
    }
}

/// Reads an unsigned LEB128 varint, returning `(value, bytes_consumed)`.
pub fn read_vint(buf: &[u8]) -> Result<(u64, usize), FormatError> {
    let mut value = 0u64;
    let mut shift = 0u32;
    for (i, byte) in buf.iter().enumerate() {
        // 10 groups of 7 bits fill a u64; a longer run is malformed.
        if shift >= 64 {
            return Err(FormatError::Truncated {
                need: i + 1,
                got: buf.len(),
            });
        }
        value |= u64::from(byte & 0x7f) << shift;
        if byte & 0x80 == 0 {
            return Ok((value, i + 1));
        }
        shift += 7;
    }
    Err(FormatError::Truncated {
        need: buf.len() + 1,
        got: buf.len(),
    })
}

// ---------------------------------------------------------------------------
// `.dic` — TermMeta records
// ---------------------------------------------------------------------------

/// Encodes one `TermMeta` exactly as C++ `TermMetaDumper::Dump` does.
pub fn encode_term_meta(meta: &TermMeta, flags: TermMetaFlags) -> Vec<u8> {
    let mut out = Vec::with_capacity(24);
    write_vint(&mut out, u64::from(meta.doc_freq));
    if flags.has_term_frequency {
        write_vint(&mut out, u64::from(meta.total_tf));
    }
    if flags.has_term_payload {
        // Raw little-endian `u32` (the C++ writer does not use WriteInt here).
        out.extend_from_slice(&meta.payload.to_le_bytes());
    }
    write_vint(&mut out, meta.doc_start);
    write_vint(&mut out, meta.pos_start);
    write_vint(&mut out, meta.pos_end);
    out
}

/// Decodes one `TermMeta`, returning it plus the number of bytes consumed.
///
/// Note the C++ reader sets `total_tf = doc_freq` when the term-frequency flag
/// is off (`term_meta_impl.cpp:55-76`), which is reproduced here.
pub fn decode_term_meta(
    buf: &[u8],
    flags: TermMetaFlags,
) -> Result<(TermMeta, usize), FormatError> {
    let (df, mut used) = read_vint(buf)?;
    let mut meta = TermMeta {
        doc_freq: df as u32,
        total_tf: df as u32,
        ..TermMeta::default()
    };
    if flags.has_term_frequency {
        let (ttf, n) = read_vint(&buf[used..])?;
        used += n;
        meta.total_tf = ttf as u32;
    }
    if flags.has_term_payload {
        if buf.len() < used + 4 {
            return Err(FormatError::Truncated {
                need: used + 4,
                got: buf.len(),
            });
        }
        meta.payload = u32::from_le_bytes(buf[used..used + 4].try_into().unwrap());
        used += 4;
    }
    let (doc_start, n) = read_vint(&buf[used..])?;
    used += n;
    let (pos_start, n) = read_vint(&buf[used..])?;
    used += n;
    let (pos_end, n) = read_vint(&buf[used..])?;
    used += n;
    meta.doc_start = doc_start;
    meta.pos_start = pos_start;
    meta.pos_end = pos_end;
    Ok((meta, used))
}

/// Encodes a whole `.dic` **minus the FST**: the concatenated TermMeta records.
///
/// Callers append the FST (and its 12-byte trailer) themselves once the
/// term-dictionary writer exists; this function only covers the record region,
/// which is the part whose bytes are fully frozen.
pub fn encode_term_meta_region(metas: &[TermMeta], flags: TermMetaFlags) -> Vec<u8> {
    let mut out = Vec::new();
    for m in metas {
        out.extend_from_slice(&encode_term_meta(m, flags));
    }
    out
}

/// Length of the FST region given the `u64` stored 12 bytes before the file end
/// (`dict_reader_impl.cpp:46-67`): `root_addr == 0` means an empty FST of 36
/// bytes, otherwise the FST ends at `root_addr + 21`.
pub fn fst_len_from_trailer(root_addr: u64) -> usize {
    if root_addr == 0 {
        36
    } else {
        root_addr as usize + 21
    }
}

// ---------------------------------------------------------------------------
// `.len` — per-document column lengths
// ---------------------------------------------------------------------------

/// Encodes `.len`: a bare little-endian `u32[row_cnt]`, no header
/// (`memory_indexer_impl.cpp:550-552`).
pub fn encode_column_lengths(lengths: &[u32]) -> Vec<u8> {
    let mut out = Vec::with_capacity(lengths.len() * 4);
    for l in lengths {
        out.extend_from_slice(&l.to_le_bytes());
    }
    out
}

/// Decodes `.len` (`column_length_io_impl.cpp:62-66`).
pub fn decode_column_lengths(buf: &[u8]) -> Result<Vec<u32>, FormatError> {
    if !buf.len().is_multiple_of(4) {
        return Err(FormatError::Truncated {
            need: (buf.len() / 4 + 1) * 4,
            got: buf.len(),
        });
    }
    Ok(buf
        .chunks_exact(4)
        .map(|c| u32::from_le_bytes(c.try_into().unwrap()))
        .collect())
}

// ---------------------------------------------------------------------------
// `.pos` — the per-term slice framing
// ---------------------------------------------------------------------------

/// The eight VInt-prefixed regions of one term's `.pos` slice
/// (`column_index_iterator_impl.cpp:86-116`). The skip-list and list bodies are
/// returned verbatim: their contents are FastPFor SIMD-BP128 blocks (u32
/// fields) and IndexLib NewPForDelta blocks (u16 fields), which are not yet
/// implemented here.
#[derive(Debug, Clone, PartialEq, Eq, Default)]
pub struct TermPostings {
    pub doc_skiplist: Vec<u8>,
    pub doc_list: Vec<u8>,
    pub pos_skiplist: Vec<u8>,
    pub pos_list: Vec<u8>,
}

impl TermPostings {
    /// Encodes the framing: `VInt doc_skip_len, VInt doc_len, doc_skip,
    /// doc_list, VInt pos_skip_len, VInt pos_len, pos_skip, pos_list`.
    pub fn encode(&self) -> Vec<u8> {
        let mut out = Vec::with_capacity(
            self.doc_skiplist.len()
                + self.doc_list.len()
                + self.pos_skiplist.len()
                + self.pos_list.len()
                + 20,
        );
        write_vint(&mut out, self.doc_skiplist.len() as u64);
        write_vint(&mut out, self.doc_list.len() as u64);
        out.extend_from_slice(&self.doc_skiplist);
        out.extend_from_slice(&self.doc_list);
        write_vint(&mut out, self.pos_skiplist.len() as u64);
        write_vint(&mut out, self.pos_list.len() as u64);
        out.extend_from_slice(&self.pos_skiplist);
        out.extend_from_slice(&self.pos_list);
        out
    }

    /// Decodes the framing, returning the structure plus bytes consumed.
    ///
    /// C++ writes **both lengths of a pair first**, then both bodies.
    pub fn decode(buf: &[u8]) -> Result<(Self, usize), FormatError> {
        let mut used = 0usize;
        let read_len = |buf: &[u8], used: &mut usize| -> Result<usize, FormatError> {
            let (len, n) = read_vint(&buf[*used..])?;
            *used += n;
            Ok(len as usize)
        };
        let take = |buf: &[u8], used: &mut usize, len: usize| -> Result<Vec<u8>, FormatError> {
            if buf.len() < *used + len {
                return Err(FormatError::Truncated {
                    need: *used + len,
                    got: buf.len(),
                });
            }
            let out = buf[*used..*used + len].to_vec();
            *used += len;
            Ok(out)
        };

        let doc_skip_len = read_len(buf, &mut used)?;
        let doc_len = read_len(buf, &mut used)?;
        let doc_skiplist = take(buf, &mut used, doc_skip_len)?;
        let doc_list = take(buf, &mut used, doc_len)?;
        let pos_skip_len = read_len(buf, &mut used)?;
        let pos_len = read_len(buf, &mut used)?;
        let pos_skiplist = take(buf, &mut used, pos_skip_len)?;
        let pos_list = take(buf, &mut used, pos_len)?;
        Ok((
            Self {
                doc_skiplist,
                doc_list,
                pos_skiplist,
                pos_list,
            },
            used,
        ))
    }
}

/// FST format version written by the C++ builder (`fst/node.cppm:40`).
pub const FST_VERSION: u64 = 3;

/// Bytes of the FST trailer: `u64 len_`, `u64 root_addr`, `u32 checksum`.
pub const FST_TRAILER_LEN: usize = 20;

/// The masked CRC-32/IEEE the C++ FST writer stores and `Fst::Verify` checks
/// (`fst/writer.cppm:80-83`; `fst.cppm:134-144`):
/// `((crc >> 15) | (crc << 17)) + 0xa282ead8`.
pub fn fst_checksum(bytes: &[u8]) -> u32 {
    crate::crc::crc32_ieee(bytes)
        .rotate_right(15)
        .wrapping_add(0xa282_ead8)
}

/// Appends the trailer the C++ FST writer puts at the end of an FST region.
///
/// `Fst::Verify` computes `CRC32IEEE(data_ptr_, data_len_ - 4)` over the whole
/// FST buffer, so the checksum covers the `len_` and `root_addr` fields too and
/// must be computed after they are appended (only the 4-byte checksum itself is
/// excluded).
pub fn append_fst_trailer(fst: &mut Vec<u8>, len: u64, root_addr: u64) {
    fst.extend_from_slice(&len.to_le_bytes());
    fst.extend_from_slice(&root_addr.to_le_bytes());
    let checksum = fst_checksum(fst);
    fst.extend_from_slice(&checksum.to_le_bytes());
}

/// Reads the FST trailer and verifies the checksum, returning `(len_, root_addr)`.
pub fn verify_fst(buf: &[u8]) -> Result<(u64, u64), FormatError> {
    if buf.len() < FST_TRAILER_LEN {
        return Err(FormatError::Truncated {
            need: FST_TRAILER_LEN,
            got: buf.len(),
        });
    }
    let at = buf.len() - FST_TRAILER_LEN;
    let len = u64::from_le_bytes(buf[at..at + 8].try_into().unwrap());
    let root_addr = u64::from_le_bytes(buf[at + 8..at + 16].try_into().unwrap());
    let expected = u32::from_le_bytes(buf[at + 16..at + 20].try_into().unwrap());
    let got = fst_checksum(&buf[..buf.len() - 4]);
    if got != expected {
        return Err(FormatError::Checksum { expected, got });
    }
    Ok((len, root_addr))
}

// ---------------------------------------------------------------------------
// `.dic` — TermMeta region + FST (`memory_indexer_impl.cpp:516-543`,
// `dict_reader_impl.cpp:46-90`)
// ---------------------------------------------------------------------------

/// The FST region starts with two `u64` header fields — `VERSION` then
/// `FstType` (`fst/node.cppm:40`; `fst.cppm:64-112`).
pub const FST_HEADER_LEN: usize = 16;

/// Assembles a `.dic`: the TermMeta records, then the FST region on the tail.
/// The C++ writes the records while building and `VirtualStore::Merge`s the FST
/// onto the end (`memory_indexer_impl.cpp:516-543`).
pub fn encode_dic(term_meta_region: &[u8], fst_region: &[u8]) -> Vec<u8> {
    let mut out = Vec::with_capacity(term_meta_region.len() + fst_region.len());
    out.extend_from_slice(term_meta_region);
    out.extend_from_slice(fst_region);
    out
}

/// Locates the FST region inside a `.dic` (`dict_reader_impl.cpp:46-67`):
/// `root_addr` is the `u64` 12 bytes before the end and `fst_len` follows
/// [`fst_len_from_trailer`]. Returns `(fst_start, fst_len, root_addr)`.
pub fn dic_fst_region(buf: &[u8]) -> Result<(usize, usize, u64), FormatError> {
    if buf.len() < 12 {
        return Err(FormatError::Truncated {
            need: 12,
            got: buf.len(),
        });
    }
    let at = buf.len() - 12;
    let root_addr = u64::from_le_bytes(buf[at..at + 8].try_into().unwrap());
    let fst_len = fst_len_from_trailer(root_addr);
    if fst_len > buf.len() {
        return Err(FormatError::Truncated {
            need: fst_len,
            got: buf.len(),
        });
    }
    Ok((buf.len() - fst_len, fst_len, root_addr))
}

/// The FST region of a `.dic` — the bytes a `Fst::Get`/iteration call reads.
pub fn dic_fst(buf: &[u8]) -> Result<&[u8], FormatError> {
    let (start, len, _) = dic_fst_region(buf)?;
    Ok(&buf[start..start + len])
}

/// Reads the FST region's `(VERSION, FstType)` header (`fst.cppm:64-112`).
pub fn fst_header(fst: &[u8]) -> Result<(u64, u64), FormatError> {
    if fst.len() < FST_HEADER_LEN {
        return Err(FormatError::Truncated {
            need: FST_HEADER_LEN,
            got: fst.len(),
        });
    }
    let version = u64::from_le_bytes(fst[0..8].try_into().unwrap());
    let fst_type = u64::from_le_bytes(fst[8..16].try_into().unwrap());
    Ok((version, fst_type))
}

/// Encodes the TermMeta region and returns each record's byte offset — the
/// `u64` value the FST maps a term to (`memory_indexer_impl.cpp:516-543`).
pub fn encode_term_meta_region_with_offsets(
    metas: &[TermMeta],
    flags: TermMetaFlags,
) -> (Vec<u8>, Vec<u64>) {
    let mut out = Vec::new();
    let mut offsets = Vec::with_capacity(metas.len());
    for m in metas {
        offsets.push(out.len() as u64);
        out.extend_from_slice(&encode_term_meta(m, flags));
    }
    (out, offsets)
}

/// Parses the TermMeta at absolute byte `offset` inside a `.dic`, which is what
/// an FST lookup yields (`dict_reader_impl.cpp:80-90`).
pub fn decode_dic_meta_at(
    buf: &[u8],
    offset: u64,
    flags: TermMetaFlags,
) -> Result<TermMeta, FormatError> {
    let off = usize::try_from(offset).map_err(|_| FormatError::Truncated {
        need: offset as usize,
        got: buf.len(),
    })?;
    if off > buf.len() {
        return Err(FormatError::Truncated {
            need: off,
            got: buf.len(),
        });
    }
    let (meta, _) = decode_term_meta(&buf[off..], flags)?;
    Ok(meta)
}

// ---------------------------------------------------------------------------
// FST byte packing (`fst/bytes.cppm`, `fst/node.cppm`)
// ---------------------------------------------------------------------------
//
// These are the integer-packing primitives the FST node state machine builds
// on. They are self-contained and fully specified, unlike the node encoders
// themselves (which the spec still marks as not frozen).

/// The FST sentinel for "no address" (`fst/node.cppm:43`).
pub const FST_EMPTY_ADDRESS: u64 = 0;

/// `PackSize`: the smallest number of bytes that can encode `n`
/// (`fst/bytes.cppm:107-128`).
pub fn fst_pack_size(n: u64) -> u8 {
    if n < 1 << 8 {
        1
    } else if n < 1 << 16 {
        2
    } else if n < 1 << 24 {
        3
    } else if n < 1 << 32 {
        4
    } else if n < 1 << 40 {
        5
    } else if n < 1 << 48 {
        6
    } else if n < 1 << 56 {
        7
    } else {
        8
    }
}

/// `PackUintIn`: append `n` as exactly `nbytes` little-endian bytes
/// (`fst/bytes.cppm:130-141`). `nbytes` must be in `1..=8`.
pub fn fst_pack_uint_in(out: &mut Vec<u8>, n: u64, nbytes: u8) {
    debug_assert!((1..=8).contains(&nbytes));
    out.extend_from_slice(&n.to_le_bytes()[..nbytes as usize]);
}

/// `PackUint`: append `n` in the smallest number of bytes, returning how many
/// were written (`fst/bytes.cppm:143-150`).
pub fn fst_pack_uint(out: &mut Vec<u8>, n: u64) -> u8 {
    let nbytes = fst_pack_size(n);
    fst_pack_uint_in(out, n, nbytes);
    nbytes
}

/// `UnpackUint`: read a little-endian integer of `nbytes` bytes
/// (`fst/bytes.cppm:152-...`). `nbytes` must be in `1..=8`.
pub fn fst_unpack_uint(buf: &[u8], nbytes: u8) -> Result<u64, FormatError> {
    let n = nbytes as usize;
    if !(1..=8).contains(&n) {
        return Err(FormatError::Truncated { need: n, got: 0 });
    }
    if buf.len() < n {
        return Err(FormatError::Truncated {
            need: n,
            got: buf.len(),
        });
    }
    let mut bytes = [0u8; 8];
    bytes[..n].copy_from_slice(&buf[..n]);
    Ok(u64::from_le_bytes(bytes))
}

/// `State::PackDeltaSize`: bytes needed to store `node_addr - trans_addr`
/// (`fst/node.cppm:319-327`). An empty transition address packs as the
/// sentinel `0`.
pub fn fst_pack_delta_size(node_addr: u64, trans_addr: u64) -> u8 {
    let delta = if trans_addr == FST_EMPTY_ADDRESS {
        FST_EMPTY_ADDRESS
    } else {
        node_addr.wrapping_sub(trans_addr)
    };
    fst_pack_size(delta)
}

/// `State::PackDelta` (`fst/node.cppm:303-307`): append the relative address
/// `node_addr - trans_addr`, returning the number of bytes written.
pub fn fst_pack_delta(out: &mut Vec<u8>, node_addr: u64, trans_addr: u64) -> u8 {
    let nbytes = fst_pack_delta_size(node_addr, trans_addr);
    let delta = if trans_addr == FST_EMPTY_ADDRESS {
        FST_EMPTY_ADDRESS
    } else {
        node_addr.wrapping_sub(trans_addr)
    };
    fst_pack_uint_in(out, delta, nbytes);
    nbytes
}

/// `State::UnpackDelta` (`fst/node.cppm:329-337`): recover an absolute address
/// from a packed relative one. The sentinel `0` maps back to `EMPTY_ADDRESS`.
pub fn fst_unpack_delta(buf: &[u8], nbytes: u8, node_addr: u64) -> Result<u64, FormatError> {
    let delta = fst_unpack_uint(buf, nbytes)?;
    if delta == FST_EMPTY_ADDRESS {
        Ok(FST_EMPTY_ADDRESS)
    } else {
        Ok(node_addr.wrapping_sub(delta))
    }
}

// ---------------------------------------------------------------------------
// FST common-input tables (`fst/common_inputs.cppm`)
// ---------------------------------------------------------------------------
//
// `COMMON_INPUTS` ranks the bytes by frequency across real dictionaries; the
// node encoders use it to store a transition's input in six bits when the byte
// is common enough. Both arrays are a permutation of 0..=255 and
// `FST_COMMON_INPUTS_INV[FST_COMMON_INPUTS[b]] == b` holds for every byte
// (checked in a test).

/// `COMMON_INPUTS[256]` (`fst/common_inputs.cppm:20-278`): byte -> rank.
pub const FST_COMMON_INPUTS: [u8; 256] = [
    84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
    100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115,
    116, 80, 117, 118, 79, 39, 30, 81, 75, 74, 82, 57, 66, 16, 12, 2,
    19, 20, 21, 27, 32, 29, 35, 36, 37, 34, 24, 73, 119, 23, 120, 40,
    83, 44, 48, 42, 43, 49, 46, 62, 61, 47, 69, 68, 58, 56, 55, 59,
    51, 72, 54, 45, 52, 64, 65, 63, 71, 67, 70, 77, 121, 78, 122, 31,
    123, 4, 25, 9, 17, 1, 26, 22, 13, 7, 50, 38, 14, 15, 10, 3,
    8, 60, 6, 5, 0, 18, 33, 11, 41, 28, 53, 124, 125, 126, 76, 127,
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
];

/// `COMMON_INPUTS_INV[256]` (`fst/common_inputs.cppm:280-291`): rank-1 -> byte.
pub const FST_COMMON_INPUTS_INV: [u8; 256] = [
    116, 101, 47, 111, 97, 115, 114, 105, 112, 99, 110, 119, 46, 104, 108, 109,
    45, 100, 117, 48, 49, 50, 103, 61, 58, 98, 102, 51, 121, 53, 38, 95,
    52, 118, 57, 54, 55, 56, 107, 37, 63, 120, 67, 68, 65, 83, 70, 73,
    66, 69, 106, 80, 84, 122, 82, 78, 77, 43, 76, 79, 113, 72, 71, 87,
    85, 86, 44, 89, 75, 74, 90, 88, 81, 59, 41, 40, 126, 91, 93, 36,
    33, 39, 42, 64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
    12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
    28, 29, 30, 31, 32, 34, 35, 60, 62, 92, 94, 96, 123, 124, 125, 127,
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
];

/// `State::CommonIdx` (`fst/node.cppm:282-290`): the six-bit index for `input`,
/// or `0` when its rank exceeds `max` (i.e. the input must be stored verbatim).
pub fn fst_common_idx(input: u8, max: u8) -> u8 {
    let val = FST_COMMON_INPUTS[input as usize].wrapping_add(1);
    if val > max {
        0
    } else {
        val
    }
}

/// `State::CommonInput` (`fst/node.cppm:292-301`): the byte for a stored index,
/// or `None` for the `0` sentinel meaning "input is stored verbatim".
pub fn fst_common_input(idx: u8) -> Option<u8> {
    if idx == 0 {
        None
    } else {
        Some(FST_COMMON_INPUTS_INV[idx as usize - 1])
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn fst_trailer_roundtrip_and_verification() {
        // A toy FST body: the version, the FstType and some node bytes.
        let mut fst = Vec::new();
        fst.extend_from_slice(&FST_VERSION.to_le_bytes());
        fst.extend_from_slice(&0u64.to_le_bytes());
        fst.extend_from_slice(&[0xab; 7]);
        append_fst_trailer(&mut fst, 42, 0); // 42 keys, root at offset 0
        assert_eq!(fst.len(), 8 + 8 + 7 + FST_TRAILER_LEN);

        let (len, root_addr) = verify_fst(&fst).unwrap();
        assert_eq!((len, root_addr), (42, 0));
        // The trailer fields sit at len-20 / len-12 / len-4.
        let at = fst.len() - FST_TRAILER_LEN;
        assert_eq!(&fst[at..at + 8], &42u64.to_le_bytes());
        assert_eq!(&fst[at + 8..at + 16], &0u64.to_le_bytes());
        // `fst_len_from_trailer` recovers the whole region from the root addr.
        assert_eq!(fst_len_from_trailer(root_addr), 36);

        // Any corruption before the checksum fails verification.
        let mut bad = fst.clone();
        bad[16] ^= 0xff;
        assert!(matches!(
            verify_fst(&bad),
            Err(FormatError::Checksum { .. })
        ));
        assert!(verify_fst(&fst[..8]).is_err());
    }

    #[test]
    fn dic_roundtrip_locates_the_fst_and_reads_meta_by_offset() {
        let flags = TermMetaFlags {
            has_term_frequency: true,
            has_term_payload: true,
        };
        let metas = [
            TermMeta {
                doc_freq: 2,
                total_tf: 3,
                payload: 0x11223344,
                doc_start: 0,
                pos_start: 8,
                pos_end: 24,
            },
            TermMeta {
                doc_freq: 5,
                total_tf: 9,
                payload: 0x55667788,
                doc_start: 24,
                pos_start: 40,
                pos_end: 96,
            },
        ];
        let (region, offsets) = encode_term_meta_region_with_offsets(&metas, flags);
        assert_eq!(offsets, vec![0, encode_term_meta(&metas[0], flags).len() as u64]);

        // A toy FST region: header, some node bytes, then the trailer. The
        // reader derives the region length from `root_addr` as `root_addr + 21`,
        // so pick the root addr the length implies.
        let mut fst = Vec::new();
        fst.extend_from_slice(&FST_VERSION.to_le_bytes());
        fst.extend_from_slice(&0u64.to_le_bytes()); // FstType
        fst.extend_from_slice(&[0xcd; 13]); // node data
        let root_addr = (FST_HEADER_LEN + 13 + FST_TRAILER_LEN - 21) as u64;
        append_fst_trailer(&mut fst, metas.len() as u64, root_addr);

        let dic = encode_dic(&region, &fst);
        // The FST sits at the tail; the trailing 12 bytes give root_addr.
        let (start, len, got_root) = dic_fst_region(&dic).unwrap();
        assert_eq!(start, region.len());
        assert_eq!(len, fst.len());
        assert_eq!(got_root, root_addr);
        assert_eq!(dic_fst(&dic).unwrap(), fst.as_slice());
        let (version, fst_type) = fst_header(dic_fst(&dic).unwrap()).unwrap();
        assert_eq!((version, fst_type), (FST_VERSION, 0));
        // The FST's trailing trailer is verifiable.
        assert_eq!(verify_fst(dic_fst(&dic).unwrap()).unwrap(), (2, root_addr));

        // Each recorded offset reads back its own record.
        for (i, off) in offsets.iter().enumerate() {
            assert_eq!(decode_dic_meta_at(&dic, *off, flags).unwrap(), metas[i]);
        }
        // An out-of-range offset is reported, not panicked.
        assert!(decode_dic_meta_at(&dic, dic.len() as u64 + 1, flags).is_err());
        assert!(dic_fst_region(&dic[..8]).is_err());
    }

    #[test]
    fn fst_pack_size_picks_the_smallest_width() {
        // Boundaries from `fst/bytes.cppm:108-128`.
        let cases: &[(u64, u8)] = &[
            (0, 1),
            (0xff, 1),
            (0x100, 2),
            (0xffff, 2),
            (0x1_0000, 3),
            (0x00ff_ffff, 3),
            (0x100_0000, 4),
            (0xffff_ffff, 4),
            (0x1_0000_0000, 5),
            (0xff_ffff_ffff, 5),
            (0x100_0000_0000, 6),
            (0xffff_ffff_ffff, 6),
            (0x1_0000_0000_0000, 7),
            (0x00ff_ffff_ffff_ffff, 7),
            (0x100_0000_0000_0000, 8),
            (u64::MAX, 8),
        ];
        for (n, w) in cases {
            assert_eq!(fst_pack_size(*n), *w, "pack_size({n:#x})");
        }
    }

    #[test]
    fn fst_pack_uint_is_little_endian_and_round_trips() {
        // `PackUint` writes the low `PackSize(n)` LE bytes.
        let mut out = Vec::new();
        assert_eq!(fst_pack_uint(&mut out, 0x0102), 2);
        assert_eq!(out, vec![0x02, 0x01]);
        assert_eq!(fst_unpack_uint(&out, 2).unwrap(), 0x0102);

        // `PackUintIn` honours an explicit (larger) width.
        let mut wide = Vec::new();
        fst_pack_uint_in(&mut wide, 0x0102, 4);
        assert_eq!(wide, vec![0x02, 0x01, 0x00, 0x00]);
        assert_eq!(fst_unpack_uint(&wide, 4).unwrap(), 0x0102);

        // A full-width value round-trips.
        let mut full = Vec::new();
        assert_eq!(fst_pack_uint(&mut full, u64::MAX), 8);
        assert_eq!(fst_unpack_uint(&full, 8).unwrap(), u64::MAX);

        // Truncated input and an out-of-range width are reported.
        assert!(fst_unpack_uint(&[0x01], 2).is_err());
        assert!(fst_unpack_uint(&[], 0).is_err());
    }

    #[test]
    fn fst_delta_is_node_minus_transition() {
        // A forward transition packs `node_addr - trans_addr`.
        let mut out = Vec::new();
        let n = fst_pack_delta(&mut out, 1000, 940);
        assert_eq!(n, 1);
        assert_eq!(out, vec![60]);
        assert_eq!(fst_unpack_delta(&out, n, 1000).unwrap(), 940);

        // The empty-address sentinel round-trips to `EMPTY_ADDRESS`.
        let mut empty = Vec::new();
        let n = fst_pack_delta(&mut empty, 1000, FST_EMPTY_ADDRESS);
        assert_eq!(empty, vec![0]);
        assert_eq!(
            fst_unpack_delta(&empty, n, 1000).unwrap(),
            FST_EMPTY_ADDRESS
        );

        // A larger delta widens the encoding.
        let mut big = Vec::new();
        let n = fst_pack_delta(&mut big, 100_000, 1);
        assert_eq!(fst_pack_delta_size(100_000, 1), 3);
        assert_eq!(n, 3);
        assert_eq!(fst_unpack_delta(&big, n, 100_000).unwrap(), 1);
    }

    #[test]
    fn common_input_tables_are_inverses() {
        // Both tables are permutations of 0..=255, and `INV[A[b]] == b` holds,
        // which is the relation `CommonIdx`/`CommonInput` rely on.
        let mut seen = [false; 256];
        for b in 0..256 {
            assert_eq!(
                FST_COMMON_INPUTS_INV[FST_COMMON_INPUTS[b] as usize],
                b as u8,
                "INV[A[{b}]] must be {b}"
            );
            assert!(!seen[b], "COMMON_INPUTS has a duplicate at {b}");
            seen[b] = true;
        }
        let mut seen_inv = [false; 256];
        for v in FST_COMMON_INPUTS_INV {
            assert!(!seen_inv[v as usize]);
            seen_inv[v as usize] = true;
        }
    }

    #[test]
    fn common_idx_and_input_round_trip_within_the_max() {
        // The most common byte ('t') ranks first, so `CommonIdx('t', 63) == 1`.
        assert_eq!(FST_COMMON_INPUTS[b't' as usize], 0);
        assert_eq!(fst_common_idx(b't', 0b11_1111), 1);
        assert_eq!(fst_common_input(1), Some(b't'));
        // 'e' ranks second, 'p' ninth.
        assert_eq!(fst_common_idx(b'e', 63), 2);
        assert_eq!(fst_common_input(2), Some(b'e'));
        assert_eq!(fst_common_input(9), Some(b'p'));
        // The 0 index means "no common input" (stored verbatim).
        assert_eq!(fst_common_input(0), None);
        // A rank above `max` falls back to 0.
        assert_eq!(fst_common_idx(b't', 0), 0);
        assert_eq!(fst_common_idx(b'z', 3), 0);
        // Every byte round-trips through its own rank when max is 255 - except
        // the rank-255 byte, whose `(rank + 1) % 256` wraps to the 0 sentinel
        // and is therefore always stored verbatim (`fst/node.cppm:284`).
        for b in 0..=255u8 {
            let idx = fst_common_idx(b, 255);
            if FST_COMMON_INPUTS[b as usize] == 255 {
                assert_eq!(idx, 0);
                continue;
            }
            assert_ne!(idx, 0);
            assert_eq!(fst_common_input(idx), Some(b));
        }
    }

    #[test]
    fn masked_checksum_matches_the_cpp_formula() {
        // The mask is a 15-bit rotate plus the Snappy constant.
        let crc = crate::crc::crc32_ieee(b"abc");
        assert_eq!(
            fst_checksum(b"abc"),
            crc.rotate_right(15).wrapping_add(0xa282_ead8)
        );
        // Distinct inputs give distinct masked checksums.
        assert_ne!(fst_checksum(b"abc"), fst_checksum(b"abd"));
    }

    #[test]
    fn vint_matches_leb128() {
        let cases: &[(u64, &[u8])] = &[
            (0, &[0x00]),
            (1, &[0x01]),
            (127, &[0x7f]),
            (128, &[0x80, 0x01]),
            (300, &[0xac, 0x02]),
            (16_384, &[0x80, 0x80, 0x01]),
        ];
        for (value, bytes) in cases {
            let mut out = Vec::new();
            write_vint(&mut out, *value);
            assert_eq!(&out, bytes, "encode {value}");
            let (got, n) = read_vint(bytes).unwrap();
            assert_eq!((got, n), (*value, bytes.len()), "decode {value}");
        }
        // A u64::MAX round-trips in the maximum 10 bytes.
        let mut out = Vec::new();
        write_vint(&mut out, u64::MAX);
        assert_eq!(out.len(), 10);
        assert_eq!(read_vint(&out).unwrap(), (u64::MAX, 10));
    }

    #[test]
    fn vint_rejects_truncated_input() {
        assert!(read_vint(&[0x80]).is_err());
        assert!(read_vint(&[]).is_err());
    }

    #[test]
    fn term_meta_roundtrip_with_and_without_optional_fields() {
        let meta = TermMeta {
            doc_freq: 300,
            total_tf: 5000,
            payload: 0xdead_beef,
            doc_start: 12,
            pos_start: 1_000,
            pos_end: 2_000,
        };
        for flags in [
            TermMetaFlags::default(),
            TermMetaFlags {
                has_term_frequency: true,
                has_term_payload: false,
            },
            TermMetaFlags {
                has_term_frequency: true,
                has_term_payload: true,
            },
        ] {
            let bytes = encode_term_meta(&meta, flags);
            let (got, n) = decode_term_meta(&bytes, flags).unwrap();
            assert_eq!(n, bytes.len(), "flags {flags:?}");
            assert_eq!(got.doc_start, meta.doc_start);
            assert_eq!(got.pos_start, meta.pos_start);
            assert_eq!(got.pos_end, meta.pos_end);
            assert_eq!(got.doc_freq, meta.doc_freq);
            if flags.has_term_frequency {
                assert_eq!(got.total_tf, meta.total_tf);
            } else {
                // Without the tf flag the C++ loader falls back to `df`.
                assert_eq!(got.total_tf, meta.doc_freq);
            }
            if flags.has_term_payload {
                assert_eq!(got.payload, meta.payload);
            }
        }
    }

    #[test]
    fn term_meta_payload_is_raw_le_u32() {
        let meta = TermMeta {
            doc_freq: 1,
            payload: 0x0102_0304,
            ..TermMeta::default()
        };
        let bytes = encode_term_meta(
            &meta,
            TermMetaFlags {
                has_term_frequency: false,
                has_term_payload: true,
            },
        );
        // VInt(1) = 0x01, then the raw payload little-endian.
        assert_eq!(&bytes[0..1], &[0x01]);
        assert_eq!(&bytes[1..5], &[0x04, 0x03, 0x02, 0x01]);
    }

    #[test]
    fn term_meta_region_is_a_concatenation() {
        let metas = [
            TermMeta::default(),
            TermMeta {
                doc_freq: 7,
                ..Default::default()
            },
        ];
        let region = encode_term_meta_region(&metas, TermMetaFlags::default());
        let (first, n) = decode_term_meta(&region, TermMetaFlags::default()).unwrap();
        assert_eq!(first.doc_freq, 0);
        let (second, _) = decode_term_meta(&region[n..], TermMetaFlags::default()).unwrap();
        assert_eq!(second.doc_freq, 7);
    }

    #[test]
    fn fst_trailer_length() {
        assert_eq!(fst_len_from_trailer(0), 36); // empty FST
        assert_eq!(fst_len_from_trailer(15), 36);
        assert_eq!(fst_len_from_trailer(100), 121);
    }

    #[test]
    fn column_lengths_are_bare_u32_array() {
        let lengths = [3u32, 0, 17];
        let bytes = encode_column_lengths(&lengths);
        assert_eq!(bytes.len(), 12);
        assert_eq!(&bytes[0..4], &3u32.to_le_bytes());
        assert_eq!(&bytes[4..8], &0u32.to_le_bytes());
        assert_eq!(decode_column_lengths(&bytes).unwrap(), lengths.to_vec());
        // A trailing partial element is a corruption, not a shorter array.
        assert!(decode_column_lengths(&bytes[..11]).is_err());
    }

    #[test]
    fn postings_framing_roundtrip() {
        let p = TermPostings {
            doc_skiplist: vec![1, 2, 3],
            doc_list: vec![4, 5],
            pos_skiplist: vec![6],
            pos_list: vec![7, 8, 9, 10],
        };
        let bytes = p.encode();
        // 4 length prefixes (1 byte each here) + 10 body bytes.
        assert_eq!(bytes.len(), 4 + 10);
        assert_eq!(&bytes[0..2], &[3, 2]); // doc skip-list / doc-list lengths
        assert_eq!(&bytes[2..5], &[1, 2, 3]); // doc skip list
        assert_eq!(&bytes[5..7], &[4, 5]); // doc list
        assert_eq!(&bytes[7..9], &[1, 4]); // pos skip-list / pos-list lengths
        assert_eq!(&bytes[9..10], &[6]); // pos skip list
        assert_eq!(&bytes[10..14], &[7, 8, 9, 10]); // pos list
        let (got, n) = TermPostings::decode(&bytes).unwrap();
        assert_eq!(n, bytes.len());
        assert_eq!(got, p);
    }

    #[test]
    fn postings_framing_rejects_truncation() {
        let p = TermPostings {
            doc_list: vec![1, 2, 3],
            ..Default::default()
        };
        let bytes = p.encode();
        assert!(TermPostings::decode(&bytes[..bytes.len() - 1]).is_err());
    }
}
