//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include "base_mixed_type.h"

namespace infinity {

struct __attribute__((packed)) NestedFloatMixedType : public BaseMixedType {
    NestedFloatMixedType() : BaseMixedType(MixedValueType::kNestedFloat) {}
    u16 array_index{};

    i8 _dummy1{};
    i32 _dummy2{};
    f64 value {0};
};

}

