//
// Created by JinHai on 2022/11/7.
//

#pragma once

#include "common/types/internal_types.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

struct TimeType {
    TimeType() = default;

    explicit
    TimeType(i32 time_value): value(time_value) {};

    [[nodiscard]] inline String
    ToString() const {
        TypeError("ToString() isn't implemented");
    }

    i32 value {};
};

}
