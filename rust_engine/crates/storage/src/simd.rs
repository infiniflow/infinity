//! Portable-SIMD kernels for the multi-vector `MaxSim` inner loop.
//!
//! `MaxSim` scores a document as `sum_q max_d dot(q, d)`. The dot products
//! dominate a multi-vector search - `nq * nd * dim` multiply-adds per candidate
//! document - and they are the one part of the path where a width-8 kernel
//! pays off on its own.
//!
//! This is written with `std::simd` rather than `std::arch` on purpose: it is
//! safe code, so the crates that `#![forbid(unsafe_code)]` (the executor, which
//! owns the in-memory `MaxSim`) can call it, and there is no per-target
//! `cfg`/runtime-feature-detection matrix to keep correct. The workspace pins a
//! nightly toolchain for exactly this reason (`rust-toolchain.toml`).
//!
//! ## Determinism and accuracy
//!
//! The lanes accumulate independently and are reduced in a fixed tree, so the
//! result is deterministic across runs and machines but differs from a strictly
//! sequential sum in the last bits (relative error ~1e-7 for `f32`). Below one
//! vector the scalar loop runs instead and the result is bit-identical to the
//! sequential form, which keeps small dimensions (and the exact-value tests
//! that use them) unchanged.

use std::simd::{f32x8, num::SimdFloat};

/// Dot product over the shorter of the two slices.
pub fn dot_f32(a: &[f32], b: &[f32]) -> f32 {
    let n = a.len().min(b.len());
    let (a, b) = (&a[..n], &b[..n]);
    if n < 8 {
        return a.iter().zip(b).map(|(x, y)| x * y).sum();
    }
    let mut acc = f32x8::splat(0.0);
    let mut i = 0usize;
    while i + 8 <= n {
        let va = f32x8::from_slice(&a[i..i + 8]);
        let vb = f32x8::from_slice(&b[i..i + 8]);
        acc += va * vb;
        i += 8;
    }
    let mut sum = acc.reduce_sum();
    while i < n {
        sum += a[i] * b[i];
        i += 1;
    }
    sum
}

/// `MaxSim` over one document held as `Vec<f32>` tokens.
///
/// A token whose width differs from the query's is skipped, matching the
/// scalar loop the callers used before (`t.len() != q.len()`).
pub fn maxsim_f32(query: &[Vec<f32>], doc: &[Vec<f32>]) -> f32 {
    let mut total = 0.0f32;
    for q in query {
        let mut best = f32::NEG_INFINITY;
        for t in doc {
            if t.len() != q.len() {
                continue;
            }
            let score = dot_f32(q, t);
            if score > best {
                best = score;
            }
        }
        if best != f32::NEG_INFINITY {
            total += best;
        }
    }
    total
}

#[cfg(test)]
mod tests {
    use super::*;

    fn sequential(a: &[f32], b: &[f32]) -> f32 {
        a.iter().zip(b).map(|(x, y)| x * y).sum()
    }

    #[test]
    fn short_dots_are_bit_identical_to_the_scalar_loop() {
        for n in 0..8usize {
            let a: Vec<f32> = (0..n).map(|i| (i as f32 + 1.0) * 0.37).collect();
            let b: Vec<f32> = (0..n).map(|i| (i as f32) * -0.11 + 0.5).collect();
            assert_eq!(dot_f32(&a, &b).to_bits(), sequential(&a, &b).to_bits());
        }
    }

    #[test]
    fn long_dots_match_the_scalar_loop_within_float_error() {
        for n in [8usize, 9, 15, 16, 128, 129] {
            let a: Vec<f32> = (0..n).map(|i| ((i * 37) % 17) as f32 - 8.0).collect();
            let b: Vec<f32> = (0..n).map(|i| ((i * 11) % 13) as f32 * 0.5).collect();
            let want = sequential(&a, &b);
            let got = dot_f32(&a, &b);
            let tolerance = want.abs().max(1.0) * 1e-6;
            assert!((got - want).abs() <= tolerance, "n={n}: {got} vs {want}");
        }
    }

    #[test]
    fn dot_uses_the_shorter_slice() {
        let a = [1.0f32, 2.0, 3.0];
        let b = [1.0f32, 1.0, 1.0, 100.0, 100.0];
        assert_eq!(dot_f32(&a, &b), 6.0);
        assert_eq!(dot_f32(&b, &a), 6.0);
    }

    #[test]
    fn maxsim_picks_the_best_token_per_query_row() {
        let query = vec![vec![1.0f32, 0.0], vec![0.0, 1.0]];
        let doc = vec![vec![1.0f32, 0.0], vec![0.0, 3.0], vec![2.0, 2.0]];
        // row 0: best is 1.0 (or 2.0 on the diagonal token), row 1: best is 3.0
        assert_eq!(maxsim_f32(&query, &doc), 2.0 + 3.0);
    }

    #[test]
    fn maxsim_skips_tokens_of_a_different_width() {
        let query = vec![vec![1.0f32, 1.0]];
        let doc = vec![vec![1.0f32, 1.0, 1.0], vec![2.0, 2.0]];
        assert_eq!(maxsim_f32(&query, &doc), 4.0);
    }

    #[test]
    fn maxsim_of_an_empty_document_is_zero() {
        let query = vec![vec![1.0f32]];
        assert_eq!(maxsim_f32(&query, &[]), 0.0);
        assert_eq!(maxsim_f32(&[], &[vec![1.0f32]]), 0.0);
    }

    /// Timing harness: `nq` query rows against one `nd`-token document, the
    /// shape a multi-vector re-rank walks per candidate.
    ///
    /// ```text
    /// cargo test --release -p storage --lib simd::tests::maxsim_throughput -- --ignored --nocapture
    /// ```
    #[test]
    #[ignore = "timing"]
    fn maxsim_throughput() {
        let (nq, nd, dim) = (32usize, 32usize, 128usize);
        let query: Vec<Vec<f32>> = (0..nq)
            .map(|q| (0..dim).map(|i| ((q + i) % 13) as f32 * 0.25).collect())
            .collect();
        let doc: Vec<Vec<f32>> = (0..nd)
            .map(|t| (0..dim).map(|i| ((t * 7 + i) % 11) as f32 - 5.0).collect())
            .collect();

        let rounds = 200;
        let t0 = std::time::Instant::now();
        let mut acc = 0f32;
        for _ in 0..rounds {
            acc += maxsim_f32(&query, &doc);
        }
        let simd = t0.elapsed() / rounds;

        // The loop the kernel replaced, so both numbers come from one run.
        let t0 = std::time::Instant::now();
        let mut scalar_acc = 0f32;
        for _ in 0..rounds {
            let mut total = 0f32;
            for q in &query {
                let mut best = f32::NEG_INFINITY;
                for t in &doc {
                    if t.len() != q.len() {
                        continue;
                    }
                    let score: f32 = q.iter().zip(t).map(|(a, b)| a * b).sum();
                    if score > best {
                        best = score;
                    }
                }
                if best != f32::NEG_INFINITY {
                    total += best;
                }
            }
            scalar_acc += total;
        }
        let scalar = t0.elapsed() / rounds;
        eprintln!(
            "maxsim nq={nq} nd={nd} dim={dim}: scalar={scalar:?} simd={simd:?} (acc={acc} {scalar_acc})"
        );
    }
}
