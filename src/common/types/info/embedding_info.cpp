//
// Created by JinHai on 2022/10/28.
//

#include "embedding_info.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

size_t
EmbeddingInfo::Size() const {
    TypeAssert(embedding_type_ != EmbeddingDataType::kElemInvalid, "Invalid embedding data type");
    size_t result = dimension_ * EmbeddingDataBitWidth(embedding_type_);
    size_t remainder = result - ((result >> 2) << 2);
    result += (remainder > 0);
    return result;
}

}