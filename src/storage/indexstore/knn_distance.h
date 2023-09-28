//
// Created by jinhai on 23-9-25.
//

#pragma once

#include "storage/indexstore/common/compound_id.h"
#include "common/types/complex/embedding_type.h"

namespace infinity {

enum class KnnDistanceAlgoType {
    kInvalid,
    kKnnFlatIp,
    kKnnFlatIpReservoir,
    kKnnFlatIpBlas,
    kKnnFlatIpBlasReservoir,
    kKnnFlatL2,
    kKnnFlatL2Blas,
};

class KnnDistanceBase {
public:
    explicit
    KnnDistanceBase(KnnDistanceAlgoType type, EmbeddingDataType elem_type): algo_type_(type), elem_type_(elem_type) {}

    [[nodiscard]] inline KnnDistanceAlgoType
    algo_type() const {
        return algo_type_;
    };

    [[nodiscard]] inline EmbeddingDataType
    elem_type() const {
        return elem_type_;
    };

protected:
    KnnDistanceAlgoType algo_type_{KnnDistanceAlgoType::kInvalid};
    EmbeddingDataType elem_type_{EmbeddingDataType::kElemInvalid};
};

template<typename DistType>
class KnnDistance : public KnnDistanceBase {
public:
    explicit
    KnnDistance(KnnDistanceAlgoType type, EmbeddingDataType elem_type): KnnDistanceBase(type, elem_type) {}

    virtual void
    Begin() = 0;

    virtual void
    Search(const DistType* base,
           i64 base_count,
           i32 segment_id) = 0;

    virtual void
    End() = 0;

    virtual DistType*
    GetDistances() const = 0;

    virtual DistType*
    GetDistanceByIdx(i64 idx) const = 0;

    virtual CompoundID*
    GetIDs() const = 0;

    virtual CompoundID*
    GetIDByIdx(i64 idx) const = 0;
};

}
