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
import knn_result_handler;
import vector_distance;
import data_block;
import column_vector;
import base_expression;
import expression_state;


module knn_scan_data;

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
            throw ExecutorException("Not implemented");
        }
    }
}

// --------------------------------------------

KnnScanFunctionData::KnnScanFunctionData(KnnScanSharedData* shared_data, u32 current_parallel_idx)
    : shared_data_(shared_data), task_id_(current_parallel_idx) {
    switch (shared_data_->elem_type_) {
        case EmbeddingDataType::kElemFloat: {
            Init<f32>();
            break;
        }
        default: {
            throw ExecutorException("Not implemented");
        }
    }
}

template <typename DataType>
void KnnScanFunctionData::Init() {
    switch (shared_data_->knn_distance_type_) {
        case KnnDistanceType::kInvalid: {
            throw ExecutorException("Invalid Knn distance type");
        }
        case KnnDistanceType::kL2:
        case KnnDistanceType::kHamming: {
            auto merge_knn_max = MakeUnique<MergeKnn<DataType, CompareMax>>(shared_data_->query_count_, shared_data_->topk_);
            merge_knn_max->Begin();
            merge_knn_base_ = Move(merge_knn_max);
            break;
        }
        case KnnDistanceType::kCosine:
        case KnnDistanceType::kInnerProduct: {
            auto merge_knn_min = MakeUnique<MergeKnn<DataType, CompareMin>>(shared_data_->query_count_, shared_data_->topk_);
            merge_knn_min->Begin();
            merge_knn_base_ = Move(merge_knn_min);
            break;
        }
    }

    knn_distance_ = MakeUnique<KnnDistance1<DataType>>(shared_data_->knn_distance_type_);

    if (shared_data_->filter_expression_) {
        filter_state_ = ExpressionState::CreateState(shared_data_->filter_expression_);
        db_for_filter_ = MakeUnique<DataBlock>();
        db_for_filter_->Init(*(shared_data_->table_ref_->column_types_));                         // default capacity
        bool_column_ = ColumnVector::Make(MakeShared<infinity::DataType>(LogicalType::kBoolean)); // default capacity
    }
}

} // namespace infinity
