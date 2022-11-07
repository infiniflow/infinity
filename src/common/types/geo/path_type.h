//
// Created by JinHai on 2022/10/30.
//

#include "common/types/internal_types.h"
#include "common/types/varlen_type.h"

namespace infinity {

// Path type is a variable length type
struct PathType {
    ptr_t ptr {nullptr};
    i32 point_count {0}; // 65535 point are the max point count
    i32 closed {0}; // Is a closed polygon?
};

}
