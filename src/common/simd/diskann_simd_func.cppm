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

#include <cmath>
#include "simd_common_intrin_include.h"

import stl;
import simd_common_tools;

export module diskann_simd_func;

namespace infinity {

#if defined(__aarch64__)
inline float hsum256_ps_avx(__m256 v) {
    const __m128 x128 = _mm_add_ps(_mm256_extractf128_ps(v, 1), _mm256_castps256_ps128(v));
    const __m128 x64 = _mm_add_ps(x128, _mm_movehl_ps(x128, x128));
    const __m128 x32 = _mm_add_ss(x64, _mm_shuffle_ps(x64, x64, 0x55));
    return _mm_cvtss_f32(x32);
}

inline float hsum_ps_sse1(__m128 v) { // v = [ D C | B A ]
    __m128 shuf = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 3, 0, 1)); // [ C D | A B ]
    __m128 sums = _mm_add_ps(v, shuf); // sums = [ D+C C+D | B+A A+B ]
    shuf = _mm_movehl_ps(shuf, sums);  //  [   C   D | D+C C+D ]  // let the
                                     //  compiler avoid a mov by reusing shuf
    sums = _mm_add_ss(sums, shuf);
    return _mm_cvtss_f32(sums);
}
#endif

export float hsumFloatVec(const float* array, size_t size) {
    float sum = 0.0f;
    size_t i = 0;

    for (; i < size; i++) {
        sum += array[i];
    }

    return sum;
}

export float hsumFloatVecSSE(const float* array, size_t size) {
    if (size < 4) {
        return hsumFloatVec(array, size);
    }

    float sum = 0.0f;
    size_t i = 0;

    for (; i + 4 <= size; i += 4) {
        __m128 vec = _mm_loadu_ps(&array[i]);
        sum += hsum_ps_sse1(vec); 
    }

    for (; i < size; i++) {
        sum += array[i];
    }

    return sum;
}

export float hsumFloatVecAVX(const float* array, size_t size) {
    if (size < 8) {
        return hsumFloatVec(array, size);
    }

    float sum = 0.0f;
    size_t i = 0;

    for (; i + 8 <= size; i += 8) {
        __m256 vec = _mm256_loadu_ps(&array[i]);
        sum += hsum256_ps_avx(vec); 
    }

    for (; i < size; i++) {
        sum += array[i];
    }

    return sum;
}

} // namespace infinity