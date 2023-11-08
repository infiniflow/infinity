
module;
#include <algorithm>
#include <iostream>
import stl;
import knn_result_handler;
import knn_distance;
import faiss;
import parser;
import third_party;
import infinity_exception;
import index_def;
import index_data;
import kmeans_partition;
// import kmeans_partition_with_triangle;
import vector_distance;
import search_top_k;

export module ann_ivf_flat;

namespace infinity {

export template <typename DistType>
class AnnIVFFlatL2 final : public KnnDistance<DistType> {

    using HeapResultHandler = NewHeapResultHandler<FaissCMax<DistType, RowID>>;
    using HeapSingleHandler = HeapResultHandler::HeapSingleResultHandler;

public:
    explicit AnnIVFFlatL2(const DistType *queries, i64 query_count, i64 top_k, i64 dimension, EmbeddingDataType elem_data_type)
        : KnnDistance<DistType>(KnnDistanceAlgoType::kKnnFlatL2, elem_data_type, query_count, dimension, top_k), queries_(queries) {

        id_array_ = MakeUnique<Vector<RowID>>(this->top_k_ * this->query_count_, RowID());
        distance_array_ = MakeUnique<DistType[]>(this->top_k_ * this->query_count_);

        heap_result_handler_ = MakeUnique<HeapResultHandler>(query_count, distance_array_.get(), id_array_->data(), top_k);
        single_heap_result_handler_ = MakeUnique<HeapSingleHandler>(*heap_result_handler_, query_count);
    }

    static UniquePtr<AnnIVFFlatIndexData<DistType>> CreateIndex(u32 dimension, u32 vector_count, const DistType *vectors_ptr, u32 partition_num) {
        auto index_data = MakeUnique<AnnIVFFlatIndexData<DistType>>(MetricType::kMerticL2, dimension, partition_num);
        k_means_partition_only_centroids_l2<f32>(dimension, vector_count, vectors_ptr, index_data->centroids_.data(), partition_num);
        add_data_to_partition_l2(dimension, vector_count, vectors_ptr, index_data.get());
        return index_data;
    }

    static UniquePtr<AnnIVFFlatIndexData<DistType>>
    CreateIndex(u32 dimension, u32 train_count, DistType *train_ptr, u32 vector_count, DistType *vectors_ptr, u32 partition_num) {
        auto index_data = MakeUnique<AnnIVFFlatIndexData<DistType>>(MetricType::kMerticL2, dimension, partition_num);
        k_means_partition_only_centroids_l2<f32>(dimension, train_count, train_ptr, index_data->centroids_.data(), partition_num);
        add_data_to_partition_l2(dimension, vector_count, vectors_ptr, index_data.get());
        return index_data;
    }

    void Begin() final {
        if (begin_ || this->query_count_ == 0) {
            return;
        }

        for (u64 i = 0; i < this->query_count_; ++i) {
            single_heap_result_handler_->begin(i);
        }

        begin_ = true;
    }

    void Search(const DistType *base, u16 base_count, u32 segment_id, u16 block_id) final {}

