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
export module emvb_simd_funcs;
import stl;
import simd_common_tools;

namespace infinity {

#if defined(__AVX2__)

// https://stackoverflow.com/questions/36932240/avx2-what-is-the-most-efficient-way-to-pack-left-based-on-a-mask/36951611#36951611
// Uses 64bit pdep / pext to save a step in unpacking.
export inline __m256i compress256i(__m256i src, u32 mask /* from movmskps */) {
    u64 expanded_mask = _pdep_u64(mask, 0x0101010101010101); // unpack each bit to a byte
    expanded_mask *= 0xFF;                                   // mask |= mask<<1 | mask<<2 | ... | mask<<7;
    // ABC... -> AAAAAAAABBBBBBBBCCCCCCCC...: replicate each bit to fill its byte

    const u64 identity_indices = 0x0706050403020100; // the identity shuffle for vpermps, packed to one index per byte
    u64 wanted_indices = _pext_u64(identity_indices, expanded_mask);

    __m128i bytevec = _mm_cvtsi64_si128(wanted_indices);
    __m256i shufmask = _mm256_cvtepu8_epi32(bytevec);

    return _mm256_permutevar8x32_epi32(src, shufmask);
}

// output_id_ptr should have extra 8 bytes for storeu
// input_scores must be aligned to 32 bytes
export u32 *filter_scores_output_ids_avx2(u32 *output_id_ptr, const f32 threshold, const f32 *input_scores, const u32 scores_len) {
    __m256i ids = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
    const __m256i SHIFT = _mm256_set1_epi32(8);
    const __m256 broad_threshold = _mm256_set1_ps(threshold);
    // const u32 avx_cycle_lenth = (doclen / 8) * 8;
    const u32 avx_cycle_lenth = scores_len & (~(7u));
    u32 j = 0;
    for (; j < avx_cycle_lenth; j += 8) {
        __m256 current_values = _mm256_load_ps(input_scores + j);
        const u32 mask = _mm256_movemask_ps(_mm256_cmp_ps(current_values, broad_threshold, _CMP_GT_OS));
        _mm256_storeu_si256((__m256i *)(output_id_ptr), compress256i(ids, mask));
        output_id_ptr += _mm_popcnt_u32(mask);
        ids = _mm256_add_epi32(ids, SHIFT);
    }
    for (; j < scores_len; ++j) {
        *output_id_ptr = j;
        output_id_ptr += static_cast<int>(input_scores[j] > threshold);
    }
    return output_id_ptr;
}

export template <u32 FIXED_QUERY_TOKEN_NUM, u32 BEGIN_OFFSET>
inline f32 GetMaxSim32Width(const f32 *centroid_distances, const u32 doclen) {
    static_assert(BEGIN_OFFSET % 32 == 0);
    static_assert(FIXED_QUERY_TOKEN_NUM > 0 && FIXED_QUERY_TOKEN_NUM % 32 == 0);
    static_assert(BEGIN_OFFSET < FIXED_QUERY_TOKEN_NUM);
    centroid_distances += BEGIN_OFFSET;
    __m256 max0 = _mm256_load_ps(centroid_distances);
    __m256 max1 = _mm256_load_ps(centroid_distances + 8);
    __m256 max2 = _mm256_load_ps(centroid_distances + 16);
    __m256 max3 = _mm256_load_ps(centroid_distances + 24);
    for (u32 i = 1; i < doclen; ++i) {
        centroid_distances += FIXED_QUERY_TOKEN_NUM;
        __m256 current0 = _mm256_load_ps(centroid_distances);
        __m256 current1 = _mm256_load_ps(centroid_distances + 8);
        __m256 current2 = _mm256_load_ps(centroid_distances + 16);
        __m256 current3 = _mm256_load_ps(centroid_distances + 24);
        auto cmp0 = _mm256_cmp_ps(current0, max0, _CMP_GT_OS);
        auto cmp1 = _mm256_cmp_ps(current1, max1, _CMP_GT_OS);
        auto cmp2 = _mm256_cmp_ps(current2, max2, _CMP_GT_OS);
        auto cmp3 = _mm256_cmp_ps(current3, max3, _CMP_GT_OS);
        max0 = _mm256_blendv_ps(max0, current0, cmp0);
        max1 = _mm256_blendv_ps(max1, current1, cmp1);
        max2 = _mm256_blendv_ps(max2, current2, cmp2);
        max3 = _mm256_blendv_ps(max3, current3, cmp3);
    }
    __m256 half_sum_1 = _mm256_add_ps(max0, max1);
    __m256 half_sum_2 = _mm256_add_ps(max2, max3);
    __m256 half_sum = _mm256_add_ps(half_sum_1, half_sum_2);
    return hsum256_ps_avx(half_sum);
}

#endif

// output_id_ptr should have extra 8 bytes for storeu
// input_scores must be aligned to 32 bytes
export u32 *filter_scores_output_ids_common(u32 *output_id_ptr, const f32 threshold, const f32 *input_scores, const u32 scores_len) {
    for (u32 j = 0; j < scores_len; ++j) {
        *output_id_ptr = j;
        output_id_ptr += static_cast<int>(input_scores[j] > threshold);
    }
    return output_id_ptr;
}

} // namespace infinity
