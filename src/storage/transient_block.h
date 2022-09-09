//
// Created by JinHai on 2022/7/31.
//

#pragma once

#include "chunk.h"

#include <any>

namespace infinity {

struct TransientBlock {
    TransientBlock() = default;
    explicit TransientBlock(const std::vector<LogicalType>& types) {
        for(auto& type: types) {
            chunks_.emplace_back(type);
        }
    }
    int64_t row_count_{0};
    std::vector<Chunk> chunks_;
};
}

