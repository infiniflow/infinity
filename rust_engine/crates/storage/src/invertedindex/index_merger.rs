//! The reference's merge path: `posting_merger` (`DocMerger`, `SortedPosting`,
//! `PostingDumper`, `PostingMerger`), `segment_term_posting`
//! (`SegmentTermPosting`, `SegmentTermPostingQueue`), `column_index_iterator`
//! and `column_index_merger` (`ColumnIndexMerger`).
//!
//! The reference merges N already-written segments into one, term by term:
//!
//! * `merge_base_rowid` is the **smallest** source base row id
//!   (`column_index_merger_impl.cpp:84-87`), so the merged segment's `docid_t`
//!   of a row is `row - merge_base_rowid`;
//! * a source's postings are rebased by `base_doc_id = base_row_id -
//!   merge_base_rowid` (`posting_merger_impl.cpp:183`);
//! * the merged column-length array is the sources' `.len` files concatenated
//!   at those offsets (`column_index_merger_impl.cpp:96-137`), and
//!   `PostingWriter::EndDocument` looks the length up **by merged doc id**
//!   (`posting_writer_impl.cpp:37`);
//! * every term is re-emitted through a fresh `PostingWriter`, so the
//!   frequencies are recounted from the replayed positions and the skip lists
//!   and block boundaries are rebuilt (never copied);
//! * `PostingWriter::Dump` appends the doc half and then the position half to
//!   one shared `.pos` file and fills the term's `TermMeta` offsets
//!   (`posting_writer_impl.cpp:52-63`).
//!
//! Deviation: the decoded position stream is materialised whole instead of a
//! 128-position record at a time. The bytes produced are identical (see the
//! `merge_matches_a_single_write` test); only the peak buffer differs.

use std::fs;

use crate::format::FormatError;
use crate::invertedindex::disk_segment_reader::SegmentIndexReader;
use crate::invertedindex::format::{
    build_fst, decode_column_lengths, decode_position_list, decode_u16_block, decode_u32_field,
    encode_column_lengths, encode_dic, encode_term_meta_region_with_offsets, fulltext_chunk_file,
    write_vint, DocListEncoder, DocListFormatOption, PositionListEncoder, TermMeta, TermMetaFlags,
    TermPostings, FULLTEXT_DICT_SUFFIX, FULLTEXT_LENGTH_SUFFIX, FULLTEXT_POSTING_SUFFIX,
    MAX_DOC_PER_RECORD, OF_POSITION_LIST,
};

/// One decoded doc-list mini-block (`PostingDecoder::DecodeDocList`).
///
/// `doc_ids` holds the **deltas**, like the reference: the caller accumulates
/// them into `docid_t` (`DocMerger::HasNext`)
/// (`posting_merger_impl.cpp:107`).
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct DocBlock {
    pub doc_ids: Vec<u32>,
    pub tfs: Vec<u32>,
    pub doc_payloads: Vec<u16>,
}

