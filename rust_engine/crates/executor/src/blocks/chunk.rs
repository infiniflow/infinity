use super::*;

/// The secondary mirror names its dumps `seg_{segment}_{block}_{rows}.idx`: the
/// block is the chunk the reference publishes, and the trailing field is how
/// many rows the dump contributed. A block can be covered by several dumps, so
/// `publish_secondary_chunks` merges them rather than copying one file.
///
/// One raw ordered key, from a chunk's key array.
/// Rebuilds a secondary index's published chunks: every spill run is decoded
/// and re-bucketed by **block**, then written as `chunk_{block}.idx`.
///
/// The spill runs are per memory dump, so a run may straddle a block boundary
/// or cover half of one; the reference's chunk is the block
/// (`SecondaryIndexDataHighCardinalityT::SaveIndexInner`,
/// `secondary_index_data_impl.cpp:223`, one per `InsertBlockData` call). The
/// offsets stay segment-relative, so the bucketing key is `offset / 8192` and
/// the values are written unchanged.
///
/// The spill file name carries the segment (`seg_{segment}_{block}_{rows}.idx`,
/// `write_secondary_pgm_chunks`), because two segments restart their offsets
/// at zero: only the runs of `segment` are published here, into that
/// segment's directory.
/// The chunks a segment directory already holds, as records
/// (`SegmentIndexMeta::GetChunkIDs1`'s contents, read from the files the seal
/// wrote).
///
/// A secondary dump writes each tail straight into `idx_{id}/seg_{n}` with the
/// id the catalog handed out, so publishing must not derive the chunk list from
/// the `secondary/` spill set again: that would name chunk 0 for every dump and
/// overwrite the record the previous one wrote.
pub(crate) fn publish_secondary_chunks_from_dir(
    seg_dir: &Path,
    kind: storage::secondary_segment::KeyKind,
    segment: u32,
) -> Vec<IndexChunk> {
    use storage::secondary_segment::SecondarySegmentReader;
    let Ok(entries) = std::fs::read_dir(seg_dir) else {
        return Vec::new();
    };
    let mut chunks = Vec::new();
    for entry in entries.flatten() {
        let name = entry.file_name().to_string_lossy().to_string();
        let Some(id) = name
            .strip_prefix("chunk_")
            .and_then(|rest| rest.strip_suffix(".idx"))
            .and_then(|id| id.parse::<u32>().ok())
        else {
            continue;
        };
        let path = entry.path();
        let Ok(bytes) = storage::chunk_store::read_file(&path) else {
            continue;
        };
        let Ok(reader) = SecondarySegmentReader::open_with_kind(&path, kind) else {
            continue;
        };
        let width = kind.key_width();
        let stride = width + 4;
        let n = reader.row_count();
        if bytes.len() < n * stride {
            continue;
        }
        let mut base_row_id = u64::from(segment) << 32;
        let mut min_offset = u32::MAX;
        for i in 0..n {
            let at = n * width + i * 4;
            let offset = u32::from_le_bytes(bytes[at..at + 4].try_into().unwrap());
            min_offset = min_offset.min(offset);
        }
        if min_offset != u32::MAX {
            base_row_id |= u64::from(min_offset);
        }
        chunks.push(IndexChunk {
            chunk_id: id,
            base_name: String::new(),
            base_row_id,
            row_count: n as u64,
            term_count: 0,
            index_size: 0,
            commit_ts: 0,
        });
    }
    chunks.sort_by_key(|c| c.chunk_id);
    chunks
}

/// The chunks a low-cardinality segment directory already holds.
pub(crate) fn publish_lowcard_chunks_from_dir(seg_dir: &Path, segment: u32) -> Vec<IndexChunk> {
    use storage::secondary_lowcard::LowCardReader;
    let Ok(entries) = std::fs::read_dir(seg_dir) else {
        return Vec::new();
    };
    let mut chunks = Vec::new();
    for entry in entries.flatten() {
        let name = entry.file_name().to_string_lossy().to_string();
        let Some(id) = name
            .strip_prefix("chunk_")
            .and_then(|rest| rest.strip_suffix(".idx"))
            .and_then(|id| id.parse::<u32>().ok())
        else {
            continue;
        };
        let Ok(reader) = LowCardReader::open(&entry.path()) else {
            continue;
        };
        chunks.push(IndexChunk {
            chunk_id: id,
            base_name: String::new(),
            base_row_id: u64::from(segment) << 32,
            row_count: reader.len() as u64,
            term_count: 0,
            index_size: 0,
            commit_ts: 0,
        });
    }
    chunks.sort_by_key(|c| c.chunk_id);
    chunks
}

