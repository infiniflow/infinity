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
import logical_node_type;
import column_binding;
import logical_node;
import base_expression;
import parser;
import base_table_ref;

export module logical_aggregate;

namespace infinity {

export class LogicalAggregate : public LogicalNode {
public:
    explicit LogicalAggregate(u64 node_id,
                              SharedPtr<BaseTableRef> base_table_ref,
                              Vector<SharedPtr<BaseExpression>> groups,
                              u64 groupby_index,
                              Vector<SharedPtr<BaseExpression>> aggregates,
                              u64 aggregate_index)
        : LogicalNode(node_id, LogicalNodeType::kAggregate), groups_(Move(groups)), groupby_index_(groupby_index), aggregates_(Move(aggregates)),
          aggregate_index_(aggregate_index), base_table_ref_(Move(base_table_ref)) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalAggregate"; }

    Vector<SharedPtr<BaseExpression>> groups_{};
    u64 groupby_index_{};

    Vector<SharedPtr<BaseExpression>> aggregates_{};
    u64 aggregate_index_{};

    SharedPtr<BaseTableRef> base_table_ref_;
};

} // namespace infinity
