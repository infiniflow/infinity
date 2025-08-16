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

export module infinity_core:column_remapper;

import :logical_node_visitor;
import :logical_node;
import :column_binding;
import :optimizer_rule;

import data_type;

namespace infinity {

class BindingRemapper : public LogicalNodeVisitor {
public:
    void VisitNode(LogicalNode &op) final;

private:
    virtual std::shared_ptr<BaseExpression> VisitReplace(const std::shared_ptr<ColumnExpression> &expression) override;

    std::vector<ColumnBinding> bindings_;
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types_;
    u32 column_cnt_{};
};

export class ColumnRemapper : public OptimizerRule {
public:
    inline void ApplyToPlan(QueryContext *, std::shared_ptr<LogicalNode> &logical_plan) final { return remapper_.VisitNode(*logical_plan); }

    [[nodiscard]] inline std::string name() const final { return "Column Remapper"; }

private:
    BindingRemapper remapper_{};
};

} // namespace infinity
