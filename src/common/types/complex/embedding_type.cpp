//
// Created by JinHai on 2022/10/30.
//

#include "embedding_type.h"

namespace infinity {

size_t
EmbeddingType::embedding_type_width[] = {
        0,   // bit
        1,   // int8
        2,  // int16
        4,  // int32
        8,  // int64
        4,  // float32
        8,  // double64
};

void
EmbeddingType::Init(const void* from_ptr, SizeT size) {
    // User need to guarantee the size is matched.
    memcpy(ptr, from_ptr, size);
}

}
