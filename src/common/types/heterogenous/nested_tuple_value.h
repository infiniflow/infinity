//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include "base_mixed_type.h"

namespace infinity {

struct NestedTupleValue {
public:
    ptr_t parent_ptr;
    MixedType array[0];
private:
    static ptr_t
    Make(i32 count);

    static void
    Set(const String& key, const MixedType& mixed_value);

    static MixedType&
    Get(const String& key);
};


}
