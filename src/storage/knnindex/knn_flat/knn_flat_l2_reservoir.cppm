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

import stl;
import knn_result_handler;
import knn_distance;
import bitmask;
import parser;

import infinity_exception;
import default_values;
import vector_distance;

export module knn_flat_l2_reservoir;

namespace infinity {

export template <typename DistType>
class KnnFlatL2Reservoir final : public KnnDistance<DistType> {

    using ResultHandler = ReservoirResultHandler<CompareMax<DistType, RowID>>;

public:
    explicit KnnFlatL2Reservoir(const DistType *queries, i64 query_count, i64 topk, i64 dimension, EmbeddingDataType elem_data_type)
        : KnnDistance<DistType>(KnnDistanceAlgoType::kKnnFlatL2Reservoir, elem_data_type, query_count, dimension, topk), queries_(queries) {

        id_array_ = MakeUniqueForOverwrite<RowID[]>(topk * query_count);
        distance_array_ = MakeUniqueForOverwrite<DistType[]>(topk * query_count);

        result_handler_ = MakeUnique<ResultHandler>(query_count, topk, distance_array_.get(), id_array_.get());
    }

    void Begin() final {
        if (begin_ || this->query_count_ == 0) {
            return;
        }

        result_handler_->Begin();

        begin_ = true;
    }

    void Search(const DistType *base, u16 base_count, u32 segment_id, u16 block_id) final {
        if (!begin_) {
            Error<ExecutorException>("KnnFlatL2Reservoir isn't begin");
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
                auto ip = L2Distance<DistType>(x_i, y_j, this->dimension_);
                result_handler_->AddResult(i, ip, RowID{segment_id, segment_offset_start + j});
            }
        }
    }

    void Search(const DistType *base, u16 base_count, u32 segment_id, u16 block_id, Bitmask &bitmask) final {
        if (bitmask.IsAllTrue()) {
            Search(base, base_count, segment_id, block_id);
            return;
        }
        if (!begin_) {
            Error<ExecutorException>("KnnFlatL2Reservoir isn't begin");
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
                auto segment_offset = segment_offset_start + j;
                if (bitmask.IsTrue(segment_offset)) {
                    auto l2 = L2Distance<DistType>(x_i, y_j, this->dimension_);
                    result_handler_->AddResult(i, l2, RowID(segment_id, segment_offset_start + j));
                }
            }
        }
    }

    void End() final {
        if (!begin_)
            return;

        result_handler_->End();

        begin_ = false;
    }

    [[nodiscard]] inline DistType *GetDistances() const final { return distance_array_.get(); }

    [[nodiscard]] inline RowID *GetIDs() const final { return id_array_.get(); }

    [[nodiscard]] inline DistType *GetDistanceByIdx(u64 idx) const final {
        if (idx >= this->query_count_) {
            Error<ExecutorException>("Query index exceeds the limit");
        }
        return distance_array_.get() + idx * this->top_k_;
    }

    [[nodiscard]] inline RowID *GetIDByIdx(u64 idx) const final {
        if (idx >= this->query_count_) {
            Error<ExecutorException>("Query index exceeds the limit");
        }
        return id_array_.get() + idx * this->top_k_;
    }

private:
    UniquePtr<RowID[]> id_array_{};
    UniquePtr<DistType[]> distance_array_{};

    UniquePtr<ResultHandler> result_handler_{};
    const DistType *queries_{};
    bool begin_{false};
};

template class KnnFlatL2Reservoir<f32>;

} // namespace infinity
