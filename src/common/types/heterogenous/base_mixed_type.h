//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include "mixed_value_type.h"
#include "common/types/internal_types.h"

namespace infinity {

struct BaseMixedType {

// static member variable
public:
    static constexpr i64 ELEMENT_SIZE = 16;
    static constexpr i64 TUPLE_SIZE = ELEMENT_SIZE * 2;
    static constexpr i64 SHORT_STR_LIMIT = 14;
    static constexpr i64 LONG_STR_HEADER = 5;

public:
    static String
    GetTypeName(MixedValueType type) noexcept;

public:

    BaseMixedType() = default;

    explicit
    BaseMixedType(MixedValueType value_type): type(value_type) {}

    MixedValueType type;
};

}