//
// Created by JinHai on 2022/11/7.
//

#pragma once

#include "common/types/internal_types.h"

namespace infinity {

struct TimeType {
    TimeType() = default;

    explicit
    TimeType(i32 time_value): value(time_value) {};

    i32 value {};
};

}
