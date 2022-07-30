//
// Created by JinHai on 2022/7/31.
//

#pragma once

#include "data_type.h"

namespace infinity {

enum class ChunkType {kInvalid, kFixedRowCount, kFixedChunkSize};

// Chunk is an column block with a row group. A Chunk contains specific type of data and nullable flags.
// Two types of chunk: fixed row count chunk and fixed size chunk.

class Chunk {
public:
    // Third parameter means row count limitation when chunk type is Fixed Row Count Chunk.
    // Third parameter means size limitation when chunk type is Fixed Chunk Size.
    explicit Chunk(ChunkType type, PhysicalType data_type, uint64_t number);
    virtual ~Chunk() = default;

    [[nodiscard]] PhysicalType DataType() const { return data_type_; }

    [[nodiscard]] uint64_t row_count() const { return row_count_; }
    [[nodiscard]] uint64_t memory_size() const { return -1; }

protected:
    ChunkType chunk_type_;
    PhysicalType data_type_;
    uint64_t row_count_{0};
    uint64_t chunk_size_{0};
    std::unique_ptr<char[]> data_ptr_;
};

}
