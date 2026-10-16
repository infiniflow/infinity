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

#[cfg(test)]
mod tests {
    use super::*;

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
