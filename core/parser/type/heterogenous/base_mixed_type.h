//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include "mixed_value_type.h"
#include <string>

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

    MixedValueType type;
};

} // namespace infinity