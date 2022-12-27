//
// Created by JinHai on 2022/7/31.
//

#pragma once

#include "common/singleton.h"
#include "common/types/internal_types.h"

#include <cstdint>

namespace infinity {

struct Options {
    i64 default_row_count_ = 8192u;
    i64 default_chunk_size = 65536u;

    String log_file = "/tmp/infinity.log";
    SizeT log_max_size = 1024*1024*10;
    SizeT log_file_rotate_count = 10;
    String log_level = "trace";
};

class Config {
public:
    Options option_;
private:
};

}
