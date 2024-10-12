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

export module varchar_cast;

import stl;
import bound_cast_func;
import column_vector_cast;
import logical_type;
import infinity_exception;
import third_party;
import column_vector;
import vector_buffer;
import internal_types;
import data_type;
import status;
import logger;

namespace infinity {

export struct TryCastVarchar;
export struct TryCastVarcharVector;
export struct TryCastVarcharToChar;
export struct TryCastVarcharToVarchar;

export inline BoundCastFunc BindVarcharCast(const DataType &source, const DataType &target) {
    if (source.type() != LogicalType::kVarchar) {
        String error_message = fmt::format("Expect Varchar type, but it is {}", source.ToString());
        UnrecoverableError(error_message);
    }
    switch (target.type()) {
        case LogicalType::kBoolean: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, BooleanT, TryCastVarchar>);
        }
        case LogicalType::kTinyInt: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, TinyIntT, TryCastVarchar>);
        }
        case LogicalType::kSmallInt: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, SmallIntT, TryCastVarchar>);
        }
        case LogicalType::kInteger: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, IntegerT, TryCastVarchar>);
        }
        case LogicalType::kBigInt: {
            return BoundCastFunc(&ColumnVectorCast::TryCastVarlenColumnVector<VarcharT, BigIntT, TryCastVarcharVector>);
        }
        case LogicalType::kHugeInt: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, HugeIntT, TryCastVarchar>);
        }
        case LogicalType::kFloat: {
            return BoundCastFunc(&ColumnVectorCast::TryCastVarlenColumnVector<VarcharT, FloatT, TryCastVarcharVector>);
        }
        case LogicalType::kDouble: {
            return BoundCastFunc(&ColumnVectorCast::TryCastVarlenColumnVector<VarcharT, DoubleT, TryCastVarcharVector>);
        }
        case LogicalType::kFloat16: {
            return BoundCastFunc(&ColumnVectorCast::TryCastVarlenColumnVector<VarcharT, Float16T, TryCastVarcharVector>);
        }
        case LogicalType::kBFloat16: {
            return BoundCastFunc(&ColumnVectorCast::TryCastVarlenColumnVector<VarcharT, BFloat16T, TryCastVarcharVector>);
        }
        case LogicalType::kDecimal: {
            String error_message = fmt::format("Not implement cast from varchar to decimal128 type.", source.ToString(), target.ToString());
            UnrecoverableError(error_message);
        }
        case LogicalType::kVarchar: {
            String error_message = "Attempt to cast from varchar to varchar";
            UnrecoverableError(error_message);
        }
        case LogicalType::kDate: {
            return BoundCastFunc(&ColumnVectorCast::TryCastVarlenColumnVector<VarcharT, DateT, TryCastVarcharVector>);
        }
        case LogicalType::kTime: {
            return BoundCastFunc(&ColumnVectorCast::TryCastVarlenColumnVector<VarcharT, TimeT, TryCastVarcharVector>);
        }
        case LogicalType::kDateTime: {
            return BoundCastFunc(&ColumnVectorCast::TryCastVarlenColumnVector<VarcharT, DateTimeT, TryCastVarcharVector>);
        }
        case LogicalType::kTimestamp: {
            return BoundCastFunc(&ColumnVectorCast::TryCastVarlenColumnVector<VarcharT, TimestampT, TryCastVarcharVector>);
        }
        case LogicalType::kInterval: {
            return BoundCastFunc(&ColumnVectorCast::TryCastVarlenColumnVector<VarcharT, IntervalT, TryCastVarcharVector>);
        }
        case LogicalType::kArray: {
            String error_message = "Cast from varchar to array";
            UnrecoverableError(error_message);
        }
        case LogicalType::kTuple: {
            String error_message = "Cast from varchar to tuple";
            UnrecoverableError(error_message);
        }
        case LogicalType::kPoint: {
            String error_message = "Cast from varchar to point";
            UnrecoverableError(error_message);
        }
        case LogicalType::kLine: {
            String error_message = "Cast from varchar to line";
            UnrecoverableError(error_message);
        }
        case LogicalType::kLineSeg: {
            String error_message = "Cast from varchar to line segment";
            UnrecoverableError(error_message);
        }
        case LogicalType::kBox: {
            String error_message = "Cast from varchar to box";
            UnrecoverableError(error_message);
        }
            //        case kPath: {
            //            UnrecoverableError("Cast from varchar to path");
            //        }
            //        case kPolygon: {
            //            UnrecoverableError("Cast from varchar to polygon");
            //        }
        case LogicalType::kCircle: {
            String error_message = "Cast from varchar to circle";
            UnrecoverableError(error_message);
        }
            //        case kBitmap: {
            //            UnrecoverableError("Cast from varchar to bitmap");
            //        }
        case LogicalType::kUuid: {
            String error_message = "Cast from varchar to uuid";
            UnrecoverableError(error_message);
        }
            //        case kBlob: {
            //            UnrecoverableError("Cast from varchar to blob");
            //        }
        case LogicalType::kEmbedding: {
            RecoverableError(Status::NotSupport(fmt::format("Attempt to cast from {} to {}", source.ToString(), target.ToString())));
        }
        case LogicalType::kRowID: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, RowID, TryCastVarchar>);
        }
        case LogicalType::kMixed: {
            String error_message = "Cast from varchar to mix";
            UnrecoverableError(error_message);
        }
        default: {
            String error_message = "Can't convert varchar";
            UnrecoverableError(error_message);
        }
    }
    return BoundCastFunc(nullptr);
}

