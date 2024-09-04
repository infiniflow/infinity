#include "date_type_std.h"
#include "parser_assert.h"
#include <format>
#include <chrono>

constexpr static int32_t DAY_HOUR = 24;
constexpr static int32_t DAY_MINUTE = DAY_HOUR * 60;
constexpr static int32_t DAY_SECOND = DAY_MINUTE * 60;

namespace infinity{

void DateTypeStd::FromString(const char *date_ptr, size_t length) {
    size_t end_length_unused;
    if (!ConvertFromString(date_ptr, length, *this, end_length_unused)) {
        ParserError("Invalid date format (YYYY-MM-DD or YYYY/MM/DD).");
    }
}

void DateTypeStd::FromString(const char *date_ptr, size_t length, size_t &end_length) {
    if (!ConvertFromString(date_ptr, length, *this, end_length)) {
        ParserError("Invalid date format (YYYY-MM-DD or YYYY/MM/DD).");
    }
}

std::string DateTypeStd::ToString() const {
    int32_t year{0}, month{0}, day{0};
    if (!Date2YMD(value, year, month, day)) {
        ParserError(std::format("Invalid date: {}-{}-{}", year, month, day));
    }
    // TODO: format for negative year?
    return std::format("{:04d}-{:02d}-{:02d}", year, month, day);
}

bool DateTypeStd::ConvertFromString(const char *date_ptr, size_t length, DateTypeStd &date, size_t &end_length) {
    // trim the string
    size_t pos{0};

    // skip spaces
    while (pos < length && std::isspace(date_ptr[pos])) {
        ++pos;
    }

    // Get year
    int32_t year{0};
    bool negative_year{false};
    if (date_ptr[pos] == '-') {
        negative_year = true;
        ++pos;
    }
    while (pos < length && std::isdigit(date_ptr[pos])) {
        if (std::isspace(date_ptr[pos])) {
            ++pos;
            continue;
        }
        if (std::isdigit(date_ptr[pos])) {
            year = year * 10 + (date_ptr[pos] - '0');
            ++pos;
            // if (year > 9999)
            //     return false;
            continue;
        }
        break;
    }
    if (negative_year) {
        year = -year;
    }
    if (date_ptr[pos] != '-' && date_ptr[pos] != '/') {
        return false;
    }
    ++pos; // skip - and /

    // Get month
    int32_t month{0};
    while (pos < length && std::isdigit(date_ptr[pos])) {
        if (std::isspace(date_ptr[pos])) {
            ++pos;
            continue;
        }
        if (std::isdigit(date_ptr[pos])) {
            month = month * 10 + (date_ptr[pos] - '0');
            ++pos;
            // if (month > 12)
            //     return false;
            continue;
        }
        break;
    }
    if (date_ptr[pos] != '-' && date_ptr[pos] != '/') {
        return false;
    }
    ++pos; // skip - and /

    // Get day
    int32_t day{0};
    while (pos < length && std::isdigit(date_ptr[pos])) {
        if (std::isspace(date_ptr[pos])) {
            ++pos;
            continue;
        }
        if (std::isdigit(date_ptr[pos])) {
            day = day * 10 + (date_ptr[pos] - '0');
            ++pos;
            // if (day > 31)
            //     return false;
            continue;
        }
        break;
    }
    end_length = pos;

    return YMD2Date(year, month, day, date);
}

bool DateTypeStd::YMD2Date(int32_t year, int32_t month, int32_t day, DateTypeStd &date) { 
    std::chrono::year_month_day ymd (
        std::chrono::year{year},
        std::chrono::month{static_cast<unsigned int>(month)},
        std::chrono::day{static_cast<unsigned int>(day)}
    );
    auto sd = std::chrono::sys_days{ymd};
    date.value = sd.time_since_epoch().count();
    return ymd.ok();
}

bool DateTypeStd::Date2YMD(int32_t days, int32_t &year, int32_t &month, int32_t &day) {
    auto sd = std::chrono::sys_days{std::chrono::days{days}};
    auto ymd = std::chrono::year_month_day{sd};

    year = static_cast<int>(ymd.year());
    month = static_cast<unsigned>(ymd.month());
    day = static_cast<unsigned>(ymd.day());
    return IsDateValid(year, month, day);
}

bool DateTypeStd::IsDateValid(int32_t year, int32_t month, int32_t day) {
    DateTypeStd date;
    return YMD2Date(year, month, day, date);
}

bool DateTypeStd::Add(DateTypeStd input, IntervalType interval, DateTypeStd &output) {
    int32_t year{0}, month{0}, day{0};
    if (!Date2YMD(input.value, year, month, day)) {
        return false;
    }
    switch (interval.unit) {
        case kYear: {
            std::chrono::sys_days output_sd(std::chrono::days{input.value} + std::chrono::duration_cast<std::chrono::days>(std::chrono::years{interval.value}));
            output.value = output_sd.time_since_epoch().count();
            return true;
        }
        case kMonth: {
            std::chrono::sys_days output_sd(std::chrono::days{input.value} + std::chrono::duration_cast<std::chrono::days>(std::chrono::months{interval.value}));
            output.value = output_sd.time_since_epoch().count();
            return true;
        }
        case kDay: {
            output.value = input.value + interval.value;
            return true;
        }
        case kHour: {
            output.value = input.value + (interval.value / DAY_HOUR);
            return true;
        }
        case kMinute: {
            output.value = input.value + (interval.value / DAY_MINUTE);
            return true;
        }
        case kSecond: {
            output.value = input.value + (interval.value / DAY_SECOND);
            return true;
        }
        case kInvalidUnit: {
            ParserError("Invalid interval unit.");
        }
    }
    return false;
}

bool DateTypeStd::Subtract(DateTypeStd input, IntervalType interval, DateTypeStd &output) {
    interval.value = -interval.value;
    return Add(input, interval, output);
}

int64_t DateTypeStd::GetDatePart(DateTypeStd input, TimeUnit unit) {
    int32_t year{}, month{}, day{};
    auto result = Date2YMD(input.value, year, month, day);
    ParserAssert(result, "Invalid date value");
    switch (unit) {
        case TimeUnit::kYear: {
            return year;
        }
        case TimeUnit::kMonth: {
            return month;
        }
        case TimeUnit::kDay: {
            return day;
        }
        case TimeUnit::kHour: {
            ParserError("Can't extract hour from date");
        }
        case TimeUnit::kMinute: {
            ParserError("Can't extract minute from date");
        }
        case TimeUnit::kSecond: {
            ParserError("Can't extract second from date");
        }
        default: {
            ParserError("Invalid time unit");
        }
    }
    return -1;
}

}