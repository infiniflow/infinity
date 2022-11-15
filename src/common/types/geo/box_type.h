//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "common/types/internal_types.h"
#include "point_type.h"

namespace infinity {

// A box is specified by two points: upper left  point and lower right point.
struct BoxType {
    BoxType() = default;
    BoxType(PointType ul, PointType lr) : upper_left(ul), lower_right(lr) {};

    PointType upper_left;
    PointType lower_right;

    void Reset() {
        upper_left.Reset();
        lower_right.Reset();
    }
};

}
