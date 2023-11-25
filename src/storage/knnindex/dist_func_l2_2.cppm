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

    for (size_t i = 0; i < dim; i += 32) {
        __m256i a_vec = _mm256_loadu_si256((__m256i_u *)(v1 + i));
        __m256i b_vec = _mm256_loadu_si256((__m256i_u *)(v2 + i));

        // Extend u8 vectors to u16
        __m256i a_vec_lo = _mm256_unpacklo_epi8(a_vec, _mm256_setzero_si256());
        __m256i a_vec_hi = _mm256_unpackhi_epi8(a_vec, _mm256_setzero_si256());
        __m256i b_vec_lo = _mm256_unpacklo_epi8(b_vec, _mm256_setzero_si256());
        __m256i b_vec_hi = _mm256_unpackhi_epi8(b_vec, _mm256_setzero_si256());

        // Multiply and add
        __m256i prod_lo = _mm256_madd_epi16(a_vec_lo, b_vec_lo);
        __m256i prod_hi = _mm256_madd_epi16(a_vec_hi, b_vec_hi);

        // Accumulate sum
        sum = _mm256_add_epi32(sum, prod_lo);
        sum = _mm256_add_epi32(sum, prod_hi);
    }

    // Horizontal add
    sum = _mm256_hadd_epi32(sum, sum);
    sum = _mm256_hadd_epi32(sum, sum);

    // Extract the result
    uint32_t result = _mm256_extract_epi32(sum, 0) + _mm256_extract_epi32(sum, 4);

    return result;
}

export float L2Sqr3(const LVQ8 &v1, const LVQ8 &v2, SizeT dim) {
    uint32_t c1c2_ip = U8IPSIMD16ExtAVX(v1.GetCompressVec(), v2.GetCompressVec(), dim);
    auto [scale1, bias1] = v1.GetScalar();
    auto [scale2, bias2] = v2.GetScalar();
    auto beta = bias1 - bias2;
    auto [norm1_scale_1, norm2sq_scalesq_1] = v1.GetConstant();
    auto [norm1_scale_2, norm2sq_scalesq_2] = v2.GetConstant();
    double dist = norm2sq_scalesq_1 + norm2sq_scalesq_2 + beta * beta * dim - 2 * scale1 * scale2 * c1c2_ip + 2 * beta * norm1_scale_1 -
                  2 * beta * norm1_scale_2;

    return dist;
}

export class FloatLVQ8L2Space : public SpaceBase<float, LVQ8> {
public:
    FloatLVQ8L2Space(SizeT dim) {
        // assert(dim <= 256);
        // assert(dim % 16 == 0);
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