//
// Created by JinHai on 2022/9/9.
//

#include "vector_operations.h"

namespace infinity {


Chunk VectorOperation::VectorCast(LogicalType src, LogicalType dst) {
    return Chunk(dst);
}


}