/// Rebuilds a sparse index's published chunks: one per block, from the rows.
///
/// The reference's chunk is the block, and every memory dump re-dumps the block
/// it touches (`BMPIndexInMem::Dump` hands the whole in-memory `BMPAlg` to the
/// chunk buffer), so two 100-row inserts into one block leave **one** chunk of
/// 200 rows. The port's spill files are per dump, so the merge happens here.
///
/// The chunk id is the catalog's, not the block number: `PopulateBMPIndexInner`
/// and `DumpMemIndex` both take one from `GetAndSetNextChunkID`, and that
/// counter only moves forward. `existing` maps a chunk's first row id to the id
/// a record already named, so a re-dump of the same block rewrites its own
/// chunk instead of appending a second one; `next_id` hands out the ones no
/// record names yet.
/// Where and how a sparse chunk is encoded: the column's `IdxWidth` and
/// declared term bound, the block size, the segment the chunk belongs to and
/// whether the whole segment folds into one chunk (an `IMPORT` does).
pub(crate) struct BmpChunkShape {
    pub(crate) width: storage::bmp::IdxWidth,
    pub(crate) term_bound: usize,
    pub(crate) block_size: usize,
    pub(crate) segment: u32,
    pub(crate) single_chunk: bool,
}

pub(crate) fn publish_bmp_chunks(
    seg_dir: &Path,
    by_block: std::collections::BTreeMap<u64, Vec<storage::bmp::SparseDoc>>,
    shape: &BmpChunkShape,
    existing: &std::collections::HashMap<u64, u32>,
    next_id: &mut u32,
) -> Vec<IndexChunk> {
    let BmpChunkShape {
        width,
        term_bound,
        block_size,
        segment,
        single_chunk,
    } = *shape;
    // `PopulateBMPIndexInner` adds every block of the segment to one
    // `BMPIndexInMem` and dumps once, so an imported segment's sparse chunk
    // covers all of its blocks.
    let by_block: std::collections::BTreeMap<u64, Vec<storage::bmp::SparseDoc>> = match single_chunk
    {
        true => {
            let docs: Vec<storage::bmp::SparseDoc> = by_block.into_values().flatten().collect();
            if docs.is_empty() {
                Default::default()
            } else {
                [(0u64, docs)].into_iter().collect()
            }
        }
        false => by_block,
    };
    let mut chunks = Vec::new();
    for (block, mut docs) in by_block {
        if std::fs::create_dir_all(seg_dir).is_err() {
            break;
        }
        docs.sort_by_key(|(row, _)| *row);
        // The term space is the column's declared bound, widened to the terms
        // this chunk actually holds (`BMPAlg` is constructed with the bound).
        let term_num = docs
            .iter()
            .flat_map(|(_, entries)| entries.iter().map(|(term, _)| *term + 1))
            .max()
            .unwrap_or(0)
            .max(1)
            .max(term_bound as i32) as usize;
        let bytes = storage::bmp::encode_bmp(block_size, width, &docs, term_num);
        // `block` is the index's own block ordinal, so its first row is
        // `block * block_size` (`BMPAlg`: `doc_id = block_id * block_size`).
        let base_row_id = (u64::from(segment) << 32) | (block * block_size as u64);
        // The id the record for **this block's** chunk already owns, else the
        // counter the catalog keeps. A block never reuses a retired id.
        let chunk_id = match existing.get(&base_row_id) {
            Some(id) => *id,
            None => {
                let id = *next_id;
                *next_id += 1;
                id
            }
        };
        let path = seg_dir.join(format!("chunk_{chunk_id}.idx"));
        if storage::definition::format::write_synced(&path, &bytes).is_err() {
            continue;
        }
        chunks.push(IndexChunk {
            chunk_id,
            base_name: String::new(),
            base_row_id,
            row_count: docs.len() as u64,
            term_count: 0,
            index_size: 0,
            commit_ts: 0,
        });
    }
    chunks
}

