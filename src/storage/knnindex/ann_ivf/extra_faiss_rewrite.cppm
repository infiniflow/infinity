//
// Created by yzq on 11/6/23.
//

module;
#include <immintrin.h>
#include <iostream>
#include <memory>
#ifndef FINTEGER
#define FINTEGER long
#endif
import extra_sgemm;
import extra_faiss_cmax_handler;

export module extra_faiss_rewrite_deprecated;

namespace infinity {

// FAISS_PRAGMA_IMPRECISE_FUNCTION_BEGIN
float fvec_norm_L2sqr(const float *x, size_t d) {
    // the double in the _ref is suspected to be a typo. Some of the manual
    // implementations this replaces used float.
    float res = 0;
    // FAISS_PRAGMA_IMPRECISE_LOOP
    for (size_t i = 0; i != d; ++i) {
        res += x[i] * x[i];
    }

    return res;
}
// FAISS_PRAGMA_IMPRECISE_FUNCTION_END
void fvec_norms_L2sqr(float *__restrict nr, const float *__restrict x, size_t d, size_t nx) {
    // #pragma omp parallel for
    for (int64_t i = 0; i < nx; i++)
        nr[i] = fvec_norm_L2sqr(x + i * d, d);
}

/*
export template <typename ID>
void faiss_exhaustive_L2sqr_blas_cmax_avx2(const float *x,
                                           const float *y,
                                           size_t d,
                                           size_t nx,
                                           size_t ny,
                                           TopKSingleBestResultHandler<CMax<float, ID>> &res,
                                           const float *y_norms = nullptr) {
    if constexpr (std::is_same_v<ID, int32_t>) {
        faiss_exhaustive_L2sqr_blas_cmax_avx2_int32(x, y, d, nx, ny, res, y_norms);
    } else if constexpr (std::is_same_v<ID, int64_t>) {
        faiss_exhaustive_L2sqr_blas_cmax_avx2_int64(x, y, d, nx, ny, res, y_norms);
    } else {
        std::cerr << "Unsupported ID type" << std::endl;
    }
}
*/

export template <typename T>
void faiss_exhaustive_L2sqr_blas_cmax_avx2(const float *x, const float *y, size_t d, size_t nx, size_t ny, T &res, const float *y_norms = nullptr) {
    // error,exit
    std::cerr << "Unsupported ID type, faiss_exhaustive_L2sqr_blas_cmax_avx2" << std::endl;
    exit(1);
}

export template <>
void faiss_exhaustive_L2sqr_blas_cmax_avx2(const float *x,
                                           const float *y,
                                           size_t d,
                                           size_t nx,
                                           size_t ny,
                                           TopKSingleBestResultHandler<CMax<float, uint64_t>> &res,
                                           const float *y_norms) {
    int distance_compute_blas_query_bs = 4096;
    int distance_compute_blas_database_bs = 1024;
    // BLAS does not like empty matrices
    if (nx == 0 || ny == 0)
        return;

    /* block sizes */
    const size_t bs_x = distance_compute_blas_query_bs;
    const size_t bs_y = distance_compute_blas_database_bs;
    // const size_t bs_x = 16, bs_y = 16;
    std::unique_ptr<float[]> ip_block(new float[bs_x * bs_y]);
    std::unique_ptr<float[]> x_norms(new float[nx]);
    std::unique_ptr<float[]> del2;

    fvec_norms_L2sqr(x_norms.get(), x, d, nx);

    if (!y_norms) {
        float *y_norms2 = new float[ny];
        del2.reset(y_norms2);
        fvec_norms_L2sqr(y_norms2, y, d, ny);
        y_norms = y_norms2;
    }

    for (size_t i0 = 0; i0 < nx; i0 += bs_x) {
        size_t i1 = i0 + bs_x;
        if (i1 > nx)
            i1 = nx;

        res.begin_multiple(i0, i1);

        for (size_t j0 = 0; j0 < ny; j0 += bs_y) {
            size_t j1 = j0 + bs_y;
            if (j1 > ny)
                j1 = ny;
            /* compute the actual dot products */
            {
                float one = 1, zero = 0;
                FINTEGER nyi = j1 - j0, nxi = i1 - i0, di = d;
                sgemm_("Transpose", "Not transpose", &nyi, &nxi, &di, &one, y + j0 * d, &di, x + i0 * d, &di, &zero, ip_block.get(), &nyi);
            }
            // #pragma omp parallel for
            for (int64_t i = i0; i < i1; i++) {
                float *ip_line = ip_block.get() + (i - i0) * (j1 - j0);

                _mm_prefetch(ip_line, _MM_HINT_NTA);
                _mm_prefetch(ip_line + 16, _MM_HINT_NTA);

                // constant
                const __m256 mul_minus2 = _mm256_set1_ps(-2);

                // Track 8 min distances + 8 min indices.
                // All the distances tracked do not take x_norms[i]
                //   into account in order to get rid of extra
                //   _mm256_add_ps(x_norms[i], ...) instructions
                //   is distance computations.
                __m256 min_distances = _mm256_set1_ps(res.dis_tab[i] - x_norms[i]);

                // these indices are local and are relative to j0.
                // so, value 0 means j0.
                __m256i min_indices = _mm256_set1_epi32(0);

                __m256i current_indices = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
                const __m256i indices_delta = _mm256_set1_epi32(8);

                // current j index
                size_t idx_j = 0;
                size_t count = j1 - j0;

                // process 16 elements per loop
                for (; idx_j < (count / 16) * 16; idx_j += 16, ip_line += 16) {
                    _mm_prefetch(ip_line + 32, _MM_HINT_NTA);
                    _mm_prefetch(ip_line + 48, _MM_HINT_NTA);

                    // load values for norms
                    const __m256 y_norm_0 = _mm256_loadu_ps(y_norms + idx_j + j0 + 0);
                    const __m256 y_norm_1 = _mm256_loadu_ps(y_norms + idx_j + j0 + 8);

                    // load values for dot products
                    const __m256 ip_0 = _mm256_loadu_ps(ip_line + 0);
                    const __m256 ip_1 = _mm256_loadu_ps(ip_line + 8);

                    // compute dis = y_norm[j] - 2 * dot(x_norm[i], y_norm[j]).
                    // x_norm[i] was dropped off because it is a constant for a
                    // given i. We'll deal with it later.
                    __m256 distances_0 = _mm256_fmadd_ps(ip_0, mul_minus2, y_norm_0);
                    __m256 distances_1 = _mm256_fmadd_ps(ip_1, mul_minus2, y_norm_1);

                    // compare the new distances to the min distances
                    // for each of the first group of 8 AVX2 components.
                    const __m256 comparison_0 = _mm256_cmp_ps(min_distances, distances_0, _CMP_LE_OS);

                    // update min distances and indices with closest vectors if
                    // needed.
                    min_distances = _mm256_blendv_ps(distances_0, min_distances, comparison_0);
                    min_indices =
                        _mm256_castps_si256(_mm256_blendv_ps(_mm256_castsi256_ps(current_indices), _mm256_castsi256_ps(min_indices), comparison_0));
                    current_indices = _mm256_add_epi32(current_indices, indices_delta);

                    // compare the new distances to the min distances
                    // for each of the second group of 8 AVX2 components.
                    const __m256 comparison_1 = _mm256_cmp_ps(min_distances, distances_1, _CMP_LE_OS);

                    // update min distances and indices with closest vectors if
                    // needed.
                    min_distances = _mm256_blendv_ps(distances_1, min_distances, comparison_1);
                    min_indices =
                        _mm256_castps_si256(_mm256_blendv_ps(_mm256_castsi256_ps(current_indices), _mm256_castsi256_ps(min_indices), comparison_1));
                    current_indices = _mm256_add_epi32(current_indices, indices_delta);
                }

                // dump values and find the minimum distance / minimum index
                float min_distances_scalar[8];
                uint32_t min_indices_scalar[8];
                _mm256_storeu_ps(min_distances_scalar, min_distances);
                _mm256_storeu_si256((__m256i *)(min_indices_scalar), min_indices);

                float current_min_distance = res.dis_tab[i];
                uint32_t current_min_index = res.ids_tab[i];

                // This unusual comparison is needed to maintain the behavior
                // of the original implementation: if two indices are
                // represented with equal distance values, then
                // the index with the min value is returned.
                for (size_t jv = 0; jv < 8; jv++) {
                    // add missing x_norms[i]
                    float distance_candidate = min_distances_scalar[jv] + x_norms[i];

                    // negative values can occur for identical vectors
                    //    due to roundoff errors.
                    if (distance_candidate < 0)
                        distance_candidate = 0;

                    int64_t index_candidate = min_indices_scalar[jv] + j0;

                    if (current_min_distance > distance_candidate) {
                        current_min_distance = distance_candidate;
                        current_min_index = index_candidate;
                    } else if (current_min_distance == distance_candidate && current_min_index > index_candidate) {
                        current_min_index = index_candidate;
                    }
                }

                // process leftovers
                for (; idx_j < count; idx_j++, ip_line++) {
                    float ip = *ip_line;
                    float dis = x_norms[i] + y_norms[idx_j + j0] - 2 * ip;
                    // negative values can occur for identical vectors
                    //    due to roundoff errors.
                    if (dis < 0)
                        dis = 0;

                    if (current_min_distance > dis) {
                        current_min_distance = dis;
                        current_min_index = idx_j + j0;
                    }
                }

                //
                res.add_result(i, current_min_distance, current_min_index);
            }
        }
        // Does nothing for SingleBestResultHandler, but
        // keeping the call for the consistency.
        res.end_multiple();
    }
}

export template <>
void faiss_exhaustive_L2sqr_blas_cmax_avx2(const float *x,
                                           const float *y,
                                           size_t d,
                                           size_t nx,
                                           size_t ny,
                                           TopKSingleBestResultHandler<CMax<float, uint32_t>> &res,
                                           const float *y_norms) {
    int distance_compute_blas_query_bs = 4096;
    int distance_compute_blas_database_bs = 1024;
    // BLAS does not like empty matrices
    if (nx == 0 || ny == 0)
        return;

    /* block sizes */
    const size_t bs_x = distance_compute_blas_query_bs;
    const size_t bs_y = distance_compute_blas_database_bs;
    // const size_t bs_x = 16, bs_y = 16;
    std::unique_ptr<float[]> ip_block(new float[bs_x * bs_y]);
    std::unique_ptr<float[]> x_norms(new float[nx]);
    std::unique_ptr<float[]> del2;

    fvec_norms_L2sqr(x_norms.get(), x, d, nx);

    if (!y_norms) {
        float *y_norms2 = new float[ny];
        del2.reset(y_norms2);
        fvec_norms_L2sqr(y_norms2, y, d, ny);
        y_norms = y_norms2;
    }

    for (size_t i0 = 0; i0 < nx; i0 += bs_x) {
        size_t i1 = i0 + bs_x;
        if (i1 > nx)
            i1 = nx;

        res.begin_multiple(i0, i1);

        for (size_t j0 = 0; j0 < ny; j0 += bs_y) {
            size_t j1 = j0 + bs_y;
            if (j1 > ny)
                j1 = ny;
            /* compute the actual dot products */
            {
                float one = 1, zero = 0;
                FINTEGER nyi = j1 - j0, nxi = i1 - i0, di = d;
                sgemm_("Transpose", "Not transpose", &nyi, &nxi, &di, &one, y + j0 * d, &di, x + i0 * d, &di, &zero, ip_block.get(), &nyi);
            }
            // #pragma omp parallel for
            for (int64_t i = i0; i < i1; i++) {
                float *ip_line = ip_block.get() + (i - i0) * (j1 - j0);

                _mm_prefetch(ip_line, _MM_HINT_NTA);
                _mm_prefetch(ip_line + 16, _MM_HINT_NTA);

                // constant
                const __m256 mul_minus2 = _mm256_set1_ps(-2);

                // Track 8 min distances + 8 min indices.
                // All the distances tracked do not take x_norms[i]
                //   into account in order to get rid of extra
                //   _mm256_add_ps(x_norms[i], ...) instructions
                //   is distance computations.
                __m256 min_distances = _mm256_set1_ps(res.dis_tab[i] - x_norms[i]);

                // these indices are local and are relative to j0.
                // so, value 0 means j0.
                __m256i min_indices = _mm256_set1_epi32(0);

                __m256i current_indices = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
                const __m256i indices_delta = _mm256_set1_epi32(8);

                // current j index
                size_t idx_j = 0;
                size_t count = j1 - j0;

                // process 16 elements per loop
                for (; idx_j < (count / 16) * 16; idx_j += 16, ip_line += 16) {
                    _mm_prefetch(ip_line + 32, _MM_HINT_NTA);
                    _mm_prefetch(ip_line + 48, _MM_HINT_NTA);

                    // load values for norms
                    const __m256 y_norm_0 = _mm256_loadu_ps(y_norms + idx_j + j0 + 0);
                    const __m256 y_norm_1 = _mm256_loadu_ps(y_norms + idx_j + j0 + 8);

                    // load values for dot products
                    const __m256 ip_0 = _mm256_loadu_ps(ip_line + 0);
                    const __m256 ip_1 = _mm256_loadu_ps(ip_line + 8);

                    // compute dis = y_norm[j] - 2 * dot(x_norm[i], y_norm[j]).
                    // x_norm[i] was dropped off because it is a constant for a
                    // given i. We'll deal with it later.
                    __m256 distances_0 = _mm256_fmadd_ps(ip_0, mul_minus2, y_norm_0);
                    __m256 distances_1 = _mm256_fmadd_ps(ip_1, mul_minus2, y_norm_1);

                    // compare the new distances to the min distances
                    // for each of the first group of 8 AVX2 components.
                    const __m256 comparison_0 = _mm256_cmp_ps(min_distances, distances_0, _CMP_LE_OS);

                    // update min distances and indices with closest vectors if
                    // needed.
                    min_distances = _mm256_blendv_ps(distances_0, min_distances, comparison_0);
                    min_indices =
                        _mm256_castps_si256(_mm256_blendv_ps(_mm256_castsi256_ps(current_indices), _mm256_castsi256_ps(min_indices), comparison_0));
                    current_indices = _mm256_add_epi32(current_indices, indices_delta);

                    // compare the new distances to the min distances
                    // for each of the second group of 8 AVX2 components.
                    const __m256 comparison_1 = _mm256_cmp_ps(min_distances, distances_1, _CMP_LE_OS);

                    // update min distances and indices with closest vectors if
                    // needed.
                    min_distances = _mm256_blendv_ps(distances_1, min_distances, comparison_1);
                    min_indices =
                        _mm256_castps_si256(_mm256_blendv_ps(_mm256_castsi256_ps(current_indices), _mm256_castsi256_ps(min_indices), comparison_1));
                    current_indices = _mm256_add_epi32(current_indices, indices_delta);
                }

                // dump values and find the minimum distance / minimum index
                float min_distances_scalar[8];
                uint32_t min_indices_scalar[8];
                _mm256_storeu_ps(min_distances_scalar, min_distances);
                _mm256_storeu_si256((__m256i *)(min_indices_scalar), min_indices);

                float current_min_distance = res.dis_tab[i];
                uint32_t current_min_index = res.ids_tab[i];

                // This unusual comparison is needed to maintain the behavior
                // of the original implementation: if two indices are
                // represented with equal distance values, then
                // the index with the min value is returned.
                for (size_t jv = 0; jv < 8; jv++) {
                    // add missing x_norms[i]
                    float distance_candidate = min_distances_scalar[jv] + x_norms[i];

                    // negative values can occur for identical vectors
                    //    due to roundoff errors.
                    if (distance_candidate < 0)
                        distance_candidate = 0;

                    int64_t index_candidate = min_indices_scalar[jv] + j0;

                    if (current_min_distance > distance_candidate) {
                        current_min_distance = distance_candidate;
                        current_min_index = index_candidate;
                    } else if (current_min_distance == distance_candidate && current_min_index > index_candidate) {
                        current_min_index = index_candidate;
                    }
                }

                // process leftovers
                for (; idx_j < count; idx_j++, ip_line++) {
                    float ip = *ip_line;
                    float dis = x_norms[i] + y_norms[idx_j + j0] - 2 * ip;
                    // negative values can occur for identical vectors
                    //    due to roundoff errors.
                    if (dis < 0)
                        dis = 0;

                    if (current_min_distance > dis) {
                        current_min_distance = dis;
                        current_min_index = idx_j + j0;
                    }
                }

                //
                res.add_result(i, current_min_distance, current_min_index);
            }
        }
        // Does nothing for SingleBestResultHandler, but
        // keeping the call for the consistency.
        res.end_multiple();
    }
}

} // namespace infinity