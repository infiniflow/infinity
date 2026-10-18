//! Segment-level full-text index: one inverted index per segment, in the two
//! forms the reference keeps (`memory_indexer*`, `disk_segment_reader*`).
//!
//! A table's rows are split into segments; each segment owns one full-text
//! index. While a segment is being built its index lives in memory; once it is
//! sealed the index is written as the C++ file family and queried through
//! `mmap`, so the resident set no longer grows with the table:
//!
//!   * `{index_dir}/{base_name}.dic` — TermMeta records followed by the FST
//!     mapping `term -> TermMeta offset` ([`crate::inverted_format::encode_dic`]).
//!   * `{index_dir}/{base_name}.pos` — the per-term postings slices.
//!   * `{index_dir}/{base_name}.len` — `u32[row_count]` document lengths.
//!
//! Row ids are stored as segment offsets (`row - base_row_id`) because the
//! reference's `docid_t` is 32-bit; the reader adds `base_row_id` back.
//!
//! The `.pos` slice uses the reference framing
//! (`VInt doc_skip_len, VInt doc_len, doc_skip, doc_list, VInt pos_skip_len,
//! VInt pos_len, pos_skip, pos_list`). A doc-list body is, per 128-row block,
//! the document-id deltas block followed by the term-frequency block, both
//! through the SIMD-BP128/VByte codec in [`crate::inverted_format`]. Position
//! lists are empty here: BM25 scoring does not read them, and a segment built
//! without positions is exactly the reference's `of_position_list`-less index.
//!
//! `.dic` and `.len` are byte-identical to the reference. The `.pos` framing is
//! too, but its block bodies currently use `encode_u32_block`'s
//! `CompositeCodec::encodeArray` framing; the reference's per-field encoder uses
//! its own `IntEncoder` prefix, so byte-exact `.pos` interop is still open (the
//! codec itself is bit-compatible — see `docs/CPP_FORMAT_SPEC.md` §9.3.5).

use std::collections::BTreeMap;
use std::fs::{self, File};

use memmap2::Mmap;

use crate::cpp_format::FormatError;
use crate::inverted_format::{
    build_fst, decode_dic_meta_at, decode_u32_block, dic_fst, encode_column_lengths, encode_dic,
    encode_term_meta_region_with_offsets, encode_u32_block, fst_get, fulltext_chunk_file, TermMeta,
    TermMetaFlags, TermPostings, FULLTEXT_DICT_SUFFIX, FULLTEXT_LENGTH_SUFFIX,
    FULLTEXT_POSTING_SUFFIX, POSTING_MINI_BLOCK,
};

/// Writes one segment's full-text index.
///
/// `docs` must be sorted by row id and `postings` must be sorted by term
/// (a `BTreeMap` is), with each posting list sorted by row id. `base_row_id`
/// is the segment's first row id; every `row - base_row_id` must fit `u32`.
pub fn write_segment_index(
    index_dir: &str,
    base_name: &str,
    base_row_id: u64,
    docs: &[(u64, u32)],
    postings: &BTreeMap<String, Vec<(u64, u32)>>,
    flags: TermMetaFlags,
) -> Result<(), FormatError> {
    fs::create_dir_all(index_dir).map_err(|e| FormatError::Io(e.to_string()))?;

    let lengths: Vec<u32> = docs.iter().map(|(_, len)| *len).collect();
    let len_bytes = encode_column_lengths(&lengths);

    let mut pos: Vec<u8> = Vec::new();
    let mut metas: Vec<TermMeta> = Vec::with_capacity(postings.len());
    let mut terms: Vec<Vec<u8>> = Vec::with_capacity(postings.len());
    for (term, list) in postings {
        if list.is_empty() {
            continue;
        }
        let slice_start = pos.len() as u64;
        let mut doc_body: Vec<u8> = Vec::new();
        let mut total_tf: u64 = 0;
        let mut last: u32 = 0;
        for chunk in list.chunks(POSTING_MINI_BLOCK) {
            let mut deltas: Vec<u32> = Vec::with_capacity(chunk.len());
            let mut tfs: Vec<u32> = Vec::with_capacity(chunk.len());
            for (row, tf) in chunk {
                // `docid_t` is the segment offset; the first delta is the
                // absolute offset (the reference starts `last_doc_id_` at 0).
                let doc = (*row)
                    .checked_sub(base_row_id)
                    .and_then(|d| u32::try_from(d).ok())
                    .ok_or_else(|| FormatError::Truncated {
                        need: (*row - base_row_id) as usize,
                        got: u32::MAX as usize,
                    })?;
                deltas.push(doc.wrapping_sub(last));
                tfs.push(*tf);
                last = doc;
                total_tf += u64::from(*tf);
            }
            // The buffer flushes every 128 rows: field 0 (doc deltas), then
            // field 1 (tf).
            doc_body.extend_from_slice(&encode_u32_block(&deltas));
            doc_body.extend_from_slice(&encode_u32_block(&tfs));
        }
        let slice = TermPostings {
            doc_skiplist: Vec::new(),
            doc_list: doc_body,
            pos_skiplist: Vec::new(),
            pos_list: Vec::new(),
        }
        .encode();
        pos.extend_from_slice(&slice);
        metas.push(TermMeta {
            doc_freq: list.len() as u32,
            total_tf: total_tf as u32,
            payload: 0,
            doc_start: slice_start,
            pos_start: slice_start,
            pos_end: pos.len() as u64,
        });
        terms.push(term.as_bytes().to_vec());
    }

    let (meta_region, offsets) = encode_term_meta_region_with_offsets(&metas, flags);
    let entries: Vec<(Vec<u8>, u64)> = terms.into_iter().zip(offsets).collect();
    let fst = build_fst(&entries);
    let dic = encode_dic(&meta_region, &fst);

    let write = |suffix: &str, bytes: &[u8]| -> Result<(), FormatError> {
        let path = fulltext_chunk_file(index_dir, base_name, suffix);
        fs::write(&path, bytes).map_err(|e| FormatError::Io(e.to_string()))
    };
    write(FULLTEXT_DICT_SUFFIX, &dic)?;
    write(FULLTEXT_POSTING_SUFFIX, &pos)?;
    write(FULLTEXT_LENGTH_SUFFIX, &len_bytes)?;
    Ok(())
}

