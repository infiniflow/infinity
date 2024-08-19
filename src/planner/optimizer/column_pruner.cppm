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
import logical_node_visitor;
import reference_expression;
import base_expression;
import column_expression;
import function_expression;
import column_binding;
import bind_context;
import optimizer_rule;
import logical_node;
import query_context;
import logical_node_type;

export module column_pruner;

namespace infinity {

class RemoveUnusedColumns : public LogicalNodeVisitor {
public:
    explicit RemoveUnusedColumns(bool is_root = false)
        : all_referenced_(is_root) {
    }

    void VisitNode(LogicalNode &op) final;

private:
    SharedPtr<BaseExpression> VisitReplace(const SharedPtr<ColumnExpression> &expression) final;

    template <class T>
    Vector<T> ClearUnusedExpressions(const Vector<T> &list, idx_t table_idx);

    template <class T>
    Vector<T> ClearUnusedBaseTableColumns(const Vector<T> &col_list, idx_t table_idx);

    bool all_referenced_;
    HashSet<ColumnBinding> column_references_;
};

export class ColumnPruner : public OptimizerRule {
public:
    inline void ApplyToPlan(QueryContext *, SharedPtr<LogicalNode> &logical_plan) final {
        auto logic_op_type = logical_plan->operator_type();
        switch (logic_op_type) {
            case LogicalNodeType::kUpdate:
            case LogicalNodeType::kInsert:
            case LogicalNodeType::kImport:
            case LogicalNodeType::kExport:
            case LogicalNodeType::kCreateTable:
            case LogicalNodeType::kCreateIndex:
            case LogicalNodeType::kDropTable:
            case LogicalNodeType::kDropIndex:
            case LogicalNodeType::kCreateSchema:
            case LogicalNodeType::kDropSchema:
            case LogicalNodeType::kShow:
            case LogicalNodeType::kCommand:
            case LogicalNodeType::kPrepare:
                return;
            default:
                remove_visitor.VisitNode(*logical_plan);
        }
    }

    [[nodiscard]] inline String name() const final { return "Column Pruner"; }

private:
    RemoveUnusedColumns remove_visitor{true};
};

}