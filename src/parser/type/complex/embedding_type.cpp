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
#include <cstring>

namespace infinity {

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
