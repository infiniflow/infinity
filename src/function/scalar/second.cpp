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

struct SecondFunction {
    template <typename TA, typename TB>
    static inline bool Run(TA left, TB &result) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
        return false;
    }

};

template <>
inline bool SecondFunction::Run(DateTimeT left, BigIntT &result) {
    result = DateTimeT::GetDateTimePart(left, TimeUnit::kSecond);
    return true;
}

template <>
inline bool SecondFunction::Run(TimeT left, BigIntT &result) {
    result = TimeT::GetTimePart(left, TimeUnit::kSecond);
    return true;
}

template <>
inline bool SecondFunction::Run(TimestampT left, BigIntT &result) {
    result = TimestampT::GetDateTimePart(left, TimeUnit::kSecond);
    return true;
}

void RegisterSecondFunction(NewCatalog *catalog_ptr) {
    String func_name = "second";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction second_datetime_function(func_name,
                                  {DataType(LogicalType::kDateTime)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::UnaryFunctionWithFailure<DateTimeT, BigIntT, SecondFunction>);
    function_set_ptr->AddFunction(second_datetime_function);

    ScalarFunction second_time_function(func_name,
                                  {DataType(LogicalType::kTime)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::UnaryFunctionWithFailure<TimeT, BigIntT, SecondFunction>);
    function_set_ptr->AddFunction(second_time_function);


    ScalarFunction second_timestamp_function(func_name,
                                  {DataType(LogicalType::kTimestamp)},
                                  {DataType(LogicalType::kBigInt)},
                                  &ScalarFunction::UnaryFunctionWithFailure<TimestampT, BigIntT, SecondFunction>);
    function_set_ptr->AddFunction(second_timestamp_function);


    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity