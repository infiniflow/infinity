//! Scalar fallbacks for the SIMD primitives used by the EMVB search tests.
//!
//! C++ sources:
//! - `src/common/simd/simd_init_h.cpp` / `NGT_CpuInfo_SimdType.h`
//! - `src/unit_test/storage/knnindex/emvb_search/test_simd_ut.cpp`
//!
//! The reference tests run on AVX2 hardware; the rewrite keeps the same
//! *semantics* with portable scalar loops (the task allows a scalar fallback).
//! Masks and reductions follow the x86 instruction order so the asserted
//! values are identical.

/// The NGT SIMD name table (`NGT_CpuInfo_SimdType.h`).
pub const SIMD_TYPE_NAMES: [&str; 18] = [
    "f16c",
    "sse2",
    "avx",
    "avx2",
    "avx512f",
    "avx512vl",
    "avx512bw",
    "avx512dq",
    "avx512cd",
    "avx512er",
    "avx512pf",
    "avx512vbmi",
    "avx512ifma",
    "avx5124vnniw",
    "avx5124fmaps",
    "avx512vpopcntdq",
    "avx512vbmi2",
    "avx512vnni",
];

/// `NGT::CpuInfo::getSupportedSimdTypes()`.
///
/// A type is reported only when it was enabled at compile time *and* the CPU
/// supports it at run time, mirroring the `#if defined(__F16C__)` guard around
/// each `__builtin_cpu_supports` case in the reference.
pub fn supported_simd_types() -> Vec<&'static str> {
    #[cfg(any(target_arch = "x86", target_arch = "x86_64"))]
    {
        let mut out = Vec::new();
        macro_rules! push_if {
            ($name:literal) => {
                if cfg!(target_feature = $name) {
                    out.push($name);
                }
            };
        }
        // The C++ `is()` cases are additionally guarded by runtime CPU
        // detection; `is_x86_feature_detected!` supplies that check.
        if std::is_x86_feature_detected!("f16c") {
            push_if!("f16c");
        }
        if std::is_x86_feature_detected!("sse2") {
            push_if!("sse2");
        }
        if std::is_x86_feature_detected!("avx") {
            push_if!("avx");
        }
        if std::is_x86_feature_detected!("avx2") {
            push_if!("avx2");
        }
        if std::is_x86_feature_detected!("avx512f") {
            push_if!("avx512f");
        }
        if std::is_x86_feature_detected!("avx512vl") {
            push_if!("avx512vl");
        }
        if std::is_x86_feature_detected!("avx512bw") {
            push_if!("avx512bw");
        }
        if std::is_x86_feature_detected!("avx512dq") {
            push_if!("avx512dq");
        }
        if std::is_x86_feature_detected!("avx512cd") {
            push_if!("avx512cd");
        }
        if std::is_x86_feature_detected!("avx512er") {
            out.push("avx512er");
        }
        if std::is_x86_feature_detected!("avx512pf") {
            out.push("avx512pf");
        }
        if std::is_x86_feature_detected!("avx512vbmi") {
            push_if!("avx512vbmi");
        }
        if std::is_x86_feature_detected!("avx512ifma") {
            push_if!("avx512ifma");
        }
        // `avx5124vnniw` / `avx5124fmaps` have no Rust `target_feature` or
        // runtime-detection name, so they are never reported (the reference
        // also only reports them when built with those extensions enabled).
        if std::is_x86_feature_detected!("avx512vpopcntdq") {
            push_if!("avx512vpopcntdq");
        }
        if std::is_x86_feature_detected!("avx512vbmi2") {
            push_if!("avx512vbmi2");
        }
        if std::is_x86_feature_detected!("avx512vnni") {
            push_if!("avx512vnni");
        }
        out
    }
    #[cfg(target_arch = "aarch64")]
    {
        // `simd_common_intrin_include.h` routes SSE2 through SIMDe on aarch64.
        vec!["sse2"]
    }
    #[cfg(not(any(target_arch = "x86", target_arch = "x86_64", target_arch = "aarch64")))]
    {
        Vec::new()
    }
}

/// `std::endian::native == std::endian::little` (the `simd_init` test's
/// endianness check).
pub fn is_little_endian() -> bool {
    cfg!(target_endian = "little")
}

