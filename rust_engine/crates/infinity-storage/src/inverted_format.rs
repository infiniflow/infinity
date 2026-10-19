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

pub mod doc_list;
pub mod position_list;
pub mod posting_slice;
pub mod skiplist;

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

/// The full-text index file-family suffixes (`index_defines.cppm:55-58`).
///
/// A chunk's files are `{index_dir}/{base_name}{suffix}`, where `index_dir` is
/// [`crate::cpp_format::segment_index_dir`] and the suffixes are these.
pub const FULLTEXT_DICT_SUFFIX: &str = ".dic";
pub const FULLTEXT_POSTING_SUFFIX: &str = ".pos";
pub const FULLTEXT_SPILL_SUFFIX: &str = ".spill";
pub const FULLTEXT_LENGTH_SUFFIX: &str = ".len";

/// The persistent members of a full-text chunk (`.spill` is transient, removed
/// after the chunk is built).
pub const FULLTEXT_CHUNK_FILES: [&str; 3] = [
    FULLTEXT_DICT_SUFFIX,
    FULLTEXT_POSTING_SUFFIX,
    FULLTEXT_LENGTH_SUFFIX,
];

/// `{index_dir}/{base_name}{suffix}` — the full-text file path shape
/// (`disk_segment_reader_impl.cpp:51-57`).
pub fn fulltext_chunk_file(index_dir: &str, base_name: &str, suffix: &str) -> String {
    format!("{index_dir}/{base_name}{suffix}")
}

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
    84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105,
    106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 80, 117, 118, 79, 39, 30, 81, 75, 74,
    82, 57, 66, 16, 12, 2, 19, 20, 21, 27, 32, 29, 35, 36, 37, 34, 24, 73, 119, 23, 120, 40, 83,
    44, 48, 42, 43, 49, 46, 62, 61, 47, 69, 68, 58, 56, 55, 59, 51, 72, 54, 45, 52, 64, 65, 63, 71,
    67, 70, 77, 121, 78, 122, 31, 123, 4, 25, 9, 17, 1, 26, 22, 13, 7, 50, 38, 14, 15, 10, 3, 8,
    60, 6, 5, 0, 18, 33, 11, 41, 28, 53, 124, 125, 126, 76, 127, 128, 129, 130, 131, 132, 133, 134,
    135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153,
    154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172,
    173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210,
    211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229,
    230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248,
    249, 250, 251, 252, 253, 254, 255,
];

/// `COMMON_INPUTS_INV[256]` (`fst/common_inputs.cppm:280-291`): rank-1 -> byte.
pub const FST_COMMON_INPUTS_INV: [u8; 256] = [
    116, 101, 47, 111, 97, 115, 114, 105, 112, 99, 110, 119, 46, 104, 108, 109, 45, 100, 117, 48,
    49, 50, 103, 61, 58, 98, 102, 51, 121, 53, 38, 95, 52, 118, 57, 54, 55, 56, 107, 37, 63, 120,
    67, 68, 65, 83, 70, 73, 66, 69, 106, 80, 84, 122, 82, 78, 77, 43, 76, 79, 113, 72, 71, 87, 85,
    86, 44, 89, 75, 74, 90, 88, 81, 59, 41, 40, 126, 91, 93, 36, 33, 39, 42, 64, 0, 1, 2, 3, 4, 5,
    6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
    31, 32, 34, 35, 60, 62, 92, 94, 96, 123, 124, 125, 127, 128, 129, 130, 131, 132, 133, 134, 135,
    136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154,
    155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173,
    174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192,
    193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211,
    212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230,
    231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249,
    250, 251, 252, 253, 254, 255,
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

// ---------------------------------------------------------------------------
// FST node decoding (`fst/node.cppm`), verified against real C++-built FSTs
// ---------------------------------------------------------------------------

/// `TRANS_INDEX_THRESHOLD` (`fst/node.cppm:52`): a node with more transitions
/// than this adds a 256-byte byte -> index table.
pub const FST_TRANS_INDEX_THRESHOLD: usize = 32;

/// The `root_addr` from an FST's trailer (`fst.cppm:77`), 12 bytes before the
/// end. Does not verify the checksum.
pub fn fst_root_addr(fst: &[u8]) -> Result<u64, FormatError> {
    if fst.len() < 12 {
        return Err(FormatError::Truncated {
            need: 12,
            got: fst.len(),
        });
    }
    let at = fst.len() - 12;
    Ok(u64::from_le_bytes(fst[at..at + 8].try_into().unwrap()))
}

/// `PackSizes` (`fst/node.cppm:110-127`): the transition/output widths packed
/// into one byte (transition size in the high nibble).
#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
struct FstPackSizes {
    tsize: usize,
    osize: usize,
}

impl FstPackSizes {
    fn from_byte(b: u8) -> Self {
        Self {
            tsize: (b >> 4) as usize,
            osize: (b & 0x0f) as usize,
        }
    }
}

/// One decoded FST node (`fst/node.cppm:203-253`, `:755-795`).
struct FstNode<'a> {
    data: &'a [u8],
    /// Address of the state byte (`start_`).
    start: usize,
    /// The end of the node's data (`end_`), used as the delta base.
    end: usize,
    ntrans: usize,
    state: u8,
    sizes: FstPackSizes,
    is_final: bool,
    final_output: u64,
}

fn fst_err(need: usize, got: usize) -> FormatError {
    FormatError::Truncated { need, got }
}

impl<'a> FstNode<'a> {
    /// `State::NtransLen` (`fst/node.cppm:615-621`).
    fn ntrans_len(state: u8) -> usize {
        if state & 0x3f == 0 {
            1
        } else {
            0
        }
    }

    /// `StateOneTransNext`/`StateOneTrans` `InputLen` (`fst/node.cppm:361-366`,
    /// `:438-443`).
    fn input_len(state: u8) -> usize {
        if state & 0x3f == 0 {
            1
        } else {
            0
        }
    }

    /// The `Node::Node(addr, data_ptr)` constructor (`fst/node.cppm:755-795`).
    fn new(data: &'a [u8], addr: u64) -> Result<Self, FormatError> {
        if addr == FST_EMPTY_ADDRESS {
            // The empty node is final with a zero output.
            return Ok(Self {
                data,
                start: 0,
                end: 0,
                ntrans: 0,
                state: 0,
                sizes: FstPackSizes::default(),
                is_final: true,
                final_output: 0,
            });
        }
        let start = usize::try_from(addr).map_err(|_| fst_err(addr as usize, data.len()))?;
        if start >= data.len() {
            return Err(fst_err(start + 1, data.len()));
        }
        let data_len = start + 1;
        let state = data[start];
        let get = |i: usize| -> Result<u8, FormatError> {
            data.get(i).copied().ok_or(fst_err(i + 1, data.len()))
        };
        match state & 0b1100_0000 {
            0b1100_0000 => {
                // StateOneTransNext: one transition to the previously compiled
                // node.
                let il = Self::input_len(state);
                let end = data_len
                    .checked_sub(1 + il)
                    .ok_or(fst_err(data_len, data.len()))?;
                Ok(Self {
                    data,
                    start,
                    end,
                    ntrans: 1,
                    state,
                    sizes: FstPackSizes::default(),
                    is_final: false,
                    final_output: 0,
                })
            }
            0b1000_0000 => {
                // StateOneTrans: one transition with an output.
                let il = Self::input_len(state);
                let ntl = Self::ntrans_len(state);
                let sizes = FstPackSizes::from_byte(get(data_len - 1 - ntl - 1)?);
                let end = data_len
                    .checked_sub(1 + il + 1 + sizes.tsize + sizes.osize)
                    .ok_or(fst_err(data_len, data.len()))?;
                Ok(Self {
                    data,
                    start,
                    end,
                    ntrans: 1,
                    state,
                    sizes,
                    is_final: false,
                    final_output: 0,
                })
            }
            _ => {
                // StateAnyTrans.
                let ntl = Self::ntrans_len(state);
                let mut ntrans = (state & 0x3f) as usize;
                if ntrans == 0 {
                    let v = get(data_len - 2)?;
                    // "1" can never be a normal count, so it means 256.
                    ntrans = if v == 1 { 256 } else { v as usize };
                }
                let sizes = FstPackSizes::from_byte(get(data_len - 1 - ntl - 1)?);
                let index_size = if ntrans > FST_TRANS_INDEX_THRESHOLD {
                    256
                } else {
                    0
                };
                let total = ntrans + ntrans * sizes.tsize + index_size;
                let is_final = state & 0b0100_0000 != 0;
                let final_osize = if is_final { sizes.osize } else { 0 };
                let end = data_len
                    .checked_sub(1 + ntl + 1 + total + ntrans * sizes.osize + final_osize)
                    .ok_or(fst_err(data_len, data.len()))?;
                let final_output = if sizes.osize == 0 || !is_final {
                    0
                } else {
                    let at = data_len - 1 - ntl - 1 - total - ntrans * sizes.osize - sizes.osize;
                    fst_unpack_uint(&data[at..], sizes.osize as u8)?
                };
                Ok(Self {
                    data,
                    start,
                    end,
                    ntrans,
                    state,
                    sizes,
                    is_final,
                    final_output,
                })
            }
        }
    }

    /// The `i`th transition's input byte (`fst/node.cppm:370-378`, `:455-463`,
    /// `:672-676`).
    fn input_at(&self, i: usize) -> Result<u8, FormatError> {
        match self.state & 0b1100_0000 {
            0b1100_0000 => {
                // A common input, else the byte just before the state byte.
                if let Some(b) = fst_common_input(self.state & 0x3f) {
                    Ok(b)
                } else {
                    self.data
                        .get(self.start.wrapping_sub(1))
                        .copied()
                        .ok_or(fst_err(self.start, self.data.len()))
                }
            }
            0b1000_0000 => {
                if let Some(b) = fst_common_input(self.state & 0x3f) {
                    Ok(b)
                } else {
                    self.data
                        .get(self.start.wrapping_sub(1))
                        .copied()
                        .ok_or(fst_err(self.start, self.data.len()))
                }
            }
            _ => {
                let ntl = Self::ntrans_len(self.state);
                let index_size = if self.ntrans > FST_TRANS_INDEX_THRESHOLD {
                    256
                } else {
                    0
                };
                let at =
                    self.start as isize - ntl as isize - 1 - index_size as isize - i as isize - 1;
                if at < 0 {
                    return Err(fst_err(self.start, self.data.len()));
                }
                self.data
                    .get(at as usize)
                    .copied()
                    .ok_or(fst_err(at as usize + 1, self.data.len()))
            }
        }
    }