/// The full-text chunks on disk, one per block, named `ft_{base_row_id:016x}`.
///
/// The chunk **id** is the catalog's, never the chunk's position in the
/// directory: `DumpMemIndex`'s full-text case calls `AddNewChunkIndex1` with the
/// id `GetAndSetNextChunkID` handed out, and that counter only moves forward.
/// Numbering the files `0..n` in row order handed a retired id to a later
/// chunk (`Cleanup` removes the file but not the counter), and a re-dump of an
/// already-recorded block rewrote its record under a different id. `existing`
/// maps a chunk's first row id to the id its record already owns, so a
/// re-publish keeps it; `next_id` supplies the ones no record names yet.
pub(crate) fn publish_fulltext_chunks(
    seg_dir: &Path,
    existing: &std::collections::HashMap<u64, u32>,
    next_id: &mut u32,
) -> Vec<IndexChunk> {
    let mut chunks = Vec::new();
    let Ok(entries) = std::fs::read_dir(seg_dir) else {
        return chunks;
    };
    for entry in entries.flatten() {
        let name = entry.file_name().to_string_lossy().to_string();
        let Some(hex) = name
            .strip_prefix("ft_")
            .and_then(|rest| rest.strip_suffix(".dic"))
        else {
            continue;
        };
        // A merged chunk is `ft_{base:016x}_{rows:x}` (`fulltext_base_name`
        // with a row count, written by `optimize_chunks`); a plain one is just
        // the base. Both record their first row id the same way, and the file
        // name is what a record has to repeat.
        let (base_hex, merged_rows) = match hex.split_once('_') {
            Some((first, rows)) => (first, u64::from_str_radix(rows, 16).ok()),
            None => (hex, None),
        };
        let Ok(base_row_id) = u64::from_str_radix(base_hex, 16) else {
            continue;
        };
        let base = storage::definition::format::fulltext_base_name(base_row_id, merged_rows);
        // The `.len` file is the chunk's per-document column length
        // (`u32[row_cnt]`, `RawFileWorker` in `chunk_index_meta_impl.cpp:157`),
        // so it carries both numbers the reference records in the chunk info:
        // the document count and the total term count
        // (`MemoryIndexer::GetDocTermCount` sums the lengths). The reference
        // needs the term count: a full-text query drops the whole search when
        // it is zero (`TermQueryNode::CreateSearch`, `query_node_impl.cpp:435`).
        let Some((row_count, term_count)) =
            fulltext_chunk_counts(&seg_dir.join(format!("{base}.len")))
        else {
            continue;
        };
        let chunk_id = match existing.get(&base_row_id) {
            Some(id) => *id,
            None => {
                let id = *next_id;
                *next_id += 1;
                id
            }
        };
        chunks.push(IndexChunk {
            chunk_id,
            base_name: base,
            base_row_id,
            row_count,
            term_count,
            index_size: 0,
            commit_ts: 0,
        });
    }
    chunks.sort_by_key(|c| c.chunk_id);
    chunks
}

