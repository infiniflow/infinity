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

#include "simd_common_intrin_include.h"
#include <cassert>
export module infinity_core:batch_bm25_simd_funcs;
import :stl;
import :simd_common_tools;

namespace infinity {

#if defined(__AVX2__)

export void BatchBM25AVX2(const u32 batch_len,
                          const u32 children_num,
                          const f32 *f1_ptr,
                          const f32 *f2_ptr,
                          const f32 *bm25_common_score_ptr,
                          const u32 *tf_ptr,
                          const u32 *doc_len_ptr,
                          u32 *match_cnt_ptr,
                          f32 *bm25_score_sum_ptr) {
    assert(batch_len > 0 && batch_len % 8 == 0);
    assert(children_num > 0);
    for (u32 batch_start = 0; batch_start < batch_len; batch_start += 8) {
        // init match_cnt and bm25_score_sum
        __m256i match_cnt = _mm256_setzero_si256();
        __m256 bm25_score_sum = _mm256_setzero_ps();
        for (u32 child_id = 0; child_id < children_num; ++child_id) {
            // load tf
            __m256i tf = _mm256_load_si256((__m256i *)(tf_ptr + child_id * batch_len + batch_start));
            // check if tf is positive, return -1 if tf is positive, 0 otherwise
            __m256i tf_positive_mask = _mm256_cmpgt_epi32(tf, _mm256_setzero_si256());
            // update match_cnt
            match_cnt = _mm256_sub_epi32(match_cnt, tf_positive_mask);
            // load doc_len
            __m256i doc_len = _mm256_load_si256((__m256i *)(doc_len_ptr + child_id * batch_len + batch_start));
            // load f1, f2, bm25_common_score
            __m256 f1 = _mm256_set1_ps(f1_ptr[child_id]);
            __m256 f2 = _mm256_set1_ps(f2_ptr[child_id]);
            __m256 bm25_common_score = _mm256_set1_ps(bm25_common_score_ptr[child_id]);
            // p = f1 + f2 * doc_len
            __m256 p = _mm256_add_ps(f1, _mm256_mul_ps(f2, _mm256_cvtepi32_ps(doc_len)));
            // bm25_score = bm25_common_score * tf / (tf + p)
            __m256 tf_f = _mm256_cvtepi32_ps(tf);
            __m256 bm25_score = _mm256_div_ps(_mm256_mul_ps(bm25_common_score, tf_f), _mm256_add_ps(tf_f, p));
            // update bm25_score_sum
            bm25_score_sum = _mm256_add_ps(bm25_score_sum, bm25_score);
        }
        // store match_cnt and bm25_score_sum to match_cnt_ptr and bm25_score_sum_ptr
        _mm256_storeu_si256((__m256i *)(match_cnt_ptr + batch_start), match_cnt);
        _mm256_storeu_ps(bm25_score_sum_ptr + batch_start, bm25_score_sum);
    }
}

#endif

export void BatchBM25Simple(const u32 batch_len,
                            const u32 children_num,
                            const f32 *f1_ptr,
                            const f32 *f2_ptr,
                            const f32 *bm25_common_score_ptr,
                            const u32 *tf_ptr,
                            const u32 *doc_len_ptr,
                            u32 *match_cnt_ptr,
                            f32 *bm25_score_sum_ptr) {
    for (u32 i = 0; i < batch_len; ++i) {
        u32 match_cnt = 0;
        f32 bm25_score_sum = 0.0f;
        for (u32 j = 0; j < children_num; ++j) {
            const auto tf = tf_ptr[j * batch_len + i];
            if (tf == 0) {
                continue;
            }
            const auto doc_len = doc_len_ptr[j * batch_len + i];
            const auto f1 = f1_ptr[j];
            const auto f2 = f2_ptr[j];
            const auto bm25_common_score = bm25_common_score_ptr[j];
            const auto p = f1 + f2 * doc_len;
            const auto bm25_score = bm25_common_score * tf / (tf + p);
            ++match_cnt;
            bm25_score_sum += bm25_score;
        }
        match_cnt_ptr[i] = match_cnt;
        bm25_score_sum_ptr[i] = bm25_score_sum;
    }
}

} // namespace infinity
