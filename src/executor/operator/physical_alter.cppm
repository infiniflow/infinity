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

export module physical_alter;

import stl;

import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import load_meta;
import infinity_exception;
import internal_types;
import data_type;
import logger;
import table_entry;
import alter_statement;
import column_def;
import constant_expr;

namespace infinity {

export class PhysicalAlter : public PhysicalOperator {
public:
    explicit PhysicalAlter(TableEntry *table_entry,
                           AlterStatementType type,
                           SharedPtr<Vector<String>> output_names,
                           SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                           u64 id,
                           SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kAlter, nullptr, nullptr, id, load_metas), table_entry_(table_entry),
          output_names_(std::move(output_names)), output_types_(std::move(output_types)) {}

    ~PhysicalAlter() override = default;

    SizeT TaskletCount() override {
        String error_message = "Not implement: TaskletCount not Implement";
        UnrecoverableError(error_message);
        return 0;
    }

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

protected:
    AlterStatementType type_;
    TableEntry *table_entry_{};
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

export class PhysicalRenameTable final : public PhysicalAlter {
public:
    PhysicalRenameTable(TableEntry *table_entry,
                        String &&new_table_name,
                        SharedPtr<Vector<String>> output_names,
                        SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                        u64 id,
                        SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalAlter(table_entry, AlterStatementType::kRenameTable, std::move(output_names), std::move(output_types), id, load_metas),
          new_table_name_(std::move(new_table_name)) {}

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

private:
    String new_table_name_;
};

export class PhysicalAddColumns final : public PhysicalAlter {
public:
    PhysicalAddColumns(TableEntry *table_entry,
                       const Vector<SharedPtr<ColumnDef>> &column_defs,
                       SharedPtr<Vector<String>> output_names,
                       SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                       u64 id,
                       SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalAlter(table_entry, AlterStatementType::kAddColumns, std::move(output_names), std::move(output_types), id, load_metas),
          column_defs_(column_defs) {}

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

private:
    const Vector<SharedPtr<ColumnDef>> &column_defs_;
};

export class PhysicalDropColumns final : public PhysicalAlter {
public:
    PhysicalDropColumns(TableEntry *table_entry,
                        const Vector<String> &column_names,
                        SharedPtr<Vector<String>> output_names,
                        SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                        u64 id,
                        SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalAlter(table_entry, AlterStatementType::kDropColumns, std::move(output_names), std::move(output_types), id, load_metas),
          column_names_(column_names) {}

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

private:
    const Vector<String> &column_names_;
};

} // namespace infinity