/// A sealed segment's index, mapped from disk. Postings are decoded per term on
/// demand: the mapping shares the page cache, so a query does not keep the
/// table's postings in the process heap.
pub struct SegmentIndexReader {
    dic: Mmap,
    pos: Mmap,
    len: Mmap,
    flags: TermMetaFlags,
    base_row_id: u64,
}

impl SegmentIndexReader {
    /// Opens `{dir}/{base_name}.{dic,pos,len}`. Fails if any file is missing;
    /// an empty segment (`len` file of zero bytes) is allowed.
    ///
    /// The mapping is the only `unsafe` operation in this crate: a segment is
    /// immutable once sealed, so the mapped region cannot change underneath the
    /// reader (rebuilds write a new base name instead of overwriting).
    #[allow(unsafe_code)]
    pub fn open(
        dir: &str,
        base_name: &str,
        base_row_id: u64,
        flags: TermMetaFlags,
    ) -> Result<Self, FormatError> {
        let map = |suffix: &str| -> Result<Mmap, FormatError> {
            let path = fulltext_chunk_file(dir, base_name, suffix);
            let file = File::open(&path).map_err(|e| FormatError::Io(e.to_string()))?;
            // SAFETY: the file is read-only for the lifetime of the mapping and
            // does not change while it is mapped (a segment is immutable once
            // sealed).
            unsafe { Mmap::map(&file) }.map_err(|e| FormatError::Io(e.to_string()))
        };
        Ok(Self {
            dic: map(FULLTEXT_DICT_SUFFIX)?,
            pos: map(FULLTEXT_POSTING_SUFFIX)?,
            len: map(FULLTEXT_LENGTH_SUFFIX)?,
            flags,
            base_row_id,
        })
    }

    /// The document length at segment offset `doc` (the BM25 `dl`).
    pub fn doc_len(&self, doc: u32) -> Option<u32> {
        let off = doc as usize * 4;
        let bytes = self.len.get(off..off + 4)?;
        Some(u32::from_le_bytes(bytes.try_into().ok()?))
    }

    /// Number of documents in the segment.
    pub fn doc_count(&self) -> usize {
        self.len.len() / 4
    }

    /// The term's dictionary entry, exactly what the FST lookup yields.
    pub fn lookup(&self, term: &str) -> Result<Option<TermMeta>, FormatError> {
        let fst = dic_fst(&self.dic)?;
        let Some(offset) = fst_get(fst, term.as_bytes())? else {
            return Ok(None);
        };
        decode_dic_meta_at(&self.dic, offset, self.flags).map(Some)
    }

