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

import stl;
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import load_meta;
import base_table_ref;
import infinity_exception;
import value;
import data_block;

export module physical_merge_aggregate;

namespace infinity {

export class PhysicalMergeAggregate final : public PhysicalOperator {
public:
    explicit PhysicalMergeAggregate(u64 id,
                                    SharedPtr<BaseTableRef> table_ref,
                                    UniquePtr<PhysicalOperator> left,
                                    SharedPtr<Vector<String>> output_names,
                                    SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                    SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kMergeAggregate, Move(left), nullptr, id, load_metas), output_names_(Move(output_names)),
          output_types_(Move(output_types)), table_ref_(Move(table_ref)) {}

    ~PhysicalMergeAggregate() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    SizeT TaskletCount() override {
        Error<NotImplementException>("TaskletCount not Implement");
        return 0;
    }

    template <typename T>
    T GetDataFromValueAtInputBlockPosition(MergeAggregateOperatorState *op_state, SizeT block_index, SizeT col_idx, SizeT row_idx);

    template <typename T>
    T GetDataFromValueAtOutputBlockPosition(MergeAggregateOperatorState *op_state, SizeT block_index, SizeT col_idx, SizeT row_idx);

    void WriteIntegerAtPosition(MergeAggregateOperatorState *op_state, SizeT block_index, SizeT col_idx, SizeT row_idx, IntegerT integer);

    template <typename T>
    T GetInputData(MergeAggregateOperatorState *op_state, SizeT block_index, SizeT col_idx, SizeT row_idx);

    template <typename T>
    T GetOutputData(MergeAggregateOperatorState *op_state, SizeT block_index, SizeT col_idx, SizeT row_idx);

    template <typename T>
    T MinValue(T a, T b);

    template <typename T>
    T MaxValue(T a, T b);

    template <typename T>
    T AddData(T a, T b);

    template <typename T>
    using MathOperation = StdFunction<T(T, T)>;

    void SimpleMergeAggregateExecute(MergeAggregateOperatorState *merge_aggregate_op_state);

    void UpdateBlockData(MergeAggregateOperatorState *merge_aggregate_op_state, SizeT col_idx);

    template <typename T>
    void UpdateData(MergeAggregateOperatorState *op_state, MathOperation<T> operation, SizeT col_idx);

    template <typename T>
    void WriteValueAtPosition(MergeAggregateOperatorState *op_state, SizeT block_index, SizeT col_idx, SizeT row_idx, T value);

    template <typename T>
    void HandleSum(MergeAggregateOperatorState *op_state, SizeT col_idx);

    template <typename T>
    void HandleCount(MergeAggregateOperatorState *op_state, SizeT col_idx);

    template <typename T>
    void HandleMin(MergeAggregateOperatorState *op_state, SizeT col_idx);

    template <typename T>
    void HandleMax(MergeAggregateOperatorState *op_state, SizeT col_idx);

    template <typename T>
    void HandleAggregateFunction(const String &function_name, MergeAggregateOperatorState *op_state, SizeT col_idx);

    template <typename T>
    Value CreateValue(T value) {
        Error<NotImplementException>("Unhandled type for makeValue");
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

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};

public:
    SharedPtr<BaseTableRef> table_ref_{};
};

} // namespace infinity
