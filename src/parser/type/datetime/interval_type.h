//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include <string>

namespace infinity {

enum TimeUnit : int32_t {
    kYear = 0,
    kMonth,
    kDay,
    kHour,
    kMinute,
    kSecond,
    kInvalidUnit,
};

struct IntervalType {
    IntervalType() = default;

    inline explicit IntervalType(int32_t v) : value(v) {}

    std::string ToString() const;

    inline void Reset() {
        unit = TimeUnit::kInvalidUnit;
        value = 0;
    }

    TimeUnit unit{TimeUnit::kInvalidUnit};
    int32_t value{};
};

} // namespace infinity
