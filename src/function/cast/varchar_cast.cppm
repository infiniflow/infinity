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

namespace infinity {

export struct TryCastVarchar;
export struct TryCastVarcharVector;
export struct TryCastVarcharToChar;
export struct TryCastVarcharToVarchar;

export inline BoundCastFunc BindVarcharCast(const DataType &source, const DataType &target) {
    if (source.type() != LogicalType::kVarchar) {
        UnrecoverableError(fmt::format("Expect Varchar type, but it is {}", source.ToString()));
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
            return BoundCastFunc(&ColumnVectorCast::TryCastVarlenColumnVector<VarcharT, BigIntT, TryCastVarcharVector>);
        }
        case kHugeInt: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, HugeIntT, TryCastVarchar>);
        }
        case kFloat: {
            return BoundCastFunc(&ColumnVectorCast::TryCastVarlenColumnVector<VarcharT, FloatT, TryCastVarcharVector>);
        }
        case kDouble: {
            return BoundCastFunc(&ColumnVectorCast::TryCastVarlenColumnVector<VarcharT, DoubleT, TryCastVarcharVector>);
        }
        case kDecimal: {
            UnrecoverableError(fmt::format("Not implement cast from varchar to decimal128 type.", source.ToString(), target.ToString()));
        }
        case kVarchar: {
            UnrecoverableError("Attempt to cast from varchar to varchar");
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
            UnrecoverableError("Cast from varchar to array");
        }
        case kTuple: {
            UnrecoverableError("Cast from varchar to tuple");
        }
        case kPoint: {
            UnrecoverableError("Cast from varchar to point");
        }
        case kLine: {
            UnrecoverableError("Cast from varchar to line");
        }
        case kLineSeg: {
            UnrecoverableError("Cast from varchar to line segment");
        }
        case kBox: {
            UnrecoverableError("Cast from varchar to box");
        }
            //        case kPath: {
            //            UnrecoverableError("Cast from varchar to path");
            //        }
            //        case kPolygon: {
            //            UnrecoverableError("Cast from varchar to polygon");
            //        }
        case kCircle: {
            UnrecoverableError("Cast from varchar to circle");
        }
            //        case kBitmap: {
            //            UnrecoverableError("Cast from varchar to bitmap");
            //        }
        case kUuid: {
            UnrecoverableError("Cast from varchar to uuid");
        }
            //        case kBlob: {
            //            UnrecoverableError("Cast from varchar to blob");
            //        }
        case kEmbedding: {
            UnrecoverableError("Cast from varchar to embedding");
        }
        case kRowID: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<VarcharT, RowID, TryCastVarchar>);
        }
        case kMixed: {
            UnrecoverableError("Cast from varchar to mix");
        }
        default: {
            UnrecoverableError("Can't convert varchar");
        }
    }
    return BoundCastFunc(nullptr);
}

struct TryCastVarchar {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &, TargetType &) {
        UnrecoverableError(
            fmt::format("No implementation to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
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
        value = std::strtol(source.short_.data_, &endptr, 10);
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
        value = std::strtol(source.short_.data_, &endptr, 10);
        len = (endptr - source.short_.data_);
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
        value = std::strtol(source.short_.data_, &endptr, 10);
        len = (endptr - source.short_.data_);
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
        UnrecoverableError("No implementation to cast from column vector Varchar to big int");
    }
    char *endptr{nullptr};
    SizeT len{0};
    if (source.IsInlined()) {
        target = std::strtol(source.short_.data_, &endptr, 10);
        len = (endptr - source.short_.data_);
    } else {
        target = std::strtol(source.value_.ptr_, &endptr, 10);
    }
    if (len != source.length_) {
        return false;
    }
    return true;
}

// Cast VarcharT to HugeIntT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &, HugeIntT &) {
    UnrecoverableError("Cast varchar to hugeint");
    return false;
}

// Cast VarcharT to FloatT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &source, FloatT &target) {
    if (!source.IsValue()) {
        UnrecoverableError("No implementation to cast from column vector Varchar to big int");
    }
    char *endptr{nullptr};
    SizeT len{0};
    if (source.IsInlined()) {
        target = std::strtof(source.short_.data_, &endptr);
    } else {
        target = std::strtof(source.value_.ptr_, &endptr);
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
        target = std::strtod(source.short_.data_, &endptr);
    } else {
        target = std::strtod(source.value_.ptr_, &endptr);
    }
    if (len != source.length_) {
        return false;
    }
    return true;
}

