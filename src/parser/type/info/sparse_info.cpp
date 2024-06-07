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

#include "sparse_info.h"
#include <limits>

namespace infinity {

SparseStoreType SparseInfo::ParseStoreType(const std::vector<std::unique_ptr<InitParameter>> &options) {
    auto store_type = SparseStoreType::kSort; // default
    for (auto &option : options) {
        if (option->param_name_ == "sort") {
            store_type = SparseStoreType::kSort;
        } else if (option->param_name_ == "notsort") {
            store_type = SparseStoreType::kNotSort;
        } else {
            ParserAssert(false, "Unknown option for sparse embedding: " + option->param_name_);
        }
    }

    return store_type;
}

std::shared_ptr<SparseInfo> SparseInfo::Make(EmbeddingDataType data_type, size_t dimension, SparseStoreType store_type) {
    if (dimension == 0) {
        return nullptr;
    }
    EmbeddingDataType index_type = EmbeddingDataType::kElemInvalid;
    if (dimension <= std::numeric_limits<int8_t>::max() + 1) {
        index_type = EmbeddingDataType::kElemInt8;
    } else if (dimension <= std::numeric_limits<int16_t>::max() + 1) {
        index_type = EmbeddingDataType::kElemInt16;
    } else if (dimension <= std::numeric_limits<int32_t>::max() + 1ull) {
        index_type = EmbeddingDataType::kElemInt32;
    } else if (dimension <= std::numeric_limits<int64_t>::max() + 1ull) {
        index_type = EmbeddingDataType::kElemInt64;
    } else {
        ParserAssert(false, "Sparse embedding dimension is too large");
    }
    return std::make_shared<SparseInfo>(data_type, index_type, dimension, store_type);
}

bool SparseInfo::operator==(const TypeInfo &other) const {
    if (other.type() != TypeInfoType::kSparse) {
        return false;
    }
    const auto *other_sparse_info = static_cast<const SparseInfo *>(&other);
    return data_type_ == other_sparse_info->data_type_ && index_type_ == other_sparse_info->index_type_ &&
           dimension_ == other_sparse_info->dimension_;
}

nlohmann::json SparseInfo::Serialize() const {
    nlohmann::json res;
    res["data_type"] = data_type_;
    res["index_type"] = index_type_;
    res["dimension"] = dimension_;
    res["sort"] = static_cast<int8_t>(store_type_);
    return res;
}

std::unique_ptr<SparseInfo> SparseInfo::Deserialize(const nlohmann::json &json) {
    auto store_type = static_cast<SparseStoreType>(json["sort"].get<int8_t>());
    return std::make_unique<SparseInfo>(json["data_type"], json["index_type"], json["dimension"], store_type);
}

} // namespace infinity