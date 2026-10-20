//! IVF retrieval over the reference's chunk layout (plan #89).
//!
//! The chunk stores `centroid_count` centroids and one part per centroid: the
//! part's `SegmentOffset`s and its raw vectors (plain storage). A search picks
//! the `nprobe` nearest centroids, scans their parts, and returns the top-k
//! candidate row offsets — the executor then scores those exactly, the same
//! shape as the HNSW candidate path.
//!
//! C++ reference: `src/storage/knn_index/knn_ivf/ivf_index_storage.cpp:291`
//! (`SearchIndex`: centroid selection + part scan) and
//! `ivf_index_storage_parts_impl.cpp` (plain part layout).

use crate::vector::l2_sq;

/// One IVF chunk in memory (plain storage).
pub struct IvfIndex {
    pub dims: usize,
    /// `centroid_count * dims` floats, row-major.
    pub centroids: Vec<f32>,
    pub centroid_count: usize,
    /// Per centroid: `(segment_offset, vector)` for every assigned embedding.
    pub parts: Vec<Vec<(u32, Vec<f32>)>>,
}

fn dot(a: &[f32], b: &[f32]) -> f32 {
    a.iter().zip(b).map(|(x, y)| x * y).sum()
}

fn cosine(a: &[f32], b: &[f32]) -> f32 {
    let na = a.iter().map(|x| x * x).sum::<f32>().sqrt();
    let nb = b.iter().map(|x| x * x).sum::<f32>().sqrt();
    if na > 0.0 && nb > 0.0 {
        dot(a, b) / (na * nb)
    } else {
        0.0
    }
}

impl IvfIndex {
    pub fn new(
        dims: usize,
        centroid_count: usize,
        centroids: Vec<f32>,
        parts: Vec<Vec<(u32, Vec<f32>)>>,
    ) -> Self {
        Self {
            dims,
            centroids,
            centroid_count,
            parts,
        }
    }

    /// Higher-is-better score for one vector (negated L2, or the similarity).
    fn score(&self, query: &[f32], vector: &[f32], metric: &str) -> f32 {
        match metric {
            "ip" => dot(query, vector),
            "cosine" => cosine(query, vector),
            _ => -l2_sq(query, vector),
        }
    }

