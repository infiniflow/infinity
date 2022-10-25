//
// Created by JinHai on 2022/10/25.
//

#include "logical_dummy_scan.h"

#include <sstream>

namespace infinity {

std::string
LogicalDummyScan::ToString(int64_t &space) {
    std::stringstream ss;
    std::string arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << std::string(space, ' ') << "-> " << "DummyScan: ";
    space += arrow_str.size();
    return ss.str();
}

}
