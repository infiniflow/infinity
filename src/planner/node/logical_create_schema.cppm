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

export module logical_create_schema;


namespace infinity {

export class LogicalCreateSchema : public LogicalNode {
public:
    static inline SharedPtr<LogicalCreateSchema> Make(u64 node_id, const SharedPtr<String> &schema_name, ConflictType conflict_type) {
        return MakeShared<LogicalCreateSchema>(node_id, schema_name, conflict_type);
    }

public:
    LogicalCreateSchema(u64 node_id, SharedPtr<String> schema_name, ConflictType conflict_type)
        : LogicalNode(node_id, LogicalNodeType::kCreateSchema), schema_name_(Move(schema_name)), conflict_type_(conflict_type) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalCreateSchema"; }

    [[nodiscard]] inline SharedPtr<String> schema_name() const { return schema_name_; }

    [[nodiscard]] inline ConflictType conflict_type() const { return conflict_type_; }

private:
    SharedPtr<String> schema_name_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
};

} // namespace infinity
