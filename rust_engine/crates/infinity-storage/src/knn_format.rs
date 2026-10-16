//! C++-compatible on-disk layout for the **HNSW vector index**
//! (RUST_REWRITE_PLAN.md §2.6.1, task #70).
//!
//! Frozen in `rust_engine/docs/CPP_FORMAT_SPEC.md` §9.1 from `file:line`
//! citations into `src/storage/knn_index/hnsw/**`. A `chunk_{chunk_id}.idx`
//! file carries **no magic, no version and no checksum**; the C++ loader
//! asserts it consumed the file exactly (`hnsw_alg.cppm:541-543`), so the bytes
//! written here must match `HnswHandler::SaveToPtr` byte for byte.
//!
//! File order (plain vector store):
//!
//! ```text
//! u64 M                 v
//! u64 ef_construction   v  (= max(M, ef_construction))
//! u64 cur_vec_num          number of vectors in this chunk
//! u64 dim                  VecStoreMeta (plain store)
//! u64 Mmax0             v  GraphStoreMeta
//! u64 Mmax
//! i32 max_layer
//! i32 enterpoint
//! [vector region]          cur_vec_num * dim * element_size
//! [graph region]           u64 layer_sum, level0 region, layerx region
//! u32[cur_vec_num]         labels (SegmentOffset = rowid.segment_offset)
//! ```
//!
//! Everything is native little-endian.

use crate::cpp_format::FormatError;

/// `GetMmax(M)` from `hnsw_alg.cppm:69`: returns `(Mmax0, Mmax)`.
pub const fn get_mmax(m: u64) -> (u64, u64) {
    (2 * m, m)
}

/// The fixed-size header of an HNSW chunk file. Its eight fields occupy exactly
/// 56 bytes (6 × u64 + 2 × i32).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct HnswHeader {
    /// `M` — the neighbour limit for layers ≥ 1.
    pub m: u64,
    /// `ef_construction` as written (already `max(M, ef_construction)`).
    pub ef_construction: u64,
    /// Number of vectors stored in this chunk.
    pub cur_vec_num: u64,
    /// `VecStoreMeta` for the plain store: the vector dimension.
    pub dim: u64,
    /// `GraphStoreMeta.Mmax0` (always [`get_mmax`]`.0`).
    pub mmax0: u64,
    /// `GraphStoreMeta.Mmax` (always [`get_mmax`]`.1`).
    pub mmax: u64,
    /// `GraphStoreMeta.max_layer_`.
    pub max_layer: i32,
    /// `GraphStoreMeta.enterpoint_`.
    pub enterpoint: i32,
}

/// Bytes the header occupies: `6 * 8 + 2 * 4` = 56.
pub const HNSW_HEADER_LEN: usize = 56;

impl HnswHeader {
    /// Builds a header for a plain store, deriving `Mmax0`/`Mmax` from `M`.
    pub fn plain(m: u64, ef_construction: u64, cur_vec_num: u64, dim: u64) -> Self {
        let (mmax0, mmax) = get_mmax(m);
        Self {
            m,
            ef_construction: ef_construction.max(m),
            cur_vec_num,
            dim,
            mmax0,
            mmax,
            max_layer: 0,
            enterpoint: 0,
        }
    }

    /// Encodes the header exactly as `SaveToPtr` lays it out.
    pub fn encode(&self) -> Vec<u8> {
        let mut out = Vec::with_capacity(HNSW_HEADER_LEN);
        out.extend_from_slice(&self.m.to_le_bytes());
        out.extend_from_slice(&self.ef_construction.to_le_bytes());
        out.extend_from_slice(&self.cur_vec_num.to_le_bytes());
        out.extend_from_slice(&self.dim.to_le_bytes());
        out.extend_from_slice(&self.mmax0.to_le_bytes());
        out.extend_from_slice(&self.mmax.to_le_bytes());
        out.extend_from_slice(&self.max_layer.to_le_bytes());
        out.extend_from_slice(&self.enterpoint.to_le_bytes());
        out
    }

    /// Decodes the header, returning it plus the bytes consumed (always
    /// [`HNSW_HEADER_LEN`]).
    pub fn decode(buf: &[u8]) -> Result<(Self, usize), FormatError> {
        if buf.len() < HNSW_HEADER_LEN {
            return Err(FormatError::Truncated {
                need: HNSW_HEADER_LEN,
                got: buf.len(),
            });
        }
        let u64_at = |at: usize| u64::from_le_bytes(buf[at..at + 8].try_into().unwrap());
        let i32_at = |at: usize| i32::from_le_bytes(buf[at..at + 4].try_into().unwrap());
        Ok((
            Self {
                m: u64_at(0),
                ef_construction: u64_at(8),
                cur_vec_num: u64_at(16),
                dim: u64_at(24),
                mmax0: u64_at(32),
                mmax: u64_at(40),
                max_layer: i32_at(48),
                enterpoint: i32_at(52),
            },
            HNSW_HEADER_LEN,
        ))
    }

    /// Bytes the vector region occupies for a plain store.
    pub fn vector_region_len(&self, element_size: usize) -> usize {
        self.cur_vec_num as usize * self.dim as usize * element_size
    }
}

/// Encodes the trailing `labels` array (`u32[cur_vec_num]`), one
/// `SegmentOffset` (i.e. `rowid.segment_offset`) per vector.
pub fn encode_labels(labels: &[u32]) -> Vec<u8> {
    let mut out = Vec::with_capacity(labels.len() * 4);
    for l in labels {
        out.extend_from_slice(&l.to_le_bytes());
    }
    out
}

/// Decodes the trailing `labels` array.
pub fn decode_labels(buf: &[u8]) -> Result<Vec<u32>, FormatError> {
    if !buf.len().is_multiple_of(4) {
        return Err(FormatError::Truncated {
            need: (buf.len() / 4 + 1) * 4,
            got: buf.len(),
        });
    }
    Ok(buf
        .chunks_exact(4)
        .map(|c| u32::from_le_bytes(c.try_into().unwrap()))
        .collect())
}

/// Assembles a whole plain-store chunk file: header, vector region, graph
/// region, labels.
///
/// `graph_region` is passed through verbatim because its inner stride depends
/// on the C++ `sizeof(VertexL0)`/`sizeof(VertexLX)` padding, which §9.5 marks
/// as **VERIFY**; callers that build it themselves must use
/// `level0_size = sizeof(VertexL0) + 4*Mmax0` and
/// `levelx_size = sizeof(VertexLX) + 4*Mmax`.
pub fn encode_hnsw_chunk(
    header: &HnswHeader,
    vector_region: &[u8],
    graph_region: &[u8],
    labels: &[u32],
) -> Vec<u8> {
    let mut out = header.encode();
    out.extend_from_slice(vector_region);
    out.extend_from_slice(graph_region);
    out.extend_from_slice(&encode_labels(labels));
    out
}

/// Expected byte length of the graph region: `u64 layer_sum`, then the level-0
/// vertex records, then the level-≥1 records.
pub fn graph_region_len(
    layer_sum: u64,
    cur_vec_num: u64,
    level0_size: usize,
    levelx_size: usize,
) -> usize {
    8 + cur_vec_num as usize * level0_size + layer_sum as usize * levelx_size
}

/// A decoded HNSW chunk file.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct HnswChunk {
    pub header: HnswHeader,
    /// Raw vector region (`cur_vec_num * dim * element_size` bytes).
    pub vectors: Vec<u8>,
    /// Raw graph region (see [`graph_region_len`]).
    pub graph: Vec<u8>,
    /// `SegmentOffset` per vector.
    pub labels: Vec<u32>,
}

/// Splits a chunk file into its header, vector region, graph region and labels.
///
/// `element_size` is the width of one vector element (4 for `float`, 1 for
/// `uint8`/`int8`, …); `vertex_l0_size`/`vertex_lx_size` are the C++
/// `sizeof(VertexL0)`/`sizeof(VertexLX)` (24 and 4 on x86_64 SysV — see §9.5).
/// The graph region is checked against `u64 layer_sum` plus the two strides, so
/// a truncated or padded file is reported instead of silently accepted (the C++
/// loader makes the same assertion).
pub fn decode_hnsw_chunk(
    buf: &[u8],
    element_size: usize,
    vertex_l0_size: usize,
    vertex_lx_size: usize,
) -> Result<HnswChunk, FormatError> {
    let (header, mut at) = HnswHeader::decode(buf)?;
    let vec_len = header.vector_region_len(element_size);
    let labels_len = 4 * header.cur_vec_num as usize;
    if buf.len() < at + vec_len + labels_len + 8 {
        return Err(FormatError::Truncated {
            need: at + vec_len + labels_len + 8,
            got: buf.len(),
        });
    }
    let vectors = buf[at..at + vec_len].to_vec();
    at += vec_len;
    let graph = buf[at..buf.len() - labels_len].to_vec();

    let layer_sum = u64::from_le_bytes(graph[0..8].try_into().unwrap());
    let expected = graph_region_len(
        layer_sum,
        header.cur_vec_num,
        level0_stride(vertex_l0_size, header.mmax0),
        levelx_stride(vertex_lx_size, header.mmax),
    );
    if graph.len() != expected {
        return Err(FormatError::Truncated {
            need: graph.len().max(expected),
            got: graph.len().min(expected),
        });
    }
    let labels = decode_labels(&buf[buf.len() - labels_len..])?;
    Ok(HnswChunk {
        header,
        vectors,
        graph,
        labels,
    })
}

/// Stride of one level-0 vertex record: `sizeof(VertexL0) + 4 * Mmax0`.
///
/// `vertex_l0_size` must be the C++ `sizeof(VertexL0)` (24 on x86_64 SysV), which
/// §9.5 flags as needing verification against a real file.
pub const fn level0_stride(vertex_l0_size: usize, mmax0: u64) -> usize {
    vertex_l0_size + 4 * mmax0 as usize
}

/// Stride of one level-≥1 vertex record: `sizeof(VertexLX) + 4 * Mmax`.
pub const fn levelx_stride(vertex_lx_size: usize, mmax: u64) -> usize {
    vertex_lx_size + 4 * mmax as usize
}

/// `sizeof(VertexL0)` on the x86_64 SysV ABI.
///
/// Measured with g++ 13 (`offsetof`: layer_n_=0, layers_p_=8, neighbor_n_=16,
/// neighbors_=20, sizeof=24), which settles the §9.5 VERIFY item: the 4 bytes
/// after `layer_n_` are alignment padding and the struct rounds up to 24 for the
/// 8-byte pointer.
pub const VERTEX_L0_SIZE: usize = 24;
/// `sizeof(VertexLX)`: a single `i32` followed by the flexible array.
pub const VERTEX_LX_SIZE: usize = 4;

/// A vertex in the serialized HNSW graph.
#[derive(Debug, Clone, PartialEq, Eq, Default)]
pub struct HnswGraphVertex {
    /// `layer_n_` — how many layers above level 0 this vertex appears in.
    pub layer_count: i32,
    /// Level-0 neighbours (vertex ids), at most `Mmax0`.
    pub neighbors_l0: Vec<u32>,
    /// Upper-layer neighbours, one entry per layer (at most `Mmax` each).
    pub upper: Vec<Vec<u32>>,
}

/// Encodes the `GraphStoreInner` region: `u64 layer_sum`, the level-0 vertex
/// records, then the upper-layer records.
///
/// Byte layout per `graph_store.cppm:31-46,172-219` — level-0 records are
/// `VERTEX_L0_SIZE + 4*Mmax0` bytes with `layers_p_` rewritten from a pointer to
/// a `u64` byte offset into the upper-layer region; upper-layer records are
/// `VERTEX_LX_SIZE + 4*Mmax` bytes (neighbour count, then the neighbour ids,
/// zero-padded to `Mmax`).
pub fn encode_graph_region(vertices: &[HnswGraphVertex], mmax0: u64, mmax: u64) -> Vec<u8> {
    let l0 = level0_stride(VERTEX_L0_SIZE, mmax0);
    let lx = levelx_stride(VERTEX_LX_SIZE, mmax);
    let layer_sum: u64 = vertices.iter().map(|v| v.layer_count.max(0) as u64).sum();

    let mut upper = Vec::with_capacity(layer_sum as usize * lx);
    // `layers_p_` for each vertex: the offset of its upper-layer block.
    let mut layers_p = Vec::with_capacity(vertices.len());
    for v in vertices {
        layers_p.push(upper.len() as u64);
        for nbrs in v.upper.iter().take(v.layer_count.max(0) as usize) {
            let mut rec = Vec::with_capacity(lx);
            rec.extend_from_slice(&(nbrs.len() as i32).to_le_bytes());
            for k in 0..mmax as usize {
                rec.extend_from_slice(&nbrs.get(k).copied().unwrap_or(0).to_le_bytes());
            }
            rec.resize(lx, 0);
            upper.extend_from_slice(&rec);
        }
    }

    let mut out = Vec::with_capacity(8 + vertices.len() * l0 + upper.len());
    out.extend_from_slice(&layer_sum.to_le_bytes());
    for (v, layer_p) in vertices.iter().zip(&layers_p) {
        let mut rec = Vec::with_capacity(l0);
        rec.extend_from_slice(&v.layer_count.to_le_bytes()); // layer_n_
        rec.resize(16, 0); // 4 bytes of padding, then the 8-byte layers_p_
        rec[8..16].copy_from_slice(&layer_p.to_le_bytes());
        rec.extend_from_slice(&(v.neighbors_l0.len() as i32).to_le_bytes());
        for k in 0..mmax0 as usize {
            rec.extend_from_slice(&v.neighbors_l0.get(k).copied().unwrap_or(0).to_le_bytes());
        }
        rec.resize(l0, 0);
        out.extend_from_slice(&rec);
    }
    out.extend_from_slice(&upper);
    out
}

