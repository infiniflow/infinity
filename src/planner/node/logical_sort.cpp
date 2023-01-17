//
// Created by JinHai on 2022/9/26.
//

#include "logical_sort.h"
#include <sstream>

namespace infinity {

String
LogicalSort::ToString(i64& space) {
    std::stringstream ss;
    String arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Sort" << std::endl;
    if(arrow_str.empty()) {
        space += 2;
        ss << String(space , ' ') << "Sort Key: ";
    } else {
        space += 5;
        ss << String(space , ' ') << "Sort Key: ";
    }

    size_t expression_count = expressions_.size();
    for(size_t i = 0; i < expression_count - 1; ++ i) {
        ss << expressions_[i]->ToString() << " " << infinity::ToString(order_by_types_[i]) << ", ";
    }
    ss << expressions_.back()->ToString() << " " << infinity::ToString(order_by_types_.back());
    space += arrow_str.size();

    return ss.str();
}

}


