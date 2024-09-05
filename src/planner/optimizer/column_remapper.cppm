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

export module column_remapper;

import internal_types;
import stl;
import logical_node_visitor;
import logical_node;
import base_expression;
import column_binding;
import query_context;
import column_expression;
import data_type;

import optimizer_rule;
import logical_node_type;

namespace infinity {

class BindingRemapper : public LogicalNodeVisitor {
public:
    void VisitNode(LogicalNode &op) final;

private:
    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<ColumnExpression> &expression) override;

    Vector<ColumnBinding> bindings_;
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_;
    u32 column_cnt_{};
};

export class ColumnRemapper : public OptimizerRule {
public:
    inline void ApplyToPlan(QueryContext *, SharedPtr<LogicalNode> &logical_plan) final {
        return remapper_.VisitNode(*logical_plan);
    }

    [[nodiscard]] inline String name() const final { return "Column Remapper"; }

private:
    BindingRemapper remapper_{};
};

} // namespace infinity
