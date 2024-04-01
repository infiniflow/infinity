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

import stl;

import column_expr;
import infinity_exception;
import status;
import third_party;
import query_context;

module column_identifer;

namespace infinity {

ColumnIdentifier ColumnIdentifier::MakeColumnIdentifier(QueryContext *, const ColumnExpr &expr) {
    if (expr.star_ && expr.names_.empty()) {
        RecoverableError(Status::SyntaxError("Star expression should be unfolded before."));
    }

    SharedPtr<String> db_name_ptr = nullptr;
    SharedPtr<String> schema_name_ptr = nullptr;
    SharedPtr<String> table_name_ptr = nullptr;
    SharedPtr<String> column_name_ptr = nullptr;

    i64 name_count = expr.names_.size();
    if (name_count > 4 || name_count <= 0) {
        RecoverableError(Status::SyntaxError("Star expression should be unfolded before."));
    }
    --name_count;
    column_name_ptr = MakeShared<String>(expr.names_[name_count]);
    --name_count;
    if (name_count >= 0) {
        table_name_ptr = MakeShared<String>(expr.names_[name_count]);
        --name_count;
        if (name_count >= 0) {
            schema_name_ptr = MakeShared<String>(expr.names_[name_count]);
            --name_count;
            if (name_count >= 0) {
                db_name_ptr = MakeShared<String>(expr.names_[name_count]);
            }
        }
    }

    SharedPtr<String> alias_name_ptr = nullptr;
    if (!expr.alias_.empty()) {
        alias_name_ptr = MakeShared<String>(expr.alias_);
    }
    return ColumnIdentifier(db_name_ptr, schema_name_ptr, table_name_ptr, column_name_ptr, alias_name_ptr);
}

ColumnIdentifier::ColumnIdentifier(SharedPtr<String> db_name,
                                   SharedPtr<String> schema_name,
                                   SharedPtr<String> table_name,
                                   SharedPtr<String> column_name,
                                   SharedPtr<String> alias_name)
    : db_name_ptr_(std::move(db_name)), schema_name_ptr_(std::move(schema_name)), column_name_ptr_(std::move(column_name)), table_name_ptr_(std::move(table_name)),
      alias_name_ptr_(std::move(alias_name)) {}

String ColumnIdentifier::ToString() const {
    if (table_name_ptr_.get() != nullptr)
        return fmt::format("{}.{}", *table_name_ptr_, *column_name_ptr_);
    else
        return *column_name_ptr_;
}

bool ColumnIdentifier::operator==(const ColumnIdentifier &other) const {
    if (this == &other)
        return true;
    if (IsEqual(*column_name_ptr_, *other.column_name_ptr_)) {
        return false;
    }
    if (table_name_ptr_.get() != nullptr && other.table_name_ptr_.get() != nullptr) {
        return IsEqual(*table_name_ptr_, *other.table_name_ptr_);
    }

    if (table_name_ptr_.get() == nullptr && other.table_name_ptr_.get() == nullptr) {
        return true;
    }

    return false;
}

} // namespace infinity