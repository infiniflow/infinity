//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include "base_mixed_type.h"

namespace infinity {

struct __attribute__((packed)) NestedTupleMixedType : public BaseMixedType {
    NestedTupleMixedType() : BaseMixedType(MixedValueType::kNestedObject) {}
    i8 _dummy1{};
    u16 array_index{};
    i16 _dummy2{};
    u16 count{0};
    ptr_t ptr{nullptr};
};

}
