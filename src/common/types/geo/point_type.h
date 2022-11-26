//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "common/types/internal_types.h"
#include <cmath>

namespace infinity {

struct PointType {
    PointType() = default;

    inline
    PointType(f64 a, f64 b): x(a), y(b) {}

    inline bool
    operator==(const PointType& other) const {
        if(this == &other) return true;
        return (fabs(x - other.x) < std::numeric_limits<f64>::epsilon())
            && (fabs(y - other.y) < std::numeric_limits<f64>::epsilon());
    }

    inline bool
    operator!=(const PointType& other) const {
        return !operator==(other);
    }

    inline void
    Reset() {
        x = 0;
        y = 0;
    }

    f64 x{};
    f64 y{};
};

}
