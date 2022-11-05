//
// Created by JinHai on 2022/10/30.
//

#include "common/types/internal_types.h"
#include "common/types/geo/box_type.h"

namespace infinity {

struct PolygonType {
    i32 start_index {0};
    i16 point_count {0}; // 65535 point are the max point count
    BoxType bounding_box;
};

}

