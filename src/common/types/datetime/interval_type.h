//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "common/types/alias/primitives.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

enum TimeUnit : i32 {
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

    inline explicit
    IntervalType(i32 v) : value(v) {}

    String
    ToString() const;

    inline void
    Reset() {
        unit = TimeUnit::kInvalidUnit;
        value = 0;
    }

    TimeUnit unit{TimeUnit::kInvalidUnit};
    i32 value{};
};

}




