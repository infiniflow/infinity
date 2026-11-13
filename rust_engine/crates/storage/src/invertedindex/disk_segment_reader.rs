//! Segment-level full-text index: one inverted index per segment, in the two
//! forms the reference keeps (`memory_indexer*`, `disk_segment_reader*`).
//!
//! A table's rows are split into segments; each segment owns one full-text
//! index. While a segment is being built its index lives in memory; once it is
//! sealed the index is written as the C++ file family and queried through
//! `mmap`, so the resident set no longer grows with the table:
//!
//!   * `{index_dir}/{base_name}.dic` — TermMeta records followed by the FST
//!     mapping `term -> TermMeta offset` ([`crate::invertedindex::format::encode_dic`]).
//!   * `{index_dir}/{base_name}.pos` — the per-term postings slices.
//!   * `{index_dir}/{base_name}.len` — `u32[row_count]` document lengths.
//!
//! `docid_t` is a segment offset, exactly like the reference: a segment covers
//! the contiguous row-id range `base_row_id..base_row_id + doc_count`, and a
//! posting's id is `row - base_row_id`. A row that carries no text (the
//! reference's `InsertGap`) is a document with no posting and a zero length, so
//! no row-id table is needed on disk - the three files above are the whole
//! segment, byte for byte what the reference writes.
//!
//! The `.pos` slice uses the reference framing
//! (`VInt doc_skip_len, VInt doc_len, doc_skip, doc_list, VInt pos_skip_len,
//! VInt pos_len, pos_skip, pos_list`). A doc-list body is, per 128-row block,
//! the document-id deltas block followed by the term-frequency block, each
//! through `IntEncoder<u32>` — a `u8` word count and the SIMD-BP128 payload
//! (`int_encoder.cppm:92-99`, `PostingByteSlice::DoFlush`
//! `posting_byte_slice_impl.cpp:18-32`) — and, when `of_doc_payload` is set, a
//! payload block through `IntEncoder<u16>`/NewPForDelta, which has no word
//! count prefix (`int_encoder.cppm:47-54`).
//!
//! The position half is produced by
//! [`crate::invertedindex::format::position_list::PositionListEncoder`], i.e. by the
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
//! `.dic` and `.len` are byte-identical to the reference, and so is `.pos`: the
//! doc half is written by [`crate::invertedindex::format::DocListEncoder`],
//! which applies the same `IntEncoder` field framing as `DocListEncoder::Dump`
//! (`doc_list_encoder_impl.cpp:96-117`), and the position half by the port's
//! `PositionListEncoder` (see `docs/CPP_FORMAT_SPEC.md` §9.3.5).

use std::collections::BTreeMap;
use std::fs;

use crate::invertedindex::chunk_manifest::{ChunkManifest, LiveStats};
use crate::mapped::MappedFile;

use crate::format::FormatError;
use crate::invertedindex::format::position_list::decode_position_list;
use crate::invertedindex::format::{
    build_fst, decode_dic_meta_at, decode_u32_block_all, dic_fst, encode_column_lengths,
    encode_dic, encode_term_meta_region_with_offsets, fst_get, fulltext_chunk_file,
    DocListFormatOption, FstStream, SkipListReader, TermMeta, TermMetaFlags, TermPostings,
    FULLTEXT_DICT_SUFFIX, FULLTEXT_LENGTH_SUFFIX, FULLTEXT_POSTING_SUFFIX, POSTING_MINI_BLOCK,
};
use crate::invertedindex::index_merger::{decode_doc_block, PostingWriter};

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
///
/// This is the reference's single-source case: one `PostingWriter` per term,
/// exactly the objects [`crate::invertedindex::index_merger::ColumnIndexMerger`]
/// drives for a multi-source merge, so both paths write the same bytes.
pub fn write_segment_index(
    index_dir: &str,
    base_name: &str,
    base_row_id: u64,
    docs: &[(u64, u32)],
    postings: &BTreeMap<String, Vec<PostingEntry>>,
    flags: TermMetaFlags,
) -> Result<(), FormatError> {
    write_segment_index_inner(
        index_dir,
        base_name,
        base_row_id,
        docs,
        postings,
        flags,
        &|_, _| 0,
    )
}

/// Same as [`write_segment_index`], with the per-`(term, row)` **doc payload**
/// (`DocListEncoder::AddDocument`'s third column) taken from `payloads`.
///
/// The reference writes that column when `of_doc_payload` is set; a
/// `rankfeatures` index uses it to carry the feature weight. The merge copies it
/// per `(term, document)`, so this entry point is what makes that dimension
/// testable against the reference's bytes.
pub fn write_segment_index_with_payloads(
    index_dir: &str,
    base_name: &str,
    base_row_id: u64,
    docs: &[(u64, u32)],
    postings: &BTreeMap<String, Vec<PostingEntry>>,
    payloads: &BTreeMap<String, BTreeMap<u64, u16>>,
    flags: TermMetaFlags,
) -> Result<(), FormatError> {
    let payload_of = |term: &str, row: u64| -> u16 {
        payloads
            .get(term)
            .and_then(|rows| rows.get(&row))
            .copied()
            .unwrap_or(0)
    };
    write_segment_index_inner(
        index_dir,
        base_name,
        base_row_id,
        docs,
        postings,
        flags,
        &payload_of,
    )
}

fn write_segment_index_inner(
    index_dir: &str,
    base_name: &str,
    base_row_id: u64,
    docs: &[(u64, u32)],
    postings: &BTreeMap<String, Vec<PostingEntry>>,
    flags: TermMetaFlags,
    payload_of: &dyn Fn(&str, u64) -> u16,
) -> Result<(), FormatError> {
    write_segment_index_streaming(
        index_dir,
        base_name,
        base_row_id,
        docs,
        flags,
        &mut |emit| {
            for (term, list) in postings {
                let payload = |row: u64| payload_of(term, row);
                emit(term, list, &payload);
            }
        },
    )
}

/// Writes the three files of a segment while the caller produces one term at a
/// time, in ascending byte order.
///
/// This is the reference's `SortMergerTermTuple` shape (`memory_indexer_impl.cpp`
/// `OfflineDump`): the term metadata and the FST dictionary still accumulate
/// (they are per term, not per posting), but only one term's postings exist at a
/// time, so sealing a large tail never needs the whole segment's postings in the
/// heap. `write_segment_index` drives the same writer from a materialised map, so
/// both paths emit identical bytes.
/// One term's postings plus the payload lookup for its rows: what
/// [`write_segment_index_streaming`] hands to the callback it is given.
pub type TermSink<'a> = dyn FnMut(&str, &[PostingEntry], &dyn Fn(u64) -> u16) + 'a;

/// One mini-block's decoded columns: `doc_id` deltas, frequencies, doc payloads.
pub type DecodedDocBlock = (Vec<u32>, Vec<u32>, Vec<u16>);

/// How many terms one seal batch encodes before the writer appends them.
///
/// Encoding is parallel, appending is sequential, and the batch bounds how much
/// encoded output is in flight: the streaming shape's point was that sealing a
/// large tail never holds the whole segment's postings, and a batch keeps that
/// true while giving the pool something to do.
const SEAL_BATCH_TERMS: usize = 256;

/// One term waiting to be encoded: its name, postings and per-document payloads.
///
/// The payloads are resolved when the term is produced (`payload_of` borrows the
/// caller's map) because the encoder runs later, on another thread.
struct TermBatch {
    term: String,
    list: Vec<PostingEntry>,
    payloads: Vec<u16>,
}

/// Encodes `batch` (in parallel) and appends the results to `pos` in order.
///
/// The per-term `TermMeta` offsets are relative to the buffer the encoder wrote
/// into; shifting them by the position the term ends up at keeps the `.pos`
/// slice layout identical to the serial writer's - only who did the encoding
/// changes (plan 6.13.3).
#[allow(clippy::too_many_arguments)] // one argument per piece of the segment being built
fn flush_term_batch(
    batch: &mut Vec<TermBatch>,
    flags: TermMetaFlags,
    base_row_id: u64,
    doc_count: u32,
    doc_len: std::sync::Arc<std::collections::HashMap<u64, u32>>,
    pos: &mut Vec<u8>,
    metas: &mut Vec<TermMeta>,
    terms: &mut Vec<Vec<u8>>,
) -> Result<(), FormatError> {
    if batch.is_empty() {
        return Ok(());
    }
    // Pure encoding: each term is independent and writes only its own frame, so
    // a term is the CPU leaf. Each becomes a tokio task under the core bound and
    // runs on the blocking pool; the writer stays a single sequential append.
    //
    // The closure owns everything it reads (the term comes with the item, the
    // doc lengths are shared by `Arc`), so each body is `'static` and a decode
    // never borrows the caller's stack.
    let workers = std::thread::available_parallelism()
        .map(|n| n.get())
        .unwrap_or(1)
        .min(batch.len());
    let items: Vec<TermBatch> = std::mem::take(batch);
    let encode = move |item: TermBatch| -> Result<(String, Vec<u8>, TermMeta), common::Error> {
        let fail = |e: FormatError| common::Error::Invalid(e.to_string());
        let mut frame: Vec<u8> = Vec::new();
        let mut writer = PostingWriter::new(flags);
        for ((row, tf, positions), payload) in item.list.iter().zip(&item.payloads) {
            // `docid_t` is the segment offset (`row - base_row_id`), the same
            // subtraction the reference makes; no table and no search.
            let Some(doc) = row
                .checked_sub(base_row_id)
                .filter(|offset| *offset < u64::from(doc_count))
            else {
                return Err(fail(FormatError::Invalid(format!(
                    "term '{}' has a posting for row {row}, which is outside the segment \
                     starting at {base_row_id}",
                    item.term
                ))));
            };
            let doc = doc as u32;
            let len = doc_len.get(row).copied().unwrap_or(*tf);
            // `PostingWriter::AddPosition` counts the frequency and writes the
            // position stream (`posting_writer_impl.cpp:87-92`), so the two can
            // never disagree.
            if positions.is_empty() {
                // Indexed without a position list: the frequency can only come
                // from the caller (`SetCurrentTF`).
                writer.set_current_tf(*tf);
            } else if positions.len() == *tf as usize {
                for p in positions {
                    writer.add_position(*p);
                }
            } else {
                return Err(fail(FormatError::Invalid(format!(
                    "term '{}' has {} positions for document {row} but its frequency is {tf}",
                    item.term,
                    positions.len()
                ))));
            }
            writer.end_document(doc, len, *payload);
        }
        let meta = writer.dump(&mut frame);
        Ok((item.term, frame, meta))
    };
    let encoded: Vec<(String, Vec<u8>, TermMeta)> =
        index::parallel::map_blocking(items, workers, encode)
            .map_err(|e| FormatError::Invalid(e.to_string()))?;
    for (term, frame, mut meta) in encoded {
        let base = pos.len() as u64;
        meta.doc_start += base;
        meta.pos_start += base;
        meta.pos_end += base;
        pos.extend_from_slice(&frame);
        metas.push(meta);
        terms.push(term.into_bytes());
    }
    Ok(())
}

