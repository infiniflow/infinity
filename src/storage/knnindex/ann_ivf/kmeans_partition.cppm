
module;

// #include <bits/random.h>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <mm_malloc.h>
#include <random>
#include <sys/time.h>

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

double elapsed() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + tv.tv_usec * 1e-6;
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

Vector<i32> random_permutation_id(i32 vector_count, i32 random_num = -1) {
    if (random_num == -1) {
        random_num = vector_count;
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    Vector<i32> permutation(vector_count);
    std::iota(permutation.begin(), permutation.end(), 0);
    for (i32 i = 0; i < random_num; ++i) {
        // generate j in range [i, vector_count)
        std::uniform_int_distribution<i32> dis(i, vector_count - 1);
        i32 j = dis(gen);
        // swap vectors[i] and vectors[j]
        std::swap(permutation[i], permutation[j]);
    }
    return permutation;
}

template <typename DistType>
UniquePtr<DistType> vector_element_random_permutation(i32 dimension, i32 vector_count, const DistType *vectors_ptr, i32 random_num = -1) {
    if (random_num == -1) {
        random_num = vector_count;
    }
    auto permutation = random_permutation_id(vector_count, random_num);
    UniquePtr<DistType> result = MakeUnique<DistType>(dimension * random_num);
    for (i32 i = 0; i < random_num; ++i) {
        memcpy(result.get() + i * dimension, vectors_ptr + permutation[i] * dimension, dimension * sizeof(DistType));
    }
    return result;
}

template <typename DistType>
void vector_element_random_permutation_output_to_centroids(i32 dimension,
                                                           i32 vector_count,
                                                           const DistType *vectors_ptr,
                                                           DistType *centroids,
                                                           i32 random_num = -1) {
    if (random_num == -1) {
        random_num = vector_count;
    }
    auto permutation = random_permutation_id(vector_count, random_num);
    for (i32 i = 0; i < random_num; ++i) {
        memcpy(centroids + i * dimension, vectors_ptr + permutation[i] * dimension, dimension * sizeof(DistType));
    }
}

/*
template <typename DistType>
void random_fill_centroid_whith_distance(i32 dimension, i32 vector_count, const DistType *vectors_ptr, i32 partition_num, DistType *centroids_ptr) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<i32> dis(0, vector_count - 1);
    std::uniform_real_distribution<f32> choose_bar(0.0, 1.0);
    Vector<f32> dist_v(vector_count);
    i32 first_index = dis(gen);
    memcpy(centroids_ptr, vectors_ptr + first_index * dimension, dimension * sizeof(DistType));
    for (i32 i = 1; i < partition_num; ++i) {
        // TODO: now output i%10
        if (i % 10 == 0) {
            std::cout << "centroid id: " << i << std::endl;
        }
        f32 total_dist = 0;
        f32 random_bar = choose_bar(gen);
        for (i32 j = 0; j < vector_count; ++j) {
            //  // TODO: output j % 100000
            //  if (j % 100000 == 0) {
            //      std::cout << "vector id: " << j << std::endl;
            //  }
            auto [_, distance] = l2_find_nearest_centroid<f32>(dimension, vectors_ptr + j * dimension, i, centroids_ptr);
            dist_v[j] = distance;
            total_dist += distance;
            // // TODO: output total_dist when j % 100000 == 0
            // if (j % 100000 == 0) {
            //     std::cout << "total_dist: " << total_dist << std::endl;
            // }
        }
        f32 partial_dist = 0;
        i32 random_index = -1;
        for (i32 j = 0; j < vector_count; ++j) {
            partial_dist += dist_v[j] / total_dist;
            if (partial_dist >= random_bar) {
                random_index = j;
                break;
            }
        }
        if (random_index == -1) {
            random_index = vector_count - 1;
        }
        memcpy(centroids_ptr + i * dimension, vectors_ptr + random_index * dimension, dimension * sizeof(DistType));
        // TODO: output random_index when i % 10 == 0
        if (i % 10 == 0) {
            std::cout << "random_index: " << random_index << std::endl;
        }
    }
}
 */

// TODO:iteration_max?
export template <typename DistType>
void l2_kmeans_partition_inplace_f32(i32 dimension,
                                     i32 vector_count,
                                     DistType *vectors_ptr,
                                     i32 partition_num,
                                     i32 segment_id,
                                     i16 block_id,
                                     IVFFlatIndexData<DistType> *index_data,
                                     i32 iteration_max = 1) {
    // TODO:time
    auto t0 = elapsed();
    std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
              << "kmeans begin." << std::endl;
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
    // TODO: generate random centroids, now use permutation
    vector_element_random_permutation_output_to_centroids(dimension, vector_count, vectors_ptr, centroids.data(), partition_num);
    std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
              << "random centroids choosen." << std::endl;
    Vector<i32> vector_partition_num(vector_count);
    Vector<i32> partition_element_count(partition_num, 0);
    for (i32 iter = 0; iter < iteration_max; ++iter) {
        std::cout << "\niteration time: " << iter << std::endl;
        // TODO: bucket has no (or very few) member?
        f32 this_iter_distance = 0;
        // memset(partition_element_count.data(), 0, sizeof(i32) * partition_element_count.size());
        std::fill(partition_element_count.begin(), partition_element_count.end(), 0);
        for (i32 i = 0; i < vector_count; ++i) {
            auto vector_pos_i = vectors_ptr + i * dimension;
            auto [centroid_id, distance] = l2_find_nearest_centroid<f32>(dimension, vector_pos_i, partition_num, centroids.data());
            vector_partition_num[i] = centroid_id;
            this_iter_distance += distance;
            ++partition_element_count[centroid_id];
            // TODO: record time
            if ((i + 1) % 100000 == 0) {
                std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                          << "vector id: " << i << std::endl;
            }
        }
        if (this_iter_distance >= total_distance) {
            break;
        }
        total_distance = this_iter_distance;
        memset(centroids.data(), 0, sizeof(DistType) * centroids.size());
        // std::fill(centroids.begin(), centroids.end(), 0);
        for (i32 i = 0; i < vector_count; ++i) {
            auto partition_of_i = vector_partition_num[i];
            for (i32 j = 0; j < dimension; ++j) {
                centroids[partition_of_i * dimension + j] += vectors_ptr[i * dimension + j];
            }
        }
        for (i32 i = 0; i < partition_num; ++i) {
            f32 inv = 1.0f / (f32)partition_element_count[i];
            for (i32 j = 0; j < dimension; ++j) {
                centroids[i * dimension + j] *= inv;
            }
        }
    }
    // TODO: now add data, maybe need to separate this function?
    std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
              << "Now add " << vector_count << " data." << std::endl;
    for (i32 i = 0; i < partition_num; ++i) {
        vectors[i].reserve(partition_element_count[i] * dimension);
        ids[i].reserve(partition_element_count[i]);
    }
    for (i32 i = 0; i < vector_count; ++i) {
        auto vector_pos_i = vectors_ptr + i * dimension;
        auto [centroid_id, _] = l2_find_nearest_centroid<f32>(dimension, vector_pos_i, partition_num, centroids.data());
        // TODO:  use memcpy
        vectors[centroid_id].insert(vectors[centroid_id].end(), vector_pos_i, vector_pos_i + dimension);
        ids[centroid_id].emplace_back(segment_id, block_id, i);
    }
    std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
              << "Now " << vector_count << " data added." << std::endl;
}

} // namespace infinity
