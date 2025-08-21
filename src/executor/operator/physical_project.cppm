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

export module infinity_core:physical_project;

import :query_context;
import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :base_expression;
import :load_meta;
import :infinity_exception;
import :highlighter;

import internal_types;
import data_type;

namespace infinity {

export class PhysicalProject : public PhysicalOperator {
public:
    explicit PhysicalProject(u64 id,
                             u64 table_index,
                             std::unique_ptr<PhysicalOperator> left,
                             std::vector<std::shared_ptr<BaseExpression>> expressions,
                             std::shared_ptr<std::vector<LoadMeta>> load_metas,
                             std::map<size_t, std::shared_ptr<HighlightInfo>> highlight_columns)
        : PhysicalOperator(PhysicalOperatorType::kProjection, std::move(left), nullptr, id, load_metas), expressions_(std::move(expressions)),
          projection_table_index_(table_index), highlight_columns_(std::move(highlight_columns)) {}

    ~PhysicalProject() override = default;

    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final;

    size_t TaskletCount() override { return left_->TaskletCount(); }

    std::vector<std::shared_ptr<BaseExpression>> expressions_{};

    inline u64 TableIndex() const { return projection_table_index_; }

private:
    //    ExpressionExecutor executor;
    u64 projection_table_index_{};
    std::map<size_t, std::shared_ptr<HighlightInfo>> highlight_columns_{};
};

} // namespace infinity
