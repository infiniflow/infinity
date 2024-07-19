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

export module hnsw_simd_func;

namespace infinity {

// for debug
template <typename T>
void log_m256(const __m256i &value) {
    const size_t n = sizeof(__m256i) / sizeof(T);
    T buffer[n];
    _mm256_storeu_si256((__m256i *)buffer, value);
    std::cout << "[";
    for (size_t i = 0; i < n; i++) {
        std::cout << (int)buffer[i];
        if (i != n - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

export float F32CosBF(const float *pv1, const float *pv2, size_t dim) {
    float dot_product = 0;
    float norm1 = 0;
    float norm2 = 0;
    for (size_t i = 0; i < dim; i++) {
        dot_product += pv1[i] * pv2[i];
        norm1 += pv1[i] * pv1[i];
        norm2 += pv2[i] * pv2[i];
    }
    return dot_product ? dot_product / sqrt(norm1 * norm2) : 0.0f;
}

#if defined(__AVX512F__)

export float F32CosAVX512(const float *pv1, const float *pv2, size_t dim) {
    size_t dim16 = dim >> 4;

    const float *pEnd1 = pv1 + (dim16 << 4);

    __m512 mul = _mm512_set1_ps(0);
    __m512 norm_v1 = _mm512_set1_ps(0);
    __m512 norm_v2 = _mm512_set1_ps(0);

    __m512 v1, v2;

    while (pv1 < pEnd1) {
        v1 = _mm512_loadu_ps(pv1);
        pv1 += 16;
        v2 = _mm512_loadu_ps(pv2);
        pv2 += 16;

        mul = _mm512_fmadd_ps(v1, v2, mul);
        norm_v1 = _mm512_fmadd_ps(v1, v1, norm_v1);
        norm_v2 = _mm512_fmadd_ps(v2, v2, norm_v2);
    }

    float mul_res = _mm512_reduce_add_ps(mul);
    float v1_res = _mm512_reduce_add_ps(norm_v1);
    float v2_res = _mm512_reduce_add_ps(norm_v2);

    size_t tail = dim & 15;
    const float *pBegin1 = pv1 + (dim & ~15);
    const float *pBegin2 = pv2 + (dim & ~15);
    for (size_t i = 0; i < tail; i++) {
        mul_res += pBegin1[i] * pBegin2[i];
        v1_res += pBegin1[i] * pBegin1[i];
        v2_res += pBegin2[i] * pBegin2[i];
    }

    return mul_res != 0 ? mul_res / sqrt(v1_res * v2_res) : 0;
}

export float F32CosAVX512Residual(const float *pv1, const float *pv2, size_t dim) {
    return F32CosAVX512(pv1, pv2, dim);
}

#endif

#if defined(__AVX2__)

export float F32CosAVX(const float *pv1, const float *pv2, size_t dim) {
    alignas(64) float MulTmpRes[8];
    alignas(64) float V1TmpRes[8];
    alignas(64) float V2TmpRes[8];
    size_t dim16 = dim >> 4;

    const float *pEnd1 = pv1 + (dim16 << 4);

    __m256 mul = _mm256_set1_ps(0);
    __m256 norm_v1 = _mm256_set1_ps(0);
    __m256 norm_v2 = _mm256_set1_ps(0);

    __m256 v1, v2;

    while (pv1 < pEnd1) {
        v1 = _mm256_loadu_ps(pv1);
        pv1 += 8;
        v2 = _mm256_loadu_ps(pv2);
        pv2 += 8;
        mul = _mm256_add_ps(mul, _mm256_mul_ps(v1, v2));
        norm_v1 = _mm256_add_ps(norm_v1, _mm256_mul_ps(v1, v1));
        norm_v2 = _mm256_add_ps(norm_v2, _mm256_mul_ps(v2, v2));

        v1 = _mm256_loadu_ps(pv1);
        pv1 += 8;
        v2 = _mm256_loadu_ps(pv2);
        pv2 += 8;
        mul = _mm256_add_ps(mul, _mm256_mul_ps(v1, v2));
        norm_v1 = _mm256_add_ps(norm_v1, _mm256_mul_ps(v1, v1));
        norm_v2 = _mm256_add_ps(norm_v2, _mm256_mul_ps(v2, v2));
    }

    _mm256_store_ps(MulTmpRes, mul);
    _mm256_store_ps(V1TmpRes, norm_v1);
    _mm256_store_ps(V2TmpRes, norm_v2);

    float mul_res = MulTmpRes[0] + MulTmpRes[1] + MulTmpRes[2] + MulTmpRes[3] + MulTmpRes[4] + MulTmpRes[5] + MulTmpRes[6] + MulTmpRes[7];
    float v1_res = V1TmpRes[0] + V1TmpRes[1] + V1TmpRes[2] + V1TmpRes[3] + V1TmpRes[4] + V1TmpRes[5] + V1TmpRes[6] + V1TmpRes[7];
    float v2_res = V2TmpRes[0] + V2TmpRes[1] + V2TmpRes[2] + V2TmpRes[3] + V2TmpRes[4] + V2TmpRes[5] + V2TmpRes[6] + V2TmpRes[7];

    size_t tail = dim & 15;
    const float *pBegin1 = pv1 + (dim & ~15);
    const float *pBegin2 = pv2 + (dim & ~15);
    for (size_t i = 0; i < tail; i++) {
        mul_res += pBegin1[i] * pBegin2[i];
        v1_res += pBegin1[i] * pBegin1[i];
        v2_res += pBegin2[i] * pBegin2[i];
    }

    return mul_res != 0 ? mul_res / sqrt(v1_res * v2_res) : 0;
}

export float F32CosAVXResidual(const float *pv1, const float *pv2, size_t dim) {
    return F32CosAVX(pv1, pv2, dim);
}

#endif

#if defined(__SSE2__)

export float F32CosSSE(const float *pv1, const float *pv2, size_t dim) {
    alignas(16) float MulTmpRes[4];
    alignas(16) float V1TmpRes[4];
    alignas(16) float V2TmpRes[4];
    size_t dim16 = dim >> 4;

    const float *pEnd1 = pv1 + (dim16 << 4);

    __m128 mul = _mm_set1_ps(0);
    __m128 norm_v1 = _mm_set1_ps(0);
    __m128 norm_v2 = _mm_set1_ps(0);

    __m128 v1, v2;

    while (pv1 < pEnd1) {
        v1 = _mm_loadu_ps(pv1);
        pv1 += 4;
        v2 = _mm_loadu_ps(pv2);
        pv2 += 4;
        mul = _mm_add_ps(mul, _mm_mul_ps(v1, v2));
        norm_v1 = _mm_add_ps(norm_v1, _mm_mul_ps(v1, v1));
        norm_v2 = _mm_add_ps(norm_v2, _mm_mul_ps(v2, v2));

        v1 = _mm_loadu_ps(pv1);
        pv1 += 4;
        v2 = _mm_loadu_ps(pv2);
        pv2 += 4;
        mul = _mm_add_ps(mul, _mm_mul_ps(v1, v2));
        norm_v1 = _mm_add_ps(norm_v1, _mm_mul_ps(v1, v1));
        norm_v2 = _mm_add_ps(norm_v2, _mm_mul_ps(v2, v2));

        v1 = _mm_loadu_ps(pv1);
        pv1 += 4;
        v2 = _mm_loadu_ps(pv2);
        pv2 += 4;
        mul = _mm_add_ps(mul, _mm_mul_ps(v1, v2));
        norm_v1 = _mm_add_ps(norm_v1, _mm_mul_ps(v1, v1));
        norm_v2 = _mm_add_ps(norm_v2, _mm_mul_ps(v2, v2));

        v1 = _mm_loadu_ps(pv1);
        pv1 += 4;
        v2 = _mm_loadu_ps(pv2);
        pv2 += 4;
        mul = _mm_add_ps(mul, _mm_mul_ps(v1, v2));
        norm_v1 = _mm_add_ps(norm_v1, _mm_mul_ps(v1, v1));
        norm_v2 = _mm_add_ps(norm_v2, _mm_mul_ps(v2, v2));
    }

    _mm_store_ps(MulTmpRes, mul);
    _mm_store_ps(V1TmpRes, norm_v1);
    _mm_store_ps(V2TmpRes, norm_v2);

    float mul_res = MulTmpRes[0] + MulTmpRes[1] + MulTmpRes[2] + MulTmpRes[3]; 
    float v1_res = V1TmpRes[0] + V1TmpRes[1] + V1TmpRes[2] + V1TmpRes[3];
    float v2_res = V2TmpRes[0] + V2TmpRes[1] + V2TmpRes[2] + V2TmpRes[3];

    size_t tail = dim & 15;
    const float *pBegin1 = pv1 + (dim & ~15);
    const float *pBegin2 = pv2 + (dim & ~15);
    for (size_t i = 0; i < tail; i++) {
        mul_res += pBegin1[i] * pBegin2[i];
        v1_res += pBegin1[i] * pBegin1[i];
        v2_res += pBegin2[i] * pBegin2[i];
    }

    return mul_res != 0 ? mul_res / sqrt(v1_res * v2_res) : 0;
}

export float F32CosSSEResidual(const float *pv1, const float *pv2, size_t dim) {
    return F32CosSSE(pv1, pv2, dim);
}

#endif

//------------------------------//------------------------------//------------------------------

export int32_t I8IPBF(const int8_t *pv1, const int8_t *pv2, size_t dim) {
    int32_t res = 0;
    for (size_t i = 0; i < dim; i++) {
        res += (int16_t)(pv1[i]) * pv2[i];
    }
    return res;
}

#if defined(__AVX512F__)

export int32_t I8IPAVX512(const int8_t *pv1, const int8_t *pv2, size_t dim) {
    size_t dim64 = dim >> 6;
    const int8_t *pend1 = pv1 + (dim64 << 6);

    __m512i v1, v2, msb, low7;
    __m512i sum = _mm512_set1_epi32(0);
    const __m512i highest_bit = _mm512_set1_epi8(0x80);
    while (pv1 < pend1) {
        v1 = _mm512_loadu_si512((__m512i_u *)pv1);
        pv1 += 64;
        v2 = _mm512_loadu_si512((__m512i_u *)pv2);
        pv2 += 64;

        msb = _mm512_maddubs_epi16(_mm512_and_si512(v1, highest_bit), v2);
        low7 = _mm512_maddubs_epi16(_mm512_andnot_si512(highest_bit, v1), v2);

        low7 = _mm512_madd_epi16(low7, _mm512_set1_epi16(1));
        msb = _mm512_madd_epi16(msb, _mm512_set1_epi16(1));

        sum = _mm512_add_epi32(sum, _mm512_sub_epi32(low7, msb));
    }

    // Reduce add
    return _mm512_reduce_add_epi32(sum);
}

export int32_t I8IPAVX512Residual(const int8_t *pv1, const int8_t *pv2, size_t dim) {
    return I8IPAVX512(pv1, pv2, dim) + I8IPBF(pv1 + (dim & ~63), pv2 + (dim & ~63), dim & 63);
}
#endif

#if defined(__AVX2__)

export int32_t I8IPAVX(const int8_t *pv1, const int8_t *pv2, size_t dim) {
    size_t dim32 = dim >> 5;
    const int8_t *pend1 = pv1 + (dim32 << 5);

    __m256i v1, v2, msb, low7;
    __m256i sum = _mm256_set1_ps(0);
    const __m256 highest_bit = _mm256_set1_epi8(0x80);
    while (pv1 < pend1) {
        v1 = _mm256_loadu_si256((__m256i *)pv1);
        pv1 += 32;
        v2 = _mm256_loadu_si256((__m256i *)pv2);
        pv2 += 32;

        msb = _mm256_maddubs_epi16(_mm256_and_si256(v1, highest_bit), v2);
        low7 = _mm256_maddubs_epi16(_mm256_andnot_si256(highest_bit, v1), v2);

        low7 = _mm256_madd_epi16(low7, _mm256_set1_epi16(1));
        msb = _mm256_madd_epi16(msb, _mm256_set1_epi16(1));

        sum = _mm256_add_epi32(sum, _mm256_sub_epi32(low7, msb));
    }

    // Horizontal add
    sum = _mm256_hadd_epi32(sum, sum);
    sum = _mm256_hadd_epi32(sum, sum);

    // Extract the result
    return _mm256_extract_epi32(sum, 0) + _mm256_extract_epi32(sum, 4);
}

export int32_t I8IPAVXResidual(const int8_t *pv1, const int8_t *pv2, size_t dim) {
    return I8IPAVX(pv1, pv2, dim) + I8IPBF(pv1 + (dim & ~31), pv2 + (dim & ~31), dim & 31);
}

#endif

#if defined(__SSE2__)

export int32_t I8IPSSE(const int8_t *pv1, const int8_t *pv2, size_t dim) {
    size_t dim16 = dim >> 4;
    const int8_t *pend1 = pv1 + (dim16 << 4);

    __m128i v1, v2, msb, low7;
    __m128i sum = _mm_set1_ps(0);
    const __m128 highest_bit = _mm_set1_epi8(0x80);
    while (pv1 < pend1) {
        v1 = _mm_loadu_si128((__m128i *)pv1);
        pv1 += 16;
        v2 = _mm_loadu_si128((__m128i *)pv2);
        pv2 += 16;

        msb = _mm_maddubs_epi16(_mm_and_si128(v1, highest_bit), v2);
        low7 = _mm_maddubs_epi16(_mm_andnot_si128(highest_bit, v1), v2);

        low7 = _mm_madd_epi16(low7, _mm_set1_epi16(1));
        msb = _mm_madd_epi16(msb, _mm_set1_epi16(1));

        sum = _mm_add_epi32(sum, _mm_sub_epi32(low7, msb));
    }

    // Horizontal add
    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);

    // Extract the result
    return _mm_extract_epi32(sum, 0);
}

export int32_t I8IPSSEResidual(const int8_t *pv1, const int8_t *pv2, size_t dim) {
    return I8IPSSE(pv1, pv2, dim) + I8IPBF(pv1 + (dim & ~15), pv2 + (dim & ~15), dim & 15);
}

#endif

//------------------------------//------------------------------//------------------------------

export float F32L2BF(const float *pv1, const float *pv2, size_t dim) {
    float res = 0;
    for (size_t i = 0; i < dim; i++) {
        float t = pv1[i] - pv2[i];
        res += t * t;
    }
    return res;
}

#if defined(__AVX512F__)

export float F32L2AVX512(const float *pv1, const float *pv2, size_t dim) {
    alignas(64) float TmpRes[16];
    size_t dim16 = dim >> 4;

    const float *pEnd1 = pv1 + (dim16 << 4);

    __m512 diff, v1, v2;
    __m512 sum = _mm512_set1_ps(0);

    while (pv1 < pEnd1) {
        v1 = _mm512_loadu_ps(pv1);
        pv1 += 16;
        v2 = _mm512_loadu_ps(pv2);
        pv2 += 16;
        diff = _mm512_sub_ps(v1, v2);
        // sum = _mm512_fmadd_ps(diff, diff, sum);
        sum = _mm512_add_ps(sum, _mm512_mul_ps(diff, diff));
    }

    _mm512_store_ps(TmpRes, sum);
    float res = TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3] + TmpRes[4] + TmpRes[5] + TmpRes[6] + TmpRes[7] + TmpRes[8] + TmpRes[9] + TmpRes[10] +
                TmpRes[11] + TmpRes[12] + TmpRes[13] + TmpRes[14] + TmpRes[15];

    return (res);
}

export float F32L2AVX512Residual(const float *pv1, const float *pv2, size_t dim) {
    return F32L2AVX512(pv1, pv2, dim) + F32L2BF(pv1 + (dim & ~15), pv2 + (dim & ~15), dim & 15);
}

#endif

#if defined(__AVX2__)

export float F32L2AVX(const float *pv1, const float *pv2, size_t dim) {
    alignas(32) float TmpRes[8];
    size_t dim16 = dim >> 4;

    const float *pEnd1 = pv1 + (dim16 << 4);

    __m256 diff, v1, v2;
    __m256 sum = _mm256_set1_ps(0);

    while (pv1 < pEnd1) {
        v1 = _mm256_loadu_ps(pv1);
        pv1 += 8;
        v2 = _mm256_loadu_ps(pv2);
        pv2 += 8;
        diff = _mm256_sub_ps(v1, v2);
        sum = _mm256_add_ps(sum, _mm256_mul_ps(diff, diff));

        v1 = _mm256_loadu_ps(pv1);
        pv1 += 8;
        v2 = _mm256_loadu_ps(pv2);
        pv2 += 8;
        diff = _mm256_sub_ps(v1, v2);
        sum = _mm256_add_ps(sum, _mm256_mul_ps(diff, diff));
    }

    _mm256_store_ps(TmpRes, sum);
    return TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3] + TmpRes[4] + TmpRes[5] + TmpRes[6] + TmpRes[7];
}

export float F32L2AVXResidual(const float *pv1, const float *pv2, size_t dim) {
    return F32L2AVX(pv1, pv2, dim) + F32L2BF(pv1 + (dim & ~15), pv2 + (dim & ~15), dim & 15);
}

#endif

#if defined(__SSE2__)

export float F32L2SSE(const float *pv1, const float *pv2, size_t dim) {
    alignas(16) float TmpRes[4];
    size_t dim16 = dim >> 4;

    const float *pEnd1 = pv1 + (dim16 << 4);

    __m128 diff, v1, v2;
    __m128 sum = _mm_set1_ps(0);

    while (pv1 < pEnd1) {
        v1 = _mm_loadu_ps(pv1);
        pv1 += 4;
        v2 = _mm_loadu_ps(pv2);
        pv2 += 4;
        diff = _mm_sub_ps(v1, v2);
        sum = _mm_add_ps(sum, _mm_mul_ps(diff, diff));

        v1 = _mm_loadu_ps(pv1);
        pv1 += 4;
        v2 = _mm_loadu_ps(pv2);
        pv2 += 4;
        diff = _mm_sub_ps(v1, v2);
        sum = _mm_add_ps(sum, _mm_mul_ps(diff, diff));

        v1 = _mm_loadu_ps(pv1);
        pv1 += 4;
        v2 = _mm_loadu_ps(pv2);
        pv2 += 4;
        diff = _mm_sub_ps(v1, v2);
        sum = _mm_add_ps(sum, _mm_mul_ps(diff, diff));

        v1 = _mm_loadu_ps(pv1);
        pv1 += 4;
        v2 = _mm_loadu_ps(pv2);
        pv2 += 4;
        diff = _mm_sub_ps(v1, v2);
        sum = _mm_add_ps(sum, _mm_mul_ps(diff, diff));
    }

    _mm_store_ps(TmpRes, sum);
    return TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3];
}

