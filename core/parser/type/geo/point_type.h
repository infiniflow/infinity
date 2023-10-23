//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "parser_assert.h"
#include <string>
#include <cmath>

namespace infinity {

struct PointType {
    PointType() = default;

    inline PointType(double a, double b) : x(a), y(b) {}

    inline bool operator==(const PointType &other) const {
        if (this == &other)
            return true;
        return (fabs(x - other.x) < std::numeric_limits<double>::epsilon()) && (fabs(y - other.y) < std::numeric_limits<double>::epsilon());
    }

    inline bool operator!=(const PointType &other) const { return !operator==(other); }

    inline void Reset() {
        x = 0;
        y = 0;
    }

    [[nodiscard]] inline std::string ToString() const { ParserError("ToString() isn't implemented"); }

    double x{};
    double y{};
};

} // namespace infinity
