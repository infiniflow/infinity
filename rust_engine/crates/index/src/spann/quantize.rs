//! RaBitQ-style 1-bit-per-dimension codes for one partition (`SPANN_DESIGN.md`
//! §6.2 / §8.5).
//!
//! Everything here is measured **relative to the partition centroid**, which is
//! why a partition owns one `QueryCode` per probe instead of one per chunk:
//! `cent_dist = ‖o − c‖` is what the slot stores, and the code carries only the
//! direction of `o − c`.
//!
//! The rotation is a block-diagonal randomized Hadamard transform (RHT) rather
//! than the reference's materialised `d × d` matrix:
//!
//! * the matrix costs `O(d²)` to apply and `4·d²` bytes to persist (9 MiB at
//!   1536d, per chunk in the HNSW port) — the design keeps only `seed + algo`;
//! * RHT costs `O(d log d)` to apply and **nothing** to persist: the sign vector
//!   is regenerated from `seed`, and a Walsh–Hadamard butterfly is its own
//!   inverse.
//!
//! The distance formulas (`recover_ip` / `recover_l2`) are the reference's
//! `rabitq.cppm` ones, already proven by `crate::rabitq`; they are restated here
//! because the codes live on a partition, not on a whole store.

// (test-only helper imports live in the `tests` module)

/// Bytes a 1-bit code occupies for `dim` dimensions.
pub const fn code_bytes(dim: usize) -> usize {
    dim.div_ceil(8)
}

/// Per-vector metadata that is not the code: `norm` (‖o−c‖), `raw_norm`
/// (‖o‖², for ip/cosine), `sum` (popcount), `error` (⟨u, û⟩).
pub const META_BYTES: usize = 16;

/// Bytes one encoded vector occupies in a slot's column group.
pub const fn record_bytes(dim: usize) -> usize {
    META_BYTES + code_bytes(dim)
}

/// A seeded, structured orthogonal transform: `sign` flips composed with a
/// block-diagonal Walsh–Hadamard transform.
///
/// `dim` is decomposed into a descending sum of powers of two (1536 = 1024 +
/// 512), so the transform is exact for any dimension — no padding, and the code
/// keeps the design's `ceil(dim/8)` width.
#[derive(Clone, Debug, PartialEq)]
pub struct Rotation {
    dim: usize,
    sign: Vec<f32>,
    blocks: Vec<usize>,
}

impl Rotation {
    /// `seed` is persisted in the chunk header; the same seed rebuilds the same
    /// transform byte for byte (§18 的第一条风险).
    pub fn new(dim: usize, seed: u64) -> Self {
        // Mix the seed before the xorshift. Both `seed | 1` and `seed ^ c | 1`
        // map 42 and 43 to the same state (they differ only in the bit that
        // `| 1` forces), so the seed has to go through a multiplication first.
        let mut state = seed
            .wrapping_mul(0x9E37_79B9_7F4A_7C15)
            .wrapping_add(0x2545_F491_4F6C_DD1D)
            | 1;
        let mut next = || {
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            state
        };
        let mut sign = Vec::with_capacity(dim);
        while sign.len() < dim {
            let word = next();
            for k in 0..64 {
                if sign.len() == dim {
                    break;
                }
                sign.push(if (word >> k) & 1 == 1 { 1.0 } else { -1.0 });
            }
        }
        let mut blocks = Vec::new();
        let mut rem = dim;
        while rem > 0 {
            let mut block = 1usize;
            while block * 2 <= rem {
                block *= 2;
            }
            blocks.push(block);
            rem -= block;
        }
        Self { dim, sign, blocks }
    }

    pub fn dim(&self) -> usize {
        self.dim
    }

    /// The block sizes, for the format writer (it only records `dim` + `seed`,
    /// this exists so a reader can assert the same decomposition).
    pub fn blocks(&self) -> &[usize] {
        &self.blocks
    }

    /// `R · v`. Orthogonal: `‖R·v‖ == ‖v‖`.
    pub fn apply(&self, v: &[f32]) -> Vec<f32> {
        let mut out = vec![0.0f32; self.dim];
        self.apply_into(v, &mut out);
        out
    }

    pub fn apply_into(&self, v: &[f32], out: &mut [f32]) {
        debug_assert_eq!(out.len(), self.dim);
        for i in 0..self.dim {
            out[i] = v.get(i).copied().unwrap_or(0.0) * self.sign[i];
        }
        let mut at = 0usize;
        for &block in &self.blocks {
            let slice = &mut out[at..at + block];
            fwht(slice);
            let scale = 1.0 / (block as f32).sqrt();
            for x in slice.iter_mut() {
                *x *= scale;
            }
            at += block;
        }
    }

