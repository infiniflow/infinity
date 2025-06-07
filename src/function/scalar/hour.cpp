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
module hour;
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

struct HourFunction {
    template <typename TA, typename TB>
    static inline bool Run(TA left, TB &result) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
        return false;
    }

};

template <>
inline bool HourFunction::Run(DateTimeT left, BigIntT &result) {
    result = DateTimeT::GetDateTimePart(left, TimeUnit::kHour);
    return true;
}

template <>
inline bool HourFunction::Run(TimeT left, BigIntT &result) {
    result = TimeT::GetTimePart(left, TimeUnit::kHour);
    return true;
}

template <>
inline bool HourFunction::Run(TimestampT left, BigIntT &result) {
    result = TimestampT::GetDateTimePart(left, TimeUnit::kHour);
    return true;
}

void RegisterHourFunction(NewCatalog *catalog_ptr) {
    String func_name = "hour";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction hour_datetime_function(func_name,
                                  {DataType(LogicalType::kDateTime)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::UnaryFunctionWithFailure<DateTimeT, BigIntT, HourFunction>);
    function_set_ptr->AddFunction(hour_datetime_function);

    ScalarFunction hour_time_function(func_name,
                                  {DataType(LogicalType::kTime)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::UnaryFunctionWithFailure<TimeT, BigIntT, HourFunction>);
    function_set_ptr->AddFunction(hour_time_function);

    ScalarFunction hour_timestamp_function(func_name,
                                  {DataType(LogicalType::kTimestamp)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::UnaryFunctionWithFailure<TimestampT, BigIntT, HourFunction>);
    function_set_ptr->AddFunction(hour_timestamp_function);


    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity