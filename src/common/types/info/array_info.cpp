//
// Created by JinHai on 2022/10/28.
//

#include "array_info.h"

namespace infinity {

bool
ArrayInfo::operator==(const TypeInfo& other) const {
    if(other.type() != TypeInfoType::kArray) return false;

    auto* array_info_ptr = (ArrayInfo*)(&other);

    return this->elem_type_ == array_info_ptr->elem_type_;
}

nlohmann::json
ArrayInfo::Serialize() const {
    NotImplementError("ArrayInfo::Serialize")
}

}
