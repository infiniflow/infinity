
module;

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iostream>
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
import search_top_k;

export module kmeans_partition;

namespace infinity {

// faiss helper
struct ClusteringIterationStats {
    float obj;               ///< objective values (sum of distances reported by index)
    double time;             ///< seconds for iteration
    double time_search;      ///< seconds for just search
    double imbalance_factor; ///< imbalance factor of iteration
    int nsplit;              ///< number of cluster splits
};

template <typename T>
static double imbalance_factor(int n, int k, const T *hist) {
    double uf = 0;
    for (int i = 0; i < k; i++) {
        uf += hist[i] * (double)hist[i];
    }
    return uf * k / ((double)n * n);
}

double elapsed() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

struct RandomGenerator {
    std::mt19937 mt;

    /// random positive integer
    int rand_int() { return mt() & 0x7fffffff; }

    /// random int64_t
    int64_t rand_int64() { return int64_t(rand_int()) | int64_t(rand_int()) << 31; }

    /// generate random integer between 0 and max-1
    int rand_int(int max) { return mt() % max; }

    /// between 0 and 1
    float rand_float() { return mt() / float(mt.max()); }

    double rand_double() { return mt() / double(mt.max()); }

    explicit RandomGenerator(int64_t seed = 1234) : mt((unsigned int)seed) {}
};

void rand_perm(int *perm, size_t n, int64_t seed) {
    for (size_t i = 0; i < n; i++)
        perm[i] = i;

    RandomGenerator rng;

    for (size_t i = 0; i + 1 < n; i++) {
        int i2 = i + rng.rand_int(n - i);
        std::swap(perm[i], perm[i2]);
    }
}

template <typename DiffType, typename ElemType, typename CentroidType>
Pair<i32, DiffType> l2_find_nearest_centroid(i32 dimension, const ElemType *vector_ptr, i32 partition_num, const CentroidType *centroids_ptr) {
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

Vector<i32> random_permutation_id_partially(i32 vector_count, i32 random_num = -1) {
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
        if (i != j) {
            std::swap(permutation[i], permutation[j]);
        }
    }
    return permutation;
}

// TODO:iteration_max?
export template <typename CentroidsType, typename ElemType, typename CentroidsOutputType>
void k_means_partition_only_centroids_l2(i32 dimension,
                                         i32 vector_count,
                                         const ElemType *vectors_ptr,
                                         i32 partition_num,
                                         CentroidsOutputType *centroids_output,
                                         i32 iteration_max = 1) {
    // Record time now.
    f64 t0;
    std::cout << "\n[" << std::fixed << std::setprecision(3) << ((t0 = elapsed()), 0.f) << " s] "
              << "k-means training begin." << std::endl;
    if (dimension <= 0 || vector_count <= 0 || partition_num <= 0 || iteration_max <= 0) {
        Error<ExecutorException>("dimension, vector_count, partition_num, iteration_max must be positive", __FILE_NAME__, __LINE__);
    }
    if (vectors_ptr == nullptr) {
        Error<ExecutorException>("vectors_ptr cannot be nullptr", __FILE_NAME__, __LINE__);
    }
    if (centroids_output == nullptr) {
        Error<ExecutorException>("centroids_output cannot be nullptr", __FILE_NAME__, __LINE__);
    }
    if (partition_num > vector_count) {
        Error<ExecutorException>("partition_num cannot be larger than vector_count", __FILE_NAME__, __LINE__);
    }

    // TODO: use f32 to calculate the centroids?
    CentroidsType *centroids = nullptr;
    UniquePtr<CentroidsType[]> centroids_destructor;
    if constexpr (std::is_same_v<CentroidsOutputType, CentroidsType>) {
        centroids = centroids_output;
    } else {
        centroids_destructor = MakeUnique<CentroidsType[]>(dimension * partition_num);
        centroids = centroids_destructor.get();
    }

    const ElemType *training_data = vectors_ptr;
    i32 training_data_num = vector_count;

    // Release temporary random training data when out of scope
    UniquePtr<ElemType[]> random_training_data_destructor;

    // If input vectors are too few, warning and use all vectors to train.
    // If input vectors are too many, randomly choose some vectors to train.
    {
        // TODO: user can change min max vectors per partition?
        i32 min_points_per_centroid = 39;
        i32 max_points_per_centroid = 256;
        // i32 max_points_per_centroid = 100;
        if (i32 min_num = min_points_per_centroid * partition_num; vector_count < min_num) {
            // warning : too few vectors, less than min_points_per_centroid * partition_num
            std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "warning : too few vectors, less than min_points_per_centroid (" << min_points_per_centroid << ") * partition_num ("
                      << partition_num << ") = " << min_num << " vectors to train" << std::endl;
        }
        if (i32 max_num = max_points_per_centroid * partition_num; vector_count > max_num) {
            // warning : too many vectors, more than max_points_per_centroid * partition_num
            std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "warning : too many vectors, more than max_points_per_centroid (" << max_points_per_centroid << ") * partition_num ("
                      << partition_num << ")" << std::endl;
            // warning : will randomly choose max_points_per_centroid * partition_num vectors to train
            std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "warning : will randomly choose max_points_per_centroid (" << max_points_per_centroid << ") * partition_num ("
                      << partition_num << ") = " << max_num << " vectors to train" << std::endl;
            training_data_num = max_num;
            // TODO: generate random training data
            // Vector<i32> random_ids = random_permutation_id_partially(vector_count, training_data_num);
            // TODO
            Vector<i32> random_ids(vector_count);
            rand_perm(random_ids.data(), vector_count, 1234);
            // output content of random_ids
            std::cout << "random_ids nums: " << training_data_num << std::endl;
            for (int i = 0; i < 20; i++) {
                std::cout << random_ids[i] << " ";
            }
            std::cout << std::endl;
            // output dimension, training_data_num, dimension * training_data_num
            std::cout << "dimension: " << dimension << std::endl;
            std::cout << "training_data_num: " << training_data_num << std::endl;
            std::cout << "dimension * training_data_num: " << dimension * training_data_num << std::endl;
            random_training_data_destructor = MakeUnique<ElemType[]>(dimension * training_data_num);
            for (i32 i = 0; i < training_data_num; ++i) {
                memcpy(random_training_data_destructor.get() + i * dimension, vectors_ptr + random_ids[i] * dimension, dimension * sizeof(ElemType));
            }
            training_data = random_training_data_destructor.get();
            std::cout << "\n[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "random training vectors generated." << std::endl;
        }
    }

    // Initializing centroids
    {
        // If training vectors are randomly chosen, centroids can be copied from training data.
        // Otherwise, centroids need to be randomly generated.
        //        if (random_training_data_destructor != nullptr) {
        //            if constexpr (std::is_same_v<ElemType, CentroidsType>) {
        //                memcpy(centroids, training_data, sizeof(ElemType) * partition_num * dimension);
        //            } else {
        //                for (i32 i = 0; i < partition_num * dimension; ++i) {
        //                    centroids[i] = training_data[i];
        //                }
        //            }
        //        } else {
        //            Vector<i32> random_ids = random_permutation_id_partially(training_data_num, partition_num);
        //            for (i32 i = 0; i < partition_num; ++i) {
        //                for (i32 j = 0; j < dimension; ++j) {
        //                    centroids[i * dimension + j] = training_data[random_ids[i] * dimension + j];
        //                }
        //            }
        //        }
        {
            std::vector<int> perm(training_data_num);
            rand_perm(perm.data(), training_data_num, 1234 + 1 + 0 * 15486557L);
            for (int i = 0; i < partition_num; i++) {
                memcpy(&centroids[i * dimension], training_data + perm[i] * dimension, sizeof(CentroidsType) * dimension);
            }
        }
        std::cout << "\n[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                  << "centroids initialized." << std::endl;
    }

    // Record some information
    f32 previous_total_distance = std::numeric_limits<f32>::max();
    // Assign each vector to a partition
    Vector<i32> training_data_partition_id(training_data_num);
    // Distance
    Vector<f32> partition_element_distance(training_data_num);
    // Record the number of vectors in each partition
    Vector<i32> partition_element_count(partition_num);

    // Iteration
    for (i32 iter = 1; iter <= iteration_max; ++iter) {
        // info
        i32 iter_split = 0;
        f32 this_iter_distance = 0;
        auto iter_begin_time = elapsed();
        auto total_search_time = 0.0;

        std::cout << "\n[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                  << "\niteration count: " << iter << std::endl;
        // First : assign each training vector to a partition
        {
            auto t_search_now = elapsed();
            // search top 1
            search_top_1(dimension,
                         training_data_num,
                         training_data,
                         partition_num,
                         centroids,
                         training_data_partition_id.data(),
                         partition_element_distance.data());
            // Clear partition_element_count
            memset(partition_element_count.data(), 0, sizeof(i32) * partition_num);
            // calculate partition_element_count
            for (auto i : training_data_partition_id)
                ++partition_element_count[i];

            // add distance to this_iter_distance
            this_iter_distance += std::reduce(partition_element_distance.begin(), partition_element_distance.end());
            total_search_time += elapsed() - t_search_now;
        }

        {
            // output sum of partition_element_count
            i32 sum = 0;
            for (i32 i = 0; i < partition_num; ++i) {
                sum += partition_element_count[i];
            }
            std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "sum of partition_element_count: " << sum << std::endl;
        }

        // Second : update centroids
        {
            // Clear old centroids data
            memset(centroids, 0, sizeof(CentroidsType) * partition_num * dimension);
            // Sum
            for (i32 i = 0; i < training_data_num; ++i) {
                auto vector_pos_i = training_data + i * dimension;
                auto partition_of_i = training_data_partition_id[i];
                for (i32 j = 0; j < dimension; ++j) {
                    centroids[partition_of_i * dimension + j] += vector_pos_i[j];
                }
            }
            // Divide. If there is no vector in a partition, the centroid of this partition will not be updated.
            for (i32 i = 0; i < partition_num; ++i) {
                if (auto cnt = partition_element_count[i]; cnt > 0) {
                    auto inv = 1.0f / (f32)cnt;
                    for (i32 j = 0; j < dimension; ++j) {
                        centroids[i * dimension + j] *= inv;
                    }
                }
            }
        }

        // output centroids
        if (true) {
            // output content of centroids, 128 per line
            std::cout << "######################################################" << std::endl;
            std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "first 10 centroids:\n";
            for (i32 i = 0; i < 10; ++i) {
                std::cout << "partition " << i << ": ";
                for (i32 j = 0; j < dimension; ++j) {
                    std::cout << centroids[i * dimension + j] << " ";
                }
                std::cout << std::endl;
            }
            std::cout << "######################################################" << std::endl;
            std::cout << std::endl;
        }

        // output imbalance factor
        {
            // output content of partition_element_count, 10 per line
            std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "partition_element_count:\n";
            for (i32 i = 0; i < partition_num; ++i) {
                std::cout << partition_element_count[i] << " ";
                if (i % 10 == 9)
                    std::cout << std::endl;
            }
            // output min, max of partition_element_count
            i32 min = std::numeric_limits<i32>::max();
            i32 max = std::numeric_limits<i32>::min();
            for (i32 i = 0; i < partition_num; ++i) {
                if (partition_element_count[i] < min) {
                    min = partition_element_count[i];
                }
                if (partition_element_count[i] > max) {
                    max = partition_element_count[i];
                }
            }
            std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "min of partition_element_count: " << min << std::endl;
            std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "max of partition_element_count: " << max << std::endl;
            std::cout << std::endl;

            std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "imbalance factor: " << std::fixed << std::setprecision(3)
                      << imbalance_factor(training_data_num, partition_num, partition_element_count.data()) << std::endl;
        }

        // Third: split partitions when needed
        // TODO: When to split? How?
        //  Now sort the centroids by the number of vectors in each partition.
        //  For every vacant partition, split the partition with the most vectors.
        {
            for (i32 i = 0; i < partition_num; ++i) {
                if (partition_element_count[i] == 0) {
                    // find the partition with the most vectors
                    i32 max_partition_id = -1;
                    i32 max_partition_element_count = -1;
                    for (i32 j = 0; j < partition_num; ++j) {
                        if (partition_element_count[j] > max_partition_element_count) {
                            max_partition_id = j;
                            max_partition_element_count = partition_element_count[j];
                        }
                    }
                    // split the partition
                    ++iter_split;
                    partition_element_count[i] = max_partition_element_count / 2;
                    partition_element_count[max_partition_id] -= partition_element_count[i];
                    // copy the centroid vector
                    memcpy(centroids + i * dimension, centroids + max_partition_id * dimension, dimension * sizeof(CentroidsType));
                    // slightly change che i and max_partition_id centroid vector
                    constexpr f32 epsilon = 1 / 1024.0;
                    constexpr f32 plus_epsilon = 1 + epsilon;
                    constexpr f32 minus_epsilon = 1 - epsilon;
                    for (i32 j = 0; j < dimension; ++j) {
                        centroids[i * dimension + j] *= j & 1 ? plus_epsilon : minus_epsilon;
                        centroids[max_partition_id * dimension + j] *= ((j & 1) ? minus_epsilon : plus_epsilon);
                    }
                }
            }
        }

        // output info
        ClusteringIterationStats stats = {this_iter_distance,
                                          elapsed() - iter_begin_time,
                                          total_search_time,
                                          imbalance_factor(training_data_num, partition_num, partition_element_count.data()),
                                          iter_split};
        std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                  << "Iteration " << iter << " (" << std::fixed << std::setprecision(2) << stats.time << " s, search " << stats.time_search << " s): "
                  << "objective=" << stats.obj << " imbalance=" << std::fixed << std::setprecision(3) << stats.imbalance_factor
                  << " nsplit=" << stats.nsplit << std::endl;
        // TODO:stop?
        previous_total_distance = this_iter_distance;

        // In the next loop, training data will be re-assigned to partitions.
    }

    // Output results if typeof(centroids_output) != typeof(centroids)
    if constexpr (!std::is_same_v<CentroidsOutputType, CentroidsType>) {
        for (i32 i = 0; i < partition_num * dimension; ++i) {
            centroids_output[i] = (CentroidsOutputType)centroids[i];
        }
    }
}

export template <typename ElemType, typename CentroidsDataType, typename VectorDataType>
void add_data_to_partition_l2(i32 dimension,
                              i32 vector_count,
                              const ElemType *vectors_ptr,
                              i32 partition_num,
                              i32 segment_id,
                              IVFFlatIndexData<CentroidsDataType, VectorDataType> *index_data) {
    // Check whether dimension, partition_num matches
    if (dimension != index_data->dimension_) {
        Error<ExecutorException>("dimension not match", __FILE_NAME__, __LINE__);
    }
    if (partition_num != index_data->partition_num_) {
        Error<ExecutorException>("partition_num not match", __FILE_NAME__, __LINE__);
    }
    const auto &centroids = index_data->centroids_;
    auto &vectors = index_data->vectors_;
    auto &ids = index_data->ids_;
    Vector<i32> assigned_partition_id(vector_count);
    Vector<i32> partition_element_count(partition_num);
    // record time
    f64 t0;
    std::cout << "\n"
              << "##################################################\n"
              << "[" << std::fixed << std::setprecision(3) << ((t0 = elapsed()), 0.0) << " s] "
              << "input vectors classification begin.\n"
              << "##################################################" << std::endl;
    // Classify vectors
    // search_top_1
    search_top_1<f32>(dimension, vector_count, vectors_ptr, partition_num, centroids.data(), assigned_partition_id.data());

    // output assigned_partition_id
    if (false) {
        // output content of assigned_partition_id, 10 per line
        std::cout << "#####################################################"
                  << "\n[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                  << "assigned_partition_id:\n";
        for (i32 i = 0; i < vector_count; ++i) {
            std::cout << assigned_partition_id[i] << " ";
        }
        std::cout << "#####################################################\n" << std::endl;
    }

    // calculate partition_element_count
    for (auto i : assigned_partition_id)
        ++partition_element_count[i];
    std::cout << "\n"
              << "##################################################\n"
              << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
              << "input vectors classification finished.\n"
              << "##################################################" << std::endl;

    // Reserve space
    for (i32 i = 0; i < partition_num; ++i) {
        vectors[i].reserve(vectors[i].size() + partition_element_count[i] * dimension);
        ids[i].reserve(ids[i].size() + partition_element_count[i]);
    }

    // insert vectors to partition
    for (i32 i = 0; i < vector_count; ++i) {
        auto vector_pos_i = vectors_ptr + i * dimension;
        auto partition_of_i = assigned_partition_id[i];
        vectors[partition_of_i].insert(vectors[partition_of_i].end(), vector_pos_i, vector_pos_i + dimension);
        ids[partition_of_i].emplace_back(segment_id, i);
    }
    std::cout << "\n"
              << "##################################################\n"
              << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
              << "input vectors insertion finished.\n"
              << "##################################################" << std::endl;
}

} // namespace infinity
