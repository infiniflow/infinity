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

#include <cmath>

import parser;
import stl;
import bound_cast_func;
import vector_buffer;
import column_vector_cast;

import infinity_exception;
import third_party;
import column_vector;

export module float_cast;

namespace infinity {

export struct FloatTryCastToFixlen;
export struct FloatTryCastToVarlen;

export template <class SourceType>
inline BoundCastFunc BindFloatCast(const DataType &source, const DataType &target) {
    Assert<TypeException>(source.type() != target.type(),
                          Format("Attempt to cast from {} to {}", source.ToString(), target.ToString()));
    switch (target.type()) {
        case LogicalType::kTinyInt: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<SourceType, TinyIntT, FloatTryCastToFixlen>);
        }
        case LogicalType::kSmallInt: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<SourceType, SmallIntT, FloatTryCastToFixlen>);
        }
        case LogicalType::kInteger: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<SourceType, IntegerT, FloatTryCastToFixlen>);
        }
        case LogicalType::kBigInt: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<SourceType, BigIntT, FloatTryCastToFixlen>);
        }
        case LogicalType::kHugeInt: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<SourceType, HugeIntT, FloatTryCastToFixlen>);
        }
        case LogicalType::kFloat: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<SourceType, FloatT, FloatTryCastToFixlen>);
        }
        case LogicalType::kDouble: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<SourceType, DoubleT, FloatTryCastToFixlen>);
        }
        case LogicalType::kDecimal: {
            Error<NotImplementException>(Format("Not implement cast from numeric to decimal128 type.", source.ToString(), target.ToString()));
        }
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<SourceType, VarcharT, FloatTryCastToVarlen>);
        }
        default: {
            Error<TypeException>(Format("Attempt to cast from {} to {}", source.ToString(), target.ToString()));
        }
    }
}

struct FloatTryCastToFixlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType source, TargetType &target) {
        Error<FunctionException>(
            Format("Not implemented to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));;
    }
};

struct FloatTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType source, TargetType &target, const SharedPtr<ColumnVector> &vector_ptr) {
        Error<FunctionException>(
            Format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));;
    }
};

// Cast FloatT to other numeric type
template <>
inline bool FloatTryCastToFixlen::Run(FloatT source, TinyIntT &target) {
    if (source < -128.0f || source > 127.0f) {
        return false;
    }
    target = static_cast<TinyIntT>(source);
    return true;
}

template <>
inline bool FloatTryCastToFixlen::Run(FloatT source, SmallIntT &target) {
    if (source < -32768.0f || source > 32767.0f) {
        return false;
    }
    target = static_cast<SmallIntT>(source);
    return true;
}

template <>
inline bool FloatTryCastToFixlen::Run(FloatT source, IntegerT &target) {
    if (!(source >= -2147483648.0f && source < 2147483648.0f)) {
        return false;
    }
    target = std::nearbyint(source);
    if (source > 0 && target < 0) {
        return false;
    }
    return true;
}

template <>
inline bool FloatTryCastToFixlen::Run(FloatT source, BigIntT &target) {
    if (source < -9223372036854775808.0f || source > 9223372036854775807.0f) {
        return false;
    }
    target = std::nearbyint(source);
    return true;
}

// TODO: Cast from float to hugeint
template <>
inline bool FloatTryCastToFixlen::Run(FloatT source, HugeIntT &target) {
    Error<NotImplementException>("Not implemented");
}

template <>
inline bool FloatTryCastToFixlen::Run(FloatT source, DoubleT &target) {
    target = source;
    return true;
}

// TODO
template <>
inline bool FloatTryCastToFixlen::Run(FloatT source, DecimalT &target) {
    Error<NotImplementException>("Not implemented");
}

// Cast FloatT to varlen type
template <>
inline bool FloatTryCastToVarlen::Run(FloatT source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    // TODO: High-performance to_string implementation is needed.
    String tmp_str = ToStr(source);
    target.length = static_cast<u16>(tmp_str.size());
    if (target.length <= VarcharT::INLINE_LENGTH) {
        Memcpy(target.prefix, tmp_str.c_str(), target.length);
        Memset(target.prefix + target.length, 0, VarcharT::INLINE_LENGTH - target.length);
    } else {
        Memcpy(target.prefix, tmp_str.c_str(), VarcharT::PREFIX_LENGTH);
        Assert<TypeException>(vector_ptr->buffer_->buffer_type_ == VectorBufferType::kHeap,
                              "Varchar column vector should use MemoryVectorBuffer. ");

        ptr_t ptr = vector_ptr->buffer_->heap_mgr_->Allocate(target.length);
        Memcpy(ptr, tmp_str.c_str(), target.length);
        target.ptr = ptr;
    }

    return true;
}

// Cast DoubleT to other numeric type
template <>
inline bool FloatTryCastToFixlen::Run(DoubleT source, TinyIntT &target) {
    if (source < -128.0f || source > 127.0f) {
        return false;
    }
    target = static_cast<TinyIntT>(source);
    return true;
}

template <>
inline bool FloatTryCastToFixlen::Run(DoubleT source, SmallIntT &target) {
    if (source < -32768.0f || source > 32767.0f) {
        return false;
    }
    target = static_cast<SmallIntT>(source);
    return true;
}

template <>
inline bool FloatTryCastToFixlen::Run(DoubleT source, IntegerT &target) {
    if (source < -2147483648.0f || source > 2147483647.0f) {
        return false;
    }
    target = std::nearbyint(source);
    return true;
}

template <>
inline bool FloatTryCastToFixlen::Run(DoubleT source, BigIntT &target) {
    if (!(source >= -9223372036854775808.0f && source < 9223372036854775808.0f)) {
        return false;
    }
    target = std::nearbyint(source);
    if (source > 0 && target < 0) {
        return false;
    }
    return true;
}

// TODO: Cast from double to hugeint
template <>
inline bool FloatTryCastToFixlen::Run(DoubleT source, HugeIntT &target) {
    Error<NotImplementException>("Not implemented");
}

template <>
inline bool FloatTryCastToFixlen::Run(DoubleT source, FloatT &target) {
    target = source;
    return true;
}

// TODO
template <>
inline bool FloatTryCastToFixlen::Run(DoubleT source, DecimalT &target) {
    Error<NotImplementException>("Not implemented");
}

// Cast double to varlen type
template <>
inline bool FloatTryCastToVarlen::Run(DoubleT source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    // TODO: High-performance to_string implementation is needed.
    String tmp_str = ToStr(source);
    target.length = static_cast<u16>(tmp_str.size());
    if (target.length <= VarcharT::INLINE_LENGTH) {
        Memcpy(target.prefix, tmp_str.c_str(), target.length);
        Memset(target.prefix + target.length, 0, VarcharT::INLINE_LENGTH - target.length);
    } else {
        Memcpy(target.prefix, tmp_str.c_str(), VarcharT::PREFIX_LENGTH);
        Assert<TypeException>(vector_ptr->buffer_->buffer_type_ == VectorBufferType::kHeap,
                              "Varchar column vector should use MemoryVectorBuffer. ");

        ptr_t ptr = vector_ptr->buffer_->heap_mgr_->Allocate(target.length);
        Memcpy(ptr, tmp_str.c_str(), target.length);
        target.ptr = ptr;
    }

    return true;
}

} // namespace infinity
