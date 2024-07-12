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
#include <algorithm>
#include <string>

module index_secondary;

import stl;
import status;
import base_table_ref;
import infinity_exception;
import third_party;
import logger;

namespace infinity {

void IndexSecondary::ValidateColumnDataType(const SharedPtr<BaseTableRef> &base_table_ref, const String &column_name) {
    auto &column_names_vector = *(base_table_ref->column_names_);
    auto &column_types_vector = *(base_table_ref->column_types_);
    SizeT column_id = std::find(column_names_vector.begin(), column_names_vector.end(), column_name) - column_names_vector.begin();
    if (column_id == column_names_vector.size()) {
        Status status = Status::ColumnNotExist(column_name);
        RecoverableError(status);
    } else if (auto &data_type = column_types_vector[column_id]; !(data_type->CanBuildSecondaryIndex())) {
        Status status = Status::InvalidIndexDefinition(
            fmt::format("Attempt to create index on column: {}, data type: {}.", column_name, data_type->ToString()));
        RecoverableError(status);
    }
}

String IndexSecondary::BuildOtherParamsString() const { return ""; }

} // namespace infinity