export float F32L2SSEResidual(const float *pv1, const float *pv2, size_t dim) {
    return F32L2SSE(pv1, pv2, dim) + F32L2BF(pv1 + (dim & ~15), pv2 + (dim & ~15), dim & 15);
}

#endif

//------------------------------//------------------------------//------------------------------

export float F32IPBF(const float *pv1, const float *pv2, size_t dim) {
    float res = 0;
    for (size_t i = 0; i < dim; i++) {
        res += pv1[i] * pv2[i];
    }
    return res;
}

#if defined(__AVX512F__)

export float F32IPAVX512(const float *pVect1, const float *pVect2, SizeT qty) {
    alignas(64) float TmpRes[16];

    size_t qty16 = qty / 16;

    const float *pEnd1 = pVect1 + 16 * qty16;

    __m512 sum512 = _mm512_set1_ps(0);

    while (pVect1 < pEnd1) {
        //_mm_prefetch((char*)(pVect2 + 16), _MM_HINT_T0);

        __m512 v1 = _mm512_loadu_ps(pVect1);
        pVect1 += 16;
        __m512 v2 = _mm512_loadu_ps(pVect2);
        pVect2 += 16;
        sum512 = _mm512_add_ps(sum512, _mm512_mul_ps(v1, v2));
    }

    _mm512_store_ps(TmpRes, sum512);
    float sum = TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3] + TmpRes[4] + TmpRes[5] + TmpRes[6] + TmpRes[7] + TmpRes[8] + TmpRes[9] + TmpRes[10] +
                TmpRes[11] + TmpRes[12] + TmpRes[13] + TmpRes[14] + TmpRes[15];

    return sum;
}

