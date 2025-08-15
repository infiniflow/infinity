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
import :status;
import :base_table_ref;
import :infinity_exception;

import std;
import third_party;

namespace infinity {

void IndexSecondary::ValidateColumnDataType(const std::shared_ptr<BaseTableRef> &base_table_ref, const std::string &column_name) {
    auto &column_names_vector = *(base_table_ref->column_names_);
    auto &column_types_vector = *(base_table_ref->column_types_);
    size_t column_id = std::find(column_names_vector.begin(), column_names_vector.end(), column_name) - column_names_vector.begin();
    if (column_id == column_names_vector.size()) {
        RecoverableError(Status::ColumnNotExist(column_name));
    } else if (auto &data_type = column_types_vector[column_id]; !(data_type->CanBuildSecondaryIndex())) {
        RecoverableError(
            Status::InvalidIndexDefinition(fmt::format("Attempt to create index on column: {}, data type: {}.", column_name, data_type->ToString())));
    }
}

std::string IndexSecondary::BuildOtherParamsString() const { return ""; }

} // namespace infinity