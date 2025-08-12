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

module infinity_core:weekday.impl;

import :weekday;
import :stl;
import :new_catalog;
import :status;
import :infinity_exception;
import :scalar_function;
import :scalar_function_set;
import :column_vector;

import std;

import internal_types;
import data_type;
import logical_type;

namespace infinity {

using namespace std::chrono;

struct WeekdayFunction {
    template <typename TA, typename TB>
    static inline bool Run(TA left, TB &result) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
        return false;
    }

};

template <>
inline bool WeekdayFunction::Run(DateT left, BigIntT &result) {
    year_month_day ymd;
    DateT::OuterDate2YMD(left, ymd);
    result = static_cast<i64>(weekday{ymd}.c_encoding());
    return true;
}

template <>
inline bool WeekdayFunction::Run(DateTimeT left, BigIntT &result) {
    year_month_day ymd;
    DateTimeT::OuterDateTime2YMD(left.date, ymd);
    result = static_cast<i64>(weekday{ymd}.c_encoding());
    return true;
}

template <>
inline bool WeekdayFunction::Run(TimestampT left, BigIntT &result) {
    year_month_day ymd;
    TimestampT::OuterDateTime2YMD(left.date, ymd);
    result = static_cast<i64>(weekday{ymd}.c_encoding());
    return true;
}

void RegisterWeekdayFunction(NewCatalog *catalog_ptr) {
    String func_name = "weekday";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction weekday_date_function(func_name,
                                  {DataType(LogicalType::kDate)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::UnaryFunctionWithFailure<DateT, BigIntT, WeekdayFunction>);
    function_set_ptr->AddFunction(weekday_date_function);

    ScalarFunction weekday_datetime_function(func_name,
                                  {DataType(LogicalType::kDateTime)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::UnaryFunctionWithFailure<DateTimeT, BigIntT, WeekdayFunction>);
    function_set_ptr->AddFunction(weekday_datetime_function);

    ScalarFunction weekday_timestamp_function(func_name,
                                  {DataType(LogicalType::kTimestamp)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::UnaryFunctionWithFailure<TimestampT, BigIntT, WeekdayFunction>);
    function_set_ptr->AddFunction(weekday_timestamp_function);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity