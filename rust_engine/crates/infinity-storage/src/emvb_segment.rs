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
//!     f32[dim * dim] OPQ rotation matrix R
//! ```
//!
//! The residual product quantizer is written the way `OPQ::Save` writes it:
//! per-subspace centroids, the `-0.5*||c||` table, one code per embedding and
//! the trailing `matrix_R_` block. `R` is trained exactly like
//! `OPQ::Train` (`product_quantizer_impl.cpp:150`, six rounds of
//! k-means + two-sided Jacobi SVD, see
//! `infinity_index::emvb::OpqRotation`), the residuals are rotated before they
//! are quantised, and the query is rotated the same way at search time.
//! `EMVBIndex` always builds an OPQ (`emvb_index_impl.cpp:62`), never a plain
//! PQ, so an identity rotation here would make the chunk unscorable.

use std::fs::File;
use std::path::Path;

use memmap2::Mmap;

use infinity_index::emvb::OpqRotation;

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
/// centroids over `(token - coarse centroid)·R`, their `-0.5 * norm` table and
/// one code per embedding. `R` is the trained `matrix_R_`; the encoded data
/// only makes sense in the rotated space.
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
    rotation: &OpqRotation,
) -> ResidualPq {
    let subspace_num = subspace_num.max(1);
    let width = dims.div_ceil(subspace_num);
    let k = 1usize << bits.min(8);
    // `OPQ::AddEmbeddings` / `OPQ::Train`: quantize `x·R`, not `x`.
    let residuals: Vec<Vec<f32>> = residuals.iter().map(|r| rotation.rotate(r)).collect();
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
    let refs: Vec<(u32, &[Vec<f32>])> = docs
        .iter()
        .map(|(row, tokens)| (*row, tokens.as_slice()))
        .collect();
    encode_emvb_refs(dims, start_segment_offset, &refs)
}

/// The EMVB arrays of one segment, before serialization.
pub struct EmvbData {
    pub start_segment_offset: u32,
    pub dims: usize,
    pub subspace_num: usize,
    pub subspace_bits: u32,
    pub n_centroids: usize,
    pub centroids: Vec<f32>,
    /// `-0.5 * ||centroid||`, the reference's `centroid_norms_neg_half_`.
    pub centroid_norms: Vec<f32>,
    /// Residual-PQ codeword norms, `subspace_num * k`.
    pub pq_norms: Vec<f32>,
    pub doc_lens: Vec<u32>,
    pub doc_segment_offsets: Vec<u32>,
    pub doc_offsets: Vec<u32>,
    pub assignments: Vec<u32>,
    pub centroids_to_docid: Vec<Vec<u32>>,
    pub codebook: Vec<f32>,
    pub codes: Vec<u8>,
    /// OPQ `matrix_R_`, `dims * dims` row-major (`OPQ::Save`'s last block).
    pub rotation: Vec<f32>,
}

