#pragma once

#include "interval_type.h"
#include "parser_assert.h"
#include <string>

namespace infinity {

// Time format: HH:MM:SS
struct TimeTypeStd {
    friend struct DateTimeTypeStd;

    TimeTypeStd() = default;

    explicit constexpr TimeTypeStd(int32_t time_value) : value(time_value){};

    inline int32_t GetValue() const { return value; }

    operator int32_t() const { return value; }

    inline void FromString(const std::string_view &time_str) { FromString(time_str.data(), time_str.size()); }

    void FromString(const char *time_ptr, size_t length);

    [[nodiscard]] std::string ToString() const;

    inline bool operator==(const TimeTypeStd &other) const { return this->value == other.value; }

    inline bool operator>=(const TimeTypeStd &other) const { return this->value >= other.value; }

    inline bool operator>(const TimeTypeStd &other) const { return this->value > other.value; }

    inline bool operator<=(const TimeTypeStd &other) const { return this->value <= other.value; }

    inline bool operator<(const TimeTypeStd &other) const { return this->value < other.value; }

    static bool Add(TimeTypeStd input, IntervalType interval, TimeTypeStd &output);

    static bool Subtract(TimeTypeStd input, IntervalType interval, TimeTypeStd &output);

    static bool Add(TimeTypeStd input, IntervalType interval, TimeTypeStd &output, int32_t &overflow_days);

    static bool Subtract(TimeTypeStd input, IntervalType interval, TimeTypeStd &output, int32_t &overflow_days);

    static int64_t GetTimePart(TimeTypeStd input, TimeUnit unit);

private:
    static bool ConvertFromString(const char *date_ptr, size_t length, TimeTypeStd &time);

    static bool HMS2Time(int32_t hour, int32_t minute, int32_t second, TimeTypeStd &time);

    static bool Time2HMS(int32_t milliseconds, int32_t &hour, int32_t &minute, int32_t &second);

    static bool IsTimeValid(int32_t hour, int32_t minute, int32_t second);

public:
    // value means the number of seconds since midnight in the range 0 to 86399
    int32_t value{};
};

}