/// Decodes one mini-block of a doc list: the `doc_id` column, the
/// term-frequency column and, when `of_doc_payload` is set, the payload column
/// (`PostingByteSlice::DoFlush`, `posting_byte_slice_impl.cpp:18-32`;
/// `DocListEncoder::AddDocument`, `doc_list_encoder_impl.cpp:57-75`).
///
/// `off` advances past the block. The columns are framed by `IntEncoder`
/// (`u8` word count for the `u32` fields, none for the NewPForDelta `u16`
/// payload), so a block is self-delimiting.
pub fn decode_doc_block(
    doc_list: &[u8],
    off: &mut usize,
    count: usize,
    flags: TermMetaFlags,
) -> Result<DocBlock, FormatError> {
    if count == 0 {
        return Ok(DocBlock::default());
    }
    let buf = doc_list.get(*off..).ok_or(FormatError::Truncated {
        need: *off,
        got: doc_list.len(),
    })?;
    let (doc_ids, used) = decode_u32_field(buf)?;
    *off += used;
    // One frame is one flush block and the writer flushes on the 128th
    // document, so a block always holds exactly `count` entries.
    if doc_ids.len() != count {
        return Err(FormatError::Invalid(format!(
            "doc list block holds {} documents, expected {count}",
            doc_ids.len()
        )));
    }
    let tfs = if flags.has_term_frequency {
        let buf = doc_list.get(*off..).ok_or(FormatError::Truncated {
            need: *off,
            got: doc_list.len(),
        })?;
        let (tfs, used) = decode_u32_field(buf)?;
        *off += used;
        tfs
    } else {
        vec![1u32; doc_ids.len()]
    };
    if tfs.len() != count {
        return Err(FormatError::Invalid(format!(
            "term frequency block holds {} entries, expected {count}",
            tfs.len()
        )));
    }
    let doc_payloads = if flags.has_doc_payload {
        let buf = doc_list.get(*off..).ok_or(FormatError::Truncated {
            need: *off,
            got: doc_list.len(),
        })?;
        let (payloads, used) = decode_u16_block(buf, count)?;
        *off += used;
        payloads
    } else {
        Vec::new()
    };
    Ok(DocBlock {
        doc_ids,
        tfs,
        doc_payloads,
    })
}

/// `PostingDecoder` (`format/posting_decoder.cppm`): decodes one term's posting
/// slice, which is the `TermMeta` slice of the shared `.pos` file.
///
/// The reference is handed a `ByteSliceReader` per half by
/// `ColumnIndexIterator::DecodeDocList`/`DecodePosList`
/// (`column_index_iterator_impl.cpp:86-116`); this port keeps the two halves
/// owned and walks them with the same block sizes (`MAX_DOC_PER_RECORD`,
/// `MAX_POS_PER_RECORD`).
pub struct PostingDecoder {
    doc_list: Vec<u8>,
    pos_list: Vec<u8>,
    flags: TermMetaFlags,
    doc_off: usize,
    /// Documents still undecoded (`TermMeta::doc_freq`).
    doc_left: u32,
    /// The whole position stream's deltas, decoded on first use.
    positions: Option<Vec<u32>>,
    pos_at: usize,
}

impl PostingDecoder {
    /// `ColumnIndexIterator::Next`: splits `slice` (the term's `doc_start..
    /// pos_end` range of `.pos`) into the doc half and the position half and
    /// primes the two decoders.
    pub fn new(flags: TermMetaFlags, slice: &[u8], meta: &TermMeta) -> Result<Self, FormatError> {
        let (framed, _) = TermPostings::decode(slice)?;
        Ok(Self {
            doc_list: framed.doc_list,
            pos_list: framed.pos_list,
            flags,
            doc_off: 0,
            doc_left: meta.doc_freq,
            positions: None,
            pos_at: 0,
        })
    }

    /// `PostingDecoder::DecodeDocList`: the next `MAX_DOC_PER_RECORD` documents
    /// (`None` at the end).
    pub fn decode_doc_list(&mut self) -> Result<Option<DocBlock>, FormatError> {
        if self.doc_left == 0 {
            return Ok(None);
        }
        let count = (self.doc_left as usize).min(MAX_DOC_PER_RECORD);
        let block = decode_doc_block(&self.doc_list, &mut self.doc_off, count, self.flags)?;
        self.doc_left -= count as u32;
        Ok(Some(block))
    }

    /// `PostingDecoder::DecodePosList`, narrowed to one document: the next `tf`
    /// positions, as absolute offsets.
    ///
    /// The stream is delta-coded with the base reset at every `EndDocument`
    /// (`position_list_encoder_impl.cpp:37-52`), which is exactly how
    /// `DocMerger::MergePosition` accumulates it (`pos += pos_buf[i]`).
    pub fn decode_doc_positions(&mut self, tf: u32) -> Result<Vec<u32>, FormatError> {
        if tf == 0 || self.pos_list.is_empty() {
            return Ok(Vec::new());
        }
        if self.positions.is_none() {
            self.positions = Some(decode_position_list(&self.pos_list)?);
        }
        let deltas = self.positions.as_ref().expect("decoded above");
        let end = self.pos_at + tf as usize;
        let chunk = deltas.get(self.pos_at..end).ok_or(FormatError::Truncated {
            need: end,
            got: deltas.len(),
        })?;
        let mut pos = 0u32;
        let out = chunk
            .iter()
            .map(|delta| {
                pos = pos.wrapping_add(*delta);
                pos
            })
            .collect();
        self.pos_at = end;
        Ok(out)
    }
}

