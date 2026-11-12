use super::*;

/// The secondary mirror names its dumps `seg_{segment}_{block}_{rows}.idx`: the
/// block is the chunk the reference publishes, and the trailing field is how
/// many rows the dump contributed. A block can be covered by several dumps, so
/// `publish_secondary_chunks` merges them rather than copying one file.
///
/// One raw ordered key, from a chunk's key array.
pub(crate) fn decode_secondary_key(
    bytes: &[u8],
    kind: storage::secondary_segment::KeyKind,
) -> Option<storage::secondary_segment::Key> {
    use storage::secondary_segment::{Key, KeyKind};
    Some(match kind {
        KeyKind::U64 => Key::U64(u64::from_le_bytes(bytes.try_into().ok()?)),
        KeyKind::I64 => Key::I64(i64::from_le_bytes(bytes.try_into().ok()?)),
        KeyKind::I32 => Key::I32(i32::from_le_bytes(bytes.try_into().ok()?)),
        KeyKind::I16 => Key::I16(i16::from_le_bytes(bytes.try_into().ok()?)),
        KeyKind::I8 => Key::I8(*bytes.first()? as i8),
        KeyKind::Bool => Key::Bool(*bytes.first()? != 0),
        KeyKind::F32 => Key::F32(f32::from_le_bytes(bytes.try_into().ok()?)),
        KeyKind::F64 => Key::F64(f64::from_le_bytes(bytes.try_into().ok()?)),
        // A JSON chunk is low-cardinality and has no ordered key array.
        KeyKind::Bytes => return None,
    })
}

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
pub(crate) fn publish_secondary_chunks(
    seg_dir: &Path,
    dir: &Path,
    kind: storage::secondary_segment::KeyKind,
    segment: u32,
    single_chunk: bool,
) -> Vec<IndexChunk> {
    use storage::secondary_segment::{write_secondary_segment_typed, SecondarySegmentReader};
    let width = kind.key_width();
    let stride = width + 4;
    let mut by_block: std::collections::BTreeMap<u64, Vec<_>> = Default::default();
    // A merged spill file holds the whole segment (`merge_secondary_chunks`),
    // which is the reference's layout after `OptimizeIndex`: one chunk per
    // segment rather than one per block.
    let mut merged = false;
    let Ok(entries) = std::fs::read_dir(dir) else {
        return Vec::new();
    };
    for entry in entries.flatten() {
        let path = entry.path();
        if path.extension().and_then(|e| e.to_str()) != Some("idx") {
            continue;
        }
        // `seg_{segment}_{block}_{rows}.idx` when `build_index` wrote it,
        // `seg_{segment}_tail{n}.idx` when a dump sealed a memory tail; both
        // carry the segment first, and a tail may span blocks, so the block of
        // an entry is derived from its offset rather than from the name.
        let name = entry.file_name().to_string_lossy().to_string();
        let Some(rest) = name
            .strip_prefix("seg_")
            .and_then(|n| n.strip_suffix(".idx"))
        else {
            continue;
        };
        let Some((run_segment, _)) = rest.split_once('_') else {
            continue;
        };
        if run_segment.parse::<u32>() != Ok(segment) {
            continue;
        };
        if rest.contains("_merged") {
            merged = true;
        }
        let (Ok(bytes), Ok(reader)) = (
            std::fs::read(&path),
            SecondarySegmentReader::open_with_kind(&path, kind),
        ) else {
            continue;
        };
        let n = reader.row_count();
        if bytes.len() < n * stride {
            continue;
        }
        for i in 0..n {
            let Some(key) = decode_secondary_key(&bytes[i * width..(i + 1) * width], kind) else {
                continue;
            };
            let at = n * width + i * 4;
            let offset = u32::from_le_bytes(bytes[at..at + 4].try_into().unwrap());
            by_block
                .entry(u64::from(offset) / DEFAULT_BLOCK_CAPACITY as u64)
                .or_default()
                .push((key, offset));
        }
    }
    // `PopulateSecondaryIndexInner` inserts every block of the segment into
    // one in-memory index and dumps **once**, so an imported segment has a
    // single chunk covering all of its blocks (`InsertBlockData` is called per
    // block, `Dump` once after the loop).
    if single_chunk || merged {
        let mut entries: Vec<_> = by_block.into_values().flatten().collect();
        if entries.is_empty() || std::fs::create_dir_all(seg_dir).is_err() {
            return Vec::new();
        }
        entries.sort_by(|a, b| a.0.order(&b.0));
        let path = seg_dir.join("chunk_0.idx");
        if write_secondary_segment_typed(&path, kind, &entries).is_err() {
            return Vec::new();
        }
        return vec![IndexChunk {
            chunk_id: 0,
            base_name: String::new(),
            base_row_id: u64::from(segment) << 32,
            row_count: entries.len() as u64,
            term_count: 0,
            index_size: 0,
            commit_ts: 0,
        }];
    }
    let mut chunks = Vec::new();
    for (block, mut entries) in by_block {
        if std::fs::create_dir_all(seg_dir).is_err() {
            break;
        }
        entries.sort_by(|a, b| a.0.order(&b.0));
        let path = seg_dir.join(format!("chunk_{block}.idx"));
        if write_secondary_segment_typed(&path, kind, &entries).is_err() {
            continue;
        }
        chunks.push(IndexChunk {
            chunk_id: block as u32,
            base_name: String::new(),
            // `RowID` of the block's first row: the segment in the high bits,
            // the in-segment offset in the low ones.
            base_row_id: (u64::from(segment) << 32) | (block * DEFAULT_BLOCK_CAPACITY as u64),
            row_count: entries.len() as u64,
            term_count: 0,
            index_size: 0,
            commit_ts: 0,
        });
    }
    chunks
}

