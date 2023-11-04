//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "parser_assert.h"
#include "point_type.h"

namespace infinity {

// A circle is specified by a center point and radius
struct CircleType {

    CircleType(PointType c, double r) : center(c), radius(r) {}

    PointType center{};
    double radius{};

    inline bool operator==(const CircleType &other) const {
        if (this == &other)
            return true;
        return (fabs(radius - other.radius) < std::numeric_limits<double>::epsilon()) && (center == other.center);
    }

    inline bool operator!=(const CircleType &other) const { return !operator==(other); }

    inline void Reset() {
        center.Reset();
        radius = 0;
    }

    [[nodiscard]] inline std::string ToString() const { ParserError("ToString() isn't implemented"); }
};

} // namespace infinity
