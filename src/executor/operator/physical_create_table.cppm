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

export module infinity_core:physical_create_table;

import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :index_base;
import :table_def;
import :load_meta;
import :infinity_exception;
import :logger;

import internal_types;
import extra_ddl_info;
import data_type;

namespace infinity {

export class PhysicalCreateTable final : public PhysicalOperator {
public:
    explicit PhysicalCreateTable(std::shared_ptr<std::string> schema_name,
                                 std::shared_ptr<TableDef> table_def_ptr,
                                 std::shared_ptr<std::vector<std::string>> output_names,
                                 std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types,
                                 ConflictType conflict_type,
                                 u64 table_index,
                                 u64 id,
                                 std::shared_ptr<std::vector<LoadMeta>> load_metas);

    explicit PhysicalCreateTable(std::shared_ptr<std::string> schema_name,
                                 std::unique_ptr<PhysicalOperator> input,
                                 std::shared_ptr<std::vector<std::string>> output_names,
                                 std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types,
                                 ConflictType conflict_type,
                                 u64 table_index,
                                 u64 id,
                                 std::shared_ptr<std::vector<LoadMeta>> load_metas);

    ~PhysicalCreateTable() override = default;

    void Init(QueryContext* query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline std::shared_ptr<std::vector<std::string>> GetOutputNames() const final { return output_names_; }

    inline std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline u64 table_index() const { return table_index_; }

    inline const std::shared_ptr<TableDef> &table_definition() const { return table_def_ptr_; }

    inline ConflictType conflict_type() const { return conflict_type_; }

    inline const std::shared_ptr<std::string> &schema_name() const { return schema_name_; }

private:
    std::shared_ptr<TableDef> table_def_ptr_{};
    std::shared_ptr<std::string> schema_name_{};
    u64 table_index_{};
    ConflictType conflict_type_{ConflictType::kInvalid};

    std::shared_ptr<std::vector<std::string>> output_names_{};
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types_{};
};

} // namespace infinity
