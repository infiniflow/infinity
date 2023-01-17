//
// Created by JinHai on 2022/9/26.
//

#include "logical_project.h"
#include <sstream>

namespace infinity {

String
LogicalProject::ToString(i64& space) {
    std::stringstream ss;
    String arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Project on: ";
    size_t expression_count = expressions_.size();
    for(size_t i = 0; i < expression_count - 1; ++ i) {
        ss << expressions_[i]->ToString() << ", ";
    }
    ss << expressions_.back()->ToString();
    space += arrow_str.size();

    return ss.str();
}

}


