//
// Created by jinhai on 22-12-23.
//

module;

import stl;
import parser;
import column_vector_cast;
import infinity_assert;
import infinity_exception;
import bound_cast_func;
import column_vector;
import third_party;

export module timestamp_cast;

namespace infinity {

export struct TimestampTryCastToFixlen;
export struct TimestampTryCastToVarlen;

export inline BoundCastFunc BindTimestampCast(DataType &target) {
    switch (target.type()) {
        case LogicalType::kDate: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<TimestampT, DateT, TimestampTryCastToFixlen>);
        }
        case LogicalType::kTime: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<TimestampT, TimeT, TimestampTryCastToFixlen>);
        }
        case LogicalType::kDateTime: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<TimestampT, DateTimeT, TimestampTryCastToFixlen>);
        }
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<TimestampT, VarcharT, TimestampTryCastToVarlen>);
        }
        default: {
            Error<TypeException>(Format("Can't cast from Timestamp type to {}", target.ToString()), __FILE_NAME__, __LINE__);
        }
    }
}

struct TimestampTryCastToFixlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType source, TargetType &target) {
        Error<FunctionException>(
                Format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()),
                __FILE_NAME__,
                __LINE__);
    }
};

struct TimestampTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType source, TargetType &target, const SharedPtr<ColumnVector> &vector_ptr) {
        Error<FunctionException>(
                Format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()),
                __FILE_NAME__,
                __LINE__);
    }
};

template <>
inline bool TimestampTryCastToFixlen::Run(TimestampT source, DateT &target) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

template <>
inline bool TimestampTryCastToFixlen::Run(TimestampT source, TimeT &target) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

template <>
inline bool TimestampTryCastToFixlen::Run(TimestampT source, DateTimeT &target) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

template <>
inline bool TimestampTryCastToVarlen::Run(TimestampT source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

} // namespace infinity
