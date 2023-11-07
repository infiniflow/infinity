
module;
#define rectime 0
// #define usefaiss 0
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <random>
#include <sys/time.h>

import stl;
// import knn_heap;
// import knn_result_handler;
// import knn_distance;
// import knn_partition;
// import faiss;
// import parser;
// import third_party;
import infinity_exception;
import index_data;
import vector_distance;
import search_top_k;
// import extra_faiss_part;

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

// from faiss
template <typename T1, typename T2, typename T3>
static double imbalance_factor(T1 n, T2 k, const T3 *hist) {
    double uf = 0;
    for (int i = 0; i < k; i++) {
        uf += ((double)hist[i]) * ((double)hist[i]);
    }
    return uf * k / ((double)n * n);
}

// from faiss
double elapsed() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}
/*
// from faiss
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

// from faiss
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
*/

Vector<u32> random_permutation_id_partially(u32 vector_count, u32 random_num = 0) {
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

// CentroidsType: the type to calculate centroids
// partition_num: the number of partitions, default to sqrt(vector_count)
// iteration_max: the max iteration count, default to 1
export template <typename CentroidsType, typename ElemType, typename CentroidsOutputType>
void k_means_partition_only_centroids_l2(u32 dimension,
                                         u32 vector_count,
                                         const ElemType *vectors_ptr,
                                         CentroidsOutputType *centroids_output,
                                         u32 partition_num = 0,
                                         u32 iteration_max = 0) {
    constexpr int default_iteration_max = 1;
    constexpr bool b_debug_info = false;
#ifdef rectime
    // Record time now.
    f64 t0;
    std::cout << "\n[" << std::fixed << std::setprecision(3) << ((t0 = elapsed()), 0.f) << " s] "
              << "k-means training begin." << std::endl;
#endif
    if (dimension <= 0 || vector_count <= 0) {
        Error<ExecutorException>("dimension and vector_count must be positive", __FILE_NAME__, __LINE__);
    }
    if (vectors_ptr == nullptr) {
        Error<ExecutorException>("vectors_ptr cannot be nullptr", __FILE_NAME__, __LINE__);
    }
    if (centroids_output == nullptr) {
        Error<ExecutorException>("centroids_output cannot be nullptr", __FILE_NAME__, __LINE__);
    }
    if (partition_num > vector_count) {
        Error<ExecutorException>("partition_num cannot be greater than vector_count", __FILE_NAME__, __LINE__);
    }
    if (partition_num <= 0) {
        partition_num = (int)sqrt(vector_count);
    }
    if (iteration_max <= 0) {
        iteration_max = default_iteration_max;
    }

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
#ifdef rectime
        auto t_prepare_training_data_now = elapsed();
#endif
        // TODO: user can change min max vectors per partition?
        u32 min_points_per_centroid = 39;
        u32 max_points_per_centroid = 256;
        // i32 max_points_per_centroid = 100;
        if (u32 min_num = min_points_per_centroid * partition_num; vector_count < min_num) {
            // warning : too few vectors, less than min_points_per_centroid * partition_num
            std::cout << "warning : too few vectors, less than min_points_per_centroid (" << min_points_per_centroid << ") * partition_num ("
                      << partition_num << ") = " << min_num << " vectors to train" << std::endl;
        }
        if (u32 max_num = max_points_per_centroid * partition_num; vector_count > max_num) {
            // warning : too many vectors, more than max_points_per_centroid * partition_num
            std::cout << "warning : too many vectors, more than max_points_per_centroid (" << max_points_per_centroid << ") * partition_num ("
                      << partition_num << ") = " << max_num << " vectors to train" << std::endl;
            // warning : will randomly choose max_points_per_centroid * partition_num vectors to train
            std::cout << "warning : will randomly choose max_points_per_centroid (" << max_points_per_centroid << ") * partition_num ("
                      << partition_num << ") = " << max_num << " vectors to train" << std::endl;
            training_data_num = max_num;
            // generate random training data
            {
                Vector<u32> random_ids = random_permutation_id_partially(vector_count, training_data_num);
                random_training_data_destructor = MakeUnique<ElemType[]>(dimension * training_data_num);
                training_data = random_training_data_destructor.get();
                for (u32 i = 0; i < training_data_num; ++i) {
                    memcpy(random_training_data_destructor.get() + i * dimension,
                           vectors_ptr + random_ids[i] * dimension,
                           dimension * sizeof(ElemType));
                }
            }
            /*
            {
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
                    memcpy(random_training_data_destructor.get() + i * dimension,
                           vectors_ptr + random_ids[i] * dimension,
                           dimension * sizeof(ElemType));
                }
                training_data = random_training_data_destructor.get();
                std::cout << "random training vectors generated." << std::endl;
            }
            */
        }
#ifdef rectime
        std::cout << "\n[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                  << "training data prepared." << std::endl;
        std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                  << "time for preparing training data: " << elapsed() - t_prepare_training_data_now << " s" << std::endl;
#endif
    }

    // Initializing centroids
    {
#ifdef rectime
        auto t_init_centroids_now = elapsed();
#endif
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
            Vector<u32> random_ids = random_permutation_id_partially(training_data_num, partition_num);
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
#ifdef rectime
        std::cout << "\n[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                  << "centroids initialized." << std::endl;
        std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                  << "time for initializing centroids: " << elapsed() - t_init_centroids_now << " s" << std::endl;
#endif
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
#ifdef rectime
        auto iter_begin_time = elapsed();
        auto total_search_time = 0.0;
        std::cout << "\n[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                  << "iteration begin: " << iter << std::endl;
#endif
        // First : assign each training vector to a partition
        {
#ifdef rectime
            auto t_search_now = elapsed();
#endif
            // search top 1
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
#ifdef rectime
            total_search_time += elapsed() - t_search_now;
            std::cout << "\n[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "training data partitioned." << std::endl;
            std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "time for partitioning training data: " << elapsed() - t_search_now << " s" << std::endl;
#endif
        }

        {
#ifdef rectime
            // output sum of partition_element_count
            u32 sum = 0;
            for (u32 i = 0; i < partition_num; ++i) {
                sum += partition_element_count[i];
            }
            std::cout << "\n[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "sum of partition_element_count: " << sum << std::endl;
#endif
        }

        // Second : update centroids
        {
#ifdef rectime
            auto t_update_centroids_now = elapsed();
#endif
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
            // Divide. If there is no vector in a partition, the centroid of this partition will not be updated.
            for (u32 i = 0; i < partition_num; ++i) {
                if (auto cnt = partition_element_count[i]; cnt > 0) {
                    f32 inv = 1.0f / (f32)cnt;
                    for (u32 j = 0; j < dimension; ++j) {
                        centroids[i * dimension + j] *= inv;
                    }
                }
            }
#ifdef rectime
            std::cout << "\n[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "centroids updated." << std::endl;
            std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "time for updating centroids: " << elapsed() - t_update_centroids_now << " s" << std::endl;
#endif
        }

        // output centroids
        {
#ifdef rectime
            // output content of centroids, 128 per line
            std::cout << "######################################################" << std::endl;
            std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "first 3 centroids:\n";
            for (u32 i = 0; i < 3; ++i) {
                std::cout << "partition " << i << ": ";
                for (u32 j = 0; j < dimension; ++j) {
                    std::cout << centroids[i * dimension + j] << " ";
                }
                std::cout << std::endl;
            }
            std::cout << "######################################################" << std::endl;
            std::cout << std::endl;
#endif
        }

        // output imbalance factor
        {
#ifdef rectime
            // output content of partition_element_count, 70 per line
            std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "partition_element_count:\n";
            for (u32 i = 0; i < partition_num; ++i) {
                std::cout << partition_element_count[i] << " ";
                if (i % 70 == 69)
                    std::cout << std::endl;
            }
            // output min, max of partition_element_count
            u32 min = std::numeric_limits<u32>::max();
            u32 max = std::numeric_limits<u32>::min();
            for (u32 i = 0; i < partition_num; ++i) {
                if (partition_element_count[i] < min) {
                    min = partition_element_count[i];
                }
                if (partition_element_count[i] > max) {
                    max = partition_element_count[i];
                }
            }
            std::cout << "\n[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "max of partition_element_count: " << max << std::endl;
            std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "min of partition_element_count: " << min << std::endl;
            std::cout << std::endl;

            std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "\nimbalance factor: " << std::fixed << std::setprecision(10)
                      << imbalance_factor(training_data_num, partition_num, partition_element_count.data()) << std::endl;
#endif
        }

        // Third: split partitions when needed
        // TODO: When to split? How?
        //  Now sort the centroids by the number of vectors in each partition.
        //  For every vacant partition, split the partition with the most vectors.
        u32 iter_split = 0;
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
                    ++iter_split;
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

        // output info
#ifdef rectime
        ClusteringIterationStats stats = {this_iter_distance,
                                          elapsed() - iter_begin_time,
                                          total_search_time,
                                          imbalance_factor(training_data_num, partition_num, partition_element_count.data()),
                                          (int)iter_split};
        std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                  << "Iteration " << iter << " (" << std::fixed << std::setprecision(2) << stats.time << " s, search " << stats.time_search << " s): "
                  << "objective=" << stats.obj << "\nimbalance=" << std::fixed << std::setprecision(7) << stats.imbalance_factor
                  << " nsplit=" << stats.nsplit << std::endl;
#endif
        // TODO:stop condition?
        if (this_iter_distance >= previous_total_distance)
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
}

export template <typename ElemType, typename CentroidsDataType, typename VectorDataType>
void add_data_to_partition_l2(u32 dimension,
                              u32 vector_count,
                              const ElemType *vectors_ptr,
                              u32 segment_id,
                              IVFFlatIndexData<CentroidsDataType, VectorDataType> *index_data) {
    if (vector_count <= 0 || index_data == nullptr) {
        std::cout << "\nwarning : vector_count <= 0 || index_data == nullptr" << std::endl;
        return;
    }
    if (index_data->dimension_ != dimension) {
        std::cerr << "\nError : index_data->dimension_ != dimension" << std::endl;
        return;
    }
    u32 partition_num = index_data->partition_num_;
    const auto &centroids = index_data->centroids_;
    auto &vectors = index_data->vectors_;
    auto &ids = index_data->ids_;
    Vector<u32> assigned_partition_id(vector_count);
    // record time
#ifdef rectime
    f64 t0;
    std::cout << "\n"
              << "##################################################\n"
              << "[" << std::fixed << std::setprecision(3) << ((t0 = elapsed()), 0.0) << " s] "
              << "input vectors classification begin.\n"
              << "##################################################" << std::endl;
#endif
    // Classify vectors
    // search_top_1

#ifdef usefaiss
    Vector<f32> distances(vector_count);
    // float_maxheap_array_t<i32> res1 = {size_t(vector_count), size_t(1), assigned_partition_id.data(), distances.data()};
    FaissSingleBestResultHandler<FaissCMax<float, i32>> res(vector_count, distances.data(), assigned_partition_id.data());
    // knn_L2sqr_select(vectors_ptr, index_data->centroids_.data(), dimension, vector_count, index_data->partition_num_, res, y_norm2, sel);
    // exhaustive_L2sqr_blas(vectors_ptr, index_data->centroids_.data(), dimension, vector_count, index_data->partition_num_, res, nullptr);
    exhaustive_L2sqr_blas_cmax_avx2(vectors_ptr, index_data->centroids_.data(), dimension, vector_count, index_data->partition_num_, res, nullptr);
    // output assigned_partition_id
    // check coarse_idx of 567736
    std::cout << "#####################################################"
              << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
              << "\nassigned_partition_id of 567736:\t";
    std::cout << assigned_partition_id[567736] << std::endl;
    std::cout << "#####################################################\n" << std::endl;
#else
    search_top_1_without_dis<f32>(dimension, vector_count, vectors_ptr, partition_num, centroids.data(), assigned_partition_id.data());
#endif

#ifdef rectime
    std::cout << "\n"
              << "##################################################\n"
              << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
              << "input vectors classification finished.\n"
              << "##################################################" << std::endl;
#endif

    Vector<u32> partition_element_count(partition_num);
    // calculate partition_element_count
    for (auto i : assigned_partition_id)
        ++partition_element_count[i];
    // Reserve space
    for (u32 i = 0; i < partition_num; ++i) {
        vectors[i].reserve(vectors[i].size() + partition_element_count[i] * dimension);
        ids[i].reserve(ids[i].size() + partition_element_count[i]);
    }

    // insert vectors to partition
    for (u32 i = 0; i < vector_count; ++i) {
        auto vector_pos_i = vectors_ptr + i * dimension;
        auto partition_of_i = assigned_partition_id[i];
        vectors[partition_of_i].insert(vectors[partition_of_i].end(), vector_pos_i, vector_pos_i + dimension);
        ids[partition_of_i].emplace_back(segment_id, i);
    }
    // update data_num_
    index_data->data_num_ += vector_count;

#ifdef rectime
    std::cout << "\n"
              << "##################################################\n"
              << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
              << "input vectors insertion finished.\n"
              << "##################################################" << std::endl;
#endif
}

} // namespace infinity
