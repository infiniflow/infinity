//
// Created by jinhai on 22-12-21.
//

#pragma once

#include "bound_cast_func.h"
#include "column_vector_cast.h"
#include "common/types/data_type.h"
#include "common/types/internal_types.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

struct FloatTryCastToFixlen;
struct FloatTryCastToVarlen;

template<class SourceType>
static inline BoundCastFunc
BindFloatCast(const DataType &source, DataType &target) {
    TypeAssert(source.type() != target.type(),
               "Attempt to cast from " + source.ToString() + " to " + target.ToString());
    switch (target.type()) {
        case LogicalType::kTinyInt: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, TinyIntT, FloatTryCastToFixlen>);
        }
        case LogicalType::kSmallInt: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, SmallIntT, FloatTryCastToFixlen>);
        }
        case LogicalType::kInteger: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, IntegerT, FloatTryCastToFixlen>);
        }
        case LogicalType::kBigInt: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, BigIntT, FloatTryCastToFixlen>);
        }
        case LogicalType::kHugeInt: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, HugeIntT, FloatTryCastToFixlen>);
        }
        case LogicalType::kFloat: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, FloatT, FloatTryCastToFixlen>);
        }
        case LogicalType::kDouble: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, DoubleT, FloatTryCastToFixlen>);
        }
        case LogicalType::kDecimal16: {
            NotImplementError("Not implement cast from numeric to decimal16 type.");
        }
        case LogicalType::kDecimal32: {
            NotImplementError("Not implement cast from numeric to decimal32 type.");
        }
        case LogicalType::kDecimal64: {
            NotImplementError("Not implement cast from numeric to decimal64 type.");
        }
        case LogicalType::kVarchar: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVectorToVarlen<SourceType, VarcharT, FloatTryCastToVarlen>);
        }
        case LogicalType::kChar1: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, Char1T, FloatTryCastToFixlen>);
        }
        case LogicalType::kChar2: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, Char2T, FloatTryCastToFixlen>);
        }
        case LogicalType::kChar4: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, Char4T, FloatTryCastToFixlen>);
        }
        case LogicalType::kChar8: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, Char8T, FloatTryCastToFixlen>);
        }
        case LogicalType::kChar16: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, Char16T, FloatTryCastToFixlen>);
        }
        case LogicalType::kChar32: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, Char32T, FloatTryCastToFixlen>);
        }
        case LogicalType::kChar64: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, Char64T, FloatTryCastToFixlen>);
        }
        default: {
            TypeError("BindIntegerCast: Can't cast from " + source.ToString() + " to " + target.ToString());
        }
    }
}

struct FloatTryCastToFixlen {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(SourceType source, TargetType &target) {
        FunctionError("No implemention to cast from " + DataType::TypeToString<SourceType>()
                      + " to " + DataType::TypeToString<TargetType>());
    }
};

struct FloatTryCastToVarlen {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(SourceType source, TargetType &target, const ColumnVector *vector_ptr) {
        FunctionError("Not implemention to cast from " + DataType::TypeToString<SourceType>()
                      + " to " + DataType::TypeToString<TargetType>());
    }
};

