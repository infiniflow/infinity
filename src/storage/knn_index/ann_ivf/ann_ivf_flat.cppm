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

export module ann_ivf_flat;

import stl;
import knn_distance;

import infinity_exception;
import index_base;
import annivfflat_index_data;
import kmeans_partition;
import vector_distance;
import search_top_k;
import knn_result_handler;
import bitmask;
import knn_expr;
import internal_types;
import logger;

namespace infinity {

template <typename Compare, MetricType metric, KnnDistanceAlgoType algo>
class AnnIVFFlat final : public KnnDistance<typename Compare::DistanceType> {
    using DistType = typename Compare::DistanceType;
    using ResultHandler = ReservoirResultHandler<Compare>;
    static inline DistType Distance(const DistType *x, const DistType *y, u32 dimension) {
        if constexpr (metric == MetricType::kMetricL2) {
            return L2Distance<DistType>(x, y, dimension);
        } else if constexpr (metric == MetricType::kMetricInnerProduct) {
            return IPDistance<DistType>(x, y, dimension);
        } else {
            String error_message = "Metric type is invalid";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }

public:
    explicit AnnIVFFlat(const DistType *queries, u64 query_count, u32 top_k, u32 dimension, EmbeddingDataType elem_data_type)
        : KnnDistance<DistType>(algo, elem_data_type, query_count, dimension, top_k), queries_(queries) {
        id_array_ = MakeUniqueForOverwrite<RowID[]>(top_k * query_count);
        distance_array_ = MakeUniqueForOverwrite<DistType[]>(top_k * query_count);
        result_handler_ = MakeUnique<ResultHandler>(query_count, top_k, distance_array_.get(), id_array_.get());
    }

    static UniquePtr<AnnIVFFlatIndexData<DistType>> CreateIndex(u32 dimension, u32 vector_count, const DistType *vectors_ptr, u32 partition_num) {
        return AnnIVFFlat<Compare, metric, algo>::CreateIndex(dimension, vector_count, vectors_ptr, vector_count, vectors_ptr, partition_num);
    }

    static UniquePtr<AnnIVFFlatIndexData<DistType>>
    CreateIndex(u32 dimension, u32 train_count, const DistType *train_ptr, u32 vector_count, const DistType *vectors_ptr, u32 partition_num) {
        auto index_data = MakeUnique<AnnIVFFlatIndexData<DistType>>(metric, dimension, partition_num);
        index_data->BuildIndex(dimension, train_count, train_ptr, vector_count, vectors_ptr);
        return index_data;
    }

    void Begin() final {
        if (begin_ || this->query_count_ == 0) {
            return;
        }
        result_handler_->Begin();
        begin_ = true;
    }

    void ReInitialize() {
        if (begin_ || this->query_count_ == 0) {
            return;
        }
        result_handler_->ReInitialize();
        begin_ = true;
    }

    void Search(const DistType *, u16, u32, u16) final {
        String error_message = "Unsupported search function";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }

    void Search(const DistType *, u16, u32, u16, Bitmask &) final {
        String error_message = "Unsupported search function";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }

    void Search(const AnnIVFFlatIndexData<DistType> *base_ivf, u32 segment_id, u32 n_probes) {
        // check metric type
        if (base_ivf->metric_ != metric) {
            String error_message = "Metric type is invalid";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        if (!begin_) {
            String error_message = "IVFFlat isn't begin";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        n_probes = std::min(n_probes, base_ivf->partition_num_);
        if ((n_probes == 0) || (base_ivf->data_num_ == 0)) {
            return;
        }
        this->total_base_count_ += base_ivf->data_num_;
        if (n_probes == 1) {
            auto assign_centroid_ids = MakeUniqueForOverwrite<u32[]>(this->query_count_);
            search_top_1_without_dis<DistType>(this->dimension_,
                                               this->query_count_,
                                               this->queries_,
                                               base_ivf->partition_num_,
                                               base_ivf->centroids_.data(),
                                               assign_centroid_ids.get());
            for (u64 i = 0; i < this->query_count_; i++) {
                u32 selected_centroid = assign_centroid_ids[i];
                u32 contain_nums = base_ivf->ids_[selected_centroid].size();
                const DistType *x_i = this->queries_ + i * this->dimension_;
                const DistType *y_j = base_ivf->vectors_[selected_centroid].data();
                for (u32 j = 0; j < contain_nums; j++, y_j += this->dimension_) {
                    DistType distance = Distance(x_i, y_j, this->dimension_);
                    result_handler_->AddResult(i, distance, RowID(segment_id, base_ivf->ids_[selected_centroid][j]));
                }
            }
        } else {
            auto centroid_dists = MakeUniqueForOverwrite<DistType[]>(n_probes * this->query_count_);
            auto centroid_ids = MakeUniqueForOverwrite<u32[]>(n_probes * this->query_count_);
            search_top_k_with_dis(n_probes,
                                  this->dimension_,
                                  this->query_count_,
                                  queries_,
                                  base_ivf->partition_num_,
                                  base_ivf->centroids_.data(),
                                  centroid_ids.get(),
                                  centroid_dists.get(),
                                  false);
            for (u64 i = 0; i < this->query_count_; i++) {
                const DistType *x_i = queries_ + i * this->dimension_;
                for (u32 k = 0; k < n_probes && centroid_dists[k + i * n_probes] != InvalidValue(); ++k) {
                    const u32 selected_centroid = centroid_ids[k + i * n_probes];
                    const u32 contain_nums = base_ivf->ids_[selected_centroid].size();
                    const DistType *y_j = base_ivf->vectors_[selected_centroid].data();
                    for (u32 j = 0; j < contain_nums; j++, y_j += this->dimension_) {
                        DistType distance = Distance(x_i, y_j, this->dimension_);
                        result_handler_->AddResult(i, distance, RowID(segment_id, base_ivf->ids_[selected_centroid][j]));
                    }
                }
            }
        }
    }

    template <typename Filter>
    void Search(const AnnIVFFlatIndexData<DistType> *base_ivf, u32 segment_id, u32 n_probes, Filter &filter) {
        // check metric type
        if (base_ivf->metric_ != metric) {
            String error_message = "Metric type is invalid";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        if (!begin_) {
            String error_message = "IVFFlat isn't begin";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        n_probes = std::min(n_probes, base_ivf->partition_num_);
        if ((n_probes == 0) || (base_ivf->data_num_ == 0)) {
            return;
        }
        this->total_base_count_ += base_ivf->data_num_;
        if (n_probes == 1) {
            auto assign_centroid_ids = MakeUniqueForOverwrite<u32[]>(this->query_count_);
            search_top_1_without_dis<DistType>(this->dimension_,
                                               this->query_count_,
                                               this->queries_,
                                               base_ivf->partition_num_,
                                               base_ivf->centroids_.data(),
                                               assign_centroid_ids.get());
            for (u64 i = 0; i < this->query_count_; i++) {
                u32 selected_centroid = assign_centroid_ids[i];
                u32 contain_nums = base_ivf->ids_[selected_centroid].size();
                const DistType *x_i = this->queries_ + i * this->dimension_;
                const DistType *y_j = base_ivf->vectors_[selected_centroid].data();
                for (u32 j = 0; j < contain_nums; j++, y_j += this->dimension_) {
                    auto segment_offset = base_ivf->ids_[selected_centroid][j];
                    if (filter(segment_offset)) {
                        DistType distance = Distance(x_i, y_j, this->dimension_);
                        result_handler_->AddResult(i, distance, RowID(segment_id, segment_offset));
                    }
                }
            }
        } else {
            auto centroid_dists = MakeUniqueForOverwrite<DistType[]>(n_probes * this->query_count_);
            auto centroid_ids = MakeUniqueForOverwrite<u32[]>(n_probes * this->query_count_);
            search_top_k_with_dis(n_probes,
                                  this->dimension_,
                                  this->query_count_,
                                  queries_,
                                  base_ivf->partition_num_,
                                  base_ivf->centroids_.data(),
                                  centroid_ids.get(),
                                  centroid_dists.get(),
                                  false);
            for (u64 i = 0; i < this->query_count_; i++) {
                const DistType *x_i = queries_ + i * this->dimension_;
                for (u32 k = 0; k < n_probes && centroid_dists[k + i * n_probes] != InvalidValue(); ++k) {
                    const u32 selected_centroid = centroid_ids[k + i * n_probes];
                    const u32 contain_nums = base_ivf->ids_[selected_centroid].size();
                    const DistType *y_j = base_ivf->vectors_[selected_centroid].data();
                    for (u32 j = 0; j < contain_nums; j++, y_j += this->dimension_) {
                        auto segment_offset = base_ivf->ids_[selected_centroid][j];
                        if (filter(segment_offset)) {
                            DistType distance = Distance(x_i, y_j, this->dimension_);
                            result_handler_->AddResult(i, distance, RowID(segment_id, segment_offset));
                        }
                    }
                }
            }
        }
    }

    void End() final {
        if (!begin_) {
            return;
        }
        result_handler_->End();
        begin_ = false;
    }

    void EndWithoutSort() {
        if (!begin_) {
            return;
        }
        result_handler_->EndWithoutSort();
        begin_ = false;
    }

    [[nodiscard]] inline DistType *GetDistances() const final { return distance_array_.get(); }

    [[nodiscard]] inline RowID *GetIDs() const final { return id_array_.get(); }

    [[nodiscard]] inline DistType *GetDistanceByIdx(u64 idx) const final {
        if (idx >= this->query_count_) {
            String error_message = "Query index exceeds the limit";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        return distance_array_.get() + idx * this->top_k_;
    }

    [[nodiscard]] inline RowID *GetIDByIdx(u64 idx) const final {
        if (idx >= this->query_count_) {
            String error_message = "Query index exceeds the limit";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        return id_array_.get() + idx * this->top_k_;
    }

    [[nodiscard]] static constexpr DistType InvalidValue() { return Compare::InitialValue(); }

    [[nodiscard]] static bool CompareDist(const DistType &a, const DistType &b) { return Compare::Compare(b, a); }

private:
    UniquePtr<RowID[]> id_array_{};
    UniquePtr<DistType[]> distance_array_{};

    UniquePtr<ResultHandler> result_handler_{};

    const DistType *queries_{};
    bool begin_{false};
};

export template <typename DistType>
using AnnIVFFlatL2 = AnnIVFFlat<CompareMax<DistType, RowID>, MetricType::kMetricL2, KnnDistanceAlgoType::kKnnFlatL2>;

export template <typename DistType>
using AnnIVFFlatIP = AnnIVFFlat<CompareMin<DistType, RowID>, MetricType::kMetricInnerProduct, KnnDistanceAlgoType::kKnnFlatIp>;

}; // namespace infinity