//
// Created by JinHai on 2022/9/26.
//

#include "logical_limit.h"
#include <sstream>

namespace infinity {

std::string
LogicalLimit::ToString(int64_t &space) {
    std::stringstream ss;
    std::string arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }

    ss << std::string(space, ' ') << arrow_str << "Limit (limit: " << limit_expression_->ToString();
    if(offset_expression_ != nullptr) {
        ss << ", offset: " << offset_expression_->ToString();
    }
    ss << ")";
    space += arrow_str.size();

    return ss.str();
}

}