    /// Top-k candidate row offsets: scan the `nprobe` best centroids' parts.
    pub fn candidates(&self, query: &[f32], k: usize, nprobe: usize, metric: &str) -> Vec<u32> {
        if k == 0
            || self.dims == 0
            || query.len() != self.dims
            || self.centroid_count == 0
            || self.parts.is_empty()
        {
            return Vec::new();
        }
        let nprobe = nprobe.clamp(1, self.centroid_count);
        let mut centroids: Vec<(usize, f32)> = (0..self.centroid_count)
            .map(|c| {
                let at = c * self.dims;
                let slice = &self.centroids[at..at + self.dims];
                (c, self.score(query, slice, metric))
            })
            .collect();
        // Higher score first; ties by centroid id.
        centroids.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        let mut hits: Vec<(u32, f32)> = Vec::new();
        for (c, _) in centroids.into_iter().take(nprobe) {
            if let Some(part) = self.parts.get(c) {
                for (offset, vector) in part {
                    if vector.len() == self.dims {
                        hits.push((*offset, self.score(query, vector, metric)));
                    }
                }
            }
        }
        hits.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        hits.truncate(k);
        hits.into_iter().map(|(offset, _)| offset).collect()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn sample() -> IvfIndex {
        // Two centroids; the first owns vectors near [1, 0], the second [0, 1].
        IvfIndex::new(
            2,
            2,
            vec![1.0, 0.0, 0.0, 1.0],
            vec![
                vec![(10, vec![1.0, 0.0]), (11, vec![0.9, 0.1])],
                vec![(20, vec![0.0, 1.0]), (21, vec![0.1, 0.9])],
            ],
        )
    }

    #[test]
    fn picks_the_nearest_part() {
        let index = sample();
        let hits = index.candidates(&[0.0, 1.0], 1, 1, "l2");
        assert_eq!(hits[0], 20);
        let hits = index.candidates(&[1.0, 0.0], 2, 1, "ip");
        assert_eq!(hits, vec![10, 11]);
    }

    #[test]
    fn nprobe_covers_every_part() {
        let index = sample();
        let hits = index.candidates(&[1.0, 0.0], 4, 2, "l2");
        assert_eq!(hits.len(), 4);
    }

    /// Training on residuals then encoding recovers each dimension to within
    /// one quantization step (`range / 2^bits`).
    #[test]
    fn scalar_quantization_error_stays_within_one_step() {
        let dims = 4;
        let mut residuals = Vec::new();
        for i in 0..200 {
            residuals.push(vec![
                (i as f32) * 0.01 - 1.0,
                (i as f32) * 0.02 - 2.0,
                0.5 - (i as f32) * 0.005,
                (i as f32 % 7.0) * 0.1,
            ]);
        }
        for bits in [4u8, 8] {
            let (a, b) = train_scalar_quantization(&residuals, dims, bits);
            for r in residuals.iter().skip(20).take(140) {
                let (codes, _) = encode_scalar_quantized(r, r, &a, &b, bits);
                let stride = sq_bytes_for(dims, bits);
                assert_eq!(codes.len(), stride);
                for d in 0..dims {
                    let n = sq_decode(&codes, 0, d, bits);
                    let step = if a[d] > 0.0 { a[d] } else { 1.0 };
                    let error = (r[d] - (a[d] * n + b[d])).abs();
                    assert!(
                        error <= step * 0.75,
                        "bits {bits} dim {d}: {error} > {step}"
                    );
                }
            }
        }
    }

    /// `AppendCodes`/`ExtractCodes` round-trip for every code width, including
    /// the odd subspace counts where a vector starts mid-byte.
    #[test]
    fn product_quantization_codes_roundtrip_across_a_part() {
        for bits in [4u8, 8, 12, 16] {
            for subspace_num in [1usize, 2, 3] {
                let max = (1u32 << bits) - 1;
                let mut stream = Vec::new();
                let mut stored: Vec<Vec<u32>> = Vec::new();
                for v in 0..5u32 {
                    let codes: Vec<u32> = (0..subspace_num as u32)
                        .map(|j| (v * 7 + j * 13) % max.min(4000))
                        .collect();
                    pq_append_codes(&mut stream, stored.len() * subspace_num, &codes, bits);
                    stored.push(codes);
                }
                for (v, codes) in stored.iter().enumerate() {
                    for (j, want) in codes.iter().enumerate() {
                        assert_eq!(
                            pq_code(&stream, v, j, subspace_num, bits),
                            *want,
                            "bits {bits} subspace_num {subspace_num} vector {v} subspace {j}"
                        );
                    }
                }
            }
        }
    }

    /// A scalar-quantized chunk ranks the vector its codes describe first, for
    /// both the inner-product and the L2 metric.
    #[test]
    fn scalar_quantized_chunk_ranks_the_nearest_vector() {
        let dims = 2;
        let centroids = vec![1.0, 0.0, 0.0, 1.0];
        let vectors = [
            (10u32, vec![1.0f32, 0.0]),
            (11, vec![0.9, 0.1]),
            (20, vec![0.0, 1.0]),
            (21, vec![0.1, 0.9]),
        ];
        let assign = [0usize, 0, 1, 1];
        let mut residuals: Vec<Vec<f32>> = Vec::new();
        for (i, (_, v)) in vectors.iter().enumerate() {
            let c = assign[i] * dims;
            residuals.push(
                v.iter()
                    .zip(&centroids[c..c + dims])
                    .map(|(x, c)| x - c)
                    .collect(),
            );
        }
        let (a, b) = train_scalar_quantization(&residuals, dims, 8);
        let mut parts = vec![IvfSqPartData::default(); 2];
        for (i, (offset, v)) in vectors.iter().enumerate() {
            let (codes, dot_v_e) = encode_scalar_quantized(v, &residuals[i], &a, &b, 8);
            let part = &mut parts[assign[i]];
            part.offsets.push(*offset);
            part.codes.extend_from_slice(&codes);
            part.dot_v_e.push(dot_v_e);
        }
        let index = IvfSqIndex {
            dims,
            centroids,
            centroid_count: 2,
            a,
            b,
            sq_bits: 8,
            parts,
        };
        assert_eq!(index.candidates(&[0.0, 1.0], 1, 1, "l2")[0], 20);
        assert_eq!(index.candidates(&[1.0, 0.0], 1, 1, "ip")[0], 10);
        assert_eq!(index.offsets().count(), 4);
    }
}

/// Scalar-quantized IVF chunk (`IndexIVFStorageOption::Type::kScalarQuantization`).
///
/// Each vector is stored as per-dimension codes of its **residual**
/// (`vector - centroid`), with the shared affine parameters `a` (stride) and
/// `b` (min + half a stride) held per dimension, exactly like the reference's
/// `IVF_Parts_Storage_Info<kScalarQuantization>`
/// (`ivf_index_storage_parts_impl.cpp:222-300,756-882`).
pub struct IvfSqIndex {
    pub dims: usize,
    pub centroids: Vec<f32>,
    pub centroid_count: usize,
    /// `common_vec_a` (one stride per dimension).
    pub a: Vec<f32>,
    /// `common_vec_b` (one offset per dimension).
    pub b: Vec<f32>,
    /// 4 or 8 bits per code.
    pub sq_bits: u8,
    /// Per centroid: offsets, packed codes and the reference's `dot_v_e`.
    pub parts: Vec<IvfSqPartData>,
}

/// One part of a scalar-quantized chunk.
#[derive(Debug, Clone, Default, PartialEq)]
pub struct IvfSqPartData {
    pub offsets: Vec<u32>,
    /// `len * sq_bytes_per_vector(dims, bits)` bytes.
    pub codes: Vec<u8>,
    /// `dot_v_e`: the inner product of the original vector with its
    /// quantization error (`AppendOneEmbedding`).
    pub dot_v_e: Vec<f32>,
}

/// Number of bytes one scalar-quantized code word list occupies.
pub fn sq_bytes_for(dims: usize, bits: u8) -> usize {
    if bits == 4 {
        dims.div_ceil(2)
    } else {
        dims
    }
}

/// The code stored for dimension `j` of vector `i` (`sq_decode`).
fn sq_decode(codes: &[u8], base: usize, j: usize, bits: u8) -> f32 {
    (if bits == 4 {
        let byte = codes[base + j / 2];
        if j % 2 == 1 {
            byte >> 4
        } else {
            byte & 0xf
        }
    } else {
        codes[base + j]
    }) as f32
}

/// Trains the shared `(a, b)` parameters on residuals
/// (`IVF_Parts_Storage_Info<kScalarQuantization>` constructor): per dimension,
/// the min/max after dropping the `training_num/200 + 1` extreme values at each
/// end, then `a = (max - min) / 2^bits` and `b = min + a / 2`.
pub fn train_scalar_quantization(
    residuals: &[Vec<f32>],
    dims: usize,
    bits: u8,
) -> (Vec<f32>, Vec<f32>) {
    let n = residuals.len();
    let exclude = (n / 200 + 1).min(n);
    let parts = 1u32 << bits;
    let mut a = vec![0.0f32; dims];
    let mut b = vec![0.0f32; dims];
    for d in 0..dims {
        let mut column: Vec<f32> = residuals.iter().map(|r| r[d]).collect();
        column.sort_by(|x, y| x.partial_cmp(y).unwrap_or(std::cmp::Ordering::Equal));
        // `std::priority_queue<f32>` keeps the `exclude` smallest values and
        // `top()` is the largest of them; the mirror heap keeps the `exclude`
        // largest and its `top()` is the smallest of them.
        let start = column
            .get(exclude.saturating_sub(1))
            .copied()
            .unwrap_or(column[0]);
        let end = column
            .get(n.saturating_sub(exclude))
            .copied()
            .unwrap_or(start);
        let stride = (end - start) / parts as f32;
        a[d] = stride;
        b[d] = start + 0.5 * stride;
    }
    (a, b)
}

/// Quantizes one residual into packed codes and returns them with the
/// reference's `dot_v_e` for the original vector (`AppendOneEmbedding`).
pub fn encode_scalar_quantized(
    vector: &[f32],
    residual: &[f32],
    a: &[f32],
    b: &[f32],
    bits: u8,
) -> (Vec<u8>, f32) {
    let dims = residual.len();
    let end = 1u32 << bits;
    let mut codes = vec![0u8; sq_bytes_for(dims, bits)];
    let mut error = vec![0.0f32; dims];
    for j in 0..dims {
        let stride = if a[j] > 0.0 { a[j] } else { 1.0 };
        let raw = ((residual[j] - b[j]) / stride).round();
        let n = raw.clamp(0.0, (end - 1) as f32) as u32;
        if bits == 4 {
            let byte = &mut codes[j / 2];
            if j % 2 == 1 {
                *byte |= (n as u8) << 4;
            } else {
                *byte |= n as u8;
            }
        } else {
            codes[j] = n as u8;
        }
        error[j] = residual[j] - (a[j] * n as f32 + b[j]);
    }
    let dot_v_e: f32 = vector.iter().zip(&error).map(|(x, e)| x * e).sum();
    (codes, dot_v_e)
}

impl IvfSqIndex {
    fn score_centroid(&self, query: &[f32], centroid: usize, metric: &str) -> f32 {
        let at = centroid * self.dims;
        let c = &self.centroids[at..at + self.dims];
        match metric {
            "ip" => dot(query, c),
            "cosine" => cosine(query, c),
            _ => -l2_sq(query, c),
        }
    }