    /// `Rᵀ · v` (the inverse; only used by decode tests and `dequantize`).
    pub fn apply_inverse(&self, v: &[f32]) -> Vec<f32> {
        let mut out: Vec<f32> = (0..self.dim)
            .map(|i| v.get(i).copied().unwrap_or(0.0))
            .collect();
        let mut at = 0usize;
        for &block in &self.blocks {
            let slice = &mut out[at..at + block];
            fwht(slice);
            let scale = 1.0 / (block as f32).sqrt();
            for x in slice.iter_mut() {
                *x *= scale;
            }
            at += block;
        }
        for i in 0..self.dim {
            out[i] *= self.sign[i];
        }
        out
    }
}

/// In-place, unscaled Walsh–Hadamard transform (`len` is a power of two).
fn fwht(x: &mut [f32]) {
    let n = x.len();
    let mut h = 1usize;
    while h < n {
        let mut i = 0usize;
        while i < n {
            for j in i..i + h {
                let a = x[j];
                let b = x[j + h];
                x[j] = a + b;
                x[j + h] = a - b;
            }
            i += h * 2;
        }
        h *= 2;
    }
}

/// One vector, encoded against its partition centroid.
#[derive(Clone, Debug, Default, PartialEq)]
pub struct Code {
    /// `‖o − c‖` in the rotated space (`cent_dist` in §6.2).
    pub norm: f32,
    /// `‖o‖²` of the raw vector, for the ip/cosine branches of §8.5.
    pub raw_norm: f32,
    /// Bits set in `bits`.
    pub sum: f32,
    /// `⟨u, û⟩` where `u = (o−c)/‖o−c‖` and `û` is its sign approximation.
    /// `1 − error` bounds the direction error (§8.3's error bound).
    pub error: f32,
    pub bits: Vec<u8>,
}

/// One query, encoded against one partition's centroid.
#[derive(Clone, Debug, Default, PartialEq)]
pub struct QueryCode {
    pub norm: f32,
    pub raw_norm: f32,
    pub sum: f32,
    pub lower_bound: f32,
    pub delta: f32,
    pub code: Vec<u8>,
    /// [`Self::code`] as `f32`, in the same order.
    ///
    /// The per-member estimate is a masked sum of these values over the set bits
    /// of the member's code: 1536 dimensions per member, per partition, per
    /// query. Converting a `u8` inside that loop kept it at ~2 ns a dimension
    /// (measured: 56 ms for 128 partitions at 1536d); the bytes cost one
    /// conversion per dimension **per query** instead.
    pub code_f32: Vec<f32>,
}

const TOLERANCE: f32 = 1e-5;
const BUCKETS: f32 = 255.0;

fn is_approx_zero(x: f32) -> bool {
    x.abs() < TOLERANCE
}

fn l2_norm(v: &[f32]) -> f32 {
    v.iter().map(|x| x * x).sum::<f32>()
}

/// Encodes `v` against `rot_centroid` (the centroid already rotated by `rot`).
pub fn encode(rot: &Rotation, rot_centroid: &[f32], v: &[f32]) -> Code {
    let dim = rot.dim();
    let raw_norm = l2_norm(v);
    let mut buf = rot.apply(v);
    let mut norm_sq = 0.0f32;
    for (x, c) in buf.iter().zip(rot_centroid) {
        let d = x - c;
        norm_sq += d * d;
    }
    // A row that sits on its centroid has no residual: the stored norm stays 0 (it
    // is what `recover_l2` multiplies by) and only the *divisor* is forced to 1, so
    // the direction stays the zero vector with every bit clear.
    let norm = if is_approx_zero(norm_sq) {
        0.0
    } else {
        norm_sq.sqrt()
    };
    let divisor = if norm == 0.0 { 1.0 } else { norm };
    for (x, c) in buf.iter_mut().zip(rot_centroid) {
        *x = (*x - c) / divisor;
    }
    let mut bits = vec![0u8; code_bytes(dim)];
    let mut sum = 0.0f32;
    for (d, x) in buf.iter().enumerate() {
        if *x >= 0.0 {
            sum += 1.0;
            bits[d / 8] |= 1 << (d % 8);
        }
    }
    let inv_sqrt_d = 1.0 / (dim as f32).sqrt();
    let mut error = 0.0f32;
    for (d, x) in buf.iter().enumerate() {
        let bit = (bits[d / 8] >> (d % 8)) & 1;
        error += if bit == 1 { inv_sqrt_d } else { -inv_sqrt_d } * *x;
    }
    Code {
        norm,
        raw_norm,
        sum,
        error: if is_approx_zero(error) { 0.8 } else { error },
        bits,
    }
}

/// Encodes the query against the same centroid (§8.5's `q̄`).
pub fn make_query(rot: &Rotation, rot_centroid: &[f32], q: &[f32]) -> QueryCode {
    let dim = rot.dim();
    let raw_norm = l2_norm(q);
    let mut buf = rot.apply(q);
    let mut norm_sq = 0.0f32;
    for (x, c) in buf.iter().zip(rot_centroid) {
        let d = x - c;
        norm_sq += d * d;
    }
    // Same split as `encode`: a zero norm is kept, the divisor is not.
    let norm = if is_approx_zero(norm_sq) {
        0.0
    } else {
        norm_sq.sqrt()
    };
    let divisor = if norm == 0.0 { 1.0 } else { norm };
    for (x, c) in buf.iter_mut().zip(rot_centroid) {
        *x = (*x - c) / divisor;
    }
    let mut lower = f32::MAX;
    let mut upper = f32::MIN;
    for x in &buf {
        lower = lower.min(*x);
        upper = upper.max(*x);
    }
    let delta = (upper - lower) / BUCKETS;
    let inv_delta = if is_approx_zero(delta) {
        0.0
    } else {
        1.0 / delta
    };
    let mut code = vec![0u8; dim];
    let mut code_f32 = vec![0.0f32; dim];
    let mut sum = 0.0f32;
    for (d, x) in buf.iter().enumerate() {
        let val = ((*x - lower) * inv_delta).round();
        sum += val;
        let bucket = val.clamp(0.0, 255.0);
        code[d] = bucket as u8;
        code_f32[d] = bucket;
    }
    QueryCode {
        norm,
        raw_norm,
        sum,
        lower_bound: lower,
        delta,
        code,
        code_f32,
    }
}

fn recover_ip(ip: f32, dim: usize, base_sum: f32, query_sum: f32, lower: f32, delta: f32) -> f32 {
    let inv_sqrt_d = 1.0 / (dim as f32).sqrt();
    let d = dim as f32;
    let p1 = inv_sqrt_d * 2.0 * delta * ip;
    let p2 = inv_sqrt_d * 2.0 * lower * base_sum;
    let p3 = inv_sqrt_d * delta * query_sum;
    let p4 = d * inv_sqrt_d * lower;
    p1 + p2 - p3 - p4
}

fn recover_l2(ip: f32, base_norm: f32, query_norm: f32) -> f32 {
    base_norm * base_norm + query_norm * query_norm - 2.0 * base_norm * query_norm * ip
}

/// Estimated **squared L2** distance between the query and the code, in the
/// rotated space (equal to the raw one: the transform is orthogonal).
pub fn distance_estimate(q: &QueryCode, c: &Code, dim: usize) -> f32 {
    distance_estimate_parts(q, c.norm, c.sum, c.error, &c.bits, dim)
}

/// [`distance_estimate`] over the raw slot columns: the file-backed reader
/// (§6.4) has the columns as byte slices and must not build a [`Code`] (one
/// allocation per member per query) just to score it.
pub fn distance_estimate_parts(
    q: &QueryCode,
    norm: f32,
    sum: f32,
    error: f32,
    bits: &[u8],
    dim: usize,
) -> f32 {
    let estimate = masked_code_sum(&q.code_f32, bits, dim);
    let ip = recover_ip(estimate, dim, sum, q.sum, q.lower_bound, q.delta);
    recover_l2(ip / error, norm, q.norm)
}

/// Eight `f32` masks, one row per possible code byte: `BIT_MASK[b][k]` is `1.0`
/// where bit `k` of `b` is set.
const BIT_MASK: [[f32; 8]; 256] = bit_masks();

const fn bit_masks() -> [[f32; 8]; 256] {
    let mut table = [[0.0f32; 8]; 256];
    let mut byte = 0usize;
    while byte < 256 {
        let mut k = 0usize;
        while k < 8 {
            table[byte][k] = if (byte >> k) & 1 == 1 { 1.0 } else { 0.0 };
            k += 1;
        }
        byte += 1;
    }
    table
}

/// `Σ_{d < dim, bit d set} code[d]`, a byte of the member's code at a time.
///
/// The one-dimension-at-a-time form (a shift, a test, a `u8 -> f32` and an add)
/// is the SPANN probe's hot loop: 128 members x 1536 dimensions per partition.
/// Turning the byte into a mask row lets the eight lanes accumulate in one
/// multiply-add, which is what eight dimensions of the reference's own SIMD pass
/// cost.
fn masked_code_sum(code: &[f32], bits: &[u8], dim: usize) -> f32 {
    use std::simd::f32x8;
    use std::simd::num::SimdFloat;
    let n = dim.min(code.len());
    let mut acc = f32x8::splat(0.0);
    let mut at = 0usize;
    while (at + 1) * 8 <= n {
        let byte = bits.get(at).copied().unwrap_or(0);
        acc += f32x8::from_slice(&code[at * 8..at * 8 + 8])
            * f32x8::from_array(BIT_MASK[byte as usize]);
        at += 1;
    }
    let mut estimate = acc.reduce_sum();
    // The last `dim % 8` dimensions are not a whole byte of the code.
    for (d, value) in code.iter().enumerate().take(n).skip(at * 8) {
        if bits
            .get(d / 8)
            .is_some_and(|byte| (byte >> (d % 8)) & 1 == 1)
        {
            estimate += *value;
        }
    }
    estimate
}

/// A bound on `|estimate − true_squared_l2|`.
///
/// The estimate divides the recovered inner product by `error` (see
/// [`distance_estimate_parts`]), i.e. it treats the reconstruction as the
/// **scaled** direction `û / ⟨u,û⟩`, whose residual is
/// `‖u − û/⟨u,û⟩‖ = √(1/⟨u,û⟩² − 1)`. That is the direction error the bound has
/// to carry: `√(2(1 − ⟨u,û⟩))` is the error of the *unscaled* sign direction and
/// under-bounds it (0.63 against 0.75 at `⟨u,û⟩ = 0.8`), which would let
/// `SearchSet` prune on an interval the truth is not in.
///
/// The query's 255-bucket quantization joins it: every coordinate of the unit
/// query direction is off by at most `delta/2`, so the direction is off by at
/// most `delta·√dim/2`.
pub fn error_bound(q: &QueryCode, c: &Code) -> f32 {
    error_bound_parts(q, c.norm, c.error)
}

/// [`error_bound`] over the raw slot columns (no allocation).
pub fn error_bound_parts(q: &QueryCode, norm: f32, error: f32) -> f32 {
    let dir_err = (1.0 / (error * error) - 1.0).max(0.0).sqrt();
    let quant_err = q.delta * (q.code.len() as f32).sqrt() * 0.5;
    2.0 * norm * q.norm * (dir_err + quant_err)
}

/// Reconstructs the raw vector from its code (tests and the `store_full_vectors`
/// debug switch).
pub fn dequantize(rot: &Rotation, rot_centroid: &[f32], c: &Code) -> Vec<f32> {
    let dim = rot.dim();
    let inv_sqrt_d = 1.0 / (dim as f32).sqrt();
    let mut buf = vec![0.0f32; dim];
    for (j, slot) in buf.iter_mut().enumerate() {
        let bit = (c.bits[j / 8] >> (j % 8)) & 1;
        *slot = if bit == 1 { inv_sqrt_d } else { -inv_sqrt_d };
    }
    for (x, centroid) in buf.iter_mut().zip(rot_centroid) {
        *x = *x * c.norm + centroid;
    }
    rot.apply_inverse(&buf)
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::rabitq::random_orthogonal_matrix;

    fn data() -> Vec<Vec<f32>> {
        (0..64)
            .map(|i| {
                (0..16)
                    .map(|d| ((i * 17 + d * 5) % 23) as f32 * 0.1 - 1.0)
                    .collect()
            })
            .collect()
    }

    #[test]
    fn rotation_is_orthogonal_and_invertible() {
        for dim in [1usize, 3, 4, 16, 100, 1536] {
            let rot = Rotation::new(dim, 7);
            let v: Vec<f32> = (0..dim).map(|i| ((i * 13) % 29) as f32 - 14.0).collect();
            let r = rot.apply(&v);
            let back = rot.apply_inverse(&r);
            for (a, b) in v.iter().zip(&back) {
                assert!((a - b).abs() < 1e-3, "dim {dim}: {a} vs {b}");
            }
            let n0: f32 = v.iter().map(|x| x * x).sum();
            let n1: f32 = r.iter().map(|x| x * x).sum();
            assert!((n0 - n1).abs() < 1e-2 * n0.max(1.0), "dim {dim} norm");
        }
    }

    /// The same seed rebuilds the same transform (§18 第一条风险的门禁).
    #[test]
    fn rotation_is_reproducible_from_the_seed() {
        let a = Rotation::new(96, 42);
        let b = Rotation::new(96, 42);
        assert_eq!(a, b);
        assert_ne!(a, Rotation::new(96, 43));
    }

    #[test]
    fn code_width_matches_the_design() {
        assert_eq!(code_bytes(128), 16);
        assert_eq!(code_bytes(1536), 192);
        assert_eq!(record_bytes(1536), 208);
        assert_eq!(record_bytes(128), 32);
    }

    #[test]
    fn estimate_and_bound_hold_on_random_vectors() {
        let data = data();
        let centroid: Vec<f32> = (0..16).map(|_| 0.0).collect();
        let rot = Rotation::new(16, 11);
        let rot_centroid = rot.apply(&centroid);
        let qc = make_query(&rot, &rot_centroid, &data[0]);
        let mut worst = 0.0f32;
        for v in &data {
            let c = encode(&rot, &rot_centroid, v);
            let est = distance_estimate(&qc, &c, 16);
            let truth: f32 = v.iter().zip(&data[0]).map(|(a, b)| (a - b) * (a - b)).sum();
            let err = (est - truth).abs();
            worst = worst.max(err);
            let bound = error_bound(&qc, &c);
            assert!(
                err <= bound + 1e-3,
                "estimate {est} vs truth {truth} (err {err} > bound {bound})"
            );
        }
        assert!(worst.is_finite());
    }

    /// A row sitting on its centroid keeps a zero residual norm.
    ///
    /// The norm is what `recover_l2` multiplies by, so storing the `1.0` that only
    /// the divisor needs put a `‖o−c‖` of 1 into every recovery of that member.
    /// With the norm kept at 0 the estimate collapses to `‖q−c‖²`, which is the
    /// truth, and the bound collapses to 0 with it.
    #[test]
    fn a_zero_residual_keeps_its_norm() {
        let rot = Rotation::new(16, 5);
        let centroid: Vec<f32> = (0..16).map(|d| d as f32 * 0.25).collect();
        let rot_centroid = rot.apply(&centroid);
        let c = encode(&rot, &rot_centroid, &centroid);
        assert_eq!(c.norm, 0.0);
        let q: Vec<f32> = (0..16).map(|d| 1.0 + d as f32).collect();
        let qc = make_query(&rot, &rot_centroid, &q);
        let truth: f32 = q
            .iter()
            .zip(&centroid)
            .map(|(a, b)| (a - b) * (a - b))
            .sum();
        let est = distance_estimate(&qc, &c, 16);
        assert!(
            (est - truth).abs() < 1e-2 * truth.max(1.0),
            "estimate {est} vs truth {truth}"
        );
        assert_eq!(error_bound(&qc, &c), 0.0);
        let back = dequantize(&rot, &rot_centroid, &c);
        let err: f32 = back
            .iter()
            .zip(&centroid)
            .map(|(a, b)| (a - b) * (a - b))
            .sum();
        assert!(err < 1e-3, "the centroid comes back, err {err}");
    }

    #[test]
    fn dequantize_returns_a_nearby_vector() {
        let data = data();
        let rot = Rotation::new(16, 3);
        let rot_centroid = rot.apply(&[0.0f32; 16]);
        for v in data.iter().take(8) {
            let c = encode(&rot, &rot_centroid, v);
            let back = dequantize(&rot, &rot_centroid, &c);
            let err: f32 = v.iter().zip(&back).map(|(a, b)| (a - b) * (a - b)).sum();
            let norm: f32 = v.iter().map(|x| x * x).sum();
            assert!(
                err.sqrt() < norm.sqrt() * 0.6 + 0.5,
                "err {err} norm {norm}"
            );
        }
    }

    /// Cross-check the RHT against the reference's dense matrix on the one
    /// property the codes depend on: both are isometries, so they preserve the
    /// norm and produce the same *distribution* of signs.
    #[test]
    fn rht_is_an_isometry_like_the_dense_matrix() {
        let dim = 16;
        let rom = random_orthogonal_matrix(dim, 9);
        let v: Vec<f32> = (0..dim).map(|i| (i as f32) - 7.5).collect();
        let mut dense = vec![0.0f32; dim];
        for (j, o) in dense.iter_mut().enumerate() {
            let mut sum = 0.0;
            for (i, x) in v.iter().enumerate() {
                sum += x * rom[i * dim + j];
            }
            *o = sum;
        }
        let rht = Rotation::new(dim, 9).apply(&v);
        let n_dense: f32 = dense.iter().map(|x| x * x).sum();
        let n_rht: f32 = rht.iter().map(|x| x * x).sum();
        assert!(
            (n_dense - n_rht).abs() < 1e-3,
            "both are isometries: {n_dense} vs {n_rht}"
        );
    }
}
