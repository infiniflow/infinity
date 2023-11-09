

module;
#define rectime 0
#include <iomanip>
#include <iostream>
#include <sys/time.h>

import stl;
import index_def;
import search_top_k;
import kmeans_partition;
import infinity_exception;

export module index_data;

namespace infinity {

// from faiss
double elapsed_334() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

export template <typename CentroidsDataType, typename VectorDataType = CentroidsDataType>
struct AnnIVFFlatIndexData {
    MetricType metric_;
    u32 dimension_;
    u32 partition_num_;
    u32 data_num_{};
    Vector<CentroidsDataType> centroids_;
    Vector<Vector<VectorDataType>> vectors_;
    Vector<Vector<u32>> ids_;
    AnnIVFFlatIndexData(MetricType metric, u32 dimension, u32 partition_num)
        : metric_(metric), dimension_(dimension), partition_num_(partition_num), centroids_(partition_num_ * dimension_), vectors_(partition_num_),
          ids_(partition_num_) {}

    template <typename ElemType>
    void train_centroids(u32 dimension,
                         u32 vector_count,
                         const ElemType *vectors_ptr,
                         u32 iteration_max = 0,
                         u32 min_points_per_centroid = 32,
                         u32 max_points_per_centroid = 256) {
        if (dimension != dimension_) {
            Error<StorageException>("Dimension not match");
        }
        if (metric_ != MetricType::kMerticL2 && metric_ != MetricType::kMerticInnerProduct) {
            if (metric_ != MetricType::kInvalid) {
                Error<StorageException>("Metric type not implemented");
            } else {
                Error<StorageException>("Metric type not supported");
            }
            return;
        }
        k_means_partition_only_centroids<metric_>(dimension,
                                                  vector_count,
                                                  vectors_ptr,
                                                  centroids_.data(),
                                                  partition_num_,
                                                  iteration_max,
                                                  min_points_per_centroid,
                                                  max_points_per_centroid);
    }
    void insert_data(i32 dimension, u64 vector_count, const VectorDataType *vectors_ptr, u32 id_begin = 0) {
        if (dimension != dimension_) {
            Error<StorageException>("Dimension not match");
        }
        if (metric_ != MetricType::kMerticL2 && metric_ != MetricType::kMerticInnerProduct) {
            if (metric_ != MetricType::kInvalid) {
                Error<StorageException>("Metric type not implemented");
            } else {
                Error<StorageException>("Metric type not supported");
            }
            return;
        }
        add_data_to_partition<metric_>(dimension, vector_count, vectors_ptr, this, id_begin);
    }
};

export template <MetricType metric, typename ElemType, typename CentroidsDataType, typename VectorDataType>
void add_data_to_partition(u32 dimension,
                           u32 vector_count,
                           const ElemType *vectors_ptr,
                           AnnIVFFlatIndexData<CentroidsDataType, VectorDataType> *index_data,
                           u32 id_begin = 0) {
    if (vector_count <= 0 || index_data == nullptr) {
        std::cout << "\nwarning : vector_count <= 0 || index_data == nullptr" << std::endl;
        return;
    }
    if (index_data->dimension_ != dimension) {
        std::cerr << "\nError : index_data->dimension_ != dimension" << std::endl;
        return;
    }
    if (id_begin == 0)
        id_begin = index_data->data_num_;
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
              << "[" << std::fixed << std::setprecision(3) << ((t0 = elapsed_334()), 0.0) << " s] "
              << "input vectors classification begin.\n"
              << "##################################################" << std::endl;
#endif

    // Classify vectors
    // search_top_1
    search_top_1_without_dis<f32>(dimension, vector_count, vectors_ptr, partition_num, centroids.data(), assigned_partition_id.data());

#ifdef rectime
    std::cout << "\n"
              << "##################################################\n"
              << "[" << std::fixed << std::setprecision(3) << elapsed_334() - t0 << " s] "
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
        ids[partition_of_i].push_back(id_begin + i);
    }
    // update data_num_
    index_data->data_num_ += vector_count;

#ifdef rectime
    std::cout << "\n"
              << "##################################################\n"
              << "[" << std::fixed << std::setprecision(3) << elapsed_334() - t0 << " s] "
              << "input vectors insertion finished.\n"
              << "##################################################" << std::endl;
#endif
}

} // namespace infinity