//
// Created by JinHai on 2022/9/26.
//

#include "logical_project.h"
#include <sstream>

namespace infinity {

Vector<ColumnBinding>
LogicalProject::GetColumnBindings() const {
    Vector<ColumnBinding> result;
    SizeT expression_count = expressions_.size();
    result.reserve(expression_count);
    for(SizeT i = 0; i < expression_count; ++ i) {
        result.emplace_back(table_index_, i);
    }
    return result;
}

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
        ss << expressions_[i]->Name() << ", ";
    }
    ss << expressions_.back()->Name();
    space += arrow_str.size();

    return ss.str();
}

}