    /// The `i`th transition's output (`fst/node.cppm:465-472`, `:703-713`).
    fn output_at(&self, i: usize) -> Result<u64, FormatError> {
        match self.state & 0b1100_0000 {
            0b1100_0000 => Ok(0),
            0b1000_0000 => {
                // `OutputOf`
                if self.sizes.osize == 0 {
                    return Ok(0);
                }
                let il = Self::input_len(self.state);
                let at = self.start - il - 1 - self.sizes.tsize - self.sizes.osize;
                fst_unpack_uint(&self.data[at..], self.sizes.osize as u8)
            }
            _ => {
                if self.sizes.osize == 0 {
                    return Ok(0);
                }
                let ntl = Self::ntrans_len(self.state);
                let total = self.ntrans
                    + self.ntrans * self.sizes.tsize
                    + if self.ntrans > FST_TRANS_INDEX_THRESHOLD {
                        256
                    } else {
                        0
                    };
                let at = self.start as isize
                    - ntl as isize
                    - 1
                    - total as isize
                    - (i * self.sizes.osize) as isize
                    - self.sizes.osize as isize;
                if at < 0 {
                    return Err(fst_err(self.start, self.data.len()));
                }
                fst_unpack_uint(&self.data[at as usize..], self.sizes.osize as u8)
            }
        }
    }

    /// The `i`th transition's target address (`fst/node.cppm:380`, `:474-478`,
    /// `:661-669`).
    fn trans_addr(&self, i: usize) -> Result<u64, FormatError> {
        match self.state & 0b1100_0000 {
            0b1100_0000 => Ok(self.end as u64 - 1),
            0b1000_0000 => {
                let il = Self::input_len(self.state);
                let at = self.start - il - 1 - self.sizes.tsize;
                fst_unpack_delta(&self.data[at..], self.sizes.tsize as u8, self.end as u64)
            }
            _ => {
                let ntl = Self::ntrans_len(self.state);
                let index_size = if self.ntrans > FST_TRANS_INDEX_THRESHOLD {
                    256
                } else {
                    0
                };
                let at = self.start as isize
                    - ntl as isize
                    - 1
                    - index_size as isize
                    - self.ntrans as isize
                    - (i * self.sizes.tsize) as isize
                    - self.sizes.tsize as isize;
                if at < 0 {
                    return Err(fst_err(self.start, self.data.len()));
                }
                fst_unpack_delta(
                    &self.data[at as usize..],
                    self.sizes.tsize as u8,
                    self.end as u64,
                )
            }
        }
    }

    /// `FindInput` (`fst/node.cppm:395-402`, `:493-500`, `:679-701`): the index
    /// of the transition whose input is `b`.
    fn find_input(&self, b: u8) -> Result<Option<usize>, FormatError> {
        match self.state & 0b1100_0000 {
            0b1100_0000 | 0b1000_0000 => {
                if self.input_at(0)? == b {
                    Ok(Some(0))
                } else {
                    Ok(None)
                }
            }
            _ => {
                if self.ntrans > FST_TRANS_INDEX_THRESHOLD {
                    let index_size = 256;
                    let start = self.start - Self::ntrans_len(self.state) - 1 - index_size;
                    let i = self.data[start + b as usize] as usize;
                    // The C++ compares `i > ntrans`; a value of `ntrans` or more
                    // marks an absent transition.
                    if i > self.ntrans {
                        Ok(None)
                    } else {
                        Ok(Some(i))
                    }
                } else {
                    if self.ntrans == 0 {
                        return Ok(None);
                    }
                    let ntl = Self::ntrans_len(self.state);
                    let start = self.start - ntl - 1 - self.ntrans;
                    for i in 0..self.ntrans {
                        if *self
                            .data
                            .get(start + i)
                            .ok_or(fst_err(start + i + 1, self.data.len()))?
                            == b
                        {
                            return Ok(Some(self.ntrans - i - 1));
                        }
                    }
                    Ok(None)
                }
            }
        }
    }
}

/// Looks up `key` in a whole FST region (`Fst::Get`, `fst.cppm:156-181`),
/// returning its value when present.
pub fn fst_get(fst: &[u8], key: &[u8]) -> Result<Option<u64>, FormatError> {
    let mut addr = fst_root_addr(fst)?;
    let mut out = 0u64;
    for &b in key {
        let node = FstNode::new(fst, addr)?;
        let Some(ti) = node.find_input(b)? else {
            return Ok(None);
        };
        out = out.wrapping_add(node.output_at(ti)?);
        addr = node.trans_addr(ti)?;
    }
    let node = FstNode::new(fst, addr)?;
    if !node.is_final {
        return Ok(None);
    }
    Ok(Some(out.wrapping_add(node.final_output)))
}

/// All `(key, value)` pairs of an FST in lexicographic order (`FstStream`).
///
/// The reference walks the automaton lazily; this port materializes the same
/// traversal once (the FST is already fully resident in these chunks).
fn fst_entries(fst: &[u8]) -> Result<Vec<(Vec<u8>, u64)>, FormatError> {
    struct Frame {
        addr: u64,
        prefix: Vec<u8>,
        out: u64,
        next: usize,
    }
    let mut entries = Vec::new();
    let root = fst_root_addr(fst)?;
    let mut stack = vec![Frame {
        addr: root,
        prefix: Vec::new(),
        out: 0,
        next: 0,
    }];
    while let Some(frame) = stack.last_mut() {
        let i = frame.next;
        let node = FstNode::new(fst, frame.addr)?;
        if i >= node.ntrans {
            stack.pop();
            continue;
        }
        frame.next += 1;
        let mut prefix = frame.prefix.clone();
        prefix.push(node.input_at(i)?);
        let out = frame.out.wrapping_add(node.output_at(i)?);
        let addr = node.trans_addr(i)?;
        let child = FstNode::new(fst, addr)?;
        if child.is_final {
            entries.push((prefix.clone(), out.wrapping_add(child.final_output)));
        }
        if child.ntrans > 0 {
            stack.push(Frame {
                addr,
                prefix,
                out,
                next: 0,
            });
        }
    }
    Ok(entries)
}

/// Lexicographically ordered iteration over an FST with optional bounds
/// (`FstStream`, `fst.cppm:231-330`).
pub struct FstStream {
    entries: Vec<(Vec<u8>, u64)>,
    start: usize,
    end: usize,
}

impl FstStream {
    /// `FstStream(fst)` with unbounded ends.
    pub fn new(fst: &[u8]) -> Result<Self, FormatError> {
        let entries = fst_entries(fst)?;
        let end = entries.len();
        Ok(Self {
            entries,
            start: 0,
            end,
        })
    }

    /// `FstStream::Reset(min, max)`: `min` is inclusive/exclusive as given,
    /// `max` is inclusive/exclusive as given; `None` is unbounded.
    pub fn reset(&mut self, min: Option<(&[u8], bool)>, max: Option<(&[u8], bool)>) {
        self.start = match min {
            None => 0,
            Some((key, inclusive)) => {
                let idx = self.entries.partition_point(|(k, _)| {
                    if inclusive {
                        k.as_slice() < key
                    } else {
                        k.as_slice() <= key
                    }
                });
                idx
            }
        };
        self.end = match max {
            None => self.entries.len(),
            Some((key, inclusive)) => self.entries.partition_point(|(k, _)| {
                if inclusive {
                    k.as_slice() <= key
                } else {
                    k.as_slice() < key
                }
            }),
        };
    }

    // `FstStream::Next` is the iterator's next item.
}

impl Iterator for FstStream {
    type Item = (Vec<u8>, u64);

    fn next(&mut self) -> Option<Self::Item> {
        if self.start >= self.end {
            return None;
        }
        let entry = self.entries[self.start].clone();
        self.start += 1;
        Some(entry)
    }
}

// ---------------------------------------------------------------------------
// FST building (`fst/build.cppm`, `fst/build_impl.cpp`, `fst/registry.cppm`)
// ---------------------------------------------------------------------------

/// `NONE_ADDRESS` (`fst/node.cppm:47`): never a real node address.
const FST_NONE_ADDRESS: usize = 1;

/// An FST output value (a `u64` with the prefix/cat/sub algebra,
/// `fst/node.cppm:67-98`).
#[derive(Debug, Clone, Copy, PartialEq, Eq, Default, PartialOrd, Ord)]
struct FstOutput(u64);

impl FstOutput {
    fn value(self) -> u64 {
        self.0
    }
    fn is_zero(self) -> bool {
        self.0 == 0
    }
    /// `Prefix`: the minimum of the two.
    fn prefix(self, o: Self) -> Self {
        Self(self.0.min(o.0))
    }
    /// `Cat`: the sum of the two.
    fn cat(self, o: Self) -> Self {
        Self(self.0.wrapping_add(o.0))
    }
    /// `Sub`: the difference (asserted non-negative in the C++).
    fn sub(self, o: Self) -> Self {
        debug_assert!(self.0 >= o.0);
        Self(self.0.wrapping_sub(o.0))
    }
}

/// A transition (`fst/node.cppm:101-111`).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
struct FstTransition {
    inp: u8,
    out: FstOutput,
    addr: usize,
}

impl Default for FstTransition {
    fn default() -> Self {
        Self {
            inp: 0,
            out: FstOutput::default(),
            addr: FST_NONE_ADDRESS,
        }
    }
}

/// A node under construction (`fst/node.cppm:143-199`), with the `Hash` the
/// registry uses.
#[derive(Debug, Clone, PartialEq, Eq, Default)]
struct BuilderNode {
    is_final: bool,
    final_output: FstOutput,
    trans: Vec<FstTransition>,
}

impl BuilderNode {
    /// `BuilderNode::Hash` (`fst/node.cppm:165-183`): FNV-1a.
    fn hash(&self) -> u64 {
        const FNV_PRIME: u64 = 1_099_511_628_211;
        let mut h: u64 = 14_695_981_039_346_656_037;
        h = (h ^ self.is_final as u64).wrapping_mul(FNV_PRIME);
        h = (h ^ self.final_output.value()).wrapping_mul(FNV_PRIME);
        for t in &self.trans {
            h = (h ^ t.inp as u64).wrapping_mul(FNV_PRIME);
            h = (h ^ t.out.value()).wrapping_mul(FNV_PRIME);
            h = (h ^ t.addr as u64).wrapping_mul(FNV_PRIME);
        }
        h
    }
}

/// A direct-mapped node cache (`fst/registry.cppm:41-68`) used for suffix
/// sharing. A slot collides destructively, exactly as the C++ does.
struct Registry {
    table: Vec<(BuilderNode, usize)>,
    mask: usize,
}

impl Registry {
    fn new(table_size_shift: u32) -> Self {
        let n = 1usize << table_size_shift;
        Self {
            table: vec![(BuilderNode::default(), 0); n],
            mask: n - 1,
        }
    }

    /// `Registry::Find` (`fst/registry.cppm:51-61`): returns the slot and, if
    /// the slot already holds an equal node, its compiled address.
    fn find(&self, key: &BuilderNode) -> (usize, Option<usize>) {
        let slot = (key.hash() as usize) & self.mask;
        if self.table[slot].0 == *key {
            (slot, Some(self.table[slot].1))
        } else {
            (slot, None)
        }
    }

    /// `Registry::Insert` (`fst/registry.cppm:64-67`).
    fn insert(&mut self, slot: usize, key: BuilderNode, value: usize) {
        self.table[slot] = (key, value);
    }
}

