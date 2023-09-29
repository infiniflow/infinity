//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include "base_mixed_type.h"

namespace infinity {

// Not nested data type
struct __attribute__((packed)) IntegerMixedType : public BaseMixedType {
    IntegerMixedType() : BaseMixedType(MixedValueType::kInteger) {}

    i8 _dummy1{};
    i16 _dummy2{};
    i32 _dummy3{};
    i64 value{0};
};

}