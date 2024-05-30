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

module match_tensor_expression;

import stl;
import expression_type;
import internal_types;
import third_party;
import base_expression;
import column_expression;
import embedding_info;
import infinity_exception;
import status;
import logger;

namespace infinity {

MatchTensorExpression::MatchTensorExpression(Vector<SharedPtr<BaseExpression>> search_column,
                                             const MatchTensorSearchMethod search_method,
                                             const EmbeddingDataType embedding_data_type,
                                             const u32 dimension,
                                             EmbeddingT query_embedding,
                                             const u32 tensor_basic_embedding_dimension,
                                             const String &options_text)
    : BaseExpression(ExpressionType::kMatchTensor, std::move(search_column)), search_method_(search_method),
      embedding_data_type_(embedding_data_type), dimension_(dimension), query_embedding_(std::move(query_embedding)),
      tensor_basic_embedding_dimension_(tensor_basic_embedding_dimension), options_text_(options_text) {
    column_expr_ = static_cast<ColumnExpression *>(arguments_[0].get());
}

LogicalType GetCommonLogicalType(const EmbeddingDataType query_data_type, const EmbeddingDataType column_type);

DataType MatchTensorExpression::Type() const {
    auto *embedding_info = static_cast<const EmbeddingInfo *>(column_expr_->Type().type_info().get());
    return DataType(GetCommonLogicalType(embedding_data_type_, embedding_info->Type()));
}

String MatchTensorExpression::MethodToString(const MatchTensorSearchMethod method) { return MatchTensorExpr::SearchMethodToString(method); }

String MatchTensorExpression::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }
    auto tensor_str =
        TensorT::Tensor2String(query_embedding_.ptr, embedding_data_type_, tensor_basic_embedding_dimension_, num_of_embedding_in_query_tensor_);
    if (options_text_.empty()) {
        return fmt::format("MATCH TENSOR ({}, [{}], {})", column_expr_->Name(), tensor_str, MethodToString(search_method_));
    }
    return fmt::format("MATCH TENSOR ({}, [{}], {}, '{}')", column_expr_->Name(), tensor_str, MethodToString(search_method_), options_text_);
}

template <typename T, typename U>
LogicalType GetCommonLogicalType() {
    using CommonType = std::common_type_t<T, U>;
    if constexpr (std::is_same_v<CommonType, i8>) {
        return LogicalType::kTinyInt;
    } else if constexpr (std::is_same_v<CommonType, i16>) {
        return LogicalType::kSmallInt;
    } else if constexpr (std::is_same_v<CommonType, i32>) {
        return LogicalType::kInteger;
    } else if constexpr (std::is_same_v<CommonType, i64>) {
        return LogicalType::kBigInt;
    } else if constexpr (std::is_same_v<CommonType, f32>) {
        return LogicalType::kFloat;
    } else if constexpr (std::is_same_v<CommonType, f64>) {
        return LogicalType::kDouble;
    } else {
        static_assert(false, "Unsupported common data type!");
        Status status = Status::NotSupport("Unsupported common data type!");
        LOG_ERROR(status.message());
        RecoverableError(std::move(status));
        return LogicalType::kInvalid;
    }
}

template <typename T>
LogicalType GetCommonLogicalType(const EmbeddingDataType column_type) {
    switch (column_type) {
        case EmbeddingDataType::kElemInt8: {
            return GetCommonLogicalType<T, i8>();
        }
        case EmbeddingDataType::kElemInt16: {
            return GetCommonLogicalType<T, i16>();
        }
        case EmbeddingDataType::kElemInt32: {
            return GetCommonLogicalType<T, i32>();
        }
        case EmbeddingDataType::kElemInt64: {
            return GetCommonLogicalType<T, i64>();
        }
        case EmbeddingDataType::kElemFloat: {
            return GetCommonLogicalType<T, f32>();
        }
        case EmbeddingDataType::kElemDouble: {
            return GetCommonLogicalType<T, f64>();
        }
        case EmbeddingDataType::kElemBit:
        case EmbeddingDataType::kElemInvalid: {
            Status status = Status::NotSupport(fmt::format("Unsupported column data type: {}", EmbeddingT::EmbeddingDataType2String(column_type)));
            LOG_ERROR(status.message());
            RecoverableError(std::move(status));
            return LogicalType::kInvalid;
        }
    }
}

LogicalType GetCommonLogicalType(const EmbeddingDataType query_data_type, const EmbeddingDataType column_type) {
    switch (query_data_type) {
        case EmbeddingDataType::kElemInt8: {
            return GetCommonLogicalType<i8>(column_type);
        }
        case EmbeddingDataType::kElemInt16: {
            return GetCommonLogicalType<i16>(column_type);
        }
        case EmbeddingDataType::kElemInt32: {
            return GetCommonLogicalType<i32>(column_type);
        }
        case EmbeddingDataType::kElemInt64: {
            return GetCommonLogicalType<i64>(column_type);
        }
        case EmbeddingDataType::kElemFloat: {
            return GetCommonLogicalType<f32>(column_type);
        }
        case EmbeddingDataType::kElemDouble: {
            return GetCommonLogicalType<f64>(column_type);
        }
        case EmbeddingDataType::kElemBit:
        case EmbeddingDataType::kElemInvalid: {
            Status status =
                Status::NotSupport(fmt::format("Unsupported query tensor data type: {}", EmbeddingT::EmbeddingDataType2String(query_data_type)));
            LOG_ERROR(status.message());
            RecoverableError(std::move(status));
            return LogicalType::kInvalid;
        }
    }
}

} // namespace infinity
