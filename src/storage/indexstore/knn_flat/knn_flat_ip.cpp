//
// Created by jinhai on 23-9-25.
//

#include "knn_flat_ip.h"
#include "common/utility/infinity_assert.h"
#ifdef __AVX2__
#include <immintrin.h>
#endif

namespace infinity {

template<typename DistType>
void
KnnFlatIP<DistType>::Begin() {
    if(begin_) return ;

    for(SizeT i = 0; i < query_count_; ++ i) {
        single_heap_result_handler_->begin(i);
    }
//
//    if(top_k_ < faiss::distance_compute_min_k_reservoir) {
//        if(query_count_ < faiss::distance_compute_blas_threshold) {
//
//
//        } else {
//            HeapResultHandler* result_handler = static_cast<HeapResultHandler*>(result_handler_.get());
//
//            const SizeT bs_x = faiss::distance_compute_blas_query_bs;
//            for (SizeT i0 = 0; i0 < query_count_; i0 += bs_x) {
//                SizeT i1 = i0 + bs_x;
//                if (i1 > query_count_)
//                    i1 = query_count_;
//
//                result_handler->begin_multiple(i0, i1);
//            }
//        }
//    } else {
//        if(query_count_ < faiss::distance_compute_blas_threshold) {
//            ReservoirSingleResultHandler* single_result_handler
//                    = static_cast<ReservoirSingleResultHandler*>(single_result_handler_.get());
//
//            for(i32 i = 0; i < query_count_; ++ i) {
//                single_result_handler->begin(i);
//            }
//        } else {
//            ReservoirResultHandler* result_handler = static_cast<ReservoirResultHandler*>(result_handler_.get());
//
//            const SizeT bs_x = faiss::distance_compute_blas_query_bs;
//            for (SizeT i0 = 0; i0 < query_count_; i0 += bs_x) {
//                SizeT i1 = i0 + bs_x;
//                if (i1 > query_count_)
//                    i1 = query_count_;
//
//                result_handler->begin_multiple(i0, i1);
//            }
//        }
//    }

    begin_ = true;
}

template<typename DistType>
void
KnnFlatIP<DistType>::Search(const DistType* base,
                            i64 base_count,
                            i32 segment_id) {
    if(!begin_) {
        ExecutorError("KnnFlatInnerProductInternal isn't begin")
    }

    for (int64_t i = 0; i < query_count_; i++) {
        const float* x_i = queries_ + i * dimension_;
        const float* y_j = base;

        for (i32 j = 0; j < base_count; j++, y_j += dimension_) {
            float ip = faiss::fvec_inner_product(x_i, y_j, dimension_);
            single_heap_result_handler_->add_result(ip, CompoundID{segment_id, j}, i);
        }
    }

    //    if(top_k_ < faiss::distance_compute_min_k_reservoir) {
//        HeapSingleResultHandler* single_result_handler
//                = static_cast<HeapSingleResultHandler*>(single_result_handler_.get());
//
//        for (int64_t i = 0; i < query_count_; i++) {
//            const float* x_i = queries_ + i * dimension_;
//            const float* y_j = base;
//
//            for (i32 j = 0; j < base_count; j++, y_j += dimension_) {
//                float ip = faiss::fvec_inner_product(x_i, y_j, dimension_);
//                single_result_handler->add_result(ip, CompoundID{segment_id, j}, i);
//            }
//        }
//    }
}

//void
//KnnFlatIP::KnnInnerProductInternalBlas(const float* base,
//                                                 i64 base_count,
//                                                 i32 segment_id) {
//    if(!begin_) {
//        const SizeT bs_x = faiss::distance_compute_blas_query_bs;
//        const size_t bs_y = faiss::distance_compute_blas_database_bs;
//        std::unique_ptr<float[]> ip_block(new float[bs_x * bs_y]);
//        for (size_t i0 = 0; i0 < query_count_; i0 += bs_x) {
//            size_t i1 = i0 + bs_x;
//            if (i1 > query_count_)
//                i1 = query_count_;
//
//            for (size_t j0 = 0; j0 < base_count; j0 += bs_y) {
//                size_t j1 = j0 + bs_y;
//                if (j1 > base_count)
//                    j1 = base_count;
//                /* compute the actual dot products */
//                {
//                    float one = 1, zero = 0;
//                    FINTEGER nyi = j1 - j0, nxi = i1 - i0, di = dimension_;
//                    sgemm_("Transpose",
//                           "Not transpose",
//                           &nyi,
//                           &nxi,
//                           &di,
//                           &one,
//                           base + j0 * dimension_,
//                           &di,
//                           queries_ + i0 * dimension_,
//                           &di,
//                           &zero,
//                           ip_block.get(),
//                           &nyi);
//                }
//
//                result_handler_->add_results(i0, i1, j0, j1, ip_block.get(), segment_id);
//            }
//        }
//    }
//}

template<typename DistType>
void
KnnFlatIP<DistType>::End() {
    if(!begin_) return ;

    for(i32 i = 0; i < query_count_; ++ i) {
        single_heap_result_handler_->end(i);
    }

//    if(top_k_ < faiss::distance_compute_min_k_reservoir) {
//        if(query_count_ < faiss::distance_compute_blas_threshold) {
//            HeapSingleResultHandler* single_result_handler
//                    = static_cast<HeapSingleResultHandler*>(single_result_handler_.get());
//
//            for(i32 i = 0; i < query_count_; ++ i) {
//                single_result_handler->end(i);
//            }
//        } else {
//            HeapResultHandler* result_handler = static_cast<HeapResultHandler*>(result_handler_.get());
//
//            const SizeT bs_x = faiss::distance_compute_blas_query_bs;
//            for (SizeT i0 = 0; i0 < query_count_; i0 += bs_x) {
//                SizeT i1 = i0 + bs_x;
//                if (i1 > query_count_)
//                    i1 = query_count_;
//
//                result_handler->end_multiple(i0, i1);
//            }
//        }
//    } else {
//        if(query_count_ < faiss::distance_compute_blas_threshold) {
//            ReservoirSingleResultHandler* single_result_handler
//                    = static_cast<ReservoirSingleResultHandler*>(single_result_handler_.get());
//
//            for(i32 i = 0; i < query_count_; ++ i) {
//                single_result_handler->end(i);
//            }
//        } else {
//            ReservoirResultHandler* result_handler = static_cast<ReservoirResultHandler*>(result_handler_.get());
//
//            const SizeT bs_x = faiss::distance_compute_blas_query_bs;
//            for (SizeT i0 = 0; i0 < query_count_; i0 += bs_x) {
//                SizeT i1 = i0 + bs_x;
//                if (i1 > query_count_)
//                    i1 = query_count_;
//
//                result_handler->end_multiple(i0, i1);
//            }
//        }
//    }

    begin_ = false;
}

}