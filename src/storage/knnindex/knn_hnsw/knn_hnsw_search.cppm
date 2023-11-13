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

#include <functional>

import stl;
import knn_distance;
import parser;
import knn_hnsw;
export module knn_hnsw_search;
import heap_twin_operation;
namespace infinity {

export template <typename DataType, typename LabelType>
class KnnHnswSearch final : public KnnDistance<DataType> {
public:
    // TODO: KnnDistanceAlgoType?
    explicit KnnHnswSearch(const DataType *queries, i64 query_count, i64 top_k, i64 dimension, EmbeddingDataType elem_data_type)
        : KnnDistance<DataType>(KnnDistanceAlgoType::kKnnFlatL2, elem_data_type, query_count, dimension, top_k), queries_(queries) {

        id_array_ = MakeUnique<Vector<RowID>>(this->top_k_ * this->query_count_, RowID());
        distance_array_ = MakeUnique<Vector<DataType>>(this->top_k_ * this->query_count_, std::numeric_limits<DataType>::max());
        heap_twin_max_multiple_ =
            MakeUnique<heap_twin_multiple<std::greater<DataType>, DataType, RowID>>(query_count, top_k, distance_array_->data(), id_array_->data());
    }

    void Begin() final {
        if (begin_ || this->query_count_ == 0) {
            return;
        }
        std::fill_n(distance_array_->data(), this->top_k_ * this->query_count_, std::numeric_limits<DataType>::max());

        begin_ = true;
    }

    void Search(const KnnHnsw<DataType, LabelType> *base) {
        for (i64 i = 0; i < this->query_count_; ++i) {
            auto query = queries_ + i * this->dimension_;
            auto ep = base->enterpoint_;
            for (i32 cur_layer = base->max_layer_; cur_layer > 0; --cur_layer) {
                ep = SearchLayerNearest(ep, query, cur_layer);
            }
            auto search_result = SearchLayer(ep, query, 0, Max(this->top_k_, base->ef_));
            while (search_result.size() > this->top_k_) {
                search_result.pop();
            }
            while (!search_result.empty()) {
                const auto &[dist, idx] = search_result.top();
                heap_twin_max_multiple_->add(i, dist, GetLabel(idx));
                search_result.pop();
            }
        }
    }

    void End() final {
        if (!begin_)
            return;

        heap_twin_max_multiple_->sort();

        begin_ = false;
    }

    [[nodiscard]] inline DataType *GetDistances() const final { return distance_array_->data(); }

    [[nodiscard]] inline RowID *GetIDs() const final { return id_array_->data(); }

private:
    UniquePtr<Vector<RowID>> id_array_{};
    UniquePtr<Vector<DataType>> distance_array_{};

    UniquePtr<heap_twin_multiple<std::greater<DataType>, DataType, RowID>> heap_twin_max_multiple_{};

    const DataType *queries_{};
    bool begin_{false};
};
}; // namespace infinity