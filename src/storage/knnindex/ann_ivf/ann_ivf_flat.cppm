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
#include <algorithm>
#include <functional>
import stl;
import knn_distance;
import parser;
import infinity_exception;
import index_base;
import annivfflat_index_data;
import kmeans_partition;
import vector_distance;
import search_top_k;
import heap_twin_operation;
import bitmask;

export module ann_ivf_flat;

namespace infinity {

export template <typename DistType>
class AnnIVFFlatL2 final : public KnnDistance<DistType> {
public:
    explicit AnnIVFFlatL2(const DistType *queries, u64 query_count, i64 top_k, i64 dimension, EmbeddingDataType elem_data_type)
        : KnnDistance<DistType>(KnnDistanceAlgoType::kKnnFlatL2, elem_data_type, query_count, dimension, top_k), queries_(queries) {

        id_array_ = MakeUnique<Vector<RowID>>(this->top_k_ * this->query_count_, RowID());
        distance_array_ = MakeUnique<Vector<DistType>>(this->top_k_ * this->query_count_, std::numeric_limits<DistType>::max());
        heap_twin_max_multiple_ =
            MakeUnique<heap_twin_multiple<std::greater<DistType>, DistType, RowID>>(query_count, top_k, distance_array_->data(), id_array_->data());
    }

    static UniquePtr<AnnIVFFlatIndexData<DistType>> CreateIndex(u32 dimension, u32 vector_count, const DistType *vectors_ptr, u32 partition_num) {
        return AnnIVFFlatL2<DistType>::CreateIndex(dimension, vector_count, vectors_ptr, vector_count, vectors_ptr, partition_num);
    }

    static UniquePtr<AnnIVFFlatIndexData<DistType>>
    CreateIndex(u32 dimension, u32 train_count, const DistType *train_ptr, u32 vector_count, const DistType *vectors_ptr, u32 partition_num) {
        auto index_data = MakeUnique<AnnIVFFlatIndexData<DistType>>(MetricType::kMerticL2, dimension, partition_num);
        k_means_partition_only_centroids<f32>(MetricType::kMerticL2, dimension, train_count, train_ptr, index_data->centroids_.data(), partition_num);
        add_data_to_partition(dimension, vector_count, vectors_ptr, index_data.get());
        return index_data;
    }

    void Begin() final {
        if (begin_ || this->query_count_ == 0) {
            return;
        }
        std::fill_n(distance_array_->data(), this->top_k_ * this->query_count_, std::numeric_limits<DistType>::max());

        begin_ = true;
    }

    void Search(const DistType *, u16, u32, u16) final {
        Error<ExecutorException>("Unsupported search function", __FILE_NAME__, __LINE__);
    }

    void Search(const DistType *, u16, u32, u16, Bitmask &) final {
        Error<ExecutorException>("Unsupported search function", __FILE_NAME__, __LINE__);
    }

