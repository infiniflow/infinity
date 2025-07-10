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
#include <arrow/type.h>
#include <limits>

namespace infinity {

SparseStoreType SparseInfo::ParseStoreType(const std::vector<std::unique_ptr<InitParameter>> &options) {
    auto store_type = SparseStoreType::kSort; // default
    for (auto &option : options) {
        if (option->param_name_ == "sort") {
            store_type = SparseStoreType::kSort;
        } else if (option->param_name_ == "sorted") {
            store_type = SparseStoreType::kSorted;
        } else {
            store_type = SparseStoreType::kInvalid;
        }
    }

    return store_type;
}

EmbeddingDataType SparseInfo::GetIndexType(size_t dimension) {
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
    return index_type;
}

std::shared_ptr<SparseInfo> SparseInfo::Make(EmbeddingDataType data_type, size_t dimension, SparseStoreType store_type) {
    if (dimension == 0 || store_type == SparseStoreType::kInvalid) {
        return nullptr;
    }
    EmbeddingDataType index_type = SparseInfo::GetIndexType(dimension);
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

bool SparseInfo::operator==(const arrow::StructType &other) const {
    std::shared_ptr<arrow::Field> index_field = other.GetFieldByName("index");
    if (!index_field) {
        return false;
    }
    auto index_type = std::dynamic_pointer_cast<arrow::ListType>(index_field->type());
    if (!index_type) {
        return false;
    }
    if (index_type_ != index_type->value_type()->id()) {
        return false;
    }
    std::shared_ptr<arrow::Field> value_field = other.GetFieldByName("value");
    if (!value_field) {
        return data_type_ == EmbeddingDataType::kElemBit;
    }
    if (data_type_ == EmbeddingDataType::kElemBit) {
        return false;
    }
    auto value_type = std::dynamic_pointer_cast<arrow::ListType>(value_field->type());
    if (!value_type) {
        return false;
    }
    return data_type_ == value_type->value_type()->id();
}

void SparseInfo::Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer) const {
    writer.Key("data_type");
    writer.Int((int8_t)data_type_);
    writer.Key("index_type");
    writer.Int((int8_t)index_type_);
    writer.Key("dimension");
    writer.Uint64(dimension_);
    writer.Key("sort");
    writer.Int((int8_t)store_type_);
}

std::unique_ptr<SparseInfo> SparseInfo::Deserialize(std::string_view json_str) {
    simdjson::ondemand::parser parser;
    simdjson::padded_string json(json_str);
    simdjson::ondemand::document doc = parser.iterate(json);
    return std::make_unique<SparseInfo>((EmbeddingDataType)(int8_t)doc["data_type"].get<int8_t>(),
                                        (EmbeddingDataType)(int8_t)doc["index_type"].get<int8_t>(),
                                        (size_t)doc["dimension"].get<size_t>(),
                                        (SparseStoreType)(int8_t)doc["sort"].get<int8_t>());
}

} // namespace infinity