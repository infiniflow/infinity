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

#pragma once

#include <span>
#include "embedding_type.h"

namespace infinity {

#pragma pack(1)

struct TensorArrayType {
    uint64_t tensor_num_ : 16 = 0;
    uint64_t file_offset_ : 48 = 0;

    [[nodiscard]] static std::string
    TensorArray2String(const std::vector<std::pair<std::span<const char>, uint64_t>> &raw_data, EmbeddingDataType type, size_t embedding_dimension);
};

static_assert(sizeof(TensorArrayType) == sizeof(uint64_t));

#pragma pack()

} // namespace infinity
