//
// Created by JinHai on 2022/9/26.
//

#include "logical_limit.h"
#include <sstream>

namespace infinity {

String
LogicalLimit::ToString(i64& space) {
    std::stringstream ss;
    String arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }

    ss << String(space, ' ') << arrow_str << "Limit (limit: " << limit_expression_->Name();
    if(offset_expression_ != nullptr) {
        ss << ", offset: " << offset_expression_->Name();
    }
    ss << ")";
    space += arrow_str.size();

    return ss.str();
}

}


