//
// Created by JinHai on 2022/7/31.
//

#pragma once

#include "data_type.h"
#include <vector>
#include <any>

namespace infinity {

enum class ChunkType {
    kInvalid,
    kNormal,
    kConstant,
};

// Chunk is an column block with a row group. A Chunk contains specific type of data and nullable flags.
// Two types of chunk: fixed row count chunk and fixed size chunk.

class Chunk {
public:
    // Second parameter means row count limitation when chunk type is Fixed Row Count Chunk.
    // Second parameter means size limitation when chunk type is Fixed Chunk Size.
    explicit Chunk(LogicalType data_type, int64_t number = 0)
            : data_type_(data_type), limit_(number) {} ;
    virtual ~Chunk() = default;

    [[nodiscard]] LogicalType DataType() const { return data_type_; }

    [[nodiscard]] int64_t row_count() const { return row_count_; }
    [[nodiscard]] int64_t memory_size() const { return row_count_ * static_cast<int64_t>(data_type_.Size()); }

    void Append(const Chunk& chunk, int64_t start_idx);
    void Append(const std::any& value);

    std::vector<std::any>& data() { return data_; }
    [[nodiscard]] const std::vector<std::any>& data() const { return data_; }
    const ChunkType chunk_type() const { return chunk_type_; }

    std::string ToString();
    std::string RowToString(int64_t row_id);
protected:
    LogicalType data_type_;
    int64_t limit_{0};
    int64_t row_count_{0};
    int64_t chunk_size_{0};
    std::vector<std::any> data_;
    ChunkType chunk_type_;
};

}
