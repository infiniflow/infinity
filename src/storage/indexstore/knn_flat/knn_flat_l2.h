//
// Created by jinhai on 23-9-27.
//

#pragma once

#include "storage/indexstore/knn_distance.h"
#include "storage/indexstore/common/compound_id.h"
#include "storage/indexstore/knn_flat/alias.h"

namespace infinity {
#if 0
class KnnFlatL2 : public KnnDistance {
public:
    explicit
    KnnFlatL2(const f32* queries,
                        i64 query_count,
                        i64 topk,
                        i64 dimension,
                        CompoundID* row_array,
                        f32* distance_array)
            : KnnDistance(KnnDistanceAlgoType::kKnnFlatIP),
              queries_(queries),
              query_count_(query_count),
              dimension_(dimension),
              top_k_(topk) {
        if(top_k_ < faiss::distance_compute_min_k_reservoir) {
            result_handler_ = MakeUnique<HeapResultHandler>(query_count, distance_array, row_array, topk);
            single_result_handler_ = MakeUnique<HeapSingleResultHandler>(*(HeapResultHandler*)result_handler_.get());
        } else {
            result_handler_ = MakeUnique<ReservoirResultHandler>(query_count, distance_array, row_array, topk);
            single_result_handler_ = MakeUnique<ReservoirSingleResultHandler>(*(ReservoirResultHandler*)result_handler_.get());
        }

    }

private:
    UniquePtr<ResultHandler> result_handler_{};
    UniquePtr<SingleResultHandler> single_result_handler_{};
    const f32* queries_{};
    i64 query_count_{};
    i64 dimension_{};
    i64 top_k_{};
    bool begin_{false};
};
#endif
}