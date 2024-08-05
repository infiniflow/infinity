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

export module float_cast;

import stl;
import bound_cast_func;
import vector_buffer;
import column_vector_cast;
import logical_type;
import infinity_exception;
import third_party;
import column_vector;
import internal_types;
import data_type;
import status;
import logger;

namespace infinity {

export struct FloatTryCastToFixlen;
export struct FloatTryCastToVarlen;

export template <class SourceType>
inline BoundCastFunc BindFloatCast(const DataType &source, const DataType &target) {
    if (source.type() == target.type()) {
        String error_message = "Can't cast from the same type";
        UnrecoverableError(error_message);
    }
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
        case LogicalType::kFloat16: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<SourceType, Float16T, FloatTryCastToFixlen>);
        }
        case LogicalType::kBFloat16: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<SourceType, BFloat16T, FloatTryCastToFixlen>);
        }
        case LogicalType::kDecimal: {
            String error_message = fmt::format("Not implement cast from numeric to decimal128 type.", source.ToString(), target.ToString());
            UnrecoverableError(error_message);
        }
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<SourceType, VarcharT, FloatTryCastToVarlen>);
        }
        case LogicalType::kBoolean:
        case LogicalType::kEmbedding: {
            Status status = Status::NotSupport(fmt::format("Attempt to cast from {} to {}", source.ToString(), target.ToString()));
            RecoverableError(status);
        }
        default: {
            String error_message = fmt::format("Attempt to cast from {} to {}", source.ToString(), target.ToString());
            UnrecoverableError(error_message);
        }
    }
    return BoundCastFunc(nullptr);
}

struct FloatTryCastToFixlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType, TargetType &) {
        String error_message = fmt::format("Not implemented to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>());
        UnrecoverableError(error_message);
        return false;
    }
};

struct FloatTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType, TargetType &, ColumnVector*) {
        String error_message = fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>());
        UnrecoverableError(error_message);
        return false;
    }
};

// Cast FloatT to other numeric type
template <>
inline bool FloatTryCastToFixlen::Run(FloatT source, u8 &target) {
    if (source < 0.0f || source > 255.0f) {
        return false;
    }
    target = static_cast<u8>(source);
    return true;
}

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
inline bool FloatTryCastToFixlen::Run(FloatT, HugeIntT &) {
    String error_message = "Not implement: FloatTryCastToFixlen::Run";
    UnrecoverableError(error_message);
    return false;
}

template <>
inline bool FloatTryCastToFixlen::Run(FloatT source, DoubleT &target) {
    target = source;
    return true;
}

template <>
inline bool FloatTryCastToFixlen::Run(FloatT source, Float16T &target) {
    target = source;
    return true;
}

template <>
inline bool FloatTryCastToFixlen::Run(FloatT source, BFloat16T &target) {
    target = source;
    return true;
}

// TODO
template <>
inline bool FloatTryCastToFixlen::Run(FloatT, DecimalT &) {
    String error_message = "Not implement: FloatTryCastToFixlen::Run";
    UnrecoverableError(error_message);
    return false;
}

// Cast FloatT to varlen type
template <>
inline bool FloatTryCastToVarlen::Run(FloatT source, VarcharT &target, ColumnVector* vector_ptr) {
    target.is_value_ = false;
    String tmp_str = std::to_string(source);
    target.length_ = static_cast<u32>(tmp_str.size());

    if (target.length_ <= VARCHAR_INLINE_LEN) {
        std::memcpy(target.short_.data_, tmp_str.c_str(), target.length_);
    } else {
        std::memcpy(target.vector_.prefix_, tmp_str.c_str(), VARCHAR_PREFIX_LEN);
        if (vector_ptr->buffer_->buffer_type_ != VectorBufferType::kHeap) {
            String error_message = "Varchar column vector should use MemoryVectorBuffer.";
            UnrecoverableError(error_message);
        }
        auto [chunk_id, chunk_offset] = vector_ptr->buffer_->fix_heap_mgr_->AppendToHeap(tmp_str.c_str(), target.length_);
        target.vector_.chunk_id_ = chunk_id;
        target.vector_.chunk_offset_ = chunk_offset;
    }

    return true;
}

// Cast DoubleT to other numeric type
template <>
inline bool FloatTryCastToFixlen::Run(DoubleT source, u8 &target) {
    if (source < 0.0f || source > 255.0f) {
        return false;
    }
    target = static_cast<u8>(source);
    return true;
}

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
inline bool FloatTryCastToFixlen::Run(DoubleT, HugeIntT &) {
    String error_message = "Not implement: FloatTryCastToFixlen::Run";
    UnrecoverableError(error_message);
    return false;
}