/// One level of the unfinished-node stack (`fst/build.cppm:36-59`).
#[derive(Debug, Clone, Default)]
struct UnfinishedNode {
    node: BuilderNode,
    /// `last_`: `None` means `present_ == false`.
    last: Option<(u8, FstOutput)>,
}

impl UnfinishedNode {
    /// `LastCompiled`: append the pending transition pointing at `addr`.
    fn last_compiled(&mut self, addr: usize) {
        if let Some((inp, out)) = self.last.take() {
            self.node.trans.push(FstTransition { inp, out, addr });
        }
    }

    /// `AddOutputPrefix`: push a prefix onto the node's and its pending
    /// transition's outputs.
    fn add_output_prefix(&mut self, prefix: FstOutput) {
        if self.node.is_final {
            self.node.final_output = prefix.cat(self.node.final_output);
        }
        for t in &mut self.node.trans {
            t.out = prefix.cat(t.out);
        }
        if let Some((_, out)) = &mut self.last {
            *out = prefix.cat(*out);
        }
    }
}

/// `FstBuilder` (`fst/build.cppm:170-242`, `build_impl.cpp`).
///
/// Keys must be inserted in lexicographic order. All I/O goes to an internal
/// buffer, and the trailer is appended by `finish` (the C++ streams through a
/// `CountingWriter`; buffering here is equivalent because the trailer's
/// checksum covers every byte written before it).
pub struct FstBuilder {
    buf: Vec<u8>,
    unfinished: Vec<UnfinishedNode>,
    registry: Registry,
    last_key: Vec<u8>,
    last_addr: usize,
    len: usize,
}

impl Default for FstBuilder {
    fn default() -> Self {
        Self::new()
    }
}

impl FstBuilder {
    pub fn new() -> Self {
        let mut buf = Vec::new();
        buf.extend_from_slice(&FST_VERSION.to_le_bytes());
        buf.extend_from_slice(&0u64.to_le_bytes()); // FstType
        Self {
            buf,
            unfinished: vec![UnfinishedNode::default()],
            registry: Registry::new(16),
            last_key: Vec::new(),
            last_addr: FST_NONE_ADDRESS,
            len: 0,
        }
    }

    /// `FstBuilder::Insert` (`fst/build.cppm:223-226`).
    pub fn insert(&mut self, key: &[u8], val: u64) {
        self.check_last_key(key, true);
        self.insert_output(key, val);
    }

    /// `FstBuilder::Add` (`fst/build.cppm:209-212`): a key with a zero output.
    pub fn add(&mut self, key: &[u8]) {
        self.check_last_key(key, false);
        self.insert_output(key, 0);
    }

    /// `FstBuilder::Finish` (`build_impl.cpp:32-43`), returning the bytes.
    pub fn finish(mut self) -> Vec<u8> {
        self.compile_from(0);
        let root = self.unfinished.pop().expect("root node").node;
        let root_addr = self.compile(root);
        append_fst_trailer(&mut self.buf, self.len as u64, root_addr as u64);
        self.buf
    }

    /// `CheckLastKey` (`build_impl.cpp:106-122`).
    ///
    /// Note the C++ returns as soon as `last_` is empty *without storing the
    /// key*, so in this version `last_` stays empty forever and the ordering
    /// checks never fire. Reproduced verbatim so the builder's behaviour is
    /// identical (with a debug assert to catch genuinely bad callers).
    fn check_last_key(&mut self, key: &[u8], check_dupe: bool) {
        if self.last_key.is_empty() {
            return;
        }
        assert!(
            !(check_dupe && self.last_key.len() == key.len() && self.last_key == key),
            "FST duplicated key"
        );
        assert!(self.last_key.as_slice() <= key, "FST out of order key");
        self.last_key.clear();
        self.last_key.extend_from_slice(key);
    }

    /// `InsertOutput` (`build_impl.cpp:45-66`).
    fn insert_output(&mut self, key: &[u8], val: u64) {
        let mut out = FstOutput(val);
        if key.is_empty() {
            self.len = 1; // must be the first key
                          // SetRootOutput
            self.unfinished[0].node.is_final = true;
            self.unfinished[0].node.final_output = out;
            return;
        }
        let prefix_len = self.find_common_prefix_and_set_output(key, &mut out);
        if prefix_len == key.len() {
            // Duplicate value with no output.
            debug_assert!(out.is_zero());
            return;
        }
        self.len += 1;
        self.compile_from(prefix_len);
        self.add_suffix(&key[prefix_len..], out);
    }

    /// `FindCommonPrefixAndSetOutput` (`fst/build.cppm:121-139`).
    fn find_common_prefix_and_set_output(&mut self, key: &[u8], out: &mut FstOutput) -> usize {
        let n = self.unfinished.len();
        let common_len = key.len().min(n - 1);
        let mut i = 0;
        while i < common_len {
            let Some((inp, t_out)) = self.unfinished[i].last else {
                panic!("registry invariant: last transition must be present");
            };
            if inp != key[i] {
                break;
            }
            let common_pre = t_out.prefix(*out);
            let add_prefix = t_out.sub(common_pre);
            *out = out.sub(common_pre);
            if !add_prefix.is_zero() {
                self.unfinished[i].last = Some((inp, common_pre));
                self.unfinished[i + 1].add_output_prefix(add_prefix);
            }
            i += 1;
        }
        i
    }

    /// `AddSuffix` (`fst/build.cppm:93-107`).
    fn add_suffix(&mut self, bs: &[u8], out: FstOutput) {
        if bs.is_empty() {
            return;
        }
        let len = self.unfinished.len();
        assert!(self.unfinished[len - 1].last.is_none());
        self.unfinished[len - 1].last = Some((bs[0], out));
        self.unfinished
            .resize(len + bs.len() - 1, UnfinishedNode::default());
        for (i, &b) in bs.iter().enumerate().skip(1) {
            self.unfinished[len + i - 1].last = Some((b, FstOutput::default()));
        }
        // PushEmpty(true)
        self.unfinished.push(UnfinishedNode {
            node: BuilderNode {
                is_final: true,
                ..BuilderNode::default()
            },
            last: None,
        });
    }

    /// `TopLastFreeze` (`fst/build.cppm:87-91`).
    fn top_last_freeze(&mut self, addr: usize) {
        if let Some(top) = self.unfinished.last_mut() {
            top.last_compiled(addr);
        }
    }

    /// `CompileFrom` (`build_impl.cpp:68-81`).
    fn compile_from(&mut self, istate: usize) {
        let mut addr = FST_NONE_ADDRESS;
        let remained = self.unfinished.len() - (istate + 1);
        for _ in 0..remained {
            if addr != FST_NONE_ADDRESS {
                self.top_last_freeze(addr);
            }
            let node = self.unfinished.pop().expect("unfinished node").node;
            addr = self.compile(node);
        }
        self.top_last_freeze(addr);
    }

    /// `Compile` (`build_impl.cpp:83-95`).
    fn compile(&mut self, node: BuilderNode) -> usize {
        if node.is_final && node.trans.is_empty() && node.final_output.is_zero() {
            return FST_EMPTY_ADDRESS as usize;
        }
        let (slot, found) = self.registry.find(&node);
        if let Some(addr) = found {
            return addr;
        }
        let start_addr = self.buf.len();
        self.compile_node(&node, start_addr);
        let last_addr = self.buf.len() - 1;
        self.last_addr = last_addr;
        self.registry.insert(slot, node, last_addr);
        last_addr
    }

    /// `Node::Compile` (`fst/node.cppm:797-810`).
    fn compile_node(&mut self, node: &BuilderNode, addr: usize) {
        let ntrans = node.trans.len();
        if ntrans == 0 && node.is_final && node.final_output.is_zero() {
            return;
        }
        if ntrans != 1 || node.is_final {
            self.compile_any_trans(node, addr);
        } else {
            let t = node.trans[0];
            if t.addr == self.last_addr && t.out.is_zero() {
                self.compile_one_trans_next(t.inp);
            } else {
                self.compile_one_trans(addr, t);
            }
        }
    }

    /// `StateOneTransNext::Compile` (`fst/node.cppm:348-355`).
    fn compile_one_trans_next(&mut self, input: u8) {
        let idx = fst_common_idx(input, 0b0011_1111);
        if idx == 0 {
            self.buf.push(input);
        }
        self.buf.push(0b1100_0000 | idx);
    }

    /// `StateOneTrans::Compile` (`fst/node.cppm:413-432`).
    fn compile_one_trans(&mut self, addr: usize, trans: FstTransition) {
        let out = trans.out.value();
        let output_pack_size = if out != 0 {
            fst_pack_uint(&mut self.buf, out)
        } else {
            0
        };
        let trans_pack_size = fst_pack_delta(&mut self.buf, addr as u64, trans.addr as u64);
        // PackSizes: transition size in the high nibble, output in the low.
        let pack_sizes = (trans_pack_size << 4) | (output_pack_size & 0x0f);
        self.buf.push(pack_sizes);
        let idx = fst_common_idx(trans.inp, 0b0011_1111);
        if idx == 0 {
            self.buf.push(trans.inp);
        }
        self.buf.push(0b1000_0000 | idx);
    }

    /// `StateAnyTrans::Compile` (`fst/node.cppm:511-575`).
    fn compile_any_trans(&mut self, node: &BuilderNode, addr: usize) {
        let ntrans = node.trans.len();
        debug_assert!(ntrans <= 256);
        let mut tsize = 0u8;
        let mut osize = fst_pack_size(node.final_output.value());
        let mut any_outs = !node.final_output.is_zero();
        for t in &node.trans {
            tsize = tsize.max(fst_pack_delta_size(addr as u64, t.addr as u64));
            osize = osize.max(fst_pack_size(t.out.value()));
            any_outs = any_outs || !t.out.is_zero();
        }
        if !any_outs {
            osize = 0;
        }
        // state byte bits: final flag + ntrans (0 when > 0x3f)
        let is_final = node.is_final;
        let state_val = (if is_final { 0b0100_0000 } else { 0 })
            | (if ntrans <= 0b0011_1111 {
                ntrans as u8
            } else {
                0
            });

        if any_outs {
            if is_final {
                fst_pack_uint_in(&mut self.buf, node.final_output.value(), osize);
            }
            for t in node.trans.iter().rev() {
                fst_pack_uint_in(&mut self.buf, t.out.value(), osize);
            }
        }
        for t in node.trans.iter().rev() {
            let delta = if t.addr == FST_EMPTY_ADDRESS as usize {
                FST_EMPTY_ADDRESS
            } else {
                (addr as u64).wrapping_sub(t.addr as u64)
            };
            fst_pack_uint_in(&mut self.buf, delta, tsize);
        }
        for t in node.trans.iter().rev() {
            self.buf.push(t.inp);
        }
        if ntrans > FST_TRANS_INDEX_THRESHOLD {
            let mut index = [255u8; 256];
            for (i, t) in node.trans.iter().enumerate() {
                index[t.inp as usize] = i as u8;
            }
            self.buf.extend_from_slice(&index);
        }
        let pack_sizes = (tsize << 4) | (osize & 0x0f);
        self.buf.push(pack_sizes);
        if ntrans == 0 || ntrans > 0b0011_1111 {
            // StateNtrans() == 0, so an explicit count byte follows.
            let count = if ntrans == 256 { 1 } else { ntrans };
            fst_pack_uint_in(&mut self.buf, count as u64, 1);
        }
        self.buf.push(state_val);
    }
}

