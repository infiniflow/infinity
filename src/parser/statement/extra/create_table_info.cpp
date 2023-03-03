//
// Created by jinhai on 23-2-25.
//

#include "create_table_info.h"

namespace infinity {

CreateTableInfo::~CreateTableInfo() {
    for(auto* column_def: column_defs_) {
        delete column_def;
    }

    for(auto* table_constraint: constraints_) {
        delete table_constraint;
    }

    if(select_ != nullptr) {
        delete select_;
    }
}

String
CreateTableInfo::ToString() const {
    std::stringstream ss;
    return ss.str();
}


}

