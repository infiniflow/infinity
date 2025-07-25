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

export module infinity_core:logical_alter;

import :stl;
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
    LogicalAlter(u64 node_id, const SharedPtr<TableInfo> &table_info, AlterStatementType type)
        : LogicalNode(node_id, LogicalNodeType::kAlter), type_(type), table_info_(table_info) {}

    Vector<ColumnBinding> GetColumnBindings() const final;

    SharedPtr<Vector<String>> GetOutputNames() const final;

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

public:
    AlterStatementType type_;
    SharedPtr<TableInfo> table_info_;
};

export class LogicalRenameTable : public LogicalAlter {
public:
    LogicalRenameTable(u64 node_id, const SharedPtr<TableInfo> &table_info, String new_table_name)
        : LogicalAlter(node_id, table_info, AlterStatementType::kRenameTable), new_table_name_(std::move(new_table_name)) {}

    String ToString(i64 &space) const final;

    String name() final { return "LogicalRenameTable"; }

public:
    String new_table_name_;
};

export class LogicalAddColumns : public LogicalAlter {
public:
    LogicalAddColumns(u64 node_id, const SharedPtr<TableInfo> &table_info, Vector<SharedPtr<ColumnDef>> column_defs)
        : LogicalAlter(node_id, table_info, AlterStatementType::kAddColumns), column_defs_(std::move(column_defs)) {}

    String ToString(i64 &space) const final;

    String name() final { return "LogicalAddColumns"; }

public:
    Vector<SharedPtr<ColumnDef>> column_defs_;
};

export class LogicalDropColumns : public LogicalAlter {
public:
    LogicalDropColumns(u64 node_id, const SharedPtr<TableInfo> &table_info, Vector<String> column_name)
        : LogicalAlter(node_id, table_info, AlterStatementType::kDropColumns), column_names_(std::move(column_name)) {}

    String ToString(i64 &space) const final;

    String name() final { return "LogicalDropColumns"; }

public:
    Vector<String> column_names_;
};

} // namespace infinity