//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "parser_assert.h"
#include "point_type.h"

namespace infinity {

// A box is specified by two points: upper left  point and lower right point.
struct BoxType {
    BoxType() = default;

    BoxType(PointType ul, PointType lr) : upper_left(ul), lower_right(lr){};

    PointType upper_left;
    PointType lower_right;

    inline bool operator==(const BoxType &other) const {
        if (this == &other)
            return true;
        return (upper_left == other.upper_left) && (lower_right == other.lower_right);
    }

    inline bool operator!=(const BoxType &other) const { return !operator==(other); }

    void Reset() {
        upper_left.Reset();
        lower_right.Reset();
    }

    [[nodiscard]] inline std::string ToString() const { ParserError("ToString() isn't implemented"); }
};

} // namespace infinity
