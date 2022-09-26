//
// Created by JinHai on 2022/9/26.
//

#include "logical_project.h"
#include <sstream>

namespace infinity {

std::string
LogicalProject::ToString(uint64_t space) {
    std::stringstream ss;
    ss << std::string(space, ' ') << "Project on: ";
    size_t expression_count = expressions_.size();
    for(size_t i = 0; i < expression_count - 1; ++ i) {
        ss << expressions_[i]->ToString() << ", ";
    }
    ss << expressions_.back()->ToString() << std::endl;
    return ss.str();
}

}


