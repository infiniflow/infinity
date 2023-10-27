//
// Created by JinHai on 2022/11/20.
//

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
