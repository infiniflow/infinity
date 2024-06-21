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

export module physical_optimize;

import stl;

import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import base_expression;
import load_meta;
import infinity_exception;
import internal_types;
import optimize_statement;
import data_type;
import logger;
import statement_common;

namespace infinity {

export class PhysicalOptimize final : public PhysicalOperator {
public:
    explicit PhysicalOptimize(u64 id,
                              String db_name,
                              String table_name,
                              String index_name,
                              Vector<UniquePtr<InitParameter>> opt_params,
                              SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kOptimize, nullptr, nullptr, id, load_metas), db_name_(std::move(db_name)),
          table_name_(std::move(table_name)), index_name_(std::move(index_name)), opt_params_(std::move(opt_params)) {}

    ~PhysicalOptimize() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    SizeT TaskletCount() override {
        String error_message = "Not implement: TaskletCount not Implement";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
        return 0;
    }

private:
    void OptimizeIndex(QueryContext *query_context, OperatorState *operator_state);

    void OptimizeAIndex(QueryContext *query_context, OperatorState *operator_state);

private:
    String db_name_{};
    String table_name_{};
    String index_name_{};
    Vector<UniquePtr<InitParameter>> opt_params_;

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
