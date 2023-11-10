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

import stl;
import parser;
import column_vector_cast;

import infinity_exception;
import bound_cast_func;
import column_vector;
import third_party;

export module timestamp_cast;

namespace infinity {

export struct TimestampTryCastToFixlen;
export struct TimestampTryCastToVarlen;

export inline BoundCastFunc BindTimestampCast(DataType &target) {
    switch (target.type()) {
        case LogicalType::kDate: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<TimestampT, DateT, TimestampTryCastToFixlen>);
        }
        case LogicalType::kTime: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<TimestampT, TimeT, TimestampTryCastToFixlen>);
        }
        case LogicalType::kDateTime: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<TimestampT, DateTimeT, TimestampTryCastToFixlen>);
        }
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<TimestampT, VarcharT, TimestampTryCastToVarlen>);
        }
        default: {
            Error<TypeException>(Format("Can't cast from Timestamp type to {}", target.ToString()));
        }
    }
    return BoundCastFunc(nullptr);
}

struct TimestampTryCastToFixlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType source, TargetType &target) {
        Error<FunctionException>(
                Format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
    }
};

struct TimestampTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType source, TargetType &target, const SharedPtr<ColumnVector> &vector_ptr) {
        Error<FunctionException>(
                Format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
    }
};

template <>
inline bool TimestampTryCastToFixlen::Run(TimestampT source, DateT &target) {
    Error<NotImplementException>("Not implemented");
    return false;
}

template <>
inline bool TimestampTryCastToFixlen::Run(TimestampT source, TimeT &target) {
    Error<NotImplementException>("Not implemented");
    return false;
}

template <>
inline bool TimestampTryCastToFixlen::Run(TimestampT source, DateTimeT &target) {
    Error<NotImplementException>("Not implemented");
    return false;
}

template <>
inline bool TimestampTryCastToVarlen::Run(TimestampT source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    Error<NotImplementException>("Not implemented");
    return false;
}

} // namespace infinity
