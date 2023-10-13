//
// Created by JinHai on 2022/11/23.
//

#include "base_mixed_type.h"

namespace infinity {

static const char *type2name[] = {
    "Invalid",
    "Integer",
    "Float",
    "Long String",
    "Short String",
    "Tuple",
    "Array",
    "Null",
    "Missing",
    "Dummy",
};

String BaseMixedType::GetTypeName(MixedValueType type) noexcept {
    i8 type_id = static_cast<i8>(type);
    return type2name[type_id];
}

} // namespace infinity
