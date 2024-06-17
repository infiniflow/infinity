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

#include <algorithm>
#include <array>
#include <sstream>
#include <string>

module index_emvb;

import stl;
import status;
import base_table_ref;
import infinity_exception;
import third_party;
import logger;
import logical_type;
import type_info;
import embedding_info;
import internal_types;
import serialize;

namespace infinity {

// TODO: now only support tensor column
// TODO: now only support float element type
void IndexEMVB::ValidateColumnDataType(const SharedPtr<BaseTableRef> &base_table_ref, const String &column_name) {
    auto &column_names_vector = *(base_table_ref->column_names_);
    auto &column_types_vector = *(base_table_ref->column_types_);
    SizeT column_id = std::find(column_names_vector.begin(), column_names_vector.end(), column_name) - column_names_vector.begin();
    if (column_id == column_names_vector.size()) {
        Status status = Status::ColumnNotExist(column_name);
        LOG_ERROR(status.message());
        RecoverableError(status);
    } else if (auto &data_type = column_types_vector[column_id]; data_type->type() != LogicalType::kTensor) {
        Status status =
            Status::InvalidIndexDefinition(fmt::format("Attempt to create index on column: {}, data type: {}.", column_name, data_type->ToString()));
        LOG_ERROR(status.message());
        RecoverableError(status);
    } else if (const auto embedding_info = static_cast<EmbeddingInfo *>(data_type->type_info().get());
               embedding_info->Type() != EmbeddingDataType::kElemFloat) {
        Status status = Status::InvalidIndexDefinition(fmt::format("Attempt to create index on column: {}, data type: {}, embedding info: {}.",
                                                                   column_name,
                                                                   data_type->ToString(),
                                                                   embedding_info->ToString()));
        LOG_ERROR(status.message());
        RecoverableError(status);
    }
}

String IndexEMVB::BuildOtherParamsString() const {
    return fmt::format("pq_subspace_num = {}, pq_subspace_bits = {}.", residual_pq_subspace_num_, residual_pq_subspace_bits_);
}

constexpr std::array<u32, 8> accepable_residual_pq_subspace_num = {1, 2, 4, 8, 16, 32, 64, 128};
constexpr std::array<u32, 2> accepable_residual_pq_subspace_bits = {8, 16};

SharedPtr<IndexBase>
IndexEMVB::Make(SharedPtr<String> index_name, const String &file_name, Vector<String> column_names, const Vector<InitParameter *> &index_param_list) {
    u32 residual_pq_subspace_num = 0;
    u32 residual_pq_subspace_bits = 0;
    for (auto para : index_param_list) {
        if (para->param_name_ == "pq_subspace_num") {
            const int val = std::stoi(para->param_value_);
            if (val <= 0) {
                Status status = Status::InvalidIndexParam("pq_subspace_num");
                LOG_ERROR(status.message());
                RecoverableError(status);
            }
            residual_pq_subspace_num = u32(val);
        } else if (para->param_name_ == "pq_subspace_bits") {
            const int val = std::stoi(para->param_value_);
            if (val <= 0) {
                Status status = Status::InvalidIndexParam("pq_subspace_bits");
                LOG_ERROR(status.message());
                RecoverableError(status);
            }
            residual_pq_subspace_bits = u32(val);
        } else {
            Status status = Status::InvalidIndexParam(para->param_name_);
            LOG_ERROR(status.message());
            RecoverableError(status);
        }
    }
    if (std::find(accepable_residual_pq_subspace_num.begin(), accepable_residual_pq_subspace_num.end(), residual_pq_subspace_num) ==
        accepable_residual_pq_subspace_num.end()) {
        Status status = Status::InvalidIndexParam("pq_subspace_num");
        LOG_ERROR(status.message());
        RecoverableError(status);
    }
    if (std::find(accepable_residual_pq_subspace_bits.begin(), accepable_residual_pq_subspace_bits.end(), residual_pq_subspace_bits) ==
        accepable_residual_pq_subspace_bits.end()) {
        Status status = Status::InvalidIndexParam("pq_subspace_bits");
        LOG_ERROR(status.message());
        RecoverableError(status);
    }
    return MakeShared<IndexEMVB>(index_name, file_name, std::move(column_names), residual_pq_subspace_num, residual_pq_subspace_bits);
}

i32 IndexEMVB::GetSizeInBytes() const {
    SizeT size = IndexBase::GetSizeInBytes();
    size += sizeof(residual_pq_subspace_num_);
    size += sizeof(residual_pq_subspace_bits_);
    return size;
}

void IndexEMVB::WriteAdv(char *&ptr) const {
    IndexBase::WriteAdv(ptr);
    WriteBufAdv(ptr, residual_pq_subspace_num_);
    WriteBufAdv(ptr, residual_pq_subspace_bits_);
}

String IndexEMVB::ToString() const {
    std::stringstream ss;
    ss << IndexBase::ToString() << ", " << BuildOtherParamsString();
    return std::move(ss).str();
}

nlohmann::json IndexEMVB::Serialize() const {
    nlohmann::json res = IndexBase::Serialize();
    res["pq_subspace_num"] = residual_pq_subspace_num_;
    res["pq_subspace_bits"] = residual_pq_subspace_bits_;
    return res;
}

} // namespace infinity