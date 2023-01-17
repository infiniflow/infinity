//
// Created by JinHai on 2022/11/7.
//

#pragma once

#include "common/types/internal_types.h"

namespace infinity {

struct TimestampTZType {
    TimestampTZType() = default;

    explicit
    TimestampTZType(i32 date_value, i32 time_value) : date(date_value), time(time_value) {}

    i32 date {};
    i32 time {};

    inline void
    Reset() {
        date = 0;
        time = 0;
    }
};

}
