//
// Created by JinHai on 2022/7/31.
//

#pragma once

#include "chunk.h"

#include <cstdint>
#include <vector>

namespace infinity {

class RowGroup {
public:
private:
    const uint64_t start_row_{0};
    uint64_t row_count_{0};

    std::vector<std::shared_ptr<Chunk>> columns_;

};

}