/// Builds an FST from `(key, value)` pairs, which must be sorted by key and
/// distinct (`FstBuilder`).
pub fn build_fst(entries: &[(Vec<u8>, u64)]) -> Vec<u8> {
    let mut b = FstBuilder::new();
    for (k, v) in entries {
        b.insert(k, *v);
    }
    b.finish()
}

// ---------------------------------------------------------------------------
// u32 posting block codec: `CompositeCodec<SIMDBinaryPacking, VariableByte>`
// (`fastpfor_impl.cpp:55-57`; layout verified against C++ ground truth)
// ---------------------------------------------------------------------------
//
// `SIMDBinaryPacking` handles whole 128-value mini-blocks; the remaining tail
// goes through `VariableByte`. Its byte layout is a plain bit-packing *per
// 32-bit SSE lane*: for a mini-block at width `w` the 128 values split by
// `index % 4` into four lanes, each lane's values are packed LSB-first into a
// `w`-word bit stream, and lane `L`'s k-th word lands at output word
// `L + 4*k`. (Verified: this reproduces the C++ bytes for widths 1..17.)

/// `SIMDBinaryPacking::MiniBlockSize` (`simdbitpacking.h:36`).
pub const POSTING_MINI_BLOCK: usize = 128;
/// `HowManyMiniBlocks` (`simdbitpacking.h:37`): mini-blocks per bit-width header.
pub const POSTING_MINI_BLOCKS_PER_GROUP: usize = 16;

/// The bit width needed to store `v` (`maxbits`), `0` for zero.
fn max_bits(v: u32) -> u8 {
    (32 - v.leading_zeros()) as u8
}

/// Packs one 128-value mini-block at width `w` into `4*w` words (the lane
/// layout above).
fn pack_mini_block(values: &[u32], w: u8, out: &mut Vec<u8>) {
    let w = w as usize;
    let mut words = vec![0u32; 4 * w];
    for lane in 0..4usize {
        let mut bit = 0usize;
        let mut idx = lane;
        while idx < values.len() {
            let v = values[idx];
            for bpos in 0..w {
                if (v >> bpos) & 1 != 0 {
                    let gb = bit + bpos;
                    words[lane + 4 * (gb / 32)] |= 1 << (gb % 32);
                }
            }
            bit += w;
            idx += 4;
        }
    }
    for word in words {
        out.extend_from_slice(&word.to_le_bytes());
    }
}

/// Unpacks one 128-value mini-block of width `w` from `buf`, appending up to
/// `count` values.
fn unpack_mini_block(
    buf: &[u8],
    w: u8,
    count: usize,
    out: &mut Vec<u32>,
) -> Result<(), FormatError> {
    let w = w as usize;
    let nwords = 4 * w;
    if buf.len() < nwords * 4 {
        return Err(FormatError::Truncated {
            need: nwords * 4,
            got: buf.len(),
        });
    }
    let word = |i: usize| -> u32 { u32::from_le_bytes(buf[i * 4..i * 4 + 4].try_into().unwrap()) };
    for idx in 0..count {
        let lane = idx % 4;
        let pos = idx / 4;
        let mut value = 0u32;
        for bpos in 0..w {
            let gb = pos * w + bpos;
            let bitval = (word(lane + 4 * (gb / 32)) >> (gb % 32)) & 1;
            value |= bitval << bpos;
        }
        out.push(value);
    }
    Ok(())
}

/// `VariableByte::encodeToByteArray` (`variablebyte.h:64-...`): 7-bit groups,
/// LSB first, high bit set on the last byte, then zero-padded to 32 bits.
fn vb_encode(values: &[u32], out: &mut Vec<u8>) {
    for &v in values {
        let mut val = v as u64;
        loop {
            let byte = (val & 0x7f) as u8;
            val >>= 7;
            if val == 0 {
                out.push(byte | 0x80);
                break;
            }
            out.push(byte);
        }
    }
    while !out.len().is_multiple_of(4) {
        out.push(0);
    }
}

/// `VariableByte::decodeArray`: reads `count` values, returning how many bytes
/// were consumed.
fn vb_decode(buf: &[u8], count: usize) -> Result<(Vec<u32>, usize), FormatError> {
    let mut out = Vec::with_capacity(count);
    let mut pos = 0usize;
    for _ in 0..count {
        let mut value = 0u64;
        let mut shift = 0u32;
        loop {
            let byte = *buf.get(pos).ok_or(FormatError::Truncated {
                need: pos + 1,
                got: buf.len(),
            })?;
            pos += 1;
            value |= ((byte & 0x7f) as u64) << shift;
            if byte & 0x80 != 0 {
                break;
            }
            shift += 7;
        }
        out.push(value as u32);
    }
    Ok((out, pos))
}

/// Encodes a u32 posting field with the C++ `SIMDBinaryPacking + VariableByte`
/// composite: whole 128-value blocks go through SIMD-BP128, the tail through
/// variable-byte.
pub fn encode_u32_block(values: &[u32]) -> Vec<u8> {
    let mut out = Vec::new();
    let simd_len = values.len() / POSTING_MINI_BLOCK * POSTING_MINI_BLOCK;
    // SIMDBinaryPacking always writes its own length first (0 for no blocks).
    out.extend_from_slice(&(simd_len as u32).to_le_bytes());
    let nblocks = simd_len / POSTING_MINI_BLOCK;
    let mut b = 0usize;
    while b < nblocks {
        let group = (nblocks - b).min(POSTING_MINI_BLOCKS_PER_GROUP);
        let mut widths = [0u8; POSTING_MINI_BLOCKS_PER_GROUP];
        for (i, w) in widths.iter_mut().enumerate().take(group) {
            let mb = &values[(b + i) * POSTING_MINI_BLOCK..(b + i + 1) * POSTING_MINI_BLOCK];
            *w = mb.iter().map(|v| max_bits(*v)).max().unwrap_or(0);
        }
        // Four words, each packing four widths big-endian.
        for g in 0..4 {
            let w = ((widths[4 * g] as u32) << 24)
                | ((widths[4 * g + 1] as u32) << 16)
                | ((widths[4 * g + 2] as u32) << 8)
                | widths[4 * g + 3] as u32;
            out.extend_from_slice(&w.to_le_bytes());
        }
        for (i, &w) in widths.iter().enumerate().take(group) {
            let mb = &values[(b + i) * POSTING_MINI_BLOCK..(b + i + 1) * POSTING_MINI_BLOCK];
            pack_mini_block(mb, w, &mut out);
        }
        b += group;
    }
    if values.len() > simd_len {
        vb_encode(&values[simd_len..], &mut out);
    }
    out
}

/// Decodes a u32 posting field, returning the decoded values and how many bytes
/// were consumed (`IntEncoder::Decode` needs `expected` because the SIMD/`VB`
/// split is not self-describing).
pub fn decode_u32_block(buf: &[u8], expected: usize) -> Result<(Vec<u32>, usize), FormatError> {
    if buf.len() < 4 {
        return Err(FormatError::Truncated {
            need: 4,
            got: buf.len(),
        });
    }
    let simd_len = u32::from_le_bytes(buf[0..4].try_into().unwrap()) as usize;
    let mut pos = 4usize;
    let mut out = Vec::with_capacity(expected);
    let nblocks = simd_len / POSTING_MINI_BLOCK;
    let mut b = 0usize;
    while out.len() < simd_len {
        let group = (nblocks - b).min(POSTING_MINI_BLOCKS_PER_GROUP);
        // Four header words carry the 16 widths.
        if pos + 16 > buf.len() {
            return Err(FormatError::Truncated {
                need: pos + 16,
                got: buf.len(),
            });
        }
        let mut widths = [0u8; POSTING_MINI_BLOCKS_PER_GROUP];
        for g in 0..4 {
            let w = u32::from_le_bytes(buf[pos + 4 * g..pos + 4 * g + 4].try_into().unwrap());
            widths[4 * g] = (w >> 24) as u8;
            widths[4 * g + 1] = (w >> 16) as u8;
            widths[4 * g + 2] = (w >> 8) as u8;
            widths[4 * g + 3] = w as u8;
        }
        pos += 16;
        for &w in widths.iter().take(group) {
            let count = (simd_len - out.len()).min(POSTING_MINI_BLOCK);
            let nbytes = 4 * w as usize * 4;
            if pos + nbytes > buf.len() {
                return Err(FormatError::Truncated {
                    need: pos + nbytes,
                    got: buf.len(),
                });
            }
            unpack_mini_block(&buf[pos..pos + nbytes], w, count, &mut out)?;
            pos += nbytes;
        }
        b += group;
    }
    // The remaining values are variable-byte encoded.
    if expected > simd_len {
        let (tail, used) = vb_decode(&buf[pos..], expected - simd_len)?;
        out.extend_from_slice(&tail);
        // `VariableByte` zero-pads to a 32-bit boundary, and the C++ reader
        // consumes the padded span.
        pos = (pos + used).div_ceil(4) * 4;
    }
    Ok((out, pos))
}

/// Decodes one `CompositeCodec<SIMDBinaryPacking, VariableByte>` payload whose
/// length is already known (the `IntEncoder<u32, SIMDBitPacking>` u8 frame): the
/// SIMD value count is the first word, and the variable-byte tail is
/// self-delimiting. `VariableByte` zero-pads to a 32-bit boundary; padding bytes
/// terminate no value, so decoding stops at the payload end without producing
/// spurious zeros (`variablebyte.h:202-284`).
pub(crate) fn decode_u32_block_all(payload: &[u8]) -> Result<Vec<u32>, FormatError> {
    if payload.len() < 4 {
        return Err(FormatError::Truncated {
            need: 4,
            got: payload.len(),
        });
    }
    let simd_len = u32::from_le_bytes(payload[0..4].try_into().unwrap()) as usize;
    let (mut out, used) = decode_u32_block(payload, simd_len)?;
    let mut pos = used;
    while pos < payload.len() {
        let mut value = 0u64;
        let mut shift = 0u32;
        loop {
            let Some(&byte) = payload.get(pos) else {
                // Trailing `needPaddingTo32Bits` zero bytes: no value.
                return Ok(out);
            };
            pos += 1;
            value |= u64::from(byte & 0x7f) << shift;
            if byte & 0x80 != 0 {
                out.push(value as u32);
                break;
            }
            shift += 7;
            if shift >= 64 {
                return Err(FormatError::Truncated {
                    need: pos,
                    got: payload.len(),
                });
            }
        }
    }
    Ok(out)
}

// ---------------------------------------------------------------------------
// u16 posting block codec: IndexLib `NewPForDeltaCompressor`
// (`third_party/newpfor/`; layout verified against C++ output)
// ---------------------------------------------------------------------------

