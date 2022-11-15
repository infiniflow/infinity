//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "common/types/internal_types.h"

namespace infinity {

struct PointType {
    PointType() = default;

    inline
    PointType(f64 a, f64 b): x(a), y(b) {}

    inline void
    Reset() {
        x = 0;
        y = 0;
    }

    f64 x{};
    f64 y{};
};

}
