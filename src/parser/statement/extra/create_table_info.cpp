//
// Created by jinhai on 23-2-25.
//

#include "create_table_info.h"

namespace infinity {

String ConstrainTypeToString(ConstraintType type) {
    switch (type) {
        case ConstraintType::kPrimaryKey:
            return "PrimaryKey";
        case ConstraintType::kUnique:
            return "Unique";
        case ConstraintType::kNull:
            return "Nullable";
        case ConstraintType::kNotNull:
            return "Not nullable";
    }
    TypeError("Unexpected error.")
}

String ColumnDef::ToString() const {
    std::stringstream ss;
    ss << name_ << " " << column_type_->ToString();
    for (auto &constraint : constraints_) {
        ss << " " << ConstrainTypeToString(constraint);
    }
    return ss.str();
}

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

String CreateTableInfo::ToString() const {
    std::stringstream ss;
    return ss.str();
}

String TableConstraint::ToString() const {
    std::stringstream ss;
    ss << ConstrainTypeToString(constraint_) << "(";
    SizeT name_count = names_ptr_->size();
    if (name_count > 0) {
        for (SizeT idx = 0; idx < name_count - 1; ++idx) {
            ss << names_ptr_->at(idx) << ", ";
        }
        ss << names_ptr_->back();
    } else {
        TypeError("Table constraint without any columns involved.")
    }
    ss << ")";
    return ss.str();
}

} // namespace infinity
