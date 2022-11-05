//
// Created by JinHai on 2022/10/30.
//

#include "common/types/internal_types.h"
#include "common/types/varlen_type.h"

namespace infinity {

// Path type is a variable length type
struct PathType {
    i32 start_index {0};
    i16 point_count {0}; // 65535 point are the max point count
    i8 closed {0}; // Is a closed polygon?
    i8 _dummy {0}; // Padding to make the struct aligned with 8 bytes.
};

}
