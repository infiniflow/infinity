//
// Created by JinHai on 2022/11/7.
//

#pragma once

#include "common/types/alias/primitives.h"
#include "common/types/alias/strings.h"
#include "common/utility/infinity_assert.h"
#include "interval_type.h"

namespace infinity {

struct DateType {
    DateType() = default;

    explicit DateType(i32 date_value) : value(date_value){};

    inline void FromString(const String &date_str) { FromString(date_str.c_str(), date_str.length()); }

    void FromString(const char *date, SizeT length);

    [[nodiscard]] String ToString() const;

    i32 value{0};

private:
    static bool ConvertFromString(const char *date_ptr, SizeT length, DateType &date);

    static bool YMD2Date(i32 year, i32 month, i32 day, DateType &date);

    static bool Date2YMD(i32 days, i32 &year, i32 &month, i32 &day);

    inline static bool IsLeapYear(i32 year) { return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0); }

    static bool IsDateValid(i32 year, i32 month, i32 day);

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

    static i64 GetDatePart(DateType input, TimeUnit unit);
};

} // namespace infinity