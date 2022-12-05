//
// Created by JinHai on 2022/10/28.
//

#include "varchar_info.h"

namespace infinity {

SharedPtr<VarcharInfo>
VarcharInfo::Make(i64 limit) {
    TypeAssert(limit >= 65 && limit <= MAX_VARCHAR_SIZE,
               "Varchar length can't be less than 65 or larger than 65535.")
    return MakeShared<VarcharInfo>(limit);
}

SharedPtr<VarcharInfo>
VarcharInfo::Make() {
    return MakeShared<VarcharInfo>(MAX_VARCHAR_SIZE);
}

bool
VarcharInfo::operator==(const TypeInfo& other) const {
    if(other.type() != TypeInfoType::kVarchar) return false;

    auto* varchar_info_ptr = (VarcharInfo*)(&other);

    return this->length_limit_ == varchar_info_ptr->length_limit_;
}

}
