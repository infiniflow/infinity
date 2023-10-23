#pragma once

#include "common/types/alias/smart_ptr.h"
#include "common/types/complex/row_id.h"

namespace infinity {

class MergeKnnBase {};

template <typename DistType>
class MergeKnn : public MergeKnnBase {
    // using IdxType = u32;

public:
    explicit MergeKnn(i64 query_count, i64 topk)
        : total_count_(0), query_count_(query_count), topk_(topk), idx_array_(MakeUnique<RowID[]>(topk * query_count)),
          distance_array_(MakeUnique<DistType[]>(topk * query_count)) {}

    virtual ~MergeKnn() = default;

public:
    virtual void Search(const DistType *dist, const RowID *row_ids, int count) = 0;

    virtual void Begin() = 0;

    virtual void End() = 0;

    virtual DistType *GetDistances() const = 0;

    virtual RowID *GetIDs() const = 0;

    virtual DistType *GetDistancesByIdx(i64 idx) const = 0;

    virtual RowID *GetIDsByIdx(i64 idx) const = 0;

    i64 total_count() const { return total_count_; }

protected:
    i64 total_count_{};
    bool begin_{false};
    i64 query_count_{};
    i64 topk_{};
    UniquePtr<RowID[]> idx_array_{};
    UniquePtr<DistType[]> distance_array_{};
};

} // namespace infinity