/// Decodes a `GraphStoreInner` region produced by [`encode_graph_region`].
pub fn decode_graph_region(
    bytes: &[u8],
    vertex_count: usize,
    mmax0: u64,
    mmax: u64,
) -> Result<Vec<HnswGraphVertex>, FormatError> {
    let l0 = level0_stride(VERTEX_L0_SIZE, mmax0);
    let lx = levelx_stride(VERTEX_LX_SIZE, mmax);
    if bytes.len() < 8 {
        return Err(FormatError::Truncated {
            need: 8,
            got: bytes.len(),
        });
    }
    let layer_sum = u64::from_le_bytes(bytes[0..8].try_into().unwrap());
    let upper_at = 8 + vertex_count * l0;
    if bytes.len() != upper_at + layer_sum as usize * lx {
        return Err(FormatError::Truncated {
            need: upper_at + layer_sum as usize * lx,
            got: bytes.len(),
        });
    }

    let mut out = Vec::with_capacity(vertex_count);
    for v in 0..vertex_count {
        let at = 8 + v * l0;
        let layer_count = i32::from_le_bytes(bytes[at..at + 4].try_into().unwrap());
        let layers_p = u64::from_le_bytes(bytes[at + 8..at + 16].try_into().unwrap()) as usize;
        let neighbor_n = i32::from_le_bytes(bytes[at + 16..at + 20].try_into().unwrap());
        let mut neighbors_l0 = Vec::with_capacity(neighbor_n.max(0) as usize);
        for k in 0..neighbor_n.max(0) as usize {
            let s = at + 20 + k * 4;
            neighbors_l0.push(u32::from_le_bytes(bytes[s..s + 4].try_into().unwrap()));
        }
        let mut upper = Vec::with_capacity(layer_count.max(0) as usize);
        for l in 0..layer_count.max(0) as usize {
            let s = upper_at + layers_p + l * lx;
            let n = i32::from_le_bytes(bytes[s..s + 4].try_into().unwrap());
            let mut nbrs = Vec::with_capacity(n.max(0) as usize);
            for k in 0..n.max(0) as usize {
                let e = s + 4 + k * 4;
                nbrs.push(u32::from_le_bytes(bytes[e..e + 4].try_into().unwrap()));
            }
            upper.push(nbrs);
        }
        out.push(HnswGraphVertex {
            layer_count,
            neighbors_l0,
            upper,
        });
    }
    Ok(out)
}

// ---------------------------------------------------------------------------
// IVF chunk (plain storage) — spec §9.2
// ---------------------------------------------------------------------------

/// The fixed prefix of an IVF `chunk_{id}.idx` file (spec §9.2): four `u32`s
/// followed by `centroid_count * dimension` `f32` centroids.
///
/// The file is **not self-describing**: the logical column type, the embedding
/// element type and the IVF options (metric, storage kind, SQ/PQ parameters)
/// live in the catalog's IndexBase JSON, not here.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct IvfChunkHeader {
    pub row_count: u32,
    pub embedding_count: u32,
    pub dimension: u32,
    pub centroid_count: u32,
}

/// One plain-storage part: the vectors assigned to one centroid.
#[derive(Debug, Clone, PartialEq, Eq, Default)]
pub struct IvfPlainPart {
    /// `SegmentOffset` (rowid.segment_offset) of each vector in this part.
    pub segment_offsets: Vec<u32>,
    /// The vectors' raw element bytes (`segment_offsets.len() * dimension *
    /// element_size`).
    pub vectors: Vec<u8>,
}

/// Encodes an IVF chunk in the **plain** storage layout (`kPlain`).
///
/// `row_count`, `embedding_count`, `dimension` and `centroid_count` come first,
/// then the `f32` centroids, then each part as `u32 embedding_num`,
/// `u32[embedding_num]` segment offsets and the raw vectors
/// (`ivf_index_storage.cpp:100-104`;
/// `ivf_index_storage_parts_impl.cpp:96-107,162-163,560-571`).
pub fn encode_ivf_plain_chunk(
    header: &IvfChunkHeader,
    centroids: &[f32],
    parts: &[IvfPlainPart],
) -> Result<Vec<u8>, FormatError> {
    if centroids.len() != header.centroid_count as usize * header.dimension as usize {
        return Err(FormatError::Truncated {
            need: header.centroid_count as usize * header.dimension as usize,
            got: centroids.len(),
        });
    }
    let mut out = Vec::new();
    out.extend_from_slice(&header.row_count.to_le_bytes());
    out.extend_from_slice(&header.embedding_count.to_le_bytes());
    out.extend_from_slice(&header.dimension.to_le_bytes());
    out.extend_from_slice(&header.centroid_count.to_le_bytes());
    for c in centroids {
        out.extend_from_slice(&c.to_le_bytes());
    }
    for part in parts {
        out.extend_from_slice(&(part.segment_offsets.len() as u32).to_le_bytes());
        for s in &part.segment_offsets {
            out.extend_from_slice(&s.to_le_bytes());
        }
        out.extend_from_slice(&part.vectors);
    }
    Ok(out)
}

