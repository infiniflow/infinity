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

export module infinity_core:column_identifier;

import :stl;
// import query_context
import column_expr;

namespace infinity {
class QueryContext;

export struct ColumnIdentifier {
public:
    static ColumnIdentifier MakeColumnIdentifier(QueryContext *query_context, const ColumnExpr &expr);

public:
    explicit ColumnIdentifier(SharedPtr<String> db_name,
                              SharedPtr<String> schema_name,
                              SharedPtr<String> table_name,
                              SharedPtr<String> column_name,
                              SharedPtr<String> alias_name);

    [[nodiscard]] String ToString() const;

    [[nodiscard]] bool operator==(const ColumnIdentifier &other) const;

    SharedPtr<String> db_name_ptr_;
    SharedPtr<String> schema_name_ptr_;
    SharedPtr<String> column_name_ptr_;
    SharedPtr<String> table_name_ptr_;
    SharedPtr<String> alias_name_ptr_;
};

} // namespace infinity
