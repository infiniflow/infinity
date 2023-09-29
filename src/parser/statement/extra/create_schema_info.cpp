//
// Created by jinhai on 23-2-23.
//

#include "create_schema_info.h"

namespace infinity {

String
CreateSchemaInfo::ToString() const {
    std::stringstream ss;
    ss << "CREATE SCHEMA: " << schema_name_;
    if(this->conflict_type_ == ConflictType::kIgnore) {
        ss << " IgnoreIfNotExists";
    } else {
        ss << " ThrowErrorIfNotExists";
    }
    return ss.str();
}

}
