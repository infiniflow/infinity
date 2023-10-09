//
// Created by jinhai on 23-3-6.
//

#include "logical_create_view.h"

#include <sstream>

namespace infinity {

String LogicalCreateView::ToString(i64 &space) {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Create View: " << create_view_info_->schema_name_ << "." << create_view_info_->view_name_;
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
