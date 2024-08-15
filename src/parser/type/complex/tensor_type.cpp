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
#include <iostream>
#include <sstream>

namespace infinity {

std::string TensorType::Tensor2String(char *tensor_ptr, const EmbeddingDataType type, const size_t embedding_dimension, const size_t embedding_num) {
    const size_t embedding_size = EmbeddingType::EmbeddingSize(type, embedding_dimension);
    std::ostringstream oss;
    // first embedding
    EmbeddingType embedding(nullptr, false);
    embedding.ptr = tensor_ptr;
    oss << '[' << EmbeddingType::Embedding2String(embedding, type, embedding_dimension);
    for (size_t i = 1; i < embedding_num; ++i) {
        // next embedding
        embedding.ptr = tensor_ptr + i * embedding_size;
        oss << ',' << EmbeddingType::Embedding2String(embedding, type, embedding_dimension);
    }
    oss << ']';
    return std::move(oss).str();
}

} // namespace infinity
