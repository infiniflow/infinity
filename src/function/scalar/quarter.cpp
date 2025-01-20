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
module quarter;
import stl;
import catalog;
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

struct QuarterFunction {
    template <typename TA, typename TB>
    static inline bool Run(TA left, TB &result) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
        return false;
    }

};

template <>
inline bool QuarterFunction::Run(DateT left, BigIntT &result) {
    auto month = DateT::GetDatePart(left, TimeUnit::kMonth);
    switch (month) {
        case 1:
        case 2:
            result = 4;
            break;
        case 3:
        case 4:
        case 5:
            result = 1;
            break;
        case 6:
        case 7:
        case 8:
            result = 2;
            break;
        case 9:
        case 10:
        case 11:
            result = 3;
            break;
        case 12:
            result = 4;
            break;
        default:
            Status status = Status::InvalidParameterValue("Date", "month", "Your month is invalid");
            RecoverableError(status);
            return false;
    }
    return true;
}

template <>
inline bool QuarterFunction::Run(DateTimeT left, BigIntT &result) {
    auto month  = DateTimeT::GetDateTimePart(left, TimeUnit::kMonth);
    switch (month) {
        case 1:
        case 2:
            result = 4;
            break;
        case 3:
        case 4:
        case 5:
            result = 1;
            break;
        case 6:
        case 7:
        case 8:
            result = 2;
            break;
        case 9:
        case 10:
        case 11:
            result = 3;
            break;
        case 12:
            result = 4;
            break;
        default:
            Status status = Status::InvalidParameterValue("Datetime", "month", "Your month is invalid");
            RecoverableError(status);
            return false;
    }
    return true;
}

template <>
inline bool QuarterFunction::Run(TimestampT left, BigIntT &result) {
    auto month = TimestampT::GetDateTimePart(left, TimeUnit::kMonth);
    switch (month) {
        case 1:
        case 2:
            result = 4;
            break;
        case 3:
        case 4:
        case 5:
            result = 1;
            break;
        case 6:
        case 7:
        case 8:
            result = 2;
            break;
        case 9:
        case 10:
        case 11:
            result = 3;
            break;
        case 12:
            result = 4;
            break;
        default:
            Status status = Status::InvalidParameterValue("TimeStamp", "month", "Your month is invalid");
            RecoverableError(status);
            return false;
    }
    return true;
}

void RegisterQuarterFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "quarter";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction quarter_date_function(func_name,
                                  {DataType(LogicalType::kDate)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::UnaryFunctionWithFailure<DateT, BigIntT, QuarterFunction>);
    function_set_ptr->AddFunction(quarter_date_function);

    ScalarFunction quarter_datetime_function(func_name,
                                  {DataType(LogicalType::kDateTime)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::UnaryFunctionWithFailure<DateTimeT, BigIntT, QuarterFunction>);
    function_set_ptr->AddFunction(quarter_datetime_function);

    ScalarFunction quarter_timestamp_function(func_name,
                                  {DataType(LogicalType::kTimestamp)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::UnaryFunctionWithFailure<TimestampT, BigIntT, QuarterFunction>);
    function_set_ptr->AddFunction(quarter_timestamp_function);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity