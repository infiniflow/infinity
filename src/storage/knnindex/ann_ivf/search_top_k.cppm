

module;

#include <limits>
#include <type_traits>

import stl;
// import extra_faiss_rewrite;
// import extra_faiss_cmax_handler;
import vector_distance;
import search_top_1_sgemm;
// import some_simd_functions;

export module search_top_k;

namespace infinity {

/*
template <typename ID>
void search_top_1_faiss_with_dis(u32 dimension, u32 nx, const f32 *x, u32 ny, const f32 *y, ID *labels, f32 *distances) {
    TopKSingleBestResultHandler<CMax<f32, ID>> res(nx, distances, labels);
    faiss_exhaustive_L2sqr_blas_cmax_avx2(x, y, dimension, nx, ny, res);
}
*/

template <typename TypeX, typename TypeY, typename ID, typename DistType>
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

export template <typename TypeX, typename TypeY, typename ID, typename DistType>
void search_top_1_with_dis(u32 dimension, u32 nx, const TypeX *x, u32 ny, const TypeY *y, ID *labels, DistType *distances) {
    if constexpr (std::is_same_v<DistType, TypeX> && std::is_same_v<DistType, TypeY> && std::is_same_v<DistType, f32>) {
        // search_top_1_faiss_with_dis(dimension, nx, x, ny, y, labels, distances);
        search_top_1_with_sgemm(dimension, nx, x, ny, y, labels, distances);
    } else {
        search_top_1_simple_with_dis(dimension, nx, x, ny, y, labels, distances);
    }
}

/*
template <typename ID>
void search_top_1_faiss_without_dis(u32 dimension, u32 nx, const f32 *x, u32 ny, const f32 *y, ID *labels) {
    Vector<f32> distances(nx);
    TopKSingleBestResultHandler<CMax<f32, ID>> res(nx, distances.data(), labels);
    faiss_exhaustive_L2sqr_blas_cmax_avx2(x, y, dimension, nx, ny, res);
}
*/

template <typename DistType, typename TypeX, typename TypeY, typename ID>
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

export template <typename DistType, typename TypeX, typename TypeY, typename ID>
void search_top_1_without_dis(u32 dimension, u32 nx, const TypeX *x, u32 ny, const TypeY *y, ID *labels) {
    if constexpr (std::is_same_v<DistType, TypeX> && std::is_same_v<DistType, TypeY> && std::is_same_v<DistType, f32>) {
        // search_top_1_faiss_without_dis(dimension, nx, x, ny, y, labels);
        search_top_1_with_sgemm(dimension, nx, x, ny, y, labels);
    } else {
        search_top_1_simple_without_dis<DistType>(dimension, nx, x, ny, y, labels);
    }
}

} // namespace infinity