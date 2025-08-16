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

export module infinity_core:physical_merge_limit;

import :base_expression;
import :query_context;
import :operator_state;
import :physical_operator;
import :physical_limit;
import :physical_operator_type;
import :load_meta;
import :infinity_exception;
import :logger;
import :base_table_ref;

import internal_types;
import data_type;

namespace infinity {

export class PhysicalMergeLimit final : public PhysicalOperator {
public:
    explicit PhysicalMergeLimit(u64 id,
                                std::unique_ptr<PhysicalOperator> left,
                                std::shared_ptr<BaseTableRef> base_table_ref,
                                std::shared_ptr<BaseExpression> limit_expr,
                                std::shared_ptr<BaseExpression> offset_expr,
                                std::shared_ptr<std::vector<LoadMeta>> load_metas);

    ~PhysicalMergeLimit() override = default;

    void Init(QueryContext* query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline std::shared_ptr<std::vector<std::string>> GetOutputNames() const final { return left_->GetOutputNames(); }

    inline std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final { return left_->GetOutputTypes(); }

    [[nodiscard]] inline const std::shared_ptr<BaseExpression> &limit_expr() const { return limit_expr_; }

    [[nodiscard]] inline const std::shared_ptr<BaseExpression> &offset_expr() const { return offset_expr_; }

    void FillingTableRefs(std::unordered_map<size_t, std::shared_ptr<BaseTableRef>> &table_refs) override {
        table_refs.insert({base_table_ref_->table_index_, base_table_ref_});
    }

private:
    std::shared_ptr<BaseTableRef> base_table_ref_;
    std::shared_ptr<BaseExpression> limit_expr_{};
    std::shared_ptr<BaseExpression> offset_expr_{};

    std::unique_ptr<LimitCounter> counter_{};
    bool total_hits_count_flag_{};
};

} // namespace infinity
