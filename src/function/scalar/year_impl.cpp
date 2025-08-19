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

module infinity_core:year.impl;

import :year;
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

struct YearFunction {
    template <typename TA, typename TB>
    static inline bool Run(TA left, TB &result) {
        RecoverableError(Status::NotSupport("Not implemented"));
        return false;
    }
};

template <>
inline bool YearFunction::Run(DateT left, BigIntT &result) {
    result = DateT::GetDatePart(left, TimeUnit::kYear);
    return true;
}

template <>
inline bool YearFunction::Run(DateTimeT left, BigIntT &result) {
    result = DateTimeT::GetDateTimePart(left, TimeUnit::kYear);
    return true;
}

template <>
inline bool YearFunction::Run(TimestampT left, BigIntT &result) {
    result = TimestampT::GetDateTimePart(left, TimeUnit::kYear);
    return true;
}

void RegisterYearFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "year";

    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);

    ScalarFunction year_date_function(func_name,
                                      {DataType(LogicalType::kDate)},
                                      {DataType(LogicalType::kBigInt)},
                                      &ScalarFunction::UnaryFunctionWithFailure<DateT, BigIntT, YearFunction>);
    function_set_ptr->AddFunction(year_date_function);

    ScalarFunction year_datetime_function(func_name,
                                          {DataType(LogicalType::kDateTime)},
                                          {DataType(LogicalType::kBigInt)},
                                          &ScalarFunction::UnaryFunctionWithFailure<DateTimeT, BigIntT, YearFunction>);
    function_set_ptr->AddFunction(year_datetime_function);

    ScalarFunction year_timestamp_function(func_name,
                                           {DataType(LogicalType::kTimestamp)},
                                           {DataType(LogicalType::kBigInt)},
                                           &ScalarFunction::UnaryFunctionWithFailure<TimestampT, BigIntT, YearFunction>);
    function_set_ptr->AddFunction(year_timestamp_function);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity