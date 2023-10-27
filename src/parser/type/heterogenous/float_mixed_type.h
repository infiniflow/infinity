//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include "base_mixed_type.h"

namespace infinity {

struct __attribute__((packed)) FloatMixedType : public BaseMixedType {
    FloatMixedType() : BaseMixedType(MixedValueType::kFloat) {}

    int8_t _dummy1{};
    int16_t _dummy2{};
    int32_t _dummy3{};
    double value{0};
};

} // namespace infinity
