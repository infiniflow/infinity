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
    if(source < std::numeric_limits<TinyIntT>::min() || source > std::numeric_limits<TinyIntT>::max()) {
        return false;
    }
    target = static_cast<TinyIntT>(source);
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(FloatT source, SmallIntT &target) {
    if(source < std::numeric_limits<SmallIntT>::min() || source > std::numeric_limits<SmallIntT>::max()) {
        return false;
    }
    target = static_cast<SmallIntT>(source);
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(FloatT source, IntegerT &target) {
    if(source < std::numeric_limits<IntegerT>::min() || source > std::numeric_limits<IntegerT>::max()) {
        return false;
    }
    target = static_cast<IntegerT>(source);
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(FloatT source, BigIntT &target) {
    if(source < std::numeric_limits<BigIntT>::min() || source > std::numeric_limits<BigIntT>::max()) {
        return false;
    }
    target = static_cast<BigIntT>(source);
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

// Cast SmallInt to Char type
template<>
inline bool
FloatTryCastToFixlen::Run(FloatT source, Char1T &target) {
    if(source < 0 or source >= 10) return false;
    target.value =  '0' + source;
    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(BigIntT source, Char2T &target) {
    if(source > -10 && source < 100) {
        if(source < 0) {
            target.value[0] = '-';
            target.value[1] = '0' - source;
        } else if(source > 0 && source < 10) {
            target.value[0] = '0' + source;
            target.value[1] = 0;
        } else if(source >= 10) {
            const i8 tens = source / 10;
            target.value[0] = '0' + tens;
            target.value[1] = '0' + source - tens * 10;
        } else {
            target.value[0] = '0';
        }
        return true;
    } else {
        return false;
    }
}

template<>
inline bool
FloatTryCastToFixlen::Run(BigIntT source, Char4T &target) {
    if(source < -999 || source > 9999) return false;
    if(source == 0) {
        target.value[0] = '0';
        return true;
    }
    size_t idx = 0;
    i64 src = source;
    if(source < 0) {
        target.value[idx ++] = '-';
        src = -src;
    }

    char_t tmp[Char4T::CHAR_LENGTH];
    i64 tmp_idx = 0;
    while(src > 0) {
        tmp[tmp_idx ++] = '0' + src % 10;
        src /= 10;
    }
    while(idx < Char4T::CHAR_LENGTH) {
        -- tmp_idx;
        target.value[idx ++] = tmp_idx >= 0 ? tmp[tmp_idx] : 0;
    }

    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(BigIntT source, Char8T &target) {
    if(source < -9999999 || source > 99999999) return false;
    if(source == 0) {
        target.value[0] = '0';
        return true;
    }
    size_t idx = 0;
    i64 src = source;
    if(source < 0) {
        target.value[idx ++] = '-';
        src = -src;
    }

    char_t tmp[Char8T::CHAR_LENGTH];
    i64 tmp_idx = 0;
    while(src > 0) {
        tmp[tmp_idx ++] = '0' + src % 10;
        src /= 10;
    }
    while(idx < Char8T::CHAR_LENGTH) {
        -- tmp_idx;
        target.value[idx ++] = tmp_idx >= 0 ? tmp[tmp_idx] : 0;
    }

    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(BigIntT source, Char16T &target) {
    if(source < -999999999999999LL || source > 9999999999999999LL ) return false;
    if(source == 0) {
        target.value[0] = '0';
        return true;
    }
    size_t idx = 0;
    i64 src = source;
    if(source < 0) {
        target.value[idx ++] = '-';
        src = -src;
    }

    char_t tmp[Char16T::CHAR_LENGTH];
    i64 tmp_idx = 0;
    while(src > 0) {
        tmp[tmp_idx ++] = '0' + src % 10;
        src /= 10;
    }
    while(idx < Char16T::CHAR_LENGTH) {
        -- tmp_idx;
        target.value[idx ++] = tmp_idx >= 0 ? tmp[tmp_idx] : 0;
    }

    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(BigIntT source, Char32T &target) {

    if(source == 0) {
        target.value[0] = '0';
        return true;
    }

    // TODO: High performance itoa needed here.
    String str = std::to_string(source);
    size_t str_len = str.size();
    TypeAssert(str_len < Char32T::CHAR_LENGTH, "BigInt number digits is more than 32");
    memcpy(target.value, str.c_str(), str_len);
    memset(target.value + str_len, 0, Char32T::CHAR_LENGTH - str_len);

    return true;
}

template<>
inline bool
FloatTryCastToFixlen::Run(BigIntT source, Char64T &target) {

    if(source == 0) {
        target.value[0] = '0';
        return true;
    }

    // TODO: High performance itoa needed here.
    String str = std::to_string(source);
    size_t str_len = str.size();
    TypeAssert(str_len < Char32T::CHAR_LENGTH, "BigInt number digits is more than 32");
    memcpy(target.value, str.c_str(), str_len);
    memset(target.value + str_len, 0, Char32T::CHAR_LENGTH - str_len);

    return true;
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