/// The `chunk_{id}.idx` files of an index that writes them in place (HNSW, IVF,
/// PLAID). An HNSW chunk's header carries its vector count, so a multi-chunk
/// index can be described exactly; the other kinds still write one chunk, which
/// covers the whole snapshot.
pub(crate) fn publish_chunk_files(
    seg_dir: &Path,
    rows: u64,
    hnsw: bool,
    segment: u32,
    deprecated: &std::collections::HashSet<u64>,
) -> Vec<IndexChunk> {
    let mut ids: Vec<u32> = Vec::new();
    if let Ok(entries) = std::fs::read_dir(seg_dir) {
        for entry in entries.flatten() {
            let name = entry.file_name().to_string_lossy().to_string();
            let Some(id) = name
                .strip_prefix("chunk_")
                .and_then(|rest| rest.strip_suffix(".idx"))
                .and_then(|id| id.parse::<u32>().ok())
            else {
                continue;
            };
            // A chunk an optimize superseded has left the segment's chunk list
            // even though its file is still on disk until `Cleanup`.
            if deprecated.contains(&u64::from(id)) {
                continue;
            }
            ids.push(id);
        }
    }
    if !hnsw {
        // One chunk per segment, but under **its own** id: a literal 0 would
        // append a second record next to the one `write_ivf_index` published.
        let Some(id) = ids.into_iter().min() else {
            return Vec::new();
        };
        return vec![IndexChunk {
            chunk_id: id,
            base_name: String::new(),
            base_row_id: u64::from(segment) << 32,
            row_count: rows,
            term_count: 0,
            index_size: 0,
            commit_ts: 0,
        }];
    }
    ids.sort_unstable();
    let mut chunks = Vec::new();
    let mut base_row_id = 0u64;
    for id in ids {
        // `[M][ef_construction][cur_vec_num]…` — the third u64 is this chunk's
        // vector count (`HnswHeader::cur_vec_num`, `knn_format.rs:44`).
        let row_count = storage::chunk_store::chunk_store()
            .read(&seg_dir.join(format!("chunk_{id}.idx")))
            .ok()
            .filter(|bytes| bytes.len() >= 24)
            .map(|bytes| u64::from_le_bytes(bytes[16..24].try_into().unwrap()))
            .unwrap_or(0);
        chunks.push(IndexChunk {
            chunk_id: id,
            base_name: String::new(),
            base_row_id: (u64::from(segment) << 32) | base_row_id,
            row_count,
            term_count: 0,
            index_size: 0,
            commit_ts: 0,
        });
        base_row_id += row_count;
    }
    chunks
}

/// The document count and the total term count of a full-text chunk, read
/// from its `.len` file.
///
/// The file is the chunk's per-document column length, a bare `u32[row_cnt]`
/// (`RawFileWorker` in `chunk_index_meta_impl.cpp:157`). The reference
/// records the same two numbers in the chunk info: the document count and the
/// sum of the lengths (`MemoryIndexer::GetDocTermCount`,
/// `memory_indexer_impl.cpp:636`).
pub(crate) fn fulltext_chunk_counts(len_path: &Path) -> Option<(u64, u64)> {
    let bytes = storage::chunk_store::read_file(len_path).ok()?;
    if bytes.len() % 4 != 0 {
        return None;
    }
    let mut doc_count = 0u64;
    let mut term_count = 0u64;
    for chunk in bytes.as_chunks::<4>().0 {
        doc_count += 1;
        term_count += u32::from_le_bytes([chunk[0], chunk[1], chunk[2], chunk[3]]) as u64;
    }
    Some((doc_count, term_count))
}

/// `chunk_{id}.idx` files under `dir`, ordered by chunk id. The reference
/// appends one chunk per memory-index dump and merges them on optimize.
pub(crate) fn hnsw_chunks(dir: &std::path::Path) -> Vec<(u64, PathBuf)> {
    let mut chunks: Vec<(u64, PathBuf)> = std::fs::read_dir(dir)
        .map(|entries| {
            entries
                .flatten()
                .filter_map(|e| {
                    let name = e.file_name().to_string_lossy().to_string();
                    let id = name
                        .strip_prefix("chunk_")?
                        .strip_suffix(".idx")?
                        .parse::<u64>()
                        .ok()?;
                    Some((id, e.path()))
                })
                .collect()
        })
        .unwrap_or_default();
    chunks.sort_by_key(|(id, _)| *id);
    chunks
}