/// Decodes an IVF chunk in the plain storage layout.
///
/// `element_size` is the element width in bytes (4 for `float`); it is not
/// stored in the file, so the caller must know the column's element type.
pub fn decode_ivf_plain_chunk(
    bytes: &[u8],
    element_size: usize,
) -> Result<(IvfChunkHeader, Vec<f32>, Vec<IvfPlainPart>), FormatError> {
    let short = |need: usize| FormatError::Truncated {
        need,
        got: bytes.len(),
    };
    if bytes.len() < 16 {
        return Err(short(16));
    }
    let u32_at = |at: usize| u32::from_le_bytes(bytes[at..at + 4].try_into().unwrap());
    let header = IvfChunkHeader {
        row_count: u32_at(0),
        embedding_count: u32_at(4),
        dimension: u32_at(8),
        centroid_count: u32_at(12),
    };
    let centroid_floats = header.centroid_count as usize * header.dimension as usize;
    let mut at = 16;
    if bytes.len() < at + centroid_floats * 4 {
        return Err(short(at + centroid_floats * 4));
    }
    let centroids: Vec<f32> = (0..centroid_floats)
        .map(|i| f32::from_le_bytes(bytes[at + i * 4..at + i * 4 + 4].try_into().unwrap()))
        .collect();
    at += centroid_floats * 4;

    let mut parts = Vec::with_capacity(header.centroid_count as usize);
    for _ in 0..header.centroid_count {
        if bytes.len() < at + 4 {
            return Err(short(at + 4));
        }
        let n = u32_at(at) as usize;
        at += 4;
        if bytes.len() < at + n * 4 {
            return Err(short(at + n * 4));
        }
        let segment_offsets: Vec<u32> = (0..n).map(|i| u32_at(at + i * 4)).collect();
        at += n * 4;
        let vec_len = n * header.dimension as usize * element_size;
        if bytes.len() < at + vec_len {
            return Err(short(at + vec_len));
        }
        let vectors = bytes[at..at + vec_len].to_vec();
        at += vec_len;
        parts.push(IvfPlainPart {
            segment_offsets,
            vectors,
        });
    }
    Ok((header, centroids, parts))
}

/// One scalar-quantized part (spec §9.2 `kScalarQuantization`): the shared
/// `common_vec_a`/`common_vec_b` live in the chunk base, each part carries its
/// quantized codes plus their dot product with the query vector.
#[derive(Debug, Clone, PartialEq)]
pub struct IvfSqPart {
    /// `SegmentOffset` (rowid.segment_offset) of each vector in this part.
    pub segment_offsets: Vec<u32>,
    /// Quantized codes, `segment_offsets.len() * sq_bytes` (8-bit: `dim`,
    /// 4-bit: `(dim + 1) / 2` per vector).
    pub codes: Vec<u8>,
    /// `dot_v_e`, one `f32` per vector.
    pub dot_v_e: Vec<f32>,
}

/// Bytes each scalar-quantized vector occupies for `dimension` and `sq_bits`
/// (8 or 4 bits per component).
pub fn sq_bytes_per_vector(dimension: u32, sq_bits: u8) -> usize {
    if sq_bits == 4 {
        (dimension as usize).div_ceil(2)
    } else {
        dimension as usize
    }
}

/// A decoded scalar-quantized IVF chunk.
#[derive(Debug, Clone, PartialEq)]
pub struct IvfSqChunk {
    pub header: IvfChunkHeader,
    pub centroids: Vec<f32>,
    pub common_vec_a: Vec<f32>,
    pub common_vec_b: Vec<f32>,
    pub parts: Vec<IvfSqPart>,
}

/// Encodes an IVF chunk in the scalar-quantized storage layout.
///
/// Layout (`ivf_index_storage_parts_impl.cpp:256-263,672-688`): the plain header
/// and centroids, then the shared `f32[dim] common_vec_a` and
/// `f32[dim] common_vec_b`, then each part as `u32 embedding_num` +
/// `u32[embedding_num]` offsets + the codes + `embedding_num` `f32` dot products.
pub fn encode_ivf_sq_chunk(
    header: &IvfChunkHeader,
    centroids: &[f32],
    common_vec_a: &[f32],
    common_vec_b: &[f32],
    parts: &[IvfSqPart],
    sq_bits: u8,
) -> Result<Vec<u8>, FormatError> {
    let dim = header.dimension as usize;
    let bad = |need: usize, got: usize| FormatError::Truncated { need, got };
    if centroids.len() != header.centroid_count as usize * dim {
        return Err(bad(header.centroid_count as usize * dim, centroids.len()));
    }
    if common_vec_a.len() != dim || common_vec_b.len() != dim {
        return Err(bad(dim, common_vec_a.len().min(common_vec_b.len())));
    }
    let per_vector = sq_bytes_per_vector(header.dimension, sq_bits);
    let mut out = Vec::new();
    out.extend_from_slice(&header.row_count.to_le_bytes());
    out.extend_from_slice(&header.embedding_count.to_le_bytes());
    out.extend_from_slice(&header.dimension.to_le_bytes());
    out.extend_from_slice(&header.centroid_count.to_le_bytes());
    for v in centroids.iter().chain(common_vec_a).chain(common_vec_b) {
        out.extend_from_slice(&v.to_le_bytes());
    }
    for part in parts {
        let n = part.segment_offsets.len();
        if part.codes.len() != n * per_vector || part.dot_v_e.len() != n {
            return Err(bad(n * per_vector, part.codes.len()));
        }
        out.extend_from_slice(&(n as u32).to_le_bytes());
        for s in &part.segment_offsets {
            out.extend_from_slice(&s.to_le_bytes());
        }
        out.extend_from_slice(&part.codes);
        for d in &part.dot_v_e {
            out.extend_from_slice(&d.to_le_bytes());
        }
    }
    Ok(out)
}

