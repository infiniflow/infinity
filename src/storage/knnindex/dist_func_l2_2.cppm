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

import stl;
import dist_func;
import lvq_store;

export module dist_func_l2_2;

namespace infinity {

using LVQ8 = LVQStore<float, uint8_t>::RtnType;

export uint32_t U8IPSIMD16ExtAVX(const uint8_t *v1, const uint8_t *v2, size_t dim) {
    __m256i sum = _mm256_setzero_si256();
    size_t qty16 = dim >> 4;

    for (size_t i = 0; i < qty16; i++) {
        __m256i vec1 = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i *)(v1 + i * 16)));
        __m256i vec2 = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i *)(v2 + i * 16)));
        __m256i prod = _mm256_madd_epi16(vec1, vec2);
        sum = _mm256_add_epi32(sum, prod);
    }

    __m128i sum_low = _mm256_extracti128_si256(sum, 0);
    __m128i sum_high = _mm256_extracti128_si256(sum, 1);
    sum_low = _mm_add_epi32(sum_low, sum_high);

    sum_low = _mm_add_epi32(sum_low, _mm_srli_si128(sum_low, 8));
    sum_low = _mm_add_epi32(sum_low, _mm_srli_si128(sum_low, 4));

    return _mm_cvtsi128_si32(sum_low);
}

// export uint32_t U8L2SIMD16ExtAVX(const uint8_t *v, size_t dim) {
//     __m256i sum = _mm256_setzero_si256();
//     size_t qty16 = dim >> 4;

//     for (size_t i = 0; i < qty16; i++) {
//         __m256i vec = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i *)(v + i * 16)));
//         __m256i square = _mm256_madd_epi16(vec, vec);
//         sum = _mm256_add_epi32(sum, square);
//     }

//     __m128i sum_low = _mm256_extracti128_si256(sum, 0);
//     __m128i sum_high = _mm256_extracti128_si256(sum, 1);
//     sum_low = _mm_add_epi32(sum_low, sum_high);

//     sum_low = _mm_add_epi32(sum_low, _mm_srli_si128(sum_low, 8));
//     sum_low = _mm_add_epi32(sum_low, _mm_srli_si128(sum_low, 4));

//     return _mm_cvtsi128_si32(sum_low);
// }

// export uint16_t U8L1SIMD16ExtAVX(const uint8_t *v, size_t dim) {
//     __m256i sum = _mm256_setzero_si256();
//     size_t qty16 = dim >> 4;

//     for (size_t i = 0; i < qty16; i++) {
//         __m256i vec = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i *)(v + i * 16)));
//         sum = _mm256_add_epi16(sum, vec);
//     }

//     __m128i sum_low = _mm256_extracti128_si256(sum, 0);
//     __m128i sum_high = _mm256_extracti128_si256(sum, 1);
//     sum_low = _mm_add_epi16(sum_low, sum_high);

//     sum_low = _mm_add_epi16(sum_low, _mm_srli_si128(sum_low, 8));
//     sum_low = _mm_add_epi16(sum_low, _mm_srli_si128(sum_low, 4));
//     sum_low = _mm_add_epi16(sum_low, _mm_srli_si128(sum_low, 2));

//     return _mm_extract_epi16(sum_low, 0);
// }

export float L2Sqr3(const LVQ8 &v1, const LVQ8 &v2, SizeT dim) {
    double alpha1 = v1.alpha_;
    double alpha2 = v2.alpha_;
    double beta = v1.lower_ - v2.lower_;
    const uint8_t *c1 = v1.c_;
    const uint8_t *c2 = v2.c_;

    uint32_t c1c2_ip = U8IPSIMD16ExtAVX(c1, c2, dim);
    double dist = alpha1 * alpha1 * v1.norm2sq_ + alpha2 * alpha2 * v2.norm2sq_ + beta * beta * dim - 2 * alpha1 * alpha2 * c1c2_ip +
                  2 * alpha1 * beta * v1.norm1_ - 2 * alpha2 * beta * v2.norm1_;

    return dist;
}

export class FloatLVQ8L2Space : public SpaceBase<float, LVQ8> {
public:
    FloatLVQ8L2Space(SizeT dim) {
        assert(dim <= 256);
        assert(dim % 16 == 0);
        this->fstdistfunc_ = L2Sqr3;
    }
};

//------------------------------//------------------------------//------------------------------

static float FloatL2SqrSIMD16ExtAVX(const float *const &pVect1, const float *const &pVect2, size_t qty) {
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
    FloatL2Space(SizeT dim) { this->fstdistfunc_ = FloatL2SqrSIMD16ExtAVX; }
};

} // namespace infinity