/// Encodes one text value into the bytes its `.col` slot holds.
///
/// NULL is represented by `None` (the caller writes a zeroed slot and clears
/// the null bit).
/// Encodes a full-text chunk the way the **reference** writes it: the `.dic`
/// (the term meta region plus the FST), the `.pos` (each term's doc list
/// followed by its position list) and the `.len` (the per-document column
/// lengths).
///
/// The layout is the reference's own:
///
/// * `postings` maps a term to the `(document ordinal, position)` pairs it
///   occurs at, and `lengths` holds each document's token count.
/// * The index is created with `OPTION_FLAG_ALL` (`index_full_text.cppm:73`):
///   the term meta carries a total term frequency and a payload, the doc list
///   carries a per-document frequency and the position list is present. A
///   chunk written with a different flag is read with the index's own flag, so
///   the reader would walk the wrong fields and find no document.
/// * The doc list and the position list are the reference's own encoders
///   (`DocListEncoder::Dump` / `PositionListEncoder::Dump`): a skip-list
///   header plus the row-wise byte slice.
/// * The term meta's offsets are byte positions **inside the `.pos` file**:
///   the doc list starts at `doc_start`, the position list at `pos_start` and
///   the term's postings end at `pos_end` (`PostingWriter::Dump`,
///   `posting_writer_impl.cpp:52-63`). The reference slices the file by them
///   (`disk_segment_reader_impl.cpp:126`), so each offset has to be taken
///   **after** the preceding part has been appended.
pub fn encode_fulltext_chunk(
    postings: &std::collections::BTreeMap<String, Vec<(u32, u32)>>,
    lengths: &[u32],
) -> (Vec<u8>, Vec<u8>, Vec<u8>) {
    const OPTION_FLAG_ALL: u8 = 1 | 2 | 4 | 8 | 16 | 32;
    // The `.dic` half records `total_tf` and the term payload, and the doc list
    // below is built with `OPTION_FLAG_ALL`, so the flags have to describe the
    // same option word (`index_full_text.cppm:73`).
    let flags = storage::invertedindex::format::OPTION_FLAG_ALL;
    let doc_option =
        storage::invertedindex::format::doc_list::DocListFormatOption::new(OPTION_FLAG_ALL);
    let mut pos = Vec::new();
    let mut metas = Vec::with_capacity(postings.len());
    let terms: Vec<(String, Vec<(u32, u32)>)> = postings
        .iter()
        .map(|(k, v)| (k.clone(), v.clone()))
        .collect();
    for (_, docs) in &terms {
        let mut doc_encoder =
            storage::invertedindex::format::doc_list::DocListEncoder::new(doc_option);
        let mut pos_encoder =
            storage::invertedindex::format::position_list::PositionListEncoder::new(
                OPTION_FLAG_ALL,
            );
        // `GeneratePosting` (`column_inverter_impl.cpp:253-257`) emits one
        // `EndDocument` per **document**, after every position of that
        // document was added: the doc list's per-document term frequency is
        // the number of positions, and `df` counts documents. Calling
        // `end_document` per position instead repeats the doc id in the doc
        // list and makes `df` count occurrences.
        let mut at = 0;
        while at < docs.len() {
            let doc = docs[at].0;
            let mut end = at;
            while end < docs.len() && docs[end].0 == doc {
                doc_encoder.add_position();
                pos_encoder.add_position(docs[end].1);
                end += 1;
            }
            doc_encoder.end_document(doc, lengths[doc as usize], 0);
            pos_encoder.end_document();
            at = end;
        }
        let doc_start = pos.len() as u64;
        let doc_bytes = doc_encoder.dump(false);
        pos.extend_from_slice(&doc_bytes);
        let pos_start = pos.len() as u64;
        let pos_bytes = pos_encoder.dump(false);
        pos.extend_from_slice(&pos_bytes);
        let pos_end = pos.len() as u64;
        metas.push(storage::invertedindex::format::TermMeta {
            doc_freq: doc_encoder.get_df(),
            // The total term frequency counts the occurrences, not the
            // documents (`DocListEncoder::AddPosition`).
            total_tf: doc_encoder.get_total_tf(),
            payload: 0,
            doc_start,
            pos_start,
            pos_end,
        });
    }
    let (region, offsets) =
        storage::invertedindex::format::encode_term_meta_region_with_offsets(&metas, flags);
    let mut fst = storage::invertedindex::format::FstBuilder::new();
    for (i, (term, _)) in terms.iter().enumerate() {
        fst.insert(term.as_bytes(), offsets[i]);
    }
    let dic = storage::invertedindex::format::encode_dic(&region, &fst.finish());
    let len = storage::invertedindex::format::encode_column_lengths(lengths);
    (dic, pos, len)
}