impl EmvbData {
    /// A borrowed view for `EMVBIndex::GetQueryResult`.
    pub fn view(&self) -> infinity_index::emvb::EmvbView<'_> {
        infinity_index::emvb::EmvbView {
            dims: self.dims,
            subspace_num: self.subspace_num,
            sub_dim: self.dims.div_ceil(self.subspace_num.max(1)),
            k: 1usize << self.subspace_bits.min(8),
            centroids: &self.centroids,
            codebook: &self.codebook,
            codes: &self.codes,
            assignments: &self.assignments,
            doc_lens: &self.doc_lens,
            doc_segment_offsets: &self.doc_segment_offsets,
            centroids_to_docid: &self.centroids_to_docid,
            rotation: Some(&self.rotation),
        }
    }

    /// `EMVBIndex::SaveIndexInner` + `PQ::Save`.
    pub fn encode(&self) -> Vec<u8> {
        let mut out = Vec::new();
        push_u32(&mut out, EMVB_MAGIC);
        push_u32(&mut out, EMVB_VERSION);
        push_u32(&mut out, self.start_segment_offset);
        push_u32(&mut out, self.dims as u32);
        push_u32(&mut out, self.subspace_num as u32);
        push_u32(&mut out, self.subspace_bits);
        push_u32(&mut out, self.n_centroids as u32);
        push_f32s(&mut out, &self.centroids);
        push_f32s(&mut out, &self.centroid_norms);
        push_u32(&mut out, self.doc_lens.len() as u32);
        // One code per embedding per subspace.
        push_u32(
            &mut out,
            (self.codes.len() / self.subspace_num.max(1)) as u32,
        );
        push_u32s(&mut out, &self.doc_lens);
        push_u32s(&mut out, &self.doc_segment_offsets);
        push_u32s(&mut out, &self.doc_offsets);
        push_u32s(&mut out, &self.assignments);
        for list in &self.centroids_to_docid {
            push_u32s(&mut out, list);
        }
        // Residual PQ (`PQ::Save` / `OPQ::Save`): per-subspace centroids, their
        // -0.5*norm table, the codes, `next_embedding_id` and the OPQ rotation.
        let sub_dim = self.dims.div_ceil(self.subspace_num.max(1));
        let k = 1usize << self.subspace_bits.min(8);
        for sub in 0..self.subspace_num {
            let base = sub * k * sub_dim;
            for v in &self.codebook[base..base + k * sub_dim] {
                out.extend_from_slice(&v.to_le_bytes());
            }
        }
        for v in &self.pq_norms {
            out.extend_from_slice(&v.to_le_bytes());
        }
        let n_embeddings = self.codes.len() / self.subspace_num.max(1);
        push_u32(&mut out, n_embeddings as u32);
        out.extend_from_slice(&self.codes);
        push_u32(&mut out, n_embeddings as u32); // next_embedding_id
                                                 // `OPQ::Save`: `matrix_R_`, `dim * dim` f32s.
        for v in &self.rotation {
            out.extend_from_slice(&v.to_le_bytes());
        }
        out
    }
}

/// Builds the EMVB arrays for one segment (`EMVBIndex::Train` + `AddOneDocEmbeddings`).
pub fn build_emvb(dims: usize, start_segment_offset: u32, docs: &[(u32, &[Vec<f32>])]) -> EmvbData {
    // The reference has no defaults (`CREATE INDEX … USING EMVB` must name
    // both), so the inference below only serves callers without an index
    // definition.
    let subspace_num = if dims.is_multiple_of(8) { 8 } else { 1 };
    build_emvb_with(dims, start_segment_offset, docs, subspace_num, 8)
}

