//
// Created by JinHai on 2022/7/30.
//

#include "logical_chunk_scan.h"
#include <sstream>

namespace infinity {

String
LogicalChunkScan::ToString(i64& space) {
    std::stringstream ss;
    String arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << "-> " << "ChunkScan: ";
    space += arrow_str.size();

    return ss.str();
}

}