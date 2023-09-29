//
// Created by jinhai on 23-2-27.
//

#include "bitmap_info.h"

namespace infinity {

SharedPtr<BitmapInfo>
BitmapInfo::Make(i64 limit) {
    TypeAssert(limit % 8 == 0, "Bitmap size should be the times of eight.")
    TypeAssert(limit >= 1 && limit <= MAX_BITMAP_SIZE,
               "Bitmap type length can't be less than 1 or larger than " + std::to_string(MAX_BITMAP_SIZE))
    return MakeShared<BitmapInfo>(limit);
}

SharedPtr<BitmapInfo>
BitmapInfo::Make() {
    return MakeShared<BitmapInfo>(MAX_VARCHAR_SIZE);
}

bool
BitmapInfo::operator==(const TypeInfo& other) const {
    if(other.type() != TypeInfoType::kBitmap)
        return false;

    auto* varchar_info_ptr = (BitmapInfo*)(&other);

    return this->length_limit_ == varchar_info_ptr->length_limit_;
}

nlohmann::json
BitmapInfo::Serialize() const {
    nlohmann::json res;
    res["length_limit"] = length_limit_;
    return res;
}

}
