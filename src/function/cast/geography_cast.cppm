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

export module geography_cast;

import stl;
import column_vector;
import vector_buffer;
import bound_cast_func;
import data_type;
import column_vector_cast;
import logical_type;
import infinity_exception;
import third_party;
import internal_types;
import logger;

namespace infinity {

export struct GeographyTryCastToVarlen;

export template <class SourceType>
inline BoundCastFunc BindGeographyCast(const DataType &source, DataType &target) {
    if (source.type() == target.type()) {
        String error_message = "Can't cast from the same type";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    switch (target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<SourceType, VarcharT, GeographyTryCastToVarlen>);
        }
        default: {
            String error_message = fmt::format("Can't cast from geography type to {}", target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }
    return BoundCastFunc(nullptr);
}

struct GeographyTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &, TargetType &, ColumnVector*) {
        String error_message = fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>());
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
        return false;
    }
};

template <>
inline bool GeographyTryCastToVarlen::Run(const PointT &, VarcharT &, ColumnVector*) {
    String error_message = "Not implement: GeographyTryCastToVarlen::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

template <>
inline bool GeographyTryCastToVarlen::Run(const LineT &, VarcharT &, ColumnVector*) {
    String error_message = "Not implement: GeographyTryCastToVarlen::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

template <>
inline bool GeographyTryCastToVarlen::Run(const LineSegT &, VarcharT &, ColumnVector*) {
    String error_message = "Not implement: GeographyTryCastToVarlen::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

template <>
inline bool GeographyTryCastToVarlen::Run(const BoxT &, VarcharT &, ColumnVector*) {
    String error_message = "Not implement: GeographyTryCastToVarlen::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}
#if 0
template <>
inline bool GeographyTryCastToVarlen::Run(const PathT &source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    String error_message = "Not implement: GeographyTryCastToVarlen::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

template <>
inline bool GeographyTryCastToVarlen::Run(const PolygonT &source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    String error_message = "Not implement: GeographyTryCastToVarlen::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}
#endif
template <>
inline bool GeographyTryCastToVarlen::Run(const CircleT &, VarcharT &, ColumnVector*) {
    String error_message = "Not implement: GeographyTryCastToVarlen::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

} // namespace infinity