/// The encoding half of `PostingWriter` (`posting_writer_impl.cpp:19-92`): a
/// `DocListEncoder` plus a `PositionListEncoder` for one term, which `dump`
/// turns into the term's `.pos` slice.
///
/// `add_position` is what counts the term frequency, for both the doc list and
/// the position list, so a writer must see every occurrence
/// (`posting_writer_impl.cpp:87-92`).
pub struct PostingWriter {
    doc: DocListEncoder,
    pos: Option<PositionListEncoder>,
    current_tf: u32,
}

impl PostingWriter {
    /// `PostingWriter::PostingWriter`.
    pub fn new(flags: TermMetaFlags) -> Self {
        let option_flag = flags.option_flag();
        let pos =
            (option_flag & OF_POSITION_LIST != 0).then(|| PositionListEncoder::new(option_flag));
        Self {
            doc: DocListEncoder::new(DocListFormatOption::new(option_flag)),
            pos,
            current_tf: 0,
        }
    }

    /// `PostingWriter::AddPosition`.
    pub fn add_position(&mut self, pos: u32) {
        self.doc.add_position();
        self.current_tf += 1;
        if let Some(encoder) = &mut self.pos {
            encoder.add_position(pos);
        }
    }

    /// `PostingWriter::SetCurrentTF`: the frequency of a segment written
    /// without a position list, where it cannot be counted from occurrences
    /// (`posting_merger_impl.cpp:60-62`).
    pub fn set_current_tf(&mut self, tf: u32) {
        self.doc.set_current_tf(tf);
        self.current_tf = tf;
    }

    /// `PostingWriter::EndDocument`: closes the current document. `doc_len` is
    /// the merged column length of `doc` (`column_lengths_.Get(doc_id)`).
    pub fn end_document(&mut self, doc: u32, doc_len: u32, doc_payload: u16) {
        // The merged column length is taken verbatim (`PostingWriter::
        // EndDocument` → `column_lengths_.Get(doc_id)`,
        // `posting_writer_impl.cpp:36-42`), so the bytes match the reference for
        // any input. `DocListEncoder::AddDocument` asserts `tf <= doc_len`
        // (`doc_list_encoder_impl.cpp:60`); a source whose `.len` is shorter than
        // its postings is corrupt and must fail here rather than be silently
        // rewritten into a doc list the reference would never produce.
        self.doc.end_document(doc, doc_len, doc_payload);
        if let Some(encoder) = &mut self.pos {
            encoder.end_document();
        }
        self.current_tf = 0;
    }

    /// `PostingWriter::EndSegment`: flushes both encoders, so the skip lists
    /// hold their tail items (`posting_writer_impl.cpp:80-85`).
    pub fn end_segment(&mut self) {
        self.doc.flush();
        if let Some(encoder) = &mut self.pos {
            encoder.flush();
        }
    }

    /// `PostingWriter::GetDF` / `GetTotalTF`.
    pub fn df(&self) -> u32 {
        self.doc.get_df()
    }

    pub fn total_tf(&self) -> u32 {
        self.doc.get_total_tf()
    }

    /// `PostingWriter::Dump`: appends the doc half and the position half to
    /// `out` (the shared `.pos` file image) and fills the term's offsets
    /// (`posting_writer_impl.cpp:52-63`).
    ///
    /// The doc half is `VInt doc_skip_len, VInt doc_len, doc_skip, doc_list`,
    /// the position half `VInt pos_skip_len, VInt pos_len, pos_skip, pos_list`
    /// (`doc_list_encoder_impl.cpp:96-117`, `position_list_encoder_impl.cpp:
    /// 66-92`). The store always sets `of_position_list`, so the position half's
    /// two VInts are written even when it is empty; the reference omits them
    /// entirely only for an index without positions.
    pub fn dump(&mut self, out: &mut Vec<u8>) -> TermMeta {
        let doc_start = out.len() as u64;
        let mut meta = TermMeta {
            doc_freq: self.doc.get_df(),
            total_tf: self.doc.get_total_tf(),
            payload: 0,
            doc_start,
            pos_start: 0,
            pos_end: 0,
        };

        let mut frame = Vec::new();
        let (doc_skiplist, doc_list) = self.doc.parts();
        write_vint(&mut frame, doc_skiplist.len() as u64);
        write_vint(&mut frame, doc_list.len() as u64);
        frame.extend_from_slice(&doc_skiplist);
        frame.extend_from_slice(&doc_list);

        meta.pos_start = doc_start + frame.len() as u64;
        let (pos_skiplist, pos_list) = match &mut self.pos {
            Some(encoder) => encoder.parts(),
            None => (Vec::new(), Vec::new()),
        };
        write_vint(&mut frame, pos_skiplist.len() as u64);
        write_vint(&mut frame, pos_list.len() as u64);
        frame.extend_from_slice(&pos_skiplist);
        frame.extend_from_slice(&pos_list);

        out.extend_from_slice(&frame);
        meta.pos_end = out.len() as u64;
        meta
    }
}

/// One segment's posting list for the term currently being merged
/// (`SegmentTermPosting` + `SortedPosting`): the segment's base row id and the
/// decoder of that term's slice.
pub struct MergingPosting {
    pub base_row_id: u64,
    pub decoder: PostingDecoder,
}

/// `SegmentTermPosting` (`segment_term_posting.cppm`): one source segment while
/// the merge walks its dictionary.
///
/// `HasNext` in the reference advances the dictionary iterator by one term
/// (`segment_term_posting_impl.cpp:19-24`); the port keeps the same
/// "the object is pointing at one term" model, over the segment's FST order.
pub struct SegmentTermPosting {
    reader: SegmentIndexReader,
    terms: Vec<(String, TermMeta)>,
    at: usize,
}

impl SegmentTermPosting {
    /// Opens `{dir}/{base_name}.{dic,pos,len}` and positions before the first
    /// term.
    pub fn open(
        dir: &str,
        base_name: &str,
        base_row_id: u64,
        flags: TermMetaFlags,
    ) -> Result<Self, FormatError> {
        let reader = SegmentIndexReader::open(dir, base_name, base_row_id, flags)?;
        let terms = reader.term_entries()?;
        Ok(Self {
            reader,
            terms,
            at: usize::MAX,
        })
    }

    /// `GetBeginRowID`.
    pub fn base_row_id(&self) -> u64 {
        self.reader.base_row_id()
    }

    /// `SegmentTermPosting::HasNext`: advances to the next term.
    pub fn has_next(&mut self) -> bool {
        let next = if self.at == usize::MAX {
            0
        } else {
            self.at + 1
        };
        if next >= self.terms.len() {
            self.at = self.terms.len();
            return false;
        }
        self.at = next;
        true
    }

    /// `term_`, `None` before the first `has_next`.
    pub fn term(&self) -> Option<&str> {
        self.terms.get(self.at).map(|(term, _)| term.as_str())
    }

    pub fn meta(&self) -> Option<&TermMeta> {
        self.terms.get(self.at).map(|(_, meta)| meta)
    }

    /// The decoder of the current term (`GetPostingDecoder` after
    /// `ColumnIndexIterator::Next` primed it).
    pub fn decoder(&self) -> Result<PostingDecoder, FormatError> {
        let meta = self
            .meta()
            .ok_or_else(|| FormatError::Invalid("no current term".to_string()))?;
        let slice = self
            .reader
            .posting_slice(meta)?
            .ok_or_else(|| FormatError::Invalid("term slice outside .pos".to_string()))?;
        PostingDecoder::new(self.reader.flags(), slice, meta)
    }
}

