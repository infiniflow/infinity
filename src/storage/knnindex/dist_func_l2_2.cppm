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

static float FloatL2SqrSIMD16ExtAVX(const float *pVect1, const float *pVect2, size_t qty) {
    float PORTABLE_ALIGN32 TmpRes[8];
    size_t qty16 = qty >> 4;

    const float *pEnd1 = pVect1 + (qty16 << 4);

    __m256 diff, v1, v2;
    __m256 sum = _mm256_set1_ps(0);

    while (pVect1 < pEnd1) {
        v1 = _mm256_loadu_ps(pVect1);
        pVect1 += 8;
        v2 = _mm256_loadu_ps(pVect2);
        pVect2 += 8;
        diff = _mm256_sub_ps(v1, v2);
        sum = _mm256_add_ps(sum, _mm256_mul_ps(diff, diff));

        v1 = _mm256_loadu_ps(pVect1);
        pVect1 += 8;
        v2 = _mm256_loadu_ps(pVect2);
        pVect2 += 8;
        diff = _mm256_sub_ps(v1, v2);
        sum = _mm256_add_ps(sum, _mm256_mul_ps(diff, diff));
    }

    _mm256_store_ps(TmpRes, sum);
    return TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3] + TmpRes[4] + TmpRes[5] + TmpRes[6] + TmpRes[7];
}

export uint16_t U8IPSIMD16ExtAVX(const uint8_t *v1, const uint8_t *v2, size_t dim) {
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

static float L2Sqr(const float *v1, const LVQ8 v2, SizeT dim) {
    float v3[dim];
    v2.DecompressForTest(v3);
    return FloatL2SqrSIMD16ExtAVX(v1, v3, dim);
}

static float L2Sqr3(const LVQ8 v1, const LVQ8 v2, SizeT dim) {
    // auto [lower1, upper1] = v1.data_store_->GetLowerUpper(v1.vec_idx_);
    // auto [lower2, upper2] = v2.data_store_->GetLowerUpper(v2.vec_idx_);
    // float *mean = v1.data_store_->GetMean();
    return 0;
}

static float L2Sqr2(const LVQ8 v1, const LVQ8 v2, SizeT dim) {
    float v3[dim];
    float v4[dim];
    v1.DecompressForTest(v3);
    v2.DecompressForTest(v4);
    return FloatL2SqrSIMD16ExtAVX(v3, v4, dim);
}

export class FloatLVQ8L2Space : public SpaceBase<float, const LVQ8> {
public:
    FloatLVQ8L2Space(SizeT dim) {
        assert(dim % 16 == 0);
        this->fstdistfunc_ = L2Sqr;
        this->fstdistfunc2_ = L2Sqr2;
    }
};
} // namespace infinity