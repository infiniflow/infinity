//
// Created by JinHai on 2022/10/28.
//
#include "embedding_info.h"

namespace infinity {

bool EmbeddingInfo::operator==(const TypeInfo &other) const {
    if (other.type() != TypeInfoType::kEmbedding)
        return false;

    auto *embedding_info_ptr = (EmbeddingInfo *)(&other);

    return this->dimension_ == embedding_info_ptr->dimension_ && this->embedding_data_type_ == embedding_info_ptr->embedding_data_type_;
}

nlohmann::json EmbeddingInfo::Serialize() const {
    nlohmann::json res;
    res["dimension"] = dimension_;
    res["embedding_type"] = embedding_data_type_;
    return res;
}

std::string EmbeddingInfo::EmbeddingDataTypeToString(EmbeddingDataType type) {
    switch (type) {
        case kElemBit:
            return "bit";
        case kElemInt8:
            return "int8";
        case kElemInt16:
            return "int16";
        case kElemInt32:
            return "int32";
        case kElemInt64:
            return "int64";
        case kElemFloat:
            return "float";
        case kElemDouble:
            return "double";
        default:
            ParserError("Unexpected embedding type");
    }
}

} // namespace infinity
