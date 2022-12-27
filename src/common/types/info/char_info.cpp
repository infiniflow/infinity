//
// Created by jinhai on 22-12-27.
//

#include "char_info.h"

namespace infinity {

SharedPtr<CharInfo>
CharInfo::Make(i64 limit) {
    TypeAssert(limit >= 1 && limit <= MAX_CHAR_SIZE,
               "Char type length can't be less than 1 or larger than 256.")
    return MakeShared<CharInfo>(limit);
}

SharedPtr<CharInfo>
CharInfo::Make() {
    return MakeShared<CharInfo>(MAX_VARCHAR_SIZE);
}

bool
CharInfo::operator==(const TypeInfo& other) const {
    if(other.type() != TypeInfoType::kChar) return false;

    auto* varchar_info_ptr = (CharInfo*)(&other);

    return this->length_limit_ == varchar_info_ptr->length_limit_;
}

}
