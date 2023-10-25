//
// Created by jinhai on 23-2-25.
//

#include "create_table_info.h"
#include <sstream>

namespace infinity {

CreateTableInfo::~CreateTableInfo() {
    for (auto *column_def : column_defs_) {
        delete column_def;
    }

    for (auto *table_constraint : constraints_) {
        delete table_constraint;
    }

    if (select_ != nullptr) {
        delete select_;
    }
}

std::string CreateTableInfo::ToString() const {
    std::stringstream ss;
    return ss.str();
}

std::string TableConstraint::ToString() const {
    std::stringstream ss;
    ss << ConstrainTypeToString(constraint_) << "(";
    size_t name_count = names_ptr_->size();
    if (name_count > 0) {
        for (size_t idx = 0; idx < name_count - 1; ++idx) {
            ss << names_ptr_->at(idx) << ", ";
        }
        ss << names_ptr_->back();
    } else {
        ParserError("Table constraint without any columns involved.")
    }
    ss << ")";
    return ss.str();
}

} // namespace infinity
