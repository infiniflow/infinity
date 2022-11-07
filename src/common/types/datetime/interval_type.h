//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "common/types/internal_types.h"

namespace infinity {

enum TimeUnit : i8 {
    kYear = 0,
    kMonth,
    kDay,
    kHour,
    kMinute,
    kSecond
};

struct IntervalType {
    i32 unit;
    i32 value;
};

}




