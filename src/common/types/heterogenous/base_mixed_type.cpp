//
// Created by JinHai on 2022/11/23.
//

#include "base_mixed_type.h"
#include "main/logger.h"

namespace infinity {

static const char* type2name[] = {
        "Invalid",
        "Integer",
        "Float",
        "Long String",
        "Short String",
        "Tuple",
        "Array",
        "Null",
        "Missing",
        "Nested Missing",
        "Nested Integer",
        "Nested Float",
        "Nested Long String",
        "Nested Short String",
        "Nested Tuple",
        "Nested Array",
        "Nested Null",
        "Dummy",
};

String
BaseMixedType::GetTypeName(MixedValueType type) noexcept {
    i8 type_id = static_cast<i8>(type);
    return type2name[type_id];
}

}