// Cast VarcharT to DateT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &, DateT &) {
    UnrecoverableError("Cast from varchar to date");
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
    UnrecoverableError("Cast from varchar to time");
    return true;
}

// Cast VarcharT to DateTimeT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &, DateTimeT &) {
    UnrecoverableError("Cast from varchar to datetime");
    return true;
}

// Cast VarcharT to TimestampT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &, TimestampT &) {
    UnrecoverableError("Cast from varchar to timestamp");
    return true;
}

// Cast VarcharT to IntervalT type
template <>
inline bool TryCastVarchar::Run(const VarcharT &, IntervalT &) {
    UnrecoverableError("Cast from varchar to interval");
    return true;
}

struct TryCastVarcharVector {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &, ColumnVector* source_vector, TargetType &) {
        UnrecoverableError(
            fmt::format("No implementation to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
    }
};

// Cast VarcharT to BigIntT type
template <>
inline bool TryCastVarcharVector::Run(const VarcharT &source, ColumnVector* source_vector, i64 &target) {
    char *endptr{nullptr};
    SizeT len{0};
    if (source.IsInlined()) {
        target = std::strtol(source.short_.data_, &endptr, 10);
        len = (endptr - source.short_.data_);
    } else {
        if(source.IsValue()) {
            target = std::strtol(source.value_.ptr_, &endptr, 10);
            len = (endptr - source.value_.ptr_);
        } else {
            // varchar is vector
            SizeT varchar_len = source.length_;
            u32 chunk_id = source.vector_.chunk_id_;
            u32 chunk_offset = source.vector_.chunk_offset_;
            auto varchar_ptr = MakeUniqueForOverwrite<char[]>(varchar_len + 1);
            varchar_ptr[varchar_len] = '\0';
            source_vector->buffer_->fix_heap_mgr_->ReadFromHeap(varchar_ptr.get(), chunk_id, chunk_offset, varchar_len);
            target = std::strtol(varchar_ptr.get(), &endptr, 10);
            len = (endptr - varchar_ptr.get());
        }
    }
    if (len != source.length_) {
        return false;
    }
    return true;
}

// Cast VarcharT to FloatT type
template <>
inline bool TryCastVarcharVector::Run(const VarcharT &source, ColumnVector* source_vector, FloatT &target) {
    char *endptr{nullptr};
    SizeT len{0};
    if (source.IsInlined()) {
        target = std::strtof(source.short_.data_, &endptr);
        len = (endptr - source.short_.data_);
    } else {
        if(source.IsValue()) {
            target = std::strtof(source.value_.ptr_, &endptr);
            len = (endptr - source.value_.ptr_);
        } else {
            // varchar is vector
            SizeT varchar_len = source.length_;
            u32 chunk_id = source.vector_.chunk_id_;
            u32 chunk_offset = source.vector_.chunk_offset_;
            auto varchar_ptr = MakeUniqueForOverwrite<char[]>(varchar_len + 1);
            varchar_ptr[varchar_len] = '\0';
            source_vector->buffer_->fix_heap_mgr_->ReadFromHeap(varchar_ptr.get(), chunk_id, chunk_offset, varchar_len);
            target = std::strtof(varchar_ptr.get(), &endptr);
            len = (endptr - varchar_ptr.get());
        }
    }
    if (len != source.length_) {
        return false;
    }
    return true;
}

// Cast VarcharT to DoubleT type
template <>
inline bool TryCastVarcharVector::Run(const VarcharT &source, ColumnVector* source_vector, DoubleT &target) {
    char *endptr{nullptr};
    SizeT len{0};
    if (source.IsInlined()) {
        target = std::strtod(source.short_.data_, &endptr);
        len = (endptr - source.short_.data_);
    } else {
        if(source.IsValue()) {
            target = std::strtod(source.value_.ptr_, &endptr);
            len = (endptr - source.value_.ptr_);
        } else {
            // varchar is vector
            SizeT varchar_len = source.length_;
            u32 chunk_id = source.vector_.chunk_id_;
            u32 chunk_offset = source.vector_.chunk_offset_;
            auto varchar_ptr = MakeUniqueForOverwrite<char[]>(varchar_len + 1);
            varchar_ptr[varchar_len] = '\0';
            source_vector->buffer_->fix_heap_mgr_->ReadFromHeap(varchar_ptr.get(), chunk_id, chunk_offset, varchar_len);
            target = std::strtod(varchar_ptr.get(), &endptr);
            len = (endptr - varchar_ptr.get());
        }
    }
    if (len != source.length_) {
        return false;
    }
    return true;
}

} // namespace infinity
