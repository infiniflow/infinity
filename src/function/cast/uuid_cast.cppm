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

export module infinity_core:uuid_cast;

import :stl;
import :column_vector;
import :vector_buffer;
import :bound_cast_func;
import :column_vector_cast;
import :infinity_exception;
import :status;

import internal_types;
import logical_type;
import data_type;

namespace infinity {

export struct UuidTryCastToVarlen;

export inline BoundCastFunc BindUuidCast(DataType &target) {
    switch (target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<UuidT, VarcharT, UuidTryCastToVarlen>);
        }
        default: {
            std::string error_message = fmt::format("Can't cast from Uuid type to {}", target.ToString());
            UnrecoverableError(error_message);
        }
    }
    return BoundCastFunc(nullptr);
}

struct UuidTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &, TargetType &, ColumnVector *) {
        UnrecoverableError(
            fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
    }
};

template <>
inline bool UuidTryCastToVarlen::Run(const UuidT &, VarcharT &, ColumnVector *) {
    Status status = Status::NotSupport("Not implemented");
    RecoverableError(status);
    return true;
}

} // namespace infinity
