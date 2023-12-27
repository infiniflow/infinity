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

#include <algorithm>
#include <string>

import stl;
import txn;
import query_context;
import table_def;
import data_table;
import data_block;
import default_values;
import column_vector;
import expression_state;
import base_expression;
import expression_evaluator;
import parser;
import physical_operator_type;
import operator_state;
import data_block;
import infinity_exception;
import third_party;

module physical_sort;

namespace infinity {

#define COMPARE(type)                                                                                                                                \
    type left_value = ((type *)(left_result_vector->data()))[left_index.offset];                                                                     \
    type right_value = ((type *)(right_result_vector->data()))[right_index.offset];                                                                  \
    if (left_value == right_value) {                                                                                                                 \
        continue;                                                                                                                                    \
    }                                                                                                                                                \
    if (order_type == OrderType::kAsc) {                                                                                                             \
        return left_value < right_value;                                                                                                             \
    } else {                                                                                                                                         \
        return left_value > right_value;                                                                                                             \
    }

class Comparator {
public:
    explicit Comparator(const Vector<UniquePtr<DataBlock>> &order_by_blocks,
                        const Vector<OrderType> &order_by_types,
                        Vector<SharedPtr<BaseExpression>> expressions)
        : order_by_blocks_(order_by_blocks), order_by_types_(order_by_types), expressions_(expressions) {}

    void Init() {
        if (order_by_blocks_.empty()) {
            return;
        }
        eval_results_.reserve(order_by_blocks_.size());

        ExpressionEvaluator expr_evaluator;
        Vector<SharedPtr<ExpressionState>> expr_states;
        for (SizeT expr_id = 0; expr_id < expressions_.size(); ++expr_id) {
            expr_states.push_back(ExpressionState::CreateState(expressions_[expr_id]));
        }

        for (SizeT block_id = 0; block_id < order_by_blocks_.size(); ++block_id) {
            Vector<SharedPtr<ColumnVector>> results;

            expr_evaluator.Init(order_by_blocks_[block_id].get());
            results.reserve(expressions_.size());

            for (SizeT expr_id = 0; expr_id < expressions_.size(); ++expr_id) {
                auto expr = expressions_[expr_id];
                SharedPtr<ColumnVector> result_vector = MakeShared<ColumnVector>(MakeShared<DataType>(expr->Type()));

                result_vector->Initialize();
                expr_evaluator.Execute(expr, expr_states[expr_id], result_vector);

                results.push_back(result_vector);
            }
            eval_results_.push_back(results);
        }
    }

    bool operator()(BlockRawIndex left_index, BlockRawIndex right_index) {
        SizeT exprs_count = expressions_.size();
        for (SizeT expr_idx = 0; expr_idx < exprs_count; ++expr_idx) {
            auto expr = expressions_[expr_idx];
            DataType type = expr->Type();
            OrderType order_type = order_by_types_[expr_idx];

            SharedPtr<ColumnVector> &left_result_vector = eval_results_[left_index.block_idx][expr_idx];
            SharedPtr<ColumnVector> &right_result_vector = eval_results_[right_index.block_idx][expr_idx];

            switch (type.type()) {
                case kBoolean: {
                    COMPARE(BooleanT)
                }
                case kTinyInt: {
                    COMPARE(TinyIntT)
                }
                case kSmallInt: {
                    COMPARE(SmallIntT)
                }
                case kInteger: {
                    COMPARE(IntegerT)
                }
                case kBigInt: {
                    COMPARE(BigIntT)
                }
                case kFloat: {
                    COMPARE(FloatT)
                }
                case kDouble: {
                    COMPARE(DoubleT)
                }
                default: {
                    Error<NotImplementException>(Format("{} not implemented.", type.type()));
                }
            }
        }
        return true;
    }

private:
    const Vector<UniquePtr<DataBlock>> &order_by_blocks_;
    const Vector<OrderType> &order_by_types_;
    Vector<SharedPtr<BaseExpression>> expressions_;

