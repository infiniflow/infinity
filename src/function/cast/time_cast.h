//
// Created by jinhai on 22-12-23.
//

#pragma once

#include "bound_cast_func.h"
#include "column_vector_cast.h"
#include "common/types/data_type.h"

#include "common/utility/infinity_assert.h"

namespace infinity {

struct TimeTryCastToVarlen;

inline static BoundCastFunc BindTimeCast(DataType &target) {
    switch (target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<TimeT, VarcharT, TimeTryCastToVarlen>);
        }
        default: {
            TypeError("Can't cast from Time type to " + target.ToString());
        }
    }
}

struct TimeTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType source, TargetType &target, const SharedPtr<ColumnVector> &vector_ptr) {
        FunctionError("Not support to cast from " + DataType::TypeToString<SourceType>() + " to " + DataType::TypeToString<TargetType>());
    }
};

template <>
inline bool TimeTryCastToVarlen::Run(TimeT source, VarcharType &target, const SharedPtr<ColumnVector> &vector_ptr) {
    NotImplementError("Not implemented");
}

} // namespace infinity
