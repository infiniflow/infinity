#include "column_def.h"

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

} // namespace infinity