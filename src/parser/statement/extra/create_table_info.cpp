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

#include "create_table_info.h"
#include <sstream>

namespace infinity {

CreateTableInfo::~CreateTableInfo() {
    for (auto *&column_def : column_defs_) {
        delete column_def;
        column_def = nullptr;
    }

    for (auto *&table_constraint : constraints_) {
        delete table_constraint;
        table_constraint = nullptr;
    }

    for (auto *&property : properties_) {
        delete property;
        property = nullptr;
    }

    if (select_ != nullptr) {
        delete select_;
        select_ = nullptr;
    }
}

std::string CreateTableInfo::ToString() const {
    std::stringstream ss;
    ss << table_name_ << " ";
    switch(this->conflict_type_) {
        case ConflictType::kIgnore: {
            ss << "Ignore ";
            break;
        }
        case ConflictType::kError: {
            ss << "Error ";
            break;
        }
        case ConflictType::kReplace: {
            ss << "Replace ";
            break;
        }
        default: {
            ParserError("Invalid conflict type.");
        }
    }
    return ss.str();
}

std::string TableConstraint::ToString() const {
    std::stringstream ss;
    ss << ConstrainTypeToString(constraint_) << "(";
    size_t name_count = names_ptr_->size();
    if (name_count > 0) {
        for (size_t idx = 0; idx < name_count - 1; ++idx) {
            ss << names_ptr_->at(idx) << ", ";
        }
        ss << names_ptr_->back();
    } else {
        ParserError("Table constraint without any columns involved.");
    }
    ss << ")";
    return ss.str();
}

} // namespace infinity
