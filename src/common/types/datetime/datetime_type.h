//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "common/types/internal_types.h"

namespace infinity {

struct DateTimeType {
    DateTimeType() = default;

    explicit
    DateTimeType(i32 date_value, i32 time_value): date(date_value), time(time_value) {};

    i32 date{};
    i32 time{};

    inline void
    Reset() {
        date = 0;
        time = 0;
    }
};

}
