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

export module infinity_core:date_cast;

import :column_vector;
import :vector_buffer;
import :bound_cast_func;
import :column_vector_cast;
import :infinity_exception;

import internal_types;
import data_type;
import logical_type;

namespace infinity {

export struct DateTryCastToFixlen;
export struct DateTryCastToVarlen;

export inline BoundCastFunc BindDateCast(DataType &target) {
    switch (target.type()) {
        case LogicalType::kDateTime: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<DateT, DateTimeT, DateTryCastToFixlen>);
        }
        case LogicalType::kTimestamp: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<DateT, TimestampT, DateTryCastToFixlen>);
        }
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<DateT, VarcharT, DateTryCastToVarlen>);
        }
        default: {
            UnrecoverableError(fmt::format("Can't cast from Date type to {}", target.ToString()));
        }
    }
    return BoundCastFunc(nullptr);
}

struct DateTryCastToFixlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType, TargetType &) {
        UnrecoverableError(fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
    }
};

struct DateTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType, TargetType &, ColumnVector *) {
        UnrecoverableError(fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
    }
};

template <>
inline bool DateTryCastToFixlen::Run(DateT, DateTimeT &) {
    UnrecoverableError("Not implemented");
    return false;
}

template <>
inline bool DateTryCastToFixlen::Run(DateT, TimestampT &) {
    UnrecoverableError("Not implemented");
    return false;
}

template <>
inline bool DateTryCastToVarlen::Run(DateT, VarcharT &, ColumnVector *) {
    UnrecoverableError("Not implemented");
    return false;
}

} // namespace infinity
