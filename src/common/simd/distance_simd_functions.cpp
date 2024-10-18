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
#include <cmath>

/*
#if defined(__x86_64__) && (defined(__clang_major__) && (__clang_major__ > 10))
#define IMPRECISE_FUNCTION_BEGIN _Pragma("float_control(precise, off, push)")
#define IMPRECISE_FUNCTION_END _Pragma("float_control(pop)")
#define IMPRECISE_LOOP _Pragma("clang loop vectorize(enable) interleave(enable)")
#else
#define IMPRECISE_FUNCTION_BEGIN
#define IMPRECISE_FUNCTION_END
#define IMPRECISE_LOOP
#endif
IMPRECISE_FUNCTION_BEGIN
float fvec_L2sqr(const float *x, const float *y, const size_t d) {
    float res = 0.0f;
    IMPRECISE_LOOP
    for (size_t i = 0; i < d; ++i) {
        const float tmp = x[i] - y[i];
        res += tmp * tmp;
    }
    return res;
}
IMPRECISE_FUNCTION_END
*/

module distance_simd_functions;

import stl;
import simd_common_tools;

namespace infinity {

f32 L2Distance_common(const f32 *x, const f32 *y, SizeT d) {
    float res = 0.0f;
    for (SizeT i = 0; i < d; ++i) {
        const float tmp = x[i] - y[i];
        res += tmp * tmp;
    }
    return res;
}

f32 IPDistance_common(const f32 *x, const f32 *y, SizeT d) {
    float res = 0.0f;
    for (SizeT i = 0; i < d; ++i) {
        res += x[i] * y[i];
    }
    return res;
}

f32 CosineDistance_common(const f32 *x, const f32 *y, SizeT d) {
    float dot = 0.0f;
    float sqr_x = 0.0f;
    float sqr_y = 0.0f;
    for (SizeT i = 0; i < d; ++i) {
        dot += x[i] * y[i];
        sqr_x += x[i] * x[i];
        sqr_y += y[i] * y[i];
    }
    return dot ? dot / sqrt(sqr_x * sqr_y) : 0.0f;
}

f32 HammingDistance_common(const u8 *x, const u8 *y, SizeT d) {
    SizeT real_d = d / 8;
    f32 result = 0;
    for (SizeT i = 0; i < real_d; ++i) {
        u8 xor_result = x[i] ^ y[i];
        while (xor_result) {
            result += (xor_result | 1);
            xor_result >>= 1;
        }
    }
    return result;
}

#if defined(__AVX2__)
inline f32 L2Distance_avx2_128(const f32 *vector1, const f32 *vector2, SizeT) {
    __m256 diff_1 = _mm256_sub_ps(_mm256_loadu_ps(vector1), _mm256_loadu_ps(vector2));
    __m256 diff_2 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 8), _mm256_loadu_ps(vector2 + 8));
    __m256 diff_3 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 16), _mm256_loadu_ps(vector2 + 16));
    __m256 diff_4 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 24), _mm256_loadu_ps(vector2 + 24));
    __m256 sum_1 = _mm256_mul_ps(diff_1, diff_1);
    __m256 sum_2 = _mm256_mul_ps(diff_2, diff_2);
    __m256 sum_3 = _mm256_mul_ps(diff_3, diff_3);
    __m256 sum_4 = _mm256_mul_ps(diff_4, diff_4);
    vector1 += 32;
    vector2 += 32;
    diff_1 = _mm256_sub_ps(_mm256_loadu_ps(vector1), _mm256_loadu_ps(vector2));
    diff_2 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 8), _mm256_loadu_ps(vector2 + 8));
    diff_3 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 16), _mm256_loadu_ps(vector2 + 16));
    diff_4 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 24), _mm256_loadu_ps(vector2 + 24));
    sum_1 = _mm256_fmadd_ps(diff_1, diff_1, sum_1);
    sum_2 = _mm256_fmadd_ps(diff_2, diff_2, sum_2);
    sum_3 = _mm256_fmadd_ps(diff_3, diff_3, sum_3);
    sum_4 = _mm256_fmadd_ps(diff_4, diff_4, sum_4);
    vector1 += 32;
    vector2 += 32;
    diff_1 = _mm256_sub_ps(_mm256_loadu_ps(vector1), _mm256_loadu_ps(vector2));
    diff_2 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 8), _mm256_loadu_ps(vector2 + 8));
    diff_3 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 16), _mm256_loadu_ps(vector2 + 16));
    diff_4 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 24), _mm256_loadu_ps(vector2 + 24));
    sum_1 = _mm256_fmadd_ps(diff_1, diff_1, sum_1);
    sum_2 = _mm256_fmadd_ps(diff_2, diff_2, sum_2);
    sum_3 = _mm256_fmadd_ps(diff_3, diff_3, sum_3);
    sum_4 = _mm256_fmadd_ps(diff_4, diff_4, sum_4);
    vector1 += 32;
    vector2 += 32;
    diff_1 = _mm256_sub_ps(_mm256_loadu_ps(vector1), _mm256_loadu_ps(vector2));
    diff_2 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 8), _mm256_loadu_ps(vector2 + 8));
    diff_3 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 16), _mm256_loadu_ps(vector2 + 16));
    diff_4 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 24), _mm256_loadu_ps(vector2 + 24));
    sum_1 = _mm256_fmadd_ps(diff_1, diff_1, sum_1);
    sum_2 = _mm256_fmadd_ps(diff_2, diff_2, sum_2);
    sum_3 = _mm256_fmadd_ps(diff_3, diff_3, sum_3);
    sum_4 = _mm256_fmadd_ps(diff_4, diff_4, sum_4);
    __m256 sum_half_1 = _mm256_add_ps(sum_1, sum_2);
    __m256 sum_half_2 = _mm256_add_ps(sum_3, sum_4);
    __m256 sum = _mm256_add_ps(sum_half_1, sum_half_2);
    return hsum256_ps_avx(sum);
}