/// `NewPForDeltaCompressor::BLOCK_SIZE` (`new_pfordelta_compressor.h:51`).
pub const NPF_BLOCK_SIZE: usize = 128;

/// `S9Compressor`'s field widths and counts (`s9compressor.h:59-60`).
const S9_BIT_LENGTH: [u32; 9] = [1, 2, 3, 4, 5, 7, 9, 14, 28];
const S9_CODE_NUM: [usize; 9] = [28, 14, 9, 7, 5, 4, 3, 2, 1];

/// The six "special frame" markers (`firstExceptionPos`,
/// `new_pfordelta_compressor.h:55-64`).
const NPF_SAME_INLINE: u32 = 255;
const NPF_SAME_ALONE: u32 = 254;
const NPF_INCREASE_INLINE: u32 = 253;
const NPF_INCREASE_ALONE: u32 = 252;
const NPF_DECREASE_INLINE: u32 = 251;
const NPF_DECREASE_ALONE: u32 = 250;

/// Header bitfields (`NewPForDeltaCompressorHeader`, `:30-42`).
#[derive(Debug, Clone, Copy)]
struct NpfHeader {
    is_last: bool,
    exception_range: u32,
    frame_bits: u32,
    first_exception_pos: u32,
    data_num: usize,
}

fn npf_header(x: u32) -> NpfHeader {
    let raw = (x >> 12) & 0x1f;
    NpfHeader {
        is_last: x & 1 != 0,
        exception_range: (x >> 1) & 0x7ff,
        frame_bits: if raw == 31 { 32 } else { raw },
        first_exception_pos: (x >> 17) & 0xff,
        data_num: ((x >> 25) & 0x7f) as usize + 1,
    }
}

/// `S9Decode` (`new_pfordelta_compressor.h`): ORs the exception magnitudes back
/// into `dest`, starting at `first_exception_pos` (already `fep - 1`).
///
/// The stream alternates exception-value and gap fields; the top four bits of
/// each word pick a case (0..8 starts with a value, 16..24 with a gap), and
/// `head` carries the alternation across words.
fn s9_apply(dest: &mut [u16], words: &[u32], first_exception_pos: usize, frame_bits: u32) {
    let mut cur = first_exception_pos;
    let mut head = 0u32;
    for &val in words {
        let c = (val >> 28) + head;
        let idx = (c % 16) as usize;
        let b = S9_BIT_LENGTH[idx];
        let n = S9_CODE_NUM[idx];
        let mut want_value = c < 16;
        for j in 0..n {
            let field = (val >> ((n - 1 - j) as u32 * b)) & ((1u32 << b) - 1);
            if want_value {
                if cur < dest.len() {
                    dest[cur] |= (field.wrapping_shl(frame_bits) & 0xffff) as u16;
                }
                want_value = false;
            } else {
                cur += field as usize + 1;
                want_value = true;
            }
        }
        head = if want_value { 0 } else { 16 };
    }
}

/// Decodes a u16 posting field (`NewPForDeltaCompressor::Decompress`), returning
/// the values and how many bytes were consumed. `expected` is the value count
/// because the last block is only marked by a header bit.
pub fn decode_u16_block(buf: &[u8], expected: usize) -> Result<(Vec<u16>, usize), FormatError> {
    let mut pos = 0usize;
    let mut out: Vec<u16> = Vec::with_capacity(expected);
    loop {
        if pos + 4 > buf.len() {
            return Err(FormatError::Truncated {
                need: pos + 4,
                got: buf.len(),
            });
        }
        let header = npf_header(u32::from_le_bytes(buf[pos..pos + 4].try_into().unwrap()));
        pos += 4;
        let word = |i: usize| -> Result<u32, FormatError> {
            // `TryDecompressSpecialBlock` reads the "alone" value at
            // `src + HEADER_INT_SIZE`, i.e. the word *after* the header.
            let at = pos + 4 * i;
            buf.get(at..at + 4)
                .map(|s| u32::from_le_bytes(s.try_into().unwrap()))
                .ok_or(FormatError::Truncated {
                    need: at + 4,
                    got: buf.len(),
                })
        };
        let dn = header.data_num;
        let mut block = vec![0u16; dn];
        match header.first_exception_pos {
            NPF_SAME_INLINE => {
                block.fill(header.exception_range as u16);
            }
            NPF_SAME_ALONE => {
                let v = word(0)? as u16;
                pos += 4;
                block.fill(v);
            }
            NPF_INCREASE_INLINE => {
                for (i, v) in block.iter_mut().enumerate() {
                    *v = (header.exception_range as u16).wrapping_add(i as u16);
                }
            }
            NPF_INCREASE_ALONE => {
                let v = word(0)? as u16;
                pos += 4;
                for (i, d) in block.iter_mut().enumerate() {
                    *d = v.wrapping_add(i as u16);
                }
            }
            NPF_DECREASE_INLINE => {
                // `dest[dataNum-1-i] = value + i`, i.e. the last value is the base.
                for (i, v) in block.iter_mut().rev().enumerate() {
                    *v = (header.exception_range as u16).wrapping_add(i as u16);
                }
            }
            NPF_DECREASE_ALONE => {
                let v = word(0)? as u16;
                pos += 4;
                for (i, d) in block.iter_mut().rev().enumerate() {
                    *d = v.wrapping_add(i as u16);
                }
            }
            fep => {
                // General frame: `dn` packed values, then the exception words.
                let fb = header.frame_bits;
                let frame_words = (dn * fb as usize).div_ceil(32);
                let nbytes = frame_words * 4;
                if pos + nbytes > buf.len() {
                    return Err(FormatError::Truncated {
                        need: pos + nbytes,
                        got: buf.len(),
                    });
                }
                unpack_n_u16(&buf[pos..pos + nbytes], fb, dn, &mut block)?;
                pos += nbytes;
                if fep != 0 {
                    // `exceptionIntRange` words of S9-encoded exceptions.
                    let exc = header.exception_range as usize;
                    if pos + exc * 4 > buf.len() {
                        return Err(FormatError::Truncated {
                            need: pos + exc * 4,
                            got: buf.len(),
                        });
                    }
                    let words: Vec<u32> = (0..exc)
                        .map(|i| {
                            u32::from_le_bytes(
                                buf[pos + 4 * i..pos + 4 * i + 4].try_into().unwrap(),
                            )
                        })
                        .collect();
                    s9_apply(&mut block, &words, fep as usize - 1, fb);
                    pos += exc * 4;
                }
            }
        }
        out.extend_from_slice(&block);
        if header.is_last || out.len() >= expected {
            break;
        }
    }
    Ok((out, pos))
}

/// `unpack_N<T>` (`unpack.h`): `n` values of `bits` bits each, packed LSB-first
/// and read sequentially (the 32-value main loop plus the unaligned tail is
/// equivalent to one naive pass).
fn unpack_n_u16(buf: &[u8], bits: u32, n: usize, out: &mut [u16]) -> Result<(), FormatError> {
    if bits == 0 {
        return Ok(());
    }
    for (j, o) in out.iter_mut().enumerate().take(n) {
        let bit = j as u64 * bits as u64;
        let word = (bit / 32) as usize;
        let off = (bit % 32) as u32;
        let at = word * 4;
        let lo = buf.get(at..at + 4).ok_or(FormatError::Truncated {
            need: at + 4,
            got: buf.len(),
        })?;
        let mut v = u32::from_le_bytes(lo.try_into().unwrap()) >> off;
        // A field may straddle a word boundary.
        if off + bits > 32 {
            let hi = buf.get(at + 4..at + 8).ok_or(FormatError::Truncated {
                need: at + 8,
                got: buf.len(),
            })?;
            let hi = u32::from_le_bytes(hi.try_into().unwrap());
            v |= hi << (32 - off);
        }
        *o = (v & ((1u32 << bits) - 1)) as u16;
    }
    Ok(())
}

/// `HighBitIdx` (`new_pfordelta_compressor.h`): the number of significant bits
/// (`0` for zero).
fn npf_high_bit(value: u16) -> u32 {
    16 - value.leading_zeros()
}

/// `MakeHeader` (`:670-687`).
fn npf_make_header(
    frame_bits: u32,
    first_exception_pos: u32,
    code_len: usize,
    exception_int_range: u32,
    is_last_block: bool,
) -> u32 {
    let fb = if frame_bits >= 32 { 31 } else { frame_bits };
    ((code_len as u32 - 1) & 0x7f) << 25
        | (first_exception_pos & 0xff) << 17
        | (fb & 0x1f) << 12
        | (exception_int_range & 0x7ff) << 1
        | u32::from(is_last_block)
}

/// `SelectCompressBits` (`:228-309`): the frame width and either the exception
/// count or a special-frame marker.
fn npf_select_bits(block: &[u16]) -> (u32, u32) {
    // `FRAME_BIT_MAP[i] == i` except for 31/32 which map to 32.
    let frame_bit_map = |i: usize| -> u32 {
        if i >= 31 {
            32
        } else {
            i as u32
        }
    };
    let mut num_bit_array = [0u32; 33];
    let first_value = block[0];
    let mut is_all_same = true;
    let mut is_increase = true;
    let mut is_decrease = true;
    let mut max_bits = 0u32;
    for (i, &v) in block.iter().enumerate() {
        is_all_same &= v == first_value;
        is_increase &= v == first_value.wrapping_add(i as u16);
        is_decrease &= v == first_value.wrapping_sub(i as u16);
        let hb = npf_high_bit(v);
        max_bits = max_bits.max(hb);
        num_bit_array[hb as usize] += 1;
    }
    if max_bits == 0 {
        return (0, 0);
    }
    let inline = |v: u16| -> u32 {
        if (v as u32) <= 2047 {
            0
        } else {
            1
        }
    };
    if is_all_same {
        return (0, NPF_SAME_INLINE - inline(first_value));
    }
    if is_increase {
        return (0, NPF_INCREASE_INLINE - inline(first_value));
    }
    if is_decrease {
        let base = block[block.len() - 1];
        return (0, NPF_DECREASE_INLINE - inline(base));
    }
    for j in 1..33 {
        num_bit_array[j] += num_bit_array[j - 1];
    }
    let max_exception_num = (block.len() as u32 * 10) / 100;
    let mut num_exceptions = 0u32;
    let mut best_frame_bits = 32u32;
    let mut best_total_bytes = 4 * block.len() as u32;
    let mut i = frame_bit_map(max_bits as usize) as i32;
    while i >= 0 {
        if frame_bit_map(i as usize) != i as u32 {
            i -= 1;
            continue;
        }
        let frame_bits = i as u32;
        let exception_num = block.len() as u32 - num_bit_array[i as usize];
        if exception_num > max_exception_num || max_bits > frame_bits + 28 {
            break;
        }
        let exception_bits = max_bits - frame_bits;
        let bytes_per_exception = if exception_bits <= 7 {
            2
        } else if exception_bits <= 9 {
            3
        } else if exception_bits <= 14 {
            4
        } else {
            8
        };
        let bytes_for_frame = (block.len() as u32 * frame_bits).div_ceil(32) * 4;
        let bytes_for_exception = (exception_num * bytes_per_exception).div_ceil(4) * 4;
        if bytes_for_exception + bytes_for_frame < best_total_bytes {
            best_total_bytes = bytes_for_exception + bytes_for_frame;
            best_frame_bits = frame_bits;
            num_exceptions = exception_num;
        }
        i -= 1;
    }
    (best_frame_bits, num_exceptions)
}

