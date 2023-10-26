//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "default_value.h"
#include "type/complex/embedding_type.h"
#include "type/logical_type.h"
#include "type/type_info.h"

#include <memory>

namespace infinity {

class EmbeddingInfo : public TypeInfo {
public:
    inline static std::shared_ptr<EmbeddingInfo> Make(EmbeddingDataType embedding_data_type, size_t dimension) {
        ParserAssert(dimension <= EMBEDDING_LIMIT_INTERNAL,
                     "Embedding dimension should less than " +
                         std::to_string(EMBEDDING_LIMIT_INTERNAL)) return std::make_shared<EmbeddingInfo>(embedding_data_type, dimension);
    }

    explicit EmbeddingInfo(EmbeddingDataType type, size_t dimension)
        : TypeInfo(TypeInfoType::kEmbedding), embedding_data_type_(type), dimension_(dimension) {}

    ~EmbeddingInfo() override = default;

    bool operator==(const TypeInfo &other) const override;

    [[nodiscard]] inline size_t Size() const override { return EmbeddingType::EmbeddingSize(embedding_data_type_, dimension_); }

    [[nodiscard]] inline EmbeddingDataType Type() const noexcept { return embedding_data_type_; }

    [[nodiscard]] inline size_t Dimension() const noexcept { return dimension_; }

    [[nodiscard]] inline std::string ToString() const override {
        return EmbeddingDataTypeToString(embedding_data_type_) + "," + std::to_string(dimension_);
    }

    [[nodiscard]] nlohmann::json Serialize() const override;

    static std::string EmbeddingDataTypeToString(EmbeddingDataType type);

private:
    EmbeddingDataType embedding_data_type_{EmbeddingDataType::kElemInvalid};
    size_t dimension_;
};

} // namespace infinity
