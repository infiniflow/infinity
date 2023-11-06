//
// Created by jinhai on 22-12-23.
//

module;

import stl;
import column_vector;
import vector_buffer;
import bound_cast_func;
import parser;
import column_vector_cast;

import infinity_exception;
import third_party;

export module date_cast;

namespace infinity {

export struct DateTryCastToFixlen;
export struct DateTryCastToVarlen;

export inline BoundCastFunc BindDateCast(DataType &target) {
    switch (target.type()) {
        case LogicalType::kDateTime: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<DateT, DateTimeT, DateTryCastToFixlen>);
        }
        case LogicalType::kTimestamp: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<DateT, TimestampT, DateTryCastToFixlen>);
        }
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<DateT, VarcharT, DateTryCastToVarlen>);
        }
        default: {
            Error<TypeException>(Format("Can't cast from Date type to {}", target.ToString()));
        }
    }
}

struct DateTryCastToFixlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType source, TargetType &target) {
        Error<FunctionException>(
                Format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
    }
};

struct DateTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType source, TargetType &target, const SharedPtr<ColumnVector> &vector_ptr) {
        Error<FunctionException>(
                Format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
    }
};

template <>
inline bool DateTryCastToFixlen::Run(DateT source, DateTimeT &target) {
    Error<FunctionException>(
            "Not implemented");;
}

template <>
inline bool DateTryCastToFixlen::Run(DateT source, TimestampT &target) {
    Error<FunctionException>(
            "Not implemented");;
}

template <>
inline bool DateTryCastToVarlen::Run(DateT source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    Error<FunctionException>(
            "Not implemented");;
}

} // namespace infinity
