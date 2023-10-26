//
// Created by jinhai on 22-12-24.
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

export module bitmap_cast;

namespace infinity {

export struct BitmapTryCastToVarlen;

export inline BoundCastFunc BindBitmapCast(DataType &target) {
    switch (target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<BitmapT, VarcharT, BitmapTryCastToVarlen>);
        }
        default: {
            Error<TypeException>(Format("Can't cast from Time type to {}", target.ToString()), __FILE_NAME__, __LINE__);
        }
    }
}

struct BitmapTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &source, TargetType &target, const SharedPtr<ColumnVector> &vector_ptr) {
        Error<FunctionException>("Not support to cast from " + DataType::TypeToString<SourceType>() + " to " + DataType::TypeToString<TargetType>(),
                                 __FILE_NAME__,
                                 __LINE__);
    }
};

template <>
inline bool BitmapTryCastToVarlen::Run(const BitmapT &source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    Error<FunctionException>("Not implemented", __FILE_NAME__, __LINE__);
}

} // namespace infinity
