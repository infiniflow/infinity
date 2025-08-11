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

module infinity_core:bound_insert_statement.impl;

import :bound_insert_statement;
import :stl;
import :logical_insert;
import :base_table_ref;
import :logical_node;
import :query_context;
import :bind_context;
import :logical_table_scan;
import :table_ref;
import :base_table_ref;
import table_reference;
import :infinity_exception;
import :status;
import :third_party;

namespace infinity {

SharedPtr<LogicalNode> BoundInsertStatement::BuildPlan(QueryContext *query_context) {
    const SharedPtr<BindContext> &bind_context = this->bind_context_;
    
    auto base_table_ref = std::static_pointer_cast<BaseTableRef>(table_ref_ptr_);
    
    if (select_plan_) {
        // INSERT SELECT case
        auto logical_insert = MakeShared<LogicalInsert>(bind_context->GetNewLogicalNodeId(),
                                                        base_table_ref->table_info_,
                                                        bind_context->GenerateTableIndex(),
                                                        Vector<Vector<SharedPtr<BaseExpression>>>{});
        logical_insert->set_left_node(select_plan_);
        return logical_insert;
    } else {
        // INSERT VALUES case
        auto logical_insert = MakeShared<LogicalInsert>(bind_context->GetNewLogicalNodeId(),
                                                        base_table_ref->table_info_,
                                                        bind_context->GenerateTableIndex(),
                                                        std::move(value_list_));
        return logical_insert;
    }
}

SharedPtr<LogicalNode>
BoundInsertStatement::BuildFrom(SharedPtr<TableRef> &table_ref, QueryContext *query_context, const SharedPtr<BindContext> &bind_context) {
    if (table_ref.get() == nullptr || table_ref->type_ != TableRefType::kTable) {
        String error_message = "Unsupported table reference type in INSERT statement";
        UnrecoverableError(error_message);
    }
    return BuildBaseTable(table_ref, query_context, bind_context);
}

SharedPtr<LogicalNode>
BoundInsertStatement::BuildBaseTable(SharedPtr<TableRef> &table_ref, QueryContext *, const SharedPtr<BindContext> &bind_context) {
    auto base_table_ref = std::static_pointer_cast<BaseTableRef>(table_ref);
    u64 table_index = bind_context->GenerateTableIndex();
    auto logical_table_scan = MakeShared<LogicalTableScan>(bind_context->GetNewLogicalNodeId(), base_table_ref, table_index);
    return logical_table_scan;
}

} // namespace infinity