    // Blocks -> Expressions
    Vector<Vector<SharedPtr<ColumnVector>>> eval_results_;
};

Vector<BlockRawIndex> MergeTwoIndexes(Vector<BlockRawIndex> &&indexes_a, Vector<BlockRawIndex> &&indexes_b, Comparator &comparator) {
    if (indexes_a.empty() || indexes_b.empty()) {
        return indexes_a.empty() ? indexes_b : indexes_a;
    }
    Vector<BlockRawIndex> merged_indexes;
    merged_indexes.reserve(indexes_a.size() + indexes_b.size());

    auto ptr_a = indexes_a.begin();
    auto ptr_b = indexes_b.begin();

    while (ptr_a != indexes_a.end() && ptr_b != indexes_b.end()) {
        if (comparator(*ptr_a, *ptr_b)) {
            merged_indexes.push_back(*ptr_a);
            ++ptr_a;
        } else {
            merged_indexes.push_back(*ptr_b);
            ++ptr_b;
        }
    }
    if (ptr_a != indexes_a.end()) {
        merged_indexes.insert(merged_indexes.end(), ptr_a, indexes_a.end());
    }
    if (ptr_b != indexes_b.end()) {
        merged_indexes.insert(merged_indexes.end(), ptr_b, indexes_b.end());
    }

    return merged_indexes;
}

Vector<BlockRawIndex> MergeIndexes(Vector<Vector<BlockRawIndex>> &indexes_group, SizeT l, SizeT r, Comparator &comparator) {
    if (l == r)
        return indexes_group[l];
    if (l > r)
        return Vector<BlockRawIndex>();
    SizeT mid = (l + r) >> 1;
    return MergeTwoIndexes(Move(MergeIndexes(indexes_group, l, mid, comparator)),
                           Move(MergeIndexes(indexes_group, mid + 1, r, comparator)),
                           comparator);
}

void CopyWithIndexes(const Vector<UniquePtr<DataBlock>> &input_blocks,
                     Vector<UniquePtr<DataBlock>> &output_blocks,
                     const Vector<BlockRawIndex> &block_indexes) {
    if (input_blocks.empty()) {
        return;
    }

    SizeT start_block_index = output_blocks.size();
    auto block_count = (block_indexes.size() + DEFAULT_BLOCK_CAPACITY - 1) / DEFAULT_BLOCK_CAPACITY;

    // copy with block_indexes and push to unmerge_sorted_blocks
    for (SizeT i = 0; i < block_count; ++i) {
        auto sorted_datablock = DataBlock::MakeUniquePtr();
        sorted_datablock->Init(input_blocks[0]->types());

        output_blocks.push_back(Move(sorted_datablock));
    }
    for (SizeT index_idx = 0; index_idx < block_indexes.size(); ++index_idx) {
        auto &block_index = block_indexes[index_idx];
        const Vector<SharedPtr<ColumnVector>> &output_column_vectors =
            output_blocks[(index_idx / DEFAULT_BLOCK_CAPACITY) + start_block_index]->column_vectors;

        for (SizeT column_id = 0; column_id < output_column_vectors.size(); ++column_id) {
            output_column_vectors[column_id]->AppendWith(*input_blocks[block_index.block_idx]->column_vectors[column_id].get(),
                                                         block_index.offset,
                                                         1);
        }
    }
    for (SizeT i = 0; i < block_count; ++i) {
        output_blocks[i + start_block_index]->Finalize();
    }
}

void PhysicalSort::Init() {}

bool PhysicalSort::Execute(QueryContext *, OperatorState *operator_state) {
    auto *prev_op_state = operator_state->prev_op_state_;
    auto *sort_operator_state = static_cast<SortOperatorState *>(operator_state);

    // Generate block indexes
    Vector<BlockRawIndex> block_indexes;
    auto pre_op_state = operator_state->prev_op_state_;

    block_indexes.reserve(pre_op_state->data_block_array_.size() * DEFAULT_BLOCK_CAPACITY);
    // filling block_indexes
    for (SizeT block_id = 0; block_id < pre_op_state->data_block_array_.size(); block_id++) {
        for (SizeT offset = 0; offset < pre_op_state->data_block_array_[block_id]->row_count(); offset++) {
            BlockRawIndex block_index(block_id, offset);

            block_indexes.push_back(block_index);
        }
    }
    auto block_comparator = Comparator(pre_op_state->data_block_array_, order_by_types_, expressions_);

    block_comparator.Init();
    // sort block_indexes
    std::sort(block_indexes.begin(), block_indexes.end(), block_comparator);

    CopyWithIndexes(pre_op_state->data_block_array_, sort_operator_state->unmerge_sorted_blocks_, block_indexes);
    prev_op_state->data_block_array_.clear();

    if (!prev_op_state->Complete()) {
        return false;
    }
    auto &unmerge_sorted_blocks = sort_operator_state->unmerge_sorted_blocks_;
    auto merge_comparator = Comparator(unmerge_sorted_blocks, order_by_types_, expressions_);
    Vector<Vector<BlockRawIndex>> indexes_group;

    merge_comparator.Init();
    indexes_group.reserve(unmerge_sorted_blocks.size());

    for (SizeT block_id = 0; block_id < unmerge_sorted_blocks.size(); ++block_id) {
        Vector<BlockRawIndex> indexes;
        indexes.reserve(unmerge_sorted_blocks[block_id]->row_count());

        for (SizeT offset = 0; offset < unmerge_sorted_blocks[block_id]->row_count(); ++offset) {
            BlockRawIndex block_index(block_id, offset);
            indexes.push_back(block_index);
        }
        indexes_group.push_back(indexes);
    }
    auto merge_indexes = MergeIndexes(indexes_group, 0, indexes_group.size() - 1, merge_comparator);
    indexes_group.clear();

    CopyWithIndexes(sort_operator_state->unmerge_sorted_blocks_, sort_operator_state->data_block_array_, merge_indexes);
    sort_operator_state->unmerge_sorted_blocks_.clear();
    sort_operator_state->SetComplete();
    return true;
}

} // namespace infinity
