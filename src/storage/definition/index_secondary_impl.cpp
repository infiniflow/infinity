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

import std;
import third_party;
import serialize;

namespace infinity {

void IndexSecondary::ValidateColumnDataType(const std::shared_ptr<BaseTableRef> &base_table_ref,
                                            const std::string &column_name,
                                            SecondaryIndexCardinality secondary_index_cardinality) {
    auto &column_names_vector = *(base_table_ref->column_names_);
    auto &column_types_vector = *(base_table_ref->column_types_);
    size_t column_id = std::find(column_names_vector.begin(), column_names_vector.end(), column_name) - column_names_vector.begin();
    if (column_id == column_names_vector.size()) {
        RecoverableError(Status::ColumnNotExist(column_name));
    } else if (auto &data_type = column_types_vector[column_id]; !(data_type->CanBuildSecondaryIndex())) {
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
    return size;
}

void IndexSecondary::WriteAdv(char *&ptr) const {
    // Call base class implementation
    IndexBase::WriteAdv(ptr);
    // Write secondary index cardinality
    WriteBufAdv(ptr, u8(secondary_index_cardinality_));
}

nlohmann::json IndexSecondary::Serialize() const {
    // Call base class implementation
    nlohmann::json res = IndexBase::Serialize();
    // Add secondary index cardinality for secondary indexes
    res["secondary_index_cardinality"] = secondary_index_cardinality_ == SecondaryIndexCardinality::kLowCardinality ? "low" : "high";

    return res;
}

std::string IndexSecondary::BuildOtherParamsString() const { return ""; }

} // namespace infinity