use super::*;

pub(crate) fn align_up8(n: usize) -> usize {
    n.div_ceil(8) * 8
}

/// Bytes of the vec-store meta inside the chunk header.
pub(crate) fn hnsw_store_meta_len(
    dims: usize,
    metric: index::vector::Metric,
    encoding: index::vector::HnswEncoding,
) -> usize {
    use index::vector::{HnswEncoding, Metric};
    match encoding {
        HnswEncoding::Plain => 8,
        HnswEncoding::Lvq => {
            8 + 8 * dims
                + match metric {
                    Metric::L2 => 0,
                    Metric::Cos => 8,
                    Metric::Ip => 16,
                }
        }
        HnswEncoding::Rabitq => {
            let d = align_up8(dims);
            8 + 4 * d * d + 4 * d
        }
    }
}

/// Bytes per vector in the chunk's vector region.
pub(crate) fn hnsw_record_size(dims: usize, encoding: index::vector::HnswEncoding) -> usize {
    use index::vector::HnswEncoding;
    match encoding {
        HnswEncoding::Plain => dims * 4,
        HnswEncoding::Lvq => 16 + dims,
        HnswEncoding::Rabitq => 16 + align_up8(dims) / 8,
    }
}

/// HNSW build options read from the catalog (`IndexHnsw`).
pub(crate) struct HnswOptions {
    /// `IndexHnsw::M_`.
    pub(crate) m: usize,
    /// `IndexHnsw::ef_construction_`.
    pub(crate) ef_construction: usize,
    pub(crate) metric: index::vector::Metric,
    pub(crate) encoding: index::vector::HnswEncoding,
    pub(crate) lsg: Option<index::lsg::LsgConfig>,
}

pub struct LoadedVectorIndex {
    pub index: HnswHandler,
    pub labels: Vec<u32>,
    /// Distinct rows the graph labels. Equal to `labels.len()` for a
    /// single-vector column; a `MULTIVECTOR` chunk labels one row several
    /// times, one per token, so the count is what the coverage check needs.
    pub labelled_rows: usize,
    /// The `chunk_{id}.idx` this graph came from, so a later reload can load
    /// only the chunks written since the last one.
    pub chunk_id: u64,
    /// Approximate heap bytes (vectors + graph + labels), for the buffer budget.
    pub bytes: usize,
    /// `(len, mtime_secs)` of the chunk file when it was decoded; a rewritten
    /// chunk under the same id is detected by a stamp change.
    pub stamp: (u64, u64),
}

/// What an encoded HNSW chunk covers.
///
/// The header records `vertices` (`cur_vec_num`): one per row for an
/// `EMBEDDING` column, one per **token** for a `MULTIVECTOR` one. The row
/// coverage a chunk is published with is the number of *distinct labels*
/// instead - a 100-row chunk of two-token rows labels 200 vertices but covers
/// 100 rows, and publishing 200 would let restart recovery skip an unsealed
/// tail inside that fictitious range.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub(crate) struct HnswChunkCover {
    pub rows: usize,
    pub vertices: usize,
}

impl HnswChunkCover {
    fn of(labels: &[u32]) -> Self {
        Self {
            rows: labelled_rows(labels),
            vertices: labels.len(),
        }
    }
}

/// One IVF chunk loaded from the mirror, with the segment it was written for.
///
/// An IVF chunk stores *in-segment* offsets (`RowID(segment, offset)`): the
/// reference trains and dumps one chunk per segment (`PopulateIvfIndexInner`
/// takes a `SegmentMeta`), so an offset only becomes a row id once the segment
/// is put in the high 32 bits. Keeping the segment next to the chunk is what
/// lets a probe answer with row ids instead of offsets that collide across
/// segments.
pub struct LoadedIvfIndex {
    /// The `seg_{segment}` directory the chunk came from.
    pub segment: u32,
    pub index: index::ivf::IvfSegment,
    /// Pretty name of the chunk file, for diagnostics.
    pub path: std::path::PathBuf,
}

/// The IVF index options the mirror has to honour
/// (`IndexIVFCentroidOption` + `IndexIVFStorageOption`).
pub(crate) struct IvfOptions {
    pub(crate) storage: String,
    pub(crate) sq_bits: u8,
    pub(crate) pq_subspace_num: usize,
    pub(crate) pq_bits: u8,
    pub(crate) ratio: f32,
    pub(crate) min_points: usize,
}

/// Where a table's PLAID mirror lives and what it indexes.
pub(crate) struct PlaidMirror {
    pub(crate) dir: PathBuf,
    pub(crate) column: String,
    pub(crate) dims: u32,
    /// SDK `ElementType` code of the tensor element (6 = float).
    pub(crate) elem: i32,
}

/// One `chunk_{id}.idx` of a PLAID mirror.
pub(crate) struct PlaidChunkFile {
    pub(crate) id: u64,
    pub(crate) index: PlaidIndex,
}

