//
// Created by JinHai on 2022/8/10.
//

#include "logical_view_scan.h"

#include <sstream>

namespace infinity {

std::string
LogicalViewScan::ToString(uint64_t space) {
    std::stringstream ss;
    ss << std::string(space, ' ') << "ViewScan: " << view_ptr_->name() << std::endl;
    return ss.str();
}

}