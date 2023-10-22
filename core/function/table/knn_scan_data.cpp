//
// Created by jinhai on 23-9-26.
//

module;

import stl;
import parser;
import infinity_assert;
import infinity_exception;
import third_party;
import knn_flat_ip;
import knn_flat_ip_blas;
import knn_flat_ip_blas_reservoir;
import knn_flat_ip_reservoir;
import knn_flat_l2;
import knn_flat_l2_blas;
import knn_flat_l2_blas_reservoir;
import knn_flat_l2_reservoir;
import knn_flat_l2_top1;
import knn_flat_l2_top1_blas;

module knn_scan_data;

namespace infinity {

void KnnScanFunctionData::Init() {
    switch (knn_distance_type_) {
        case KnnDistanceType::kInvalid: {
            Error<ExecutorException>("Invalid Knn distance type", __FILE_NAME__, __LINE__);
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
                    if (query_embedding_count_ < faiss_distance_compute_blas_threshold) {
                        if (topk_ == 1) {
                            auto knn_flat_l2 =
                                MakeUnique<KnnFlatL2Top1<f32>>((f32 *)query_embedding_, query_embedding_count_, dimension_, elem_type_);
                            knn_flat_l2->Begin();
                            knn_distance_ = Move(knn_flat_l2);
                        } else if (topk_ < faiss_distance_compute_min_k_reservoir) {
                            auto knn_flat_l2 =
                                MakeUnique<KnnFlatL2<f32>>((f32 *)query_embedding_, query_embedding_count_, topk_, dimension_, elem_type_);

                            knn_flat_l2->Begin();
                            knn_distance_ = Move(knn_flat_l2);
                        } else {
                            auto knn_flat_l2 =
                                MakeUnique<KnnFlatL2Reservoir<f32>>((f32 *)query_embedding_, query_embedding_count_, topk_, dimension_, elem_type_);

                            knn_flat_l2->Begin();
                            knn_distance_ = Move(knn_flat_l2);
                        }

                    } else {
                        if (topk_ == 1) {
                            auto knn_flat_l2 =
                                MakeUnique<KnnFlatL2Top1Blas<f32>>((f32 *)query_embedding_, query_embedding_count_, dimension_, elem_type_);
                            knn_flat_l2->Begin();
                            knn_distance_ = Move(knn_flat_l2);
                        } else if (topk_ < faiss_distance_compute_min_k_reservoir) {
                            auto knn_flat_l2 =
                                MakeUnique<KnnFlatL2Blas<f32>>((f32 *)query_embedding_, query_embedding_count_, topk_, dimension_, elem_type_);

                            knn_flat_l2->Begin();
                            knn_distance_ = Move(knn_flat_l2);
                        } else {
                            auto knn_flat_l2 = MakeUnique<KnnFlatL2BlasReservoir<f32>>((f32 *)query_embedding_,
                                                                                       query_embedding_count_,
                                                                                       topk_,
                                                                                       dimension_,
                                                                                       elem_type_);

                            knn_flat_l2->Begin();
                            knn_distance_ = Move(knn_flat_l2);
                        }
                    }
                    break;
                }
                case kElemDouble:
                    break;
                case kElemInvalid: {
                    Error<ExecutorException>("Invalid embedding data type", __FILE_NAME__, __LINE__);
                }
            }

            break;
        }
        case KnnDistanceType::kCosine: {
            Error<ExecutorException>("Not implemented cosine", __FILE_NAME__, __LINE__);
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
                    if (query_embedding_count_ < faiss_distance_compute_blas_threshold) {
                        if (topk_ < faiss_distance_compute_min_k_reservoir) {
                            auto knn_flat_inner_product =
                                MakeUnique<KnnFlatIP<f32>>((f32 *)query_embedding_, query_embedding_count_, topk_, dimension_, elem_type_);

                            knn_flat_inner_product->Begin();
                            knn_distance_ = Move(knn_flat_inner_product);
                        } else {
                            auto knn_flat_inner_product =
                                MakeUnique<KnnFlatIPReservoir<f32>>((f32 *)query_embedding_, query_embedding_count_, topk_, dimension_, elem_type_);

                            knn_flat_inner_product->Begin();
                            knn_distance_ = Move(knn_flat_inner_product);
                        }

                    } else {
                        if (topk_ < faiss_distance_compute_min_k_reservoir) {
                            auto knn_flat_inner_product =
                                MakeUnique<KnnFlatIPBlas<f32>>((f32 *)query_embedding_, query_embedding_count_, topk_, dimension_, elem_type_);

                            knn_flat_inner_product->Begin();
                            knn_distance_ = Move(knn_flat_inner_product);
                        } else {
                            auto knn_flat_inner_product = MakeUnique<KnnFlatIPBlasReservoir<f32>>((f32 *)query_embedding_,
                                                                                                  query_embedding_count_,
                                                                                                  topk_,
                                                                                                  dimension_,
                                                                                                  elem_type_);

                            knn_flat_inner_product->Begin();
                            knn_distance_ = Move(knn_flat_inner_product);
                        }
                    }

                    break;
                }
                case kElemDouble:
                    break;
                case kElemInvalid: {
                    Error<ExecutorException>("Invalid embedding data type", __FILE_NAME__, __LINE__);
                }
            }

            break;
        }
        case KnnDistanceType::kHamming: {
            Error<ExecutorException>("Not implemented Hamming", __FILE_NAME__, __LINE__);
        }
    }
}

} // namespace infinity
