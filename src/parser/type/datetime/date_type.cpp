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

#include "date_type.h"
#include "parser_assert.h"
#include "spdlog/fmt/fmt.h"

namespace infinity {

// min date: 0001-01-01
constexpr static int32_t MIN_DATE_YEAR = -1969;
constexpr static int32_t MIN_DATE_MONTH = 1;
constexpr static int32_t MIN_DATE_DAY = 1;
// max date: 9999-12-31
constexpr static int32_t MAX_DATE_YEAR = 9999;
constexpr static int32_t MAX_DATE_MONTH = 12;
constexpr static int32_t MAX_DATE_DAY = 31;
constexpr static int32_t EPOCH_YEAR = 1970;

constexpr static int32_t YEAR_LEAP = 400;
constexpr static int32_t DAYS_PER_YEAR_LEAP = 146097;
constexpr static int32_t DAY_HOUR = 24;
constexpr static int32_t DAY_MINUTE = DAY_HOUR * 60;
constexpr static int32_t DAY_SECOND = DAY_MINUTE * 60;

static const int32_t NORMAL_DAYS[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static const int32_t CUMULATIVE_DAYS[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

static const int32_t LEAP_DAYS[13] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static const int32_t CUMULATIVE_LEAP_DAYS[13] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};

static const int32_t MONTH_PER_DAY_OF_YEAR[365] = {
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,
    3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
    4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
    5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
    6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
    7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
    8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
    9,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11,
    11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12};

static const int32_t LEAP_MONTH_PER_DAY_OF_YEAR[366] = {
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,
    3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
    4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
    5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
    6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
    7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
    8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
    9,  9,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11,
    11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12};

static const int32_t CUMULATIVE_YEAR_DAYS[401] = {
    0,      365,    730,    1096,   1461,   1826,   2191,   2557,   2922,   3287,   3652,   4018,   4383,   4748,   5113,   5479,   5844,   6209,
    6574,   6940,   7305,   7670,   8035,   8401,   8766,   9131,   9496,   9862,   10227,  10592,  10957,  11323,  11688,  12053,  12418,  12784,
    13149,  13514,  13879,  14245,  14610,  14975,  15340,  15706,  16071,  16436,  16801,  17167,  17532,  17897,  18262,  18628,  18993,  19358,
    19723,  20089,  20454,  20819,  21184,  21550,  21915,  22280,  22645,  23011,  23376,  23741,  24106,  24472,  24837,  25202,  25567,  25933,
    26298,  26663,  27028,  27394,  27759,  28124,  28489,  28855,  29220,  29585,  29950,  30316,  30681,  31046,  31411,  31777,  32142,  32507,
    32872,  33238,  33603,  33968,  34333,  34699,  35064,  35429,  35794,  36160,  36525,  36890,  37255,  37621,  37986,  38351,  38716,  39082,
    39447,  39812,  40177,  40543,  40908,  41273,  41638,  42004,  42369,  42734,  43099,  43465,  43830,  44195,  44560,  44926,  45291,  45656,
    46021,  46387,  46752,  47117,  47482,  47847,  48212,  48577,  48942,  49308,  49673,  50038,  50403,  50769,  51134,  51499,  51864,  52230,
    52595,  52960,  53325,  53691,  54056,  54421,  54786,  55152,  55517,  55882,  56247,  56613,  56978,  57343,  57708,  58074,  58439,  58804,
    59169,  59535,  59900,  60265,  60630,  60996,  61361,  61726,  62091,  62457,  62822,  63187,  63552,  63918,  64283,  64648,  65013,  65379,
    65744,  66109,  66474,  66840,  67205,  67570,  67935,  68301,  68666,  69031,  69396,  69762,  70127,  70492,  70857,  71223,  71588,  71953,
    72318,  72684,  73049,  73414,  73779,  74145,  74510,  74875,  75240,  75606,  75971,  76336,  76701,  77067,  77432,  77797,  78162,  78528,
    78893,  79258,  79623,  79989,  80354,  80719,  81084,  81450,  81815,  82180,  82545,  82911,  83276,  83641,  84006,  84371,  84736,  85101,
    85466,  85832,  86197,  86562,  86927,  87293,  87658,  88023,  88388,  88754,  89119,  89484,  89849,  90215,  90580,  90945,  91310,  91676,
    92041,  92406,  92771,  93137,  93502,  93867,  94232,  94598,  94963,  95328,  95693,  96059,  96424,  96789,  97154,  97520,  97885,  98250,
    98615,  98981,  99346,  99711,  100076, 100442, 100807, 101172, 101537, 101903, 102268, 102633, 102998, 103364, 103729, 104094, 104459, 104825,
    105190, 105555, 105920, 106286, 106651, 107016, 107381, 107747, 108112, 108477, 108842, 109208, 109573, 109938, 110303, 110669, 111034, 111399,
    111764, 112130, 112495, 112860, 113225, 113591, 113956, 114321, 114686, 115052, 115417, 115782, 116147, 116513, 116878, 117243, 117608, 117974,
    118339, 118704, 119069, 119435, 119800, 120165, 120530, 120895, 121260, 121625, 121990, 122356, 122721, 123086, 123451, 123817, 124182, 124547,
    124912, 125278, 125643, 126008, 126373, 126739, 127104, 127469, 127834, 128200, 128565, 128930, 129295, 129661, 130026, 130391, 130756, 131122,
    131487, 131852, 132217, 132583, 132948, 133313, 133678, 134044, 134409, 134774, 135139, 135505, 135870, 136235, 136600, 136966, 137331, 137696,
    138061, 138427, 138792, 139157, 139522, 139888, 140253, 140618, 140983, 141349, 141714, 142079, 142444, 142810, 143175, 143540, 143905, 144271,
    144636, 145001, 145366, 145732, 146097};

void DateType::FromString(const char *date_ptr, size_t length) {
    size_t end_length_unused;
    if (!ConvertFromString(date_ptr, length, *this, end_length_unused)) {
        ParserError("Invalid date format (YYYY-MM-DD or YYYY/MM/DD).");
    }
}

void DateType::FromString(const char *date_ptr, size_t length, size_t &end_length) {
    if (!ConvertFromString(date_ptr, length, *this, end_length)) {
        ParserError("Invalid date format (YYYY-MM-DD or YYYY/MM/DD).");
    }
}

std::string DateType::ToString() const {
    int32_t year{0}, month{0}, day{0};
    if (!Date2YMD(value, year, month, day)) {
        ParserError(fmt::format("Invalid date: {}-{}-{}", year, month, day));
    }
    // TODO: format for negative year?
    return fmt::format("{:04d}-{:02d}-{:02d}", year, month, day);
}

bool DateType::ConvertFromString(const char *date_ptr, size_t length, DateType &date, size_t &end_length) {
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
            if (year > 9999)
                return false;
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
            if (month > 12)
                return false;
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
            if (day > 31)
                return false;
            continue;
        }
        break;
    }
    end_length = pos;

