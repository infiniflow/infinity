//
// Created by JinHai on 2022/7/30.
//

#include "logical_chunk_scan.h"
#include <sstream>

namespace infinity {

std::string
LogicalChunkScan::ToString(int64_t& space) {
    std::stringstream ss;
    std::string arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << std::string(space, ' ') << "-> " << "ChunkScan: " << std::endl;
    space += arrow_str.size();
    return ss.str();
}

}