template <>
inline bool FloatTryCastToFixlen::Run(DoubleT source, FloatT &target) {
    target = source;
    return true;
}

template <>
inline bool FloatTryCastToFixlen::Run(DoubleT source, Float16T &target) {
    target = static_cast<float>(source);
    return true;
}

template <>
inline bool FloatTryCastToFixlen::Run(DoubleT source, BFloat16T &target) {
    target = static_cast<float>(source);
    return true;
}

// TODO
template <>
inline bool FloatTryCastToFixlen::Run(DoubleT, DecimalT &) {
    String error_message = "Not implement: FloatTryCastToFixlen::Run";
    UnrecoverableError(error_message);
    return false;
}

// Cast double to varlen type
template <>
inline bool FloatTryCastToVarlen::Run(DoubleT source, VarcharT &target, ColumnVector* vector_ptr) {
    // TODO: High-performance to_string implementation is needed.
    target.is_value_ = false;
    String tmp_str = std::to_string(source);
    target.length_ = static_cast<u32>(tmp_str.size());

    if (target.length_ <= VARCHAR_INLINE_LEN) {
        std::memcpy(target.short_.data_, tmp_str.c_str(), target.length_);
    } else {
        std::memcpy(target.vector_.prefix_, tmp_str.c_str(), VARCHAR_PREFIX_LEN);
        if (vector_ptr->buffer_->buffer_type_ != VectorBufferType::kHeap) {
            String error_message = "Varchar column vector should use MemoryVectorBuffer. ";
            UnrecoverableError(error_message);
        }
        auto [chunk_id, chunk_offset] = vector_ptr->buffer_->fix_heap_mgr_->AppendToHeap(tmp_str.c_str(), target.length_);
        target.vector_.chunk_id_ = chunk_id;
        target.vector_.chunk_offset_ = chunk_offset;
    }


    return true;
}

// FOR_EACH
#define PARENS ()
#define EXPAND(...) EXPAND4(EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__))))
#define EXPAND4(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
#define EXPAND3(...) EXPAND2(EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__))))
#define EXPAND2(...) EXPAND1(EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__))))
#define EXPAND1(...) __VA_ARGS__
#define FOR_EACH(macro, ...) __VA_OPT__(EXPAND(FOR_EACH_HELPER(macro, __VA_ARGS__)))
#define FOR_EACH_HELPER(macro, a1, ...) macro(a1) __VA_OPT__(FOR_EACH_AGAIN PARENS(macro, __VA_ARGS__))
#define FOR_EACH_AGAIN() FOR_EACH_HELPER

// reuse
#define FLOAT_TRY_CAST_TO_FIXLEN_REUSE(SOURCE_TYPE, TARGET_TYPE)                                                                                     \
    template <>                                                                                                                                      \
    inline bool FloatTryCastToFixlen::Run(SOURCE_TYPE source, TARGET_TYPE &target) {                                                                 \
        return FloatTryCastToFixlen::Run(static_cast<float>(source), target);                                                                        \
    }
#define FLOAT16_TRY_CAST_TO_FIXLEN_REUSE(TARGET_TYPE) FLOAT_TRY_CAST_TO_FIXLEN_REUSE(Float16T, TARGET_TYPE)
#define BFLOAT16_TRY_CAST_TO_FIXLEN_REUSE(TARGET_TYPE) FLOAT_TRY_CAST_TO_FIXLEN_REUSE(BFloat16T, TARGET_TYPE)

// apply
FOR_EACH(FLOAT16_TRY_CAST_TO_FIXLEN_REUSE, u8, TinyIntT, SmallIntT, IntegerT, BigIntT, HugeIntT, DoubleT, BFloat16T, DecimalT)
FOR_EACH(BFLOAT16_TRY_CAST_TO_FIXLEN_REUSE, u8, TinyIntT, SmallIntT, IntegerT, BigIntT, HugeIntT, DoubleT, Float16T, DecimalT)

template <>
inline bool FloatTryCastToFixlen::Run(Float16T source, FloatT &target) {
    target = static_cast<float>(source);
    return true;
}

template <>
inline bool FloatTryCastToFixlen::Run(BFloat16T source, FloatT &target) {
    target = static_cast<float>(source);
    return true;
}

template <>
inline bool FloatTryCastToVarlen::Run(Float16T source, VarcharT &target, ColumnVector *vector_ptr) {
    return FloatTryCastToVarlen::Run(static_cast<float>(source), target, vector_ptr);
}

template <>
inline bool FloatTryCastToVarlen::Run(BFloat16T source, VarcharT &target, ColumnVector *vector_ptr) {
    return FloatTryCastToVarlen::Run(static_cast<float>(source), target, vector_ptr);
}

} // namespace infinity
