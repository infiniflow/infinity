//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "common/types/alias/primitives.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

struct DateTimeType {
    DateTimeType() = default;

    explicit DateTimeType(i32 date_value, i32 time_value) : date(date_value), time(time_value){};

    inline void Reset() {
        date = 0;
        time = 0;
    }

    [[nodiscard]] inline String ToString() const { TypeError("ToString() isn't implemented"); }

    i32 date{};
    i32 time{};
};

} // namespace infinity
