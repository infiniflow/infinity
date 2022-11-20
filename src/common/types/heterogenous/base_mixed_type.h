//
// Created by JinHai on 2022/11/20.
//

#pragma once

namespace infinity {

struct BaseMixedType {

// static member variable
public:
    static constexpr i64 ELEMENT_SIZE = 16;
    static constexpr i64 SHORT_STR_LIMIT = 15;
    static constexpr i64 SHORT_NESTED_STR_LIMIT = 13;
    static constexpr i64 LONG_STR_HEADER = 5;
    static constexpr i64 LONG_NESTED_STR_HEADER = 3;

public:

    BaseMixedType() = default;

    explicit
    BaseMixedType(MixedValueType value_type): type(value_type) {}

    MixedValueType type;
};

}