/// Rebuilds a sparse index's published chunks: one per block, from the rows.
///
/// The reference's chunk is the block, and every memory dump re-dumps the block
/// it touches (`BMPIndexInMem::Dump` hands the whole in-memory `BMPAlg` to the
/// chunk buffer), so two 100-row inserts into one block leave **one** chunk of
/// 200 rows. The port's spill files are per dump, so the merge happens here.
pub(crate) fn publish_bmp_chunks(
    seg_dir: &Path,
    by_block: std::collections::BTreeMap<u64, Vec<storage::bmp::SparseDoc>>,
    width: storage::bmp::IdxWidth,
    term_bound: usize,
    block_size: usize,
    segment: u32,
    single_chunk: bool,
) -> Vec<IndexChunk> {
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
        let path = seg_dir.join(format!("chunk_{block}.idx"));
        if storage::definition::format::write_synced(&path, &bytes).is_err() {
            continue;
        }
        chunks.push(IndexChunk {
            chunk_id: block as u32,
            base_name: String::new(),
            base_row_id: (u64::from(segment) << 32) | (block * DEFAULT_BLOCK_CAPACITY as u64),
            row_count: docs.len() as u64,
            term_count: 0,
            index_size: 0,
            commit_ts: 0,
        });
    }
    chunks
}

/// The full-text chunks on disk, one per block, named `ft_{base_row_id:016x}`.
pub(crate) fn publish_fulltext_chunks(seg_dir: &Path) -> Vec<IndexChunk> {
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
        let Ok(base_row_id) = u64::from_str_radix(hex, 16) else {
            continue;
        };
        let base = storage::definition::format::fulltext_base_name(base_row_id, None);
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
        chunks.push(IndexChunk {
            chunk_id: 0, // assigned below, in base-row-id order
            base_name: base,
            base_row_id,
            row_count,
            term_count,
            index_size: 0,
            commit_ts: 0,
        });
    }
    // The reference's chunk ids are the order the chunks were dumped in, not
    // the block each one starts at: a second run that appends 110 rows to a
    // 100-row table dumps `ft_0000000000000064` as chunk 1, although both
    // chunks live in block 0. Reading them back, `CountMemIndexGapInSegment`
    // requires the chunk infos to line up end to end from row 0, so the ids
    // have to be distinct and ascending.
    chunks.sort_by_key(|c| c.base_row_id);
    for (id, chunk) in chunks.iter_mut().enumerate() {
        chunk.chunk_id = id as u32;
    }
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
        return if ids.is_empty() {
            Vec::new()
        } else {
            vec![IndexChunk {
                chunk_id: 0,
                base_name: String::new(),
                base_row_id: u64::from(segment) << 32,
                row_count: rows,
                term_count: 0,
                index_size: 0,
                commit_ts: 0,
            }]
        };
    }
    ids.sort_unstable();
    let mut chunks = Vec::new();
    let mut base_row_id = 0u64;
    for id in ids {
        // `[M][ef_construction][cur_vec_num]…` — the third u64 is this chunk's
        // **vertex** count (`HnswHeader::cur_vec_num`, `knn_format.rs:44`), which
        // is the token count for a `MULTIVECTOR` column. Row coverage is the
        // number of distinct labels instead: publishing the vertex count would
        // claim rows the chunk does not hold, and advancing `base_row_id` by it
        // would put the next chunk's base past the end of the segment.
        let row_count = std::fs::read(seg_dir.join(format!("chunk_{id}.idx")))
            .ok()
            .map(|bytes| hnsw_chunk_rows(&bytes))
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

/// The rows an encoded HNSW chunk covers: the distinct labels of its trailing
/// `u32[cur_vec_num]` array.
///
/// A chunk that does not parse (truncated, or shorter than its own header
/// claims) reports its `cur_vec_num`, which is the best the file can say about
/// itself; the chunk is then covered by restart recovery rather than skipped.
fn hnsw_chunk_rows(bytes: &[u8]) -> u64 {
    if bytes.len() < 24 {
        return 0;
    }
    let vertices = u64::from_le_bytes(bytes[16..24].try_into().unwrap()) as usize;
    let Some(start) = bytes.len().checked_sub(vertices.saturating_mul(4)) else {
        return vertices as u64;
    };
    let labels: Vec<u32> = bytes[start..]
        .as_chunks::<4>()
        .0
        .iter()
        .map(|c| u32::from_le_bytes(*c))
        .collect();
    labelled_rows(&labels) as u64
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
    let bytes = std::fs::read(len_path).ok()?;
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
