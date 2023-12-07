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
import logical_node;
import logical_node_type;
import base_expression;
import column_binding;
import query_context;
import column_expression;
import optimizer_rule;
import parser;
import load_meta;

export module lazy_load;

namespace infinity {

class RefencecColumnCollection : public LogicalNodeVisitor {
public:
    void VisitNode(LogicalNode &op) final;

private:
    SharedPtr<BaseExpression> VisitReplace(const SharedPtr<ColumnExpression> &expression) final;

    Vector<ColumnBinding> scan_bindings_;
    HashMap<SizeT, SharedPtr<Vector<SharedPtr<DataType>>>> column_types_{};

    HashSet<ColumnBinding> unloaded_bindings_;
    Vector<LoadMeta> load_metas_;
};

class CleanScan : public LogicalNodeVisitor {
public:
    void VisitNode(LogicalNode &op) final;

private:
    SharedPtr<BaseExpression> VisitReplace(const SharedPtr<ColumnExpression> &expression) final;

    SharedPtr<Vector<LoadMeta>> last_op_load_metas_{};
    Vector<SizeT> scan_table_indexes_{};
};

export class LazyLoad : public OptimizerRule {
public:
    inline void ApplyToPlan(QueryContext *query_context_ptr, const SharedPtr<LogicalNode> &logical_plan) final {
        if (logical_plan->operator_type() == LogicalNodeType::kUpdate) {
            return;
        }

        collector.VisitNode(*logical_plan);
        cleaner_.VisitNode(*logical_plan);
    }

    [[nodiscard]] inline String name() const final { return "Lazy Load"; }

private:
    RefencecColumnCollection collector{};
    CleanScan cleaner_{};
};

}