// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
