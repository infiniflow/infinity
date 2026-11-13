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
    /// The `chunk_{id}.idx` this graph came from, so a later reload can load
    /// only the chunks written since the last one.
    pub chunk_id: u64,
    /// Approximate heap bytes (vectors + graph + labels), for the buffer budget.
    pub bytes: usize,
    /// `(len, mtime_secs)` of the chunk file when it was decoded; a rewritten
    /// chunk under the same id is detected by a stamp change.
    pub stamp: (u64, u64),
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
pub(crate) fn encode_hnsw_chunk_from(index: &HnswIndex, labels: &[u32]) -> (usize, Vec<u8>) {
    let m = index.m() as u64;
    let (mmax0, mmax) = knn_format::get_mmax(m);
    let ef_construction = index.ef_construction() as u64;
    let max_layer = index.max_level() as i32;
    let enterpoint = index.entry_point().map(|e| e as i32).unwrap_or(0);
    let store_meta = index.store_meta_bytes();
    let vector_region = index.store_records_bytes();
    // Neighbour counts must not exceed the level's limit.
    let vertices: Vec<knn_format::HnswGraphVertex> = (0..index.len())
        .map(|n| {
            let level = index.level_of(n);
            let neighbors_l0 = index
                .neighbors(0, n)
                .iter()
                .take(mmax0 as usize)
                .map(|&x| x as u32)
                .collect();
            let upper = (1..=level)
                .map(|l| {
                    index
                        .neighbors(l, n)
                        .iter()
                        .take(mmax as usize)
                        .map(|&x| x as u32)
                        .collect()
                })
                .collect();
            knn_format::HnswGraphVertex {
                layer_count: level as i32,
                neighbors_l0,
                upper,
            }
        })
        .collect();
    let graph = knn_format::encode_graph_region(&vertices, mmax0, mmax);
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
    (index.len(), chunk)
}
