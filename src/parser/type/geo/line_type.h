//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "parser_assert.h"
#include <cmath>
#include <string>

namespace infinity {

// ax + by + c = 0 to represent a line
struct LineType {

    LineType(double input_a, double input_b, double input_c) : a(input_a), b(input_b), c(input_c) {}

    double a;
    double b;
    double c;

    inline bool operator==(const LineType &other) const {
        if (this == &other)
            return true;
        return (fabs(a - other.a) < std::numeric_limits<double>::epsilon()) && (fabs(b - other.b) < std::numeric_limits<double>::epsilon()) &&
               (fabs(c - other.c) < std::numeric_limits<double>::epsilon());
    }

    inline bool operator!=(const LineType &other) const { return !operator==(other); }

    inline void Reset() {
        a = 0;
        b = 0;
        c = 0;
    }

    [[nodiscard]] inline std::string ToString() const { ParserError("ToString() isn't implemented"); }
};

} // namespace infinity