/// `Pack` (`:453-466`) / `pack_N<T>`: the LSB-first sequential inverse of
/// [`unpack_n_u16`].
fn npf_pack(out: &mut Vec<u8>, code: &[u16], frame_bits: u32) {
    if frame_bits == 0 {
        return;
    }
    let nwords = (code.len() * frame_bits as usize).div_ceil(32);
    let mut words = vec![0u32; nwords];
    let mask = if frame_bits >= 32 {
        u32::MAX
    } else {
        (1u32 << frame_bits) - 1
    };
    for (j, &v) in code.iter().enumerate() {
        let bit = j as u64 * frame_bits as u64;
        let w = (bit / 32) as usize;
        let off = (bit % 32) as u32;
        let masked = (v as u32) & mask;
        words[w] |= masked << off;
        if off + frame_bits > 32 && w + 1 < nwords {
            words[w + 1] |= masked >> (32 - off);
        }
    }
    for w in words {
        out.extend_from_slice(&w.to_le_bytes());
    }
}

/// `S9Compressor::Encode` (`s9compressor.h:68-110`).
fn s9_encode(src: &[u32]) -> Vec<u32> {
    let mut out = Vec::new();
    let mut cur = 0usize;
    while cur < src.len() {
        for k in 0..9usize {
            let cur_size = S9_CODE_NUM[k];
            if src.len() < cur + cur_size {
                continue;
            }
            let b = S9_BIT_LENGTH[k];
            let mut res = 0u32;
            let mut i = 0usize;
            while i < cur_size && src[cur + i] < (1u32 << b) {
                res = (res << b) + src[cur + i];
                i += 1;
            }
            if i == cur_size {
                out.push(res | ((k as u32) << 28));
                cur += cur_size;
                break;
            }
            assert!(k < 8, "S9 exception value exceeds 28 bits");
        }
    }
    out
}

/// `TransformToFrame` (`:414-451`).
#[allow(clippy::too_many_arguments)]
fn npf_transform_to_frame(
    out: &mut Vec<u8>,
    frame_bits: u32,
    first_exception_pos: usize,
    code: &[u16],
    exceptions: &[u32],
    exception_offsets: &[u32],
    num_exceptions: usize,
    is_last_block: bool,
) {
    let header = |exc_range: u32| {
        npf_make_header(
            frame_bits,
            first_exception_pos as u32,
            code.len(),
            exc_range,
            is_last_block,
        )
    };
    if first_exception_pos == 0 {
        out.extend_from_slice(&header(0).to_le_bytes());
        npf_pack(out, code, frame_bits);
        return;
    }
    // Interleave exception values and offsets, then S9-encode them.
    let mut datum = vec![0u32; num_exceptions * 2 - 1];
    for i in 0..num_exceptions {
        datum[2 * i] = exceptions[i];
    }
    for i in 0..num_exceptions - 1 {
        datum[2 * i + 1] = exception_offsets[i];
    }
    let exc_words = s9_encode(&datum);
    out.extend_from_slice(&header(exc_words.len() as u32).to_le_bytes());
    npf_pack(out, code, frame_bits);
    for w in exc_words {
        out.extend_from_slice(&w.to_le_bytes());
    }
}

/// `TransformToSpecialFrame` (`:380-405`).
fn npf_transform_to_special_frame(
    out: &mut Vec<u8>,
    block: &[u16],
    marker: u32,
    is_last_block: bool,
) {
    let store_alone = matches!(
        marker,
        NPF_SAME_ALONE | NPF_INCREASE_ALONE | NPF_DECREASE_ALONE
    );
    let mut value = block[0];
    if matches!(marker, NPF_DECREASE_INLINE | NPF_DECREASE_ALONE) {
        value = block[block.len() - 1];
    }
    let header = npf_make_header(
        0,
        marker,
        block.len(),
        if store_alone { 0 } else { value as u32 },
        is_last_block,
    );
    out.extend_from_slice(&header.to_le_bytes());
    if store_alone {
        out.extend_from_slice(&(value as u32).to_le_bytes());
    }
}

/// `CompressBlock` (`:321-368`).
fn npf_compress_block(
    out: &mut Vec<u8>,
    block: &[u16],
    frame_bits: u32,
    num_exceptions: u32,
    is_last_block: bool,
) {
    if num_exceptions == 0 {
        npf_transform_to_frame(out, frame_bits, 0, block, &[], &[], 0, is_last_block);
        return;
    }
    if matches!(
        num_exceptions,
        NPF_SAME_ALONE
            | NPF_SAME_INLINE
            | NPF_INCREASE_ALONE
            | NPF_INCREASE_INLINE
            | NPF_DECREASE_ALONE
            | NPF_DECREASE_INLINE
    ) {
        npf_transform_to_special_frame(out, block, num_exceptions, is_last_block);
        return;
    }
    // Split each value into the low `frame_bits` (the frame) and the rest (the
    // exception), recording the positions that need patching.
    let mask = if frame_bits >= 32 {
        u32::MAX
    } else {
        (1u32 << frame_bits) - 1
    };
    let max = (1u32 << frame_bits) as u16;
    let n = block.len();
    let mut code = block.to_vec();
    let mut miss = Vec::new();
    for (i, &v) in block.iter().enumerate() {
        if max <= v {
            miss.push(i);
        }
    }
    debug_assert_eq!(miss.len(), num_exceptions as usize);
    let mut exceptions = vec![0u32; num_exceptions as usize];
    let mut offsets = vec![0u32; num_exceptions.saturating_sub(1) as usize];
    for (k, &m) in miss.iter().enumerate() {
        exceptions[k] = (code[m] as u32) >> frame_bits;
        code[m] = ((code[m] as u32) & mask) as u16;
        if k > 0 {
            offsets[k - 1] = (m - miss[k - 1] - 1) as u32;
        }
    }
    let first_exception_pos = miss[0] + 1;
    let _ = n;
    npf_transform_to_frame(
        out,
        frame_bits,
        first_exception_pos,
        &code,
        &exceptions,
        &offsets,
        num_exceptions as usize,
        is_last_block,
    );
}

/// Encodes a u16 posting field (`NewPForDeltaCompressor::Compress(..., true)`).
pub fn encode_u16_block(values: &[u16]) -> Vec<u8> {
    let mut out = Vec::new();
    let total = values.len().div_ceil(NPF_BLOCK_SIZE);
    for blk in 0..total {
        let off = blk * NPF_BLOCK_SIZE;
        let len = (values.len() - off).min(NPF_BLOCK_SIZE);
        let block = &values[off..off + len];
        let (frame_bits, num_exceptions) = npf_select_bits(block);
        npf_compress_block(
            &mut out,
            block,
            frame_bits,
            num_exceptions,
            blk + 1 == total,
        );
    }
    out
}

#[cfg(test)]
mod tests {
    use super::*;

    /// C++: `TermMetaTest.test1` (`term_meta_ut.cpp`): `TermMeta(1, 2, 3)`
    /// dumped with `OPTION_FLAG_ALL` and read back.
    #[test]
    fn term_meta_all_options_roundtrip() {
        let meta = TermMeta {
            doc_freq: 1,
            total_tf: 2,
            payload: 3,
            doc_start: 0,
            pos_start: 0,
            pos_end: 0,
        };
        let flags = TermMetaFlags {
            has_term_frequency: true,
            has_term_payload: true,
        };
        let bytes = encode_term_meta(&meta, flags);
        let (decoded, used) = decode_term_meta(&bytes, flags).unwrap();
        assert_eq!(used, bytes.len());
        assert_eq!(decoded.doc_freq, meta.doc_freq);
        assert_eq!(decoded.total_tf, meta.total_tf);
        assert_eq!(decoded.payload, meta.payload);
    }

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
        assert_eq!(
            offsets,
            vec![0, encode_term_meta(&metas[0], flags).len() as u64]
        );

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

    /// Decodes a lowercase hex string (test helper).
    fn unhex(s: &str) -> Vec<u8> {
        (0..s.len())
            .step_by(2)
            .map(|i| u8::from_str_radix(&s[i..i + 2], 16).unwrap())
            .collect()
    }

    #[test]
    fn golden_fsts_from_the_cpp_builder_verify() {
        // Real bytes emitted by the C++ `FstBuilder` (the amalgamated
        // `src/storage/invertedindex/fst/*` built with clang, keys "app",
        // "apple", "apply", "banana", "band", "cat" etc. mapped to 100, 200…).
        // They pin the header, the trailer layout, the masked CRC-32/IEEE and
        // the `root_addr + 21` length rule against ground truth.
        let goldens: &[(&str, &str, u64, u64)] = &[
            // label, hex, len_, root_addr
            (
                "empty",
                "0300000000000000000000000000000000000000000000000000000000000000028ba85c",
                0,
                0,
            ),
            (
                "one",
                "0300000000000000000000000000000064001185010000000000000013000000000000003963da13",
                1,
                19,
            ),
            (
                "two",
                "03000000000000000000000000000000c86400006261110202000000000000001700000000000000d16c7686",
                2,
                23,
            ),
            (
                "words",
                "0300000000000000000000000000000064000000796511020064016c1141c9c9001085cb6400000164611102cbc5001081c5580290016400010513636261120306000000000000003f00000000000000b514bf07",
                6,
                63,
            ),
            (
                "shared",
                "03000000000000000000000000000000640000006261110200109ac80001046261110209109a2c010000010462611202001085c5c5f401640001066261120205000000000000003e00000000000000a7fbf8f8",
                5,
                62,
            ),
        ];
        for (label, hex, want_len, want_root) in goldens {
            let fst = unhex(hex);
            // Header is VERSION then FstType.
            assert_eq!(fst_header(&fst).unwrap(), (FST_VERSION, 0), "{label}");
            // The trailer's `len_`/`root_addr` and its checksum all verify.
            let (len, root) = verify_fst(&fst).unwrap_or_else(|e| panic!("{label}: {e:?}"));
            assert_eq!((len, root), (*want_len, *want_root), "{label}");
            // The region length the reader derives equals the real length.
            assert_eq!(fst_len_from_trailer(root), fst.len(), "{label}");
            // The empty FST is exactly the header plus the trailer (no nodes).
            if *want_root == 0 {
                assert_eq!(fst.len(), FST_HEADER_LEN + FST_TRAILER_LEN, "{label}");
                assert_eq!(fst.len(), 36);
            }
        }
    }

