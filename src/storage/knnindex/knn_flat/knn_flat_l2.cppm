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

import infinity_exception;
import default_values;
import vector_distance;
import heap_twin_operation;

export module knn_flat_l2;

namespace infinity {

export template <typename DistType>
class KnnFlatL2 final : public KnnDistance<DistType> {
public:
    explicit KnnFlatL2(const DistType *queries, i64 query_count, i64 topk, i64 dimension, EmbeddingDataType elem_data_type)
        : KnnDistance<DistType>(KnnDistanceAlgoType::kKnnFlatL2, elem_data_type, query_count, dimension, topk), queries_(queries) {

        id_array_ = MakeUnique<Vector<RowID>>(this->top_k_ * this->query_count_, RowID());
        distance_array_ = MakeUnique<Vector<DistType>>(this->top_k_ * this->query_count_);
        heap_twin_max_multiple_ =
            MakeUnique<heap_twin_multiple<std::greater<DistType>, DistType, RowID>>(query_count, topk, distance_array_->data(), id_array_->data());
    }

    void Begin() final {
        if (begin_ || this->query_count_ == 0) {
            return;
        }

        std::fill_n(distance_array_->data(), this->top_k_ * this->query_count_, std::numeric_limits<DistType>::max());

        begin_ = true;
    }

    void Search(const DistType *base, u16 base_count, u32 segment_id, u16 block_id) final {
        if (!begin_) {
            Error<ExecutorException>("KnnFlatL2 isn't begin");
        }

        this->total_base_count_ += base_count;

        if (base_count == 0) {
            return;
        }

        u32 segment_offset_start = block_id * DEFAULT_BLOCK_CAPACITY;

        for (u64 i = 0; i < this->query_count_; ++i) {
            const DistType *x_i = queries_ + i * this->dimension_;
            const DistType *y_j = base;

            for (u16 j = 0; j < base_count; j++, y_j += this->dimension_) {
                auto l2 = L2Distance<DistType>(x_i, y_j, this->dimension_);
                heap_twin_max_multiple_->add(i, l2, RowID(segment_id, segment_offset_start + j));
            }
        }
    }

    void End() final {
        if (!begin_)
            return;

        heap_twin_max_multiple_->sort();

        begin_ = false;
    }

    [[nodiscard]] inline DistType *GetDistances() const final { return distance_array_->data(); }

    [[nodiscard]] inline RowID *GetIDs() const final { return id_array_->data(); }

    [[nodiscard]] inline DistType *GetDistanceByIdx(u64 idx) const final {
        if (idx >= this->query_count_) {
            Error<ExecutorException>("Query index exceeds the limit");
        }
        return distance_array_->data() + idx * this->top_k_;
    }

    [[nodiscard]] inline RowID *GetIDByIdx(u64 idx) const final {
        if (idx >= this->query_count_) {
            Error<ExecutorException>("Query index exceeds the limit");
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

template class KnnFlatL2<f32>;

} // namespace infinity
