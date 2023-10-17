//
// Created by jinhai on 23-2-27.
//

#include "bitmap_info.h"
#include "parser_assert.h"
#include "default_value.h"
#include <memory>

namespace infinity {

std::shared_ptr<BitmapInfo> BitmapInfo::Make(int64_t limit) {
    ParserAssert(limit % 8 == 0, "Bitmap size should be the times of eight.");
    ParserAssert(limit >= 1 && limit <= MAX_BITMAP_SIZE,
               "Bitmap type length can't be less than 1 or larger than " + std::to_string(MAX_BITMAP_SIZE)) return std::make_shared<BitmapInfo>(limit);
}

std::shared_ptr<BitmapInfo> BitmapInfo::Make() { return std::make_shared<BitmapInfo>(MAX_BITMAP_SIZE); }

bool BitmapInfo::operator==(const TypeInfo &other) const {
    if (other.type() != TypeInfoType::kBitmap)
        return false;

    auto *varchar_info_ptr = (BitmapInfo *)(&other);

    return this->length_limit_ == varchar_info_ptr->length_limit_;
}

} // namespace infinity