/// `SegmentTermPostingQueue` (`segment_term_posting.cppm:41-74`): a k-way merge
/// over the sources' dictionaries.
///
/// The reference keeps a priority queue ordered by `(term, base_row_id)` and
/// pops every entry of the smallest term, which is what `next_group` returns —
/// in that same order, which `PostingMerger` relies on to rebase doc ids
/// monotonically.
pub struct SegmentTermPostingQueue {
    postings: Vec<SegmentTermPosting>,
}

impl SegmentTermPostingQueue {
    /// Opens every source and primes it with its first term; a source without
    /// any term is dropped, like the reference's constructor
    /// (`segment_term_posting_impl.cpp:31-37`).
    pub fn new(
        dir: &str,
        sources: &[(String, u64)],
        flags: TermMetaFlags,
    ) -> Result<Self, FormatError> {
        let mut postings = Vec::with_capacity(sources.len());
        for (base_name, base_row_id) in sources {
            let mut posting = SegmentTermPosting::open(dir, base_name, *base_row_id, flags)?;
            if posting.has_next() {
                postings.push(posting);
            }
        }
        Ok(Self { postings })
    }

    /// `Empty`.
    pub fn is_empty(&self) -> bool {
        self.postings.is_empty()
    }

    /// `GetCurrentMerging` + `MoveToNextTerm`: the smallest term still pending,
    /// one decoder per source that posts it (ascending `base_row_id`), and
    /// those sources advanced to their next term.
    pub fn next_group(&mut self) -> Result<Option<(String, Vec<MergingPosting>)>, FormatError> {
        if self.postings.is_empty() {
            return Ok(None);
        }
        let term = self
            .postings
            .iter()
            .filter_map(|posting| posting.term())
            .min()
            .expect("a live posting always has a term")
            .to_string();
        // `(term, base_row_id)` ascending, the comparator of the reference's
        // priority queue (`segment_term_posting.cppm:23-31`).
        let mut members: Vec<usize> = (0..self.postings.len())
            .filter(|i| self.postings[*i].term() == Some(term.as_str()))
            .collect();
        members.sort_by_key(|i| self.postings[*i].base_row_id());

        let mut group = Vec::with_capacity(members.len());
        for i in &members {
            let posting = &self.postings[*i];
            group.push(MergingPosting {
                base_row_id: posting.base_row_id(),
                decoder: posting.decoder()?,
            });
        }
        for i in &members {
            self.postings[*i].has_next();
        }
        self.postings.retain(|posting| posting.term().is_some());
        Ok(Some((term, group)))
    }
}

/// `PostingMerger` (`posting_merger_impl.cpp:171-201`): merges one term's
/// postings from several segments into a single `PostingWriter`.
pub struct PostingMerger<'a> {
    writer: PostingWriter,
    /// The **merged** segment's column lengths, indexed by merged `docid_t`
    /// (`ColumnIndexMerger`'s `column_lengths_`).
    column_lengths: &'a [u32],
}

impl<'a> PostingMerger<'a> {
    pub fn new(flags: TermMetaFlags, column_lengths: &'a [u32]) -> Self {
        Self {
            writer: PostingWriter::new(flags),
            column_lengths,
        }
    }