export float F32IPAVX512Residual(const float *pVect1, const float *pVect2, SizeT qty) {
    return F32IPAVX512(pVect1, pVect2, qty) + F32IPBF(pVect1 + (qty & ~15), pVect2 + (qty & ~15), qty & 15);
}

#endif

#if defined(__AVX2__)

export float F32IPAVX(const float *pVect1, const float *pVect2, SizeT qty) {
    alignas(32) float TmpRes[8];

    size_t qty16 = qty / 16;

    const float *pEnd1 = pVect1 + 16 * qty16;

    __m256 sum256 = _mm256_set1_ps(0);

    while (pVect1 < pEnd1) {
        //_mm_prefetch((char*)(pVect2 + 16), _MM_HINT_T0);

        __m256 v1 = _mm256_loadu_ps(pVect1);
        pVect1 += 8;
        __m256 v2 = _mm256_loadu_ps(pVect2);
        pVect2 += 8;
        sum256 = _mm256_add_ps(sum256, _mm256_mul_ps(v1, v2));

        v1 = _mm256_loadu_ps(pVect1);
        pVect1 += 8;
        v2 = _mm256_loadu_ps(pVect2);
        pVect2 += 8;
        sum256 = _mm256_add_ps(sum256, _mm256_mul_ps(v1, v2));
    }

    _mm256_store_ps(TmpRes, sum256);
    float sum = TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3] + TmpRes[4] + TmpRes[5] + TmpRes[6] + TmpRes[7];

    return sum;
}

