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

#include "embedding_info.h"
#include "statement/statement_common.h"
#include "type/complex/embedding_type.h"
#include "type/complex/sparse_type.h"
#include "type/type_info.h"

namespace infinity {

enum class SparseStoreType : int8_t {
    kSort,
    kSorted,
    kInvalid,
};

class SparseInfo : public TypeInfo {
public:
    static SparseStoreType ParseStoreType(const std::vector<std::unique_ptr<InitParameter>> &options);

    static EmbeddingDataType GetIndexType(size_t dimension);

    static std::shared_ptr<SparseInfo>
    Make(EmbeddingDataType data_type, size_t dimension, SparseStoreType store_type);

    static std::shared_ptr<SparseInfo> Make(EmbeddingDataType data_type, EmbeddingDataType idx_type, size_t dimension, SparseStoreType store_type) {
        return std::make_shared<SparseInfo>(data_type, idx_type, dimension, store_type);
    }

public:
    explicit SparseInfo(EmbeddingDataType data_type, EmbeddingDataType index_type, size_t dim, SparseStoreType store_type)
        : TypeInfo(TypeInfoType::kSparse), data_type_(data_type), index_type_(index_type), dimension_(dim), store_type_(store_type) {}

    bool operator==(const TypeInfo &other) const override;

    [[nodiscard]] inline size_t Size() const override { return sizeof(SparseType); }

    inline size_t SparseSize(size_t nnz) const { return IndiceSize(nnz) + DataSize(nnz); }

    inline size_t IndiceSize(size_t nnz) const { return EmbeddingType::EmbeddingSize(index_type_, nnz); }

    inline size_t DataSize(size_t nnz) const {
        if (data_type_ == EmbeddingDataType::kElemBit) {
            return 0;
        }
        return EmbeddingType::EmbeddingSize(data_type_, nnz);
    }

    [[nodiscard]] inline std::string ToString() const override {
        return EmbeddingInfo::EmbeddingDataTypeToString(data_type_) + "," + EmbeddingInfo::EmbeddingDataTypeToString(index_type_) + "," +
               std::to_string(dimension_);
    }

    [[nodiscard]] nlohmann::json Serialize() const override;

    static std::unique_ptr<SparseInfo> Deserialize(const nlohmann::json &json);

    inline EmbeddingDataType DataType() const noexcept { return data_type_; }

    inline EmbeddingDataType IndexType() const noexcept { return index_type_; }

    inline size_t Dimension() const noexcept { return dimension_; }

    inline SparseStoreType StoreType() const noexcept { return store_type_; }

private:
    EmbeddingDataType data_type_{EmbeddingDataType::kElemInvalid};
    EmbeddingDataType index_type_{EmbeddingDataType::kElemInvalid};
    size_t dimension_{};
    SparseStoreType store_type_{SparseStoreType::kSort};
};

} // namespace infinity