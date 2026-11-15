//! Portable-SIMD distance kernels for the dense vector indexes.
//!
//! C++: `src/common/simd/hnsw_simd_func.cppm` (`F32L2*`, `F32IP*`, `F32Cos*`)
//! and `src/common/simd/distance_simd_functions_impl.cpp`. The reference compiles
//! one variant per ISA - `__AVX512F__` / `__AVX2__` / `__SSE2__` in
//! `hnsw_simd_func.cppm`, selected by `-march=native` - and each variant
//! accumulates into a vector register and reduces to a scalar at the end
//! (`F32L2AVX` etc.).
//!
//! This module has the same shape with `std::simd`: an `f32x8` accumulator
//! (the reference's AVX width), a scalar tail for the remaining `dim % 8`
//! elements, and `reduce_sum` at the end. Because Rust does not enable
//! fast-math, LLVM cannot reassociate the plain iterator sums these replace, so
//! the scalar loops stayed scalar; the explicit lanes are what makes the four
//! (or eight, with AVX2) parallel accumulators expressible.
//!
//! The lane-wise order means a distance can differ from the old scalar sum in
//! the last bits, exactly as the reference's SIMD path does. Nothing here
//! reorders *results*: the comparison order is the same, only the rounding of
//! equal-to-a-few-ulps sums moves.

use std::simd::f32x8;
use std::simd::num::SimdFloat;

/// Lanes of the accumulator: `f32x8` is one AVX register, or two SSE ones.
const LANES: usize = 8;

/// Squared L2 distance, the reference's `F32L2` family.
#[inline]
pub fn l2_sq(a: &[f32], b: &[f32]) -> f32 {
    let n = a.len().min(b.len());
    let mut acc = f32x8::splat(0.0);
    let mut i = 0;
    while i + LANES <= n {
        let diff = f32x8::from_slice(&a[i..]) - f32x8::from_slice(&b[i..]);
        // `sum + diff * diff`, not `mul_add`: the reference commented the FMA
        // out in `F32L2AVX512` and this keeps the same rounding.
        acc += diff * diff;
        i += LANES;
    }
    let mut sum = acc.reduce_sum();
    for j in i..n {
        let diff = a[j] - b[j];
        sum += diff * diff;
    }
    sum
}

/// Inner product, the reference's `F32IP` family.
#[inline]
pub fn ip(a: &[f32], b: &[f32]) -> f32 {
    let n = a.len().min(b.len());
    let mut acc = f32x8::splat(0.0);
    let mut i = 0;
    while i + LANES <= n {
        acc += f32x8::from_slice(&a[i..]) * f32x8::from_slice(&b[i..]);
        i += LANES;
    }
    let mut sum = acc.reduce_sum();
    for j in i..n {
        sum += a[j] * b[j];
    }
    sum
}

/// `(dot, |a|, |b|)` in one pass, the reference's `F32Cos*` (`F32CosAVX512`
/// carries three accumulators through the same loop).
#[inline]
pub fn cos_parts(a: &[f32], b: &[f32]) -> (f32, f32, f32) {
    let n = a.len().min(b.len());
    let (mut dot, mut na, mut nb) = (f32x8::splat(0.0), f32x8::splat(0.0), f32x8::splat(0.0));
    let mut i = 0;
    while i + LANES <= n {
        let x = f32x8::from_slice(&a[i..]);
        let y = f32x8::from_slice(&b[i..]);
        dot += x * y;
        na += x * x;
        nb += y * y;
        i += LANES;
    }
    let (mut dot, mut na, mut nb) = (dot.reduce_sum(), na.reduce_sum(), nb.reduce_sum());
    for j in i..n {
        dot += a[j] * b[j];
        na += a[j] * a[j];
        nb += b[j] * b[j];
    }
    (dot, na, nb)
}

/// Cosine **similarity** (larger is closer), 0 when either vector is zero.
/// This is what `KnnDistance1` and the flat/IP scans report.
#[inline]
pub fn cosine(a: &[f32], b: &[f32]) -> f32 {
    let (dot, na, nb) = cos_parts(a, b);
    let (na, nb) = (na.sqrt(), nb.sqrt());
    if na == 0.0 || nb == 0.0 {
        0.0
    } else {
        dot / (na * nb)
    }
}

/// Cosine **distance** `-cos` (smaller is closer), what the HNSW store ranks by
/// (`PlainCosDist` in the reference).
#[inline]
pub fn cos_dist(a: &[f32], b: &[f32]) -> f32 {
    -cosine(a, b)
}

#[cfg(test)]
mod tests {
    use super::*;

    /// The scalar loops these replaced.
    fn l2_scalar(a: &[f32], b: &[f32]) -> f32 {
        a.iter().zip(b).map(|(x, y)| (x - y) * (x - y)).sum()
    }
    fn ip_scalar(a: &[f32], b: &[f32]) -> f32 {
        a.iter().zip(b).map(|(x, y)| x * y).sum()
    }

    fn data(len: usize, seed: u64) -> Vec<f32> {
        let mut state = seed;
        (0..len)
            .map(|_| {
                state ^= state << 13;
                state ^= state >> 7;
                state ^= state << 17;
                (state % 2000) as f32 / 100.0 - 10.0
            })
            .collect()
    }

    fn close(got: f32, want: f32, what: &str) {
        let tolerance = 1e-5 * want.abs().max(1.0);
        assert!(
            (got - want).abs() <= tolerance,
            "{what}: got {got}, want {want}"
        );
    }

    /// Every tail length, and both accumulator paths, agree with the scalar
    /// formula the reference's `*BF` fallbacks use.
    #[test]
    fn kernels_match_the_scalar_formulas() {
        for len in [0usize, 1, 2, 3, 7, 8, 9, 15, 16, 17, 127, 128, 129, 300] {
            let a = data(len, 0x9e37_79b9_7f4a_7c15);
            let b = data(len, 0x1234_5678_9abc_def1);
            close(l2_sq(&a, &b), l2_scalar(&a, &b), "l2");
            close(ip(&a, &b), ip_scalar(&a, &b), "ip");
            let (dot, na, nb) = cos_parts(&a, &b);
            close(dot, ip_scalar(&a, &b), "cos dot");
            close(na, ip_scalar(&a, &a), "cos |a|");
            close(nb, ip_scalar(&b, &b), "cos |b|");
            let want = if na == 0.0 || nb == 0.0 {
                0.0
            } else {
                ip_scalar(&a, &b) / (na.sqrt() * nb.sqrt())
            };
            close(cosine(&a, &b), want, "cosine");
            close(cos_dist(&a, &b), -want, "cos distance");
        }
    }

    /// A zero vector scores 0 for cosine, not NaN.
    #[test]
    fn cosine_of_a_zero_vector_is_zero() {
        let zero = vec![0.0f32; 16];
        let other = data(16, 7);
        assert_eq!(cosine(&zero, &other), 0.0);
        assert_eq!(cos_dist(&other, &zero), 0.0);
    }

    /// Mismatched lengths compare the common prefix, as the `zip` loops did.
    #[test]
    fn mismatched_lengths_use_the_common_prefix() {
        let a = data(13, 3);
        let b = data(9, 5);
        close(l2_sq(&a, &b), l2_scalar(&a, &b), "l2 prefix");
        close(ip(&a, &b), ip_scalar(&a, &b), "ip prefix");
    }
}
