//
// Created by JinHai on 2022/12/17.
//

#pragma once

#include "bound_cast_func.h"
#include "column_vector_cast.h"
#include "common/types/data_type.h"
#include "common/types/internal_types.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

struct IntegerTryCastCheckOverflow;

template<class SourceType>
inline static BoundCastFunc
BindIntegerCast(const DataType &source, DataType &target) {
    TypeAssert(source.type() != target.type(), "Attempt to cast from " + source.ToString() + " to " + target.ToString());
    // TODO: How to check data type is same as SourceType ?
    switch (target.type()) {
        case LogicalType::kTinyInt: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, TinyIntT, IntegerTryCastCheckOverflow>);
        }
        case LogicalType::kSmallInt: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, SmallIntT, IntegerTryCastCheckOverflow>);
        }
        case LogicalType::kInteger: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, IntegerT, IntegerTryCastCheckOverflow>);
        }
        case LogicalType::kBigInt: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, BigIntT, IntegerTryCastCheckOverflow>);
        }
        case LogicalType::kHugeInt: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, HugeIntT, IntegerTryCastCheckOverflow>);
        }
        case LogicalType::kFloat: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, FloatT, IntegerTryCastCheckOverflow>);
        }
        case LogicalType::kDouble: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, DoubleT, IntegerTryCastCheckOverflow>);
        }
        case LogicalType::kDecimal16: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, Decimal16T, IntegerTryCastCheckOverflow>);
        }
        case LogicalType::kDecimal32: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, Decimal32T, IntegerTryCastCheckOverflow>);
        }
        case LogicalType::kDecimal64: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, Decimal64T, IntegerTryCastCheckOverflow>);
        }
        case LogicalType::kVarchar: {
            return BoundCastFunc(nullptr);
        }
        case LogicalType::kChar1: {
            return BoundCastFunc(nullptr);
        }
        case LogicalType::kChar2: {
            return BoundCastFunc(nullptr);
        }
        case LogicalType::kChar4: {
            return BoundCastFunc(nullptr);
        }
        case LogicalType::kChar8: {
            return BoundCastFunc(nullptr);
        }
        case LogicalType::kChar16: {
            return BoundCastFunc(nullptr);
        }
        case LogicalType::kChar32: {
            return BoundCastFunc(nullptr);
        }
        case LogicalType::kChar64: {
            return BoundCastFunc(nullptr);
        }
        case LogicalType::kTimestamp: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, Decimal64T, IntegerTryCastCheckOverflow>);
        }
        case LogicalType::kTimestampTZ: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, Decimal64T, IntegerTryCastCheckOverflow>);
        }
        default: {
            TypeError("BindIntegerCast: Can't cast from " + source.ToString() + " to " + target.ToString());
        }
    }
}

struct IntegerTryCastCheckOverflow {
    template<typename SourceType, typename TargetType>
    static inline bool
    Operation(SourceType source, TargetType &target) {
        FunctionError("Not implement to cast from " + DataType::TypeToString<SourceType>()
                 + " to " + DataType::TypeToString<TargetType>());
    }
};

// Cast TinyInt to other numeric type
template<>
bool IntegerTryCastCheckOverflow::Operation(TinyIntT source, SmallIntT &target) {
    target = source;
    return true;
}

template<>
bool IntegerTryCastCheckOverflow::Operation(TinyIntT source, IntegerT &target) {
    target = source;
    return true;
}

template<>
bool IntegerTryCastCheckOverflow::Operation(TinyIntT source, BigIntT &target) {
    target = source;
    return true;
}

template<>
bool IntegerTryCastCheckOverflow::Operation(TinyIntT source, HugeIntT &target) {
    target.lower = source;
    target.upper = (source < 0) * -1;
    return true;
}

template<>
bool IntegerTryCastCheckOverflow::Operation(TinyIntT source, FloatT &target) {
    target = source;
    return true;
}

template<>
bool IntegerTryCastCheckOverflow::Operation(TinyIntT source, DoubleT &target) {
    target = source;
    return true;
}

}