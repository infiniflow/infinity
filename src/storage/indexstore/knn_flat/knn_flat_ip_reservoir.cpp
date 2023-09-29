//
// Created by jinhai on 23-9-27.
//

#include "knn_flat_ip_reservoir.h"


#include "knn_flat_ip.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

template<typename DistType>
void
KnnFlatIPReservoir<DistType>::Begin() {
    if(begin_)
        return;

    for(SizeT i = 0; i < query_count_; ++i) {
        single_reservoir_result_handler_->begin(i);
    }

    begin_ = true;
}

template<typename DistType>
void
KnnFlatIPReservoir<DistType>::Search(const DistType* base,
                                     i64 base_count,
                                     i32 segment_id) {
    if(!begin_) {
        ExecutorError("KnnFlatInnerProductInternal isn't begin")
    }

    for(int64_t i = 0; i < query_count_; i++) {
        const DistType* x_i = queries_ + i * dimension_;
        const DistType* y_j = base;

        for(i32 j = 0; j < base_count; j++, y_j += dimension_) {
            DistType ip = faiss::fvec_inner_product(x_i, y_j, dimension_);
            single_reservoir_result_handler_->add_result(ip, CompoundID{segment_id, j}, i);
        }
    }
}

template<typename DistType>
void
KnnFlatIPReservoir<DistType>::End() {
    if(!begin_)
        return;

    for(i32 i = 0; i < query_count_; ++i) {
        single_reservoir_result_handler_->end(i);
    }

    begin_ = false;
}

}