//
// Created by jinhai on 22-12-22.
//

#pragma once

#include "bound_cast_func.h"
#include "column_vector_cast.h"
#include "common/types/data_type.h"
#include "common/types/internal_types.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

struct FloatTryCastToFixlen;
struct FloatTryCastToVarlen;

template<class SourceType>
static inline BoundCastFunc
BindDecimalCast(const DataType& source, DataType& target) {
    TypeAssert(source.type() != target.type(),
               "Attempt to cast from " + source.ToString() + " to " + target.ToString());
    switch(target.type()) {
        case LogicalType::kTinyInt: {
            NotImplementError("Not implement cast from numeric to tiny int type.");
        }
        case LogicalType::kSmallInt: {
            NotImplementError("Not implement cast from numeric to small int type.");
        }
        case LogicalType::kInteger: {
            NotImplementError("Not implement cast from numeric to integer type.");
        }
        case LogicalType::kBigInt: {
            NotImplementError("Not implement cast from numeric to big int type.");
        }
        case LogicalType::kHugeInt: {
            NotImplementError("Not implement cast from numeric to huge int type.");
        }
        case LogicalType::kFloat: {
            NotImplementError("Not implement cast from numeric to float type.");
        }
        case LogicalType::kDouble: {
            NotImplementError("Not implement cast from numeric to double type.");
        }
        case LogicalType::kDecimal: {
            NotImplementError("Not implement cast from numeric to decimal= type.");
        }
        case LogicalType::kVarchar: {
            NotImplementError("Not implement cast from numeric to varchar type.");
        }
        default: {
            TypeError("BindIntegerCast: Can't cast from " + source.ToString() + " to " + target.ToString());
        }
    }
}

struct DecimalTryCastToFixlen {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(SourceType source, TargetType& target) {
        FunctionError("No implementation to cast from " + DataType::TypeToString<SourceType>()
                      + " to " + DataType::TypeToString<TargetType>());
    }
};

struct DecimalTryCastToVarlen {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(SourceType source, TargetType& target, const SharedPtr<ColumnVector>& vector_ptr) {
        FunctionError("Not implementation to cast from " + DataType::TypeToString<SourceType>()
                      + " to " + DataType::TypeToString<TargetType>());
    }
};

// Cast DecimalT to TinyIntT
template<>
inline bool
DecimalTryCastToFixlen::Run(DecimalT source, TinyIntT& target) {
    NotImplementError("Not implemented");
}

template<>
inline bool
DecimalTryCastToFixlen::Run(DecimalT source, SmallIntT& target) {
    NotImplementError("Not implemented");
}

template<>
inline bool
DecimalTryCastToFixlen::Run(DecimalT source, IntegerT& target) {
    NotImplementError("Not implemented");
}

template<>
inline bool
DecimalTryCastToFixlen::Run(DecimalT source, BigIntT& target) {
    NotImplementError("Not implemented");
}

template<>
inline bool
DecimalTryCastToFixlen::Run(DecimalT source, HugeIntT& target) {
    NotImplementError("Not implemented");
}

template<>
inline bool
DecimalTryCastToFixlen::Run(DecimalT source, FloatT& target) {
    NotImplementError("Not implemented");
}

template<>
inline bool
DecimalTryCastToFixlen::Run(DecimalT source, DoubleT& target) {
    NotImplementError("Not implemented");
}

template<>
inline bool
DecimalTryCastToVarlen::Run(DecimalT source, VarcharT& target, const SharedPtr<ColumnVector>& vector_ptr) {
    NotImplementError("Not implemented");
}


}
