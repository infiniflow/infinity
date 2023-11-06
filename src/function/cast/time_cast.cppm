//
// Created by jinhai on 22-12-23.
//

module;

import stl;
import parser;
import column_vector_cast;

import infinity_exception;
import bound_cast_func;
import column_vector;
import third_party;

export module time_cast;

namespace infinity {

export struct TimeTryCastToVarlen;

export inline BoundCastFunc BindTimeCast(DataType &target) {
    switch (target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<TimeT, VarcharT, TimeTryCastToVarlen>);
        }
        default: {
            Error<FunctionException>(
                    Format("Can't cast from Time type to  {}", target.ToString()));
        }
    }
}

struct TimeTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType source, TargetType &target, const SharedPtr<ColumnVector> &vector_ptr) {
        Error<FunctionException>(
                Format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
    }
};

template <>
inline bool TimeTryCastToVarlen::Run(TimeT source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    Error<NotImplementException>("Not implemented");
}

} // namespace infinity
