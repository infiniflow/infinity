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
#include "type/complex/embedding_type.h"
#include "type/type_info.h"
#include <limits>

namespace infinity {

class SparseInfo : public TypeInfo {
public:
    static std::shared_ptr<SparseInfo> Make(EmbeddingDataType data_type, size_t dimension) {
        if (dimension == 0) {
            ParserError("dimension of sparse should not be 0");
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
        return std::make_shared<SparseInfo>(data_type, index_type, dimension);
    }

public:
    explicit SparseInfo(EmbeddingDataType data_type, EmbeddingDataType index_type, size_t dim)
        : TypeInfo(TypeInfoType::kSparse), data_type_(data_type), index_type_(index_type), dimension_(dim) {}

    bool operator==(const TypeInfo &other) const override;

    [[nodiscard]] inline size_t Size() const override {
        return EmbeddingType::EmbeddingSize(data_type_, dimension_) + EmbeddingType::EmbeddingSize(index_type_, dimension_);
    }

    [[nodiscard]] inline std::string ToString() const override {
        return EmbeddingInfo::EmbeddingDataTypeToString(data_type_) + "," + EmbeddingInfo::EmbeddingDataTypeToString(index_type_) + "," +
               std::to_string(dimension_);
    }

    [[nodiscard]] nlohmann::json Serialize() const override;

    inline EmbeddingDataType DataType() const noexcept { return data_type_; }

    inline EmbeddingDataType IndexType() const noexcept { return index_type_; }

    inline size_t Dimension() const noexcept { return dimension_; }

private:
    EmbeddingDataType data_type_{EmbeddingDataType::kElemInvalid};
    EmbeddingDataType index_type_{EmbeddingDataType::kElemInvalid};
    size_t dimension_{};
};

} // namespace infinity