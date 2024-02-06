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

#include "column_def.h"
#include "type/data_type.h"

namespace infinity {

std::string ConstrainTypeToString(ConstraintType type) {
    switch (type) {
        case ConstraintType::kPrimaryKey:
            return "PrimaryKey";
        case ConstraintType::kUnique:
            return "Unique";
        case ConstraintType::kNull:
            return "Nullable";
        case ConstraintType::kNotNull:
            return "Not nullable";
    }
    ParserError("Unexpected error.");
}

ColumnDef::ColumnDef(int64_t id, std::shared_ptr<DataType> column_type, std::string column_name, std::unordered_set<ConstraintType> constraints)
    : TableElement(TableElementType::kColumn), id_(id), column_type_(std::move(column_type)), name_(std::move(column_name)),
      constraints_(std::move(constraints)) {}

ColumnDef::ColumnDef(LogicalType logical_type, const std::shared_ptr<TypeInfo> &type_info_ptr)
    : TableElement(TableElementType::kColumn), column_type_(std::make_shared<DataType>(logical_type, type_info_ptr)) {}

std::string ColumnDef::ToString() const {
    std::stringstream ss;
    ss << name_ << " " << column_type_->ToString();
    for (auto &constraint : constraints_) {
        ss << " " << ConstrainTypeToString(constraint);
    }
    return ss.str();
}

} // namespace infinity