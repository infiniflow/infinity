//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "common/types/complex/embedding_type.h"
#include "common/types/type_info.h"
#include "common/types/logical_type.h"
//#include "common/types/internal_types.h"

namespace infinity {

class EmbeddingInfo : public TypeInfo {
public:

    inline static UniquePtr<EmbeddingInfo>
    Make(EmbeddingDataType embedding_data_type, i64 dimension) {
        return MakeUnique<EmbeddingInfo>(embedding_data_type, dimension);
    }

    explicit EmbeddingInfo(EmbeddingDataType type, i64 dimension):
            TypeInfo(TypeInfoType::kEmbedding),
            embedding_data_type_(type),
            dimension_(dimension)
            {}

    ~EmbeddingInfo() override = default;

    [[nodiscard]] inline size_t
    Size() const override {
        return EmbeddingType::EmbeddingSize(embedding_data_type_, dimension_);
    }
private:
    EmbeddingDataType embedding_data_type_{EmbeddingDataType::kElemInvalid};
    size_t dimension_;
};

}
