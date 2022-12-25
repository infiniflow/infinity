//
// Created by JinHai on 2022/12/17.
//

#pragma once

#include "bound_cast_func.h"
#include "column_vector_cast.h"
#include "common/types/data_type.h"
#include "common/types/internal_types.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

struct IntegerTryCastToFixlen;
struct IntegerTryCastToVarlen;

template<class SourceType>
static inline BoundCastFunc
BindIntegerCast(const DataType &source, DataType &target) {
    TypeAssert(source.type() != target.type(), "Attempt to cast from " + source.ToString() + " to " + target.ToString());
    switch (target.type()) {
        case LogicalType::kTinyInt: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, TinyIntT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kSmallInt: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, SmallIntT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kInteger: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, IntegerT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kBigInt: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, BigIntT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kHugeInt: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, HugeIntT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kFloat: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, FloatT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kDouble: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, DoubleT, IntegerTryCastToFixlen>);
        }
        case LogicalType::kDecimal16: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, Decimal16T, IntegerTryCastToFixlen>);
        }
        case LogicalType::kDecimal32: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, Decimal32T, IntegerTryCastToFixlen>);
        }
        case LogicalType::kDecimal64: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, Decimal64T, IntegerTryCastToFixlen>);
        }
        case LogicalType::kDecimal128: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVector<SourceType, Decimal128T, IntegerTryCastToFixlen>);
        }
        case LogicalType::kVarchar: {
            return BoundCastFunc(
                    &ColumnVectorCast::TryCastColumnVectorToVarlen<SourceType, VarcharT, IntegerTryCastToVarlen>);
        }
        default: {
            TypeError("BindIntegerCast: Can't cast from " + source.ToString() + " to " + target.ToString());
        }
    }
}

struct IntegerTryCastToFixlen {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(SourceType source, TargetType &target) {
        FunctionError("Not support to cast from " + DataType::TypeToString<SourceType>()
                 + " to " + DataType::TypeToString<TargetType>());
    }
};

struct IntegerTryCastToVarlen {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(SourceType source, TargetType &target, const ColumnVector* vector_ptr){
        FunctionError("Not support to cast from " + DataType::TypeToString<SourceType>()
                      + " to " + DataType::TypeToString<TargetType>());
    }
};

// Cast TinyInt to other numeric type
template<>
inline bool
IntegerTryCastToFixlen::Run(TinyIntT source, SmallIntT &target) {
    target = source;
    return true;
}

template<>
inline bool
IntegerTryCastToFixlen::Run(TinyIntT source, IntegerT &target) {
    target = source;
    return true;
}

template<>
inline bool
IntegerTryCastToFixlen::Run(TinyIntT source, BigIntT &target) {
    target = source;
    return true;
}

template<>
inline bool
IntegerTryCastToFixlen::Run(TinyIntT source, HugeIntT &target) {
    target.lower = source;
    target.upper = (source < 0) * -1;
    return true;
}

template<>
inline bool
IntegerTryCastToFixlen::Run(TinyIntT source, FloatT &target) {
    target = source;
    return true;
}

template<>
inline bool
IntegerTryCastToFixlen::Run(TinyIntT source, DoubleT &target) {
    target = source;
    return true;
}

// TODO
template<>
inline bool
IntegerTryCastToFixlen::Run(TinyIntT source, Decimal16T &target) {
    NotImplementError("Not implemented");
}
template<>
inline bool
IntegerTryCastToFixlen::Run(TinyIntT source, Decimal32T &target) {
    NotImplementError("Not implemented");
}
template<>
inline bool
IntegerTryCastToFixlen::Run(TinyIntT source, Decimal64T &target) {
    NotImplementError("Not implemented");
}
template<>
inline bool
IntegerTryCastToFixlen::Run(TinyIntT source, Decimal128T &target) {
    NotImplementError("Not implemented");
}

// Cast TinyIntT to VarcharT type
template<>
inline bool
IntegerTryCastToVarlen::Run(TinyIntT source, VarcharT &target, const ColumnVector *vector_ptr){
    if(source == 0) {
        target.prefix[0] = '0';
        target.length = 1;
        return true;
    }
    // TODO: High performance itoa needed here.
    String tmp_str = std::to_string(source);
    target.length = static_cast<u16>(tmp_str.size());
    TypeAssert(tmp_str.size() <= VarcharT::INLINE_LENGTH, "Integer digits number should less than 14.");
    memcpy(target.prefix, tmp_str.c_str(), target.length);
    memset(target.prefix + target.length, 0, VarcharT::INLINE_LENGTH - target.length);
    return true;
}

// Cast SmallInt to other numeric type
template<>
inline bool
IntegerTryCastToFixlen::Run(SmallIntT source, TinyIntT &target) {
    if(source < std::numeric_limits<TinyIntT>::min() || source > std::numeric_limits<TinyIntT>::max()) {
        return false;
    }
    target = static_cast<TinyIntT>(source);
    return true;
}

template<>
inline bool
IntegerTryCastToFixlen::Run(SmallIntT source, IntegerT &target) {
    target = source;
    return true;
}

template<>
inline bool
IntegerTryCastToFixlen::Run(SmallIntT source, BigIntT &target) {
    target = source;
    return true;
}

template<>
inline bool
IntegerTryCastToFixlen::Run(SmallIntT source, HugeIntT &target) {
    target.lower = source;
    target.upper = (source < 0) * -1;
    return true;
}

template<>
inline bool
IntegerTryCastToFixlen::Run(SmallIntT source, FloatT &target) {
    target = source;
    return true;
}

template<>
inline bool
IntegerTryCastToFixlen::Run(SmallIntT source, DoubleT &target) {
    target = source;
    return true;
}

// TODO
template<>
inline bool
IntegerTryCastToFixlen::Run(SmallIntT source, Decimal16T &target) {
    NotImplementError("Not implemented");
}
template<>
inline bool
IntegerTryCastToFixlen::Run(SmallIntT source, Decimal32T &target) {
    NotImplementError("Not implemented");
}
template<>
inline bool
IntegerTryCastToFixlen::Run(SmallIntT source, Decimal64T &target) {
    NotImplementError("Not implemented");
}
template<>
inline bool
IntegerTryCastToFixlen::Run(SmallIntT source, Decimal128T &target) {
    NotImplementError("Not implemented");
}

// Cast SmallIntT to VarcharT type
template<>
inline bool
IntegerTryCastToVarlen::Run(SmallIntT source, VarcharT &target, const ColumnVector *vector_ptr){
    if(source == 0) {
        target.prefix[0] = '0';
        target.length = 1;
        return true;
    }
    // TODO: High performance itoa needed here.
    String tmp_str = std::to_string(source);
    target.length = static_cast<u16>(tmp_str.size());
    TypeAssert(tmp_str.size() <= VarcharT::INLINE_LENGTH, "Integer digits number should less than 14.");
    memcpy(target.prefix, tmp_str.c_str(), target.length);
    memset(target.prefix + target.length, 0, VarcharT::INLINE_LENGTH - target.length);
    return true;
}

// Cast Integer to other numeric type
template<>
inline bool
IntegerTryCastToFixlen::Run(IntegerT source, TinyIntT &target) {
    if(source < std::numeric_limits<TinyIntT>::min() || source > std::numeric_limits<TinyIntT>::max()) {
        return false;
    }
    target = static_cast<TinyIntT>(source);
    return true;
}

template<>
inline bool
IntegerTryCastToFixlen::Run(IntegerT source, SmallIntT &target) {
    if(source < std::numeric_limits<SmallIntT>::min() || source > std::numeric_limits<SmallIntT>::max()) {
        return false;
    }
    target = static_cast<SmallIntT>(source);
    return true;
}

template<>
inline bool
IntegerTryCastToFixlen::Run(IntegerT source, BigIntT &target) {
    target = source;
    return true;
}

template<>
inline bool
IntegerTryCastToFixlen::Run(IntegerT source, HugeIntT &target) {
    target.lower = source;
    target.upper = (source < 0) * -1;
    return true;
}

template<>
inline bool
IntegerTryCastToFixlen::Run(IntegerT source, FloatT &target) {
    target = source;
    return true;
}

template<>
inline bool
IntegerTryCastToFixlen::Run(IntegerT source, DoubleT &target) {
    target = source;
    return true;
}

// TODO
template<>
inline bool
IntegerTryCastToFixlen::Run(IntegerT source, Decimal16T &target) {
    NotImplementError("Not implemented");
}
template<>
inline bool
IntegerTryCastToFixlen::Run(IntegerT source, Decimal32T &target) {
    NotImplementError("Not implemented");
}
template<>
inline bool
IntegerTryCastToFixlen::Run(IntegerT source, Decimal64T &target) {
    NotImplementError("Not implemented");
}
template<>
inline bool
IntegerTryCastToFixlen::Run(IntegerT source, Decimal128T &target) {
    NotImplementError("Not implemented");
}

