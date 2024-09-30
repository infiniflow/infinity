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