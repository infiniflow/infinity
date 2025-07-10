// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

void BitmapInfo::Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer) const {
    writer.Key("length_limit");
    writer.Uint64(length_limit_);
}

} // namespace infinity
