//
// Created by jinhai on 22-12-23.
//

#pragma once

#include "bound_cast_func.h"
#include "column_vector_cast.h"
#include "common/types/data_type.h"
#include "common/types/internal_types.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

struct IntervalTryCastToVarlen;

inline static BoundCastFunc
BindTimeCast(DataType& target) {
    switch(target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<IntervalT, VarcharT, IntervalTryCastToVarlen>);
        }
        default: {
            TypeError("Can't cast from Time type to " + target.ToString());
        }
    }
}

struct IntervalTryCastToVarlen {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(SourceType source, TargetType& target, const SharedPtr<ColumnVector>& vector_ptr) {
        FunctionError("Not support to cast from " + DataType::TypeToString<SourceType>()
                      + " to " + DataType::TypeToString<TargetType>());
    }
};

template<>
inline bool
IntervalTryCastToVarlen::Run(IntervalT source, VarcharType& target, const SharedPtr<ColumnVector>& vector_ptr) {
    NotImplementError("Not implemented");
}

}
