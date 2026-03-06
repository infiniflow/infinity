// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
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

export module infinity_core:physical_hash_aggregate;

import :query_context;
import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :data_table;
import :base_expression;
import :load_meta;
import :infinity_exception;
import :data_block;
import :physical_aggregate;
import :aggregate_utils;

import internal_types;
import data_type;
import third_party;

namespace infinity {

export class PhysicalHashAggregate final : public PhysicalOperator {
public:
    explicit PhysicalHashAggregate(u64 id,
                                   std::unique_ptr<PhysicalOperator> left,
                                   std::vector<std::shared_ptr<BaseExpression>> group_by_expressions,
                                   std::vector<std::shared_ptr<BaseExpression>> distinct_columns,
                                   std::vector<std::shared_ptr<BaseExpression>> non_distinct_columns,
                                   std::shared_ptr<std::vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kHashAggregate, std::move(left), nullptr, id, load_metas), groups_(std::move(group_by_expressions)),
          distinct_columns_(std::move(distinct_columns)), non_distinct_columns_(std::move(non_distinct_columns)) {}

    ~PhysicalHashAggregate() override = default;

    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final;

    bool IsSink() const override { return true; }

    std::vector<HashRange> GetHashRanges(i64 parallel_count) const;

    // Getters for explain
    const std::vector<std::shared_ptr<BaseExpression>> &GroupByExpressions() const { return groups_; }
    const std::vector<std::shared_ptr<BaseExpression>> &DistinctColumns() const { return distinct_columns_; }
    const std::vector<std::shared_ptr<BaseExpression>> &NonDistinctColumns() const { return non_distinct_columns_; }

private:
    std::vector<std::shared_ptr<BaseExpression>> groups_;
    std::vector<std::shared_ptr<BaseExpression>> distinct_columns_;
    std::vector<std::shared_ptr<BaseExpression>> non_distinct_columns_;

    std::vector<std::shared_ptr<DataType>> groupby_types_{};
    std::vector<std::shared_ptr<DataType>> distinct_types_{};
    std::vector<std::shared_ptr<DataType>> output_types_{};
    std::vector<std::shared_ptr<ColumnDef>> groupby_columns_{};
    std::vector<std::shared_ptr<ColumnDef>> distinct_column_defs_{};
    std::vector<std::shared_ptr<ColumnDef>> non_distinct_column_defs_{};

    size_t hash_key_size_{0};
};

} // namespace infinity
