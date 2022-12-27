//
// Created by JinHai on 2022/7/31.
//
#if 0
#pragma once

#include "chunk.h"

#include <any>

#define DEFAULT_CHUNK_SIZE 8192

namespace infinity {

struct TransBlock {
    TransBlock() = default;

    void
    Init(const std::vector<LogicalType>& types);

    void
    Reset();

    [[nodiscard]] int64_t ColumnCount() const { return static_cast<int64_t>(chunks_.size()); }

    int64_t row_count_{0};
    std::vector<Chunk> chunks_;
};

}
#endif
