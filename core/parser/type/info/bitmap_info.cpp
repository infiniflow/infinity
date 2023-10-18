//
// Created by jinhai on 23-2-27.
//

#include "bitmap_info.h"
#include "default_value.h"
#include "parser_assert.h"
#include <memory>

namespace infinity {

std::shared_ptr<BitmapInfo> BitmapInfo::Make(int64_t limit) {
    ParserAssert(limit % 8 == 0, "Bitmap size should be the times of eight.");
    ParserAssert(limit >= 1 && limit <= MAX_BITMAP_SIZE_INTERNAL,
                 "Bitmap type length can't be less than 1 or larger than " + std::to_string(MAX_BITMAP_SIZE_INTERNAL));
    return std::make_shared<BitmapInfo>(limit);
}

std::shared_ptr<BitmapInfo> BitmapInfo::Make() { return std::make_shared<BitmapInfo>(MAX_BITMAP_SIZE_INTERNAL); }

bool BitmapInfo::operator==(const TypeInfo &other) const {
    if (other.type() != TypeInfoType::kBitmap)
        return false;

    auto *varchar_info_ptr = (BitmapInfo *)(&other);

    return this->length_limit_ == varchar_info_ptr->length_limit_;
}

nlohmann::json BitmapInfo::Serialize() const {
    nlohmann::json res;
    res["length_limit"] = length_limit_;
    return res;
}

} // namespace infinity
