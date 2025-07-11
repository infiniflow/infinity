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

export module infinity_core:logical_create_collection;

import :stl;
import :logical_node_type;
import :column_binding;
import :logical_node;
import data_type;
import internal_types;
import extra_ddl_info;

namespace infinity {

export class LogicalCreateCollection : public LogicalNode {
public:
    static inline SharedPtr<LogicalCreateCollection>
    Make(u64 node_id, const SharedPtr<String> &schema_name, const SharedPtr<String> &collection_name, u64 table_index, ConflictType conflict_type) {
        return MakeShared<LogicalCreateCollection>(node_id, schema_name, collection_name, table_index, conflict_type);
    }

public:
    LogicalCreateCollection(u64 node_id,
                            SharedPtr<String> schema_name,
                            SharedPtr<String> collection_name_,
                            u64 table_index,
                            ConflictType conflict_type)
        : LogicalNode(node_id, LogicalNodeType::kCreateCollection), schema_name_(std::move(schema_name)),
          collection_name_(std::move(collection_name_)), table_index_(table_index), conflict_type_(conflict_type) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalCreateCollection"; }

    [[nodiscard]] inline SharedPtr<String> collection_name() const { return collection_name_; }

    [[nodiscard]] inline SharedPtr<String> schema_name() const { return schema_name_; }

    [[nodiscard]] inline u64 table_index() const { return table_index_; }

    [[nodiscard]] inline ConflictType conflict_type() const { return conflict_type_; }

private:
    SharedPtr<String> schema_name_{};
    SharedPtr<String> collection_name_{};
    u64 table_index_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
};

} // namespace infinity
