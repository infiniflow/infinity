// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

module;

#include "simd_common_intrin_include.h"
#include <bit>
#include <cassert>

export module maxsim_simd_funcs;
import stl;
import simd_common_tools;

namespace infinity {

export f32 maxsim_f32_bit_ip_plain(const f32 *v1, const u8 *v2, SizeT dim) {
    assert(dim % 8 == 0);
    const auto u8_mask_p = GetU8MasksForAVX2();
    const SizeT dim8 = dim / 8;
    f32 sum = 0.0f;
    for (SizeT i = 0; i < dim8; ++i) {
        auto *mask_ptr = u8_mask_p[v2[i]];
        auto *v1_ptr = v1 + i * 8;
        for (SizeT j = 0; j < 8; ++j) {
            sum += std::bit_cast<f32>(std::bit_cast<u32>(v1_ptr[j]) & mask_ptr[j]);
        }
    }
    return sum;
}

#if defined(__AVX2__)
export f32 maxsim_f32_bit_ip_avx2(const f32 *v1, const u8 *v2, SizeT dim) {
    assert(dim % 8 == 0);
    const auto u8_mask_p = GetU8MasksForAVX2();
    const SizeT dim8 = dim / 8;
    __m256 v2_mask = _mm256_load_ps(reinterpret_cast<float const *>(u8_mask_p[v2[0]]));
    __m256 v1_8 = _mm256_loadu_ps(v1);
    __m256 sum_8 = _mm256_and_ps(v1_8, v2_mask);
    for (SizeT i = 1; i < dim8; ++i) {
        v2_mask = _mm256_load_ps(reinterpret_cast<float const *>(u8_mask_p[v2[i]]));
        v1_8 = _mm256_loadu_ps(v1 + i * 8);
        __m256 mid_v = _mm256_and_ps(v1_8, v2_mask);
        sum_8 = _mm256_add_ps(sum_8, mid_v);
    }
    return hsum256_ps_avx(sum_8);
}
#endif

#if defined(__AVX512F__)
export f32 maxsim_f32_bit_ip_avx512(const f32 *v1, const u8 *v2, SizeT dim) {
    assert(dim % 8 == 0);
    if (dim < 16) {
        return maxsim_f32_bit_ip_avx2(v1, v2, dim);
    }
    if (reinterpret_cast<uintptr_t>(v2) % alignof(u16) == 0) {
        // use u16 ptr
        // assume endianness
        const SizeT dim8 = dim / 8;
        const auto *v2_16 = reinterpret_cast<const u16 *>(v2);
        __m512 sum = _mm512_maskz_loadu_ps(v2_16[0], v1);
        for (SizeT i = 1; 2 * i < dim8; ++i) {
            __m512 mid_v = _mm512_maskz_loadu_ps(v2_16[i], v1 + i * 16);
            sum = _mm512_add_ps(sum, mid_v);
        }
        return _mm512_reduce_add_ps(sum);
    } else {
        const SizeT dim8 = dim / 8;
        u16 mask_16 = v2[0] | (static_cast<u32>(v2[1]) << 8);
        __m512 sum = _mm512_maskz_loadu_ps(mask_16, v1);
        SizeT i = 2;
        for (; i < dim8; i += 2) {
            mask_16 = v2[i] | (static_cast<u32>(v2[i + static_cast<int>(i + 1 < dim8)]) << 8);
            __m512 mid_v = _mm512_maskz_loadu_ps(mask_16, v1 + i * 8);
            sum = _mm512_add_ps(sum, mid_v);
        }
        return _mm512_reduce_add_ps(sum);
    }
}
#endif

export i32 maxsim_i32_bit_ip_plain(const i32 *v1, const u8 *v2, SizeT dim) {
    assert(dim % 8 == 0);
    const auto u8_mask_p = GetU8MasksForAVX2();
    const SizeT dim8 = dim / 8;
    i32 sum = 0;
    for (SizeT i = 0; i < dim8; ++i) {
        auto *mask_ptr = u8_mask_p[v2[i]];
        auto *v1_ptr = v1 + i * 8;
        for (SizeT j = 0; j < 8; ++j) {
            sum += v1_ptr[j] & static_cast<i32>(mask_ptr[j]);
        }
    }
    return sum;
}

#if defined(__AVX2__)
export i32 maxsim_i32_bit_ip_avx2(const i32 *v1, const u8 *v2, SizeT dim) {
    assert(dim % 8 == 0);
    const auto u8_mask_p = GetU8MasksForAVX2();
    const SizeT dim8 = dim / 8;
    __m256i v2_mask = _mm256_load_si256(reinterpret_cast<const __m256i *>(u8_mask_p[v2[0]]));
    __m256i v1_8 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(v1));
    __m256i sum_8 = _mm256_and_si256(v1_8, v2_mask);
    for (SizeT i = 1; i < dim8; ++i) {
        v2_mask = _mm256_load_si256(reinterpret_cast<const __m256i *>(u8_mask_p[v2[i]]));
        v1_8 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(v1 + i * 8));
        __m256i mid_v = _mm256_and_si256(v1_8, v2_mask);
        sum_8 = _mm256_add_epi32(sum_8, mid_v);
    }
    return hsum_8x32_avx2(sum_8);
}
#endif

