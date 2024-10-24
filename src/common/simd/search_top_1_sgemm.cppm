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

export module search_top_1_sgemm;
import stl;
import mlas_matrix_multiply;
import vector_distance;

namespace infinity {

#if defined(__AVX2__)
template <typename ID>
void inner_search_top_1_with_sgemm_avx2(u32 dimension,
                                        u32 nx,
                                        const f32 *x,
                                        u32 ny,
                                        const f32 *y,
                                        ID *labels,
                                        f32 *distances = nullptr,
                                        u32 block_size_x = 4096,
                                        u32 block_size_y = 1024) {
    if (nx == 0 || ny == 0)
        return;
    UniquePtr<f32[]> distances_holder;
    if (distances == nullptr) {
        distances_holder = MakeUniqueForOverwrite<f32[]>(nx);
        distances = distances_holder.get();
    }
    std::fill_n(distances, nx, std::numeric_limits<f32>::max());
    auto square_x = MakeUniqueForOverwrite<f32[]>(nx);
    auto square_y = MakeUniqueForOverwrite<f32[]>(ny);
    auto x_y_inner_product_buffer = MakeUniqueForOverwrite<f32[]>(block_size_x * block_size_y);
    L2NormsSquares(square_x.get(), x, dimension, nx);
    L2NormsSquares(square_y.get(), y, dimension, ny);
    for (u32 x_part_begin = 0; x_part_begin < nx; x_part_begin += block_size_x) {
        u32 x_part_end = std::min(nx, x_part_begin + block_size_x);
        for (u32 y_part_begin = 0; y_part_begin < ny; y_part_begin += block_size_y) {
            u32 y_part_end = std::min(ny, y_part_begin + block_size_y);
            u32 x_part_size = x_part_end - x_part_begin;
            u32 y_part_size = y_part_end - y_part_begin;
            matrixA_multiply_transpose_matrixB_output_to_C(x + x_part_begin * dimension,
                                                           y + y_part_begin * dimension,
                                                           x_part_size,
                                                           y_part_size,
                                                           dimension,
                                                           x_y_inner_product_buffer.get());
            for (u32 i = 0; i < x_part_size; ++i) {
                u32 x_id = i + x_part_begin;
                float *ip_line = x_y_inner_product_buffer.get() + i * y_part_size;

                _mm_prefetch(ip_line, _MM_HINT_NTA);
                _mm_prefetch(ip_line + 16, _MM_HINT_NTA);

                const __m256 mul_minus2 = _mm256_set1_ps(-2);

                __m256 min_distances = _mm256_set1_ps(distances[x_id] - square_x[x_id]);

                __m256i min_indices = _mm256_set1_epi32(0);

                __m256i current_indices = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
                const __m256i indices_delta = _mm256_set1_epi32(8);

                u32 j = 0;
                for (; j < (y_part_size / 16) * 16; j += 16, ip_line += 16) {
                    u32 j_id = j + y_part_begin;
                    _mm_prefetch(ip_line + 32, _MM_HINT_NTA);
                    _mm_prefetch(ip_line + 48, _MM_HINT_NTA);

                    const __m256 y_norm_0 = _mm256_loadu_ps(square_y.get() + j_id + 0);
                    const __m256 y_norm_1 = _mm256_loadu_ps(square_y.get() + j_id + 8);

                    const __m256 ip_0 = _mm256_loadu_ps(ip_line + 0);
                    const __m256 ip_1 = _mm256_loadu_ps(ip_line + 8);

                    __m256 distances_0 = _mm256_fmadd_ps(ip_0, mul_minus2, y_norm_0);
                    __m256 distances_1 = _mm256_fmadd_ps(ip_1, mul_minus2, y_norm_1);

                    const __m256 comparison_0 = _mm256_cmp_ps(min_distances, distances_0, _CMP_LE_OS);

                    min_distances = _mm256_blendv_ps(distances_0, min_distances, comparison_0);
                    min_indices =
                        _mm256_castps_si256(_mm256_blendv_ps(_mm256_castsi256_ps(current_indices), _mm256_castsi256_ps(min_indices), comparison_0));
                    current_indices = _mm256_add_epi32(current_indices, indices_delta);

                    const __m256 comparison_1 = _mm256_cmp_ps(min_distances, distances_1, _CMP_LE_OS);

                    min_distances = _mm256_blendv_ps(distances_1, min_distances, comparison_1);
                    min_indices =
                        _mm256_castps_si256(_mm256_blendv_ps(_mm256_castsi256_ps(current_indices), _mm256_castsi256_ps(min_indices), comparison_1));
                    current_indices = _mm256_add_epi32(current_indices, indices_delta);
                }

                f32 min_distances_scalar[8];
                u32 min_indices_scalar[8];
                _mm256_storeu_ps(min_distances_scalar, min_distances);
                _mm256_storeu_si256((__m256i *)(min_indices_scalar), min_indices);

                f32 current_min_distance = distances[x_id];
                ID current_min_index = labels[x_id];

                for (u32 jv = 0; jv < 8; ++jv) {
                    f32 distance_candidate = min_distances_scalar[jv] + square_x[x_id];

                    if (distance_candidate < 0)
                        distance_candidate = 0;

                    u32 index_candidate = min_indices_scalar[jv] + y_part_begin;

                    if (current_min_distance > distance_candidate) {
                        current_min_distance = distance_candidate;
                        current_min_index = index_candidate;
                    } else if (current_min_distance == distance_candidate && current_min_index > index_candidate) {
                        current_min_index = index_candidate;
                    }
                }

                for (; j < y_part_size; ++j, ++ip_line) {
                    f32 ip = *ip_line;
                    f32 dis = square_x[x_id] + square_y[j + y_part_begin] - 2 * ip;
                    if (dis < 0)
                        dis = 0;
                    if (current_min_distance > dis) {
                        current_min_distance = dis;
                        current_min_index = j + y_part_begin;
                    }
                }
                if (distances[x_id] > current_min_distance) {
                    distances[x_id] = current_min_distance;
                    labels[x_id] = current_min_index;
                }
            }
        }
    }
}

export template <typename ID>
void search_top_1_with_dis_avx2(u32 dimension, u32 nx, const f32 *x, u32 ny, const f32 *y, ID *labels, f32 *distances) {
    return inner_search_top_1_with_sgemm_avx2(dimension, nx, x, ny, y, labels, distances);
}

export template <typename ID>
void search_top_1_without_dis_avx2(u32 dimension, u32 nx, const f32 *x, u32 ny, const f32 *y, ID *labels) {
    return inner_search_top_1_with_sgemm_avx2(dimension, nx, x, ny, y, labels);
}
#endif

#if defined(__SSE2__)
template <typename ID>
void inner_search_top_1_with_sgemm_sse2(u32 dimension,
                                        u32 nx,
                                        const f32 *x,
                                        u32 ny,
                                        const f32 *y,
                                        ID *labels,
                                        f32 *distances = nullptr,
                                        u32 block_size_x = 4096,
                                        u32 block_size_y = 1024) {
    if (nx == 0 || ny == 0)
        return;
    UniquePtr<f32[]> distances_holder;
    if (distances == nullptr) {
        distances_holder = MakeUniqueForOverwrite<f32[]>(nx);
        distances = distances_holder.get();
    }
    std::fill_n(distances, nx, std::numeric_limits<f32>::max());
    auto square_x = MakeUniqueForOverwrite<f32[]>(nx);
    auto square_y = MakeUniqueForOverwrite<f32[]>(ny);
    auto x_y_inner_product_buffer = MakeUniqueForOverwrite<f32[]>(block_size_x * block_size_y);
    L2NormsSquares(square_x.get(), x, dimension, nx);
    L2NormsSquares(square_y.get(), y, dimension, ny);
    for (u32 x_part_begin = 0; x_part_begin < nx; x_part_begin += block_size_x) {
        u32 x_part_end = std::min(nx, x_part_begin + block_size_x);
        for (u32 y_part_begin = 0; y_part_begin < ny; y_part_begin += block_size_y) {
            u32 y_part_end = std::min(ny, y_part_begin + block_size_y);
            u32 x_part_size = x_part_end - x_part_begin;
            u32 y_part_size = y_part_end - y_part_begin;
            matrixA_multiply_transpose_matrixB_output_to_C(x + x_part_begin * dimension,
                                                           y + y_part_begin * dimension,
                                                           x_part_size,
                                                           y_part_size,
                                                           dimension,
                                                           x_y_inner_product_buffer.get());
            for (u32 i = 0; i < x_part_size; ++i) {
                u32 x_id = i + x_part_begin;
                float *ip_line = x_y_inner_product_buffer.get() + i * y_part_size;

                _mm_prefetch((const char *)(ip_line), _MM_HINT_NTA);
                _mm_prefetch((const char *)(ip_line + 8), _MM_HINT_NTA);

                const __m128 mul_minus2 = _mm_set1_ps(-2);

                __m128 min_distances = _mm_set1_ps(distances[x_id] - square_x[x_id]);

                __m128i min_indices = _mm_set1_epi32(0);

                __m128i current_indices = _mm_setr_epi32(0, 1, 2, 3);
                const __m128i indices_delta = _mm_set1_epi32(4);

                u32 j = 0;
                for (; j < (y_part_size / 8) * 8; j += 8, ip_line += 8) {
                    u32 j_id = j + y_part_begin;
                    _mm_prefetch((const char *)(ip_line + 16), _MM_HINT_NTA);
                    _mm_prefetch((const char *)(ip_line + 24), _MM_HINT_NTA);

                    __m128 y_norm_0 = _mm_loadu_ps(square_y.get() + j_id);
                    __m128 y_norm_1 = _mm_loadu_ps(square_y.get() + j_id + 4);

                    __m128 ip_0 = _mm_loadu_ps(ip_line);
                    __m128 ip_1 = _mm_loadu_ps(ip_line + 4);

                    __m128 distances_0 = _mm_fmadd_ps(ip_0, mul_minus2, y_norm_0);
                    __m128 distances_1 = _mm_fmadd_ps(ip_1, mul_minus2, y_norm_1);

                    const __m128 comparison_0 = _mm_cmp_ps(min_distances, distances_0, _CMP_LE_OS);

                    min_distances = _mm_blendv_ps(distances_0, min_distances, comparison_0);
                    min_indices = _mm_castps_si128(_mm_blendv_ps(_mm_castsi128_ps(current_indices), _mm_castsi128_ps(min_indices), comparison_0));
                    current_indices = _mm_add_epi32(current_indices, indices_delta);

                    const __m128 comparison_1 = _mm_cmp_ps(min_distances, distances_1, _CMP_LE_OS);

                    min_distances = _mm_blendv_ps(distances_1, min_distances, comparison_1);
                    min_indices = _mm_castps_si128(_mm_blendv_ps(_mm_castsi128_ps(current_indices), _mm_castsi128_ps(min_indices), comparison_1));
                    current_indices = _mm_add_epi32(current_indices, indices_delta);
                }

                f32 min_distances_scalar[4];
                u32 min_indices_scalar[4];
                _mm_storeu_ps(min_distances_scalar, min_distances);
                _mm_storeu_si128((__m128i *)(min_indices_scalar), min_indices);

                f32 current_min_distance = distances[x_id];
                ID current_min_index = labels[x_id];

                for (u32 jv = 0; jv < 4; ++jv) {
                    f32 distance_candidate = min_distances_scalar[jv] + square_x[x_id];

                    if (distance_candidate < 0)
                        distance_candidate = 0;

                    u32 index_candidate = min_indices_scalar[jv] + y_part_begin;

                    if (current_min_distance > distance_candidate) {
                        current_min_distance = distance_candidate;
                        current_min_index = index_candidate;
                    } else if (current_min_distance == distance_candidate && current_min_index > index_candidate) {
                        current_min_index = index_candidate;
                    }
                }

                for (; j < y_part_size; ++j, ++ip_line) {
                    f32 ip = *ip_line;
                    f32 dis = square_x[x_id] + square_y[j + y_part_begin] - 2 * ip;
                    if (dis < 0)
                        dis = 0;
                    if (current_min_distance > dis) {
                        current_min_distance = dis;
                        current_min_index = j + y_part_begin;
                    }
                }
                if (distances[x_id] > current_min_distance) {
                    distances[x_id] = current_min_distance;
                    labels[x_id] = current_min_index;
                }
            }
        }
    }
}

export template <typename ID>
void search_top_1_with_dis_sse2(u32 dimension, u32 nx, const f32 *x, u32 ny, const f32 *y, ID *labels, f32 *distances) {
    return inner_search_top_1_with_sgemm_sse2(dimension, nx, x, ny, y, labels, distances);
}

export template <typename ID>
void search_top_1_without_dis_sse2(u32 dimension, u32 nx, const f32 *x, u32 ny, const f32 *y, ID *labels) {
    return inner_search_top_1_with_sgemm_sse2(dimension, nx, x, ny, y, labels);
}
#endif

export template <typename TypeX, typename TypeY, typename ID, typename DistType>
void search_top_1_simple_with_dis(u32 dimension, u32 nx, const TypeX *x, u32 ny, const TypeY *y, ID *labels, DistType *distances) {
    for (u32 i = 0; i < nx; ++i) {
        DistType min_dist = std::numeric_limits<DistType>::max();
        u32 min_idx = 0;
        for (u32 j = 0; j < ny; ++j) {
            DistType dist = L2Distance<DistType>(x + i * dimension, y + j * dimension, dimension);
            if (dist < min_dist) {
                min_dist = dist;
                min_idx = j;
            }
        }
        distances[i] = min_dist;
        labels[i] = min_idx;
    }
}

export template <typename DistType, typename TypeX, typename TypeY, typename ID>
void search_top_1_simple_without_dis(u32 dimension, u32 nx, const TypeX *x, u32 ny, const TypeY *y, ID *labels) {
    for (u32 i = 0; i < nx; ++i) {
        DistType min_dist = std::numeric_limits<DistType>::max();
        u32 min_idx = 0;
        for (u32 j = 0; j < ny; ++j) {
            DistType dist = L2Distance<DistType>(x + i * dimension, y + j * dimension, dimension);
            if (dist < min_dist) {
                min_dist = dist;
                min_idx = j;
            }
        }
        labels[i] = min_idx;
    }
}

} // namespace infinity
