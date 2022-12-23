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

struct TimestampTZTryCastToFixlen;
struct TimestampTZTryCastToVarlen;

inline static BoundCastFunc
BindTimestampTZCast(DataType& target) {
    switch(target.type()) {
        case LogicalType::kTimestamp: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<TimestampTZT, TimestampT, TimestampTZTryCastToFixlen>);
        }
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<TimestampTZT, VarcharT, TimestampTZTryCastToVarlen>);
        }
        default: {
            TypeError("Can't cast from Timestamp Timezone type to " + target.ToString());
        }
    }
}

struct TimestampTZTryCastToFixlen {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(SourceType source, TargetType &target) {
        FunctionError("Not support to cast from " + DataType::TypeToString<SourceType>()
                      + " to " + DataType::TypeToString<TargetType>());
    }
};

struct TimestampTZTryCastToVarlen {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(SourceType source, TargetType &target, const ColumnVector* vector_ptr){
        FunctionError("Not support to cast from " + DataType::TypeToString<SourceType>()
                      + " to " + DataType::TypeToString<TargetType>());
    }
};

template<>
inline bool
TimestampTZTryCastToFixlen::Run(TimestampTZT source, TimestampT &target) {
    NotImplementError("Not implemented");
}

template<>
inline bool
TimestampTZTryCastToVarlen::Run(TimestampTZT source, VarcharType &target, const ColumnVector* vector_ptr) {
    NotImplementError("Not implemented");
}

}
