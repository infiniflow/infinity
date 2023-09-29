//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "common/types/complex/embedding_type.h"
#include "common/types/type_info.h"
#include "common/types/logical_type.h"
#include "common/default_values.h"

namespace infinity {

class EmbeddingInfo : public TypeInfo {
public:

    inline static SharedPtr<EmbeddingInfo>
    Make(EmbeddingDataType embedding_data_type, SizeT dimension) {
        TypeAssert(dimension <= EMBEDDING_LIMIT,
                   "Embedding dimension should less than " + std::to_string(EMBEDDING_LIMIT))
        return MakeShared<EmbeddingInfo>(embedding_data_type, dimension);
    }

    explicit
    EmbeddingInfo(EmbeddingDataType type, SizeT dimension) :
            TypeInfo(TypeInfoType::kEmbedding),
            embedding_data_type_(type),
            dimension_(dimension) {}

    ~EmbeddingInfo() override = default;

    bool
    operator==(const TypeInfo& other) const override;

    [[nodiscard]] inline SizeT
    Size() const override {
        return EmbeddingType::EmbeddingSize(embedding_data_type_, dimension_);
    }

    [[nodiscard]] inline EmbeddingDataType
    Type() const noexcept {
        return embedding_data_type_;
    }

    [[nodiscard]] inline SizeT
    Dimension() const noexcept {
        return dimension_;
    }

    [[nodiscard]] nlohmann::json
    Serialize() const override;

    [[nodiscard]] inline
    String
    ToString() const override {
        return EmbeddingDataTypeToString(embedding_data_type_) + "," + std::to_string(dimension_);
    }

    static
    String
    EmbeddingDataTypeToString(EmbeddingDataType type);


private:
    EmbeddingDataType embedding_data_type_{EmbeddingDataType::kElemInvalid};
    SizeT dimension_;
};

}
