//
// Created by jinhai on 22-12-21.
//

#pragma once

#include "bound_cast_func.h"
#include "column_vector_cast.h"
#include "common/types/data_type.h"

#include "common/utility/infinity_assert.h"

namespace infinity {

struct FloatTryCastToFixlen;
struct FloatTryCastToVarlen;

template<class SourceType>
static inline BoundCastFunc
BindFloatCast(const DataType& source, const DataType& target) {
    TypeAssert(source.type() != target.type(),
               "Attempt to cast from " + source.ToString() + " to " + target.ToString());
    switch(target.type()) {
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
        case LogicalType::kDecimal: {
            NotImplementError("Not implement cast from numeric to decimal128 type.");
        }
        case LogicalType::kVarchar: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVectorToVarlen<SourceType, VarcharT, FloatTryCastToVarlen>);
        }
        default: {
            TypeError("BindFloatCast: Can't cast from " + source.ToString() + " to " + target.ToString());
        }
    }
}

struct FloatTryCastToFixlen {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(SourceType source, TargetType& target) {
        FunctionError("No implemention to cast from " + DataType::TypeToString<SourceType>()
                      + " to " + DataType::TypeToString<TargetType>());
    }
};

struct FloatTryCastToVarlen {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(SourceType source, TargetType& target, const SharedPtr<ColumnVector>& vector_ptr) {
        FunctionError("Not implemention to cast from " + DataType::TypeToString<SourceType>()
                      + " to " + DataType::TypeToString<TargetType>());
    }
};

// Cast FloatT to other numeric type
template<>
inline bool
FloatTryCastToFixlen::Run(FloatT source, TinyIntT& target) {
    if(source < -128.0f || source > 127.0f) {
        return false;
    }
    target = static_cast<TinyIntT>(source);
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(FloatT source, SmallIntT& target) {
    if(source < -32768.0f || source > 32767.0f) {
        return false;
    }
    target = static_cast<SmallIntT>(source);
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(FloatT source, IntegerT& target) {
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
FloatTryCastToFixlen::Run(FloatT source, BigIntT& target) {
    if(source < -9223372036854775808.0f || source > 9223372036854775807.0f) {
        return false;
    }
    target = std::nearbyint(source);
    return true;
}

// TODO: Cast from float to hugeint
template<>
inline bool
FloatTryCastToFixlen::Run(FloatT source, HugeIntT& target) {
    NotImplementError("Not implemented");
}

template<>
inline bool
FloatTryCastToFixlen::Run(FloatT source, DoubleT& target) {
    target = source;
    return true;
}

// TODO
template<>
inline bool
FloatTryCastToFixlen::Run(FloatT source, DecimalT& target) {
    NotImplementError("Not implemented");
}

// Cast FloatT to varlen type
template<>
inline bool
FloatTryCastToVarlen::Run(FloatT source, VarcharT& target, const SharedPtr<ColumnVector>& vector_ptr) {
    // TODO: High-performance to_string implementation is needed.
    String tmp_str = std::to_string(source);
    target.length = static_cast<u16>(tmp_str.size());
    if(target.length <= VarcharT::INLINE_LENGTH) {
        memcpy(target.prefix, tmp_str.c_str(), target.length);
        memset(target.prefix + target.length, 0, VarcharT::INLINE_LENGTH - target.length);
    } else {
        memcpy(target.prefix, tmp_str.c_str(), VarcharT::PREFIX_LENGTH);
        TypeAssert(vector_ptr->buffer_->buffer_type_ == VectorBufferType::kHeap,
                   "Varchar column vector should use MemoryVectorBuffer. ");

        ptr_t ptr = vector_ptr->buffer_->heap_mgr_->Allocate(target.length);
        memcpy(ptr, tmp_str.c_str(), target.length);
        target.ptr = ptr;
    }

    return true;
}

// Cast DoubleT to other numeric type
template<>
inline bool
FloatTryCastToFixlen::Run(DoubleT source, TinyIntT& target) {
    if(source < -128.0f || source > 127.0f) {
        return false;
    }
    target = static_cast<TinyIntT>(source);
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(DoubleT source, SmallIntT& target) {
    if(source < -32768.0f || source > 32767.0f) {
        return false;
    }
    target = static_cast<SmallIntT>(source);
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(DoubleT source, IntegerT& target) {
    if(source < -2147483648.0f || source > 2147483647.0f) {
        return false;
    }
    target = std::nearbyint(source);
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(DoubleT source, BigIntT& target) {
    if(source < -9223372036854775808.0f || source > 9223372036854775807.0f) {
        return false;
    }
    target = std::nearbyint(source);
    if(source > 0 && target < 0) {
        return false;
    }
    return true;
}

// TODO: Cast from double to hugeint
template<>
inline bool
FloatTryCastToFixlen::Run(DoubleT source, HugeIntT& target) {
    NotImplementError("Not implemented");
}

template<>
inline bool
FloatTryCastToFixlen::Run(DoubleT source, FloatT& target) {
    target = source;
    return true;
}

// TODO
template<>
inline bool
FloatTryCastToFixlen::Run(DoubleT source, DecimalT& target) {
    NotImplementError("Not implemented");
}

// Cast double to varlen type
template<>
inline bool
FloatTryCastToVarlen::Run(DoubleT source, VarcharT& target, const SharedPtr<ColumnVector>& vector_ptr) {
    // TODO: High-performance to_string implementation is needed.
    String tmp_str = std::to_string(source);
    target.length = static_cast<u16>(tmp_str.size());
    if(target.length <= VarcharT::INLINE_LENGTH) {
        memcpy(target.prefix, tmp_str.c_str(), target.length);
        memset(target.prefix + target.length, 0, VarcharT::INLINE_LENGTH - target.length);
    } else {
        memcpy(target.prefix, tmp_str.c_str(), VarcharT::PREFIX_LENGTH);
        TypeAssert(vector_ptr->buffer_->buffer_type_ == VectorBufferType::kHeap,
                   "Varchar column vector should use MemoryVectorBuffer. ");

        ptr_t ptr = vector_ptr->buffer_->heap_mgr_->Allocate(target.length);
        memcpy(ptr, tmp_str.c_str(), target.length);
        target.ptr = ptr;
    }

    return true;
}

}