export float F32IPAVXResidual(const float *pVect1, const float *pVect2, SizeT qty) {
    return F32IPAVX(pVect1, pVect2, qty) + F32IPBF(pVect1 + (qty & ~15), pVect2 + (qty & ~15), qty & 15);
}

#endif

#if defined(__SSE2__)

export float F32IPSSE(const float *pVect1, const float *pVect2, SizeT qty) {
    alignas(16) float TmpRes[4];

    size_t qty16 = qty / 16;

    const float *pEnd1 = pVect1 + 16 * qty16;

    __m128 sum128 = _mm_set1_ps(0);

    while (pVect1 < pEnd1) {
        //_mm_prefetch((char*)(pVect2 + 16), _MM_HINT_T0);

        __m128 v1 = _mm_loadu_ps(pVect1);
        pVect1 += 4;
        __m128 v2 = _mm_loadu_ps(pVect2);
        pVect2 += 4;
        sum128 = _mm_add_ps(sum128, _mm_mul_ps(v1, v2));

        v1 = _mm_loadu_ps(pVect1);
        pVect1 += 4;
        v2 = _mm_loadu_ps(pVect2);
        pVect2 += 4;
        sum128 = _mm_add_ps(sum128, _mm_mul_ps(v1, v2));

        v1 = _mm_loadu_ps(pVect1);
        pVect1 += 4;
        v2 = _mm_loadu_ps(pVect2);
        pVect2 += 4;
        sum128 = _mm_add_ps(sum128, _mm_mul_ps(v1, v2));

        v1 = _mm_loadu_ps(pVect1);
        pVect1 += 4;
        v2 = _mm_loadu_ps(pVect2);
        pVect2 += 4;
        sum128 = _mm_add_ps(sum128, _mm_mul_ps(v1, v2));
    }

    _mm_store_ps(TmpRes, sum128);
    float sum = TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3];

    return sum;
}

export float F32IPSSEResidual(const float *pVect1, const float *pVect2, SizeT qty) {
    return F32IPSSE(pVect1, pVect2, qty) + F32IPBF(pVect1 + (qty & ~15), pVect2 + (qty & ~15), qty & 15);
}

#endif

} // namespace infinity
