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

#[cfg(test)]
mod tests {
    use super::*;

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
