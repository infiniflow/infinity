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

export module infinity_core:physical_aggregate;

import :stl;
import :query_context;
import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :data_table;
import :hash_table;
import :base_expression;
import :load_meta;
import :infinity_exception;
import :data_block;
import :logger;

import internal_types;
import data_type;

namespace infinity {

export struct HashRange {
    i64 start_{};
    i64 end_{};
};

export class PhysicalAggregate final : public PhysicalOperator {
public:
    explicit PhysicalAggregate(u64 id,
                               std::unique_ptr<PhysicalOperator> left,
                               std::vector<std::shared_ptr<BaseExpression>> groups,
                               u64 groupby_index,
                               std::vector<std::shared_ptr<BaseExpression>> aggregates,
                               u64 aggregate_index,
                               std::shared_ptr<std::vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kAggregate, std::move(left), nullptr, id, load_metas), groups_(std::move(groups)),
          aggregates_(std::move(aggregates)), groupby_index_(groupby_index), aggregate_index_(aggregate_index) {}

    ~PhysicalAggregate() override = default;

    void Init(QueryContext* query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    void GroupByInputTable(const std::vector<std::unique_ptr<DataBlock>> &input_blocks, std::vector<std::unique_ptr<DataBlock>> &output_blocks, HashTable &hash_table);

    void GenerateGroupByResult(const std::shared_ptr<DataTable> &input_table, std::shared_ptr<DataTable> &output_table, HashTable &hash_table);

    std::vector<std::shared_ptr<BaseExpression>> groups_{};
    std::vector<std::shared_ptr<BaseExpression>> aggregates_{};

    bool SimpleAggregateExecute(const std::vector<std::unique_ptr<DataBlock>> &input_blocks,
                                std::vector<std::unique_ptr<DataBlock>> &output_blocks,
                                std::vector<std::unique_ptr<char[]>> &states,
                                bool task_completed);

    inline u64 GroupTableIndex() const { return groupby_index_; }

    inline u64 AggregateTableIndex() const { return aggregate_index_; }

    std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final;

    bool IsSink() const override { return true; }

    std::vector<HashRange> GetHashRanges(i64 parallel_count) const;

private:
    u64 groupby_index_{};
    u64 aggregate_index_{};
};

} // namespace infinity
