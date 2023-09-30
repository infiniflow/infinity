//
// Created by jinhai on 23-3-10.
//

#pragma once

#include "bound_cast_func.h"
#include "column_vector_cast.h"
#include "common/types/data_type.h"

#include "common/utility/infinity_assert.h"

namespace infinity {

struct TryCastVarchar;
struct TryCastVarcharToChar;
struct TryCastVarcharToVarchar;

inline static BoundCastFunc
BindVarcharCast(const DataType& source, const DataType& target) {
    TypeAssert(source.type() == LogicalType::kVarchar, "Expect Varchar type, but it is " + source.ToString());
    switch(target.type()) {
        case kBoolean: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, BooleanT, TryCastVarchar>);
        }
        case kTinyInt: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, TinyIntT, TryCastVarchar>);
        }
        case kSmallInt: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, SmallIntT, TryCastVarchar>);
        }
        case kInteger: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, IntegerT, TryCastVarchar>);
        }
        case kBigInt: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, BigIntT, TryCastVarchar>);
        }
        case kHugeInt: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, HugeIntT, TryCastVarchar>);
        }
        case kFloat: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, FloatT, TryCastVarchar>);
        }
        case kDouble: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, DoubleT, TryCastVarchar>);
        }
        case kDecimal: {
            NotImplementError("Varchar to Decimal")
        }
        case kVarchar: {
            PlannerError("Varchar to Varchar")
        }
        case kDate: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, DateT, TryCastVarchar>);
        }
        case kTime: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, TimeT, TryCastVarchar>);
        }
        case kDateTime: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, DateTimeT, TryCastVarchar>);
        }
        case kTimestamp: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, TimestampT, TryCastVarchar>);
        }
        case kInterval: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, IntervalT, TryCastVarchar>);
        }
        case kArray: {
            NotImplementError("Cast from varchar to array")
        }
        case kTuple: {
            NotImplementError("Cast from varchar to tuple")
        }
        case kPoint: {
            NotImplementError("Cast from varchar to point")
        }
        case kLine: {
            NotImplementError("Cast from varchar to line")
        }
        case kLineSeg: {
            NotImplementError("Cast from varchar to line segment")
        }
        case kBox: {
            NotImplementError("Cast from varchar to box")
        }
        case kPath: {
            NotImplementError("Cast from varchar to path")
        }
        case kPolygon: {
            NotImplementError("Cast from varchar to polygon")
        }
        case kCircle: {
            NotImplementError("Cast from varchar to circle")
        }
        case kBitmap: {
            NotImplementError("Cast from varchar to bitmap")
        }
        case kUuid: {
            NotImplementError("Cast from varchar to uuid")
        }
        case kBlob: {
            NotImplementError("Cast from varchar to blob")
        }
        case kEmbedding: {
            NotImplementError("Cast from varchar to embedding")
        }
        case kMixed: {
            NotImplementError("Cast from varchar to mix")
        }
        default: {
            NotImplementError("Can't convert varchar")
        }
    }
}

struct TryCastVarchar {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(const SourceType& input, TargetType& target) {
        FunctionError("No implementation to cast from " + DataType::TypeToString<SourceType>()
                      + " to " + DataType::TypeToString<TargetType>());
    }
};

// Cast VarcharT to Boolean type
template<>
inline bool
TryCastVarchar::Run(const VarcharT& source, BooleanT& target) {

    if(source.length == 4) {
        bool res = tolower(source.prefix[0]) == 't' && tolower(source.prefix[1]) == 'r'
                   && tolower(source.prefix[2]) == 'u' && tolower(source.prefix[3]) == 'e';
        if(res) {
            target = true;
            return true;
        } else {
            return false;
        }
    }
    if(source.length == 5) {
        bool res = tolower(source.prefix[0]) == 'f' && tolower(source.prefix[1]) == 'a'
                   && tolower(source.prefix[2]) == 'l' && tolower(source.prefix[3]) == 's'
                   && tolower(source.prefix[4]) == 'e';
        if(res) {
            target = false;
            return true;
        } else {
            return false;
        }
    }
    return false;
}

