//
// Created by jinhai on 23-2-24.
//

#include "drop_schema_info.h"

namespace infinity {

String
DropSchemaInfo::ToString() const {
    std::stringstream ss;
    ss << "DROP SCHEMA: " << schema_name_;
    if(this->conflict_type_ == ConflictType::kIgnore) {
        ss << " IgnoreIfExists";
    } else {
        ss << " ThrowErrorIfExists";
    }
    return ss.str();
}

}
