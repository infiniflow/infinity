//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include "common/types/alias/primitives.h"

namespace infinity {

enum class MixedValueType : i8 {
    kInvalid,
    // Not nested data type
    kInteger,
    kFloat,
    kLongStr,
    kShortStr,
    kTuple,
    kArray,
    kNull,
    kMissing,

    kDummy,
};

}
