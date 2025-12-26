// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

export module infinity_core:json_cast;

import :bound_cast_func;
import :column_vector_cast;
import :infinity_exception;
import :column_vector;
import :vector_buffer;
import :status;
import :json_manager;

import data_type;
import internal_types;
import logical_type;

namespace infinity {

struct TryCastJson;

export inline BoundCastFunc BindJsonCast(const DataType &source, const DataType &target) {
    if (source.type() != LogicalType::kJson) {
        UnrecoverableError(fmt::format("Expect Json type, but it is {}", source.ToString()));
    }
    switch (target.type()) {
        case LogicalType::kVarchar: {
            break;
        }
        case LogicalType::kInteger: {
            break;
        }
        case LogicalType::kDouble: {
            break;
        }
        case LogicalType::kBoolean: {
            break;
        }
        default: {
            RecoverableError(Status::SyntaxError("BindJsonCast: Invalid target type"));
        }
    }

    return BoundCastFunc(nullptr);
}

struct TryCastJson {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &input,
                           DataType source_type,
                           ColumnVector *source_vector,
                           TargetType &result,
                           DataType target_type,
                           ColumnVector *target_vector) {
        UnrecoverableError(
            fmt::format("No implementation to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
    }
};

template <>
inline bool TryCastJson::Run(const JsonT &input,
                             DataType source_type,
                             ColumnVector *source_vector,
                             VarcharT &result,
                             DataType target_type,
                             ColumnVector *target_vector) {
    const auto data = source_vector->buffer_->GetVarchar(input.file_offset_, input.length_);
    std::vector<uint8_t> bson_data(reinterpret_cast<const uint8_t *>(data), reinterpret_cast<const uint8_t *>(data) + input.length_);
    auto json_data = JsonManager::from_bson(bson_data);

    Value value = Value::MakeVarchar(json_data.dump());
    target_vector->AppendValue(value);
    return true;
}

} // namespace infinity