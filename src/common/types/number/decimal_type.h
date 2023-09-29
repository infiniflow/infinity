//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "common/types/logical_type.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

struct DecimalType {
public:
    DecimalType() = default;

    DecimalType(i64 val1, i64 val2) : upper(val1), lower(val2) {}

    bool
    operator==(const DecimalType& other) const {
        if(this == &other)
            return true;
        return other.upper == upper && other.lower == lower;
    }

    inline void
    Reset() {
        upper = 0;
        lower = 0;
    }

    [[nodiscard]] inline String
    ToString() const {
        TypeError("ToString() isn't implemented");
    }

    i64 upper{0};
    i64 lower{0};
};

}
