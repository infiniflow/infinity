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

export module infinity_core:join_table_ref;

import :stl;
import :table_ref;
import :base_expression;

import table_reference;
import join_reference;

namespace infinity {

class BindContext;

export class JoinTableRef : public TableRef {
public:
    explicit JoinTableRef(String alias) : TableRef(TableRefType::kJoin, std::move(alias)) {}

    SharedPtr<BindContext> left_bind_context_;
    SharedPtr<BindContext> right_bind_context_;

    SharedPtr<TableRef> left_table_ref_;
    SharedPtr<TableRef> right_table_ref_;

    // Condition ???
    Vector<SharedPtr<BaseExpression>> on_conditions_;

    // Join Type ???
    JoinType join_type_;
};

} // namespace infinity