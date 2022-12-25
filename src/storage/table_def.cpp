//
// Created by jinhai on 22-12-25.
//

#include "table_def.h"

namespace infinity {

String
TableDef::ToString() const {
    std::stringstream ss;
    ss << "Table name: " << name_ << std::endl;
    for(auto& column_ptr: this->columns_) {
        ss << column_ptr->ToString() << std::endl;
    }
    return ss.str();
}

}