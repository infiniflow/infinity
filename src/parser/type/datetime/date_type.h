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

#pragma once

#include "interval_type.h"
#include <string>

namespace infinity {

struct DateType {
    friend struct DateTimeType;

    DateType() = default;

    explicit constexpr DateType(int32_t date_value) : value(date_value){};

    inline int32_t GetValue() const { return value; }

    operator int32_t() const { return value; }

    inline void FromString(const std::string_view &date_str) { FromString(date_str.data(), date_str.size()); }

    void FromString(const char *date, size_t length);

    void FromString(const char *date, size_t length, size_t &end_length);

    [[nodiscard]] std::string ToString() const;

    int32_t value{0};

private:
    static bool ConvertFromString(const char *date_ptr, size_t length, DateType &date, size_t &end_length);

    static bool YMD2Date(int32_t year, int32_t month, int32_t day, DateType &date);

    static bool Date2YMD(int32_t days, int32_t &year, int32_t &month, int32_t &day);

    inline static bool IsLeapYear(int32_t year) { return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0); }

    static bool IsDateValid(int32_t year, int32_t month, int32_t day);

public:
    inline bool operator==(const DateType &other) const { return this->value == other.value; }

    inline bool operator>=(const DateType &other) const { return this->value >= other.value; }

    inline bool operator>(const DateType &other) const { return this->value > other.value; }

    inline bool operator<=(const DateType &other) const { return this->value <= other.value; }

    inline bool operator<(const DateType &other) const { return this->value < other.value; }

public:
    // Operation
    static bool Add(DateType input, IntervalType interval, DateType &output);

    static bool Subtract(DateType input, IntervalType interval, DateType &output);

    static int64_t GetDatePart(DateType input, TimeUnit unit);
};

} // namespace infinity