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

#include "../header.h"

export module bmp_simd_func;

namespace infinity {

#if defined(USE_AVX)
void avx2_i32scatter_ps(float *base_addr, __m256i vindex, __m256 v8floats) {
    float *floats = (float *)&v8floats;
    int *indices = (int *)&vindex;

    base_addr[indices[0]] = floats[0];
    base_addr[indices[1]] = floats[1];
    base_addr[indices[2]] = floats[2];
    base_addr[indices[3]] = floats[3];
    base_addr[indices[4]] = floats[4];
    base_addr[indices[5]] = floats[5];
    base_addr[indices[6]] = floats[6];
    base_addr[indices[7]] = floats[7];
}

// for every data[i], multiple with x and add to dest[idx[i]]
void MultiF32StoreI32AVX(const int32_t *idx, const float *data, float *dest, float x, size_t dim) {
    const float *data_end = data + (dim & ~7);
    while (data < data_end) {
        __m256i vindex = _mm256_loadu_si256((const __m256i *)idx);
        __m256 vdata = _mm256_loadu_ps(data);
        __m256 vdest = _mm256_i32gather_ps(dest, vindex, 4);
        vdest = _mm256_fmadd_ps(vdata, _mm256_set1_ps(x), vdest);
        avx2_i32scatter_ps(dest, vindex, vdest);
        idx += 8;
        data += 8;
    }
}

#endif

#if defined(USE_SSE)
void avx2_i32scatter_ps(float *base_addr, __m128i vindex, __m128i v8floats) {
    float *floats = (float *)&v8floats;
    int *indices = (int *)&vindex;

    base_addr[indices[0]] = floats[0];
    base_addr[indices[1]] = floats[1];
    base_addr[indices[2]] = floats[2];
    base_addr[indices[3]] = floats[3];
}

void MultiF32StoreI32SSE(const int32_t *idx, const float *data, float *dest, float x, size_t dim) {
    const float *data_end = data + (dim & ~3);
    while (data < data_end) {
        __m128i vindex = _mm_loadu_si128((const __m128i *)idx);
        __m128 vdata = _mm_loadu_ps(data);
        __m128 vdest = _mm_i32gather_ps(dest, vindex, 4);
        vdest = _mm_fmadd_ps(vdata, _mm_set1_ps(x), vdest);
        avx2_i32scatter_ps(dest, vindex, vdest);
        idx += 4;
        data += 4;
    }
}

#endif

void MultiF32StoreI32BF(const int32_t *idx, const float *data, float *dest, float x, size_t dim) {
    for (size_t i = 0; i < dim; ++i) {
        dest[idx[i]] += data[i] * x;
    }
}

export void MultiF32StoreI32(const int32_t *idx, const float *data, float *dest, float x, size_t dim) {
#if defined(USE_AVX)
    if (dim >= 8) {
        MultiF32StoreI32AVX(idx, data, dest, x, dim);
        size_t step = dim & ~7;
        idx += step;
        data += step;
        dim &= 7;
    }
#endif
#if defined(USE_SSE) || defined(USE_AVX)
    if (dim >= 4) {
        MultiF32StoreI32SSE(idx, data, dest, x, dim);
        size_t step = dim & ~3;
        idx += step;
        data += step;
        dim &= 3;
    }
#endif
    if (dim > 0) {
        MultiF32StoreI32BF(idx, data, dest, x, dim);
    }
}

#if defined(USE_AVX)

void MultiF32StoreI8AVX(const int8_t *idx, const float *data, float *dest, float x, size_t dim) {
    const float *data_end = data + (dim & ~31);
    while (data < data_end) {
        __m256i vindex = _mm256_loadu_si256((const __m256i *)idx);
        __m256 vdata = _mm256_loadu_ps(data);
        __m256 vdest = _mm256_i32gather_ps(dest, vindex, 4);
        vdest = _mm256_fmadd_ps(vdata, _mm256_set1_ps(x), vdest);
        avx2_i32scatter_ps(dest, vindex, vdest);
        idx += 32;
        data += 32;
    }
}
#endif

} // namespace infinity