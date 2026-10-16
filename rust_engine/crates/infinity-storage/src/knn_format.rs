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

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn mmax_follows_the_cpp_formula() {
        assert_eq!(get_mmax(16), (32, 16));
        assert_eq!(get_mmax(1), (2, 1));
    }

    #[test]
    fn header_is_48_bytes_at_fixed_offsets() {
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
}
