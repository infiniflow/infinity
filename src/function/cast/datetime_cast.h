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

struct DateTimeTryCastToFixlen;
struct DateTimeTryCastToVarlen;

inline static BoundCastFunc
BindDateTimeCast(DataType& target) {
    switch(target.type()) {
        case LogicalType::kDate: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<DateTimeT, DateT, DateTimeTryCastToFixlen>);
        }
        case LogicalType::kTime: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<DateTimeT, TimeT, DateTimeTryCastToFixlen>);
        }
        case LogicalType::kTimestamp: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<DateTimeT, TimestampT, DateTimeTryCastToFixlen>);
        }
        case LogicalType::kTimestampTZ: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<DateTimeT, TimestampTZT, DateTimeTryCastToFixlen>);
        }
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<DateTimeT, VarcharT, DateTimeTryCastToVarlen>);
        }
        default: {
            TypeError("Can't cast from DateTime type to " + target.ToString());
        }
    }
}

struct DateTimeTryCastToFixlen {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(SourceType source, TargetType &target) {
        FunctionError("Not support to cast from " + DataType::TypeToString<SourceType>()
                      + " to " + DataType::TypeToString<TargetType>());
    }
};

struct DateTimeTryCastToVarlen {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(SourceType source, TargetType &target, const ColumnVector* vector_ptr){
        FunctionError("Not support to cast from " + DataType::TypeToString<SourceType>()
                      + " to " + DataType::TypeToString<TargetType>());
    }
};

template<>
inline bool
DateTimeTryCastToFixlen::Run(DateTimeT source, DateT &target) {
    NotImplementError("Not implemented");
}

template<>
inline bool
DateTimeTryCastToFixlen::Run(DateTimeT source, TimeT &target) {
    NotImplementError("Not implemented");
}

template<>
inline bool
DateTimeTryCastToFixlen::Run(DateTimeT source, TimestampT &target) {
    NotImplementError("Not implemented");
}

template<>
inline bool
DateTimeTryCastToFixlen::Run(DateTimeT source, TimestampTZT &target) {
    NotImplementError("Not implemented");
}

template<>
inline bool
DateTimeTryCastToVarlen::Run(DateTimeT source, VarcharType &target, const ColumnVector* vector_ptr) {
    NotImplementError("Not implemented");
}

}