    void Search(const AnnIVFFlatIndexData<DistType> *base_ivf, u32 segment_id, i32 n_probes) {
        constexpr bool b_debug_info = false;
        if (base_ivf->partition_num_ < n_probes) {
            Error<ExecutorException>("n_probes > partition_num_", __FILE_NAME__, __LINE__);
        }
        if (!begin_) {
            Error<ExecutorException>("IVFFlatL2 isn't begin", __FILE_NAME__, __LINE__);
        }
        // TODO: data_num_?
        if (base_ivf->data_num_ == 0) {
            return;
        }
        this->total_base_count_ += base_ivf->data_num_;
        if (n_probes == 1) {
            Vector<u32> assign_centroid_ids(this->query_count_);
            search_top_1_without_dis<DistType>(this->dimension_,
                                               this->query_count_,
                                               this->queries_,
                                               base_ivf->partition_num_,
                                               base_ivf->centroids_.data(),
                                               assign_centroid_ids.data());
            for (u64 i = 0; i < this->query_count_; i++) {
                u32 selected_centroid = assign_centroid_ids[i];
                u32 contain_nums = base_ivf->ids_[selected_centroid].size();
                const DistType *x_i = this->queries_ + i * this->dimension_;
                const DistType *y_j = base_ivf->vectors_[selected_centroid].data();
                for (u32 j = 0; j < contain_nums; j++, y_j += this->dimension_) {
                    DistType ip = L2Distance<DistType>(x_i, y_j, this->dimension_);
                    single_heap_result_handler_->add_result(ip, RowID(segment_id, base_ivf->ids_[selected_centroid][j]), i);
                }
            }
        } else {
            if constexpr (true) {
                using HeapResultHandler_INT = NewHeapResultHandler<FaissCMax<DistType, u32>>;
                using HeapSingleHandler_INT = HeapResultHandler_INT::HeapSingleResultHandler;
                Vector<DistType> centroid_dists(n_probes);
                Vector<u32> centroid_ids(n_probes);
                for (u64 i = 0; i < this->query_count_; i++) {
                    const DistType *x_i = queries_ + i * this->dimension_;
                    HeapResultHandler_INT centroid_heap_result(1, centroid_dists.data(), centroid_ids.data(), n_probes);
                    HeapSingleHandler_INT centroid_single_heap_result(centroid_heap_result, 1);
                    centroid_single_heap_result.begin(0);
                    for (u32 j = 0; j < base_ivf->partition_num_; j++) {
                        const DistType *y_j = base_ivf->centroids_.data() + j * this->dimension_;
                        DistType ip = L2Distance<DistType>(x_i, y_j, this->dimension_);
                        centroid_single_heap_result.add_result(ip, j, 0);
                    }
                    centroid_single_heap_result.end(0);
                    for (u32 k = 0; k < n_probes; k++) {
                        const u32 selected_centroid = centroid_ids[k];
                        const u32 contain_nums = base_ivf->ids_[selected_centroid].size();
                        const DistType *y_j = base_ivf->vectors_[selected_centroid].data();
                        for (u32 j = 0; j < contain_nums; j++, y_j += this->dimension_) {
                            DistType ip = L2Distance<DistType>(x_i, y_j, this->dimension_);
                            single_heap_result_handler_->add_result(ip, RowID(segment_id, base_ivf->ids_[selected_centroid][j]), i);
                        }
                    }
                }
            } else {
            }
        }
    }

    void End() final {
        if (!begin_)
            return;

        for (u64 i = 0; i < this->query_count_; ++i) {
            single_heap_result_handler_->end(i);
        }

        begin_ = false;
    }

    [[nodiscard]] inline DistType *GetDistances() const final { return heap_result_handler_->heap_dis_tab; }

    [[nodiscard]] inline RowID *GetIDs() const final { return heap_result_handler_->heap_ids_tab; }

    [[nodiscard]] inline DistType *GetDistanceByIdx(i64 idx) const final {
        if (idx >= this->query_count_) {
            Error<ExecutorException>("Query index exceeds the limit", __FILE_NAME__, __LINE__);
        }
        return heap_result_handler_->heap_dis_tab + idx * this->top_k_;
    }

    [[nodiscard]] inline RowID *GetIDByIdx(i64 idx) const final {
        if (idx >= this->query_count_) {
            Error<ExecutorException>("Query index exceeds the limit", __FILE_NAME__, __LINE__);
        }
        return heap_result_handler_->heap_ids_tab + idx * this->top_k_;
    }

private:
    UniquePtr<Vector<RowID>> id_array_{};
    UniquePtr<DistType[]> distance_array_{};

    UniquePtr<HeapResultHandler> heap_result_handler_{};
    UniquePtr<HeapSingleHandler> single_heap_result_handler_{};
    const DistType *queries_{};
    bool begin_{false};
};

}; // namespace infinity