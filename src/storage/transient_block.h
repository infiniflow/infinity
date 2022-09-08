//
// Created by JinHai on 2022/7/31.
//

#pragma once

#include <any>

namespace infinity {

// This is not a block, but a row.
// FIXME: Use real block style to store the transient block
struct TransientBlock {
    uint64_t row_count_{0};
    std::vector<std::any> columns_;
};
}

