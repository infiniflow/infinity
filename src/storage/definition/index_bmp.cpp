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

module;

#include <sstream>
#include <vector>

module index_bmp;

import status;
import logger;
import infinity_exception;
import default_values;
import logical_type;
import serialize;
import sparse_info;
import internal_types;

namespace infinity {

SharedPtr<IndexBMP>
IndexBMP::Make(SharedPtr<String> index_name, const String &file_name, Vector<String> column_names, const Vector<InitParameter *> &index_param_list) {
    SizeT block_size = BMP_BLOCK_SIZE;
    BMPCompressType compress_type = BMPCompressType::kCompressed;
    for (const auto *para : index_param_list) {
        if (para->param_name_ == "block_size") {
            block_size = std::stoi(para->param_value_);
            if (block_size <= 0 || block_size > std::numeric_limits<u8>::max() + 1) {
                Status status = Status::InvalidIndexParam("Block size");
                LOG_ERROR(status.message());
                RecoverableError(status);
            }
        } else if (para->param_name_ == "compress_type") {
            compress_type = BMCompressTypeFromString(para->param_value_);
        } else {
            Status status = Status::InvalidIndexParam(para->param_name_);
            LOG_ERROR(status.message());
            RecoverableError(status);
        }
    }
    return MakeShared<IndexBMP>(index_name, file_name, column_names, block_size, compress_type);
}

void IndexBMP::ValidateColumnDataType(const SharedPtr<BaseTableRef> &base_table_ref, const String &column_name) {
    auto &column_names_vector = *(base_table_ref->column_names_);
    auto &column_types_vector = *(base_table_ref->column_types_);
    SizeT column_id = std::find(column_names_vector.begin(), column_names_vector.end(), column_name) - column_names_vector.begin();
    if (column_id == column_names_vector.size()) {
        Status status = Status::ColumnNotExist(column_name);
        LOG_ERROR(status.message());
        RecoverableError(status);
    }
    bool error_type = false;
    auto &data_type = column_types_vector[column_id];
    if (data_type->type() != LogicalType::kSparse) {
        error_type = true;
    } else {
        auto *sparse_info = static_cast<SparseInfo *>(data_type->type_info().get());
        if (sparse_info->DataType() != EmbeddingDataType::kElemFloat && sparse_info->DataType() != EmbeddingDataType::kElemDouble) {
            error_type = true;
        } else if (sparse_info->IndexType() != EmbeddingDataType::kElemInt32 && sparse_info->IndexType() != EmbeddingDataType::kElemInt16) {
            error_type = true;
        }
    }
    if (error_type) {
        Status status = Status::InvalidIndexDefinition(
            fmt::format("Attempt to create HNSW index on column: {}, data type: {}.", column_name, data_type->ToString()));
        LOG_ERROR(status.message());
        RecoverableError(status);
    }
}

i32 IndexBMP::GetSizeInBytes() const {
    i32 size = IndexBase::GetSizeInBytes();
    size += sizeof(block_size_);
    return size;
}

void IndexBMP::WriteAdv(char *&ptr) const {
    IndexBase::WriteAdv(ptr);
    WriteBufAdv(ptr, block_size_);
}

String IndexBMP::ToString() const {
    std::stringstream ss;
    ss << IndexBase::ToString() << ", " << block_size_;
    return ss.str();
}

String IndexBMP::BuildOtherParamsString() const {
    std::stringstream ss;
    ss << "block_size = " << block_size_;
    return ss.str();
}

nlohmann::json IndexBMP::Serialize() const {
    nlohmann::json res = IndexBase::Serialize();
    res["block_size"] = block_size_;
    return res;
}

} // namespace infinity