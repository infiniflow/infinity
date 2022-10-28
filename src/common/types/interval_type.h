//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "internal_types.h"

namespace infinity {

enum class TimeUnit : i8 {
    kYear,
    kMonth,
    kDay,
    kHour,
    kMinute,
    kSecond
};

struct IntervalType {
    i8 unit;
    i64 value;
};

}




