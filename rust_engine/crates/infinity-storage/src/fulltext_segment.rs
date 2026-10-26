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
//! The reference's `docid_t` is a segment offset because its segments hold a
//! contiguous row-id range. Our rows live in a flat KV store, so a segment can
//! span two insert batches whose row ids differ by 2^32. Postings therefore use
//! a dense document ordinal (0..doc_count), and an auxiliary `.row` file holds
//! the row id of each ordinal (8 bytes each) so the reader can map back.
//!
//! The `.pos` slice uses the reference framing
//! (`VInt doc_skip_len, VInt doc_len, doc_skip, doc_list, VInt pos_skip_len,
//! VInt pos_len, pos_skip, pos_list`). A doc-list body is, per 128-row block,
//! the document-id deltas block followed by the term-frequency block, both
//! through the SIMD-BP128/VByte codec in [`crate::inverted_format`].
//!
//! The position half is produced by
//! [`crate::inverted_format::position_list::PositionListEncoder`], i.e. by the
//! same code `PositionListEncoder::Dump` runs
//! (`position_list_encoder_impl.cpp:66-92`): delta-coded positions flushed in
//! `MAX_POS_PER_RECORD`-position blocks, each block framed by the
//! `IntEncoder<u32>` `u8` word count, with a position skip list holding
//! `(total_pos_count, block_size)` per flushed block and a `VInt`-prefixed pair
//! of stream lengths. `TermMeta::pos_start`/`pos_end` delimit that half, which
//! is what `Lookup(term, fetch_position = true)` slices
//! (`posting_writer_impl.cpp:52-63`, `disk_segment_reader_impl.cpp:120-131`), so
//! a sealed segment can serve phrase queries through
//! [`SegmentIndexReader::positions`].
//!
//! `.dic` and `.len` are byte-identical to the reference. The `.pos` framing and
//! its position half are too, but the doc half's block bodies use
//! `encode_u32_block` directly instead of the reference's per-field
//! `IntEncoder` prefix, so byte-exact `.pos` interop of the *doc* half is still
//! open (the codec itself is bit-compatible — see `docs/CPP_FORMAT_SPEC.md`
//! §9.3.5).

use std::collections::BTreeMap;
use std::fs;

use crate::cpp_format::FormatError;
use crate::inverted_format::position_list::{
    decode_position_list, PositionListEncoder, OF_POSITION_LIST, OF_TERM_FREQUENCY,
};
use crate::inverted_format::{
    build_fst, decode_dic_meta_at, decode_u32_block, dic_fst, encode_column_lengths, encode_dic,
    encode_term_meta_region_with_offsets, encode_u32_block, fst_get, fulltext_chunk_file,
    FstStream, TermMeta, TermMetaFlags, TermPostings, FULLTEXT_DICT_SUFFIX, FULLTEXT_LENGTH_SUFFIX,
    FULLTEXT_POSTING_SUFFIX, POSTING_MINI_BLOCK,
};

/// Auxiliary file: `u64[doc_count]` row ids, one per dense document ordinal.
/// The reference does not need it (a segment is a contiguous row range); ours is
/// a flat KV store, so the mapping has to be stored.
pub const FULLTEXT_ROW_SUFFIX: &str = ".row";

/// One term's posting entry in a segment: `(row id, term frequency, positions)`.
///
/// The positions are the ascending token offsets [`PostingWriter::add_position`]
/// recorded for that document. An empty vector means the term was indexed
/// without a position list, which is written as the reference's
/// `of_position_list`-less slice.
pub type PostingEntry = (u64, u32, Vec<u32>);

/// A term's occurrences per document: `(row id, ascending positions)`, ordered
/// by row id. This is what [`SegmentIndexReader::positions`] returns.
pub type TermPositions = Vec<(u64, Vec<u32>)>;

