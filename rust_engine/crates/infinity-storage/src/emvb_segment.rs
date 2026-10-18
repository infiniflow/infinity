//! Multi-vector segment in the reference's EMVB layout.
//!
//! Mirrors `EMVBIndex::SaveIndexInner` (`emvb_index_impl.cpp:598`):
//!
//! ```text
//! u32 magic "EMVB" (0x454D5642), u32 version 1
//! u32 start_segment_offset, u32 embedding_dimension
//! u32 residual_pq_subspace_num, u32 residual_pq_subspace_bits
//! u32 n_centroids
//! u32 count + f32[] centroids_data
//! u32 count + f32[] centroid_norms_neg_half
//! u32 n_docs, u32 n_total_embeddings
//! u32 n + u32[] doc_lens
//! u32 n + u32[] doc_segment_offsets
//! u32 n + u32[] doc_offsets
//! u32 n + u32[] centroid_id_assignments
//! per centroid: u32 count + u32[] doc ids
//! PQ: (no subspaces) u32 0 encoded embeddings, u32 0 next id,
//!     f32[dim * dim] rotation matrix R (identity)
//! ```
//!
//! The residual product quantizer is written with `subspace_num = 0`: the
//! coarse centroids and the document tables are the ones the reference reads
//! back, and the quantizer carries an identity rotation with no codes. Our
//! query path scores from the companion `.mvec` segment, which keeps the raw
//! tokens for exact `MaxSim`.

use std::fs::File;
use std::path::Path;

use memmap2::Mmap;

use crate::cpp_format::FormatError;

const EMVB_MAGIC: u32 = 0x454D_5642;
const EMVB_VERSION: u32 = 1;

/// Coarse centroids: k-means over every token of the segment.
fn coarse_centroids(dims: usize, tokens: &[&[f32]], k: usize) -> Vec<f32> {
    let k = k.max(1);
    let mut centroids = vec![0.0f32; k * dims];
    for (c, chunk) in centroids.chunks_exact_mut(dims).enumerate() {
        let idx = (c * tokens.len()) / k;
        if let Some(t) = tokens.get(idx.min(tokens.len().saturating_sub(1))) {
            let n = chunk.len().min(t.len());
            chunk[..n].copy_from_slice(&t[..n]);
        }
    }
    for _ in 0..8 {
        let mut sums = vec![0.0f32; k * dims];
        let mut counts = vec![0usize; k];
        for t in tokens {
            let mut best = 0usize;
            let mut best_d = f32::INFINITY;
            for c in 0..k {
                let base = c * dims;
                let d: f32 = (0..dims.min(t.len()))
                    .map(|i| (centroids[base + i] - t[i]).powi(2))
                    .sum();
                if d < best_d {
                    best_d = d;
                    best = c;
                }
            }
            counts[best] += 1;
            let base = best * dims;
            for i in 0..dims.min(t.len()) {
                sums[base + i] += t[i];
            }
        }
        for (c, &count) in counts.iter().enumerate() {
            if count == 0 {
                continue;
            }
            let base = c * dims;
            for i in 0..dims {
                centroids[base + i] = sums[base + i] / count as f32;
            }
        }
    }
    centroids
}

fn nearest(centroids: &[f32], dims: usize, token: &[f32]) -> u32 {
    let mut best = 0u32;
    let mut best_d = f32::INFINITY;
    for c in 0..centroids.len() / dims {
        let base = c * dims;
        let d: f32 = (0..dims.min(token.len()))
            .map(|i| (centroids[base + i] - token[i]).powi(2))
            .sum();
        if d < best_d {
            best_d = d;
            best = c as u32;
        }
    }
    best
}

fn push_u32(out: &mut Vec<u8>, v: u32) {
    out.extend_from_slice(&v.to_le_bytes());
}

fn push_u32s(out: &mut Vec<u8>, vs: &[u32]) {
    push_u32(out, vs.len() as u32);
    for v in vs {
        push_u32(out, *v);
    }
}

fn push_f32s(out: &mut Vec<u8>, vs: &[f32]) {
    push_u32(out, vs.len() as u32);
    for v in vs {
        out.extend_from_slice(&v.to_le_bytes());
    }
}