    /// Decodes one term's posting list into `(row id, term frequency)` pairs,
    /// ascending by row id, with `base_row_id` added back.
    pub fn postings(&self, term: &str) -> Result<Vec<(u64, u32)>, FormatError> {
        let Some(meta) = self.lookup(term)? else {
            return Ok(Vec::new());
        };
        let start = meta.doc_start as usize;
        let end = meta.pos_end as usize;
        let slice = self.pos.get(start..end).ok_or(FormatError::Truncated {
            need: end,
            got: self.pos.len(),
        })?;
        let (framed, _) = TermPostings::decode(slice)?;
        let df = meta.doc_freq as usize;
        let mut docs: Vec<u32> = Vec::with_capacity(df);
        let mut tfs: Vec<u32> = Vec::with_capacity(df);
        let mut off = 0usize;
        let mut left = df;
        while left > 0 {
            let take = left.min(POSTING_MINI_BLOCK);
            let (d, used) = decode_u32_block(&framed.doc_list[off..], take)?;
            off += used;
            let (t, used) = decode_u32_block(&framed.doc_list[off..], take)?;
            off += used;
            docs.extend_from_slice(&d);
            tfs.extend_from_slice(&t);
            left -= take;
        }
        let mut out = Vec::with_capacity(df);
        let mut last: u32 = 0;
        for (doc, tf) in docs.into_iter().zip(tfs) {
            last = last.wrapping_add(doc);
            out.push((self.base_row_id + u64::from(last), tf));
        }
        Ok(out)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[allow(clippy::type_complexity)]
    fn sample() -> (Vec<(u64, u32)>, BTreeMap<String, Vec<(u64, u32)>>) {
        // Three documents at segment offsets 0..2, base row id 1<<32.
        let base = 1u64 << 32;
        let docs = vec![(base, 4u32), (base + 1, 6), (base + 2, 2)];
        let mut postings: BTreeMap<String, Vec<(u64, u32)>> = BTreeMap::new();
        postings.insert("alpha".into(), vec![(base, 1), (base + 2, 2)]);
        postings.insert("beta".into(), vec![(base + 1, 3)]);
        postings.insert(
            "gamma".into(),
            (0..300).map(|i| (base + i, 1)).collect::<Vec<_>>(),
        );
        postings.insert("delta".into(), vec![(base + 1, 1), (base + 2, 1)]);
        (docs, postings)
    }

    #[test]
    fn roundtrips_through_disk() {
        let dir = tempfile::tempdir().unwrap();
        let index_dir = dir.path().to_str().unwrap();
        let (docs, postings) = sample();
        let base = 1u64 << 32;
        write_segment_index(
            index_dir,
            "ft_0000000100000000",
            base,
            &docs,
            &postings,
            TermMetaFlags::default(),
        )
        .unwrap();

        let reader = SegmentIndexReader::open(
            index_dir,
            "ft_0000000100000000",
            base,
            TermMetaFlags::default(),
        )
        .unwrap();
        assert_eq!(reader.doc_count(), 3);
        assert_eq!(reader.doc_len(0), Some(4));
        assert_eq!(reader.doc_len(2), Some(2));

        assert_eq!(
            reader.postings("alpha").unwrap(),
            vec![(base, 1), (base + 2, 2)]
        );
        assert_eq!(reader.postings("beta").unwrap(), vec![(base + 1, 3)]);
        assert_eq!(reader.postings("missing").unwrap(), Vec::new());

        // More than one 128-row block exercises the per-block field split.
        let gamma = reader.postings("gamma").unwrap();
        assert_eq!(gamma.len(), 300);
        assert_eq!(gamma[0], (base, 1));
        assert_eq!(gamma[127], (base + 127, 1));
        assert_eq!(gamma[128], (base + 128, 1));
        assert_eq!(gamma[299], (base + 299, 1));

        let meta = reader.lookup("gamma").unwrap().unwrap();
        assert_eq!(meta.doc_freq, 300);
        assert_eq!(meta.total_tf, 300);
    }

    #[test]
    fn lengths_file_is_bare_u32() {
        let dir = tempfile::tempdir().unwrap();
        let index_dir = dir.path().to_str().unwrap();
        let docs = vec![(0u64, 7u32), (1, 9)];
        let postings: BTreeMap<String, Vec<(u64, u32)>> = BTreeMap::new();
        write_segment_index(
            index_dir,
            "ft_0000000000000000",
            0,
            &docs,
            &postings,
            TermMetaFlags::default(),
        )
        .unwrap();
        let bytes = std::fs::read(format!("{index_dir}/ft_0000000000000000.len")).unwrap();
        assert_eq!(bytes, vec![7, 0, 0, 0, 9, 0, 0, 0]);
        let path = std::path::Path::new(index_dir).join("ft_0000000000000000.dic");
        assert!(path.exists());
    }
}
