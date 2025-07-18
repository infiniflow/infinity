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

#include "time_type.h"
#include "spdlog/fmt/fmt.h"

namespace infinity {
// min time: 00:00:00.000
constexpr static int32_t MIN_TIME_HOUR = 0;
constexpr static int32_t MIN_TIME_MINUTE = 0;
constexpr static int32_t MIN_TIME_SECOND = 0;
// constexpr static int32_t MIN_TIME_MILLISECOND = 0;

// max time: 23:59:59.999
constexpr static int32_t MAX_TIME_HOUR = 23;
constexpr static int32_t MAX_TIME_MINUTE = 59;
constexpr static int32_t MAX_TIME_SECOND = 59;
// constexpr static int32_t MAX_TIME_MILLISECOND = 999;

// seconds
constexpr static int32_t SECOND_SECOND = 1;
constexpr static int32_t MINUTE_SECOND = 60;
constexpr static int32_t HOUR_SECOND = 60 * MINUTE_SECOND;
constexpr static int32_t DAY_SECOND = 24 * HOUR_SECOND;

// deprecated milliseconds
// constexpr static int32_t SECOND_MILLI = 1000;
// constexpr static int32_t MINUTE_MILLI = 60 * SECOND_MILLI;
// constexpr static int32_t HOUR_MILLI = 60 * MINUTE_MILLI;
// constexpr static int32_t DAY_MILLI = 24 * HOUR_MILLI;

void TimeType::FromString(const char *time_ptr, size_t length) {
    if (!ConvertFromString(time_ptr, length, *this)) {
        ParserError("Invalid time format (need to be HH:MM:SS).");
    }
}

std::string TimeType::ToString() const {
    int32_t hour{}, minute{}, second{};
    if (!Time2HMS(value, hour, minute, second)) {
        ParserError(fmt::format("Invalid second value: {}", value));
    }
    return fmt::format("{:02d}:{:02d}:{:02d}", hour, minute, second);
}

bool TimeType::Add(TimeType input, IntervalType interval, TimeType &output) {
    auto calc_time_add = [&](int32_t interval_product_seconds) {
        auto tot = input.value + interval.value * interval_product_seconds;
        output.value = ((tot % DAY_SECOND) + DAY_SECOND) % DAY_SECOND;
    };
    switch (interval.unit) {
        case kYear:
        case kMonth:
        case kDay: {
            output = input;
            return true;
        }
        case kHour: {
            calc_time_add(HOUR_SECOND);
            return true;
        }
        case kMinute: {
            calc_time_add(MINUTE_SECOND);
            return true;
        }
        case kSecond: {
            calc_time_add(SECOND_SECOND);
            return true;
        }
        case kInvalidUnit: {
            ParserError("Invalid interval unit.");
        }
    }
    return false;
}

bool TimeType::Subtract(TimeType input, IntervalType interval, TimeType &output) {
    interval.value = -interval.value;
    return Add(input, interval, output);
}

bool TimeType::Add(TimeType input, IntervalType interval, TimeType &output, int32_t &overflow_days) {
    auto calc_overflow_days = [&](int32_t interval_product_seconds) {
        int32_t tot = input.value + interval.value * interval_product_seconds;
        int32_t res = ((tot % DAY_SECOND) + DAY_SECOND) % DAY_SECOND;
        overflow_days = (tot - res) / DAY_SECOND;
        // now tot == overflow_days * DAY_SECOND + res
        output.value = res;
    };
    switch (interval.unit) {
        case kYear:
        case kMonth:
        case kDay: {
            output = input;
            return true;
        }
        case kHour: {
            calc_overflow_days(HOUR_SECOND);
            return true;
        }
        case kMinute: {
            calc_overflow_days(MINUTE_SECOND);
            return true;
        }
        case kSecond: {
            calc_overflow_days(SECOND_SECOND);
            return true;
        }
        case kInvalidUnit: {
            ParserError("Invalid interval unit.");
        }
    }
    return false;
}

bool TimeType::Subtract(TimeType input, IntervalType interval, TimeType &output, int32_t &overflow_days) {
    interval.value = -interval.value;
    return Add(input, interval, output, overflow_days);
}

int64_t TimeType::GetTimePart(TimeType input, TimeUnit unit) {
    int32_t hour{}, minute{}, second{};
    auto result = Time2HMS(input.value, hour, minute, second);
    ParserAssert(result, "Invalid time value");
    switch (unit) {
        case TimeUnit::kYear: {
            ParserError("Can't extract year from time");
        }
        case TimeUnit::kMonth: {
            ParserError("Can't extract month from time");
        }
        case TimeUnit::kDay: {
            ParserError("Can't extract day from time");
        }
        case TimeUnit::kHour: {
            return hour;
        }
        case TimeUnit::kMinute: {
            return minute;
        }
        case TimeUnit::kSecond: {
            return second;
        }
        default: {
            ParserError("Invalid time unit");
        }
    }
    return -1;
}

bool TimeType::ConvertFromString(const char *time_ptr, size_t length, TimeType &time) {
    int32_t hour{}, minute{}, second{};
    // trim the string
    size_t pos{0};
    // skip spaces
    while (pos < length && std::isspace(time_ptr[pos])) {
        ++pos;
    }
    // Get hour
    while (pos < length && std::isdigit(time_ptr[pos])) {
        if (std::isspace(time_ptr[pos])) {
            ++pos;
            continue;
        }
        if (std::isdigit(time_ptr[pos])) {
            hour = hour * 10 + (time_ptr[pos] - '0');
            ++pos;
            if (hour > MAX_TIME_HOUR)
                return false;
            continue;
        }
        break;
    }
    if (time_ptr[pos] != ':') {
        return false;
    }
    ++pos; // skip :
    // Get minute
    while (pos < length && std::isdigit(time_ptr[pos])) {
        if (std::isspace(time_ptr[pos])) {
            ++pos;
            continue;
        }
        if (std::isdigit(time_ptr[pos])) {
            minute = minute * 10 + (time_ptr[pos] - '0');
            ++pos;
            if (minute > MAX_TIME_MINUTE)
                return false;
            continue;
        }
        break;
    }
    if (time_ptr[pos] != ':') {
        return false;
    }
    ++pos; // skip :
    // Get second
    while (pos < length && std::isdigit(time_ptr[pos])) {
        if (std::isspace(time_ptr[pos])) {
            ++pos;
            continue;
        }
        if (std::isdigit(time_ptr[pos])) {
            second = second * 10 + (time_ptr[pos] - '0');
            ++pos;
            if (second > MAX_TIME_SECOND)
                return false;
            continue;
        }
        break;
    }
    return HMS2Time(hour, minute, second, time);
}

bool TimeType::HMS2Time(int32_t hour, int32_t minute, int32_t second, TimeType &time) {
    if (!IsTimeValid(hour, minute, second)) {
        return false;
    }
    time.value = hour * HOUR_SECOND + minute * MINUTE_SECOND + second * SECOND_SECOND;
    return true;
}

bool TimeType::Time2HMS(int32_t seconds, int32_t &hour, int32_t &minute, int32_t &second) {
    if (seconds < 0 || seconds >= DAY_SECOND) {
        return false;
    }
    hour = seconds / HOUR_SECOND;
    seconds = seconds % HOUR_SECOND;
    minute = seconds / MINUTE_SECOND;
    second = seconds % MINUTE_SECOND;
    return true;
}

bool TimeType::IsTimeValid(int32_t hour, int32_t minute, int32_t second) {
    return hour >= MIN_TIME_HOUR && hour <= MAX_TIME_HOUR && minute >= MIN_TIME_MINUTE && minute <= MAX_TIME_MINUTE && second >= MIN_TIME_SECOND &&
           second <= MAX_TIME_SECOND;
}

} // namespace infinity