// Cast IntegerT to VarcharT type
template<>
inline bool
IntegerTryCastToVarlen::Run(IntegerT source, VarcharT &target, const ColumnVector *vector_ptr){
    if(source == 0) {
        target.prefix[0] = '0';
        target.length = 1;
        return true;
    }

    // TODO: High performance itoa needed here.
    String tmp_str = std::to_string(source);
    target.length = tmp_str.size();
    TypeAssert(tmp_str.size() <= VarcharT::INLINE_LENGTH, "Integer digits number should less than 14.");
    memcpy(target.prefix, tmp_str.c_str(), target.length);
    memset(target.prefix + target.length, 0, VarcharT::INLINE_LENGTH - target.length);
    return true;
}

// Cast BigInt to other numeric type
template<>
inline bool
IntegerTryCastToFixlen::Run(BigIntT source, TinyIntT &target) {
    if(source < std::numeric_limits<TinyIntT>::min() || source > std::numeric_limits<TinyIntT>::max()) {
        return false;
    }
    target = static_cast<TinyIntT>(source);
    return true;
}

template<>
inline bool
IntegerTryCastToFixlen::Run(BigIntT source, SmallIntT &target) {
    if(source < std::numeric_limits<SmallIntT>::min() || source > std::numeric_limits<SmallIntT>::max()) {
        return false;
    }
    target = static_cast<SmallIntT>(source);
    return true;
}

template<>
inline bool
IntegerTryCastToFixlen::Run(BigIntT source, IntegerT &target) {
    if(source < std::numeric_limits<IntegerT>::min() || source > std::numeric_limits<IntegerT>::max()) {
        return false;
    }
    target = static_cast<IntegerT>(source);
    return true;
}

template<>
inline bool
IntegerTryCastToFixlen::Run(BigIntT source, HugeIntT &target) {
    target.lower = source;
    target.upper = (source < 0) * -1;
    return true;
}

template<>
inline bool
IntegerTryCastToFixlen::Run(BigIntT source, FloatT &target) {
    target = source;
    return true;
}

template<>
inline bool
IntegerTryCastToFixlen::Run(BigIntT source, DoubleT &target) {
    target = source;
    return true;
}

// TODO
template<>
inline bool
IntegerTryCastToFixlen::Run(BigIntT source, Decimal16T &target) {
    NotImplementError("Not implemented");
}
template<>
inline bool
IntegerTryCastToFixlen::Run(BigIntT source, Decimal32T &target) {
    NotImplementError("Not implemented");
}
template<>
inline bool
IntegerTryCastToFixlen::Run(BigIntT source, Decimal64T &target) {
    NotImplementError("Not implemented");
}
template<>
inline bool
IntegerTryCastToFixlen::Run(BigIntT source, Decimal128T &target) {
    NotImplementError("Not implemented");
}

// Cast integer to varlen type
template<>
inline bool
IntegerTryCastToVarlen::Run(BigIntT source, VarcharT &target, const ColumnVector *vector_ptr){
    if(source == 0) {
        target.prefix[0] = '0';
        target.length = 1;
        return true;
    }

    // TODO: High performance itoa needed here.
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

// TODO: Cast HugeInt to other numeric type
template<>
inline bool
IntegerTryCastToFixlen::Run(HugeIntT source, TinyIntT &target) {
    NotImplementError("Not implemented");
}

template<>
inline bool
IntegerTryCastToFixlen::Run(HugeIntT source, SmallIntT &target) {
    NotImplementError("Not implemented");
}

template<>
inline bool
IntegerTryCastToFixlen::Run(HugeIntT source, IntegerT &target) {
    NotImplementError("Not implemented");
}

template<>
inline bool
IntegerTryCastToFixlen::Run(HugeIntT source, BigIntT &target) {
    NotImplementError("Not implemented");
}

template<>
inline bool
IntegerTryCastToFixlen::Run(HugeIntT source, FloatT &target) {
    NotImplementError("Not implemented");
}

template<>
inline bool
IntegerTryCastToFixlen::Run(HugeIntT source, DoubleT &target) {
    NotImplementError("Not implemented");
}

// TODO
template<>
inline bool
IntegerTryCastToFixlen::Run(HugeIntT source, Decimal16T &target) {
    NotImplementError("Not implemented");
}
template<>
inline bool
IntegerTryCastToFixlen::Run(HugeIntT source, Decimal32T &target) {
    NotImplementError("Not implemented");
}
template<>
inline bool
IntegerTryCastToFixlen::Run(HugeIntT source, Decimal64T &target) {
    NotImplementError("Not implemented");
}
template<>
inline bool
IntegerTryCastToFixlen::Run(HugeIntT source, Decimal128T &target) {
    NotImplementError("Not implemented");
}

// Cast integer to varlen type
template<>
inline bool
IntegerTryCastToVarlen::Run(HugeIntT source, VarcharT &target, const ColumnVector *vector_ptr){
    NotImplementError("Not implemented");
}

}