// Cast FloatT to other numeric type
template<>
inline bool
FloatTryCastToFixlen::Run(FloatT source, TinyIntT &target) {
    if(source < -128.0f || source > 127.0f) {
        return false;
    }
    target = static_cast<TinyIntT>(source);
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(FloatT source, SmallIntT &target) {
    if(source < -32768.0f || source > 32767.0f) {
        return false;
    }
    target = static_cast<SmallIntT>(source);
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(FloatT source, IntegerT &target) {
    if(source < -2147483648.0f || source > 2147483647.0f) {
        return false;
    }
    target = std::nearbyint(source);
    if(source > 0 && target < 0) {
        return false;
    }
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(FloatT source, BigIntT &target) {
    if(source < -9223372036854775808.0f || source > 9223372036854775807.0f) {
        return false;
    }
    target = std::nearbyint(source);
    return true;
}

// TODO: Cast from float to hugeint
//template<>
//inline bool
//FloatTryCastToFixlen::Run(FloatT source, HugeIntT &target) {
//    target.lower = source;
//    target.upper = (source < 0) * -1;
//    return true;
//}

template<>
inline bool
FloatTryCastToFixlen::Run(FloatT source, DoubleT &target) {
    target = source;
    return true;
}

// TODO
//template<>
//bool FloatTryCastToFixlen::Run(TinyIntT source, Decimal16T &target);
//template<>
//bool FloatTryCastToFixlen::Run(TinyIntT source, Decimal32T &target);
//template<>
//bool FloatTryCastToFixlen::Run(TinyIntT source, Decimal64T &target);
//template<>
//bool FloatTryCastToFixlen::Run(TinyIntT source, Decimal128T &target);

// Cast Float to Char type
// Float can't be casted to Char1, Char2 and Char4.

template<>
inline bool
FloatTryCastToFixlen::Run(FloatT source, Char8T &target) {
    // TODO: High-performance to_string implementation is needed.
    String res = std::to_string(source);

    size_t result_size = res.size();
    if(result_size > Char8T::CHAR_LENGTH) {
        return false;
    }
    memcpy(target.value, res.c_str(), result_size);
    memset(target.value + result_size, 0, Char8T::CHAR_LENGTH - result_size);
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(FloatT source, Char16T &target) {
    // TODO: High-performance to_string implementation is needed.
    String res = std::to_string(source);

    size_t result_size = res.size();
    if(result_size > Char16T::CHAR_LENGTH) {
        return false;
    }
    memcpy(target.value, res.c_str(), result_size);
    memset(target.value + result_size, 0, Char16T::CHAR_LENGTH - result_size);
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(FloatT source, Char32T &target) {
    // TODO: High-performance to_string implementation is needed.
    String res = std::to_string(source);

    size_t result_size = res.size();
    if(result_size > Char32T::CHAR_LENGTH) {
        return false;
    }
    memcpy(target.value, res.c_str(), result_size);
    memset(target.value + result_size, 0, Char32T::CHAR_LENGTH - result_size);
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(FloatT source, Char64T &target) {
    // TODO: High-performance to_string implementation is needed.
    String res = std::to_string(source);

    size_t result_size = res.size();
    if(result_size > Char64T::CHAR_LENGTH) {
        return false;
    }
    memcpy(target.value, res.c_str(), result_size);
    memset(target.value + result_size, 0, Char64T::CHAR_LENGTH - result_size);
    return true;
}

// Cast FloatT to varlen type
template<>
inline bool
FloatTryCastToVarlen::Run(FloatT source, VarcharT &target, const ColumnVector *vector_ptr){
    // TODO: High-performance to_string implementation is needed.
    String tmp_str = std::to_string(source);
    target.length = static_cast<i16>(tmp_str.size());
    if(target.length <= VarcharT::INLINE_LENGTH) {
        memcpy(target.prefix, tmp_str.c_str(), target.length);
        memset(target.prefix + target.length, 0, VarcharT::INLINE_LENGTH - target.length);
    } else {
        memcpy(target.prefix, tmp_str.c_str(), VarcharT::PREFIX_LENGTH);
        TypeAssert(vector_ptr->buffer_->buffer_type_ == VectorBufferType::kMemory,
                   "Varchar column vector should use MemoryVectorBuffer. ");

        auto* string_vector_buffer_ptr = (MemoryVectorBuffer*)(vector_ptr->buffer_.get());

        ptr_t ptr = string_vector_buffer_ptr->chunk_mgr_->Allocate(target.length);
        memcpy(ptr, tmp_str.c_str(), target.length);
        target.ptr = ptr;
    }

    return true;
}


// Cast DoubleT to other numeric type
template<>
inline bool
FloatTryCastToFixlen::Run(DoubleT source, TinyIntT &target) {
    if(source < std::numeric_limits<TinyIntT>::min() || source > std::numeric_limits<TinyIntT>::max()) {
        return false;
    }
    target = static_cast<TinyIntT>(source);
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(DoubleT source, SmallIntT &target) {
    if(source < std::numeric_limits<SmallIntT>::min() || source > std::numeric_limits<SmallIntT>::max()) {
        return false;
    }
    target = static_cast<SmallIntT>(source);
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(DoubleT source, IntegerT &target) {
    if(source < std::numeric_limits<IntegerT>::min() || source > std::numeric_limits<IntegerT>::max()) {
        return false;
    }
    target = static_cast<IntegerT>(source);
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(DoubleT source, BigIntT &target) {
    if(source < std::numeric_limits<BigIntT>::min() || source > std::numeric_limits<BigIntT>::max()) {
        return false;
    }
    target = static_cast<BigIntT>(source);
    return true;
}

// TODO: Cast from double to hugeint
//template<>
//inline bool
//FloatTryCastToFixlen::Run(FloatT source, HugeIntT &target) {
//    target.lower = source;
//    target.upper = (source < 0) * -1;
//    return true;
//}

template<>
inline bool
FloatTryCastToFixlen::Run(DoubleT source, DoubleT &target) {
    target = source;
    return true;
}

// TODO
//template<>
//bool FloatTryCastToFixlen::Run(TinyIntT source, Decimal16T &target);
//template<>
//bool FloatTryCastToFixlen::Run(TinyIntT source, Decimal32T &target);
//template<>
//bool FloatTryCastToFixlen::Run(TinyIntT source, Decimal64T &target);
//template<>
//bool FloatTryCastToFixlen::Run(TinyIntT source, Decimal128T &target);

// Cast Float to Char type
// Float can't be casted to Char1 and Char2.

template<>
inline bool
FloatTryCastToFixlen::Run(DoubleT source, Char4T &target) {
    // TODO: High-performance to_string implementation is needed.
    String res = std::to_string(source);

    size_t result_size = res.size();
    if(result_size > Char4T::CHAR_LENGTH) {
        return false;
    }
    memcpy(target.value, res.c_str(), result_size);
    memset(target.value + result_size, 0, Char4T::CHAR_LENGTH - result_size);
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(DoubleT source, Char8T &target) {
    // TODO: High-performance to_string implementation is needed.
    String res = std::to_string(source);

    size_t result_size = res.size();
    if(result_size > Char8T::CHAR_LENGTH) {
        return false;
    }
    memcpy(target.value, res.c_str(), result_size);
    memset(target.value + result_size, 0, Char8T::CHAR_LENGTH - result_size);
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(DoubleT source, Char16T &target) {
    // TODO: High-performance to_string implementation is needed.
    String res = std::to_string(source);

    size_t result_size = res.size();
    if(result_size > Char16T::CHAR_LENGTH) {
        return false;
    }
    memcpy(target.value, res.c_str(), result_size);
    memset(target.value + result_size, 0, Char16T::CHAR_LENGTH - result_size);
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(DoubleT source, Char32T &target) {
    // TODO: High-performance to_string implementation is needed.
    String res = std::to_string(source);

    size_t result_size = res.size();
    if(result_size > Char32T::CHAR_LENGTH) {
        return false;
    }
    memcpy(target.value, res.c_str(), result_size);
    memset(target.value + result_size, 0, Char32T::CHAR_LENGTH - result_size);
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(DoubleT source, Char64T &target) {
    // TODO: High-performance to_string implementation is needed.
    String res = std::to_string(source);

    size_t result_size = res.size();
    if(result_size > Char64T::CHAR_LENGTH) {
        return false;
    }
    memcpy(target.value, res.c_str(), result_size);
    memset(target.value + result_size, 0, Char64T::CHAR_LENGTH - result_size);
    return true;
}

// Cast double to varlen type
template<>
inline bool
FloatTryCastToVarlen::Run(DoubleT source, VarcharT &target, const ColumnVector *vector_ptr){
    // TODO: High-performance to_string implementation is needed.
    String tmp_str = std::to_string(source);
    target.length = static_cast<i16>(tmp_str.size());
    if(target.length <= VarcharT::INLINE_LENGTH) {
        memcpy(target.prefix, tmp_str.c_str(), target.length);
        memset(target.prefix + target.length, 0, VarcharT::INLINE_LENGTH - target.length);
    } else {
        memcpy(target.prefix, tmp_str.c_str(), VarcharT::PREFIX_LENGTH);
        TypeAssert(vector_ptr->buffer_->buffer_type_ == VectorBufferType::kMemory,
                   "Varchar column vector should use MemoryVectorBuffer. ");

        auto* string_vector_buffer_ptr = (MemoryVectorBuffer*)(vector_ptr->buffer_.get());

        ptr_t ptr = string_vector_buffer_ptr->chunk_mgr_->Allocate(target.length);
        memcpy(ptr, tmp_str.c_str(), target.length);
        target.ptr = ptr;
    }

    return true;
}

}