// Cast VarcharT to TinyT type
template<>
inline bool
TryCastVarchar::Run(const VarcharT& source, TinyIntT& target) {
    i64 value{0};
    char* endptr{nullptr};
    SizeT len{0};
    if(source.IsInlined()) {
        value = std::strtol(source.prefix, &endptr, 10);
        len = (endptr - source.prefix);
    } else {
        value = std::strtol(source.ptr, &endptr, 10);
        len = (endptr - source.ptr);
    }
    if(len != source.length) {
        return false;
    }
    target = static_cast<TinyIntT>(value);
    return true;
}

// Cast VarcharT to SmallIntT type
template<>
inline bool
TryCastVarchar::Run(const VarcharT& source, SmallIntT& target) {
    i64 value{0};
    char* endptr{nullptr};
    SizeT len{0};
    if(source.IsInlined()) {
        value = std::strtol(source.prefix, &endptr, 10);
    } else {
        value = std::strtol(source.ptr, &endptr, 10);
    }
    if(len != source.length) {
        return false;
    }
    target = static_cast<SmallIntT>(value);
    return true;
}

// Cast VarcharT to IntegerT type
template<>
inline bool
TryCastVarchar::Run(const VarcharT& source, IntegerT& target) {
    i64 value{0};
    char* endptr{nullptr};
    SizeT len{0};
    if(source.IsInlined()) {
        value = std::strtol(source.prefix, &endptr, 10);
    } else {
        value = std::strtol(source.ptr, &endptr, 10);
    }
    if(len != source.length) {
        return false;
    }
    target = static_cast<IntegerT>(value);
    return true;
}

// Cast VarcharT to BigIntT type
template<>
inline bool
TryCastVarchar::Run(const VarcharT& source, i64& target) {
    char* endptr{nullptr};
    SizeT len{0};
    if(source.IsInlined()) {
        target = std::strtol(source.prefix, &endptr, 10);
    } else {
        target = std::strtol(source.ptr, &endptr, 10);
    }
    if(len != source.length) {
        return false;
    }
    return true;
}

// Cast VarcharT to HugeIntT type
template<>
inline bool
TryCastVarchar::Run(const VarcharT& source, HugeIntT& target) {
    NotImplementError("Cast varchar to hugeint")
}

// Cast VarcharT to FloatT type
template<>
inline bool
TryCastVarchar::Run(const VarcharT& source, FloatT& target) {
    char* endptr{nullptr};
    SizeT len{0};
    if(source.IsInlined()) {
        target = std::strtof(source.prefix, &endptr);
    } else {
        target = std::strtof(source.ptr, &endptr);
    }
    if(len != source.length) {
        return false;
    }
    return true;
}

// Cast VarcharT to DoubleT type
template<>
inline bool
TryCastVarchar::Run(const VarcharT& source, DoubleT& target) {
    char* endptr{nullptr};
    SizeT len{0};
    if(source.IsInlined()) {
        target = std::strtod(source.prefix, &endptr);
    } else {
        target = std::strtod(source.ptr, &endptr);
    }
    if(len != source.length) {
        return false;
    }
    return true;
}

// Cast VarcharT to DateT type
template<>
inline bool
TryCastVarchar::Run(const VarcharT& source, DateT& target) {
    if(source.IsInlined()) {
        target.FromString(source.prefix, source.length);
    } else {
        target.FromString(source.ptr, source.length);
    }
    return true;
}

// Cast VarcharT to TimeT type
template<>
inline bool
TryCastVarchar::Run(const VarcharT& source, TimeT& target) {
    NotImplementError("Cast from varchar to time")
    return true;
}

// Cast VarcharT to DateTimeT type
template<>
inline bool
TryCastVarchar::Run(const VarcharT& source, DateTimeT& target) {
    NotImplementError("Cast from varchar to datetime")
    return true;
}

// Cast VarcharT to TimestampT type
template<>
inline bool
TryCastVarchar::Run(const VarcharT& source, TimestampT& target) {
    NotImplementError("Cast from varchar to timestamp")
    return true;
}

// Cast VarcharT to IntervalT type
template<>
inline bool
TryCastVarchar::Run(const VarcharT& source, IntervalT& target) {
    NotImplementError("Cast from varchar to interval")
    return true;
}

}
