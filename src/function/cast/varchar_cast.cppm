// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

import parser;
import stl;
import bound_cast_func;
import column_vector_cast;

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
    if (source.type() != LogicalType::kVarchar) {
        Error<TypeException>(Format("Expect Varchar type, but it is {}", source.ToString()));
    }
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
            Error<NotImplementException>(Format("Not implement cast from varchar to decimal128 type.", source.ToString(), target.ToString()));
        }
        case kVarchar: {
            Error<TypeException>("Attempt to cast from varchar to varchar");
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
            Error<TypeException>("Cast from varchar to array");
        }
        case kTuple: {
            Error<TypeException>("Cast from varchar to tuple");
        }
        case kPoint: {
            Error<TypeException>("Cast from varchar to point");
        }
        case kLine: {
            Error<TypeException>("Cast from varchar to line");
        }
        case kLineSeg: {
            Error<TypeException>("Cast from varchar to line segment");
        }
        case kBox: {
            Error<TypeException>("Cast from varchar to box");
        }
            //        case kPath: {
            //            Error<TypeException>("Cast from varchar to path");
            //        }
            //        case kPolygon: {
            //            Error<TypeException>("Cast from varchar to polygon");
            //        }
        case kCircle: {
            Error<TypeException>("Cast from varchar to circle");
        }
            //        case kBitmap: {
            //            Error<TypeException>("Cast from varchar to bitmap");
            //        }
        case kUuid: {
            Error<TypeException>("Cast from varchar to uuid");
        }
            //        case kBlob: {
            //            Error<TypeException>("Cast from varchar to blob");
            //        }
        case kEmbedding: {
            Error<TypeException>("Cast from varchar to embedding");
        }
        case kRowID: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, RowID, TryCastVarchar>);
        }
        case kMixed: {
            Error<TypeException>("Cast from varchar to mix");
        }
        default: {
            Error<TypeException>("Can't convert varchar");
        }
    }
    return BoundCastFunc(nullptr);
}

struct TryCastVarchar {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &, TargetType &) {
        Error<FunctionException>(
            Format("No implementation to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
    }
};

// Cast VarcharT to Boolean type
template <>
inline bool TryCastVarchar::Run(const VarcharT &source, BooleanT &target) {

    if (source.length_ == 4) {
        bool res = ToLower(source.short_.data_[0]) == 't' && ToLower(source.short_.data_[1]) == 'r' && ToLower(source.short_.data_[2]) == 'u' &&
                   ToLower(source.short_.data_[3]) == 'e';
        if (res) {
            target = true;
            return true;
        } else {
            return false;
        }
    }
    if (source.length_ == 5) {
        bool res = ToLower(source.short_.data_[0]) == 'f' && ToLower(source.short_.data_[1]) == 'a' && ToLower(source.short_.data_[2]) == 'l' &&
                   ToLower(source.short_.data_[3]) == 's' && ToLower(source.short_.data_[4]) == 'e';
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
        value = StrToL(source.short_.data_, &endptr, 10);
        len = (endptr - source.short_.data_);
    } else {
        // No tiny int isn't inline
        return false;
    }
    if (len != source.length_) {
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
        value = StrToL(source.short_.data_, &endptr, 10);
    } else {
        // No tiny int isn't inline
        return false;
    }
    if (len != source.length_) {
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
        value = StrToL(source.short_.data_, &endptr, 10);
    } else {
        return false;
    }
    if (len != source.length_) {
        return false;
    }
    target = static_cast<IntegerT>(value);
    return true;
}

// Cast VarcharT to BigIntT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &source, i64 &target) {
    if (!source.IsValue()) {
        Error<FunctionException>("No implementation to cast from column vector Varchar to big int");
    }
    char *endptr{nullptr};
    SizeT len{0};
    if (source.IsInlined()) {
        target = StrToL(source.short_.data_, &endptr, 10);
    } else {
        target = StrToL(source.value_.ptr_, &endptr, 10);
    }
    if (len != source.length_) {
        return false;
    }
    return true;
}

// Cast VarcharT to HugeIntT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &, HugeIntT &) {
    Error<TypeException>("Cast varchar to hugeint");
    return false;
}

// Cast VarcharT to FloatT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &source, FloatT &target) {
    if (!source.IsValue()) {
        Error<FunctionException>("No implementation to cast from column vector Varchar to big int");
    }
    char *endptr{nullptr};
    SizeT len{0};
    if (source.IsInlined()) {
        target = StrToF(source.short_.data_, &endptr);
    } else {
        target = StrToF(source.value_.ptr_, &endptr);
    }
    if (len != source.length_) {
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
        target = StrToD(source.short_.data_, &endptr);
    } else {
        target = StrToD(source.value_.ptr_, &endptr);
    }
    if (len != source.length_) {
        return false;
    }
    return true;
}

// Cast VarcharT to DateT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &, DateT &) {
    Error<TypeException>("Cast from varchar to date");
//    if (source.IsInlined()) {
//        target.FromString(source.prefix, source.length);
//    } else {
//        target.FromString(source.ptr, source.length);
//    }
    return true;
}

// Cast VarcharT to TimeT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &, TimeT &) {
    Error<TypeException>("Cast from varchar to time");
    return true;
}

// Cast VarcharT to DateTimeT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &, DateTimeT &) {
    Error<TypeException>("Cast from varchar to datetime");
    return true;
}

// Cast VarcharT to TimestampT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &, TimestampT &) {
    Error<TypeException>("Cast from varchar to timestamp");
    return true;
}

// Cast VarcharT to IntervalT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &, IntervalT &) {
    Error<TypeException>("Cast from varchar to interval");
    return true;
}

} // namespace infinity
