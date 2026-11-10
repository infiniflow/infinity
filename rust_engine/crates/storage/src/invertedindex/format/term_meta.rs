use super::*;

/// `optionflag_t` bits that change how a [`TermMeta`] is encoded. The flag word
/// itself lives in the catalog's IndexBase JSON, **not** in the index files
/// (`index_defines.cppm:13-38`; `index_full_text.cppm:42,73`).
#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
pub struct TermMetaFlags {
    /// `of_term_frequency` — a `total_tf` field follows `doc_freq`, and the doc
    /// list carries a term-frequency column.
    pub has_term_frequency: bool,
    /// `of_term_payload` — a raw `u32` payload follows the frequency fields.
    pub has_term_payload: bool,
    /// `of_doc_payload` — the doc list carries a `u16` per-document payload
    /// column (`DocListEncoder::AddDocument`, `doc_list_encoder_impl.cpp:63`).
    pub has_doc_payload: bool,
    /// `of_block_max` — the doc skip list carries `block_max_tf` and
    /// `block_max_percentage`, which needs the frequency column
    /// (`doc_list_encoder_impl.cpp:164-168`).
    pub has_block_max: bool,
}

impl TermMetaFlags {
    /// The `optionflag_t` word these bits stand for (`index_defines.cppm:32`).
    ///
    /// A position list is always written: this store never indexes a term
    /// without positions, so `of_position_list` is always set.
    pub fn option_flag(self) -> u8 {
        let mut flag = super::position_list::OF_POSITION_LIST;
        if self.has_term_frequency {
            flag |= super::position_list::OF_TERM_FREQUENCY;
        }
        if self.has_term_payload {
            flag |= super::doc_list::OF_TERM_PAYLOAD;
        }
        if self.has_doc_payload {
            flag |= super::doc_list::OF_DOC_PAYLOAD;
        }
        if self.has_block_max {
            flag |= super::doc_list::OF_BLOCK_MAX;
        }
        flag
    }

    /// Whether the option word carries `of_position_list`. Always true here
    /// (see [`Self::option_flag`]), kept as a predicate so the merge can state
    /// the dependency on a frequency column explicitly.
    pub fn has_position_list(self) -> bool {
        self.option_flag() & super::position_list::OF_POSITION_LIST != 0
    }
}

/// `OPTION_FLAG_ALL` (`index_defines.cppm:32`): the flag word a full-text index
/// is created with (`index_full_text.cppm:73`), and therefore the one both the
/// writer and the reader of its files must use.
pub const OPTION_FLAG_ALL: TermMetaFlags = TermMetaFlags {
    has_term_frequency: true,
    has_term_payload: true,
    has_doc_payload: true,
    has_block_max: true,
};

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

/// The number of bytes [`write_vint`] emits for `value`
/// (`VByteCompressor::GetVInt32Length`/`GetVInt64Length`, `vbyte_compressor.cppm`).
pub fn vint_len(mut value: u64) -> usize {
    let mut len = 1;
    while value >= 0x80 {
        value >>= 7;
        len += 1;
    }
    len
}

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

    /// Bytes the position half of this slice occupies: `VInt pos_skip_len`,
    /// `VInt pos_len` and the two streams. That is exactly
    /// `TermMeta::pos_end - TermMeta::pos_start` in the reference
    /// (`posting_writer_impl.cpp:58-62`).
    pub fn pos_section_len(&self) -> usize {
        self.pos_skiplist.len()
            + self.pos_list.len()
            + vint_len(self.pos_skiplist.len() as u64)
            + vint_len(self.pos_list.len() as u64)
    }
}

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
