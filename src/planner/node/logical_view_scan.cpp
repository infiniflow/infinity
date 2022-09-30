//
// Created by JinHai on 2022/8/10.
//

#include "logical_view_scan.h"

#include <sstream>

namespace infinity {

std::string
LogicalViewScan::ToString(int64_t& space) {
    std::stringstream ss;
    std::string arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << std::string(space, ' ') << arrow_str << "ViewScan: " << view_ptr_->name();
    space += arrow_str.size();
    return ss.str();
}

}