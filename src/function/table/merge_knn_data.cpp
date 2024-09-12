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

import base_table_ref;

import infinity_exception;
import merge_knn;
import knn_result_handler;
import status;
import logger;

module merge_knn_data;

namespace infinity {

MergeKnnFunctionData::MergeKnnFunctionData(i64 query_count,
                                           i64 topk,
                                           EmbeddingDataType elem_type,
                                           KnnDistanceType knn_distance_type,
                                           SharedPtr<BaseTableRef> table_ref)
    : query_count_(query_count), topk_(topk), elem_type_(elem_type), table_ref_(table_ref) {
    switch (elem_type) {
        case EmbeddingDataType::kElemInvalid: {
            String error_message = "Invalid element type";
            UnrecoverableError(error_message);
        }
        case EmbeddingDataType::kElemUInt8:
        case EmbeddingDataType::kElemInt8:
        case EmbeddingDataType::kElemFloat: {
            MergeKnnFunctionData::InitMergeKnn<f32, f32>(knn_distance_type);
            break;
        }
        default: {
            Status status = Status::NotSupport("Not implemented");
            RecoverableError(status);
        }
    }
}

template <typename DatType, typename DistType>
void MergeKnnFunctionData::InitMergeKnn(KnnDistanceType knn_distance_type) {
    switch (knn_distance_type) {
        case KnnDistanceType::kInvalid: {
            String error_message = "Invalid knn distance type";
            UnrecoverableError(error_message);
        }
        case KnnDistanceType::kL2:
        case KnnDistanceType::kHamming: {
            auto merge_knn_max = MakeShared<MergeKnn<DatType, CompareMax, DistType>>(query_count_, topk_);
            merge_knn_max->Begin();
            merge_knn_base_ = std::move(merge_knn_max);
            heap_type_ = MergeKnnHeapType::kMaxHeap;
            break;
        }
        case KnnDistanceType::kCosine:
        case KnnDistanceType::kInnerProduct: {
            auto merge_knn_min = MakeShared<MergeKnn<DatType, CompareMin, DistType>>(query_count_, topk_);
            merge_knn_min->Begin();
            merge_knn_base_ = std::move(merge_knn_min);
            heap_type_ = MergeKnnHeapType::kMinHeap;
            break;
        }
    }
}
} // namespace infinity