    #[test]
    fn fst_lookup_decodes_the_cpp_built_fsts() {
        // The same goldens as `golden_fsts_from_the_cpp_builder_verify`, but
        // now driving the node decoder: every inserted key must resolve to its
        // value, and a missing key must not.
        let cases: &[(&str, &[(&str, u64)])] = &[
            ("empty", &[]),
            ("one", &[("a", 100)]),
            ("two", &[("a", 100), ("b", 200)]),
            (
                "words",
                &[
                    ("app", 100),
                    ("apple", 200),
                    ("apply", 300),
                    ("banana", 400),
                    ("band", 500),
                    ("cat", 600),
                ],
            ),
            (
                "shared",
                &[
                    ("aaaa", 100),
                    ("aaab", 200),
                    ("aabb", 300),
                    ("abbb", 400),
                    ("baaa", 500),
                ],
            ),
        ];
        let hexes = [
            "0300000000000000000000000000000000000000000000000000000000000000028ba85c",
            "0300000000000000000000000000000064001185010000000000000013000000000000003963da13",
            "03000000000000000000000000000000c86400006261110202000000000000001700000000000000d16c7686",
            "0300000000000000000000000000000064000000796511020064016c1141c9c9001085cb6400000164611102cbc5001081c5580290016400010513636261120306000000000000003f00000000000000b514bf07",
            "03000000000000000000000000000000640000006261110200109ac80001046261110209109a2c010000010462611202001085c5c5f401640001066261120205000000000000003e00000000000000a7fbf8f8",
        ];
        for ((_, pairs), hex) in cases.iter().zip(hexes) {
            let fst = unhex(hex);
            for (k, v) in *pairs {
                assert_eq!(fst_get(&fst, k.as_bytes()).unwrap(), Some(*v), "key {k}");
            }
            // Keys not in the set are absent (only meaningful when the set is
            // non-empty and we can pick a stranger).
            if !pairs.is_empty() {
                assert_eq!(fst_get(&fst, b"\xff\xfe").unwrap(), None);
                assert_eq!(fst_get(&fst, b"ap").unwrap(), None);
            }
        }
    }

    #[test]
    fn fst_builder_reproduces_the_cpp_bytes() {
        // The decisive test: building the same key sets with the Rust port of
        // `FstBuilder` must yield byte-for-byte the C++ builder's output.
        #[allow(clippy::type_complexity)]
        let cases: &[(&str, &[(&str, u64)], &str)] = &[
            (
                "empty",
                &[],
                "0300000000000000000000000000000000000000000000000000000000000000028ba85c",
            ),
            (
                "one",
                &[("a", 100)],
                "0300000000000000000000000000000064001185010000000000000013000000000000003963da13",
            ),
            (
                "two",
                &[("a", 100), ("b", 200)],
                "03000000000000000000000000000000c86400006261110202000000000000001700000000000000d16c7686",
            ),
            (
                "words",
                &[
                    ("app", 100),
                    ("apple", 200),
                    ("apply", 300),
                    ("banana", 400),
                    ("band", 500),
                    ("cat", 600),
                ],
                "0300000000000000000000000000000064000000796511020064016c1141c9c9001085cb6400000164611102cbc5001081c5580290016400010513636261120306000000000000003f00000000000000b514bf07",
            ),
            (
                "shared",
                &[
                    ("aaaa", 100),
                    ("aaab", 200),
                    ("aabb", 300),
                    ("abbb", 400),
                    ("baaa", 500),
                ],
                "03000000000000000000000000000000640000006261110200109ac80001046261110209109a2c010000010462611202001085c5c5f401640001066261120205000000000000003e00000000000000a7fbf8f8",
            ),
        ];
        for (label, pairs, hex) in cases {
            let entries: Vec<(Vec<u8>, u64)> = pairs
                .iter()
                .map(|(k, v)| (k.as_bytes().to_vec(), *v))
                .collect();
            let built = build_fst(&entries);
            let want = unhex(hex);
            assert_eq!(built.len(), want.len(), "{label}: length");
            assert_eq!(built, want, "{label}: bytes");
            // And the result reads back through our own decoder.
            for (k, v) in *pairs {
                assert_eq!(
                    fst_get(&built, k.as_bytes()).unwrap(),
                    Some(*v),
                    "{label}/{k}"
                );
            }
        }
    }

    #[test]
    fn u32_block_matches_the_cpp_bytes() {
        // C++ `CompositeCodec<SIMDBinaryPacking, VariableByte>` output (see
        // CPP_FORMAT_SPEC.md 9.3.5 for the harness recipe), labels matching the
        // inputs below. `small`/`big`/`mixed` have no 128-block, so they are
        // pure variable-byte; `tail260` exercises SIMD + VB together.
        let words = |i: usize| (i % 5) as u32;
        let cases: &[(&str, Vec<u32>, &str)] = &[
            ("small", vec![1, 2, 3, 4, 5], "000000008182838485000000"),
            (
                "zeros",
                vec![0u32; 128],
                "8000000000000000000000000000000000000000",
            ),
            (
                "mixed",
                vec![1, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89],
                "00000000818181828385888d95a2b7d9",
            ),
            (
                "big",
                vec![100000, 200000, 300000, 400000],
                "00000000200d86401a8c602792003598",
            ),
            (
                "bw1",
                (0..128).map(|i| 1 + (i % 3) as u32).collect(),
                "8000000000000002000000000000000000000000799ee7799ee7799ee7799ee7799ee7799ee7799ee7799ee7799ee7799ee7799e",
            ),
            (
                "bw2",
                (0..128).map(|i| 2 + (i % 3) as u32).collect(),
                "80000000000000030000000000000000000000001a356ad4a3468d1ad4a851a31a356ad4a851a346356ad4a8468d1a35a851a3468d1a356a51a3468d6ad4a8518d1a356a",
            ),
            (
                "bw7",
                (0..128).map(|i| 7 + (i % 3) as u32).collect(),
                "800000000000000400000000000000000000000087799887988779987998877987799887799887798779988798877998799887799887799879988779877998879887799887799887988779987998877987799887",
            ),
            (
                "bw255",
                (0..128).map(|i| 255 + (i % 3) as u32).collect(),
                "8000000000000009000000000000000000000000ff0006fc0003fe0301ff010cff0006fc0730e03f18f01fc0f80f60c00730e03f8001ff0180ff00067f0003fe8001ff010cf80f60fc0730e00318f01f0cf80f60c07f00033f8001ffc080ff00c07f0003fe0318f0010cf80f06fc0730fe0318f01fc080ff60c07f00e03f80011fc080ff0006fc0703fe0318ff010cf80006fc0730e03f80f01fc0800f60c07f30e03f80",
            ),
            (
                "bw65535",
                (0..128).map(|i| 65535 + (i % 3) as u32).collect(),
                "8000000000000011000000000000000000000000ffff0000000003000100ffffffff00000600fcfffeff030001000c000600fcff070030001800f0fff8ff0f0007003000e0ff3f001f00c0006000c0ffe0ff3f00800100ff80ffff007f000003800100ffff01000c000600fc00feff03ff01000c00f8ff0fff070030001800f000f8ff0f006000c000e0ff3fff1f00c0006000c0ff7f0000008001000080ffffff7f00000300feffffff0100000006000300feff030018000c00f8fffcff070003001800f0ff1f000f0060003000e0fff0ff1f00c00080ffc0ff7f003f008001c00080ffff000006000300fe00ffff01ff00000600fcff07ff030018000c00f800fcff07003000e000f0ff1fff0f0060003000e0ff3f008000c0008000c0ff7fff3f0080",
            ),
            (
                "sq128",
                (0..128).map(|i| (i * i) as u32).collect(),
                "800000000000000e00000000000000000000000000000400014006100400094009400c900440020005a40221061003440784036901640024416e10270179402a4184902d400c0004240d4144100e8404040fc94444016440561169a469416e107c9173841e0009a41f6149be21c409d922294af402c4403812cb243a42d2103c92d9043e001084048150a6140411c9448951ec9444415a004da55ca156115f445f8561e919e406e4590e17ef1a3947fa5a649705418400242587c164118a8425068d4966c409a442f619b1a6294abe125c9acb86b6003124b9e1715ebdc43299c0a973d40d8443f01d9327f44da213f89db107fc",
            ),
            (
                "tail260",
                (0..260).map(words).collect(),
                "0001000000000303000000000000000000000000e014700a01a780530a38059c53c029e038059c02c029e014024e01a714700a384e01a780700a38058053c029059c024e53c029e09c024e01e014700a01a7805314700a38a78053c038059c02c029e014059c024e29e014704e01a780700a380581828384",
            ),
        ];
        for (label, values, hex) in cases {
            let want = unhex(hex);
            // Encode must be byte-identical to the C++ codec.
            assert_eq!(encode_u32_block(values), want, "{label}: encode");
            // And decode must recover the values.
            let (got, used) = decode_u32_block(&want, values.len()).unwrap();
            assert_eq!(&got, values, "{label}: decode");
            assert_eq!(used, want.len(), "{label}: bytes consumed");
        }
    }

    #[test]
    fn u16_block_decodes_the_cpp_bytes() {
        // C++ `NewPForDeltaCompressor::Compress(..., true)` output, covering
        // every special frame, a plain frame and frames with S9 exceptions.
        let mut e3: Vec<u16> = (0..128).map(|i| (i % 8) as u16).collect();
        e3[5] = 1000;
        e3[40] = 2000;
        e3[100] = 300;
        let mut e1: Vec<u16> = (0..4).cycle().take(128).collect();
        e1[0] = 5000;
        let cases: Vec<(&str, Vec<u16>, &str)> = vec![
            ("inc5", vec![1, 2, 3, 4, 5], "0300fa09"),
            (
                "fib12",
                vec![1, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89],
                "017000168140403028201a15d12d0b00",
            ),
            (
                "mod7",
                (0..128).map(|i| (i % 7) as u16).collect(),
                "013000fe88c61ad158231a6b44638d68ac118d35a2b14634d688c61ad158231a6b44638d68ac118d35a2b14634d688c61ad15823",
            ),
            ("const", vec![1000u16; 128], "d107feff"),
            (
                "rand",
                (0..128).map(|i| ((i * 37) % 1000) as u16).collect(),
                "01a000fe0094a0c41b94e4e2cd40283525d765bc8567e08a50d6a9e9afe426ecf2d478772efcf92424e1c624b87423d0494cc565d96ee015a8e2937466eaebb808b72cf5dd9c076ffe0848b421c92ddc0464d2527055a6db7704a6e8e49c98f62aeec12c476df7e6c0972fc2116c4462cb360095a4d45b94e5e6dd80283629e7a5bc866bf0ca50d7adf9efe48760c41a90d4a2cd3f2425e5d664b87527e0894cc669e9ae",
            ),
            (
                "two_blocks",
                (0..130).map(|i| (i % 3) as u16).collect(),
                "002000fe24499224499224499224499224499224499224499224499224499224499224490120000202000000",
            ),
            (
                "exc3",
                e3,
                "05300cfe8846f888c6fa88c6fa88c6fa88c6fa88c6fa88c6fa88c6fa88c6fa88c6fa88c6fa88c6fa88c6fa88c6fa88c6fa88c6fafa44f46125c00e70",
            ),
            (
                "exc1",
                e1,
                "032002fee4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e2040080",
            ),
            (
                "exc20",
                {
                    let mut v: Vec<u16> = (0..128).map(|i| (i % 16) as u16).collect();
                    for k in 0..20 {
                        v[k * 6] = 4000 + k as u16;
                    }
                    v
                },
                "01c000fea01f00023000045000a17f000890000ab000a2df000ef000001000a33f00045000067000a49f000ab0000cd000a5ff00001000023000a65f00067000089000a7bf000cd0000ef000a81f00023000045000a97f000890000ab000aadf000ef000001000ab3f00045000067000ac9f000ab0000cd000adff00001000023000ae5f00067000089000afbf000cd0000ef000b01f00023000045000b17f000890000ab000b2df000ef000001000b33f000450000670000890000ab0000cd0000ef000",
            ),
        ];
        for (label, values, hex) in cases {
            let bytes = unhex(hex);
            let (got, used) = decode_u16_block(&bytes, values.len()).unwrap();
            assert_eq!(got, values, "{label}: decode");
            assert_eq!(used, bytes.len(), "{label}: bytes consumed");
            // And the encoder must reproduce the C++ bytes.
            assert_eq!(encode_u16_block(&values), bytes, "{label}: encode");
        }
    }

