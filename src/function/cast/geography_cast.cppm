// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

export module infinity_core:geography_cast;

import :stl;
import :column_vector;
import :vector_buffer;
import :bound_cast_func;
import :column_vector_cast;
import :infinity_exception;

import logical_type;
import data_type;
import internal_types;

namespace infinity {

export struct GeographyTryCastToVarlen;

export template <class SourceType>
inline BoundCastFunc BindGeographyCast(const DataType &source, DataType &target) {
    if (source.type() == target.type()) {
        UnrecoverableError("Can't cast from the same type");
    }
    switch (target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<SourceType, VarcharT, GeographyTryCastToVarlen>);
        }
        default: {
            UnrecoverableError(fmt::format("Can't cast from geography type to {}", target.ToString()));
        }
    }
    return BoundCastFunc(nullptr);
}

struct GeographyTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &, TargetType &, ColumnVector *) {
        String error_message =
            fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>());
        UnrecoverableError(error_message);
        return false;
    }
};

template <>
inline bool GeographyTryCastToVarlen::Run(const PointT &, VarcharT &, ColumnVector *) {
    UnrecoverableError("Not implement: GeographyTryCastToVarlen::Run");
    return false;
}

template <>
inline bool GeographyTryCastToVarlen::Run(const LineT &, VarcharT &, ColumnVector *) {
    UnrecoverableError("Not implement: GeographyTryCastToVarlen::Run");
    return false;
}

template <>
inline bool GeographyTryCastToVarlen::Run(const LineSegT &, VarcharT &, ColumnVector *) {
    UnrecoverableError("Not implement: GeographyTryCastToVarlen::Run");
    return false;
}

template <>
inline bool GeographyTryCastToVarlen::Run(const BoxT &, VarcharT &, ColumnVector *) {
    UnrecoverableError("Not implement: GeographyTryCastToVarlen::Run");
    return false;
}
#if 0
template <>
inline bool GeographyTryCastToVarlen::Run(const PathT &source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    UnrecoverableError("Not implement: GeographyTryCastToVarlen::Run");
    return false;
}

template <>
inline bool GeographyTryCastToVarlen::Run(const PolygonT &source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    UnrecoverableError("Not implement: GeographyTryCastToVarlen::Run");
    return false;
}
#endif
template <>
inline bool GeographyTryCastToVarlen::Run(const CircleT &, VarcharT &, ColumnVector *) {
    UnrecoverableError("Not implement: GeographyTryCastToVarlen::Run");
    return false;
}

} // namespace infinity
