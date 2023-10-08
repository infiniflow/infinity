//
// Created by jinhai on 23-9-25.
//

#include "knn_flat_ip.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

template<typename DistType>
void
KnnFlatIP<DistType>::Begin() {
    if(begin_ || query_count_ == 0) {
        return;
    }

    for(SizeT i = 0; i < query_count_; ++i) {
        single_heap_result_handler_->begin(i);
    }

    begin_ = true;
}

template<typename DistType>
void
KnnFlatIP<DistType>::Search(const DistType* base,
                            i16 base_count,
                            i32 segment_id,
                            i16 block_id) {
    if(!begin_) {
        ExecutorError("KnnFlatIP isn't begin")
    }

    if(base_count == 0) {
        return;
    }

    for(int64_t i = 0; i < query_count_; i++) {
        const DistType* x_i = queries_ + i * dimension_;
        const DistType* y_j = base;

        for(i16 j = 0; j < base_count; j++, y_j += dimension_) {
            DistType ip = faiss::fvec_inner_product(x_i, y_j, dimension_);
            single_heap_result_handler_->add_result(ip, RowID{segment_id, block_id, j}, i);
        }
    }
}

template<typename DistType>
void
KnnFlatIP<DistType>::End() {
    if(!begin_)
        return;

    for(i32 i = 0; i < query_count_; ++i) {
        single_heap_result_handler_->end(i);
    }

    begin_ = false;
}

}
