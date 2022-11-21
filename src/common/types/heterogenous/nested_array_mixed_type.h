//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include "base_mixed_type.h"

namespace infinity {

struct __attribute__((packed)) NestedArrayMixedType : public BaseMixedType {
public:
    NestedArrayMixedType() : BaseMixedType(MixedValueType::kNestedArray) {}
    inline void
    Reset() {
        if(ptr != nullptr) {
            delete[] ptr;
            ptr = nullptr;
            count = 0;
            array_index = 0;
        }
    }

    ~NestedArrayMixedType() {
        Reset();
    }

public:
    i8 _dummy{};
    u16 array_index{};
    i16 _dummy1{};
    u16 count {0};
    ptr_t ptr {nullptr};
};

}
