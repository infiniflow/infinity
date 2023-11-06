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

export module datetime_cast;

namespace infinity {

export struct DateTimeTryCastToFixlen;
export struct DateTimeTryCastToVarlen;

export inline BoundCastFunc BindDateTimeCast(DataType &target) {
    switch (target.type()) {
        case LogicalType::kDate: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<DateTimeT, DateT, DateTimeTryCastToFixlen>);
        }
        case LogicalType::kTime: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<DateTimeT, TimeT, DateTimeTryCastToFixlen>);
        }
        case LogicalType::kTimestamp: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<DateTimeT, TimestampT, DateTimeTryCastToFixlen>);
        }
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<DateTimeT, VarcharT, DateTimeTryCastToVarlen>);
        }
        default: {
            Error<TypeException>(Format("Can't cast from DateTime type to {}", target.ToString()));
        }
    }
}

struct DateTimeTryCastToFixlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType source, TargetType &target) {
        Error<FunctionException>(
                Format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
    }
};

struct DateTimeTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType source, TargetType &target, const SharedPtr<ColumnVector> &vector_ptr) {
        Error<FunctionException>(
                Format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
    }
};

template <>
inline bool DateTimeTryCastToFixlen::Run(DateTimeT source, DateT &target) {
    Error<FunctionException>(
            "Not implemented");;
}

template <>
inline bool DateTimeTryCastToFixlen::Run(DateTimeT source, TimeT &target) {
        Error<FunctionException>(
            "Not implemented");;
}

template <>
inline bool DateTimeTryCastToFixlen::Run(DateTimeT source, TimestampT &target) {
        Error<FunctionException>(
            "Not implemented");;
}

template <>
inline bool DateTimeTryCastToVarlen::Run(DateTimeT source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
        Error<FunctionException>(
            "Not implemented");;
}

} // namespace infinity
