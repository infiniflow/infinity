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

    void UpdateBlockData(MergeAggregateOperatorState *merge_aggregate_op_state, SizeT col_idx);

    template <typename T>
    T AddData(Value input_value, MergeAggregateOperatorState *merge_aggregate_op_state);

    template <typename T>
    T GetDataFromValueAtInputBlockPosition(MergeAggregateOperatorState *op_state, SizeT block_index, SizeT col_idx, SizeT row_idx) {
        Value value = op_state->input_data_block_->GetValue(col_idx, row_idx);
        return value.GetValue<T>();
    }

    template <typename T>
    T GetDataFromValueAtOutputBlockPosition(MergeAggregateOperatorState *op_state, SizeT block_index, SizeT col_idx, SizeT row_idx) {
        Value value = op_state->data_block_array_[block_index]->GetValue(col_idx, row_idx);
        return value.GetValue<T>();
    }

    // template <typename T>
    // T GetDataFromValueAtInputBlockPosition2(const Vector<UniquePtr<DataBlock>> &input_blocks, SizeT block_index, SizeT row, SizeT col) {
    //     Value value = input_blocks[block_index]->GetValue(col, row);
    //     return value.GetValue<T>();
    // }
    //
    // template <typename T>
    // T GetDataFromValueAtOutputBlockPosition2(const Vector<UniquePtr<DataBlock>> &output_blocks, SizeT block_index, SizeT row, SizeT col) {
    //     Value value = output_blocks[block_index]->GetValue(col, row);
    //     return value.GetValue<T>();
    // }

    void WriteIntegerAtPosition(MergeAggregateOperatorState *op_state, SizeT block_index, SizeT col_idx, SizeT row_idx, IntegerT integer) {
        op_state->data_block_array_[block_index]->SetValue(col_idx, row_idx, Value::MakeInt(integer));
    }

    // void WriteIntegerAtPosition2(Vector<UniquePtr<DataBlock>> &output_blocks, SizeT block_index, SizeT row, SizeT col, IntegerT integer) {
    //     output_blocks[block_index]->SetValue(col, row, Value::MakeInt(integer));
    // }

    IntegerT MinValue(IntegerT a, IntegerT b) { return (a < b) ? a : b; }

    IntegerT MaxValue(IntegerT a, IntegerT b) { return (a > b) ? a : b; }

    bool SimpleMergeAggregateExecute(Vector<UniquePtr<DataBlock>> &input_blocks, Vector<UniquePtr<DataBlock>> &output_blocks);

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};

public:
    SharedPtr<BaseTableRef> table_ref_{};
};

} // namespace infinity
