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

export module infinity_core:logical_alter;

import :logical_node_type;
import :column_binding;
import :logical_node;
import :meta_info;

import internal_types;
import data_type;
import alter_statement;
import column_def;

namespace infinity {

export class LogicalAlter : public LogicalNode {
public:
    LogicalAlter(u64 node_id, const std::shared_ptr<TableInfo> &table_info, AlterStatementType type)
        : LogicalNode(node_id, LogicalNodeType::kAlter), type_(type), table_info_(table_info) {}

    std::vector<ColumnBinding> GetColumnBindings() const final;

    std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final;

public:
    AlterStatementType type_;
    std::shared_ptr<TableInfo> table_info_;
};

export class LogicalRenameTable : public LogicalAlter {
public:
    LogicalRenameTable(u64 node_id, const std::shared_ptr<TableInfo> &table_info, std::string new_table_name)
        : LogicalAlter(node_id, table_info, AlterStatementType::kRenameTable), new_table_name_(std::move(new_table_name)) {}

    std::string ToString(i64 &space) const final;

    std::string name() final { return "LogicalRenameTable"; }

public:
    std::string new_table_name_;
};

export class LogicalAddColumns : public LogicalAlter {
public:
    LogicalAddColumns(u64 node_id, const std::shared_ptr<TableInfo> &table_info, std::vector<std::shared_ptr<ColumnDef>> column_defs)
        : LogicalAlter(node_id, table_info, AlterStatementType::kAddColumns), column_defs_(std::move(column_defs)) {}

    std::string ToString(i64 &space) const final;

    std::string name() final { return "LogicalAddColumns"; }

public:
    std::vector<std::shared_ptr<ColumnDef>> column_defs_;
};

export class LogicalDropColumns : public LogicalAlter {
public:
    LogicalDropColumns(u64 node_id, const std::shared_ptr<TableInfo> &table_info, std::vector<std::string> column_name)
        : LogicalAlter(node_id, table_info, AlterStatementType::kDropColumns), column_names_(std::move(column_name)) {}

    std::string ToString(i64 &space) const final;

    std::string name() final { return "LogicalDropColumns"; }

public:
    std::vector<std::string> column_names_;
};

} // namespace infinity