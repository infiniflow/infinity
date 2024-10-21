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

#include <cstdlib>
#include "common/simd/simd_common_intrin_include.h"

#include "gtest/gtest.h"
import base_test;
import stl;
import emvb_simd_funcs;
import simd_common_tools;

using namespace infinity;

class SIMDTest : public BaseTest {};

TEST_F(SIMDTest, testsum256) {
    constexpr u32 test_sum256_loop = 20;

    std::array<f32, 8> random_f32 = {};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<f32> dis(-1024.0f, 1024.0f);
    for (u32 i = 0; i < test_sum256_loop; ++i) {
        for (u32 j = 0; j < 8; ++j) {
            random_f32[j] = dis(gen);
        }
        __m256 load_v = _mm256_loadu_ps(random_f32.data());
        const f32 sum_f32 = hsum256_ps_avx(load_v);
        const f32 expect_sum = std::reduce(random_f32.begin(), random_f32.end());
        const f32 abs_sum = std::accumulate(random_f32.begin(), random_f32.end(), 0.0f, [](f32 acc, f32 v) { return acc + std::abs(v); });
        const f32 diff_percent = std::abs(sum_f32 - expect_sum) / abs_sum;
//        std::cerr << "sum_f32: " << sum_f32 << " expect_sum: " << expect_sum << " abs_sum: " << abs_sum << " diff_percent: " << diff_percent
//                  << std::endl;
        EXPECT_LE(diff_percent, 1e-6);
    }
}

TEST_F(SIMDTest, testloadstore) {
    auto ptr = std::aligned_alloc(32, 32 * sizeof(u32));
    EXPECT_NE(ptr, nullptr);
    std::unique_ptr<u32[], decltype([](u32 *p) { std::free(p); })> test_output(static_cast<u32 *>(ptr));
    auto misaligned_addr = test_output.get() + 1;
    __m256i ids = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
    _mm256_storeu_si256((__m256i *)misaligned_addr, ids);
    for (u32 i = 0; i < 8; ++i) {
        EXPECT_EQ(misaligned_addr[i], i);
    }
}

TEST_F(SIMDTest, testmask) {
    f32 f_array[8] = {};
    // set 1.0f at index 0, 3, 4
    f_array[0] = 1.0f;
    f_array[3] = 1.0f;
    f_array[4] = 1.0f;
    f32 threshold = 0.5f;
    __m256 broad_th = _mm256_set1_ps(threshold);
    __m256 current_values = _mm256_loadu_ps(f_array);
    const u32 mask = _mm256_movemask_ps(_mm256_cmp_ps(current_values, broad_th, _CMP_GT_OS));
    EXPECT_EQ(mask, 0b11001);
}

TEST_F(SIMDTest, testmax) {
    f32 f_array_1[8] = {};
    f_array_1[0] = 1.0f;
    f_array_1[3] = 1.0f;
    f_array_1[4] = 1.0f;
    f32 f_array_2[8] = {};
    f_array_2[2] = 1.0f;
    f_array_2[6] = 1.0f;
    __m256 value1 = _mm256_loadu_ps(f_array_1);
    __m256 value2 = _mm256_loadu_ps(f_array_2);
    auto cmp_v = _mm256_cmp_ps(value1, value2, _CMP_GT_OS);
    auto max_v = _mm256_blendv_ps(value2, value1, cmp_v);
    f32 f_array_3[8] = {};
    _mm256_storeu_ps(f_array_3, max_v);
    for (u32 i = 0; i < 8; ++i) {
        if (i == 1 || i == 5 || i == 7) {
            EXPECT_FLOAT_EQ(f_array_3[i], 0.0f);
        } else {
            EXPECT_FLOAT_EQ(f_array_3[i], 1.0f);
        }
    }
}

TEST_F(SIMDTest, testleftpack) {
#if defined(__AVX2__)
    auto ptr = std::aligned_alloc(32, 128 * sizeof(u32));
    EXPECT_NE(ptr, nullptr);
    std::unique_ptr<u32[], decltype([](u32 *p) { std::free(p); })> test_output(static_cast<u32 *>(ptr));
    auto misaligned_addr = test_output.get() + 1;
    auto out_ptr = misaligned_addr;
    f32 f_array[8] = {};
    // set 1.0f at index 0, 3, 4
    f_array[0] = 1.0f;
    f_array[3] = 1.0f;
    f_array[4] = 1.0f;
    f32 threshold = 0.5f;
    __m256i ids = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
    const __m256i SHIFT = _mm256_set1_epi32(8);
    const __m256 broad_th = _mm256_set1_ps(threshold);
    u32 j = 0;
    for (; j < 64; j += 8) {
        __m256 current_values = _mm256_loadu_ps(f_array);
        const u32 mask = _mm256_movemask_ps(_mm256_cmp_ps(current_values, broad_th, _CMP_GT_OS));
        _mm256_storeu_si256((__m256i *)(out_ptr), compress256i(ids, mask));
        out_ptr += _mm_popcnt_u32(mask);
        ids = _mm256_add_epi32(ids, SHIFT);
    }
    for (; j < 70; ++j) {
        *out_ptr = j;
        ++out_ptr;
    }
    // test
    u32 expect_out_id = 0;
    u32 i = 0;
    for (; i < 64; ++i) {
        if (const auto k = i % 8; k == 0 || k == 3 || k == 4) {
            EXPECT_EQ(misaligned_addr[expect_out_id++], i);
        }
    }
    for (; i < 70; ++i) {
        EXPECT_EQ(misaligned_addr[expect_out_id++], i);
    }
#endif
}

TEST_F(SIMDTest, testleftpackf) {
#if defined(__AVX2__)
    auto ptr = std::aligned_alloc(32, 1024 * sizeof(f32));
    EXPECT_NE(ptr, nullptr);
    std::unique_ptr<f32[], decltype([](f32 *p) { std::free(p); })> test_input(static_cast<f32 *>(ptr));
    Vector<u32> ids(1000);
    std::iota(ids.begin(), ids.end(), 0);
    Vector<u32> expect_out_ids;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<f32> dis(0.0f, 2.0f);
    for (u32 i = 0; i < 1000; ++i) {
        const f32 value = dis(gen);
        test_input[i] = value;
        if (value > 1.0f) {
            expect_out_ids.push_back(i);
        }
    }
    auto out_ptr = std::make_unique_for_overwrite<u32[]>(1008);
    auto out_ptr_compare = std::make_unique_for_overwrite<u32[]>(1008);
    auto out_ptr_end = filter_scores_output_ids_avx2(out_ptr.get(), 1.0f, test_input.get(), 1000);
    auto out_ptr_compare_end = filter_scores_output_ids_common(out_ptr_compare.get(), 1.0f, test_input.get(), 1000);
    EXPECT_EQ(out_ptr_end, out_ptr.get() + expect_out_ids.size());
    EXPECT_EQ(out_ptr_compare_end, out_ptr_compare.get() + expect_out_ids.size());
    for (u32 i = 0; i < expect_out_ids.size(); ++i) {
        EXPECT_EQ(out_ptr[i], expect_out_ids[i]);
        EXPECT_EQ(out_ptr_compare[i], out_ptr[i]);
    }
#endif
}
