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

export module infinity_core:logical_join;

import :stl;
import :logical_node_type;
import :column_binding;
import :logical_node;
import :base_expression;

import data_type;
import internal_types;
import join_reference;

namespace infinity {

export class LogicalJoin : public LogicalNode {
public:
    explicit LogicalJoin(u64 node_id,
                         JoinType join_type,
                         std::string alias,
                         std::vector<std::shared_ptr<BaseExpression>> conditions,
                         const std::shared_ptr<LogicalNode> &left,
                         const std::shared_ptr<LogicalNode> &right);

    [[nodiscard]] std::vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final;

    std::string ToString(i64 &space) const final;

    inline std::string name() final { return "LogicalJoin"; }

    std::string alias_{};

    u64 mark_index_{}; // Only for mark join

public:
    JoinType join_type_{JoinType::kInner};
    std::vector<std::shared_ptr<BaseExpression>> conditions_{};
};

} // namespace infinity
