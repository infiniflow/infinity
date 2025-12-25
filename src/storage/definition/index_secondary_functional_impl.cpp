// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:index_secondary_functional.impl;

import :index_secondary_functional;
import :index_base;
import :status;
import :base_table_ref;
import :infinity_exception;

import std;
import third_party;
import serialize;

namespace infinity {

void IndexSecondaryFunctional::ValidateColumnAndReturnDataType(const std::shared_ptr<BaseTableRef> &base_table_ref,
                                                               const std::string &column_name,
                                                               DataType &return_type,
                                                               SecondaryIndexCardinality secondary_index_cardinality) {
    auto &column_names_vector = *(base_table_ref->column_names_);
    size_t column_id = std::find(column_names_vector.begin(), column_names_vector.end(), column_name) - column_names_vector.begin();
    if (column_id == column_names_vector.size()) {
        RecoverableError(Status::ColumnNotExist(column_name));
    } else if (!(return_type.CanBuildSecondaryIndex())) {
        // For low cardinality secondary indexes, we can relax the data type restrictions
        if (secondary_index_cardinality == SecondaryIndexCardinality::kHighCardinality) {
            RecoverableError(Status::InvalidIndexDefinition(
                fmt::format("Attempt to create index on column: {}, data type: {}.", column_name, return_type.ToString())));
        }
        // For low cardinality, we allow more data types
        // The actual implementation will handle the low cardinality case differently
    }
}

i32 IndexSecondaryFunctional::GetSizeInBytes() const {
    i32 size = IndexBase::GetSizeInBytes();

    size_t func_col_params_size = func_col_params_->length();
    size += sizeof(int32_t) + func_col_params_size;
    size_t function_expression_str_size = function_expression_json_str_->length();
    size += sizeof(int32_t) + function_expression_str_size;
    size += sizeof(SecondaryIndexCardinality);
    return size;
}

void IndexSecondaryFunctional::WriteAdv(char *&ptr) const {
    IndexBase::WriteAdv(ptr);

    WriteBufAdv(ptr, *func_col_params_);
    WriteBufAdv(ptr, *function_expression_json_str_);
    WriteBufAdv(ptr, u8(secondary_index_cardinality_));
}

nlohmann::json IndexSecondaryFunctional::Serialize() const {
    nlohmann::json res = IndexBase::Serialize();

    res["func_col_params"] = *func_col_params_;
    res["function_expression"] = nlohmann::json::parse(*function_expression_json_str_);
    res["cardinality"] = secondary_index_cardinality_ == SecondaryIndexCardinality::kLowCardinality ? "low" : "high";

    return res;
}
std::string IndexSecondaryFunctional::ToString() const {
    std::stringstream ss;
    ss << "IndexBase: " << IndexInfo::IndexTypeToString(index_type_) << ", name: " << *index_name_ << ", func_col_params: " << *func_col_params_
       << ", function_expression: " << *function_expression_json_str_
       << ", cardinality: " << (secondary_index_cardinality_ == SecondaryIndexCardinality::kLowCardinality ? "low" : "high");
    return ss.str();
}

std::string IndexSecondaryFunctional::BuildOtherParamsString() const { return ""; }

} // namespace infinity
