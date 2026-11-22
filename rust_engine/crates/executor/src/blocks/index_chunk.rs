// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
#[derive(Clone, Copy)]
pub(crate) struct HnswOptions {
    /// `IndexHnsw::M_`.
    pub(crate) m: usize,
    /// `IndexHnsw::ef_construction_`.
    pub(crate) ef_construction: usize,
    pub(crate) metric: index::vector::Metric,
    pub(crate) encoding: index::vector::HnswEncoding,
    pub(crate) lsg: Option<index::lsg::LsgConfig>,
}

impl HnswOptions {
    /// The `WITH (...)` options a `CREATE INDEX ... USING HNSW` stored.
    ///
    /// `build_type='lsg'` is what turns the index into a local-scaling one; the
    /// reference then requires `lsg_config` and falls back to `LSGConfig`'s own
    /// defaults (`0.01 / 10 / 1`) when it is missing.
    pub(crate) fn from_options(options: &[(String, String)]) -> Self {
        let raw = |name: &str| {
            options
                .iter()
                .find(|(k, _)| k.eq_ignore_ascii_case(name))
                .map(|(_, v)| v.as_str())
        };
        let num = |name: &str, default: usize| {
            raw(name)
                .and_then(|v| v.trim().parse().ok())
                .unwrap_or(default)
        };
        let lsg = match raw("build_type") {
            Some(t)
                if t.trim()
                    .trim_matches(['\'', '"'])
                    .eq_ignore_ascii_case("lsg") =>
            {
                Some(
                    raw("lsg_config")
                        .and_then(index::lsg::LsgConfig::parse)
                        .unwrap_or_default(),
                )
            }
            _ => None,
        };
        Self {
            m: num("M", 16),
            ef_construction: num("EF_CONSTRUCTION", 200),
            metric: index::vector::Metric::parse(raw("metric").unwrap_or("l2")),
            encoding: index::vector::HnswEncoding::parse(raw("encode").unwrap_or_default()),
            lsg,
        }
    }
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

/// Rows a PLAID chunk grows to before a dump starts a new one.
///
/// The engine's segment holds `DEFAULT_SEGMENT_CAPACITY` = 8M rows
/// (`common/default_values.cppm:69`) and a segment's mirror must stay loadable
/// as a handful of chunks, so `8M / max_open_chunks_per_index` (8) is the size a
/// chunk aims for. A chunk is a *dump accumulator*, not a segment: the segment
/// is what seals, and a sealed segment's chunks never change again.
pub(crate) const PLAID_CHUNK_TARGET_ROWS: usize = 1 << 20;

/// A chunk below `total / PLAID_SMALL_CHUNK_DIVISOR` is folded into its
/// neighbour even when the chunk count is under the cap: eight chunks that are
/// all tiny still make the loader copy the whole mirror.
pub(crate) const PLAID_SMALL_CHUNK_DIVISOR: u32 = 16;

/// Outliers buffered before the centroid set is expanded
/// (`PlaidIndexInMem::Update`'s buffer, the reference's 100).
pub(crate) const PLAID_OUTLIER_BUFFER: usize = 100;

/// L2 distance above which a document counts as an outlier of the centroid set.
pub(crate) const PLAID_OUTLIER_THRESHOLD: f32 = 0.1;

/// Which chunks an optimize pass folds, from their document counts in chunk-id
/// order: `Some(positions)` when the mirror needs work, `None` when it does not.
///
/// - over the cap: every chunk folds into one, because the loader would
///   otherwise merge them all on every open;
/// - under the cap but with a chunk below `total / 16`: that chunk and its
///   neighbour fold together. Folding *all* of them for one small chunk would
///   rewrite the mirror for nothing, which is the cost this rule exists to keep
///   off the write path.
pub(crate) fn plaid_fold_plan(doc_nums: &[u32], cap: usize) -> Option<Vec<usize>> {
    if doc_nums.len() > cap {
        return Some((0..doc_nums.len()).collect());
    }
    if doc_nums.len() < 2 {
        return None;
    }
    let total: u64 = doc_nums.iter().map(|n| u64::from(*n)).sum();
    let (smallest, &small_docs) = doc_nums.iter().enumerate().min_by_key(|(_, n)| **n)?;
    if u64::from(small_docs) * u64::from(PLAID_SMALL_CHUNK_DIVISOR) >= total {
        return None;
    }
    // Its neighbour: the previous chunk, so the pair covers a contiguous run of
    // documents (`base_row_id` order is chunk-id order).
    let neighbour = smallest.saturating_sub(1).min(doc_nums.len() - 2);
    let mut plan = vec![neighbour, neighbour + 1];
    plan.sort_unstable();
    plan.dedup();
    Some(plan)
}

/// Squared L2 distance beyond which a document is an outlier of `codec`.
///
/// The scale is the codec's own: the median squared distance between a centroid
/// and its nearest neighbour, halved. A document farther from every centroid than
/// half the spacing between them is served no better by the codec than by the
/// neighbour, which is the case an expansion can fix.
///
/// An absolute constant cannot work here - a `TENSOR` column holds whatever
/// floats the caller stored, normalized embeddings or not - and the reference
/// never calls its own `PlaidIndex::ExpandCentroids`, so there is no default to
/// inherit.
pub(crate) fn plaid_outlier_threshold_sq(codec: &PlaidIndex, dims: usize) -> f32 {
    if dims == 0 || codec.n_centroids() < 2 {
        return 0.0;
    }
    // At most 256 centroids: the estimate is a median over pairs, and a full
    // codec's k^2 x dims is more than a buffering heuristic is worth.
    let stride = (codec.n_centroids() as usize / 256).max(1);
    let picked: Vec<usize> = (0..codec.n_centroids() as usize).step_by(stride).collect();
    let data = codec.centroids_data();
    let mut spacing: Vec<f32> = Vec::with_capacity(picked.len());
    for (i, a) in picked.iter().enumerate() {
        let mut best = f32::MAX;
        for (j, b) in picked.iter().enumerate() {
            if i == j {
                continue;
            }
            let mut d = 0.0f32;
            for k in 0..dims {
                let diff = data[a * dims + k] - data[b * dims + k];
                d += diff * diff;
            }
            best = best.min(d);
        }
        if best.is_finite() {
            spacing.push(best);
        }
    }
    if spacing.is_empty() {
        return 0.0;
    }
    spacing.sort_by(|x, y| x.partial_cmp(y).unwrap_or(std::cmp::Ordering::Equal));
    spacing[spacing.len() / 2] / 2.0
}

/// The merge a mirror last performed: its source chunk ids, the chunk it wrote,
/// and the `(len, mtime)` of each source.
///
/// A merge whose deprecation did not land leaves the sources listed beside the
/// chunk that already contains them. Merging that set again would count those
/// documents twice, so the manifest is what makes the retry a no-op instead.
#[derive(Debug, Default, Clone, PartialEq, Eq)]
pub(crate) struct PlaidMergeManifest {
    pub(crate) sources: Vec<u64>,
    pub(crate) stamps: Vec<(u64, u64)>,
    pub(crate) merged: u64,
}

impl PlaidMergeManifest {
    pub(crate) const FILE: &'static str = "merge.manifest";

