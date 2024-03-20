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

std::unordered_map<std::string, ConstraintType> string_to_constraint_type = {
    {"primary key", ConstraintType::kPrimaryKey},
    {"unique", ConstraintType::kUnique},
    {"null", ConstraintType::kNull},
    {"not null", ConstraintType::kNotNull},
    {"invalid", ConstraintType::kInvalid},
};

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
        case ConstraintType::kInvalid:
            return "Invalid";
    }
    ParserError("Unexpected error.");
}

ColumnDef::ColumnDef(int64_t id, std::shared_ptr<DataType> column_type, std::string column_name, std::unordered_set<ConstraintType> constraints)
    : TableElement(TableElementType::kColumn), id_(id), column_type_(std::move(column_type)), name_(std::move(column_name)),
      constraints_(std::move(constraints)) {}

ColumnDef::ColumnDef(LogicalType logical_type, const std::shared_ptr<TypeInfo> &type_info_ptr)
    : TableElement(TableElementType::kColumn), column_type_(std::make_shared<DataType>(logical_type, type_info_ptr)) {}

bool ColumnDef::operator==(const ColumnDef &other) const {
    bool res = type_ == other.type_ && id_ == other.id_ && name_ == other.name_ && column_type_ != nullptr && other.column_type_ != nullptr &&
               *column_type_ == *other.column_type_ && constraints_.size() == other.constraints_.size() &&
               build_bloom_filter_ == other.build_bloom_filter_;
    if (!res) {
        return false;
    }
    for (auto &con : constraints_) {
        if (!other.constraints_.contains(con)) {
            return false;
        }
    }
    return true;
}

std::string ColumnDef::ToString() const {
    std::stringstream ss;
    ss << name_ << " " << column_type_->ToString();
    for (auto &constraint : constraints_) {
        ss << " " << ConstrainTypeToString(constraint);
    }
    return ss.str();
}

ConstraintType ColumnDef::StringToConstraintType(std::string &type) {
    if(string_to_constraint_type.find(type) != string_to_constraint_type.end()){
        ConstraintType constraint_type = string_to_constraint_type.find(type)->second;
        return constraint_type;
    }
    return ConstraintType::kInvalid;
}

} // namespace infinity