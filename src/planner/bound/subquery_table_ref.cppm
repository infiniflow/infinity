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

export module infinity_core:subquery_table_ref;

import :stl;

import :table_ref;
import :bound_statement;
import table_reference;

namespace infinity {

export class SubqueryTableRef : public TableRef {
public:
    explicit SubqueryTableRef(UniquePtr<BoundStatement> subquery_node, u64 table_index, String alias)
        : TableRef(TableRefType::kSubquery, std::move(alias)), subquery_node_(std::move(subquery_node)), table_index_(table_index) {}

    UniquePtr<BoundStatement> subquery_node_{nullptr};
    u64 table_index_{};
};

} // namespace infinity