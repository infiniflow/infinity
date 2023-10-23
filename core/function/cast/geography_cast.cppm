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

export module geography_cast;

namespace infinity {

export struct GeographyTryCastToVarlen;

export template <class SourceType>
inline BoundCastFunc BindGeographyCast(const DataType &source, DataType &target) {
    Assert<FunctionException>(source.type() != target.type(),
                              Format("Attempt to cast from {} to {}", source.ToString(), target.ToString()),
                              __FILE_NAME__,
                              __LINE__);
    switch (target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<SourceType, VarcharT, GeographyTryCastToVarlen>);
        }
        default: {
            Error<TypeException>(Format("Can't cast from geography type to {}", target.ToString()), __FILE_NAME__, __LINE__);
        }
    }
}

struct GeographyTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &source, TargetType &target, const SharedPtr<ColumnVector> &vector_ptr) {
        Error<FunctionException>(
                Format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()),
                __FILE_NAME__,
                __LINE__);
    }
};

template <>
inline bool GeographyTryCastToVarlen::Run(const PointT &source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

template <>
inline bool GeographyTryCastToVarlen::Run(const LineT &source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

template <>
inline bool GeographyTryCastToVarlen::Run(const LineSegT &source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

template <>
inline bool GeographyTryCastToVarlen::Run(const BoxT &source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

template <>
inline bool GeographyTryCastToVarlen::Run(const PathT &source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

template <>
inline bool GeographyTryCastToVarlen::Run(const PolygonT &source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

template <>
inline bool GeographyTryCastToVarlen::Run(const CircleT &source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    Error<NotImplementException>("Not implemented", __FILE_NAME__, __LINE__);
}

} // namespace infinity
