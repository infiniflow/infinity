//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "common/types/varlen_type.h"

namespace infinity {

enum EmbeddingDataType : i8 {
    kElemBit,
    kElemInt8,
    kElemInt16,
    kElemInt32,
    kElemInt64,
    kElemFloat,
    kElemDouble,
    kElemInvalid
};

size_t
EmbeddingDataBitWidth(EmbeddingDataType type_index);

using EmbeddingType = ptr_t;

}

