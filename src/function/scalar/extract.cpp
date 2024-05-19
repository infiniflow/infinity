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

module extract;

import stl;
import catalog;
import status;
import infinity_exception;
import scalar_function;
import scalar_function_set;

import third_party;
import logical_type;
import internal_types;
import data_type;
import logger;

namespace infinity {

struct ExtractYearFunction {
    template <typename TA, typename TB>
    static inline void Run(TA, TB &) {
        Status status = Status::NotSupport("ExtractYear function isn't implemented");
        LOG_ERROR(status.message());
        RecoverableError(status);
    }
};

template <>
inline void ExtractYearFunction::Run(DateT left, BigIntT &result) {
    result = DateT::GetDatePart(left, TimeUnit::kYear);
}

template <>
inline void ExtractYearFunction::Run(DateTimeT left, BigIntT &result) {
    result = DateTimeT::GetDateTimePart(left, TimeUnit::kYear);
}

template <>
inline void ExtractYearFunction::Run(TimestampT left, BigIntT &result) {
    result = TimestampT::GetDateTimePart(left, TimeUnit::kYear);
}

struct ExtractMonthFunction {
    template <typename TA, typename TB>
    static inline void Run(TA, TB &) {
        Status status = Status::NotSupport("ExtractMonth function isn't implemented");
        LOG_ERROR(status.message());
        RecoverableError(status);
    }
};

template <>
inline void ExtractMonthFunction::Run(DateT left, BigIntT &result) {
    result = DateT::GetDatePart(left, TimeUnit::kMonth);
}

template <>
inline void ExtractMonthFunction::Run(DateTimeT left, BigIntT &result) {
    result = DateTimeT::GetDateTimePart(left, TimeUnit::kMonth);
}

template <>
inline void ExtractMonthFunction::Run(TimestampT left, BigIntT &result) {
    result = TimestampT::GetDateTimePart(left, TimeUnit::kMonth);
}

struct ExtractDayFunction {
    template <typename TA, typename TB>
    static inline void Run(TA, TB &) {
        Status status = Status::NotSupport("ExtractDay function isn't implemented");
        LOG_ERROR(status.message());
        RecoverableError(status);
    }
};

template <>
inline void ExtractDayFunction::Run(DateT left, BigIntT &result) {
    result = DateT::GetDatePart(left, TimeUnit::kDay);
}

template <>
inline void ExtractDayFunction::Run(DateTimeT left, BigIntT &result) {
    result = DateTimeT::GetDateTimePart(left, TimeUnit::kDay);
}

template <>
inline void ExtractDayFunction::Run(TimestampT left, BigIntT &result) {
    result = TimestampT::GetDateTimePart(left, TimeUnit::kDay);
}

struct ExtractHourFunction {
    template <typename TA, typename TB>
    static inline void Run(TA, TB &) {
        Status status = Status::NotSupport("ExtractHour function isn't implemented");
        LOG_ERROR(status.message());
        RecoverableError(status);
    }
};

template <>
inline void ExtractHourFunction::Run(DateTimeT left, BigIntT &result) {
    result = DateTimeT::GetDateTimePart(left, TimeUnit::kHour);
}

template <>
inline void ExtractHourFunction::Run(TimestampT left, BigIntT &result) {
    result = TimestampT::GetDateTimePart(left, TimeUnit::kHour);
}

template <>
inline void ExtractHourFunction::Run(TimeT left, BigIntT &result) {
    result = TimeT::GetTimePart(left, TimeUnit::kHour);
}

struct ExtractMinuteFunction {
    template <typename TA, typename TB>
    static inline void Run(TA, TB &) {
        Status status = Status::NotSupport("ExtractMinute function isn't implemented");
        LOG_ERROR(status.message());
        RecoverableError(status);
    }
};

template <>
inline void ExtractMinuteFunction::Run(DateTimeT left, BigIntT &result) {
    result = DateTimeT::GetDateTimePart(left, TimeUnit::kMinute);
}

template <>
inline void ExtractMinuteFunction::Run(TimestampT left, BigIntT &result) {
    result = TimestampT::GetDateTimePart(left, TimeUnit::kMinute);
}

template <>
inline void ExtractMinuteFunction::Run(TimeT left, BigIntT &result) {
    result = TimeT::GetTimePart(left, TimeUnit::kMinute);
}

struct ExtractSecondFunction {
    template <typename TA, typename TB>
    static inline void Run(TA, TB &) {
        Status status = Status::NotSupport("ExtractSecond function isn't implemented");
        LOG_ERROR(status.message());
        RecoverableError(status);
    }
};

template <>
inline void ExtractSecondFunction::Run(DateTimeT left, BigIntT &result) {
    result = DateTimeT::GetDateTimePart(left, TimeUnit::kSecond);
}

template <>
inline void ExtractSecondFunction::Run(TimestampT left, BigIntT &result) {
    result = TimestampT::GetDateTimePart(left, TimeUnit::kSecond);
}

template <>
inline void ExtractSecondFunction::Run(TimeT left, BigIntT &result) {
    result = TimeT::GetTimePart(left, TimeUnit::kSecond);
}

void RegisterExtractFunction(const UniquePtr<Catalog> &catalog_ptr) {
    {
        String func_name = "extract_year";
        SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);
        ScalarFunction extract_year_from_date(func_name,
                                              {DataType(LogicalType::kDate)},
                                              DataType(kBigInt),
                                              &ScalarFunction::UnaryFunction<DateT, BigIntT, ExtractYearFunction>);
        function_set_ptr->AddFunction(extract_year_from_date);
        ScalarFunction extract_year_from_datetime(func_name,
                                                  {DataType(LogicalType::kDateTime)},
                                                  DataType(kBigInt),
                                                  &ScalarFunction::UnaryFunction<DateTimeT, BigIntT, ExtractYearFunction>);
        function_set_ptr->AddFunction(extract_year_from_datetime);
        ScalarFunction extract_year_from_timestamp(func_name,
                                                   {DataType(LogicalType::kTimestamp)},
                                                   DataType(kBigInt),
                                                   &ScalarFunction::UnaryFunction<TimestampT, BigIntT, ExtractYearFunction>);
        function_set_ptr->AddFunction(extract_year_from_timestamp);
        Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
    }

