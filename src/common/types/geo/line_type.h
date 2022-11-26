//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "common/types/internal_types.h"
#include <cmath>

namespace infinity {

// ax + by + c = 0 to represent a line
struct LineType {

    LineType(f64 input_a, f64 input_b, f64 input_c) : a(input_a), b(input_b), c(input_c) {}

    f64 a;
    f64 b;
    f64 c;

    inline bool
    operator==(const LineType& other) const {
        if(this == &other) return true;
        return (fabs(a - other.a) < std::numeric_limits<f64>::epsilon())
            && (fabs(b - other.b) < std::numeric_limits<f64>::epsilon())
            && (fabs(c - other.c) < std::numeric_limits<f64>::epsilon());
    }

    inline bool
    operator!=(const LineType& other) const {
        return !operator==(other);
    }

    inline void
    Reset() {
        a = 0;
        b = 0;
        c = 0;
    }
};

}
