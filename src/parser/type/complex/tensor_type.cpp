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

#include "tensor_type.h"
#include <cstring>

namespace infinity {

static_assert(std::is_same_v<uintptr_t, uint64_t>, "uintptr_t is not uint64_t");

char *TensorType::GetMemPtr() const {
    if (in_mem_val_.is_in_mem_ == 0) {
        return nullptr;
    }
    const uintptr_t ptr_v = in_mem_val_.ptr_val_;
    return reinterpret_cast<char *>(ptr_v);
}

TensorType::TensorType(char *&&from_ptr, const bool new_alllocated) {
    in_mem_val_.is_in_mem_ = 1;
    in_mem_val_.new_allocated_ = new_alllocated;
    in_mem_val_.ptr_val_ = reinterpret_cast<uintptr_t>(from_ptr);
    from_ptr = nullptr;
}

TensorType::TensorType(const EmbeddingDataType type, const size_t embedding_dimension, const size_t embedding_num) {
    in_mem_val_.is_in_mem_ = 1;
    in_mem_val_.new_allocated_ = 1;
    const size_t embedding_size = EmbeddingType::EmbeddingSize(type, embedding_dimension);
    auto ptr = new char[embedding_size * embedding_num];
    in_mem_val_.ptr_val_ = reinterpret_cast<uintptr_t>(ptr);
}

TensorType::~TensorType() { Reset(); }

TensorType::TensorType(TensorType &&other) noexcept {
    if (other.in_mem_val_.is_in_mem_) {
        in_mem_val_ = other.in_mem_val_;
        other.in_mem_val_.ptr_val_ = 0;
    } else {
        on_disk_val_ = other.on_disk_val_;
    }
}

TensorType &TensorType::operator=(TensorType &&other) noexcept {
    if (this == &other) {
        return *this;
    }
    Reset();
    if (other.in_mem_val_.is_in_mem_) {
        in_mem_val_ = other.in_mem_val_;
        other.in_mem_val_.ptr_val_ = 0;
    } else {
        on_disk_val_ = other.on_disk_val_;
    }
    return *this;
}

void TensorType::Reset() {
    if (in_mem_val_.is_in_mem_ == 0) {
        return;
    }
    if (in_mem_val_.new_allocated_) {
        auto ptr = GetMemPtr();
        delete[] ptr;
    }
    in_mem_val_.ptr_val_ = 0;
}

void TensorType::SetNull() {
    if (in_mem_val_.is_in_mem_) {
        in_mem_val_.ptr_val_ = 0;
    }
}

void TensorType::Init(const void *from_ptr, const size_t size) {
    // User need to guarantee the size is matched.
    auto ptr = GetMemPtr();
    memcpy(ptr, from_ptr, size);
}

std::string
TensorType::Tensor2String(const TensorType &tensor, const EmbeddingDataType type, const size_t embedding_dimension, const size_t embedding_num) {
    const size_t embedding_size = EmbeddingType::EmbeddingSize(type, embedding_dimension);
    std::ostringstream oss;
    // first embedding
    EmbeddingType embedding(nullptr, false);
    auto tenstr_mem_ptr = tensor.GetMemPtr();
    embedding.ptr = tenstr_mem_ptr;
    oss << EmbeddingType::Embedding2String(embedding, type, embedding_dimension);
    for (size_t i = 1; i < embedding_num; ++i) {
        oss << ",\n";
        // next embedding
        embedding.ptr = tenstr_mem_ptr + i * embedding_size;
        oss << EmbeddingType::Embedding2String(embedding, type, embedding_dimension);
    }
    return std::move(oss).str();
}

[[nodiscard]] std::string TensorType::ToString() const {
    ParserError("ToString() isn't implemented");
    return std::string();
}

} // namespace infinity
