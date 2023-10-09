//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "point_type.h"

namespace infinity {

// A circle is specified by a center point and radius
struct CircleType {

    CircleType(PointType c, f64 r) : center(c), radius(r) {}

    PointType center{};
    f64 radius{};

    inline bool operator==(const CircleType &other) const {
        if (this == &other)
            return true;
        return (fabs(radius - other.radius) < std::numeric_limits<f64>::epsilon()) && (center == other.center);
    }

    inline bool operator!=(const CircleType &other) const { return !operator==(other); }

    inline void Reset() {
        center.Reset();
        radius = 0;
    }

    [[nodiscard]] inline String ToString() const { TypeError("ToString() isn't implemented"); }
};

} // namespace infinity
