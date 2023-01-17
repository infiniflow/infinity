//
// Created by JinHai on 2022/8/9.
//

#include "logical_filter.h"

#include <sstream>

namespace infinity {

String LogicalFilter::ToString(i64& space) {
    std::stringstream ss;
    String arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Logical Filter: " << expression_->ToString();
    space += arrow_str.size();
    return ss.str();
}

}