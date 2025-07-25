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

export module infinity_core:logical_sort;

import :stl;
import :logical_node_type;
import :column_binding;
import :logical_node;
import data_type;
import :base_expression;
import internal_types;
import select_statement;

namespace infinity {

export class LogicalSort : public LogicalNode {
public:
    inline LogicalSort(u64 node_id, Vector<SharedPtr<BaseExpression>> expressions, Vector<OrderType> order_by_types)
        : LogicalNode(node_id, LogicalNodeType::kSort), expressions_(std::move(expressions)), order_by_types_(std::move(order_by_types)) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalSort"; }

    Vector<SharedPtr<BaseExpression>> expressions_{};
    Vector<OrderType> order_by_types_{};
};

} // namespace infinity