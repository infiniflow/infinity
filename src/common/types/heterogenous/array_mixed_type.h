//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include "base_mixed_type.h"

namespace infinity {

struct __attribute__((packed)) ArrayMixedType : public BaseMixedType {

public:
    void
    Reset(bool in_constructor = false);

    ~ArrayMixedType() {
        Reset();
    }

public:
    ArrayMixedType() : BaseMixedType(MixedValueType::kArray) {}

    i8 _dummy1{};
    i16 _dummy2{};
    i16 _dummy3{};
    u16 count{0};        // Array count, which limit is 65535;
    ptr_t ptr{nullptr};  // pointer to child mixed type
};

}
