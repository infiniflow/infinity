//
// Created by jinhai on 23-9-26.
//

#include "knn_scan_data.h"
#include "storage/knnindex/knn_flat/knn_flat_ip.h"
#include "storage/knnindex/knn_flat/knn_flat_ip_blas.h"
#include "storage/knnindex/knn_flat/knn_flat_ip_blas_reservoir.h"
#include "storage/knnindex/knn_flat/knn_flat_ip_reservoir.h"
#include "storage/knnindex/knn_flat/knn_flat_l2.h"
#include "storage/knnindex/knn_flat/knn_flat_l2_blas.h"
#include "storage/knnindex/knn_flat/knn_flat_l2_blas_reservoir.h"
#include "storage/knnindex/knn_flat/knn_flat_l2_reservoir.h"
#include "storage/knnindex/knn_flat/knn_flat_l2_top1.h"
#include "storage/knnindex/knn_flat/knn_flat_l2_top1_blas.h"

namespace infinity {

void KnnScanFunctionData::Init() {
    switch (knn_distance_type_) {
        case KnnDistanceType::kInvalid: {
            SchedulerError("Invalid Knn distance type")
        }
        case KnnDistanceType::kL2: {
            switch (elem_type_) {

                case kElemBit:
                    break;
                case kElemInt8:
                    break;
                case kElemInt16:
                    break;
                case kElemInt32:
                    break;
                case kElemInt64:
                    break;
                case kElemFloat: {
                    if (query_embedding_count_ < faiss::distance_compute_blas_threshold) {
                        if (topk_ == 1) {
                            auto knn_flat_l2 =
                                MakeUnique<KnnFlatL2Top1<f32>>((f32 *)query_embedding_, query_embedding_count_, dimension_, elem_type_);
                            knn_flat_l2->Begin();
                            knn_distance_ = std::move(knn_flat_l2);
                        } else if (topk_ < faiss::distance_compute_min_k_reservoir) {
                            auto knn_flat_l2 =
                                MakeUnique<KnnFlatL2<f32>>((f32 *)query_embedding_, query_embedding_count_, topk_, dimension_, elem_type_);

                            knn_flat_l2->Begin();
                            knn_distance_ = std::move(knn_flat_l2);
                        } else {
                            auto knn_flat_l2 =
                                MakeUnique<KnnFlatL2Reservoir<f32>>((f32 *)query_embedding_, query_embedding_count_, topk_, dimension_, elem_type_);

                            knn_flat_l2->Begin();
                            knn_distance_ = std::move(knn_flat_l2);
                        }

                    } else {
                        if (topk_ == 1) {
                            auto knn_flat_l2 =
                                MakeUnique<KnnFlatL2Top1Blas<f32>>((f32 *)query_embedding_, query_embedding_count_, dimension_, elem_type_);
                            knn_flat_l2->Begin();
                            knn_distance_ = std::move(knn_flat_l2);
                        } else if (topk_ < faiss::distance_compute_min_k_reservoir) {
                            auto knn_flat_l2 =
                                MakeUnique<KnnFlatL2Blas<f32>>((f32 *)query_embedding_, query_embedding_count_, topk_, dimension_, elem_type_);

                            knn_flat_l2->Begin();
                            knn_distance_ = std::move(knn_flat_l2);
                        } else {
                            auto knn_flat_l2 = MakeUnique<KnnFlatL2BlasReservoir<f32>>((f32 *)query_embedding_,
                                                                                       query_embedding_count_,
                                                                                       topk_,
                                                                                       dimension_,
                                                                                       elem_type_);

                            knn_flat_l2->Begin();
                            knn_distance_ = std::move(knn_flat_l2);
                        }
                    }
                    break;
                }
                case kElemDouble:
                    break;
                case kElemInvalid: {
                    SchedulerError("Invalid embedding data type")
                }
            }

            break;
        }
        case KnnDistanceType::kCosine: {
            SchedulerError("Not implemented Cosine")
        }
        case KnnDistanceType::kInnerProduct: {
            switch (elem_type_) {

                case kElemBit:
                    break;
                case kElemInt8:
                    break;
                case kElemInt16:
                    break;
                case kElemInt32:
                    break;
                case kElemInt64:
                    break;
                case kElemFloat: {
                    if (query_embedding_count_ < faiss::distance_compute_blas_threshold) {
                        if (topk_ < faiss::distance_compute_min_k_reservoir) {
                            auto knn_flat_inner_product =
                                MakeUnique<KnnFlatIP<f32>>((f32 *)query_embedding_, query_embedding_count_, topk_, dimension_, elem_type_);

                            knn_flat_inner_product->Begin();
                            knn_distance_ = std::move(knn_flat_inner_product);
                        } else {
                            auto knn_flat_inner_product =
                                MakeUnique<KnnFlatIPReservoir<f32>>((f32 *)query_embedding_, query_embedding_count_, topk_, dimension_, elem_type_);

                            knn_flat_inner_product->Begin();
                            knn_distance_ = std::move(knn_flat_inner_product);
                        }

                    } else {
                        if (topk_ < faiss::distance_compute_min_k_reservoir) {
                            auto knn_flat_inner_product =
                                MakeUnique<KnnFlatIPBlas<f32>>((f32 *)query_embedding_, query_embedding_count_, topk_, dimension_, elem_type_);

                            knn_flat_inner_product->Begin();
                            knn_distance_ = std::move(knn_flat_inner_product);
                        } else {
                            auto knn_flat_inner_product = MakeUnique<KnnFlatIPBlasReservoir<f32>>((f32 *)query_embedding_,
                                                                                                  query_embedding_count_,
                                                                                                  topk_,
                                                                                                  dimension_,
                                                                                                  elem_type_);

                            knn_flat_inner_product->Begin();
                            knn_distance_ = std::move(knn_flat_inner_product);
                        }
                    }

                    break;
                }
                case kElemDouble:
                    break;
                case kElemInvalid: {
                    SchedulerError("Invalid embedding data type")
                }
            }

            break;
        }
        case KnnDistanceType::kHamming: {
            SchedulerError("Not implemented Hamming")
        }
    }
}

} // namespace infinity
