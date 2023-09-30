//
// Created by jinhai on 23-3-5.
//

#include "drop_view_info.h"
#include <sstream>

namespace infinity {

String
DropViewInfo::ToString() const {
    std::stringstream ss;
    ss << "DROP VIEW: " << schema_name_ << "." << view_name_;
    if(this->conflict_type_ == ConflictType::kIgnore) {
        ss << " IgnoreIfExists";
    } else {
        ss << " ThrowErrorIfExists";
    }
    return ss.str();
}

}
