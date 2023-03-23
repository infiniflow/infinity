//
// Created by JinHai on 2022/7/30.
//

#include "logical_chunk_scan.h"
#include <sstream>

namespace infinity {

String
ToString(ChunkScanType type) {
    switch(type) {
        case ChunkScanType::kShowTables:
            return "Show tables";
        case ChunkScanType::kShowViews:
            return "Show views";
        case ChunkScanType::kShowColumn:
            return "Show column";
        default: {
            PlannerError("Invalid chunk scan type");
        }
    }
}

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