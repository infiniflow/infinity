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

export module date_cast;

import stl;
import column_vector;
import vector_buffer;
import bound_cast_func;
import column_vector_cast;
import logical_type;
import infinity_exception;
import third_party;
import internal_types;
import data_type;
import logger;

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
            String error_message = fmt::format("Can't cast from Date type to {}", target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }
    return BoundCastFunc(nullptr);
}

struct DateTryCastToFixlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType , TargetType &) {
        String error_message = fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>());
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
        return false;
    }
};

struct DateTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType , TargetType &, ColumnVector*) {
        String error_message = fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>());
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
        return false;
    }
};

template <>
inline bool DateTryCastToFixlen::Run(DateT , DateTimeT &) {
    String error_message = "Not implemented";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

template <>
inline bool DateTryCastToFixlen::Run(DateT , TimestampT &) {
    String error_message = "Not implemented";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

template <>
inline bool DateTryCastToVarlen::Run(DateT , VarcharT &, ColumnVector*) {
    String error_message = "Not implemented";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

} // namespace infinity
