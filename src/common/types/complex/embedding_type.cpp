//
// Created by JinHai on 2022/10/30.
//

#include "embedding_type.h"

namespace infinity {

static size_t embedding_type_size[] = {
    1,   // bit
    8,   // int8
    16,  // int16
    32,  // int32
    64,  // int64
    32,  // int32
    64,  // int64
};

size_t
EmbeddingDataBitWidth(EmbeddingDataType type_index) {
    return embedding_type_size[type_index];
}

}

