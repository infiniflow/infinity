//
// Created by JinHai on 2022/11/20.
//

#pragma once

#include <cstdint>

namespace infinity {

enum class MixedValueType : int8_t {
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
