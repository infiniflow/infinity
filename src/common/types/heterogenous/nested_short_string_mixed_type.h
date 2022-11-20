//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include "base_mixed_type.h"

namespace infinity {

struct __attribute__((packed)) NestedShortStrMixedType : public BaseMixedType {
    NestedShortStrMixedType() : BaseMixedType(MixedValueType::kNestedShortStr) {}
    u16 array_index{};

    // not more than 13 bytes string will use short string type
    char_t ptr[BaseMixedType::SHORT_NESTED_STR_LIMIT] {};
};

}
