//
// Created by jinhai on 22-12-24.
//

#pragma once

#include "bound_cast_func.h"
#include "column_vector_cast.h"
#include "common/types/data_type.h"

#include "common/utility/infinity_assert.h"

namespace infinity {

struct BitmapTryCastToVarlen;

inline static BoundCastFunc BindBitmapCast(DataType &target) {
    switch (target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<BitmapT, VarcharT, BitmapTryCastToVarlen>);
        }
        default: {
            TypeError("Can't cast from Time type to " + target.ToString());
        }
    }
}

struct BitmapTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &source, TargetType &target, const SharedPtr<ColumnVector> &vector_ptr) {
        FunctionError("Not support to cast from " + DataType::TypeToString<SourceType>() + " to " + DataType::TypeToString<TargetType>());
    }
};

template <>
inline bool BitmapTryCastToVarlen::Run(const BitmapT &source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    NotImplementError("Not implemented");
}

} // namespace infinity
