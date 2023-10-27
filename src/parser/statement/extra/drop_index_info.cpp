//
// Created by jinhai on 23-3-5.
//

#include "drop_index_info.h"
#include <sstream>

namespace infinity {

std::string DropIndexInfo::ToString() const {
    std::stringstream ss;
    ss << "DROP INDEX: " << index_name_;
    if (this->conflict_type_ == ConflictType::kIgnore) {
        ss << " IgnoreIfNotExists";
    } else {
        ss << " ThrowErrorIfNotExists";
    }
    return ss.str();
}

} // namespace infinity
