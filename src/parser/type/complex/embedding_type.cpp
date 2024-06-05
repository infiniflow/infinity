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

namespace {

template <typename T>
void Embedding2JsonInternal(const EmbeddingType &embedding, size_t dimension, nlohmann::json& embedding_json) {
    for (size_t i = 0; i < dimension; ++i) {
        embedding_json.push_back(((T *)(embedding.ptr))[i]);
    }
}

void BitmapEmbedding2JsonInternal(const EmbeddingType &embedding, size_t dimension, nlohmann::json& embedding_json) {
    ParserAssert(dimension % 8 == 0, "Binary embedding dimension should be the times of 8.");
    auto *array = reinterpret_cast<const uint8_t *>(embedding.ptr);
    for (size_t i = 0; i < dimension / 8; ++i) {
        const uint8_t byte = array[i];
        for (size_t j = 0; j < 8; ++j) {
            int8_t elem = (byte & (1 << j)) ? 1 : 0;
            embedding_json.push_back(elem);
        }
    }
}

}

size_t EmbeddingType::embedding_type_width[] = {
    0, // bit
    1, // int8
    2, // int16
    4, // int32
    8, // int64
    4, // float32
    8, // double64
};

void EmbeddingType::Init(const void *from_ptr, size_t size) {
    // User need to guarantee the size is matched.
    memcpy(ptr, from_ptr, size);
}


void EmbeddingType::Embedding2Json(const EmbeddingType &embedding, EmbeddingDataType type, size_t dimension, nlohmann::json& embedding_json) {
    switch (type) {
        case kElemBit: {
            BitmapEmbedding2JsonInternal(embedding, dimension, embedding_json);
            break;
        }
        case kElemInt8: {
            Embedding2JsonInternal<int8_t>(embedding, dimension, embedding_json);
            break;
        }
        case kElemInt16: {
            Embedding2JsonInternal<int16_t>(embedding, dimension, embedding_json);
            break;
        }
        case kElemInt32: {
            Embedding2JsonInternal<int32_t>(embedding, dimension, embedding_json);
            break;
        }
        case kElemInt64: {
            Embedding2JsonInternal<int64_t>(embedding, dimension, embedding_json);
            break;
        }
        case kElemFloat: {
            Embedding2JsonInternal<float>(embedding, dimension, embedding_json);
            break;
        }
        case kElemDouble: {
            Embedding2JsonInternal<double>(embedding, dimension, embedding_json);
            break;
        }
        default: {
            ParserError("Unexpected embedding type");
        }
    }
}

} // namespace infinity
