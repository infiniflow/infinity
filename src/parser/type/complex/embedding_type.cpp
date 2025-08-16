// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "embedding_type.h"

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std;
import std.compat;
#endif

#ifndef PARESER_USE_THIRD_PARTY_MODULE
#define PARESER_USE_THIRD_PARTY_MODULE 1
import third_party;
#endif

namespace infinity {

bool operator==(const EmbeddingDataType &type, const arrow::Type::type &arrow_type) {
    switch (type) {
        case EmbeddingDataType::kElemBit:
            return arrow_type == arrow::Type::BOOL;
        case EmbeddingDataType::kElemUInt8:
            return arrow_type == arrow::Type::UINT8;
        case EmbeddingDataType::kElemInt8:
            return arrow_type == arrow::Type::INT8;
        case EmbeddingDataType::kElemInt16:
            return arrow_type == arrow::Type::INT16;
        case EmbeddingDataType::kElemInt32:
            return arrow_type == arrow::Type::INT32;
        case EmbeddingDataType::kElemInt64:
            return arrow_type == arrow::Type::INT64;
        case EmbeddingDataType::kElemFloat:
            return arrow_type == arrow::Type::FLOAT;
        case EmbeddingDataType::kElemFloat16:
            return arrow_type == arrow::Type::HALF_FLOAT;
        case EmbeddingDataType::kElemDouble:
            return arrow_type == arrow::Type::DOUBLE;
        default:
            return false;
    }
}

bool operator!=(const EmbeddingDataType &type, const arrow::Type::type &arrow_type) { return !(type == arrow_type); }

size_t EmbeddingType::embedding_type_width[] = {
    0, // bit
    1, // int8
    2, // int16
    4, // int32
    8, // int64
    4, // float32
    8, // double64
    1, // uint8
    2, // float16
    2, // bfloat16
};

static_assert(std::size(EmbeddingType::embedding_type_width) == static_cast<size_t>(EmbeddingDataType::kElemInvalid),
              "embedding_type_width size should be equal to EmbeddingDataType enum num.");

void EmbeddingType::Init(const void *from_ptr, size_t size) {
    // User need to guarantee the size is matched.
    memcpy(ptr, from_ptr, size);
}

} // namespace infinity
