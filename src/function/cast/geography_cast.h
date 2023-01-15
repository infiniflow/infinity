//
// Created by jinhai on 22-12-23.
//

#pragma once

#include "bound_cast_func.h"
#include "column_vector_cast.h"
#include "common/types/data_type.h"
#include "common/types/internal_types.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

struct GeographyTryCastToVarlen;

template<class SourceType>
static inline BoundCastFunc
BindGeographyCast(const DataType &source, DataType &target) {
    TypeAssert(source.type() != target.type(),
               "Attempt to cast from " + source.ToString() + " to " + target.ToString());
    switch(target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<SourceType, VarcharT, GeographyTryCastToVarlen>);
        }
        default: {
            TypeError("Can't cast from Time type to " + target.ToString());
        }
    }
}

struct GeographyTryCastToVarlen {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(const SourceType& source, TargetType &target, const SharedPtr<ColumnVector>& vector_ptr){
        FunctionError("Not support to cast from " + DataType::TypeToString<SourceType>()
                      + " to " + DataType::TypeToString<TargetType>());
    }
};

template<>
inline bool
GeographyTryCastToVarlen::Run(const PointT& source, VarcharT &target, const SharedPtr<ColumnVector>& vector_ptr) {
    NotImplementError("Not implemented");
}

template<>
inline bool
GeographyTryCastToVarlen::Run(const LineT& source, VarcharT &target, const SharedPtr<ColumnVector>& vector_ptr) {
    NotImplementError("Not implemented");
}

template<>
inline bool
GeographyTryCastToVarlen::Run(const LineSegT& source, VarcharT &target, const SharedPtr<ColumnVector>& vector_ptr) {
    NotImplementError("Not implemented");
}

template<>
inline bool
GeographyTryCastToVarlen::Run(const BoxT& source, VarcharT &target, const SharedPtr<ColumnVector>& vector_ptr) {
    NotImplementError("Not implemented");
}

template<>
inline bool
GeographyTryCastToVarlen::Run(const PathT& source, VarcharT &target, const SharedPtr<ColumnVector>& vector_ptr) {
    NotImplementError("Not implemented");
}

template<>
inline bool
GeographyTryCastToVarlen::Run(const PolygonT& source, VarcharT &target, const SharedPtr<ColumnVector>& vector_ptr) {
    NotImplementError("Not implemented");
}

template<>
inline bool
GeographyTryCastToVarlen::Run(const CircleT& source, VarcharT &target, const SharedPtr<ColumnVector>& vector_ptr) {
    NotImplementError("Not implemented");
}

}
