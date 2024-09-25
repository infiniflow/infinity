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

export module knn_diskann;

import stl;
import knn_distance;

import infinity_exception;
import index_base;
import kmeans_partition;
import search_top_1;
import search_top_k;
import knn_result_handler;
import bitmask;
import knn_expr;
import internal_types;
import logger;
import diskann_index_data;
import diskann_dist_func;

namespace infinity {
template <typename Compare, MetricType metric, KnnDistanceAlgoType algo>
class KnnDiskAnn final : public KnnDistance<typename Compare::DistanceType> {
    using DistType = typename Compare::DistanceType;
    using ResultHandler = ReservoirResultHandler<Compare>;

public:
    explicit KnnDiskAnn(const DistType *queries, u64 query_count, u32 top_k, u32 dimension, EmbeddingDataType elem_data_type)
        : KnnDistance<DistType>(algo, elem_data_type, query_count, dimension, top_k), queries_(queries) {
        id_array_ = MakeUniqueForOverwrite<RowID[]>(top_k * query_count);
        distance_array_ = MakeUniqueForOverwrite<DistType[]>(top_k * query_count);
        result_handler_ = MakeUnique<ResultHandler>(query_count, top_k, distance_array_.get(), id_array_.get());
    }

    void CreateIndex() {
        //TODO
    }

    void Search() {
        //TODO
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
            UnrecoverableError(error_message);
        }
        return distance_array_.get() + idx * this->top_k_;
    }

    [[nodiscard]] inline RowID *GetIDByIdx(u64 idx) const final {
        if (idx >= this->query_count_) {
            String error_message = "Query index exceeds the limit";
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

} // namespace infinity