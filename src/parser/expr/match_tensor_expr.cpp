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

#include "match_tensor_expr.h"
#include "spdlog/fmt/fmt.h"

namespace infinity {

std::string MatchTensorExpr::MethodToString(MatchTensorMethod method) {
    switch (method) {
        case MatchTensorMethod::kInvalid:
            return "INVALID";
        case MatchTensorMethod::kMaxSim:
            return "MAX_SIM";
    }
}

MatchTensorExpr::~MatchTensorExpr() {
    if (column_expr_ != nullptr) {
        delete column_expr_;
        column_expr_ = nullptr;
    }

    if (!own_memory_) {
        return;
    }
    if (embedding_data_ptr_ != nullptr) {
        switch (embedding_data_type_) {
            case EmbeddingDataType::kElemDouble: {
                double *data_ptr = reinterpret_cast<double *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemFloat: {
                float *data_ptr = reinterpret_cast<float *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemBit: {
                uint8_t *data_ptr = reinterpret_cast<uint8_t *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemInt8: {
                int8_t *data_ptr = reinterpret_cast<int8_t *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemInt16: {
                int16_t *data_ptr = reinterpret_cast<int16_t *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemInt32: {
                int32_t *data_ptr = reinterpret_cast<int32_t *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemInt64: {
                int64_t *data_ptr = reinterpret_cast<int64_t *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemInvalid: {
                //                LOG_CRITICAL("Unexpected embedding data type")
                int8_t *data_ptr = reinterpret_cast<int8_t *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
        }
        embedding_data_ptr_ = nullptr;
    }
}

std::string MatchTensorExpr::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }
    const EmbeddingType query_embedding((char *)embedding_data_ptr_, false);
    if (options_text_.empty()) {
        return fmt::format("MATCH TENSOR ({}, {}, {})",
                           column_expr_->ToString(),
                           EmbeddingType::Embedding2String(query_embedding, embedding_data_type_, dimension_),
                           MethodToString(search_method_));
    }
    return fmt::format("MATCH TENSOR ({}, {}, {}, EXTRA OPTION : '{}')",
                       column_expr_->ToString(),
                       EmbeddingType::Embedding2String(query_embedding, embedding_data_type_, dimension_),
                       MethodToString(search_method_),
                       options_text_);
}

} // namespace infinity
