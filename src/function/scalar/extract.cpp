// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

import stl;
import new_catalog;

import infinity_exception;
import scalar_function;
import scalar_function_set;
import parser;
import third_party;

module extract;

namespace infinity {

struct ExtractYearFunction {
    template <typename TA, typename TB>
    static inline void Run(TA input, TB &result) {
        Error<NotImplementException>("ExtractYear function isn't implemented");
    }
};

template <>
inline void ExtractYearFunction::Run(DateT left, BigIntT &result) {
    result = DateT::GetDatePart(left, TimeUnit::kYear);
}

struct ExtractMonthFunction {
    template <typename TA, typename TB>
    static inline void Run(TA input, TB &result) {
        Error<NotImplementException>("ExtractMonth function isn't implemented");
    }
};

template <>
inline void ExtractMonthFunction::Run(DateT left, BigIntT &result) {
    result = DateT::GetDatePart(left, TimeUnit::kMonth);
}

struct ExtractDayFunction {
    template <typename TA, typename TB>
    static inline void Run(TA input, TB &result) {
        Error<NotImplementException>("ExtractDay function isn't implemented");
    }
};

template <>
inline void ExtractDayFunction::Run(DateT left, BigIntT &result) {
    result = DateT::GetDatePart(left, TimeUnit::kDay);
}

void RegisterExtractFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    {
        String func_name = "extract_year";
        SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);
        ScalarFunction extract_year_from_date(func_name,
                                              {DataType(LogicalType::kDate)},
                                              DataType(kBigInt),
                                              &ScalarFunction::UnaryFunction<DateT, BigIntT, ExtractYearFunction>);
        function_set_ptr->AddFunction(extract_year_from_date);
        NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
    }

    {
        String func_name = "extract_month";
        SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);
        ScalarFunction extract_month_from_date(func_name,
                                               {DataType(LogicalType::kDate)},
                                               DataType(kBigInt),
                                               &ScalarFunction::UnaryFunction<DateT, BigIntT, ExtractMonthFunction>);
        function_set_ptr->AddFunction(extract_month_from_date);
        NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
    }

    {
        String func_name = "extract_day";
        SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);
        ScalarFunction extract_day_from_date(func_name,
                                             {DataType(LogicalType::kDate)},
                                             DataType(kBigInt),
                                             &ScalarFunction::UnaryFunction<DateT, BigIntT, ExtractDayFunction>);
        function_set_ptr->AddFunction(extract_day_from_date);
        NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
    }
}

} // namespace infinity
