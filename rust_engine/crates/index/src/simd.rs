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

use std::simd::num::SimdFloat;
use std::simd::{f32x16, f32x8};

/// Lanes of the accumulator: `f32x8` is one AVX register, or two SSE ones.
const LANES: usize = 8;

/// One `f32x8` loaded from a fixed-size lane group.
#[inline(always)]
fn lane8(chunk: &[f32; LANES]) -> f32x8 {
    f32x8::from_array(*chunk)
}

/// Squared L2 distance, the reference's `F32L2` family.
///
/// Uses `f32x16` (16 floats/iter: one AVX-512 `zmm` on Zen 4+, or two AVX2
/// `ymm` accumulators on `x86-64-v3`) with an `f32x8` + scalar tail.
#[inline]
pub fn l2_sq(a: &[f32], b: &[f32]) -> f32 {
    let n = a.len().min(b.len());
    let (a, b) = (&a[..n], &b[..n]);
    let (av16, at16) = a.as_chunks::<16>();
    let (bv16, bt16) = b.as_chunks::<16>();
    let mut acc16 = f32x16::splat(0.0);
    for (x, y) in av16.iter().zip(bv16) {
        let diff = f32x16::from_array(*x) - f32x16::from_array(*y);
        // `sum + diff * diff`, not `mul_add`: the reference commented the FMA
        // out in `F32L2AVX512` and this keeps the same rounding.
        acc16 += diff * diff;
    }
    let mut sum = acc16.reduce_sum();
    let (av8, at) = at16.as_chunks::<8>();
    let (bv8, bt) = bt16.as_chunks::<8>();
    if let (Some(x), Some(y)) = (av8.first(), bv8.first()) {
        let diff = lane8(x) - lane8(y);
        sum += (diff * diff).reduce_sum();
    }
    for (x, y) in at.iter().zip(bt) {
        let diff = x - y;
        sum += diff * diff;
    }
    sum
}

/// [`l2_sq`] over a row that is still its little-endian `f32` bytes.
///
/// A block stores `EMBEDDING(FLOAT, d)` as `d` LE `f32` per row, so the exact
/// scan has the query as `f32` and the row as bytes. Transcoding the row into a
/// `Vec<f32>` first costs one allocation-free pass but still touches every byte
/// twice; reading it in place makes the load the comparison. `8` lanes of `f32`
/// are `32` bytes, so one `[u8; 32]` group is one `f32x8`.
#[inline]
pub fn l2_sq_bytes(a: &[f32], b: &[u8]) -> f32 {
    const GROUP: usize = LANES * 4;
    let (bv, bt) = b.as_chunks::<GROUP>();
    let n = a.len().min(bv.len() * LANES);
    let (av, at) = a[..n].as_chunks::<LANES>();
    let mut acc = f32x8::splat(0.0);
    for (x, y) in av.iter().zip(bv) {
        let mut lanes = [0f32; LANES];
        for (slot, bytes) in lanes.iter_mut().zip(y.as_chunks::<4>().0) {
            *slot = f32::from_le_bytes(*bytes);
        }
        let diff = lane8(x) - f32x8::from_array(lanes);
        acc += diff * diff;
    }
    let mut sum = acc.reduce_sum();
    // The tail: the row's bytes past the last whole group, paired with the
    // query's remaining lanes. `bt` is that remainder for the bytes.
    for (x, bytes) in at.iter().zip(bt.as_chunks::<4>().0) {
        let diff = x - f32::from_le_bytes(*bytes);
        sum += diff * diff;
    }
    sum
}

/// Inner product, the reference's `F32IP` family.
#[inline]
pub fn ip(a: &[f32], b: &[f32]) -> f32 {
    let n = a.len().min(b.len());
    let (a, b) = (&a[..n], &b[..n]);
    let (av16, at16) = a.as_chunks::<16>();
    let (bv16, bt16) = b.as_chunks::<16>();
    let mut acc16 = f32x16::splat(0.0);
    for (x, y) in av16.iter().zip(bv16) {
        acc16 += f32x16::from_array(*x) * f32x16::from_array(*y);
    }
    let mut sum = acc16.reduce_sum();
    let (av8, at) = at16.as_chunks::<8>();
    let (bv8, bt) = bt16.as_chunks::<8>();
    if let (Some(x), Some(y)) = (av8.first(), bv8.first()) {
        sum += (lane8(x) * lane8(y)).reduce_sum();
    }
    for (x, y) in at.iter().zip(bt) {
        sum += x * y;
    }
    sum
}

/// One centroid against four rows in a single pass.
///
/// k-means assignment is "one centroid against every row", so the centroid is the
/// loop-invariant operand: loading its lane blocks once and reusing them for four
/// rows quarters its traffic and leaves the rows streaming from memory. This is
/// the register-blocked form of the `‖x‖² − 2·x·c + ‖c‖²` assignment in
/// [`crate::spann::kmeans::assign_by_norms`].
#[inline]
pub fn ip_x4(rows: [&[f32]; 4], b: &[f32]) -> [f32; 4] {
    let n = rows
        .iter()
        .map(|row| row.len())
        .min()
        .unwrap_or(0)
        .min(b.len());
    let mut acc = [f32x8::splat(0.0); 4];
    let mut i = 0;
    while i + LANES <= n {
        let y = f32x8::from_slice(&b[i..]);
        for (a, row) in acc.iter_mut().zip(rows) {
            *a += f32x8::from_slice(&row[i..]) * y;
        }
        i += LANES;
    }
    let mut sum = [
        acc[0].reduce_sum(),
        acc[1].reduce_sum(),
        acc[2].reduce_sum(),
        acc[3].reduce_sum(),
    ];
    for j in i..n {
        for (s, row) in sum.iter_mut().zip(rows) {
            *s += row[j] * b[j];
        }
    }
    sum
}

/// `(dot, |a|, |b|)` in one pass, the reference's `F32Cos*` (`F32CosAVX512`
/// carries three accumulators through the same loop).
#[inline]
pub fn cos_parts(a: &[f32], b: &[f32]) -> (f32, f32, f32) {
    let n = a.len().min(b.len());
    let (a, b) = (&a[..n], &b[..n]);
    let (av16, at16) = a.as_chunks::<16>();
    let (bv16, bt16) = b.as_chunks::<16>();
    let (mut dot16, mut na16, mut nb16) =
        (f32x16::splat(0.0), f32x16::splat(0.0), f32x16::splat(0.0));
    for (xc, yc) in av16.iter().zip(bv16) {
        let x = f32x16::from_array(*xc);
        let y = f32x16::from_array(*yc);
        dot16 += x * y;
        na16 += x * x;
        nb16 += y * y;
    }
    let (mut dot, mut na, mut nb) = (dot16.reduce_sum(), na16.reduce_sum(), nb16.reduce_sum());
    let (av8, at) = at16.as_chunks::<8>();
    let (bv8, bt) = bt16.as_chunks::<8>();
    if let (Some(xc), Some(yc)) = (av8.first(), bv8.first()) {
        let x = lane8(xc);
        let y = lane8(yc);
        dot += (x * y).reduce_sum();
        na += (x * x).reduce_sum();
        nb += (y * y).reduce_sum();
    }
    for (x, y) in at.iter().zip(bt) {
        dot += x * y;
        na += x * x;
        nb += y * y;
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