struct TryCastVarchar {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &, TargetType &) {
        String error_message =
            fmt::format("No implementation to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>());
        UnrecoverableError(error_message);
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
    if (source.IsInlined()) {
        auto [ptr, ec] = std::from_chars(source.short_.data_, source.short_.data_ + source.length_, target);
        if (ec != std::errc()) {
            return false;
        }
    } else {
        // No tiny int isn't inline
        return false;
    }

    return true;
}

// Cast VarcharT to SmallIntT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &source, SmallIntT &target) {
    if (source.IsInlined()) {
        auto [ptr, ec] = std::from_chars(source.short_.data_, source.short_.data_ + source.length_, target);
        if (ec != std::errc()) {
            return false;
        }
    } else {
        // No small int isn't inline
        return false;
    }

    return true;
}

// Cast VarcharT to IntegerT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &source, IntegerT &target) {
    if (source.IsInlined()) {
        auto [ptr, ec] = std::from_chars(source.short_.data_, source.short_.data_ + source.length_, target);
        if (ec != std::errc()) {
            return false;
        }
    } else {
        // No integer isn't inline
        return false;
    }

    return true;
}

// Cast VarcharT to DateT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &, DateT &) {
    String error_message = "Cast from varchar to date";
    UnrecoverableError(error_message);
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
    String error_message = "Cast from varchar to time";
    UnrecoverableError(error_message);
    return true;
}

// Cast VarcharT to DateTimeT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &, DateTimeT &) {
    String error_message = "Cast from varchar to datetime";
    UnrecoverableError(error_message);
    return true;
}

// Cast VarcharT to TimestampT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &source, TimestampT &target) {
    String error_message = "Cast from varchar to timestamp";
    UnrecoverableError(error_message);
    return true;
}

// Cast VarcharT to IntervalT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &, IntervalT &) {
    String error_message = "Cast from varchar to interval";
    UnrecoverableError(error_message);
    return true;
}

struct TryCastVarcharVector {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &, ColumnVector *source_vector, TargetType &) {
        String error_message =
            fmt::format("No implementation to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>());
        UnrecoverableError(error_message);
        return false;
    }
};

// Cast VarcharT to DateT type
template <>
inline bool TryCastVarcharVector::Run(const VarcharT &source, ColumnVector *source_vector, DateT &target) {
    Span<const char> data = source_vector->GetVarcharInner(source);
    // Used in libc++
    String substr(data.data(), data.size());
    target.FromString(substr);
    return true;
}

