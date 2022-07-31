//
// Created by JinHai on 2022/7/31.
//

#pragma once

#include "common/singleton.h"

#include <cstdint>

namespace infinity {

struct Options {
    uint64_t default_row_count_ = 8192u;
    uint64_t default_chunk_size = 65536u;
};

class Config {
public:
    Options option_;
private:
};

}
