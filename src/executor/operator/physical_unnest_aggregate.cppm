// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

export module infinity_core:physical_unnest_aggregate;

import :query_context;
import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :base_expression;
import :data_table;
import :load_meta;
import :infinity_exception;

import internal_types;
import data_type;

namespace infinity {

export class PhysicalUnnestAggregate : public PhysicalOperator {
public:
    explicit PhysicalUnnestAggregate(u64 id,
                                     std::unique_ptr<PhysicalOperator> left,
                                     std::vector<std::shared_ptr<BaseExpression>> groups,
                                     u64 groupby_index,
                                     std::vector<std::shared_ptr<BaseExpression>> aggregates,
                                     u64 aggregate_index,
                                     std::vector<std::shared_ptr<BaseExpression>> unnest_expression_list,
                                     std::shared_ptr<std::vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kUnnestAggregate, std::move(left), nullptr, id, load_metas), groups_(std::move(groups)),
          aggregates_(std::move(aggregates)), groupby_index_(groupby_index), aggregate_index_(aggregate_index),
          unnest_expression_list_(std::move(unnest_expression_list)) {}

    ~PhysicalUnnestAggregate() override = default;

    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline std::shared_ptr<std::vector<std::string>> GetOutputNames() const final {
        return PhysicalCommonFunctionUsingLoadMeta::GetOutputNames(*this);
    }

    inline std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final {
        return PhysicalCommonFunctionUsingLoadMeta::GetOutputTypes(*this);
    }

    size_t TaskletCount() override { return left_->TaskletCount(); }

    std::vector<std::shared_ptr<BaseExpression>> expression_list() const { return unnest_expression_list_; }
    u64 group_by_index() { return groupby_index_; }
    u64 aggregate_index() { return aggregate_index_; }

    std::vector<std::shared_ptr<BaseExpression>> groups_{};
    std::vector<std::shared_ptr<BaseExpression>> aggregates_{};

private:
    u64 groupby_index_{};
    u64 aggregate_index_{};
    std::vector<std::shared_ptr<BaseExpression>> unnest_expression_list_;
    std::shared_ptr<DataTable> input_table_{};
};

} // namespace infinity
