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

struct IntegerTryCastToFixlen;
struct IntegerTryCastToVarlen;

template<class SourceType>
inline static BoundCastFunc
BindIntegerCast(const DataType &source, DataType &target) {
    TypeAssert(source.type() != target.type(), "Attempt to cast from " + source.ToString() + " to " + target.ToString());
    // TODO: How to check data type is same as SourceType ?
    switch (target.type()) {
        case LogicalType::kTinyInt: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, TinyIntT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kSmallInt: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, SmallIntT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kInteger: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, IntegerT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kBigInt: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, BigIntT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kHugeInt: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, HugeIntT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kFloat: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, FloatT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kDouble: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, DoubleT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kDecimal16: {
            NotImplementError("Not implement cast from numeric to decimal16 type.");
        }
        case LogicalType::kDecimal32: {
            NotImplementError("Not implement cast from numeric to decimal32 type.");
        }
        case LogicalType::kDecimal64: {
            NotImplementError("Not implement cast from numeric to decimal64 type.");
        }
        case LogicalType::kVarchar: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVectorToVarlen<SourceType, DoubleT, IntegerTryCastToVarlen>);
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
                    &ColumnVectorCast::TryCastColumnVector<SourceType, Decimal64T, IntegerTryCastToFixlen>);
        }
        case LogicalType::kTimestampTZ: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, Decimal64T, IntegerTryCastToFixlen>);
        }
        default: {
            TypeError("BindIntegerCast: Can't cast from " + source.ToString() + " to " + target.ToString());
        }
    }
}

struct IntegerTryCastToFixlen {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(SourceType source, TargetType &target) {
        FunctionError("Not implement to cast from " + DataType::TypeToString<SourceType>()
                 + " to " + DataType::TypeToString<TargetType>());
    }
};

struct IntegerTryCastToVarlen {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(SourceType source, TargetType &target, const ColumnVector* vector_ptr) {
        FunctionError("Not implement to cast from " + DataType::TypeToString<SourceType>()
                      + " to " + DataType::TypeToString<TargetType>());
    }
};

// Cast TinyInt to other numeric type
template<>
bool
IntegerTryCastToFixlen::Run(TinyIntT source, SmallIntT &target) {
    target = source;
    return true;
}

template<>
bool
IntegerTryCastToFixlen::Run(TinyIntT source, IntegerT &target) {
    target = source;
    return true;
}

template<>
bool
IntegerTryCastToFixlen::Run(TinyIntT source, BigIntT &target) {
    target = source;
    return true;
}

template<>
bool
IntegerTryCastToFixlen::Run(TinyIntT source, HugeIntT &target) {
    target.lower = source;
    target.upper = (source < 0) * -1;
    return true;
}

template<>
bool
IntegerTryCastToFixlen::Run(TinyIntT source, FloatT &target) {
    target = source;
    return true;
}

template<>
bool
IntegerTryCastToFixlen::Run(TinyIntT source, DoubleT &target) {
    target = source;
    return true;
}

// TODO
//template<>
//bool IntegerTryCastToFixlen::Run(TinyIntT source, Decimal16T &target);
//template<>
//bool IntegerTryCastToFixlen::Run(TinyIntT source, Decimal32T &target);
//template<>
//bool IntegerTryCastToFixlen::Run(TinyIntT source, Decimal64T &target);
//template<>
//bool IntegerTryCastToFixlen::Run(TinyIntT source, Decimal128T &target);

template<>
bool
IntegerTryCastToFixlen::Run(TinyIntT source, Char1T &target) {
    if(source < 0 or source >= 10) return false;
    target.value =  '0' + source;
    return true;
}

template<>
bool
IntegerTryCastToFixlen::Run(TinyIntT source, Char2T &target) {
    if(source > -10 && source < 100) {
        if(source < 0) {
            target.value[0] = '-';
            target.value[1] = '0' - source;
        } else if(source > 0 && source < 10) {
            target.value[0] = '0' + source;
            target.value[1] = 0;
        } else if(source >= 10) {
            const i8 tens = source / 10;
            target.value[0] = '0' + tens;
            target.value[1] = '0' + source - tens * 10;
        } else {
            target.value[0] = '0';
        }
        return true;
    } else {
        return false;
    }
}

template<>
bool
IntegerTryCastToFixlen::Run(TinyIntT source, Char4T &target) {
    i64 src = source;
    if(source == 0) {
        target.value[0] = '0';
        return true;
    }
    size_t idx = 0;
    if(source < 0) {
        target.value[idx ++] = '-';
        src = -src;
    }

    char_t tmp[Char4T::CHAR_LENGTH];
    i64 tmp_idx = 0;
    while(src > 0) {
        tmp[tmp_idx ++] = '0' + src % 10;
        src /= 10;
    }
    while(idx < Char4T::CHAR_LENGTH) {
        -- tmp_idx;
        target.value[idx ++] = tmp_idx >= 0 ? tmp[tmp_idx] : 0;
    }

    return true;
}
//
//template<>
//bool IntegerTryCastToFixlen::Run(TinyIntT source, Char8T &target) {
//    return false;
//}
//
//template<>
//bool IntegerTryCastToFixlen::Run(TinyIntT source, Char16T &target) {
//
//}
//
//template<>
//bool IntegerTryCastToFixlen::Run(TinyIntT source, Char32T &target) {
//
//}
//
//template<>
//bool IntegerTryCastToFixlen::Run(TinyIntT source, Char64T &target) {
//
//}
//
//
//// Cast TinyInt to varlen type
//template<>
//bool IntegerTryCastToVarlen::Run(TinyIntT source, VarcharT &target, const ColumnVector *vector_ptr) {
//    NotImplementError("Not implemented");
//}

}