/// Like [`build_emvb`] but with the index's `pq_subspace_num` /
/// `pq_subspace_bits`.
pub fn build_emvb_with(
    dims: usize,
    start_segment_offset: u32,
    docs: &[(u32, &[Vec<f32>])],
    subspace_num: usize,
    subspace_bits: u32,
) -> EmvbData {
    let tokens: Vec<&[f32]> = docs
        .iter()
        .flat_map(|(_, t)| t.iter().map(|v| v.as_slice()))
        .collect();
    // `EMVBIndex::BuildEMVBIndex`: `min(sqrt(n), n / 32)` rounded **up** to a
    // multiple of 8 (`emvb_index_impl.cpp:152-157`).
    let n_centroids = {
        let sqrt = (tokens.len() as f64).sqrt() as usize;
        let before_8 = sqrt.min(tokens.len() / 32);
        // The reference rejects a training set this small
        // (`ExpectLeastTrainingDataNum`), so the floor only keeps the port's
        // own small-data tests working.
        ((before_8 + 7) & !7).max(8)
    };
    let centroids = coarse_centroids(dims, &tokens, n_centroids);
    let mut centroid_norms = Vec::with_capacity(n_centroids);
    for c in 0..n_centroids {
        let base = c * dims;
        let norm: f32 = (0..dims)
            .map(|i| centroids[base + i].powi(2))
            .sum::<f32>()
            .sqrt();
        centroid_norms.push(-0.5 * norm);
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
        for t in *toks {
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
    // `EMVBIndex::Train` ends with `product_quantizer_->Train(residuals, n, 20)`,
    // i.e. `OPQ::Train` with 20 iterations.
    let residual_refs: Vec<&[f32]> = residuals.iter().map(|v| v.as_slice()).collect();
    let rotation = OpqRotation::train(
        dims,
        subspace_num,
        1usize << subspace_bits.min(16),
        &residual_refs,
        20,
    );
    let pq = train_residual_pq(dims, subspace_num, subspace_bits, &residuals, &rotation);

    // The codebook layout is `subspace_num * k * sub_dim`.
    let sub_dim = dims.div_ceil(pq.subspace_num);
    let k = 1usize << pq.bits.min(8);
    let mut codebook = Vec::with_capacity(pq.subspace_num * k * sub_dim);
    for sub in 0..pq.subspace_num {
        let base = sub * k * sub_dim;
        codebook.extend_from_slice(&pq.centroids[base..base + k * sub_dim]);
    }

    EmvbData {
        start_segment_offset,
        dims,
        subspace_num: pq.subspace_num,
        subspace_bits: pq.bits,
        n_centroids,
        centroids,
        centroid_norms,
        pq_norms: pq.norms.clone(),
        doc_lens,
        doc_segment_offsets,
        doc_offsets,
        assignments,
        centroids_to_docid,
        codebook,
        codes: pq.codes.into_iter().flatten().collect(),
        rotation: rotation.matrix().to_vec(),
    }
}

/// Like [`encode_emvb`] but borrowing the token rows.
pub fn encode_emvb_refs(
    dims: usize,
    start_segment_offset: u32,
    docs: &[(u32, &[Vec<f32>])],
) -> Vec<u8> {
    build_emvb(dims, start_segment_offset, docs).encode()
}

/// Writes the segment to `path`.
pub fn write_emvb_segment(
    path: &Path,
    dims: usize,
    start_segment_offset: u32,
    docs: &[(u32, Vec<Vec<f32>>)],
) -> Result<(), FormatError> {
    let bytes = encode_emvb(dims, start_segment_offset, docs);
    write_bytes(path, bytes)
}

/// Like [`write_emvb_segment`] but with borrowed token rows.
pub fn write_emvb_segment_refs(
    path: &Path,
    dims: usize,
    start_segment_offset: u32,
    docs: &[(u32, &[Vec<f32>])],
) -> Result<(), FormatError> {
    let bytes = encode_emvb_refs(dims, start_segment_offset, docs);
    write_bytes(path, bytes)
}

fn write_bytes(path: &Path, bytes: Vec<u8>) -> Result<(), FormatError> {
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
    /// Documents per coarse centroid (the reference's `centroids_to_docid_`).
    pub centroids_to_docid: Vec<Vec<u32>>,
    /// OPQ `matrix_R_` (`dims * dims`). Both a chunk written here and one
    /// written by the reference carry it as `OPQ::Save`'s last block, so the
    /// residual codes stay scorable across implementations.
    pub rotation: Vec<f32>,
}

impl EmvbPqIndex {
    /// Approximate MaxSim ranking through the reference's three search stages
    /// (`EMVBIndex::GetQueryResult`). Returns **document indices** best-first.
    pub fn prefilter(&self, query: &[Vec<f32>], k: usize) -> Vec<usize> {
        if self.doc_lens.is_empty() || k == 0 || self.dims == 0 {
            return Vec::new();
        }
        if self.centroids.len() < self.dims
            || self.codebook.len() < self.subspace_num * self.k * self.sub_dim
        {
            return Vec::new();
        }
        let view = infinity_index::emvb::EmvbView {
            dims: self.dims,
            subspace_num: self.subspace_num,
            sub_dim: self.sub_dim,
            k: self.k,
            centroids: &self.centroids,
            codebook: &self.codebook,
            codes: &self.codes,
            assignments: &self.assignments,
            doc_lens: &self.doc_lens,
            doc_segment_offsets: &self.doc_segment_offsets,
            centroids_to_docid: &self.centroids_to_docid,
            rotation: Some(&self.rotation),
        };
        let mut hits = view.search(
            query,
            k,
            infinity_index::emvb::EmvbSearchOptions::for_topk(k),
        );
        hits.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        // The view returns segment offsets; callers index by dense document id.
        if self
            .doc_segment_offsets
            .iter()
            .enumerate()
            .all(|(i, &off)| off == i as u32)
        {
            return hits.into_iter().map(|(doc, _)| doc as usize).collect();
        }
        let dense: std::collections::HashMap<u32, usize> = self
            .doc_segment_offsets
            .iter()
            .enumerate()
            .map(|(i, &off)| (off, i))
            .collect();
        hits.into_iter()
            .filter_map(|(offset, _)| dense.get(&offset).copied())
            .collect()
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
        let codes = m[at..at + code_bytes].to_vec();
        at += code_bytes;
        // `next_embedding_id`, then `OPQ::Save`'s `matrix_R_` block.
        if m.len() < at + 4 {
            return None;
        }
        at += 4;
        let rotation_len = dims * dims;
        if m.len() < at + rotation_len * 4 {
            return None;
        }
        let rotation: Vec<f32> = (0..rotation_len)
            .map(|i| f32::from_le_bytes(m[at + i * 4..at + i * 4 + 4].try_into().unwrap()))
            .collect();
        // `centroids_to_docid_`: one document list per coarse centroid.
        let n_centroids = self.header.n_centroids as usize;
        let mut centroids_to_docid: Vec<Vec<u32>> = vec![Vec::new(); n_centroids];
        let mut offset = 0u32;
        for (doc, &len) in doc_lens.iter().enumerate() {
            for e in offset..offset + len {
                if let Some(&c) = assignments.get(e as usize) {
                    if let Some(list) = centroids_to_docid.get_mut(c as usize) {
                        list.push(doc as u32);
                    }
                }
            }
            offset += len;
        }
        Some(EmvbPqIndex {
            dims,
            subspace_num: sn,
            sub_dim,
            k,
            centroids,
            codebook,
            codes,
            assignments,
            doc_lens,
            doc_segment_offsets,
            centroids_to_docid,
            rotation,
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

    /// The three-stage search must keep the exact MaxSim winner inside its
    /// candidate set (the executor re-scores exactly afterwards).
    #[test]
    fn prefilter_keeps_the_exact_maxsim_winner() {
        // 300 docs in four well-separated clusters, three tokens each.
        let dims = 8usize;
        let mut docs: Vec<(u32, Vec<Vec<f32>>)> = Vec::new();
        let mut state = 7u64;
        let mut next = || {
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            state
        };
        for row in 0..300u32 {
            let cluster = (row % 4) as usize;
            let toks = (0..3)
                .map(|_| {
                    let mut v: Vec<f32> =
                        (0..dims).map(|_| ((next() % 100) as f32) / 500.0).collect();
                    v[cluster] += 1.0;
                    v
                })
                .collect();
            docs.push((row, toks));
        }
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("seg_0.emvb");
        write_emvb_segment(&path, dims, 0, &docs).unwrap();
        let reader = EmvbReader::open(&path).unwrap();
        let pq = reader.pq_index().expect("residual PQ tables");

        let mut query = vec![0.0f32; dims];
        query[1] = 1.0; // cluster 1
        let exact = docs
            .iter()
            .map(|(row, toks)| {
                let score: f32 = toks
                    .iter()
                    .map(|t| t.iter().zip(&query).map(|(a, b)| a * b).sum::<f32>())
                    .max_by(|a, b| a.partial_cmp(b).unwrap())
                    .unwrap()
                    .max(0.0);
                (*row as usize, score)
            })
            .max_by(|a, b| a.1.partial_cmp(&b.1).unwrap())
            .unwrap()
            .0;

        let candidates = pq.prefilter(&[query], 64);
        // The candidate set is the documents of the probed centroids, so it can
        // be smaller than `k`; what matters is that it contains the winner.
        assert!(!candidates.is_empty());
        assert!(
            candidates.contains(&exact),
            "exact winner {exact} missing from {:?}",
            &candidates[..candidates.len().min(8)]
        );
    }

    /// Residuals with a decaying spectrum in the coordinate basis plus a strong
    /// cross-dimension component, so the coordinate subspaces cut across the
    /// principal directions and OPQ has something to rotate.
    fn correlated_docs(n: u32, dims: usize) -> Vec<(u32, Vec<Vec<f32>>)> {
        let mut state = 0x2545_F491_4F6C_DD1Du64;
        let mut rng = || {
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            ((state >> 40) as f32) / ((1u64 << 24) as f32) - 0.5
        };
        (0..n)
            .map(|row| {
                let toks = (0..3)
                    .map(|_| {
                        let z: Vec<f32> = (0..dims).map(|_| rng()).collect();
                        (0..dims)
                            .map(|k| {
                                let scale = 4.0 / (1.0 + k as f32);
                                scale * z[k] + 0.6 * z[(k + 1) % dims]
                            })
                            .collect()
                    })
                    .collect();
                (row, toks)
            })
            .collect()
    }

    /// The chunk carries the trained `matrix_R_` (`OPQ::Save`) and reading it
    /// back yields the same matrix, both from `EmvbPqIndex` and in the view the
    /// search uses.
    #[test]
    fn chunk_persists_the_trained_opq_rotation() {
        let dims = 32usize;
        let docs = correlated_docs(120, dims);
        let refs: Vec<(u32, &[Vec<f32>])> = docs
            .iter()
            .map(|(row, toks)| (*row, toks.as_slice()))
            .collect();
        let data = build_emvb(dims, 0, &refs);
        assert_eq!(data.rotation.len(), dims * dims);
        let identity = OpqRotation::identity(dims);
        assert_ne!(
            data.rotation,
            identity.matrix(),
            "the encoder must train OPQ, not ship the identity rotation"
        );

        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("seg_0.emvb");
        std::fs::write(&path, data.encode()).unwrap();
        let reader = EmvbReader::open(&path).unwrap();
        let pq = reader.pq_index().expect("residual PQ tables");
        assert_eq!(pq.rotation.len(), dims * dims);
        assert_eq!(pq.rotation, data.rotation, "matrix_R_ did not round-trip");

        // The same rotation reaches the search view.
        let query = vec![docs[5].1[1].clone()];
        let in_memory = data.view().search(
            &query,
            3,
            infinity_index::emvb::EmvbSearchOptions::for_topk(3),
        );
        let reloaded = pq.prefilter(&query, 3);
        assert_eq!(in_memory[0].0 as usize, reloaded[0]);
    }

    /// Encoding rotates the residual (and the query does too): dropping `R`
    /// from the same chunk degrades the matching document's score, which is the
    /// audit's failure mode for a chunk whose rotation is ignored.
    #[test]
    fn opq_rotation_is_applied_to_the_query() {
        let dims = 32usize;
        let docs = correlated_docs(120, dims);
        let refs: Vec<(u32, &[Vec<f32>])> = docs
            .iter()
            .map(|(row, toks)| (*row, toks.as_slice()))
            .collect();
        let mut data = build_emvb(dims, 0, &refs);
        let query = vec![docs[7].1[0].clone()];
        // Widen the pre-filters so the residual stage sees every document; the
        // rotation only affects that stage.
        let options = infinity_index::emvb::EmvbSearchOptions {
            centroid_nprobe: 8,
            threshold_first: -0.1,
            n_doc_to_score: 10_000,
            out_second_stage: 10_000,
            threshold_final: 0.0,
        };
        let n = docs.len();
        let with_rotation = data.view().search(&query, n, options);
        data.rotation = OpqRotation::identity(dims).matrix().to_vec();
        let without_rotation = data.view().search(&query, n, options);
        // The exact MaxSim winner over the raw tokens.
        let exact = docs
            .iter()
            .map(|(_, toks)| {
                toks.iter()
                    .map(|t| t.iter().zip(&query[0]).map(|(a, b)| a * b).sum::<f32>())
                    .fold(f32::NEG_INFINITY, f32::max)
            })
            .enumerate()
            .max_by(|a, b| a.1.partial_cmp(&b.1).unwrap())
            .unwrap()
            .0;
        assert_eq!(
            with_rotation[0].0 as usize, exact,
            "the rotated chunk should rank the exact MaxSim winner first"
        );
        let score_of = |hits: &[(u32, f32)], doc: usize| {
            hits.iter()
                .find(|(d, _)| *d as usize == doc)
                .map(|(_, s)| *s)
                .unwrap_or(f32::NEG_INFINITY)
        };
        // Losing the rotation must not change *which* document wins: the codes
        // are quantised in the rotated space, so the answer has to survive the
        // query being read the same way. The chunk's total is dominated by the
        // centroid MaxSim, though, so the score itself only moves by a fraction
        // of a percent and may move in either direction - the residual term the
        // rotation actually drives is pinned exactly, with a fixture where it
        // dominates, by `opq_rotation_is_applied_to_the_residual_table` in
        // `infinity-index`.
        assert_eq!(
            without_rotation[0].0, exact as u32,
            "the winner must not depend on the rotation: {:?}",
            &without_rotation[..1]
        );
        let rotated = score_of(&with_rotation, exact);
        let unrotated = score_of(&without_rotation, exact);
        assert!(
            (rotated - unrotated).abs() <= 0.01 * rotated.abs().max(unrotated.abs()),
            "dropping the rotation may only perturb the total: {rotated} vs {unrotated}"
        );
    }
}
