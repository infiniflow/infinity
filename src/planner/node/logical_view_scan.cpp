//
// Created by JinHai on 2022/8/10.
//

#include "logical_view_scan.h"

#include <sstream>

namespace infinity {

String LogicalViewScan::ToString(i64 &space) {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "ViewScan: " << view_ptr_->view_name();
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity