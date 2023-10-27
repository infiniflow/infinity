//
// Created by JinHai on 2022/11/7.
//

#pragma once

#include "parser_assert.h"
#include <string>

namespace infinity {

struct TimeType {
    TimeType() = default;

    explicit TimeType(int32_t time_value) : value(time_value){};

    [[nodiscard]] inline std::string ToString() const { ParserError("ToString() isn't implemented"); }

    int32_t value{};
};

} // namespace infinity
