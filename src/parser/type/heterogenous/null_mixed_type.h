//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include "base_mixed_type.h"

namespace infinity {

struct __attribute__((packed)) NullMixedType : public BaseMixedType {
    NullMixedType() : BaseMixedType(MixedValueType::kNull) {}

    int8_t _dummy1{};
    int16_t _dummy2{};
    int32_t _dummy3{};
    int64_t _dummy4{};
};

} // namespace infinity