    void Search(const AnnIVFFlatIndexData <DistType> *base_ivf, u32 segment_id, u32 n_probes) {
        // check metric type
        if (base_ivf->metric_ != MetricType::kMerticL2) {
            Error<ExecutorException>("Metric type is invalid", __FILE_NAME__, __LINE__);
        }
        if (!begin_) {
            Error<ExecutorException>("IVFFlatL2 isn't begin", __FILE_NAME__, __LINE__);
        }
        n_probes = Min(n_probes, base_ivf->partition_num_);
        if ((n_probes == 0) || (base_ivf->data_num_ == 0)) {
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
                    DistType distance = L2Distance<DistType>(x_i, y_j, this->dimension_);
                    heap_twin_max_multiple_->add(i, distance, RowID(segment_id, base_ivf->ids_[selected_centroid][j]));
                }
            }
        } else {
            Vector<DistType> centroid_dists(n_probes * this->query_count_);
            Vector<u32> centroid_ids(n_probes * this->query_count_);
            search_top_k_with_dis(n_probes,
                                  this->dimension_,
                                  this->query_count_,
                                  queries_,
                                  base_ivf->partition_num_,
                                  base_ivf->centroids_.data(),
                                  centroid_ids.data(),
                                  centroid_dists.data(),
                                  false);
            for (u64 i = 0; i < this->query_count_; i++) {
                const DistType *x_i = queries_ + i * this->dimension_;
                for (u32 k = 0; k < n_probes && centroid_dists[k + i * n_probes] != std::numeric_limits<DistType>::max(); ++k) {
                    const u32 selected_centroid = centroid_ids[k + i * n_probes];
                    const u32 contain_nums = base_ivf->ids_[selected_centroid].size();
                    const DistType *y_j = base_ivf->vectors_[selected_centroid].data();
                    for (u32 j = 0; j < contain_nums; j++, y_j += this->dimension_) {
                        DistType distance = L2Distance<DistType>(x_i, y_j, this->dimension_);
                        heap_twin_max_multiple_->add(i, distance, RowID(segment_id, base_ivf->ids_[selected_centroid][j]));
                    }
                }
            }
        }
    }

    void Search(const AnnIVFFlatIndexData <DistType> *base_ivf, u32 segment_id, u32 n_probes, Bitmask &bitmask) {
        if (bitmask.IsAllTrue()) {
            Search(base_ivf, segment_id, n_probes);
            return;
        }
        // check metric type
        if (base_ivf->metric_ != MetricType::kMerticL2) {
            Error<ExecutorException>("Metric type is invalid", __FILE_NAME__, __LINE__);
        }
        if (!begin_) {
            Error<ExecutorException>("IVFFlatL2 isn't begin", __FILE_NAME__, __LINE__);
        }
        n_probes = Min(n_probes, base_ivf->partition_num_);
        if ((n_probes == 0) || (base_ivf->data_num_ == 0)) {
            return;
        }
        this->total_base_count_ += base_ivf->data_num_;
        if (n_probes == 1) {
            Vector <u32> assign_centroid_ids(this->query_count_);
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
                    auto segment_offset = base_ivf->ids_[selected_centroid][j];
                    if (bitmask.IsTrue(segment_offset)) {
                        DistType distance = L2Distance<DistType>(x_i, y_j, this->dimension_);
                        heap_twin_max_multiple_->add(i, distance, RowID(segment_id, segment_offset));
                    }
                }
            }
        } else {
            Vector <DistType> centroid_dists(n_probes * this->query_count_);
            Vector <u32> centroid_ids(n_probes * this->query_count_);
            search_top_k_with_dis(n_probes,
                                  this->dimension_,
                                  this->query_count_,
                                  queries_,
                                  base_ivf->partition_num_,
                                  base_ivf->centroids_.data(),
                                  centroid_ids.data(),
                                  centroid_dists.data(),
                                  false);
            for (u64 i = 0; i < this->query_count_; i++) {
                const DistType *x_i = queries_ + i * this->dimension_;
                for (u32 k = 0;
                     k < n_probes && centroid_dists[k + i * n_probes] != std::numeric_limits<DistType>::max(); ++k) {
                    const u32 selected_centroid = centroid_ids[k + i * n_probes];
                    const u32 contain_nums = base_ivf->ids_[selected_centroid].size();
                    const DistType *y_j = base_ivf->vectors_[selected_centroid].data();
                    for (u32 j = 0; j < contain_nums; j++, y_j += this->dimension_) {
                        auto segment_offset = base_ivf->ids_[selected_centroid][j];
                        if (bitmask.IsTrue(segment_offset)) {
                            DistType distance = L2Distance<DistType>(x_i, y_j, this->dimension_);
                            heap_twin_max_multiple_->add(i, distance, RowID(segment_id, segment_offset));
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

        heap_twin_max_multiple_->sort();

        begin_ = false;
    }

    [[nodiscard]] inline DistType *GetDistances() const final { return distance_array_->data(); }

    [[nodiscard]] inline RowID *GetIDs() const final { return id_array_->data(); }

    [[nodiscard]] inline DistType *GetDistanceByIdx(u64 idx) const final {
        if (idx >= this->query_count_) {
            Error<ExecutorException>("Query index exceeds the limit", __FILE_NAME__, __LINE__);
        }
        return distance_array_->data() + idx * this->top_k_;
    }

    [[nodiscard]] inline RowID *GetIDByIdx(u64 idx) const final {
        if (idx >= this->query_count_) {
            Error<ExecutorException>("Query index exceeds the limit", __FILE_NAME__, __LINE__);
        }
        return id_array_->data() + idx * this->top_k_;
    }

private:
    UniquePtr<Vector<RowID>> id_array_{};
    UniquePtr<Vector<DistType>> distance_array_{};

    UniquePtr<heap_twin_multiple<std::greater<DistType>, DistType, RowID>> heap_twin_max_multiple_{};

    const DistType *queries_{};
    bool begin_{false};
};

export template <typename DistType>
class AnnIVFFlatIP final : public KnnDistance<DistType> {
public:
    explicit AnnIVFFlatIP(const DistType *queries, u64 query_count, i64 top_k, i64 dimension, EmbeddingDataType elem_data_type)
        : KnnDistance<DistType>(KnnDistanceAlgoType::kKnnFlatIp, elem_data_type, query_count, dimension, top_k), queries_(queries) {

        id_array_ = MakeUnique<Vector<RowID>>(this->top_k_ * this->query_count_, RowID());
        distance_array_ = MakeUnique<Vector<DistType>>(this->top_k_ * this->query_count_, std::numeric_limits<DistType>::lowest());
        heap_twin_min_multiple_ =
            MakeUnique<heap_twin_multiple<std::less<DistType>, DistType, RowID>>(query_count, top_k, distance_array_->data(), id_array_->data());
    }

    static UniquePtr<AnnIVFFlatIndexData<DistType>> CreateIndex(u32 dimension, u32 vector_count, const DistType *vectors_ptr, u32 partition_num) {
        return AnnIVFFlatIP<DistType>::CreateIndex(dimension, vector_count, vectors_ptr, vector_count, vectors_ptr, partition_num);
    }

    static UniquePtr<AnnIVFFlatIndexData<DistType>>
    CreateIndex(u32 dimension, u32 train_count, const DistType *train_ptr, u32 vector_count, const DistType *vectors_ptr, u32 partition_num) {
        auto index_data = MakeUnique<AnnIVFFlatIndexData<DistType>>(MetricType::kMerticInnerProduct, dimension, partition_num);
        k_means_partition_only_centroids<f32>(MetricType::kMerticInnerProduct,
                                              dimension,
                                              train_count,
                                              train_ptr,
                                              index_data->centroids_.data(),
                                              partition_num);
        add_data_to_partition(dimension, vector_count, vectors_ptr, index_data.get());
        return index_data;
    }

    void Begin() final {
        if (begin_ || this->query_count_ == 0) {
            return;
        }
        std::fill_n(distance_array_->data(), this->top_k_ * this->query_count_, std::numeric_limits<DistType>::lowest());

        begin_ = true;
    }

    void Search(const DistType *, u16, u32, u16) final {
        Error<ExecutorException>("Unsupported search function", __FILE_NAME__, __LINE__);
    }

    void Search(const DistType *, u16, u32, u16, Bitmask &) final {
        Error<ExecutorException>("Unsupported search function", __FILE_NAME__, __LINE__);
    }

    void Search(const AnnIVFFlatIndexData <DistType> *base_ivf, u32 segment_id, u32 n_probes) {
        // check metric type
        if (base_ivf->metric_ != MetricType::kMerticInnerProduct) {
            Error<ExecutorException>("Metric type is invalid", __FILE_NAME__, __LINE__);
        }
        if (!begin_) {
            Error<ExecutorException>("IVFFlatL2 isn't begin", __FILE_NAME__, __LINE__);
        }
        n_probes = Min(n_probes, base_ivf->partition_num_);
        if ((n_probes == 0) || (base_ivf->data_num_ == 0)) {
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
                    DistType distance = IPDistance<DistType>(x_i, y_j, this->dimension_);
                    heap_twin_min_multiple_->add(i, distance, RowID(segment_id, base_ivf->ids_[selected_centroid][j]));
                }
            }
        } else {
            Vector<DistType> centroid_dists(n_probes * this->query_count_);
            Vector<u32> centroid_ids(n_probes * this->query_count_);
            search_top_k_with_dis(n_probes,
                                  this->dimension_,
                                  this->query_count_,
                                  queries_,
                                  base_ivf->partition_num_,
                                  base_ivf->centroids_.data(),
                                  centroid_ids.data(),
                                  centroid_dists.data(),
                                  false);
            for (u64 i = 0; i < this->query_count_; i++) {
                const DistType *x_i = queries_ + i * this->dimension_;
                for (u32 k = 0; k < n_probes && centroid_dists[k + i * n_probes] != std::numeric_limits<DistType>::lowest(); ++k) {
                    const u32 selected_centroid = centroid_ids[k + i * n_probes];
                    const u32 contain_nums = base_ivf->ids_[selected_centroid].size();
                    const DistType *y_j = base_ivf->vectors_[selected_centroid].data();
                    for (u32 j = 0; j < contain_nums; j++, y_j += this->dimension_) {
                        DistType distance = IPDistance<DistType>(x_i, y_j, this->dimension_);
                        heap_twin_min_multiple_->add(i, distance, RowID(segment_id, base_ivf->ids_[selected_centroid][j]));
                    }
                }
            }
        }
    }

    void Search(const AnnIVFFlatIndexData <DistType> *base_ivf, u32 segment_id, u32 n_probes, Bitmask &bitmask) {
        if (bitmask.IsAllTrue()) {
            Search(base_ivf, segment_id, n_probes);
            return;
        }
        // check metric type
        if (base_ivf->metric_ != MetricType::kMerticInnerProduct) {
            Error<ExecutorException>("Metric type is invalid", __FILE_NAME__, __LINE__);
        }
        if (!begin_) {
            Error<ExecutorException>("IVFFlatL2 isn't begin", __FILE_NAME__, __LINE__);
        }
        n_probes = Min(n_probes, base_ivf->partition_num_);
        if ((n_probes == 0) || (base_ivf->data_num_ == 0)) {
            return;
        }
        this->total_base_count_ += base_ivf->data_num_;
        if (n_probes == 1) {
            Vector <u32> assign_centroid_ids(this->query_count_);
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
                    auto segment_offset = base_ivf->ids_[selected_centroid][j];
                    if (bitmask.IsTrue(segment_offset)) {
                        DistType distance = IPDistance<DistType>(x_i, y_j, this->dimension_);
                        heap_twin_min_multiple_->add(i, distance, RowID(segment_id, segment_offset));
                    }
                }
            }
        } else {
            Vector <DistType> centroid_dists(n_probes * this->query_count_);
            Vector <u32> centroid_ids(n_probes * this->query_count_);
            search_top_k_with_dis(n_probes,
                                  this->dimension_,
                                  this->query_count_,
                                  queries_,
                                  base_ivf->partition_num_,
                                  base_ivf->centroids_.data(),
                                  centroid_ids.data(),
                                  centroid_dists.data(),
                                  false);
            for (u64 i = 0; i < this->query_count_; i++) {
                const DistType *x_i = queries_ + i * this->dimension_;
                for (u32 k = 0;
                     k < n_probes && centroid_dists[k + i * n_probes] != std::numeric_limits<DistType>::lowest(); ++k) {
                    const u32 selected_centroid = centroid_ids[k + i * n_probes];
                    const u32 contain_nums = base_ivf->ids_[selected_centroid].size();
                    const DistType *y_j = base_ivf->vectors_[selected_centroid].data();
                    for (u32 j = 0; j < contain_nums; j++, y_j += this->dimension_) {
                        auto segment_offset = base_ivf->ids_[selected_centroid][j];
                        if (bitmask.IsTrue(segment_offset)) {
                            DistType distance = IPDistance<DistType>(x_i, y_j, this->dimension_);
                            heap_twin_min_multiple_->add(i, distance, RowID(segment_id, segment_offset));
                        }
                    }
                }
            }
        }
    }

    void End() final {
        if (!begin_)
            return;

        heap_twin_min_multiple_->sort();

        begin_ = false;
    }

    [[nodiscard]] inline DistType *GetDistances() const final { return distance_array_->data(); }

    [[nodiscard]] inline RowID *GetIDs() const final { return id_array_->data(); }

    [[nodiscard]] inline DistType *GetDistanceByIdx(u64 idx) const final {
        if (idx >= this->query_count_) {
            Error<ExecutorException>("Query index exceeds the limit", __FILE_NAME__, __LINE__);
        }
        return distance_array_->data() + idx * this->top_k_;
    }

    [[nodiscard]] inline RowID *GetIDByIdx(u64 idx) const final {
        if (idx >= this->query_count_) {
            Error<ExecutorException>("Query index exceeds the limit", __FILE_NAME__, __LINE__);
        }
        return id_array_->data() + idx * this->top_k_;
    }

private:
    UniquePtr<Vector<RowID>> id_array_{};
    UniquePtr<Vector<DistType>> distance_array_{};

    UniquePtr<heap_twin_multiple<std::less<DistType>, DistType, RowID>> heap_twin_min_multiple_{};

    const DistType *queries_{};
    bool begin_{false};
};

}; // namespace infinity