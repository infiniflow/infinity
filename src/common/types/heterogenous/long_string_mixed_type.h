//
// Created by JinHai on 2022/11/20.
//


#pragma once

#include "base_mixed_type.h"
#include "main/logger.h"

namespace infinity {

struct __attribute__((packed)) LongStrMixedType : public BaseMixedType {
public:
    LongStrMixedType() : BaseMixedType(MixedValueType::kLongStr) {}

    LongStrMixedType(const LongStrMixedType& from) = delete;
    LongStrMixedType(LongStrMixedType&& from) = delete;
    LongStrMixedType&
    operator=(const LongStrMixedType& from) = delete;
    LongStrMixedType&
    operator=(LongStrMixedType&& from) = delete;

    inline void
    Reset() {
        if(ptr != nullptr) {
            LOG_TRACE("Release the long str ptr");
            delete[] ptr;
            ptr = nullptr;
            length = 0;
        }
    }

    ~LongStrMixedType() {
        Reset();
    }
public:
    char_t header[BaseMixedType::LONG_STR_HEADER]{}; // 5 bytes is used to store the header of the string to shortcut some computation.
    u16 length{0};  // Max string length will be 65535
    ptr_t ptr{nullptr};
};

}
