//
// Created by JinHai on 2022/7/30.
//

#include "logical_chunk_scan.h"
#include <sstream>

namespace infinity {

std::string
LogicalChunkScan::ToString(uint64_t space) {
    std::stringstream ss;
    ss << std::string(space, ' ') << "ChunkScan: " << table_ptr_->table_def()->name() << std::endl;
    return ss.str();
}

}