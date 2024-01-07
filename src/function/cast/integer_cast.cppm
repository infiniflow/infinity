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

export module integer_cast;

namespace infinity {

export struct IntegerTryCastToFixlen;
export struct IntegerTryCastToVarlen;

export template <class SourceType>
inline BoundCastFunc BindIntegerCast(const DataType &source, const DataType &target) {
    if (source.type() == target.type()) {
        Error<FunctionException>("Can't cast from the same type");
    }
    switch (target.type()) {
        case LogicalType::kTinyInt: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<SourceType, TinyIntT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kSmallInt: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<SourceType, SmallIntT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kInteger: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<SourceType, IntegerT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kBigInt: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<SourceType, BigIntT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kHugeInt: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<SourceType, HugeIntT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kFloat: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<SourceType, FloatT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kDouble: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<SourceType, DoubleT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kDecimal: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<SourceType, DecimalT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<SourceType, VarcharT, IntegerTryCastToVarlen>);
        }
        default: {
            Error<TypeException>(fmt::format("Attempt to cast from {} to {}", source.ToString(), target.ToString()));
        }
    }
    return BoundCastFunc(nullptr);
}

struct IntegerTryCastToFixlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType, TargetType &) {
        Error<FunctionException>(
            fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
    }
};

struct IntegerTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType, TargetType &, const SharedPtr<ColumnVector> &) {
        Error<FunctionException>(
            fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
    }
};

