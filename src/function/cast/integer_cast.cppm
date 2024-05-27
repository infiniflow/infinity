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

export module integer_cast;


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

export struct IntegerTryCastToFixlen;
export struct IntegerTryCastToVarlen;

export template <class SourceType>
inline BoundCastFunc BindIntegerCast(const DataType &source, const DataType &target) {
    if (source.type() == target.type()) {
        String error_message = "Can't cast from the same type";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
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
            RecoverableError(Status::NotSupport(fmt::format("Attempt to cast from {} to {}", source.ToString(), target.ToString())));
        }
    }
    return BoundCastFunc(nullptr);
}

struct IntegerTryCastToFixlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType, TargetType &) {
        String error_message = fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>());
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
        return false;
    }
};

struct IntegerTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType, TargetType &, ColumnVector*) {
        String error_message = fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>());
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
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
    String error_message = "Not implemented";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

// Cast TinyIntT to VarcharT type
template <>
inline bool IntegerTryCastToVarlen::Run(TinyIntT source, VarcharT &target, ColumnVector*) {
    target.is_value_ = false;
    if (source == 0) {
        target.short_.data_[0] = '0';
        target.length_ = 1;
        return true;
    }
    // TODO: High performance itoa needed here.
    String tmp_str = std::to_string(source);
    target.length_ = static_cast<u32>(tmp_str.size());
    if (target.length_ > VARCHAR_INLINE_LEN) {
        String error_message = "Integer digits number should less than 14.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    std::memcpy(target.short_.data_, tmp_str.c_str(), target.length_);
    return true;
}

// Cast SmallInt to other numeric type
template <>
inline bool IntegerTryCastToFixlen::Run(SmallIntT source, TinyIntT &target) {
    if (source < std::numeric_limits<i8>::min() || source > std::numeric_limits<i8>::max()) {
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
    UnrecoverableError("Not implement: IntegerTryCastToFixlen::Run");
    return false;
}

// Cast SmallIntT to VarcharT type
template <>
inline bool IntegerTryCastToVarlen::Run(SmallIntT source, VarcharT &target, ColumnVector*) {
    target.is_value_ = false;
    if (source == 0) {
        target.short_.data_[0] = '0';
        target.length_ = 1;
        return true;
    }
    // TODO: High performance itoa needed here.
    String tmp_str = std::to_string(source);
    target.length_ = static_cast<u32>(tmp_str.size());
    if (target.length_ > VARCHAR_INLINE_LEN) {
        String error_message = "Integer digits number should less than 14.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    std::memcpy(target.short_.data_, tmp_str.c_str(), target.length_);
    return true;
}

// Cast Integer to other numeric type
template <>
inline bool IntegerTryCastToFixlen::Run(IntegerT source, TinyIntT &target) {
    if (source < std::numeric_limits<i8>::min() || source > std::numeric_limits<i8>::max()) {
        return false;
    }
    target = static_cast<TinyIntT>(source);
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(IntegerT source, SmallIntT &target) {
    if (source < std::numeric_limits<i16>::min() || source > std::numeric_limits<i16>::max()) {
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
    UnrecoverableError("Not implement: IntegerTryCastToFixlen::Run");
    return false;
}

// Cast IntegerT to VarcharT type
template <>
inline bool IntegerTryCastToVarlen::Run(IntegerT source, VarcharT &target, ColumnVector*) {
    target.is_value_ = false;
    if (source == 0) {
        target.short_.data_[0] = '0';
        target.length_ = 1;
        return true;
    }
    // TODO: High performance itoa needed here.
    String tmp_str = std::to_string(source);
    target.length_ = static_cast<u32>(tmp_str.size());
    if (target.length_ > VARCHAR_INLINE_LEN) {
        String error_message = "Integer digits number should less than 14.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    std::memcpy(target.short_.data_, tmp_str.c_str(), target.length_);
    return true;
}

// Cast BigInt to other numeric type
template <>
inline bool IntegerTryCastToFixlen::Run(BigIntT source, TinyIntT &target) {
    if (source < std::numeric_limits<i8>::min() || source > std::numeric_limits<i8>::max()) {
        return false;
    }
    target = static_cast<TinyIntT>(source);
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(BigIntT source, SmallIntT &target) {
    if (source < std::numeric_limits<i16>::min() || source > std::numeric_limits<i16>::max()) {
        return false;
    }
    target = static_cast<SmallIntT>(source);
    return true;
}

template <>
inline bool IntegerTryCastToFixlen::Run(BigIntT source, IntegerT &target) {
    if (source < std::numeric_limits<i32>::min() || source > std::numeric_limits<i32>::max()) {
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
    UnrecoverableError("Not implement: IntegerTryCastToFixlen::Run");
    return false;
}

// Cast integer to varlen type
template <>
inline bool IntegerTryCastToVarlen::Run(BigIntT source, VarcharT &target, ColumnVector* vector_ptr) {
    target.is_value_ = false;
    if (source == 0) {
        target.short_.data_[0] = '0';
        target.length_ = 1;
        return true;
    }
    // TODO: High performance itoa needed here.
    String tmp_str = std::to_string(source);
    target.length_ = static_cast<u32>(tmp_str.size());
    if (target.length_ <= VARCHAR_INLINE_LEN) {
        std::memcpy(target.short_.data_, tmp_str.c_str(), target.length_);
    } else {
        std::memcpy(target.vector_.prefix_, tmp_str.c_str(), VARCHAR_PREFIX_LEN);
        if (vector_ptr->buffer_->buffer_type_ != VectorBufferType::kHeap) {
            UnrecoverableError("Varchar column vector should use MemoryVectorBuffer. ");
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
    UnrecoverableError("Not implement: IntegerTryCastToFixlen::Run");
    return false;
}

template <>
inline bool IntegerTryCastToFixlen::Run(HugeIntT, SmallIntT &) {
    UnrecoverableError("Not implement: IntegerTryCastToFixlen::Run");
    return false;
}

template <>
inline bool IntegerTryCastToFixlen::Run(HugeIntT, IntegerT &) {
    UnrecoverableError("Not implement: IntegerTryCastToFixlen::Run");
    return false;
}

template <>
inline bool IntegerTryCastToFixlen::Run(HugeIntT, BigIntT &) {
    UnrecoverableError("Not implement: IntegerTryCastToFixlen::Run");
    return false;
}

template <>
inline bool IntegerTryCastToFixlen::Run(HugeIntT, FloatT &) {
    UnrecoverableError("Not implement: IntegerTryCastToFixlen::Run");
    return false;
}

template <>
inline bool IntegerTryCastToFixlen::Run(HugeIntT, DoubleT &) {
    UnrecoverableError("Not implemented");
    return false;
}

// TODO
template <>
inline bool IntegerTryCastToFixlen::Run(HugeIntT, DecimalT &) {
    UnrecoverableError("Not implemented");
    return false;
}

// Cast integer to varlen type
template <>
inline bool IntegerTryCastToVarlen::Run(HugeIntT, VarcharT &, ColumnVector*) {
    UnrecoverableError("Not implemented");
    return false;
}

} // namespace infinity
