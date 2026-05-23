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

module infinity_core:index_secondary.impl;

import :index_secondary;
import :index_base;
import :status;
import :base_table_ref;
import :infinity_exception;
import :utility;
import logical_type;

import std;
import third_party;
import serialize;

namespace infinity {

std::shared_ptr<IndexBase> IndexSecondary::Make(std::shared_ptr<std::string> index_name,
                                                std::shared_ptr<std::string> index_comment,
                                                const std::string &file_name,
                                                std::vector<std::string> column_names,
                                                const std::vector<InitParameter *> &index_param_list,
                                                bool is_json_index) {
    SecondaryIndexCardinality cardinality = SecondaryIndexCardinality::kHighCardinality;
    std::map<std::string, std::string> unknown_params;

    for (const auto *param : index_param_list) {
        std::string param_name = param->param_name_;
        std::string param_value = param->param_value_;
        ToLower(param_name);
        ToLower(param_value);

        if (param_name == "cardinality") {
            if (param_value == "low") {
                cardinality = SecondaryIndexCardinality::kLowCardinality;
            } else if (param_value == "high") {
                cardinality = SecondaryIndexCardinality::kHighCardinality;
            }
        } else {
            unknown_params[param_name] = param_value;
        }
    }

    if (!unknown_params.empty()) {
        std::ostringstream oss;
        oss << "Following parameters are left unused:";
        for (const auto &[k, v] : unknown_params) {
            oss << " (" << k << ", " << v << ')';
        }
        oss << '.';
        RecoverableError(Status::InvalidIndexDefinition(std::move(oss).str()));
    }

    return std::make_shared<IndexSecondary>(index_name, index_comment, file_name, std::move(column_names), cardinality, is_json_index);
}

void IndexSecondary::ValidateIndexColumn(const std::shared_ptr<BaseTableRef> &base_table_ref,
                                         const std::string &column_name,
                                         SecondaryIndexCardinality secondary_index_cardinality,
                                         bool is_json_index) {
    auto &column_names_vector = *(base_table_ref->column_names_);
    auto &column_types_vector = *(base_table_ref->column_types_);
    size_t column_id = std::find(column_names_vector.begin(), column_names_vector.end(), column_name) - column_names_vector.begin();
    if (column_id == column_names_vector.size()) {
        RecoverableError(Status::ColumnNotExist(column_name));
    }
    auto &data_type = column_types_vector[column_id];

    // JSON column must use low cardinality
    if (is_json_index && secondary_index_cardinality == SecondaryIndexCardinality::kHighCardinality) {
        RecoverableError(Status::InvalidIndexDefinition(fmt::format("cardinality is always low for JSON column. Column: {}", column_name)));
    }

    if (!(data_type->CanBuildSecondaryIndex())) {
        // For low cardinality secondary indexes, we can relax the data type restrictions
        if (secondary_index_cardinality == SecondaryIndexCardinality::kHighCardinality) {
            RecoverableError(Status::InvalidIndexDefinition(
                fmt::format("Attempt to create index on column: {}, data type: {}.", column_name, data_type->ToString())));
        }
        // For low cardinality, we allow more data types
        // The actual implementation will handle the low cardinality case differently
    }
}

i32 IndexSecondary::GetSizeInBytes() const {
    // Call base class implementation
    i32 size = IndexBase::GetSizeInBytes();
    // Add size for secondary index cardinality
    size += sizeof(SecondaryIndexCardinality);
    // Add size for is_json_index flag
    size += sizeof(bool);
    return size;
}

void IndexSecondary::WriteAdv(char *&ptr) const {
    // Call base class implementation
    IndexBase::WriteAdv(ptr);
    // Write secondary index cardinality
    WriteBufAdv(ptr, u8(secondary_index_cardinality_));
    // Write is_json_index flag
    WriteBufAdv(ptr, u8(is_json_index_));
}

nlohmann::json IndexSecondary::Serialize() const {
    // Call base class implementation
    nlohmann::json res = IndexBase::Serialize();
    // Add secondary index cardinality
    res["cardinality"] = secondary_index_cardinality_ == SecondaryIndexCardinality::kLowCardinality ? "low" : "high";
    // Add is_json_index flag
    res["is_json_index"] = is_json_index_;
    return res;
}

std::string IndexSecondary::BuildOtherParamsString() const {
    return fmt::format("cardinality = {}", secondary_index_cardinality_ == SecondaryIndexCardinality::kLowCardinality ? "low" : "high");
}

} // namespace infinity