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

#include "date_type.h"
#include "parser_assert.h"
#include "time_type.h"
#include <string>

namespace infinity {

struct DateTimeType {
    DateTimeType() = default;

    // time_value: seconds since 00:00:00
    // date_value: days since 1970-01-01
    explicit constexpr DateTimeType(int32_t date_value, int32_t time_value) : date(date_value), time(time_value){};

    // epoch_time: seconds since 1970-01-01 00:00:00
    explicit constexpr DateTimeType(int64_t epoch_time) {
        constexpr int64_t TotalSecondsInDay = 24 * 60 * 60;
        int64_t seconds = epoch_time % TotalSecondsInDay;
        if (seconds < 0) {
            seconds += TotalSecondsInDay;
        }
        date = DateType(static_cast<int32_t>((epoch_time - seconds) / TotalSecondsInDay));
        time = TimeType(static_cast<int32_t>(seconds));
    }

    inline void Reset() {
        date = {};
        time = {};
    }

    inline void FromString(const std::string_view &datetime_str) { FromString(datetime_str.data(), datetime_str.size()); }

    // NOTICE: datetime is in format "YYYY-MM-DD HH:MM:SS"
    void FromString(const char *datetime_ptr, size_t length);

    [[nodiscard]] inline std::string ToString() const { return date.ToString() + " " + time.ToString(); }

    inline bool operator==(const DateTimeType &other) const { return this->date == other.date && this->time == other.time; }

    inline bool operator>=(const DateTimeType &other) const {
        return this->date > other.date || (this->date == other.date && this->time >= other.time);
    }

    inline bool operator>(const DateTimeType &other) const {
        return this->date > other.date || (this->date == other.date && this->time > other.time);
    }

    inline bool operator<=(const DateTimeType &other) const {
        return this->date < other.date || (this->date == other.date && this->time <= other.time);
    }

    inline bool operator<(const DateTimeType &other) const {
        return this->date < other.date || (this->date == other.date && this->time < other.time);
    }

    static bool Add(DateTimeType input, IntervalType interval, DateTimeType &output);

    static bool Subtract(DateTimeType input, IntervalType interval, DateTimeType &output);

    static int64_t GetDateTimePart(DateTimeType input, TimeUnit unit);

    int64_t GetEpochTime() const;

private:
    static bool YMDHMS2DateTime(int32_t year, int32_t month, int32_t day, int32_t hour, int32_t minute, int32_t second, DateTimeType &datetime);

    static bool
    DateTime2YMDHMS(int32_t days, int32_t seconds, int32_t &year, int32_t &month, int32_t &day, int32_t &hour, int32_t &minute, int32_t &second);

    static bool IsDateTimeValid(int32_t year, int32_t month, int32_t day, int32_t hour, int32_t minute, int32_t second);

public:
    DateType date{};
    TimeType time{};
};

} // namespace infinity