// Cast TinyInt to other numeric type
template <>
inline bool IntegerTryCastToFixlen::Run(TinyIntT source, SmallIntT &target) {
    target = source;
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(TinyIntT source, IntegerT &target) {
    target = source;
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(TinyIntT source, BigIntT &target) {
    target = source;
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(TinyIntT source, HugeIntT &target) {
    target.lower = source;
    target.upper = (source < 0) * -1;
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(TinyIntT source, FloatT &target) {
    target = source;
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(TinyIntT source, DoubleT &target) {
    target = source;
    return true;
}

// TODO
template <>
inline bool IntegerTryCastToFixlen::Run(TinyIntT, DecimalT &) {
    Error<NotImplementException>(fmt::format("Not implemented"));
    return false;
}

// Cast TinyIntT to VarcharT type
template <>
inline bool IntegerTryCastToVarlen::Run(TinyIntT source, VarcharT &target, const SharedPtr<ColumnVector> &) {
    target.is_value_ = false;
    if (source == 0) {
        target.short_.data_[0] = '0';
        target.length_ = 1;
        return true;
    }
    // TODO: High performance itoa needed here.
    String tmp_str = ToStr(source);
    target.length_ = static_cast<u32>(tmp_str.size());
    if (target.length_ > VARCHAR_INLINE_LEN) {
        Error<TypeException>("Integer digits number should less than 14.");
    }
    Memcpy(target.short_.data_, tmp_str.c_str(), target.length_);
    return true;
}

// Cast SmallInt to other numeric type
template <>
inline bool IntegerTryCastToFixlen::Run(SmallIntT source, TinyIntT &target) {
    if (source < i8_min || source > i8_max) {
        return false;
    }
    target = static_cast<TinyIntT>(source);
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(SmallIntT source, IntegerT &target) {
    target = source;
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(SmallIntT source, BigIntT &target) {
    target = source;
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(SmallIntT source, HugeIntT &target) {
    target.lower = source;
    target.upper = (source < 0) * -1;
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(SmallIntT source, FloatT &target) {
    target = source;
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(SmallIntT source, DoubleT &target) {
    target = source;
    return true;
}

// TODO
template <>
inline bool IntegerTryCastToFixlen::Run(SmallIntT, DecimalT &) {
    Error<NotImplementException>("Not implemented");
    return false;
}

// Cast SmallIntT to VarcharT type
template <>
inline bool IntegerTryCastToVarlen::Run(SmallIntT source, VarcharT &target, const SharedPtr<ColumnVector> &) {
    target.is_value_ = false;
    if (source == 0) {
        target.short_.data_[0] = '0';
        target.length_ = 1;
        return true;
    }
    // TODO: High performance itoa needed here.
    String tmp_str = ToStr(source);
    target.length_ = static_cast<u32>(tmp_str.size());
    if (target.length_ > VARCHAR_INLINE_LEN) {
        Error<TypeException>("Integer digits number should less than 14.");
    }
    Memcpy(target.short_.data_, tmp_str.c_str(), target.length_);
    return true;
}

// Cast Integer to other numeric type
template <>
inline bool IntegerTryCastToFixlen::Run(IntegerT source, TinyIntT &target) {
    if (source < i8_min || source > i8_max) {
        return false;
    }
    target = static_cast<TinyIntT>(source);
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(IntegerT source, SmallIntT &target) {
    if (source < i16_min || source > i16_max) {
        return false;
    }
    target = static_cast<SmallIntT>(source);
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(IntegerT source, BigIntT &target) {
    target = source;
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(IntegerT source, HugeIntT &target) {
    target.lower = source;
    target.upper = (source < 0) * -1;
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(IntegerT source, FloatT &target) {
    target = source;
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(IntegerT source, DoubleT &target) {
    target = source;
    return true;
}

// TODO
template <>
inline bool IntegerTryCastToFixlen::Run(IntegerT, DecimalT &) {
    Error<NotImplementException>("Not implemented");
    return false;
}

// Cast IntegerT to VarcharT type
template <>
inline bool IntegerTryCastToVarlen::Run(IntegerT source, VarcharT &target, const SharedPtr<ColumnVector> &) {
    target.is_value_ = false;
    if (source == 0) {
        target.short_.data_[0] = '0';
        target.length_ = 1;
        return true;
    }
    // TODO: High performance itoa needed here.
    String tmp_str = ToStr(source);
    target.length_ = static_cast<u32>(tmp_str.size());
    if (target.length_ > VARCHAR_INLINE_LEN) {
        Error<TypeException>("Integer digits number should less than 14.");
    }
    Memcpy(target.short_.data_, tmp_str.c_str(), target.length_);
    return true;
}

// Cast BigInt to other numeric type
template <>
inline bool IntegerTryCastToFixlen::Run(BigIntT source, TinyIntT &target) {
    if (source < i8_min || source > i8_max) {
        return false;
    }
    target = static_cast<TinyIntT>(source);
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(BigIntT source, SmallIntT &target) {
    if (source < i16_min || source > i16_max) {
        return false;
    }
    target = static_cast<SmallIntT>(source);
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(BigIntT source, IntegerT &target) {
    if (source < i32_min || source > i32_max) {
        return false;
    }
    target = static_cast<IntegerT>(source);
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(BigIntT source, HugeIntT &target) {
    target.lower = source;
    target.upper = (source < 0) * -1;
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(BigIntT source, FloatT &target) {
    target = source;
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(BigIntT source, DoubleT &target) {
    target = source;
    return true;
}

// TODO
template <>
inline bool IntegerTryCastToFixlen::Run(BigIntT, DecimalT &) {
    Error<NotImplementException>("Not implemented");
    return false;
}

// Cast integer to varlen type
template <>
inline bool IntegerTryCastToVarlen::Run(BigIntT source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    target.is_value_ = false;
    if (source == 0) {
        target.short_.data_[0] = '0';
        target.length_ = 1;
        return true;
    }
    // TODO: High performance itoa needed here.
    String tmp_str = ToStr(source);
    target.length_ = static_cast<u32>(tmp_str.size());
    if (target.length_ <= VARCHAR_INLINE_LEN) {
        Memcpy(target.short_.data_, tmp_str.c_str(), target.length_);
    } else {
        Memcpy(target.vector_.prefix_, tmp_str.c_str(), VARCHAR_PREFIX_LEN);
        if (vector_ptr->buffer_->buffer_type_ != VectorBufferType::kHeap) {
            Error<TypeException>("Varchar column vector should use MemoryVectorBuffer. ");
        }
        auto [chunk_id, chunk_offset] = vector_ptr->buffer_->fix_heap_mgr_->AppendToHeap(tmp_str.c_str(), target.length_);
        target.vector_.chunk_id_ = chunk_id;
        target.vector_.chunk_offset_ = chunk_offset;
    }

    return true;
}

// TODO: Cast HugeInt to other numeric type
template <>
inline bool IntegerTryCastToFixlen::Run(HugeIntT, TinyIntT &) {
    Error<NotImplementException>("Not implemented");
    return false;
}

template <>
inline bool IntegerTryCastToFixlen::Run(HugeIntT, SmallIntT &) {
    Error<NotImplementException>("Not implemented");
    return false;
}

template <>
inline bool IntegerTryCastToFixlen::Run(HugeIntT, IntegerT &) {
    Error<NotImplementException>("Not implemented");
    return false;
}

template <>
inline bool IntegerTryCastToFixlen::Run(HugeIntT, BigIntT &) {
    Error<NotImplementException>("Not implemented");
    return false;
}

template <>
inline bool IntegerTryCastToFixlen::Run(HugeIntT, FloatT &) {
    Error<NotImplementException>("Not implemented");
    return false;
}

template <>
inline bool IntegerTryCastToFixlen::Run(HugeIntT, DoubleT &) {
    Error<NotImplementException>("Not implemented");
    return false;
}

// TODO
template <>
inline bool IntegerTryCastToFixlen::Run(HugeIntT, DecimalT &) {
    Error<NotImplementException>("Not implemented");
    return false;
}

// Cast integer to varlen type
template <>
inline bool IntegerTryCastToVarlen::Run(HugeIntT, VarcharT &, const SharedPtr<ColumnVector> &) {
    Error<NotImplementException>("Not implemented");
    return false;
}

} // namespace infinity