#if defined(__AVX512F__)
export i32 maxsim_i32_bit_ip_avx512(const i32 *v1, const u8 *v2, SizeT dim) {
    assert(dim % 8 == 0);
    if (dim < 16) {
        return maxsim_i32_bit_ip_avx2(v1, v2, dim);
    }
    if (reinterpret_cast<uintptr_t>(v2) % alignof(u16) == 0) {
        // use u16 ptr
        // assume endianness
        const SizeT dim8 = dim / 8;
        const auto *v2_16 = reinterpret_cast<const u16 *>(v2);
        __m512i sum = _mm512_maskz_loadu_epi32(v2_16[0], v1);
        for (SizeT i = 1; 2 * i < dim8; ++i) {
            __m512i mid_v = _mm512_maskz_loadu_epi32(v2_16[i], v1 + i * 16);
            sum = _mm512_add_epi32(sum, mid_v);
        }
        return _mm512_reduce_add_epi32(sum);
    } else {
        const SizeT dim8 = dim / 8;
        u16 mask_16 = v2[0] | (static_cast<u32>(v2[1]) << 8);
        __m512i sum = _mm512_maskz_loadu_epi32(mask_16, v1);
        SizeT i = 2;
        for (; i < dim8; i += 2) {
            mask_16 = v2[i] | (static_cast<u32>(v2[i + static_cast<int>(i + 1 < dim8)]) << 8);
            __m512i mid_v = _mm512_maskz_loadu_epi32(mask_16, v1 + i * 8);
            sum = _mm512_add_epi32(sum, mid_v);
        }
        return _mm512_reduce_add_epi32(sum);
    }
}
#endif

export i64 maxsim_i64_bit_ip_plain(const i64 *v1, const u8 *v2, SizeT dim) {
    assert(dim % 8 == 0);
    const auto u8_mask_p = GetU8MasksForAVX2();
    const SizeT dim8 = dim / 8;
    i64 sum = 0;
    for (SizeT i = 0; i < dim8; ++i) {
        auto *mask_ptr = u8_mask_p[v2[i]];
        auto *v1_ptr = v1 + i * 8;
        for (SizeT j = 0; j < 8; ++j) {
            sum += v1_ptr[j] & static_cast<i64>(static_cast<i32>(mask_ptr[j])); // -1 -> -1
        }
    }
    return sum;
}

#if defined(__AVX2__)
export i64 maxsim_i64_bit_ip_avx2(const i64 *v1, const u8 *v2, SizeT dim) {
    assert(dim % 8 == 0);
    const auto u8_mask_p = GetU8MasksForAVX2();
    const SizeT dim8 = dim / 8;
    __m256i v2_mask = _mm256_load_si256(reinterpret_cast<const __m256i *>(u8_mask_p[v2[0]]));
    __m256i v2_mask_0 = _mm256_unpacklo_epi32(v2_mask, v2_mask);
    __m256i v2_mask_1 = _mm256_unpackhi_epi32(v2_mask, v2_mask);
    __m256i v1_4_0 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(v1));
    __m256i v1_4_1 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(v1 + 4));
    __m256i sum_4_0 = _mm256_and_si256(v1_4_0, v2_mask_0);
    __m256i sum_4_1 = _mm256_and_si256(v1_4_1, v2_mask_1);
    for (SizeT i = 1; i < dim8; ++i) {
        v2_mask = _mm256_load_si256(reinterpret_cast<const __m256i *>(u8_mask_p[v2[i]]));
        v2_mask_0 = _mm256_unpacklo_epi32(v2_mask, v2_mask);
        v2_mask_1 = _mm256_unpackhi_epi32(v2_mask, v2_mask);
        v1_4_0 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(v1 + i * 8));
        v1_4_1 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(v1 + i * 8 + 4));
        __m256i mid_v_0 = _mm256_and_si256(v1_4_0, v2_mask_0);
        __m256i mid_v_1 = _mm256_and_si256(v1_4_1, v2_mask_1);
        sum_4_0 = _mm256_add_epi64(sum_4_0, mid_v_0);
        sum_4_1 = _mm256_add_epi64(sum_4_1, mid_v_1);
    }
    __m256i sum_4 = _mm256_add_epi64(sum_4_0, sum_4_1);
    __m128i sum_2 = _mm_add_epi64(_mm256_castsi256_si128(sum_4), _mm256_extracti128_si256(sum_4, 1));
    alignas(16) i64 v[2];
    _mm_store_si128(reinterpret_cast<__m128i *>(v), sum_2);
    return v[0] + v[1];
}
#endif

#if defined(__AVX512F__)
export i64 maxsim_i64_bit_ip_avx512(const i64 *v1, const u8 *v2, SizeT dim) {
    assert(dim % 8 == 0);
    const SizeT dim8 = dim / 8;
    __m512i sum = _mm512_maskz_loadu_epi64(v2[0], v1);
    for (SizeT i = 1; i < dim8; ++i) {
        __m512i mid_v = _mm512_maskz_loadu_epi64(v2[i], v1 + i * 8);
        sum = _mm512_add_epi64(sum, mid_v);
    }
    return _mm512_reduce_add_epi64(sum);
}
#endif

} // namespace infinity