    pub(crate) fn read(dir: &std::path::Path) -> Option<Self> {
        let text = std::fs::read_to_string(dir.join(Self::FILE)).ok()?;
        let mut manifest = PlaidMergeManifest::default();
        for line in text.lines() {
            let (key, value) = line.split_once('=')?;
            match key.trim() {
                "sources" => {
                    manifest.sources = value
                        .split(',')
                        .filter_map(|v| v.trim().parse().ok())
                        .collect();
                }
                "stamps" => {
                    manifest.stamps = value
                        .split(',')
                        .filter_map(|v| {
                            let (len, mtime) = v.trim().split_once(':')?;
                            Some((len.parse().ok()?, mtime.parse().ok()?))
                        })
                        .collect();
                }
                "merged" => manifest.merged = value.trim().parse().unwrap_or(0),
                _ => {}
            }
        }
        (!manifest.sources.is_empty()).then_some(manifest)
    }

    pub(crate) fn write(&self, dir: &std::path::Path) -> std::io::Result<()> {
        let sources: Vec<String> = self.sources.iter().map(|id| id.to_string()).collect();
        let stamps: Vec<String> = self
            .stamps
            .iter()
            .map(|(len, mtime)| format!("{len}:{mtime}"))
            .collect();
        let text = format!(
            "sources={}\nstamps={}\nmerged={}\n",
            sources.join(","),
            stamps.join(","),
            self.merged
        );
        storage::definition::format::write_synced(&dir.join(Self::FILE), text.as_bytes())
    }

