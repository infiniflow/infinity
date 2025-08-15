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

export module infinity_core:logical_create_collection;

import :logical_node_type;
import :column_binding;
import :logical_node;

import data_type;
import internal_types;
import extra_ddl_info;

namespace infinity {

export class LogicalCreateCollection : public LogicalNode {
public:
    static inline std::shared_ptr<LogicalCreateCollection>
    Make(u64 node_id, const std::shared_ptr<std::string> &schema_name, const std::shared_ptr<std::string> &collection_name, u64 table_index, ConflictType conflict_type) {
        return std::make_shared<LogicalCreateCollection>(node_id, schema_name, collection_name, table_index, conflict_type);
    }

public:
    LogicalCreateCollection(u64 node_id,
                            std::shared_ptr<std::string> schema_name,
                            std::shared_ptr<std::string> collection_name_,
                            u64 table_index,
                            ConflictType conflict_type)
        : LogicalNode(node_id, LogicalNodeType::kCreateCollection), schema_name_(std::move(schema_name)),
          collection_name_(std::move(collection_name_)), table_index_(table_index), conflict_type_(conflict_type) {}

    [[nodiscard]] std::vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const;

    std::string ToString(i64 &space) const final;

    inline std::string name() final { return "LogicalCreateCollection"; }

    [[nodiscard]] inline std::shared_ptr<std::string> collection_name() const { return collection_name_; }

    [[nodiscard]] inline std::shared_ptr<std::string> schema_name() const { return schema_name_; }

    [[nodiscard]] inline u64 table_index() const { return table_index_; }

    [[nodiscard]] inline ConflictType conflict_type() const { return conflict_type_; }

private:
    std::shared_ptr<std::string> schema_name_{};
    std::shared_ptr<std::string> collection_name_{};
    u64 table_index_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
};

} // namespace infinity