    /// `PostingMerger::Merge`: replays every source's postings in
    /// `base_row_id` order, rebasing each document by
    /// `base_row_id - merge_base_rowid`, and recounts the frequencies from the
    /// positions it replays (`DocMerger::Merge`/`MergePosition`,
    /// `posting_merger_impl.cpp:57-113,178-192`).
    pub fn merge(
        &mut self,
        postings: &mut [MergingPosting],
        merge_base_rowid: u64,
    ) -> Result<(), FormatError> {
        for posting in postings.iter_mut() {
            let base_doc_id =
                u32::try_from(posting.base_row_id - merge_base_rowid).map_err(|_| {
                    FormatError::Invalid(format!(
                        "source base row id {} is below the merge base {merge_base_rowid}",
                        posting.base_row_id
                    ))
                })?;
            let mut doc_id: u32 = 0;
            while let Some(block) = posting.decoder.decode_doc_list()? {
                for (slot, (delta, tf)) in block.doc_ids.iter().zip(block.tfs.iter()).enumerate() {
                    doc_id = doc_id.wrapping_add(*delta);
                    let merged_doc = base_doc_id.wrapping_add(doc_id);
                    let positions = posting.decoder.decode_doc_positions(*tf)?;
                    if positions.is_empty() {
                        // Written without a position list: the frequency cannot
                        // be recounted, so it is taken from the doc list.
                        self.writer.set_current_tf(*tf);
                    } else {
                        for pos in positions {
                            self.writer.add_position(pos);
                        }
                    }
                    // `VectorWithLock::Get` returns `u32::MAX` past the end
                    // (`vector_with_lock.cppm:40-43`), so a missing column
                    // length has to produce the same byte the reference would.
                    let doc_len = self
                        .column_lengths
                        .get(merged_doc as usize)
                        .copied()
                        .unwrap_or(u32::MAX);
                    // `doc_payload_buf_[last_doc_buf_cursor_]`: the payload is
                    // addressed by the slot inside the decoded block, not by the
                    // accumulated doc id (`posting_merger_impl.cpp:63`).
                    let payload = block.doc_payloads.get(slot).copied().unwrap_or(0);
                    self.writer.end_document(merged_doc, doc_len, payload);
                }
            }
        }
        // `EndSegment`: the flush that closes the merged segment, so the tail
        // blocks of every term get their skip-list items.
        self.writer.end_segment();
        Ok(())
    }

    /// `PostingMerger::Dump` (`posting_merger_impl.cpp:194-196`).
    pub fn dump(&mut self, out: &mut Vec<u8>) -> TermMeta {
        self.writer.dump(out)
    }

    /// `GetDF` / `GetTotalTF`.
    pub fn df(&self) -> u32 {
        self.writer.df()
    }

    pub fn total_tf(&self) -> u32 {
        self.writer.total_tf()
    }
}

/// One source of a merge: the segment's base name and its base row id
/// (`ColumnIndexMerger::Merge`'s parallel `base_names`/`base_rowids`).
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct ColumnSource {
    pub base_name: String,
    pub base_row_id: u64,
}

/// `ColumnIndexMerger` (`column_index_merger.cppm:17-38`).
pub struct ColumnIndexMerger {
    index_dir: String,
    flags: TermMetaFlags,
}

impl ColumnIndexMerger {
    pub fn new(index_dir: &str, flags: TermMetaFlags) -> Self {
        Self {
            index_dir: index_dir.to_string(),
            flags,
        }
    }