// Cast VarcharT to TimeT type
template <>
inline bool TryCastVarcharVector::Run(const VarcharT &source, ColumnVector *source_vector, TimeT &target) {
    Span<const char> data = source_vector->GetVarcharInner(source);
    // Used in libc++
    String substr(data.data(), data.size());
    target.FromString(substr);
    return true;
}

// Cast VarcharT to DateTimeT type
template <>
inline bool TryCastVarcharVector::Run(const VarcharT &source, ColumnVector *source_vector, DateTimeT &target) {
    Span<const char> data = source_vector->GetVarcharInner(source);
    // Used in libc++
    String substr(data.data(), data.size());
    target.FromString(substr);
    return true;
}

// Cast VarcharT to TimestampT type
template <>
inline bool TryCastVarcharVector::Run(const VarcharT &source, ColumnVector *source_vector, TimestampT &target) {
    Span<const char> data = source_vector->GetVarcharInner(source);
    // Used in libc++
    String substr(data.data(), data.size());
    target.FromString(substr);
    return true;
}

// Cast VarcharT to BigIntT type
template <>
inline bool TryCastVarcharVector::Run(const VarcharT &source, ColumnVector *source_vector, i64 &target) {
    Span<const char> data = source_vector->GetVarcharInner(source);
    auto [ptr, ec] = std::from_chars(data.data(), data.data() + data.size(), target);
    if (ec != std::errc()) {
        return false;
    }
    return true;
}

// Cast VarcharT to FloatT type
template <>
inline bool TryCastVarcharVector::Run(const VarcharT &source, ColumnVector *source_vector, FloatT &target) {
    Span<const char> data = source_vector->GetVarcharInner(source);
    // Used in libc++
    String substr(data.data(), data.size());
    try {
        target = std::stof(substr);
    } catch (const std::exception &e) {
        return false;
    }
    // Used in libstdc++
    // auto [ptr, ec] = std::from_chars(data.data(), data.data() + data.size(), target);
    // if (ec != std::errc()) {
    //     return false;
    // }
    return true;
}

// Cast VarcharT to DoubleT type
template <>
inline bool TryCastVarcharVector::Run(const VarcharT &source, ColumnVector *source_vector, DoubleT &target) {
    Span<const char> data = source_vector->GetVarcharInner(source);
    // Used in libc++
    String substr(data.data(), data.size());
    try {
        target = std::stod(substr);
    } catch (const std::exception &e) {
        return false;
    }
    // Used in libstdc++
    // auto [ptr, ec] = std::from_chars(data.data(), data.data() + data.size(), target);
    // if (ec != std::errc()) {
    //     return false;
    // }
    return true;
}

// Cast VarcharT to Float16T type
template <>
inline bool TryCastVarcharVector::Run(const VarcharT &source, ColumnVector *source_vector, Float16T &target) {
    // Used in libc++
    Span<const char> data = source_vector->GetVarcharInner(source);
    String substr(data.data(), data.size());
    try {
        target = std::stof(substr);
    } catch (const std::exception &e) {
        return false;
    }
    // Used in libstdc++
    // auto [ptr, ec] = std::from_chars(data.data(), data.data() + data.size(), target);
    // if (ec != std::errc()) {
    //     return false;
    // }
    return true;
}

// Cast VarcharT to BFloat16T type
template <>
inline bool TryCastVarcharVector::Run(const VarcharT &source, ColumnVector *source_vector, BFloat16T &target) {
    Span<const char> data = source_vector->GetVarcharInner(source);
    // Used in libc++
    String substr(data.data(), data.size());
    try {
        target = std::stof(substr);
    } catch (const std::exception &e) {
        return false;
    }
    // Used in libstdc++
    // auto [ptr, ec] = std::from_chars(data.data(), data.size() + data.size(), target);
    // if (ec != std::errc()) {
    //     return false;
    // }
    return true;
}

} // namespace infinity
