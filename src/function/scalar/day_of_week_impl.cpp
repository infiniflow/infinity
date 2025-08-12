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
#include <chrono>
module infinity_core:day_of_week.impl;

import :day_of_week;
import :stl;
import :new_catalog;
import :status;
import logical_type;
import :infinity_exception;
import :scalar_function;
import :scalar_function_set;
import :third_party;
import internal_types;
import data_type;
import :column_vector;

namespace infinity {

using namespace std::chrono;

struct DayOfWeekFunction {
    template <typename TA, typename TB>
    static inline bool Run(TA left, TB &result) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
        return false;
    }
};

template <>
inline bool DayOfWeekFunction::Run(DateT left, BigIntT &result) {
    year_month_day ymd;
    DateT::OuterDate2YMD(left, ymd);
    weekday wd = weekday{ymd};
    days diff = (wd - weekday{0}) % days{7};
    sys_days ymd_sys_days = sys_days(ymd);
    sys_days monday_sys_days = ymd_sys_days - diff;
    result = (ymd_sys_days - monday_sys_days).count();
    return true;
}

template <>
inline bool DayOfWeekFunction::Run(DateTimeT left, BigIntT &result) {
    year_month_day ymd;
    DateTimeT::OuterDateTime2YMD(left.date, ymd);
    weekday wd = weekday{ymd};
    days diff = (wd - weekday{0}) % days{7};
    sys_days ymd_sys_days = sys_days(ymd);
    sys_days monday_sys_days = ymd_sys_days - diff;
    result = (ymd_sys_days - monday_sys_days).count();
    return true;
}

template <>
inline bool DayOfWeekFunction::Run(TimestampT left, BigIntT &result) {
    year_month_day ymd;
    TimestampT::OuterDateTime2YMD(left.date, ymd);
    weekday wd = weekday{ymd};
    days diff = (wd - weekday{0}) % days{7};
    sys_days ymd_sys_days = sys_days(ymd);
    sys_days monday_sys_days = ymd_sys_days - diff;
    result = (ymd_sys_days - monday_sys_days).count();
    return true;
}

void RegisterDayOfWeekFunction(NewCatalog *catalog_ptr) {
    String func_name = "dayofweek";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction day_of_week_date_function(func_name,
                                             {DataType(LogicalType::kDate)},
                                             {DataType(LogicalType::kBigInt)},
                                             &ScalarFunction::UnaryFunctionWithFailure<DateT, BigIntT, DayOfWeekFunction>);
    function_set_ptr->AddFunction(day_of_week_date_function);

    ScalarFunction day_of_week_datetime_function(func_name,
                                                 {DataType(LogicalType::kDateTime)},
                                                 {DataType(LogicalType::kBigInt)},
                                                 &ScalarFunction::UnaryFunctionWithFailure<DateTimeT, BigIntT, DayOfWeekFunction>);
    function_set_ptr->AddFunction(day_of_week_datetime_function);

    ScalarFunction day_of_week_timestamp_function(func_name,
                                                  {DataType(LogicalType::kTimestamp)},
                                                  {DataType(LogicalType::kBigInt)},
                                                  &ScalarFunction::UnaryFunctionWithFailure<TimestampT, BigIntT, DayOfWeekFunction>);
    function_set_ptr->AddFunction(day_of_week_timestamp_function);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity