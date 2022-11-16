//
// Created by JinHai on 2022/10/31.
//

#pragma once

#include "common/types/internal_types.h"

namespace infinity {

struct UuidType {
public:
    static constexpr i64 LENGTH = 16;

public:
    i8 body[16];

    inline void
    Reset() {
        memset(body, 0, LENGTH);
    }
};

}
