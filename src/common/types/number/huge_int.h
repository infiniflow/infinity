//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "common/types/internal_types.h"

namespace infinity {

struct HugeInt {
public:
    HugeInt() = default;

    explicit HugeInt(i64 upper, i64 lower) :
        upper(upper), lower(lower) {}

    bool
    operator==(const HugeInt& other) const {
        return other.upper == upper && other.lower == lower;
    }

    i64 upper{};
    i64 lower{};

public:
    [[nodiscard]] inline String
    ToString() const;
};

String
HugeInt::ToString() const {
    TypeError("Not implemented");
}

}


