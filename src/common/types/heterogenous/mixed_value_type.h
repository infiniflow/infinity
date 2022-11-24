//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include "common/types/internal_types.h"

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
