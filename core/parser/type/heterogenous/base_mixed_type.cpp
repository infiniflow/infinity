//
// Created by JinHai on 2022/11/23.
//

#include "base_mixed_type.h"

namespace infinity {

static const char *type2name[] = {
    "Invalid",
    "Integer",
    "Float",
    "Long std::string",
    "Short std::string",
    "Tuple",
    "Array",
    "Null",
    "Missing",
    "Dummy",
};

std::string BaseMixedType::GetTypeName(MixedValueType type) noexcept {
    int8_t type_id = static_cast<int8_t>(type);
    return type2name[type_id];
}

} // namespace infinity
