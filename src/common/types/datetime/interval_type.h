//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "common/types/internal_types.h"

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

    TimeUnit unit {TimeUnit::kInvalidUnit};
    i32 value {};

    inline void
    Reset() {
        unit = TimeUnit::kInvalidUnit;
        value = 0;
    }
};

}




