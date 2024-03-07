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

struct __attribute__((packed)) ArrayMixedType : public BaseMixedType {

public:
    void Reset(bool in_constructor = false);

    ~ArrayMixedType() { Reset(); }

public:
    ArrayMixedType() : BaseMixedType(MixedValueType::kArray) {}

    int8_t _dummy1{};
    int16_t _dummy2{};
    int16_t _dummy3{};
    uint16_t count{0};  // Array count, which limit is 65535;
    char *ptr{nullptr}; // pointer to child mixed type
};

} // namespace infinity
