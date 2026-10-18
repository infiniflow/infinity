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

/// Residual product quantizer (`PQ::Save` / `OPQ::Save`): per-subspace
/// centroids over `token - coarse centroid`, their `-0.5 * norm` table and one
/// code per embedding.
struct ResidualPq {
    subspace_num: usize,
    bits: u32,
    centroids: Vec<f32>,
    norms: Vec<f32>,
    codes: Vec<Vec<u8>>,
}

fn train_residual_pq(
    dims: usize,
    subspace_num: usize,
    bits: u32,
    residuals: &[Vec<f32>],
) -> ResidualPq {
    let subspace_num = subspace_num.max(1);
    let width = dims.div_ceil(subspace_num);
    let k = 1usize << bits.min(8);
    let mut centroids = Vec::with_capacity(subspace_num * k * width);
    let mut norms = Vec::with_capacity(subspace_num * k);
    // Sample so training stays bounded on large segments.
    let sample: Vec<&Vec<f32>> = residuals
        .iter()
        .step_by((residuals.len() / 4096).max(1))
        .collect();
    for sub in 0..subspace_num {
        let lo = sub * width;
        let hi = (lo + width).min(dims);
        let sub_dim = hi - lo;
        let mut cs = vec![0.0f32; k * sub_dim];
        for c in 0..k {
            let t = sample.get((c * sample.len()) / k.max(1)).copied();
            if let Some(t) = t {
                let n = sub_dim.min(t.len().saturating_sub(lo));
                cs[c * sub_dim..c * sub_dim + n].copy_from_slice(&t[lo..lo + n]);
            }
        }
        for _ in 0..4 {
            let mut sums = vec![0.0f32; k * sub_dim];
            let mut counts = vec![0usize; k];
            for t in &sample {
                if lo >= t.len() {
                    continue;
                }
                let v = &t[lo..hi.min(t.len())];
                let mut best = 0usize;
                let mut best_d = f32::INFINITY;
                for c in 0..k {
                    let d: f32 = cs[c * sub_dim..c * sub_dim + v.len()]
                        .iter()
                        .zip(v)
                        .map(|(a, b)| (a - b) * (a - b))
                        .sum();
                    if d < best_d {
                        best_d = d;
                        best = c;
                    }
                }
                counts[best] += 1;
                for (acc, x) in sums[best * sub_dim..best * sub_dim + v.len()]
                    .iter_mut()
                    .zip(v)
                {
                    *acc += x;
                }
            }
            for (c, &count) in counts.iter().enumerate() {
                if count == 0 {
                    continue;
                }
                for i in 0..sub_dim {
                    cs[c * sub_dim + i] = sums[c * sub_dim + i] / count as f32;
                }
            }
        }
        for c in 0..k {
            let norm: f32 = cs[c * sub_dim..(c + 1) * sub_dim]
                .iter()
                .map(|x| x * x)
                .sum::<f32>()
                .sqrt();
            norms.push(-0.5 * norm);
        }
        centroids.extend_from_slice(&cs);
    }
    // Encode every embedding.
    let mut codes = Vec::with_capacity(residuals.len());
    for r in residuals {
        let mut code = Vec::with_capacity(subspace_num);
        for sub in 0..subspace_num {
            let lo = sub * width;
            let hi = (lo + width).min(dims);
            let sub_dim = hi - lo;
            let v = if lo < r.len() {
                &r[lo..hi.min(r.len())]
            } else {
                &[][..]
            };
            let mut best = 0u8;
            let mut best_d = f32::INFINITY;
            for c in 0..k {
                let base = (sub * k + c) * sub_dim;
                let d: f32 = centroids[base..base + sub_dim]
                    .iter()
                    .zip(v)
                    .map(|(a, b)| (a - b) * (a - b))
                    .sum();
                if d < best_d {
                    best_d = d;
                    best = c as u8;
                }
            }
            code.push(best);
        }
        codes.push(code);
    }
    ResidualPq {
        subspace_num,
        bits,
        centroids,
        norms,
        codes,
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

    // Residuals feed the product quantizer: one vector per embedding.
    let mut residuals: Vec<Vec<f32>> = Vec::with_capacity(tokens.len());
    for (i, t) in tokens.iter().enumerate() {
        let c = assignments[i] as usize;
        let base = c * dims;
        residuals.push(
            (0..dims)
                .map(|j| t.get(j).copied().unwrap_or(0.0) - centroids[base + j])
                .collect(),
        );
    }
    let subspace_num = if dims.is_multiple_of(8) { 8 } else { 1 };
    let pq = train_residual_pq(dims, subspace_num, 8, &residuals);

    let mut out = Vec::new();
    push_u32(&mut out, EMVB_MAGIC);
    push_u32(&mut out, EMVB_VERSION);
    push_u32(&mut out, start_segment_offset);
    push_u32(&mut out, dims as u32);
    push_u32(&mut out, pq.subspace_num as u32);
    push_u32(&mut out, pq.bits);
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
    // Residual PQ (`PQ::Save` / `OPQ::Save`): per-subspace centroids, their
    // -0.5*norm table, the codes, and the OPQ rotation (identity here).
    let sub_dim = dims.div_ceil(pq.subspace_num);
    for sub in 0..pq.subspace_num {
        let k = 1usize << pq.bits.min(8);
        let base = sub * k * sub_dim;
        for v in &pq.centroids[base..base + k * sub_dim] {
            out.extend_from_slice(&v.to_le_bytes());
        }
    }
    for v in &pq.norms {
        out.extend_from_slice(&v.to_le_bytes());
    }
    push_u32(&mut out, pq.codes.len() as u32);
    for code in &pq.codes {
        out.extend_from_slice(code);
    }
    push_u32(&mut out, pq.codes.len() as u32); // next_embedding_id
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

/// The persisted residual product quantizer of one EMVB segment (plan #82).
/// C++ reference: `EMVBIndex::SaveIndexInner` + `PQ::Save`.
pub struct EmvbPqIndex {
    pub dims: usize,
    pub subspace_num: usize,
    pub sub_dim: usize,
    pub k: usize,
    pub centroids: Vec<f32>,
    pub codebook: Vec<f32>,
    pub codes: Vec<u8>,
    pub assignments: Vec<u32>,
    pub doc_lens: Vec<u32>,
    pub doc_segment_offsets: Vec<u32>,
}

impl EmvbPqIndex {
    /// Approximate MaxSim ranking from the coarse centroid plus residual code
    /// (`EMVB` stage 2). Returns **document indices** in descending score.
    pub fn prefilter(&self, query: &[Vec<f32>], k: usize) -> Vec<usize> {
        let n_docs = self.doc_lens.len();
        if n_docs == 0 || k == 0 || self.dims == 0 {
            return Vec::new();
        }
        let dims = self.dims;
        let sn = self.subspace_num;
        if self.centroids.len() < dims || self.codebook.len() < sn * self.k * self.sub_dim {
            return Vec::new();
        }
        let n_centroids = self.centroids.len() / dims;
        let mut offset = 0usize;
        let mut scored: Vec<(usize, f32)> = Vec::with_capacity(n_docs);
        for doc in 0..n_docs {
            let len = self.doc_lens[doc] as usize;
            let mut total = 0.0f32;
            for q in query {
                if q.len() != dims {
                    continue;
                }
                let mut best = f32::NEG_INFINITY;
                for e in offset..offset + len {
                    let Some(&centroid) = self.assignments.get(e) else {
                        continue;
                    };
                    let cbase = centroid as usize * dims;
                    if centroid as usize >= n_centroids {
                        continue;
                    }
                    let mut score: f32 = q
                        .iter()
                        .zip(&self.centroids[cbase..cbase + dims])
                        .map(|(a, b)| a * b)
                        .sum();
                    for sub in 0..sn {
                        let Some(&code) = self.codes.get(e * sn + sub) else {
                            continue;
                        };
                        let lo = sub * self.sub_dim;
                        let hi = (lo + self.sub_dim).min(dims);
                        if lo >= dims {
                            break;
                        }
                        let cbase = (sub * self.k + code as usize) * self.sub_dim;
                        score += q[lo..hi]
                            .iter()
                            .zip(&self.codebook[cbase..cbase + (hi - lo)])
                            .map(|(a, b)| a * b)
                            .sum::<f32>();
                    }
                    if score > best {
                        best = score;
                    }
                }
                if best != f32::NEG_INFINITY {
                    total += best;
                }
            }
            scored.push((doc, total));
            offset += len;
        }
        scored.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        scored.truncate(k);
        scored.into_iter().map(|(doc, _)| doc).collect()
    }
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

    /// Parses the residual PQ tables (`PQ::Save`) that follow the document and
    /// centroid tables. `None` when the file is truncated or carries no codes.
    pub fn pq_index(&self) -> Option<EmvbPqIndex> {
        let m = &self.map;
        let u32_at = |o: usize| -> Option<u32> {
            m.get(o..o + 4)
                .map(|b| u32::from_le_bytes(b.try_into().unwrap()))
        };
        let mut at = 28usize;
        // Coarse centroids (floats) and their -0.5*norm table.
        let clen = u32_at(at)? as usize;
        at += 4;
        if m.len() < at + clen * 4 {
            return None;
        }
        let centroids: Vec<f32> = (0..clen)
            .map(|i| f32::from_le_bytes(m[at + i * 4..at + i * 4 + 4].try_into().unwrap()))
            .collect();
        at += clen * 4;
        let nlen = u32_at(at)? as usize;
        at += 4 + nlen * 4;
        // n_docs / n_embeddings.
        at += 8;
        let read_u32s = |o: usize| -> Option<(Vec<u32>, usize)> {
            let n = u32_at(o)? as usize;
            if m.len() < o + 4 + n * 4 {
                return None;
            }
            let values = (0..n)
                .map(|i| u32::from_le_bytes(m[o + 4 + i * 4..o + 8 + i * 4].try_into().unwrap()))
                .collect();
            Some((values, o + 4 + n * 4))
        };
        let (doc_lens, next) = read_u32s(at)?;
        at = next;
        let (doc_segment_offsets, next) = read_u32s(at)?;
        at = next;
        let (_doc_offsets, next) = read_u32s(at)?;
        at = next;
        let (assignments, next) = read_u32s(at)?;
        at = next;
        for _ in 0..self.header.n_centroids {
            let (_, next) = read_u32s(at)?;
            at = next;
        }
        let sn = self.header.subspace_num as usize;
        let dims = self.header.dims as usize;
        if sn == 0 || dims == 0 {
            return None;
        }
        let k = 1usize << self.header.subspace_bits.min(8);
        let sub_dim = dims.div_ceil(sn);
        let cb_len = sn * k * sub_dim;
        if m.len() < at + cb_len * 4 {
            return None;
        }
        let codebook: Vec<f32> = (0..cb_len)
            .map(|i| f32::from_le_bytes(m[at + i * 4..at + i * 4 + 4].try_into().unwrap()))
            .collect();
        at += cb_len * 4;
        // Residual-code norms: one per code word.
        at += sn * k * 4;
        // The prefix counts embeddings; each code is `subspace_num` bytes.
        let code_len = u32_at(at)? as usize;
        at += 4;
        let code_bytes = code_len * sn;
        if m.len() < at + code_bytes {
            return None;
        }
        Some(EmvbPqIndex {
            dims,
            subspace_num: sn,
            sub_dim,
            k,
            centroids,
            codebook,
            codes: m[at..at + code_bytes].to_vec(),
            assignments,
            doc_lens,
            doc_segment_offsets,
        })
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
        assert_eq!(h.subspace_num, 8);
        assert_eq!(h.subspace_bits, 8);
        assert_eq!(h.n_centroids % 8, 0);
        assert!(reader.len() > 16);
        // The residual PQ tables parse and rank the owning documents.
        let pq = reader.pq_index().expect("residual PQ tables");
        assert_eq!(pq.dims, 8);
        assert_eq!(pq.subspace_num, 8);
        assert_eq!(pq.doc_lens.len(), 40);
        assert_eq!(pq.codes.len(), 120 * 8);
        let ranked = pq.prefilter(&[vec![0.0f32; 8]], 5);
        assert_eq!(ranked.len(), 5);
        assert!(ranked.iter().all(|d| *d < 40));
    }
}