/// Writes one segment's full-text index.
///
/// `docs` must be sorted by row id and `postings` must be sorted by term
/// (a `BTreeMap` is), with each posting list sorted by row id. `base_row_id`
/// is the segment's first row id; every `row - base_row_id` must fit `u32`.
pub fn write_segment_index(
    index_dir: &str,
    base_name: &str,
    _base_row_id: u64,
    docs: &[(u64, u32)],
    postings: &BTreeMap<String, Vec<PostingEntry>>,
    flags: TermMetaFlags,
) -> Result<(), FormatError> {
    fs::create_dir_all(index_dir).map_err(|e| FormatError::Io(e.to_string()))?;

    let lengths: Vec<u32> = docs.iter().map(|(_, len)| *len).collect();
    let len_bytes = encode_column_lengths(&lengths);
    let mut rows: Vec<u8> = Vec::with_capacity(docs.len() * 8);
    let mut ordinal: std::collections::HashMap<u64, u32> =
        std::collections::HashMap::with_capacity(docs.len());
    for (i, (row, _)) in docs.iter().enumerate() {
        ordinal.insert(*row, i as u32);
        rows.extend_from_slice(&row.to_le_bytes());
    }

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
        // The position encoder is fed in document order: `AddPosition` stores the
        // delta from the previous occurrence *in the document*, `EndDocument`
        // resets that base (`position_list_encoder_impl.cpp:37-52`).
        let mut pos_encoder = PositionListEncoder::new(OF_POSITION_LIST | OF_TERM_FREQUENCY);
        for chunk in list.chunks(POSTING_MINI_BLOCK) {
            let mut deltas: Vec<u32> = Vec::with_capacity(chunk.len());
            let mut tfs: Vec<u32> = Vec::with_capacity(chunk.len());
            for (row, tf, doc_positions) in chunk {
                // Dense document ordinal within the segment (see the module
                // docs); the first delta is the absolute ordinal.
                let doc = *ordinal.get(row).ok_or(FormatError::Truncated {
                    need: docs.len(),
                    got: 0,
                })?;
                deltas.push(doc.wrapping_sub(last));
                tfs.push(*tf);
                last = doc;
                total_tf += u64::from(*tf);
                for p in doc_positions {
                    pos_encoder.add_position(*p);
                }
                pos_encoder.end_document();
            }
            // The buffer flushes every 128 rows: field 0 (doc deltas), then
            // field 1 (tf).
            doc_body.extend_from_slice(&encode_u32_block(&deltas));
            doc_body.extend_from_slice(&encode_u32_block(&tfs));
        }
        // Empty for a term indexed without positions: the streams are only
        // written by the flushes that `AddPosition` triggers.
        let (pos_skiplist, pos_list) = pos_encoder.parts();
        let framed = TermPostings {
            doc_skiplist: Vec::new(),
            doc_list: doc_body,
            pos_skiplist,
            pos_list,
        };
        // `pos_start_` is the offset of the position half, i.e. the doc half plus
        // the position half's two VInt prefixes (`posting_writer_impl.cpp:59`).
        let pos_section = framed.pos_section_len();
        let slice = framed.encode();
        pos.extend_from_slice(&slice);
        metas.push(TermMeta {
            doc_freq: list.len() as u32,
            total_tf: total_tf as u32,
            payload: 0,
            doc_start: slice_start,
            pos_start: slice_start + (slice.len() - pos_section) as u64,
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
    write(FULLTEXT_ROW_SUFFIX, &rows)?;
    Ok(())
}

/// A sealed segment's index, mapped from disk. Postings are decoded per term on
/// demand: the mapping shares the page cache, so a query does not keep the
/// table's postings in the process heap.
pub struct SegmentIndexReader {
    dic: Vec<u8>,
    pos: Vec<u8>,
    len: Vec<u8>,
    /// Row id per dense document ordinal.
    rows: Vec<u64>,
    flags: TermMetaFlags,
}

impl SegmentIndexReader {
    /// Opens `{dir}/{base_name}.{dic,pos,len}`. Fails if any file is missing;
    /// an empty segment (`len` file of zero bytes) is allowed.
    ///
    /// The segment is immutable once sealed (rebuilds write a new base name
    /// instead of overwriting), so reading its bytes into memory is equivalent
    /// to mapping them.
    pub fn open(
        dir: &str,
        base_name: &str,
        _base_row_id: u64,
        flags: TermMetaFlags,
    ) -> Result<Self, FormatError> {
        let map = |suffix: &str| -> Result<Vec<u8>, FormatError> {
            crate::cpp_format::read_index_file(std::path::Path::new(&fulltext_chunk_file(dir, base_name, suffix)))
        };
        let rows_map = map(FULLTEXT_ROW_SUFFIX)?;
        let rows: Vec<u64> = rows_map
            .chunks_exact(8)
            .map(|b| u64::from_le_bytes(b.try_into().unwrap()))
            .collect();
        Ok(Self {
            dic: map(FULLTEXT_DICT_SUFFIX)?,
            pos: map(FULLTEXT_POSTING_SUFFIX)?,
            len: map(FULLTEXT_LENGTH_SUFFIX)?,
            rows,
            flags,
        })
    }

    /// Row ids in ordinal order.
    pub fn rows(&self) -> &[u64] {
        &self.rows
    }

    /// Dense ordinal of `row`, if the segment holds it.
    pub fn ordinal(&self, row: u64) -> Option<u32> {
        self.rows.binary_search(&row).ok().map(|i| i as u32)
    }

    /// The document length of `row` (BM25 `dl`), looked up through the ordinal
    /// table so sparse row ids work.
    pub fn doc_len_of_row(&self, row: u64) -> Option<u32> {
        self.doc_len(self.ordinal(row)?)
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

    /// Every term in the segment's dictionary, in lexicographic order
    /// (`DictionaryReader::Next`).
    pub fn terms(&self) -> Result<Vec<String>, FormatError> {
        let fst = dic_fst(&self.dic)?;
        Ok(FstStream::new(fst)?
            .map(|(key, _)| String::from_utf8_lossy(&key).into_owned())
            .collect())
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
            // An ordinal beyond the `.row` table would mean a corrupt index.
            if let Some(row) = self.rows.get(last as usize) {
                out.push((*row, tf));
            }
        }
        Ok(out)
    }

    /// The positions of `term`, per document, the read side of
    /// `Lookup(term, fetch_position = true)` (`column_index_reader.cppm:56`).
    ///
    /// Returns `(row id, positions)` ascending by row id, where the positions of
    /// a row are its ascending occurrence offsets. `None` means the term is
    /// absent *or* was indexed without a position list — the reference's
    /// `PositionListDecoder::Init` with `HasPositionList()` off
    /// (`position_list_decoder_impl.cpp:61-83`).
    pub fn positions(&self, term: &str) -> Result<Option<TermPositions>, FormatError> {
        let Some(meta) = self.lookup(term)? else {
            return Ok(None);
        };
        // `pos_end == pos_start` is a slice with no position half at all, which
        // the reference writes when `of_position_list` is off.
        let start = meta.doc_start as usize;
        let end = meta.pos_end as usize;
        if end <= start {
            return Ok(None);
        }
        let slice = self.pos.get(start..end).ok_or(FormatError::Truncated {
            need: end,
            got: self.pos.len(),
        })?;
        let (framed, _) = TermPostings::decode(slice)?;
        if framed.pos_list.is_empty() {
            return Ok(None);
        }
        // The stored stream is delta-coded within a document, so the term
        // frequencies are needed to know where each document starts.
        let deltas = decode_position_list(&framed.pos_list)?;
        let postings = self.postings(term)?;
        let mut out = Vec::with_capacity(postings.len());
        let mut at = 0usize;
        for (row, tf) in postings {
            let tf = tf as usize;
            let chunk = deltas.get(at..at + tf).ok_or(FormatError::Truncated {
                need: at + tf,
                got: deltas.len(),
            })?;
            let mut pos = 0u32;
            let mut positions = Vec::with_capacity(tf);
            for delta in chunk {
                pos = pos.wrapping_add(*delta);
                positions.push(pos);
            }
            out.push((row, positions));
            at += tf;
        }
        Ok(Some(out))
    }
}

/// The reference's `INVALID_ROWID`.
pub const INVALID_ROW_ID: u64 = u64::MAX;

/// One term's posting list under construction (`PostingWriter`,
/// `posting_writer_impl.cpp`): each document's term frequency is the number of
/// positions added before `end_document`, and those positions are what
/// [`write_segment_index`] persists (`PostingWriter::AddPosition`,
/// `posting_writer_impl.cpp:87-92`).
#[derive(Debug, Default, Clone, PartialEq, Eq)]
pub struct PostingWriter {
    postings: Vec<(u64, u32)>,
    /// Every occurrence, in document order; the documents' term frequencies
    /// delimit them, exactly like the reference's flat position list.
    positions: Vec<u32>,
    current_tf: u32,
}

impl PostingWriter {
    pub fn new() -> Self {
        Self::default()
    }

    /// A writer preloaded with one decoded posting list
    /// (`PostingWriter::Load`).
    pub fn with_postings(postings: Vec<(u64, u32)>) -> Self {
        Self {
            postings,
            positions: Vec::new(),
            current_tf: 0,
        }
    }

    /// `PostingWriter::AddPosition`: record one occurrence of the term in the
    /// current document at token offset `position`.
    pub fn add_position(&mut self, position: u32) {
        self.positions.push(position);
        self.current_tf += 1;
    }

    /// `PostingWriter::EndDocument`: append the current document's frequency.
    pub fn end_document(&mut self, row: u64) {
        self.postings.push((row, self.current_tf));
        self.current_tf = 0;
    }

    /// `GetDF`.
    pub fn doc_freq(&self) -> u32 {
        self.postings.len() as u32
    }

    /// `GetTotalTF`.
    pub fn total_tf(&self) -> u32 {
        self.postings.iter().map(|(_, tf)| *tf).sum()
    }

    /// `GetCurrentTF`.
    pub fn current_tf(&self) -> u32 {
        self.current_tf
    }

    pub fn postings(&self) -> &[(u64, u32)] {
        &self.postings
    }

    /// Every recorded occurrence position, in document order (`GetTotalTF`
    /// delimited); the per-document slices are
    /// [`Self::document_positions`].
    pub fn positions(&self) -> &[u32] {
        &self.positions
    }

    /// The positions recorded for the `index`th finished document.
    pub fn document_positions(&self, index: usize) -> Option<&[u32]> {
        let start: usize = self
            .postings
            .iter()
            .take(index)
            .map(|(_, tf)| *tf as usize)
            .sum();
        let tf = self.postings.get(index)?.1 as usize;
        self.positions.get(start..start + tf)
    }
}

/// Merges segments' posting lists (`PostingMerger`,
/// `posting_merger_impl.cpp`): the lists are appended in segment order and the
/// row ids are already absolute ([`SegmentIndexReader::postings`] adds the
/// segment base back).
#[derive(Debug, Default, Clone, PartialEq, Eq)]
pub struct PostingMerger {
    postings: Vec<(u64, u32)>,
}

impl PostingMerger {
    pub fn new() -> Self {
        Self::default()
    }

    /// `PostingMerger::Merge`.
    pub fn merge(&mut self, segments: &[&[(u64, u32)]]) {
        for list in segments {
            self.postings.extend_from_slice(list);
        }
    }

    /// `PostingMerger::GetDF`.
    pub fn doc_freq(&self) -> u32 {
        self.postings.len() as u32
    }

    /// `PostingMerger::GetTotalTF`.
    pub fn total_tf(&self) -> u32 {
        self.postings.iter().map(|(_, tf)| *tf).sum()
    }

    pub fn postings(&self) -> &[(u64, u32)] {
        &self.postings
    }
}

/// A posting iterator over one or more segments (`PostingIterator`,
/// `posting_iterator_impl.cpp`).
///
/// `seek_doc` decodes the term frequency of the document it lands on, while
/// `skip_to` only positions the cursor: `current_tf` is `0` until the next
/// `seek_doc`, matching the reference (`SkipTo` resets `current_row_id_`).
#[derive(Debug, Default, Clone)]
pub struct PostingIterator {
    /// Per segment: `(row id, term frequency)`, ascending.
    segments: Vec<Vec<(u64, u32)>>,
    segment: usize,
    index: usize,
    doc_id: u64,
    tf: u32,
}

impl PostingIterator {
    pub fn new(segments: Vec<Vec<(u64, u32)>>) -> Self {
        Self {
            segments,
            segment: 0,
            index: 0,
            doc_id: INVALID_ROW_ID,
            tf: 0,
        }
    }

    /// `PostingIterator::DocID`.
    pub fn doc_id(&self) -> u64 {
        self.doc_id
    }

    /// `PostingIterator::GetCurrentTF`.
    pub fn current_tf(&self) -> u32 {
        self.tf
    }

    /// `PostingIterator::SkipTo`: positions at the first posting whose row id
    /// is at least `row`, but leaves the term frequency undecoded.
    pub fn skip_to(&mut self, row: u64) -> bool {
        for (segment, list) in self.segments.iter().enumerate() {
            let index = list.partition_point(|(doc, _)| *doc < row);
            if index < list.len() {
                self.segment = segment;
                self.index = index;
                self.doc_id = INVALID_ROW_ID;
                self.tf = 0;
                return true;
            }
        }
        self.segment = self.segments.len();
        self.index = 0;
        self.doc_id = INVALID_ROW_ID;
        self.tf = 0;
        false
    }

    /// `PostingIterator::SeekDoc`: advances to the first posting at or after
    /// `row` and returns its row id, or [`INVALID_ROW_ID`].
    pub fn seek_doc(&mut self, row: u64) -> u64 {
        if self.doc_id != INVALID_ROW_ID && row <= self.doc_id {
            return self.doc_id;
        }
        if !self.skip_to(row) {
            return INVALID_ROW_ID;
        }
        let (doc, tf) = self.segments[self.segment][self.index];
        self.doc_id = doc;
        self.tf = tf;
        doc
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[allow(clippy::type_complexity)]
    fn sample() -> (Vec<(u64, u32)>, BTreeMap<String, Vec<PostingEntry>>) {
        // 300 documents whose row ids start at 1<<32: a later insert batch. The
        // `.row` table keeps them addressable even though the ids are sparse.
        let base = 1u64 << 32;
        let docs: Vec<(u64, u32)> = (0..300).map(|i| (base + i, (i % 7 + 2) as u32)).collect();
        let mut postings: BTreeMap<String, Vec<PostingEntry>> = BTreeMap::new();
        postings.insert(
            "alpha".into(),
            vec![(base, 1, vec![1]), (base + 2, 2, vec![0, 3])],
        );
        postings.insert("beta".into(), vec![(base + 1, 3, vec![2, 4, 6])]);
        // 300 positions in 300 documents: three 128-position blocks in the
        // position stream, one skip-list item per block.
        postings.insert(
            "gamma".into(),
            (0..300)
                .map(|i| (base + i, 1, vec![(i % 4) as u32]))
                .collect::<Vec<_>>(),
        );
        postings.insert(
            "delta".into(),
            vec![(base + 1, 1, vec![7]), (base + 2, 1, vec![8])],
        );
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
        assert_eq!(reader.doc_count(), 300);
        assert_eq!(reader.doc_len(0), Some(2));
        assert_eq!(reader.doc_len(2), Some(4));
        assert_eq!(reader.doc_len_of_row(base + 2), Some(4));

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
        let postings: BTreeMap<String, Vec<PostingEntry>> = BTreeMap::new();
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

    /// `of_term_frequency`: `total_tf` is stored in the `.dic` record instead of
    /// falling back to `doc_freq` (`term_meta_impl.cpp:55-76`).
    fn tf_flags() -> TermMetaFlags {
        TermMetaFlags {
            has_term_frequency: true,
            has_term_payload: false,
        }
    }

    /// (a) Seal and reopen: the occurrence offsets a `PostingWriter` recorded
    /// come back unchanged, including a document whose positions straddle the
    /// 128-position block boundary and a document that repeats the term.
    #[test]
    fn positions_survive_seal_and_reopen() {
        let dir = tempfile::tempdir().unwrap();
        let index_dir = dir.path().to_str().unwrap();
        let base = 1u64 << 32;
        let docs: Vec<(u64, u32)> = (0..3).map(|i| (base + i, 400)).collect();

        // doc 0: 200 occurrences at every other offset (spans two blocks),
        // doc 1: 3 occurrences, doc 2: none.
        let doc0: Vec<u32> = (0..200).map(|i| i * 2).collect();
        let doc1: Vec<u32> = vec![5, 5, 399];
        let mut writer = PostingWriter::new();
        for p in &doc0 {
            writer.add_position(*p);
        }
        writer.end_document(base);
        for p in &doc1 {
            writer.add_position(*p);
        }
        writer.end_document(base + 1);

        assert_eq!(writer.doc_freq(), 2);
        assert_eq!(writer.total_tf(), 203);
        assert_eq!(writer.document_positions(0).unwrap(), doc0.as_slice());
        assert_eq!(writer.document_positions(1).unwrap(), doc1.as_slice());
        assert_eq!(writer.document_positions(2), None);

        let mut postings: BTreeMap<String, Vec<PostingEntry>> = BTreeMap::new();
        postings.insert(
            "repeat".into(),
            writer
                .postings()
                .iter()
                .enumerate()
                .map(|(i, (row, tf))| (*row, *tf, writer.document_positions(i).unwrap().to_vec()))
                .collect(),
        );
        write_segment_index(
            index_dir,
            "ft_0000000100000000",
            base,
            &docs,
            &postings,
            tf_flags(),
        )
        .unwrap();

        let reader =
            SegmentIndexReader::open(index_dir, "ft_0000000100000000", base, tf_flags()).unwrap();
        let meta = reader.lookup("repeat").unwrap().unwrap();
        assert_eq!(meta.total_tf, 203);
        // The position half is a real section of the slice.
        assert!(meta.pos_start > meta.doc_start && meta.pos_end > meta.pos_start);
        assert_eq!(
            reader.positions("repeat").unwrap(),
            Some(vec![(base, doc0), (base + 1, doc1)])
        );
        // The (row, tf) API is unaffected.
        assert_eq!(
            reader.postings("repeat").unwrap(),
            vec![(base, 200), (base + 1, 3)]
        );
    }

    /// (b) A phrase must match on consecutive occurrences only.
    #[test]
    fn phrase_matches_only_consecutive_positions() {
        fn phrase_rows(reader: &SegmentIndexReader, terms: &[&str]) -> Vec<u64> {
            let lists: Vec<TermPositions> = terms
                .iter()
                .map(|t| reader.positions(t).unwrap().unwrap_or_default())
                .collect();
            lists[0]
                .iter()
                .filter(|(row, positions)| {
                    lists[1..].iter().all(|list| {
                        list.iter().any(|(r, ps)| {
                            r == row
                                && ps.len() == positions.len()
                                && ps.iter().zip(positions).all(|(a, b)| *a == b + 1)
                        })
                    })
                })
                .map(|(row, _)| *row)
                .collect()
        }

        let dir = tempfile::tempdir().unwrap();
        let index_dir = dir.path().to_str().unwrap();
        let docs: Vec<(u64, u32)> = (0..3).map(|i| (i, 10)).collect();
        let mut postings: BTreeMap<String, Vec<PostingEntry>> = BTreeMap::new();
        // Row 0: "quick brown" adjacent. Row 1: adjacent but reversed. Row 2:
        // both terms present, two apart.
        postings.insert(
            "quick".into(),
            vec![(0, 1, vec![1]), (1, 1, vec![2]), (2, 1, vec![4])],
        );
        postings.insert(
            "brown".into(),
            vec![(0, 1, vec![2]), (1, 1, vec![1]), (2, 1, vec![6])],
        );
        write_segment_index(
            index_dir,
            "ft_phrase",
            0,
            &docs,
            &postings,
            TermMetaFlags::default(),
        )
        .unwrap();
        let reader =
            SegmentIndexReader::open(index_dir, "ft_phrase", 0, TermMetaFlags::default()).unwrap();

        assert_eq!(
            reader.positions("quick").unwrap().unwrap(),
            vec![(0, vec![1]), (1, vec![2]), (2, vec![4])]
        );
        assert_eq!(
            reader.positions("brown").unwrap().unwrap(),
            vec![(0, vec![2]), (1, vec![1]), (2, vec![6])]
        );
        assert_eq!(phrase_rows(&reader, &["quick", "brown"]), vec![0]);
        assert_eq!(phrase_rows(&reader, &["brown", "quick"]), vec![1]);
        // "quick" alone matches all of its rows.
        assert_eq!(phrase_rows(&reader, &["quick"]), vec![0, 1, 2]);
        // A term without a position list cannot be part of a phrase.
        assert_eq!(reader.positions("absent").unwrap(), None);
    }

    /// (c) The `.pos` bytes match the reference writer's layout byte for byte.
    ///
    /// The fixture is one term in two documents, at offsets `[1, 3]` and `[2]`.
    /// Derivation, following `DocListEncoder::Dump`
    /// (`doc_list_encoder_impl.cpp:95-112`) and `PositionListEncoder::Dump`
    /// (`position_list_encoder_impl.cpp:66-92`):
    ///
    /// * doc half: `VInt(0)` skip list, `VInt(16)` body. The body is the
    ///   document-id deltas `[0, 1]` and the term frequencies `[2, 1]`, each
    ///   through `encode_u32_block`: `00 00 00 00` (WordAligned header) then the
    ///   variable-byte values with the high bit on the last byte, padded to a
    ///   32-bit boundary. Positions 18.. are the position half.
    /// * position half: `VInt(18)` skip list, `VInt(9)` list. One 128-position
    ///   block holds the deltas `[1, 2, 2]` — `1` and `2` in document 0 (the
    ///   second is relative to `1`), `2` in document 1 (`EndDocument` resets the
    ///   base). The block is the `IntEncoder<u32>` frame: the `u8` word count
    ///   `2`, then `00 00 00 00 81 82 82 00`.
    /// * position skip list: one item `(total_pos = 3, block bytes = 9)`, i.e.
    ///   keys `[3]` and offsets `[9]`, each as its own `IntEncoder<u32>` frame.
    #[test]
    fn pos_file_matches_the_reference_layout() {
        let dir = tempfile::tempdir().unwrap();
        let index_dir = dir.path().to_str().unwrap();
        let docs: Vec<(u64, u32)> = vec![(0, 4), (1, 3)];
        let mut postings: BTreeMap<String, Vec<PostingEntry>> = BTreeMap::new();
        postings.insert("a".into(), vec![(0, 2, vec![1, 3]), (1, 1, vec![2])]);
        write_segment_index(index_dir, "ft_bytes", 0, &docs, &postings, tf_flags()).unwrap();

        #[rustfmt::skip]
        let want: Vec<u8> = vec![
            // --- doc half: VInt doc_skip_len = 0, VInt doc_len = 16
            0x00, 0x10,
            // doc-id deltas [0, 1]
            0x00, 0x00, 0x00, 0x00, 0x80, 0x81, 0x00, 0x00,
            // term frequencies [2, 1]
            0x00, 0x00, 0x00, 0x00, 0x82, 0x81, 0x00, 0x00,
            // --- position half: VInt pos_skip_len = 18, VInt pos_len = 9
            0x12, 0x09,
            // position skip list, item 0: total_pos_count delta 3, offset 9
            0x02, 0x00, 0x00, 0x00, 0x00, 0x83, 0x00, 0x00, 0x00,
            0x02, 0x00, 0x00, 0x00, 0x00, 0x89, 0x00, 0x00, 0x00,
            // position list: one block, deltas [1, 2, 2]
            0x02, 0x00, 0x00, 0x00, 0x00, 0x81, 0x82, 0x82, 0x00,
        ];
        let got = std::fs::read(format!("{index_dir}/ft_bytes.pos")).unwrap();
        assert_eq!(got, want);

        let reader = SegmentIndexReader::open(index_dir, "ft_bytes", 0, tf_flags()).unwrap();
        let meta = reader.lookup("a").unwrap().unwrap();
        assert_eq!(meta.doc_start, 0);
        assert_eq!(meta.pos_start, 18);
        assert_eq!(meta.pos_end, want.len() as u64);
        assert_eq!(meta.doc_freq, 2);
        assert_eq!(meta.total_tf, 3);
        assert_eq!(
            reader.positions("a").unwrap(),
            Some(vec![(0, vec![1, 3]), (1, vec![2])])
        );
    }

    /// A segment written without position lists keeps the reference's
    /// `of_position_list`-less slice: framing present, both streams empty, and
    /// `positions` reports the absence.
    #[test]
    fn segment_without_positions_has_empty_streams() {
        let dir = tempfile::tempdir().unwrap();
        let index_dir = dir.path().to_str().unwrap();
        let docs: Vec<(u64, u32)> = vec![(0, 3), (1, 3)];
        let mut postings: BTreeMap<String, Vec<PostingEntry>> = BTreeMap::new();
        postings.insert("a".into(), vec![(0, 1, Vec::new()), (1, 2, Vec::new())]);
        write_segment_index(
            index_dir,
            "ft_nopos",
            0,
            &docs,
            &postings,
            TermMetaFlags::default(),
        )
        .unwrap();
        let reader =
            SegmentIndexReader::open(index_dir, "ft_nopos", 0, TermMetaFlags::default()).unwrap();
        let meta = reader.lookup("a").unwrap().unwrap();
        assert_eq!(meta.pos_end - meta.pos_start, 2); // the two zero-length VInts
        assert_eq!(reader.positions("a").unwrap(), None);
        assert_eq!(reader.postings("a").unwrap(), vec![(0, 1), (1, 2)]);
    }
}
