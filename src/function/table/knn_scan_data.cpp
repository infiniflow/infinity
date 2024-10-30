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

#include <string>
module knn_scan_data;

import stl;
import infinity_exception;
import third_party;
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
import logger;
import simd_functions;

namespace infinity {

template <>
void KnnDistance1<f32, f32>::InitKnnDistance1(KnnDistanceType dist_type) {
    switch (dist_type) {
        case KnnDistanceType::kL2: {
            dist_func_ = GetSIMD_FUNCTIONS().L2Distance_func_ptr_;
            break;
        }
        case KnnDistanceType::kCosine: {
            dist_func_ = GetSIMD_FUNCTIONS().CosineDistance_func_ptr_;
            break;
        }
        case KnnDistanceType::kInnerProduct: {
            dist_func_ = GetSIMD_FUNCTIONS().IPDistance_func_ptr_;
            break;
        }
        default: {
            Status status = Status::NotSupport(fmt::format("KnnDistanceType: {} is not support.", (i32)dist_type));
            RecoverableError(status);
        }
    }
}

template <>
void KnnDistance1<u8, i32>::InitKnnDistance1(KnnDistanceType dist_type) {
    switch (dist_type) {
        case KnnDistanceType::kL2: {
            dist_func_ = GetSIMD_FUNCTIONS().HNSW_U8L2_ptr_;
            break;
        }
        case KnnDistanceType::kInnerProduct: {
            dist_func_ = GetSIMD_FUNCTIONS().HNSW_U8IP_ptr_;
            break;
        }
        default: {
            Status status = Status::NotSupport(fmt::format("KnnDistanceType: {} is not support.", (i32)dist_type));
            RecoverableError(status);
        }
    }
}

f32 hnsw_u8l2_f32_wrapper(const u8 *v1, const u8 *v2, SizeT dim) { return static_cast<f32>(GetSIMD_FUNCTIONS().HNSW_U8L2_ptr_(v1, v2, dim)); }
f32 hnsw_u8ip_f32_wrapper(const u8 *v1, const u8 *v2, SizeT dim) { return static_cast<f32>(GetSIMD_FUNCTIONS().HNSW_U8IP_ptr_(v1, v2, dim)); }

template <>
void KnnDistance1<u8, f32>::InitKnnDistance1(KnnDistanceType dist_type) {
    switch (dist_type) {
        case KnnDistanceType::kL2: {
            dist_func_ = &hnsw_u8l2_f32_wrapper;
            break;
        }
        case KnnDistanceType::kCosine: {
            dist_func_ = GetSIMD_FUNCTIONS().HNSW_U8Cos_ptr_;
            break;
        }
        case KnnDistanceType::kInnerProduct: {
            dist_func_ = &hnsw_u8ip_f32_wrapper;
            break;
        }
        case KnnDistanceType::kHamming: {
            dist_func_ = GetSIMD_FUNCTIONS().HammingDistance_func_ptr_;
            break;
        }
        default: {
            Status status = Status::NotSupport(fmt::format("KnnDistanceType: {} is not support.", (i32)dist_type));
            RecoverableError(status);
        }
    }
}

template <>
void KnnDistance1<i8, i32>::InitKnnDistance1(KnnDistanceType dist_type) {
    switch (dist_type) {
        case KnnDistanceType::kL2: {
            dist_func_ = GetSIMD_FUNCTIONS().HNSW_I8L2_ptr_;
            break;
        }
        case KnnDistanceType::kInnerProduct: {
            dist_func_ = GetSIMD_FUNCTIONS().HNSW_I8IP_ptr_;
            break;
        }
        default: {
            Status status = Status::NotSupport(fmt::format("KnnDistanceType: {} is not support.", (i32)dist_type));
            RecoverableError(status);
        }
    }
}

f32 hnsw_i8l2_f32_wrapper(const i8 *v1, const i8 *v2, SizeT dim) { return static_cast<f32>(GetSIMD_FUNCTIONS().HNSW_I8L2_ptr_(v1, v2, dim)); }
f32 hnsw_i8ip_f32_wrapper(const i8 *v1, const i8 *v2, SizeT dim) { return static_cast<f32>(GetSIMD_FUNCTIONS().HNSW_I8IP_ptr_(v1, v2, dim)); }

template <>
void KnnDistance1<i8, f32>::InitKnnDistance1(KnnDistanceType dist_type) {
    switch (dist_type) {
        case KnnDistanceType::kL2: {
            dist_func_ = &hnsw_i8l2_f32_wrapper;
            break;
        }
        case KnnDistanceType::kCosine: {
            dist_func_ = GetSIMD_FUNCTIONS().HNSW_I8Cos_ptr_;
            break;
        }
        case KnnDistanceType::kInnerProduct: {
            dist_func_ = &hnsw_i8ip_f32_wrapper;
            break;
        }
        default: {
            Status status = Status::NotSupport(fmt::format("KnnDistanceType: {} is not support.", (i32)dist_type));
            RecoverableError(status);
        }
    }
}

// --------------------------------------------

KnnScanFunctionData::KnnScanFunctionData(KnnScanSharedData *shared_data, u32 current_parallel_idx, bool execute_block_scan_job)
    : knn_scan_shared_data_(shared_data), task_id_(current_parallel_idx), execute_block_scan_job_(execute_block_scan_job) {
    switch (knn_scan_shared_data_->query_elem_type_) {
        case EmbeddingDataType::kElemFloat: {
            Init<f32, f32>();
            break;
        }
        case EmbeddingDataType::kElemUInt8: {
            Init<u8, f32>();
            break;
        }
        case EmbeddingDataType::kElemInt8: {
            Init<i8, f32>();
            break;
        }
        case EmbeddingDataType::kElemBit: {
            Init<u8, f32>();
            break;
        }
        default: {
            Status status = Status::NotSupport(fmt::format("Query EmbeddingDataType: {} is not support.",
                                                           EmbeddingType::EmbeddingDataType2String(knn_scan_shared_data_->query_elem_type_)));
            RecoverableError(status);
        }
    }
}

template <typename QueryDataType, typename DistDataType>
void KnnScanFunctionData::Init() {
    static_assert(std::is_same_v<DistDataType, f32>);
    switch (knn_scan_shared_data_->knn_distance_type_) {
        case KnnDistanceType::kInvalid: {
            String error_message = "Invalid Knn distance type";
            UnrecoverableError(error_message);
        }
        case KnnDistanceType::kL2:
        case KnnDistanceType::kHamming: {
            const auto knn_threshold = GetKnnThreshold(knn_scan_shared_data_->opt_params_);
            auto merge_knn_max = MakeUnique<MergeKnn<QueryDataType, CompareMax, DistDataType>>(knn_scan_shared_data_->query_count_,
                                                                                               knn_scan_shared_data_->topk_,
                                                                                               knn_threshold);
            merge_knn_max->Begin();
            merge_knn_base_ = std::move(merge_knn_max);
            break;
        }
        case KnnDistanceType::kCosine:
        case KnnDistanceType::kInnerProduct: {
            const auto knn_threshold = GetKnnThreshold(knn_scan_shared_data_->opt_params_);
            auto merge_knn_min = MakeUnique<MergeKnn<QueryDataType, CompareMin, DistDataType>>(knn_scan_shared_data_->query_count_,
                                                                                               knn_scan_shared_data_->topk_,
                                                                                               knn_threshold);
            merge_knn_min->Begin();
            merge_knn_base_ = std::move(merge_knn_min);
            break;
        }
    }

    knn_distance_ = MakeUnique<KnnDistance1<QueryDataType, DistDataType>>(knn_scan_shared_data_->knn_distance_type_);
}

} // namespace infinity
