//
// Created by JinHai on 2022/12/15.
//

#pragma once

#include "function/cast/bound_cast_func.h"
#include "function/cast/column_vector_cast.h"

namespace infinity {

struct TryCastBoolean;

inline static BoundCastFunc BindBoolCast(const DataType &source, const DataType &target) {
    TypeAssert(source.type() == LogicalType::kBoolean, "Expect boolean type, but it is " + source.ToString());
    switch (target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<BooleanT, VarcharT, TryCastBoolean>);
        }
        default: {
            TypeError("Can't cast from Boolean to " + target.ToString());
        }
    }
}

struct TryCastBoolean {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType input, TargetType &target) {
        FunctionError("No implementation to cast from " + DataType::TypeToString<SourceType>() + " to " + DataType::TypeToString<TargetType>());
    }
};

// Cast BooleanT to other VarcharT type
template <>
inline bool TryCastBoolean::Run(BooleanT source, VarcharT &target) {
    if (source) {
        constexpr u16 TRUE_LEN = 4;
        memcpy(target.prefix, "true", TRUE_LEN);
        memset(target.prefix + TRUE_LEN, 0, VarcharT::INLINE_LENGTH - TRUE_LEN);
        target.length = TRUE_LEN;
    } else {
        constexpr u16 FALSE_LEN = 5;
        memcpy(target.prefix, "false", FALSE_LEN);
        memset(target.prefix + FALSE_LEN, 0, VarcharT::INLINE_LENGTH - FALSE_LEN);
        target.length = FALSE_LEN;
    }
    return true;
}

} // namespace infinity