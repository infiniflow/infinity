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

#include "datetime_type.h"

namespace infinity {

void DateTimeType::FromString(const char *datetime_ptr, size_t length) {
    // NOTICE: datetime is in format "YYYY-MM-DD HH:MM:SS"
    size_t date_length;
    date.FromString(datetime_ptr, length, date_length);
    // skip space
    while (date_length < length && datetime_ptr[date_length] == ' ') {
        ++date_length;
    }
    if (date_length < length) {
        time.FromString(datetime_ptr + date_length, length - date_length);
    } else {
        time = {};
    }
}

bool DateTimeType::Add(DateTimeType input, IntervalType interval, DateTimeType &output) {
    switch (interval.unit) {
        case kYear:
        case kMonth:
        case kDay: {
            DateType::Add(input.date, interval, input.date);
            output = input;
            return true;
        }
        case kHour:
        case kMinute:
        case kSecond: {
            int32_t overflow_days{};
            TimeType::Add(input.time, interval, input.time, overflow_days);
            if (overflow_days) {
                IntervalType days(overflow_days);
                days.unit = TimeUnit::kDay;
                DateType::Add(input.date, days, input.date);
            }
            output = input;
            return true;
        }
        case kInvalidUnit: {
            ParserError("Invalid interval unit.");
        }
    }
    return false;
}

bool DateTimeType::Subtract(DateTimeType input, IntervalType interval, DateTimeType &output) {
    interval.value = -interval.value;
    return Add(input, interval, output);
}

int64_t DateTimeType::GetDateTimePart(DateTimeType input, TimeUnit unit) {
    switch (unit) {
        case TimeUnit::kYear:
        case TimeUnit::kMonth:
        case TimeUnit::kDay: {
            return DateType::GetDatePart(input.date, unit);
        }
        case TimeUnit::kHour:
        case TimeUnit::kMinute:
        case TimeUnit::kSecond: {
            return TimeType::GetTimePart(input.time, unit);
        }
        default: {
            ParserError("Invalid time unit");
        }
    }
    return -1;
}

int64_t DateTimeType::GetEpochTime(const DateTimeType &dt) {
    constexpr int32_t TotalSecondsInDay = 24 * 60 * 60;
    return dt.date.value * TotalSecondsInDay + dt.time.value;
}

bool DateTimeType::YMDHMS2DateTime(int32_t year, int32_t month, int32_t day, int32_t hour, int32_t minute, int32_t second, DateTimeType &datetime) {
    return TimeType::HMS2Time(hour, minute, second, datetime.time) and DateType::YMD2Date(year, month, day, datetime.date);
}

bool DateTimeType::DateTime2YMDHMS(int32_t days,
                                   int32_t seconds,
                                   int32_t &year,
                                   int32_t &month,
                                   int32_t &day,
                                   int32_t &hour,
                                   int32_t &minute,
                                   int32_t &second) {
    return TimeType::Time2HMS(seconds, hour, minute, second) and DateType::Date2YMD(days, year, month, day);
}

bool DateTimeType::IsDateTimeValid(int32_t year, int32_t month, int32_t day, int32_t hour, int32_t minute, int32_t second) {
    return TimeType::IsTimeValid(hour, minute, second) and DateType::IsDateValid(year, month, day);
}

} // namespace infinity