    {
        String func_name = "extract_month";
        SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);
        ScalarFunction extract_month_from_date(func_name,
                                               {DataType(LogicalType::kDate)},
                                               DataType(kBigInt),
                                               &ScalarFunction::UnaryFunction<DateT, BigIntT, ExtractMonthFunction>);
        function_set_ptr->AddFunction(extract_month_from_date);
        ScalarFunction extract_month_from_datetime(func_name,
                                                   {DataType(LogicalType::kDateTime)},
                                                   DataType(kBigInt),
                                                   &ScalarFunction::UnaryFunction<DateTimeT, BigIntT, ExtractMonthFunction>);
        function_set_ptr->AddFunction(extract_month_from_datetime);
        ScalarFunction extract_month_from_timestamp(func_name,
                                                    {DataType(LogicalType::kTimestamp)},
                                                    DataType(kBigInt),
                                                    &ScalarFunction::UnaryFunction<TimestampT, BigIntT, ExtractMonthFunction>);
        function_set_ptr->AddFunction(extract_month_from_timestamp);
        Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
    }

    {
        String func_name = "extract_day";
        SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);
        ScalarFunction extract_day_from_date(func_name,
                                             {DataType(LogicalType::kDate)},
                                             DataType(kBigInt),
                                             &ScalarFunction::UnaryFunction<DateT, BigIntT, ExtractDayFunction>);
        function_set_ptr->AddFunction(extract_day_from_date);
        ScalarFunction extract_day_from_datetime(func_name,
                                                 {DataType(LogicalType::kDateTime)},
                                                 DataType(kBigInt),
                                                 &ScalarFunction::UnaryFunction<DateTimeT, BigIntT, ExtractDayFunction>);
        function_set_ptr->AddFunction(extract_day_from_datetime);
        ScalarFunction extract_day_from_timestamp(func_name,
                                                  {DataType(LogicalType::kTimestamp)},
                                                  DataType(kBigInt),
                                                  &ScalarFunction::UnaryFunction<TimestampT, BigIntT, ExtractDayFunction>);
        function_set_ptr->AddFunction(extract_day_from_timestamp);
        Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
    }

    {
        String func_name = "extract_hour";
        SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);
        ScalarFunction extract_hour_from_datetime(func_name,
                                                  {DataType(LogicalType::kDateTime)},
                                                  DataType(kBigInt),
                                                  &ScalarFunction::UnaryFunction<DateTimeT, BigIntT, ExtractHourFunction>);
        function_set_ptr->AddFunction(extract_hour_from_datetime);
        ScalarFunction extract_hour_from_timestamp(func_name,
                                                   {DataType(LogicalType::kTimestamp)},
                                                   DataType(kBigInt),
                                                   &ScalarFunction::UnaryFunction<TimestampT, BigIntT, ExtractHourFunction>);
        function_set_ptr->AddFunction(extract_hour_from_timestamp);
        ScalarFunction extract_hour_from_time(func_name,
                                              {DataType(LogicalType::kTime)},
                                              DataType(kBigInt),
                                              &ScalarFunction::UnaryFunction<TimeT, BigIntT, ExtractHourFunction>);
        function_set_ptr->AddFunction(extract_hour_from_time);
        Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
    }

    {
        String func_name = "extract_minute";
        SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);
        ScalarFunction extract_minute_from_datetime(func_name,
                                                    {DataType(LogicalType::kDateTime)},
                                                    DataType(kBigInt),
                                                    &ScalarFunction::UnaryFunction<DateTimeT, BigIntT, ExtractMinuteFunction>);
        function_set_ptr->AddFunction(extract_minute_from_datetime);
        ScalarFunction extract_minute_from_timestamp(func_name,
                                                     {DataType(LogicalType::kTimestamp)},
                                                     DataType(kBigInt),
                                                     &ScalarFunction::UnaryFunction<TimestampT, BigIntT, ExtractMinuteFunction>);
        function_set_ptr->AddFunction(extract_minute_from_timestamp);
        ScalarFunction extract_minute_from_time(func_name,
                                                {DataType(LogicalType::kTime)},
                                                DataType(kBigInt),
                                                &ScalarFunction::UnaryFunction<TimeT, BigIntT, ExtractMinuteFunction>);
        function_set_ptr->AddFunction(extract_minute_from_time);
        Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
    }

    {
        String func_name = "extract_second";
        SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);
        ScalarFunction extract_second_from_datetime(func_name,
                                                    {DataType(LogicalType::kDateTime)},
                                                    DataType(kBigInt),
                                                    &ScalarFunction::UnaryFunction<DateTimeT, BigIntT, ExtractSecondFunction>);
        function_set_ptr->AddFunction(extract_second_from_datetime);
        ScalarFunction extract_second_from_timestamp(func_name,
                                                     {DataType(LogicalType::kTimestamp)},
                                                     DataType(kBigInt),
                                                     &ScalarFunction::UnaryFunction<TimestampT, BigIntT, ExtractSecondFunction>);
        function_set_ptr->AddFunction(extract_second_from_timestamp);
        ScalarFunction extract_second_from_time(func_name,
                                                {DataType(LogicalType::kTime)},
                                                DataType(kBigInt),
                                                &ScalarFunction::UnaryFunction<TimeT, BigIntT, ExtractSecondFunction>);
        function_set_ptr->AddFunction(extract_second_from_time);
        Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
    }
}

} // namespace infinity
