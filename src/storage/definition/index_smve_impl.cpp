// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:index_smve.impl;

import :index_smve;
import :status;
import :infinity_exception;
import :default_values;

import std;

import statement_common;
import serialize;
import embedding_info;
import internal_types;
import logical_type;

namespace infinity {

std::shared_ptr<IndexSMVE> IndexSMVE::Make(std::shared_ptr<std::string> index_name,
                                           std::shared_ptr<std::string> index_comment,
                                           const std::string &file_name,
                                           std::vector<std::string> column_names,
                                           const std::vector<InitParameter *> &index_param_list) {
    u32 width = 2048;
    u32 topk = 8;
    for (const auto *para : index_param_list) {
        if (para->param_name_ == "width") {
            width = std::stoul(para->param_value_);
        } else if (para->param_name_ == "topk") {
            topk = std::stoul(para->param_value_);
        } else {
            Status status = Status::InvalidIndexParam(para->param_name_);
            RecoverableError(status);
        }
    }
    return std::make_shared<IndexSMVE>(index_name, index_comment, file_name, column_names, width, topk);
}

void IndexSMVE::ValidateColumnDataType(const std::shared_ptr<BaseTableRef> &base_table_ref, const std::string &column_name) {
    auto &column_names_vector = *(base_table_ref->column_names_);
    auto &column_types_vector = *(base_table_ref->column_types_);
    size_t column_id = std::find(column_names_vector.begin(), column_names_vector.end(), column_name) - column_names_vector.begin();
    if (column_id == column_names_vector.size()) {
        Status status = Status::ColumnNotExist(column_name);
        RecoverableError(status);
    }
    bool error_type = false;
    auto &data_type = column_types_vector[column_id];
    if (data_type->type() != LogicalType::kTensor) {
        error_type = true;
    }
    if (error_type) {
        Status status = Status::InvalidIndexDefinition(
            fmt::format("Attempt to create SMVE index on column: {}, data type: {}.", column_name, data_type->ToString()));
        RecoverableError(status);
    }
}

i32 IndexSMVE::GetSizeInBytes() const {
    i32 size = IndexBase::GetSizeInBytes();
    size += sizeof(width_);
    size += sizeof(topk_);
    return size;
}

void IndexSMVE::WriteAdv(char *&ptr) const {
    IndexBase::WriteAdv(ptr);
    WriteBufAdv(ptr, width_);
    WriteBufAdv(ptr, topk_);
}

std::string IndexSMVE::ToString() const {
    std::stringstream ss;
    ss << IndexBase::ToString() << ", width=" << width_ << ", topk=" << topk_;
    return ss.str();
}

std::string IndexSMVE::BuildOtherParamsString() const {
    std::stringstream ss;
    ss << "width = " << width_ << ", topk = " << topk_;
    return ss.str();
}

nlohmann::json IndexSMVE::Serialize() const {
    nlohmann::json res = IndexBase::Serialize();
    res["width"] = width_;
    res["topk"] = topk_;
    return res;
}

} // namespace infinity
