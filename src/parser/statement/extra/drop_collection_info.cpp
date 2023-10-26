//
// Created by jinhai on 23-2-24.
//

#include "drop_collection_info.h"
#include <sstream>

namespace infinity {

std::string DropCollectionInfo::ToString() const {
    std::stringstream ss;
    ss << "DROP COLLECTION: " << schema_name_ << "." << collection_name_;
    if (this->conflict_type_ == ConflictType::kIgnore) {
        ss << " IgnoreIfExists";
    } else {
        ss << " ThrowErrorIfExists";
    }
    return ss.str();
}

} // namespace infinity
