//
// Created by JinHai on 2022/7/31.
//

#pragma once

#include "common/singleton.h"
#include "storage/chunk.h"

#include <cstdint>

namespace infinity {

struct Options {
    int64_t default_row_count_ = 8192u;
    int64_t default_chunk_size = 65536u;
    TableType table_type_ = TableType::kFixedRowCount;
};

class Config {
public:
    Options option_;
private:
};

}
