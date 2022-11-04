//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "internal_types.h"
#include "point_type.h"

namespace infinity {

// A box is specified by two points: upper left  point and lower right point.
struct BoxType {
    PointType upper_left;
    PointType lower_right;
};

}