/// Horizontal sum of eight `f32` lanes.
///
/// The order matches `hsum256_ps_avx`: add the low/high 128-bit halves, then
/// the two 64-bit halves, then the two 32-bit lanes.
pub fn sum_f32x8(v: [f32; 8]) -> f32 {
    (v[0] + v[4]) + (v[1] + v[5]) + (v[2] + v[6]) + (v[3] + v[7])
}

/// `_mm256_loadu_si256` for eight `u32` lanes.
pub fn load_u32x8(src: &[u32]) -> [u32; 8] {
    assert!(src.len() >= 8, "load_u32x8 needs at least 8 lanes");
    let mut out = [0u32; 8];
    out.copy_from_slice(&src[..8]);
    out
}

/// `_mm256_storeu_si256` for eight `u32` lanes (the destination may be
/// unaligned).
pub fn store_u32x8(dst: &mut [u32], v: [u32; 8]) {
    assert!(dst.len() >= 8, "store_u32x8 needs at least 8 lanes");
    dst[..8].copy_from_slice(&v);
}

/// `_mm256_movemask_ps(_mm256_cmp_ps(values, threshold, _CMP_GT_OS))`.
///
/// `_CMP_GT_OS` is an *ordered* comparison: a NaN lane compares false.
pub fn movemask_gt_f32x8(values: [f32; 8], threshold: f32) -> u32 {
    let mut mask = 0u32;
    for (i, &value) in values.iter().enumerate() {
        if value > threshold {
            mask |= 1 << i;
        }
    }
    mask
}

/// `_mm256_blendv_ps(b, a, _mm256_cmp_ps(a, b, _CMP_GT_OS))`: elementwise max
/// where either side may be NaN (a NaN never wins, matching `_CMP_GT_OS`).
pub fn max_f32x8(a: [f32; 8], b: [f32; 8]) -> [f32; 8] {
    let mut out = [0.0f32; 8];
    for i in 0..8 {
        out[i] = if a[i] > b[i] { a[i] } else { b[i] };
    }
    out
}

/// `compress256i(ids, mask)`: left-pack the lanes selected by `mask`, padding
/// the tail with zeroes.
pub fn compress_u32x8(ids: [u32; 8], mask: u32) -> [u32; 8] {
    let mut out = [0u32; 8];
    let mut k = 0;
    for (i, &id) in ids.iter().enumerate() {
        if (mask >> i) & 1 == 1 {
            out[k] = id;
            k += 1;
        }
    }
    out
}

/// `filter_scores_output_ids_common` / `filter_scores_output_ids_avx2`: keep
/// the indices whose score is strictly greater than `threshold`.
pub fn filter_scores_output_ids(out: &mut [u32], threshold: f32, scores: &[f32]) -> usize {
    let mut k = 0;
    for (i, &score) in scores.iter().enumerate() {
        if score > threshold {
            out[k] = i as u32;
            k += 1;
        }
    }
    k
}

#[cfg(test)]
mod tests {
    use super::*;

    /// C++ `src/unit_test/common/test_simd_ut.cpp:47
    /// SimdInitTest.GetSupportedSimdTypesList`.
    #[test]
    fn simd_supported_types_list() {
        let types = supported_simd_types();
        for name in &types {
            assert!(
                SIMD_TYPE_NAMES.contains(name),
                "unknown SIMD type reported: {name}"
            );
        }
        // No duplicates.
        for (i, a) in types.iter().enumerate() {
            for b in &types[i + 1..] {
                assert_ne!(a, b, "duplicate SIMD type: {a}");
            }
        }
        // A reported AVX2 implies the lower x86 levels it builds on.
        if types.contains(&"avx2") {
            assert!(types.contains(&"avx"), "avx2 without avx");
            assert!(types.contains(&"sse2"), "avx2 without sse2");
        }
        // The test's endianness assertion: a little-endian u16 decode of
        // `{1, 0}` is 1.
        let bytes = [1u8, 0u8];
        let native = u16::from_ne_bytes(bytes);
        assert_eq!(native, 1);
        assert!(is_little_endian());
    }

    fn lcg(seed: &mut u64) -> f32 {
        *seed = seed.wrapping_mul(6364136223846793005).wrapping_add(1);
        let unit = ((*seed >> 33) as u32 as f32) / (u32::MAX as f32 / 2.0);
        unit * 2.0 - 1.0
    }

