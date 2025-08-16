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

export module infinity_core:decimal_cast;

import :column_vector;
import :vector_buffer;
import :bound_cast_func;
import :infinity_exception;
import :column_vector_cast;

import data_type;
import logical_type;
import internal_types;

namespace infinity {

export struct DecimalTryCastToFixlen;
export struct DecimalTryCastToVarlen;

export template <class SourceType>
inline BoundCastFunc BindDecimalCast(const DataType &source, DataType &target) {
    if (source.type() == target.type()) {
        UnrecoverableError("Can't cast from the same type");
    }
    switch (target.type()) {
        case LogicalType::kTinyInt: {
            UnrecoverableError("Not implemented");
        }
        case LogicalType::kSmallInt: {
            UnrecoverableError("Not implemented");
        }
        case LogicalType::kInteger: {
            UnrecoverableError("Not implemented");
        }
        case LogicalType::kBigInt: {
            UnrecoverableError("Not implemented");
        }
        case LogicalType::kHugeInt: {
            UnrecoverableError("Not implemented");
        }
        case LogicalType::kFloat: {
            UnrecoverableError("Not implemented");
        }
        case LogicalType::kDouble: {
            UnrecoverableError("Not implemented");
        }
        case LogicalType::kDecimal: {
            UnrecoverableError("Not implemented");
        }
        case LogicalType::kVarchar: {
            UnrecoverableError("Not implemented");
        }
        default: {
            UnrecoverableError(fmt::format("Can't cast from Decimal type to {}", target.ToString()));
        }
    }
    return BoundCastFunc(nullptr);
}

struct DecimalTryCastToFixlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType, TargetType &) {
        UnrecoverableError(
            fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
    }
};

struct DecimalTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType, TargetType &, const std::shared_ptr<ColumnVector> &) {
        UnrecoverableError(
            fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
    }
};

// Cast DecimalT to TinyIntT
template <>
inline bool DecimalTryCastToFixlen::Run(DecimalT, TinyIntT &) {
    UnrecoverableError("Not implement: DecimalTryCastToFixlen::Run");
    return false;
}

template <>
inline bool DecimalTryCastToFixlen::Run(DecimalT, SmallIntT &) {
    UnrecoverableError("Not implement: DecimalTryCastToFixlen::Run");
    return false;
}

template <>
inline bool DecimalTryCastToFixlen::Run(DecimalT, IntegerT &) {
    UnrecoverableError("Not implement: DecimalTryCastToFixlen::Run");
    return false;
}

template <>
inline bool DecimalTryCastToFixlen::Run(DecimalT, BigIntT &) {
    UnrecoverableError("Not implement: DecimalTryCastToFixlen::Run");
    return false;
}

template <>
inline bool DecimalTryCastToFixlen::Run(DecimalT, HugeIntT &) {
    UnrecoverableError("Not implement: DecimalTryCastToFixlen::Run");
    return false;
}

template <>
inline bool DecimalTryCastToFixlen::Run(DecimalT, FloatT &) {
    UnrecoverableError("Not implement: DecimalTryCastToFixlen::Run");
    return false;
}

template <>
inline bool DecimalTryCastToFixlen::Run(DecimalT, DoubleT &) {
    UnrecoverableError("Not implement: DecimalTryCastToFixlen::Run");
    return false;
}

template <>
inline bool DecimalTryCastToVarlen::Run(DecimalT, VarcharT &, const std::shared_ptr<ColumnVector> &) {
    UnrecoverableError("Not implement: DecimalTryCastToFixlen::Run");
    return false;
}

} // namespace infinity
