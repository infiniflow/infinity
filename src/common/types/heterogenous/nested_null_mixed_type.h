//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include "base_mixed_type.h"

namespace infinity {

struct __attribute__((packed)) NestedNullMixedType : public BaseMixedType {
    NestedNullMixedType() : BaseMixedType(MixedValueType::kNestedNull) {}
    u16 array_index{};

    i8 _dummy1{};
    i32 _dummy2{};
    i64 _dummy3{};
};

}
