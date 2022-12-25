//
// Created by jinhai on 22-12-25.
//

#include "column_def.h"

namespace infinity {

static String
ConstrainTypeToString(ConstrainType type) {
    switch(type) {
        case ConstrainType::kInvalid:
            TypeError("Invalid constraint type")
        case ConstrainType::kPrimaryKey:
            return "PrimaryKey";
        case ConstrainType::kUnique:
            return "Unique";
        case ConstrainType::kNull:
            return "Nullable";
        case ConstrainType::kNotNull:
            return "Not nullable";
    }
    TypeError("Unexpected error.")
}

String
ColumnDef::ToString() const {
    std::stringstream ss;
    ss << "( " << name_ << " " << type_.ToString() << " ";
    for(auto& constraint: constrains_) {
        ss << ConstrainTypeToString(constraint) << " ";
    }
    ss << ")";
    return ss.str();
}

}