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
#include "parser_assert.h"
#include <string>

namespace infinity {

// Time format: HH:MM:SS
struct TimeType {
    friend struct DateTimeType;

    TimeType() = default;

    explicit constexpr TimeType(int32_t time_value) : value(time_value){};

    inline int32_t GetValue() const { return value; }

    operator int32_t() const { return value; }

    inline void FromString(const std::string_view &time_str) { FromString(time_str.data(), time_str.size()); }

    void FromString(const char *time_ptr, size_t length);

    [[nodiscard]] std::string ToString() const;

    inline bool operator==(const TimeType &other) const { return this->value == other.value; }

    inline bool operator>=(const TimeType &other) const { return this->value >= other.value; }

    inline bool operator>(const TimeType &other) const { return this->value > other.value; }

    inline bool operator<=(const TimeType &other) const { return this->value <= other.value; }

    inline bool operator<(const TimeType &other) const { return this->value < other.value; }

    static bool Add(TimeType input, IntervalType interval, TimeType &output);

    static bool Subtract(TimeType input, IntervalType interval, TimeType &output);

    static bool Add(TimeType input, IntervalType interval, TimeType &output, int32_t &overflow_days);

    static bool Subtract(TimeType input, IntervalType interval, TimeType &output, int32_t &overflow_days);

    static int64_t GetTimePart(TimeType input, TimeUnit unit);

private:
    static bool ConvertFromString(const char *date_ptr, size_t length, TimeType &time);

    static bool HMS2Time(int32_t hour, int32_t minute, int32_t second, TimeType &time);

    static bool Time2HMS(int32_t milliseconds, int32_t &hour, int32_t &minute, int32_t &second);

    static bool IsTimeValid(int32_t hour, int32_t minute, int32_t second);

public:
    // value means the number of seconds since midnight in the range 0 to 86399
    int32_t value{};
};

} // namespace infinity
