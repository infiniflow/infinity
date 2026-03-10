// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
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

export module infinity_core:logical_hash_aggregate;

import :logical_node_type;
import :column_binding;
import :logical_node;
import :base_expression;

import internal_types;
import data_type;

namespace infinity {

// HashAggregate for multiple distinct aggregates (including mixed aggregates with distinct and non-distinct columns)
export class LogicalHashAggregate : public LogicalNode {
public:
    explicit LogicalHashAggregate(u64 node_id,
                                  std::vector<std::shared_ptr<BaseExpression>> group_by_expressions,
                                  std::vector<std::shared_ptr<BaseExpression>> distinct_columns,
                                  std::vector<std::shared_ptr<BaseExpression>> non_distinct_columns,
                                  u64 groupby_index,
                                  u64 distinct_table_index)
        : LogicalNode(node_id, LogicalNodeType::kHashAggregate), groups_(std::move(group_by_expressions)),
          distinct_columns_(std::move(distinct_columns)), non_distinct_columns_(std::move(non_distinct_columns)), groupby_index_(groupby_index),
          distinct_table_index_(distinct_table_index) {}

    [[nodiscard]] std::vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final;

    std::string ToString(i64 &space) const final;

    inline std::string name() final { return "LogicalHashAggregate"; }

    std::vector<std::shared_ptr<BaseExpression>> groups_;

    std::vector<std::shared_ptr<BaseExpression>> distinct_columns_;

    std::vector<std::shared_ptr<BaseExpression>> non_distinct_columns_;

    u64 groupby_index_{};

    // Output table index for the deduplicated results
    u64 distinct_table_index_{};
};

} // namespace infinity
