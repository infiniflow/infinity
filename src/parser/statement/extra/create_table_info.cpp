//
// Created by jinhai on 23-2-25.
//

#include "create_table_info.h"

namespace infinity {

static String
ConstrainTypeToString(ConstraintType type) {
    switch(type) {
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


String
ColumnDef::ToString() const {
    std::stringstream ss;
    ss << "(" << name_ << " " << column_type_.ToString();
    for(auto& constraint: constraints_) {
        ss << " " << ConstrainTypeToString(constraint);
    }
    ss << ")";
    return ss.str();
}

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