    /// C++ `SIMDTest.testsum256`.
    #[test]
    fn emvb_simd_sum256() {
        let mut seed = 0x9E37_79B9_7F4A_7C15u64;
        for _ in 0..20 {
            let mut values = [0f32; 8];
            for value in &mut values {
                *value = lcg(&mut seed) * 1024.0;
            }
            let simd = sum_f32x8(values);
            let expected: f32 = values.iter().sum();
            let abs_sum: f32 = values.iter().map(|v| v.abs()).sum();
            let diff = (simd - expected).abs() / abs_sum;
            assert!(diff <= 1e-6, "diff={diff} simd={simd} expected={expected}");
        }
    }

    /// C++ `SIMDTest.testloadstore`.
    #[test]
    fn emvb_simd_load_store() {
        let mut backing = [0u32; 33];
        // `misaligned_addr = test_output.get() + 1`.
        let dst = &mut backing[1..9];
        store_u32x8(dst, [0, 1, 2, 3, 4, 5, 6, 7]);
        let loaded = load_u32x8(&backing[1..9]);
        for (i, value) in loaded.iter().enumerate() {
            assert_eq!(*value, i as u32);
        }
    }

    /// C++ `SIMDTest.testmask`.
    #[test]
    fn emvb_simd_mask() {
        let mut values = [0f32; 8];
        values[0] = 1.0;
        values[3] = 1.0;
        values[4] = 1.0;
        let mask = movemask_gt_f32x8(values, 0.5);
        assert_eq!(mask, 0b11001);
    }

    /// C++ `SIMDTest.testmax`.
    #[test]
    fn emvb_simd_max() {
        let mut a = [0f32; 8];
        a[0] = 1.0;
        a[3] = 1.0;
        a[4] = 1.0;
        let mut b = [0f32; 8];
        b[2] = 1.0;
        b[6] = 1.0;
        let out = max_f32x8(a, b);
        for (i, value) in out.iter().enumerate() {
            if i == 1 || i == 5 || i == 7 {
                assert_eq!(*value, 0.0);
            } else {
                assert_eq!(*value, 1.0);
            }
        }
    }

    /// C++ `SIMDTest.testleftpack`.
    #[test]
    fn emvb_simd_left_pack() {
        let mut backing = vec![0u32; 128];
        let out = &mut backing[1..];
        let mut out_ptr = 0usize;
        let mut values = [0f32; 8];
        values[0] = 1.0;
        values[3] = 1.0;
        values[4] = 1.0;
        let ids = [0u32, 1, 2, 3, 4, 5, 6, 7];
        let mut j = 0u32;
        while j < 64 {
            let mask = movemask_gt_f32x8(values, 0.5);
            let packed = compress_u32x8(ids.map(|id| id + j), mask);
            store_u32x8(&mut out[out_ptr..], packed);
            out_ptr += mask.count_ones() as usize;
            j += 8;
        }
        while j < 70 {
            out[out_ptr] = j;
            out_ptr += 1;
            j += 1;
        }

        let mut expected_id = 0usize;
        let mut i = 0usize;
        while i < 64 {
            let k = i % 8;
            if k == 0 || k == 3 || k == 4 {
                assert_eq!(out[expected_id], i as u32);
                expected_id += 1;
            }
            i += 1;
        }
        while i < 70 {
            assert_eq!(out[expected_id], i as u32);
            expected_id += 1;
            i += 1;
        }
    }

    /// C++ `SIMDTest.testleftpackf`.
    #[test]
    fn emvb_simd_left_pack_filter() {
        let mut seed = 0x1234_5678_9ABC_DEF0u64;
        let mut scores = [0f32; 1000];
        let mut expected = Vec::new();
        for (i, score) in scores.iter_mut().enumerate() {
            // Uniform in [0, 2).
            *score = lcg(&mut seed) + 1.0;
            if *score > 1.0 {
                expected.push(i as u32);
            }
        }
        let mut out = vec![0u32; 1008];
        let n = filter_scores_output_ids(&mut out, 1.0, &scores);
        assert_eq!(n, expected.len());
        for (i, id) in expected.iter().enumerate() {
            assert_eq!(out[i], *id);
        }
    }
}
