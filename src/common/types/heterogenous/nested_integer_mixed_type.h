//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include "base_mixed_type.h"

namespace infinity {

// Nested in array data type
struct __attribute__((packed)) NestedIntegerMixedType : public BaseMixedType {
    NestedIntegerMixedType() : BaseMixedType(MixedValueType::kNestedInteger) {}
    u16 array_index{};

    i8 _dummy1{};
    i32 _dummy2{};
    i64 value {0};
};

}
