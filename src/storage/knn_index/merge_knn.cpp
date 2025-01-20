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
#include <vector>
module merge_knn;

import stl;
import statement_common;
import knn_scan_data;
import status;
import knn_expr;
import third_party;

namespace infinity {

namespace {

template <typename QueryDataType, typename DistDataType>
UniquePtr<MergeKnnBase> InitMergeKnn(KnnScanSharedData *knn_scan_shared_data) {
    switch (knn_scan_shared_data->knn_distance_type_) {
        case KnnDistanceType::kInvalid: {
            String error_message = "Invalid Knn distance type";
            UnrecoverableError(error_message);
            return nullptr;
        }
        case KnnDistanceType::kL2:
        case KnnDistanceType::kHamming: {
            const auto knn_threshold = GetKnnThreshold(knn_scan_shared_data->opt_params_);
            auto ret = MakeUnique<MergeKnn<QueryDataType, CompareMax, DistDataType>>(knn_scan_shared_data->query_count_,
                                                                                     knn_scan_shared_data->topk_,
                                                                                     knn_threshold);
            ret->Begin();
            return ret;
        }
        case KnnDistanceType::kCosine:
        case KnnDistanceType::kInnerProduct: {
            const auto knn_threshold = GetKnnThreshold(knn_scan_shared_data->opt_params_);
            auto ret = MakeUnique<MergeKnn<QueryDataType, CompareMin, DistDataType>>(knn_scan_shared_data->query_count_,
                                                                                     knn_scan_shared_data->topk_,
                                                                                     knn_threshold);
            ret->Begin();
            return ret;
        }
    }
}

} // namespace

UniquePtr<MergeKnnBase> MergeKnnBase::Make(KnnScanSharedData *knn_scan_shared_data) {
    switch (knn_scan_shared_data->query_elem_type_) {
        case EmbeddingDataType::kElemFloat:
            return InitMergeKnn<f32, f32>(knn_scan_shared_data);
        case EmbeddingDataType::kElemUInt8:
            return InitMergeKnn<u8, f32>(knn_scan_shared_data);
        case EmbeddingDataType::kElemInt8:
            return InitMergeKnn<i8, f32>(knn_scan_shared_data);
        case EmbeddingDataType::kElemBit:
            return InitMergeKnn<u8, f32>(knn_scan_shared_data);
        default: {
            Status status = Status::NotSupport(fmt::format("Query EmbeddingDataType: {} is not support.",
                                                           EmbeddingType::EmbeddingDataType2String(knn_scan_shared_data->query_elem_type_)));
            RecoverableError(status);
            return nullptr;
        }
    }
}

Optional<f32> GetKnnThreshold(const Vector<InitParameter> &opt_params) {
    Optional<f32> knn_threshold;
    for (const auto &opt_param : opt_params) {
        if (opt_param.param_name_ == "threshold") {
            knn_threshold = std::stof(opt_param.param_value_);
        }
    }
    return knn_threshold;
}

Optional<f32> GetKnnThreshold(const Vector<UniquePtr<InitParameter>> &opt_params) {
    Optional<f32> knn_threshold;
    for (const auto &opt_param : opt_params) {
        if (opt_param->param_name_ == "threshold") {
            knn_threshold = std::stof(opt_param->param_value_);
        }
    }
    return knn_threshold;
}

} // namespace infinity