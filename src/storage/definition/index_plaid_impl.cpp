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

module infinity_core:index_plaid.impl;

import :index_plaid;
import :status;
import :base_table_ref;
import :infinity_exception;

import std;
import third_party;

import logical_type;
import type_info;
import embedding_info;
import internal_types;
import serialize;
import statement_common;

namespace infinity {

void IndexPLAID::ValidateColumnDataType(const std::shared_ptr<BaseTableRef> &base_table_ref, const std::string &column_name) {
    auto &column_names_vector = *(base_table_ref->column_names_);
    auto &column_types_vector = *(base_table_ref->column_types_);
    size_t column_id = std::find(column_names_vector.begin(), column_names_vector.end(), column_name) - column_names_vector.begin();
    if (column_id == column_names_vector.size()) {
        Status status = Status::ColumnNotExist(column_name);
        RecoverableError(status);
    } else if (auto &data_type = column_types_vector[column_id]; data_type->type() != LogicalType::kTensor) {
        Status status =
            Status::InvalidIndexDefinition(fmt::format("Attempt to create index on column: {}, data type: {}.", column_name, data_type->ToString()));
        RecoverableError(status);
    } else if (const auto embedding_info = static_cast<EmbeddingInfo *>(data_type->type_info().get());
               embedding_info->Type() != EmbeddingDataType::kElemFloat) {
        Status status = Status::InvalidIndexDefinition(fmt::format("Attempt to create index on column: {}, data type: {}, embedding info: {}.",
                                                                   column_name,
                                                                   data_type->ToString(),
                                                                   embedding_info->ToString()));
        RecoverableError(status);
    }
}

std::string IndexPLAID::BuildOtherParamsString() const { return fmt::format("nbits = {}, n_centroids = {}.", nbits_, n_centroids_); }

constexpr std::array<u32, 2> acceptable_nbits = {2, 4};

std::shared_ptr<IndexBase> IndexPLAID::Make(std::shared_ptr<std::string> index_name,
                                            std::shared_ptr<std::string> index_comment,
                                            const std::string &file_name,
                                            std::vector<std::string> column_names,
                                            const std::vector<InitParameter *> &index_param_list) {
    u32 nbits = 4;       // default: 4-bit quantization
    u32 n_centroids = 0; // default: auto (sqrt(N))

    for (auto para : index_param_list) {
        if (para->param_name_ == "nbits") {
            const int val = std::stoi(para->param_value_);
            if (val != 2 && val != 4) {
                Status status = Status::InvalidIndexParam("nbits must be 2 or 4");
                RecoverableError(status);
            }
            nbits = u32(val);
        } else if (para->param_name_ == "n_centroids") {
            const int val = std::stoi(para->param_value_);
            if (val < 0) {
                Status status = Status::InvalidIndexParam("n_centroids");
                RecoverableError(status);
            }
            n_centroids = u32(val);
        } else {
            Status status = Status::InvalidIndexParam(para->param_name_);
            RecoverableError(status);
        }
    }

    if (std::find(acceptable_nbits.begin(), acceptable_nbits.end(), nbits) == acceptable_nbits.end()) {
        Status status = Status::InvalidIndexParam("nbits must be 2 or 4");
        RecoverableError(status);
    }

    return std::make_shared<IndexPLAID>(index_name, index_comment, file_name, std::move(column_names), nbits, n_centroids);
}

i32 IndexPLAID::GetSizeInBytes() const {
    size_t size = IndexBase::GetSizeInBytes();
    size += sizeof(nbits_);
    size += sizeof(n_centroids_);
    return size;
}

void IndexPLAID::WriteAdv(char *&ptr) const {
    IndexBase::WriteAdv(ptr);
    WriteBufAdv(ptr, nbits_);
    WriteBufAdv(ptr, n_centroids_);
}

std::string IndexPLAID::ToString() const {
    std::stringstream ss;
    ss << IndexBase::ToString() << ", " << BuildOtherParamsString();
    return std::move(ss).str();
}

nlohmann::json IndexPLAID::Serialize() const {
    nlohmann::json res = IndexBase::Serialize();
    res["nbits"] = nbits_;
    res["n_centroids"] = n_centroids_;
    return res;
}

} // namespace infinity
