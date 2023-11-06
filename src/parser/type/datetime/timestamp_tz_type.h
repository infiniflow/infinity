//
// Created by JinHai on 2022/11/7.
//

#pragma once

#include "parser_assert.h"
#include <cstdint>
#include <string>

namespace infinity {

struct TimestampTZType {
    TimestampTZType() = default;

    explicit TimestampTZType(int32_t date_value, int32_t time_value) : date(date_value), time(time_value) {}

    inline void Reset() {
        date = 0;
        time = 0;
    }

    [[nodiscard]] inline std::string ToString() const { ParserError("ToString() isn't implemented"); }

    int32_t date{};
    int32_t time{};
};

} // namespace infinity
