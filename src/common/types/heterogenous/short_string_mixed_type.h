//
// Created by JinHai on 2022/11/20.
//


#pragma once

#include "base_mixed_type.h"

namespace infinity {

struct __attribute__((packed)) ShortStrMixedType : public BaseMixedType {
    ShortStrMixedType() : BaseMixedType(MixedValueType::kShortStr) {}

    char_t ptr[BaseMixedType::SHORT_STR_LIMIT] {};  // not more than 15 bytes string will use short string type
};

}
