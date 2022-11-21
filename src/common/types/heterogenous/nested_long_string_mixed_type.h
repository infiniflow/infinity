//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include "base_mixed_type.h"

namespace infinity {

struct __attribute__((packed)) NestedLongStrMixedType : public BaseMixedType {

public:
    inline void
    Reset() {
        if(ptr != nullptr) {
            delete[] ptr;
            ptr = nullptr;
            length = 0;
            array_index = 0;
        }
    }

    ~NestedLongStrMixedType() {
        Reset();
    }

public:
    NestedLongStrMixedType() : BaseMixedType(MixedValueType::kNestedLongStr) {}
    u16 array_index{};

    // 3 bytes is used to store the header of the string to shortcut some computation.
    char_t header[BaseMixedType::LONG_NESTED_STR_HEADER] {};

    u16 length {0};   // Max string length will be 65535
    ptr_t ptr {nullptr};
};

}
