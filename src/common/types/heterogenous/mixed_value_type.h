//
// Created by JinHai on 2022/11/20.
//

#pragma once

namespace infinity {

enum class MixedValueType : i8 {
    // Not nested data type
    kInteger,
    kFloat,
    kLongStr,
    kShortStr,
    kTuple,
    kArray,
    kNull,
    kMissing,

    // Nested in array data type
    kNestedMissing,
    kNestedInteger,
    kNestedFloat,
    kNestedLongStr,
    kNestedShortStr,
    kNestedTuple,
    kNestedArray,
    kNestedNull,

    kDummy,
};

}