    /// Top-k candidate row offsets (`SearchIndexT` for `kScalarQuantization`).
    pub fn candidates(&self, query: &[f32], k: usize, nprobe: usize, metric: &str) -> Vec<u32> {
        if k == 0
            || self.dims == 0
            || query.len() != self.dims
            || self.centroid_count == 0
            || self.parts.is_empty()
        {
            return Vec::new();
        }
        let nprobe = nprobe.clamp(1, self.centroid_count);
        let mut centroids: Vec<(usize, f32)> = (0..self.centroid_count)
            .map(|c| (c, self.score_centroid(query, c, metric)))
            .collect();
        centroids.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        let bytes = sq_bytes_for(self.dims, self.sq_bits);
        let query_l2: f32 = query.iter().map(|x| x * x).sum();
        let x_mult_a: Vec<f32> = query.iter().zip(&self.a).map(|(x, a)| x * a).collect();
        let a_square: Vec<f32> = self.a.iter().map(|a| a * a).collect();
        let mut hits: Vec<(u32, f32)> = Vec::new();
        for (part_id, _) in centroids.into_iter().take(nprobe) {
            let Some(part) = self.parts.get(part_id) else {
                continue;
            };
            let at = part_id * self.dims;
            let c = &self.centroids[at..at + self.dims];
            // `c + b` and the prefactors the reference hoists out of the loop.
            let c_plus_b: Vec<f32> = c.iter().zip(&self.b).map(|(c, b)| c + b).collect();
            let b_plus_c_minus_x: Vec<f32> =
                c_plus_b.iter().zip(query).map(|(cb, x)| cb - x).collect();
            let dot_x_c_b: f32 = query.iter().zip(&c_plus_b).map(|(x, y)| x * y).sum();
            let c_plus_b_l2: f32 = c_plus_b.iter().map(|v| v * v).sum();
            let b_plus_c_minus_x_l2: f32 = b_plus_c_minus_x.iter().map(|v| v * v).sum();
            let c_plus_b_mult_a_2: Vec<f32> = c_plus_b
                .iter()
                .zip(&self.a)
                .map(|(cb, a)| 2.0 * cb * a)
                .collect();
            let a_mult_b_plus_c_minus_x_2: Vec<f32> = b_plus_c_minus_x
                .iter()
                .zip(&self.a)
                .map(|(d, a)| 2.0 * a * d)
                .collect();
            for i in 0..part.offsets.len() {
                let base = i * bytes;
                let offset = part.offsets[i];
                let score = match metric {
                    // dot(x, v) = dot(x, c + b) + dot(v, e) + dot(x * a, code)
                    "ip" => {
                        let mut d = dot_x_c_b + part.dot_v_e.get(i).copied().unwrap_or(0.0);
                        for (j, xa) in x_mult_a.iter().enumerate() {
                            d += xa * sq_decode(&part.codes, base, j, self.sq_bits);
                        }
                        d
                    }
                    // `Sift1M` shows dropping `dot(v, e)` / `dot(e, e)` is better.
                    "cosine" => {
                        let mut ip = dot_x_c_b;
                        let mut v_l2 = c_plus_b_l2;
                        for j in 0..self.dims {
                            let n = sq_decode(&part.codes, base, j, self.sq_bits);
                            ip += x_mult_a[j] * n;
                            v_l2 += (a_square[j] * n + c_plus_b_mult_a_2[j]) * n;
                        }
                        ip / (query_l2 * v_l2).sqrt().max(f32::MIN_POSITIVE)
                    }
                    // l2² is a distance; negate so callers keep "higher is better".
                    _ => {
                        let mut d = b_plus_c_minus_x_l2;
                        for j in 0..self.dims {
                            let n = sq_decode(&part.codes, base, j, self.sq_bits);
                            d += (a_square[j] * n + a_mult_b_plus_c_minus_x_2[j]) * n;
                        }
                        -d
                    }
                };
                hits.push((offset, score));
            }
        }
        hits.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        hits.truncate(k);
        hits.into_iter().map(|(offset, _)| offset).collect()
    }

    /// Every row offset the chunk covers.
    pub fn offsets(&self) -> impl Iterator<Item = u32> + '_ {
        self.parts.iter().flat_map(|p| p.offsets.iter().copied())
    }
}

/// One sealed IVF chunk, in whichever storage layout its index was created
/// with (`IndexIVFStorageOption`).
pub enum IvfSegment {
    Plain(IvfIndex),
    Scalar(IvfSqIndex),
    Product(IvfPqIndex),
}

impl IvfSegment {
    /// Top-k candidate row offsets for `query`.
    pub fn candidates(&self, query: &[f32], k: usize, nprobe: usize, metric: &str) -> Vec<u32> {
        match self {
            IvfSegment::Plain(index) => index.candidates(query, k, nprobe, metric),
            IvfSegment::Scalar(index) => index.candidates(query, k, nprobe, metric),
            IvfSegment::Product(index) => index.candidates(query, k, nprobe, metric),
        }
    }

    /// Every row offset the chunk covers, so the executor can tell whether the
    /// index still covers the whole table.
    pub fn offsets(&self) -> Vec<u32> {
        match self {
            IvfSegment::Plain(index) => index
                .parts
                .iter()
                .flat_map(|p| p.iter().map(|(offset, _)| *offset))
                .collect(),
            IvfSegment::Scalar(index) => index.offsets().collect(),
            IvfSegment::Product(index) => index.offsets().collect(),
        }
    }
}

