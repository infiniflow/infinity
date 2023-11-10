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
import stl;
export module some_simd_functions;

namespace infinity {

// from https://stackoverflow.com/questions/13219146/how-to-sum-m256-horizontally
// x = ( x7, x6, x5, x4, x3, x2, x1, x0 )
float sum8(__m256 x) {
    // hiQuad = ( x7, x6, x5, x4 )
    const __m128 hiQuad = _mm256_extractf128_ps(x, 1);
    // loQuad = ( x3, x2, x1, x0 )
    const __m128 loQuad = _mm256_castps256_ps128(x);
    // sumQuad = ( x3 + x7, x2 + x6, x1 + x5, x0 + x4 )
    const __m128 sumQuad = _mm_add_ps(loQuad, hiQuad);
    // loDual = ( -, -, x1 + x5, x0 + x4 )
    const __m128 loDual = sumQuad;
    // hiDual = ( -, -, x3 + x7, x2 + x6 )
    const __m128 hiDual = _mm_movehl_ps(sumQuad, sumQuad);
    // sumDual = ( -, -, x1 + x3 + x5 + x7, x0 + x2 + x4 + x6 )
    const __m128 sumDual = _mm_add_ps(loDual, hiDual);
    // lo = ( -, -, -, x0 + x2 + x4 + x6 )
    const __m128 lo = sumDual;
    // hi = ( -, -, -, x1 + x3 + x5 + x7 )
    const __m128 hi = _mm_shuffle_ps(sumDual, sumDual, 0x1);
    // sum = ( -, -, -, x0 + x1 + x2 + x3 + x4 + x5 + x6 + x7 )
    const __m128 sum = _mm_add_ss(lo, hi);
    return _mm_cvtss_f32(sum);
}

export f32 L2Distance_simd(const f32 *vector1, const f32 *vector2, u32 dimension) {
    u32 i = 0;
    __m256 sum_1 = _mm256_setzero_ps();
    __m256 sum_2 = _mm256_setzero_ps();
    _mm_prefetch(vector1, _MM_HINT_NTA);
    _mm_prefetch(vector2, _MM_HINT_NTA);
    for (; i + 16 <= dimension; i += 16) {
        _mm_prefetch(vector1 + i + 16, _MM_HINT_NTA);
        _mm_prefetch(vector2 + i + 16, _MM_HINT_NTA);
        auto diff_1 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + i), _mm256_loadu_ps(vector2 + i));
        auto diff_2 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + i + 8), _mm256_loadu_ps(vector2 + i + 8));
        auto mul_1 = _mm256_mul_ps(diff_1, diff_1);
        auto mul_2 = _mm256_mul_ps(diff_2, diff_2);
        // add mul to sum
        sum_1 = _mm256_add_ps(sum_1, mul_1);
        sum_2 = _mm256_add_ps(sum_2, mul_2);
    }
    if (i + 8 <= dimension) {
        auto diff = _mm256_sub_ps(_mm256_loadu_ps(vector1 + i), _mm256_loadu_ps(vector2 + i));
        auto mul = _mm256_mul_ps(diff, diff);
        sum_1 = _mm256_add_ps(sum_1, mul);
        i += 8;
    }
    f32 distance = sum8(sum_1) + sum8(sum_2);
    for (; i < dimension; ++i) {
        auto diff = vector1[i] - vector2[i];
        distance += diff * diff;
    }
    return distance;
}

export f32 IPDistance_simd(const f32 *vector1, const f32 *vector2, u32 dimension) {
    u32 i = 0;
    __m256 sum_1 = _mm256_setzero_ps();
    __m256 sum_2 = _mm256_setzero_ps();
    _mm_prefetch(vector1, _MM_HINT_NTA);
    _mm_prefetch(vector2, _MM_HINT_NTA);
    for (; i + 16 <= dimension; i += 16) {
        _mm_prefetch(vector1 + i + 16, _MM_HINT_NTA);
        _mm_prefetch(vector2 + i + 16, _MM_HINT_NTA);
        auto mul_1 = _mm256_mul_ps(_mm256_loadu_ps(vector1 + i), _mm256_loadu_ps(vector2 + i));
        auto mul_2 = _mm256_mul_ps(_mm256_loadu_ps(vector1 + i + 8), _mm256_loadu_ps(vector2 + i + 8));
        // add mul to sum
        sum_1 = _mm256_add_ps(sum_1, mul_1);
        sum_2 = _mm256_add_ps(sum_2, mul_2);
    }
    if (i + 8 <= dimension) {
        auto mul = _mm256_mul_ps(_mm256_loadu_ps(vector1 + i), _mm256_loadu_ps(vector2 + i));
        sum_1 = _mm256_add_ps(sum_1, mul);
        i += 8;
    }
    f32 distance = sum8(sum_1) + sum8(sum_2);
    for (; i < dimension; ++i) {
        distance += vector1[i] * vector2[i];
    }
    return distance;
}

} // namespace infinity