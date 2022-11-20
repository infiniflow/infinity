//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include "base_mixed_type.h"

namespace infinity {

struct __attribute__((packed)) TupleMixedType : public BaseMixedType {
    TupleMixedType() : BaseMixedType(MixedValueType::kTuple) {}

    i8 _dummy1{};
    i16 _dummy2{};
    i16 _dummy3{};
    u16 count {0};        // count of the tuple, each tuple will have two MixedType member, one for key, another for value.
    ptr_t ptr {nullptr};  // pointer to mixed type of tuple
};

}

