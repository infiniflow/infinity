//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include "base_mixed_type.h"

namespace infinity {

struct __attribute__((packed)) FloatMixedType : public BaseMixedType {
    FloatMixedType() : BaseMixedType(MixedValueType::kFloat) {}

    i8 _dummy1{};
    i16 _dummy2{};
    i32 _dummy3{};
    f64 value{0};
};

}
