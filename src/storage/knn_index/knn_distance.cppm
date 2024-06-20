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

export module knn_distance;

import stl;

import bitmask;
import knn_expr;
import internal_types;

namespace infinity {

export enum class KnnDistanceAlgoType {
    kInvalid,
    kKnnFlatCosine,
    kKnnFlatIp,
    kKnnFlatIpReservoir,
    kKnnFlatIpBlas,
    kKnnFlatIpBlasReservoir,
    kKnnFlatL2,
    kKnnFlatL2Reservoir,
    kKnnFlatL2Top1,
    kKnnFlatL2Top1Blas,
    kKnnFlatL2Blas,
    kKnnFlatL2BlasReservoir,
};

export class KnnDistanceBase {
public:
    explicit KnnDistanceBase(KnnDistanceAlgoType type, EmbeddingDataType elem_type, u64 query_count, u64 dimension, u64 topk)
        : query_count_(query_count), dimension_(dimension), top_k_(topk), algo_type_(type), elem_type_(elem_type) {}

    virtual ~KnnDistanceBase() = default;

    [[nodiscard]] inline KnnDistanceAlgoType algo_type() const { return algo_type_; };

    [[nodiscard]] inline EmbeddingDataType elem_type() const { return elem_type_; };

    [[nodiscard]] inline u64 QueryCount() const { return query_count_; }

    [[nodiscard]] inline u64 TopK() const { return top_k_; }

    [[nodiscard]] inline u64 TotalBaseCount() const { return total_base_count_; }

protected:

    u64 query_count_{};
    u64 dimension_{};
    u64 top_k_{};
    u64 total_base_count_{};

    KnnDistanceAlgoType algo_type_{KnnDistanceAlgoType::kInvalid};
    EmbeddingDataType elem_type_{EmbeddingDataType::kElemInvalid};
};

export template <typename DistType>
class KnnDistance : public KnnDistanceBase {
public:
    explicit KnnDistance(KnnDistanceAlgoType type, EmbeddingDataType elem_type, u64 query_count, u64 dimension, u64 topk)
        : KnnDistanceBase(type, elem_type, query_count, dimension, topk) {}

    virtual ~KnnDistance() = default;

    virtual void Begin() = 0;

    virtual void Search(const DistType *base, u16 base_count, u32 segment_id, u16 block_id) = 0;

    virtual void Search(const DistType *base, u16 base_count, u32 segment_id, u16 block_id, Bitmask &bitmask) = 0;

    virtual void End() = 0;

    virtual DistType *GetDistances() const = 0;

    virtual DistType *GetDistanceByIdx(u64 idx) const = 0;

    virtual RowID *GetIDs() const = 0;

    virtual RowID *GetIDByIdx(u64 idx) const = 0;
};

} // namespace infinity
