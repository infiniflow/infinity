//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include "base_mixed_type.h"

namespace infinity {

struct __attribute__((packed)) MissingMixedType : public BaseMixedType {
    MissingMixedType() : BaseMixedType(MixedValueType::kMissing) {}

    i8 _dummy1{};
    i16 _dummy2{};
    i32 _dummy3{};
    i64 _dummy4{};
};

} // namespace infinity
