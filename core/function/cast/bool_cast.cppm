//
// Created by JinHai on 2022/12/15.
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

export module bool_cast;

namespace infinity {

export struct TryCastBoolean {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType input, TargetType &target) {
        Error<FunctionException>("No implementation to cast from " + DataType::TypeToString<SourceType>() + " to " +
                                     DataType::TypeToString<TargetType>(),
                                 __FILE_NAME__,
                                 __LINE__);
    }
};

// Cast BooleanT to other VarcharT type
template <>
inline bool TryCastBoolean::Run(BooleanT source, VarcharT &target) {
    if (source) {
        constexpr u16 TRUE_LEN = 4;
        Memcpy(target.prefix, "true", TRUE_LEN);
        Memset(target.prefix + TRUE_LEN, 0, VarcharT::INLINE_LENGTH - TRUE_LEN);
        target.length = TRUE_LEN;
    } else {
        constexpr u16 FALSE_LEN = 5;
        Memcpy(target.prefix, "false", FALSE_LEN);
        Memset(target.prefix + FALSE_LEN, 0, VarcharT::INLINE_LENGTH - FALSE_LEN);
        target.length = FALSE_LEN;
    }
    return true;
}

export inline BoundCastFunc BindBoolCast(const DataType &source, const DataType &target) {
    Assert<TypeException>(source.type() == LogicalType::kBoolean,
                          Format("Expect boolean type, but it is {}", source.ToString()),
                          __FILE_NAME__,
                          __LINE__);

    switch (target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<BooleanT, VarcharT, TryCastBoolean>);
        }
        default: {
            Error<TypeException>(Format("Can't cast from Boolean to {}", target.ToString()), __FILE_NAME__, __LINE__);
        }
    }
}

} // namespace infinity