//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "common/types/varlen_type.h"

namespace infinity {

enum class EmbeddingDataType {
    kBit,
    kInt8,
    kInt16,
    kInt32,
    kInt64,
    kFloat,
    kDouble,
    kInvalid
};

using EmbeddingType = ptr_t;

}

