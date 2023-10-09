//
// Created by jinhai on 22-12-23.
//

#pragma once

#include "bound_cast_func.h"
#include "column_vector_cast.h"
#include "common/types/data_type.h"

#include "common/utility/infinity_assert.h"

namespace infinity {

struct TimestampTryCastToFixlen;
struct TimestampTryCastToVarlen;

inline static BoundCastFunc BindTimestampCast(DataType &target) {
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
            TypeError("Can't cast from Timestamp type to " + target.ToString());
        }
    }
}

struct TimestampTryCastToFixlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType source, TargetType &target) {
        FunctionError("Not support to cast from " + DataType::TypeToString<SourceType>() + " to " + DataType::TypeToString<TargetType>());
    }
};

struct TimestampTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType source, TargetType &target, const SharedPtr<ColumnVector> &vector_ptr) {
        FunctionError("Not support to cast from " + DataType::TypeToString<SourceType>() + " to " + DataType::TypeToString<TargetType>());
    }
};

template <>
inline bool TimestampTryCastToFixlen::Run(TimestampT source, DateT &target) {
    NotImplementError("Not implemented");
}

template <>
inline bool TimestampTryCastToFixlen::Run(TimestampT source, TimeT &target) {
    NotImplementError("Not implemented");
}

template <>
inline bool TimestampTryCastToFixlen::Run(TimestampT source, DateTimeT &target) {
    NotImplementError("Not implemented");
}

template <>
inline bool TimestampTryCastToVarlen::Run(TimestampT source, VarcharType &target, const SharedPtr<ColumnVector> &vector_ptr) {
    NotImplementError("Not implemented");
}

} // namespace infinity
