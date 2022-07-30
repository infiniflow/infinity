//
// Created by JinHai on 2022/7/31.
//

#include "chunk.h"

namespace infinity {
Chunk::Chunk(ChunkType chunk_type, PhysicalType data_type, uint64_t number)
    : chunk_type_(chunk_type), data_type_(data_type) {
    if(chunk_type == ChunkType::kFixedChunkSize) {
        chunk_size_ = number;
    } else {
        row_count_ = number;
    }
}
}