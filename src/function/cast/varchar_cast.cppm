//
// Created by jinhai on 23-3-10.
//

module;

import parser;
import stl;
import bound_cast_func;
import column_vector_cast;
import infinity_assert;
import infinity_exception;
import third_party;
import column_vector;
import vector_buffer;

export module varchar_cast;

namespace infinity {

export struct TryCastVarchar;
export struct TryCastVarcharToChar;
export struct TryCastVarcharToVarchar;

export inline BoundCastFunc BindVarcharCast(const DataType &source, const DataType &target) {
    Assert<TypeException>(source.type() == LogicalType::kVarchar,
                          Format("Expect Varchar type, but it is {}", source.ToString()),
                          __FILE_NAME__,
                          __LINE__);
    switch (target.type()) {
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
            Error<NotImplementException>(Format("Not implement cast from varchar to decimal128 type.", source.ToString(), target.ToString()),
                                         __FILE_NAME__,
                                         __LINE__);
        }
        case kVarchar: {
            Error<TypeException>("Attempt to cast from varchar to varchar", __FILE_NAME__, __LINE__);
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
            Error<TypeException>("Cast from varchar to array", __FILE_NAME__, __LINE__);
        }
        case kTuple: {
            Error<TypeException>("Cast from varchar to tuple", __FILE_NAME__, __LINE__);
        }
        case kPoint: {
            Error<TypeException>("Cast from varchar to point", __FILE_NAME__, __LINE__);
        }
        case kLine: {
            Error<TypeException>("Cast from varchar to line", __FILE_NAME__, __LINE__);
        }
        case kLineSeg: {
            Error<TypeException>("Cast from varchar to line segment", __FILE_NAME__, __LINE__);
        }
        case kBox: {
            Error<TypeException>("Cast from varchar to box", __FILE_NAME__, __LINE__);
        }
        case kPath: {
            Error<TypeException>("Cast from varchar to path", __FILE_NAME__, __LINE__);
        }
        case kPolygon: {
            Error<TypeException>("Cast from varchar to polygon", __FILE_NAME__, __LINE__);
        }
        case kCircle: {
            Error<TypeException>("Cast from varchar to circle", __FILE_NAME__, __LINE__);
        }
        case kBitmap: {
            Error<TypeException>("Cast from varchar to bitmap", __FILE_NAME__, __LINE__);
        }
        case kUuid: {
            Error<TypeException>("Cast from varchar to uuid", __FILE_NAME__, __LINE__);
        }
        case kBlob: {
            Error<TypeException>("Cast from varchar to blob", __FILE_NAME__, __LINE__);
        }
        case kEmbedding: {
            Error<TypeException>("Cast from varchar to embedding", __FILE_NAME__, __LINE__);
        }
        case kRowID: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, RowT, TryCastVarchar>);
        }
        case kMixed: {
            Error<TypeException>("Cast from varchar to mix", __FILE_NAME__, __LINE__);
        }
        default: {
            Error<TypeException>("Can't convert varchar", __FILE_NAME__, __LINE__);
        }
    }
}

struct TryCastVarchar {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &input, TargetType &target) {
        Error<FunctionException>(Format("No implementation to cast from {} to {}",
                                 DataType::TypeToString<SourceType>(),
                                 DataType::TypeToString<TargetType>()), __FILE_NAME__, __LINE__);
    }
};

// Cast VarcharT to Boolean type
template <>
inline bool TryCastVarchar::Run(const VarcharT &source, BooleanT &target) {

    if (source.length == 4) {
        bool res = ToLower(source.prefix[0]) == 't' && ToLower(source.prefix[1]) == 'r' && ToLower(source.prefix[2]) == 'u' &&
                   ToLower(source.prefix[3]) == 'e';
        if (res) {
            target = true;
            return true;
        } else {
            return false;
        }
    }
    if (source.length == 5) {
        bool res = ToLower(source.prefix[0]) == 'f' && ToLower(source.prefix[1]) == 'a' && ToLower(source.prefix[2]) == 'l' &&
                   ToLower(source.prefix[3]) == 's' && ToLower(source.prefix[4]) == 'e';
        if (res) {
            target = false;
            return true;
        } else {
            return false;
        }
    }
    return false;
}

