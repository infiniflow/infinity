//
// Created by JinHai on 2022/9/26.
//

#include "logical_sort.h"
#include <sstream>

namespace infinity {

std::string
LogicalSort::ToString(int64_t& space) {
    std::stringstream ss;
    std::string arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << std::string(space, ' ') << arrow_str << "Sort" << std::endl;
    if(arrow_str.empty()) {
        space += 2;
        ss << std::string(space , ' ') << "Sort Key: ";
    } else {
        space += 5;
        ss << std::string(space , ' ') << "Sort Key: ";
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


