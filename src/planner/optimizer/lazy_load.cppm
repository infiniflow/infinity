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

export module infinity_core:lazy_load;

import :stl;
import :logical_node_visitor;
import :logical_node;
import :logical_node_type;
import :base_expression;
import :column_binding;
import :query_context;
import :column_expression;
import :optimizer_rule;
import :load_meta;
import :base_table_ref;

import internal_types;
import data_type;

namespace infinity {

class RefencecColumnCollection : public LogicalNodeVisitor {
public:
    void VisitNode(LogicalNode &op) final;

private:
    std::shared_ptr<BaseExpression> VisitReplace(const std::shared_ptr<ColumnExpression> &expression) final;

    std::unordered_map<size_t, std::vector<ColumnBinding>> scan_bindings_{};
    std::unordered_map<size_t, std::shared_ptr<std::vector<std::shared_ptr<DataType>>>> column_types_{};
    std::unordered_map<size_t, std::shared_ptr<std::vector<std::string>>> column_names_{};

    std::unordered_set<ColumnBinding> unloaded_bindings_;
    std::vector<LoadMeta> load_metas_;
};

class CleanScan : public LogicalNodeVisitor {
public:
    void VisitNode(LogicalNode &op) final;

private:
    std::shared_ptr<BaseExpression> VisitReplace(const std::shared_ptr<ColumnExpression> &expression) final;

    std::shared_ptr<std::vector<LoadMeta>> last_op_load_metas_{};
    u64 last_op_node_id_{};
    std::vector<size_t> scan_table_indexes_{};
};

export class LazyLoad : public OptimizerRule {
public:
    inline void ApplyToPlan(QueryContext *query_context_ptr, std::shared_ptr<LogicalNode> &logical_plan) final {
        auto logic_op_type = logical_plan->operator_type();
        switch (logic_op_type) {
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
                collector.VisitNode(*logical_plan);
                cleaner_.VisitNode(*logical_plan);
        }
    }

    [[nodiscard]] inline std::string name() const final { return "Lazy Load"; }

private:
    RefencecColumnCollection collector{};
    CleanScan cleaner_{};
};

export std::optional<BaseTableRef *> GetScanTableRef(LogicalNode &op);

} // namespace infinity