// Cast VarcharT to TinyT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &source, TinyIntT &target) {
    i64 value{0};
    char *endptr{nullptr};
    SizeT len{0};
    if (source.IsInlined()) {
        value = StrToL(source.prefix, &endptr, 10);
        len = (endptr - source.prefix);
    } else {
        value = StrToL(source.ptr, &endptr, 10);
        len = (endptr - source.ptr);
    }
    if (len != source.length) {
        return false;
    }
    target = static_cast<TinyIntT>(value);
    return true;
}

// Cast VarcharT to SmallIntT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &source, SmallIntT &target) {
    i64 value{0};
    char *endptr{nullptr};
    SizeT len{0};
    if (source.IsInlined()) {
        value = StrToL(source.prefix, &endptr, 10);
    } else {
        value = StrToL(source.ptr, &endptr, 10);
    }
    if (len != source.length) {
        return false;
    }
    target = static_cast<SmallIntT>(value);
    return true;
}

// Cast VarcharT to IntegerT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &source, IntegerT &target) {
    i64 value{0};
    char *endptr{nullptr};
    SizeT len{0};
    if (source.IsInlined()) {
        value = StrToL(source.prefix, &endptr, 10);
    } else {
        value = StrToL(source.ptr, &endptr, 10);
    }
    if (len != source.length) {
        return false;
    }
    target = static_cast<IntegerT>(value);
    return true;
}

// Cast VarcharT to BigIntT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &source, i64 &target) {
    char *endptr{nullptr};
    SizeT len{0};
    if (source.IsInlined()) {
        target = StrToL(source.prefix, &endptr, 10);
    } else {
        target = StrToL(source.ptr, &endptr, 10);
    }
    if (len != source.length) {
        return false;
    }
    return true;
}

// Cast VarcharT to HugeIntT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &source, HugeIntT &target) {
    Error<TypeException>("Cast varchar to hugeint", __FILE_NAME__, __LINE__);
}

// Cast VarcharT to FloatT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &source, FloatT &target) {
    char *endptr{nullptr};
    SizeT len{0};
    if (source.IsInlined()) {
        target = StrToF(source.prefix, &endptr);
    } else {
        target = StrToF(source.ptr, &endptr);
    }
    if (len != source.length) {
        return false;
    }
    return true;
}

// Cast VarcharT to DoubleT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &source, DoubleT &target) {
    char *endptr{nullptr};
    SizeT len{0};
    if (source.IsInlined()) {
        target = StrToD(source.prefix, &endptr);
    } else {
        target = StrToD(source.ptr, &endptr);
    }
    if (len != source.length) {
        return false;
    }
    return true;
}

// Cast VarcharT to DateT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &source, DateT &target) {
    if (source.IsInlined()) {
        target.FromString(source.prefix, source.length);
    } else {
        target.FromString(source.ptr, source.length);
    }
    return true;
}

// Cast VarcharT to TimeT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &source, TimeT &target) {
    Error<TypeException>("Cast from varchar to time", __FILE_NAME__, __LINE__);
    return true;
}

// Cast VarcharT to DateTimeT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &source, DateTimeT &target) {
    Error<TypeException>("Cast from varchar to datetime", __FILE_NAME__, __LINE__);
    return true;
}

// Cast VarcharT to TimestampT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &source, TimestampT &target) {
    Error<TypeException>("Cast from varchar to timestamp", __FILE_NAME__, __LINE__);
    return true;
}

// Cast VarcharT to IntervalT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &source, IntervalT &target) {
    Error<TypeException>("Cast from varchar to interval", __FILE_NAME__, __LINE__);
    return true;
}

} // namespace infinity
