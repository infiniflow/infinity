// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

export module logical_unnest_aggregate;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import base_expression;
import data_type;
import internal_types;

namespace infinity {

export class LogicalUnnestAggregate : public LogicalNode {
public:
    explicit LogicalUnnestAggregate(u64 node_id,
                                    Vector<SharedPtr<BaseExpression>> groups,
                                    u64 groupby_index,
                                    Vector<SharedPtr<BaseExpression>> aggregates,
                                    u64 aggregate_index,
                                    Vector<SharedPtr<BaseExpression>> unnest_expressions)
        : LogicalNode(node_id, LogicalNodeType::kUnnestAggregate), groups_(std::move(groups)), groupby_index_(groupby_index),
          aggregates_(std::move(aggregates)), aggregate_index_(aggregate_index), unnest_expression_list_(std::move(unnest_expressions)) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalUnnestAggregate"; }
    Vector<SharedPtr<BaseExpression>> &groups() { return groups_; }
    u64 group_by_index() { return groupby_index_; }
    Vector<SharedPtr<BaseExpression>> &aggregates() { return aggregates_; }
    u64 aggregate_index() { return aggregate_index_; }
    Vector<SharedPtr<BaseExpression>> &unnest_expression_list() { return unnest_expression_list_; }
    const Vector<SharedPtr<BaseExpression>> &unnest_expression_list() const { return unnest_expression_list_; }

    Vector<SharedPtr<BaseExpression>> groups_{};
    u64 groupby_index_{};
    Vector<SharedPtr<BaseExpression>> aggregates_{};
    u64 aggregate_index_{};
private:
    Vector<SharedPtr<BaseExpression>> unnest_expression_list_{};
};

} // namespace infinity
