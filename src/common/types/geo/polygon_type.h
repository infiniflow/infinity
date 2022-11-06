//
// Created by JinHai on 2022/10/30.
//

#include "common/types/internal_types.h"
#include "common/types/geo/box_type.h"

namespace infinity {

struct PolygonType {
    ptr_t ptr;
    i32 point_count; // 65535 point are the max point count
    BoxType bounding_box;
};

}

