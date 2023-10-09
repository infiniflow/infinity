//
// Created by jinhai on 23-9-25.
//

#pragma once

#include "common/types/complex/embedding_type.h"
#include "common/types/complex/row_id.h"

namespace infinity {

enum class KnnDistanceAlgoType {
    kInvalid,
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

class KnnDistanceBase {
public:
    explicit KnnDistanceBase(KnnDistanceAlgoType type, EmbeddingDataType elem_type) : algo_type_(type), elem_type_(elem_type) {}

    [[nodiscard]] inline KnnDistanceAlgoType algo_type() const { return algo_type_; };

    [[nodiscard]] inline EmbeddingDataType elem_type() const { return elem_type_; };

protected:
    KnnDistanceAlgoType algo_type_{KnnDistanceAlgoType::kInvalid};
    EmbeddingDataType elem_type_{EmbeddingDataType::kElemInvalid};
};

template <typename DistType>
class KnnDistance : public KnnDistanceBase {
public:
    explicit KnnDistance(KnnDistanceAlgoType type, EmbeddingDataType elem_type) : KnnDistanceBase(type, elem_type) {}

    virtual void Begin() = 0;

    virtual void Search(const DistType *base, i16 base_count, i32 segment_id, i16 block_id) = 0;

    virtual void End() = 0;

    virtual DistType *GetDistances() const = 0;

    virtual DistType *GetDistanceByIdx(i64 idx) const = 0;

    virtual RowID *GetIDs() const = 0;

    virtual RowID *GetIDByIdx(i64 idx) const = 0;
};

} // namespace infinity
