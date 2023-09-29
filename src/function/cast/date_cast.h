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

struct DateTryCastToFixlen;
struct DateTryCastToVarlen;

inline static BoundCastFunc
BindDateCast(DataType& target) {
    switch(target.type()) {
        case LogicalType::kDateTime: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<DateT, DateTimeT, DateTryCastToFixlen>);
        }
        case LogicalType::kTimestamp: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<DateT, TimestampT, DateTryCastToFixlen>);
        }
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<DateT, VarcharT, DateTryCastToVarlen>);
        }
        default: {
            TypeError("Can't cast from Date type to " + target.ToString());
        }
    }
}

struct DateTryCastToFixlen {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(SourceType source, TargetType& target) {
        FunctionError("Not support to cast from " + DataType::TypeToString<SourceType>()
                      + " to " + DataType::TypeToString<TargetType>());
    }
};

struct DateTryCastToVarlen {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(SourceType source, TargetType& target, const SharedPtr<ColumnVector>& vector_ptr) {
        FunctionError("Not support to cast from " + DataType::TypeToString<SourceType>()
                      + " to " + DataType::TypeToString<TargetType>());
    }
};

template<>
inline bool
DateTryCastToFixlen::Run(DateT source, DateTimeT& target) {
    NotImplementError("Not implemented");
}

template<>
inline bool
DateTryCastToFixlen::Run(DateT source, TimestampT& target) {
    NotImplementError("Not implemented");
}

template<>
inline bool
DateTryCastToVarlen::Run(DateT source, VarcharType& target, const SharedPtr<ColumnVector>& vector_ptr) {
    NotImplementError("Not implemented");
}

}
