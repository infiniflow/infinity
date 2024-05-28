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

export module decimal_cast;

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

export struct DecimalTryCastToFixlen;
export struct DecimalTryCastToVarlen;

export template <class SourceType>
inline BoundCastFunc BindDecimalCast(const DataType &source, DataType &target) {
    if (source.type() == target.type()) {
        String error_message = "Can't cast from the same type";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    switch (target.type()) {
        case LogicalType::kTinyInt: {
            String error_message = "Not implemented";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        case LogicalType::kSmallInt: {
            String error_message = "Not implemented";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        case LogicalType::kInteger: {
            String error_message = "Not implemented";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        case LogicalType::kBigInt: {
            String error_message = "Not implemented";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        case LogicalType::kHugeInt: {
            String error_message = "Not implemented";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        case LogicalType::kFloat: {
            String error_message = "Not implemented";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        case LogicalType::kDouble: {
            String error_message = "Not implemented";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        case LogicalType::kDecimal: {
            String error_message = "Not implemented";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        case LogicalType::kVarchar: {
            String error_message = "Not implemented";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        default: {
            String error_message = fmt::format("Can't cast from Decimal type to {}", target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }
    return BoundCastFunc(nullptr);
}

struct DecimalTryCastToFixlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType , TargetType &) {
        UnrecoverableError(
            fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
    }
};

struct DecimalTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType , TargetType &, const SharedPtr<ColumnVector> &) {
        UnrecoverableError(
            fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
    }
};

// Cast DecimalT to TinyIntT
template <>
inline bool DecimalTryCastToFixlen::Run(DecimalT, TinyIntT &) {
    String error_message = "Not implement: DecimalTryCastToFixlen::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

template <>
inline bool DecimalTryCastToFixlen::Run(DecimalT, SmallIntT &) {
    String error_message = "Not implement: DecimalTryCastToFixlen::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

template <>
inline bool DecimalTryCastToFixlen::Run(DecimalT, IntegerT &) {
    String error_message = "Not implement: DecimalTryCastToFixlen::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

template <>
inline bool DecimalTryCastToFixlen::Run(DecimalT, BigIntT &) {
    String error_message = "Not implement: DecimalTryCastToFixlen::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

template <>
inline bool DecimalTryCastToFixlen::Run(DecimalT, HugeIntT &) {
    String error_message = "Not implement: DecimalTryCastToFixlen::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

template <>
inline bool DecimalTryCastToFixlen::Run(DecimalT, FloatT &) {
    String error_message = "Not implement: DecimalTryCastToFixlen::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

template <>
inline bool DecimalTryCastToFixlen::Run(DecimalT, DoubleT &) {
    String error_message = "Not implement: DecimalTryCastToFixlen::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

template <>
inline bool DecimalTryCastToVarlen::Run(DecimalT, VarcharT &, const SharedPtr<ColumnVector> &) {
    String error_message = "Not implement: DecimalTryCastToFixlen::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

} // namespace infinity
