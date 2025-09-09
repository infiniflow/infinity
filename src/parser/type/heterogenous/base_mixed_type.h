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

#include "mixed_value_type.h"

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std.compat;
#endif

namespace infinity {

struct BaseMixedType {

    // static member variable
public:
    static constexpr int64_t ELEMENT_SIZE = 16;
    static constexpr int64_t TUPLE_SIZE = ELEMENT_SIZE * 2;
    static constexpr int64_t SHORT_STR_LIMIT = 14;
    static constexpr int64_t LONG_STR_HEADER = 5;

public:
    static std::string GetTypeName(MixedValueType type) noexcept;

public:
    BaseMixedType() = default;

    explicit BaseMixedType(MixedValueType value_type) : type(value_type) {}

    MixedValueType type{MixedValueType::kInvalid};
};

} // namespace infinity