    /// `ColumnIndexMerger::Merge`: merges `sources` into
    /// `{index_dir}/{dst_base_name}.{dic,pos,len}`.
    ///
    /// The sources must come from the same segment (their row ids are within
    /// `u32` of each other), which the reference asserts by using
    /// `base_row_id - merge_base_rowid` as a `u32` (`column_index_merger_impl.
    /// cpp:91-92`).
    pub fn merge(&self, sources: &[ColumnSource], dst_base_name: &str) -> Result<(), FormatError> {
        if sources.is_empty() {
            return Ok(());
        }
        // A merged list is re-blocked from `tf` documents per mini-block, so the
        // frequencies are what delimit the position stream
        // (`DocMerger::MergePosition`, `posting_merger_impl.cpp:68-94`: without
        // `HasTfList()` its loop has no per-document stop and runs to the end of
        // the stream). The reference never builds such an index, and merging one
        // would silently collapse every document to a single occurrence, so it
        // is refused instead. Every index this store writes carries a position
        // list ([`TermMetaFlags::option_flag`] always sets `OF_POSITION_LIST`), so
        // the condition is about the frequency column, not about positions.
        if !self.flags.has_term_frequency {
            return Err(FormatError::Invalid(
                "cannot merge an index with position lists but no term frequencies: \
                 the position stream has no per-document boundary"
                    .to_string(),
            ));
        }
        fs::create_dir_all(&self.index_dir).map_err(|e| FormatError::Io(e.to_string()))?;
        let merge_base_rowid = sources
            .iter()
            .map(|source| source.base_row_id)
            .min()
            .expect("sources is not empty");

        // `prepare column length info`: each source's `.len` lands at
        // `base_row_id - merge_base_rowid` in the merged array
        // (`column_index_merger_impl.cpp:96-137`).
        let mut column_lengths: Vec<u32> = Vec::new();
        for source in sources {
            // `docid_t` is a `u32` in the reference, so a source more than
            // `u32::MAX` rows above the merge base cannot be placed at all. Check
            // before sizing the array: two sources straddling a segment boundary
            // are 2^32 apart, and `as usize` would ask for a 16 GiB vector before
            // `PostingMerger` ever sees the offset.
            let offset = u32::try_from(source.base_row_id - merge_base_rowid).map_err(|_| {
                FormatError::Invalid(format!(
                    "source base row id {} is more than u32::MAX rows above the merge base \
                     {merge_base_rowid}",
                    source.base_row_id
                ))
            })? as usize;
            let path =
                fulltext_chunk_file(&self.index_dir, &source.base_name, FULLTEXT_LENGTH_SUFFIX);
            let bytes = fs::read(&path).map_err(|e| FormatError::Io(e.to_string()))?;
            let lengths = decode_column_lengths(&bytes)?;
            if column_lengths.len() < offset + lengths.len() {
                column_lengths.resize(offset + lengths.len(), 0);
            }
            column_lengths[offset..offset + lengths.len()].copy_from_slice(&lengths);
        }

        let source_refs: Vec<(String, u64)> = sources
            .iter()
            .map(|source| (source.base_name.clone(), source.base_row_id))
            .collect();
        let mut queue = SegmentTermPostingQueue::new(&self.index_dir, &source_refs, self.flags)?;

        let mut posting: Vec<u8> = Vec::new();
        let mut metas: Vec<TermMeta> = Vec::new();
        let mut terms: Vec<Vec<u8>> = Vec::new();
        while let Some((term, mut group)) = queue.next_group()? {
            let mut merger = PostingMerger::new(self.flags, &column_lengths);
            merger.merge(&mut group, merge_base_rowid)?;
            metas.push(merger.dump(&mut posting));
            terms.push(term.into_bytes());
        }

        let (meta_region, offsets) = encode_term_meta_region_with_offsets(&metas, self.flags);
        let entries: Vec<(Vec<u8>, u64)> = terms.into_iter().zip(offsets).collect();
        let fst = build_fst(&entries);
        let dic = encode_dic(&meta_region, &fst);
        let len_bytes = encode_column_lengths(&column_lengths);

        // The three files are staged as `<final>.tmp` and renamed into place, so
        // a failure between the writes cannot leave a chunk that `load_sealed`
        // would pick up half-written (it matches `*.dic`, and `*.dic.tmp` does
        // not). The reference has the same hazard with its three `FileWriter`s;
        // the rename costs nothing and the bytes are unchanged.
        let mut staged: Vec<(std::path::PathBuf, std::path::PathBuf)> = Vec::new();
        for (suffix, bytes) in [
            (FULLTEXT_DICT_SUFFIX, &dic),
            (FULLTEXT_POSTING_SUFFIX, &posting),
            (FULLTEXT_LENGTH_SUFFIX, &len_bytes),
        ] {
            let final_path = fulltext_chunk_file(&self.index_dir, dst_base_name, suffix);
            let staging = format!("{final_path}.tmp");
            crate::format::write_synced(std::path::Path::new(&staging), bytes)
                .map_err(|e| FormatError::Io(e.to_string()))?;
            staged.push((
                std::path::PathBuf::from(staging),
                std::path::PathBuf::from(final_path),
            ));
        }
        for (staging, final_path) in staged {
            fs::rename(&staging, &final_path).map_err(|e| FormatError::Io(e.to_string()))?;
        }
        Ok(())
    }
}

#[cfg(test)]
mod tests;