/// Encodes one multi-vector segment. `docs` are `(row offset, tokens)` in row
/// order.
pub fn encode_emvb(
    dims: usize,
    start_segment_offset: u32,
    docs: &[(u32, Vec<Vec<f32>>)],
) -> Vec<u8> {
    let tokens: Vec<&[f32]> = docs
        .iter()
        .flat_map(|(_, t)| t.iter().map(|v| v.as_slice()))
        .collect();
    let n_centroids = {
        // The reference requires a multiple of 8.
        let raw = (tokens.len() / 8).clamp(8, 256);
        (raw / 8) * 8
    };
    let centroids = coarse_centroids(dims, &tokens, n_centroids);
    let mut norms = Vec::with_capacity(n_centroids);
    for c in 0..n_centroids {
        let base = c * dims;
        let norm: f32 = (0..dims)
            .map(|i| centroids[base + i].powi(2))
            .sum::<f32>()
            .sqrt();
        norms.push(-0.5 * norm);
    }

    let mut doc_lens = Vec::with_capacity(docs.len());
    let mut doc_segment_offsets = Vec::with_capacity(docs.len());
    let mut doc_offsets = Vec::with_capacity(docs.len());
    let mut assignments: Vec<u32> = Vec::with_capacity(tokens.len());
    let mut centroids_to_docid: Vec<Vec<u32>> = vec![Vec::new(); n_centroids];
    let mut offset = 0u32;
    for (doc_id, (row, toks)) in docs.iter().enumerate() {
        doc_lens.push(toks.len() as u32);
        doc_segment_offsets.push(*row);
        doc_offsets.push(offset);
        for t in toks {
            let c = nearest(&centroids, dims, t);
            assignments.push(c);
            centroids_to_docid[c as usize].push(doc_id as u32);
            offset += 1;
        }
    }

    let mut out = Vec::new();
    push_u32(&mut out, EMVB_MAGIC);
    push_u32(&mut out, EMVB_VERSION);
    push_u32(&mut out, start_segment_offset);
    push_u32(&mut out, dims as u32);
    push_u32(&mut out, 0); // residual_pq_subspace_num
    push_u32(&mut out, 0); // residual_pq_subspace_bits
    push_u32(&mut out, n_centroids as u32);
    push_f32s(&mut out, &centroids);
    push_f32s(&mut out, &norms);
    push_u32(&mut out, docs.len() as u32);
    push_u32(&mut out, tokens.len() as u32);
    push_u32s(&mut out, &doc_lens);
    push_u32s(&mut out, &doc_segment_offsets);
    push_u32s(&mut out, &doc_offsets);
    push_u32s(&mut out, &assignments);
    for list in &centroids_to_docid {
        push_u32s(&mut out, list);
    }
    // Empty residual PQ + identity rotation.
    push_u32(&mut out, 0);
    push_u32(&mut out, 0);
    for i in 0..dims {
        for j in 0..dims {
            let v = if i == j { 1.0f32 } else { 0.0 };
            out.extend_from_slice(&v.to_le_bytes());
        }
    }
    out
}

/// Writes the segment to `path`.
pub fn write_emvb_segment(
    path: &Path,
    dims: usize,
    start_segment_offset: u32,
    docs: &[(u32, Vec<Vec<f32>>)],
) -> Result<(), FormatError> {
    let bytes = encode_emvb(dims, start_segment_offset, docs);
    if let Some(parent) = path.parent() {
        std::fs::create_dir_all(parent)
            .map_err(|e| FormatError::Io(format!("{}: {e}", parent.display())))?;
    }
    std::fs::write(path, bytes).map_err(|e| FormatError::Io(format!("{}: {e}", path.display())))
}

/// Header of an EMVB segment, enough to validate the layout.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct EmvbHeader {
    pub start_segment_offset: u32,
    pub dims: u32,
    pub subspace_num: u32,
    pub subspace_bits: u32,
    pub n_centroids: u32,
    pub n_docs: u32,
    pub n_embeddings: u32,
}

/// A segment mapped read-only.
pub struct EmvbReader {
    map: Mmap,
    header: EmvbHeader,
}

impl EmvbReader {
    #[allow(unsafe_code)]
    pub fn open(path: &Path) -> Result<Self, FormatError> {
        let file =
            File::open(path).map_err(|e| FormatError::Io(format!("{}: {e}", path.display())))?;
        // SAFETY: read-only mapping of an immutable chunk file.
        let map = unsafe { Mmap::map(&file) }.map_err(|e| FormatError::Io(e.to_string()))?;
        if map.len() < 16 {
            return Err(FormatError::Truncated {
                need: 16,
                got: map.len(),
            });
        }
        let u32_at = |o: usize| u32::from_le_bytes(map[o..o + 4].try_into().unwrap());
        if u32_at(0) != EMVB_MAGIC {
            return Err(FormatError::BadMagic(u32_at(0) as u64));
        }
        if u32_at(4) != EMVB_VERSION {
            return Err(FormatError::BadMagic(u32_at(4) as u64));
        }
        // Walk to the document counts past the centroid tables.
        let mut at = 28;
        let centroids = u32_at(at) as usize;
        at += 4 + centroids * 4;
        let norms = u32_at(at) as usize;
        at += 4 + norms * 4;
        let n_docs = u32_at(at);
        let n_embeddings = u32_at(at + 4);
        Ok(Self {
            header: EmvbHeader {
                start_segment_offset: u32_at(8),
                dims: u32_at(12),
                subspace_num: u32_at(16),
                subspace_bits: u32_at(20),
                n_centroids: u32_at(24),
                n_docs,
                n_embeddings,
            },
            map,
        })
    }

    pub fn header(&self) -> EmvbHeader {
        self.header
    }

    pub fn len(&self) -> usize {
        self.map.len()
    }

    pub fn is_empty(&self) -> bool {
        self.map.is_empty()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn docs() -> Vec<(u32, Vec<Vec<f32>>)> {
        (0..40u32)
            .map(|row| {
                let toks = (0..3)
                    .map(|t| {
                        (0..8)
                            .map(|i| ((row + t + i as u32) % 7) as f32 / 7.0)
                            .collect()
                    })
                    .collect();
                (row, toks)
            })
            .collect()
    }

    #[test]
    fn header_roundtrips() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("seg_0.emvb");
        let d = docs();
        write_emvb_segment(&path, 8, 0, &d).unwrap();
        let reader = EmvbReader::open(&path).unwrap();
        let h = reader.header();
        assert_eq!(h.dims, 8);
        assert_eq!(h.n_docs, 40);
        assert_eq!(h.n_embeddings, 120);
        assert_eq!(h.subspace_num, 0);
        assert_eq!(h.n_centroids % 8, 0);
        assert!(reader.len() > 16);
    }
}