/// Product-quantized IVF chunk (`IndexIVFStorageOption::Type::kProductQuantization`).
///
/// The residuals are split into `subspace_num` subspaces; each subspace keeps
/// `real_centroid_num` codewords and every vector stores one code per subspace
/// (`ivf_index_storage_parts_impl.cpp:302-430,1080-1215`).
pub struct IvfPqIndex {
    pub dims: usize,
    pub centroids: Vec<f32>,
    pub centroid_count: usize,
    pub subspace_num: usize,
    pub real_centroid_num: usize,
    /// `[subspace][centroid][sub_dim]` — `subspace_centroids_data_`.
    pub codewords: Vec<f32>,
    /// `-0.5 * ||centroid||^2` per `[subspace][centroid]`.
    pub norms_neg_half: Vec<f32>,
    /// 4, 8, 12 or 16 bits per code.
    pub bits: u8,
    pub parts: Vec<IvfPqPartData>,
}

/// One part of a product-quantized chunk.
#[derive(Debug, Clone, Default, PartialEq)]
pub struct IvfPqPartData {
    pub offsets: Vec<u32>,
    /// Packed code words, `subspace_num` codes per vector.
    pub codes: Vec<u8>,
}

/// Bytes one vector's code words occupy (the reference's row memory cost).
pub fn pq_bytes_per_vector(subspace_num: usize, bits: u8) -> usize {
    match bits {
        0..=4 => subspace_num.div_ceil(2),
        5..=8 => subspace_num,
        9..=12 => subspace_num * 3 / 2 + subspace_num % 2,
        _ => subspace_num * 2,
    }
}

