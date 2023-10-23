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
import infinity_assert;
import infinity_exception;
import third_party;
// import logger;

export module interval_cast;

namespace infinity {

export struct IntervalTryCastToVarlen;

export inline BoundCastFunc BindTimeCast(DataType &target) {
    switch (target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<IntervalT, VarcharT, IntervalTryCastToVarlen>);
        }
        default: {
            Error<TypeException>(Format("Can't cast from Interval type to {}", target.ToString()), __FILE_NAME__, __LINE__);
        }
    }
}

struct IntervalTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType source, TargetType &target, const SharedPtr<ColumnVector> &vector_ptr) {
        Error<FunctionException>(
            Format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()),
            __FILE_NAME__,
            __LINE__);
    }
};

template <>
inline bool IntervalTryCastToVarlen::Run(IntervalT source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

} // namespace infinity
