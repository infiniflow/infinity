//
// Created by jinhai on 23-8-26.
//

#include "logical_flush.h"

#include <sstream>

namespace infinity {

String LogicalFlush::ToString(i64 &space) {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }

    switch (flush_type_) {
        case FlushType::kData:
            ss << String(space, ' ') << "-> "
               << "Flush Data: ";
            break;
        case FlushType::kLog:
            ss << String(space, ' ') << "-> "
               << "Flush Log: ";
            break;
        case FlushType::kBuffer:
            ss << String(space, ' ') << "-> "
               << "Flush Buffer: ";
            break;
    }
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
