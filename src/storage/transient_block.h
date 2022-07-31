//
// Created by JinHai on 2022/7/31.
//

#pragma once

#include <any>

namespace infinity {
struct TransientBlock {
    uint64_t row_count_{0};
    std::vector<std::any> columns_;
};
}

