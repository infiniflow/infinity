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

#include <cstring>
#include <random>
export module kmeans_partition;

import stl;
import infinity_exception;
import index_base;
import vector_distance;
import logger;
import simd_functions;

namespace infinity {

inline Vector<u32> RandomPermutatePartially(u32 vector_count, u32 random_num = 0) {
    if (random_num == 0 || random_num > vector_count) {
        random_num = vector_count;
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    Vector<u32> permutation(vector_count);
    std::iota(permutation.begin(), permutation.end(), 0);
    for (u32 i = 0; i < random_num; ++i) {
        // generate j in range [i, vector_count)
        std::uniform_int_distribution<u32> dis(i, vector_count - 1);
        u32 j = dis(gen);
        // swap vectors[i] and vectors[j]
        if (i != j) {
            std::swap(permutation[i], permutation[j]);
        }
    }
    return permutation;
}

// normalize centroids
template <typename CentroidType>
inline void NormalizeCentroids(u32 dimension, u32 partition_num, CentroidType *centroids) {
    for (u32 i = 0; i < partition_num; ++i) {
        CentroidType *centroid = centroids + i * dimension;
        f32 square = IPDistance<f32>(centroid, centroid, dimension);
        if (square > 0) {
            f32 div = 1.0f / sqrt(square);
            for (u32 j = 0; j < dimension; ++j) {
                centroid[j] *= div;
            }
        }
    }
}

// CentroidsType: the type to calculate centroids
// partition_num: the number of partitions, default to sqrt(vector_count)
// iteration_max: the max iteration count, default to 10
export template <typename CentroidsType, typename ElemType, typename CentroidsOutputType>
[[nodiscard]] u32 GetKMeansCentroids(const MetricType metric,
                                     const u32 dimension,
                                     const u32 vector_count,
                                     const ElemType *vectors_ptr,
                                     Vector<CentroidsOutputType> &centroids_output_vector,
                                     u32 partition_num = 0,
                                     u32 iteration_max = 0,
                                     u32 min_points_per_centroid = 32,
                                     u32 max_points_per_centroid = 256) {
    constexpr int default_iteration_max = 10;
    if (metric != MetricType::kMetricL2 && metric != MetricType::kMetricInnerProduct) {
        String error_message = "Metric type not implemented";
        UnrecoverableError(error_message);
    }
    if (dimension <= 0 || vector_count <= 0) {
        String error_message = "Dimension and vector_count must be positive";
        UnrecoverableError(error_message);
    }
    if (vectors_ptr == nullptr) {
        String error_message = "vectors_ptr cannot be nullptr";
        UnrecoverableError(error_message);
    }
    if (partition_num > vector_count) {
        String error_message = "partition_num cannot be greater than vector_count";
        UnrecoverableError(error_message);
    }
    if (partition_num <= 0) {
        partition_num = (int)sqrt(vector_count);
    }
    if (iteration_max <= 0) {
        iteration_max = default_iteration_max;
    }
    centroids_output_vector.resize(dimension * partition_num);
    CentroidsOutputType *centroids_output = centroids_output_vector.data();
    CentroidsType *centroids = nullptr;
    UniquePtr<CentroidsType[]> centroids_destructor;
    if constexpr (std::is_same_v<CentroidsOutputType, CentroidsType>) {
        centroids = centroids_output;
    } else {
        centroids_destructor = MakeUnique<CentroidsType[]>(dimension * partition_num);
        centroids = centroids_destructor.get();
    }

    const ElemType *training_data = vectors_ptr;
    u32 training_data_num = vector_count;

    // Release temporary random training data when out of scope
    UniquePtr<ElemType[]> random_training_data_destructor;

    // If input vectors are too few, warning and use all vectors to train.
    // If input vectors are too many, randomly choose some vectors to train.
    {
        if (u32 min_num = min_points_per_centroid * partition_num; vector_count < min_num) {
            LOG_TRACE("GetKMeansCentroids: warning : too few vectors, less than min_points_per_centroid * partition_num");
        }
        if (u32 max_num = max_points_per_centroid * partition_num; vector_count > max_num) {
            LOG_TRACE("GetKMeansCentroids: warning : too many vectors, more than max_points_per_centroid * partition_num");
            training_data_num = max_num;
            // generate random training data
            {
                Vector<u32> random_ids = RandomPermutatePartially(vector_count, training_data_num);
                random_training_data_destructor = MakeUnique<ElemType[]>(dimension * training_data_num);
                training_data = random_training_data_destructor.get();
                for (u32 i = 0; i < training_data_num; ++i) {
                    memcpy(random_training_data_destructor.get() + i * dimension,
                           vectors_ptr + random_ids[i] * dimension,
                           dimension * sizeof(ElemType));
                }
            }
        }
    }

    // Initializing centroids
    {
        // If training vectors are randomly chosen, centroids can be copied from training data.
        // Otherwise, centroids need to be randomly generated.
        if (random_training_data_destructor) {
            if constexpr (std::is_same_v<ElemType, CentroidsType>) {
                memcpy(centroids, training_data, sizeof(ElemType) * partition_num * dimension);
            } else {
                for (u32 i = 0; i < partition_num * dimension; ++i) {
                    centroids[i] = training_data[i];
                }
            }
        } else {
            Vector<u32> random_ids = RandomPermutatePartially(training_data_num, partition_num);
            if constexpr (std::is_same_v<ElemType, CentroidsType>) {
                for (u32 i = 0; i < partition_num; ++i) {
                    memcpy(centroids + i * dimension, training_data + random_ids[i] * dimension, sizeof(ElemType) * dimension);
                }
            } else {
                for (u32 i = 0; i < partition_num; ++i) {
                    for (u32 j = 0; j < dimension; ++j) {
                        centroids[i * dimension + j] = training_data[random_ids[i] * dimension + j];
                    }
                }
            }
        }
        // normalize centroids if inner product metric is used
        if (metric == MetricType::kMetricInnerProduct) {
            NormalizeCentroids(dimension, partition_num, centroids);
        }
    }

    // Record some information
    f32 previous_total_distance = std::numeric_limits<f32>::max();
    // Assign each vector to a partition
    Vector<u32> training_data_partition_id(training_data_num);
    // Distance
    Vector<f32> partition_element_distance(training_data_num);
    // Record the number of vectors in each partition
    Vector<u32> partition_element_count(partition_num);

    // Iteration
    for (u32 iter = 1; iter <= iteration_max; ++iter) {
        // info
        f32 this_iter_distance = 0;
        // First : assign each training vector to a partition
        {
            // search top 1
            auto search_top_1_with_dis = GetSIMD_FUNCTIONS().SearchTop1WithDisF32U32_func_ptr_;
            search_top_1_with_dis(dimension,
                                  training_data_num,
                                  training_data,
                                  partition_num,
                                  centroids,
                                  training_data_partition_id.data(),
                                  partition_element_distance.data());
            // Clear partition_element_count
            memset(partition_element_count.data(), 0, sizeof(u32) * partition_num);
            // calculate partition_element_count
            for (auto i : training_data_partition_id)
                ++partition_element_count[i];

            // add distance to this_iter_distance
            this_iter_distance += std::reduce(partition_element_distance.begin(), partition_element_distance.end());
        }
        // Second : update centroids
        {
            // Clear old centroids data
            memset(centroids, 0, sizeof(CentroidsType) * partition_num * dimension);
            // Sum
            for (u32 i = 0; i < training_data_num; ++i) {
                auto vector_pos_i = training_data + i * dimension;
                auto centroid_pos_i = centroids + training_data_partition_id[i] * dimension;
                for (u32 j = 0; j < dimension; ++j) {
                    centroid_pos_i[j] += vector_pos_i[j];
                }
            }
            // For L2 metric, divide the count. If there is no vector in a partition, the centroid of this partition will not be updated.
            // For IP metric, normalize centroids.
            if (metric == MetricType::kMetricL2) {
                for (u32 i = 0; i < partition_num; ++i) {
                    if (auto cnt = partition_element_count[i]; cnt > 0) {
                        f32 inv = 1.0f / (f32)cnt;
                        for (u32 j = 0; j < dimension; ++j) {
                            centroids[i * dimension + j] *= inv;
                        }
                    }
                }
            } else if (metric == MetricType::kMetricInnerProduct) {
                NormalizeCentroids(dimension, partition_num, centroids);
            }
        }

        // Third: split partitions when needed
        // TODO: When to split? How?
        //  Now sort the centroids by the number of vectors in each partition.
        //  For every vacant partition, split the partition with the most vectors.
        {
            for (u32 i = 0; i < partition_num; ++i) {
                if (partition_element_count[i] == 0) {
                    // find the partition with the most vectors
                    u32 max_partition_id = 0;
                    u32 max_partition_element_count = 0;
                    for (u32 j = 0; j < partition_num; ++j) {
                        if (partition_element_count[j] > max_partition_element_count) {
                            max_partition_id = j;
                            max_partition_element_count = partition_element_count[j];
                        }
                    }
                    // split the partition
                    partition_element_count[i] = max_partition_element_count / 2;
                    partition_element_count[max_partition_id] -= partition_element_count[i];
                    // copy the centroid vector
                    memcpy(centroids + i * dimension, centroids + max_partition_id * dimension, dimension * sizeof(CentroidsType));
                    // slightly change che i and max_partition_id centroid vector
                    constexpr f32 epsilon = 1 / 1024.0;
                    constexpr f32 plus_epsilon = 1 + epsilon;
                    constexpr f32 minus_epsilon = 1 - epsilon;
                    for (u32 j = 0; j < dimension; ++j) {
                        centroids[i * dimension + j] *= ((j & 1) ? plus_epsilon : minus_epsilon);
                        centroids[max_partition_id * dimension + j] *= ((j & 1) ? minus_epsilon : plus_epsilon);
                    }
                }
            }
        }

        // TODO:stop condition?
        if (metric == MetricType::kMetricL2 && this_iter_distance >= previous_total_distance)
            break;
        previous_total_distance = this_iter_distance;

        // In the next loop, training data will be re-assigned to partitions.
    }

    // Output results if typeof(centroids_output) != typeof(centroids)
    if constexpr (!std::is_same_v<CentroidsOutputType, CentroidsType>) {
        for (u32 i = 0; i < partition_num * dimension; ++i) {
            centroids_output[i] = (CentroidsOutputType)centroids[i];
        }
    }
    return partition_num;
}

} // namespace infinity