/// Reads code `subspace` of vector `idx` (`PQ_Code_StorageT::ExtractCodes`).
fn pq_code(codes: &[u8], idx: usize, subspace: usize, subspace_num: usize, bits: u8) -> u32 {
    match bits {
        0..=4 => {
            let pos = idx * subspace_num + subspace;
            let byte = codes[pos >> 1];
            u32::from(if pos & 1 == 1 { byte >> 4 } else { byte & 0xf })
        }
        5..=8 => u32::from(codes[idx * subspace_num + subspace]),
        9..=12 => {
            let half = idx * subspace_num * 3;
            let mut byte_pos = half >> 1;
            let mut is_odd = half & 1 == 1;
            for _ in 0..subspace {
                byte_pos += if is_odd { 2 } else { 1 };
                is_odd = !is_odd;
            }
            let pack_16 = u32::from(codes[byte_pos]) | (u32::from(codes[byte_pos + 1]) << 8);
            if is_odd {
                pack_16 >> 4
            } else {
                pack_16 & 0xfff
            }
        }
        _ => {
            let at = (idx * subspace_num + subspace) * 2;
            u32::from(u16::from_le_bytes([codes[at], codes[at + 1]]))
        }
    }
}

/// Appends one vector's codes to a part's packed stream
/// (`PQ_Code_StorageT::AppendCodes`).
///
/// The codes are a continuous nibble/byte/word stream across the whole part,
/// so `last_code_id` (the number of codes already stored) decides where this
/// vector starts — a vector may begin mid-byte for 4- and 12-bit codes.
pub fn pq_append_codes(out: &mut Vec<u8>, last_code_id: usize, codes: &[u32], bits: u8) {
    let total = last_code_id + codes.len();
    match bits {
        0..=4 => {
            out.resize(total.div_ceil(2), 0);
            for (i, code) in codes.iter().enumerate() {
                let pos = last_code_id + i;
                if pos & 1 == 1 {
                    out[pos >> 1] |= (*code as u8) << 4;
                } else {
                    out[pos >> 1] |= *code as u8;
                }
            }
        }
        5..=8 => out.extend(codes.iter().map(|c| *c as u8)),
        9..=12 => {
            out.resize((total * 3).div_ceil(2), 0);
            let mut half = last_code_id * 3;
            for code in codes {
                let byte_pos = half >> 1;
                let is_odd = half & 1 == 1;
                let write_16 = if is_odd { code << 4 } else { *code };
                out[byte_pos] |= (write_16 & 0xff) as u8;
                out[byte_pos + 1] |= (write_16 >> 8) as u8;
                half += 3;
            }
        }
        _ => {
            for code in codes {
                out.extend_from_slice(&(*code as u16).to_le_bytes());
            }
        }
    }
}

