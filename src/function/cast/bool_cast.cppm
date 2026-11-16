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

export module infinity_core:bool_cast;

import :bound_cast_func;
import :column_vector_cast;
import :infinity_exception;
import :status;
import :column_vector;

import logical_type;
import internal_types;
import data_type;

namespace infinity {

export struct TryCastBoolean {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType, TargetType &) {
        std::string error_msg =
            fmt::format("No implementation to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>());
        RecoverableError(Status::NotSupport(error_msg));
        return false;
    }
};

// Cast BooleanT to other VarcharT type
template <>
inline bool TryCastBoolean::Run(BooleanT source, VarcharT &target) {
    const std::string_view text = source ? "true" : "false";
    target.length_ = static_cast<u32>(text.size());
    std::memcpy(target.short_.data_, text.data(), target.length_);
    return true;
}

export inline BoundCastFunc BindBoolCast(const DataType &source, const DataType &target) {
    if (source.type() != LogicalType::kBoolean) {
        UnrecoverableError(fmt::format("Expect boolean type, but it is {}", source.ToString()));
    }

    switch (target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<BooleanT, VarcharT, TryCastBoolean>);
        }
        default: {
            std::string error_msg = fmt::format("Can't cast from Boolean to {}", target.ToString());
            RecoverableError(Status::NotSupport(error_msg));
        }
    }
    return BoundCastFunc(nullptr);
}

} // namespace infinity
