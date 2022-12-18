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
        case LogicalType::kChar8: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<BooleanT, Char8T, BoolTryCast>);
        }
        case LogicalType::kChar16: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<BooleanT, Char16T, BoolTryCast>);
        }
        case LogicalType::kChar32: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<BooleanT, Char32T, BoolTryCast>);
        }
        case LogicalType::kChar64: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<BooleanT, Char64T, BoolTryCast>);
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
        return true;
    }
};

}