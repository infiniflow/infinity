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

export module infinity_core:physical_optimize;

import :query_context;
import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :base_expression;
import :load_meta;
import :infinity_exception;
import :logger;

import statement_common;
import internal_types;
import optimize_statement;
import data_type;

namespace infinity {

export class PhysicalOptimize final : public PhysicalOperator {
public:
    explicit PhysicalOptimize(u64 id, std::string db_name, std::string table_name, std::shared_ptr<std::vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kOptimize, nullptr, nullptr, id, load_metas), db_name_(std::move(db_name)),
          table_name_(std::move(table_name)) {}

    ~PhysicalOptimize() override = default;

    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline std::shared_ptr<std::vector<std::string>> GetOutputNames() const final { return output_names_; }

    inline std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final { return output_types_; }

private:
    std::string db_name_{};
    std::string table_name_{};

    std::shared_ptr<std::vector<std::string>> output_names_{};
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types_{};
};

} // namespace infinity
