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

export module infinity_core:physical_merge_aggregate;

import :base_table_ref;
import :load_meta;
import :query_context;
import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :infinity_exception;
import :value;
import :data_block;
import :logger;

import internal_types;
import data_type;

namespace infinity {

export class PhysicalMergeAggregate final : public PhysicalOperator {
public:
    explicit PhysicalMergeAggregate(u64 id,
                                    std::shared_ptr<BaseTableRef> table_ref,
                                    std::unique_ptr<PhysicalOperator> left,
                                    std::shared_ptr<std::vector<std::string>> output_names,
                                    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types,
                                    std::shared_ptr<std::vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kMergeAggregate, std::move(left), nullptr, id, load_metas), output_names_(std::move(output_names)),
          output_types_(std::move(output_types)), table_ref_(std::move(table_ref)) {}

    ~PhysicalMergeAggregate() override = default;

    void Init(QueryContext* query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline std::shared_ptr<std::vector<std::string>> GetOutputNames() const final { return output_names_; }

    inline std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final { return output_types_; }

    template <typename T>
    T GetInputData(MergeAggregateOperatorState *op_state, size_t block_index, size_t col_idx, size_t row_idx);

    template <typename T>
    T GetOutputData(MergeAggregateOperatorState *op_state, size_t block_index, size_t col_idx, size_t row_idx);

    template <typename T>
    using MathOperation = std::function<T(T, T)>;

    void SimpleMergeAggregateExecute(MergeAggregateOperatorState *merge_aggregate_op_state);

    void GroupByMergeAggregateExecute(MergeAggregateOperatorState *merge_aggregate_op_state);

    template <typename T>
    void UpdateData(MergeAggregateOperatorState *op_state,
                    MathOperation<T> operation,
                    size_t col_idx,
                    const std::pair<size_t, size_t> &input_block_row_id,
                    const std::pair<size_t, size_t> &output_block_row_id);

    template <typename T>
    void WriteValueAtPosition(MergeAggregateOperatorState *op_state, size_t block_index, size_t col_idx, size_t row_idx, T value);

    template <typename T>
    void HandleSum(MergeAggregateOperatorState *op_state,
                   size_t col_idx,
                   const std::pair<size_t, size_t> &input_block_row_id,
                   const std::pair<size_t, size_t> &output_block_row_id);

    template <typename T>
    void HandleCount(MergeAggregateOperatorState *op_state,
                     size_t col_idx,
                     const std::pair<size_t, size_t> &input_block_row_id,
                     const std::pair<size_t, size_t> &output_block_row_id);

    template <typename T>
    void HandleMin(MergeAggregateOperatorState *op_state,
                   size_t col_idx,
                   const std::pair<size_t, size_t> &input_block_row_id,
                   const std::pair<size_t, size_t> &output_block_row_id);

    template <typename T>
    void HandleMax(MergeAggregateOperatorState *op_state,
                   size_t col_idx,
                   const std::pair<size_t, size_t> &input_block_row_id,
                   const std::pair<size_t, size_t> &output_block_row_id);

    template <typename T>
    void HandleAggregateFunction(const std::string &function_name,
                                 MergeAggregateOperatorState *op_state,
                                 size_t col_idx,
                                 const std::pair<size_t, size_t> &input_block_row_id = {0, 0},
                                 const std::pair<size_t, size_t> &output_block_row_id = {0, 0});

    template <typename T>
    Value CreateValue(T value) {
        UnrecoverableError("Unhandled type for makeValue");
    }

    template <>
    Value CreateValue<IntegerT>(IntegerT value) {
        return Value::MakeInt(value);
    }

    template <>
    Value CreateValue<BigIntT>(BigIntT value) {
        return Value::MakeBigInt(value);
    }

    template <>
    Value CreateValue<DoubleT>(DoubleT value) {
        return Value::MakeDouble(value);
    }

    template <>
    Value CreateValue<FloatT>(FloatT value) {
        return Value::MakeFloat(value);
    }

    template <>
    Value CreateValue<TinyIntT>(TinyIntT value) {
        return Value::MakeTinyInt(value);
    }

    template <>
    Value CreateValue<SmallIntT>(SmallIntT value) {
        return Value::MakeSmallInt(value);
    }

private:
    std::shared_ptr<std::vector<std::string>> output_names_{};
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types_{};

public:
    std::shared_ptr<BaseTableRef> table_ref_{};
};

} // namespace infinity