/// Decodes an IVF chunk in the scalar-quantized storage layout, returning the
/// header, centroids, the two shared vectors and the parts.
pub fn decode_ivf_sq_chunk(bytes: &[u8], sq_bits: u8) -> Result<IvfSqChunk, FormatError> {
    let short = |need: usize| FormatError::Truncated {
        need,
        got: bytes.len(),
    };
    if bytes.len() < 16 {
        return Err(short(16));
    }
    let u32_at = |at: usize| u32::from_le_bytes(bytes[at..at + 4].try_into().unwrap());
    let header = IvfChunkHeader {
        row_count: u32_at(0),
        embedding_count: u32_at(4),
        dimension: u32_at(8),
        centroid_count: u32_at(12),
    };
    let dim = header.dimension as usize;
    let floats = header.centroid_count as usize * dim + 2 * dim;
    let mut at = 16;
    if bytes.len() < at + floats * 4 {
        return Err(short(at + floats * 4));
    }
    let read_f32 = |from: &mut usize, n: usize| -> Vec<f32> {
        let out = (0..n)
            .map(|i| {
                f32::from_le_bytes(bytes[*from + i * 4..*from + i * 4 + 4].try_into().unwrap())
            })
            .collect();
        *from += n * 4;
        out
    };
    let centroids = read_f32(&mut at, header.centroid_count as usize * dim);
    let common_vec_a = read_f32(&mut at, dim);
    let common_vec_b = read_f32(&mut at, dim);

    let per_vector = sq_bytes_per_vector(header.dimension, sq_bits);
    let mut parts = Vec::with_capacity(header.centroid_count as usize);
    for _ in 0..header.centroid_count {
        if bytes.len() < at + 4 {
            return Err(short(at + 4));
        }
        let n = u32_at(at) as usize;
        at += 4;
        if bytes.len() < at + n * 4 {
            return Err(short(at + n * 4));
        }
        let segment_offsets: Vec<u32> = (0..n).map(|i| u32_at(at + i * 4)).collect();
        at += n * 4;
        if bytes.len() < at + n * per_vector + n * 4 {
            return Err(short(at + n * per_vector + n * 4));
        }
        let codes = bytes[at..at + n * per_vector].to_vec();
        at += n * per_vector;
        let dot_v_e = read_f32(&mut at, n);
        parts.push(IvfSqPart {
            segment_offsets,
            codes,
            dot_v_e,
        });
    }
    Ok(IvfSqChunk {
        header,
        centroids,
        common_vec_a,
        common_vec_b,
        parts,
    })
}

/// One product-quantized part (spec §9.2 `kProductQuantization`): the per-chunk
/// codebook lives in the chunk base, each part carries its code words.
#[derive(Debug, Clone, PartialEq, Eq, Default)]
pub struct IvfPqPart {
    /// `SegmentOffset` (rowid.segment_offset) of each vector in this part.
    pub segment_offsets: Vec<u32>,
    /// `PQ_Code_Storage::last_code_id_` - the code count in this part.
    pub last_code_id: u64,
    /// Raw code words: `storage_size * word_size` bytes, `word_size` 1 for
    /// 4/8/12-bit codes or 2 for 16-bit codes.
    pub codes: Vec<u8>,
    /// Width of one code word (1 or 2 bytes).
    pub word_size: usize,
}

/// The per-chunk product-quantization codebook.
#[derive(Debug, Clone, PartialEq)]
pub struct IvfPqCodebook {
    /// `real_subspace_centroid_num`.
    pub real_subspace_centroid_num: u32,
    /// `f32[dim * real_subspace_centroid_num]`.
    pub codewords: Vec<f32>,
    /// `f32[real_subspace_centroid_num * subspace_num]`.
    pub subspace_weights: Vec<f32>,
}

/// A decoded product-quantized IVF chunk.
#[derive(Debug, Clone, PartialEq)]
pub struct IvfPqChunk {
    pub header: IvfChunkHeader,
    pub centroids: Vec<f32>,
    pub codebook: IvfPqCodebook,
    pub parts: Vec<IvfPqPart>,
}

/// Encodes an IVF chunk in the product-quantized storage layout.
///
/// `ivf_index_storage_parts_impl.cpp:355-367,904-916,1057-1064`: the plain header
/// and centroids, then the codebook (`u32 real_subspace_centroid_num`, the
/// codewords, the subspace weights), then each part as `u32 embedding_num` +
/// `u32[embedding_num]` offsets + `u64 last_code_id` + `u32 storage_size` + the
/// code words.
pub fn encode_ivf_pq_chunk(
    header: &IvfChunkHeader,
    centroids: &[f32],
    codebook: &IvfPqCodebook,
    parts: &[IvfPqPart],
) -> Result<Vec<u8>, FormatError> {
    let dim = header.dimension as usize;
    let bad = |need: usize, got: usize| FormatError::Truncated { need, got };
    if centroids.len() != header.centroid_count as usize * dim {
        return Err(bad(header.centroid_count as usize * dim, centroids.len()));
    }
    let mut out = Vec::new();
    out.extend_from_slice(&header.row_count.to_le_bytes());
    out.extend_from_slice(&header.embedding_count.to_le_bytes());
    out.extend_from_slice(&header.dimension.to_le_bytes());
    out.extend_from_slice(&header.centroid_count.to_le_bytes());
    for c in centroids {
        out.extend_from_slice(&c.to_le_bytes());
    }
    out.extend_from_slice(&codebook.real_subspace_centroid_num.to_le_bytes());
    for f in &codebook.codewords {
        out.extend_from_slice(&f.to_le_bytes());
    }
    for f in &codebook.subspace_weights {
        out.extend_from_slice(&f.to_le_bytes());
    }
    for part in parts {
        let n = part.segment_offsets.len();
        if part.word_size == 0 || !part.codes.len().is_multiple_of(part.word_size) {
            return Err(bad(part.word_size, part.codes.len()));
        }
        out.extend_from_slice(&(n as u32).to_le_bytes());
        for s in &part.segment_offsets {
            out.extend_from_slice(&s.to_le_bytes());
        }
        out.extend_from_slice(&part.last_code_id.to_le_bytes());
        out.extend_from_slice(&((part.codes.len() / part.word_size) as u32).to_le_bytes());
        out.extend_from_slice(&part.codes);
    }
    Ok(out)
}

