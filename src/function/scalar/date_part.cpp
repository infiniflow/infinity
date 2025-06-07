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
#include <unordered_map>
module date_part;

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

struct DatePartFunction {
    static std::unordered_map<std::string, TimeUnit> time_unit_map;
    template <typename TA, typename TB, typename TC>
    static inline bool Run(TA &left, TB right,TC &result) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
        return false;
    }
    static inline void InitDatePartFunction();

};
std::unordered_map<std::string, TimeUnit> DatePartFunction::time_unit_map;
inline void DatePartFunction::InitDatePartFunction() {

    time_unit_map["year"] = TimeUnit::kYear;
    time_unit_map["years"] = TimeUnit::kYear;
    time_unit_map["y"] = TimeUnit::kYear;
    time_unit_map["yr"] = TimeUnit::kYear;
    time_unit_map["yrs"] = TimeUnit::kYear;


    time_unit_map["month"] = TimeUnit::kMonth;
    time_unit_map["months"] = TimeUnit::kMonth;
    time_unit_map["mons"] = TimeUnit::kMonth;
    time_unit_map["mon"] = TimeUnit::kMonth;


    time_unit_map["day"] = TimeUnit::kDay;
    time_unit_map["days"] = TimeUnit::kDay;
    time_unit_map["d"] = TimeUnit::kDay;
    time_unit_map["dayofmonth"] = TimeUnit::kDay;


    time_unit_map["h"] = TimeUnit::kHour;
    time_unit_map["hr"] = TimeUnit::kHour;
    time_unit_map["hrs"] = TimeUnit::kHour;
    time_unit_map["hour"] = TimeUnit::kHour;
    time_unit_map["hours"] = TimeUnit::kHour;


    time_unit_map["m"] = TimeUnit::kMinute;
    time_unit_map["min"] = TimeUnit::kMinute;
    time_unit_map["mins"] = TimeUnit::kMinute;
    time_unit_map["minute"] = TimeUnit::kMinute;
    time_unit_map["minutes"] = TimeUnit::kMinute;


    time_unit_map["s"] = TimeUnit::kSecond;
    time_unit_map["sec"] = TimeUnit::kSecond;
    time_unit_map["secs"] = TimeUnit::kSecond;
    time_unit_map["second"] = TimeUnit::kSecond;
    time_unit_map["seconds"] = TimeUnit::kSecond;
}

template <>
inline bool DatePartFunction::Run(VarcharT &left, DateT right, BigIntT &result) {
    auto input = left.ToString();
    auto it = time_unit_map.find(input);
    if (it == time_unit_map.end()) {
        Status status = Status::InvalidParameterValue("Time Unit", input, "The format is invalid.");
        RecoverableError(status);
        return false;
    }

    switch (it->second) {
        case TimeUnit::kYear:
            result = DateT::GetDatePart(right, TimeUnit::kYear);
            break;
        case TimeUnit::kMonth:
            result = DateT::GetDatePart(right, TimeUnit::kMonth);
            break;
        case TimeUnit::kDay:
            result = DateT::GetDatePart(right, TimeUnit::kDay);
            break;
        default:
            Status status = Status::InvalidParameterValue("Time Unit", input, "Time Unit is invalid.");
            RecoverableError(status);
            return false;
    }
    return true;
}

template <>
inline bool DatePartFunction::Run(VarcharT &left, DateTimeT right, BigIntT &result) {
    auto input = left.ToString();
    auto it = time_unit_map.find(input);
    if (it == time_unit_map.end()) {
        Status status = Status::InvalidParameterValue("Time Unit", input, "The format is invalid.");
        RecoverableError(status);
        return false;
    }

    switch (it->second) {
        case TimeUnit::kYear:
            result = DateTimeT::GetDateTimePart(right, TimeUnit::kYear);
            break;
        case TimeUnit::kMonth:
            result = DateTimeT::GetDateTimePart(right, TimeUnit::kMonth);
            break;
        case TimeUnit::kDay:
            result = DateTimeT::GetDateTimePart(right, TimeUnit::kDay);
            break;
        case TimeUnit::kHour:
            result = DateTimeT::GetDateTimePart(right, TimeUnit::kHour);
            break;
        case TimeUnit::kMinute:
            result = DateTimeT::GetDateTimePart(right, TimeUnit::kMinute);
            break;
        case TimeUnit::kSecond:
            result = DateTimeT::GetDateTimePart(right, TimeUnit::kSecond);
            break;
        default:
            Status status = Status::InvalidParameterValue("Time Unit", input, "Time Unit Value is invalid.");
            RecoverableError(status);
            return false;
    }
    return true;
}

template <>
inline bool DatePartFunction::Run(VarcharT &left, TimestampT right, BigIntT &result) {
    auto input = left.ToString();
    auto it = time_unit_map.find(input);
    if (it == time_unit_map.end()) {
        Status status = Status::InvalidParameterValue("Time Unit", input, "The format is invalid.");
        RecoverableError(status);
        return false;
    }

    switch (it->second) {
        case TimeUnit::kYear:
            result = TimestampT::GetDateTimePart(right, TimeUnit::kYear);
            break;
        case TimeUnit::kMonth:
            result = TimestampT::GetDateTimePart(right, TimeUnit::kMonth);
            break;
        case TimeUnit::kDay:
            result = TimestampT::GetDateTimePart(right, TimeUnit::kDay);
            break;
        case TimeUnit::kHour:
            result = TimestampT::GetDateTimePart(right, TimeUnit::kHour);
            break;
        case TimeUnit::kMinute:
            result = TimestampT::GetDateTimePart(right, TimeUnit::kMinute);
            break;
        case TimeUnit::kSecond:
            result = TimestampT::GetDateTimePart(right, TimeUnit::kSecond);
            break;
        default:
            Status status = Status::InvalidParameterValue("Time Unit", input, "Time Unit Value is invalid.");
            RecoverableError(status);
            return false;
    }
    return true;
}

void RegisterDatePartFunction(NewCatalog *catalog_ptr) {
    String func_name = "datepart";
    DatePartFunction::InitDatePartFunction();
    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction date_part_date_function(func_name,
                                  {DataType(LogicalType::kVarchar), DataType(LogicalType::kDate)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::BinaryFunctionWithFailure<VarcharT, DateT, BigIntT, DatePartFunction>);
    function_set_ptr->AddFunction(date_part_date_function);

    ScalarFunction date_part_datetime_function(func_name,
                                  {DataType(LogicalType::kVarchar), DataType(LogicalType::kDateTime)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::BinaryFunctionWithFailure<VarcharT, DateTimeT, BigIntT, DatePartFunction>);
    function_set_ptr->AddFunction(date_part_datetime_function);

    ScalarFunction date_part_timestamp_function(func_name,
                                  {DataType(LogicalType::kVarchar), DataType(LogicalType::kTimestamp)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::BinaryFunctionWithFailure<VarcharT, TimestampT, BigIntT, DatePartFunction>);
    function_set_ptr->AddFunction(date_part_timestamp_function);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity