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
import parser;
import third_party;

export module logical_project;

namespace infinity {

class BaseExpression;

export class LogicalProject : public LogicalNode {
public:
    inline LogicalProject(u64 node_id, Vector<SharedPtr<BaseExpression>> expressions, u64 projection_index)
        : LogicalNode(node_id, LogicalNodeType::kProjection), expressions_(Move(expressions)), table_index_(projection_index) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return Format("logical_project_{}", ToStr(table_index_)); }

    Vector<SharedPtr<BaseExpression>> expressions_{};

    u64 table_index_{};
};

} // namespace infinity
