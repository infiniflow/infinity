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

export module infinity_core:physical_update;

import :stl;
import :query_context;
import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :meta_info;
import :base_expression;
import :load_meta;
import :infinity_exception;
import :logger;

import internal_types;
import data_type;

namespace infinity {

export class PhysicalUpdate : public PhysicalOperator {
public:
    explicit PhysicalUpdate(const u64 id,
                            std::unique_ptr<PhysicalOperator> left,
                            std::shared_ptr<TableInfo> table_info,
                            const std::vector<std::pair<size_t, std::shared_ptr<BaseExpression>>> &update_columns,
                            const std::vector<std::shared_ptr<BaseExpression>> &all_columns_in_table,
                            const std::vector<std::shared_ptr<BaseExpression>> &final_result_columns,
                            std::shared_ptr<std::vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kUpdate, std::move(left), nullptr, id, std::move(load_metas)), table_info_(std::move(table_info)),
          update_columns_(update_columns), all_columns_in_table_(all_columns_in_table), final_result_columns_(final_result_columns) {}

    ~PhysicalUpdate() override = default;

    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline std::shared_ptr<std::vector<std::string>> GetOutputNames() const final { return output_names_; }

    inline std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final { return output_types_; }

    std::shared_ptr<TableInfo> table_info_{};
    const std::vector<std::pair<size_t, std::shared_ptr<BaseExpression>>> &update_columns_;
    const std::vector<std::shared_ptr<BaseExpression>> &all_columns_in_table_;
    const std::vector<std::shared_ptr<BaseExpression>> &final_result_columns_;

private:

    std::shared_ptr<std::vector<std::string>> output_names_{};
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types_{};
};

} // namespace infinity
