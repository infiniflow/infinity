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

#include <immintrin.h>

module some_simd_functions;

import stl;
import emvb_simd_funcs;

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

namespace infinity {

inline f32 L2Distance_simd_128(const f32 *vector1, const f32 *vector2, u32) {
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

inline f32 L2Distance_simd_16_multi(const f32 *vector1, const f32 *vector2, const u32 dimension) {
    if (dimension < 16) {
        return fvec_L2sqr(vector1, vector2, dimension);
    }
    __m256 diff_1 = _mm256_sub_ps(_mm256_loadu_ps(vector1), _mm256_loadu_ps(vector2));
    __m256 diff_2 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 8), _mm256_loadu_ps(vector2 + 8));
    __m256 sum_1 = _mm256_mul_ps(diff_1, diff_1);
    __m256 sum_2 = _mm256_mul_ps(diff_2, diff_2);
    u32 pos = 16;
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
        distance += fvec_L2sqr(vector1 + 16, vector2 + 16, dimension - pos);
    }
    return distance;
}

f32 L2Distance_simd(const f32 *vector1, const f32 *vector2, const u32 dimension) {
    if (dimension == 128) {
        return L2Distance_simd_128(vector1, vector2, dimension);
    }
    if (dimension % 32 != 0) {
        return L2Distance_simd_16_multi(vector1, vector2, dimension);
    }
    __m256 diff_1 = _mm256_sub_ps(_mm256_loadu_ps(vector1), _mm256_loadu_ps(vector2));
    __m256 diff_2 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 8), _mm256_loadu_ps(vector2 + 8));
    __m256 diff_3 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 16), _mm256_loadu_ps(vector2 + 16));
    __m256 diff_4 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + 24), _mm256_loadu_ps(vector2 + 24));
    __m256 sum_1 = _mm256_mul_ps(diff_1, diff_1);
    __m256 sum_2 = _mm256_mul_ps(diff_2, diff_2);
    __m256 sum_3 = _mm256_mul_ps(diff_3, diff_3);
    __m256 sum_4 = _mm256_mul_ps(diff_4, diff_4);
    for (u32 pos = 32; pos + 32 <= dimension; pos += 32) {
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

} // namespace infinity