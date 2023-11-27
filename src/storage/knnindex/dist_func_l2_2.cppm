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

#include "header.h"
#include <cassert>
#include <iostream>

import stl;
import dist_func;
import lvq_store;

export module dist_func_l2_2;

namespace infinity {

using LVQ8 = LVQStore<float, int8_t>::RtnType;

template <typename T>
void log_m256(const __m256i &value) {
    const size_t n = sizeof(__m256i) / sizeof(T);
    T buffer[n];
    _mm256_storeu_si256((__m256i_u *)buffer, value);
    std::cout << "[";
    for (size_t i = 0; i < n; i++) {
        std::cout << (int)buffer[i];
        if (i != n - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

#if defined(USE_AVX512)
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
#endif

export int32_t I8IPAVX(const int8_t *pv1, const int8_t *pv2, size_t dim) {
    size_t dim32 = dim >> 5;
    const int8_t *pend1 = pv1 + (dim32 << 5);

    __m256i v1, v2, msb, low7;
    __m256i sum = _mm256_set1_ps(0);
    const __m256 highest_bit = _mm256_set1_epi8(0x80);
    while (pv1 < pend1) {
        v1 = _mm256_loadu_si256((__m256i_u *)pv1);
        pv1 += 32;
        v2 = _mm256_loadu_si256((__m256i_u *)pv2);
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

export float L2Sqr3(LVQ8 v1, LVQ8 v2, SizeT dim) {
    int32_t c1c2_ip = I8IPAVX(v1.GetCompressVec(), v2.GetCompressVec(), dim);
    auto [scale1, bias1] = v1.GetScalar();
    auto [scale2, bias2] = v2.GetScalar();
    auto beta = bias1 - bias2;
    auto [norm1_scale_1, norm2sq_scalesq_1] = v1.GetConstant();
    auto [norm1_scale_2, norm2sq_scalesq_2] = v2.GetConstant();
    double dist = norm2sq_scalesq_1 + norm2sq_scalesq_2 + beta * beta * dim - 2 * scale1 * scale2 * c1c2_ip + 2 * beta * norm1_scale_1 -
                  2 * beta * norm1_scale_2;

    return dist;
}

#if defined(USE_AVX512)
export float L2Sqr4(LVQ8 v1, LVQ8 v2, SizeT dim) {
    int32_t c1c2_ip = I8IPAVX512(v1.GetCompressVec(), v2.GetCompressVec(), dim);
    auto [scale1, bias1] = v1.GetScalar();
    auto [scale2, bias2] = v2.GetScalar();
    auto beta = bias1 - bias2;
    auto [norm1_scale_1, norm2sq_scalesq_1] = v1.GetConstant();
    auto [norm1_scale_2, norm2sq_scalesq_2] = v2.GetConstant();
    double dist = norm2sq_scalesq_1 + norm2sq_scalesq_2 + beta * beta * dim - 2 * scale1 * scale2 * c1c2_ip + 2 * beta * norm1_scale_1 -
                  2 * beta * norm1_scale_2;

    return dist;
}
#endif

export class FloatLVQ8L2Space : public SpaceBase<float, LVQ8> {
public:
    FloatLVQ8L2Space(SizeT dim) {
        // assert(dim <= 256);

#if defined(USE_AVX512)
        assert(dim % 64 == 0);
        this->fstdistfunc_ = L2Sqr4;
#else
        assert(dim % 32 == 0);
        this->fstdistfunc_ = L2Sqr3;
#endif
    }
};

//------------------------------//------------------------------//------------------------------

#if defined(USE_AVX512)

// Favor using AVX512 if available.
static float FloatL2AVX512(const float *pVect1, const float *pVect2, size_t qty) {
    float PORTABLE_ALIGN64 TmpRes[16];
    size_t qty16 = qty >> 4;

    const float *pEnd1 = pVect1 + (qty16 << 4);

    __m512 diff, v1, v2;
    __m512 sum = _mm512_set1_ps(0);

    while (pVect1 < pEnd1) {
        v1 = _mm512_loadu_ps(pVect1);FloatL2AVX
        pVect1 += 16;
        v2 = _mm512_loadu_ps(pVect2);
        pVect2 += 16;
        diff = _mm512_sub_ps(v1, v2);
        // sum = _mm512_fmadd_ps(diff, diff, sum);
        sum = _mm512_add_ps(sum, _mm512_mul_ps(diff, diff));
    }

    _mm512_store_ps(TmpRes, sum);
    float res = TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3] + TmpRes[4] + TmpRes[5] + TmpRes[6] + TmpRes[7] + TmpRes[8] + TmpRes[9] + TmpRes[10] +
                TmpRes[11] + TmpRes[12] + TmpRes[13] + TmpRes[14] + TmpRes[15];

    return (res);
}
#endif

static float FloatL2AVX(const float *pVect1, const float *pVect2, size_t qty) {
    float PORTABLE_ALIGN32 TmpRes[8];
    size_t qty16 = qty >> 4;

    const float *pEnd1 = pVect1 + (qty16 << 4);
    const float *pv1 = pVect1, *pv2 = pVect2;

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

export class FloatL2Space : public SpaceBase<float, const float *> {
public:
    FloatL2Space(SizeT dim) {
#if defined(USE_AVX512)
        assert(dim % 16 == 0);
        this->fstdistfunc_ = FloatL2AVX512;
#else
        assert(dim % 16 == 0);
        this->fstdistfunc_ = FloatL2AVX;
#endif
    }
};

} // namespace infinity