//
// Created by jinhai on 22-12-25.
//

#include "table_def.h"

namespace infinity {

String
TableDef::ToString() const {
    std::stringstream ss;
    ss << name_ << '[';
    for(size_t i = 0; auto& column_ptr: this->columns_) {
        ss << column_ptr->ToString();
        ++ i;
        if(i != this->column_count()) {
            ss << ", ";
        }
    }
    ss << ']';
    return ss.str();
}

}