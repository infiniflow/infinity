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

export module infinity_core:logical_top;

import :stl;
import :logical_node_type;
import :column_binding;
import :logical_node;
import :base_expression;
import :base_table_ref;

import internal_types;
import select_statement;
import data_type;

namespace infinity {

export class LogicalTop : public LogicalNode {
public:
    inline explicit LogicalTop(u64 node_id,
                               std::shared_ptr<BaseTableRef> base_table_ref,
                               std::shared_ptr<BaseExpression> limit_expression,
                               std::shared_ptr<BaseExpression> offset_expression,
                               std::vector<std::shared_ptr<BaseExpression>> sort_expressions,
                               std::vector<OrderType> order_by_types,
                               bool total_hits_count_flag)
        : LogicalNode(node_id, LogicalNodeType::kTop), base_table_ref_(std::move(base_table_ref)), limit_expression_(std::move(limit_expression)),
          offset_expression_(std::move(offset_expression)), sort_expressions_(std::move(sort_expressions)),
          order_by_types_(std::move(order_by_types)), total_hits_count_flag_(total_hits_count_flag) {}

    [[nodiscard]] std::vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final;

    std::string ToString(i64 &space) const final;

    inline std::string name() final { return "LogicalTop"; }

    std::shared_ptr<BaseTableRef> base_table_ref_{};
    std::shared_ptr<BaseExpression> limit_expression_{};
    std::shared_ptr<BaseExpression> offset_expression_{};
    std::vector<std::shared_ptr<BaseExpression>> sort_expressions_{};
    std::vector<OrderType> order_by_types_{};
    bool total_hits_count_flag_{};
};

} // namespace infinity
