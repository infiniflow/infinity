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

export module infinity_core:logical_unnest;

import :logical_node_type;
import :column_binding;
import :logical_node;
import :base_expression;

import data_type;
import internal_types;

namespace infinity {

export class LogicalUnnest : public LogicalNode {
public:
    explicit LogicalUnnest(u64 node_id, std::vector<std::shared_ptr<BaseExpression>> expressions, u64 unnest_idx)
        : LogicalNode(node_id, LogicalNodeType::kUnnest), expression_list_(std::move(expressions)), unnest_idx_(unnest_idx) {}

    [[nodiscard]] std::vector<ColumnBinding> GetColumnBindings() const;

    [[nodiscard]] std::vector<ColumnBinding> RemoveColumnBindings() const;

    [[nodiscard]] std::shared_ptr<std::vector<std::string>> GetOutputNames() const;

    [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const;

    std::string ToString(i64 &space) const final;

    inline std::string name() final { return "LogicalUnnest"; }

public:
    // getter
    std::vector<std::shared_ptr<BaseExpression>> &expression_list() { return expression_list_; }
    const std::vector<std::shared_ptr<BaseExpression>> &expression_list() const { return expression_list_; }
    u64 unnest_idx() const { return unnest_idx_; }

private:
    std::vector<std::shared_ptr<BaseExpression>> expression_list_{};
    u64 unnest_idx_{};
};

} // namespace infinity