    return YMD2Date(year, month, day, date);
}

bool DateType::YMD2Date(int32_t year, int32_t month, int32_t day, DateType &date) {

    int32_t result = 0;
    if (!IsDateValid(year, month, day)) {
        return false;
    }

    if (IsLeapYear(year)) {
        result += CUMULATIVE_LEAP_DAYS[month - 1];
    } else {
        result += CUMULATIVE_DAYS[month - 1];
    }

    result += day - 1;
    if (year < 1970) {
        // e.g. 970 - 1970 = 1000
        int32_t diff_from_base = 1970 - year;

        // 400 - 1000 % 400 = 200;
        int32_t year_index = 400 - (diff_from_base % 400);

        // 1000 / 400 = 2;
        int32_t fractions = diff_from_base / 400;

        // days of 200 years = CUMULATIVE_YEAR_DAYS[200];
        result += CUMULATIVE_YEAR_DAYS[year_index];

        // result -= days of 400 years
        result -= DAYS_PER_YEAR_LEAP;

        // result -= days of 800 years
        result -= fractions * DAYS_PER_YEAR_LEAP;

        // result = days of (200 - 400 - 800 = 1000) year
    } else if (year >= 2370) {
        // 2370 = 1970 + 400; e.g.  2870 - 2370 = 500
        int32_t diff_from_base = year - 2370;

        // 500 % 400 = 100
        int32_t year_index = diff_from_base % 400;

        // 500 / 400 = 1
        int32_t fractions = diff_from_base / 400;

        // result += days of 100 years
        result += CUMULATIVE_YEAR_DAYS[year_index];

        // result += days of 400 years
        result += DAYS_PER_YEAR_LEAP;

        // result += days of 1 * 400 years
        result += fractions * DAYS_PER_YEAR_LEAP;

        // result = days of (100 + 400 + 400 = 900) years (2870 - 1970 = 900);
    } else {
        result += CUMULATIVE_YEAR_DAYS[year - 1970];
    }

    date.value = result;
    return true;
}