/// Decodes an IVF chunk in the product-quantized storage layout.
///
/// `word_size` is the code word width (1 or 2 bytes), which the file does not
/// record - it follows from the code bit width in the IVF options.
pub fn decode_ivf_pq_chunk(
    bytes: &[u8],
    word_size: usize,
    codeword_floats: usize,
    subspace_weight_floats: usize,
) -> Result<IvfPqChunk, FormatError> {
    let short = |need: usize| FormatError::Truncated {
        need,
        got: bytes.len(),
    };
    if bytes.len() < 16 || word_size == 0 {
        return Err(short(16));
    }
    let u32_at = |at: usize| u32::from_le_bytes(bytes[at..at + 4].try_into().unwrap());
    let header = IvfChunkHeader {
        row_count: u32_at(0),
        embedding_count: u32_at(4),
        dimension: u32_at(8),
        centroid_count: u32_at(12),
    };
    let dim = header.dimension as usize;
    let centroid_floats = header.centroid_count as usize * dim;
    let mut at = 16;
    let read_f32 = |from: &mut usize, n: usize| -> Vec<f32> {
        let out = (0..n)
            .map(|i| {
                f32::from_le_bytes(bytes[*from + i * 4..*from + i * 4 + 4].try_into().unwrap())
            })
            .collect();
        *from += n * 4;
        out
    };
    if bytes.len() < at + centroid_floats * 4 {
        return Err(short(at + centroid_floats * 4));
    }
    let centroids = read_f32(&mut at, centroid_floats);
    if bytes.len() < at + 4 {
        return Err(short(at + 4));
    }
    let real_subspace_centroid_num = u32_at(at);
    at += 4;
    let need = at + (codeword_floats + subspace_weight_floats) * 4;
    if bytes.len() < need {
        return Err(short(need));
    }
    let codewords = read_f32(&mut at, codeword_floats);
    let subspace_weights = read_f32(&mut at, subspace_weight_floats);

    let mut parts = Vec::with_capacity(header.centroid_count as usize);
    for _ in 0..header.centroid_count {
        if bytes.len() < at + 4 {
            return Err(short(at + 4));
        }
        let n = u32_at(at) as usize;
        at += 4;
        if bytes.len() < at + n * 4 {
            return Err(short(at + n * 4));
        }
        let segment_offsets: Vec<u32> = (0..n).map(|i| u32_at(at + i * 4)).collect();
        at += n * 4;
        if bytes.len() < at + 12 {
            return Err(short(at + 12));
        }
        let last_code_id = u64::from_le_bytes(bytes[at..at + 8].try_into().unwrap());
        let storage_size = u32_at(at + 8) as usize;
        at += 12;
        let code_bytes = storage_size * word_size;
        if bytes.len() < at + code_bytes {
            return Err(short(at + code_bytes));
        }
        let codes = bytes[at..at + code_bytes].to_vec();
        at += code_bytes;
        parts.push(IvfPqPart {
            segment_offsets,
            last_code_id,
            codes,
            word_size,
        });
    }
    Ok(IvfPqChunk {
        header,
        centroids,
        codebook: IvfPqCodebook {
            real_subspace_centroid_num,
            codewords,
            subspace_weights,
        },
        parts,
    })
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn ivf_pq_chunk_roundtrip() {
        let header = IvfChunkHeader {
            row_count: 2,
            embedding_count: 2,
            dimension: 2,
            centroid_count: 1,
        };
        let codebook = IvfPqCodebook {
            real_subspace_centroid_num: 2,
            codewords: vec![1.0, 2.0, 3.0, 4.0],
            subspace_weights: vec![0.1, 0.2, 0.3, 0.4, 0.5, 0.6],
        };
        let parts = vec![IvfPqPart {
            segment_offsets: vec![7, 8],
            last_code_id: 5,
            codes: vec![0x12, 0x34, 0x56, 0x78],
            word_size: 1,
        }];
        let bytes = encode_ivf_pq_chunk(&header, &[0.5, 0.5], &codebook, &parts).unwrap();
        let got = decode_ivf_pq_chunk(&bytes, 1, 4, 6).unwrap();
        assert_eq!(got.header, header);
        assert_eq!(got.centroids, vec![0.5, 0.5]);
        assert_eq!(got.codebook, codebook);
        assert_eq!(got.parts, parts);

        // 16-bit codes use two bytes per word.
        let wide = vec![IvfPqPart {
            segment_offsets: vec![0],
            last_code_id: 1,
            codes: vec![0x01, 0x02],
            word_size: 2,
        }];
        let bytes = encode_ivf_pq_chunk(&header, &[0.5, 0.5], &codebook, &wide).unwrap();
        let got = decode_ivf_pq_chunk(&bytes, 2, 4, 6).unwrap();
        assert_eq!(got.parts, wide);
    }

    #[test]
    fn ivf_pq_chunk_rejects_bad_code_words() {
        let header = IvfChunkHeader {
            row_count: 1,
            embedding_count: 1,
            dimension: 2,
            centroid_count: 1,
        };
        let codebook = IvfPqCodebook {
            real_subspace_centroid_num: 1,
            codewords: vec![0.0, 0.0],
            subspace_weights: vec![0.0],
        };
        // Codes are not a whole number of 2-byte words.
        let bad = vec![IvfPqPart {
            segment_offsets: vec![0],
            last_code_id: 1,
            codes: vec![1, 2, 3],
            word_size: 2,
        }];
        assert!(encode_ivf_pq_chunk(&header, &[0.0, 0.0], &codebook, &bad).is_err());
        // A zero word size is meaningless.
        let zero = vec![IvfPqPart {
            segment_offsets: vec![0],
            last_code_id: 1,
            codes: vec![],
            word_size: 0,
        }];
        assert!(encode_ivf_pq_chunk(&header, &[0.0, 0.0], &codebook, &zero).is_err());
    }

    #[test]
    fn ivf_sq_chunk_roundtrip() {
        let header = IvfChunkHeader {
            row_count: 2,
            embedding_count: 2,
            dimension: 2,
            centroid_count: 1,
        };
        let centroids = [0.5f32, 0.5];
        let a = [1.0f32, 0.0];
        let b = [0.0f32, 1.0];
        let parts = vec![IvfSqPart {
            segment_offsets: vec![3, 4],
            // 8-bit: dim (2) bytes per vector.
            codes: vec![1, 2, 3, 4],
            dot_v_e: vec![0.25, 0.5],
        }];
        let bytes = encode_ivf_sq_chunk(&header, &centroids, &a, &b, &parts, 8).unwrap();
        // 16 header + (1*2 + 2*2) centroids/a/b floats + 4 + 2*4 offsets + 4 codes + 2*4 dots
        assert_eq!(bytes.len(), 16 + 6 * 4 + 4 + 8 + 4 + 8);

        let got = decode_ivf_sq_chunk(&bytes, 8).unwrap();
        assert_eq!(got.header, header);
        assert_eq!(got.centroids, centroids.to_vec());
        assert_eq!(got.common_vec_a, a.to_vec());
        assert_eq!(got.common_vec_b, b.to_vec());
        assert_eq!(got.parts, parts);

        // 4-bit codes halve the code bytes per vector.
        assert_eq!(sq_bytes_per_vector(2, 4), 1);
        assert_eq!(sq_bytes_per_vector(3, 4), 2);
        let ok = IvfSqPart {
            segment_offsets: vec![0, 1],
            codes: vec![0x12, 0x34],
            dot_v_e: vec![0.0, 0.0],
        };
        let bytes =
            encode_ivf_sq_chunk(&header, &centroids, &a, &b, std::slice::from_ref(&ok), 4).unwrap();
        let got = decode_ivf_sq_chunk(&bytes, 4).unwrap();
        assert_eq!(got.parts, vec![ok]);
    }

    #[test]
    fn ivf_sq_chunk_rejects_shape_mismatches() {
        let header = IvfChunkHeader {
            row_count: 1,
            embedding_count: 1,
            dimension: 2,
            centroid_count: 1,
        };
        // Wrong code length for the declared embedding count.
        let bad = vec![IvfSqPart {
            segment_offsets: vec![0],
            codes: vec![1, 2, 3],
            dot_v_e: vec![0.0],
        }];
        assert!(
            encode_ivf_sq_chunk(&header, &[0.0, 0.0], &[0.0, 0.0], &[0.0, 0.0], &bad, 8).is_err()
        );
        // Wrong shared-vector length.
        assert!(encode_ivf_sq_chunk(&header, &[0.0, 0.0], &[0.0], &[0.0, 0.0], &[], 8).is_err());
        // Truncated input.
        let ok = vec![IvfSqPart {
            segment_offsets: vec![0],
            codes: vec![1, 2],
            dot_v_e: vec![0.0],
        }];
        let bytes =
            encode_ivf_sq_chunk(&header, &[0.0, 0.0], &[0.0, 0.0], &[0.0, 0.0], &ok, 8).unwrap();
        assert!(decode_ivf_sq_chunk(&bytes[..bytes.len() - 1], 8).is_err());
    }

    #[test]
    fn ivf_plain_chunk_roundtrip() {
        let header = IvfChunkHeader {
            row_count: 2,
            embedding_count: 2,
            dimension: 2,
            centroid_count: 2,
        };
        // Two centroids of dimension 2.
        let centroids = [0.0f32, 0.0, 1.0, 1.0];
        let parts = vec![
            IvfPlainPart {
                segment_offsets: vec![0],
                vectors: [1.0f32, 2.0].iter().flat_map(|v| v.to_le_bytes()).collect(),
            },
            IvfPlainPart {
                segment_offsets: vec![1],
                vectors: [3.0f32, 4.0].iter().flat_map(|v| v.to_le_bytes()).collect(),
            },
        ];
        let bytes = encode_ivf_plain_chunk(&header, &centroids, &parts).unwrap();
        // 16 (header) + 2*2*4 (centroids) + (4 + 1*4 + 8) * 2 parts.
        assert_eq!(bytes.len(), 16 + 16 + 16 * 2);
        assert_eq!(&bytes[0..4], &2u32.to_le_bytes()); // row_count
        assert_eq!(&bytes[4..8], &2u32.to_le_bytes()); // embedding_count
        assert_eq!(&bytes[8..12], &2u32.to_le_bytes()); // dimension
        assert_eq!(&bytes[12..16], &2u32.to_le_bytes()); // centroid_count
        assert_eq!(&bytes[16..20], &0.0f32.to_le_bytes()); // first centroid

        let (h, c, p) = decode_ivf_plain_chunk(&bytes, 4).unwrap();
        assert_eq!(h, header);
        assert_eq!(c, centroids.to_vec());
        assert_eq!(p, parts);
    }

    #[test]
    fn ivf_plain_chunk_rejects_bad_centroid_count() {
        let header = IvfChunkHeader {
            row_count: 0,
            embedding_count: 0,
            dimension: 2,
            centroid_count: 3,
        };
        assert!(encode_ivf_plain_chunk(&header, &[0.0f32; 4], &[]).is_err());
    }

    #[test]
    fn ivf_plain_chunk_rejects_truncation() {
        let header = IvfChunkHeader {
            row_count: 1,
            embedding_count: 1,
            dimension: 2,
            centroid_count: 1,
        };
        let parts = vec![IvfPlainPart {
            segment_offsets: vec![0],
            vectors: vec![0u8; 8],
        }];
        let bytes = encode_ivf_plain_chunk(&header, &[0.0f32, 0.0], &parts).unwrap();
        assert!(decode_ivf_plain_chunk(&bytes[..bytes.len() - 1], 4).is_err());
        assert!(decode_ivf_plain_chunk(&bytes[..8], 4).is_err());
    }

    #[test]
    fn vertex_struct_sizes_match_the_measured_cpp_layout() {
        // Measured with g++ on x86_64 SysV (see VERTEX_L0_SIZE).
        assert_eq!(VERTEX_L0_SIZE, 24);
        assert_eq!(VERTEX_LX_SIZE, 4);
        // M = 16 -> Mmax0 = 32, Mmax = 16.
        assert_eq!(level0_stride(VERTEX_L0_SIZE, 32), 152);
        assert_eq!(levelx_stride(VERTEX_LX_SIZE, 16), 68);
    }
    #[test]
    fn mmax_follows_the_cpp_formula() {
        assert_eq!(get_mmax(16), (32, 16));
        assert_eq!(get_mmax(1), (2, 1));
    }

    #[test]
    fn header_is_56_bytes_at_fixed_offsets() {
        let h = HnswHeader::plain(16, 200, 3, 4);
        let bytes = h.encode();
        assert_eq!(bytes.len(), HNSW_HEADER_LEN);
        assert_eq!(&bytes[0..8], &16u64.to_le_bytes()); // M
        assert_eq!(&bytes[8..16], &200u64.to_le_bytes()); // ef_construction
        assert_eq!(&bytes[16..24], &3u64.to_le_bytes()); // cur_vec_num
        assert_eq!(&bytes[24..32], &4u64.to_le_bytes()); // dim
        assert_eq!(&bytes[32..40], &32u64.to_le_bytes()); // Mmax0 = 2M
        assert_eq!(&bytes[40..48], &16u64.to_le_bytes()); // Mmax = M
        assert_eq!(&bytes[48..52], &0i32.to_le_bytes()); // max_layer
        assert_eq!(&bytes[52..56], &0i32.to_le_bytes()); // enterpoint

        let (got, n) = HnswHeader::decode(&bytes).unwrap();
        assert_eq!(n, HNSW_HEADER_LEN);
        assert_eq!(got, h);
    }

    #[test]
    fn ef_construction_is_raised_to_m() {
        // The C++ writer stores max(M, ef_construction).
        let h = HnswHeader::plain(32, 10, 1, 2);
        assert_eq!(h.ef_construction, 32);
        assert_eq!(h.encode().len(), HNSW_HEADER_LEN);
    }

    #[test]
    fn header_decode_rejects_short_buffer() {
        assert!(HnswHeader::decode(&[0u8; 47]).is_err());
    }

    #[test]
    fn vector_region_length() {
        let h = HnswHeader::plain(16, 200, 3, 4);
        assert_eq!(h.vector_region_len(4), 3 * 4 * 4); // 3 vectors, dim 4, f32
        assert_eq!(h.vector_region_len(1), 12);
    }

    #[test]
    fn labels_roundtrip() {
        let labels = [7u32, 0, 12345];
        let bytes = encode_labels(&labels);
        assert_eq!(bytes.len(), 12);
        assert_eq!(&bytes[0..4], &7u32.to_le_bytes());
        assert_eq!(decode_labels(&bytes).unwrap(), labels.to_vec());
        assert!(decode_labels(&bytes[..11]).is_err());
    }

    #[test]
    fn chunk_roundtrip_with_plain_store() {
        let h = HnswHeader::plain(16, 200, 2, 3);
        let vectors: Vec<u8> = (0..6).flat_map(|i| (i as f32).to_le_bytes()).collect();
        // A minimal graph region: u64 layer_sum = 0 (no upper layers), then two
        // level-0 vertex records (cur_vec_num = 2).
        let mut graph = 0u64.to_le_bytes().to_vec();
        graph.extend_from_slice(&vec![0u8; 2 * level0_stride(24, h.mmax0)]);
        let labels = [11u32, 12];
        let file = encode_hnsw_chunk(&h, &vectors, &graph, &labels);
        assert_eq!(
            file.len(),
            HNSW_HEADER_LEN + h.vector_region_len(4) + graph.len() + 8
        );

        let got = decode_hnsw_chunk(&file, 4, 24, 4).unwrap();
        assert_eq!(got.header, h);
        assert_eq!(got.vectors, vectors);
        assert_eq!(got.graph, graph);
        assert_eq!(got.labels, labels.to_vec());
    }

    #[test]
    fn chunk_decode_rejects_truncation() {
        let h = HnswHeader::plain(16, 200, 2, 3);
        let vectors = vec![0u8; 24];
        let mut graph = 0u64.to_le_bytes().to_vec();
        graph.extend_from_slice(&vec![0u8; 2 * level0_stride(24, h.mmax0)]);
        let file = encode_hnsw_chunk(&h, &vectors, &graph, &[1, 2]);
        assert!(decode_hnsw_chunk(&file[..file.len() - 1], 4, 24, 4).is_err());
        // A graph region that is too short for its layer_sum is rejected too.
        let bad_graph = 1u64.to_le_bytes().to_vec();
        let bad = encode_hnsw_chunk(&h, &vectors, &bad_graph, &[1, 2]);
        assert!(decode_hnsw_chunk(&bad, 4, 24, 4).is_err());
    }

    #[test]
    fn vertex_strides_follow_the_spec() {
        // x86_64 SysV: sizeof(VertexL0) = 24, sizeof(VertexLX) = 4.
        let h = HnswHeader::plain(16, 200, 1, 4);
        assert_eq!(level0_stride(24, h.mmax0), 24 + 4 * 32);
        assert_eq!(levelx_stride(4, h.mmax), 4 + 4 * 16);
    }

    #[test]
    fn graph_region_roundtrip() {
        let mmax0 = 4u64;
        let mmax = 2u64;
        let vertices = vec![
            HnswGraphVertex {
                layer_count: 1,
                neighbors_l0: vec![1, 2],
                upper: vec![vec![3]],
            },
            HnswGraphVertex {
                layer_count: 0,
                neighbors_l0: vec![0],
                upper: vec![],
            },
            HnswGraphVertex {
                layer_count: 2,
                neighbors_l0: vec![0, 1],
                upper: vec![vec![1], vec![]],
            },
        ];
        let bytes = encode_graph_region(&vertices, mmax0, mmax);
        let l0 = level0_stride(VERTEX_L0_SIZE, mmax0);
        let lx = levelx_stride(VERTEX_LX_SIZE, mmax);
        // u64 layer_sum + 3 level-0 records + (1 + 2) upper records.
        assert_eq!(&bytes[0..8], &3u64.to_le_bytes());
        assert_eq!(bytes.len(), 8 + 3 * l0 + 3 * lx);
        // Vertex 0's level-0 record: layer_n_ = 1, layers_p_ = 0, neighbor_n_ = 2.
        assert_eq!(i32::from_le_bytes(bytes[8..12].try_into().unwrap()), 1);
        assert_eq!(u64::from_le_bytes(bytes[16..24].try_into().unwrap()), 0);
        assert_eq!(i32::from_le_bytes(bytes[24..28].try_into().unwrap()), 2);
        assert_eq!(u32::from_le_bytes(bytes[28..32].try_into().unwrap()), 1);
        assert_eq!(u32::from_le_bytes(bytes[32..36].try_into().unwrap()), 2);
        // Vertex 1 has no upper layers; vertex 2's block starts after vertex 0's.
        let v2_at = 8 + 2 * l0;
        assert_eq!(
            u64::from_le_bytes(bytes[v2_at + 8..v2_at + 16].try_into().unwrap()),
            lx as u64
        );

        let decoded = decode_graph_region(&bytes, vertices.len(), mmax0, mmax).unwrap();
        assert_eq!(decoded, vertices);
    }

    #[test]
    fn graph_region_rejects_wrong_length() {
        let bytes = encode_graph_region(&[], 4, 2);
        assert_eq!(bytes, 0u64.to_le_bytes());
        // Declaring a vertex that is not there is a corruption.
        assert!(decode_graph_region(&bytes, 1, 4, 2).is_err());
    }

    #[test]
    fn graph_region_pads_neighbour_slots() {
        let v = vec![HnswGraphVertex {
            layer_count: 0,
            neighbors_l0: vec![7],
            upper: vec![],
        }];
        let bytes = encode_graph_region(&v, 4, 2);
        // The single neighbour is followed by three zeroed slots.
        assert_eq!(u32::from_le_bytes(bytes[28..32].try_into().unwrap()), 7);
        assert_eq!(&bytes[32..44], &[0u8; 12]);
    }
}
