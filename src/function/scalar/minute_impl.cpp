// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:minute.impl;

import :minute;
import :stl;
import :new_catalog;
import :status;
import :column_vector;
import :infinity_exception;
import :scalar_function;
import :scalar_function_set;

import internal_types;
import data_type;
import logical_type;

namespace infinity {

struct MinuteFunction {
    template <typename TA, typename TB>
    static inline bool Run(TA left, TB &result) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
        return false;
    }

};

template <>
inline bool MinuteFunction::Run(DateTimeT left, BigIntT &result) {
    result = DateTimeT::GetDateTimePart(left, TimeUnit::kMinute);
    return true;
}

template <>
inline bool MinuteFunction::Run(TimeT left, BigIntT &result) {
    result = TimeT::GetTimePart(left, TimeUnit::kMinute);
    return true;
}

template <>
inline bool MinuteFunction::Run(TimestampT left, BigIntT &result) {
    result = TimestampT::GetDateTimePart(left, TimeUnit::kMinute);
    return true;
}

void RegisterMinuteFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "minute";

    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);

    ScalarFunction minute_datetime_function(func_name,
                                  {DataType(LogicalType::kDateTime)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::UnaryFunctionWithFailure<DateTimeT, BigIntT, MinuteFunction>);
    function_set_ptr->AddFunction(minute_datetime_function);

    ScalarFunction minute_time_function(func_name,
                                  {DataType(LogicalType::kTime)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::UnaryFunctionWithFailure<TimeT, BigIntT, MinuteFunction>);
    function_set_ptr->AddFunction(minute_time_function);

    ScalarFunction minute_timestamp_function(func_name,
                                  {DataType(LogicalType::kTimestamp)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::UnaryFunctionWithFailure<TimestampT, BigIntT, MinuteFunction>);
    function_set_ptr->AddFunction(minute_timestamp_function);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity