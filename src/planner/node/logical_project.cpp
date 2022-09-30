//
// Created by JinHai on 2022/9/26.
//

#include "logical_project.h"
#include <sstream>

namespace infinity {

std::string
LogicalProject::ToString(int64_t& space) {
    std::stringstream ss;
    std::string arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << std::string(space, ' ') << arrow_str << "Project on: ";
    size_t expression_count = expressions_.size();
    for(size_t i = 0; i < expression_count - 1; ++ i) {
        ss << expressions_[i]->ToString() << ", ";
    }
    ss << expressions_.back()->ToString();
    space += arrow_str.size();
    return ss.str();
}

}


