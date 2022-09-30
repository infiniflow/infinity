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

    std::string log_file = "/tmp/infinity.log";
    std::size_t log_max_size = 1024*1024*10;
    std::size_t log_file_rotate_count = 10;
    std::string log_level = "trace";
};

class Config {
public:
    Options option_;
private:
};

}
