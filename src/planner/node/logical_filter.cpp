//
// Created by JinHai on 2022/8/9.
//

#include "logical_filter.h"

#include <sstream>

namespace infinity {

std::string LogicalFilter::ToString(int64_t& space) {
    std::stringstream ss;
    std::string arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << std::string(space, ' ') << arrow_str << "Logical Filter: " << expression_->ToString();
    space += arrow_str.size();
    return ss.str();
}

}