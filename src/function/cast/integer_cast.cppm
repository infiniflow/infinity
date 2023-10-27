//
// Created by JinHai on 2022/12/17.
//

module;

import parser;
import stl;
import bound_cast_func;
import column_vector_cast;
import infinity_assert;
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
    Assert<TypeException>(source.type() != target.type(),
                          Format("Attempt to cast from {} to {}", source.ToString(), target.ToString()),
                          __FILE_NAME__,
                          __LINE__);
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
            Error<TypeException>(Format("Attempt to cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
    }
}

struct IntegerTryCastToFixlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType source, TargetType &target) {
        Error<FunctionException>(
            Format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()),
            __FILE_NAME__,
            __LINE__);
    }
};

struct IntegerTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType source, TargetType &target, const SharedPtr<ColumnVector> &vector_ptr) {
        Error<FunctionException>(
            Format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()),
            __FILE_NAME__,
            __LINE__);
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
inline bool IntegerTryCastToFixlen::Run(TinyIntT source, DecimalT &target) {
    Error<NotImplementException>(Format("Not implemented"), __FILE_NAME__, __LINE__);
}

// Cast TinyIntT to VarcharT type
template <>
inline bool IntegerTryCastToVarlen::Run(TinyIntT source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    if (source == 0) {
        target.prefix[0] = '0';
        target.length = 1;
        return true;
    }
    // TODO: High performance itoa needed here.
    String tmp_str = ToStr(source);
    target.length = static_cast<u16>(tmp_str.size());
    Assert<TypeException>(tmp_str.size() <= VarcharT::INLINE_LENGTH, "Integer digits number should less than 14.", __FILE_NAME__, __LINE__);
    Memcpy(target.prefix, tmp_str.c_str(), target.length);
    Memset(target.prefix + target.length, 0, VarcharT::INLINE_LENGTH - target.length);
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
inline bool IntegerTryCastToFixlen::Run(SmallIntT source, DecimalT &target) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

// Cast SmallIntT to VarcharT type
template <>
inline bool IntegerTryCastToVarlen::Run(SmallIntT source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    if (source == 0) {
        target.prefix[0] = '0';
        target.length = 1;
        return true;
    }
    // TODO: High performance itoa needed here.
    String tmp_str = ToStr(source);
    target.length = static_cast<u16>(tmp_str.size());
    Assert<TypeException>(tmp_str.size() <= VarcharT::INLINE_LENGTH, "Integer digits number should less than 14.", __FILE_NAME__, __LINE__);
    Memcpy(target.prefix, tmp_str.c_str(), target.length);
    Memset(target.prefix + target.length, 0, VarcharT::INLINE_LENGTH - target.length);
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
inline bool IntegerTryCastToFixlen::Run(IntegerT source, DecimalT &target) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

// Cast IntegerT to VarcharT type
template <>
inline bool IntegerTryCastToVarlen::Run(IntegerT source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    if (source == 0) {
        target.prefix[0] = '0';
        target.length = 1;
        return true;
    }

    // TODO: High performance itoa needed here.
    String tmp_str = ToStr(source);
    target.length = tmp_str.size();
    Assert<TypeException>(tmp_str.size() <= VarcharT::INLINE_LENGTH, "Integer digits number should less than 14.", __FILE_NAME__, __LINE__);
    Memcpy(target.prefix, tmp_str.c_str(), target.length);
    Memset(target.prefix + target.length, 0, VarcharT::INLINE_LENGTH - target.length);
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
inline bool IntegerTryCastToFixlen::Run(BigIntT source, DecimalT &target) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

// Cast integer to varlen type
template <>
inline bool IntegerTryCastToVarlen::Run(BigIntT source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    if (source == 0) {
        target.prefix[0] = '0';
        target.length = 1;
        return true;
    }

    // TODO: High performance itoa needed here.
    String tmp_str = ToStr(source);
    target.length = static_cast<u16>(tmp_str.size());
    if (target.length <= VarcharT::INLINE_LENGTH) {
        Memcpy(target.prefix, tmp_str.c_str(), target.length);
        Memset(target.prefix + target.length, 0, VarcharT::INLINE_LENGTH - target.length);
    } else {
        Memcpy(target.prefix, tmp_str.c_str(), VarcharT::PREFIX_LENGTH);
        Assert<TypeException>(vector_ptr->buffer_->buffer_type_ == VectorBufferType::kHeap,
                              "Varchar column vector should use MemoryVectorBuffer. ",
                              __FILE_NAME__,
                              __LINE__);

        ptr_t ptr = vector_ptr->buffer_->heap_mgr_->Allocate(target.length);
        Memcpy(ptr, tmp_str.c_str(), target.length);
        target.ptr = ptr;
    }

    return true;
}

// TODO: Cast HugeInt to other numeric type
template <>
inline bool IntegerTryCastToFixlen::Run(HugeIntT source, TinyIntT &target) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

template <>
inline bool IntegerTryCastToFixlen::Run(HugeIntT source, SmallIntT &target) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

template <>
inline bool IntegerTryCastToFixlen::Run(HugeIntT source, IntegerT &target) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

template <>
inline bool IntegerTryCastToFixlen::Run(HugeIntT source, BigIntT &target) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

template <>
inline bool IntegerTryCastToFixlen::Run(HugeIntT source, FloatT &target) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

template <>
inline bool IntegerTryCastToFixlen::Run(HugeIntT source, DoubleT &target) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

// TODO
template <>
inline bool IntegerTryCastToFixlen::Run(HugeIntT source, DecimalT &target) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

// Cast integer to varlen type
template <>
inline bool IntegerTryCastToVarlen::Run(HugeIntT source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

} // namespace infinity
