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
#include <cmath>
module century;

import stl;
import new_catalog;
import status;
import logical_type;
import infinity_exception;
import scalar_function;
import scalar_function_set;
import third_party;
import internal_types;
import data_type;
import column_vector;

namespace infinity {

struct CenturyFunction {
    template <typename TA, typename TB>
    static inline bool Run(TA left, TB &result) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
        return false;
    }

};

template <>
inline bool CenturyFunction::Run(DateT left, BigIntT &result) {
    auto year = DateT::GetDatePart(left, TimeUnit::kYear);
    if (year > 0) {
        result = static_cast<i64>(ceil(year / 100.0));
    } else if (year < 0) {
        result = static_cast<i64>(floor(year / 100.0));
    } else {
        Status status = Status::InvalidParameterValue("Date", "0", "There is no year 0 in the AD/BC dating system.");
        RecoverableError(status);
        return false;
    }
    return true;
}

template <>
inline bool CenturyFunction::Run(DateTimeT left, BigIntT &result) {
    auto year = DateTimeT::GetDateTimePart(left, TimeUnit::kYear);
    if (year > 0) {
        result = static_cast<i64>(ceil(year / 100.0));
    } else if (year < 0) {
        result = static_cast<i64>(floor(year / 100.0));
    } else {
        Status status = Status::InvalidParameterValue("DateTime", "0", "There is no year 0 in the AD/BC dating system.");
        RecoverableError(status);
        return false;
    }
    
    return true;
}

template <>
inline bool CenturyFunction::Run(TimestampT left, BigIntT &result) {
    auto year = TimestampT::GetDateTimePart(left, TimeUnit::kYear);
    if (year > 0) {
        result = static_cast<i64>(ceil(year / 100.0));
    } else if (year < 0) {
        result = static_cast<i64>(floor(year / 100.0));
    } else {
        Status status = Status::InvalidParameterValue("TimeStamp", "0", "There is no year 0 in the AD/BC dating system.");
        RecoverableError(status);
        return false;
    }
    return true;
}

void RegisterCenturyFunction(NewCatalog *catalog_ptr) {
    String func_name = "century";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction century_date_function(func_name,
                                  {DataType(LogicalType::kDate)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::UnaryFunctionWithFailure<DateT, BigIntT, CenturyFunction>);
    function_set_ptr->AddFunction(century_date_function);

    ScalarFunction century_datetime_function(func_name,
                                  {DataType(LogicalType::kDateTime)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::UnaryFunctionWithFailure<DateTimeT, BigIntT, CenturyFunction>);
    function_set_ptr->AddFunction(century_datetime_function);

    ScalarFunction century_timestamp_function(func_name,
                                  {DataType(LogicalType::kTimestamp)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::UnaryFunctionWithFailure<TimestampT, BigIntT, CenturyFunction>);
    function_set_ptr->AddFunction(century_timestamp_function);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity