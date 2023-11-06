//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include "base_mixed_type.h"
#include <cstdint>

namespace infinity {

struct __attribute__((packed)) LongStrMixedType : public BaseMixedType {
public:
    LongStrMixedType() : BaseMixedType(MixedValueType::kLongStr) {}

    LongStrMixedType(const LongStrMixedType &from) = delete;

    LongStrMixedType(LongStrMixedType &&from) = delete;

    LongStrMixedType &operator=(const LongStrMixedType &from) = delete;

    LongStrMixedType &operator=(LongStrMixedType &&from) = delete;

public:
    bool Equal(const std::string &input);

    inline void Reset(bool in_constructor = false) {
        if (in_constructor) {
            ptr = nullptr;
            length = 0;
            return;
        }
        if (ptr != nullptr) {
            //            LOG_TRACE("Release the long str ptr");
            delete[] ptr;

            ptr = nullptr;
            length = 0;
        }
    }

    ~LongStrMixedType() { Reset(); }

public:
    char header[BaseMixedType::LONG_STR_HEADER]{}; // 5 bytes is used to store the header of the string to shortcut some computation.
    uint16_t length{0};                            // Max string length will be 65535
    char *ptr{nullptr};
};

} // namespace infinity
