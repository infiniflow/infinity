//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "parser_assert.h"
#include "point_type.h"

namespace infinity {

// A line segment is specified by two endpoints.
struct LineSegmentType {
    PointType point1;
    PointType point2;

    LineSegmentType(PointType p1, PointType p2) : point1(p1), point2(p2) {}

    inline bool operator==(const LineSegmentType &other) const {
        if (this == &other)
            return true;
        return (point1 == other.point1) && (point2 == other.point2);
    }

    inline bool operator!=(const LineSegmentType &other) const { return !operator==(other); }

    inline void Reset() {
        point1.Reset();
        point2.Reset();
    }

    [[nodiscard]] inline std::string ToString() const { ParserError("ToString() isn't implemented"); }
};

} // namespace infinity
