//
// Created by jinhai on 23-2-27.
//

#include "blob_info.h"

namespace infinity {

    SharedPtr<BlobInfo>
    BlobInfo::Make(i64 limit) {
        TypeAssert(limit >= 1 && limit <= MAX_BLOB_SIZE,
                   "Blob type size can't be less than 1 or larger than " + std::to_string(MAX_BLOB_SIZE))
        return MakeShared<BlobInfo>(limit);
    }

    SharedPtr<BlobInfo>
    BlobInfo::Make() {
        return MakeShared<BlobInfo>(MAX_VARCHAR_SIZE);
    }

    bool
    BlobInfo::operator==(const TypeInfo& other) const {
        if(other.type() != TypeInfoType::kBlob) return false;

        auto* varchar_info_ptr = (BlobInfo*)(&other);

        return this->length_limit_ == varchar_info_ptr->length_limit_;
    }

}