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

#include "embedding_type.h"

namespace infinity {

#pragma pack(1)

struct OnDiskTensorFormat {
    uint64_t is_in_mem_ : 1 = 0;
    uint64_t embedding_num_ : 15 = 0;
    uint64_t chunk_id_ : 16 = 0;
    uint64_t chunk_offset_ : 32 = 0;
};

struct InMemTensorFormat {
    uint64_t is_in_mem_ : 1 = 1;
    uint64_t new_allocated_ : 1 = 0;
    uint64_t ptr_val_ : 62 = 0;
};

static_assert(sizeof(OnDiskTensorFormat) == sizeof(uint64_t));
static_assert(sizeof(InMemTensorFormat) == sizeof(uint64_t));

struct TensorType {
    union {
        OnDiskTensorFormat on_disk_val_;
        InMemTensorFormat in_mem_val_;
    };

    char *GetMemPtr() const;

    [[nodiscard]] static std::string
    Tensor2String(const TensorType &tensor, EmbeddingDataType type, size_t embedding_dimension, size_t embedding_num);

    TensorType(char *&&from_ptr, bool new_alllocated);

    TensorType(EmbeddingDataType type, size_t embedding_dimension, size_t embedding_num);

    ~TensorType();

    TensorType(const TensorType &other) = delete;

    TensorType(TensorType &&other) noexcept;

    TensorType &operator=(const TensorType &other) = delete;

    TensorType &operator=(TensorType &&other) noexcept;

    void Init(const void *ptr, size_t size);

    bool operator==(const TensorType &other) const = delete;

    bool operator!=(const TensorType &other) const = delete;

    void Reset();

    void SetNull();

    [[nodiscard]] std::string ToString() const;
};

static_assert(sizeof(TensorType) == sizeof(uint64_t));

#pragma pack()

} // namespace infinity