/// Trains the per-subspace codebooks on residuals
/// (`IVF_Parts_Storage_Info<kProductQuantization>::TrainResidual`).
///
/// `real_centroid_num` is `min(2^bits, training_num)`; below that threshold the
/// reference copies the training residuals instead of running k-means. The
/// k-means itself follows the deterministic seeding this crate already uses for
/// IVF centroids.
pub fn train_product_quantization(
    residuals: &[Vec<f32>],
    dims: usize,
    subspace_num: usize,
    bits: u8,
) -> (usize, Vec<f32>, Vec<f32>) {
    let subspace_dim = dims / subspace_num.max(1);
    let expect = 1usize << bits;
    let real = expect.min(residuals.len().max(1));
    let mut codewords = vec![0.0f32; dims * real];
    let mut norms = vec![0.0f32; subspace_num * real];
    for j in 0..subspace_num {
        let at = j * subspace_dim;
        let mut samples: Vec<&[f32]> = residuals
            .iter()
            .map(|r| &r[at..at + subspace_dim])
            .collect();
        let centroids: Vec<Vec<f32>> = if residuals.len() <= expect {
            samples.iter().take(real).map(|s| s.to_vec()).collect()
        } else {
            let mut assign = vec![0usize; samples.len()];
            let mut centroids: Vec<Vec<f32>> = (0..real)
                .map(|c| samples[c * samples.len() / real].to_vec())
                .collect();
            for _ in 0..10 {
                let mut changed = false;
                for (i, s) in samples.iter().enumerate() {
                    let mut best = 0usize;
                    let mut best_d = f32::INFINITY;
                    for (c, cent) in centroids.iter().enumerate() {
                        let d = l2_sq(s, cent);
                        if d < best_d {
                            best_d = d;
                            best = c;
                        }
                    }
                    if assign[i] != best {
                        assign[i] = best;
                        changed = true;
                    }
                }
                let mut sums = vec![vec![0.0f64; subspace_dim]; real];
                let mut counts = vec![0usize; real];
                for (i, s) in samples.iter().enumerate() {
                    counts[assign[i]] += 1;
                    for (d, x) in s.iter().enumerate() {
                        sums[assign[i]][d] += *x as f64;
                    }
                }
                for c in 0..real {
                    if counts[c] > 0 {
                        centroids[c] = sums[c]
                            .iter()
                            .map(|s| (*s / counts[c] as f64) as f32)
                            .collect();
                    }
                }
                if !changed {
                    break;
                }
            }
            centroids
        };
        for (c, centroid) in centroids.iter().enumerate().take(real) {
            let dst = (j * real + c) * subspace_dim;
            codewords[dst..dst + subspace_dim].copy_from_slice(centroid);
            norms[j * real + c] = -0.5 * centroid.iter().map(|x| x * x).sum::<f32>();
        }
        samples.clear();
    }
    (real, codewords, norms)
}

/// The nearest codeword of each subspace for one residual
/// (`IVF_Part_Storage_PQ::AppendOneEmbedding`, before packing).
pub fn encode_product_quantized(
    residual: &[f32],
    dims: usize,
    subspace_num: usize,
    real: usize,
    codewords: &[f32],
) -> Vec<u32> {
    let subspace_dim = dims / subspace_num.max(1);
    let mut codes = Vec::with_capacity(subspace_num);
    for j in 0..subspace_num {
        let at = j * subspace_dim;
        let slice = &residual[at..at + subspace_dim];
        let mut best = 0usize;
        let mut best_d = f32::INFINITY;
        for c in 0..real {
            let dst = (j * real + c) * subspace_dim;
            let d = l2_sq(slice, &codewords[dst..dst + subspace_dim]);
            if d < best_d {
                best_d = d;
                best = c;
            }
        }
        codes.push(best as u32);
    }
    codes
}

impl IvfPqIndex {
    fn score_centroid(&self, query: &[f32], centroid: usize, metric: &str) -> f32 {
        let at = centroid * self.dims;
        let c = &self.centroids[at..at + self.dims];
        match metric {
            "ip" => dot(query, c),
            "cosine" => cosine(query, c),
            _ => -l2_sq(query, c),
        }
    }

