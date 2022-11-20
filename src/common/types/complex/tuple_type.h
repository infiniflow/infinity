//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "common/types/varlen_type.h"

namespace infinity {

// Represent a tuple.
struct TupleType {

    i16 count;       // count of the tuple fields
    ptr_t value_ptr; // pointer to value list

    inline void
    Reset() {}
};

}