inline f32 L2Distance_avx2_16_multi(const f32 *vector1, const f32 *vector2, const SizeT dimension) {
    if (dimension < 16) [[unlikely]] {
        return L2Distance_common(vector1, vector2, dimension);
    }
    __m256 diff_1 = _mm256_sub_ps(_mm256_loadu_ps(vector1), _mm256_loadu_ps(vector2));
    __m256 diff_2 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 8), _mm256_loadu_ps(vector2 + 8));
    __m256 sum_1 = _mm256_mul_ps(diff_1, diff_1);
    __m256 sum_2 = _mm256_mul_ps(diff_2, diff_2);
    SizeT pos = 16;
    while (pos + 16 <= dimension) {
        vector1 += 16;
        vector2 += 16;
        diff_1 = _mm256_sub_ps(_mm256_loadu_ps(vector1), _mm256_loadu_ps(vector2));
        diff_2 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 8), _mm256_loadu_ps(vector2 + 8));
        sum_1 = _mm256_fmadd_ps(diff_1, diff_1, sum_1);
        sum_2 = _mm256_fmadd_ps(diff_2, diff_2, sum_2);
        pos += 16;
    }
    __m256 sum = _mm256_add_ps(sum_1, sum_2);
    float distance = hsum256_ps_avx(sum);
    if (pos < dimension) {
        distance += L2Distance_common(vector1 + 16, vector2 + 16, dimension - pos);
    }
    return distance;
}

f32 L2Distance_avx2(const f32 *vector1, const f32 *vector2, const SizeT dimension) {
    if (dimension == 128) {
        return L2Distance_avx2_128(vector1, vector2, dimension);
    }
    if (dimension % 32 != 0) [[unlikely]] {
        return L2Distance_avx2_16_multi(vector1, vector2, dimension);
    }
    __m256 diff_1 = _mm256_sub_ps(_mm256_loadu_ps(vector1), _mm256_loadu_ps(vector2));
    __m256 diff_2 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 8), _mm256_loadu_ps(vector2 + 8));
    __m256 diff_3 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 16), _mm256_loadu_ps(vector2 + 16));
    __m256 diff_4 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 24), _mm256_loadu_ps(vector2 + 24));
    __m256 sum_1 = _mm256_mul_ps(diff_1, diff_1);
    __m256 sum_2 = _mm256_mul_ps(diff_2, diff_2);
    __m256 sum_3 = _mm256_mul_ps(diff_3, diff_3);
    __m256 sum_4 = _mm256_mul_ps(diff_4, diff_4);
    for (SizeT pos = 32; pos + 32 <= dimension; pos += 32) {
        vector1 += 32;
        vector2 += 32;
        diff_1 = _mm256_sub_ps(_mm256_loadu_ps(vector1), _mm256_loadu_ps(vector2));
        diff_2 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 8), _mm256_loadu_ps(vector2 + 8));
        diff_3 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 16), _mm256_loadu_ps(vector2 + 16));
        diff_4 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 24), _mm256_loadu_ps(vector2 + 24));
        sum_1 = _mm256_fmadd_ps(diff_1, diff_1, sum_1);
        sum_2 = _mm256_fmadd_ps(diff_2, diff_2, sum_2);
        sum_3 = _mm256_fmadd_ps(diff_3, diff_3, sum_3);
        sum_4 = _mm256_fmadd_ps(diff_4, diff_4, sum_4);
    }
    __m256 sum_half_1 = _mm256_add_ps(sum_1, sum_2);
    __m256 sum_half_2 = _mm256_add_ps(sum_3, sum_4);
    __m256 sum = _mm256_add_ps(sum_half_1, sum_half_2);
    return hsum256_ps_avx(sum);
}

f32 IPDistance_avx2(const f32 *vector1, const f32 *vector2, SizeT dimension) {
    if (dimension < 16) [[unlikely]] {
        return IPDistance_common(vector1, vector2, dimension);
    }
    __m256 sum_1 = _mm256_mul_ps(_mm256_loadu_ps(vector1), _mm256_loadu_ps(vector2));
    __m256 sum_2 = _mm256_mul_ps(_mm256_loadu_ps(vector1 + 8), _mm256_loadu_ps(vector2 + 8));
    SizeT i = 16;
    for (; i + 16 <= dimension; i += 16) {
        vector1 += 16;
        vector2 += 16;
        sum_1 = _mm256_fmadd_ps(_mm256_loadu_ps(vector1), _mm256_loadu_ps(vector2), sum_1);
        sum_2 = _mm256_fmadd_ps(_mm256_loadu_ps(vector1 + 8), _mm256_loadu_ps(vector2 + 8), sum_2);
    }
    f32 distance = hsum256_ps_avx(_mm256_add_ps(sum_1, sum_2));
    if (i < dimension) [[unlikely]] {
        distance += IPDistance_common(vector1 + 16, vector2 + 16, dimension - i);
    }
    return distance;
}

f32 CosineDistance_avx2(const f32 *vector1, const f32 *vector2, SizeT dimension) {
    if (dimension < 16) [[unlikely]] {
        return CosineDistance_common(vector1, vector2, dimension);
    }
    __m256 v1_1 = _mm256_loadu_ps(vector1);
    __m256 v1_2 = _mm256_loadu_ps(vector1 + 8);
    __m256 v2_1 = _mm256_loadu_ps(vector2);
    __m256 v2_2 = _mm256_loadu_ps(vector2 + 8);
    __m256 dot_sum_1 = _mm256_mul_ps(v1_1, v2_1);
    __m256 dot_sum_2 = _mm256_mul_ps(v1_2, v2_2);
    __m256 norm_v1_1 = _mm256_mul_ps(v1_1, v1_1);
    __m256 norm_v1_2 = _mm256_mul_ps(v1_2, v1_2);
    __m256 norm_v2_1 = _mm256_mul_ps(v2_1, v2_1);
    __m256 norm_v2_2 = _mm256_mul_ps(v2_2, v2_2);
    SizeT i = 16;
    for (; i + 16 <= dimension; i += 16) {
        vector1 += 16;
        vector2 += 16;
        v1_1 = _mm256_loadu_ps(vector1);
        v1_2 = _mm256_loadu_ps(vector1 + 8);
        v2_1 = _mm256_loadu_ps(vector2);
        v2_2 = _mm256_loadu_ps(vector2 + 8);
        dot_sum_1 = _mm256_fmadd_ps(v1_1, v2_1, dot_sum_1);
        dot_sum_2 = _mm256_fmadd_ps(v1_2, v2_2, dot_sum_2);
        norm_v1_1 = _mm256_fmadd_ps(v1_1, v1_1, norm_v1_1);
        norm_v1_2 = _mm256_fmadd_ps(v1_2, v1_2, norm_v1_2);
        norm_v2_1 = _mm256_fmadd_ps(v2_1, v2_1, norm_v2_1);
        norm_v2_2 = _mm256_fmadd_ps(v2_2, v2_2, norm_v2_2);
    }
    f32 dot = hsum256_ps_avx(_mm256_add_ps(dot_sum_1, dot_sum_2));
    f32 norm_v1 = hsum256_ps_avx(_mm256_add_ps(norm_v1_1, norm_v1_2));
    f32 norm_v2 = hsum256_ps_avx(_mm256_add_ps(norm_v2_1, norm_v2_2));
    if (i < dimension) [[unlikely]] {
        vector1 += 16;
        vector2 += 16;
        for (; i < dimension; ++i) {
            const f32 f1 = *vector1;
            const f32 f2 = *vector2;
            dot += f1 * f2;
            norm_v1 += f1 * f1;
            norm_v2 += f2 * f2;
            ++vector1;
            ++vector2;
        }
    }
    return dot ? dot / sqrt(norm_v1 * norm_v2) : 0.0f;
}
#endif

} // namespace infinity
