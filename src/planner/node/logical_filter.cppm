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

export module infinity_core:logical_filter;

import :stl;
import :logical_node_type;
import :column_binding;
import :logical_node;
import :base_expression;

import data_type;
import internal_types;

namespace infinity {

export class LogicalFilter : public LogicalNode {
public:
    explicit LogicalFilter(u64 node_id, SharedPtr<BaseExpression> expression)
        : LogicalNode(node_id, LogicalNodeType::kFilter), expression_(std::move(expression)) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalFilter"; }

    [[nodiscard]] inline SharedPtr<BaseExpression> &expression() { return expression_; }

    [[nodiscard]] inline const SharedPtr<BaseExpression> &expression() const { return expression_; }

private:
    SharedPtr<BaseExpression> expression_{};
};

} // namespace infinity
