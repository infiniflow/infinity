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

export module infinity_core:logical_aggregate;

import :logical_node_type;
import :column_binding;
import :logical_node;
import :base_expression;
import :base_table_ref;

import internal_types;
import data_type;

namespace infinity {

export class LogicalAggregate : public LogicalNode {
public:
    explicit LogicalAggregate(u64 node_id,
                              std::shared_ptr<BaseTableRef> base_table_ref,
                              std::vector<std::shared_ptr<BaseExpression>> groups,
                              u64 groupby_index,
                              std::vector<std::shared_ptr<BaseExpression>> aggregates,
                              u64 aggregate_index)
        : LogicalNode(node_id, LogicalNodeType::kAggregate), groups_(std::move(groups)), groupby_index_(groupby_index),
          aggregates_(std::move(aggregates)), aggregate_index_(aggregate_index), base_table_ref_(std::move(base_table_ref)) {}

    [[nodiscard]] std::vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final;

    std::string ToString(i64 &space) const final;

    inline std::string name() final { return "LogicalAggregate"; }

    std::vector<std::shared_ptr<BaseExpression>> groups_{};
    u64 groupby_index_{};

    std::vector<std::shared_ptr<BaseExpression>> aggregates_{};
    u64 aggregate_index_{};

    std::shared_ptr<BaseTableRef> base_table_ref_;
};

} // namespace infinity
