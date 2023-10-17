//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include "base_mixed_type.h"

namespace infinity {

struct __attribute__((packed)) TupleMixedType : public BaseMixedType {

public:
    TupleMixedType() : BaseMixedType(MixedValueType::kTuple) {}

    void Reset(bool in_constructor = false);

    ~TupleMixedType() { Reset(); }

public:
    int8_t _dummy1{};
    int16_t _dummy2{};
    int16_t _dummy3{};
    uint16_t count{0};       // count of the tuple, each tuple will have two MixedType member, one for key, another for value.
    char* ptr{nullptr}; // pointer to mixed type of tuple
};

} // namespace infinity