    /// `IVF_Parts_Storage_Info::GetIPTable`: the inner product of `v` with every
    /// codeword, indexed `[subspace][centroid]`.
    fn ip_table(&self, v: &[f32]) -> Vec<f32> {
        let subspace_dim = self.dims / self.subspace_num.max(1);
        let mut table = vec![0.0f32; self.subspace_num * self.real_centroid_num];
        for j in 0..self.subspace_num {
            let at = j * subspace_dim;
            let slice = &v[at..at + subspace_dim];
            for c in 0..self.real_centroid_num {
                let dst = (j * self.real_centroid_num + c) * subspace_dim;
                table[j * self.real_centroid_num + c] =
                    dot(slice, &self.codewords[dst..dst + subspace_dim]);
            }
        }
        table
    }

    /// Top-k candidate row offsets (`SearchIndexT` for `kProductQuantization`).
    pub fn candidates(&self, query: &[f32], k: usize, nprobe: usize, metric: &str) -> Vec<u32> {
        if k == 0
            || self.dims == 0
            || query.len() != self.dims
            || self.centroid_count == 0
            || self.parts.is_empty()
            || self.subspace_num == 0
        {
            return Vec::new();
        }
        let nprobe = nprobe.clamp(1, self.centroid_count);
        let mut centroids: Vec<(usize, f32)> = (0..self.centroid_count)
            .map(|c| (c, self.score_centroid(query, c, metric)))
            .collect();
        centroids.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        let query_l2: f32 = query.iter().map(|x| x * x).sum();
        let query_table = self.ip_table(query);
        let mut hits: Vec<(u32, f32)> = Vec::new();
        for (part_id, _) in centroids.into_iter().take(nprobe) {
            let Some(part) = self.parts.get(part_id) else {
                continue;
            };
            let at = part_id * self.dims;
            let c = &self.centroids[at..at + self.dims];
            // The two tables the reference builds once per part.
            let centroid_table = self.ip_table(c);
            let centroid_l2: f32 = c.iter().map(|x| x * x).sum();
            let residual: Vec<f32> = query.iter().zip(c).map(|(q, c)| q - c).collect();
            let residual_l2: f32 = residual.iter().map(|x| x * x).sum();
            let residual_table = self.ip_table(&residual);
            let query_centroid_ip = dot(query, c);
            for (i, offset) in part.offsets.iter().enumerate() {
                let score = match metric {
                    "ip" => {
                        let mut d = query_centroid_ip;
                        for j in 0..self.subspace_num {
                            let code =
                                pq_code(&part.codes, i, j, self.subspace_num, self.bits) as usize;
                            d += query_table[j * self.real_centroid_num + code];
                        }
                        d
                    }
                    "cosine" => {
                        let mut ip = query_centroid_ip;
                        let mut target_l2 = centroid_l2 * 0.5;
                        for j in 0..self.subspace_num {
                            let code =
                                pq_code(&part.codes, i, j, self.subspace_num, self.bits) as usize;
                            let idx = j * self.real_centroid_num + code;
                            ip += query_table[idx];
                            target_l2 += centroid_table[idx] - self.norms_neg_half[idx];
                        }
                        target_l2 *= 2.0;
                        ip / (query_l2 * target_l2).sqrt().max(f32::MIN_POSITIVE)
                    }
                    _ => {
                        let mut d = residual_l2 * 0.5;
                        for j in 0..self.subspace_num {
                            let code =
                                pq_code(&part.codes, i, j, self.subspace_num, self.bits) as usize;
                            let idx = j * self.real_centroid_num + code;
                            d -= residual_table[idx] + self.norms_neg_half[idx];
                        }
                        d *= 2.0;
                        -d
                    }
                };
                hits.push((*offset, score));
            }
        }
        hits.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        hits.truncate(k);
        hits.into_iter().map(|(offset, _)| offset).collect()
    }

    /// Every row offset the chunk covers.
    pub fn offsets(&self) -> impl Iterator<Item = u32> + '_ {
        self.parts.iter().flat_map(|p| p.offsets.iter().copied())
    }
}
