
module;

#include <cstring>
#include <mm_malloc.h>

import std;
import stl;
import knn_heap;
import knn_result_handler;
import knn_distance;
import knn_partition;
import faiss;
import parser;
import third_party;
import infinity_assert;
import infinity_exception;
import index_data;
import vector_distance;

export module kmeans_partition;

namespace infinity {

template <typename DistType>
void random_fill_centroid(i32 dimension, i32 vector_count, const DistType *vectors_ptr, i32 partition_num, DistType *centroids_ptr) {
    for (i32 i = 0; i < partition_num; ++i) {
        // TODO: choose wisely. maybe the same?
        i32 random_index = rand() % vector_count;
        memcpy(centroids_ptr + i * dimension, vectors_ptr + random_index * dimension, dimension * sizeof(DistType));
    }
}

template <typename DiffType, typename DistType>
Pair<i32, DiffType> l2_find_nearest_centroid(i32 dimension, const DistType *vector_ptr, i32 partition_num, const DistType *centroids_ptr) {
    i32 nearest_centroid_id = -1;
    DiffType nearest_distance = std::numeric_limits<DiffType>::max();
    for (i32 i = 0; i < partition_num; ++i) {
        DiffType distance = L2Distance<DiffType>(vector_ptr, centroids_ptr + i * dimension, dimension);
        if (distance < nearest_distance) {
            nearest_distance = distance;
            nearest_centroid_id = i;
        }
    }
    return {nearest_centroid_id, nearest_distance};
}

export template <typename DistType>
void l2_kmeans_partition_inplace_f32(i32 dimension,
                                     i32 vector_count,
                                     DistType *vectors_ptr,
                                     i32 partition_num,
                                     i32 segment_id,
                                     i16 block_id,
                                     IVFFlatIndexData<DistType> *index_data,
                                     i32 iteration_max = 10) {
    // assert dimension, vector_count, partition_num, iteration_max > 0
    if (dimension <= 0 || vector_count <= 0 || partition_num <= 0 || iteration_max <= 0) {
        Error<ExecutorException>("dimension, vector_count, partition_num, iteration_max must be positive", __FILE_NAME__, __LINE__);
    }
    if (partition_num > vector_count) {
        Error<ExecutorException>("partition_num cannot be larger than vector_count", __FILE_NAME__, __LINE__);
    }
    auto &centroids = index_data->centroids_;
    auto &vectors = index_data->vectors_;
    auto &ids = index_data->ids_;
    f32 total_distance = std::numeric_limits<f32>::max();
    random_fill_centroid(dimension, vector_count, vectors_ptr, partition_num, index_data->centroids_.data());
    Vector<f32> sum(partition_num * dimension, 0);
    Vector<i32> count(partition_num, 0);
    for (i32 iter = 0; iter < iteration_max; ++iter) {
        std::cout << "iteration time: " << iter << std::endl;
        // TODO: bucket has no (or very few) member?
        f32 this_iter_distance = 0;
        memset(sum.data(), 0, sizeof(f32) * sum.size());
        memset(count.data(), 0, sizeof(i32) * count.size());
        for (i32 i = 0; i < vector_count; ++i) {
            if (i % 10000 == 0) {
                std::cout << "vector id: " << i << std::endl;
            }
            auto vector_pos_i = vectors_ptr + i * dimension;
            auto [centroid_id, distance] = l2_find_nearest_centroid<f32>(dimension, vector_pos_i, partition_num, centroids.data());
            this_iter_distance += distance;
            ++count[centroid_id];
            for (i32 j = 0; j < dimension; ++j) {
                sum[centroid_id * dimension + j] += vector_pos_i[j];
            }
        }
        if (this_iter_distance >= total_distance) {
            break;
        }
        total_distance = this_iter_distance;
        for (i32 i = 0; i < partition_num; ++i) {
            for (i32 j = 0; j < dimension; ++j) {
                centroids[i * dimension + j] = sum[i * dimension + j] / count[i];
            }
        }
    }
    for (i32 i = 0; i < partition_num; ++i) {
        vectors[i].reserve(count[i] * dimension);
        ids[i].reserve(count[i]);
    }
    for (i32 i = 0; i < vector_count; ++i) {
        auto vector_pos_i = vectors_ptr + i * dimension;
        auto [centroid_id, _] = l2_find_nearest_centroid<f32>(dimension, vector_pos_i, partition_num, centroids.data());
        // TODO:  use memcpy
        vectors[centroid_id].insert(vectors[centroid_id].end(), vector_pos_i, vector_pos_i + dimension);
        ids[centroid_id].emplace_back(segment_id, block_id, i);
    }
}

} // namespace infinity