bool DateType::Date2YMD(int32_t days, int32_t &year, int32_t &month, int32_t &day) {
    int32_t year_offset;
    {
        year = EPOCH_YEAR;
        // Normalize the days to range [1970, 2370] and relocate the EPOCH year
        while (days < 0) {
            days += DAYS_PER_YEAR_LEAP;
            year -= YEAR_LEAP;
        }
        while (days >= DAYS_PER_YEAR_LEAP) {
            days -= DAYS_PER_YEAR_LEAP;
            year += YEAR_LEAP;
        }

        // Try to locate the range of the year_offset
        year_offset = days / 365;

        // Close find the correct year_offset
        while (days < CUMULATIVE_YEAR_DAYS[year_offset]) {
            year_offset--;
        }
        year += year_offset;
    }

    day = days - CUMULATIVE_YEAR_DAYS[year_offset];

    if (IsLeapYear(year)) {
        month = LEAP_MONTH_PER_DAY_OF_YEAR[day];
        day -= CUMULATIVE_LEAP_DAYS[month - 1];
    } else {
        month = MONTH_PER_DAY_OF_YEAR[day];
        day -= CUMULATIVE_DAYS[month - 1];
    }
    day++;

    return IsDateValid(year, month, day);
}

bool DateType::IsDateValid(int32_t year, int32_t month, int32_t day) {
    if (year <= MIN_DATE_YEAR) {
        if (year < MIN_DATE_YEAR) {
            return false;
        } else if (year == MIN_DATE_YEAR) {
            if (month < MIN_DATE_MONTH || (month == MIN_DATE_MONTH && day < MIN_DATE_DAY)) {
                return false;
            }
        }
    }
    if (year >= MAX_DATE_YEAR) {
        if (year > MAX_DATE_YEAR) {
            return false;
        } else if (year == MAX_DATE_YEAR) {
            if (month > MAX_DATE_MONTH || (month == MAX_DATE_MONTH && day > MAX_DATE_DAY)) {
                return false;
            }
        }
    }

    if (month < 1 || month > 12) {
        return false;
    }

    if (day < 1) {
        return false;
    }

    if (IsLeapYear(year)) {
        return day <= LEAP_DAYS[month];
    } else {
        return day <= NORMAL_DAYS[month];
    }
}

bool DateType::Add(DateType input, IntervalType interval, DateType &output) {
    switch (interval.unit) {
        case kYear: {
            int32_t year{0}, month{0}, day{0};
            if (!Date2YMD(input.value, year, month, day)) {
                return false;
            }
            year += interval.value;
            return YMD2Date(year, month, day, output);
        }
        case kMonth: {
            int32_t year{0}, month{0}, day{0};
            if (!Date2YMD(input.value, year, month, day)) {
                return false;
            }
            month += interval.value;
            if (month > 12) {
                year += (month - 1) / 12;
                month = 1 + ((month - 1) % 12);
            } else if (month < 1) {
                year += (month - 12) / 12;
                month = 12 + ((month - 12) % 12);
            }
            // TODO: 2020-01-30 + 1 month = 2020-02-30 ?
            day = std::min(day, (IsLeapYear(year) ? LEAP_DAYS[month] : NORMAL_DAYS[month]));
            return YMD2Date(year, month, day, output);
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

bool DateType::Subtract(DateType input, IntervalType interval, DateType &output) {
    interval.value = -interval.value;
    return Add(input, interval, output);
}

int64_t DateType::GetDatePart(DateType input, TimeUnit unit) {
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

} // namespace infinity
