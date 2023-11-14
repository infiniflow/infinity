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

import stl;
import parser;

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

import merge_knn;
import faiss;

module knn_scan_data;

namespace infinity {

// void KnnScanFunctionData::Init() {
//     switch (knn_distance_type_) {
//         case KnnDistanceType::kInvalid: {
//             Error<ExecutorException>("Invalid Knn distance type");
//         }
//         case KnnDistanceType::kL2: {
//             switch (elem_type_) {

//                 case kElemBit:
//                     break;
//                 case kElemInt8:
//                     break;
//                 case kElemInt16:
//                     break;
//                 case kElemInt32:
//                     break;
//                 case kElemInt64:
//                     break;
//                 case kElemFloat: {
//                     if (query_embedding_count_ < faiss_distance_compute_blas_threshold) {
//                         if (topk_ == 1) {
//                             auto knn_flat_l2 =
//                                 MakeUnique<KnnFlatL2Top1<f32>>((f32 *)query_embedding_, query_embedding_count_, dimension_, elem_type_);
//                             knn_flat_l2->Begin();
//                             knn_distance_ = Move(knn_flat_l2);
//                         } else if (topk_ < faiss_distance_compute_min_k_reservoir) {
//                             auto knn_flat_l2 =
//                                 MakeUnique<KnnFlatL2<f32>>((f32 *)query_embedding_, query_embedding_count_, topk_, dimension_, elem_type_);

//                             knn_flat_l2->Begin();
//                             knn_distance_ = Move(knn_flat_l2);
//                         } else {
//                             auto knn_flat_l2 =
//                                 MakeUnique<KnnFlatL2Reservoir<f32>>((f32 *)query_embedding_, query_embedding_count_, topk_, dimension_, elem_type_);

//                             knn_flat_l2->Begin();
//                             knn_distance_ = Move(knn_flat_l2);
//                         }

//                     } else {
//                         if (topk_ == 1) {
//                             auto knn_flat_l2 =
//                                 MakeUnique<KnnFlatL2Top1Blas<f32>>((f32 *)query_embedding_, query_embedding_count_, dimension_, elem_type_);
//                             knn_flat_l2->Begin();
//                             knn_distance_ = Move(knn_flat_l2);
//                         } else if (topk_ < faiss_distance_compute_min_k_reservoir) {
//                             auto knn_flat_l2 =
//                                 MakeUnique<KnnFlatL2Blas<f32>>((f32 *)query_embedding_, query_embedding_count_, topk_, dimension_, elem_type_);

//                             knn_flat_l2->Begin();
//                             knn_distance_ = Move(knn_flat_l2);
//                         } else {
//                             auto knn_flat_l2 = MakeUnique<KnnFlatL2BlasReservoir<f32>>((f32 *)query_embedding_,
//                                                                                        query_embedding_count_,
//                                                                                        topk_,
//                                                                                        dimension_,
//                                                                                        elem_type_);

//                             knn_flat_l2->Begin();
//                             knn_distance_ = Move(knn_flat_l2);
//                         }
//                     }
//                     break;
//                 }
//                 case kElemDouble:
//                     break;
//                 case kElemInvalid: {
//                     Error<ExecutorException>("Invalid embedding data type");
//                 }
//             }

//             break;
//         }
//         case KnnDistanceType::kCosine: {
//             Error<ExecutorException>("Not implemented cosine");
//         }
//         case KnnDistanceType::kInnerProduct: {
//             switch (elem_type_) {

//                 case kElemBit:
//                     break;
//                 case kElemInt8:
//                     break;
//                 case kElemInt16:
//                     break;
//                 case kElemInt32:
//                     break;
//                 case kElemInt64:
//                     break;
//                 case kElemFloat: {
//                     if (query_embedding_count_ < faiss_distance_compute_blas_threshold) {
//                         if (topk_ < faiss_distance_compute_min_k_reservoir) {
//                             auto knn_flat_inner_product =
//                                 MakeUnique<KnnFlatIP<f32>>((f32 *)query_embedding_, query_embedding_count_, topk_, dimension_, elem_type_);

//                             knn_flat_inner_product->Begin();
//                             knn_distance_ = Move(knn_flat_inner_product);
//                         } else {
//                             auto knn_flat_inner_product =
//                                 MakeUnique<KnnFlatIPReservoir<f32>>((f32 *)query_embedding_, query_embedding_count_, topk_, dimension_, elem_type_);

//                             knn_flat_inner_product->Begin();
//                             knn_distance_ = Move(knn_flat_inner_product);
//                         }

//                     } else {
//                         if (topk_ < faiss_distance_compute_min_k_reservoir) {
//                             auto knn_flat_inner_product =
//                                 MakeUnique<KnnFlatIPBlas<f32>>((f32 *)query_embedding_, query_embedding_count_, topk_, dimension_, elem_type_);

//                             knn_flat_inner_product->Begin();
//                             knn_distance_ = Move(knn_flat_inner_product);
//                         } else {
//                             auto knn_flat_inner_product = MakeUnique<KnnFlatIPBlasReservoir<f32>>((f32 *)query_embedding_,
//                                                                                                   query_embedding_count_,
//                                                                                                   topk_,
//                                                                                                   dimension_,
//                                                                                                   elem_type_);

//                             knn_flat_inner_product->Begin();
//                             knn_distance_ = Move(knn_flat_inner_product);
//                         }
//                     }

//                     break;
//                 }
//                 case kElemDouble:
//                     break;
//                 case kElemInvalid: {
//                     Error<ExecutorException>("Invalid embedding data type");
//                 }
//             }

//             break;
//         }
//         case KnnDistanceType::kHamming: {
//             Error<ExecutorException>("Not implemented Hamming");
//         }
//     }
// }

KnnScanFunctionData1::KnnScanFunctionData1(SharedPtr<KnnScanSharedData> shared_data) : shared_data_(shared_data) {
    switch (shared_data_->elem_type_) {
        case kElemInvalid: {
            Error<ExecutorException>("Invalid element type");
        }
        case kElemFloat: {
            KnnScanFunctionData1::InitKnnScan<f32>();
            break;
        }
        default: {
            Error<ExecutorException>("Not implemented");
        }
    }
}

template <typename DataType>
void KnnScanFunctionData1::InitKnnScan() {
    switch (shared_data_->knn_distance_type_) {
        case KnnDistanceType::kInvalid: {
            throw ExecutorException("Invalid Knn distance type");
        }
        case KnnDistanceType::kL2:
        case KnnDistanceType::kHamming: {
            auto merge_knn_max = MakeUnique<MergeKnn<DataType, FaissCMax>>(shared_data_->query_count_, shared_data_->topk_);
            merge_knn_max->Begin();
            merge_knn_base_ = Move(merge_knn_max);
            break;
        }
        case KnnDistanceType::kCosine:
        case KnnDistanceType::kInnerProduct: {
            auto merge_knn_min = MakeUnique<MergeKnn<DataType, FaissCMin>>(shared_data_->query_count_, shared_data_->topk_);
            merge_knn_min->Begin();
            merge_knn_base_ = Move(merge_knn_min);
            break;
        }
    }
}

} // namespace infinity
