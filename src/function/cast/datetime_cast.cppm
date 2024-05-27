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

export module datetime_cast;

import stl;
import column_vector;
import vector_buffer;
import bound_cast_func;
import data_type;
import column_vector_cast;
import logical_type;
import infinity_exception;
import third_party;
import internal_types;
import logger;

namespace infinity {

export struct DateTimeTryCastToFixlen;
export struct DateTimeTryCastToVarlen;

export inline BoundCastFunc BindDateTimeCast(DataType &target) {
    switch (target.type()) {
        case LogicalType::kDate: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<DateTimeT, DateT, DateTimeTryCastToFixlen>);
        }
        case LogicalType::kTime: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<DateTimeT, TimeT, DateTimeTryCastToFixlen>);
        }
        case LogicalType::kTimestamp: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<DateTimeT, TimestampT, DateTimeTryCastToFixlen>);
        }
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<DateTimeT, VarcharT, DateTimeTryCastToVarlen>);
        }
        default: {
            UnrecoverableError(fmt::format("Can't cast from DateTime type to {}", target.ToString()));
        }
    }
    return BoundCastFunc(nullptr);
}

struct DateTimeTryCastToFixlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType, TargetType &) {
        UnrecoverableError(
            fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
    }
};

struct DateTimeTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType, TargetType &, ColumnVector*) {
        UnrecoverableError(
            fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
    }
};

template <>
inline bool DateTimeTryCastToFixlen::Run(DateTimeT, DateT &) {
    String error_message = "Not implemented";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

template <>
inline bool DateTimeTryCastToFixlen::Run(DateTimeT, TimeT &) {
    String error_message = "Not implemented";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

template <>
inline bool DateTimeTryCastToFixlen::Run(DateTimeT, TimestampT &) {
    String error_message = "Not implemented";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

template <>
inline bool DateTimeTryCastToVarlen::Run(DateTimeT, VarcharT &, ColumnVector*) {
    String error_message = "Not implemented";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

} // namespace infinity
