//
// Created by JinHai on 2022/11/7.
//

#include "common/types/internal_types.h"

namespace infinity {

struct TimestampType {
    i32 date {};
    i32 time {};

    inline void
    Reset() {
        date = 0;
        time = 0;
    }
};

}
