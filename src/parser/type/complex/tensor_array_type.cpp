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

#include "tensor_array_type.h"
#include "tensor_type.h"
#include <sstream>

namespace infinity {
std::string TensorArrayType::TensorArray2String(const std::vector<std::pair<std::span<const char>, size_t>> &raw_data,
                                                EmbeddingDataType type,
                                                size_t embedding_dimension) {
    std::ostringstream oss;
    oss << '[';
    size_t tensor_num = raw_data.size();
    for (size_t tensor_id = 0; tensor_id < tensor_num; ++tensor_id) {
        const auto &[data, embedding_num] = raw_data[tensor_id];
        oss << TensorType::Tensor2String(data.data(), type, embedding_dimension, embedding_num);
        if (tensor_id != tensor_num - 1) {
            oss << ',';
        }
    }
    oss << ']';
    return std::move(oss).str();
}
} // namespace infinity