/// Whether the newest chunk of a mirror already holds every document the older
/// ones hold, i.e. it is the output of an optimize merge. A merge drops the
/// chunks it folded in; this is what tells a reader (and a leftover set of
/// files whose removal failed) to count the newest chunk alone.
pub(crate) fn plaid_merged_tail(doc_nums: &[u32]) -> bool {
    match doc_nums.split_last() {
        Some((newest, older)) if !older.is_empty() => *newest >= older.iter().sum::<u32>(),
        _ => false,
    }
}

/// `(documents, embeddings)` the chunks of a mirror already hold, in chunk-id
/// order, counting a merged newest chunk instead of the ones it supersedes.
pub(crate) fn plaid_coverage(sizes: &[(u32, u32)]) -> (u32, u32) {
    let doc_nums: Vec<u32> = sizes.iter().map(|size| size.0).collect();
    match sizes.last() {
        Some(&newest) if plaid_merged_tail(&doc_nums) => newest,
        _ => (
            sizes.iter().map(|size| size.0).sum(),
            sizes.iter().map(|size| size.1).sum(),
        ),
    }
}

/// `PlaidIndex::SaveIndexInner`'s header: `format_version`,
/// `start_segment_offset`, `embedding_dimension`, `nbits`, `n_centroids`,
/// `n_docs`. Reading the count from the header avoids decoding a chunk that
/// cleanup only has to look at.
pub(crate) fn plaid_chunk_doc_num(path: &std::path::Path) -> Option<u32> {
    use std::io::Read;
    let mut head = [0u8; 24];
    std::fs::File::open(path).ok()?.read_exact(&mut head).ok()?;
    Some(u32::from_le_bytes(head[20..24].try_into().ok()?))
}

/// PLAID index parameters from `WITH (nbits=…, n_centroids=…)`.
#[derive(Debug, Clone, Copy)]
pub(crate) struct PlaidOptions {
    pub(crate) nbits: u32,
    pub(crate) n_centroids: u32,
    pub(crate) colbertsar: bool,
}

/// Maps one sealed HNSW chunk read-only.
///
/// A chunk is immutable once published (every publisher renames a temp file over
/// it), so a reader points at the page cache instead of reading the payload into
/// the heap — the reference's buffer-manager chunk, without the LRU. Publisher
/// and reader agree because a rewrite swaps the directory entry and leaves the
/// inode this mapping names untouched.
pub(crate) fn map_hnsw_chunk(path: &std::path::Path) -> Option<index::vector::ByteSource> {
    let mapped = storage::buffer::mapped::MappedFile::open(path).ok()?;
    if mapped.is_empty() {
        return None;
    }
    Some(std::sync::Arc::new(mapped))
}

pub(crate) fn file_stamp(path: &std::path::Path) -> (u64, u64) {
    std::fs::metadata(path)
        .ok()
        .map(|m| {
            let modified = m
                .modified()
                .ok()
                .and_then(|t| t.duration_since(std::time::UNIX_EPOCH).ok())
                .map(|d| d.as_secs())
                .unwrap_or(0);
            (m.len(), modified)
        })
        .unwrap_or((0, 0))
}

/// Serializes an existing graph as a C++ HNSW chunk (vec-store meta, graph
/// region, `u32` labels). The vector region depends on the encoding: plain
/// f32 vectors, LVQ i8 codes or RaBitQ 1-bit codes.
///
/// The coverage it returns is what a publisher may claim: [`HnswChunkCover::rows`]
/// for row coverage, [`HnswChunkCover::vertices`] for the header's
/// `cur_vec_num` (they differ for a `MULTIVECTOR` chunk).
pub(crate) fn encode_hnsw_chunk_from(
    index: &HnswIndex,
    labels: &[u32],
) -> (HnswChunkCover, Vec<u8>) {
    let m = index.m() as u64;
    let (mmax0, mmax) = knn_format::get_mmax(m);
    let ef_construction = index.ef_construction() as u64;
    let max_layer = index.max_level() as i32;
    let enterpoint = index.entry_point().map(|e| e as i32).unwrap_or(0);
    let store_meta = index.store_meta_bytes();
    let vector_region = index.store_records_bytes();
    // The graph region is written straight from the compact slabs: routing it
    // through one `HnswGraphVertex` per node cost a full second copy of the
    // graph, which is what the compact store exists to avoid.
    let graph = index.encode_graph_region();
    let chunk = knn_format::encode_hnsw_chunk_ext(
        m,
        ef_construction,
        index.len() as u64,
        mmax0,
        mmax,
        max_layer,
        enterpoint,
        &store_meta,
        &vector_region,
        &graph,
        labels,
    );
    (HnswChunkCover::of(labels), chunk)
}
