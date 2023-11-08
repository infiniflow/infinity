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
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import index_def;
import table_def;

export module physical_create_table;

namespace infinity {

export class PhysicalCreateTable final : public PhysicalOperator {
public:
    explicit PhysicalCreateTable(SharedPtr<String> schema_name,
                                 SharedPtr<TableDef> table_def_ptr,
                                 SharedPtr<Vector<String>> output_names,
                                 SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                 ConflictType conflict_type,
                                 u64 table_index,
                                 u64 id);

    explicit PhysicalCreateTable(SharedPtr<String> schema_name,
                                 const SharedPtr<PhysicalOperator> &input,
                                 SharedPtr<Vector<String>> output_names,
                                 SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                 ConflictType conflict_type,
                                 u64 table_index,
                                 u64 id);

    ~PhysicalCreateTable() override = default;

    void Init() override;

    void Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline u64 table_index() const { return table_index_; }

    inline const SharedPtr<TableDef> &table_definition() const { return table_def_ptr_; }

    inline ConflictType conflict_type() const { return conflict_type_; }

    inline const SharedPtr<String> &schema_name() const { return schema_name_; }

private:
    SharedPtr<TableDef> table_def_ptr_{};
    SharedPtr<String> schema_name_{};
    u64 table_index_{};
    ConflictType conflict_type_{ConflictType::kInvalid};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
