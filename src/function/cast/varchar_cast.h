//
// Created by jinhai on 23-3-10.
//

#pragma once

#include "bound_cast_func.h"
#include "column_vector_cast.h"
#include "common/types/data_type.h"
#include "common/types/internal_types.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

struct TryCastBoolean;

inline static BoundCastFunc
BindVarcharCast(const DataType& source, const DataType& target) {
    TypeAssert(source.type() == LogicalType::kVarchar, "Expect Varchar type, but it is " + source.ToString());
    switch(target.type()) {
        case kBoolean: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, BooleanT, TryCastVarchar>);
            break;
        }
        case kTinyInt:
            break;
        case kSmallInt:
            break;
        case kInteger:
            break;
        case kBigInt:
            break;
        case kHugeInt:
            break;
        case kFloat:
            break;
        case kDouble:
            break;
        case kDecimal16:
            break;
        case kDecimal32:
            break;
        case kDecimal64:
            break;
        case kDecimal128:
            break;
        case kVarchar:
            break;
        case kChar:
            break;
        case kDate:
            break;
        case kTime:
            break;
        case kDateTime:
            break;
        case kTimestamp:
            break;
        case kTimestampTZ:
            break;
        case kInterval:
            break;
        case kArray:
            break;
        case kTuple:
            break;
        case kPoint:
            break;
        case kLine:
            break;
        case kLineSeg:
            break;
        case kBox:
            break;
        case kPath:
            break;
        case kPolygon:
            break;
        case kCircle:
            break;
        case kBitmap:
            break;
        case kUuid:
            break;
        case kBlob:
            break;
        case kEmbedding:
            break;
        case kMixed:
            break;
        case kNull:
            break;
        case kMissing:
            break;
        case kInvalid:
            break;

    }
}

struct TryCastVarchar  {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(SourceType input, TargetType& target) {
        FunctionError("No implementation to cast from " + DataType::TypeToString<SourceType>()
                      + " to " + DataType::TypeToString<TargetType>());
    }
};

// Cast BooleanT to other VarcharT type
template<>
inline bool
TryCastVarchar::Run(const VarcharT& source, BooleanT& target) {
    return true;
}

}
