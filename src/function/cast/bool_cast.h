//
// Created by JinHai on 2022/12/15.
//

#pragma once

#include "bound_cast_func.h"
#include "column_vector_cast.h"
#include "common/types/data_type.h"
#include "common/types/internal_types.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

struct BoolTryCast;

inline static BoundCastFunc
BindBoolCast(const DataType& source, DataType& target) {
    TypeAssert(source.type() == LogicalType::kBoolean, "Expect boolean type, but it is " + source.ToString());
    switch(target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<BooleanT, VarcharT, BoolTryCast>);
        }
        default: {
            TypeError("Can't cast from Boolean to " + target.ToString());
        }
    }
}

struct BoolTryCast  {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(SourceType input, TargetType& target) {
        FunctionError("No implementation to cast from " + DataType::TypeToString<SourceType>()
                      + " to " + DataType::TypeToString<TargetType>());
    }
};

// Cast TinyInt to other numeric type
template<>
inline bool
BoolTryCast::Run(BooleanT source, VarcharT &target) {
    if(source) {
        constexpr size_t TRUE_LEN = 4;
        memcpy(target.prefix, "true", TRUE_LEN);
        memset(target.prefix + TRUE_LEN, 0, VarcharT::INLINE_LENGTH - TRUE_LEN);
        target.length = TRUE_LEN;
    } else {
        constexpr size_t FALSE_LEN = 5;
        memcpy(target.prefix, "false", FALSE_LEN);
        memset(target.prefix + FALSE_LEN, 0, VarcharT::INLINE_LENGTH - FALSE_LEN);
        target.length = FALSE_LEN;
    }
    return true;
}

}