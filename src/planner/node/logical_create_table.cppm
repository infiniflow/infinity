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

export module infinity_core:logical_create_table;

import :logical_node;
import :table_def;
import :logical_node_type;
import :column_binding;

import data_type;
import internal_types;
import extra_ddl_info;

namespace infinity {

export class LogicalCreateTable : public LogicalNode {
public:
    static inline std::shared_ptr<LogicalCreateTable>
    Make(u64 node_id, const std::shared_ptr<std::string> &schema_name, const std::shared_ptr<TableDef> &table_def_ptr, u64 table_index, ConflictType conflict_type) {
        return std::make_shared<LogicalCreateTable>(node_id, schema_name, table_def_ptr, table_index, conflict_type);
    }

public:
    LogicalCreateTable(u64 node_id, std::shared_ptr<std::string> schema_name, std::shared_ptr<TableDef> table_def_ptr, u64 table_index, ConflictType conflict_type)
        : LogicalNode(node_id, LogicalNodeType::kCreateTable), schema_name_(std::move(schema_name)), table_definition_(std::move(table_def_ptr)),
          table_index_(table_index), conflict_type_(conflict_type) {}

    [[nodiscard]] std::vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final;

    std::string ToString(i64 &space) const final;

    inline std::string name() final { return "LogicalCreateTable"; }

    [[nodiscard]] std::shared_ptr<TableDef> table_definitions() const { return table_definition_; }

    [[nodiscard]] std::shared_ptr<std::string> schema_name() const { return schema_name_; }

    [[nodiscard]] inline u64 table_index() const { return table_index_; }

    inline ConflictType conflict_type() const { return conflict_type_; }

private:
    std::shared_ptr<std::string> schema_name_{};
    std::shared_ptr<TableDef> table_definition_{};
    u64 table_index_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
};

} // namespace infinity
