//
// Created by jinhai on 23-2-23.
//

#include "create_collection_info.h"

namespace infinity {

String
CreateCollectionInfo::ToString() const {
    std::stringstream ss;
    ss << "CREATE COLLECTION: " << schema_name_ << "." << collection_name_;
    if (this->conflict_type_ == ConflictType::kIgnore) {
        ss << " IgnoreIfNotExists";
    } else {
        ss << " ThrowErrorIfNotExists";
    }
    return ss.str();
}

}