    #[test]
    fn u16_block_roundtrips_the_alone_frames() {
        // Values above `IsUniqValueStoreInline`'s 11-bit limit (2047) cannot live
        // in the header, so the writer uses the "*_ALONE" frames, whose value is
        // the word after the header (`new_pfordelta_compressor.h:488-540`).
        let cases: &[(u32, Vec<u16>)] = &[
            (254, vec![2560]),                              // same, alone
            (252, vec![3000, 3001, 3002, 3003]),            // increase, alone
            (250, vec![3003, 3002, 3001, 3000]),            // decrease, alone
        ];
        for (marker, values) in cases {
            let bytes = encode_u16_block(values);
            let header = u32::from_le_bytes(bytes[0..4].try_into().unwrap());
            assert_eq!(*marker, (header >> 17) & 0xff, "frame marker");
            let (got, used) = decode_u16_block(&bytes, values.len()).unwrap();
            assert_eq!(&got, values, "marker {marker}");
            assert_eq!(used, bytes.len(), "marker {marker}");
        }
    }

    #[test]
    fn fulltext_dic_and_pos_compose() {
        // End-to-end: build a `.dic` (TermMeta region + FST + trailer) and a
        // `.pos` (per-term framing over the u32/u16 codecs), then read both
        // back through the public API. This exercises the whole chain the
        // engine will use:
        //   term -> FST value -> TermMeta offset -> doc_start/pos_start/pos_end
        //   -> `.pos` slice -> TermPostings framing -> u32/u16 blocks.
        // (The field-level interleaving *inside* a doc-list body is the
        // posting_decoder's job and is not modelled here.)
        let flags = TermMetaFlags {
            has_term_frequency: true,
            has_term_payload: true,
        };
        let terms = ["apple", "banana", "cat"];
        let docs: [Vec<u32>; 3] = [vec![1, 5, 9], vec![3], vec![2, 4, 6, 7, 130, 200]];
        let poss: [Vec<u32>; 3] = [vec![10, 20, 30], vec![5], vec![1, 2, 3, 4, 5, 6]];
        let tfs: [Vec<u16>; 3] = [vec![2, 1, 1], vec![1], vec![3, 1, 2, 4, 5, 6]];

        // `.pos`: each term's slice is framed by TermPostings; the doc list
        // holds the u32 doc-id block and the position list the u32 positions,
        // while the "skiplist" slots carry the u16 tf block.
        let mut pos = Vec::new();
        let mut slices = Vec::new();
        for i in 0..terms.len() {
            let doc_list = encode_u32_block(&docs[i]);
            let pos_list = encode_u32_block(&poss[i]);
            let tf_block = encode_u16_block(&tfs[i]);
            let tp = TermPostings {
                doc_skiplist: tf_block,
                doc_list,
                pos_skiplist: Vec::new(),
                pos_list,
            };
            let bytes = tp.encode();
            slices.push((pos.len() as u64, (pos.len() + bytes.len()) as u64));
            pos.extend_from_slice(&bytes);
        }

        // `.dic`: TermMeta records (doc/pos offsets point into `.pos`), then an
        // FST mapping each term to its record's offset.
        let metas: Vec<TermMeta> = (0..terms.len())
            .map(|i| TermMeta {
                doc_freq: docs[i].len() as u32,
                total_tf: tfs[i].iter().map(|v| *v as u32).sum(),
                payload: (i as u32) * 7,
                doc_start: slices[i].0,
                pos_start: slices[i].0,
                pos_end: slices[i].1,
            })
            .collect();
        let (region, offsets) = encode_term_meta_region_with_offsets(&metas, flags);
        let mut fst = FstBuilder::new();
        for (i, t) in terms.iter().enumerate() {
            fst.insert(t.as_bytes(), offsets[i]);
        }
        let dic = encode_dic(&region, &fst.finish());

        // Read `.dic` back.
        let (start, _, _) = dic_fst_region(&dic).unwrap();
        assert_eq!(start, region.len());
        let fst_bytes = dic_fst(&dic).unwrap();
        assert_eq!(fst_header(fst_bytes).unwrap().0, FST_VERSION);
        for (i, t) in terms.iter().enumerate() {
            let off = fst_get(fst_bytes, t.as_bytes()).unwrap().unwrap();
            assert_eq!(off, offsets[i]);
            assert_eq!(decode_dic_meta_at(&dic, off, flags).unwrap(), metas[i]);
        }
        assert_eq!(fst_get(fst_bytes, b"zebra").unwrap(), None);

        // Follow a term into `.pos` and decode its blocks.
        for (i, t) in terms.iter().enumerate() {
            let off = fst_get(fst_bytes, t.as_bytes()).unwrap().unwrap();
            let meta = decode_dic_meta_at(&dic, off, flags).unwrap();
            let slice = &pos[meta.doc_start as usize..meta.pos_end as usize];
            let (tp, used) = TermPostings::decode(slice).unwrap();
            assert_eq!(used, slice.len());
            let (got_docs, _) = decode_u32_block(&tp.doc_list, docs[i].len()).unwrap();
            assert_eq!(got_docs, docs[i], "{t}: doc ids");
            let (got_pos, _) = decode_u32_block(&tp.pos_list, poss[i].len()).unwrap();
            assert_eq!(got_pos, poss[i], "{t}: positions");
            let (got_tf, _) = decode_u16_block(&tp.doc_skiplist, tfs[i].len()).unwrap();
            assert_eq!(got_tf, tfs[i], "{t}: term frequencies");
        }
    }

    #[test]
    fn common_input_tables_are_inverses() {
        // Both tables are permutations of 0..=255, and `INV[A[b]] == b` holds,
        // which is the relation `CommonIdx`/`CommonInput` rely on.
        let mut seen = [false; 256];
        for b in 0..256 {
            assert_eq!(
                FST_COMMON_INPUTS_INV[FST_COMMON_INPUTS[b] as usize], b as u8,
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

    /// The reference's `FstTest` months fixture, sorted lexicographically.
    fn months() -> Vec<(Vec<u8>, u64)> {
        let names = [
            "January",
            "February",
            "March",
            "April",
            "May",
            "June",
            "July",
            "August",
            "September",
            "October",
            "November",
            "December",
        ];
        let mut entries: Vec<(Vec<u8>, u64)> = names
            .iter()
            .enumerate()
            .map(|(i, name)| (name.as_bytes().to_vec(), i as u64 + 1))
            .collect();
        entries.sort();
        entries
    }

    /// C++: `FstTest.BuildMemEmpty` (`fst_ut.cpp`).
    #[test]
    fn fst_build_mem_empty() {
        let buffer = FstBuilder::new().finish();
        // The 20-byte trailer (len, root, checksum) is included here whereas
        // `BytesWritten()` excludes the checksum: 32 + 4 = 36.
        assert_eq!(buffer.len(), 36);
        let (len, root) = verify_fst(&buffer).unwrap();
        assert_eq!(len, 0);
        assert_eq!(root, 0);
        assert!(len == 0, "IsEmpty");
    }

    /// C++: `FstTest.BuildFileEmpty` (`fst_ut.cpp`).
    #[test]
    fn fst_build_file_empty() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("empty.fst");
        let buffer = FstBuilder::new().finish();
        std::fs::write(&path, &buffer).unwrap();
        assert_eq!(std::fs::metadata(&path).unwrap().len(), buffer.len() as u64);
        assert_eq!(buffer.len(), 36);
    }

    /// C++: `FstTest.BuildMem` (`fst_ut.cpp`).
    #[test]
    fn fst_build_mem() {
        let buffer = build_fst(&months());
        // `buffer.size() == written + 4`: everything but the checksum is the
        // writer's count.
        let written = buffer.len() - 4;
        assert_eq!(buffer.len(), written + 4);
        verify_fst(&buffer).unwrap();
    }

    /// C++: `FstTest.BuildFile` (`fst_ut.cpp`).
    #[test]
    fn fst_build_file() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("months.fst");
        let buffer = build_fst(&months());
        std::fs::write(&path, &buffer).unwrap();
        assert_eq!(
            std::fs::metadata(&path).unwrap().len() as usize,
            buffer.len()
        );
    }

    /// C++: `FstTest.Get` (`fst_ut.cpp`).
    #[test]
    fn fst_get_values() {
        let entries = months();
        let fst = build_fst(&entries);
        let (len, _) = verify_fst(&fst).unwrap();
        assert_eq!(len as usize, entries.len());
        assert!(fst.len() > 36);
        for (key, value) in &entries {
            assert_eq!(fst_get(&fst, key).unwrap(), Some(*value));
        }
        assert_eq!(fst_get(&fst, b"NotAMonth").unwrap(), None);
    }

    /// C++: `FstTest.Iterate` (`fst_ut.cpp`): full-order iteration and the
    /// `[included, excluded)` bounds.
    #[test]
    fn fst_iterate_with_bounds() {
        let entries = months();
        let fst = build_fst(&entries);
        verify_fst(&fst).unwrap();
        let mut stream = FstStream::new(&fst).unwrap();
        let mut got = Vec::new();
        for (key, value) in stream.by_ref() {
            got.push((key, value));
        }
        assert_eq!(got, entries);

        // [months[3], months[7])
        let b1 = 3;
        let b2 = 7;
        stream.reset(Some((&entries[b1].0, true)), Some((&entries[b2].0, false)));
        let mut i = b1;
        for (key, value) in stream.by_ref() {
            assert_eq!(key, entries[i].0);
            assert_eq!(value, entries[i].1);
            i += 1;
        }
        assert_eq!(i, b2);
        // An excluded max also stops before that key.
        stream.reset(Some((&entries[b1].0, true)), Some((&entries[b2].0, true)));
        let mut seen = 0;
        while stream.next().is_some() {
            seen += 1;
        }
        assert_eq!(seen, b2 - b1 + 1);
    }
}
