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

namespace infinity {

struct __attribute__((packed)) ShortStrMixedType : public BaseMixedType {
public:
    ShortStrMixedType() : BaseMixedType(MixedValueType::kShortStr) {}

    ShortStrMixedType(const ShortStrMixedType &from) = delete;

    ShortStrMixedType(ShortStrMixedType &&from) = delete;

    ShortStrMixedType &operator=(const ShortStrMixedType &from) = delete;

    ShortStrMixedType &operator=(ShortStrMixedType &&from) = delete;

    int8_t length{0};
    char ptr[BaseMixedType::SHORT_STR_LIMIT]{}; // not more than 15 bytes string will use short string type

public:
    bool Equal(const std::string &input);
};

} // namespace infinity