    /// Whether `ids` is exactly the set this manifest merged (with the same
    /// bytes), i.e. the chunk it wrote already covers them.
    pub(crate) fn covers(&self, ids: &[u64], stamps: &[(u64, u64)]) -> bool {
        !self.sources.is_empty() && self.sources == ids && self.stamps == stamps
    }
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
    /// First row id the chunk covers, from its catalog record. Chunks are read
    /// in `base_row_id` order: a fold of a subset writes a newer id for an
    /// earlier row range, and document order is what maps a chunk's documents to
    /// row ids.
    pub(crate) base_row_id: u64,
    pub(crate) index: PlaidIndex,
}

/// What an optimize pass should do with a PLAID mirror, decided from chunk
/// headers and the merge manifest alone.
pub(crate) struct PlaidFoldCandidates {
    /// The chunk ids this pass folds into one, in row order. Empty when the
    /// mirror is fine.
    pub(crate) plan: Vec<u64>,
    /// Chunk ids a previous merge already folded but that are still listed (its
    /// deprecation did not land). Their rows are inside `plan`'s successor and
    /// must not be counted again.
    pub(crate) folded: Vec<u64>,
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
    // Only the header's last field is needed, while a PLAID chunk is as large as
    // its vectors: map it (through the store, so an object-backed chunk maps its
    // range) and read the 24 header bytes instead of loading the whole file.
    let map = storage::chunk_store::open_file(path).ok()?;
    let head = map.get(..24)?;
    Some(u32::from_le_bytes(head[20..24].try_into().ok()?))
}

/// Bytes of token vectors one PLAID build batch materializes.
///
/// The batch is what bounds a build: `add_multiple_docs_embeddings` holds the
/// batch's vectors, their residuals and their packed codes at once, so a whole
/// table handed to it in one call cost GBs (`docs` + `flat` + residuals +
/// packed + the serialized image were five full-size buffers). Sized in bytes
/// rather than tokens because a ColBERT column is 128d and a tensor search
/// column is often 1536d - `32 MiB` is 65k tokens at one and 5.4k at the other.
pub(crate) const PLAID_BUILD_BATCH_BYTES: usize = 32 << 20;

/// Bytes of token vectors the centroid/quantizer training samples.
///
/// `PlaidIndex::Train` wants at least `32 * n_centroids` embeddings and samples
/// at most `256 * n_centroids`; on a large corpus that bound is gigabytes, so
/// the sample is capped here and drawn uniformly from the whole stream.
pub(crate) const PLAID_TRAIN_SAMPLE_BYTES: usize = 64 << 20;

/// Test-only override of [`PLAID_BUILD_BATCH_BYTES`] (`0` = use the constant).
///
/// A unit-sized table never crosses 32 MiB of token vectors, so the flush path
/// that makes the build bounded would otherwise be untested - the batch would
/// only ever be flushed once, at the end. A test sets this to a few hundred
/// bytes and gets a build with many flushes over the same data.
#[cfg(test)]
pub(crate) static PLAID_BUILD_BATCH_BYTES_OVERRIDE: std::sync::atomic::AtomicUsize =
    std::sync::atomic::AtomicUsize::new(0);

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
