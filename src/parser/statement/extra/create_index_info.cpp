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

#include "create_index_info.h"
#include "parser_assert.h"

#include <sstream>

namespace infinity {

CreateIndexInfo::~CreateIndexInfo() {
    if (column_names_ != nullptr) {
        delete column_names_;
        column_names_ = nullptr;
    }
}

std::string CreateIndexInfo::ToString() const {
    std::stringstream ss;
    ss << "CREATE INDEX ";
    switch (conflict_type_) {
        case ConflictType::kIgnore: {
            ss << "IF NOT EXISTS ";
        }
        case ConflictType::kError: {
            break;
        }
        case ConflictType::kReplace: {
            ParserError("Not implemented.");
        }
        case ConflictType::kInvalid: {
            ParserError("Invalid conflict type.");
        }
    }
    if (index_name_.empty()) {
        ss << "DEFAULT_INDEX_NAME ";
    } else {
        ss << index_name_;
    }
    ss << "ON " << table_name_ << "(";
    size_t column_count = column_names_->size();
    if (column_count == 0) {
        ParserError("No column name specified.");
    }
    for (size_t idx = 0; idx < column_count - 1; ++idx) {
        ss << (*column_names_)[idx] << ", ";
    }
    ss << (*column_names_)[column_count - 1] << ")";
    ss << "USING " << method_type_;
    if (index_para_list_ != nullptr && !index_para_list_->empty()) {
        ss << "WITH(";
        size_t para_count = index_para_list_->size();
        for (size_t idx = 0; idx < para_count; ++idx) {
            ss << (*index_para_list_)[idx]->para_name_ << " " << (*index_para_list_)[idx]->para_value_;
            if (idx != para_count - 1) {
                ss << ", ";
            }
        }
        ss << ")";
    }
    ss << ";";
    return ss.str();
}

} // namespace infinity
