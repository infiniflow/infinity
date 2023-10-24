//
// Created by jinhai on 22-12-22.
//

module;

import stl;
import column_vector;
import vector_buffer;
import bound_cast_func;
import parser;
import column_vector_cast;
import infinity_assert;
import infinity_exception;
import third_party;

export module decimal_cast;

namespace infinity {

export struct DecimalTryCastToFixlen;
export struct DecimalTryCastToVarlen;

export template <class SourceType>
inline BoundCastFunc BindDecimalCast(const DataType &source, DataType &target) {
    Assert<FunctionException>(source.type() != target.type(),
                              Format("Attempt to cast from {} to {}", source.ToString(), target.ToString()),
                              __FILE_NAME__,
                              __LINE__);
    switch (target.type()) {
        case LogicalType::kTinyInt: {
            Error<FunctionException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case LogicalType::kSmallInt: {
            Error<FunctionException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case LogicalType::kInteger: {
            Error<FunctionException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case LogicalType::kBigInt: {
            Error<FunctionException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case LogicalType::kHugeInt: {
            Error<FunctionException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case LogicalType::kFloat: {
            Error<FunctionException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case LogicalType::kDouble: {
            Error<FunctionException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case LogicalType::kDecimal: {
            Error<FunctionException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        case LogicalType::kVarchar: {
            Error<FunctionException>("Not implemented", __FILE_NAME__, __LINE__);
        }
        default: {
            Error<TypeException>(Format("Can't cast from Decimal type to {}", target.ToString()), __FILE_NAME__, __LINE__);
        }
    }
}

struct DecimalTryCastToFixlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType source, TargetType &target) {
        Error<FunctionException>(
            Format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()),
            __FILE_NAME__,
            __LINE__);
    }
};

struct DecimalTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType source, TargetType &target, const SharedPtr<ColumnVector> &vector_ptr) {
        Error<FunctionException>(
            Format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()),
            __FILE_NAME__,
            __LINE__);
    }
};

// Cast DecimalT to TinyIntT
template <>
inline bool DecimalTryCastToFixlen::Run(DecimalT source, TinyIntT &target) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

template <>
inline bool DecimalTryCastToFixlen::Run(DecimalT source, SmallIntT &target) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

template <>
inline bool DecimalTryCastToFixlen::Run(DecimalT source, IntegerT &target) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

template <>
inline bool DecimalTryCastToFixlen::Run(DecimalT source, BigIntT &target) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

template <>
inline bool DecimalTryCastToFixlen::Run(DecimalT source, HugeIntT &target) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

template <>
inline bool DecimalTryCastToFixlen::Run(DecimalT source, FloatT &target) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

template <>
inline bool DecimalTryCastToFixlen::Run(DecimalT source, DoubleT &target) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

template <>
inline bool DecimalTryCastToVarlen::Run(DecimalT source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

} // namespace infinity
