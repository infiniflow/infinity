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

module knn_scan_data;

import stl;

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
import logical_type;
import merge_knn;
import knn_result_handler;
import vector_distance;
import data_block;
import column_vector;
import base_expression;
import expression_state;
import internal_types;
import data_type;
import status;

namespace infinity {

template <>
KnnDistance1<f32>::KnnDistance1(KnnDistanceType dist_type) {
    switch (dist_type) {
        case KnnDistanceType::kL2: {
            dist_func_ = L2Distance<f32, f32, f32, SizeT>;
            break;
        }
        case KnnDistanceType::kInnerProduct: {
            dist_func_ = IPDistance<f32, f32, f32, SizeT>;
            break;
        }
        default: {
            RecoverableError(Status::NotSupport(fmt::format("KnnDistanceType: {} is not support.", (i32)dist_type)));
        }
    }
}

// --------------------------------------------

KnnScanFunctionData::KnnScanFunctionData(KnnScanSharedData *shared_data, u32 current_parallel_idx)
    : knn_scan_shared_data_(shared_data), task_id_(current_parallel_idx) {
    switch (knn_scan_shared_data_->elem_type_) {
        case EmbeddingDataType::kElemFloat: {
            Init<f32>();
            break;
        }
        default: {
            RecoverableError(Status::NotSupport(
                fmt::format("EmbeddingDataType: {} is not support.", EmbeddingType::EmbeddingDataType2String(knn_scan_shared_data_->elem_type_))));
        }
    }
}

template <typename DataType>
void KnnScanFunctionData::Init() {
    switch (knn_scan_shared_data_->knn_distance_type_) {
        case KnnDistanceType::kInvalid: {
            UnrecoverableError("Invalid Knn distance type");
        }
        case KnnDistanceType::kL2:
        case KnnDistanceType::kHamming: {
            auto merge_knn_max = MakeUnique<MergeKnn<DataType, CompareMax>>(knn_scan_shared_data_->query_count_, knn_scan_shared_data_->topk_);
            merge_knn_max->Begin();
            merge_knn_base_ = std::move(merge_knn_max);
            break;
        }
        case KnnDistanceType::kCosine:
        case KnnDistanceType::kInnerProduct: {
            auto merge_knn_min = MakeUnique<MergeKnn<DataType, CompareMin>>(knn_scan_shared_data_->query_count_, knn_scan_shared_data_->topk_);
            merge_knn_min->Begin();
            merge_knn_base_ = std::move(merge_knn_min);
            break;
        }
    }

    knn_distance_ = MakeUnique<KnnDistance1<DataType>>(knn_scan_shared_data_->knn_distance_type_);
}

} // namespace infinity