pub fn write_segment_index_streaming(
    index_dir: &str,
    base_name: &str,
    base_row_id: u64,
    docs: &[(u64, u32)],
    flags: TermMetaFlags,
    for_each_term: &mut dyn FnMut(&mut TermSink<'_>),
) -> Result<(), FormatError> {
    write_segment_index_streaming_live(
        index_dir,
        base_name,
        base_row_id,
        docs,
        flags,
        None,
        for_each_term,
    )
}

/// [`write_segment_index_streaming`] with the chunk's live statistics stated.
///
/// `docs` still describes every row the chunk spans (the `.len` slots are
/// addressed by `row - base_row_id`, so they cannot be compacted), while
/// `live` is what a predicate still sees: the token count of the documents the
/// caller kept (`sum(dl)`) and how many of them there are (BM25 `N`). `None`
/// means the chunk holds every document it lists, which is the seal path's
/// answer once deleted rows are filtered out of `docs`.
pub fn write_segment_index_streaming_live(
    index_dir: &str,
    base_name: &str,
    base_row_id: u64,
    docs: &[(u64, u32)],
    flags: TermMetaFlags,
    live: Option<LiveStats>,
    for_each_term: &mut dyn FnMut(&mut TermSink<'_>),
) -> Result<(), FormatError> {
    write_segment_index_streaming_with_live(
        index_dir,
        base_name,
        base_row_id,
        docs,
        flags,
        SEAL_BATCH_TERMS,
        live,
        for_each_term,
    )
}

/// [`write_segment_index_streaming`] with the encode batch size stated.
///
/// The batch only decides how many terms are encoded together; the bytes are
/// the same for any value, which is what `batching_does_not_change_the_bytes`
/// pins. It exists so a test can drive the extremes (one term per batch, one
/// batch for the whole segment) without a build-time knob.
pub fn write_segment_index_streaming_with(
    index_dir: &str,
    base_name: &str,
    base_row_id: u64,
    docs: &[(u64, u32)],
    flags: TermMetaFlags,
    batch_terms: usize,
    for_each_term: &mut dyn FnMut(&mut TermSink<'_>),
) -> Result<(), FormatError> {
    write_segment_index_streaming_with_live(
        index_dir,
        base_name,
        base_row_id,
        docs,
        flags,
        batch_terms,
        None,
        for_each_term,
    )
}

/// [`write_segment_index_streaming_with`] with the live statistics and the
/// publish marker of the chunk.
pub fn write_segment_index_streaming_with_live(
    index_dir: &str,
    base_name: &str,
    base_row_id: u64,
    docs: &[(u64, u32)],
    flags: TermMetaFlags,
    batch_terms: usize,
    live: Option<LiveStats>,
    for_each_term: &mut dyn FnMut(&mut TermSink<'_>),
) -> Result<(), FormatError> {
    let batch_terms = batch_terms.max(1);
    fs::create_dir_all(index_dir).map_err(|e| FormatError::Io(e.to_string()))?;

    // A segment covers the **contiguous** row-id range `base_row_id..base_row_id
    // + doc_count`, exactly like the reference's `ChunkIndexMetaInfo`: a posting's
    // `docid_t` is `row - base_row_id` and `.len` is indexed by that subtraction,
    // so a row that carries no text (the reference's `InsertGap`) is simply a
    // document with no posting and length 0 - it is still addressable. `docs`
    // only lists the rows that do carry text; the holes between them are padded
    // into `.len` here instead of being compressed away.
    if docs.windows(2).any(|pair| pair[0].0 >= pair[1].0) {
        return Err(FormatError::Invalid(
            "a segment's documents must be strictly ascending by row id".into(),
        ));
    }
    let Some((first_row, _)) = docs.first() else {
        return Err(FormatError::Invalid(
            "a segment needs at least one document".into(),
        ));
    };
    if *first_row < base_row_id {
        return Err(FormatError::Invalid(format!(
            "the segment starts at row {base_row_id} but holds a document at row {first_row}"
        )));
    }
    let last_row = docs[docs.len() - 1].0;
    // A chunk is one `base_row_id` plus one `.len` for `base_row_id..=last_row`,
    // so a set that mixes two `ROW_ID` segments (2^32 apart) is not a wide chunk
    // but a hole: it would allocate and write up to 4 G padding entries where no
    // row exists. The caller splits at the boundary (`TextIndex::claim_seal`
    // seals a tail before a row of the next segment arrives) - refuse instead of
    // padding across the gap.
    if (base_row_id >> 32) != (last_row >> 32) {
        return Err(FormatError::Invalid(format!(
            "a segment cannot span two ROW_ID segments: it starts at row {base_row_id} \
             (segment {}) and holds row {last_row} (segment {})",
            base_row_id >> 32,
            last_row >> 32
        )));
    }
    let doc_count = u32::try_from(last_row - base_row_id + 1).map_err(|_| {
        FormatError::Invalid(format!(
            "the segment at {base_row_id} spans more than u32::MAX rows"
        ))
    })?;
    // `doc_count` is the span, holes included, so a sparse segment can ask for a
    // large padding buffer. `try_reserve` turns "the span is absurd" into a
    // format error the caller can report, instead of an abort from `vec![0; n]`.
    let mut lengths: Vec<u32> = Vec::new();
    lengths
        .try_reserve_exact(doc_count as usize)
        .map_err(|error| {
            FormatError::Invalid(format!(
                "the segment at {base_row_id} would need {doc_count} length entries: {error}"
            ))
        })?;
    lengths.resize(doc_count as usize, 0);
    let mut doc_len: std::collections::HashMap<u64, u32> =
        std::collections::HashMap::with_capacity(docs.len());
    for (row, len) in docs {
        lengths[(row - base_row_id) as usize] = *len;
        doc_len.insert(*row, *len);
    }
    // Shared with every encode task instead of copied per batch.
    let doc_len = std::sync::Arc::new(doc_len);
    let len_bytes = encode_column_lengths(&lengths);

    let mut pos: Vec<u8> = Vec::new();
    let mut metas: Vec<TermMeta> = Vec::new();
    let mut terms: Vec<Vec<u8>> = Vec::new();
    // Terms are encoded as tokio tasks in batches (plan 6.13.3): the CPU
    // half - doc-list and position-list encoding - is what a large seal spends
    // its time on, while the writer itself must stay a single sequential
    // append. A batch bounds the memory at `batch_terms` terms' postings,
    // which is the same bound the streaming shape was introduced for.
    let mut batch: Vec<TermBatch> = Vec::new();
    let mut failure: Option<FormatError> = None;
    {
        let mut emit = |term: &str, list: &[PostingEntry], payload_of: &dyn Fn(u64) -> u16| {
            if failure.is_some() || list.is_empty() {
                return;
            }
            // The payload closure borrows the caller's storage, so the values
            // are taken now; encoding later must not touch it.
            let payloads: Vec<u16> = list.iter().map(|(row, _, _)| payload_of(*row)).collect();
            batch.push(TermBatch {
                term: term.to_string(),
                list: list.to_vec(),
                payloads,
            });
            if batch.len() >= batch_terms {
                if let Err(error) = flush_term_batch(
                    &mut batch,
                    flags,
                    base_row_id,
                    doc_count,
                    std::sync::Arc::clone(&doc_len),
                    &mut pos,
                    &mut metas,
                    &mut terms,
                ) {
                    failure = Some(error);
                }
            }
        };
        for_each_term(&mut emit);
    }
    if failure.is_none() {
        if let Err(error) = flush_term_batch(
            &mut batch,
            flags,
            base_row_id,
            doc_count,
            std::sync::Arc::clone(&doc_len),
            &mut pos,
            &mut metas,
            &mut terms,
        ) {
            failure = Some(error);
        }
    }
    if let Some(failure) = failure {
        return Err(failure);
    }

    let (meta_region, offsets) = encode_term_meta_region_with_offsets(&metas, flags);
    let entries: Vec<(Vec<u8>, u64)> = terms.into_iter().zip(offsets).collect();
    let fst = build_fst(&entries);
    let dic = encode_dic(&meta_region, &fst);

    // Every file goes through the chunk store, dedicated (`try_compose = false`
    // like the reference's full-text writer): a persistence profile publishes it
    // as an object of its own at the same moment, instead of a checkpoint-time
    // sweep finding it later.
    let store = crate::chunk_store::chunk_store();
    let write = |suffix: &str, bytes: &[u8]| -> Result<(), FormatError> {
        let path = fulltext_chunk_file(index_dir, base_name, suffix);
        store
            .write_dedicated(std::path::Path::new(&path), bytes)
            .map_err(|e| FormatError::Io(e.to_string()))
    };
    write(FULLTEXT_DICT_SUFFIX, &dic)?;
    write(FULLTEXT_POSTING_SUFFIX, &pos)?;
    write(FULLTEXT_LENGTH_SUFFIX, &len_bytes)?;
    // The publish marker goes last: it is what makes "all three files are on
    // the disk at these sizes" a fact the loader can check. Without it a crash
    // between the writes above leaves a `.dic` that `load_sealed` would find
    // beside the previous generation's `.pos`/`.len`.
    let live = live.unwrap_or_else(|| LiveStats::of(docs));
    ChunkManifest {
        span_docs: doc_count,
        live_docs: live.docs.min(doc_count),
        total_len: live.total_len,
        dic_len: dic.len() as u64,
        pos_len: pos.len() as u64,
        len_len: len_bytes.len() as u64,
    }
    .write(index_dir, base_name)?;
    Ok(())
}

/// A sealed segment's index, mapped from disk. Postings are decoded per term on
/// demand: the mapping shares the page cache, so a query does not keep the
/// table's postings in the process heap.
pub struct SegmentIndexReader {
    dic: MappedFile,
    pos: MappedFile,
    len: MappedFile,
    /// First row id of the segment: the reference's `base_row_id`, which makes a
    /// posting's `docid_t` equal `row - base_row_id`
    /// (`ChunkIndexMetaInfo::base_row_id_`).
    base_row_id: u64,
    flags: TermMetaFlags,
    /// The name the three files were opened under (`ft_{base:016x}` or the
    /// merged `ft_{base:016x}_{rows:x}`), which a merge needs as its source id.
    base_name: String,
}

impl SegmentIndexReader {
    /// Opens `{dir}/{base_name}.{dic,pos,len}` and validates the set.
    ///
    /// The mapping is the only `unsafe` operation in this crate: a segment is
    /// immutable once sealed, so the mapped region cannot change underneath the
    /// reader (rebuilds write a new base name instead of overwriting).
    ///
    /// Validation, in the order it costs:
    ///
    /// * `.len` must be a whole number of `u32` slots — a truncated write
    ///   leaves a partial slot, and `doc_count` is derived from that length;
    /// * `.dic` must carry a parseable FST region — a truncated dictionary
    ///   would otherwise be read as a shorter one with valid-looking offsets;
    /// * when the chunk's [`ChunkManifest`] is present, the three files must be
    ///   the exact bytes it recorded. That is what rejects a file set published
    ///   in two generations (`.dic` from a retried seal, `.pos`/`.len` left from
    ///   the previous one), which decodes into wrong rows rather than failing.
    ///
    /// A chunk with no manifest (written by the reference, or by this port
    /// before the marker existed) passes on the structural checks alone, so old
    /// data stays readable.
    pub fn open(
        dir: &str,
        base_name: &str,
        base_row_id: u64,
        flags: TermMetaFlags,
    ) -> Result<Self, FormatError> {
        // `chunk_store().open` is the resolution step of the reference's
        // `pm->GetObjCache` + `MmapFile(pm->GetObjPath(...))`: the local copy is
        // used when it is there, and the object's part is mapped when the file
        // set was only ever published to the persistence manager.
        let store = crate::chunk_store::chunk_store();
        let map = |suffix: &str| -> Result<MappedFile, FormatError> {
            let path = fulltext_chunk_file(dir, base_name, suffix);
            store
                .open(std::path::Path::new(&path))
                .map_err(|e| FormatError::Io(e.to_string()))
        };
        let len = map(FULLTEXT_LENGTH_SUFFIX)?;
        if len.len() % 4 != 0 {
            return Err(FormatError::Invalid(format!(
                "chunk '{base_name}': the .len file is {} bytes, not a whole number of u32 slots",
                len.len()
            )));
        }
        let dic = map(FULLTEXT_DICT_SUFFIX)?;
        // The FST region's own trailer is the only structural check a `.dic`
        // carries; a truncated write loses it.
        dic_fst(&dic)?;
        let pos = map(FULLTEXT_POSTING_SUFFIX)?;
        if let Some(manifest) = ChunkManifest::read(dir, base_name) {
            if !manifest.matches_files(dic.len() as u64, pos.len() as u64, len.len() as u64) {
                return Err(FormatError::Invalid(format!(
                    "chunk '{base_name}': the files are {} / {} / {} bytes, but its marker \
                     records {} / {} / {}",
                    dic.len(),
                    pos.len(),
                    len.len(),
                    manifest.dic_len,
                    manifest.pos_len,
                    manifest.len_len
                )));
            }
            if manifest.span_docs as usize != len.len() / 4 {
                return Err(FormatError::Invalid(format!(
                    "chunk '{base_name}': the marker records {} documents but .len holds {}",
                    manifest.span_docs,
                    len.len() / 4
                )));
            }
        }
        Ok(Self {
            dic,
            pos,
            len,
            base_row_id,
            flags,
            base_name: base_name.to_string(),
        })
    }

    /// Row ids in segment order: the contiguous range
    /// `base_row_id..base_row_id + doc_count`, which is the only shape the
    /// reference's segments have (there is no row table on disk).
    pub fn rows(&self) -> Vec<u64> {
        let base = self.base_row_id;
        (0..self.doc_count() as u64).map(|ord| base + ord).collect()
    }

    /// Row id of segment offset `ord`.
    pub fn row_at(&self, ord: u32) -> Option<u64> {
        (ord as usize)
            .lt(&self.doc_count())
            .then_some(self.base_row_id + u64::from(ord))
    }

    /// Segment offset (`docid_t`) of `row`, if the segment holds it.
    pub fn ordinal(&self, row: u64) -> Option<u32> {
        let doc = row.checked_sub(self.base_row_id)?;
        (doc < self.doc_count() as u64).then_some(doc as u32)
    }

    /// Ordinal (`docid_t`) of the first row the segment holds at or after
    /// `row`, `None` when it holds nothing that far out.
    ///
    /// This is the mapping a seek needs: the skip list is keyed by ordinal, and
    /// an ordinal is `row - base_row_id` **only while the segment is dense**.
    /// Deriving it from the stored rows keeps a segment that skips row ids
    /// addressable - a target inside a hole seeks to the next stored row instead
    /// of to a doc id that carries no posting.
    pub fn ordinal_at_or_after(&self, row: u64) -> Option<u32> {
        let span = self.doc_count() as u64;
        if span == 0 {
            return None;
        }
        Some(match row.checked_sub(self.base_row_id) {
            // Before the segment: its first row is the first one at or after.
            None => 0,
            Some(offset) if offset < span => offset as u32,
            Some(_) => return None,
        })
    }

    /// The document length of `row` (BM25 `dl`), addressed by its segment
    /// offset, which is the index into `.len`.
    pub fn doc_len_of_row(&self, row: u64) -> Option<u32> {
        self.doc_len(self.ordinal(row)?)
    }

    /// The document length at segment offset `doc` (the BM25 `dl`).
    pub fn doc_len(&self, doc: u32) -> Option<u32> {
        let off = doc as usize * 4;
        let bytes = self.len.get(off..off + 4)?;
        Some(u32::from_le_bytes(bytes.try_into().ok()?))
    }

    /// The row range the segment covers (first and last row id), when it holds
    /// any document. Consecutive segments are disjoint, which is what lets a
    /// query concatenate their posting lists instead of merging them.
    pub fn row_range(&self) -> Option<(u64, u64)> {
        let doc_count = self.doc_count();
        if doc_count == 0 {
            return None;
        }
        Some((self.base_row_id, self.base_row_id + doc_count as u64 - 1))
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

    /// The dictionary as `(term, TermMeta)` in lexicographic order — what
    /// `ColumnIndexIterator::Next` walks (`column_index_iterator_impl.cpp:73-84`)
    /// and therefore what a merge iterates.
    pub fn term_entries(&self) -> Result<Vec<(String, TermMeta)>, FormatError> {
        let fst = dic_fst(&self.dic)?;
        let mut out = Vec::new();
        for (key, offset) in FstStream::new(fst)? {
            let term = String::from_utf8_lossy(&key).into_owned();
            out.push((term, decode_dic_meta_at(&self.dic, offset, self.flags)?));
        }
        Ok(out)
    }

    /// This segment's base row id (`ChunkIndexMetaInfo::base_row_id_`).
    pub fn base_row_id(&self) -> u64 {
        self.base_row_id
    }

    /// The base name the segment was opened under
    /// (`ChunkIndexMetaInfo::base_name_`).
    pub fn base_name(&self) -> &str {
        &self.base_name
    }

    /// The option flags the segment is read (and was written) with.
    pub fn flags(&self) -> TermMetaFlags {
        self.flags
    }

    /// The term's `.pos` slice, i.e. `pos[doc_start..pos_end]`.
    pub fn posting_slice(&self, meta: &TermMeta) -> Result<Option<&[u8]>, FormatError> {
        let start = meta.doc_start as usize;
        let end = meta.pos_end as usize;
        if end < start {
            return Ok(None);
        }
        Ok(self.pos.get(start..end))
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
            let (d, t, _) = self.decode_doc_block(&framed.doc_list, &mut off, take)?;
            docs.extend_from_slice(&d);
            tfs.extend_from_slice(&t);
            left -= take;
        }
        let mut out = Vec::with_capacity(df);
        let mut last: u32 = 0;
        for (doc, tf) in docs.into_iter().zip(tfs) {
            last = last.wrapping_add(doc);
            // An ordinal beyond `.len` would mean a corrupt index.
            if let Some(row) = self.row_at(last) {
                out.push((row, tf));
            }
        }
        Ok(out)
    }

    /// The doc-payload column of `term`'s doc list (`of_doc_payload`, written by
    /// `DocListEncoder::add_document`): `(row id, payload)` ascending by row id.
    ///
    /// The `rankfeatures` analyzer is the only producer (its
    /// `SmallFloat::Float122ToUInt16` weight) and
    /// `RankFeatureDocIterator` the only consumer. An index without the payload
    /// column, or a term that carries no payload, yields an empty list.
    pub fn doc_payloads(&self, term: &str) -> Result<Vec<(u64, u16)>, FormatError> {
        if !DocListFormatOption::new(self.flags.option_flag()).has_doc_payload {
            return Ok(Vec::new());
        }
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
        let mut off = 0usize;
        let mut left = meta.doc_freq as usize;
        let mut out = Vec::with_capacity(left);
        let mut last: u32 = 0;
        while left > 0 {
            let take = left.min(POSTING_MINI_BLOCK);
            let (deltas, _, payloads) = self.decode_doc_block(&framed.doc_list, &mut off, take)?;
            for (delta, payload) in deltas.into_iter().zip(payloads) {
                last = last.wrapping_add(delta);
                if let Some(row) = self.row_at(last) {
                    out.push((row, payload));
                }
            }
            left -= take;
        }
        Ok(out)
    }

    /// A lazy reader over one term's doc list: the reference's
    /// `DocListReader`, decoded one 128-document mini-block at a time straight
    /// from the mmap. `postings()` above materialises the whole list; the query
    /// path uses this instead so a lookup costs `O(block)` and not `O(df)`.
    pub fn term_doc_cursor(&self, term: &str) -> Result<Option<TermDocCursor<'_>>, FormatError> {
        let Some(meta) = self.lookup(term)? else {
            return Ok(None);
        };
        let start = meta.doc_start as usize;
        let end = meta.pos_end as usize;
        let slice = self.pos.get(start..end).ok_or(FormatError::Truncated {
            need: end,
            got: self.pos.len(),
        })?;
        // Framing: `VInt doc_skip_len, VInt doc_list_len, doc_skip, doc_list, …`
        let (doc_skip_len, n1) = crate::invertedindex::format::read_vint(slice)?;
        let (doc_list_len, n2) = crate::invertedindex::format::read_vint(&slice[n1..])?;
        let body = n1 + n2 + doc_skip_len as usize;
        let doc_list =
            slice
                .get(body..body + doc_list_len as usize)
                .ok_or(FormatError::Truncated {
                    need: body + doc_list_len as usize,
                    got: slice.len(),
                })?;
        // The position half is the second framed pair of the slice
        // (`VInt pos_skip_len, VInt pos_list_len, pos_skip, pos_list`); take a
        // slice of it instead of decoding the whole list.
        let mut pos_bytes = None;
        let mut at = body + doc_list_len as usize;
        if let Ok((pos_skip_len, n3)) = crate::invertedindex::format::read_vint(&slice[at..]) {
            at += n3;
            if let Ok((pos_list_len, n4)) = crate::invertedindex::format::read_vint(&slice[at..]) {
                let start = at + n4 + pos_skip_len as usize;
                if let Some(list) = slice.get(start..start + pos_list_len as usize) {
                    if !list.is_empty() {
                        pos_bytes = Some(list);
                    }
                }
            }
        }
        // The doc half's skip list (`DocListEncoder::add_skip_list_item`), read
        // with the same format the writer used. `skip_to` turns a forward seek
        // into one block decode instead of one per block in between; a list
        // that never filled a block has no items and is left out.
        let skiplist = (doc_skip_len > 0)
            .then(|| slice.get(n1 + n2..n1 + n2 + doc_skip_len as usize))
            .flatten()
            .map(|bytes| {
                let option = DocListFormatOption::new(self.flags.option_flag());
                let mut reader = SkipListReader::new(&option.skip_list_format());
                reader.load_byte_slice(bytes, 0, bytes.len());
                Box::new(reader)
            });
        Ok(Some(TermDocCursor {
            reader: self,
            doc_list,
            off: 0,
            left: meta.doc_freq as usize,
            rows: Vec::new(),
            tfs: Vec::new(),
            ordinals: Vec::new(),
            at: 0,
            last_ordinal: 0,
            df: meta.doc_freq as usize,
            pos_bytes,
            pos_off: 0,
            pos_buf: Vec::new(),
            pos_at: 0,
            consumed: 0,
            cur_positions: Vec::new(),
            skiplist,
            skiplist_at: None,
            block_max: None,
            pos_skip: 0,
            #[cfg(test)]
            blocks_decoded: 0,
        }))
    }

    /// The row id of segment offset `doc` (`base_row_id + docid_t`).
    fn row_of_doc(&self, doc: u32) -> Option<u64> {
        self.row_at(doc)
    }

    /// Decodes one 128-document mini-block of a doc list: the `doc_id` column,
    /// the term-frequency column and the doc-payload column, each in the
    /// reference's per-field framing (`IntEncoder<u32>` writes a `u8` word
    /// count, `IntEncoder<u16>`/NewPForDelta writes none) — see
    /// `PostingByteSlice::DoFlush` (`posting_byte_slice_impl.cpp:18-32`) and
    /// `DocListEncoder::AddDocument` (`doc_list_encoder_impl.cpp:57-75`).
    ///
    /// Returnsthe `doc_id` deltas, the frequencies and the doc-payload column,
    /// and advances `off` past the block.
    fn decode_doc_block(
        &self,
        doc_list: &[u8],
        off: &mut usize,
        count: usize,
    ) -> Result<DecodedDocBlock, FormatError> {
        // One decoder for the cursor and the merger
        // (`index_merger::decode_doc_block`).
        let block = decode_doc_block(doc_list, off, count, self.flags)?;
        Ok((block.doc_ids, block.tfs, block.doc_payloads))
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

/// The reference's `DocListReader` for one term of one sealed segment
/// (`doc_list_reader.cppm`, `SkipListReader` + `DocListDecoder`): the 128-document
/// mini-blocks are decoded on demand, so `seek` costs at most one block decode
/// per call and a query that prunes never touches the rest of the list.
pub struct TermDocCursor<'a> {
    reader: &'a SegmentIndexReader,
    doc_list: &'a [u8],
    off: usize,
    /// Documents still undecoded.
    left: usize,
    /// The decoded mini-block (`row id`, `tf`, segment ordinal) and the
    /// position inside it. The ordinal indexes the segment's `.len`, so BM25's
    /// document length costs an array read instead of a search from the row id.
    rows: Vec<u64>,
    tfs: Vec<u32>,
    ordinals: Vec<u32>,
    at: usize,
    /// Running document ordinal (the store is ordinal-coded, see the module
    /// docs) and the term's total document frequency.
    last_ordinal: u32,
    df: usize,
    /// The position half of the term's `.pos` slice, decoded one record at a
    /// time (`MAX_POS_PER_RECORD` delta-coded positions per `u8`-framed record).
    pos_bytes: Option<&'a [u8]>,
    pos_off: usize,
    pos_buf: Vec<u32>,
    pos_at: usize,
    /// Documents of the loaded block whose positions were consumed.
    consumed: usize,
    /// The current document's absolute positions (reused buffer).
    cur_positions: Vec<u32>,
    /// The term's doc skip list (`DocListEncoder`'s items, one per
    /// `MAX_DOC_PER_RECORD`-document block). `None` for a list the writer never
    /// flushed a block for, i.e. one shorter than a block. Boxed: the reader's
    /// 32-item decode buffers make it larger than the whole cursor.
    skiplist: Option<Box<SkipListReader>>,
    /// The last doc id the skip list was positioned at. The reader only moves
    /// forward, so a target at or before it is walked block by block instead.
    skiplist_at: Option<u32>,
    /// The loaded block's `(block_max_tf, block_max_percentage)` **as stored by
    /// the writer** in the doc skip list (`DocListEncoder::add_skip_list_item`),
    /// read back through `SkipListReader::GetBlockMaxInfo`. `None` for a list
    /// without a skip item (shorter than one block), where the caller has to
    /// compute the bound from the decoded postings.
    block_max: Option<(u32, u16)>,
    /// Occurrences still to drop from the position stream: after a block hop
    /// the stream is re-read from its start and the skipped blocks' positions
    /// are discarded lazily, on the first `positions()` call.
    pos_skip: usize,
    /// Blocks decoded so far (the seek path's cost; tests assert on it).
    #[cfg(test)]
    blocks_decoded: usize,
}

impl<'a> TermDocCursor<'a> {
    /// `GetDF`: documents containing the term in this segment.
    pub fn df(&self) -> usize {
        self.df
    }

    /// Decodes the next mini-block. Returns false at the end of the list.
    fn fill(&mut self) -> bool {
        while self.left > 0 {
            let take = self.left.min(POSTING_MINI_BLOCK);
            let Ok((docs, tfs, _)) =
                self.reader
                    .decode_doc_block(self.doc_list, &mut self.off, take)
            else {
                self.left = 0;
                return false;
            };
            self.left -= take;
            #[cfg(test)]
            {
                self.blocks_decoded += 1;
            }
            // The position stream is sequential, so the documents this block
            // still holds consume their deltas before the block is dropped.
            self.consume_to_end();
            self.rows.clear();
            self.tfs.clear();
            self.ordinals.clear();
            self.at = 0;
            // `consume_to_end` pushed `consumed` to the *previous* block's length;
            // the new block starts its own position accounting, so a cursor that
            // crosses a block boundary naturally (no `seek`, no hop) would
            // otherwise hand back the old block's last document positions.
            self.consumed = 0;
            for (delta, tf) in docs.into_iter().zip(tfs) {
                self.last_ordinal = self.last_ordinal.wrapping_add(delta);
                if let Some(row) = self.reader.row_of_doc(self.last_ordinal) {
                    self.rows.push(row);
                    self.tfs.push(tf);
                    self.ordinals.push(self.last_ordinal);
                }
            }
            if !self.rows.is_empty() {
                // Keep the doc skip list on the block that was just decoded: an
                // item is keyed by its block's **last** doc id, so seeking to
                // this block's last ordinal lands on its own item. Only then is
                // `GetBlockMaxInfo` the bound of the postings now in `rows`.
                if let Some(skiplist) = self.skiplist.as_mut() {
                    if let Some(&block_last) = self.ordinals.last() {
                        if skiplist.current_doc_id() != block_last {
                            skiplist.skip_to(block_last);
                        }
                        let (max_tf, pct) = skiplist.block_max_info();
                        // `(0, 0)` means the list has no item here (a block the
                        // writer's flush never reached): no bound to hand out.
                        self.block_max = (max_tf > 0 && pct > 0).then_some((max_tf, pct));
                    }
                }
                return true;
            }
        }
        false
    }

    /// The loaded block's `(block_max_tf, block_max_percentage)` from the doc
    /// skip list, i.e. the values the writer recorded for this exact block
    /// (`term_doc_iterator_impl.cpp:99`, `PostingIterator::GetBlockMaxInfo`).
    pub fn block_max(&self) -> Option<(u32, u16)> {
        self.block_max
    }

    fn ensure(&mut self) -> bool {
        self.at < self.rows.len() || self.fill()
    }

    /// The rest of the decoded mini-block as the three columns the BM25 path
    /// reads: rows, term frequencies and segment ordinals. A caller that wants
    /// the whole block can copy the slices instead of pulling one posting per
    /// call; `None` once the block is exhausted.
    pub fn block(&self) -> Option<(&[u64], &[u32], &[u32])> {
        if self.at >= self.rows.len() {
            return None;
        }
        Some((
            &self.rows[self.at..],
            &self.tfs[self.at..],
            &self.ordinals[self.at..],
        ))
    }

    /// The document length of the block entry `i` positions ahead of the
    /// cursor (the same `.len` read [`Self::len`] does, without re-checking
    /// the block).
    pub fn doc_len_at(&self, i: usize) -> u32 {
        let ordinal = self.ordinals.get(self.at + i).copied().unwrap_or(u32::MAX);
        self.reader.doc_len(ordinal).unwrap_or(1)
    }

    /// Consumes `n` entries of the decoded block.
    pub fn skip(&mut self, n: usize) {
        self.at = (self.at + n).min(self.rows.len());
    }

    /// The current document (`INVALID_ROW_ID` on the reference's exhausted
    /// cursor).
    pub fn doc(&mut self) -> Option<u64> {
        if !self.ensure() {
            return None;
        }
        self.rows.get(self.at).copied()
    }

    /// The reader the cursor decodes from, for the row range a query needs to
    /// decide whether two sources can be concatenated.
    pub fn reader(&self) -> &'a SegmentIndexReader {
        self.reader
    }

    /// The current document's segment ordinal (the index into `.len`).
    pub fn ordinal(&mut self) -> u32 {
        if !self.ensure() {
            return u32::MAX;
        }
        self.ordinals.get(self.at).copied().unwrap_or(u32::MAX)
    }

    /// The current document's length (BM25 `dl`), read straight from `.len`
    /// through the ordinal the cursor already carries.
    #[allow(clippy::len_without_is_empty)] // this is BM25's `dl`, not a container size
    pub fn len(&mut self) -> u32 {
        let ordinal = self.ordinal();
        self.reader.doc_len(ordinal).unwrap_or(1)
    }

    /// The current posting in one call: row, term frequency and document
    /// length. One `ensure` and one `.len` read for the three fields every
    /// BM25 score needs, instead of one call each.
    pub fn posting(&mut self) -> Option<(u64, u32, u32)> {
        if !self.ensure() {
            return None;
        }
        let row = *self.rows.get(self.at)?;
        let tf = self.tfs.get(self.at).copied().unwrap_or(0);
        let ordinal = self.ordinals.get(self.at).copied().unwrap_or(u32::MAX);
        Some((row, tf, self.reader.doc_len(ordinal).unwrap_or(1)))
    }

    /// `GetCurrentTF` for the document [`Self::doc`] returned.
    pub fn tf(&mut self) -> u32 {
        self.ensure();
        self.tfs.get(self.at).copied().unwrap_or(0)
    }

    /// `GetCurrentPosition`: the current document's token offsets, in order.
    /// Empty when the term carries no position list.
    pub fn positions(&mut self) -> &[u32] {
        if !self.ensure() {
            return &[];
        }
        while self.consumed < self.at {
            let tf = self.tfs.get(self.consumed).copied().unwrap_or(0);
            self.consume_doc(tf);
            self.consumed += 1;
        }
        if self.consumed == self.at {
            let tf = self.tfs.get(self.at).copied().unwrap_or(0);
            self.consume_doc(tf);
            self.consumed += 1;
        }
        &self.cur_positions
    }

    /// Positions of every document of the loaded block that was not consumed
    /// yet (the block is about to be replaced).
    fn consume_to_end(&mut self) {
        while self.consumed < self.rows.len() {
            let tf = self.tfs.get(self.consumed).copied().unwrap_or(0);
            self.consume_doc(tf);
            self.consumed += 1;
        }
    }

    fn consume_doc(&mut self, tf: u32) {
        self.cur_positions.clear();
        let mut pos = 0u32;
        for _ in 0..tf {
            let Some(delta) = self.next_delta() else {
                break;
            };
            pos = pos.wrapping_add(delta);
            self.cur_positions.push(pos);
        }
    }

    /// The next delta-coded offset from the position stream, dropping the
    /// occurrences of the blocks a seek hopped over (`pos_skip`).
    fn next_delta(&mut self) -> Option<u32> {
        loop {
            while self.pos_at >= self.pos_buf.len() {
                let bytes = self.pos_bytes?;
                let words = usize::from(*bytes.get(self.pos_off)?);
                self.pos_off += 1;
                let end = self.pos_off + words * 4;
                let payload = bytes.get(self.pos_off..end)?;
                self.pos_off = end;
                self.pos_buf = decode_u32_block_all(payload).ok()?;
                self.pos_at = 0;
                if self.pos_buf.is_empty() {
                    return None;
                }
            }
            let delta = self.pos_buf[self.pos_at];
            self.pos_at += 1;
            if self.pos_skip > 0 {
                self.pos_skip -= 1;
                continue;
            }
            return Some(delta);
        }
    }

    /// `SeekDoc`: position on the first document `>= target`.
    ///
    /// Inside the decoded block this is a linear step, exactly like the
    /// reference's `PostingIterator::SkipTo`, which only leaves the block when
    /// the target is past `last_doc_id_in_current_block_`. Leaving it hops to
    /// the block that holds the target through the doc skip list
    /// (`SkipListReader::SkipTo`) instead of decoding every block in between, so
    /// a seek across a long posting list costs O(1) block decodes.
    pub fn advance(&mut self, target: u64) {
        loop {
            if !self.ensure() {
                return;
            }
            let Some(doc) = self.rows.get(self.at).copied() else {
                return;
            };
            if doc >= target {
                return;
            }
            // Still inside the loaded block, with the target in it: one step.
            if self.rows.last().is_some_and(|last| target <= *last) {
                self.at += 1;
                continue;
            }
            if !self.seek_block_of(target) {
                return;
            }
        }
    }

    /// Leaves the loaded block for the one holding `target` and decodes it.
    ///
    /// `None` for the skip list (a list the writer never filled a block for)
    /// falls back to decoding the blocks one by one, which is what this cursor
    /// always did.
    fn seek_block_of(&mut self, target: u64) -> bool {
        if self.skiplist.is_none() {
            return self.fill();
        }
        // The skip list is keyed by the segment's own doc id (`docid_t`), so the
        // target row id has to be mapped to the ordinal it occupies among the
        // segment's rows - not subtracted from `base_row_id`, which only holds
        // while the segment is dense. A target before the segment seeks to its
        // first block; one past its last row exhausts the cursor.
        let Some(query) = self.reader.ordinal_at_or_after(target) else {
            self.exhaust();
            return false;
        };
        // `SkipListReader::SkipTo` only moves forward: a target at or before the
        // block it already points at is walked through `fill`, which is the
        // reference's in-block path.
        if self.skiplist_at.is_some_and(|at| query <= at) {
            return self.fill();
        }
        let skiplist = self.skiplist.as_mut().expect("checked above");
        let Some(hit) = skiplist.skip_to(query) else {
            self.exhaust();
            return false;
        };
        // The running item count is the block ordinal the cursor landed on: a
        // sequential walk advances one item per block too, so the total stays
        // "blocks already passed", which is what the remaining count needs.
        let blocks = skiplist.get_skipped_item_count().max(0) as usize;
        let prev_ttf = skiplist.get_prev_ttf();
        self.skiplist_at = Some(hit.doc_id);
        // `offset` is where the hit's block starts (the previous block's end) and
        // its first doc id delta is relative to `prev_doc_id`.
        self.off = hit.offset as usize;
        self.last_ordinal = hit.prev_doc_id;
        self.left = self.df.saturating_sub(blocks * POSTING_MINI_BLOCK);
        self.rows.clear();
        self.tfs.clear();
        self.ordinals.clear();
        self.at = 0;
        // The position stream is sequential: it is re-read from its start and the
        // skipped blocks' occurrences dropped lazily, on the first `positions()`.
        self.reset_positions(prev_ttf);
        self.fill()
    }

    /// Drops the rest of the list (`left == 0`, no block loaded).
    fn exhaust(&mut self) {
        self.left = 0;
        self.at = 0;
        self.rows.clear();
        self.tfs.clear();
        self.ordinals.clear();
    }

    /// Rewinds the position stream and marks the first `skip` occurrences as
    /// already consumed (the blocks a seek hopped over).
    fn reset_positions(&mut self, skip: u32) {
        self.pos_off = 0;
        self.pos_buf.clear();
        self.pos_at = 0;
        self.pos_skip = skip as usize;
        self.consumed = 0;
        self.cur_positions.clear();
    }
}

/// The reference's `INVALID_ROWID`.
pub const INVALID_ROW_ID: u64 = u64::MAX;

/// A decoded posting list plus its occurrences, in row-id order.
///
/// The reference has no such type: a posting list only exists encoded
/// (`PostingWriter` on the write side,
/// [`crate::invertedindex::index_merger::PostingWriter`] here, and
/// `PostingDecoder` on the read side). This is the materialised form the
/// in-memory tests drive `PostingIterator` with.
#[derive(Debug, Default, Clone, PartialEq, Eq)]
pub struct DecodedPostingList {
    postings: Vec<(u64, u32)>,
    /// Every occurrence, in document order; the documents' term frequencies
    /// delimit them, exactly like the reference's flat position list.
    positions: Vec<u32>,
    current_tf: u32,
}

impl DecodedPostingList {
    pub fn new() -> Self {
        Self::default()
    }

    /// A list preloaded with one decoded posting list (`PostingWriter::Load`).
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

    /// A cursor that crosses a mini-block boundary naturally - no seek, no skip
    /// list hop - must hand out the **new** block's positions. `fill` used to
    /// reset the block cursor (`at`) but not the position accounting
    /// (`consumed`), so every document of the new block got the previous block's
    /// last positions; phrase / `~N` queries read exactly this.
    #[test]
    fn positions_survive_a_natural_block_boundary() {
        let dir = tempfile::tempdir().unwrap();
        let dir_str = dir.path().to_str().unwrap();
        let docs: Vec<(u64, u32)> = (0..400u64).map(|row| (row, 5)).collect();
        let postings = BTreeMap::from([(
            "t".to_string(),
            (0..400u64)
                .map(|row| (row, 1, vec![(row % 5) as u32]))
                .collect::<Vec<_>>(),
        )]);
        let flags = crate::invertedindex::format::OPTION_FLAG_ALL;
        write_segment_index(dir_str, "bnd", 0, &docs, &postings, flags).unwrap();
        let reader = SegmentIndexReader::open(dir_str, "bnd", 0, flags).unwrap();
        let mut cursor = reader.term_doc_cursor("t").unwrap().unwrap();
        while let Some(doc) = cursor.doc() {
            assert_eq!(cursor.positions(), [(doc % 5) as u32], "doc {doc}");
            cursor.skip(1);
        }
    }

    #[allow(clippy::type_complexity)]
    fn sample() -> (Vec<(u64, u32)>, BTreeMap<String, Vec<PostingEntry>>) {
        // 300 documents whose row ids start at 1<<32: a later insert batch, far
        // from 0 but still a contiguous range (`base_row_id + docid_t`).
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
        // `of_term_frequency` on: the fixture carries frequencies other than 1,
        // and without the column the doc list can only report 1 per document.
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

    /// A segment is exactly the reference's three files (`.dic`, `.pos`, `.len`)
    /// plus the publish marker.
    ///
    /// The port used to add a `.row` sidecar (a `u64[]` row-id table); the row
    /// of a document is `base_row_id + docid_t`, so no fourth *data* file may be
    /// written. `.ok` is not data: it names the three files' lengths and the
    /// chunk's live statistics, is written last, and is what tells a complete
    /// chunk from one whose publish was interrupted between the three writes.
    /// An engine that does not know it ignores it.
    #[test]
    fn a_segment_is_exactly_the_reference_file_family() {
        let dir = tempfile::tempdir().unwrap();
        let index_dir = dir.path().to_str().unwrap();
        let docs = vec![(0u64, 7u32), (1, 9)];
        let postings: BTreeMap<String, Vec<PostingEntry>> =
            BTreeMap::from([("t".to_string(), vec![(0u64, 1u32, vec![0u32])])]);
        write_segment_index(
            index_dir,
            "ft_0000000000000000",
            0,
            &docs,
            &postings,
            TermMetaFlags::default(),
        )
        .unwrap();
        let mut names: Vec<String> = std::fs::read_dir(index_dir)
            .unwrap()
            .flatten()
            .map(|entry| entry.file_name().into_string().unwrap())
            .collect();
        names.sort();
        assert_eq!(
            names,
            vec![
                "ft_0000000000000000.dic".to_string(),
                "ft_0000000000000000.len".to_string(),
                "ft_0000000000000000.ok".to_string(),
                "ft_0000000000000000.pos".to_string(),
            ]
        );
        // The marker describes the files that were just written, and every
        // document of a fresh chunk is live.
        let manifest = crate::invertedindex::chunk_manifest::ChunkManifest::read(
            index_dir,
            "ft_0000000000000000",
        )
        .expect("the chunk is published with a marker");
        let size = |suffix: &str| -> u64 {
            std::fs::metadata(crate::invertedindex::format::fulltext_chunk_file(
                index_dir,
                "ft_0000000000000000",
                suffix,
            ))
            .unwrap()
            .len()
        };
        assert!(manifest.matches_files(
            size(FULLTEXT_DICT_SUFFIX),
            size(FULLTEXT_POSTING_SUFFIX),
            size(FULLTEXT_LENGTH_SUFFIX)
        ));
        assert_eq!(manifest.span_docs, 2);
        assert_eq!(manifest.live_docs, 2);
        assert_eq!(manifest.total_len, 16);
    }

    /// A row with no text (the reference's `InsertGap`) keeps its `docid_t`: the
    /// segment covers `base_row_id..base_row_id + doc_count`, the hole has no
    /// posting and length 0, and every later row keeps `row - base_row_id`.
    #[test]
    fn a_gap_row_is_a_zero_length_document() {
        let dir = tempfile::tempdir().unwrap();
        let index_dir = dir.path().to_str().unwrap();
        // Rows 0 and 2 carry text, row 1 is a gap.
        let docs = vec![(0u64, 5u32), (2u64, 7u32)];
        let postings: BTreeMap<String, Vec<PostingEntry>> =
            BTreeMap::from([("t".to_string(), vec![(2u64, 1u32, vec![0u32])])]);
        write_segment_index(
            index_dir,
            "ft_0000000000000000",
            0,
            &docs,
            &postings,
            TermMetaFlags::default(),
        )
        .unwrap();
        let reader = SegmentIndexReader::open(
            index_dir,
            "ft_0000000000000000",
            0,
            TermMetaFlags::default(),
        )
        .unwrap();
        assert_eq!(reader.doc_count(), 3, "the span, not the document count");
        assert_eq!(reader.row_range(), Some((0, 2)));
        assert_eq!(reader.doc_len(0), Some(5));
        assert_eq!(reader.doc_len(1), Some(0), "the gap has no text");
        assert_eq!(reader.doc_len(2), Some(7));
        assert_eq!(reader.ordinal(2), Some(2), "row - base_row_id");
        assert_eq!(reader.row_at(2), Some(2));
        assert_eq!(reader.postings("t").unwrap(), vec![(2, 1)]);
    }

    /// The block bound the WAND reads is the writer's own `block_max_tf` /
    /// `block_max_percentage` (`DocListEncoder::add_skip_list_item`), carried in
    /// the doc skip list - not a value recomputed from the decoded postings.
    #[test]
    fn block_max_comes_from_the_doc_skip_list() {
        let dir = tempfile::tempdir().unwrap();
        let index_dir = dir.path().to_str().unwrap();
        let docs: Vec<(u64, u32)> = (0..300u64).map(|row| (row, 10)).collect();
        let postings: BTreeMap<String, Vec<PostingEntry>> = BTreeMap::from([(
            "t".to_string(),
            (0..300u64)
                .map(|row| (row, (row % 5 + 1) as u32, Vec::new()))
                .collect::<Vec<_>>(),
        )]);
        let flags = crate::invertedindex::format::OPTION_FLAG_ALL;
        write_segment_index(index_dir, "ft_0000000000000000", 0, &docs, &postings, flags).unwrap();
        let reader = SegmentIndexReader::open(index_dir, "ft_0000000000000000", 0, flags).unwrap();
        let mut cursor = reader.term_doc_cursor("t").unwrap().unwrap();
        let mut blocks = 0;
        loop {
            if cursor.doc().is_none() {
                break;
            }
            let Some((rows, tfs, _)) = cursor.block() else {
                break;
            };
            let count = rows.len();
            let max_tf = tfs.iter().copied().max().expect("a block has postings");
            let (stored_tf, stored_pct) = cursor.block_max().expect("a skip-list item per block");
            assert_eq!(stored_tf, max_tf, "block {blocks}: the writer's max tf");
            // Every document is 10 tokens long here, so the ratio bound is
            // `ceil(max_tf / 10 * u16::MAX)` - the value the writer stored.
            let expected = (max_tf as f32 / 10.0 * f32::from(u16::MAX)).ceil() as u16;
            assert_eq!(stored_pct, expected, "block {blocks}: stored percentage");
            blocks += 1;
            cursor.skip(count);
        }
        assert_eq!(blocks, 3, "300 postings are three 128-document blocks");
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
            has_doc_payload: false,
            has_block_max: false,
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
        let mut writer = DecodedPostingList::new();
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
            Some(vec![(base, doc0.clone()), (base + 1, doc1.clone())])
        );
        // The (row, tf) API is unaffected.
        assert_eq!(
            reader.postings("repeat").unwrap(),
            vec![(base, 200), (base + 1, 3)]
        );
        // The lazy cursor serves the same positions document by document;
        // seeking past a document consumes its share of the position stream.
        let mut cursor = reader.term_doc_cursor("repeat").unwrap().unwrap();
        assert_eq!(cursor.doc(), Some(base));
        assert_eq!(cursor.positions(), doc0.as_slice());
        assert_eq!(cursor.positions(), doc0.as_slice(), "idempotent");
        cursor.advance(base + 1);
        assert_eq!(cursor.doc(), Some(base + 1));
        assert_eq!(cursor.positions(), doc1.as_slice());
        cursor.advance(base + 2);
        assert_eq!(cursor.doc(), None);
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
    /// * doc half: `VInt(27)` skip list, `VInt(18)` body. `Dump` flushes the
    ///   encoder (`doc_list_encoder_impl.cpp:96`), so the tail block still gets
    ///   one skip item `(last_doc_id = 1, total_tf = 3, block bytes = 18)`; the
    ///   doc skip list's three `u32` rows hold the deltas `1`, `3` and the
    ///   offset `18`, each as its own `IntEncoder<u32>` frame. The body is the
    ///   document-id deltas `[0, 1]` and the term frequencies `[2, 1]`, again
    ///   through `IntEncoder<u32>` (`int_encoder.cppm:92-99`): the `u8` word
    ///   count `2`, then `encode_u32_block`'s `00 00 00 00` (WordAligned header)
    ///   and the variable-byte values with the high bit on the last byte, padded
    ///   to a 32-bit boundary. Positions 47.. are the position half: the doc
    ///   half is `2 + 27 + 18` bytes.
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
            // --- doc half: VInt doc_skip_len = 27, VInt doc_len = 18
            0x1b, 0x12,
            // doc skip list, item 0: last_doc_id 1, total_tf 3, block bytes 18
            0x02, 0x00, 0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x00,
            0x02, 0x00, 0x00, 0x00, 0x00, 0x83, 0x00, 0x00, 0x00,
            0x02, 0x00, 0x00, 0x00, 0x00, 0x92, 0x00, 0x00, 0x00,
            // doc-id deltas [0, 1]: IntEncoder<u32> word count 2
            0x02, 0x00, 0x00, 0x00, 0x00, 0x80, 0x81, 0x00, 0x00,
            // term frequencies [2, 1]: IntEncoder<u32> word count 2
            0x02, 0x00, 0x00, 0x00, 0x00, 0x82, 0x81, 0x00, 0x00,
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
        assert_eq!(meta.pos_start, 47);
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
    ///
    /// The segment has no frequency column either (`TermMetaFlags::default`),
    /// so the reference's contract applies: one occurrence per document, i.e.
    /// every recorded frequency is 1 (`DocListEncoder::AddDocument` only writes
    /// the column when `of_term_frequency` is set).
    #[test]
    fn segment_without_positions_has_empty_streams() {
        let dir = tempfile::tempdir().unwrap();
        let index_dir = dir.path().to_str().unwrap();
        let docs: Vec<(u64, u32)> = vec![(0, 3), (1, 3)];
        let mut postings: BTreeMap<String, Vec<PostingEntry>> = BTreeMap::new();
        postings.insert("a".into(), vec![(0, 1, Vec::new()), (1, 1, Vec::new())]);
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
        assert_eq!(reader.postings("a").unwrap(), vec![(0, 1), (1, 1)]);
    }

    /// A chunk is one `base_row_id` plus one `.len` for `base_row_id..=last_row`.
    /// A set that mixes two `ROW_ID` segments would therefore pad ~4 G entries of
    /// a hole that holds no row: the caller has to split at the boundary, and the
    /// writer refuses instead of allocating it.
    #[test]
    fn a_writer_refuses_rows_from_two_row_id_segments() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().to_str().unwrap();
        let docs = vec![(0u64, 1u32), ((1u64 << 32) + 5, 1u32)];
        let mut postings: BTreeMap<String, Vec<PostingEntry>> = BTreeMap::new();
        postings.insert(
            "alpha".into(),
            vec![(0, 1, vec![0]), ((1 << 32) + 5, 1, vec![0])],
        );
        let error = write_segment_index(
            path,
            "ft_mixed",
            0,
            &docs,
            &postings,
            crate::invertedindex::format::OPTION_FLAG_ALL,
        )
        .unwrap_err();
        let message = format!("{error}");
        assert!(
            message.contains("ROW_ID"),
            "the error must name the segment boundary: {message}"
        );
        // Nothing was written: a refused chunk must not leave files behind.
        assert!(!std::path::Path::new(&format!("{path}/ft_mixed.dic")).exists());
    }

    // -----------------------------------------------------------------------
    // The doc skip list on the seek path (`SkipListReader::SkipTo`)
    // -----------------------------------------------------------------------

    /// A term that occurs once in each of `n` documents, at offset `i % 5`.
    ///
    /// Written with the flags a production full-text index uses
    /// (`OPTION_FLAG_ALL`, i.e. block max included), so the doc list carries one
    /// skip-list item per `POSTING_MINI_BLOCK` documents.
    #[allow(clippy::type_complexity)]
    fn every_document(base: u64, n: u64) -> (Vec<(u64, u32)>, BTreeMap<String, Vec<PostingEntry>>) {
        let docs: Vec<(u64, u32)> = (0..n).map(|i| (base + i, 8)).collect();
        let list: Vec<PostingEntry> = (0..n)
            .map(|i| (base + i, 1, vec![(i % 5) as u32]))
            .collect();
        let mut postings: BTreeMap<String, Vec<PostingEntry>> = BTreeMap::new();
        postings.insert("alpha".into(), list);
        (docs, postings)
    }

    fn open_every_document(index_dir: &str, base: u64, n: u64) -> SegmentIndexReader {
        let (docs, postings) = every_document(base, n);
        write_segment_index(
            index_dir,
            "ft_skip",
            base,
            &docs,
            &postings,
            crate::invertedindex::format::OPTION_FLAG_ALL,
        )
        .unwrap();
        SegmentIndexReader::open(
            index_dir,
            "ft_skip",
            base,
            crate::invertedindex::format::OPTION_FLAG_ALL,
        )
        .unwrap()
    }

    /// A term that occurs once in every `stride`-th row of `n` stored rows, so
    /// the segment covers `base..base + (n - 1) * stride` while its postings
    /// only name some of those row ids.
    #[allow(clippy::type_complexity)]
    fn every_strided_document(
        base: u64,
        n: u64,
        stride: u64,
    ) -> (Vec<(u64, u32)>, BTreeMap<String, Vec<PostingEntry>>) {
        let docs: Vec<(u64, u32)> = (0..n).map(|i| (base + i * stride, 8)).collect();
        let list: Vec<PostingEntry> = (0..n)
            .map(|i| (base + i * stride, 1, vec![(i % 5) as u32]))
            .collect();
        let mut postings: BTreeMap<String, Vec<PostingEntry>> = BTreeMap::new();
        postings.insert("alpha".into(), list);
        (docs, postings)
    }

    fn open_strided_document(
        index_dir: &str,
        base: u64,
        n: u64,
        stride: u64,
    ) -> SegmentIndexReader {
        let (docs, postings) = every_strided_document(base, n, stride);
        write_segment_index(
            index_dir,
            "ft_gap",
            base,
            &docs,
            &postings,
            crate::invertedindex::format::OPTION_FLAG_ALL,
        )
        .unwrap();
        SegmentIndexReader::open(
            index_dir,
            "ft_gap",
            base,
            crate::invertedindex::format::OPTION_FLAG_ALL,
        )
        .unwrap()
    }

    /// `advance` past the loaded block hops through the skip list instead of
    /// decoding every block in between: five blocks, two decodes.
    #[test]
    fn advance_hops_blocks_with_the_skip_list() {
        let dir = tempfile::tempdir().unwrap();
        let reader = open_every_document(dir.path().to_str().unwrap(), 0, 600);
        let mut cursor = reader.term_doc_cursor("alpha").unwrap().unwrap();
        assert_eq!(cursor.doc(), Some(0));
        assert_eq!(cursor.blocks_decoded, 1, "the first block");
        cursor.advance(599);
        assert_eq!(cursor.doc(), Some(599));
        assert_eq!(
            cursor.blocks_decoded, 2,
            "the seek decoded the blocks it jumped over"
        );
    }

    /// A segment whose postings name only every other row id (a document that
    /// carries no text is a hole, not a posting) still has to land a seek on the
    /// **first stored row at or after** the target, across block boundaries.
    ///
    /// The ordinal the skip list is keyed by is not `row - base_row_id` in
    /// general; deriving it from the stored rows is what keeps the cursor from
    /// hopping past a posting that the target precedes.
    #[test]
    fn advance_lands_on_the_next_stored_row_in_a_gapped_segment() {
        let dir = tempfile::tempdir().unwrap();
        let base = 1000u64;
        // 600 stored rows, every second row id: 1000, 1002, ... 2198.
        let reader = open_strided_document(dir.path().to_str().unwrap(), base, 600, 2);
        assert_eq!(reader.doc_count(), 1199, "the span, holes included");
        assert_eq!(reader.ordinal_at_or_after(base), Some(0));
        // Inside a hole the ordinal is the hole's own offset - the cursor then
        // steps to the stored row that follows.
        assert_eq!(reader.ordinal_at_or_after(base + 999), Some(999));
        assert_eq!(reader.ordinal_at_or_after(base + 1199), None);
        assert_eq!(reader.ordinal_at_or_after(0), Some(0), "before the segment");

        // Seek across several blocks, landing inside a hole.
        let mut cursor = reader.term_doc_cursor("alpha").unwrap().unwrap();
        assert_eq!(cursor.doc(), Some(base));
        cursor.advance(base + 999);
        assert_eq!(
            cursor.doc(),
            Some(base + 1000),
            "a target inside a hole lands on the next stored row"
        );
        // The posting the seek landed on is the one a linear walk reaches too:
        // nothing between the old position and the target was skipped.
        let mut stepwise = reader.term_doc_cursor("alpha").unwrap().unwrap();
        while let Some(doc) = stepwise.doc() {
            if doc >= base + 999 {
                break;
            }
            stepwise.skip(1);
        }
        assert_eq!(stepwise.doc(), Some(base + 1000));

        // An exact stored row is not skipped either.
        cursor.advance(base + 1000);
        assert_eq!(cursor.doc(), Some(base + 1000));
        cursor.advance(base + 1100);
        assert_eq!(cursor.doc(), Some(base + 1100));
        // Past the segment's last row the cursor is exhausted, not clamped onto
        // the last posting.
        cursor.advance(base + 5000);
        assert_eq!(cursor.doc(), None);
    }

    /// The skip list is keyed by the segment's own doc id, so a segment whose
    /// rows start above 0 has to rebase the target before seeking.
    #[test]
    fn advance_rebases_the_target_on_a_sparse_segment() {
        let dir = tempfile::tempdir().unwrap();
        let base = 1u64 << 32;
        let reader = open_every_document(dir.path().to_str().unwrap(), base, 400);
        let mut cursor = reader.term_doc_cursor("alpha").unwrap().unwrap();
        cursor.advance(base + 300);
        assert_eq!(cursor.doc(), Some(base + 300));
        assert_eq!(cursor.blocks_decoded, 2);
    }

    /// Every target lands on the same document a linear walk would, and costs at
    /// most the initial block plus the one the skip list hopped to.
    #[test]
    fn advance_lands_where_a_linear_walk_does() {
        let dir = tempfile::tempdir().unwrap();
        let reader = open_every_document(dir.path().to_str().unwrap(), 0, 700);
        for target in 0..700u64 {
            let mut cursor = reader.term_doc_cursor("alpha").unwrap().unwrap();
            cursor.advance(target);
            assert_eq!(cursor.doc(), Some(target), "target {target}");
            assert!(
                cursor.blocks_decoded <= 2,
                "target {target} decoded {} blocks",
                cursor.blocks_decoded
            );
        }
        // Past the last document the cursor is exhausted.
        let mut cursor = reader.term_doc_cursor("alpha").unwrap().unwrap();
        cursor.advance(700);
        assert_eq!(cursor.doc(), None);
        // A target before the segment seeks to its first block.
        let mut cursor = reader.term_doc_cursor("alpha").unwrap().unwrap();
        cursor.advance(0);
        assert_eq!(cursor.doc(), Some(0));
    }

    /// A seek has to land on the first posting at or after the target even when
    /// the segment's rows are not all present. The chunk's `docid_t` is the dense
    /// span (`row - base_row_id`, the holes padded with length 0), so the target's
    /// ordinal is exactly that subtraction - and hopping over the blocks in
    /// between must not drop a posting that sits between the cursor and the
    /// target. The skip list is keyed by those ordinals, so this is also what
    /// makes a gapped chunk seekable at all.
    #[test]
    fn advance_over_a_gapped_segment_keeps_every_posting_between() {
        let dir = tempfile::tempdir().unwrap();
        let index_dir = dir.path().to_str().unwrap();
        // Every third row carries text: 300 documents over the 898-row span
        // `0..=897`, i.e. seven blocks with holes in them.
        let rows: Vec<u64> = (0..300u64).map(|i| i * 3).collect();
        let docs: Vec<(u64, u32)> = rows.iter().map(|row| (*row, 8)).collect();
        let list: Vec<PostingEntry> = rows.iter().map(|row| (*row, 1u32, vec![0u32])).collect();
        let postings = BTreeMap::from([("alpha".to_string(), list)]);
        write_segment_index(
            index_dir,
            "ft_gap",
            0,
            &docs,
            &postings,
            crate::invertedindex::format::OPTION_FLAG_ALL,
        )
        .unwrap();
        let reader = SegmentIndexReader::open(
            index_dir,
            "ft_gap",
            0,
            crate::invertedindex::format::OPTION_FLAG_ALL,
        )
        .unwrap();
        assert_eq!(reader.doc_count(), 898, "the span, not the 300 documents");

        // A linear pass over the whole list: what every seek has to reproduce.
        let mut expected: Vec<u64> = Vec::new();
        let mut walk = reader.term_doc_cursor("alpha").unwrap().unwrap();
        while let Some(doc) = walk.doc() {
            expected.push(doc);
            walk.advance(doc + 1);
        }
        assert_eq!(expected, rows);

        // Every row of the span, gap or not: the cursor lands on the first
        // posting at or after it, and a target past the last one exhausts.
        for target in 0..900u64 {
            let want = rows.iter().copied().find(|row| *row >= target);
            let mut cursor = reader.term_doc_cursor("alpha").unwrap().unwrap();
            cursor.advance(target);
            assert_eq!(cursor.doc(), want, "target {target}");
        }

        // A cursor that already walked into a block and then seeks further must
        // land on the target, not past it.
        let mut cursor = reader.term_doc_cursor("alpha").unwrap().unwrap();
        assert_eq!(cursor.doc(), Some(0));
        cursor.advance(301);
        assert_eq!(cursor.doc(), Some(303));
        cursor.advance(447);
        assert_eq!(cursor.doc(), Some(447));
        cursor.advance(897);
        assert_eq!(cursor.doc(), Some(897));
    }

    /// The position stream is sequential, so a hop has to drop the occurrences of
    /// the blocks it skipped: `positions()` after a seek must still be the
    /// document's own offsets.
    #[test]
    fn positions_survive_a_block_hop() {
        let dir = tempfile::tempdir().unwrap();
        let reader = open_every_document(dir.path().to_str().unwrap(), 0, 500);
        let mut cursor = reader.term_doc_cursor("alpha").unwrap().unwrap();
        cursor.advance(301);
        assert_eq!(cursor.doc(), Some(301));
        assert_eq!(cursor.positions(), &[301 % 5]);
        cursor.advance(399);
        assert_eq!(cursor.doc(), Some(399));
        assert_eq!(cursor.positions(), &[399 % 5]);
        // Back inside the same block: linear steps keep the stream aligned.
        cursor.advance(400);
        assert_eq!(cursor.positions(), &[0]);
    }

    /// A list shorter than one block still gets a single skip-list item (the
    /// encoder flushes its partial block at `Dump`), and a seek inside that one
    /// block never hops.
    #[test]
    fn a_short_list_still_advances() {
        let dir = tempfile::tempdir().unwrap();
        let reader = open_every_document(dir.path().to_str().unwrap(), 0, 50);
        let mut cursor = reader.term_doc_cursor("alpha").unwrap().unwrap();
        assert!(cursor.skiplist.is_some(), "the partial block is flushed");
        cursor.advance(49);
        assert_eq!(cursor.doc(), Some(49));
        assert_eq!(cursor.blocks_decoded, 1);
        assert_eq!(cursor.positions(), &[49 % 5]);
    }

    /// Terms are encoded in batches as tokio tasks (plan 6.13.3), and the
    /// batch size only decides who encodes what: the three files must be the
    /// same bytes for one term per batch, the production batch, and one batch
    /// for the whole segment.
    ///
    /// This is the guarantee the byte gates rely on - a parallel encoder that
    /// reordered or mis-offset the `.pos` slices would show up here first.
    #[test]
    fn batching_does_not_change_the_bytes() {
        let dir = tempfile::tempdir().unwrap();
        let index_dir = dir.path().to_str().unwrap();
        let (docs, postings) = many_term_sample();
        let flags = tf_flags();

        let write = |base_name: &str, batch_terms: usize| {
            write_segment_index_streaming_with(
                index_dir,
                base_name,
                0,
                &docs,
                flags,
                batch_terms,
                &mut |emit| {
                    for (term, list) in &postings {
                        let payload = |_row: u64| 0u16;
                        emit(term, list, &payload);
                    }
                },
            )
            .unwrap();
            let read =
                |suffix: &str| std::fs::read(format!("{index_dir}/{base_name}{suffix}")).unwrap();
            (
                read(FULLTEXT_DICT_SUFFIX),
                read(FULLTEXT_POSTING_SUFFIX),
                read(FULLTEXT_LENGTH_SUFFIX),
            )
        };

        let one_at_a_time = write("ft_one", 1);
        let production = write("ft_batch", SEAL_BATCH_TERMS);
        let all_at_once = write("ft_all", postings.len());
        assert_eq!(one_at_a_time.0, production.0, "the dictionary moved");
        assert_eq!(one_at_a_time.1, production.1, "the postings moved");
        assert_eq!(one_at_a_time.2, production.2, "the lengths moved");
        assert_eq!(all_at_once.0, production.0);
        assert_eq!(all_at_once.1, production.1);
        assert_eq!(all_at_once.2, production.2);

        // And the segment still reads back what went in.
        let reader = SegmentIndexReader::open(index_dir, "ft_batch", 0, flags).unwrap();
        assert_eq!(reader.doc_count(), docs.len());
        for (term, list) in &postings {
            let got = reader.postings(term).unwrap();
            let want: Vec<(u64, u32)> = list.iter().map(|(row, tf, _)| (*row, *tf)).collect();
            assert_eq!(got, want, "term '{term}'");
        }
    }

    /// Enough terms to cross several encode batches, several postings each.
    #[allow(clippy::type_complexity)]
    fn many_term_sample() -> (Vec<(u64, u32)>, BTreeMap<String, Vec<PostingEntry>>) {
        let rows = 200u64;
        let docs: Vec<(u64, u32)> = (0..rows).map(|row| (row, 4)).collect();
        let mut postings: BTreeMap<String, Vec<PostingEntry>> = BTreeMap::new();
        // 600 terms, each on every third row, so the batch boundary falls inside
        // a term's list rather than only between single-posting terms.
        for t in 0..600u32 {
            let list: Vec<PostingEntry> = (t as u64..rows)
                .step_by(3)
                .map(|row| (row, 1u32, vec![(row % 4) as u32]))
                .collect();
            postings.insert(format!("term{t:04}"), list);
        }
        (docs, postings)
    }
}
