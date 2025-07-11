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
module infinity_core;
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
struct DayOfMonthFunction {
    template <typename TA, typename TB>
    static inline bool Run(TA left, TB &result) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
        return false;
    }

};

template <>
inline bool DayOfMonthFunction::Run(DateT left, BigIntT &result) {
    year_month_day ymd;
    DateT::OuterDate2YMD(left, ymd);
    sys_days sd = sys_days(ymd);
    year_month_day start{ymd.year(), ymd.month(), day(1)};
    sys_days start_sd = sys_days(start);
    auto days_diff = sd - start_sd;
    result = days_diff.count() + 1;
    return true;
}

template <>
inline bool DayOfMonthFunction::Run(DateTimeT left, BigIntT &result) {
    year_month_day ymd;
    DateTimeT::OuterDateTime2YMD(left.date, ymd);
    sys_days sd = sys_days(ymd);
    year_month_day start{ymd.year(), ymd.month(), day(1)};
    sys_days start_sd = sys_days(start);
    auto days_diff = sd - start_sd;
    result = days_diff.count() + 1;
    return true;
}

template <>
inline bool DayOfMonthFunction::Run(TimestampT left, BigIntT &result) {
    year_month_day ymd;
    TimestampT::OuterDateTime2YMD(left.date, ymd);
    sys_days sd = sys_days(ymd);
    year_month_day start{ymd.year(), ymd.month(), day(1)};
    sys_days start_sd = sys_days(start);
    auto days_diff = sd - start_sd;
    result = days_diff.count() + 1;
    return true;
}

void RegisterDayOfMonthFunction(NewCatalog *catalog_ptr) {
    String func_name = "dayofmonth";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction day_of_month_date_function(func_name,
                                  {DataType(LogicalType::kDate)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::UnaryFunctionWithFailure<DateT, BigIntT, DayOfMonthFunction>);
    function_set_ptr->AddFunction(day_of_month_date_function);

    ScalarFunction day_of_month_datetime_function(func_name,
                                  {DataType(LogicalType::kDateTime)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::UnaryFunctionWithFailure<DateTimeT, BigIntT, DayOfMonthFunction>);
    function_set_ptr->AddFunction(day_of_month_datetime_function);

    ScalarFunction day_of_month_timestamp_function(func_name,
                                  {DataType(LogicalType::kTimestamp)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::UnaryFunctionWithFailure<TimestampT, BigIntT, DayOfMonthFunction>);
    function_set_ptr->AddFunction(day_of_month_timestamp_function);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity